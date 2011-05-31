/*
 DatasourcePool.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2011 David Nichols
 
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

DatasourcePool::DatasourcePool(DBIDriver *ndsl, const char *user, const char *pass, const char *db, const char *charset, const char *hostname, unsigned mn, unsigned mx, ExceptionSink *xsink) {
   init_pool(ndsl, user, pass, db, charset, hostname, mn, mx, 0, xsink);
}

DatasourcePool::DatasourcePool(DBIDriver *ndsl, const char *user, const char *pass, const char *db, const char *charset, const char *hostname, unsigned mn, unsigned mx, int port, ExceptionSink *xsink) {
   init_pool(ndsl, user, pass, db, charset, hostname, mn, mx, port, xsink);
}

void DatasourcePool::init_pool(DBIDriver *ndsl, const char *user, const char *pass, const char *db, const char *charset, const char *hostname, unsigned mn, unsigned mx, int port, ExceptionSink *xsink) {
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

      ++cmax;
      
      if (*xsink)
	 break;
   } while (cmax < min);

   valid = true;
}

DatasourcePool::~DatasourcePool() {
   //printd(0, "DatasourcePool::~DatasourcePool() trlist.remove() this=%08p\n", this);
   for (unsigned i = 0; i < cmax; ++i)
      delete pool[i];
   delete [] tid_list;
   delete [] pool;
}

void DatasourcePool::cleanup(ExceptionSink *xsink) {
   int tid = gettid();

#ifndef DEBUG_1
   xsink->raiseException("DATASOURCEPOOL-LOCK-EXCEPTION", "%s:%s@%s: TID %d terminated while in a transaction; transaction will be automatically rolled back and the datasource returned to the pool", pool[0]->getDriverName(), pool[0]->getUsernameStr().c_str(), pool[0]->getDBNameStr().c_str(), tid);
#else
   QoreString *sql = getAndResetSQL();
   xsink->raiseException("DATASOURCEPOOL-LOCK-EXCEPTION", "%s:%s@%s: TID %d terminated while in a transaction; transaction will be automatically rolled back and the datasource returned to the pool\n%s", pool[0]->getDriverName(), pool[0]->getUsernameStr().c_str(), pool[0]->getDBNameStr().c_str(), tid, sql ? sql->getBuffer() : "<no data>");
   if (sql)
      delete sql;
#endif

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

void DatasourcePool::destructor(ExceptionSink *xsink) {
   SafeLocker sl((QoreThreadLock *)this);

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

      freeDS();
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
   if (ds && !ds->isOpen() && (ds->open(xsink) || *xsink)) {
      freeDS();
      return 0;
   }

   assert(ds->isOpen());
   return ds;
}

Datasource *DatasourcePool::getAllocatedDS() {
   SafeLocker sl((QoreThreadLock *)this);
   // see if thread already has a datasource allocated
   thread_use_t::iterator i = tmap.find(gettid());
   assert(i != tmap.end());
   return pool[i->second];
}

Datasource *DatasourcePool::getDSIntern(bool &new_ds, ExceptionSink *xsink) {
   assert(!new_ds);

   int tid = gettid();
   
   SafeLocker sl((QoreThreadLock *)this);

   // see if thread already has a datasource allocated
   thread_use_t::iterator i = tmap.find(tid);
   if (i != tmap.end())
      return pool[i->second]; 

   Datasource *ds;

   // will be a new allocation, not already in a transaction
   new_ds = true;
   
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
	    ++wait_count;
	    wait((QoreThreadLock *)this);
	    wait_count--;
	    
	    if (!valid) {
	       xsink->raiseException("DATASOURCEPOOL-ERROR", "%s:%s@%s: DatasourcePool deleted while TID %d waiting on a connection to become free", pool[0]->getDriverName(), pool[0]->getUsernameStr().c_str(), pool[0]->getDBNameStr().c_str(), tid);
	       return 0;
	    }
	    
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
   DatasourcePoolActionHelper dpah(*this, xsink);
   if (!dpah)
      return 0;

   return dpah->select(sql, args, xsink);
}

QoreHashNode *DatasourcePool::selectRow(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink);
   if (!dpah)
      return 0;

   return dpah->selectRow(sql, args, xsink);
}

AbstractQoreNode *DatasourcePool::selectRows(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink);
   if (!dpah)
      return 0;

   return dpah->selectRows(sql, args, xsink);
}

int DatasourcePool::beginTransaction(ExceptionSink *xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink, DAH_ACQUIRE);
   if (!dpah)
      return 0;

   return dpah->beginTransaction(xsink);
}

AbstractQoreNode *DatasourcePool::exec_internal(bool doBind, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink, DAH_ACQUIRE);
   if (!dpah)
      return 0;

   return doBind ? dpah->exec(sql, args, xsink) : dpah->execRaw(sql, xsink);;
}

AbstractQoreNode *DatasourcePool::exec(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) {
   return exec_internal(true, sql, args, xsink);
}

AbstractQoreNode *DatasourcePool::execRaw(const QoreString *sql, ExceptionSink *xsink) {
   return exec_internal(false, sql, 0, xsink);
}

int DatasourcePool::commit(ExceptionSink *xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink, DAH_RELEASE);
   if (!dpah)
      return -1;

   return dpah->commit(xsink);
}

int DatasourcePool::rollback(ExceptionSink *xsink) {
   DatasourcePoolActionHelper dpah(*this, xsink, DAH_RELEASE);
   if (!dpah)
      return -1;

   return dpah->rollback(xsink);
}

QoreStringNode *DatasourcePool::toString() {
   QoreStringNode *str = new QoreStringNode();

   SafeLocker sl((QoreThreadLock *)this);
   str->sprintf("this=%08p, min=%d, max=%d, cmax=%d, wait_count=%d, thread_map = (", this, min, max, cmax, wait_count);
   thread_use_t::const_iterator ti = tmap.begin();
   while (ti != tmap.end()) {
      str->sprintf("tid %d=%d, ", ti->first, ti->second);
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

bool DatasourcePool::inTransaction() {
   int tid = gettid();
   AutoLocker al((QoreThreadLock *)this);
   return tmap.find(tid) != tmap.end();
}
