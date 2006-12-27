/*
  Function.cc

  Qore Programming language

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
#include <qore/Function.h>
#include <qore/QoreNode.h>
#include <qore/Statement.h>
#include <qore/Variable.h>
#include <qore/support.h>
#include <qore/qore_thread.h>
#include <qore/Exception.h>
#include <qore/params.h>
#include <qore/CallStack.h>
#include <qore/QoreClass.h>
#include <qore/ArgvStack.h>
#include <qore/QoreType.h>
#include <qore/QoreProgram.h>
#include <qore/VRMutex.h>
#include <qore/Operator.h>
#include <qore/Object.h>
#include <qore/NamedScope.h>

#include <stdio.h>
#include <ctype.h>
#include <assert.h>

static inline void param_error()
{
   parse_error("parameter list contains non-variable reference expressions.");
}

static inline void push_argv(lvh_t argvid)
{
   //tracein("push_argv()");
   class ArgvStack *as = new ArgvStack(argvid); 
   as->next = get_argvstack();
   update_argvstack(as);
   //traceout("push_argv()");
}

static inline void pop_argv()
{
   //tracein("pop_argv()");
   ArgvStack *oldargs = get_argvstack();
   update_argvstack(oldargs->next);
   delete oldargs;
   //traceout("pop_argv()");
}

SelfFunctionCall::SelfFunctionCall(char *n) 
{ 
   ns = NULL;
   name = n; 
   func = NULL; 
}

SelfFunctionCall::SelfFunctionCall(class NamedScope *n) 
{ 
   ns = n;
   name = NULL; 
   func = NULL; 
}

SelfFunctionCall::SelfFunctionCall(class Method *f) 
{ 
   ns = NULL;
   name = NULL;
   func = f; 
}

SelfFunctionCall::~SelfFunctionCall() 
{ 
   if (name) 
      free(name); 
   if (ns)
      delete ns;
}

inline class QoreNode *SelfFunctionCall::eval(class QoreNode *args, class ExceptionSink *xsink)
{
   class Object *self = getStackObject();
   if (func)
      return func->eval(self, args, xsink);
   // otherwise exec copy method
   return self->getClass()->execCopy(self, xsink);
}

// called at parse time
void SelfFunctionCall::resolve()
{
#ifdef DEBUG
   if (ns)
      printd(5, "SelfFunctionCall:resolve() resolving base class call '%s'\n", ns->ostr);
   else 
      printd(5, "SelfFunctionCall:resolve() resolving '%s'\n", name ? name : "(null)");
   assert(!func);
#endif
   if (name)
   {
      // FIXME: warn if argument list passed (will be ignored)

      // copy method calls will be recognized by func = NULL
      if (!strcmp(name, "copy"))
      {
	 free(name);
	 name = NULL;
	 printd(5, "SelfFunctionCall:resolve() resolved to copy constructor\n");
	 return;
      }
      func = getParseClass()->resolveSelfMethod(name);
   }
   else
      func = getParseClass()->resolveSelfMethod(ns);
   if (func)
   {
      printd(5, "SelfFunctionCall:resolve() resolved '%s' to %08p\n", func->getName(), func);
      if (name)
      {
	 free(name);
	 name = NULL;
      }
      else if (ns)
      {
	 delete ns;
	 ns = NULL;
      }
   }
}

class QoreNode *ImportedFunctionCall::eval(class QoreNode *args, class ExceptionSink *xsink)
{
   return pgm->callFunction(func, args, xsink);
}

FunctionCall::FunctionCall(class UserFunction *u, class QoreNode *a)
{
   type = FC_USER;
   f.ufunc = u;
   args = a;
}

FunctionCall::FunctionCall(class BuiltinFunction *b, class QoreNode *a)
{
   type = FC_BUILTIN;
   f.bfunc = b;
   args = a;
}

FunctionCall::FunctionCall(class QoreNode *a, char *name)
{
   printd(5, "FunctionCall::FunctionCall(a=%08p, name=%s) FC_SELF this=%08p\n", a, name, this);
   type = FC_SELF;
   f.sfunc = new SelfFunctionCall(name);
   args = a;
}

FunctionCall::FunctionCall(class QoreNode *a, class NamedScope *n)
{
   printd(5, "FunctionCall::FunctionCall(a=%08p, n=%s) FC_SELF this=%08p\n", a, n->ostr, this);
   type = FC_SELF;
   f.sfunc = new SelfFunctionCall(n);
   args = a;
}

FunctionCall::FunctionCall(class Method *m, class QoreNode *a)
{
   printd(5, "FunctionCall::FunctionCall(a=%08p, method=%08p %s) FC_SELF this=%08p\n", a, m, m->getName(), this);
   type = FC_SELF;
   f.sfunc = new SelfFunctionCall(m);
   args = a;
}

FunctionCall::FunctionCall(char *name, class QoreNode *a)
{
   type = FC_UNRESOLVED;
   f.c_str = name;
   args = a;
}

FunctionCall::FunctionCall(class QoreProgram *p, class UserFunction *u, class QoreNode *a)
{
   type = FC_IMPORTED;
   f.ifunc = new ImportedFunctionCall(p, u);
   args = a;
}

FunctionCall::~FunctionCall()
{
   printd(5, "FunctionCall::~FunctionCall(): type=%d args=%08p (%s)\n",
	  type, args, (type == FC_UNRESOLVED && f.c_str) ? f.c_str : "(null)");

   // there could be an object here in the case of a background expression
   if (args)
   {
      ExceptionSink xsink;
      args->deref(&xsink);
   }

   switch (type)
   {
      case FC_USER:
      case FC_BUILTIN:
	 break;
      case FC_SELF:
	 delete f.sfunc;
	 break;
      case FC_METHOD:
      case FC_UNRESOLVED:
	 if (f.c_str)
	    free(f.c_str);
	 break;
      case FC_IMPORTED:
	 delete f.ifunc;
	 break;
   }
}

void FunctionCall::parseMakeMethod()
{
   type = FC_METHOD;
}

// makes a "new" operator call from a function call
class QoreNode *FunctionCall::parseMakeNewObject()
{
   class QoreNode *rv = new QoreNode(new NamedScope(f.c_str), args);
   f.c_str = NULL;
   args = NULL;
   return rv;
}

class QoreNode *FunctionCall::eval(class ExceptionSink *xsink)
{
   switch (type)
   {
      case FC_USER:
	 return f.ufunc->eval(args, NULL, xsink);
      case FC_BUILTIN:
	 return f.bfunc->eval(args, xsink);
      case FC_SELF:
	 return f.sfunc->eval(args, xsink);
      case FC_IMPORTED:
	 return f.ifunc->eval(args, xsink);
   }
   return NULL;
}

int FunctionCall::existsUserParam(int i) const
{
   if (type == FC_USER)
      return f.ufunc->params->num_params > i;
   if (type == FC_IMPORTED)
      return f.ifunc->func->params->num_params > i;
   return 1;
}

int FunctionCall::getType() const
{
   return type;
}

char *FunctionCall::getName() const
{
   switch (type)
   {
      case FC_USER:
	 return f.ufunc->name;
      case FC_BUILTIN:
	 return f.bfunc->name;
      case FC_SELF:
	 return f.sfunc->name;
      case FC_IMPORTED:
	 return f.ifunc->func->name;
      case FC_UNRESOLVED:
      case FC_METHOD:
	 return f.c_str ? f.c_str : (char *)"copy";
   }
   return NULL;   
}

Paramlist::Paramlist(class QoreNode *params)
{
   ids = NULL;
   if (!params)
   {
      num_params = 0;
      names = NULL;
   }
   else if (params->type == NT_VARREF)
   {
      num_params = 1;
      names = new char *[1];
      names[0] = strdup(params->val.vref->name);
      params->deref(NULL);
   }
   else if (params->type != NT_LIST)
   {
      param_error();
      num_params = 0;
      names = NULL;
   }
   else
   {
      num_params = params->val.list->size();
      names = new char *[params->val.list->size()];
      for (int i = 0; i < params->val.list->size(); i++)
      {
         if (params->val.list->retrieve_entry(i)->type != NT_VARREF)
         {
            param_error();
            num_params = 0;
            delete [] names;
            names = NULL;
            break;
         }
         else
            names[i] = strdup(params->val.list->retrieve_entry(i)->val.vref->name);
      }
      params->deref(NULL);
   }
}

Paramlist::~Paramlist()
{
   for (int i = 0; i < num_params; i++)
      free(names[i]);
   if (names)
      delete [] names;
   if (ids)
      delete [] ids;
}

UserFunction::UserFunction(char *nme, class Paramlist *parms, class StatementBlock *b, bool synced)
{
   synchronized = synced;
   if (synced)
      gate = new VRMutex();
# ifdef DEBUG
   else
      gate = NULL;
# endif
   name = nme;
   params = parms;
   statements = b;
}

UserFunction::~UserFunction()
{
   printd(5, "UserFunction::~UserFunction() deleting %s\n", name);
   if (synchronized)
      delete gate;
   free(name);
   delete params;
   if (statements)
      delete statements;
}

void UserFunction::deref()
{
   if (ROdereference())
      delete this;
}

BuiltinFunction::BuiltinFunction(char *nme, q_func_t f, int typ)
{
   type = typ;
   name = nme;
   code.func = f;
   next = NULL;
}

BuiltinFunction::BuiltinFunction(char *nme, q_method_t m, int typ)
{
   type = typ;
   name = nme;
   code.method = m;
   next = NULL;
}

BuiltinFunction::BuiltinFunction(q_constructor_t m, int typ)
{
   type = typ;
   name = "constructor";
   code.constructor = m;
   next = NULL;
}

BuiltinFunction::BuiltinFunction(q_destructor_t m, int typ)
{
   type = typ;
   name = "destructor";
   code.destructor = m;
   next = NULL;
}

BuiltinFunction::BuiltinFunction(q_copy_t m, int typ)
{
   type = typ;
   name = "copy";
   code.copy = m;
   next = NULL;
}

void BuiltinFunction::evalConstructor(class Object *self, class QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalConstructor()");

   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);
   
   if (!xsink->isEvent())
   {
      // push call on call stack
      pushCall("constructor", CT_BUILTIN, self);

      code.constructor(self, args, xsink);

      popCall(xsink);
   }
   traceout("BuiltinFunction::evalWithArgs()");
}

void BuiltinFunction::evalSystemConstructor(class Object *self, class QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink)
{
   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);
   
   code.constructor(self, args, xsink);
}

QoreNode *BuiltinFunction::evalWithArgs(class Object *self, class QoreNode *args, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalWithArgs()");
   printd(2, "BuiltinFunction::evalWithArgs() calling builtin function \"%s\"\n", name);
   
   // push call on call stack
   pushCall(name, CT_BUILTIN, self);

   QoreNode *rv = code.func(args, xsink);

   popCall(xsink);

   traceout("BuiltinFunction::evalWithArgs()");
   return rv;
}

QoreNode *BuiltinFunction::evalMethod(class Object *self, void *private_data, class QoreNode *args, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalMethod()");
   printd(2, "BuiltinFunction::evalMethod() calling builtin function '%s' obj=%08p data=%08p\n", name, self, private_data);
   
   // push call on call stack
   pushCall(name, CT_BUILTIN, self);

   QoreNode *rv = code.method(self, private_data, args, xsink);

   popCall(xsink);

   traceout("BuiltinFunction::evalWithArgs()");
   return rv;
}

void BuiltinFunction::evalDestructor(class Object *self, void *private_data, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalDestructor()");
   
   // push call on call stack
   pushCall("destructor", CT_BUILTIN, self);

   code.destructor(self, private_data, xsink);

   popCall(xsink);

   traceout("BuiltinFunction::destructor()");
}

void BuiltinFunction::evalCopy(class Object *self, class Object *old, void *private_data, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalCopy()");
   
   // push call on call stack
   pushCall("copy", CT_BUILTIN, self);

   code.copy(self, old, private_data, xsink);

   popCall(xsink);

   traceout("BuiltinFunction::evalCopy()");
}

void BuiltinFunction::evalSystemDestructor(class Object *self, void *private_data, class ExceptionSink *xsink)
{
   code.destructor(self, private_data, xsink);
}

QoreNode *BuiltinFunction::eval(QoreNode *args, ExceptionSink *xsink)
{
   class QoreNode *tmp, *rv;
   ExceptionSink newsink;

   tracein("BuiltinFunction::eval(Node)");
   printd(3, "BuiltinFunction::eval(Node) calling builtin function \"%s\"\n", name);
   
   //printd(5, "BuiltinFunction::eval(Node) args=%08p %s\n", args, args ? args->type->getName() : "(null)");

   if (args)
      tmp = args->eval(&newsink);
   else
      tmp = NULL;

   //printd(5, "BuiltinFunction::eval(Node) after eval tmp args=%08p %s\n", tmp, tmp ? tmp->type->getName() : "(null)");

   // push call on call stack
   pushCall(name, CT_BUILTIN);

   // execute the function if no new exception has happened
   // necessary only in the case of a builtin object destructor
   if (!newsink.isEvent())
      rv = code.func(tmp, xsink);
   else
      rv = NULL;

   xsink->assimilate(&newsink);

   // pop call off call stack
   popCall(xsink);

   discard(tmp, xsink);

   traceout("BuiltinFunction::eval(Node)");
   return rv;
}

// calls a user function
class QoreNode *UserFunction::eval(QoreNode *args, Object *self, class ExceptionSink *xsink)
{
   tracein("UserFunction::eval()");
   printd(2, "UserFunction::eval(): function='%s' args=%08p (size=%d)\n", 
          name, args, args ? args->val.list->size() : 0);

   int i = 0;
   class QoreNode *val = NULL;
   int num_args, num_params;

   if (args)
      num_args = args->val.list->size();
   else
      num_args = 0;

   // instantiate local vars from param list
   num_params = params->num_params;
   for (i = 0; i < num_params; i++)
   {
      QoreNode *n = args ? args->val.list->retrieve_entry(i) : NULL;
      printd(4, "UserFunction::eval() %d: instantiating param lvar %d (%08p %s)\n", i, params->ids[i], n, n ? n->type->getName() : "(null)");
      if (n)
      {
         if (n->type == NT_REFERENCE)
         {
	    bool is_self_ref = false;
            n = doPartialEval(n->val.lvexp, &is_self_ref, xsink);
	    //printd(5, "UserFunction::eval() ref self_ref=%d, self=%08p (%s) so=%08p (%s)\n", is_self_ref, self, self ? self->getClass()->name : "NULL", getStackObject(), getStackObject() ? getStackObject()->getClass()->name : "NULL");
            if (!xsink->isEvent())
	       instantiateLVar(params->ids[i], n, is_self_ref ? getStackObject() : NULL);
         }
         else
         {
            n = n->eval(xsink);
	    if (!xsink->isEvent())
	       instantiateLVar(params->ids[i], n);
         }
	 // the above if block will only instantiate the local variable if no
	 // exceptions have occurred. therefore here we do the cleanup the rest
	 // of any already instantiated local variables if an exception does occur
         if (xsink->isEvent())
         {
            if (n)
               n->deref(xsink);
            for (int j = i; j; j--)
               uninstantiateLVar(xsink);
            return NULL;
         }
      }
      else
         instantiateLVar(params->ids[i], NULL);
   }

   // if there are more arguments than parameters
   printd(5, "UserFunction::eval() params=%d arg=%d\n", num_params, num_args);
   class QoreNode *argv;
   
   if (num_params < num_args)
   {
      List *l = new List();
      
      for (i = 0; i < (num_args - num_params); i++)
         if (args->val.list->retrieve_entry(i + num_params))
         {
            QoreNode *v = args->val.list->eval_entry(i + num_params, xsink);
            if (xsink->isEvent())
            {
	       if (v)
		  v->deref(xsink);
               l->derefAndDelete(xsink);
               // uninstantiate local vars from param list
               for (int j = 0; j < num_params; j++)
                  uninstantiateLVar(xsink);
               return NULL;
            }
            l->push(v);
         }
         else
            l->push(NULL);
      argv = new QoreNode(l);
   }
   else
      argv = NULL;

   if (statements)
   {
      pushCall(name, CT_USER, self);

      // push call on stack
      if (self)
         self->instantiateLVar(params->selfid);
   
      // instantiate argv and push id on stack
      instantiateLVar(params->argvid, argv);
      push_argv(params->argvid);

      // enter gate if necessary
      if (synchronized)
         gate->enter();

      // execute function
      val = statements->exec(xsink);

      // exit gate if necessary
      if (synchronized)
	 gate->exit();

      // pop argv from stack and uninstantiate
      pop_argv();
      uninstantiateLVar(xsink);

      // if self then uninstantiate
      if (self)
	 self->uninstantiateLVar(xsink);

      popCall(xsink);   
   }
   else
      discard(argv, xsink);

   if (num_params)
   {
      printd(5, "UserFunction::eval() about to uninstantiate %d vars\n", 
	     params->num_params);

      // uninstantiate local vars from param list
      for (i = 0; i < num_params; i++)
	 uninstantiateLVar(xsink);
   }
   traceout("UserFunction::eval()");
   return val;
}

// this function will set up user copy constructor calls
void UserFunction::evalCopy(Object *oold, Object *self, ExceptionSink *xsink)
{
   tracein("UserFunction::evalCopy()");
   printd(2, "UserFunction::evalCopy(): function='%s', num_params=%d, oldobj=%08p\n", name, params->num_params, oold);

   // create QoreNode for "old" for either param or argv list
   oold->ref();
   QoreNode *old = new QoreNode(oold);

   // instantiate local vars from param list
   for (int i = 0; i < params->num_params; i++)
   {
      class QoreNode *n = (i ? NULL : old);
      printd(5, "UserFunction::evalCopy(): instantiating param lvar %d (%08p)\n", i, params->ids[i], n);
      instantiateLVar(params->ids[i], n);
   }

   class QoreNode *argv;

   if (!params->num_params)
   {
      List *l = new List();
      l->push(old);
      argv = new QoreNode(l);
   }
   else
      argv = NULL;

   if (statements)
   {
      // push call on stack
      pushCall(name, CT_USER, self);

      // instantiate self
      self->instantiateLVar(params->selfid);
   
      // instantiate argv and push id on stack (for shift)
      instantiateLVar(params->argvid, argv);
      push_argv(params->argvid);
   
      // execute function
      discard(statements->exec(xsink), xsink);

      // pop argv from stack
      pop_argv();
      uninstantiateLVar(xsink);
   
      // uninstantiate self
      self->uninstantiateLVar(xsink);

      popCall(xsink);
   }
   else
      discard(argv, xsink);

   if (params->num_params)
   {
      printd(5, "UserFunction::evalCopy() about to uninstantiate %d vars\n", 
	     params->num_params);

      // uninstantiate local vars from param list
      for (int i = 0; i < params->num_params; i++)
	 uninstantiateLVar(xsink);
   }
   traceout("UserFunction::evalCopy()");
}

// calls a user constructor method
class QoreNode *UserFunction::evalConstructor(QoreNode *args, Object *self, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink)
{
   tracein("UserFunction::evalConstructor()");
   printd(2, "UserFunction::evalConstructor(): method='%s:%s' args=%08p (size=%d)\n", 
          self->getClass()->getName(), name, args, args ? args->val.list->size() : 0);

   int i = 0;
   class QoreNode *val = NULL;
   int num_args, num_params;

   if (args)
      num_args = args->val.list->size();
   else
      num_args = 0;

   // instantiate local vars from param list
   num_params = params->num_params;
   for (i = 0; i < num_params; i++)
   {
      QoreNode *n = args ? args->val.list->retrieve_entry(i) : NULL;
      printd(4, "UserFunction::evalConstructor() %d: instantiating param lvar %d (%08p)\n", i, params->ids[i], n);
      if (n)
      {
         if (n->type == NT_REFERENCE)
         {
	    bool is_self_ref = false;
            n = doPartialEval(n->val.lvexp, &is_self_ref, xsink);
            if (!xsink->isEvent())
	       instantiateLVar(params->ids[i], n, is_self_ref ? getStackObject() : NULL);
         }
         else
         {
            n = n->eval(xsink);
	    if (!xsink->isEvent())
	       instantiateLVar(params->ids[i], n);
         }
	 // the above if block will only instantiate the local variable if no
	 // exceptions have occurred. therefore here we do the cleanup the rest
	 // of any already instantiated local variables if an exception does occur
         if (xsink->isEvent())
         {
            if (n)
               n->deref(xsink);
            for (int j = i; j; j--)
               uninstantiateLVar(xsink);
	    traceout("UserFunction::evalConstructor()");
            return NULL;
         }
      }
      else
         instantiateLVar(params->ids[i], NULL);
   }

   // if there are more arguments than parameters
   printd(5, "UserFunction::evalConstructor() params=%d arg=%d\n", num_params, num_args);
   class QoreNode *argv;
   
   if (num_params < num_args)
   {
      List *l = new List();
      
      for (i = 0; i < (num_args - num_params); i++)
         if (args->val.list->retrieve_entry(i + num_params))
         {
            QoreNode *v = args->val.list->eval_entry(i + num_params, xsink);
            if (xsink->isEvent())
            {
	       if (v)
		  v->deref(xsink);
               l->derefAndDelete(xsink);
               // uninstantiate local vars from param list
               for (int j = 0; j < num_params; j++)
                  uninstantiateLVar(xsink);
               return NULL;
            }
            l->push(v);
         }
         else
            l->push(NULL);
      argv = new QoreNode(l);
   }
   else
      argv = NULL;

   // evaluate base class constructors (if any)
   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);

   if (!xsink->isEvent())
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

      // execute constructor
      if (statements)
      {
	 // push call on stack
	 pushCall(name, CT_USER, self);

	 // instantiate "$self" variable
	 self->instantiateLVar(params->selfid);
	 
	 // instantiate argv and push id on stack
	 instantiateLVar(params->argvid, argv);
	 push_argv(params->argvid);
	 
	 // enter gate if necessary
	 if (synchronized)
	    gate->enter();
	 
	 // execute function
	 val = statements->exec(xsink);
	 
	 // exit gate if necessary
	 if (synchronized)
	    gate->exit();
	 
	 // pop argv from stack and uninstantiate
	 pop_argv();
	 uninstantiateLVar(xsink);
	    
	 // uninstantiate "$self" variable
	 self->uninstantiateLVar(xsink);
	 
	 popCall(xsink);   
      }
      else
	 discard(argv, xsink);
      
      // switch back to original program if necessary
      if (opgm && cpgm != opgm)
	 popProgram();
   }

   if (num_params)
   {
      printd(5, "UserFunction::evalConstructor() about to uninstantiate %d vars\n", params->num_params);

      // uninstantiate local vars from param list
      for (i = 0; i < num_params; i++)
	 uninstantiateLVar(xsink);
   }
   traceout("UserFunction::evalConstructor()");
   return val;
}

// this will only be called with lvalue expressions
class QoreNode *doPartialEval(class QoreNode *n, bool *is_self_ref, class ExceptionSink *xsink)
{
   QoreNode *rv;
   if (n->type == NT_TREE)
   {
      class QoreNode *nn = n->val.tree->right->eval(xsink);
      if (xsink->isEvent())
      {
	 discard(nn, xsink);
	 return NULL;
      }
      rv = new QoreNode(NT_TREE);
      rv->val.tree->right = nn ? nn : nothing();
      rv->val.tree->op = n->val.tree->op;
      if (!(rv->val.tree->left = doPartialEval(n->val.tree->left, is_self_ref, xsink)))
      {
	 rv->deref(xsink);
	 rv = NULL;
      }
   }
   else
   {
      rv = n->RefSelf();
      if (n->type == NT_SELF_VARREF)
	 (*is_self_ref) = true;
   }
   return rv;
}
