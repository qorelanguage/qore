/*
 DatasourcePool.h
 
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

#ifndef _QORUS_DATASOURCE_POOL_H

#define _QORUS_DATASOURCE_POOL_H

#include <qore/Datasource.h>
#include <qore/LockedObject.h>
#include <qore/QoreCondition.h>
#include <qore/QoreString.h>
#include <qore/AbstractThreadResource.h>

#include <map>
#include <deque>

typedef std::map<int, int> thread_use_t;   // for marking a datasource in use
typedef std::deque<int> free_list_t;       // for the free list

class DatasourcePool : public AbstractThreadResource, public QoreCondition, public LockedObject
{
   private:
      class Datasource **pool;
      int *tid_list;            // list of thread IDs per pool index
      thread_use_t tmap;        // map from tids to pool index
      free_list_t free_list;
      int min, 
	 max,
	 cmax,			 // current max
	 wait_count;
      bool valid;

#ifdef DEBUG
      pthread_key_t thread_local_storage;
      void addSQL(char *cmd, class QoreString *sql);
      void resetSQL();
#endif

      DLLLOCAL class Datasource *getDS(bool &new_ds, class ExceptionSink *xsink);
      DLLLOCAL void freeDS();
      
   public:
#ifdef DEBUG
      class QoreString *getAndResetSQL();
#endif

      // min must be 1 or more, max must be greater than min
      DLLLOCAL DatasourcePool(DBIDriver *ndsl, const char *user, const char *pass, const char *db, const char *charset, const char *hostname, int mn, int mx, class ExceptionSink *xsink);
      DLLLOCAL virtual ~DatasourcePool();
      DLLLOCAL void destructor(class ExceptionSink *xsink);
      DLLLOCAL virtual void cleanup(class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *select(class QoreString *sql, QoreList *args, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *selectRow(class QoreString *sql, QoreList *args, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *selectRows(class QoreString *sql, QoreList *args, class ExceptionSink *xsink);
      DLLLOCAL int beginTransaction(class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *exec(class QoreString *sql, QoreList *args, class ExceptionSink *xsink);
      DLLLOCAL int commit(class ExceptionSink *xsink);
      DLLLOCAL int rollback(class ExceptionSink *xsink);
      DLLLOCAL class QoreString *toString();
      DLLLOCAL int getMin() const;
      DLLLOCAL int getMax() const;
      DLLLOCAL class QoreNode *getPendingUsername() const;
      DLLLOCAL class QoreNode *getPendingPassword() const;
      DLLLOCAL class QoreNode *getPendingDBName() const;
      DLLLOCAL class QoreNode *getPendingDBEncoding() const;
      DLLLOCAL class QoreNode *getPendingHostName() const;
      DLLLOCAL class QoreEncoding *getQoreEncoding() const;
      DLLLOCAL const char *getDriverName () const
      {
	 return pool[0]->getDriverName();
      }
      DLLLOCAL class QoreNode *getServerVersion(class ExceptionSink *xsink)
      {
	 return pool[0]->getServerVersion(xsink);
      }
      DLLLOCAL class QoreNode *getClientVersion(class ExceptionSink *xsink)
      {
	 return pool[0]->getClientVersion(xsink);
      }
};

#endif
