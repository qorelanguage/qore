/*
  QC_Queue.cc

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
#include <qore/QC_Queue.h>

int CID_QUEUE;

static void QUEUE_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QUEUE, new Queue());
}

static void QUEUE_copy(class Object *self, class Object *old, class Queue *tq, ExceptionSink *xsink)
{
   self->setPrivate(CID_QUEUE, new Queue());
}

static class QoreNode *QUEUE_push(class Object *self, class Queue *tq, class QoreNode *params, ExceptionSink *xsink)
{
   tq->push(get_param(params, 0));
   return NULL;
}

// can't use shift because it's a reserved word
static class QoreNode *QUEUE_get(class Object *self, class Queue *tq, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;

   int timeout = getMsZeroInt(get_param(params, 0));
   if (timeout)
   {
      bool to;
      rv = tq->shift(timeout, &to);
      if (to)
	 xsink->raiseException("QUEUE-TIMEOUT", "timed out after %d ms", timeout);
   }
   else
      rv = tq->shift();

   return rv;
}

static class QoreNode *QUEUE_pop(class Object *self, class Queue *tq, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;

   int timeout = getMsZeroInt(get_param(params, 0));
   if (timeout)
   {
      bool to;
      rv = tq->pop(timeout, &to);
      if (to)
	 xsink->raiseException("QUEUE-TIMEOUT", "timed out after %d ms", timeout);
   }
   else
      rv = tq->pop();

   return rv;
}

static class QoreNode *QUEUE_size(class Object *self, class Queue *tq, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)tq->size());
}

class QoreClass *initQueueClass()
{
   tracein("initQueueClass()");

   class QoreClass *QC_QUEUE = new QoreClass(QDOM_THREAD_CLASS, strdup("Queue"));
   CID_QUEUE = QC_QUEUE->getID();
   QC_QUEUE->setConstructor(QUEUE_constructor);
   QC_QUEUE->setCopy((q_copy_t)QUEUE_copy);
   QC_QUEUE->addMethod("push",          (q_method_t)QUEUE_push);
   QC_QUEUE->addMethod("get",           (q_method_t)QUEUE_get);
   QC_QUEUE->addMethod("pop",           (q_method_t)QUEUE_pop);
   QC_QUEUE->addMethod("size",          (q_method_t)QUEUE_size);

   traceout("initQueueClass()");
   return QC_QUEUE;
}
