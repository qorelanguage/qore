/*
 DatasourcePool.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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
#include <qore/DatasourcePool.h>

DatasourcePool::DatasourcePool(DBIDriver *ndsl, const char *user, const char *pass, const char *db, const char *charset, const char *hostname, int mn, int mx, class ExceptionSink *xsink)
{
   //assert(mn > 0);
   //assert(mx > min);   
   //assert(db != NULL && db[0]);

#ifdef DEBUG
   pthread_key_create(&thread_local_storage, NULL);
#endif

   wait_count = 0;
   min = mn;
   max = mx;
   // allocate memory for lists
   pool     = new Datasource *[max];
   tid_list = new int[max];

   // create minimum datasources if possible
   printd(5, "DatasourcePool::DatasourcePool(driver=%08p user=%s pass=%s db=%s charset=%s host=%s min=%d max=%d) pool=%08p\n", 
          ndsl, user ? user : "(null)", pass ? pass : "(null)", db ? db : "(null)", charset ? charset : "(null)", hostname ? hostname : "(null)", min, max, pool);
   cmax = 0;
   do
   {
      pool[cmax] = new Datasource(ndsl);
      if (user)     pool[cmax]->setPendingUsername(user);
      if (pass)     pool[cmax]->setPendingPassword(pass);
      if (db)       pool[cmax]->setPendingDBName(db);
      if (charset)  pool[cmax]->setPendingDBEncoding(charset);
      if (hostname) pool[cmax]->setPendingHostName(hostname);
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

DatasourcePool::~DatasourcePool()
{
#ifdef DEBUG
   pthread_key_delete(thread_local_storage);
#endif

   //printd(0, "DatasourcePool::~DatasourcePool() trlist.remove() this=%08p\n", this);
   
   for (int i = 0; i < cmax; i++)
      delete pool[i];
   delete [] tid_list;
   delete [] pool;
}

void DatasourcePool::destructor(class ExceptionSink *xsink)
{
   AutoLocker al((LockedObject *)this);

   for (int i = 0; i < cmax; i++)
   {
      if (pool[i]->isInTransaction())
      {
	 xsink->raiseException("DATASOURCEPOOL-ERROR", "TID %d deleted DatasourcePool while TID %d using connection %d/%d was in a transaction", gettid(), tid_list[i], i + 1, cmax);
      }
   } 
}

#ifdef DEBUG
void DatasourcePool::addSQL(char *cmd, QoreString *sql)
{
   class QoreString *str = (QoreString *)pthread_getspecific(thread_local_storage);
   if (!str)
      str = new QoreString();
   else
      str->concat('\n');
   str->sprintf("%s(): %s", cmd, sql->getBuffer());
   pthread_setspecific(thread_local_storage, str);
}

void DatasourcePool::resetSQL()
{
   class QoreString *str = (QoreString *)pthread_getspecific(thread_local_storage);
   if (str)
   {
      delete str;
      pthread_setspecific(thread_local_storage, NULL);
   }
}

class QoreString *DatasourcePool::getAndResetSQL()
{
   class QoreString *str = (QoreString *)pthread_getspecific(thread_local_storage);
   pthread_setspecific(thread_local_storage, NULL);
   return str;
}
#endif

void DatasourcePool::freeDS()
{
   // remove from thread resource list
   int tid = gettid();

   //printd(0, "DatasourcePool::freeDS() trlist.remove() this=%08p, tid=%d\n", this, tid);
   remove_thread_resource(this);

   AutoLocker al((LockedObject *)this);

   thread_use_t::iterator i = tmap.find(tid);
   free_list.push_back(i->second);
   tmap.erase(i);
   if (wait_count)
      signal();
}      

class Datasource *DatasourcePool::getDS(bool &new_ds, class ExceptionSink *xsink)
{
   int tid = gettid();
   
   SafeLocker sl((LockedObject *)this);
   // see if thread already has a datasource allocated
   thread_use_t::iterator i = tmap.find(tid);
   if (i != tmap.end())
   {
      // DEBUG
      // printf("DSP::getDS() tid %d has index %d (%N)\n", $tid, $.t.$tid, $.p[$.t.$tid]);
      int index = i->second;
      return pool[index];
   }
   
   // will be a new allocation, not already in a transaction
   new_ds = True;
   class Datasource *ds;
   
   // see if there is a datasource free
   while (true)
   {
      if (!free_list.empty())
      {
	 int i = free_list.front();
	 free_list.pop_front();
	 // DEBUG
	 //printf("DSP::getDS() assigning tid %d index %d from free list (%N)\n", $tid, $i, $.p[$i]);
	 
	 tmap[tid] = i;
	 ds = pool[i];
	 tid_list[i] = tid;
      }
      else
      {
	 // see if we can open a new connection
	 if (cmax < max)
	 {
	    ds = pool[cmax] = pool[0]->copy();
	    if (ds->open(xsink))
	    {
	       delete ds;
	       return NULL;
	    }

	    tmap[tid] = cmax;
	    tid_list[cmax++] = tid;
	 }
	 else
	 {
	    // otherwise we sleep until a connection becomes available
	    wait_count++;
	    wait((LockedObject *)this);
	    wait_count--;
	    continue;
	 }
      }
      break;
   }
   
   sl.unlock();

   // add to thread resource list
   //printd(0, "DatasourcePool::getDS() trlist.set() this=%08p, tid=%d\n", this, gettid());
   set_thread_resource(this);
   
   return ds;
}

class QoreNode *DatasourcePool::select(class QoreString *sql, List *args, class ExceptionSink *xsink)
{
   bool new_ds = false;
   class QoreNode *rv = NULL;
   class Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return NULL;

   rv = ds->select(sql, args, xsink);

   if (new_ds)
      freeDS();
#ifdef DEBUG
   else
      addSQL("select", sql);
#endif
   //printf("DSP::selectRow() ds=%N SQL=%n rv=%n\n", $ds, $sql, $rv);
   
   return rv;
}

// FIXME: should be a native DBI driver method
class QoreNode *DatasourcePool::selectRow(class QoreString *sql, List *args, class ExceptionSink *xsink)
{
   bool new_ds = false;
   class QoreNode *rv;
   class Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return NULL;

   rv = ds->selectRows(sql, args, xsink);
   if (new_ds)
      freeDS();
#ifdef DEBUG
   else
      addSQL("selectRow", sql);
#endif

   // return only hash of first row, if any
   if (rv && rv->type == NT_LIST)
   {
      class QoreNode *h = rv->val.list->shift();
      rv->deref(xsink);
      if (h)
	 rv = h;
      else
	 rv = NULL;
   }
   //printd(5, "DSP::selectRow() ds=%08p SQL=%s rv=%s (%08p)\n", ds, sql->getBuffer(), rv ? rv->type->name : "(null)", rv);
   
   return rv;
}

class QoreNode *DatasourcePool::selectRows(class QoreString *sql, List *args, class ExceptionSink *xsink)
{
   bool new_ds = false;
   class QoreNode *rv;
   class Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return NULL;

   rv = ds->selectRows(sql, args, xsink);
   if (new_ds)
      freeDS();
#ifdef DEBUG
   else
      addSQL("selectRows", sql);
#endif
   //printf("DSP::selectRow() ds=%N SQL=%n rv=%n\n", $ds, $sql, $rv);
   
   return rv;
}

int DatasourcePool::beginTransaction(class ExceptionSink *xsink)
{
   bool new_ds = false;
   class Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return -1;

   int rc = ds->beginTransaction(xsink);

   if (xsink->isException() && new_ds)
      freeDS();

   // DEBUG
   //printf("DSP::beginTransaction() ds=%N\n", $ds);
   return rc;
}

class QoreNode *DatasourcePool::exec(class QoreString *sql, List *args, class ExceptionSink *xsink)
{
   bool new_ds = false;
   class Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return NULL;
   
#ifdef DEBUG
   addSQL("exec", sql);
#endif

   class QoreNode *rv = ds->exec(sql, args, xsink);
   
   if (xsink->isException() && new_ds)
      freeDS();
   
   // DEBUG
   //printf("DSP::beginTransaction() ds=%N\n", $ds);
   return rv;
}

int DatasourcePool::commit(class ExceptionSink *xsink)
{
   bool new_ds = false;
   class Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return -1;

   int rc = ds->commit(xsink);
   freeDS();

#ifdef DEBUG
   resetSQL();
#endif

   return rc;
}

int DatasourcePool::rollback(class ExceptionSink *xsink)
{
   bool new_ds = false;
   class Datasource *ds = getDS(new_ds, xsink);

   if (!ds)
      return -1;

   int rc = ds->rollback(xsink);
   freeDS();

#ifdef DEBUG
   resetSQL();
#endif

   return rc;
}

class QoreString *DatasourcePool::toString()
{
   class QoreString *str = new QoreString();

   SafeLocker sl((LockedObject *)this);
   str->sprintf("this=%08p, min=%d, max=%d, cmax=%d, wait_count=%d, thread_map = (", this, min, max, cmax, wait_count);
   thread_use_t::const_iterator ti = tmap.begin();
   while (ti != tmap.end())
   {
      str->sprintf("tid %d=%d, ", ti->first, ti->second);
      ti++;
   }
   if (!tmap.empty())
      str->terminate(str->strlen() - 2);

   str->sprintf("), free_list = (");
   free_list_t::const_iterator fi = free_list.begin();
   while (fi != free_list.end())
   {
      str->sprintf("%d, ", *fi);
      fi++;
   }
   if (!free_list.empty())
      str->terminate(str->strlen() - 2);
   sl.unlock();
   str->concat(')');
   return str;
}

int DatasourcePool::getMin() const 
{ 
   return min; 
}

int DatasourcePool::getMax() const 
{ 
   return max; 
}

class QoreNode *DatasourcePool::getPendingUsername() const
{
   return pool[0]->getPendingUsername();
}

class QoreNode *DatasourcePool::getPendingPassword() const
{
   return pool[0]->getPendingPassword();
}

class QoreNode *DatasourcePool::getPendingDBName() const
{
   return pool[0]->getPendingDBName();
}

class QoreNode *DatasourcePool::getPendingDBEncoding() const
{
   return pool[0]->getPendingDBEncoding();
}

class QoreNode *DatasourcePool::getPendingHostName() const
{
   return pool[0]->getPendingHostName();
}

class QoreEncoding *DatasourcePool::getQoreEncoding() const
{
   return pool[0]->getQoreEncoding();
}

void DatasourcePool::cleanup(class ExceptionSink *xsink)
{
#ifndef DEBUG
   xsink->raiseException("DATASOURCEPOOL-LOCK-EXCEPTION", "TID %d terminated while in a transaction; transaction will be automatically rolled back and the datasource returned to the pool", gettid());
#else
   class QoreString *sql = getAndResetSQL();
   xsink->raiseException("DATASOURCEPOOL-LOCK-EXCEPTION", "TID %d terminated while in a transaction; transaction will be automatically rolled back and the datasource returned to the pool\n%s", gettid(), sql ? sql->getBuffer() : "<no data>");
   if (sql)
      delete sql;
#endif
   rollback(xsink);
}
