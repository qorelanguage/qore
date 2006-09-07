/*
  Function.h

  Qore programming language

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

#ifndef _QORE_FUNCTION_H

#define _QORE_FUNCTION_H

#include <qore/ReferenceObject.h>
#include <qore/Restrictions.h>
#include <qore/LockedObject.h>
#include <qore/common.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_node(FILE *fp, class QoreNode *node);
class QoreNode *doPartialEval(class QoreNode *n, bool *is_self_ref, class ExceptionSink *xsink);

// prototypes for inlined functions in this header file
static inline void push_argv(lvh_t argvid);
static inline void pop_argv();

class SelfFunctionCall {
  public:
      char *name;
      class NamedScope *ns;
      class Method *func;

      inline SelfFunctionCall(char *n) 
      { 
	 ns = NULL;
	 name = n; 
	 func = NULL; 
      }

      inline SelfFunctionCall(class NamedScope *n) 
      { 
	 ns = n;
	 name = NULL; 
	 func = NULL; 
      }

      inline SelfFunctionCall(class Method *f) 
      { 
	 ns = NULL;
	 name = NULL;
	 func = f; 
      }

      inline ~SelfFunctionCall();

      inline class QoreNode *eval(class QoreNode *args, class ExceptionSink *xsink);

      inline void resolve();
};

class ImportedFunctionCall {
   public:
      class QoreProgram *pgm;
      class UserFunction *func;
      ImportedFunctionCall(class QoreProgram *p, class UserFunction *f) { pgm = p; func = f; }
      inline class QoreNode *eval(class QoreNode *args, class ExceptionSink *xsink);
};

// constructors and destructors can never be explicitly called so we don't need FunctionCall constants for them
#define FC_UNRESOLVED     1
#define FC_USER           2
#define FC_BUILTIN        3
#define FC_SELF           4
#define FC_IMPORTED       5
#define FC_METHOD         6

class FunctionCall {
   public:
      union uFCall {
	    class UserFunction *ufunc;
	    class BuiltinFunction *bfunc;
	    class SelfFunctionCall *sfunc;
	    class ImportedFunctionCall *ifunc;
	    char *c_str;
      } f;
      class QoreNode *args;
      int type;
      inline FunctionCall(class UserFunction *u, class QoreNode *a);
      inline FunctionCall(class BuiltinFunction *b, class QoreNode *a);

      // "self" in-object function call constructors
      inline FunctionCall(class QoreNode *a, char *name);
      inline FunctionCall(class QoreNode *a, class NamedScope *n);
      inline FunctionCall(class Method *func, class QoreNode *a);

      // normal function call constructor
      inline FunctionCall(char *name, class QoreNode *a);

      inline FunctionCall(class QoreProgram *p, class UserFunction *u, class QoreNode *a);
      inline ~FunctionCall();
      inline class QoreNode *eval(class ExceptionSink *);
      inline int existsUserParam(int i);
      inline char *getName();
};

// object definitions and interfaces
class BuiltinFunction
{
   private:
      int type;

   public:
      char *name;
      class BuiltinFunction *next;
      union {
	    q_func_t func;
	    q_method_t method;
	    q_constructor_t constructor;
	    q_destructor_t destructor;
	    q_copy_t copy;
      } code;

      inline BuiltinFunction(char *nme, q_func_t f, int typ);
      inline BuiltinFunction(char *nme, q_method_t m, int typ);
      inline BuiltinFunction(q_constructor_t m, int typ);
      inline BuiltinFunction(q_destructor_t m, int typ);
      inline BuiltinFunction(q_copy_t m, int typ);
      //inline class QoreNode *evalSystemMethod(class Object *self, class QoreNode *args, class ExceptionSink *xsink);
      inline class QoreNode *evalMethod(class Object *self, void *private_data, class QoreNode *args, class ExceptionSink *xsink);
      inline void evalConstructor(class Object *self, class QoreNode *args, class ExceptionSink *xsink);
      inline void evalDestructor(class Object *self, void *private_data, class ExceptionSink *xsink);
      inline void evalCopy(class Object *self, class Object *old, void *private_data, class ExceptionSink *xsink);
      inline void evalSystemConstructor(class Object *self, class QoreNode *args, class ExceptionSink *xsink);
      inline void evalSystemDestructor(class Object *self, void *private_data, class ExceptionSink *xsink);
      inline class QoreNode *evalWithArgs(class Object *self, class QoreNode *args, class ExceptionSink *xsink);
      inline class QoreNode *eval(class QoreNode *args, class ExceptionSink *xsink);
      inline int getType() { return type; }
};

class Paramlist {
   public:
      int num_params;
      char **names;
      lvh_t *ids;
      lvh_t argvid;
      lvh_t selfid;

      Paramlist(class QoreNode *params);
      inline ~Paramlist();
};

class UserFunction : public ReferenceObject
{
   private:
      int synchronized;
      // for "synchronized" functions
      class RMutex *gate;

   protected:
      inline ~UserFunction();

   public:
      char *name;
      class UserFunction *next;
      class Paramlist *params;
      class StatementBlock *statements;

      inline UserFunction(char *nme, class Paramlist *parms, class StatementBlock *states, int synced = 0);
      class QoreNode *eval(class QoreNode *args, class Object *self, class ExceptionSink *xsink);
      class QoreNode *evalConstructor(class QoreNode *args, class Object *self, class BCList *bcl, class BCEAList *scbceal, class ExceptionSink *xsink);
      void evalCopy(class Object *old, class Object *self, class ExceptionSink *xsink);
      inline void deref();
};

#include <qore/common.h>
#include <qore/QoreNode.h>
#include <qore/support.h>
#include <qore/Variable.h>
#include <qore/Statement.h>
#include <qore/common.h>
#include <qore/thread.h>
#include <qore/ArgvStack.h>
#include <qore/QoreType.h>
#include <qore/Exception.h>
#include <qore/QoreProgram.h>
#include <qore/RMutex.h>
#include <qore/Operator.h>
#include <qore/Object.h>
#include <qore/NamedScope.h>

inline SelfFunctionCall::~SelfFunctionCall() 
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
inline void SelfFunctionCall::resolve()
{
#ifdef DEBUG
   if (ns)
      printd(5, "SelfFunctionCall:resolve() resolving base class call '%s'\n", ns->ostr);
   else 
      printd(5, "SelfFunctionCall:resolve() resolving '%s'\n", name ? name : "(null)");
   if (func)
      run_time_error("SelfFunctionCall:resolve() already resolved %s (%08p)", func->name, func);
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
      printd(5, "SelfFunctionCall:resolve() resolved '%s' to %08p\n", func->name, func);
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

inline class QoreNode *ImportedFunctionCall::eval(class QoreNode *args, class ExceptionSink *xsink)
{
   return pgm->callFunction(func, args, xsink);
}

inline FunctionCall::FunctionCall(class UserFunction *u, class QoreNode *a)
{
   type = FC_USER;
   f.ufunc = u;
   args = a;
}

inline FunctionCall::FunctionCall(class BuiltinFunction *b, class QoreNode *a)
{
   type = FC_BUILTIN;
   f.bfunc = b;
   args = a;
}

inline FunctionCall::FunctionCall(class QoreNode *a, char *name)
{
   printd(5, "FunctionCall::FunctionCall(a=%08p, name=%s) FC_SELF this=%08p\n", a, name, this);
   type = FC_SELF;
   f.sfunc = new SelfFunctionCall(name);
   args = a;
}

inline FunctionCall::FunctionCall(class QoreNode *a, class NamedScope *n)
{
   printd(5, "FunctionCall::FunctionCall(a=%08p, n=%s) FC_SELF this=%08p\n", a, n->ostr, this);
   type = FC_SELF;
   f.sfunc = new SelfFunctionCall(n);
   args = a;
}

inline FunctionCall::FunctionCall(class Method *m, class QoreNode *a)
{
   printd(5, "FunctionCall::FunctionCall(a=%08p, method=%08p %s) FC_SELF this=%08p\n", a, m, m->name, this);
   type = FC_SELF;
   f.sfunc = new SelfFunctionCall(m);
   args = a;
}

inline FunctionCall::FunctionCall(char *name, class QoreNode *a)
{
   type = FC_UNRESOLVED;
   f.c_str = name;
   args = a;
}

inline FunctionCall::FunctionCall(class QoreProgram *p, class UserFunction *u, class QoreNode *a)
{
   type = FC_IMPORTED;
   f.ifunc = new ImportedFunctionCall(p, u);
   args = a;
}

inline FunctionCall::~FunctionCall()
{
   printd(5, "FunctionCall::~FunctionCall(): type=%d args=%08p (%s)\n",
	  type, args, (type == FC_UNRESOLVED && f.c_str) ? f.c_str : "(null)");
   // there could be object here in the case of a background expression

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

inline class QoreNode *FunctionCall::eval(class ExceptionSink *xsink)
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

inline int FunctionCall::existsUserParam(int i)
{
   if (type == FC_USER)
      return f.ufunc->params->num_params > i;
   if (type == FC_IMPORTED)
      return f.ifunc->func->params->num_params > i;
   return 1;
}

inline char *FunctionCall::getName()
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

inline BuiltinFunction::BuiltinFunction(char *nme, q_func_t f, int typ)
{
   type = typ;
   name = nme;
   code.func = f;
   next = NULL;
}

inline BuiltinFunction::BuiltinFunction(char *nme, q_method_t m, int typ)
{
   type = typ;
   name = nme;
   code.method = m;
   next = NULL;
}

inline BuiltinFunction::BuiltinFunction(q_constructor_t m, int typ)
{
   type = typ;
   name = "constructor";
   code.constructor = m;
   next = NULL;
}

inline BuiltinFunction::BuiltinFunction(q_destructor_t m, int typ)
{
   type = typ;
   name = "destructor";
   code.destructor = m;
   next = NULL;
}

inline BuiltinFunction::BuiltinFunction(q_copy_t m, int typ)
{
   type = typ;
   name = "copy";
   code.copy = m;
   next = NULL;
}

/*
inline QoreNode *BuiltinFunction::evalSystemMethod(class Object *self, class QoreNode *args, class ExceptionSink *xsink)
{
   return code.method(self, args, xsink);
}
*/

inline QoreNode *BuiltinFunction::evalWithArgs(class Object *self, class QoreNode *args, class ExceptionSink *xsink)
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

inline QoreNode *BuiltinFunction::evalMethod(class Object *self, void *private_data, class QoreNode *args, class ExceptionSink *xsink)
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

inline void BuiltinFunction::evalConstructor(class Object *self, class QoreNode *args, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalConstructor()");
   
   // push call on call stack
   pushCall("constructor", CT_BUILTIN, self);

   code.constructor(self, args, xsink);

   popCall(xsink);

   traceout("BuiltinFunction::evalWithArgs()");
}

inline void BuiltinFunction::evalDestructor(class Object *self, void *private_data, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalDestructor()");
   
   // push call on call stack
   pushCall("destructor", CT_BUILTIN, self);

   code.destructor(self, private_data, xsink);

   popCall(xsink);

   traceout("BuiltinFunction::destructor()");
}

inline void BuiltinFunction::evalCopy(class Object *self, class Object *old, void *private_data, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalCopy()");
   
   // push call on call stack
   pushCall("copy", CT_BUILTIN, self);

   code.copy(self, old, private_data, xsink);

   popCall(xsink);

   traceout("BuiltinFunction::evalCopy()");
}

inline void BuiltinFunction::evalSystemConstructor(class Object *self, class QoreNode *args, class ExceptionSink *xsink)
{
   code.constructor(self, args, xsink);
}

inline void BuiltinFunction::evalSystemDestructor(class Object *self, void *private_data, class ExceptionSink *xsink)
{
   code.destructor(self, private_data, xsink);
}

inline QoreNode *BuiltinFunction::eval(QoreNode *args, ExceptionSink *xsink)
{
   class QoreNode *tmp, *rv;
   ExceptionSink newsink;

   tracein("BuiltinFunction::eval(Node)");
   printd(3, "BuiltinFunction::eval(Node) calling builtin function \"%s\"\n", name);
   
   //printd(5, "BuiltinFunction::eval(Node) args=%08p %s\n", args, args ? args->type->name : "(null)");

   if (args)
      tmp = args->eval(&newsink);
   else
      tmp = NULL;

   //printd(5, "BuiltinFunction::eval(Node) after eval tmp args=%08p %s\n", tmp, tmp ? tmp->type->name : "(null)");

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

static inline void param_error()
{
   parse_error("parameter list contains non-variable reference expressions.");
}

inline Paramlist::~Paramlist()
{
   for (int i = 0; i < num_params; i++)
      free(names[i]);
   if (names)
      delete [] names;
   if (ids)
      delete [] ids;
}

inline UserFunction::UserFunction(char *nme, class Paramlist *parms, class StatementBlock *b, int synced)
{
   next = NULL;
   synchronized = synced;
   if (synced)
      gate = new RMutex();
# ifdef DEBUG
   else
      gate = NULL;
# endif
   name = nme;
   params = parms;
   statements = b;
}

inline UserFunction::~UserFunction()
{
   printd(5, "UserFunction::~UserFunction() deleting %s\n", name);
   if (synchronized)
      delete gate;
   free(name);
   delete params;
   if (statements)
      delete statements;
}

inline void UserFunction::deref()
{
   if (ROdereference())
      delete this;
}

static inline class QoreNode *splice_expressions(class QoreNode *a1, class QoreNode *a2)
{
   //tracein("splice_expressions()");
   if (a1->type == NT_LIST)
   {
      //printd(5, "LIST x\n");
      a1->val.list->push(a2);
      return a1;
   }
   //printd(5, "NODE x\n");
   class QoreNode *nl = new QoreNode(NT_LIST);
   nl->val.list = new List(1);
   nl->val.list->push(a1);
   nl->val.list->push(a2);
   //traceout("splice_expressions()");
   return nl;
}

#endif // _QORE_FUNCTION_H
