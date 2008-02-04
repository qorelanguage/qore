/*
  QC_Condition.cc

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
#include <qore/intern/QC_Condition.h>
#include <qore/intern/QC_Mutex.h>
#include <qore/ReferenceHolder.h>

#include <errno.h>

int CID_CONDITION;

static void CONDITION_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_CONDITION, new Condition());
}

static void CONDITION_copy(QoreObject *self, QoreObject *old, class Condition *c, ExceptionSink *xsink)
{
   self->setPrivate(CID_CONDITION, new Condition());
}

AbstractQoreNode *CONDITION_signal(QoreObject *self, class Condition *c, const QoreListNode *params, ExceptionSink *xsink)
{
   if (c->signal())
      xsink->raiseException("CONDITION-SIGNAL-ERROR", strerror(errno)); 

   return NULL;
}

static AbstractQoreNode *CONDITION_broadcast(QoreObject *self, class Condition *c, const QoreListNode *params, ExceptionSink *xsink)
{
   if (c->broadcast())
      xsink->raiseException("CONDITION-BROADCAST-ERROR", strerror(errno));

   return NULL;
}

static AbstractQoreNode *CONDITION_wait(QoreObject *self, class Condition *c, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p0 = test_object_param(params, 0);
   SmartMutex *m = p0 ? (SmartMutex *)p0->getReferencedPrivateData(CID_MUTEX, xsink) : 0;
   if (!m)
   {
      if (!*xsink)
	 xsink->raiseException("CONDITION-WAIT-PARAMETER-EXCEPTION", "expecting a Mutex object as parameter to Condition::wait()");
      return 0;
   }
   ReferenceHolder<SmartMutex> holder(m, xsink);

   int timeout = getMsZeroInt(get_param(params, 1));

   int rc;
   if (timeout)
      rc = c->wait(m, timeout, xsink);
   else
      rc = c->wait(m, xsink);

   if (rc && rc != ETIMEDOUT && !*xsink)
   {
      xsink->raiseException("CONDITION-WAIT-ERROR", strerror(errno));
      return 0;
   }
   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *CONDITION_wait_count(QoreObject *self, class Condition *c, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *p0 = test_object_param(params, 0);
   SmartMutex *m = p0 ? (SmartMutex *)p0->getReferencedPrivateData(CID_MUTEX, xsink) : NULL;
   if (!m)
   {
      if (!*xsink)
	 xsink->raiseException("CONDITION-WAIT-COUNT-PARAMETER-EXCEPTION", "expecting a Mutex object as parameter to Condition::wait_count()");
      return 0;
   }
   ReferenceHolder<SmartMutex> holder(m, xsink);

   return new QoreBigIntNode(c->wait_count(m));
}

class QoreClass *initConditionClass()
{
   tracein("initConditionClass()");

   class QoreClass *QC_CONDITION = new QoreClass("Condition", QDOM_THREAD_CLASS);
   CID_CONDITION = QC_CONDITION->getID();

   QC_CONDITION->setConstructor(CONDITION_constructor);
   QC_CONDITION->setCopy((q_copy_t)CONDITION_copy);
   QC_CONDITION->addMethod("signal",        (q_method_t)CONDITION_signal);
   QC_CONDITION->addMethod("broadcast",     (q_method_t)CONDITION_broadcast);
   QC_CONDITION->addMethod("wait",          (q_method_t)CONDITION_wait);
   QC_CONDITION->addMethod("wait_count",    (q_method_t)CONDITION_wait_count);

   traceout("initConditionClass()");
   return QC_CONDITION;
}
