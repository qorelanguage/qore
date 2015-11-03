/*
 DatasourcePool.h
 
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

#ifndef _QORUS_DATASOURCE_POOL_H

#define _QORUS_DATASOURCE_POOL_H

#include <qore/Datasource.h>
#include <qore/QoreThreadLock.h>
#include <qore/QoreCondition.h>
#include <qore/QoreString.h>
#include <qore/AbstractThreadResource.h>

#include <map>
#include <deque>

typedef std::map<int, int> thread_use_t;   // for marking a datasource in use
typedef std::deque<int> free_list_t;       // for the free list

class DatasourcePool : public AbstractThreadResource, public QoreCondition, public QoreThreadLock {
   private:
      Datasource **pool;
      int *tid_list;            // list of thread IDs per pool index
      thread_use_t tmap;        // map from tids to pool index
      free_list_t free_list;
      int min, 
	 max,
	 cmax,			 // current max
	 wait_count;
      bool valid;

#ifdef DEBUG
      QoreThreadLocalStorage<QoreString> thread_local_storage;
      void addSQL(const char *cmd, const QoreString *sql);
      void resetSQL();
#endif

      DLLLOCAL Datasource *getDSIntern(bool &new_ds, ExceptionSink *xsink);
      DLLLOCAL Datasource *getDS(bool &new_ds, ExceptionSink *xsink);
      DLLLOCAL void freeDS();
      DLLLOCAL void init(DBIDriver *ndsl, const char *user, const char *pass, const char *db, const char *charset, const char *hostname, int mn, int mx, int port, ExceptionSink *xsink);
      
   public:
#ifdef DEBUG
      QoreString *getAndResetSQL();
#endif

      // min must be 1 or more, max must be greater than min
      DLLLOCAL DatasourcePool(DBIDriver *ndsl, const char *user, const char *pass, const char *db, const char *charset, const char *hostname, int mn, int mx, ExceptionSink *xsink);
      DLLLOCAL DatasourcePool(DBIDriver *ndsl, const char *user, const char *pass, const char *db, const char *charset, const char *hostname, int mn, int mx, int port, ExceptionSink *xsink);
      DLLLOCAL virtual ~DatasourcePool();
      DLLLOCAL void destructor(ExceptionSink *xsink);
      DLLLOCAL virtual void cleanup(ExceptionSink *xsink);
      DLLLOCAL AbstractQoreNode *select(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
      DLLLOCAL AbstractQoreNode *selectRow(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
      DLLLOCAL AbstractQoreNode *selectRows(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
      DLLLOCAL int beginTransaction(ExceptionSink *xsink);
      DLLLOCAL AbstractQoreNode *exec(const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
      DLLLOCAL int commit(ExceptionSink *xsink);
      DLLLOCAL int rollback(ExceptionSink *xsink);
      DLLLOCAL QoreStringNode *toString();
      DLLLOCAL int getMin() const;
      DLLLOCAL int getMax() const;
      DLLLOCAL QoreStringNode *getPendingUsername() const;
      DLLLOCAL QoreStringNode *getPendingPassword() const;
      DLLLOCAL QoreStringNode *getPendingDBName() const;
      DLLLOCAL QoreStringNode *getPendingDBEncoding() const;
      DLLLOCAL QoreStringNode *getPendingHostName() const;
      DLLLOCAL int getPendingPort() const;
      DLLLOCAL const QoreEncoding *getQoreEncoding() const;
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
};

#endif
