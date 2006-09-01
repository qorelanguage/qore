/*
  modules/TIBCO/TibrvDistributedQueue.cc

  TIBCO integration to QORE

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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/params.h>

#include "QC_TibrvDistributedQueue.h"

int CID_TIBRVDQ;

static inline void *getDQ(void *obj)
{
   ((QoreTibrvDistributedQueue *)obj)->ROreference();
   return obj;
}

// syntax: name, [desc, service, network, daemon] 
class QoreNode *TIBRVDQ_constructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVDQ_constructor");

   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "missing fault-tolerant group name as first parameter to TibrvDistributedQueue::constructor()");
      return NULL;
   }
   char *cmName = pt->val.String->getBuffer();

   unsigned workerWeight, workerTasks;
   pt = get_param(params, 1);
   int64 t = pt ? pt->getAsBigInt() : TIBRVCM_DEFAULT_WORKER_WEIGHT;
   if (t < 0)
   {
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "workerWeight cannot be negative (value passed: %d)", t);
      return NULL;
   }
   workerWeight = t;
   
   pt = get_param(params, 2);
   t = pt ? pt->getAsBigInt() : TIBRVCM_DEFAULT_WORKER_TASKS;
   if (t <= 0)
   {
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "workerTasks must be greater than zero (value passed: %d)", t);
      return NULL;
   }
   workerTasks = t;
   
   unsigned short schedulerWeight;
   pt = get_param(params, 3);
   t = pt ? pt->getAsBigInt() : TIBRVCM_DEFAULT_SCHEDULER_WEIGHT;
   if (t < 0)
   {
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "schedulerWeight must be greater than or equal to zero (value passed: %d)", t);
      return NULL;
   }
   schedulerWeight = t;
   
   int64 schedulerHeartbeat, schedulerActivation;
   pt = get_param(params, 4);
   schedulerHeartbeat = pt ? pt->getAsBigInt() : 1000;
   if (schedulerHeartbeat < 0)
   {
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "schedulerHeartbeat must not be negative (value passed: %lld)", schedulerHeartbeat);
      return NULL;
   }

   pt = get_param(params, 5);
   schedulerActivation = pt ? pt->getAsBigInt() : 3500;
   if (schedulerActivation <= 0)
   {
      xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-CONSTRUCTOR-ERROR", "schedulerActivation must be greater than 0 (value passed: %lld)", schedulerActivation);
      return NULL;
   }
   
   char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   pt = test_param(params, NT_STRING, 6);
   if (pt)
      service = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 7);
   if (pt)
      network = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 8);
   if (pt)
      daemon = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 9);
   if (pt)
      desc = pt->val.String->getBuffer();

   class QoreTibrvDistributedQueue *qdq = new QoreTibrvDistributedQueue(cmName, workerWeight, workerTasks, 
									schedulerWeight, schedulerHeartbeat, schedulerActivation, 
									service, network, daemon, desc, xsink);

   if (xsink->isException() || self->setPrivate(CID_TIBRVDQ, qdq, getDQ))
      qdq->deref();

   traceout("TIBRVDQ_constructor");
   return NULL;
}

class QoreNode *TIBRVDQ_destructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVDQ_destructor()");
   // set adapter paramter
   QoreTibrvDistributedQueue *ftm = (QoreTibrvDistributedQueue *)self->getAndClearPrivateData(CID_TIBRVDQ);
   if (ftm)
      ftm->deref();

   traceout("TIBRVDQ_destructor()");
   return NULL;
}

static QoreNode *TIBRVDQ_copy(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-COPY-ERROR", "copying TibrvDistributedQueue objects is curently not supported");
   return NULL;
}

class QoreNode *TIBRVDQ_setWorkerWeight(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvDistributedQueue *ftm = (QoreTibrvDistributedQueue *)self->getReferencedPrivateData(CID_TIBRVDQ);

   if (ftm)
   {
      class QoreNode *pt = get_param(params, 0);
      int64 weight = pt ? pt->getAsBigInt() : 0;
      if (weight < 0)
	 xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-SETWEIGHT-ERROR", "workerWeight cannot be negative (value passed: %d)", weight);
      else
	 ftm->setWorkerWeight((unsigned)weight, xsink);
      ftm->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvDistributedQueue::setWorkerWeight");

   return NULL;
}

class QoreNode *TIBRVDQ_setWorkerTasks(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvDistributedQueue *ftm = (QoreTibrvDistributedQueue *)self->getReferencedPrivateData(CID_TIBRVDQ);
   
   if (ftm)
   {
      class QoreNode *pt = get_param(params, 0);
      int64 tasks = pt ? pt->getAsBigInt() : 0;
      if (tasks <= 0)
	 xsink->raiseException("TIBRV-DISTRIBUTEDQUEUE-SETWEIGHT-ERROR", "workerTasks must be positive (value passed: %d)", tasks);
      else
	 ftm->setWorkerTasks((unsigned)tasks, xsink);
      ftm->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvDistributedQueue::setWorkerTasks");
   
   return NULL;
}

class QoreNode *TIBRVDQ_getWorkerWeight(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvDistributedQueue *ftm = (QoreTibrvDistributedQueue *)self->getReferencedPrivateData(CID_TIBRVDQ);
   class QoreNode *rv = NULL;
   
   if (ftm)
   {
      int64 weight = ftm->getWorkerWeight(xsink);
      if (!xsink->isException())
	 rv = new QoreNode(weight);
      ftm->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvDistributedQueue::getWorkerWeight");
   
   return rv;
}

class QoreNode *TIBRVDQ_getWorkerTasks(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvDistributedQueue *ftm = (QoreTibrvDistributedQueue *)self->getReferencedPrivateData(CID_TIBRVDQ);
   class QoreNode *rv = NULL;
   
   if (ftm)
   {
      int64 tasks = ftm->getWorkerTasks(xsink);
      if (!xsink->isException())
	 rv = new QoreNode(tasks);
      ftm->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvDistributedQueue::getWorkerTasks");
   
   return rv;
}

class QoreClass *initTibrvDistributedQueueClass()
{
   tracein("initTibrvDistributedQueueClass()");

   class QoreClass *QC_TIBRVDQ = new QoreClass(strdup("TibrvDistributedQueue"));
   CID_TIBRVDQ = QC_TIBRVDQ->getID();
   QC_TIBRVDQ->addMethod("constructor",      TIBRVDQ_constructor);
   QC_TIBRVDQ->addMethod("destructor",       TIBRVDQ_destructor);
   QC_TIBRVDQ->addMethod("copy",             TIBRVDQ_copy);
   QC_TIBRVDQ->addMethod("setWorkerWeight",  TIBRVDQ_setWorkerWeight);
   QC_TIBRVDQ->addMethod("setWorkerTasks",   TIBRVDQ_setWorkerTasks);
   QC_TIBRVDQ->addMethod("getWorkerWeight",  TIBRVDQ_getWorkerWeight);
   QC_TIBRVDQ->addMethod("getWorkerTasks",   TIBRVDQ_getWorkerTasks);

   traceout("initTibrvDistributedQueueClass()");
   return QC_TIBRVDQ;
}
