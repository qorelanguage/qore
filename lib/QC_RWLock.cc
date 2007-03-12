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

#include <qore/Qore.h>
#include <qore/QC_RWLock.h>

int CID_RWLOCK;

static void RWLOCK_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_RWLOCK, new QoreRWLock());
}

static void RWLOCK_destructor(class Object *self, class QoreRWLock *rwl, ExceptionSink *xsink)
{
   rwl->destructor(xsink);
   rwl->deref(xsink);
}

static void RWLOCK_copy(class Object *self, class Object *old, class QoreRWLock *rwl, ExceptionSink *xsink)
{
   self->setPrivate(CID_RWLOCK, new QoreRWLock());
}

static class QoreNode *RWLOCK_readLock(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   int rc;
   if (!is_nothing(p))
   {
      int timeout_ms = getMsZeroInt(p);
      rc = rwl->readLock(xsink, timeout_ms);
   }
   else
      rc = rwl->readLock(xsink);

   if (*xsink)
      return NULL;

   return new QoreNode((int64)rc);
}

static class QoreNode *RWLOCK_readUnlock(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   rwl->readUnlock(xsink);
   return NULL;
}

static class QoreNode *RWLOCK_writeLock(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   int rc;
   if (!is_nothing(p))
   {
      int timeout_ms = getMsZeroInt(p);
      rc = rwl->grab(xsink, timeout_ms);
   }
   else
      rc = rwl->grab(xsink);

   if (*xsink)
      return NULL;

   return new QoreNode((int64)rc);
}

static class QoreNode *RWLOCK_writeUnlock(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   rwl->release(xsink);
   return NULL;
}

static class QoreNode *RWLOCK_tryReadLock(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)rwl->tryReadLock());
}

static class QoreNode *RWLOCK_tryWriteLock(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)rwl->tryGrab());
}

static class QoreNode *RWLOCK_numReaders(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)rwl->numReaders());
}

static class QoreNode *RWLOCK_getReadWaiting(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)rwl->getReadWaiting());
}

static class QoreNode *RWLOCK_getWriteWaiting(class Object *self, class QoreRWLock *rwl, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)rwl->getWriteWaiting());
}

class QoreClass *initRWLockClass()
{
   tracein("initRWLockClass()");

   class QoreClass *QC_RWLOCK = new QoreClass("RWLock", QDOM_THREAD_CLASS);
   CID_RWLOCK = QC_RWLOCK->getID();
   QC_RWLOCK->setConstructor(RWLOCK_constructor);
   QC_RWLOCK->setDestructor((q_destructor_t)RWLOCK_destructor);
   QC_RWLOCK->setCopy((q_copy_t)RWLOCK_copy);
   QC_RWLOCK->addMethod("readLock",        (q_method_t)RWLOCK_readLock);
   QC_RWLOCK->addMethod("writeLock",       (q_method_t)RWLOCK_writeLock);
   QC_RWLOCK->addMethod("readUnlock",      (q_method_t)RWLOCK_readUnlock);
   QC_RWLOCK->addMethod("writeUnlock",     (q_method_t)RWLOCK_writeUnlock);
   QC_RWLOCK->addMethod("tryReadLock",     (q_method_t)RWLOCK_tryReadLock);
   QC_RWLOCK->addMethod("tryWriteLock",    (q_method_t)RWLOCK_tryWriteLock);
   QC_RWLOCK->addMethod("numReaders",      (q_method_t)RWLOCK_numReaders);
   QC_RWLOCK->addMethod("getReadWaiting",  (q_method_t)RWLOCK_getReadWaiting);
   QC_RWLOCK->addMethod("getWriteWaiting", (q_method_t)RWLOCK_getWriteWaiting);

   traceout("initRWLockClass()");
   return QC_RWLOCK;
}
