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

static void CONDITION_constructor(class QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_CONDITION, new Condition());
}

static void CONDITION_copy(class QoreObject *self, class QoreObject *old, class Condition *c, ExceptionSink *xsink)
{
   self->setPrivate(CID_CONDITION, new Condition());
}

class QoreNode *CONDITION_signal(class QoreObject *self, class Condition *c, const QoreList *params, ExceptionSink *xsink)
{
   if (c->signal())
      xsink->raiseException("CONDITION-SIGNAL-ERROR", strerror(errno)); 

   return NULL;
}

static class QoreNode *CONDITION_broadcast(class QoreObject *self, class Condition *c, const QoreList *params, ExceptionSink *xsink)
{
   if (c->broadcast())
      xsink->raiseException("CONDITION-BROADCAST-ERROR", strerror(errno));

   return NULL;
}

static class QoreNode *CONDITION_wait(class QoreObject *self, class Condition *c, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   SmartMutex *m = p0 ? (SmartMutex *)p0->val.object->getReferencedPrivateData(CID_MUTEX, xsink) : NULL;
   if (!p0 || !m)
   {
      if (!xsink->isException())
	 xsink->raiseException("CONDITION-WAIT-PARAMETER-EXCEPTION", "expecting a Mutex object as parameter to Condition::wait()");
      return NULL;
   }
   ReferenceHolder<SmartMutex> holder(m, xsink);

   int timeout = getMsZeroInt(get_param(params, 1));
   QoreNode *rv;

   int rc;
   if (timeout)
      rc = c->wait(m, timeout, xsink);
   else
      rc = c->wait(m, xsink);

   if (rc && rc != ETIMEDOUT && !*xsink)
   {
      xsink->raiseException("CONDITION-WAIT-ERROR", strerror(errno));
      rv = NULL;
   }
   else
      rv = new QoreNode((int64)rc);

   return rv;
}

static class QoreNode *CONDITION_wait_count(class QoreObject *self, class Condition *c, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   SmartMutex *m = p0 ? (SmartMutex *)p0->val.object->getReferencedPrivateData(CID_MUTEX, xsink) : NULL;
   if (!p0 || !m)
   {
      if (!xsink->isException())
	 xsink->raiseException("CONDITION-WAIT-COUNT-PARAMETER-EXCEPTION", "expecting a Mutex object as parameter to Condition::wait_count()");
      return NULL;
   }
   ReferenceHolder<SmartMutex> holder(m, xsink);

   return new QoreNode((int64)c->wait_count(m));
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
