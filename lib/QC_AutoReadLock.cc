/*
 QC_AutoReadLock.cc
 
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
#include <qore/intern/QC_AutoReadLock.h>

qore_classid_t CID_AUTOREADLOCK;

static void ARL_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   RWLock *rwl = p ? (RWLock *)p->getReferencedPrivateData(CID_RWLOCK, xsink) : 0;
   if (*xsink)
      return;

   if (!rwl)
   {
      xsink->raiseException("AUTOREADLOCK-CONSTRUCTOR-ERROR", "expecting RWLock type as argument to constructor");
      return;
   }

   QoreAutoReadLock *arwl = new QoreAutoReadLock(rwl, xsink);
   if (*xsink)
      arwl->deref(xsink);
   else
      self->setPrivate(CID_AUTOREADLOCK, arwl);
}

static void ARL_destructor(QoreObject *self, class QoreAutoReadLock *arwl, ExceptionSink *xsink)
{
   arwl->destructor(xsink);
   arwl->deref(xsink);
}

static void ARL_copy(QoreObject *self, QoreObject *old, class QoreAutoReadLock *m, ExceptionSink *xsink)
{
   xsink->raiseException("AUTOREADLOCK-COPY-ERROR", "objects of this class cannot be copied");
}

class QoreClass *initAutoReadLockClass()
{
   QORE_TRACE("initAutoReadLockClass()");
   
   class QoreClass *QC_AutoReadLock = new QoreClass("AutoReadLock", QDOM_THREAD_CLASS);
   CID_AUTOREADLOCK = QC_AutoReadLock->getID();
   QC_AutoReadLock->setConstructor(ARL_constructor);
   QC_AutoReadLock->setDestructor((q_destructor_t)ARL_destructor);
   QC_AutoReadLock->setCopy((q_copy_t)ARL_copy);
   

   return QC_AutoReadLock;
}
