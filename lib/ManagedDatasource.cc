/*
 ManagedDatasource.cc
 
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

#include <qore/config.h>
#include <qore/ManagedDatasource.h>
#include <qore/charset.h>
#include <qore/QoreNode.h>
#include <qore/Exception.h>
#include <qore/DBI.h>

#include <stdlib.h>
#include <string.h>

int ManagedDatasource::grabLockIntern(class ExceptionSink *xsink)
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

void ManagedDatasource::releaseLockIntern()
{
   tGate.exit();
}

// returns 0 for OK, -1 for error
int ManagedDatasource::grabLock(class ExceptionSink *xsink)
{	 
   if (grabLockIntern(xsink))
      return -1;
   if (!in_transaction)
      trlist.set(this, datasource_thread_lock_cleanup);
   return 0;
}

void ManagedDatasource::releaseLock()
{
   tGate.exit();
   trlist.remove(this);
}

ManagedDatasource *ManagedDatasource::copy()
{
   class ManagedDatasource *nds = new ManagedDatasource(dsl);
   
   nds->p_username    = p_username    ? strdup(p_username)    : NULL;
   nds->p_password    = p_password    ? strdup(p_password)    : NULL;
   nds->p_dbname      = p_dbname      ? strdup(p_dbname)      : NULL;
   nds->p_hostname    = p_hostname    ? strdup(p_hostname)    : NULL;
   nds->p_db_encoding = p_db_encoding ? strdup(p_db_encoding) : NULL;
   return nds;
}

ManagedDatasource::ManagedDatasource(DBIDriver *ndsl) : Datasource(ndsl)
{
   counter = 0;
   tl_timeout = DEFAULT_TL_TIMEOUT;
}

int ManagedDatasource::startDBAction(class ExceptionSink *xsink)
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

void ManagedDatasource::endDBAction()
{
   lock();
   if (!--counter)
      cStatus.signal();
   unlock();
}

void ManagedDatasource::setTransactionLockTimeout(int t)
{
   tl_timeout = t;
}

int ManagedDatasource::getTransactionLockTimeout()
{
   return tl_timeout;
}

void ManagedDatasource::setAutoCommit(bool ac)
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

QoreNode *ManagedDatasource::select(class QoreString *query_str, class List *args, ExceptionSink *xsink)
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

QoreNode *ManagedDatasource::selectRows(class QoreString *query_str, class List *args, ExceptionSink *xsink)
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

QoreNode *ManagedDatasource::exec(class QoreString *query_str, class List *args, ExceptionSink *xsink)
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

void ManagedDatasource::beginTransaction(class ExceptionSink *xsink)
{
   //printd(0, "ManagedDatasource::beginTransaction() autocommit=%s\n", autocommit ? "true" : "false");
   if (!startDBAction(xsink))
   {
      if (!Datasource::beginTransaction(xsink) && grabLock(xsink))
	 in_transaction = false;
      
      endDBAction();
   }
}

int ManagedDatasource::commit(ExceptionSink *xsink)
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

int ManagedDatasource::rollback(ExceptionSink *xsink)
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

int ManagedDatasource::open(ExceptionSink *xsink)
{
   lock();
   int rc = Datasource::open(xsink);
   unlock();
   
   return rc;
}

int ManagedDatasource::closeUnlocked()
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

int ManagedDatasource::close()
{
   lock();
   int rc = closeUnlocked();
   unlock();
   return rc;
}

// forces a close and open to reset a database connection
void ManagedDatasource::reset(ExceptionSink *xsink)
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

void ManagedDatasource::setPendingUsername(char *u)
{
   lock();
   Datasource::setPendingUsername(u);
   unlock();
}

void ManagedDatasource::setPendingPassword(char *p)
{
   lock();
   Datasource::setPendingPassword(p);
   unlock();
}

void ManagedDatasource::setPendingDBName(char *d)
{
   lock();
   Datasource::setPendingDBName(d);
   unlock();
}

void ManagedDatasource::setPendingDBEncoding(char *c)
{
   lock();
   Datasource::setPendingDBEncoding(c);
   unlock();
}

void ManagedDatasource::setPendingHostName(char *h)
{
   lock();
   Datasource::setPendingHostName(h);
   unlock();
}

QoreNode *ManagedDatasource::getPendingUsername()
{
   lock();
   QoreNode *rv = Datasource::getPendingUsername();
   unlock();
   return rv;
}

QoreNode *ManagedDatasource::getPendingPassword()
{
   lock();
   QoreNode *rv = Datasource::getPendingPassword();
   unlock();
   return rv;
}

QoreNode *ManagedDatasource::getPendingDBName()
{
   lock();
   QoreNode *rv = Datasource::getPendingDBName();
   unlock();
   return rv;
}

QoreNode *ManagedDatasource::getPendingDBEncoding()
{
   lock();
   QoreNode *rv = Datasource::getPendingDBEncoding();
   unlock();
   return rv;
}

QoreNode *ManagedDatasource::getPendingHostName()
{
   lock();
   QoreNode *rv = Datasource::getPendingHostName();
   unlock();
   return rv;
}
