/*
 QC_AutoLock.cc
 
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
#include <qore/intern/QC_AutoLock.h>

qore_classid_t CID_AUTOLOCK;

static void AL_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   SmartMutex *m = p ? (SmartMutex *)p->getReferencedPrivateData(CID_MUTEX, xsink) : 0;
   if (*xsink)
      return;

   if (!m)
   {
      xsink->raiseException("AUTOLOCK-CONSTRUCTOR-ERROR", "expecting Mutex type as argument to constructor");
      return;
   }

   QoreAutoLock *qsl = new QoreAutoLock(m, xsink);
   if (*xsink)
      qsl->deref(xsink);
   else
      self->setPrivate(CID_AUTOLOCK, qsl);
}

static void AL_destructor(QoreObject *self, class QoreAutoLock *al, ExceptionSink *xsink)
{
   al->destructor(xsink);
   al->deref(xsink);
}

static void AL_copy(QoreObject *self, QoreObject *old, class QoreAutoLock *m, ExceptionSink *xsink)
{
   xsink->raiseException("AUTOLOCK-COPY-ERROR", "objects of this class cannot be copied");
}

static AbstractQoreNode *AL_lock(QoreObject *self, class QoreAutoLock *m, const QoreListNode *params, ExceptionSink *xsink)
{
   m->lock(xsink);
   return 0;
}

static AbstractQoreNode *AL_trylock(QoreObject *self, class QoreAutoLock *m, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(m->trylock()); 
}

static AbstractQoreNode *AL_unlock(QoreObject *self, class QoreAutoLock *m, const QoreListNode *params, ExceptionSink *xsink)
{
   m->unlock(xsink);
   return 0;
}

class QoreClass *initAutoLockClass()
{
   QORE_TRACE("initAutoLockClass()");
   
   class QoreClass *QC_AutoLock = new QoreClass("AutoLock", QDOM_THREAD_CLASS);
   CID_AUTOLOCK = QC_AutoLock->getID();
   QC_AutoLock->setConstructor(AL_constructor);
   QC_AutoLock->setDestructor((q_destructor_t)AL_destructor);
   QC_AutoLock->setCopy((q_copy_t)AL_copy);
   QC_AutoLock->addMethod("lock",          (q_method_t)AL_lock);
   QC_AutoLock->addMethod("trylock",       (q_method_t)AL_trylock);
   QC_AutoLock->addMethod("unlock",        (q_method_t)AL_unlock);
   

   return QC_AutoLock;
}
