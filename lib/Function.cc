/*
  Function.cc

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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

SelfFunctionCall::~SelfFunctionCall() { 
   if (name) 
      free(name); 
   if (ns)
      delete ns;
}

char *SelfFunctionCall::takeName() {
   char *n = name;
   name = 0;
   return n;
}

NamedScope *SelfFunctionCall::takeNScope() {
   NamedScope *rns = ns;
   ns = 0;
   return rns;
}

AbstractQoreNode *SelfFunctionCall::eval(const QoreListNode *args, ExceptionSink *xsink) const {
   QoreObject *self = getStackObject();
   
   if (func)
      return self->evalMethod(*func, args, xsink);
   // otherwise exec copy method
   return self->getClass()->execCopy(self, xsink);
}

// called at parse time
void SelfFunctionCall::resolve(ParamList *&params, const QoreTypeInfo *&returnTypeInfo) {
   params = 0;
#ifdef DEBUG
   if (ns)
      printd(5, "SelfFunctionCall:resolve() resolving base class call '%s'\n", ns->ostr);
   else 
      printd(5, "SelfFunctionCall:resolve() resolving '%s'\n", name ? name : "(null)");
   assert(!func);
#endif
   if (name) {
      // copy method calls will be recognized by name = 0
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
      params = func->getParams();
      returnTypeInfo = func->getReturnTypeInfo();
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

AbstractQoreNode *ImportedFunctionCall::eval(const QoreListNode *args, ExceptionSink *xsink) const {
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
   
   AbstractQoreNode *rv = pgm->callFunction(func, args, xsink);

   if (xsink->isException())
      xsink->addStackInfo(CT_USER, 0, func->getName(), o_fn, o_ln, o_eln);
   
   return rv;
}

void UserParamList::parseInitPushLocalVars(const QoreTypeInfo *classTypeInfo) {
   lv = num_params ? new lvar_ptr_t[num_params] : 0;

   selfid = classTypeInfo ? push_local_var("self", classTypeInfo, false) : 0;
   
   // push $argv var on stack and save id
   // FIXME: xxx push as list if hard typing enforced with parse options
   argvid = push_local_var("argv", 0, false);
   printd(5, "UserParamList::parseInitPushLocalVars() this=%p argvid=%p\n", this, argvid);
   
   // init param ids and push local param vars on stack
   for (unsigned i = 0; i < num_params; ++i) {
      lv[i] = push_local_var(names[i], typeList[i]);
      if (typeList[i])
	 typeList[i]->resolve();
      printd(3, "UserParamList::parseInitPushLocalVars() registered local var %s (id=%p)\n", names[i], lv[i]);
   }
}

void UserParamList::parseInitPopLocalVars() {
   for (unsigned i = 0; i < num_params; i++)
      pop_local_var();
   
   // pop $argv param off stack
   pop_local_var();

   // pop $self off stack if present
   if (selfid)
      pop_local_var();
}

void UserParamList::assignParam(int i, VarRefNode *v) {
   names[i] = strdup(v->getName());
   typeList[i] = v->takeTypeInfo();
   
   if (v->getType() == VT_LOCAL)
      parse_error("invalid local variable declaration in argument list; by default all variables declared in argument lists are local");
   else if (v->getType() == VT_GLOBAL)
      parse_error("invalid global variable declaration in argument list; by default all variables declared in argument lists are local");
}

UserFunction::UserFunction(char *n_name, UserParamList *parms, StatementBlock *b, QoreParseTypeInfo *rv, bool synced) 
   : synchronized(synced), gate(synced ? new VRMutex() : 0), name(n_name), returnTypeInfo(rv), initialized(false), params(parms), statements(b) {
   printd(5, "UserFunction::UserFunction(%s) parms=%p b=%p synced=%d\n", n_name ? n_name : "null", parms, b, synced);
}

UserFunction::~UserFunction() {
   printd(5, "UserFunction::~UserFunction() deleting %s\n", name);
   delete gate;
   delete params;
   delete statements;
   if (name)
      free(name);
}

void UserFunction::deref() {
   if (ROdereference())
      delete this;
}

void UserFunction::parseInit() {
   if (initialized)
      return;
   initialized = true;

   if (returnTypeInfo)
      returnTypeInfo->resolve();
   
   // push current return type on stack
   ReturnTypeInfoHelper rtih(returnTypeInfo);

   // can (and must) be called even if statements is NULL                                                                                                           
   statements->parseInit(params);
}

void UserFunction::parseInitMethod(const QoreClass &parent_class, bool static_flag) {
   if (initialized)
      return;
   initialized = true;

   if (returnTypeInfo)
      returnTypeInfo->resolve();
   
   // push current return type on stack
   ReturnTypeInfoHelper rtih(returnTypeInfo);
   
   // must be called even if statements is NULL
   //printd(5, "QoreMethod::parseInit() this=%08p '%s' static_flag=%d\n", this, getName(), static_flag);
   if (!static_flag)
      statements->parseInitMethod(parent_class.getTypeInfo(), params, 0);
   else
      statements->parseInit(params);
}

void UserFunction::parseInitConstructor(const QoreClass &parent_class, BCList *bcl) {
   if (initialized)
      return;
   initialized = true;

   assert(!returnTypeInfo);

   // push return type on stack (no return value can be used)
   ReturnTypeInfoHelper rtih(&nothingTypeInfo);

   // must be called even if statements is NULL
   statements->parseInitMethod(parent_class.getTypeInfo(), params, bcl);
}

void UserFunction::parseInitDestructor(const QoreClass &parent_class) {
   if (initialized)
      return;
   initialized = true;

   assert(!returnTypeInfo);

   // make sure there are no parameters in the destructor
   if (params->numParams())
      parse_error("no parameters may be defined in class destructors");

   // push return type on stack (no return value can be used)
   ReturnTypeInfoHelper rtih(&nothingTypeInfo);

   // must be called even if statements is NULL
   statements->parseInitMethod(parent_class.getTypeInfo(), params, 0);
}

void UserFunction::parseInitCopy(const QoreClass &parent_class) {
   if (initialized)
      return;
   initialized = true;

   // make sure there is max one parameter in the copy method      
   if (params->numParams() > 1)
      parse_error("maximum of one parameter may be defined in class copy methods (%d defined)", params->numParams());

   // push return type on stack (no return value can be used)
   ReturnTypeInfoHelper rtih(&nothingTypeInfo);
   
   // must be called even if statements is NULL
   statements->parseInitMethod(parent_class.getTypeInfo(), params, 0);
   
   // see if there is a type specification for the sole parameter and make sure it matches the class if there is
   if (params->numParams()) {
      if (params->typeList[0]) {
	 if (!parent_class.getTypeInfo()->parseEqual(params->typeList[0])) {
	    // raise parse exception if parse exceptions have not been suppressed
	    if (getProgram()->getParseExceptionSink()) {
	       QoreStringNode *desc = new QoreStringNode("copy constructor will be passed ");
	       parent_class.getTypeInfo()->getThisType(*desc);
	       desc->concat(", but the object's parameter was defined expecting ");
	       params->typeList[0]->getThisType(*desc);
	       desc->concat(" instead");
	       getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
	    }
	 }
      }
      else { // set to class' type
	 params->typeList[0] = new QoreParseTypeInfo(parent_class.getTypeInfo());
      }
   }
}

// evaluates arguments and sets up the argv variable
int UserFunction::setupCall(const QoreListNode *args, ReferenceHolder<QoreListNode> &argv, ExceptionSink *xsink) const {
   unsigned num_args = args ? args->size() : 0;
   // instantiate local vars from param list
   unsigned num_params = params->numParams();

   for (unsigned i = 0; i < num_params; i++) {
      AbstractQoreNode *np = args ? const_cast<AbstractQoreNode *>(args->retrieve_entry(i)) : 0;
      AbstractQoreNode *n = 0;
      printd(4, "UserFunction::setupCall() eval %d: instantiating param lvar %d (%08p)\n", i, params->lv[i], n);
      if (!is_nothing(np)) {
	 if (np->getType() == NT_REFERENCE) {
	    const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(np);
	    bool is_self_ref = false;
	    n = doPartialEval(r->getExpression(), &is_self_ref, xsink);
	    if (!*xsink && !params->typeList[i]->checkTypeInstantiation(params->names[i], n, xsink))
	       params->lv[i]->instantiate(n, is_self_ref ? getStackObject() : 0);
	 }
	 else {
	    n = np->eval(xsink);
	    if (!*xsink && ~params->typeList[i]->checkTypeInstantiation(params->names[i], n, xsink))
	       params->lv[i]->instantiate(n);
	 }
      }
      else {
	 if (!params->typeList[i]->checkTypeInstantiation(params->names[i], 0, xsink))
	    params->lv[i]->instantiate(0);
      }

      // the above if block will only instantiate the local variable if no
      // exceptions have occurred. therefore here we do the cleanup the rest
      // of any already instantiated local variables if an exception does occur
      if (*xsink) {
	 if (n)
	    n->deref(xsink);
	 while (i)
	    params->lv[--i]->uninstantiate(xsink);
	 return -1;
      }
   }

   // if there are more arguments than parameters
   printd(5, "UserFunction::setupCall() params=%d args=%d\n", num_params, num_args);
   
   if (num_params < num_args) {
      argv = new QoreListNode();

      for (unsigned i = 0; i < (num_args - num_params); i++) {
	 AbstractQoreNode *v = args->eval_entry(i + num_params, xsink);
	 argv->push(v);
	 if (*xsink) {
	    // uninstantiate local vars from param list
	    for (unsigned j = 0; j < num_params; j++)
	       params->lv[j]->uninstantiate(xsink);
	    return -1;
	 }
      }
   }

   return 0;
}

// calls a user function
AbstractQoreNode *UserFunction::eval(const QoreListNode *args, QoreObject *self, ExceptionSink *xsink, const char *class_name) const {
   QORE_TRACE("UserFunction::eval()");
   printd(5, "UserFunction::eval(): function='%s' args=%08p (size=%d)\n", getName(), args, args ? args->size() : 0);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   ReferenceHolder<QoreListNode> argv(xsink);
   
   if (setupCall(args, argv, xsink))
      return 0;

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

   if (params->numParams()) {
      printd(5, "UserFunction::eval() about to uninstantiate %d vars\n", params->numParams());

      // uninstantiate local vars from param list
      for (unsigned i = 0; i < params->numParams(); i++)
	 params->lv[i]->uninstantiate(xsink);
   }

   if (xsink->isException()) {
      //printd(5, "UserFunction::eval() this=%08p '%s' addStackInfo() %s:%d\n", this, getName(), o_fn, o_ln);
      xsink->addStackInfo(CT_USER, self ? (class_name ? class_name : self->getClassName()) : (class_name ? class_name : 0), getName(), o_fn, o_ln, o_eln);
   }

   return val;
}

// this function will set up user copy constructor calls
void UserFunction::evalCopy(QoreObject *old, QoreObject *self, const char *class_name, ExceptionSink *xsink) const {
   QORE_TRACE("UserFunction::evalCopy()");
   printd(5, "UserFunction::evalCopy(): function='%s', num_params=%d, oldobj=%08p\n", getName(), params->numParams(), old);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   // there can only be max 1 param
   assert(params->numParams() <= 1);

   ReferenceHolder<QoreListNode> argv(xsink);

   // instantiate local vars from param list
   if (params->numParams()) {
      //printd(5, "UserFunction::evalCopy(): instantiating param %d '%s' lvar %p (n=%p %s)\n", i, params->lv[i]->getName(), params->lv[i], n, n ? n->getTypeName() : 0);
      params->lv[0]->instantiate(old->refSelf());
   }
   else {
      argv = new QoreListNode();
      argv->push(old->refSelf());
   }

   if (statements) {
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

   if (params->numParams()) {
      printd(5, "UserFunction::evalCopy() about to uninstantiate %d vars\n", params->numParams());

      // uninstantiate local vars from param list
      for (unsigned i = 0; i < params->numParams(); i++)
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

   ReferenceHolder<QoreListNode> argv(xsink);
   
   if (setupCall(args, argv, xsink))
      return 0;

   // evaluate base constructors (if any)
   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);

   AbstractQoreNode *val = 0;

   if (!*xsink) {
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
	    
	 // uninstantiate "$self" variable
         params->selfid->uninstantiate(xsink);
      }
      else
	 argv = 0; // dereference argv now
   }

   if (params->numParams()) {
      printd(5, "UserFunction::evalConstructor() about to uninstantiate %d vars\n", params->numParams());

      // uninstantiate local vars from param list
      for (unsigned i = 0; i < params->numParams(); i++)
	 params->lv[i]->uninstantiate(xsink);
   }
   if (xsink->isException())
      xsink->addStackInfo(CT_USER, class_name, getName(), o_fn, o_ln, o_eln);

   return val;
}

// this will only be called with lvalue expressions
AbstractQoreNode *doPartialEval(AbstractQoreNode *n, bool *is_self_ref, ExceptionSink *xsink) {
   AbstractQoreNode *rv = 0;
   qore_type_t ntype = n->getType();
   if (ntype == NT_TREE) {
      QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(n);
      ReferenceHolder<AbstractQoreNode> nn(tree->right->eval(xsink), xsink);
      if (*xsink)
	 return 0;

      SimpleRefHolder<QoreTreeNode> t(new QoreTreeNode(doPartialEval(tree->left, is_self_ref, xsink), tree->op, nn ? nn.release() : nothing()));
      if (t->left)
	 rv = t.release();
   }
   else {
      rv = n->refSelf();
      if (ntype == NT_SELF_VARREF)
	 (*is_self_ref) = true;
   }
   return rv;
}
