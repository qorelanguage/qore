/*
  Function.cc

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
#include <qore/intern/QoreClassIntern.h>

#include <stdio.h>
#include <ctype.h>
#include <assert.h>

static inline void param_error()
{
   parse_error("parameter list contains non-variable reference expressions.");
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

SelfFunctionCall::SelfFunctionCall(const QoreMethod *f) 
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

char *SelfFunctionCall::takeName()
{
   char *n = name;
   name = 0;
   return n;
}

class NamedScope *SelfFunctionCall::takeNScope()
{
   NamedScope *rns = ns;
   ns = 0;
   return rns;
}

AbstractQoreNode *SelfFunctionCall::eval(const QoreListNode *args, ExceptionSink *xsink) const
{
   QoreObject *self = getStackObject();
   
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

AbstractQoreNode *ImportedFunctionCall::eval(const QoreListNode *args, ExceptionSink *xsink) const
{
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
   
   AbstractQoreNode *rv = pgm->callFunction(func, args, xsink);

   if (xsink->isException())
      xsink->addStackInfo(CT_USER, NULL, func->getName(), o_fn, o_ln, o_eln);
   
   return rv;
}


Paramlist::Paramlist(AbstractQoreNode *params)
{
   ReferenceHolder<AbstractQoreNode> param_holder(params, 0);

   ids = NULL;
   if (!params)
   {
      num_params = 0;
      names = NULL;
      return;
   }

   if (params->type == NT_VARREF) {
      num_params = 1;
      names = new char *[1];
      names[0] = strdup(reinterpret_cast<const VarRefNode *>(params)->name);
      return;
   }

   if (params->type != NT_LIST) {
      param_error();
      num_params = 0;
      names = NULL;
      return;
   }

   const QoreListNode *l = reinterpret_cast<const QoreListNode *>(params);

   num_params = l->size();
   names = new char *[num_params];
   for (int i = 0; i < num_params; i++)
   {
      if (l->retrieve_entry(i)->type != NT_VARREF)
      {
	 param_error();
	 num_params = 0;
	 delete [] names;
	 names = NULL;
	 break;
      }
      else
	 names[i] = strdup(reinterpret_cast<const VarRefNode *>(l->retrieve_entry(i))->name);
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
   delete params;
   if (statements)
      delete statements;
   free(name);
}

void UserFunction::deref()
{
   if (ROdereference())
      delete this;
}

BuiltinFunction::BuiltinFunction(const char *nme, q_func_t f, int typ)
{
   type = typ;
   name = nme;
   code.func = f;
   next = NULL;
}

BuiltinFunction::BuiltinFunction(const char *nme, q_method_t m, int typ)
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

void BuiltinFunction::evalConstructor(QoreObject *self, const QoreListNode *args, class BCList *bcl, class BCEAList *bceal, const char *class_name, ExceptionSink *xsink) const
{
   tracein("BuiltinFunction::evalConstructor()");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   CodeContextHelper cch("constructor", self, xsink);

#ifdef DEBUG
   // push call on call stack
   CallStackHelper csh("constructor", CT_BUILTIN, self, xsink);
#endif

   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);
   
   if (!xsink->isEvent())
   {
      code.constructor(self, args, xsink);
      if (xsink->isException())
	 xsink->addStackInfo(CT_BUILTIN, class_name, "constructor", o_fn, o_ln, o_eln);
   }
   
   traceout("BuiltinFunction::evalConstructor()");
}

void BuiltinFunction::evalSystemConstructor(QoreObject *self, const QoreListNode *args, class BCList *bcl, class BCEAList *bceal, ExceptionSink *xsink) const
{
   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);
   
   code.constructor(self, args, xsink);
}

AbstractQoreNode *BuiltinFunction::evalWithArgs(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const
{
   tracein("BuiltinFunction::evalWithArgs()");
   printd(2, "BuiltinFunction::evalWithArgs() calling builtin function \"%s\"\n", name);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   AbstractQoreNode *rv;
   {
      CodeContextHelper cch(name, self, xsink);

#ifdef DEBUG
      // push call on call stac
      CallStackHelper csh(name, CT_BUILTIN, self, xsink);
#endif

      rv = code.func(args, xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, self ? self->getClass()->getName() : NULL, name, o_fn, o_ln, o_eln);

   traceout("BuiltinFunction::evalWithArgs()");
   return rv;
}

AbstractQoreNode *BuiltinFunction::evalMethod(QoreObject *self, void *private_data, const QoreListNode *args, ExceptionSink *xsink) const
{
   printd(2, "BuiltinFunction::evalMethod() calling builtin function '%s' obj=%08p data=%08p\n", name, self, private_data);
   CodeContextHelper cch(name, self, xsink);
#ifdef DEBUG
   // push call on call stack in debugging mode
   CallStackHelper csh(name, CT_BUILTIN, self, xsink);
#endif

   // note: exception stack trace is added at the level above
   return code.method(self, private_data, args, xsink);
}

void BuiltinFunction::evalDestructor(QoreObject *self, void *private_data, const char *class_name, ExceptionSink *xsink) const
{
   tracein("BuiltinFunction::evalDestructor()");
   
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   {
      CodeContextHelper cch("destructor", self, xsink);
#ifdef DEBUG
      // push call on call stack
      CallStackHelper csh("destructor", CT_BUILTIN, self, xsink);
#endif

      code.destructor(self, private_data, xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, class_name, "destructor", o_fn, o_ln, o_eln);
   
   traceout("BuiltinFunction::destructor()");
}

void BuiltinFunction::evalCopy(QoreObject *self, QoreObject *old, void *private_data, const char *class_name, ExceptionSink *xsink) const
{
   tracein("BuiltinFunction::evalCopy()");
   
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
   
   {
      CodeContextHelper cch("copy", self, xsink);
#ifdef DEBUG
      // push call on call stack
      CallStackHelper csh("copy", CT_BUILTIN, self, xsink);
#endif

      code.copy(self, old, private_data, xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, class_name, "copy", o_fn, o_ln, o_eln);
   
   traceout("BuiltinFunction::evalCopy()");
}

void BuiltinFunction::evalSystemDestructor(QoreObject *self, void *private_data, ExceptionSink *xsink) const
{
   code.destructor(self, private_data, xsink);
}

AbstractQoreNode *BuiltinFunction::eval(const QoreListNode *args, ExceptionSink *xsink) const
{
   AbstractQoreNode *rv;
   ExceptionSink newsink;

   tracein("BuiltinFunction::eval(Node)");
   printd(3, "BuiltinFunction::eval(Node) calling builtin function \"%s\"\n", name);
   
   //printd(5, "BuiltinFunction::eval(Node) args=%08p %s\n", args, args ? args->getTypeName() : "(null)");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   QoreListNodeEvalOptionalRefHolder tmp(args, xsink);

   //printd(5, "BuiltinFunction::eval(Node) after eval tmp args=%08p %s\n", *tmp, *tmp ? *tmp->getTypeName() : "(null)");

   {
      CodeContextHelper cch(name, NULL, xsink);
#ifdef DEBUG
      // push call on call stack
      CallStackHelper csh(name, CT_BUILTIN, 0, xsink);
#endif

      // execute the function if no new exception has happened
      // necessary only in the case of a builtin object destructor
      if (!newsink.isEvent())
	 rv = code.func(*tmp, xsink);
      else
	 rv = NULL;

      xsink->assimilate(&newsink);
   }

   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, NULL, name, o_fn, o_ln, o_eln);
   
   traceout("BuiltinFunction::eval(Node)");
   return rv;
}

// calls a user function
AbstractQoreNode *UserFunction::eval(const QoreListNode *args, QoreObject *self, ExceptionSink *xsink, const char *class_name) const
{
   tracein("UserFunction::eval()");
   printd(2, "UserFunction::eval(): function='%s' args=%08p (size=%d)\n", name, args, args ? args->size() : 0);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   int num_args = args ? args->size() : 0;
   // instantiate local vars from param list
   int num_params = params->num_params;
   
   ReferenceHolder<QoreListNode> argv(xsink);

   for (int i = 0; i < num_params; i++) {
      AbstractQoreNode *n = args ? const_cast<AbstractQoreNode *>(args->retrieve_entry(i)) : NULL;
      printd(4, "UserFunction::eval() eval %d: instantiating param lvar %s (id=%08p) (n=%08p %s)\n", i, params->ids[i], params->ids[i], n, n ? n->getTypeName() : "(null)");
      if (n)
      {
	 if (n->type == NT_REFERENCE) {
	    const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(n);
	    bool is_self_ref = false;
	    n = doPartialEval(r->lvexp, &is_self_ref, xsink);
	    //printd(5, "UserFunction::eval() ref self_ref=%d, self=%08p (%s) so=%08p (%s)\n", is_self_ref, self, self ? self->getClass()->name : "NULL", getStackObject(), getStackObject() ? getStackObject()->getClass()->name : "NULL");
	    if (!*xsink)
	       thread_instantiate_lvar(params->ids[i], n, is_self_ref ? getStackObject() : 0);
	 }
	 else
	 {
	    n = n->eval(xsink);
	    if (!xsink->isEvent())
	       thread_instantiate_lvar(params->ids[i], n);
	 }
	 // the above if block will only instantiate the local variable if no
	 // exceptions have occurred. therefore here we do the cleanup the rest
	 // of any already instantiated local variables if an exception does occur
	 if (*xsink)
	 {
	    if (n)
	       n->deref(xsink);
	    while (i--)
	       thread_uninstantiate_lvar(xsink);
	    return 0;
	 }
      }
      else
	 thread_instantiate_lvar(params->ids[i], 0);
   }

   // if there are more arguments than parameters
   printd(5, "UserFunction::eval() params=%d arg=%d\n", num_params, num_args);
   
   if (num_params < num_args)
   {
      argv = new QoreListNode();
      
      for (int i = 0; i < (num_args - num_params); i++) {
	 AbstractQoreNode *v = args->eval_entry(i + num_params, xsink);
	 argv->push(v);
	 if (*xsink) {
	    // uninstantiate local vars from param list
	    for (int j = 0; j < num_params; j++)
	       thread_uninstantiate_lvar(xsink);
	    return 0;
	 }
      }
   }

   AbstractQoreNode *val = 0;
   if (statements)
   {
      CodeContextHelper cch(name, self, xsink);

#ifdef DEBUG
      // push call on stack
      CallStackHelper csh(name, CT_USER, self, xsink);
#endif

      if (self)
         self->instantiateLVar(params->selfid);
   
      // instantiate argv and push id on stack
      thread_instantiate_lvar(params->argvid, argv.release());

      {
	 ArgvContextHelper(params->argvid);

	 // enter gate if necessary
	 if (!synchronized || (gate->enter(xsink) >= 0))
	 {
	    // execute function
	    val = statements->exec(xsink);

	    // exit gate if necessary
	    if (synchronized)
	       gate->exit();
	 }
      }

      // uninstantiate argv
      thread_uninstantiate_lvar(xsink);
	 
      // if self then uninstantiate
      if (self)
	 self->uninstantiateLVar(xsink);
   }
   else
      argv = 0; // dereference argv now

   if (num_params) {
      printd(5, "UserFunction::eval() about to uninstantiate %d vars\n", params->num_params);

      // uninstantiate local vars from param list
      for (int i = 0; i < num_params; i++)
	 thread_uninstantiate_lvar(xsink);
   }
   if (xsink->isException())
      xsink->addStackInfo(CT_USER, self ? (class_name ? class_name : self->getClass()->getName()) : NULL, name, o_fn, o_ln, o_eln);
   
   return val;
}

// this function will set up user copy constructor calls
void UserFunction::evalCopy(QoreObject *old, QoreObject *self, const char *class_name, ExceptionSink *xsink) const
{
   tracein("UserFunction::evalCopy()");
   printd(2, "UserFunction::evalCopy(): function='%s', num_params=%d, oldobj=%08p\n", name, params->num_params, old);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   // instantiate local vars from param list
   for (int i = 0; i < params->num_params; i++)
   {
      QoreObject *n = (i ? NULL : old);
      printd(5, "UserFunction::evalCopy(): instantiating param lvar %d (%08p)\n", i, params->ids[i], n);
      instantiateLVar(params->ids[i], n ? n->refSelf() : 0);
   }

   QoreListNode *argv;

   if (!params->num_params)
   {
      argv = new QoreListNode();
      old->ref();
      argv->push(old);
   }
   else
      argv = NULL;

   if (statements)
   {
      CodeContextHelper cch(name, self, xsink);
#ifdef DEBUG
      // push call on stack
      CallStackHelper csh(name, CT_USER, self, xsink);
#endif

      // instantiate self
      self->instantiateLVar(params->selfid);
   
      // instantiate argv and push id on stack (for shift)
      instantiateLVar(params->argvid, argv);
      {
	 ArgvContextHelper(params->argvid);
	 // execute function
	 discard(statements->exec(xsink), xsink);
      }
      uninstantiateLVar(xsink);
      
      // uninstantiate self
      self->uninstantiateLVar(xsink);
   }
   else
      discard(argv, xsink);

   if (params->num_params)
   {
      printd(5, "UserFunction::evalCopy() about to uninstantiate %d vars\n", params->num_params);

      // uninstantiate local vars from param list
      for (int i = 0; i < params->num_params; i++)
	 uninstantiateLVar(xsink);
   }
   if (xsink->isException())
      xsink->addStackInfo(CT_USER, class_name, name, o_fn, o_ln, o_eln);
   
   traceout("UserFunction::evalCopy()");
}

// calls a user constructor method
AbstractQoreNode *UserFunction::evalConstructor(const QoreListNode *args, QoreObject *self, class BCList *bcl, class BCEAList *bceal, const char *class_name, ExceptionSink *xsink) const
{
   tracein("UserFunction::evalConstructor()");
   printd(2, "UserFunction::evalConstructor(): method='%s:%s' args=%08p (size=%d)\n", 
          class_name, name, args, args ? args->size() : 0);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
   
   AbstractQoreNode *val = NULL;

   int num_args = args ? args->size() : 0;
   // instantiate local vars from param list
   int num_params = params->num_params;

   for (int i = 0; i < num_params; i++)
   {
      AbstractQoreNode *n = args ? const_cast<AbstractQoreNode *>(args->retrieve_entry(i)) : NULL;
      printd(4, "UserFunction::evalConstructor() eval %d: instantiating param lvar %d (%08p)\n", i, params->ids[i], n);
      if (n)
      {
	 if (n->type == NT_REFERENCE) {
	    const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(n);
	    bool is_self_ref = false;
	    n = doPartialEval(r->lvexp, &is_self_ref, xsink);
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
   ReferenceHolder<QoreListNode> argv(xsink);
   
   if (num_params < num_args)
   {
      argv = new QoreListNode();

      for (int i = 0; i < (num_args - num_params); i++) {
	 AbstractQoreNode *v = args->eval_entry(i + num_params, xsink);
	 argv->push(v);
	 if (*xsink) {
	    // uninstantiate local vars from param list
	    for (int j = 0; j < num_params; j++)
	       thread_uninstantiate_lvar(xsink);
	    return 0;
	 }
      }
   }

   // evaluate base constructors (if any)
   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);

   if (!xsink->isEvent())
   {
      // switch to new program for imported objects
      ProgramContextHelper pch(self->getProgram());
 
      // execute constructor
      if (statements)
      {
	 CodeContextHelper cch(name, self, xsink);
#ifdef DEBUG
	 // push call on stack
	 CallStackHelper csh(name, CT_USER, self, xsink);
#endif

	 // instantiate "$self" variable
	 self->instantiateLVar(params->selfid);
	 
	 // instantiate argv and push id on stack
	 instantiateLVar(params->argvid, argv.release());

	 {
	    ArgvContextHelper(params->argvid);
	    
	    // enter gate if necessary
	    if (!synchronized || (gate->enter(xsink) >= 0))
	    {
	       // execute function
	       val = statements->exec(xsink);
	       
	       // exit gate if necessary
	       if (synchronized)
		  gate->exit();
	    }
	 }
	 uninstantiateLVar(xsink);
	    
	 // uninstantiate "$self" variable
	 self->uninstantiateLVar(xsink);
      }
      else
	 argv = 0; // dereference argv now
   }

   if (num_params)
   {
      printd(5, "UserFunction::evalConstructor() about to uninstantiate %d vars\n", params->num_params);

      // uninstantiate local vars from param list
      for (int i = 0; i < num_params; i++)
	 thread_uninstantiate_lvar(xsink);
   }
   if (xsink->isException())
      xsink->addStackInfo(CT_USER, class_name, name, o_fn, o_ln, o_eln);
   
   traceout("UserFunction::evalConstructor()");
   return val;
}

// this will only be called with lvalue expressions
AbstractQoreNode *doPartialEval(AbstractQoreNode *n, bool *is_self_ref, ExceptionSink *xsink)
{
   AbstractQoreNode *rv = NULL;
   const QoreType *ntype = n->type;
   if (ntype == NT_TREE)
   {
      QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(n);
      ReferenceHolder<AbstractQoreNode> nn(tree->right->eval(xsink), xsink);
      if (*xsink)
	 return 0;

      SimpleRefHolder<QoreTreeNode> t(new QoreTreeNode(doPartialEval(tree->left, is_self_ref, xsink), tree->op, nn ? nn.release() : nothing()));
      if (t->left)
	 rv = t.release();
   }
   else
   {
      rv = n->refSelf();
      if (ntype == NT_SELF_VARREF)
	 (*is_self_ref) = true;
   }
   return rv;
}
