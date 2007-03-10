/*
 QC_AutoLock.cc
 
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
#include <qore/QC_AutoLock.h>

int CID_AUTOLOCK;

static void AL_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   Mutex *m = p ? (Mutex *)p->val.object->getReferencedPrivateData(CID_MUTEX, xsink) : NULL;
   if (xsink->isException())
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

static void AL_destructor(class Object *self, class QoreAutoLock *al, ExceptionSink *xsink)
{
   al->destructor(xsink);
   al->deref(xsink);
}

static void AL_copy(class Object *self, class Object *old, class QoreAutoLock *m, ExceptionSink *xsink)
{
   xsink->raiseException("AUTOLOCK-COPY-ERROR", "objects of this class cannot be copied");
}

static class QoreNode *AL_lock(class Object *self, class QoreAutoLock *m, class QoreNode *params, ExceptionSink *xsink)
{
   m->lock(xsink);
   return NULL;
}

static class QoreNode *AL_trylock(class Object *self, class QoreAutoLock *m, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)m->trylock()); 
}

static class QoreNode *AL_unlock(class Object *self, class QoreAutoLock *m, class QoreNode *params, ExceptionSink *xsink)
{
   m->unlock(xsink);
   return NULL;
}

class QoreClass *initAutoLockClass()
{
   tracein("initAutoLockClass()");
   
   class QoreClass *QC_AutoLock = new QoreClass("AutoLock", QDOM_THREAD_CLASS);
   CID_AUTOLOCK = QC_AutoLock->getID();
   QC_AutoLock->setConstructor(AL_constructor);
   QC_AutoLock->setDestructor((q_destructor_t)AL_destructor);
   QC_AutoLock->setCopy((q_copy_t)AL_copy);
   QC_AutoLock->addMethod("lock",          (q_method_t)AL_lock);
   QC_AutoLock->addMethod("trylock",       (q_method_t)AL_trylock);
   QC_AutoLock->addMethod("unlock",        (q_method_t)AL_unlock);
   
   traceout("initAutoLockClass()");
   return QC_AutoLock;
}
