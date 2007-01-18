/*
  QC_RWLock.cc

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/qore_thread.h>
#include <qore/QC_RWLock.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>

int CID_RWLOCK;

static void RWLOCK_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_RWLOCK, new QoreRWLock());
}

static void RWLOCK_copy(class Object *self, class Object *old, class QoreRWLock *rwl, ExceptionSink *xsink)
{
   self->setPrivate(CID_RWLOCK, new QoreRWLock());
}

static class QoreNode *RWLOCK_readLock(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   rwl->readLock();
   return NULL;
}

static class QoreNode *RWLOCK_readUnlock(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   rwl->readUnlock();
   return NULL;
}

static class QoreNode *RWLOCK_writeLock(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   rwl->writeLock();
   return NULL;
}

static class QoreNode *RWLOCK_writeUnlock(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   rwl->writeUnlock();
   return NULL;
}

static class QoreNode *RWLOCK_tryReadLock(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)rwl->tryReadLock());
}

static class QoreNode *RWLOCK_tryWriteLock(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)rwl->tryWriteLock());
}

static class QoreNode *RWLOCK_numReaders(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)rwl->numReaders());
}

class QoreClass *initRWLockClass()
{
   tracein("initRWLockClass()");

   class QoreClass *QC_RWLOCK = new QoreClass(QDOM_THREAD_CLASS, strdup("RWLock"));
   CID_RWLOCK = QC_RWLOCK->getID();
   QC_RWLOCK->setConstructor(RWLOCK_constructor);
   QC_RWLOCK->setCopy((q_copy_t)RWLOCK_copy);
   QC_RWLOCK->addMethod("readLock",      (q_method_t)RWLOCK_readLock);
   QC_RWLOCK->addMethod("writeLock",     (q_method_t)RWLOCK_writeLock);
   QC_RWLOCK->addMethod("readUnlock",    (q_method_t)RWLOCK_readUnlock);
   QC_RWLOCK->addMethod("writeUnlock",   (q_method_t)RWLOCK_writeUnlock);
   QC_RWLOCK->addMethod("tryReadLock",   (q_method_t)RWLOCK_tryReadLock);
   QC_RWLOCK->addMethod("tryWriteLock",  (q_method_t)RWLOCK_tryWriteLock);
   QC_RWLOCK->addMethod("numReaders",    (q_method_t)RWLOCK_numReaders);

   traceout("initRWLockClass()");
   return QC_RWLOCK;
}
