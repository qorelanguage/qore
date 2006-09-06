/*
  QC_Queue.cc

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
#include <qore/QC_Queue.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>

int CID_QUEUE;

static inline void getQueue(void *obj)
{
   ((Queue *)obj)->ROreference();
}

static inline void releaseQueue(void *obj)
{
   class ExceptionSink xsink;
   ((Queue *)obj)->deref(&xsink);
}

static void QUEUE_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QUEUE, new Queue(), getQueue, releaseQueue);
}

static void QUEUE_destructor(class Object *self, class Queue *tq, ExceptionSink *xsink)
{
   tq->deref(xsink);
}

static void QUEUE_copy(class Object *self, class Object *old, class Queue *tq, ExceptionSink *xsink)
{
   self->setPrivate(CID_QUEUE, new Queue(), getQueue, releaseQueue);
}

class QoreNode *QUEUE_push(class Object *self, class Queue *tq, class QoreNode *params, ExceptionSink *xsink)
{
   tq->push(get_param(params, 0));
   return NULL;
}

// can't use shift because it's a reserved word
class QoreNode *QUEUE_get(class Object *self, class Queue *tq, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;

   class QoreNode *p0 = get_param(params, 0);
   int timeout = p0 ? p0->getAsInt() : 0;

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

class QoreNode *QUEUE_pop(class Object *self, class Queue *tq, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;

   class QoreNode *p0 = get_param(params, 0);
   int timeout = p0 ? p0->getAsInt() : 0;

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

class QoreNode *QUEUE_size(class Object *self, class Queue *tq, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)tq->size());
}

class QoreClass *initQueueClass()
{
   tracein("initQueueClass()");

   class QoreClass *QC_QUEUE = new QoreClass(strdup("Queue"));
   CID_QUEUE = QC_QUEUE->getID();
   QC_QUEUE->setConstructor(QUEUE_constructor);
   QC_QUEUE->setDestructor((q_destructor_t)QUEUE_destructor);
   QC_QUEUE->setCopy((q_copy_t)QUEUE_copy);
   QC_QUEUE->addMethod("push",          (q_method_t)QUEUE_push);
   QC_QUEUE->addMethod("get",           (q_method_t)QUEUE_get);
   QC_QUEUE->addMethod("pop",           (q_method_t)QUEUE_pop);
   QC_QUEUE->addMethod("size",          (q_method_t)QUEUE_size);

   traceout("initQueueClass()");
   return QC_QUEUE;
}
