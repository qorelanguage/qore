/*
  QC_Counter.cc

  Qore Programming Language
  
  Copyright 2003 - 2009 David Nichols
  
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
#include <qore/intern/QC_Counter.h>

qore_classid_t CID_COUNTER;

static void COUNTER_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int c = p ? p->getAsInt() : 0;
   self->setPrivate(CID_COUNTER, new Counter(c));
}

static void COUNTER_destructor(QoreObject *self, class Counter *c, ExceptionSink *xsink)
{
   c->destructor(xsink);
   c->deref(xsink);
}

static void COUNTER_copy(QoreObject *self, QoreObject *old, class Counter *c, ExceptionSink *xsink)
{
   self->setPrivate(CID_COUNTER, new Counter(c->getCount()));
}

static AbstractQoreNode *COUNTER_inc(QoreObject *self, class Counter *c, const QoreListNode *params, ExceptionSink *xsink)
{
   c->inc();
   return 0;
}

static AbstractQoreNode *COUNTER_dec(QoreObject *self, class Counter *c, const QoreListNode *params, ExceptionSink *xsink)
{
   c->dec(xsink);
   return 0;
}

static AbstractQoreNode *COUNTER_waitForZero(QoreObject *self, class Counter *c, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   // we only return a return value if we have a timeout, otherwise we save allocating a QoreNode
   int timeout_ms = 0;
   if (!is_nothing(p))
      timeout_ms = getMsZeroInt(p);

   int rc = c->waitForZero(xsink, timeout_ms);
   if (!*xsink)
      return new QoreBigIntNode(rc);
   return 0;
}

static AbstractQoreNode *COUNTER_getCount(QoreObject *self, class Counter *c, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(c->getCount());
}

static AbstractQoreNode *COUNTER_getWaiting(QoreObject *self, class Counter *c, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(c->getWaiting());
}

class QoreClass *initCounterClass()
{
   QORE_TRACE("initCounterClass()");

   class QoreClass *QC_COUNTER = new QoreClass("Counter", QDOM_THREAD_CLASS);
   CID_COUNTER = QC_COUNTER->getID();

   QC_COUNTER->setConstructor(COUNTER_constructor);
   QC_COUNTER->setDestructor((q_destructor_t)COUNTER_destructor);
   QC_COUNTER->setCopy((q_copy_t)COUNTER_copy);
   QC_COUNTER->addMethod("inc",           (q_method_t)COUNTER_inc);
   QC_COUNTER->addMethod("dec",           (q_method_t)COUNTER_dec);
   QC_COUNTER->addMethod("waitForZero",   (q_method_t)COUNTER_waitForZero);
   QC_COUNTER->addMethod("getCount",      (q_method_t)COUNTER_getCount);
   QC_COUNTER->addMethod("getWaiting",    (q_method_t)COUNTER_getWaiting);


   return QC_COUNTER;
}
