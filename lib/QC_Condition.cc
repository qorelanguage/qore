/*
  QC_Condition.cc

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
#include <qore/qore_thread.h>
#include <qore/QC_Condition.h>
#include <qore/QC_Mutex.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>
#include <errno.h>

int CID_CONDITION;

static void CONDITION_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_CONDITION, new Condition());
}

static void CONDITION_copy(class Object *self, class Object *old, class Condition *c, ExceptionSink *xsink)
{
   self->setPrivate(CID_CONDITION, new Condition());
}

class QoreNode *CONDITION_signal(class Object *self, class Condition *c, class QoreNode *params, ExceptionSink *xsink)
{
   if (c->signal())
      xsink->raiseException("CONDITION-SIGNAL-ERROR", strerror(errno)); 

   return NULL;
}

class QoreNode *CONDITION_broadcast(class Object *self, class Condition *c, class QoreNode *params, ExceptionSink *xsink)
{
   if (c->broadcast())
      xsink->raiseException("CONDITION-BROADCAST-ERROR", strerror(errno));

   return NULL;
}

class QoreNode *CONDITION_wait(class Object *self, class Condition *c, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   Mutex *m = p0 ? (Mutex *)p0->val.object->getReferencedPrivateData(CID_MUTEX) : NULL;
   if (!p0 || !m)
   {
      xsink->raiseException("CONDITION-WAIT-PARAMETER-EXCEPTION", "expecting a Mutex object as parameter to Condition::wait()");
      return NULL;
   }

   int timeout = getMsZeroInt(get_param(params, 1));
   QoreNode *rv;

   int rc;
   if (timeout)
      rc = c->wait(&m->ptm_lock, timeout);
   else
      rc = c->wait(&m->ptm_lock);

   if (rc && rc != ETIMEDOUT)
   {
      xsink->raiseException("CONDITION-WAIT-ERROR", strerror(errno));
      rv = NULL;
   }
   else
      rv = new QoreNode((int64)rc);

   m->deref();
   return rv;
}

class QoreClass *initConditionClass()
{
   tracein("initConditionClass()");

   class QoreClass *QC_CONDITION = new QoreClass(QDOM_THREAD_CLASS, strdup("Condition"));
   CID_CONDITION = QC_CONDITION->getID();

   QC_CONDITION->setConstructor(CONDITION_constructor);
   QC_CONDITION->setCopy((q_copy_t)CONDITION_copy);
   QC_CONDITION->addMethod("signal",        (q_method_t)CONDITION_signal);
   QC_CONDITION->addMethod("broadcast",     (q_method_t)CONDITION_broadcast);
   QC_CONDITION->addMethod("wait",          (q_method_t)CONDITION_wait);

   traceout("initConditionClass()");
   return QC_CONDITION;
}
