/*
  QC_Mutex.cc

  Qore Programming Language
  
  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/qore_thread.h>
#include <qore/QC_Mutex.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>

int CID_MUTEX;

static inline void getMutex(void *obj)
{
   ((Mutex *)obj)->ROreference();
}

static inline void releaseMutex(void *obj)
{
   ((Mutex *)obj)->deref();
}

static void MUTEX_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_MUTEX, new Mutex(), getMutex, releaseMutex);
}

static void MUTEX_destructor(class Object *self, class Mutex *m, ExceptionSink *xsink)
{
   m->deref();
}

static void MUTEX_copy(class Object *self, class Object *old, class Mutex *m, ExceptionSink *xsink)
{
   self->setPrivate(CID_MUTEX, new Mutex(), getMutex, releaseMutex);
}

class QoreNode *MUTEX_lock(class Object *self, class Mutex *m, class QoreNode *params, ExceptionSink *xsink)
{
   m->lock();
   return NULL;
}

class QoreNode *MUTEX_trylock(class Object *self, class Mutex *m, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)m->trylock()); 
}

class QoreNode *MUTEX_unlock(class Object *self, class Mutex *m, class QoreNode *params, ExceptionSink *xsink)
{
   m->unlock();
   return NULL;
}

class QoreClass *initMutexClass()
{
   tracein("initMutexClass()");

   class QoreClass *QC_MUTEX = new QoreClass(QDOM_THREAD_CLASS, strdup("Mutex"));
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
