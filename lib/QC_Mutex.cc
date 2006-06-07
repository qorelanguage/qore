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
#include <qore/thread.h>
#include <qore/QC_Mutex.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>

int CID_MUTEX;

static inline void *getMutex(void *obj)
{
   ((Mutex *)obj)->ROreference();
   return obj;
}

class QoreNode *MUTEX_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_MUTEX, new Mutex(), getMutex);
   return NULL;
}

class QoreNode *MUTEX_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Mutex *m = (Mutex *)self->getAndClearPrivateData(CID_MUTEX);
   if (m)
      m->deref();
   return NULL;
}

class QoreNode *MUTEX_lock(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Mutex *m = (Mutex *)self->getReferencedPrivateData(CID_MUTEX);
   if (m)
   {
      m->lock();
      m->deref();
   }
   else
      alreadyDeleted(xsink, "Mutex::lock");
   return NULL;
}

class QoreNode *MUTEX_trylock(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Mutex *m = (Mutex *)self->getReferencedPrivateData(CID_MUTEX);
   QoreNode *rv;
   if (m)
   {
      rv = new QoreNode(NT_INT, m->trylock()); 
      m->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Mutex::trylock");
   }
   return rv;
}

class QoreNode *MUTEX_unlock(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Mutex *m = (Mutex *)self->getReferencedPrivateData(CID_MUTEX);
   if (m)
   {
      m->unlock();
      m->deref();
   }
   else
      alreadyDeleted(xsink, "Mutex::unlock");
   return NULL;
}

class QoreClass *initMutexClass()
{
   tracein("initMutexClass()");

   class QoreClass *QC_MUTEX = new QoreClass(strdup("Mutex"));
   CID_MUTEX = QC_MUTEX->getID();
   QC_MUTEX->addMethod("constructor",   MUTEX_constructor);
   QC_MUTEX->addMethod("destructor",    MUTEX_destructor);
   QC_MUTEX->addMethod("copy",          MUTEX_constructor);
   QC_MUTEX->addMethod("lock",          MUTEX_lock);
   QC_MUTEX->addMethod("trylock",       MUTEX_trylock);
   QC_MUTEX->addMethod("unlock",        MUTEX_unlock);

   traceout("initMutexClass()");
   return QC_MUTEX;
}
