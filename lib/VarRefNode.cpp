/*
  VarRefNode.cpp

  Qore programming language

  Copyright (C) 2003 - 2014 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/ParserSupport.h>
#include <qore/intern/QoreNamespaceIntern.h>
#include <qore/intern/qore_program_private.h>

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int VarRefNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("variable reference '%s' %s (0x%p)", name.ostr, type == VT_GLOBAL ? "global" : type == VT_LOCAL ? "local" : "unresolved", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *VarRefNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *VarRefNode::getTypeName() const {
   return "variable reference";
}

void VarRefNode::resolve(const QoreTypeInfo* typeInfo) {
   LocalVar* id;

   printd(5, "VarRefNode::resolve() name: '%s' size: %d\n", name.ostr, name.size());

   bool in_closure;
   if (name.size() == 1 && (id = find_local_var(name.ostr, in_closure))) {
      if (typeInfo)
	 parse_error(loc, "type definition given for existing local variable '%s'", id->getName());

      ref.id = id;
      if (in_closure)
         setClosureIntern();
      else
	 type = VT_LOCAL;

      printd(5, "VarRefNode::resolve(): local var %s resolved (id=%p, in_closure=%d)\n", name.ostr, ref.id, in_closure);
   }
   else {
      ref.var = qore_root_ns_private::parseCheckImplicitGlobalVar(loc, name, typeInfo);
      type = VT_GLOBAL;
      printd(5, "VarRefNode::resolve(): implicit global var %s resolved (var=%p)\n", name.ostr, ref.var);
   }
}

AbstractQoreNode *VarRefNode::evalImpl(ExceptionSink *xsink) const {
   AbstractQoreNode* v;
   if (type == VT_LOCAL) {
      printd(5, "VarRefNode::evalImpl() this=%p lvar %p (%s)\n", this, ref.id, ref.id->getName());
      v = ref.id->eval(xsink);
   }
   else if (type == VT_CLOSURE) {
      printd(5, "VarRefNode::evalImpl() this=%p closure var %p (%s)\n", this, ref.id, ref.id->getName());
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      v = val->eval(xsink);
   }
   else if (type == VT_LOCAL_TS) {
      printd(5, "VarRefNode::evalImpl() this=%p local thread-safe var %p (%s)\n", this, ref.id, ref.id->getName());
      ClosureVarValue *val = thread_find_closure_var(ref.id->getName());
      v = val->eval(xsink);
   }
   else if (type == VT_IMMEDIATE)
      v = ref.cvv->eval(xsink);
   else {
      printd(5, "VarRefNode::evalImpl() this=%p global var=%p (%s)\n", this, ref.var, ref.var->getName());
      v = ref.var->eval();
   }

   if (v && v->getType() == NT_REFERENCE) {
      AbstractQoreNode* nv = v->eval(xsink);
      v->deref(xsink);
      v = nv;
   }

   return v;
}

AbstractQoreNode* VarRefNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
   AbstractQoreNode* v;
   if (type == VT_LOCAL)
      v = ref.id->eval(needs_deref, xsink);
   else if (type == VT_CLOSURE) {
      ClosureVarValue* val = thread_get_runtime_closure_var(ref.id);
      v = val->eval(needs_deref, xsink);
   }
   else if (type == VT_LOCAL_TS) {
      ClosureVarValue* val = thread_find_closure_var(ref.id->getName());
      v = val->eval(needs_deref, xsink);
   }
   else if (type == VT_IMMEDIATE)
      v = ref.cvv->eval(needs_deref, xsink);
   else
      v = ref.var->eval(needs_deref);

   if (v && v->getType() == NT_REFERENCE) {
      AbstractQoreNode* nv = v->eval(xsink);
      if (needs_deref)
         v->deref(xsink);
      else
         needs_deref = true;
      v = nv;
   }
   return v;
}

int64 VarRefNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->bigIntEval(xsink);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->bigIntEval(xsink);
   }
   if (type == VT_LOCAL_TS) {
      ClosureVarValue *val = thread_find_closure_var(ref.id->getName());
      return val->bigIntEval(xsink);
   }
   if (type == VT_IMMEDIATE)
      return ref.cvv->bigIntEval(xsink);

   return ref.var->bigIntEval();
}

int VarRefNode::integerEvalImpl(ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->intEval(xsink);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->intEval(xsink);
   }
   if (type == VT_LOCAL_TS) {
      ClosureVarValue *val = thread_find_closure_var(ref.id->getName());
      return val->intEval(xsink);
   }
   if (type == VT_IMMEDIATE)
      return ref.cvv->intEval(xsink);

   return (int)ref.var->bigIntEval();
}

bool VarRefNode::boolEvalImpl(ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->boolEval(xsink);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->boolEval(xsink);
   }
   if (type == VT_LOCAL_TS) {
      ClosureVarValue *val = thread_find_closure_var(ref.id->getName());
      return val->boolEval(xsink);
   }
   if (type == VT_IMMEDIATE)
      return ref.cvv->boolEval(xsink);

   return (bool)ref.var->boolEval();
}

double VarRefNode::floatEvalImpl(ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->floatEval(xsink);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->floatEval(xsink);
   }
   if (type == VT_LOCAL_TS) {
      ClosureVarValue *val = thread_find_closure_var(ref.id->getName());
      return val->floatEval(xsink);
   }
   if (type == VT_IMMEDIATE)
      return ref.cvv->floatEval(xsink);

   return ref.var->floatEval();
}

AbstractQoreNode *VarRefNode::parseInitIntern(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *typeInfo, bool is_new) {
   if (pflag & PF_CONST_EXPRESSION)
      parseException("ILLEGAL-VARIABLE-REFERENCE", "variable reference '%s' used illegally in an expression executed at parse time to initialize a constant value", name.ostr);

   //printd(5, "VarRefNode::parseInitIntern() this: %p '%s' type: %d %p '%s'\n", this, name.ostr, type, typeInfo, typeInfo->getName());
   // if it is a new variable being declared
   if (type == VT_LOCAL || type == VT_CLOSURE || type == VT_LOCAL_TS) {
      if (!ref.id) {
	 ref.id = push_local_var(name.ostr, loc, typeInfo, true, is_new ? 1 : 0, pflag & PF_TOP_LEVEL);
	 ++lvids;
      }
      //printd(5, "VarRefNode::parseInitIntern() this=%p local var '%s' declared (id=%p)\n", this, name.ostr, ref.id);
   }
   else if (type != VT_GLOBAL) {
      assert(type == VT_UNRESOLVED);
      // otherwise reference must be resolved
      resolve(typeInfo);
   }

   return this;
}

AbstractQoreNode *VarRefNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&outTypeInfo) {
   parseInitIntern(oflag, pflag, lvids, 0);

   bool is_assignment = pflag & PF_FOR_ASSIGNMENT;

   // this expression returns nothing if it's a new local variable
   // so if we're not assigning we return nothingTypeInfo as the
   // return type
   if (!is_assignment && new_decl) {
      assert(!outTypeInfo);
      outTypeInfo = nothingTypeInfo;
   }
   else
      outTypeInfo = parseGetTypeInfo();

   return this;
}

VarRefNewObjectNode *VarRefNode::globalMakeNewCall(AbstractQoreNode *args) {
   assert(type == VT_GLOBAL);
   if (ref.var->hasTypeInfo()) {
      QoreParseTypeInfo* pti = ref.var->copyParseTypeInfo();
      VarRefNewObjectNode *rv = new VarRefNewObjectNode(takeName(), ref.var, makeArgs(args), pti ? 0 : ref.var->getTypeInfo(), pti);
      deref();
      return rv;
   }

   return 0;
}

AbstractQoreNode *VarRefNode::makeNewCall(AbstractQoreNode *args) {
   return type == VT_GLOBAL && new_decl ? globalMakeNewCall(args) : 0;
}

void VarRefNode::makeGlobal() {
   assert(type != VT_GLOBAL);
   assert(type == VT_UNRESOLVED || !ref.id);

   type = VT_GLOBAL;
   ref.var = qore_root_ns_private::parseAddGlobalVarDef(name, 0);
   new_decl = true;
}

int VarRefNode::getLValue(LValueHelper& lvh, bool for_remove) const {
   if (type == VT_LOCAL)
      return ref.id->getLValue(lvh, for_remove);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->getLValue(lvh, for_remove);
   if (type == VT_LOCAL_TS)
      return thread_find_closure_var(ref.id->getName())->getLValue(lvh, for_remove);
   if (type == VT_IMMEDIATE)
      return ref.cvv->getLValue(lvh, for_remove);
   assert(type == VT_GLOBAL);
   return ref.var->getLValue(lvh, for_remove);
}

DLLLOCAL void VarRefNode::remove(LValueRemoveHelper& lvrh) {
   if (type == VT_LOCAL)
      return ref.id->remove(lvrh);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->remove(lvrh);
   if (type == VT_LOCAL_TS)
      return thread_find_closure_var(ref.id->getName())->remove(lvrh);
   if (type == VT_IMMEDIATE)
      return ref.cvv->remove(lvrh);
   assert(type == VT_GLOBAL);
   return ref.var->remove(lvrh);
}

GlobalVarRefNode::GlobalVarRefNode(char *n, const QoreTypeInfo* typeInfo) : VarRefNode(n, 0, false, true) {
   explicit_scope = true;
   ref.var = qore_root_ns_private::parseAddResolvedGlobalVarDef(name, typeInfo);
}

GlobalVarRefNode::GlobalVarRefNode(char *n, QoreParseTypeInfo* parseTypeInfo) : VarRefNode(n, 0, false, true) {
   explicit_scope = true;
   ref.var = qore_root_ns_private::parseAddGlobalVarDef(name, parseTypeInfo);
}

void VarRefDeclNode::parseInitCommon(LocalVar *oflag, int pflag, int &lvids, bool is_new) {
   if (!typeInfo) {
      typeInfo = parseTypeInfo->resolveAndDelete(loc);
      parseTypeInfo = 0;
   }
#ifdef DEBUG
   else assert(!parseTypeInfo);
#endif

   parseInitIntern(oflag, pflag, lvids, typeInfo, is_new);
}

AbstractQoreNode *VarRefDeclNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&outTypeInfo) {
   parseInitCommon(oflag, pflag, lvids, outTypeInfo);

   bool is_assignment = pflag & PF_FOR_ASSIGNMENT;

   // this expression returns nothing if it's a new local variable
   // so if we're not assigning we return nothingTypeInfo as the
   // return type
   if (!is_assignment && new_decl)
      outTypeInfo = nothingTypeInfo;
   else
      outTypeInfo = parseGetTypeInfo();

   return this;
}

// for checking for new object calls
AbstractQoreNode *VarRefDeclNode::makeNewCall(AbstractQoreNode *args) {
   VarRefNewObjectNode *rv = new VarRefNewObjectNode(loc, takeName(), typeInfo, takeParseTypeInfo(), makeArgs(args), type);
   deref();
   return rv;
}

void VarRefDeclNode::makeGlobal() {
   assert(type == VT_UNRESOLVED);

   type = VT_GLOBAL;
   if (parseTypeInfo)
      ref.var = qore_root_ns_private::parseAddGlobalVarDef(name, takeParseTypeInfo());
   else
      ref.var = qore_root_ns_private::parseAddResolvedGlobalVarDef(name, typeInfo);
   new_decl = true;
}

void VarRefFunctionCallBase::parseInitConstructorCall(const QoreProgramLocation& loc, LocalVar *oflag, int pflag, int &lvids, const QoreClass *qc) {
   if (qc) {
      // throw an exception if trying to instantiate a class with abstract method variants
      qore_class_private::parseCheckAbstractNew(*const_cast<QoreClass*>(qc));

      if (qore_program_private::parseAddDomain(getProgram(), qc->getDomain()))
	 parseException(loc, "ILLEGAL-CLASS-INSTANTIATION", "parse options do not allow access to the '%s' class", qc->getName());

      // FIXME: make common code with ScopedObjectCallNode
      const QoreMethod *constructor = qc ? qc->parseGetConstructor() : 0;
      const QoreTypeInfo *typeInfo;
      lvids += parseArgsVariant(loc, oflag, pflag, constructor ? constructor->getFunction() : 0, typeInfo);

      //printd(5, "VarRefFunctionCallBase::parseInitConstructorCall() this=%p constructor=%p variant=%p\n", this, constructor, variant);

      if (((constructor && constructor->parseIsPrivate()) || (variant && CONMV_const(variant)->isPrivate())) && !qore_class_private::parseCheckPrivateClassAccess(*qc)) {
	 if (variant)
	    parse_error(loc, "illegal external access to private constructor %s::constructor(%s)", qc->getName(), variant->getSignature()->getSignatureText());
	 else
	    parse_error(loc, "illegal external access to private constructor of class %s", qc->getName());
      }

      //printd(5, "VarRefFunctionCallBase::parseInitConstructorCall() this=%p class=%s (%p) constructor=%p function=%p variant=%p\n", this, qc->getName(), qc, constructor, constructor ? constructor->getFunction() : 0, variant);
   }

   if (pflag & PF_FOR_ASSIGNMENT)
      parse_error(loc, "variable new object instantiation will be assigned when the object is created; it is an error to make an additional assignment");
}

AbstractQoreNode *VarRefNewObjectNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&outTypeInfo) {
   parseInitCommon(oflag, pflag, lvids, true);

   const QoreClass *qc = typeInfo->getUniqueReturnClass();
   if (!qc)
      parse_error(loc, "cannot instantiate type '%s' as a class", typeInfo->getName());

   parseInitConstructorCall(loc, oflag, pflag, lvids, qc);
   outTypeInfo = typeInfo;
   return this;
}

AbstractQoreNode *VarRefNewObjectNode::evalImpl(ExceptionSink *xsink) const {
   assert(typeInfo->getUniqueReturnClass());
   ReferenceHolder<QoreObject> obj(typeInfo->getUniqueReturnClass()->execConstructor(variant, args, xsink), xsink);
   if (*xsink)
      return 0;

   QoreObject *rv = *obj;
   LValueHelper lv(this, xsink);
   if (!lv)
      return 0;
   lv.assign(obj.release());
   if (*xsink)
      return 0;
   return rv->refSelf();
}

AbstractQoreNode *VarRefNewObjectNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return VarRefNewObjectNode::evalImpl(xsink);
}
