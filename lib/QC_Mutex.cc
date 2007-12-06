/*
  QC_Mutex.cc

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
#include <qore/intern/QC_Mutex.h>
#include <qore/intern/QC_Condition.h>

int CID_MUTEX;

static void MUTEX_constructor(class QoreObject *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_MUTEX, new SmartMutex());
}

static void MUTEX_destructor(class QoreObject *self, class SmartMutex *m, ExceptionSink *xsink)
{
   m->destructor(xsink);
   m->deref(xsink);
}

static void MUTEX_copy(class QoreObject *self, class QoreObject *old, class SmartMutex *m, ExceptionSink *xsink)
{
   self->setPrivate(CID_MUTEX, new SmartMutex());
}

static class QoreNode *MUTEX_lock(class QoreObject *self, class SmartMutex *m, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   // we only return a return value if we have a timeout, otherwise we save allocating a QoreNode
   if (!is_nothing(p))
   {
      int timeout_ms = getMsZeroInt(p);
      int rc = m->grab(xsink, timeout_ms);
      if (!*xsink)
	 return new QoreNode((int64)rc);
   }
   else
      m->grab(xsink);
   return NULL;
}

static class QoreNode *MUTEX_trylock(class QoreObject *self, class SmartMutex *m, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)m->tryGrab()); 
}

static class QoreNode *MUTEX_unlock(class QoreObject *self, class SmartMutex *m, class QoreNode *params, ExceptionSink *xsink)
{
   m->release(xsink);
   return NULL;
}

class QoreClass *initMutexClass()
{
   tracein("initMutexClass()");

   class QoreClass *QC_MUTEX = new QoreClass("Mutex", QDOM_THREAD_CLASS);
   CID_MUTEX = QC_MUTEX->getID();
   QC_MUTEX->setConstructor(MUTEX_constructor);
   QC_MUTEX->setDestructor((q_destructor_t)MUTEX_destructor);
   QC_MUTEX->setCopy((q_copy_t)MUTEX_copy);
   QC_MUTEX->addMethod("lock",          (q_method_t)MUTEX_lock);
   QC_MUTEX->addMethod("trylock",       (q_method_t)MUTEX_trylock);
   QC_MUTEX->addMethod("unlock",        (q_method_t)MUTEX_unlock);

   traceout("initMutexClass()");
   return QC_MUTEX;
}
