/*
  QC_RMutex.cc

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
#include <qore/QC_RMutex.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>

int CID_RMUTEX;

static void getQoreRMutex(void *obj)
{
   ((QoreRMutex *)obj)->ROreference();
}

static void releaseQoreRMutex(void *obj)
{
   ((QoreRMutex *)obj)->deref();
}

static void RMUTEX_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_RMUTEX, new QoreRMutex(), getQoreRMutex, releaseQoreRMutex);
}

static void RMUTEX_destructor(class Object *self, class QoreRMutex *m, ExceptionSink *xsink)
{
   m->deref();
}

static void RMUTEX_copy(class Object *self, class Object *old, class QoreRMutex *m, ExceptionSink *xsink)
{
   self->setPrivate(CID_RMUTEX, new QoreRMutex(), getQoreRMutex, releaseQoreRMutex);
}

class QoreNode *RMUTEX_enter(class Object *self, class QoreRMutex *m, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)m->enter());
}

class QoreNode *RMUTEX_tryEnter(class Object *self, class QoreRMutex *m, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)m->tryEnter()); 
}

class QoreNode *RMUTEX_exit(class Object *self, class QoreRMutex *m, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)m->exit());
}

class QoreClass *initRMutexClass()
{
   tracein("initRMutexClass()");

   class QoreClass *QC_RMUTEX = new QoreClass(strdup("RMutex"));
   CID_RMUTEX = QC_RMUTEX->getID();
   QC_RMUTEX->setConstructor(RMUTEX_constructor);
   QC_RMUTEX->setDestructor((q_destructor_t)RMUTEX_destructor);
   QC_RMUTEX->setCopy((q_copy_t)RMUTEX_copy);
   QC_RMUTEX->addMethod("enter",         (q_method_t)RMUTEX_enter);
   QC_RMUTEX->addMethod("tryEnter",      (q_method_t)RMUTEX_tryEnter);
   QC_RMUTEX->addMethod("exit",          (q_method_t)RMUTEX_exit);

   traceout("initRMutexClass()");
   return QC_RMUTEX;
}
