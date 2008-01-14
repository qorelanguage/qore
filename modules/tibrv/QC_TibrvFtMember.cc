/*
  modules/TIBCO/TibrvFtMember.cc

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

#include "QC_TibrvFtMember.h"

int CID_TIBRVFTMEMBER;

// syntax: subject, [desc, service, network, daemon] 
static void TIBRVFTMEMBER_constructor(class QoreObject *self, const QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVFTMEMBER_constructor");

   QoreStringNode *str = test_string_param(params, 0);
   if (!str)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "missing fault-tolerant group name as first parameter to TibrvFtMember::constructor()");
      return;
   }
   const char *groupName = str->getBuffer();

   int weight, activeGoal;
   QoreNode *pt = get_param(params, 1);
   weight = pt ? pt->getAsInt() : 0;
   if (weight <= 0)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "weight must be greater than zero (value passed: %d)", weight);
      return;
   }

   pt = get_param(params, 2);
   activeGoal = pt ? pt->getAsInt() : 0;
   if (activeGoal <= 0)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "activeGoal must be greater than zero (value passed: %d)", activeGoal);
      return;
   }
   
   int64 heartbeat, prep, activation;
   pt = get_param(params, 3);
   heartbeat = pt ? pt->getAsBigInt() : 0;
   if (heartbeat <= 0)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "heartbeat interval must be greater than zero (value passed: %d)", heartbeat);
      return;
   }
   
   pt = get_param(params, 4);
   prep = pt ? pt->getAsBigInt() : 0;
   if (prep < 0)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "preparation interval must not be negative (value passed: %lld)", prep);
      return;
   }

   pt = get_param(params, 5);
   activation = pt ? pt->getAsBigInt() : 0;
   if (prep <= 0)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "activation interval must be greater than 0 (value passed: %lld)", activation);
      return;
   }
   
   if (activation < heartbeat)
   {
      xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "activation interval (%lld) must be greater than the heartbeat interval (%lld)", activation, heartbeat);
      return;      
   }
   if (prep)
   {
      if (prep < heartbeat)
      {
	 xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "preparation interval (%lld) must be greater than the heartbeat interval (%lld)", prep, heartbeat);
	 return;      
      }
      if (prep > activation)
      {
	 xsink->raiseException("TIBRVFTMEMBER-CONSTRUCTOR-ERROR", "preparation interval (%lld) must be less than the activation interval (%lld)", prep, activation);
	 return;
      }
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

   class QoreTibrvFtMember *qftmember = new QoreTibrvFtMember(groupName, weight, activeGoal, heartbeat, prep, activation, service, network, daemon, desc, xsink);

   if (xsink->isException())
      qftmember->deref();
   else
      self->setPrivate(CID_TIBRVFTMEMBER, qftmember);

   traceout("TIBRVFTMEMBER_constructor");
}

static void TIBRVFTMEMBER_destructor(class QoreObject *self, class QoreTibrvFtMember *ftm, class ExceptionSink *xsink)
{
   tracein("TIBRVFTMEMBER_destructor()");
   ftm->stop();
   ftm->deref();
   traceout("TIBRVFTMEMBER_destructor()");
}

static void TIBRVFTMEMBER_copy(class QoreObject *self, class QoreObject *old, class QoreTibrvFtMember *ftm, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVFTMEMBER-COPY-ERROR", "copying TibrvFtMember objects is curently not supported");
}

static QoreNode *TIBRVFTMEMBER_getEvent(class QoreObject *self, class QoreTibrvFtMember *ftm, const QoreNode *params, ExceptionSink *xsink)
{
   return ftm->getEvent(xsink);
}

static QoreNode *TIBRVFTMEMBER_stop(class QoreObject *self, class QoreTibrvFtMember *ftm, const QoreNode *params, ExceptionSink *xsink)
{
   ftm->stop();
   return NULL;
}

class QoreNode *TIBRVFTMEMBER_getGroupName(class QoreObject *self, class QoreTibrvFtMember *ftm, const QoreNode *params, ExceptionSink *xsink)
{
   const char *name = ftm->getGroupName();
   return name ? new QoreStringNode((char *)name) : 0;
}

class QoreNode *TIBRVFTMEMBER_setWeight(class QoreObject *self, class QoreTibrvFtMember *ftm, const QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = get_param(params, 0);
   int weight = pt ? pt->getAsInt() : 0;
   if (weight <= 0)
      xsink->raiseException("TIBRVFTMEMBER-SETWEIGHT-ERROR", "weight must be greater than zero (value passed: %d)", weight);
   else
      ftm->setWeight(weight, xsink);

   return NULL;
}

class QoreClass *initTibrvFtMemberClass()
{
   tracein("initTibrvFtMemberClass()");

   class QoreClass *QC_TIBRVFTMEMBER = new QoreClass("TibrvFtMember", QDOM_NETWORK);
   CID_TIBRVFTMEMBER = QC_TIBRVFTMEMBER->getID();
   QC_TIBRVFTMEMBER->setConstructor(TIBRVFTMEMBER_constructor);
   QC_TIBRVFTMEMBER->setDestructor((q_destructor_t)TIBRVFTMEMBER_destructor);
   QC_TIBRVFTMEMBER->setCopy((q_copy_t)TIBRVFTMEMBER_copy);
   QC_TIBRVFTMEMBER->addMethod("getEvent",      (q_method_t)TIBRVFTMEMBER_getEvent);
   QC_TIBRVFTMEMBER->addMethod("stop",          (q_method_t)TIBRVFTMEMBER_stop);
   QC_TIBRVFTMEMBER->addMethod("getGroupName",  (q_method_t)TIBRVFTMEMBER_getGroupName);
   QC_TIBRVFTMEMBER->addMethod("setWeight",     (q_method_t)TIBRVFTMEMBER_setWeight);

   traceout("initTibrvFtMemberClass()");
   return QC_TIBRVFTMEMBER;
}
