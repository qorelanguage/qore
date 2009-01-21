/*
  QC_RWLock.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols
  
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
#include <qore/intern/QC_RWLock.h>

qore_classid_t CID_RWLOCK;

static void RWLOCK_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_RWLOCK, new RWLock());
}

static void RWLOCK_destructor(QoreObject *self, class RWLock *rwl, ExceptionSink *xsink)
{
   rwl->destructor(xsink);
   rwl->deref(xsink);
}

static void RWLOCK_copy(QoreObject *self, QoreObject *old, class RWLock *rwl, ExceptionSink *xsink)
{
   self->setPrivate(CID_RWLOCK, new RWLock());
}

static AbstractQoreNode *RWLOCK_readLock(QoreObject *self, class RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);

   int rc;
   if (!is_nothing(p))
   {
      int timeout_ms = getMsZeroInt(p);
      rc = rwl->readLock(xsink, timeout_ms);
   }
   else
      rc = rwl->readLock(xsink);

   if (*xsink)
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *RWLOCK_readUnlock(QoreObject *self, class RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink)
{
   rwl->readUnlock(xsink);
   return 0;
}

static AbstractQoreNode *RWLOCK_writeLock(QoreObject *self, class RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);

   int rc;
   if (!is_nothing(p))
   {
      int timeout_ms = getMsZeroInt(p);
      rc = rwl->grab(xsink, timeout_ms);
   }
   else
      rc = rwl->grab(xsink);

   if (*xsink)
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *RWLOCK_writeUnlock(QoreObject *self, class RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink)
{
   rwl->release(xsink);
   return 0;
}

static AbstractQoreNode *RWLOCK_tryReadLock(QoreObject *self, class RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(rwl->tryReadLock());
}

static AbstractQoreNode *RWLOCK_tryWriteLock(QoreObject *self, class RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(rwl->tryGrab());
}

static AbstractQoreNode *RWLOCK_numReaders(QoreObject *self, class RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(rwl->numReaders());
}

static AbstractQoreNode *RWLOCK_getReadWaiting(QoreObject *self, class RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(rwl->getReadWaiting());
}

static AbstractQoreNode *RWLOCK_getWriteWaiting(QoreObject *self, class RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(rwl->getWriteWaiting());
}

class QoreClass *initRWLockClass()
{
   QORE_TRACE("initRWLockClass()");

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


   return QC_RWLOCK;
}
