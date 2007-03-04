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
#include <qore/QC_Mutex.h>

int CID_MUTEX;

static void MUTEX_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_MUTEX, new Mutex());
}

static void MUTEX_copy(class Object *self, class Object *old, class Mutex *m, ExceptionSink *xsink)
{
   self->setPrivate(CID_MUTEX, new Mutex());
}

static class QoreNode *MUTEX_lock(class Object *self, class Mutex *m, class QoreNode *params, ExceptionSink *xsink)
{
   m->lock(xsink);
   return NULL;
}

static class QoreNode *MUTEX_trylock(class Object *self, class Mutex *m, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)m->trylock()); 
}

static class QoreNode *MUTEX_unlock(class Object *self, class Mutex *m, class QoreNode *params, ExceptionSink *xsink)
{
   m->unlock(xsink);
   return NULL;
}

class QoreClass *initMutexClass()
{
   tracein("initMutexClass()");

   class QoreClass *QC_MUTEX = new QoreClass(QDOM_THREAD_CLASS, strdup("Mutex"));
   CID_MUTEX = QC_MUTEX->getID();
   QC_MUTEX->setConstructor(MUTEX_constructor);
   QC_MUTEX->setCopy((q_copy_t)MUTEX_copy);
   QC_MUTEX->addMethod("lock",          (q_method_t)MUTEX_lock);
   QC_MUTEX->addMethod("trylock",       (q_method_t)MUTEX_trylock);
   QC_MUTEX->addMethod("unlock",        (q_method_t)MUTEX_unlock);

   traceout("initMutexClass()");
   return QC_MUTEX;
}
