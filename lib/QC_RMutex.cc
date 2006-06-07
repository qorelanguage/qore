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

static inline void *getQoreRMutex(void *obj)
{
   ((QoreRMutex *)obj)->ROreference();
   return obj;
}

class QoreNode *RMUTEX_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_RMUTEX, new QoreRMutex(), getQoreRMutex);
   return NULL;
}

class QoreNode *RMUTEX_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreRMutex *m = (QoreRMutex *)self->getAndClearPrivateData(CID_RMUTEX);
   if (m)
      m->deref();
   return NULL;
}

class QoreNode *RMUTEX_enter(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreRMutex *m = (QoreRMutex *)self->getReferencedPrivateData(CID_RMUTEX);
   QoreNode *rv;
   if (m)
   {
      rv = new QoreNode((int64)m->enter());
      m->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "RMutex::enter");
   }
   return rv;
}

class QoreNode *RMUTEX_tryEnter(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreRMutex *m = (QoreRMutex *)self->getReferencedPrivateData(CID_RMUTEX);
   QoreNode *rv;
   if (m)
   {
      rv = new QoreNode((int64)m->tryEnter()); 
      m->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "RMutex::tryEnter");
   }
   return rv;
}

class QoreNode *RMUTEX_exit(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreRMutex *m = (QoreRMutex *)self->getReferencedPrivateData(CID_RMUTEX);
   QoreNode *rv;
   if (m)
   {
      rv = new QoreNode((int64)m->exit());
      m->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "RMutex::exit");
   }
   return rv;
}

class QoreClass *initRMutexClass()
{
   tracein("initRMutexClass()");

   class QoreClass *QC_RMUTEX = new QoreClass(strdup("RMutex"));
   CID_RMUTEX = QC_RMUTEX->getID();
   QC_RMUTEX->addMethod("constructor",   RMUTEX_constructor);
   QC_RMUTEX->addMethod("destructor",    RMUTEX_destructor);
   QC_RMUTEX->addMethod("copy",          RMUTEX_constructor);
   QC_RMUTEX->addMethod("enter",         RMUTEX_enter);
   QC_RMUTEX->addMethod("tryEnter",      RMUTEX_tryEnter);
   QC_RMUTEX->addMethod("exit",          RMUTEX_exit);

   traceout("initRMutexClass()");
   return QC_RMUTEX;
}
