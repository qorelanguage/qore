/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 ManagedDatasource.h
 
 Qore Programming Language
 
 Copyright (C) 2003 - 2015 David Nichols
 
  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

/* 
 FIXME: when raising an timeout exception there is a race condition
 getting the TID of the thread holding the lock, because the lock 
 could have been released after the ::enter() call fails... but it's
 only cosmetic (for the exception text)
 */

#ifndef _QORE_MANAGEDDATASOURCE_H
#define _QORE_MANAGEDDATASOURCE_H

#include <qore/intern/DatasourceStatementHelper.h>
#include <qore/intern/QoreSQLStatement.h>

#include <set>

// default timeout set to 120 seconds
#define DEFAULT_TL_TIMEOUT 120000

class ManagedDatasource : public AbstractThreadResource, public Datasource, public DatasourceStatementHelper {
   friend class DatasourceActionHelper;
   friend class DatasourceLockHelper;
protected:
   // connection/transaction lock
   mutable QoreThreadLock ds_lock;

   int tid,                        // TID of thread holding the connection/transaction lock
      waiting,                     // number of threads waiting on the transaction lock
      tl_timeout_ms;               // transaction timeout in milliseconds

   QoreCondition cond;             // condition when transaction lock is freed

   DLLLOCAL int startDBAction(ExceptionSink* xsink, bool& new_transaction);
   // returns true if we have the transaction lock, false if not
   DLLLOCAL bool endDBActionIntern(char cmd = DAH_NOCHANGE, bool new_transaction = false);
   // returns true if we have the transaction lock, false if not
   DLLLOCAL bool endDBAction(char cmd = DAH_NOCHANGE, bool new_transaction = false);
   DLLLOCAL int closeUnlocked(ExceptionSink* xsink);
   // returns 0 for OK, -1 for error
   DLLLOCAL int grabLockIntern();
   DLLLOCAL void grabLockUnconditionalIntern();
   // returns 0 for OK, -1 for error
   DLLLOCAL int grabLock(ExceptionSink* xsink);
   DLLLOCAL void releaseLock();
   DLLLOCAL void releaseLockIntern();
   DLLLOCAL void forceReleaseLockIntern();
   DLLLOCAL void finish_transaction();
   
protected:
   DLLLOCAL virtual ~ManagedDatasource() {
   }

public:
   DLLLOCAL ManagedDatasource(DBIDriver *ndsl) : Datasource(ndsl), tid(-1), waiting(0), tl_timeout_ms(DEFAULT_TL_TIMEOUT) {
   }

   DLLLOCAL ManagedDatasource(const ManagedDatasource& old) : Datasource(old), tid(-1), waiting(0), tl_timeout_ms(old.tl_timeout_ms) {
   }

   DLLLOCAL virtual void cleanup(ExceptionSink* xsink);
   DLLLOCAL virtual void destructor(ExceptionSink* xsink);
   DLLLOCAL virtual void deref(ExceptionSink* xsink);
   DLLLOCAL virtual void deref();

   DLLLOCAL AbstractQoreNode* select(const QoreString *query_str, const QoreListNode* args, ExceptionSink* xsink);
   DLLLOCAL QoreHashNode* selectRow(const QoreString *query_str, const QoreListNode* args, ExceptionSink* xsink);
   DLLLOCAL AbstractQoreNode* selectRows(const QoreString *query_str, const QoreListNode* args, ExceptionSink* xsink);
   DLLLOCAL AbstractQoreNode* exec(const QoreString *query_str, const QoreListNode* args, ExceptionSink* xsink);
   DLLLOCAL AbstractQoreNode* execRaw(const QoreString *query_str, ExceptionSink* xsink);
   DLLLOCAL QoreHashNode* describe(const QoreString *query_str, const QoreListNode* args, ExceptionSink* xsink);

   DLLLOCAL int commit(ExceptionSink* xsink);
   DLLLOCAL int rollback(ExceptionSink* xsink);
   DLLLOCAL int open(ExceptionSink* xsink);

   using Datasource::close;
   DLLLOCAL int close(ExceptionSink* xsink);
   DLLLOCAL int reset(ExceptionSink* xsink);
   DLLLOCAL void setPendingUsername(const char* u);
   DLLLOCAL void setPendingPassword(const char* p);
   DLLLOCAL void setPendingDBName(const char* d);
   DLLLOCAL void setPendingDBEncoding(const char* c);
   DLLLOCAL void setPendingHostName(const char* h);
   DLLLOCAL void setPendingPort(int port);
   DLLLOCAL QoreStringNode* getPendingUsername() const;
   DLLLOCAL QoreStringNode* getPendingPassword() const;
   DLLLOCAL QoreStringNode* getPendingDBName() const;
   DLLLOCAL QoreStringNode* getPendingDBEncoding() const;
   DLLLOCAL QoreStringNode* getPendingHostName() const;
   DLLLOCAL int getPendingPort() const;
   DLLLOCAL void setTransactionLockTimeout(int t_ms);
   DLLLOCAL int getTransactionLockTimeout() const;
   // returns true if a new transaction was started
   DLLLOCAL bool beginTransaction(ExceptionSink* xsink);

   using Datasource::setAutoCommit;
   DLLLOCAL void setAutoCommit(bool ac, ExceptionSink* xsink);   

   using Datasource::copy;
   DLLLOCAL ManagedDatasource* copy();
   DLLLOCAL AbstractQoreNode* getServerVersion(ExceptionSink* xsink);
   DLLLOCAL AbstractQoreNode* getClientVersion(ExceptionSink* xsink) const;

   DLLLOCAL void setEventQueue(Queue* q, AbstractQoreNode* arg, ExceptionSink* xsink);

   DLLLOCAL int transactionTid() const {
      return tid;
   }

   DLLLOCAL bool currentThreadInTransaction() const {
      return tid == gettid();
   }

   DLLLOCAL QoreHashNode* getOptionHash(ExceptionSink* xsink);
   DLLLOCAL int setOption(const char* opt, const AbstractQoreNode* val, ExceptionSink* xsink);
   DLLLOCAL AbstractQoreNode* getOption(const char* opt, ExceptionSink* xsink);

   // functions supporting DatasourceStatementHelper
   DLLLOCAL DatasourceStatementHelper *getReferencedHelper(QoreSQLStatement* s) {
      ref();
      return this;
   }

   // implementing DatasourceStatementHelper virtual functions
   DLLLOCAL virtual void helperDestructor(QoreSQLStatement* s, ExceptionSink* xsink) {
      deref(xsink);
   }

   DLLLOCAL virtual Datasource* helperStartAction(ExceptionSink* xsink, bool& new_transaction) {
      if (!startDBAction(xsink, new_transaction))
         return this;

      // only return "this" when there was an exception in startDBAction if we already had the lock
      return tid == gettid() ? this : 0;
   }

   DLLLOCAL virtual Datasource* helperEndAction(char cmd, bool new_transaction, ExceptionSink* xsink) {
      // execute a commit if auto-commit is enabled and the resource is being released
      // and the connection was not aborted
      if (cmd == DAH_RELEASE)
         autoCommit(xsink);
      return endDBAction(cmd, new_transaction) ? this : 0;
   }
};

class DatasourceActionHelper {
protected:
   ManagedDatasource& ds;
   bool ok, new_transaction;
   char cmd;

public:
   DLLLOCAL DatasourceActionHelper(ManagedDatasource& n_ds, ExceptionSink* xsink, char n_cmd = DAH_NOCHANGE) : 
      ds(n_ds), ok(!ds.startDBAction(xsink, new_transaction)), cmd(n_cmd) {
   }
   DLLLOCAL ~DatasourceActionHelper() {
      if (ok) {
         // FIXME: check connection aborted handling if exec could have been executed after connection reset
         if (ds.wasConnectionAborted() 
             || (new_transaction && ((cmd == DAH_NOCHANGE) || !ds.isInTransaction())))
            cmd = DAH_RELEASE;
	 ds.endDBAction(cmd, new_transaction);
      }
   }

   DLLLOCAL bool newTransaction() const { return new_transaction; }

   DLLLOCAL operator bool() const { return ok; }   
};

class DatasourceLockHelper {
protected:
   ManagedDatasource& ds;
   bool valid, had_lock;

public:
   DLLLOCAL DatasourceLockHelper(ManagedDatasource& n_ds, ExceptionSink* xsink) : ds(n_ds) {
      ds.ds_lock.lock();
      had_lock = ds.tid == gettid();
      valid = !ds.grabLock(xsink);
      if (!valid)
         ds.ds_lock.unlock();
   }

   DLLLOCAL ~DatasourceLockHelper() {
      if (valid) {
         if (!had_lock)
            ds.releaseLockIntern();
         ds.ds_lock.unlock();
      }
   }

   DLLLOCAL operator bool() const { return valid; }
};

#endif // _QORE_SQL_OBJECTS_DATASOURCE_H
