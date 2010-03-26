/*
 QC_AutoLock.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2010 David Nichols
 
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
#include <qore/intern/QC_AutoLock.h>

qore_classid_t CID_AUTOLOCK;

static void AL_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_PARAM(m, SmartMutex, params, 0, CID_MUTEX, xsink);
   if (*xsink)
      return;

   assert(m);

   QoreAutoLock *qsl = new QoreAutoLock(m, xsink);
   if (*xsink)
      qsl->deref(xsink);
   else
      self->setPrivate(CID_AUTOLOCK, qsl);
}

static void AL_destructor(QoreObject *self, QoreAutoLock *al, ExceptionSink *xsink) {
   al->destructor(xsink);
   al->deref(xsink);
}

static void AL_copy(QoreObject *self, QoreObject *old, QoreAutoLock *m, ExceptionSink *xsink) {
   xsink->raiseException("AUTOLOCK-COPY-ERROR", "objects of this class cannot be copied");
}

static AbstractQoreNode *AL_lock(QoreObject *self, QoreAutoLock *m, const QoreListNode *params, ExceptionSink *xsink) {
   m->lock(xsink);
   return 0;
}

static AbstractQoreNode *AL_trylock(QoreObject *self, QoreAutoLock *m, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(m->trylock()); 
}

static AbstractQoreNode *AL_unlock(QoreObject *self, QoreAutoLock *m, const QoreListNode *params, ExceptionSink *xsink) {
   m->unlock(xsink);
   return 0;
}

QoreClass *initAutoLockClass(QoreClass *Mutex) {
   QORE_TRACE("initAutoLockClass()");
   
   QoreClass *QC_AutoLock = new QoreClass("AutoLock", QDOM_THREAD_CLASS);
   CID_AUTOLOCK = QC_AutoLock->getID();
   QC_AutoLock->setConstructorExtended(AL_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, Mutex->getTypeInfo(), QORE_PARAM_NO_ARG);
   QC_AutoLock->setDestructor((q_destructor_t)AL_destructor);
   QC_AutoLock->setCopy((q_copy_t)AL_copy);

   QC_AutoLock->addMethodExtended("lock",    (q_method_t)AL_lock, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_AutoLock->addMethodExtended("trylock", (q_method_t)AL_trylock, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);
   QC_AutoLock->addMethodExtended("unlock",  (q_method_t)AL_unlock, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   return QC_AutoLock;
}
