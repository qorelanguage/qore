/*
  QC_Counter.cc

  Qore Programming Language
  
  Copyright (C) 2005 David Nichols
  
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
#include <qore/QC_Counter.h>
#include <qore/params.h>
#include <qore/support.h>

int CID_COUNTER;

static inline void *getCounter(void *obj)
{
   ((Counter *)obj)->ROreference();
   return obj;
}

class QoreNode *COUNTER_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_COUNTER, new Counter(), getCounter);
   return NULL;
}

class QoreNode *COUNTER_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Counter *c = (Counter *)self->getAndClearPrivateData(CID_COUNTER);
   if (c)
      c->deref();
   return NULL;
}

class QoreNode *COUNTER_inc(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Counter *c = (Counter *)self->getReferencedPrivateData(CID_COUNTER);
   if (c)
   {
      c->inc();
      c->deref();
   }
   else
      alreadyDeleted(xsink, "Counter::inc");
   return NULL;
}

class QoreNode *COUNTER_dec(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Counter *c = (Counter *)self->getReferencedPrivateData(CID_COUNTER);
   if (c)
   {
      c->dec();
      c->deref();
   }
   else
      alreadyDeleted(xsink, "Counter::dec");
   return NULL;
}

class QoreNode *COUNTER_waitForZero(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Counter *c = (Counter *)self->getReferencedPrivateData(CID_COUNTER);
   if (c)
   {
      c->waitForZero();
      c->deref();
   }
   else
      alreadyDeleted(xsink, "Counter::waitForZero");
   return NULL;
}

class QoreNode *COUNTER_getCount(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Counter *c = (Counter *)self->getReferencedPrivateData(CID_COUNTER);
   QoreNode *rv;
   if (c)
   {
      rv = new QoreNode(NT_INT, c->getCount());
      c->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Counter::getCount");
      rv = NULL;
   }
   return rv;
}

class QoreClass *initCounterClass()
{
   tracein("initCounterClass()");

   class QoreClass *QC_COUNTER = new QoreClass(strdup("Counter"));
   CID_COUNTER = QC_COUNTER->getID();

   QC_COUNTER->addMethod("constructor",   COUNTER_constructor);
   QC_COUNTER->addMethod("destructor",    COUNTER_destructor);
   QC_COUNTER->addMethod("copy",          COUNTER_constructor);
   QC_COUNTER->addMethod("inc",           COUNTER_inc);
   QC_COUNTER->addMethod("dec",           COUNTER_dec);
   QC_COUNTER->addMethod("waitForZero",   COUNTER_waitForZero);
   QC_COUNTER->addMethod("getCount",      COUNTER_getCount);

   traceout("initCounterClass()");
   return QC_COUNTER;
}
