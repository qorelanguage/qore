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
#include <qore/thread.h>
#include <qore/QC_Gate.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>

int CID_GATE;

static inline void *getGate(void *obj)
{
   ((QoreGate *)obj)->ROreference();
   return obj;
}

class QoreNode *GATE_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_GATE, new QoreGate(), getGate);
   return NULL;
}

class QoreNode *GATE_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreGate *g = (QoreGate *)self->getAndClearPrivateData(CID_GATE);
   if (g)
      g->deref();
   return NULL;
}

class QoreNode *GATE_enter(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreGate *cg = (QoreGate *)self->getReferencedPrivateData(CID_GATE);
   QoreNode *p = get_param(params, 0);
   QoreNode *rv;

   if (cg)
   {
      if (p)
      {
	 int code = p->getAsInt();
	 if (code == G_NOBODY)
	 {
	    xsink->raiseException("GATE-ENTER-PARAMETER-EXCEPTION", "%d is not allowed as a code when calling Gate::enter()", 
			   code);
	    rv = NULL;
	 }
	 else
	    rv = new QoreNode(NT_INT, cg->enter(code));
      }
      else
	 rv = new QoreNode(NT_INT, cg->enter());
      cg->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Gate::enter");
   }
   return rv;
}

class QoreNode *GATE_exit(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreGate *cg = (QoreGate *)self->getReferencedPrivateData(CID_GATE);
   QoreNode *rv;

   if (cg)
   {
      int rc = cg->exit();
      rv = new QoreNode(NT_INT, rc);
      cg->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Gate::exit");
      rv = NULL;
   }
   return rv;
}

class QoreNode *GATE_tryEnter(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreGate *cg = (QoreGate *)self->getReferencedPrivateData(CID_GATE);
   QoreNode *p = get_param(params, 0);
   QoreNode *rv;

   if (cg)
   {
      if (p)
      {
	 int code = p->getAsInt();
	 if (code == G_NOBODY)
	 {
	    xsink->raiseException("GATE-TRYENTER-PARAMETER-EXCEPTION", "%d is not allowed as a code when calling Gate::tryEnter()", 
			   code);
	    rv = NULL;
	 }
	 rv = new QoreNode(NT_INT, cg->tryEnter(code));
      }
      else
	 rv = new QoreNode(NT_INT, cg->tryEnter());
      cg->deref();      
   }
   else
   {
      alreadyDeleted(xsink, "Gate::tryEnter");
      rv = NULL;
   }
   return rv;      
}

class QoreNode *GATE_numInside(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreGate *cg = (QoreGate *)self->getReferencedPrivateData(CID_GATE);
   QoreNode *rv;
   if (cg)
   {
      rv = new QoreNode(NT_INT, cg->numInside());
      cg->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Gate::numInside");
   }
   return rv;
}

class QoreNode *GATE_numWaiting(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreGate *cg = (QoreGate *)self->getReferencedPrivateData(CID_GATE);
   QoreNode *rv;
   if (cg)
   {
      rv = new QoreNode(NT_INT, cg->numWaiting());
      cg->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Gate::numWaiting");
   }
   return rv;
}

class QoreClass *initGateClass()
{
   tracein("initGateClass()");

   class QoreClass *QC_GATE = new QoreClass(strdup("Gate"));
   CID_GATE = QC_GATE->getID();
   QC_GATE->addMethod("constructor",   GATE_constructor);
   QC_GATE->addMethod("destructor",    GATE_destructor);
   QC_GATE->addMethod("copy",          GATE_constructor);
   QC_GATE->addMethod("enter",         GATE_enter);
   QC_GATE->addMethod("exit",          GATE_exit);
   QC_GATE->addMethod("numInside",     GATE_numInside);
   QC_GATE->addMethod("numWaiting",    GATE_numWaiting);

   traceout("initGateClass()");
   return QC_GATE;
}
