/*
  FunctionCallNode.cpp

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

#include <vector>

static void invalid_access(AbstractQoreFunction *func) {
   // func will always be non-zero with builtin functions
   const char *class_name = func->className();
   parse_error("parse options do not allow access to builtin %s '%s%s%s()'", class_name ? "method" : "function", class_name ? class_name : "", class_name ? "::" : "", func->getName());
}

static void warn_retval_ignored(AbstractQoreFunction *func) {
   const char *class_name = func->className();
   getProgram()->makeParseWarning(QP_WARN_RETURN_VALUE_IGNORED, "RETURN-VALUE-IGNORED", "call to %s %s%s%s() does not have any side effects and the return value is ignored; to disable this warning, use '%%disable-warning return-value-ignored' in your code", class_name ? "method" : "function", class_name ? class_name : "", class_name ? "::" : "", func->getName());
}

static void warn_deprecated(AbstractQoreFunction *func) {
   const char *class_name = func->className();
   getProgram()->makeParseWarning(QP_WARN_DEPRECATED, "DEPRECATED", "call to deprecated %s %s%s%s(); to disable this warning, use '%%disable-warning deprecated' in your code", class_name ? "method" : "function", class_name ? class_name : "", class_name ? "::" : "", func->getName());
}

static void check_flags(AbstractQoreFunction *func, int64 flags, int64 pflag) {
   if ((pflag & PF_RETURN_VALUE_IGNORED) && (flags & QC_RET_VALUE_ONLY))
      warn_retval_ignored(func);
   if (flags & QC_DEPRECATED)
      warn_deprecated(func);
}

int FunctionCallBase::parseArgsFindVariant(LocalVar *oflag, int pflag, AbstractQoreFunction *func, const QoreTypeInfo *&returnTypeInfo) {
   // number of local variables declared in arguments
   int lvids = 0;

   // number of arguments in call
   unsigned num_args = args ? args->size() : 0;

   // argument type list
   type_vec_t argTypeInfo;
   argTypeInfo.reserve(num_args);

   bool have_arg_type_info = num_args ? false : true;
   // initialize arguments and setup argument type list (argTypeInfo)
   if (num_args) {
      // do arguments need to be evaluated?
      bool needs_eval = args->needs_eval();

      // turn off reference ok and retval ignored flags
      int n_pflag = pflag & ~(PF_REFERENCE_OK | PF_RETURN_VALUE_IGNORED);
      
      // loop through all args
      for (unsigned i = 0; i < num_args; ++i) {
	 AbstractQoreNode **n = args->get_entry_ptr(i);
	 assert(*n);
	 argTypeInfo.push_back(0);
	 //printd(5, "FunctionCallBase::parseArgsFindVariant() this=%p (%s) oflag=%p pflag=%d func=%p i=%d/%d arg=%p (%d %s)\n", this, func ? func->getName() : "n/a", oflag, pflag, func, i, num_args, *n, (*n)->getType(), (*n)->getTypeName());
	 if ((*n)->getType() == NT_REFERENCE)
	    (*n) = (*n)->parseInit(oflag, n_pflag | PF_REFERENCE_OK, lvids, argTypeInfo[i]);
	 else
	    (*n) = (*n)->parseInit(oflag, n_pflag, lvids, argTypeInfo[i]);
	 if (!have_arg_type_info && argTypeInfo[i])
	    have_arg_type_info = true;
	 if (!needs_eval && (*n)->needs_eval()) {
	    args->setNeedsEval();
	    needs_eval = true;
	 }
      }
   }

   // resolves pending signatures unconditionally
   if (func)
      func->resolvePendingSignatures();
   
   // find variant
   variant = func && have_arg_type_info ? func->parseFindVariant(argTypeInfo) : 0;

   int64 po = getProgram()->getParseOptions64();

   //printd(5, "FunctionCallBase::parseArgsFindVariant() this=%p po=%lld, ign=%d\n", this, po, pflag & PF_RETURN_VALUE_IGNORED);

   if (variant) {
      //printd(5, "FunctionCallBase::parseArgsFindVariant() this=%p variant=%p f=%lld (%lld) c=%lld (%lld)\n", this, variant, variant->getFunctionality(), variant->getFunctionality() & po, variant->getFlags(), variant->getFlags() & QC_RET_VALUE_ONLY);

      if (variant->getFunctionality() & po)
	 invalid_access(func);
      check_flags(func, variant->getFlags(), pflag);
   }
   else if (func) {
      //printd(5, "FunctionCallBase::parseArgsFindVariant() this=%p func=%p f=%lld (%lld) c=%lld (%lld)\n", this, func, func->getUniqueFunctionality(), func->getUniqueFunctionality() & po, func->getUniqueFlags(), func->getUniqueFlags() & QC_RET_VALUE_ONLY);

      if (func->getUniqueFunctionality() & po)
	 invalid_access(func);
      check_flags(func, func->getUniqueFlags(), pflag);
   }

   returnTypeInfo = variant ? variant->parseGetReturnTypeInfo() : (func ? func->parseGetUniqueReturnTypeInfo() : 0);

   return lvids;
}
 
// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *AbstractFunctionCallNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return evalImpl(xsink);
}

int64 AbstractFunctionCallNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(evalImpl(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int AbstractFunctionCallNode::integerEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(evalImpl(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool AbstractFunctionCallNode::boolEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(evalImpl(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double AbstractFunctionCallNode::floatEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(evalImpl(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}

AbstractQoreNode *SelfFunctionCallNode::evalImpl(ExceptionSink *xsink) const {
   QoreObject *self = getStackObject();
   
   //printd(0, "SelfFunctionCallNode::evalImpl() this=%p self=%p method=%p (name=%s ns=%s)\n", this, self, method, name ? name : "(null)", ns ? ns->ostr : "(null)");

   if (method)
      return self->evalMethod(*method, args, xsink);
   // otherwise exec copy method
   return self->getClass()->execCopy(self, xsink);
}

// called at parse time
AbstractQoreNode *SelfFunctionCallNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo) {
   if (!oflag) {
      parse_error("cannot call method '%s' outside of class code", getName());
      return this;
   }

   assert(name || ns);

#ifdef DEBUG
   if (ns)
      printd(5, "SelfFunctionCallNode::parseInit() this=%p resolving base class call '%s'\n", this, ns->ostr);
   else 
      printd(5, "SelfFunctionCallNode::parseInit() this=%p resolving '%s'\n", this, name ? name : "(null)");
   assert(!method);
#endif
   if (name) {
      // copy method calls will be recognized by name = 0
      if (!strcmp(name, "copy")) {
	 free(name);
	 name = 0;
	 printd(5, "SelfFunctionCallNode::parseInit() this=%p resolved to copy constructor\n", this);
	 if (args)
	    parse_error("no arguments may be passed to copy methods (%d argument%s given in call to %s::copy())", args->size(), args->size() == 1 ? "" : "s", oflag->getTypeInfo()->qc->getName());
      }
      else
	 method = getParseClass()->parseResolveSelfMethod(name);
   }
   else
      method = getParseClass()->parseResolveSelfMethod(ns);

   lvids += parseArgsFindVariant(oflag, pflag, method ? method->getFunction() : 0, returnTypeInfo);

   if (method) {
      printd(5, "SelfFunctionCallNode::parseInit() this=%p resolved '%s' to %p\n", this, method->getName(), method);
      if (name) {
	 free(name);
	 name = 0;
      }
      else if (ns) {
	 delete ns;
	 ns = 0;
      }

   }

   return this;
}

int SelfFunctionCallNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("in-object method call (0x%p) to %s::%s()", this, method->getClass()->getName(), method->getName());
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *SelfFunctionCallNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

AbstractQoreNode *SelfFunctionCallNode::makeReferenceNodeAndDeref() {
   AbstractQoreNode *rv;
   if (name)
      rv = new ParseSelfMethodReferenceNode(takeName());
   else {
      assert(ns);
      rv = new ParseScopedSelfMethodReferenceNode(takeNScope());
   }
   deref();
   return rv;
}

AbstractQoreNode *MethodCallNode::exec(QoreObject *o, ExceptionSink *xsink) const {
   /* the class and method saved at parse time are used here for this run-time
      optimization: the method pointer saved at parse time is used to execute the
      method directly if the object used at run-time is of the same class as
      either the method or the parse-time class.  Actually any class between the
      parse-time class and the method's class could be used, however I'd have to
      check and make sure that search would be quicker than the quick check
      implemented below on average
   */
   if (qc && (o->getClass() == qc || o->getClass() == method->getClass())) {
      assert(method);
      return variant 
	 ? method->evalNormalVariant(o, reinterpret_cast<const QoreExternalMethodVariant*>(variant), args, xsink)
	 : method->eval(o, args, xsink);
   }
   return o->evalMethod(c_str, args, xsink);
}

/* get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
   the ExceptionSink is only needed for QoreObject where a method may be executed
   use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
   returns -1 for exception raised, 0 = OK
*/
int FunctionCallNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("function call to '%s()' (0x%p)", getName(), this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *FunctionCallNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *FunctionCallNode::evalImpl(ExceptionSink *xsink) const {
   //printd(5, "FunctionCallNode::evalImpl() calling %s() current pgm=%p new pgm=%p\n", func->getName(), ::getProgram(), pgm);
   // if pgm is 0, then ProgramContextHelper does nothing
   ProgramContextHelper pch(pgm, xsink);
   return func->evalFunction(variant, args, xsink);
}

AbstractQoreNode *FunctionCallNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo) {
   assert(!func);
   assert(c_str);

   // resolves the function and assigns pgm for imported code
   func = ::getProgram()->resolveFunction(c_str, pgm);
   free(c_str);
   c_str = 0;
   if (!func)
      return this;

   lvids += parseArgsFindVariant(oflag, pflag, const_cast<AbstractQoreFunction *>(func), returnTypeInfo);

   return this;
}

AbstractQoreNode *ScopedObjectCallNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   if (name) {
      assert(!oc);
      // find object class
      if ((oc = getRootNS()->parseFindScopedClass(name))) {
	 // check if parse options allow access to this class
	 if (oc->getDomain() & getProgram()->getParseOptions())
	    parseException("ILLEGAL-CLASS-INSTANTIATION", "parse options do not allow access to the '%s' class", oc->getName());
      }
      delete name;
      name = 0;
   }
#ifdef DEBUG
   else assert(oc);
#endif

   const QoreMethod *constructor = oc ? oc->parseGetConstructor() : 0;
   lvids += parseArgsFindVariant(oflag, pflag, constructor ? constructor->getFunction() : 0, typeInfo);

   if (oc) {
      typeInfo = oc->getTypeInfo();
      desc.sprintf("new %s", oc->getName());
   }

   //printd(5, "ScopedObjectCallNode::parseInit() this=%p constructor=%p variant=%p\n", this, constructor, variant);

   if (((constructor && constructor->parseIsPrivate()) || (variant && CONMV_const(variant)->isPrivate())) && !parseCheckPrivateClassAccess(oc))
      parse_error("illegal external access to private constructor of class %s", oc->getName());

   //printd(5, "ScopedObjectCallNode::parseInit() this=%p class=%s (%p) constructor=%p function=%p variant=%p\n", this, oc->getName(), oc, constructor, constructor ? constructor->getFunction() : 0, variant);
      
   return this;
}
