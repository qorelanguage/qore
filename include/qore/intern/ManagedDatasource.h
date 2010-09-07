/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 ManagedDatasource.h
 
 Qore Programming Language
 
 Copyright 2003 - 2010 David Nichols
 
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
 FIXME: when raising an timeout exception there is a race condition
 getting the TID of the thread holding the lock, because the lock 
 could have been released after the ::enter() call fails... but it's
 only cosmetic (for the exception text)
 */

#ifndef _QORE_MANAGEDDATASOURCE_H
#define _QORE_MANAGEDDATASOURCE_H

#include <qore/intern/DatasourceStatementHelper.h>

// default timeout set to 120 seconds
#define DEFAULT_TL_TIMEOUT 120000

class ManagedDatasource : public AbstractThreadResource, public Datasource, public DatasourceStatementHelper {
protected:
   mutable QoreThreadLock
      ds_lock,                     // transaction lock
      connection_lock;             // connection lock

   int counter,                    // flag if SQL is in progress
      tid,                         // TID of thread holding the connection/transaction lock
      waiting,                     // number of threads waiting on the transaction lock
      sql_waiting,                 // number of threads waiting on the SQL lock
      tl_timeout_ms;               // transaction timeout in milliseconds

   QoreCondition cSQL,             // condition when no SQL is in-progress
      cTransaction;                // condition when transaction lock is freed

   bool stmt_new_transaction;      // statement grabbed transaction lock
   
   DLLLOCAL int startDBAction(ExceptionSink *xsink, bool need_transaction_lock = false);
   DLLLOCAL void endDBActionIntern();
   DLLLOCAL void endDBAction();
   DLLLOCAL int closeUnlocked(ExceptionSink *xsink);
   // returns 0 for OK, -1 for error
   DLLLOCAL int grabLockIntern();
   // returns 0 for OK, -1 for error
   DLLLOCAL int grabLock(ExceptionSink *xsink);
   DLLLOCAL void releaseLock();
   DLLLOCAL void forceReleaseLock();
   DLLLOCAL int wait_for_sql(ExceptionSink *xsink);
   DLLLOCAL void wait_for_sql();
   DLLLOCAL void finish_transaction();
   
protected:
   DLLLOCAL virtual ~ManagedDatasource();

public:
   DLLLOCAL ManagedDatasource(DBIDriver *);
   DLLLOCAL virtual void cleanup(ExceptionSink *xsink);
   DLLLOCAL virtual void destructor(ExceptionSink *xsink);
   DLLLOCAL virtual void deref(ExceptionSink *xsink);
   DLLLOCAL virtual void deref() {
      assert(false);
   }
   DLLLOCAL AbstractQoreNode *select(const QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode *selectRow(const QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode *selectRows(const QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode *exec(const QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink);
   DLLLOCAL int commit(ExceptionSink *xsink);
   DLLLOCAL int rollback(ExceptionSink *xsink);
   DLLLOCAL int open(ExceptionSink *xsink);
   DLLLOCAL int close(ExceptionSink *xsink);
   DLLLOCAL void reset(ExceptionSink *xsink);
   DLLLOCAL void setPendingUsername(const char *u);
   DLLLOCAL void setPendingPassword(const char *p);
   DLLLOCAL void setPendingDBName(const char *d);
   DLLLOCAL void setPendingDBEncoding(const char *c);
   DLLLOCAL void setPendingHostName(const char *h);
   DLLLOCAL void setPendingPort(int port);
   DLLLOCAL QoreStringNode *getPendingUsername() const;
   DLLLOCAL QoreStringNode *getPendingPassword() const;
   DLLLOCAL QoreStringNode *getPendingDBName() const;
   DLLLOCAL QoreStringNode *getPendingDBEncoding() const;
   DLLLOCAL QoreStringNode *getPendingHostName() const;
   DLLLOCAL int getPendingPort() const;
   DLLLOCAL void setTransactionLockTimeout(int t_ms);
   DLLLOCAL int getTransactionLockTimeout() const;
   // returns true if a new transaction was started
   DLLLOCAL bool beginTransaction(ExceptionSink *xsink);
   DLLLOCAL void setAutoCommit(bool ac);   
   DLLLOCAL ManagedDatasource *copy();
   DLLLOCAL AbstractQoreNode *getServerVersion(ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode *getClientVersion(ExceptionSink *xsink) const;

   // functions supporting DatasourceStatementHelper
   DLLLOCAL DatasourceStatementHelper *getReferencedHelper() {
      ref();
      return this;
   }

   // implementing DatasourceStatementHelper virtual functions
   DLLLOCAL virtual void helperDestructor(ExceptionSink *xsink) {
      deref(xsink);
   }

   DLLLOCAL virtual Datasource *helperGetDatasource(ExceptionSink *xsink) {
      bool nt = beginTransaction(xsink);
      if (*xsink)
         return 0;

      stmt_new_transaction = nt;

      return this;
   }

   DLLLOCAL virtual int helperReleaseDatasource(bool started_transaction, ExceptionSink *xsink) {
      if (getAutoCommit())
         return commit(xsink);

      // only release transaction lock if stmt_new_transaction is true and started_transaction is false
      if (stmt_new_transaction && !started_transaction) {
         remove_thread_resource(this);
         releaseLock();
         endDBAction();
      }

      return 0;
   }

   DLLLOCAL virtual QoreThreadLock *helperGetConnectionLock() {
      return &connection_lock;
   }

};

#endif // _QORE_SQL_OBJECTS_DATASOURCE_H
