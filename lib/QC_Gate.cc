/*
  QC_Gate.cc

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
#include <qore/QC_Gate.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>

int CID_GATE;

static void GATE_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_GATE, new QoreGate());
}

static void GATE_copy(class Object *self, class Object *old, class QoreGate *g, ExceptionSink *xsink)
{
   self->setPrivate(CID_GATE, new QoreGate());
}

class QoreNode *GATE_enter(class Object *self, class QoreGate *g, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   if (!is_nothing(p))
   {
      int code = p->getAsInt();
      if (code == G_NOBODY)
      {
	 xsink->raiseException("GATE-ENTER-PARAMETER-EXCEPTION", "%d is not allowed as a code when calling Gate::enter()", code);
	 return NULL;
      }

      return new QoreNode((int64)g->enter(code));
   }

   return new QoreNode((int64)g->enter());
}

class QoreNode *GATE_exit(class Object *self, class QoreGate *g, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)g->exit());
}

class QoreNode *GATE_tryEnter(class Object *self, class QoreGate *g, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   if (p)
   {
      int code = p->getAsInt();
      if (code == G_NOBODY)
      {
	 xsink->raiseException("GATE-TRYENTER-PARAMETER-EXCEPTION", "%d is not allowed as a code when calling Gate::tryEnter()", code);
	 return NULL;
      }
      return new QoreNode((int64)g->tryEnter(code));
   }
   return new QoreNode((int64)g->tryEnter());
}

class QoreNode *GATE_numInside(class Object *self, class QoreGate *g, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)g->numInside());
}

class QoreNode *GATE_numWaiting(class Object *self, class QoreGate *g, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)g->numWaiting());
}

class QoreClass *initGateClass()
{
   tracein("initGateClass()");

   class QoreClass *QC_GATE = new QoreClass(QDOM_THREAD_CLASS, strdup("Gate"));
   CID_GATE = QC_GATE->getID();
   QC_GATE->setConstructor(GATE_constructor);
   QC_GATE->setCopy((q_copy_t)GATE_copy);
   QC_GATE->addMethod("enter",         (q_method_t)GATE_enter);
   QC_GATE->addMethod("exit",          (q_method_t)GATE_exit);
   QC_GATE->addMethod("numInside",     (q_method_t)GATE_numInside);
   QC_GATE->addMethod("numWaiting",    (q_method_t)GATE_numWaiting);

   traceout("initGateClass()");
   return QC_GATE;
}
