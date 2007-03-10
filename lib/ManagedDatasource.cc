/*
 ManagedDatasource.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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

ManagedDatasource::~ManagedDatasource()
{
}

void ManagedDatasource::thread_cleanup(class ExceptionSink *xsink)
{
   AutoLocker al(&ds_lock);
   // wait for any in-progress action to complete
   if (counter > 0)
   {
      while (counter)
	 cStatus.wait(&ds_lock);      
      // in case there are other calls waiting on the condition
      cStatus.signal();
   }
   if (in_transaction)
   {
      xsink->raiseException("DATASOURCE-TRANSACTION-EXCEPTION", "TID %d terminated while in a transaction; transaction will be automatically rolled back and the lock released", gettid());
      Datasource::rollback(xsink);
      in_transaction = false;
      // force-exit the transaction lock
      tGate.forceExit();
   }
}

void ManagedDatasource::destructor(class ExceptionSink *xsink)
{
   AutoLocker al(&ds_lock);
   closeUnlocked(xsink);
   counter = -1;
}

void ManagedDatasource::deref(class ExceptionSink *xsink)
{
   if (ROdereference())
   {
      close(xsink);
      delete this;
   }
}

void ManagedDatasource::deref()
{
   if (ROdereference())
   {
      close();
      delete this;
   }
}

int ManagedDatasource::grabLockIntern(class ExceptionSink *xsink)
{
   if (tGate.enter(tl_timeout_ms) < 0)
   {
      endDBAction();
      xsink->raiseException("TRANSACTION-TIMEOUT", "timed out on datasource '%s@%s' after waiting %d millisecond%s on transaction lock held by TID %d", 
			    username.empty() ? "<n/a>" : username.c_str(), 
			    dbname.empty() ? "<n/a>" : dbname.c_str(), tl_timeout_ms, 
			    tl_timeout_ms == 1 ? "" : "s", tGate.getLockTID());
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
   
   nds->p_username    = p_username;
   nds->p_password    = p_password;
   nds->p_dbname      = p_dbname;
   nds->p_hostname    = p_hostname;
   nds->p_db_encoding = p_db_encoding;
   return nds;
}

ManagedDatasource::ManagedDatasource(DBIDriver *ndsl) : Datasource(ndsl)
{
   counter = 0;
   tl_timeout_ms = DEFAULT_TL_TIMEOUT;
}

int ManagedDatasource::wait_for_counter(class ExceptionSink *xsink)
{
   // object has been deleted in another thread
   if (counter == -1)
   {
      xsink->raiseException("DATASOURCE-ERROR", "This object has been deleted in another thread");
      return -1;
   }
   if (counter)
   {
      while (counter)
	 cStatus.wait(&ds_lock);      
      // in case there are other calls waiting
      cStatus.signal();
   }
   return 0;
}

int ManagedDatasource::startDBAction(class ExceptionSink *xsink)
{
   AutoLocker al(&ds_lock);
   // object has been deleted in another thread
   if (counter == -1)
   {
      xsink->raiseException("DATASOURCE-ERROR", "This object has been deleted in another thread");
      return -1;
   }
      
   if (isopen || (!Datasource::open(xsink) && !(xsink->isEvent())))
   {
      counter++;
      return 0;
   }
   return -1;
}

void ManagedDatasource::endDBAction()
{
   AutoLocker al(&ds_lock);
   if (!--counter)
      cStatus.signal();
}

void ManagedDatasource::setTransactionLockTimeout(int t_ms)
{
   tl_timeout_ms = t_ms;
}

int ManagedDatasource::getTransactionLockTimeout()
{
   return tl_timeout_ms;
}

void ManagedDatasource::setAutoCommit(bool ac)
{
   AutoLocker al(&ds_lock);
   if (counter > 0)
   {
      while (counter)
	 cStatus.wait(&ds_lock);      
      // in case there are other calls waiting
      cStatus.signal();
   }
   Datasource::setAutoCommit(ac);
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
      if (!grabLock(xsink))
      {
	 if (Datasource::beginTransaction(xsink))
	 {
	    releaseLock();
	    in_transaction = false;
	 }
      }
      
      endDBAction();
   }
   //printd(0, "ManagedDatasource::beginTransaction() this=%08p in_transaction=%d\n", this, in_transaction);
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
   AutoLocker al(&ds_lock);
   if (wait_for_counter(xsink))
      return -1;
   return Datasource::open(xsink);
}

/*
int ManagedDatasource::closeUnlocked()
{
   int rc = -1;
   
   if (isopen)
   {
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
*/

// returns 0 for OK, -1 for exception
int ManagedDatasource::closeUnlocked(class ExceptionSink *xsink)
{
   int rc = 0;

   // wait for any in-progress action to complete
   if (counter > 0)
   {
      while (counter)
	 cStatus.wait(&ds_lock);      
      // in case there are other calls waiting on the condition
      cStatus.signal();
   }
   if (isopen)
   {
      if (in_transaction)
      {
	 xsink->raiseException("DATASOURCE-TRANSACTION-EXCEPTION", "Datasource closed while in a transaction; transaction will be automatically rolled back and the lock released");
	 Datasource::rollback(xsink);
	 trlist.remove(this);
	 in_transaction = false;
	 // force-exit the transaction lock
	 tGate.forceExit();
	 rc = -1;
      }
      
      Datasource::close();
      isopen = false;
   }
   
   return rc;
}

int ManagedDatasource::close()
{
   assert(false);
   
/*
   AutoLocker al(&ds_lock);
   // wait for any in-progress action to complete
   if (counter)
   {
      while (counter)
	 cStatus.wait(&ds_lock);      
      // in case there are other calls waiting on the condition
      cStatus.signal();
   }
   return closeUnlocked();
 */
}

int ManagedDatasource::close(class ExceptionSink *xsink)
{
   AutoLocker al(&ds_lock);
   return closeUnlocked(xsink);
}

// forces a close and open to reset a database connection
void ManagedDatasource::reset(ExceptionSink *xsink)
{
   AutoLocker al(&ds_lock);
   closeUnlocked(xsink);
   // open the connection
   Datasource::open(xsink);
}

void ManagedDatasource::setPendingUsername(const char *u)
{
   AutoLocker al(&ds_lock);
   Datasource::setPendingUsername(u);
}

void ManagedDatasource::setPendingPassword(const char *p)
{
   AutoLocker al(&ds_lock);
   Datasource::setPendingPassword(p);
}

void ManagedDatasource::setPendingDBName(const char *d)
{
   AutoLocker al(&ds_lock);
   Datasource::setPendingDBName(d);
}

void ManagedDatasource::setPendingDBEncoding(const char *c)
{
   AutoLocker al(&ds_lock);
   Datasource::setPendingDBEncoding(c);
}

void ManagedDatasource::setPendingHostName(const char *h)
{
   AutoLocker al(&ds_lock);
   Datasource::setPendingHostName(h);
}

QoreNode *ManagedDatasource::getPendingUsername()
{
   AutoLocker al(&ds_lock);
   return Datasource::getPendingUsername();
}

QoreNode *ManagedDatasource::getPendingPassword()
{
   AutoLocker al(&ds_lock);
   return Datasource::getPendingPassword();
}

QoreNode *ManagedDatasource::getPendingDBName()
{
   AutoLocker al(&ds_lock);
   return Datasource::getPendingDBName();
}

QoreNode *ManagedDatasource::getPendingDBEncoding()
{
   AutoLocker al(&ds_lock);
   return Datasource::getPendingDBEncoding();
}

QoreNode *ManagedDatasource::getPendingHostName()
{
   AutoLocker al(&ds_lock);
   return Datasource::getPendingHostName();
}
