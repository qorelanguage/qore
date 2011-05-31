/*
  QC_RWLock.cpp

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
#include <qore/intern/QC_RWLock.h>

qore_classid_t CID_RWLOCK;

static void RWLOCK_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   self->setPrivate(CID_RWLOCK, new RWLock);
}

static void RWLOCK_destructor(QoreObject *self, RWLock *rwl, ExceptionSink *xsink) {
   rwl->destructor(xsink);
   rwl->deref(xsink);
}

static void RWLOCK_copy(QoreObject *self, QoreObject *old, RWLock *rwl, ExceptionSink *xsink) {
   self->setPrivate(CID_RWLOCK, new RWLock);
}

static AbstractQoreNode *RWLOCK_readLock(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   rwl->readLock(xsink);
   return 0;
}

// int RWLock::readLock(timeout $timeout)  
static AbstractQoreNode *RWLOCK_readLock_timeout(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   int timeout_ms = (int)HARD_QORE_INT(params, 0);
   int rc = rwl->readLock(xsink, timeout_ms);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *RWLOCK_readUnlock(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   rwl->readUnlock(xsink);
   return 0;
}

static AbstractQoreNode *RWLOCK_writeLock(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   rwl->grab(xsink);
   return 0;
}

// int RWLock::writeLock(timeout $timeout)  
static AbstractQoreNode *RWLOCK_writeLock_timeout(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   int timeout_ms = (int)HARD_QORE_INT(params, 0);
   int rc = rwl->grab(xsink, timeout_ms);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *RWLOCK_writeUnlock(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   rwl->release(xsink);
   return 0;
}

static AbstractQoreNode *RWLOCK_tryReadLock(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(rwl->tryReadLock());
}

static AbstractQoreNode *RWLOCK_tryWriteLock(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(rwl->tryGrab());
}

static AbstractQoreNode *RWLOCK_numReaders(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(rwl->numReaders());
}

static AbstractQoreNode *RWLOCK_getReadWaiting(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(rwl->getReadWaiting());
}

static AbstractQoreNode *RWLOCK_getWriteWaiting(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(rwl->getWriteWaiting());
}

// bool RWLock::lockOwner()
static AbstractQoreNode *RWLOCK_lockOwner(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(rwl->lockOwner());
}

// bool RWLock::readLockOwner()
static AbstractQoreNode *RWLOCK_readLockOwner(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(rwl->readLockOwner());
}

// bool RWLock::wrteLockOwner()
static AbstractQoreNode *RWLOCK_writeLockOwner(QoreObject *self, RWLock *rwl, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(rwl->writeLockOwner());
}

QoreClass *initRWLockClass(QoreClass *AbstractSmartLock) {
   QORE_TRACE("initRWLockClass()");

   QoreClass *QC_RWLOCK = new QoreClass("RWLock", QDOM_THREAD_CLASS);
   CID_RWLOCK = QC_RWLOCK->getID();

   QC_RWLOCK->addBuiltinVirtualBaseClass(AbstractSmartLock);

   QC_RWLOCK->setConstructorExtended(RWLOCK_constructor);

   QC_RWLOCK->setDestructor((q_destructor_t)RWLOCK_destructor);

   QC_RWLOCK->setCopy((q_copy_t)RWLOCK_copy);

   QC_RWLOCK->addMethodExtended("readLock",        (q_method_t)RWLOCK_readLock, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // int RWLock::readLock(timeout $timeout)  
   QC_RWLOCK->addMethodExtended("readLock",        (q_method_t)RWLOCK_readLock_timeout, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, QORE_PARAM_NO_ARG);

   QC_RWLOCK->addMethodExtended("readUnlock",      (q_method_t)RWLOCK_readUnlock, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   QC_RWLOCK->addMethodExtended("writeLock",       (q_method_t)RWLOCK_writeLock, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // int RWLock::writeLock(timeout $timeout)  
   QC_RWLOCK->addMethodExtended("writeLock",       (q_method_t)RWLOCK_writeLock_timeout, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, QORE_PARAM_NO_ARG);

   QC_RWLOCK->addMethodExtended("writeUnlock",     (q_method_t)RWLOCK_writeUnlock, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   QC_RWLOCK->addMethodExtended("tryReadLock",     (q_method_t)RWLOCK_tryReadLock, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   QC_RWLOCK->addMethodExtended("tryWriteLock",    (q_method_t)RWLOCK_tryWriteLock, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   QC_RWLOCK->addMethodExtended("numReaders",      (q_method_t)RWLOCK_numReaders, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   QC_RWLOCK->addMethodExtended("getReadWaiting",  (q_method_t)RWLOCK_getReadWaiting, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   QC_RWLOCK->addMethodExtended("getWriteWaiting", (q_method_t)RWLOCK_getWriteWaiting, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   QC_RWLOCK->addMethodExtended("lockOwner",       (q_method_t)RWLOCK_lockOwner, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);
   QC_RWLOCK->addMethodExtended("readLockOwner",   (q_method_t)RWLOCK_readLockOwner, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);
   QC_RWLOCK->addMethodExtended("writeLockOwner",  (q_method_t)RWLOCK_writeLockOwner, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   return QC_RWLOCK;
}
