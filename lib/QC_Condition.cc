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
#include <qore/thread.h>
#include <qore/QC_Condition.h>
#include <qore/QC_Mutex.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>
#include <errno.h>

int CID_CONDITION;

static inline void *getCond(void *obj)
{
   ((Condition *)obj)->ROreference();
   return obj;
}

class QoreNode *CONDITION_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_CONDITION, new Condition(), getCond);
   return NULL;
}

class QoreNode *CONDITION_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Condition *c = (Condition *)self->getAndClearPrivateData(CID_CONDITION);
   if (c)
      c->deref();
   return NULL;
}

class QoreNode *CONDITION_signal(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Condition *c = (Condition *)self->getReferencedPrivateData(CID_CONDITION);
   if (c)
   {
      int rc = c->signal();
      c->deref();
      if (rc)
	 xsink->raiseException("CONDITION-SIGNAL-ERROR", strerror(errno)); 
   }
   else
      alreadyDeleted(xsink, "Condition::signal");
   return NULL;
}

class QoreNode *CONDITION_broadcast(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Condition *c = (Condition *)self->getReferencedPrivateData(CID_CONDITION);
   if (c)
   {
      int rc = c->broadcast();
      c->deref();
      if (rc)
	 xsink->raiseException("CONDITION-BROADCAST-ERROR", strerror(errno));
   }
   else
      alreadyDeleted(xsink, "Condition::broadcast");
   return NULL;
}

class QoreNode *CONDITION_wait(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   Mutex *m;
   if (p0)
      m = (Mutex *)p0->val.object->getReferencedPrivateData(CID_MUTEX);
   else
      m = NULL;
   if (!p0 || !m)
   {
      xsink->raiseException("CONDITION-WAIT-PARAMETER-EXCEPTION", "expecting a Mutex object as parameter to Condition::wait()");
      return NULL;
   }

   QoreNode *p1 = get_param(params, 1);
   int timeout = p1 ? p1->getAsInt() : 0;
   QoreNode *rv;

   Condition *c = (Condition *)self->getReferencedPrivateData(CID_CONDITION);
   if (c)
   {
      int rc;
      if (timeout)
	 rc = c->wait(&m->ptm_lock, timeout);
      else
	 rc = c->wait(&m->ptm_lock);
      c->deref();
      if (rc && rc != ETIMEDOUT)
      {
	 xsink->raiseException("CONDITION-WAIT-ERROR", strerror(errno));
	 rv = NULL;
      }
      else
	 rv = new QoreNode((int64)rc);
   }
   else
   {
      alreadyDeleted(xsink, "Condition::wait");
      rv = NULL;
   }
   m->deref();
   return rv;
}

class QoreClass *initConditionClass()
{
   tracein("initConditionClass()");

   class QoreClass *QC_CONDITION = new QoreClass(strdup("Condition"));
   CID_CONDITION = QC_CONDITION->getID();

   QC_CONDITION->addMethod("constructor",   CONDITION_constructor);
   QC_CONDITION->addMethod("destructor",    CONDITION_destructor);
   QC_CONDITION->addMethod("copy",          CONDITION_constructor);
   QC_CONDITION->addMethod("signal",        CONDITION_signal);
   QC_CONDITION->addMethod("broadcast",     CONDITION_broadcast);
   QC_CONDITION->addMethod("wait",          CONDITION_wait);

   traceout("initConditionClass()");
   return QC_CONDITION;
}
