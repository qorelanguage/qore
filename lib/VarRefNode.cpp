/*
  VarRefNode.cpp

  Qore programming language

  Copyright 2003 - 2010 David Nichols

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

VarRefNode::~VarRefNode() {
   if (name) {
      printd(3, "VarRefNode::~VarRefNode() deleting variable reference %08p %s\n", name, name);
      free(name);
   }
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int VarRefNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("variable reference '%s' %s (0x%p)", name, type == VT_GLOBAL ? "global" : type == VT_LOCAL ? "local" : "unresolved", this);
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

void VarRefNode::resolve(const QoreTypeInfo *typeInfo, const QoreTypeInfo *&outTypeInfo) {
   LocalVar *id;

   bool in_closure;
   if ((id = find_local_var(name, in_closure))) {
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
      outTypeInfo = id->getTypeInfo();
      printd(5, "VarRefNode::resolve(): local var %s resolved (id=%p, in_closure=%d)\n", name, ref.id, in_closure);
   }
   else {
      ref.var = getProgram()->checkGlobalVar(name, typeInfo);
      outTypeInfo = ref.var->parseGetTypeInfo();
      type = VT_GLOBAL;
      printd(5, "VarRefNode::resolve(): global var %s resolved (var=%p)\n", name, ref.var);
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
   return ref.var->eval(xsink);
}

AbstractQoreNode *VarRefNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->eval(needs_deref, xsink);
   if (type == VT_CLOSURE) {
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->eval(needs_deref, xsink);
   }
   needs_deref = true;
   return ref.var->eval(xsink);
}

int64 VarRefNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   VarRefNodeEvalOptionalRefHolder rv(this, xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int VarRefNode::integerEvalImpl(ExceptionSink *xsink) const {
   VarRefNodeEvalOptionalRefHolder rv(this, xsink);
   return rv ? rv->getAsInt() : 0;
}

bool VarRefNode::boolEvalImpl(ExceptionSink *xsink) const {
   VarRefNodeEvalOptionalRefHolder rv(this, xsink);
   return rv ? rv->getAsBool() : 0;
}

double VarRefNode::floatEvalImpl(ExceptionSink *xsink) const {
   VarRefNodeEvalOptionalRefHolder rv(this, xsink);
   return rv ? rv->getAsFloat() : 0;
}

AbstractQoreNode **VarRefNode::getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->getValuePtr(vl, typeInfo, xsink);
   if (type == VT_CLOSURE) {
      printd(5, "VarRefNode::eval() closure var %p (%s)\n", ref.id, ref.id);
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->getValuePtr(vl, typeInfo, xsink);
   }
   return ref.var->getValuePtr(vl, typeInfo, xsink);
}

AbstractQoreNode *VarRefNode::getValue(AutoVLock *vl, ExceptionSink *xsink) const {
   if (type == VT_LOCAL)
      return ref.id->getValue(vl, xsink);
   if (type == VT_CLOSURE) {
      printd(5, "VarRefNode::eval() closure var %p (%s)\n", ref.id, ref.id);
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      return val->getValue(vl, xsink);
   }
   return ref.var->getValue(vl);
}

void VarRefNode::setValue(AbstractQoreNode *n, ExceptionSink *xsink) {
   if (type == VT_LOCAL)
      ref.id->setValue(n, xsink);
   else if (type == VT_CLOSURE) {
      printd(5, "VarRefNode::eval() closure var %p (%s)\n", ref.id, ref.id);
      ClosureVarValue *val = thread_get_runtime_closure_var(ref.id);
      val->setValue(n, xsink);
   }
   else
      ref.var->setValue(n, xsink);
}

char *VarRefNode::takeName() {
   assert(name);
   char *p = name;
   name = 0;
   return p;
}

AbstractQoreNode *VarRefNode::parseInitIntern(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *typeInfo, const QoreTypeInfo *&outTypeInfo) {
   //printd(5, "VarRefNode::parseInitIntern() this=%p '%s' type=%d\n", this, name, type);
   // if it is a new variable being declared
   if (type == VT_LOCAL) {
      outTypeInfo = typeInfo;
      ref.id = push_local_var(name, typeInfo);
      ++lvids;
      //printd(5, "VarRefNode::parseInitIntern() this=%p local var '%s' declared (id=%p)\n", this, name, ref.id);
   }
   else if (type == VT_GLOBAL) {
      outTypeInfo = typeInfo;
   }
   else // otherwise reference must be resolved
      resolve(typeInfo, outTypeInfo);
   
   return this;
}

AbstractQoreNode *VarRefNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&outTypeInfo) {
   parseInitIntern(oflag, pflag, lvids, 0, outTypeInfo);

   bool is_assignment = pflag & PF_FOR_ASSIGNMENT;

   // this expression returns nothing if it's a new local variable
   // so if we're not assigning we return nothingTypeInfo as the
   // return type
   if (!is_assignment && new_decl) { 
      assert(!outTypeInfo);
      outTypeInfo = nothingTypeInfo;
   }

   return this;
}

GlobalVarRefNewObjectNode *VarRefNode::globalMakeNewCall(AbstractQoreNode *args) {
   assert(type == VT_GLOBAL);
   if (ref.var->hasTypeInfo()) {
      GlobalVarRefNewObjectNode *rv = new GlobalVarRefNewObjectNode(takeName(), ref.var, makeArgs(args));
      deref();
      return rv;
   }

   return 0;
}

AbstractQoreNode *VarRefNode::makeNewCall(AbstractQoreNode *args) {
   return type == VT_GLOBAL && new_decl ? globalMakeNewCall(args) : 0;
}

void VarRefDeclNode::parseInitCommon(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&outTypeInfo) {
   if (!typeInfo) {
      typeInfo = parseTypeInfo->resolveAndDelete();
      parseTypeInfo = 0;
   }
#ifdef DEBUG
   else assert(!parseTypeInfo);
#endif

   outTypeInfo = typeInfo;
   parseInitIntern(oflag, pflag, lvids, typeInfo, outTypeInfo);
}

AbstractQoreNode *VarRefDeclNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&outTypeInfo) {
   parseInitCommon(oflag, pflag, lvids, outTypeInfo);

   bool is_assignment = pflag & PF_FOR_ASSIGNMENT;

   if (type == VT_LOCAL) {
      if (!is_assignment) {
	 if (ref.id->needsAssignmentAtInstantiation())
	     parseException("PARSE-TYPE-ERROR", "local variable '$%s' has been defined with a complex type and must be assigned when instantiated", name);
      }
      else
	 ref.id->unsetNeedsValueInstantiation();
   }

   // this expression returns nothing if it's a new local variable
   // so if we're not assigning we return nothingTypeInfo as the
   // return type
   if (!is_assignment && new_decl)
      outTypeInfo = nothingTypeInfo;

   return this;
}

// for checking for new object calls
AbstractQoreNode *VarRefDeclNode::makeNewCall(AbstractQoreNode *args) {
   if (type == VT_GLOBAL)
      return globalMakeNewCall(args);

   LocalVarRefNewObjectNode *rv = new LocalVarRefNewObjectNode(takeName(), typeInfo, takeParseTypeInfo(), makeArgs(args));
   deref();
   return rv;
}

void VarRefFunctionCallBase::parseInitConstructorCall(LocalVar *oflag, int pflag, int &lvids, const QoreClass *qc) {
   if (qc && (qc->getDomain() & getProgram()->getParseOptions()))
      parseException("ILLEGAL-CLASS-INSTANTIATION", "parse options do not allow access to the '%s' class", qc->getName());

   // FIXME: make common code with ScopedObjectCallNode
   const QoreMethod *constructor = qc ? qc->parseGetConstructor() : 0;
   lvids += parseArgsFindVariant(oflag, pflag, constructor ? constructor->getFunction() : 0, qc ? qc->getName() : 0);

   //printd(5, "LocalVarRefNewObjectNode::parseInit() this=%p constructor=%p variant=%p\n", this, constructor, variant);

   if (((constructor && constructor->parseIsPrivate()) || (variant && CONMV_const(variant)->isPrivate())) && !parseCheckPrivateClassAccess(qc))
      parse_error("illegal external access to private constructor of class %s", qc->getName());

   //printd(5, "LocalVarRefNewObjectNode::parseInit() this=%p class=%s (%p) constructor=%p function=%p variant=%p\n", this, qc->getName(), qc, constructor, constructor ? constructor->getFunction() : 0, variant);

   if (pflag & PF_FOR_ASSIGNMENT)
      parse_error("local variable new object instantiation will be assigned when the object is created; it is an error to make an additional assignment");
}

AbstractQoreNode *LocalVarRefNewObjectNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&outTypeInfo) {
   parseInitCommon(oflag, pflag, lvids, outTypeInfo);

   const QoreClass *qc = typeInfo ? typeInfo->qc : 0;
   parseInitConstructorCall(oflag, pflag, lvids, qc);
   return this;
}

AbstractQoreNode *LocalVarRefNewObjectNode::evalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<QoreObject> obj(typeInfo->qc->execConstructor(variant, args, xsink), xsink);
   if (*xsink)
      return 0;
   QoreObject *rv = *obj;
   ref.id->setValue(obj.release(), xsink);
   if (*xsink)
      return 0;
   return rv->refSelf();
}

AbstractQoreNode *LocalVarRefNewObjectNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return LocalVarRefNewObjectNode::evalImpl(xsink);
}

AbstractQoreNode *GlobalVarRefNewObjectNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&outTypeInfo) {
   outTypeInfo = ref.var->parseGetTypeInfo();

   const QoreClass *qc = outTypeInfo ? outTypeInfo->qc : 0;
   parseInitConstructorCall(oflag, pflag, lvids, qc);
   return this;
}

AbstractQoreNode *GlobalVarRefNewObjectNode::evalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<QoreObject> obj(ref.var->getTypeInfo()->qc->execConstructor(variant, args, xsink), xsink);
   if (*xsink)
      return 0;
   QoreObject *rv = *obj;
   ref.var->setValue(obj.release(), xsink);
   if (*xsink)
      return 0;
   return rv->refSelf();
}

AbstractQoreNode *GlobalVarRefNewObjectNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return GlobalVarRefNewObjectNode::evalImpl(xsink);
}
