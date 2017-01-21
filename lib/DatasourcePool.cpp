/*
  DatasourcePool.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#include <qore/Qore.h>
#include "qore/intern/DatasourcePool.h"
#include "qore/intern/qore_ds_private.h"
#include <memory>

DatasourcePool::DatasourcePool(ExceptionSink* xsink, DBIDriver* ndsl, const char* user, const char* pass,
			       const char* db, const char* charset, const char* hostname, unsigned mn, unsigned mx, int port, const QoreHashNode* opts,
			       Queue* q, AbstractQoreNode* a) :
      pool(new Datasource*[mx]),
      tid_list(new int[mx]),
      min(mn),
      max(mx),
      cmax(0),
      wait_count(0),
      wait_max(0),
      tl_warning_ms(0),
      tl_timeout_ms(120000),
      stats_reqs(0),
      stats_hits(0),
      warning_callback(0),
      callback_arg(0),
      config(ndsl, user, pass, db, charset, hostname, port, opts, q, a),
      valid(false) {
   //assert(mn > 0);
   //assert(mx > min);
   //assert(db != 0 && db[0]);

   assert(!(a && !q));

   // create minimum datasources if possible
   printd(5, "DatasourcePool::DatasourcePool(driver: %p user: %s pass: %s db: %s charset: %s host: %s min: %d max: %d port: %d) pool: %p\n",
          ndsl, user ? user : "(null)", pass ? pass : "(null)", db ? db : "(null)", charset ? charset : "(null)",
	  hostname ? hostname : "(null)", min, max, port, pool);

   init(xsink);
}

DatasourcePool::DatasourcePool(const DatasourcePool& old, ExceptionSink* xsink) :
   pool(new Datasource*[old.max]),
   tid_list(new int[old.max]),
   min(old.min),
   max(old.max),
   cmax(0),
   wait_count(0),
   wait_max(0),
   tl_warning_ms(old.tl_warning_ms),
   tl_timeout_ms(old.tl_timeout_ms),
   stats_reqs(0),
   stats_hits(0),
   warning_callback(old.warning_callback ? old.warning_callback->refRefSelf() : 0),
   callback_arg(old.callback_arg ? old.callback_arg->refSelf() : 0),
   config(old.config),
   valid(false) {
   init(xsink);
}

DatasourcePool::~DatasourcePool() {
   //printd(5, "DatasourcePool::~DatasourcePool() this: %p\n", this);
   for (unsigned i = 0; i < cmax; ++i)
      delete pool[i];
   delete [] tid_list;
   delete [] pool;
   assert(!warning_callback);
   assert(!callback_arg);
}

// common constructor code
void DatasourcePool::init(ExceptionSink* xsink) {
   // ths intiial Datasource creation could throw an exception if there is an error in a driver option, for example
   std::unique_ptr<Datasource> ds(config.get(xsink));
   if (*xsink)
      return;

   ds->open(xsink);
   if (*xsink)
      return;

   pool[0] = ds.release();
   //printd(5, "DP::init() open %s: %p (%d)\n", ndsl->getName(), pool[0], xsink->isEvent());
   // add to free list
   free_list.push_back(0);

   while (++cmax < min) {
      ds.reset(config.get());
      ds->open(xsink);
      if (*xsink)
         return;
      pool[cmax] = ds.release();
      //printd(5, "DP::init() open %s: %p (%d)\n", ndsl->getName(), pool[cmax], xsink->isEvent());
      // add to free list
      free_list.push_back(cmax);
   }
   valid = true;
}

void DatasourcePool::cleanup(ExceptionSink* xsink) {
   int tid = gettid();

   // thread must have a Datasource allocated
   SafeLocker sl((QoreThreadLock *)this);
   thread_use_t::iterator i = tmap.find(tid);
   assert(i != tmap.end());
   sl.unlock();

#ifndef DEBUG_1
   xsink->raiseException("DATASOURCEPOOL-LOCK-EXCEPTION", "%s:%s@%s: TID %d terminated while in a transaction with connection %d; transaction will be automatically rolled back and the datasource returned to the pool", pool[0]->getDriverName(), pool[0]->getUsernameStr().c_str(), pool[0]->getDBNameStr().c_str(), tid, i->second);
#else
   QoreString* sql = getAndResetSQL();
   xsink->raiseException("DATASOURCEPOOL-LOCK-EXCEPTION", "%s:%s@%s: TID %d terminated while in a transaction; transaction will be automatically rolled back and the datasource returned to the pool\n%s", pool[0]->getDriverName(), pool[0]->getUsernameStr().c_str(), pool[0]->getDBNameStr().c_str(), tid, sql ? sql->getBuffer() : "<no data>");
   if (sql)
      delete sql;
#endif

   // execute rollback on Datasource before releasing to pool
   pool[i->second]->rollback(xsink);

   // grab lock to add to free list and erase thread map entry
   sl.lock();
   free_list.push_back(i->second);
   // erase thread map entry
   tmap.erase(i);
   // signal any waiting threads
   if (wait_count)
      signal();
}

void DatasourcePool::destructor(ExceptionSink* xsink) {
   //printd(5, "DatasourcePool::destructor() this: %p\n", this);
   SafeLocker sl((QoreThreadLock*)this);

   // mark object as invalid in case any threads are waiting on a free Datasource
   valid = false;

   int tid = gettid();
   thread_use_t::iterator i = tmap.find(tid);
   unsigned curr = i == tmap.end() ? (unsigned)-1 : i->second;

   for (unsigned j = 0; j < cmax; ++j) {
      if (j != curr && pool[j]->isInTransaction())
	 xsink->raiseException("DATASOURCEPOOL-ERROR", "%s:%s@%s: TID %d deleted DatasourcePool while TID %d using connection %d/%d was in a transaction", pool[0]->getDriverName(), pool[0]->getUsernameStr().c_str(), pool[0]->getDBNameStr().c_str(), gettid(), tid_list[j], j + 1, cmax);
   }

   if (i != tmap.end() && pool[curr]->isInTransaction()) {
      xsink->raiseException("DATASOURCEPOOL-LOCK-EXCEPTION", "%s:%s@%s: TID %d deleted DatasourcePool while in a transaction; transaction will be automatically rolled back", pool[0]->getDriverName(), pool[0]->getUsernameStr().c_str(), pool[0]->getDBNameStr().c_str(), tid);
      sl.unlock();

      // execute rollback on Datasource before releasing to pool
      pool[curr]->rollback(xsink);

      freeDS(xsink);
   }

   if (warning_callback) {
      warning_callback->deref(xsink);
      discard(callback_arg, xsink);
#ifdef DEBUG
      warning_callback = 0;
      callback_arg = 0;
#endif
   }

   config.del(xsink);
}

#if 0
void DatasourcePool::addSQL(const char* cmd, const QoreString* sql) {
   QoreString* str = thread_local_storage.get();
   if (!str)
      str = new QoreString;
   else
      str->concat('\n');
   str->sprintf("%s(): %s", cmd, sql->getBuffer());
   thread_local_storage.set(str);
}

void DatasourcePool::resetSQL() {
   QoreString* str = thread_local_storage.get();
   if (str) {
      delete str;
      thread_local_storage.set(0);
   }
}

QoreString* DatasourcePool::getAndResetSQL() {
   QoreString* str = thread_local_storage.get();
   thread_local_storage.set(0);
   return str;
}
#endif

void DatasourcePool::freeDS(ExceptionSink* xsink) {
   // remove from thread resource list
   //printd(5, "DatasourcePool::freeDS() remove_thread_resource(this: %p)\n", this);

   remove_thread_resource(this);

   int tid = gettid();

   AutoLocker al((QoreThreadLock*)this);

   thread_use_t::iterator i = tmap.find(tid);
   assert(!pool[i->second]->isInTransaction());
   free_list.push_back(i->second);

   // issue 1250: close any other statements created on this datasource
   qore_ds_private::get(*pool[i->second])->transactionDone(true, xsink);

   tmap.erase(i);

   if (wait_count)
      signal();
}

Datasource* DatasourcePool::getDS(bool &new_ds, ExceptionSink* xsink) {
   assert(!*xsink);

   // total # of microseconds waiting for a new connection
   int64 wait_total = 0;
   Datasource* ds = getDSIntern(new_ds, wait_total, xsink);
   if (!ds) {
      assert(*xsink);
      return 0;
   }
   assert(ds);
   assert(!*xsink);

   assert(!(new_ds && ds->isInTransaction()));

   // only run the wait callback if we have a wait time and if no exception was raised acquiring a datasource
   if (wait_total && checkWait(wait_total, xsink)) {
      assert(new_ds);
      assert(!ds->isInTransaction());
      freeDS(xsink);
      return 0;
   }

   // try to open Datasource if it's not open already
   if (!ds->isOpen()) {
      assert(new_ds);
      if (ds->open(xsink)) {
	 assert(!ds->isInTransaction());
	 freeDS(xsink);
	 return 0;
      }
   }

   assert(ds->isOpen());
   assert(!*xsink);

   return ds;
}

Datasource* DatasourcePool::getAllocatedDS() {
   SafeLocker sl((QoreThreadLock*)this);
   // see if thread already has a datasource allocated
   thread_use_t::iterator i = tmap.find(gettid());
   assert(i != tmap.end());
   return pool[i->second];
}

// must be called in the lock
int DatasourcePool::checkWait(int64 wait_total, ExceptionSink* xsink) {
   assert(wait_total);

   ReferenceHolder<ResolvedCallReferenceNode> wc(xsink);

   {
      // get reference to callback and check wait threshold only while holding lock
      AutoLocker al((QoreThreadLock*)this);
      if (!warning_callback || (wait_total / 1000) < tl_warning_ms)
	 return 0;

      wc = warning_callback->refRefSelf();
   }

   // build argument list
   ReferenceHolder<QoreListNode> args(new QoreListNode, xsink);
   args->push(getConfigString());
   args->push(new QoreBigIntNode(wait_total));
   args->push(new QoreBigIntNode(tl_warning_ms));
   args->push(callback_arg ? callback_arg->refSelf() : 0);
   wc->execValue(*args, xsink).discard(xsink);
   return *xsink ? -1 : 0;
}

Datasource* DatasourcePool::getDSIntern(bool& new_ds, int64& wait_total, ExceptionSink* xsink) {
   assert(!new_ds);

   int tid = gettid();

   Datasource* ds;

   SafeLocker sl((QoreThreadLock*)this);

   // increase request counter
   ++stats_reqs;

   // see if thread already has a datasource allocated
   thread_use_t::iterator i = tmap.find(tid);
   if (i != tmap.end()) {
      ++stats_hits;
      //printd(5, "DatasourcePool::getDSIntern() this: %p returning already allocated ds: %p\n", this, pool[i->second]);
      return pool[i->second];
   }

   // will be a new allocation, not already in a transaction
   new_ds = true;

   // iteration flag
   bool iter = false;

   // see if there is a datasource free
   while (true) {
      if (!free_list.empty()) {
	 int fi = free_list.front();
	 free_list.pop_front();
	 // DEBUG
	 //printf("DSP::getDS() assigning tid %d index %d from free list (%N)\n", $tid, $i, $.p[$i]);

	 tmap[tid] = fi;
	 ds = pool[fi];
	 tid_list[fi] = tid;

	 // increase hit counter
	 if (!iter)
	    ++stats_hits;
	 break;
      }

      // see if we can open a new connection
      if (cmax < max) {
	 ds = pool[cmax] = config.get();

	 tmap[tid] = cmax;
	 tid_list[cmax++] = tid;

	 // increase hit counter
	 if (!iter)
	    ++stats_hits;

	 break;
      }

      //printd(5, "DatasourcePool::getDSIntern() this: %p tl_timeout_ms: %d max: %d\n", this, tl_timeout_ms, max);
      // otherwise we sleep until a connection becomes available
      ++wait_count;
      int64 warn_start = q_clock_getmicros();
      int rc = tl_timeout_ms ? wait((QoreThreadLock*)this, tl_timeout_ms) : wait((QoreThreadLock*)this);
      wait_count--;

      // add waiting time to total time
      wait_total += (q_clock_getmicros() - warn_start);

      if (!valid) {
	 xsink->raiseException("DATASOURCEPOOL-ERROR", "%s:%s@%s: DatasourcePool deleted while TID %d waiting on a connection to become free", getDriverName(), pool[0]->getUsernameStr().c_str(), pool[0]->getDBNameStr().c_str(), tid);
	 return 0;
      }

      if (rc && tl_timeout_ms) {
	 xsink->raiseException("DATASOURCEPOOL-TIMEOUT", "%s:%s@%s: TID %d timed out on datasource pool after waiting %d millisecond%s for a free connection (max %d connections in use)",
			       getDriverName(), pool[0]->getUsernameStr().c_str(), pool[0]->getDBNameStr().c_str(), tid,
			       tl_timeout_ms, tl_timeout_ms == 1 ? "" : "s", max);
	 return 0;
      }

      if (!iter)
	 iter = true;
      continue;
   }

   if (wait_total > wait_max)
      wait_max = wait_total;

   sl.unlock();

   // add to thread resource list
   //printd(5, "DatasourcePool::getDSIntern() set_thread_resource(this: %p) ds: %p\n", this, ds);

   set_thread_resource(this);

   assert(ds);
   return ds;
}

AbstractQoreNode* DatasourcePool::select(const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink);
   if (!dpah)
      return 0;

   return dpah->select(sql, args, xsink);
}

QoreHashNode* DatasourcePool::selectRow(const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink);
   if (!dpah)
      return 0;

   return dpah->selectRow(sql, args, xsink);
}

AbstractQoreNode* DatasourcePool::selectRows(const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink);
   if (!dpah)
      return 0;

   return dpah->selectRows(sql, args, xsink);
}

QoreHashNode* DatasourcePool::describe(const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink);
   if (!dpah)
      return 0;

   return dpah->describe(sql, args, xsink);
}

int DatasourcePool::beginTransaction(ExceptionSink* xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink, DAH_ACQUIRE);
   if (!dpah)
      return 0;

   return dpah->beginTransaction(xsink);
}

AbstractQoreNode* DatasourcePool::exec_internal(bool doBind, const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink, DAH_ACQUIRE);
   if (!dpah)
      return 0;

   return doBind ? dpah->exec(sql, args, xsink) : dpah->execRaw(sql, args, xsink);;
}

AbstractQoreNode* DatasourcePool::exec(const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink) {
   return exec_internal(true, sql, args, xsink);
}

AbstractQoreNode* DatasourcePool::execRaw(const QoreString* sql, ExceptionSink* xsink) {
   return exec_internal(false, sql, 0, xsink);
}

int DatasourcePool::commit(ExceptionSink* xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink, DAH_RELEASE);
   if (!dpah)
      return -1;

   //printd(5, "DatasourcePool::commit() this: %p ds: %p: %s@%s\n", this, *dpah, dpah->getUsername(), dpah->getDBName());

   return dpah->commit(xsink);
}

int DatasourcePool::rollback(ExceptionSink* xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink, DAH_RELEASE);
   if (!dpah)
      return -1;

   //printd(5, "DatasourcePool::rollback() this: %p ds: %p\n", this, *dpah);

   return dpah->rollback(xsink);
}

AbstractQoreNode* DatasourcePool::getServerVersion(ExceptionSink* xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink);
   if (!dpah)
      return 0;

   return dpah->getServerVersion(xsink);
}

QoreStringNode* DatasourcePool::toString() {
   QoreStringNode* str = new QoreStringNode();

   SafeLocker sl((QoreThreadLock *)this);
   str->sprintf("this: %p, min: %d, max: %d, cmax: %d, wait_count: %d, thread_map = (", this, min, max, cmax, wait_count);
   thread_use_t::const_iterator ti = tmap.begin();
   while (ti != tmap.end()) {
      str->sprintf("tid %d: %d, ", ti->first, ti->second);
      ++ti;
   }
   if (!tmap.empty())
      str->terminate(str->strlen() - 2);

   str->sprintf("), free_list = (");
   free_list_t::const_iterator fi = free_list.begin();
   while (fi != free_list.end()) {
      str->sprintf("%d, ", *fi);
      ++fi;
   }
   if (!free_list.empty())
      str->terminate(str->strlen() - 2);
   sl.unlock();
   str->concat(')');
   return str;
}

unsigned DatasourcePool::getMin() const {
   return min;
}

unsigned DatasourcePool::getMax() const {
   return max;
}

QoreStringNode* DatasourcePool::getPendingUsername() const {
   return pool[0]->getPendingUsername();
}

QoreStringNode* DatasourcePool::getPendingPassword() const {
   return pool[0]->getPendingPassword();
}

QoreStringNode* DatasourcePool::getPendingDBName() const {
   return pool[0]->getPendingDBName();
}

QoreStringNode* DatasourcePool::getPendingDBEncoding() const {
   return pool[0]->getPendingDBEncoding();
}

QoreStringNode* DatasourcePool::getPendingHostName() const {
   return pool[0]->getPendingHostName();
}

int DatasourcePool::getPendingPort() const {
   return pool[0]->getPendingPort();
}

const QoreEncoding* DatasourcePool::getQoreEncoding() const {
   return pool[0]->getQoreEncoding();
}

bool DatasourcePool::inTransaction() {
   int tid = gettid();
   AutoLocker al((QoreThreadLock *)this);
   return tmap.find(tid) != tmap.end();
}

QoreHashNode* DatasourcePool::getConfigHash() const {
   QoreHashNode* h = pool[0]->getConfigHash();

   // add min and max options
   QoreHashNode* opt = reinterpret_cast<QoreHashNode*>(h->getKeyValue("options"));
   if (!opt) {
      opt = new QoreHashNode;
      h->setKeyValue("options", opt, 0);
   }
   opt->setKeyValue("min", new QoreBigIntNode(min), 0);
   opt->setKeyValue("max", new QoreBigIntNode(max), 0);

   return h;
}

QoreStringNode* DatasourcePool::getConfigString() const {
   QoreStringNode* str = pool[0]->getConfigString();

   // add min and max options
   QoreStringMaker mm(",min=%d,max=%d", min, max);
   if ((*str)[str->size() - 1] == '}')
      str->splice(str->size() - 1, 0, mm, 0);
   else
      str->sprintf("{%s}", mm.getBuffer() + 1);

   return str;
}

void DatasourcePool::clearWarningCallback(ExceptionSink* xsink) {
   AutoLocker al((QoreThreadLock*)this);
   if (warning_callback) {
      discard(callback_arg, xsink);
      warning_callback->deref(xsink);
      warning_callback = 0;
      tl_warning_ms = 0;
   }
}

void DatasourcePool::setWarningCallback(int64 warning_ms, ResolvedCallReferenceNode* cb, AbstractQoreNode* arg, ExceptionSink* xsink) {
   if (warning_ms <= 0) {
      xsink->raiseException("DATASOURCEPOOL-SETWARNINGCALLBACK-ERROR", "DatasourcePool::setWarningCallback() warning ms argument: " QLLD " must be greater than zero; to clear, call DatasourcePool::clearWarningCallback() with no arguments", warning_ms);
      return;
   }
   AutoLocker al((QoreThreadLock*)this);
   if (warning_callback) {
      warning_callback->deref(xsink);
      if (callback_arg)
	 callback_arg->deref(xsink);
   }
   warning_callback = cb;
   tl_warning_ms = warning_ms;
   callback_arg = arg;
}

QoreHashNode* DatasourcePool::getUsageInfo() const {
   AutoLocker al((QoreThreadLock*)this);
   QoreHashNode* h = new QoreHashNode;
   if (warning_callback) {
      h->setKeyValue("callback", warning_callback->refRefSelf(), 0);
      h->setKeyValue("arg", callback_arg ? callback_arg->refSelf() : 0, 0);
      h->setKeyValue("timeout", new QoreBigIntNode(tl_warning_ms), 0);
   }
   h->setKeyValue("wait_max", new QoreBigIntNode(wait_max), 0);
   h->setKeyValue("stats_reqs", new QoreBigIntNode(stats_reqs), 0);
   h->setKeyValue("stats_hits", new QoreBigIntNode(stats_hits), 0);
   return h;
}

void DatasourcePool::setEventQueue(Queue* q, AbstractQoreNode* arg, ExceptionSink* xsink) {
   AutoLocker al((QoreThreadLock*)this);

   // assign to all datasources
   bool first = true;
   for (unsigned i = 0; i < cmax; ++i) {
      if (first) {
	 pool[i]->setEventQueue(q, arg, xsink);
	 first = false;
      }
      else
	 pool[i]->setEventQueue(q ? q->queueRefSelf() : 0, arg ? arg->refSelf() : 0, xsink);
   }

   config.setQueue(q, arg, xsink);
}

QoreListNode* DatasourcePool::getCapabilityList() const {
   return pool[0]->getCapabilityList();
}

int DatasourcePool::getCapabilities() const {
   return pool[0]->getCapabilities();
}
