/*
  QC_Datasource.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  NOTE that 2 copies of connection values are kept in case
  the values are changed while a connection is in use

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

#include <stdlib.h>
#include <string.h>

#define DEFAULT_TL_TIMEOUT 120

extern int CID_DATASOURCE;

class QoreClass *initDatasourceClass();

void datasource_thread_lock_cleanup(void *ptr, class ExceptionSink *xsink);

class Datasource : private ReferenceObject, private LockedObject
{
   private:
      class SingleExitGate tGate;
      bool in_transaction;
      int tl_timeout;
      bool isopen;
      int counter;
      bool autocommit;
      class QoreCondition cStatus;

      // for pending connection values
      char *p_username,  // for Oracle, MySQL
	 *p_password,  // for Oracle, MySQL
	 *p_dbname,    // for Oracle, MySQL
	 *p_charset,   // for Oracle
	 *p_hostname;  // for MySQL

      inline int startDBAction(class ExceptionSink *xsink);
      inline void endDBAction();
      inline int openUnlocked(ExceptionSink *xsink);
      inline void freeConnectionValues();
      inline void setConnectionValues();

      // returns 0 for OK, -1 for error
      inline int grabLockIntern(class ExceptionSink *xsink)
      {	 
	 if (tGate.enter(tl_timeout))
	 {
	    endDBAction();
	    xsink->raiseException("TRANSACTION-TIMEOUT", "timed out on datasource \"%s@%s\" after waiting %d second%s on transaction lock held by TID %d", 
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
      inline ~Datasource();

   public:
      class QoreEncoding *qorecharset;
      class DBIDriver *dsl;
      void *private_data;
      class Datasource *next;

      // actual connection values set by init()
      char *username,  // for Oracle, MySQL
	 *password,  // for Oracle, MySQL
	 *dbname,    // for Oracle, MySQL
	 *charset,   // for Oracle
	 *hostname;  // for MySQL

      inline Datasource(DBIDriver *);
      inline class QoreNode *select(class QoreString *query_str, class List *args, ExceptionSink *xsink);
      inline class QoreNode *selectRows(class QoreString *query_str, class List *args, ExceptionSink *xsink);
      inline class QoreNode *exec(class QoreString *query_str, class List *args, ExceptionSink *xsink);
      //inline class Hash *describe(char *table_name, ExceptionSink *xsink);
      inline int commit(ExceptionSink *xsink);
      inline int rollback(ExceptionSink *xsink);
      inline int open(ExceptionSink *xsink);
      inline int close();
      inline void reset(ExceptionSink *xsink);
      inline List *getCapabilityList()
      {
	 return dsl->getCapList();
      }
      inline int getCapabilities()
      {
	 return dsl->getCaps();
      }
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
      inline bool isInTransaction() 
      { 
	 return in_transaction; 
      }
      inline void setAutoCommit(bool ac);
      inline bool getAutoCommit() 
      { 
	 return autocommit;
      }
      inline bool isOpen()
      { 
	 return isopen; 
      }

      inline void deref()
      {
	 if (ROdereference())
	    delete this;
      }
      inline void ref()
      {
	 ROreference();
      }

      inline Datasource *copy()
      {
	 class Datasource *nds = new Datasource(dsl);
	 
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

inline Datasource::Datasource(DBIDriver *ndsl)
{
   dsl = ndsl;
   isopen = false;
   counter = 0;
   in_transaction = false;
   tl_timeout = DEFAULT_TL_TIMEOUT;
   private_data = NULL;
   p_dbname = p_username = p_password = p_charset = p_hostname = NULL;
   dbname = username = password = charset = hostname = NULL;
   autocommit = false;
   qorecharset = QCS_DEFAULT;
}

inline Datasource::~Datasource()
{
   if (isopen)
      close();
#ifdef DEBUG
   if (private_data) 
      printe("ERROR: Datasource::~Datasource() private_data is not NULL\n");
#endif
   if (p_dbname)   free(p_dbname);
   if (p_username) free(p_username);
   if (p_password) free(p_password);
   if (p_charset)  free(p_charset);
   if (p_hostname) free(p_hostname);

   freeConnectionValues();
}

inline void Datasource::freeConnectionValues()
{
   if (dbname)   free(dbname);
   if (username) free(username);
   if (password) free(password);
   if (charset)  free(charset);
   if (hostname) free(hostname);
}

inline void Datasource::setConnectionValues()
{
   dbname   = p_dbname   ? strdup(p_dbname)   : NULL;
   username = p_username ? strdup(p_username) : NULL;
   password = p_password ? strdup(p_password) : NULL;
   charset  = p_charset  ? strdup(p_charset)  : NULL;
   hostname = p_hostname ? strdup(p_hostname) : NULL;
}

inline int Datasource::startDBAction(class ExceptionSink *xsink)
{
   int rc = 0;

   lock();

   if (isopen || (!openUnlocked(xsink) && !(xsink->isEvent())))
      counter++;
   else
      rc = -1;

   unlock();

   return rc;
}

inline void Datasource::endDBAction()
{
   lock();
   if (!--counter)
      cStatus.signal();
   unlock();
}

inline void Datasource::setTransactionLockTimeout(int t)
{
   tl_timeout = t;
}

inline int Datasource::getTransactionLockTimeout()
{
   return tl_timeout;
}

inline void Datasource::setAutoCommit(bool ac)
{
   lock();
   if (counter)
   {
      while (counter)
	 cStatus.wait(&ptm_lock);
    
      // in case there are other close calls waiting
      cStatus.signal();
   }
   autocommit = ac;

   unlock();
}

inline QoreNode *Datasource::select(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   class QoreNode *rv;

   if (!startDBAction(xsink))
   {
      rv = dsl->select(this, query_str, args, xsink);
      endDBAction();
   }
   else
      rv = NULL;

   return rv;
}

inline QoreNode *Datasource::selectRows(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   class QoreNode *rv;

   if (!startDBAction(xsink))
   {
      rv = dsl->selectRows(this, query_str, args, xsink);
      endDBAction();
   }
   else
      rv = NULL;

   return rv;
}

inline QoreNode *Datasource::exec(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   class QoreNode *rv;

   if (!startDBAction(xsink))
   {
      if (!autocommit && grabLock(xsink))
	 return NULL;

      rv = dsl->execSQL(this, query_str, args, xsink);

      // exit the transaction lock if autocommit enabled & exception has occurred & we weren't already in a transaction
      if (!autocommit && !in_transaction)
	 if (xsink->isEvent())
	    releaseLock();
	 else
	    in_transaction = true;

      endDBAction();
   }
   else
      rv = NULL;

   return rv;
}

inline void Datasource::beginTransaction(class ExceptionSink *xsink)
{
   //printd(0, "Datasource::beginTransaction() autocommit=%s\n", autocommit ? "true" : "false");
   if (!startDBAction(xsink))
   {
      if (autocommit)
      {
	 endDBAction();
	 xsink->raiseException("AUTOCOMMIT-ERROR", "transaction management is not available because autocommit is enabled for this datasource");
	 return;
      }

      if (grabLock(xsink))
	 return;

      in_transaction = true;

      endDBAction();
   }
}

inline int Datasource::commit(ExceptionSink *xsink)
{
   int rc;

   if (!startDBAction(xsink))
   {
      if (!autocommit && grabLock(xsink))
	 return -1;

      rc = dsl->commit(this, xsink);

      in_transaction = false;

      // transaction is complete, exit the lock
      if (!autocommit) 
	 releaseLock();

      endDBAction();
   }
   else
      rc = -1;

   return rc;
}

inline int Datasource::rollback(ExceptionSink *xsink)
{
   int rc;

   if (!startDBAction(xsink))
   {
      if (!autocommit && grabLock(xsink))
	 return -1;

      rc = dsl->rollback(this, xsink);

      in_transaction = false;

      // transaction is complete, exit the lock
      if (!autocommit)
	 releaseLock();

      endDBAction();
   }
   else
      rc = -1;

   return rc;
}

// must be called in the status lock
inline int Datasource::openUnlocked(ExceptionSink *xsink)
{
   int rc;

   if (!isopen)
   {
      // copy pending connection values to connection values
      freeConnectionValues();
      setConnectionValues();

      rc = dsl->init(this, xsink);
      if (!xsink->isEvent())
	 isopen = true;
   }
   else
      rc = 0;

   return rc;
}

inline int Datasource::open(ExceptionSink *xsink)
{
   lock();
   openUnlocked(xsink);
   unlock();

   return 0;
}

inline int Datasource::close()
{
   int rc;

   lock();
   if (isopen)
   {
      while (counter)
	 cStatus.wait(&ptm_lock);

      dsl->close(this);
      rc = 0;
      isopen = false;

      // close any open transaction
      in_transaction = false;

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
   else
      rc = -1;
   unlock();

   return rc;
}

// forces a close and open to reset a database connection
inline void Datasource::reset(ExceptionSink *xsink)
{
   lock();
   if (isopen)
   {
      while (counter)
	 cStatus.wait(&ptm_lock);

      // close the datasource
      dsl->close(this);
      isopen = false;

      // open the connection
      openUnlocked(xsink);

      // close any open transaction(s)
      in_transaction = false;
      tGate.forceExit();

      // in case there are other calls waiting on the status lock
      cStatus.signal();
   }
   unlock();
}

inline void Datasource::setUsername(char *u)
{
   lock();
   if (p_username)
      free(p_username);
   p_username = strdup(u);
   unlock();
}

inline void Datasource::setPassword(char *p)
{
   lock();
   if (p_password)
      free(p_password);
   p_password = strdup(p);
   unlock();
}

inline void Datasource::setDBName(char *d)
{
   lock();
   if (p_dbname)
      free(p_dbname);
   p_dbname = strdup(d);
   unlock();
}

inline void Datasource::setCharset(char *c)
{
   lock();
   if (p_charset)
      free(p_charset);
   p_charset = strdup(c);
   unlock();
}

inline void Datasource::setHostName(char *h)
{
   lock();
   if (p_hostname)
      free(p_hostname);
   p_hostname = strdup(h);
   unlock();
}

inline QoreNode *Datasource::getUsername()
{
   QoreNode *rv;
   lock();
   if (p_username)
      rv = new QoreNode(p_username);
   else
      rv = NULL;
   unlock();
   return rv;
}

inline QoreNode *Datasource::getPassword()
{
   QoreNode *rv;
   lock();
   if (p_password)
      rv = new QoreNode(p_password);
   else
      rv = NULL;
   unlock();
   return rv;
}

inline QoreNode *Datasource::getDBName()
{
   QoreNode *rv;
   lock();
   if (p_dbname)
      rv = new QoreNode(p_dbname);
   else
      rv = NULL;
   unlock();
   return rv;
}

inline QoreNode *Datasource::getCharset()
{
   QoreNode *rv;
   lock();
   if (p_charset)
      rv = new QoreNode(p_charset);
   else
      rv = NULL;
   unlock();
   return rv;
}

inline QoreNode *Datasource::getHostName()
{
   QoreNode *rv;
   lock();
   if (p_hostname)
      rv = new QoreNode(p_hostname);
   else
      rv = NULL;
   unlock();
   return rv;
}

#endif // _QORE_SQL_OBJECTS_DATASOURCE_H
