/*
  modules/TIBCO/TibrvFtMember.cc

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

#include "QC_TibrvFtMember.h"

int CID_TIBRVFTMEMBER;

static inline void *getFTM(void *obj)
{
   ((QoreTibrvFtMember *)obj)->ROreference();
   return obj;
}

// syntax: subject, [desc, service, network, daemon] 
class QoreNode *TIBRVFTMEMBER_constructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVFTMEMBER_constructor");

   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "missing fault-tolerant group name as first parameter to TibrvFtMember::constructor()");
      return NULL;
   }
   char *groupName = pt->val.String->getBuffer();

   int weight, activeGoal;
   pt = get_param(params, 1);
   weight = pt ? pt->getAsInt() : 0;
   if (weight <= 0)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "weight must be greater than zero (value passed: %d)", weight);
      return NULL;
   }

   pt = get_param(params, 2);
   activeGoal = pt ? pt->getAsInt() : 0;
   if (activeGoal <= 0)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "activeGoal must be greater than zero (value passed: %d)", activeGoal);
      return NULL;
   }
   
   int64 heartbeat, prep, activation;
   pt = get_param(params, 3);
   heartbeat = pt ? pt->getAsInt() : 0;
   if (heartbeat <= 0)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "heartbeat interval must be greater than zero (value passed: %d)", heartbeat);
      return NULL;
   }
   
   pt = get_param(params, 4);
   prep = pt ? pt->getAsBigInt() : 0;
   if (prep < 0)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "preparation interval must not be negative (value passed: %lld)", prep);
      return NULL;
   }

   pt = get_param(params, 5);
   activation = pt ? pt->getAsBigInt() : 0;
   if (prep <= 0)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "activation interval must be greater than 0 (value passed: %lld)", activation);
      return NULL;
   }
   
   if (activation < heartbeat)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "activation interval (%lld) must be greater than the heartbeat interval (%lld)", activation, heartbeat);
      return NULL;      
   }
   if (prep)
   {
      if (prep < heartbeat)
      {
	 xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "preparation interval (%lld) must be greater than the heartbeat interval (%lld)", prep, heartbeat);
	 return NULL;      
      }
      if (prep > activation)
      {
	 xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "preparation interval (%lld) must be less than the activation interval (%lld)", prep, activation);
	 return NULL;
      }
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

   class QoreTibrvFtMember *qftmember = new QoreTibrvFtMember(groupName, weight, activeGoal, heartbeat, prep, activation, 
							      service, network, daemon, desc, xsink);

   if (xsink->isException() || self->setPrivate(CID_TIBRVFTMEMBER, qftmember, getFTM))
      qftmember->deref();

   traceout("TIBRVFTMEMBER_constructor");
   return NULL;
}

class QoreNode *TIBRVFTMEMBER_destructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVFTMEMBER_destructor()");
   // set adapter paramter
   QoreTibrvFtMember *ftm = (QoreTibrvFtMember *)self->getAndClearPrivateData(CID_TIBRVFTMEMBER);
   if (ftm)
      ftm->deref();
   traceout("TIBRVFTMEMBER_destructor()");
   return NULL;
}

static QoreNode *TIBRVFTMEMBER_copy(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVFTMEMBER-COPY-ERROR", "copying TibrvFtMember objects is curently not supported");
   return NULL;
}

static QoreNode *TIBRVFTMEMBER_getEvent(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvFtMember *ftm = (QoreTibrvFtMember *)self->getReferencedPrivateData(CID_TIBRVFTMEMBER);
   class QoreNode *rv = NULL;

   if (ftm)
   {
      rv = ftm->getEvent(xsink);

      ftm->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvFtMember::getEvent");

   return rv;
}

static QoreNode *TIBRVFTMEMBER_stop(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvFtMember *ftm = (QoreTibrvFtMember *)self->getReferencedPrivateData(CID_TIBRVFTMEMBER);

   if (ftm)
   {
      ftm->stop();
      ftm->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvFtMember::stop");

   return NULL;
}

class QoreNode *TIBRVFTMEMBER_getGroupName(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvFtMember *ftm = (QoreTibrvFtMember *)self->getReferencedPrivateData(CID_TIBRVFTMEMBER);
   class QoreNode *rv = NULL;

   if (ftm)
   {
      const char *name = ftm->getGroupName();
      ftm->deref();
      if (name)
	 rv = new QoreNode((char *)name);
   }
   else
      alreadyDeleted(xsink, "TibrvFtMember::getGroupName");

   return rv;
}

class QoreNode *TIBRVFTMEMBER_setWeight(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvFtMember *ftm = (QoreTibrvFtMember *)self->getReferencedPrivateData(CID_TIBRVFTMEMBER);

   if (ftm)
   {
      class QoreNode *pt = get_param(params, 0);
      int weight = pt ? pt->getAsInt() : 0;
      if (weight <= 0)
	 xsink->raiseException("TIBRVFTMEMBER-SETWEIGHT-ERROR", "weight must be greater than zero (value passed: %d)", weight);
      else
	 ftm->setWeight(weight, xsink);
      ftm->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvFtMember::setWeight");

   return NULL;
}

class QoreClass *initTibrvFtMemberClass()
{
   tracein("initTibrvFtMemberClass()");

   class QoreClass *QC_TIBRVFTMEMBER = new QoreClass(strdup("TibrvFtMember"));
   CID_TIBRVFTMEMBER = QC_TIBRVFTMEMBER->getID();
   QC_TIBRVFTMEMBER->addMethod("constructor",   TIBRVFTMEMBER_constructor);
   QC_TIBRVFTMEMBER->addMethod("destructor",    TIBRVFTMEMBER_destructor);
   QC_TIBRVFTMEMBER->addMethod("copy",          TIBRVFTMEMBER_copy);
   QC_TIBRVFTMEMBER->addMethod("getEvent",      TIBRVFTMEMBER_getEvent);
   QC_TIBRVFTMEMBER->addMethod("stop",          TIBRVFTMEMBER_stop);
   QC_TIBRVFTMEMBER->addMethod("getGroupName",  TIBRVFTMEMBER_getGroupName);
   QC_TIBRVFTMEMBER->addMethod("setWeight",     TIBRVFTMEMBER_setWeight);

   traceout("initTibrvFtMemberClass()");
   return QC_TIBRVFTMEMBER;
}
