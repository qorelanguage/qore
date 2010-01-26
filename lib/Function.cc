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

void UserSignature::parseInitPushLocalVars(const QoreTypeInfo *classTypeInfo) {
   lv = num_params ? new lvar_ptr_t[num_params] : 0;

   selfid = classTypeInfo ? push_local_var("self", classTypeInfo, false) : 0;
   
   // push $argv var on stack and save id
   // FIXME: xxx push as list if hard typing enforced with parse options
   argvid = push_local_var("argv", 0, false);
   printd(5, "UserSignature::parseInitPushLocalVars() this=%p argvid=%p\n", this, argvid);
   
   // init param ids and push local param vars on stack
   for (unsigned i = 0; i < num_params; ++i) {
      lv[i] = push_local_var(names[i], typeList[i]);
      if (typeList[i])
	 typeList[i]->resolve();
      printd(3, "UserSignature::parseInitPushLocalVars() registered local var %s (id=%p)\n", names[i], lv[i]);
   }
}

void UserSignature::parseInitPopLocalVars() {
   for (unsigned i = 0; i < num_params; i++)
      pop_local_var();
   
   // pop $argv param off stack
   pop_local_var();

   // pop $self off stack if present
   if (selfid)
      pop_local_var();
}

void UserSignature::assignParam(int i, VarRefNode *v) {
   names[i] = strdup(v->getName());
   typeList[i] = v->takeTypeInfo();
   
   if (v->getType() == VT_LOCAL)
      parse_error("invalid local variable declaration in argument list; by default all variables declared in argument lists are local");
   else if (v->getType() == VT_GLOBAL)
      parse_error("invalid global variable declaration in argument list; by default all variables declared in argument lists are local");
}

UserFunction::UserFunction(char *n_name, StatementBlock *b, AbstractQoreNode *params, QoreParseTypeInfo *rv, bool synced) 
   : synchronized(synced), gate(synced ? new VRMutex() : 0), name(n_name), signature(params, rv), statements(b) {
   printd(5, "UserFunction::UserFunction(%s) params=%p rv=%p b=%p synced=%d\n", n_name ? n_name : "null", params, rv, b, synced);
}

UserFunction::~UserFunction() {
   printd(5, "UserFunction::~UserFunction() deleting %s\n", name);
   delete gate;
   delete statements;
   if (name)
      free(name);
}

void UserFunction::parseInit() {
   // resolve and push current return type on stack
   ReturnTypeInfoHelper rtih(parseGetReturnTypeInfo());

   // can (and must) be called even if statements is NULL
   statements->parseInit(&signature);
}

void UserFunction::parseInitMethod(const QoreClass &parent_class, bool static_flag) {
   // resolve and push current return type on stack
   ReturnTypeInfoHelper rtih(parseGetReturnTypeInfo());
   
   // must be called even if statements is NULL
   //printd(5, "QoreMethod::parseInit() this=%08p '%s' static_flag=%d\n", this, getName(), static_flag);
   if (!static_flag)
      statements->parseInitMethod(parent_class.getTypeInfo(), &signature, 0);
   else
      statements->parseInit(&signature);
}

void UserFunction::parseInitConstructor(const QoreClass &parent_class, BCList *bcl) {
   assert(!signature.getReturnTypeInfo());

   // push return type on stack (no return value can be used)
   ReturnTypeInfoHelper rtih(&nothingTypeInfo);

   // must be called even if statements is NULL
   statements->parseInitMethod(parent_class.getTypeInfo(), &signature, bcl);
}

void UserFunction::parseInitDestructor(const QoreClass &parent_class) {
   assert(!signature.getReturnTypeInfo());

   // make sure there are no parameters in the destructor
   if (signature.numParams())
      parse_error("no parameters may be defined in class destructors");

   // push return type on stack (no return value can be used)
   ReturnTypeInfoHelper rtih(&nothingTypeInfo);

   // must be called even if statements is NULL
   statements->parseInitMethod(parent_class.getTypeInfo(), &signature, 0);
}

void UserFunction::parseInitCopy(const QoreClass &parent_class) {
   // make sure there is max one parameter in the copy method      
   if (signature.numParams() > 1)
      parse_error("maximum of one parameter may be defined in class copy methods (%d defined)", signature.numParams());

   // push return type on stack (no return value can be used)
   ReturnTypeInfoHelper rtih(&nothingTypeInfo);
   
   // must be called even if statements is NULL
   statements->parseInitMethod(parent_class.getTypeInfo(), &signature, 0);
   
   // see if there is a type specification for the sole parameter and make sure it matches the class if there is
   if (signature.numParams()) {
      if (signature.typeList[0]) {
	 if (!parent_class.getTypeInfo()->parseEqual(signature.typeList[0])) {
	    // raise parse exception if parse exceptions have not been suppressed
	    if (getProgram()->getParseExceptionSink()) {
	       QoreStringNode *desc = new QoreStringNode("copy constructor will be passed ");
	       parent_class.getTypeInfo()->getThisType(*desc);
	       desc->concat(", but the object's parameter was defined expecting ");
	       signature.typeList[0]->getThisType(*desc);
	       desc->concat(" instead");
	       getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
	    }
	 }
      }
      else { // set to class' type
	 signature.typeList[0] = new QoreParseTypeInfo(parent_class.getTypeInfo());
      }
   }
}

// evaluates arguments and sets up the argv variable
int UserFunction::setupCall(const QoreListNode *args, ReferenceHolder<QoreListNode> &argv, ExceptionSink *xsink) const {
   unsigned num_args = args ? args->size() : 0;
   // instantiate local vars from param list
   unsigned num_params = signature.numParams();

   for (unsigned i = 0; i < num_params; i++) {
      AbstractQoreNode *np = args ? const_cast<AbstractQoreNode *>(args->retrieve_entry(i)) : 0;
      AbstractQoreNode *n = 0;
      printd(4, "UserFunction::setupCall() eval %d: instantiating param lvar %d (%08p)\n", i, signature.lv[i], n);
      if (!is_nothing(np)) {
	 if (np->getType() == NT_REFERENCE) {
	    const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(np);
	    bool is_self_ref = false;
	    n = doPartialEval(r->getExpression(), &is_self_ref, xsink);
	    if (!*xsink && !signature.typeList[i]->checkTypeInstantiation(signature.names[i], n, xsink))
	       signature.lv[i]->instantiate(n, is_self_ref ? getStackObject() : 0);
	 }
	 else {
	    n = np->eval(xsink);
	    if (!*xsink && ~signature.typeList[i]->checkTypeInstantiation(signature.names[i], n, xsink))
	       signature.lv[i]->instantiate(n);
	 }
      }
      else {
	 if (!signature.typeList[i]->checkTypeInstantiation(signature.names[i], 0, xsink))
	    signature.lv[i]->instantiate(0);
      }

      // the above if block will only instantiate the local variable if no
      // exceptions have occurred. therefore here we cleanup the rest
      // of any already instantiated local variables if an exception does occur
      if (*xsink) {
	 if (n)
	    n->deref(xsink);
	 while (i)
	    signature.lv[--i]->uninstantiate(xsink);
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
	       signature.lv[j]->uninstantiate(xsink);
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
         signature.selfid->instantiate_object(self);
   
      // instantiate argv and push id on stack
      signature.argvid->instantiate(argv ? argv->refSelf() : 0);

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
      signature.argvid->uninstantiate(xsink);
	 
      // if self then uninstantiate
      if (self)
         signature.selfid->uninstantiate(xsink);
   }
   else
      argv = 0; // dereference argv now

   if (signature.numParams()) {
      printd(5, "UserFunction::eval() about to uninstantiate %d vars\n", signature.numParams());

      // uninstantiate local vars from param list
      for (unsigned i = 0; i < signature.numParams(); i++)
	 signature.lv[i]->uninstantiate(xsink);
   }

   if (xsink->isException()) {
      //printd(5, "UserFunction::eval() this=%08p '%s' addStackInfo() %s:%d\n", this, getName(), o_fn, o_ln);
      xsink->addStackInfo(CT_USER, self ? (class_name ? class_name : self->getClassName()) : (class_name ? class_name : 0), getName(), o_fn, o_ln, o_eln);
   }

   return val;
}

// this function will set up user copy constructor calls
void UserFunction::evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, ExceptionSink *xsink) const {
   QORE_TRACE("UserFunction::evalCopy()");
   printd(5, "UserFunction::evalCopy(): function='%s', num_params=%d, oldobj=%08p\n", getName(), signature.numParams(), old);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   // there can only be max 1 param
   assert(signature.numParams() <= 1);

   ReferenceHolder<QoreListNode> argv(xsink);

   // instantiate local vars from param list
   if (signature.numParams()) {
      //printd(5, "UserFunction::evalCopy(): instantiating param %d '%s' lvar %p (n=%p %s)\n", i, signature.lv[i]->getName(), signature.lv[i], n, n ? n->getTypeName() : 0);
      signature.lv[0]->instantiate(old->refSelf());
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
      signature.selfid->instantiate_object(self);
   
      // instantiate argv and push id on stack (for shift)
      signature.argvid->instantiate(argv ? argv->refSelf() : 0);

      {
	 ArgvContextHelper argv_helper(argv.release(), xsink);

	 // execute function
	 discard(statements->exec(xsink), xsink);
      }

      // uninstantiate argv
      signature.argvid->uninstantiate(xsink);
      
      // uninstantiate self
      signature.selfid->uninstantiate(xsink);
   }

   if (signature.numParams()) {
      printd(5, "UserFunction::evalCopy() about to uninstantiate %d vars\n", signature.numParams());

      // uninstantiate local vars from param list
      for (unsigned i = 0; i < signature.numParams(); i++)
	 signature.lv[i]->uninstantiate(xsink);
   }
   if (xsink->isException())
      xsink->addStackInfo(CT_USER, thisclass.getName(), getName(), o_fn, o_ln, o_eln);
}

// calls a user constructor method
void UserFunction::evalConstructor(const QoreClass &thisclass, QoreObject *self, const QoreListNode *args, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const {
   QORE_TRACE("UserFunction::evalConstructor()");
   printd(2, "UserFunction::evalConstructor(): %s::%s(): args=%08p (size=%d)\n", thisclass.getName(), getName(), args, args ? args->size() : 0);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   ReferenceHolder<QoreListNode> argv(xsink);
   
   if (setupCall(args, argv, xsink))
      return;

   // evaluate base constructors (if any)
   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);

   if (!*xsink) {
      // execute constructor
      if (statements) {
	 CodeContextHelper cch(getName(), self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
	 // push call on stack
	 CallStackHelper csh(getName(), CT_USER, self, xsink);
#endif

	 // instantiate "$self" variable
         signature.selfid->instantiate_object(self);
	 
	 // instantiate argv and push id on stack
	 signature.argvid->instantiate(argv ? argv->refSelf() : 0);

	 {
	    ArgvContextHelper argv_helper(argv.release(), xsink);
	    
	    // enter gate if necessary
	    if (!synchronized || (gate->enter(xsink) >= 0)) {
	       // execute function
	       discard(statements->exec(xsink), xsink);

	       // exit gate if necessary
	       if (synchronized)
		  gate->exit();
	    }
	 }

	 // uninstantiate argv
	 signature.argvid->uninstantiate(xsink);
	    
	 // uninstantiate "$self" variable
         signature.selfid->uninstantiate(xsink);
      }
      else
	 argv = 0; // dereference argv now
   }

   if (signature.numParams()) {
      printd(5, "UserFunction::evalConstructor() about to uninstantiate %d vars\n", signature.numParams());

      // uninstantiate local vars from param list
      for (unsigned i = 0; i < signature.numParams(); i++)
	 signature.lv[i]->uninstantiate(xsink);
   }
   if (xsink->isException())
      xsink->addStackInfo(CT_USER, thisclass.getName(), getName(), o_fn, o_ln, o_eln);

   return;
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
