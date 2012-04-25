/*
  VarRefNode.cpp

  Qore programming language

  Copyright 2003 - 2012 David Nichols

  This library is free software; you can redistribute it and/o
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
#include <qore/intern/ParserSupport.h>
#include <qore/intern/QoreNamespaceIntern.h>

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

void VarRefNode::resolve(const QoreTypeInfo *typeInfo) {
   LocalVar *id;

   bool in_closure;
   if (name.size() == 1 && (id = find_local_var(name.ostr, in_closure))) {
      if (typeInfo)
	 parse_error("type definition given for existing local variable '%s'", id->getName());

      if (in_closure) {
	 id->setClosureUse();
	 type = VT_CLOSURE;
	 ref.id = id;
      }
      else {
	 type = VT_LOCAL;
	 ref.id = id;
      }
      printd(5, "VarRefNode::resolve(): local var %s resolved (id=%p, in_closure=%d)\n", name.ostr, ref.id, in_closure);
   }
   else {
      ref.var = qore_root_ns_private::parseCheckImplicitGlobalVar(name, typeInfo);
      type = VT_GLOBAL;
      printd(5, "VarRefNode::resolve(): global var %s resolved (var=%p)\n", name.ostr, ref.var);
   }
}

AbstractQoreNode *VarRefNode::evalImpl(ExceptionSink *xsink) const {
   if (type == VT_LOCAL) {
      printd(5, "VarRefNode::evalImpl() this=%p lvar %p (%s)\n", this, ref.id, ref.id->getName());
      return ref.id->eval(xsink);
   }
   if (type == VT_CLOSURE) {
      printd(5, "VarRefNode::evalImpl() this=%p closure var %p (%s)\n", this, ref.id, ref.id->getName());
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->eval(xsink);
   }
   printd(5, "VarRefNode::evalImpl() this=%p global var=%p (%s)\n", this, ref.var, ref.var->getName());
   return ref.var->eval();
}

AbstractQoreNode *VarRefNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->eval(needs_deref, xsink);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->eval(needs_deref, xsink);
   }
   return ref.var->eval(needs_deref);
}

int64 VarRefNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->bigIntEval(xsink);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->bigIntEval(xsink);
   }
   return ref.var->bigIntEval();
}

int VarRefNode::integerEvalImpl(ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->intEval(xsink);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->intEval(xsink);
   }

   return (int)ref.var->bigIntEval();
}

bool VarRefNode::boolEvalImpl(ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->boolEval(xsink);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->boolEval(xsink);
   }
   return (bool)ref.var->bigIntEval();
}

double VarRefNode::floatEvalImpl(ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->floatEval(xsink);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->floatEval(xsink);
   }
   return ref.var->floatEval();
}

AbstractQoreNode **VarRefNode::getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->getValuePtr(vl, typeInfo, omap, xsink);
   if (type == VT_CLOSURE) {
      printd(5, "VarRefNode::eval() closure var %p (%s)\n", ref.id, ref.id);
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->getValuePtr(vl, typeInfo, omap, xsink);
   }
   return ref.var->getValuePtr(vl, typeInfo, omap, xsink);
}

AbstractQoreNode **VarRefNode::getContainerValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->getContainerValuePtr(vl, typeInfo, omap, xsink);
   if (type == VT_CLOSURE) {
      printd(5, "VarRefNode::eval() closure var %p (%s)\n", ref.id, ref.id);
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->getContainerValuePtr(vl, typeInfo, omap, xsink);
   }
   return ref.var->getContainerValuePtr(vl, typeInfo, omap, xsink);
}

void VarRefNode::assign(AbstractQoreNode *n, ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      ref.id->assign(n, xsink);
   else if (type == VT_CLOSURE) {
      printd(5, "VarRefNode::assign() closure var %p (%s)\n", ref.id, ref.id);
      ClosureVarValue* val = thread_get_runtime_closure_var(ref.id);
      val->assign(n, xsink);
   }
   else
      ref.var->assign(n, xsink);
}

void VarRefNode::assignBigInt(int64 v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      ref.id->assignBigInt(v, xsink);
   else if (type == VT_CLOSURE)
      thread_get_runtime_closure_var(ref.id)->assignBigInt(v, xsink);
   else
      ref.var->assignBigInt(v, xsink);
}

void VarRefNode::assignFloat(double v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      ref.id->assignFloat(v, xsink);
   else if (type == VT_CLOSURE)
      thread_get_runtime_closure_var(ref.id)->assignFloat(v, xsink);
   else
      ref.var->assignFloat(v, xsink);
}

int64 VarRefNode::plusEqualsBigInt(int64 v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->plusEqualsBigInt(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->plusEqualsBigInt(v, xsink);

   return ref.var->plusEqualsBigInt(v, xsink);
}

double VarRefNode::plusEqualsFloat(double v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->plusEqualsFloat(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->plusEqualsFloat(v, xsink);

   return ref.var->plusEqualsFloat(v, xsink);
}

int64 VarRefNode::minusEqualsBigInt(int64 v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->minusEqualsBigInt(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->minusEqualsBigInt(v, xsink);

   return ref.var->minusEqualsBigInt(v, xsink);
}

double VarRefNode::minusEqualsFloat(double v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->minusEqualsFloat(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->minusEqualsFloat(v, xsink);

   return ref.var->minusEqualsFloat(v, xsink);
}

int64 VarRefNode::orEqualsBigInt(int64 v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->orEqualsBigInt(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->orEqualsBigInt(v, xsink);

   return ref.var->orEqualsBigInt(v, xsink);
}

int64 VarRefNode::andEqualsBigInt(int64 v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->andEqualsBigInt(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->andEqualsBigInt(v, xsink);

   return ref.var->andEqualsBigInt(v, xsink);
}

int64 VarRefNode::modulaEqualsBigInt(int64 v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->modulaEqualsBigInt(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->modulaEqualsBigInt(v, xsink);

   return ref.var->modulaEqualsBigInt(v, xsink);
}

int64 VarRefNode::multiplyEqualsBigInt(int64 v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->multiplyEqualsBigInt(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->multiplyEqualsBigInt(v, xsink);

   return ref.var->multiplyEqualsBigInt(v, xsink);
}

int64 VarRefNode::divideEqualsBigInt(int64 v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->divideEqualsBigInt(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->divideEqualsBigInt(v, xsink);

   return ref.var->divideEqualsBigInt(v, xsink);
}

int64 VarRefNode::xorEqualsBigInt(int64 v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->xorEqualsBigInt(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->xorEqualsBigInt(v, xsink);

   return ref.var->xorEqualsBigInt(v, xsink);
}

int64 VarRefNode::shiftLeftEqualsBigInt(int64 v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->shiftLeftEqualsBigInt(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->shiftLeftEqualsBigInt(v, xsink);

   return ref.var->shiftLeftEqualsBigInt(v, xsink);
}

int64 VarRefNode::shiftRightEqualsBigInt(int64 v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->shiftRightEqualsBigInt(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->shiftRightEqualsBigInt(v, xsink);

   return ref.var->shiftRightEqualsBigInt(v, xsink);
}

int64 VarRefNode::postIncrement(ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->postIncrement(xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->postIncrement(xsink);

   return ref.var->postIncrement(xsink);
}

int64 VarRefNode::preIncrement(ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->preIncrement(xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->preIncrement(xsink);

   return ref.var->preIncrement(xsink);
}

int64 VarRefNode::postDecrement(ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->postDecrement(xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->postDecrement(xsink);

   return ref.var->postDecrement(xsink);
}

int64 VarRefNode::preDecrement(ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->preDecrement(xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->preDecrement(xsink);

   return ref.var->preDecrement(xsink);
}

double VarRefNode::multiplyEqualsFloat(double v, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      return ref.id->multiplyEqualsFloat(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->multiplyEqualsFloat(v, xsink);

   return ref.var->multiplyEqualsFloat(v, xsink);
}

double VarRefNode::divideEqualsFloat(double v, ExceptionSink *xsink) {
   assert(v);
   if (type == VT_LOCAL)
      return ref.id->divideEqualsFloat(v, xsink);
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->divideEqualsFloat(v, xsink);

   return ref.var->divideEqualsFloat(v, xsink);
}

int64 VarRefNode::removeBigInt(ExceptionSink* xsink) {
   if (type == VT_LOCAL)
      return ref.id->removeBigInt(xsink);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->removeBigInt(xsink);
   }
   assert(type == VT_GLOBAL);
   return ref.var->removeBigInt(xsink);
}

double VarRefNode::removeFloat(ExceptionSink* xsink) {
   if (type == VT_LOCAL)
      return ref.id->removeFloat(xsink);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->removeFloat(xsink);
   }
   assert(type == VT_GLOBAL);
   return ref.var->removeFloat(xsink);
}

AbstractQoreNode* VarRefNode::remove(ExceptionSink* xsink, bool for_del) {
   if (type == VT_LOCAL)
      return ref.id->remove(xsink, for_del);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->remove(xsink, for_del);
   }
   assert(type == VT_GLOBAL);
   return ref.var->remove(xsink, for_del);
}

AbstractQoreNode *VarRefNode::parseInitIntern(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *typeInfo, bool is_new) {
   if (pflag & PF_CONST_EXPRESSION)
      parseException("ILLEGAL-VARIABLE-REFERENCE", "variable reference '%s' used illegally in an expression executed at parse time to initialize a constant value", name.ostr);

   //printd(5, "VarRefNode::parseInitIntern() this=%p '%s' type=%d\n", this, name.ostr, type);
   // if it is a new variable being declared
   if (type == VT_LOCAL || type == VT_CLOSURE) {
      if (!ref.id) {
	 ref.id = push_local_var(name.ostr, typeInfo, true, is_new ? 1 : 0, pflag & PF_TOP_LEVEL);
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

VarRefNode* VarRefNode::isOptimized(const QoreTypeInfo*& typeInfo) const {
   if (type == VT_LOCAL)
      return ref.id->isOptimized(typeInfo) ? const_cast<VarRefNode*>(this) : 0;
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->isOptimized(typeInfo) ? const_cast<VarRefNode*>(this) : 0;
   assert(type == VT_GLOBAL);
   return ref.var->isOptimized(typeInfo) ? const_cast<VarRefNode*>(this) : 0;
}

qore_type_t VarRefNode::getValueType() const {
   if (type == VT_LOCAL)
      return ref.id->getValueType();
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->val.getType();
   assert(type == VT_GLOBAL);
   return ref.var->getValueType();
}

const char* VarRefNode::getValueTypeName() const{
   if (type == VT_LOCAL)
      return ref.id->getValueTypeName();
   if (type == VT_CLOSURE)
      return thread_get_runtime_closure_var(ref.id)->val.getTypeName();
   assert(type == VT_GLOBAL);
   return ref.var->getValueTypeName();
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
      typeInfo = parseTypeInfo->resolveAndDelete();
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
   VarRefNewObjectNode *rv = new VarRefNewObjectNode(takeName(), typeInfo, takeParseTypeInfo(), makeArgs(args), type);
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

void VarRefFunctionCallBase::parseInitConstructorCall(LocalVar *oflag, int pflag, int &lvids, const QoreClass *qc) {
   if (qc) {
      if (qore_program_private::parseAddDomain(getProgram(), qc->getDomain()))
	 parseException("ILLEGAL-CLASS-INSTANTIATION", "parse options do not allow access to the '%s' class", qc->getName());

      // FIXME: make common code with ScopedObjectCallNode
      const QoreMethod *constructor = qc ? qc->parseGetConstructor() : 0;
      const QoreTypeInfo *typeInfo;
      lvids += parseArgsVariant(oflag, pflag, constructor ? constructor->getFunction() : 0, typeInfo);

      //printd(5, "VarRefFunctionCallBase::parseInitConstructorCall() this=%p constructor=%p variant=%p\n", this, constructor, variant);

      if (((constructor && constructor->parseIsPrivate()) || (variant && CONMV_const(variant)->isPrivate())) && !qore_class_private::parseCheckPrivateClassAccess(*qc)) {
	 if (variant)
	    parse_error("illegal external access to private constructor %s::constructor(%s)", qc->getName(), variant->getSignature()->getSignatureText());
	 else
	    parse_error("illegal external access to private constructor of class %s", qc->getName());
      }

      //printd(5, "VarRefFunctionCallBase::parseInitConstructorCall() this=%p class=%s (%p) constructor=%p function=%p variant=%p\n", this, qc->getName(), qc, constructor, constructor ? constructor->getFunction() : 0, variant);
   }

   if (pflag & PF_FOR_ASSIGNMENT)
      parse_error("variable new object instantiation will be assigned when the object is created; it is an error to make an additional assignment");
}

AbstractQoreNode *VarRefNewObjectNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&outTypeInfo) {
   parseInitCommon(oflag, pflag, lvids, true);

   const QoreClass *qc = typeInfo->getUniqueReturnClass();
   if (!qc)
      parse_error("cannot instantiate type '%s' as a class", typeInfo->getName());
   parseInitConstructorCall(oflag, pflag, lvids, qc);
   outTypeInfo = typeInfo;
   return this;
}

AbstractQoreNode *VarRefNewObjectNode::evalImpl(ExceptionSink *xsink) const {
   assert(typeInfo->getUniqueReturnClass());
   ReferenceHolder<QoreObject> obj(typeInfo->getUniqueReturnClass()->execConstructor(variant, args, xsink), xsink);
   if (*xsink)
      return 0;
   QoreObject *rv = *obj;
   assign(obj.release(), xsink);
   if (*xsink)
      return 0;
   return rv->refSelf();
}

AbstractQoreNode *VarRefNewObjectNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return VarRefNewObjectNode::evalImpl(xsink);
}
