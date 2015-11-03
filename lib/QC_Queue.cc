/*
  QC_Queue.cc

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
#include <qore/intern/QC_Queue.h>

qore_classid_t CID_QUEUE;

static void QUEUE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   self->setPrivate(CID_QUEUE, new Queue());
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
static AbstractQoreNode *QUEUE_get(QoreObject *self, Queue *tq, const QoreListNode *params, ExceptionSink *xsink) {
   AbstractQoreNode *rv;

   int timeout = getMsZeroInt(get_param(params, 0));
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

static AbstractQoreNode *QUEUE_pop(QoreObject *self, Queue *tq, const QoreListNode *params, ExceptionSink *xsink) {
   AbstractQoreNode *rv;

   int timeout = getMsZeroInt(get_param(params, 0));
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

   QoreClass *QC_QUEUE = new QoreClass("Queue", QDOM_THREAD_CLASS);
   CID_QUEUE = QC_QUEUE->getID();
   QC_QUEUE->setConstructor(QUEUE_constructor);
   QC_QUEUE->setDestructor((q_destructor_t)QUEUE_destructor);
   QC_QUEUE->setCopy((q_copy_t)QUEUE_copy);
   QC_QUEUE->addMethod("push",          (q_method_t)QUEUE_push);
   QC_QUEUE->addMethod("insert",        (q_method_t)QUEUE_insert);
   QC_QUEUE->addMethod("get",           (q_method_t)QUEUE_get);
   QC_QUEUE->addMethod("pop",           (q_method_t)QUEUE_pop);
   QC_QUEUE->addMethod("size",          (q_method_t)QUEUE_size);
   QC_QUEUE->addMethod("getWaiting",    (q_method_t)QUEUE_getWaiting);

   return QC_QUEUE;
}
