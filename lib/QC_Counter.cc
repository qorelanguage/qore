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

#include <qore/Qore.h>
#include <qore/QC_Counter.h>

int CID_COUNTER;

static void COUNTER_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   int c = p ? p->getAsInt() : 0;
   self->setPrivate(CID_COUNTER, new Counter(c));
}

static void COUNTER_copy(class Object *self, class Object *old, class Counter *c, ExceptionSink *xsink)
{
   self->setPrivate(CID_COUNTER, new Counter(c->getCount()));
}

static class QoreNode *COUNTER_inc(class Object *self, class Counter *c, class QoreNode *params, ExceptionSink *xsink)
{
   c->inc();
   return NULL;
}

static class QoreNode *COUNTER_dec(class Object *self, class Counter *c, class QoreNode *params, ExceptionSink *xsink)
{
   c->dec();
   return NULL;
}

static class QoreNode *COUNTER_waitForZero(class Object *self, class Counter *c, class QoreNode *params, ExceptionSink *xsink)
{
   c->waitForZero();
   return NULL;
}

static class QoreNode *COUNTER_getCount(class Object *self, class Counter *c, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)c->getCount());
}

class QoreClass *initCounterClass()
{
   tracein("initCounterClass()");

   class QoreClass *QC_COUNTER = new QoreClass("Counter", QDOM_THREAD_CLASS);
   CID_COUNTER = QC_COUNTER->getID();

   QC_COUNTER->setConstructor(COUNTER_constructor);
   QC_COUNTER->setCopy((q_copy_t)COUNTER_copy);
   QC_COUNTER->addMethod("inc",           (q_method_t)COUNTER_inc);
   QC_COUNTER->addMethod("dec",           (q_method_t)COUNTER_dec);
   QC_COUNTER->addMethod("waitForZero",   (q_method_t)COUNTER_waitForZero);
   QC_COUNTER->addMethod("getCount",      (q_method_t)COUNTER_getCount);

   traceout("initCounterClass()");
   return QC_COUNTER;
}
