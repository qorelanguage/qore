/*
 QC_AutoWriteLock.cc
 
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
#include <qore/intern/QC_AutoWriteLock.h>

int CID_AUTOWRITELOCK;

static void AWL_constructor(class QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   RWLock *rwl = p ? (RWLock *)p->val.object->getReferencedPrivateData(CID_RWLOCK, xsink) : NULL;
   if (xsink->isException())
      return;

   if (!rwl)
   {
      xsink->raiseException("AUTOWRITELOCK-CONSTRUCTOR-ERROR", "expecting RWLock type as argument to constructor");
      return;
   }

   QoreAutoWriteLock *arwl = new QoreAutoWriteLock(rwl, xsink);
   if (*xsink)
      arwl->deref(xsink);
   else
      self->setPrivate(CID_AUTOWRITELOCK, arwl);
}

static void AWL_destructor(class QoreObject *self, class QoreAutoWriteLock *arwl, ExceptionSink *xsink)
{
   arwl->destructor(xsink);
   arwl->deref(xsink);
}

static void AWL_copy(class QoreObject *self, class QoreObject *old, class QoreAutoWriteLock *m, ExceptionSink *xsink)
{
   xsink->raiseException("AUTOWRITELOCK-COPY-ERROR", "objects of this class cannot be copied");
}

class QoreClass *initAutoWriteLockClass()
{
   tracein("initAutoWriteLockClass()");
   
   class QoreClass *QC_AutoWriteLock = new QoreClass("AutoWriteLock", QDOM_THREAD_CLASS);
   CID_AUTOWRITELOCK = QC_AutoWriteLock->getID();
   QC_AutoWriteLock->setConstructor(AWL_constructor);
   QC_AutoWriteLock->setDestructor((q_destructor_t)AWL_destructor);
   QC_AutoWriteLock->setCopy((q_copy_t)AWL_copy);
   
   traceout("initAutoWriteLockClass()");
   return QC_AutoWriteLock;
}
