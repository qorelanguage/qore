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

#include <qore/Qore.h>
#include <qore/intern/ManagedDatasource.h>

#include <stdlib.h>
#include <string.h>

ManagedDatasource::ManagedDatasource(DBIDriver *ndsl) : Datasource(ndsl)
{
   counter = 0;
   waiting = 0;
   sql_waiting = 0;
   tl_timeout_ms = DEFAULT_TL_TIMEOUT;
   tid = -1;
}

ManagedDatasource::~ManagedDatasource()
{
}

void ManagedDatasource::cleanup(class ExceptionSink *xsink)
{
   AutoLocker al(&ds_lock);
   // wait for any in-progress action to complete
   wait_for_sql();
   if (isInTransaction())
   {
      xsink->raiseException("DATASOURCE-TRANSACTION-EXCEPTION", "TID %d terminated while in a transaction; transaction will be automatically rolled back and the lock released", gettid());
      Datasource::rollback(xsink);
      setTransactionStatus(false);
      // force-exit the transaction lock
      forceReleaseLock();
   }
}

void ManagedDatasource::destructor(class ExceptionSink *xsink)
{
   AutoLocker al(&ds_lock);
   // closeUnlocked will throw an exception if a transaction is in progress (and release the lock)
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

int ManagedDatasource::grabLockIntern()
{
   int ctid = gettid();
   
   while (tid != -1 && tid != ctid)
   {
      ++waiting;
      if (tl_timeout_ms)
      {
	 if (!cTransaction.wait(&ds_lock, tl_timeout_ms))
	    break;

	 printd(5, "ManagedDatasource %08p timed out after %dms waiting for tid %d to release lock\n", this, tl_timeout_ms, tid);
	 --waiting;
	 return -1;
      }
      else
	 cTransaction.wait(&ds_lock);
      --waiting;
   }
   
   tid = ctid;
   
   return 0;   
}

int ManagedDatasource::grabLock(class ExceptionSink *xsink)
{
   if (grabLockIntern() < 0)
   {
      endDBActionIntern();
      const char *un = getUsername();
      const char *db = getDBName();
      xsink->raiseException("TRANSACTION-LOCK-TIMEOUT", "timed out on datasource '%s@%s' after waiting %d millisecond%s on transaction lock held by TID %d", 
			    un ? un : "<n/a>", db ? db : "<n/a>", tl_timeout_ms, 
			    tl_timeout_ms == 1 ? "" : "s", tid); //tGate.getLockTID());
      return -1;
   }
   return 0;
}

void ManagedDatasource::releaseLock()
{
   assert(tid == gettid());
   tid = -1;
   if (waiting)
      cTransaction.signal();
}

void ManagedDatasource::forceReleaseLock()
{
   tid = -1;
   if (waiting)
      cTransaction.signal();
}

ManagedDatasource *ManagedDatasource::copy()
{
   class ManagedDatasource *nds = new ManagedDatasource(const_cast<DBIDriver *>(getDriver()));   
   nds->setPendingConnectionValues(static_cast<Datasource *>(this));
   return nds;
}

// must be holding ds_lock
int ManagedDatasource::wait_for_sql(class ExceptionSink *xsink)
{
   // object has been deleted in another thread
   if (counter < 0)
   {
      xsink->raiseException("DATASOURCE-ERROR", "This object has been deleted in another thread");
      return -1;
   }
   wait_for_sql();
   return 0;
}

// must be holding ds_lock
void ManagedDatasource::wait_for_sql()
{
   while (counter > 0)
   {
      ++sql_waiting;
      cSQL.wait(&ds_lock);
      --sql_waiting;
   }
   // in case there are other calls waiting
   cSQL.signal();
}

int ManagedDatasource::startDBAction(class ExceptionSink *xsink, bool need_transaction_lock)
{
   AutoLocker al(&ds_lock);
   if (wait_for_sql(xsink))
      return -1;

   if (isOpen() || (!Datasource::open(xsink) && !(xsink->isEvent())))
   {
      if (need_transaction_lock && !getAutoCommit() && grabLock(xsink))
	 return -1;
      
      counter = 1;
      return 0;
   }
   return -1;
}

void ManagedDatasource::endDBActionIntern()
{
   counter = 0;
   if (sql_waiting)
      cSQL.signal();
}

void ManagedDatasource::endDBAction()
{
   AutoLocker al(&ds_lock);
   endDBActionIntern();
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
   wait_for_sql();
   Datasource::setAutoCommit(ac);
}

QoreNode *ManagedDatasource::select(const QoreString *query_str, class QoreList *args, ExceptionSink *xsink)
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

// FIXME: should be a native DBI driver method
class QoreNode *ManagedDatasource::selectRow(const QoreString *sql, QoreList *args, class ExceptionSink *xsink)
{
   class QoreNode *rv;
   
   if (!startDBAction(xsink))
   {
      rv = Datasource::selectRows(sql, args, xsink);

      endDBAction();

      // return only hash of first row, if any
      if (rv && rv->type == NT_LIST)
      {
	 class QoreNode *h = rv->val.list->shift();
	 rv->deref(xsink);
	 rv = h;
      }
   }
   else
      rv = NULL;
   
   return rv;
}

QoreNode *ManagedDatasource::selectRows(const QoreString *query_str, class QoreList *args, ExceptionSink *xsink)
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

QoreNode *ManagedDatasource::exec(const QoreString *query_str, class QoreList *args, ExceptionSink *xsink)
{
   class QoreNode *rv = NULL;

   if (!startDBAction(xsink, true))
   {
      bool start_transaction = !isInTransaction();
      rv = Datasource::exec(query_str, args, xsink);

      // save thread resource if we just started a transaction
      if (!getAutoCommit() && start_transaction)
	 if (isInTransaction())
	    set_thread_resource(this);
	 else
	    releaseLock();

      endDBAction();
   }

   return rv;
}

void ManagedDatasource::beginTransaction(class ExceptionSink *xsink)
{
   //printd(0, "ManagedDatasource::beginTransaction() autocommit=%s\n", getAutoCommit() ? "true" : "false");
   if (!startDBAction(xsink, true))
   {
      bool start_transaction = !isInTransaction();

      // save thread resource if we just started a transaction
      if (!Datasource::beginTransaction(xsink) && start_transaction)
	 if (isInTransaction())
	    set_thread_resource(this);
	 else
	    releaseLock();
	    
      endDBAction();
   }
   //printd(0, "ManagedDatasource::beginTransaction() this=%08p isInTransaction()=%d\n", this, isInTransaction());
}

int ManagedDatasource::commit(ExceptionSink *xsink)
{
   int rc = -1;

   if (!startDBAction(xsink, true))
   {
      bool was_in_transaction = isInTransaction();

      rc = Datasource::commit(xsink);

      // transaction is complete, remove the transaction thread resource
      if (was_in_transaction)
	 remove_thread_resource(this);

      if (!getAutoCommit())
	 releaseLock();
      
      endDBAction();
   }
   
   return rc;
}

int ManagedDatasource::rollback(ExceptionSink *xsink)
{
   int rc = -1;
   
   if (!startDBAction(xsink, true))
   {
      bool was_in_transaction = isInTransaction();

      rc = Datasource::rollback(xsink);

      // transaction is complete, remove the transaction thread resource
      if (was_in_transaction)
	 remove_thread_resource(this);

      if (!getAutoCommit())
	 releaseLock();

      endDBAction();
   }
   
   return rc;
}

int ManagedDatasource::open(ExceptionSink *xsink)
{
   AutoLocker al(&ds_lock);
   if (wait_for_sql(xsink))
      return -1;
   return Datasource::open(xsink);
}

/*
int ManagedDatasource::closeUnlocked()
{
   int rc = -1;
   
   if (isOpen())
   {
      rc = Datasource::close();
      
      // see if the transaction lock is held and, if so, break it
      int tid;
      if ((tid = tGate.getLockTID()) != -1)
      {
	 // remove the thread resource 
	 remove_thread_resource(this);
	 // force-exit the transaction lock if it's held
	 tGate.forceExit();
      }
      
      // in case there are other close calls waiting
      cSQL.signal();
   }
   
   return rc;
}
*/

// returns 0 for OK, -1 for exception
int ManagedDatasource::closeUnlocked(class ExceptionSink *xsink)
{
   int rc = 0;

   // wait for any in-progress action to complete
   wait_for_sql();
   if (isOpen())
   {
      if (isInTransaction())
      {
	 xsink->raiseException("DATASOURCE-TRANSACTION-EXCEPTION", "Datasource closed while in a transaction; transaction will be automatically rolled back and the lock released");
	 Datasource::rollback(xsink);
	 remove_thread_resource(this);
	 setTransactionStatus(false);
	 // force-exit the transaction lock
	 forceReleaseLock();
	 rc = -1;
      }

      Datasource::close();
   }
   
   return rc;
}

// this method (without the ExceptionSink) is never called!
int ManagedDatasource::close()
{
   assert(false);
/*
   AutoLocker al(&ds_lock);
   // wait for any in-progress action to complete
   if (counter)
   {
      while (counter)
	 cSQL.wait(&ds_lock);      
      // in case there are other calls waiting on the condition
      cSQL.signal();
   }
   return closeUnlocked();
 */
   return 0;
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

QoreNode *ManagedDatasource::getServerVersion(ExceptionSink *xsink)
{
   class QoreNode *rv;
   
   if (!startDBAction(xsink))
   {
      rv = Datasource::getServerVersion(xsink);
      
      endDBAction();
   }
   else
      rv = NULL;
   
   return rv;
}

QoreNode *ManagedDatasource::getClientVersion(ExceptionSink *xsink)
{
   class QoreNode *rv;
   
   if (!startDBAction(xsink))
   {
      rv = Datasource::getClientVersion(xsink);
      
      endDBAction();
   }
   else
      rv = NULL;
   
   return rv;
}
