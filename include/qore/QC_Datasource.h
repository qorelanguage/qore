/*
  QC_Datasource.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* 
   FIXME: commit()s when autocommit=true should be made here, also after
          select()s (in case of a select for update, for example)

   FIXME: when raising an timeout exception there is a race condition
          getting the TID of the thread holding the lock, because the lock 
	  could have been released after the ::enter() call fails... but it's
	  only cosmetic (for the exception text)
 */

#ifndef _QORE_SQL_OBJECTS_DATASOURCE_H

#define _QORE_SQL_OBJECTS_DATASOURCE_H

#include <qore/config.h>
#include <qore/ReferenceObject.h>
#include <qore/LockedObject.h>
#include <qore/QoreCondition.h>
#include <qore/Exception.h>
#include <qore/SingleExitGate.h>
#include <qore/DBI.h>
#include <qore/Datasource.h>

#include <stdlib.h>
#include <string.h>

#define DEFAULT_TL_TIMEOUT 120

extern int CID_DATASOURCE;

class QoreClass *initDatasourceClass();

void datasource_thread_lock_cleanup(void *ptr, class ExceptionSink *xsink);

class ManagedDatasource : public Datasource, private ReferenceObject, private LockedObject
{
   private:
      class SingleExitGate tGate;
      int counter;
      int tl_timeout;
      class QoreCondition cStatus;

      inline int startDBAction(class ExceptionSink *xsink);
      inline void endDBAction();
      inline int closeUnlocked();

      // returns 0 for OK, -1 for error
      inline int grabLockIntern(class ExceptionSink *xsink)
      {	 
	 if (tGate.enter(tl_timeout))
	 {
	    endDBAction();
	    xsink->raiseException("TRANSACTION-TIMEOUT", "timed out on datasource '%s@%s' after waiting %d second%s on transaction lock held by TID %d", 
				  username, dbname, tl_timeout, tl_timeout == 1 ? "" : "s", tGate.getLockTID());
	    return -1;
	 }
	 return 0;
      }
      inline void releaseLockIntern()
      {
	 tGate.exit();
      }

      // returns 0 for OK, -1 for error
      inline int grabLock(class ExceptionSink *xsink)
      {	 
	 if (grabLockIntern(xsink))
	    return -1;
	 if (!in_transaction)
	    trlist.set(this, datasource_thread_lock_cleanup);
	 return 0;
      }
      inline void releaseLock()
      {
	 tGate.exit();
	 trlist.remove(this);
      }

   protected:
      inline ~ManagedDatasource() {}

   public:
      inline ManagedDatasource(DBIDriver *);
      inline class QoreNode *select(class QoreString *query_str, class List *args, ExceptionSink *xsink);
      inline class QoreNode *selectRows(class QoreString *query_str, class List *args, ExceptionSink *xsink);
      inline class QoreNode *exec(class QoreString *query_str, class List *args, ExceptionSink *xsink);
      //inline class Hash *describe(char *table_name, ExceptionSink *xsink);
      inline int commit(ExceptionSink *xsink);
      inline int rollback(ExceptionSink *xsink);
      inline int open(ExceptionSink *xsink);
      inline int close();
      inline void reset(ExceptionSink *xsink);
      inline void setUsername(char *u);
      inline void setPassword(char *p);
      inline void setDBName(char *d);
      inline void setCharset(char *c);
      inline void setHostName(char *h);
      inline QoreNode *getUsername();
      inline QoreNode *getPassword();
      inline QoreNode *getDBName();
      inline QoreNode *getCharset();
      inline QoreNode *getHostName();
      
      inline void setTransactionLockTimeout(int t);
      inline int getTransactionLockTimeout();
      inline void beginTransaction(class ExceptionSink *xsink);
      inline void setAutoCommit(bool ac);

      inline void deref()
      {
	 if (ROdereference())
	    delete this;
      }
      inline void ref()
      {
	 ROreference();
      }

      inline ManagedDatasource *copy()
      {
	 class ManagedDatasource *nds = new ManagedDatasource(dsl);
	 
	 nds->p_username = p_username ? strdup(p_username) : NULL;
	 nds->password   = p_password ? strdup(p_password) : NULL;
	 nds->dbname     = p_dbname   ? strdup(p_dbname)   : NULL;
	 nds->charset    = p_charset  ? strdup(p_charset)  : NULL;
	 nds->hostname   = p_hostname ? strdup(p_hostname) : NULL;
	 return nds;
      }
};

#include <qore/charset.h>
#include <qore/QoreNode.h>
#include <qore/Exception.h>

inline ManagedDatasource::ManagedDatasource(DBIDriver *ndsl) : Datasource(ndsl)
{
   counter = 0;
   tl_timeout = DEFAULT_TL_TIMEOUT;
}

inline int ManagedDatasource::startDBAction(class ExceptionSink *xsink)
{
   int rc = 0;

   lock();

   if (isopen || (!Datasource::open(xsink) && !(xsink->isEvent())))
      counter++;
   else
      rc = -1;

   unlock();

   return rc;
}

inline void ManagedDatasource::endDBAction()
{
   lock();
   if (!--counter)
      cStatus.signal();
   unlock();
}

inline void ManagedDatasource::setTransactionLockTimeout(int t)
{
   tl_timeout = t;
}

inline int ManagedDatasource::getTransactionLockTimeout()
{
   return tl_timeout;
}

inline void ManagedDatasource::setAutoCommit(bool ac)
{
   lock();
   if (counter)
   {
      while (counter)
	 cStatus.wait(&ptm_lock);
    
      // in case there are other close calls waiting
      cStatus.signal();
   }
   Datasource::setAutoCommit(ac);

   unlock();
}

inline QoreNode *ManagedDatasource::select(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   class QoreNode *rv;

   if (!startDBAction(xsink))
   {
      rv = Datasource::select(query_str, args, xsink);
      endDBAction();
   }
   else
      rv = NULL;

   return rv;
}

inline QoreNode *ManagedDatasource::selectRows(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   class QoreNode *rv;

   if (!startDBAction(xsink))
   {
      rv = Datasource::selectRows(query_str, args, xsink);
      endDBAction();
   }
   else
      rv = NULL;

   return rv;
}

inline QoreNode *ManagedDatasource::exec(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   class QoreNode *rv = NULL;

   if (!startDBAction(xsink))
   {
      // only call grabLock if autocommit is disabled, skip execution if grabLock returns non-zero
      if (autocommit || !grabLock(xsink))
      {
	 rv = Datasource::exec(query_str, args, xsink);
      
	 // exit the transaction lock if we have failed to start a transaction (due to an exception)
	 if (!autocommit && !in_transaction)
	    releaseLock();
      }
      endDBAction();
   }
   
   return rv;
}

inline void ManagedDatasource::beginTransaction(class ExceptionSink *xsink)
{
   //printd(0, "ManagedDatasource::beginTransaction() autocommit=%s\n", autocommit ? "true" : "false");
   if (!startDBAction(xsink))
   {
      if (!Datasource::beginTransaction(xsink) && grabLock(xsink))
	 in_transaction = false;

      endDBAction();
   }
}

inline int ManagedDatasource::commit(ExceptionSink *xsink)
{
   int rc = -1;

   if (!startDBAction(xsink))
   {
      // only call grabLock if autocommit is disabled, skip execution if grabLock returns non-zero
      if (autocommit || !grabLock(xsink))
      {
	 rc = Datasource::commit(xsink);

	 // transaction is complete, exit the lock
	 if (!autocommit) 
	    releaseLock();
      }
      endDBAction();
   }

   return rc;
}

inline int ManagedDatasource::rollback(ExceptionSink *xsink)
{
   int rc = -1;

   if (!startDBAction(xsink))
   {
      // only call grabLock if autocommit is disabled, skip execution if grabLock returns non-zero
      if (autocommit || !grabLock(xsink))
      {
	 rc = Datasource::rollback(xsink);

	 // transaction is complete, exit the lock
	 if (!autocommit)
	    releaseLock();

	 endDBAction();
      }
   }

   return rc;
}

inline int ManagedDatasource::open(ExceptionSink *xsink)
{
   lock();
   int rc = Datasource::open(xsink);
   unlock();

   return rc;
}

inline int ManagedDatasource::closeUnlocked()
{
   int rc = -1;
   
   if (isopen)
   {
      while (counter)
	 cStatus.wait(&ptm_lock);
      
      rc = Datasource::close();
      
      // see if the transaction lock is held and, if so, break it
      int tid;
      if ((tid = tGate.getLockTID()) != -1)
      {
	 // remove the thread resource 
	 trlist.remove(this);
	 // force-exit the transaction lock if it's held
	 tGate.forceExit();
      }
      
      // in case there are other close calls waiting
      cStatus.signal();
   }
   
   return rc;
}

inline int ManagedDatasource::close()
{
   lock();
   int rc = closeUnlocked();
   unlock();
   return rc;
}

// forces a close and open to reset a database connection
inline void ManagedDatasource::reset(ExceptionSink *xsink)
{
   lock();
   if (isopen)
   {
      // force-close the connection
      closeUnlocked();

      // open the connection
      Datasource::open(xsink);
   }
   unlock();
}

inline void ManagedDatasource::setUsername(char *u)
{
   lock();
   Datasource::setUsername(u);
   unlock();
}

inline void ManagedDatasource::setPassword(char *p)
{
   lock();
   Datasource::setPassword(p);
   unlock();
}

inline void ManagedDatasource::setDBName(char *d)
{
   lock();
   Datasource::setDBName(d);
   unlock();
}

inline void ManagedDatasource::setCharset(char *c)
{
   lock();
   Datasource::setCharset(c);
   unlock();
}

inline void ManagedDatasource::setHostName(char *h)
{
   lock();
   Datasource::setHostName(h);
   unlock();
}

inline QoreNode *ManagedDatasource::getUsername()
{
   lock();
   QoreNode *rv = Datasource::getUsername();
   unlock();
   return rv;
}

inline QoreNode *ManagedDatasource::getPassword()
{
   lock();
   QoreNode *rv = Datasource::getPassword();
   unlock();
   return rv;
}

inline QoreNode *ManagedDatasource::getDBName()
{
   lock();
   QoreNode *rv = Datasource::getDBName();
   unlock();
   return rv;
}

inline QoreNode *ManagedDatasource::getCharset()
{
   lock();
   QoreNode *rv = Datasource::getCharset();
   unlock();
   return rv;
}

inline QoreNode *ManagedDatasource::getHostName()
{
   lock();
   QoreNode *rv = Datasource::getHostName();
   unlock();
   return rv;
}

#endif // _QORE_SQL_OBJECTS_DATASOURCE_H
