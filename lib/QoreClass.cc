/*
  QoreClass.cc

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
#include <qore/QoreClass.h>
#include <qore/qore_thread.h>
#include <qore/params.h>
#include <qore/Namespace.h>
#include <qore/Sequence.h>
#include <qore/BuiltinFunctionList.h>

// global class ID sequence
class Sequence classIDSeq;

class QoreNode *Method::eval(Object *self, QoreNode *args, ExceptionSink *xsink)
{
   QoreNode *rv = NULL;

   tracein("Method::eval()");
#ifdef DEBUG
   char *oname = self->getClass()->getName();
   printd(5, "Method::eval() %s::%s() (object=%08p, pgm=%08p)\n", 
	  oname, name, self, self->getProgram());
#endif

   int need_deref = 0;

   // need to evaluate arguments before pushing object context
   QoreNode *new_args;
   if (args)
   {
      if (args->val.list->needsEval())
      {
	 printd(5, "Method::eval() about to evaluate args=%08p (%s)\n", args, args->type->name);
	 new_args = args->eval(xsink);
	 printd(5, "Method::eval() args=%08p (%s) new_args=%08p (%s)\n",
		args, args->type->name, new_args, new_args ? new_args->type->name : "NONE");
	 if (xsink->isEvent())
	 {
	    if (new_args)
	       new_args->deref(xsink);
	    traceout("Method::eval()");
	    return NULL;
	 }
	 need_deref = 1;
      }
      else
	 new_args = args;
   }
   else
      new_args = NULL;

   // switch to new program for imported objects
   QoreProgram *cpgm;
   QoreProgram *opgm = self->getProgram();
   if (opgm)
   {
      cpgm = getProgram();
      if (opgm && cpgm != opgm)
	 pushProgram(opgm);
   }
   else
      cpgm = NULL;

   if (type == OTF_USER)
      rv = func.userFunc->eval(new_args, self, xsink);
   else
      rv = self->evalBuiltinMethodWithPrivateData(func.builtin, new_args, xsink);

   // switch back to original program if necessary
   if (opgm && cpgm != opgm)
      popProgram();

   if (new_args && need_deref)
      new_args->deref(xsink);
#ifdef DEBUG
   printd(5, "Method::eval() %s::%s() returning %08p (type=%s, refs=%d)\n",
	  oname, name, rv, rv ? rv->type->name : "(null)", rv ? rv->reference_count() : 0);
#endif
   traceout("Method::eval()");
   return rv;
}

void Method::evalConstructor(Object *self, QoreNode *args, class BCList *bcl, class BCEAList *bceal, ExceptionSink *xsink)
{
   tracein("Method::evalConstructor()");
#ifdef DEBUG
   char *oname = self->getClass()->getName();
   printd(5, "Method::evalConstructor() %s::%s() (object=%08p, pgm=%08p)\n", 
	  oname, name, self, self->getProgram());
#endif

   int need_deref = 0;

   // need to evaluate arguments before pushing object context
   QoreNode *new_args;
   if (args)
   {
      if (args->val.list->needsEval())
      {
	 printd(5, "Method::evalConstructor() about to evaluate args=%08p (%s)\n", args, args->type->name);
	 new_args = args->eval(xsink);
	 printd(5, "Method::evalConstructor() args=%08p (%s) new_args=%08p (%s)\n",
		args, args->type->name, new_args, new_args ? new_args->type->name : "NONE");
	 if (xsink->isEvent())
	 {
	    if (new_args)
	       new_args->deref(xsink);
	    traceout("Method::evalConstructor()");
	    return;
	 }
	 need_deref = 1;
      }
      else
	 new_args = args;
   }
   else
      new_args = NULL;

   if (!xsink->isEvent())
   {
      if (type == OTF_USER)
      {
	 class QoreNode *rv = func.userFunc->evalConstructor(new_args, self, bcl, bceal, xsink);
	 if (rv)
	    rv->deref(xsink);
      }
      else
      {
	 // switch to new program for imported objects
	 QoreProgram *cpgm;
	 QoreProgram *opgm = self->getProgram();
	 if (opgm)
	 {
	    cpgm = getProgram();
	    if (opgm && cpgm != opgm)
	       pushProgram(opgm);
	 }
	 else
	    cpgm = NULL;

	 func.builtin->evalConstructor(self, new_args, xsink);

	 // switch back to original program if necessary
	 if (opgm && cpgm != opgm)
	    popProgram();
      }
   }

   if (new_args && need_deref)
      new_args->deref(xsink);
#ifdef DEBUG
   printd(5, "Method::evalConstructor() %s::%s() done\n", oname, name);
#endif
   traceout("Method::evalConstructor()");
}

void Method::evalCopy(Object *self, Object *old, ExceptionSink *xsink)
{
   // switch to new program for imported objects
   QoreProgram *cpgm;
   QoreProgram *opgm = self->getProgram();
   if (opgm)
   {
      cpgm = getProgram();
      if (opgm && cpgm != opgm)
	 pushProgram(opgm);
   }
   else
      cpgm = NULL;

   if (type == OTF_USER)
      func.userFunc->evalCopy(old, self, xsink);
   else // builtin function
      old->evalCopyMethodWithPrivateData(func.builtin, self, xsink);

   // switch back to original program if necessary
   if (opgm && cpgm != opgm)
      popProgram();
}

void Method::evalDestructor(Object *self, ExceptionSink *xsink)
{
   // switch to new program for imported objects
   QoreProgram *cpgm;
   QoreProgram *opgm = self->getProgram();
   if (opgm)
   {
      cpgm = getProgram();
      if (opgm && cpgm != opgm)
	 pushProgram(opgm);
   }
   else
      cpgm = NULL;

   if (type == OTF_USER)
      func.userFunc->eval(NULL, self, xsink);
   else // builtin function
   {
      void *ptr = self->getAndClearPrivateData(func.builtin->myclass->getID());
      if (ptr)
	 func.builtin->evalDestructor(self, ptr, xsink);
      else
      {
	 if (self->getClass() == func.builtin->myclass)
	    xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::destructor() cannot be executed because the object has already been deleted", self->getClass()->getName());
	 else
	    xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::destructor() (base class of '%s') cannot be executed because the object has already been deleted", func.builtin->myclass->getName(), self->getClass()->getName());
      }
   }

   // switch back to original program if necessary
   if (opgm && cpgm != opgm)
      popProgram();
}

class QoreClass *QoreClass::copyAndDeref()
{
   tracein("QoreClass::copyAndDeref");
   class QoreClass *noc = new QoreClass(name, classID);

   printd(0, "QoreClass::copyAndDeref() name=%s (%08p) new name=%s (%08p)\n", name, name, noc->name, noc->name);

   // set up function list

   for (hm_method_t::iterator i = hm.begin(); i != hm.end(); i++)
   {
      class Method *nf = i->second->copy();

      noc->hm[nf->name] = nf;
      if (i->second == constructor)
	 noc->constructor  = nf;
      else if (i->second == destructor)
	 noc->destructor   = nf;
      else if (i->second == copyMethod)
	 noc->copyMethod   = nf;
      else if (i->second == methodGate)
	 noc->methodGate   = nf;
      else if (i->second == memberGate)
	 noc->memberGate   = nf;
   }
   // copy private member list
   for (hm_qn_t::iterator i = pmm.begin(); i != pmm.end(); i++)
      noc->pmm[strdup(i->first)] = NULL;

   // note that if there is a base class argument list, it
   // is referenced when the constructor is copied
   noc->bcal = bcal;
   if (scl)
   {
      scl->ref();
      noc->scl = scl;
   }

   nderef();
   traceout("QoreClass::copyAndDeref");
   return noc;
}

void QoreClass::insertMethod(Method *o)
{
   //printd(5, "QoreClass::insertMethod() %s::%s() size=%d\n", name, o->name, numMethods());

   hm[o->name] = o;
}      

class QoreNode *QoreClass::evalMethod(Object *self, char *nme, QoreNode *args, class ExceptionSink *xsink)
{
   tracein("QoreClass::evalMethod()");
   Method *w;
   int external = (this != getStackClass());
   printd(5, "QoreClass::evalMethod() %s::%s() %s call attempted\n", name, nme, external ? "external" : "internal" );

   if (!strcmp(nme, "copy"))
   {
      traceout("QoreClass::evalMethod()");
      return execCopy(self, xsink);
   }

   bool priv = false;
   if (!(w = findMethod(nme, &priv)))
   {
      if (methodGate && !methodGate->inMethod(self)) // call methodGate with unknown method name and arguments
	 return evalMethodGate(self, nme, args, xsink);
      // otherwise return an exception
      xsink->raiseException("METHOD-DOES-NOT-EXIST", "no method %s::%s() has been defined", name, nme);
      traceout("QoreClass::evalMethod()");
      return NULL;
   }
   // check for illegal explicit call
   if (//w == copyMethod || 
       w == constructor || w == destructor)
   {
      xsink->raiseException("ILLEGAL-EXPLICIT-METHOD-CALL", "explicit calls to ::%s() methods are not allowed", nme);
      traceout("QoreClass::evalMethod()");
      return NULL;      
   }

   if (external)
      if (w->isPrivate())
      {
	 xsink->raiseException("METHOD-IS-PRIVATE", "%s::%s() is private and cannot be accessed externally", name, nme);
	 traceout("QoreClass::evalMethod()");
	 return NULL;
      }
      else if (priv)
      {
	 xsink->raiseException("BASE-CLASS-IS-PRIVATE", "%s() is a method of a privately-inherited class of %s", nme, name);
	 traceout("QoreClass::evalMethod()");
	 return NULL;
      }
   traceout("QoreClass::evalMethod()");
   return w->eval(self, args, xsink);
}

class QoreNode *QoreClass::evalMethodGate(Object *self, char *nme, QoreNode *args, ExceptionSink *xsink)
{
   tracein("QoreClass::evalMethodGate()");
   printd(5, "QoreClass::evalMethodGate() method=%s args=%08p\n", nme, args);
   // build new argument list
   if (args)
   {
      args = args->eval(xsink);
      if (xsink->isEvent())
      {
	 args->deref(xsink);
	 traceout("QoreClass::evalMethodGate()");
	 return NULL;
      }
      args->val.list->insert(new QoreNode(nme));
   }
   else
   {
      args = new QoreNode(new List());
      args->val.list->push(new QoreNode(nme));
   }
   QoreNode *rv = methodGate->eval(self, args, xsink);
   args->deref(xsink);
   
   traceout("QoreClass::evalMethodGate()");
   return rv;
}

bool QoreClass::isPrivateMember(char *str)
{
   hm_qn_t::iterator i = pmm.find(str);
   if (i != pmm.end())
      return true;

   if (scl)
      return scl->isPrivateMember(str);
   return false;
}

class QoreNode *internalObjectVarRef(QoreNode *n, ExceptionSink *xsink)
{
   //tracein("internalObjectVarRef()");
   //traceout("internalObjectVarRef()");
   return evalStackObjectValue(n->val.c_str, xsink);
}

static QoreNode *f_getMethodList(QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   if (!p0)
      return NULL;

   return new QoreNode(p0->val.object->getClass()->getMethodList());
}

static QoreNode *f_callObjectMethod(QoreNode *params, ExceptionSink *xsink)
{
   // get object
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   if (!p0)
      return NULL;

   // get method name
   QoreNode *p1 = test_param(params, NT_STRING, 1);
   if (!p1)
      return NULL;

   QoreNode *args;

   // if there are arguments to pass
   if (get_param(params, 2))
   {
      // create argument list by copying current list
      List *l = params->val.list->copyListFrom(2);
      if (xsink->isEvent())
      {
         if (l)
	    l->derefAndDelete(xsink);
         return NULL;
      }
      args = new QoreNode(l);
   }
   else
      args = NULL;

   // make sure method call is internal (allows access to private methods) if this function was called internally
   substituteObjectIfEqual(p0->val.object);
   QoreNode *rv = p0->val.object->evalMethod(p1->val.String, args, xsink);
   if (args)
      args->deref(xsink);
   return rv;
}

static QoreNode *f_callObjectMethodArgs(QoreNode *params, ExceptionSink *xsink)
{
   // get object
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   if (!p0)
      return NULL;

   // get method name
   QoreNode *p1 = test_param(params, NT_STRING, 1);
   if (!p1)
      return NULL;

   QoreNode *args, *p2;

   // if there are arguments to pass
   if ((p2 = get_param(params, 2)))
   {
      if (p2->type == NT_LIST)
	 args = p2;
      else
      {
	 args = new QoreNode(new List());
	 args->val.list->push(p2);
      }
   }
   else
      args = NULL;

   // make sure method call is internal (allows access to private methods) if this function was called internally
   substituteObjectIfEqual(p0->val.object);
   QoreNode *rv = p0->val.object->evalMethod(p1->val.String, args, xsink);

   if (p2 != args)
   {
      args->val.list->shift();
      args->deref(xsink);
   }

   return rv;
}

// object subsystem is initialized here
void initObjects()
{
   tracein("initObjects()");

   builtinFunctions.add("getMethodList", f_getMethodList);
   builtinFunctions.add("callObjectMethod", f_callObjectMethod);
   builtinFunctions.add("callObjectMethodArgs", f_callObjectMethodArgs);
   traceout("initObjects()");
}

void deleteObjects()
{
}
