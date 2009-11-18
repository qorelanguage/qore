/*
 DatasourcePool.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
 The Datasource class provides the low-level interface to Qore DBI drivers.
 
 NOTE that this class is *not* thread-safe.  To use this class in a multi-
 threaded context, per-thread connection locking must be done at a level
 above this class...
 
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

#include <qore/Qore.h>
#include <qore/intern/DatasourcePool.h>

DatasourcePool::DatasourcePool(DBIDriver *ndsl, const char *user, const char *pass, const char *db, const char *charset, const char *hostname, int mn, int mx, ExceptionSink *xsink) {
   init(ndsl, user, pass, db, charset, hostname, mn, mx, 0, xsink);
}

DatasourcePool::DatasourcePool(DBIDriver *ndsl, const char *user, const char *pass, const char *db, const char *charset, const char *hostname, int mn, int mx, int port, ExceptionSink *xsink) {
   init(ndsl, user, pass, db, charset, hostname, mn, mx, port, xsink);
}

void DatasourcePool::init(DBIDriver *ndsl, const char *user, const char *pass, const char *db, const char *charset, const char *hostname, int mn, int mx, int port, ExceptionSink *xsink) {
   //assert(mn > 0);
   //assert(mx > min);   
   //assert(db != 0 && db[0]);

   wait_count = 0;
   min = mn;
   max = mx;
   // allocate memory for lists
   pool     = new Datasource *[max];
   tid_list = new int[max];

   // create minimum datasources if possible
   printd(5, "DatasourcePool::DatasourcePool(driver=%08p user=%s pass=%s db=%s charset=%s host=%s min=%d max=%d port=%d) pool=%08p\n", 
          ndsl, user ? user : "(null)", pass ? pass : "(null)", db ? db : "(null)", charset ? charset : "(null)", hostname ? hostname : "(null)", min, max, port, pool);
   cmax = 0;
   do {
      pool[cmax] = new Datasource(ndsl);
      if (user)     pool[cmax]->setPendingUsername(user);
      if (pass)     pool[cmax]->setPendingPassword(pass);
      if (db)       pool[cmax]->setPendingDBName(db);
      if (charset)  pool[cmax]->setPendingDBEncoding(charset);
      if (hostname) pool[cmax]->setPendingHostName(hostname);
      if (port)     pool[cmax]->setPendingPort(port);
      pool[cmax]->setAutoCommit(false);
      pool[cmax]->open(xsink);
 
      //printd(0, "DP::DP() open %s: %08p (%d)\n", ndsl->getName(), pool[cmax], xsink->isEvent());

      // add to free list
      free_list.push_back(cmax);

      cmax++;
      
      if (xsink->isException())
	 break;
   } while (cmax < min);
   valid = true;
}

DatasourcePool::~DatasourcePool() {
   //printd(0, "DatasourcePool::~DatasourcePool() trlist.remove() this=%08p\n", this);
   
   for (int i = 0; i < cmax; i++)
      delete pool[i];
   delete [] tid_list;
   delete [] pool;
}

void DatasourcePool::destructor(ExceptionSink *xsink) {
   AutoLocker al((QoreThreadLock *)this);

   for (int i = 0; i < cmax; i++) {
      if (pool[i]->isInTransaction()) {
	 xsink->raiseException("DATASOURCEPOOL-ERROR", "TID %d deleted DatasourcePool while TID %d using connection %d/%d was in a transaction", gettid(), tid_list[i], i + 1, cmax);
      }
   } 
}

#ifdef DEBUG
void DatasourcePool::addSQL(const char *cmd, const QoreString *sql) {
   QoreString *str = thread_local_storage.get();
   if (!str)
      str = new QoreString();
   else
      str->concat('\n');
   str->sprintf("%s(): %s", cmd, sql->getBuffer());
   thread_local_storage.set(str);
}

void DatasourcePool::resetSQL() {
   QoreString *str = thread_local_storage.get();
   if (str) {
      delete str;
      thread_local_storage.set(0);
   }
}

QoreString *DatasourcePool::getAndResetSQL() {
   QoreString *str = thread_local_storage.get();
   thread_local_storage.set(0);
   return str;
}
#endif

void DatasourcePool::freeDS() {
   // remove from thread resource list
   //printd(0, "DatasourcePool::freeDS() remove_thread_resource(this=%08p), tid=%d\n", this, tid);
   remove_thread_resource(this);

   int tid = gettid();

   AutoLocker al((QoreThreadLock *)this);

   thread_use_t::iterator i = tmap.find(tid);
   free_list.push_back(i->second);
   tmap.erase(i);
   if (wait_count)
      signal();
}      

Datasource *DatasourcePool::getDS(bool &new_ds, ExceptionSink *xsink) {
   Datasource *ds = getDSIntern(new_ds, xsink);

   // try to open Datasource if it's not open already
   if (!ds->isOpen() && ds->open(xsink)) {
      freeDS();
      return 0;
   }

   return ds;
}

Datasource *DatasourcePool::getDSIntern(bool &new_ds, ExceptionSink *xsink) {
   int tid = gettid();
   
   SafeLocker sl((QoreThreadLock *)this);
   // see if thread already has a datasource allocated
   thread_use_t::iterator i = tmap.find(tid);
   if (i != tmap.end()) {
      // DEBUG
      // printf("DSP::getDS() tid %d has index %d (%N)\n", $tid, $.t.$tid, $.p[$.t.$tid]);
      int index = i->second;
      return pool[index];
   }
   
   // will be a new allocation, not already in a transaction
   new_ds = true;
   Datasource *ds;
   
   // see if there is a datasource free
   while (true) {
      if (!free_list.empty()) {
	 int i = free_list.front();
	 free_list.pop_front();
	 // DEBUG
	 //printf("DSP::getDS() assigning tid %d index %d from free list (%N)\n", $tid, $i, $.p[$i]);
	 
	 tmap[tid] = i;
	 ds = pool[i];
	 tid_list[i] = tid;
      }
      else {
	 // see if we can open a new connection
	 if (cmax < max) {
	    ds = pool[cmax] = pool[0]->copy();
	    
	    tmap[tid] = cmax;
	    tid_list[cmax++] = tid;
	 }
	 else {
	    // otherwise we sleep until a connection becomes available
	    wait_count++;
	    wait((QoreThreadLock *)this);
	    wait_count--;
	    continue;
	 }
      }
      break;
   }
   
   sl.unlock();

   // add to thread resource list
   //printd(0, "DatasourcePool::getDS() set_thread_resource(this=%08p), tid=%d\n", this, gettid());
   set_thread_resource(this);

   return ds;
}

AbstractQoreNode *DatasourcePool::select(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) {
   bool new_ds = false;
   AbstractQoreNode *rv = 0;
   Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return 0;

   rv = ds->select(sql, args, xsink);

   if (new_ds || ds->wasConnectionAborted())
      freeDS();
#ifdef DEBUG
   else
      addSQL("select", sql);
#endif

   //printf("DSP::select() ds=%N SQL=%n rv=%n\n", $ds, $sql, $rv);   
   return rv;
}

// FIXME: should be a native DBI driver method
AbstractQoreNode *DatasourcePool::selectRow(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) {
   bool new_ds = false;
   AbstractQoreNode *rv;
   Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return 0;

   rv = ds->selectRows(sql, args, xsink);
   //printd(5, "DatasourcePool::selectRow() ds=%08p, trans=%d, xsink=%d, new_ds=%d\n", ds, ds->isInTransaction(), xsink->isException(), new_ds);

   if (new_ds || ds->wasConnectionAborted())
      freeDS();
#ifdef DEBUG
   else
      addSQL("selectRow", sql);
#endif

   // return only hash of first row, if any
   QoreListNode *l = dynamic_cast<QoreListNode *>(rv);
   if (l) {
      AbstractQoreNode *h = l->shift();
      rv->deref(xsink);
      rv = h;
   }
   //printd(5, "DSP::selectRow() ds=%08p SQL=%s rv=%s (%08p)\n", ds, sql->getBuffer(), rv ? rv->getTypeName() : "(null)", rv);
   
   return rv;
}

AbstractQoreNode *DatasourcePool::selectRows(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) {
   bool new_ds = false;
   AbstractQoreNode *rv;
   Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return 0;

   rv = ds->selectRows(sql, args, xsink);
   if (new_ds || ds->wasConnectionAborted())
      freeDS();
#ifdef DEBUG
   else
      addSQL("selectRows", sql);
#endif
   //printf("DSP::selectRow() ds=%N SQL=%n rv=%n\n", $ds, $sql, $rv);
   
   return rv;
}

int DatasourcePool::beginTransaction(ExceptionSink *xsink) {
   bool new_ds = false;
   Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return -1;

   int rc = ds->beginTransaction(xsink);

   if ((xsink->isException() && new_ds) || ds->wasConnectionAborted())
      freeDS();

   // DEBUG
   //printf("DSP::beginTransaction() ds=%N\n", $ds);
   return rc;
}

AbstractQoreNode *DatasourcePool::exec(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) {
   bool new_ds = false;
   Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return 0;
   
#ifdef DEBUG
   addSQL("exec", sql);
#endif

   AbstractQoreNode *rv = ds->exec(sql, args, xsink);
   //printd(5, "DatasourcePool::exec() ds=%08p, trans=%d, xsink=%d, new_ds=%d\n", ds, ds->isInTransaction(), xsink->isException(), new_ds);

   if ((xsink->isException() && new_ds) || ds->wasConnectionAborted())
      freeDS();
   
   // DEBUG
   //printf("DSP::beginTransaction() ds=%N\n", $ds);
   return rv;
}

int DatasourcePool::commit(ExceptionSink *xsink) {
   bool new_ds = false;
   Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return -1;

   int rc = ds->commit(xsink);
   freeDS();

#ifdef DEBUG
   resetSQL();
#endif

   return rc;
}

int DatasourcePool::rollback(ExceptionSink *xsink) {
   bool new_ds = false;
   Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return -1;

   int rc = ds->rollback(xsink);
   freeDS();

#ifdef DEBUG
   resetSQL();
#endif

   return rc;
}

QoreStringNode *DatasourcePool::toString() {
   QoreStringNode *str = new QoreStringNode();

   SafeLocker sl((QoreThreadLock *)this);
   str->sprintf("this=%08p, min=%d, max=%d, cmax=%d, wait_count=%d, thread_map = (", this, min, max, cmax, wait_count);
   thread_use_t::const_iterator ti = tmap.begin();
   while (ti != tmap.end()) {
      str->sprintf("tid %d=%d, ", ti->first, ti->second);
      ti++;
   }
   if (!tmap.empty())
      str->terminate(str->strlen() - 2);

   str->sprintf("), free_list = (");
   free_list_t::const_iterator fi = free_list.begin();
   while (fi != free_list.end()) {
      str->sprintf("%d, ", *fi);
      fi++;
   }
   if (!free_list.empty())
      str->terminate(str->strlen() - 2);
   sl.unlock();
   str->concat(')');
   return str;
}

int DatasourcePool::getMin() const { 
   return min; 
}

int DatasourcePool::getMax() const { 
   return max; 
}

QoreStringNode *DatasourcePool::getPendingUsername() const {
   return pool[0]->getPendingUsername();
}

QoreStringNode *DatasourcePool::getPendingPassword() const {
   return pool[0]->getPendingPassword();
}

QoreStringNode *DatasourcePool::getPendingDBName() const {
   return pool[0]->getPendingDBName();
}

QoreStringNode *DatasourcePool::getPendingDBEncoding() const {
   return pool[0]->getPendingDBEncoding();
}

QoreStringNode *DatasourcePool::getPendingHostName() const {
   return pool[0]->getPendingHostName();
}

int DatasourcePool::getPendingPort() const {
   return pool[0]->getPendingPort();
}

const QoreEncoding *DatasourcePool::getQoreEncoding() const {
   return pool[0]->getQoreEncoding();
}

void DatasourcePool::cleanup(ExceptionSink *xsink) {
#ifndef DEBUG
   xsink->raiseException("DATASOURCEPOOL-LOCK-EXCEPTION", "TID %d terminated while in a transaction; transaction will be automatically rolled back and the datasource returned to the pool", gettid());
#else
   class QoreString *sql = getAndResetSQL();
   xsink->raiseException("DATASOURCEPOOL-LOCK-EXCEPTION", "TID %d terminated while in a transaction; transaction will be automatically rolled back and the datasource returned to the pool\n%s", gettid(), sql ? sql->getBuffer() : "<no data>");
   if (sql)
      delete sql;
#endif

   int tid = gettid();
   // thread must have a Datasource allocated
   SafeLocker sl((QoreThreadLock *)this);
   thread_use_t::iterator i = tmap.find(tid);
   assert(i != tmap.end());
   sl.unlock();

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

bool DatasourcePool::inTransaction() {
   int tid = gettid();
   AutoLocker al((QoreThreadLock *)this);
   return tmap.find(tid) != tmap.end();
}
