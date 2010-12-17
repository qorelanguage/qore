/*
  QC_Counter.cpp

  Qore Programming Language
  
  Copyright 2003 - 2010 David Nichols
  
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

static void COUNTER_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   int c = p ? p->getAsInt() : 0;
   self->setPrivate(CID_COUNTER, new Counter(c));
}

static void COUNTER_destructor(QoreObject *self, Counter *c, ExceptionSink *xsink) {
   c->destructor(xsink);
   c->deref(xsink);
}

static void COUNTER_copy(QoreObject *self, QoreObject *old, Counter *c, ExceptionSink *xsink) {
   self->setPrivate(CID_COUNTER, new Counter(c->getCount()));
}

static AbstractQoreNode *COUNTER_inc(QoreObject *self, Counter *c, const QoreListNode *params, ExceptionSink *xsink) {
   c->inc();
   return 0;
}

static AbstractQoreNode *COUNTER_dec(QoreObject *self, Counter *c, const QoreListNode *params, ExceptionSink *xsink) {
   c->dec(xsink);
   return 0;
}

static AbstractQoreNode *COUNTER_waitForZero(QoreObject *self, Counter *c, const QoreListNode *params, ExceptionSink *xsink) {
   c->waitForZero(xsink);
   return 0;
}

// int Counter::waitForZero(timeout $timeout)  
static AbstractQoreNode *COUNTER_waitForZero_timeout(QoreObject *self, Counter *c, const QoreListNode *params, ExceptionSink *xsink) {
   int timeout_ms = (int)HARD_QORE_INT(params, 0);
   int rc = c->waitForZero(xsink, timeout_ms);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *COUNTER_getCount(QoreObject *self, Counter *c, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(c->getCount());
}

static AbstractQoreNode *COUNTER_getWaiting(QoreObject *self, Counter *c, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(c->getWaiting());
}

QoreClass *initCounterClass() {
   QORE_TRACE("initCounterClass()");

   QoreClass *QC_COUNTER = new QoreClass("Counter", QDOM_THREAD_CLASS);
   CID_COUNTER = QC_COUNTER->getID();

   QC_COUNTER->setConstructorExtended(COUNTER_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, softBigIntTypeInfo, zero());
   QC_COUNTER->setDestructor((q_destructor_t)COUNTER_destructor);
   QC_COUNTER->setCopy((q_copy_t)COUNTER_copy);
   QC_COUNTER->addMethodExtended("inc",           (q_method_t)COUNTER_inc, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_COUNTER->addMethodExtended("dec",           (q_method_t)COUNTER_dec, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_COUNTER->addMethodExtended("waitForZero",   (q_method_t)COUNTER_waitForZero, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   // int Counter::waitForZero(timeout $timeout)  
   QC_COUNTER->addMethodExtended("waitForZero",   (q_method_t)COUNTER_waitForZero_timeout, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, QORE_PARAM_NO_ARG);

   QC_COUNTER->addMethodExtended("getCount",      (q_method_t)COUNTER_getCount, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);
   QC_COUNTER->addMethodExtended("getWaiting",    (q_method_t)COUNTER_getWaiting, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   return QC_COUNTER;
}
