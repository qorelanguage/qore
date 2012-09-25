/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 DatasourcePool.h
 
 Qore Programming Language
 
 Copyright 2003 - 2012 David Nichols
 
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
      wait_count;
   bool valid;

#ifdef DEBUG
   QoreThreadLocalStorage<QoreString> thread_local_storage;
   void addSQL(const char *cmd, const QoreString *sql);
   void resetSQL();
#endif

   DLLLOCAL Datasource *getAllocatedDS();
   DLLLOCAL Datasource *getDSIntern(bool &new_ds, ExceptionSink *xsink);
   DLLLOCAL Datasource *getDS(bool &new_ds, ExceptionSink *xsink);
   DLLLOCAL void freeDS();
   // share the code for exec() and execRaw()
   DLLLOCAL AbstractQoreNode *exec_internal(bool doBind, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
      
public:
#ifdef DEBUG
   QoreString *getAndResetSQL();
#endif

   // min must be 1 or more, max must be greater than min
   DLLLOCAL DatasourcePool(ExceptionSink *xsink, DBIDriver *ndsl, const char *user, const char *pass, const char *db, const char *charset, const char *hostname, unsigned mn, unsigned mx, int port = 0, const QoreHashNode* opts = 0);
   DLLLOCAL virtual ~DatasourcePool();
   DLLLOCAL void destructor(ExceptionSink *xsink);
   DLLLOCAL virtual void cleanup(ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode *select(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
   DLLLOCAL QoreHashNode *selectRow(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode *selectRows(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
   DLLLOCAL int beginTransaction(ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode *exec(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode *execRaw(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
   DLLLOCAL int commit(ExceptionSink *xsink);
   DLLLOCAL int rollback(ExceptionSink *xsink);
   DLLLOCAL QoreStringNode *toString();
   DLLLOCAL unsigned getMin() const;
   DLLLOCAL unsigned getMax() const;
   DLLLOCAL QoreStringNode *getPendingUsername() const;
   DLLLOCAL QoreStringNode *getPendingPassword() const;
   DLLLOCAL QoreStringNode *getPendingDBName() const;
   DLLLOCAL QoreStringNode *getPendingDBEncoding() const;
   DLLLOCAL QoreStringNode *getPendingHostName() const;
   DLLLOCAL int getPendingPort() const;
   DLLLOCAL const QoreEncoding *getQoreEncoding() const;
   DLLLOCAL const DBIDriver *getDriver () const {
      return pool[0]->getDriver();
   }
   DLLLOCAL const char *getDriverName () const {
      return pool[0]->getDriverName();
   }
   DLLLOCAL AbstractQoreNode *getServerVersion(ExceptionSink *xsink) {
      return pool[0]->getServerVersion(xsink);
   }
   DLLLOCAL AbstractQoreNode *getClientVersion(ExceptionSink *xsink) {
      return pool[0]->getClientVersion(xsink);
   }
   DLLLOCAL bool inTransaction();

   // functions supporting DatasourceStatementHelper
   DLLLOCAL DatasourceStatementHelper *getReferencedHelper(QoreSQLStatement *s) {
      ref();
      return this;
   }

   // implementing DatasourceStatementHelper virtual functions
   DLLLOCAL virtual void helperDestructor(QoreSQLStatement *s, ExceptionSink *xsink) {
      deref(xsink);
   }

   DLLLOCAL virtual Datasource *helperStartAction(ExceptionSink *xsink, bool &new_transaction) {
      return getDS(new_transaction, xsink);
   }

   DLLLOCAL virtual Datasource *helperEndAction(char orig_cmd, char &cmd, bool new_transaction, ExceptionSink *xsink) {
      //printd(0, "DatasourcePool::helperEndAction() cmd=%d, nt=%d\n", cmd, new_transaction);
      if (cmd == DAH_RELEASE) {
         if (orig_cmd != DAH_NONE) {
            //|| (new_transaction && cmd == DAH_NONE)) {
            //printd(0, "DatasourcePool::helperEndAction() returning 0\n");

            freeDS();
            return 0;
         }
         cmd = DAH_NONE;
      }

      return getAllocatedDS();
   }
   
   //DLLLOCAL virtual void helperAssignDatasource(ExceptionSink *xsink) { return 0; }
   //DLLLOCAL virtual void helperReleaseDatasource() { }
};

class DatasourcePoolActionHelper {
protected:
   DatasourcePool &dsp;
   ExceptionSink *xsink;
   Datasource *ds;
   bool new_ds;
   char cmd;

public:
   DLLLOCAL DatasourcePoolActionHelper(DatasourcePool &n_dsp, ExceptionSink *n_xsink, char n_cmd = DAH_NONE) : dsp(n_dsp), xsink(n_xsink), new_ds(false), cmd(n_cmd) {
      ds = dsp.getDS(new_ds, xsink);
   }
   DLLLOCAL ~DatasourcePoolActionHelper() {
      if (!ds)
	 return;

      if (cmd == DAH_RELEASE 
          || ds->wasConnectionAborted()
          || (new_ds && ((cmd == DAH_NONE) || *xsink)))
	 dsp.freeDS();
   }

#if 0
   DLLLOCAL void addSQL(const QoreString *sql) {
      if (ds && !((cmd == DAH_RELEASE) || (new_ds && ((cmd == DAH_NONE) || *xsink)) || ds->wasConnectionAborted()))
         dsp.addSQL(cmd == DAH_NONE ? "select" : "exec", sql);
   }
#endif

   DLLLOCAL operator bool() const { return ds; }

   DLLLOCAL Datasource *operator*() const { return ds; }
   DLLLOCAL Datasource *operator->() const { return ds; }
};

#endif
