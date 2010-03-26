/*
 QC_AutoReadLock.cpp
 
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
#include <qore/intern/QC_AutoReadLock.h>

qore_classid_t CID_AUTOREADLOCK;

static void ARL_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_PARAM(rwl, RWLock, params, 0, CID_RWLOCK, xsink);
   if (*xsink)
      return;

   assert(rwl);

   QoreAutoReadLock *arwl = new QoreAutoReadLock(rwl, xsink);
   if (*xsink)
      arwl->deref(xsink);
   else
      self->setPrivate(CID_AUTOREADLOCK, arwl);
}

static void ARL_destructor(QoreObject *self, QoreAutoReadLock *arwl, ExceptionSink *xsink) {
   arwl->destructor(xsink);
   arwl->deref(xsink);
}

static void ARL_copy(QoreObject *self, QoreObject *old, QoreAutoReadLock *m, ExceptionSink *xsink) {
   xsink->raiseException("AUTOREADLOCK-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initAutoReadLockClass(QoreClass *RWLock) {
   QORE_TRACE("initAutoReadLockClass()");
   
   QoreClass *QC_AutoReadLock = new QoreClass("AutoReadLock", QDOM_THREAD_CLASS);
   CID_AUTOREADLOCK = QC_AutoReadLock->getID();
   QC_AutoReadLock->setConstructorExtended(ARL_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, RWLock->getTypeInfo(), QORE_PARAM_NO_ARG);
   QC_AutoReadLock->setDestructor((q_destructor_t)ARL_destructor);
   QC_AutoReadLock->setCopy((q_copy_t)ARL_copy);
   
   return QC_AutoReadLock;
}
