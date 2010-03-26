/*
 QC_AutoWriteLock.cpp
 
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
#include <qore/intern/QC_AutoWriteLock.h>

qore_classid_t CID_AUTOWRITELOCK;

static void AWL_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(rwl, RWLock, params, 0, CID_RWLOCK, "RWLock", "AutoWriteLock::constructor", xsink);
   if (*xsink)
      return;

   assert(rwl);

   ReferenceHolder<QoreAutoWriteLock> arwl(new QoreAutoWriteLock(rwl, xsink), xsink);
   if (!*xsink)
      self->setPrivate(CID_AUTOWRITELOCK, arwl.release());
}

static void AWL_destructor(QoreObject *self, QoreAutoWriteLock *arwl, ExceptionSink *xsink) {
   arwl->destructor(xsink);
   arwl->deref(xsink);
}

static void AWL_copy(QoreObject *self, QoreObject *old, QoreAutoWriteLock *m, ExceptionSink *xsink) {
   xsink->raiseException("AUTOWRITELOCK-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initAutoWriteLockClass(QoreClass *RWLock) {
   QORE_TRACE("initAutoWriteLockClass()");
   
   QoreClass *QC_AutoWriteLock = new QoreClass("AutoWriteLock", QDOM_THREAD_CLASS);
   CID_AUTOWRITELOCK = QC_AutoWriteLock->getID();
   QC_AutoWriteLock->setConstructorExtended(AWL_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, RWLock->getTypeInfo(), QORE_PARAM_NO_ARG);
   QC_AutoWriteLock->setDestructor((q_destructor_t)AWL_destructor);
   QC_AutoWriteLock->setCopy((q_copy_t)AWL_copy);

   return QC_AutoWriteLock;
}
