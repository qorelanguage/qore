/*
  Function.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

static inline void param_error() {
   parse_error("parameter list contains non-variable reference expressions.");
}

SelfFunctionCall::SelfFunctionCall(char *n) { 
   ns = 0;
   name = n; 
   func = 0; 
}

SelfFunctionCall::SelfFunctionCall(class NamedScope *n) { 
   ns = n;
   name = 0; 
   func = 0; 
}

SelfFunctionCall::SelfFunctionCall(const QoreMethod *f) { 
   ns = 0;
   name = 0;
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
      return self->evalMethod(*func, args, xsink);
   // otherwise exec copy method
   return self->getClass()->execCopy(self, xsink);
}

// called at parse time
void SelfFunctionCall::resolve() {
#ifdef DEBUG
   if (ns)
      printd(5, "SelfFunctionCall:resolve() resolving base class call '%s'\n", ns->ostr);
   else 
      printd(5, "SelfFunctionCall:resolve() resolving '%s'\n", name ? name : "(null)");
   assert(!func);
#endif
   if (name) {
      // FIXME: warn if argument list passed (will be ignored)

      // copy method calls will be recognized by func = 0
      if (!strcmp(name, "copy")) {
	 free(name);
	 name = 0;
	 printd(5, "SelfFunctionCall:resolve() resolved to copy constructor\n");
	 return;
      }
      func = getParseClass()->resolveSelfMethod(name);
   }
   else
      func = getParseClass()->resolveSelfMethod(ns);
   if (func) {
      printd(5, "SelfFunctionCall:resolve() resolved '%s' to %08p\n", func->getName(), func);
      if (name) {
	 free(name);
	 name = 0;
      }
      else if (ns) {
	 delete ns;
	 ns = 0;
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
      xsink->addStackInfo(CT_USER, 0, func->getName(), o_fn, o_ln, o_eln);
   
   return rv;
}


Paramlist::Paramlist(AbstractQoreNode *params)
{
   ReferenceHolder<AbstractQoreNode> param_holder(params, 0);

   lv = 0;
   if (!params) {
      num_params = 0;
      names = 0;
      return;
   }

   if (params->getType() == NT_VARREF) {
      num_params = 1;
      names = new char *[1];
      names[0] = strdup(reinterpret_cast<const VarRefNode *>(params)->name);
      return;
   }

   if (params->getType() != NT_LIST) {
      param_error();
      num_params = 0;
      names = 0;
      return;
   }

   const QoreListNode *l = reinterpret_cast<const QoreListNode *>(params);

   num_params = l->size();
   names = new char *[num_params];
   for (int i = 0; i < num_params; i++) {
      if (l->retrieve_entry(i)->getType() != NT_VARREF) {
	 param_error();
	 num_params = 0;
	 delete [] names;
	 names = 0;
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
   if (lv)
      delete [] lv;
}

UserFunction::UserFunction(char *n_name, Paramlist *parms, StatementBlock *b, bool synced) {
   printd(5, "UserFunction::UserFunction(%s) parms=%p b=%p synced=%d\n", n_name ? n_name : "null", parms, b, synced);
   synchronized = synced;
   if (synced)
      gate = new VRMutex();
# ifdef DEBUG
   else
      gate = 0;
# endif
   name = n_name;
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
   if (name)
      free(name);
}

void UserFunction::deref()
{
   if (ROdereference())
      delete this;
}

BuiltinFunction::BuiltinFunction(const char *nme, q_func_t f, int typ) {
   type = typ;
   name = nme;
   code.func = f;
   next = 0;
}

BuiltinFunction::BuiltinFunction(const char *nme, q_static_method2_t f, int typ) {
   type = typ;
   name = nme;
   code.static_method = f;
   next = 0;
}

BuiltinFunction::BuiltinFunction(const char *nme, q_method_t m, int typ) {
   type = typ;
   name = nme;
   code.method = m;
   next = 0;
}

BuiltinFunction::BuiltinFunction(const char *nme, q_method2_t m, int typ) {
   type = typ;
   name = nme;
   code.method2 = m;
   next = 0;
}

BuiltinFunction::BuiltinFunction(q_constructor_t m, int typ) {
   type = typ;
   name = "constructor";
   code.constructor = m;
   next = 0;
}

BuiltinFunction::BuiltinFunction(q_constructor2_t m, int typ) {
   type = typ;
   name = "constructor";
   code.constructor2 = m;
   next = 0;
}

BuiltinFunction::BuiltinFunction(q_system_constructor_t m, int typ) {
   type = typ;
   name = "constructor";
   code.system_constructor = m;
   next = 0;
}

BuiltinFunction::BuiltinFunction(q_system_constructor2_t m, int typ) {
   type = typ;
   name = "constructor";
   code.system_constructor2 = m;
   next = 0;
}

BuiltinFunction::BuiltinFunction(q_destructor_t m, int typ) {
   type = typ;
   name = "destructor";
   code.destructor = m;
   next = 0;
}

BuiltinFunction::BuiltinFunction(q_destructor2_t m, int typ) {
   type = typ;
   name = "destructor";
   code.destructor2 = m;
   next = 0;
}

BuiltinFunction::BuiltinFunction(q_copy_t m, int typ) {
   type = typ;
   name = "copy";
   code.copy = m;
   next = 0;
}

BuiltinFunction::BuiltinFunction(q_copy2_t m, int typ) {
   type = typ;
   name = "copy";
   code.copy2 = m;
   next = 0;
}

BuiltinFunction::BuiltinFunction(q_delete_blocker_t m) {
   type = QDOM_DEFAULT;
   name = "(delete_blocker)";
   code.delete_blocker = m;
   next = 0;
}

void BuiltinFunction::evalConstructor(QoreObject *self, const QoreListNode *args, class BCList *bcl, class BCEAList *bceal, const char *class_name, ExceptionSink *xsink) const {
   QORE_TRACE("BuiltinFunction::evalConstructor()");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   CodeContextHelper cch("constructor", self, xsink);

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   // push call on call stack
   CallStackHelper csh("constructor", CT_BUILTIN, self, xsink);
#endif

   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);
   
   if (!xsink->isEvent()) {
      code.constructor(self, args, xsink);
      if (xsink->isException())
	 xsink->addStackInfo(CT_BUILTIN, class_name, "constructor", o_fn, o_ln, o_eln);
   }
}

void BuiltinFunction::evalConstructor2(const QoreClass &thisclass, QoreObject *self, const QoreListNode *args, class BCList *bcl, class BCEAList *bceal, const char *class_name, ExceptionSink *xsink) const {
   QORE_TRACE("BuiltinFunction::evalConstructor2()");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   CodeContextHelper cch("constructor", self, xsink);

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   // push call on call stack
   CallStackHelper csh("constructor", CT_BUILTIN, self, xsink);
#endif

   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);
   
   if (!xsink->isEvent()) {
      code.constructor2(thisclass, self, args, xsink);
      if (xsink->isException())
	 xsink->addStackInfo(CT_BUILTIN, class_name, "constructor", o_fn, o_ln, o_eln);
   }
}

void BuiltinFunction::evalSystemConstructor(const QoreClass &thisclass, bool new_calling_convention, QoreObject *self, int val, va_list args) const {
   if (new_calling_convention)
      code.system_constructor2(thisclass, self, val, args);
   else
      code.system_constructor(self, val, args);
}

/*
AbstractQoreNode *BuiltinFunction::evalWithArgs(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const
{
   QORE_TRACE("BuiltinFunction::evalWithArgs()");
   printd(2, "BuiltinFunction::evalWithArgs() calling builtin function \"%s\"\n", name);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   AbstractQoreNode *rv;
   {
      CodeContextHelper cch(name, self, xsink);

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stac
      CallStackHelper csh(name, CT_BUILTIN, self, xsink);
#endif

      rv = code.func(args, xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, self ? self->getClassName() : 0, name, o_fn, o_ln, o_eln);

   return rv;
}
*/

AbstractQoreNode *BuiltinFunction::evalMethod(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const {
   printd(2, "BuiltinFunction::evalMethod() calling builtin func '%s' old calling convention obj=%08p data=%08p\n", name, self, private_data);

   CodeContextHelper cch(name, self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   // push call on call stack in debugging mode
   CallStackHelper csh(name, CT_BUILTIN, self, xsink);
#endif

   // exception information added at the level above
   // (program location must be saved before arguments are evaluated)
   return code.method(self, private_data, args, xsink);
}

AbstractQoreNode *BuiltinFunction::evalMethod(const QoreMethod &method, QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const {
   printd(2, "BuiltinFunction::evalMethod() calling builtin func '%s' new calling convention obj=%08p data=%08p\n", name, self, private_data);

   CodeContextHelper cch(name, self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   // push call on call stack in debugging mode
   CallStackHelper csh(name, CT_BUILTIN, self, xsink);
#endif

   // exception information added at the level above
   // (program location must be saved before arguments are evaluated)
   return code.method2(method, self, private_data, args, xsink);
}

void BuiltinFunction::evalDestructor(const QoreClass &thisclass, QoreObject *self, AbstractPrivateData *private_data, const char *class_name, bool new_calling_convention, ExceptionSink *xsink) const {
   QORE_TRACE("BuiltinFunction::evalDestructor()");
   
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   {
      CodeContextHelper cch("destructor", self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh("destructor", CT_BUILTIN, self, xsink);
#endif

      if (new_calling_convention)
	 code.destructor2(thisclass, self, private_data, xsink);
      else
	 code.destructor(self, private_data, xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, class_name, "destructor", o_fn, o_ln, o_eln);
}

void BuiltinFunction::evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, bool new_calling_convention, ExceptionSink *xsink) const {
   QORE_TRACE("BuiltinFunction::evalCopy()");
   
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
   
   {
      CodeContextHelper cch("copy", self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh("copy", CT_BUILTIN, self, xsink);
#endif

      if (new_calling_convention)
	 code.copy2(thisclass, self, old, private_data, xsink);
      else
	 code.copy(self, old, private_data, xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, thisclass.getName(), "copy", o_fn, o_ln, o_eln);
}

bool BuiltinFunction::evalDeleteBlocker(QoreObject *self, AbstractPrivateData *private_data) const {
   return code.delete_blocker(self, private_data);
}

void BuiltinFunction::evalSystemDestructor(const QoreClass &thisclass, bool new_calling_convention, QoreObject *self, AbstractPrivateData *private_data, ExceptionSink *xsink) const {
   if (new_calling_convention)
      code.destructor2(thisclass, self, private_data, xsink);
   else
      code.destructor(self, private_data, xsink);
}

AbstractQoreNode *BuiltinFunction::evalStatic2(const QoreMethod &method, const QoreListNode *args, ExceptionSink *xsink) const {
   AbstractQoreNode *rv;
   ExceptionSink newsink;

   QORE_TRACE("BuiltinFunction::evalStatic2()");
   printd(3, "BuiltinFunction::evalStatic2() calling builtin function \"%s\"\n", name);
   
   //printd(5, "BuiltinFunction::eval(Node) args=%08p %s\n", args, args ? args->getTypeName() : "(null)");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   QoreListNodeEvalOptionalRefHolder tmp(args, xsink);
   if (*xsink)
      return 0;

   //printd(5, "BuiltinFunction::eval(Node) after eval tmp args=%08p %s\n", *tmp, *tmp ? *tmp->getTypeName() : "(null)");

   {
      CodeContextHelper cch(name, 0, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh(name, CT_BUILTIN, 0, xsink);
#endif

      // execute the function if no new exception has happened
      // necessary only in the case of a builtin object destructor
      if (!newsink.isEvent())
	 rv = code.static_method(method, *tmp, xsink);
      else
	 rv = 0;

      xsink->assimilate(&newsink);
   }

   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, method.getClass()->getName(), name, o_fn, o_ln, o_eln);

   return rv;
}

AbstractQoreNode *BuiltinFunction::eval(const QoreListNode *args, ExceptionSink *xsink, const char *class_name) const {
   AbstractQoreNode *rv;
   ExceptionSink newsink;

   QORE_TRACE("BuiltinFunction::eval(Node)");
   printd(3, "BuiltinFunction::eval(Node) calling builtin function \"%s\"\n", name);
   
   //printd(5, "BuiltinFunction::eval(Node) args=%08p %s\n", args, args ? args->getTypeName() : "(null)");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   QoreListNodeEvalOptionalRefHolder tmp(args, xsink);
   if (*xsink)
      return 0;

   //printd(5, "BuiltinFunction::eval(Node) after eval tmp args=%08p %s\n", *tmp, *tmp ? *tmp->getTypeName() : "(null)");

   {
      CodeContextHelper cch(name, 0, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh(name, CT_BUILTIN, 0, xsink);
#endif

      // execute the function if no new exception has happened
      // necessary only in the case of a builtin object destructor
      if (!newsink.isEvent())
	 rv = code.func(*tmp, xsink);
      else
	 rv = 0;

      xsink->assimilate(&newsink);
   }

   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, class_name ? class_name : 0, name, o_fn, o_ln, o_eln);

   return rv;
}

// calls a user function
AbstractQoreNode *UserFunction::eval(const QoreListNode *args, QoreObject *self, ExceptionSink *xsink, const char *class_name) const {
   QORE_TRACE("UserFunction::eval()");
   printd(5, "UserFunction::eval(): function='%s' args=%08p (size=%d)\n", getName(), args, args ? args->size() : 0);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   int num_args = args ? args->size() : 0;
   // instantiate local vars from param list
   int num_params = params->num_params;
   
   ReferenceHolder<QoreListNode> argv(xsink);

   for (int i = 0; i < num_params; i++) {
      AbstractQoreNode *n = args ? const_cast<AbstractQoreNode *>(args->retrieve_entry(i)) : 0;
      printd(4, "UserFunction::eval() eval %d: instantiating param lvar %s (id=%08p) (n=%08p %s)\n", i, params->lv[i], params->lv[i], n, n ? n->getTypeName() : "(null)");
      if (n) {
	 if (n->getType() == NT_REFERENCE) {
	    const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(n);
	    bool is_self_ref = false;
	    n = doPartialEval(r->getExpression(), &is_self_ref, xsink);
	    //printd(5, "UserFunction::eval() ref self_ref=%d, self=%08p (%s) so=%08p (%s)\n", is_self_ref, self, self ? self->getClass()->name : "NULL", getStackObject(), getStackObject() ? getStackObject()->getClass()->name : "NULL");
	    if (!*xsink)
	       params->lv[i]->instantiate(n, is_self_ref ? getStackObject() : 0);
	 }
	 else {
	    n = n->eval(xsink);
	    if (!xsink->isEvent())
	       params->lv[i]->instantiate(n);
	 }
	 // the above if block will only instantiate the local variable if no
	 // exceptions have occurred. therefore here we do the cleanup the rest
	 // of any already instantiated local variables if an exception does occur
	 if (*xsink) {
	    if (n)
	       n->deref(xsink);
	    while (i)
	       params->lv[--i]->uninstantiate(xsink);
	    return 0;
	 }
      }
      else
	 params->lv[i]->instantiate(0);
   }

   // if there are more arguments than parameters
   printd(5, "UserFunction::eval() params=%d arg=%d\n", num_params, num_args);
   
   if (num_params < num_args) {
      argv = new QoreListNode();
      
      for (int i = 0; i < (num_args - num_params); i++) {
	 AbstractQoreNode *v = args->eval_entry(i + num_params, xsink);
	 argv->push(v);
	 if (*xsink) {
	    // uninstantiate local vars from param list
	    for (int j = 0; j < num_params; j++)
	       params->lv[j]->uninstantiate(xsink);
	    return 0;
	 }
      }
   }

   AbstractQoreNode *val = 0;
   if (statements) {
      CodeContextHelper cch(getName(), self, xsink);

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on stack
      CallStackHelper csh(getName(), CT_USER, self, xsink);
#endif

      if (self)
         params->selfid->instantiate_object(self);
   
      // instantiate argv and push id on stack
      params->argvid->instantiate(argv ? argv->refSelf() : 0);

      {
	 ArgvContextHelper argv_helper(argv.release(), xsink);

	 // enter gate if necessary
	 if (!synchronized || (gate->enter(xsink) >= 0)) {
	    // execute function
	    val = statements->exec(xsink);

	    // exit gate if necessary
	    if (synchronized)
	       gate->exit();
	 }
      }

      // uninstantiate argv
      params->argvid->uninstantiate(xsink);
	 
      // if self then uninstantiate
      if (self)
         params->selfid->uninstantiate(xsink);
   }
   else
      argv = 0; // dereference argv now

   if (num_params) {
      printd(5, "UserFunction::eval() about to uninstantiate %d vars\n", params->num_params);

      // uninstantiate local vars from param list
      for (int i = 0; i < num_params; i++)
	 params->lv[i]->uninstantiate(xsink);
   }

   if (xsink->isException()) {
      //printd(5, "UserFunction::eval() this=%08p '%s' addStackInfo() %s:%d\n", this, getName(), o_fn, o_ln);
      xsink->addStackInfo(CT_USER, self ? (class_name ? class_name : self->getClassName()) : (class_name ? class_name : 0), getName(), o_fn, o_ln, o_eln);
   }

   return val;
}

// this function will set up user copy constructor calls
void UserFunction::evalCopy(QoreObject *old, QoreObject *self, const char *class_name, ExceptionSink *xsink) const
{
   QORE_TRACE("UserFunction::evalCopy()");
   printd(2, "UserFunction::evalCopy(): function='%s', num_params=%d, oldobj=%08p\n", getName(), params->num_params, old);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   // instantiate local vars from param list
   for (int i = 0; i < params->num_params; i++)
   {
      QoreObject *n = (i ? 0 : old);
      printd(5, "UserFunction::evalCopy(): instantiating param lvar %d (%08p)\n", i, params->lv[i], n);
      params->lv[i]->instantiate(n ? n->refSelf() : 0);
   }

   ReferenceHolder<QoreListNode> argv(xsink);

   if (!params->num_params)
   {
      argv = new QoreListNode();
      old->ref();
      argv->push(old);
   }

   if (statements)
   {
      CodeContextHelper cch(getName(), self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on stack
      CallStackHelper csh(getName(), CT_USER, self, xsink);
#endif

      // instantiate self
      params->selfid->instantiate_object(self);
   
      // instantiate argv and push id on stack (for shift)
      params->argvid->instantiate(argv ? argv->refSelf() : 0);

      {
	 ArgvContextHelper argv_helper(argv.release(), xsink);

	 // execute function
	 discard(statements->exec(xsink), xsink);
      }

      // uninstantiate argv
      params->argvid->uninstantiate(xsink);
      
      // uninstantiate self
      params->selfid->uninstantiate(xsink);
   }

   if (params->num_params)
   {
      printd(5, "UserFunction::evalCopy() about to uninstantiate %d vars\n", params->num_params);

      // uninstantiate local vars from param list
      for (int i = 0; i < params->num_params; i++)
	 params->lv[i]->uninstantiate(xsink);
   }
   if (xsink->isException())
      xsink->addStackInfo(CT_USER, class_name, getName(), o_fn, o_ln, o_eln);
}

// calls a user constructor method
AbstractQoreNode *UserFunction::evalConstructor(const QoreListNode *args, QoreObject *self, class BCList *bcl, class BCEAList *bceal, const char *class_name, ExceptionSink *xsink) const {
   QORE_TRACE("UserFunction::evalConstructor()");
   printd(2, "UserFunction::evalConstructor(): method='%s:%s' args=%08p (size=%d)\n", class_name, getName(), args, args ? args->size() : 0);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
   
   AbstractQoreNode *val = 0;

   int num_args = args ? args->size() : 0;
   // instantiate local vars from param list
   int num_params = params->num_params;

   for (int i = 0; i < num_params; i++)
   {
      AbstractQoreNode *n = args ? const_cast<AbstractQoreNode *>(args->retrieve_entry(i)) : 0;
      printd(4, "UserFunction::evalConstructor() eval %d: instantiating param lvar %d (%08p)\n", i, params->lv[i], n);
      if (n)
      {
	 if (n->getType() == NT_REFERENCE) {
	    const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(n);
	    bool is_self_ref = false;
	    n = doPartialEval(r->getExpression(), &is_self_ref, xsink);
	    if (!xsink->isEvent())
	       params->lv[i]->instantiate(n, is_self_ref ? getStackObject() : 0);
	 }
	 else
	 {
	    n = n->eval(xsink);
	    if (!xsink->isEvent())
	       params->lv[i]->instantiate(n);
	 }
	 // the above if block will only instantiate the local variable if no
	 // exceptions have occurred. therefore here we do the cleanup the rest
	 // of any already instantiated local variables if an exception does occur
	 if (xsink->isEvent())
	 {
	    if (n)
	       n->deref(xsink);
	    for (int j = i; j; j--)
	       params->lv[j - 1]->uninstantiate(xsink);

	    return 0;
	 }
      }
      else
	 params->lv[i]->instantiate(0);
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
	       params->lv[j]->uninstantiate(xsink);
	    return 0;
	 }
      }
   }

   // evaluate base constructors (if any)
   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);

   if (!xsink->isEvent()) {
      // switch to new program for imported objects
      ProgramContextHelper pch(self->getProgram(), xsink);
 
      // execute constructor
      if (statements) {
	 CodeContextHelper cch(getName(), self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
	 // push call on stack
	 CallStackHelper csh(getName(), CT_USER, self, xsink);
#endif

	 // instantiate "$self" variable
         params->selfid->instantiate_object(self);
	 
	 // instantiate argv and push id on stack
	 params->argvid->instantiate(argv ? argv->refSelf() : 0);

	 {
	    ArgvContextHelper argv_helper(argv.release(), xsink);
	    
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
	 params->argvid->uninstantiate(xsink);
	    
	 // uninstantiate "$self" variable
         params->selfid->uninstantiate(xsink);
      }
      else
	 argv = 0; // dereference argv now
   }

   if (num_params)
   {
      printd(5, "UserFunction::evalConstructor() about to uninstantiate %d vars\n", params->num_params);

      // uninstantiate local vars from param list
      for (int i = 0; i < num_params; i++)
	 params->lv[i]->uninstantiate(xsink);
   }
   if (xsink->isException())
      xsink->addStackInfo(CT_USER, class_name, getName(), o_fn, o_ln, o_eln);
   

   return val;
}

// this will only be called with lvalue expressions
AbstractQoreNode *doPartialEval(AbstractQoreNode *n, bool *is_self_ref, ExceptionSink *xsink)
{
   AbstractQoreNode *rv = 0;
   qore_type_t ntype = n->getType();
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
