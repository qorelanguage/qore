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

static inline void *getQueue(void *obj)
{
   ((Queue *)obj)->ROreference();
   return obj;
}

class QoreNode *QUEUE_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QUEUE, new Queue(), getQueue);
   return NULL;
}

class QoreNode *QUEUE_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Queue *tq = (Queue *)self->getAndClearPrivateData(CID_QUEUE);
   if (tq)
      tq->deref(xsink);
   return NULL;
}

class QoreNode *QUEUE_push(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Queue *tq = (Queue *)self->getReferencedPrivateData(CID_QUEUE);
   if (tq)
   {
      tq->push(get_param(params, 0));
      tq->deref(xsink);
   }
   else
      alreadyDeleted(xsink, "Queue::push");
   return NULL;
}

// can't use shift because it's a reserved word
class QoreNode *QUEUE_get(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Queue *tq = (Queue *)self->getReferencedPrivateData(CID_QUEUE);
   QoreNode *rv;

   class QoreNode *p0 = get_param(params, 0);
   int timeout = p0 ? p0->getAsInt() : 0;

   if (tq)
   {
      if (timeout)
      {
	 bool to;
	 rv = tq->shift(timeout, &to);
	 if (to)
	    xsink->raiseException("QUEUE-TIMEOUT", "timed out after %d ms", timeout);
      }
      else
	 rv = tq->shift();
      tq->deref(xsink);
   }
   else
   {
      alreadyDeleted(xsink, "Queue::get");
      rv = NULL;
   }
   return rv;
}

class QoreNode *QUEUE_pop(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Queue *tq = (Queue *)self->getReferencedPrivateData(CID_QUEUE);
   QoreNode *rv;

   class QoreNode *p0 = get_param(params, 0);
   int timeout = p0 ? p0->getAsInt() : 0;

   if (tq)
   {
      if (timeout)
      {
	 bool to;
	 rv = tq->pop(timeout, &to);
	 if (to)
	    xsink->raiseException("QUEUE-TIMEOUT", "timed out after %d ms", timeout);
      }
      else
	 rv = tq->pop();
      tq->deref(xsink);
   }
   else
   {
      alreadyDeleted(xsink, "Queue::pop");
      rv = NULL;
   }
   return rv;
}

class QoreNode *QUEUE_size(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Queue *tq = (Queue *)self->getReferencedPrivateData(CID_QUEUE);
   QoreNode *rv;

   if (tq)
   {
      rv = new QoreNode(NT_INT, tq->size());
      tq->deref(xsink);
   }
   else
   {
      alreadyDeleted(xsink, "Queue::size");
      rv = NULL;
   }
   return rv;
}

class QoreClass *initQueueClass()
{
   tracein("initQueueClass()");

   class QoreClass *QC_QUEUE = new QoreClass(strdup("Queue"));
   CID_QUEUE = QC_QUEUE->getID();
   QC_QUEUE->addMethod("constructor",   QUEUE_constructor);
   QC_QUEUE->addMethod("destructor",    QUEUE_destructor);
   QC_QUEUE->addMethod("copy",          QUEUE_constructor);
   QC_QUEUE->addMethod("push",          QUEUE_push);
   QC_QUEUE->addMethod("get",           QUEUE_get);
   QC_QUEUE->addMethod("pop",           QUEUE_pop);
   QC_QUEUE->addMethod("size",          QUEUE_size);

   traceout("initQueueClass()");
   return QC_QUEUE;
}
