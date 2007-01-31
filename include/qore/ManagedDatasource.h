/*
 ManagedDatasource.h
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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
FIXME: commit()s when autocommit=true should be made here, also after
 select()s (in case of a select for update, for example)
 
 FIXME: when raising an timeout exception there is a race condition
 getting the TID of the thread holding the lock, because the lock 
 could have been released after the ::enter() call fails... but it's
 only cosmetic (for the exception text)
 */

#ifndef _QORE_MANAGEDDATASOURCE_H

#define _QORE_MANAGEDDATASOURCE_H

#ifdef _QORE_LIB_INTERN

#include <qore/common.h>
#include <qore/LockedObject.h>
#include <qore/QoreCondition.h>
#include <qore/SingleExitGate.h>
#include <qore/Datasource.h>
#include <qore/AbstractPrivateData.h>

#define DEFAULT_TL_TIMEOUT 120

DLLLOCAL void datasource_thread_lock_cleanup(void *ptr, class ExceptionSink *xsink);

class ManagedDatasource : public AbstractPrivateData, public Datasource, private LockedObject
{
private:
   class SingleExitGate tGate;
   int counter;
   int tl_timeout;
   class QoreCondition cStatus;
   
   DLLLOCAL int startDBAction(class ExceptionSink *xsink);
   DLLLOCAL void endDBAction();
   DLLLOCAL int closeUnlocked();   
   // returns 0 for OK, -1 for error
   DLLLOCAL int grabLockIntern(class ExceptionSink *xsink);
   DLLLOCAL void releaseLockIntern();
   // returns 0 for OK, -1 for error
   DLLLOCAL int grabLock(class ExceptionSink *xsink);
   DLLLOCAL void releaseLock();
   
protected:
   DLLLOCAL virtual ~ManagedDatasource();
   
public:
   DLLLOCAL ManagedDatasource(DBIDriver *);
   DLLLOCAL virtual void deref(class ExceptionSink *xsink);
   DLLLOCAL virtual void deref();
   DLLLOCAL class QoreNode *select(class QoreString *query_str, class List *args, ExceptionSink *xsink);
   DLLLOCAL class QoreNode *selectRows(class QoreString *query_str, class List *args, ExceptionSink *xsink);
   DLLLOCAL class QoreNode *exec(class QoreString *query_str, class List *args, ExceptionSink *xsink);
   //DLLLOCAL class Hash *describe(char *table_name, ExceptionSink *xsink);
   DLLLOCAL int commit(ExceptionSink *xsink);
   DLLLOCAL int rollback(ExceptionSink *xsink);
   DLLLOCAL int open(ExceptionSink *xsink);
   DLLLOCAL int close();
   DLLLOCAL void reset(ExceptionSink *xsink);
   DLLLOCAL void setPendingUsername(char *u);
   DLLLOCAL void setPendingPassword(char *p);
   DLLLOCAL void setPendingDBName(char *d);
   DLLLOCAL void setPendingDBEncoding(char *c);
   DLLLOCAL void setPendingHostName(char *h);
   DLLLOCAL QoreNode *getPendingUsername();
   DLLLOCAL QoreNode *getPendingPassword();
   DLLLOCAL QoreNode *getPendingDBName();
   DLLLOCAL QoreNode *getPendingDBEncoding();
   DLLLOCAL QoreNode *getPendingHostName();
   DLLLOCAL void setTransactionLockTimeout(int t);
   DLLLOCAL int getTransactionLockTimeout();
   DLLLOCAL void beginTransaction(class ExceptionSink *xsink);
   DLLLOCAL void setAutoCommit(bool ac);   
   DLLLOCAL ManagedDatasource *copy();
};

#endif

#endif // _QORE_SQL_OBJECTS_DATASOURCE_H
