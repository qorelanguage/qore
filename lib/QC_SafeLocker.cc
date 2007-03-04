/*
 QC_SafeLocker.cc
 
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
#include <qore/QC_SafeLocker.h>

int CID_SAFELOCKER;

static void SL_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   Mutex *m = p ? (Mutex *)p->val.object->getReferencedPrivateData(CID_MUTEX, xsink) : NULL;
   if (xsink->isException())
      return;

   if (!m)
   {
      xsink->raiseException("SAFELOCKER-CONSTRUCTOR-ERROR", "expecting Mutex type as argument to constructor");
      return;
   }

   QoreSafeLocker *qsl = new QoreSafeLocker(m, xsink);
   if (*xsink)
      qsl->deref(xsink);
   else
      self->setPrivate(CID_SAFELOCKER, qsl);
}

static void SL_copy(class Object *self, class Object *old, class QoreSafeLocker *m, ExceptionSink *xsink)
{
   xsink->raiseException("SAFELOCKER-COPY-ERROR", "objects of this class cannot be copied");
}

static class QoreNode *SL_lock(class Object *self, class QoreSafeLocker *m, class QoreNode *params, ExceptionSink *xsink)
{
   m->lock(xsink);
   return NULL;
}

static class QoreNode *SL_trylock(class Object *self, class QoreSafeLocker *m, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)m->trylock()); 
}

static class QoreNode *SL_unlock(class Object *self, class QoreSafeLocker *m, class QoreNode *params, ExceptionSink *xsink)
{
   m->unlock(xsink);
   return NULL;
}

class QoreClass *initSafeLockerClass()
{
   tracein("initSafeLockerClass()");
   
   class QoreClass *QC_SafeLocker = new QoreClass(QDOM_THREAD_CLASS, strdup("SafeLocker"));
   CID_SAFELOCKER = QC_SafeLocker->getID();
   QC_SafeLocker->setConstructor(SL_constructor);
   QC_SafeLocker->setCopy((q_copy_t)SL_copy);
   QC_SafeLocker->addMethod("lock",          (q_method_t)SL_lock);
   QC_SafeLocker->addMethod("trylock",       (q_method_t)SL_trylock);
   QC_SafeLocker->addMethod("unlock",        (q_method_t)SL_unlock);
   
   traceout("initSafeLockerClass()");
   return QC_SafeLocker;
}
