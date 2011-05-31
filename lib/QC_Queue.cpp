/*
  QC_Queue.cpp

  Qore Programming Language
  
  Copyright 2003 - 2011 David Nichols
  
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
#include <qore/intern/QC_Queue.h>

qore_classid_t CID_QUEUE;
QoreClass *QC_QUEUE;

static void QUEUE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   self->setPrivate(CID_QUEUE, new Queue);
}

static void QUEUE_destructor(QoreObject *self, Queue *tq, ExceptionSink *xsink) {
   tq->destructor(xsink);
   tq->deref(xsink);
}

static void QUEUE_copy(QoreObject *self, QoreObject *old, Queue *tq, ExceptionSink *xsink) {
   self->setPrivate(CID_QUEUE, new Queue(*tq));
}

static AbstractQoreNode *QUEUE_push(QoreObject *self, Queue *tq, const QoreListNode *params, ExceptionSink *xsink) {
   tq->push(get_param(params, 0));
   return 0;
}

static AbstractQoreNode *QUEUE_insert(QoreObject *self, Queue *tq, const QoreListNode *params, ExceptionSink *xsink) {
   tq->insert(get_param(params, 0));
   return 0;
}

// can't use shift because it's a reserved word
// any Queue::get(timeout $timeout_ms = 0)  
static AbstractQoreNode *QUEUE_get(QoreObject *self, Queue *tq, const QoreListNode *params, ExceptionSink *xsink) {
   AbstractQoreNode *rv;

   int timeout = (int)HARD_QORE_INT(params, 0);
   if (timeout) {
      bool to;
      rv = tq->shift(xsink, timeout, &to);
      if (to)
	 xsink->raiseException("QUEUE-TIMEOUT", "timed out after %d ms", timeout);
   }
   else
      rv = tq->shift(xsink);

   return rv;
}

// any Queue::pop(timeout $timeout_ms = 0)  
static AbstractQoreNode *QUEUE_pop(QoreObject *self, Queue *tq, const QoreListNode *params, ExceptionSink *xsink) {
   AbstractQoreNode *rv;

   int timeout = (int)HARD_QORE_INT(params, 0);
   if (timeout) {
      bool to;
      rv = tq->pop(xsink, timeout, &to);
      if (to)
	 xsink->raiseException("QUEUE-TIMEOUT", "timed out after %d ms", timeout);
   }
   else
      rv = tq->pop(xsink);

   return rv;
}

static AbstractQoreNode *QUEUE_size(QoreObject *self, Queue *tq, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(tq->size());
}

static AbstractQoreNode *QUEUE_getWaiting(QoreObject *self, Queue *q, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(q->getWaiting());
}

QoreClass *initQueueClass() {
   QORE_TRACE("initQueueClass()");

   QC_QUEUE = new QoreClass("Queue", QDOM_THREAD_CLASS);
   CID_QUEUE = QC_QUEUE->getID();

   QC_QUEUE->setConstructorExtended(QUEUE_constructor);

   QC_QUEUE->setDestructor((q_destructor_t)QUEUE_destructor);
   QC_QUEUE->setCopy((q_copy_t)QUEUE_copy);

   // Queue::push()
   QC_QUEUE->addMethodExtended("push",          (q_method_t)QUEUE_push, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);
   QC_QUEUE->addMethodExtended("insert",        (q_method_t)QUEUE_insert, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   // any Queue::get(timeout $timeout_ms = 0)  
   QC_QUEUE->addMethodExtended("get",           (q_method_t)QUEUE_get, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 1, timeoutTypeInfo, zero());

   // any Queue::pop(timeout $timeout_ms = 0)  
   QC_QUEUE->addMethodExtended("pop",           (q_method_t)QUEUE_pop, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 1, timeoutTypeInfo, zero());

   QC_QUEUE->addMethodExtended("size",          (q_method_t)QUEUE_size, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);
   QC_QUEUE->addMethodExtended("getWaiting",    (q_method_t)QUEUE_getWaiting, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   return QC_QUEUE;
}
