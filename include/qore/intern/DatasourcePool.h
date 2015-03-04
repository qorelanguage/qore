/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  DatasourcePool.h
 
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

#ifndef _QORUS_DATASOURCE_POOL_H

#define _QORUS_DATASOURCE_POOL_H

#include <qore/Datasource.h>
#include <qore/QoreThreadLock.h>
#include <qore/QoreCondition.h>
#include <qore/QoreString.h>
#include <qore/AbstractThreadResource.h>

#include <qore/intern/DatasourceStatementHelper.h>
#include <qore/intern/QoreSQLStatement.h>

#include <map>
#include <deque>

typedef std::map<int, int> thread_use_t;   // for marking a datasource in use
typedef std::deque<int> free_list_t;       // for the free list

class DatasourcePool : public AbstractThreadResource, public QoreCondition, public QoreThreadLock, public DatasourceStatementHelper {
   friend class DatasourcePoolActionHelper;
protected:
   Datasource** pool;
   int* tid_list;            // list of thread IDs per pool index
   thread_use_t tmap;        // map from tids to pool index
   free_list_t free_list;
   unsigned min, 
      max,
      cmax,			 // current max
      wait_count,
      wait_max,
      tl_warning_ms;

   int64 tl_timeout_ms,
      stats_reqs,
      stats_hits
      ;
   ResolvedCallReferenceNode* warning_callback;
   AbstractQoreNode* callback_arg;
   bool valid;

#ifdef DEBUG
   QoreThreadLocalStorage<QoreString> thread_local_storage;
   void addSQL(const char* cmd, const QoreString* sql);
   void resetSQL();
#endif

   DLLLOCAL Datasource* getAllocatedDS();
   DLLLOCAL Datasource* getDSIntern(bool& new_ds, int64& wait_total, ExceptionSink* xsink);
   DLLLOCAL Datasource* getDS(bool& new_ds, ExceptionSink* xsink);
   DLLLOCAL void freeDS();
   // share the code for exec() and execRaw()
   DLLLOCAL AbstractQoreNode* exec_internal(bool doBind, const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink);
   DLLLOCAL int checkWait(int64 warn_total, ExceptionSink* xsink);
     
public:
#ifdef DEBUG
   QoreString* getAndResetSQL();
#endif

   // min must be 1 or more, max must be greater than min
   DLLLOCAL DatasourcePool(ExceptionSink* xsink, DBIDriver* ndsl, const char* user, const char* pass, const char* db, const char* charset, const char* hostname, unsigned mn, unsigned mx, int port = 0, const QoreHashNode* opts = 0, Queue* q = 0, AbstractQoreNode* a = 0);
   DLLLOCAL DatasourcePool(const DatasourcePool& old, ExceptionSink* xsink);

   DLLLOCAL virtual ~DatasourcePool();
   DLLLOCAL void destructor(ExceptionSink* xsink);
   DLLLOCAL virtual void cleanup(ExceptionSink* xsink);
   DLLLOCAL AbstractQoreNode* select(const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink);
   DLLLOCAL QoreHashNode* selectRow(const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink);
   DLLLOCAL AbstractQoreNode* selectRows(const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink);
   DLLLOCAL int beginTransaction(ExceptionSink* xsink);
   DLLLOCAL AbstractQoreNode* exec(const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink);
   DLLLOCAL AbstractQoreNode* execRaw(const QoreString* sql, ExceptionSink* xsink);
   DLLLOCAL QoreHashNode* describe(const QoreString* query_str, const QoreListNode* args, ExceptionSink* xsink);
   DLLLOCAL int commit(ExceptionSink* xsink);
   DLLLOCAL int rollback(ExceptionSink* xsink);
   DLLLOCAL QoreStringNode* toString();
   DLLLOCAL unsigned getMin() const;
   DLLLOCAL unsigned getMax() const;
   DLLLOCAL QoreStringNode* getPendingUsername() const;
   DLLLOCAL QoreStringNode* getPendingPassword() const;
   DLLLOCAL QoreStringNode* getPendingDBName() const;
   DLLLOCAL QoreStringNode* getPendingDBEncoding() const;
   DLLLOCAL QoreStringNode* getPendingHostName() const;
   DLLLOCAL int getPendingPort() const;
   DLLLOCAL const QoreEncoding* getQoreEncoding() const;
   DLLLOCAL const DBIDriver* getDriver () const {
      return pool[0]->getDriver();
   }
   DLLLOCAL const char* getDriverName () const {
      return pool[0]->getDriverName();
   }

   DLLLOCAL AbstractQoreNode* getServerVersion(ExceptionSink* xsink);

   DLLLOCAL AbstractQoreNode* getClientVersion(ExceptionSink* xsink) {
      return pool[0]->getClientVersion(xsink);
   }
   DLLLOCAL bool inTransaction();

   DLLLOCAL QoreHashNode* getOptionHash() const {
      return pool[0]->getOptionHash();
   }

   DLLLOCAL AbstractQoreNode* getOption(const char* opt, ExceptionSink* xsink) {
      return pool[0]->getOption(opt, xsink);
   }

   // functions supporting DatasourceStatementHelper
   DLLLOCAL DatasourceStatementHelper* getReferencedHelper(QoreSQLStatement* s) {
      ref();
      return this;
   }

   // implementing DatasourceStatementHelper virtual functions
   DLLLOCAL virtual void helperDestructor(QoreSQLStatement* s, ExceptionSink* xsink) {
      deref(xsink);
   }

   DLLLOCAL virtual Datasource* helperStartAction(ExceptionSink* xsink, bool& new_transaction) {
      return getDS(new_transaction, xsink);
   }

   DLLLOCAL virtual Datasource* helperEndAction(char cmd, bool new_transaction, ExceptionSink* xsink) {
      //printd(0, "DatasourcePool::helperEndAction() cmd=%d, nt=%d\n", cmd, new_transaction);
      if (cmd == DAH_RELEASE) {
         freeDS();
         return 0;
      }

      return getAllocatedDS();
   }

   DLLLOCAL bool currentThreadInTransaction() const {
      SafeLocker sl((QoreThreadLock*)this);
      return tmap.find(gettid()) != tmap.end();
   }

   DLLLOCAL QoreHashNode* getConfigHash() const;
   DLLLOCAL QoreStringNode* getConfigString() const;

   DLLLOCAL void clearWarningCallback(ExceptionSink* xsink);
   DLLLOCAL void setWarningCallback(int64 warning_ms, ResolvedCallReferenceNode* cb, AbstractQoreNode* arg, ExceptionSink* xsink);
   DLLLOCAL QoreHashNode* getUsageInfo() const;

   DLLLOCAL void setErrorTimeout(unsigned t_ms) {
      tl_timeout_ms = t_ms;
   }

   DLLLOCAL unsigned getErrorTimeout() const {
      return tl_timeout_ms;
   }

   DLLLOCAL void setEventQueue(Queue* q, AbstractQoreNode* arg, ExceptionSink* xsink);
};

class DatasourcePoolActionHelper {
protected:
   DatasourcePool& dsp;
   ExceptionSink* xsink;
   Datasource* ds;
   bool new_ds;
   char cmd;

public:
   DLLLOCAL DatasourcePoolActionHelper(DatasourcePool& n_dsp, ExceptionSink* n_xsink, char n_cmd = DAH_NOCHANGE) : dsp(n_dsp), xsink(n_xsink), new_ds(false), cmd(n_cmd) {
      ds = dsp.getDS(new_ds, xsink);
   }
   DLLLOCAL ~DatasourcePoolActionHelper() {
      if (!ds)
	 return;

      if (cmd == DAH_RELEASE 
          || ds->wasConnectionAborted()
          || (new_ds && ((cmd == DAH_NOCHANGE) || *xsink)))
	 dsp.freeDS();
   }

#if 0
   DLLLOCAL void addSQL(const QoreString* sql) {
      if (ds && !((cmd == DAH_RELEASE) || (new_ds && ((cmd == DAH_NOCHANGE) || *xsink)) || ds->wasConnectionAborted()))
         dsp.addSQL(cmd == DAH_NOCHANGE ? "select" : "exec", sql);
   }
#endif

   DLLLOCAL operator bool() const { return ds; }

   DLLLOCAL Datasource* operator*() const { return ds; }
   DLLLOCAL Datasource* operator->() const { return ds; }
};

#endif
