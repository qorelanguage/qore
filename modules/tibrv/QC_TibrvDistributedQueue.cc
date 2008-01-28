/*
  modules/TIBCO/TibrvDistributedQueue.cc

  TIBCO integration to QORE

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

#include "QC_TibrvDistributedQueue.h"

int CID_TIBRVDQ;

// syntax: name, [desc, service, network, daemon] 
void TIBRVDQ_constructor(class QoreObject *self, const QoreListNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVDQ_constructor");

   QoreStringNode *str = test_string_param(params, 0);
   if (!str)
   {
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "missing fault-tolerant group name as first parameter to TibrvDistributedQueue::constructor()");
      return;
   }
   const char *cmName = str->getBuffer();

   unsigned workerWeight, workerTasks;
   QoreNode *pt = get_param(params, 1);
   int64 t = pt ? pt->getAsBigInt() : TIBRVCM_DEFAULT_WORKER_WEIGHT;
   if (t < 0)
   {
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "workerWeight cannot be negative (value passed: %d)", t);
      return;
   }
   workerWeight = t;
   
   pt = get_param(params, 2);
   t = pt ? pt->getAsBigInt() : TIBRVCM_DEFAULT_WORKER_TASKS;
   if (t <= 0)
   {
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "workerTasks must be greater than zero (value passed: %d)", t);
      return;
   }
   workerTasks = t;

   unsigned short schedulerWeight;
   pt = get_param(params, 3);
   t = pt ? pt->getAsBigInt() : TIBRVCM_DEFAULT_SCHEDULER_WEIGHT;
   if (t < 0)
   {
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "schedulerWeight must be greater than or equal to zero (value passed: %d)", t);
      return;
   }
   schedulerWeight = t;
   
   int64 schedulerHeartbeat, schedulerActivation;
   pt = get_param(params, 4);
   schedulerHeartbeat = pt ? pt->getAsBigInt() : 1000;
   if (schedulerHeartbeat < 0)
   {
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "schedulerHeartbeat must not be negative (value passed: %lld)", schedulerHeartbeat);
      return;
   }

   pt = get_param(params, 5);
   schedulerActivation = pt ? pt->getAsBigInt() : 3500;
   if (schedulerActivation <= 0)
   {
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "schedulerActivation must be greater than 0 (value passed: %lld)", schedulerActivation);
      return;
   }
   
   const char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   str = test_string_param(params, 6);
   if (str)
      service = str->getBuffer();
   str = test_string_param(params, 7);
   if (str)
      network = str->getBuffer();
   str = test_string_param(params, 8);
   if (str)
      daemon = str->getBuffer();
   str = test_string_param(params, 9);
   if (str)
      desc = str->getBuffer();

   class QoreTibrvDistributedQueue *qdq = new QoreTibrvDistributedQueue(cmName, workerWeight, workerTasks, 
									schedulerWeight, schedulerHeartbeat, schedulerActivation, 
									service, network, daemon, desc, xsink);

   if (xsink->isException())
      qdq->deref();
   else
      self->setPrivate(CID_TIBRVDQ, qdq);

   traceout("TIBRVDQ_constructor");
}

void TIBRVDQ_copy(class QoreObject *self, class QoreObject *old, class QoreTibrvDistributedQueue *dq, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-COPY-ERROR", "copying TibrvDistributedQueue objects is curently not supported");
}

class QoreNode *TIBRVDQ_setWorkerWeight(class QoreObject *self, class QoreTibrvDistributedQueue *dq, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = get_param(params, 0);
   int64 weight = pt ? pt->getAsBigInt() : 0;
   if (weight < 0)
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-SETWEIGHT-ERROR", "workerWeight cannot be negative (value passed: %d)", weight);
   else
      dq->setWorkerWeight((unsigned)weight, xsink);

   return NULL;
}

class QoreNode *TIBRVDQ_setWorkerTasks(class QoreObject *self, class QoreTibrvDistributedQueue *dq, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = get_param(params, 0);
   int64 tasks = pt ? pt->getAsBigInt() : 0;
   if (tasks <= 0)
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-SETWEIGHT-ERROR", "workerTasks must be positive (value passed: %d)", tasks);
   else
      dq->setWorkerTasks((unsigned)tasks, xsink);
   
   return NULL;
}

class QoreNode *TIBRVDQ_getWorkerWeight(class QoreObject *self, class QoreTibrvDistributedQueue *dq, const QoreListNode *params, ExceptionSink *xsink)
{
   int64 weight = dq->getWorkerWeight(xsink);
   if (!xsink->isException())
      return new QoreBigIntNode(weight);
   
   return NULL;
}

class QoreNode *TIBRVDQ_getWorkerTasks(class QoreObject *self, class QoreTibrvDistributedQueue *dq, const QoreListNode *params, ExceptionSink *xsink)
{
   int64 tasks = dq->getWorkerTasks(xsink);
   if (!xsink->isException())
      return new QoreBigIntNode(tasks);

   return NULL;
}

class QoreClass *initTibrvDistributedQueueClass()
{
   tracein("initTibrvDistributedQueueClass()");

   class QoreClass *QC_TIBRVDQ = new QoreClass("TibrvDistributedQueue", QDOM_NETWORK);
   CID_TIBRVDQ = QC_TIBRVDQ->getID();
   QC_TIBRVDQ->setConstructor(TIBRVDQ_constructor);
   QC_TIBRVDQ->setCopy((q_copy_t)TIBRVDQ_copy);
   QC_TIBRVDQ->addMethod("setWorkerWeight",  (q_method_t)TIBRVDQ_setWorkerWeight);
   QC_TIBRVDQ->addMethod("setWorkerTasks",   (q_method_t)TIBRVDQ_setWorkerTasks);
   QC_TIBRVDQ->addMethod("getWorkerWeight",  (q_method_t)TIBRVDQ_getWorkerWeight);
   QC_TIBRVDQ->addMethod("getWorkerTasks",   (q_method_t)TIBRVDQ_getWorkerTasks);

   traceout("initTibrvDistributedQueueClass()");
   return QC_TIBRVDQ;
}
