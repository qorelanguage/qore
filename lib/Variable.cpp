/*
  Variable.cpp

  Qore programming language

  Copyright (C) 2003 - 2016 David Nichols

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include <qore/QoreType.h>
#include <qore/intern/ParserSupport.h>
#include <qore/intern/QoreObjectIntern.h>
#include <qore/intern/QoreLValue.h>
#include <qore/intern/qore_number_private.h>
#include <qore/intern/qore_list_private.h>
#include <qore/intern/QoreHashNodeIntern.h>

#include <memory>

// global environment hash
QoreHashNode* ENV;

int qore_gvar_ref_u::write(ExceptionSink* xsink) const {
   if (_refptr & 1) {
      xsink->raiseException("ACCESS-ERROR", "attempt to write to read-only imported global variable '%s'", getPtr()->getName());
      return -1;
   }
   return 0;
}

int Var::getLValue(LValueHelper& lvh, bool for_remove) const {
   if (val.type == QV_Ref) {
      if (val.v.write(lvh.vl.xsink))
         return -1;
      return val.v.getPtr()->getLValue(lvh, for_remove);
   }

   lvh.setTypeInfo(typeInfo);
   lvh.setAndLock(rwl);
   if (checkFinalized(lvh.vl.xsink))
      return -1;

   lvh.setValue((QoreLValueGeneric&)val);
   return 0;
}

void Var::remove(LValueRemoveHelper& lvrh) {
   if (val.type == QV_Ref) {
      if (val.v.write(lvrh.getExceptionSink()))
         return;
      val.v.getPtr()->remove(lvrh);
      return;
   }

   QoreAutoVarRWWriteLocker al(rwl);
   lvrh.doRemove((QoreLValueGeneric&)val, typeInfo);
}

void Var::del(ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      printd(4, "Var::~Var() refptr: %p\n", val.v.getPtr());
      val.v.getPtr()->deref(xsink);
      // clear type so no further deleting will be done
   }
   else
      discard(val.removeNode(true), xsink);
}

bool Var::isImported() const {
   return val.type == QV_Ref;
}

const char* Var::getName() const {
   return name.c_str();
}

QoreValue Var::eval() const {
   if (val.type == QV_Ref)
      return val.v.getPtr()->eval();
   QoreAutoVarRWReadLocker al(rwl);
   return val.getReferencedValue();
}


/*
AbstractQoreNode* Var::eval() const {
   if (val.type == QV_Ref)
      return val.v.getPtr()->eval();
   QoreAutoVarRWReadLocker al(rwl);
   return val.getReferencedValue();
}

AbstractQoreNode* Var::eval(bool &needs_deref) const {
   if (val.type == QV_Ref)
      return val.v.getPtr()->eval(needs_deref);
   QoreAutoVarRWReadLocker al(rwl);
   return val.getReferencedValue(needs_deref, true);
}

bool Var::boolEval() const {
   if (val.type == QV_Ref)
      return val.v.getPtr()->boolEval();

   QoreAutoVarRWReadLocker al(rwl);
   return val.getAsBool();
}

int64 Var::bigIntEval() const {
   if (val.type == QV_Ref)
      return val.v.getPtr()->bigIntEval();

   QoreAutoVarRWReadLocker al(rwl);
   return val.getAsBigInt();
}

double Var::floatEval() const {
   if (val.type == QV_Ref)
      return val.v.getPtr()->floatEval();

   QoreAutoVarRWReadLocker al(rwl);
   return val.getAsFloat();
}
*/

void Var::deref(ExceptionSink* xsink) {
   //printd(5, "Var::deref() this: %p '%s' %d -> %d\n", this, getName(), reference_count(), reference_count() - 1);
   if (ROdereference()) {
      del(xsink);
      delete this;
   }
}

ObjCountRec::ObjCountRec(const QoreListNode* c) : con(c), before((bool)qore_list_private::getScanCount(*c)) {
   //printd(5, "ObjCountRec::ObjCountRec() list %p count: %d\n", c, qore_list_private::getScanCount(*c));
}

ObjCountRec::ObjCountRec(const QoreHashNode* c) : con(c), before((bool)qore_hash_private::getScanCount(*c)) {
   //printd(5, "ObjCountRec::ObjCountRec() hash %p count: %d\n", c, qore_hash_private::getScanCount(*c));
}

ObjCountRec::ObjCountRec(const QoreObject* c) : con(c), before((bool)qore_object_private::getScanCount(*c)) {
   //printd(5, "ObjCountRec::ObjCountRec() object %p count: %d\n", c, qore_object_private::getScanCount(*c));
}

int ObjCountRec::getDifference() {
   bool after = needs_scan(con);
   if (after)
      return !before ? 1 : 0;
   return before ? -1 : 0;
}

LValueHelper::LValueHelper(const ReferenceNode& ref, ExceptionSink* xsink, bool for_remove) : vl(xsink), v(0), lvid_set(0), before(false), rdt(0), robj(0), val(0), typeInfo(0) {
   RuntimeReferenceHelper rh(ref, xsink);
   doLValue(lvalue_ref::get(&ref)->vexp, for_remove);
}

LValueHelper::LValueHelper(const AbstractQoreNode* exp, ExceptionSink* xsink, bool for_remove) : vl(xsink), v(0), lvid_set(0), before(false), rdt(0), robj(0), val(0), typeInfo(0) {
   // exp can be 0 when called from LValueRefHelper if the attach to the Program fails, for example
   //printd(5, "LValueHelper::LValueHelper() exp: %p (%s %d)\n", exp, get_type_name(exp), get_node_type(exp));
   if (exp)
      doLValue(exp, for_remove);
}

// this constructor function is used to scan objects after initialization
LValueHelper::LValueHelper(QoreObject& self, ExceptionSink* xsink) : vl(xsink), v(0), lvid_set(0), before(true), rdt(0), robj(qore_object_private::get(self)), val(0), typeInfo(0) {
   ocvec.push_back(ObjCountRec(&self));
}

LValueHelper::~LValueHelper() {
   bool obj_chg = before;
   bool obj_ref = false;
   if (!(*vl.xsink)) {
      // see if we have any object count changes
      if (!ocvec.empty()) {
	 // v could be 0 if the constructor taking QoreObject& was used (to scan objects after initialization)
	 if (v) {
	    if (rdt) {
	       assert(*v);
	       if (!obj_chg)
		  obj_chg = true;
	       inc_container_obj(ocvec[ocvec.size() - 1].con, rdt);
	    }
	    else {
	       bool after = needs_scan(*v);
	       if (before) {
		  // we have to assume that the objects have changed when before and after = true
		  if (!obj_chg)
		     obj_chg = true;
		  if (!after)
		     inc_container_obj(ocvec[ocvec.size() - 1].con, -1);
	       }
	       else if (after) {
		  if (!obj_chg)
		     obj_chg = true;
		  inc_container_obj(ocvec[ocvec.size() - 1].con, 1);
	       }
	    }
	 }

	 if (ocvec.size() > 1) {
	    for (int i = ocvec.size() - 2; i >= 0; --i) {
	       int dt = ocvec[i + 1].getDifference();
	       if (dt)
		  inc_container_obj(ocvec[i].con, dt);

	       //printd(5, "LValueHelper::~LValueHelper() %s %p has obj: %d\n", get_type_name(ocvec[i].con), ocvec[i].con, (int)needs_scan(ocvec[i].con));
	    }
	 }
      }

      if (!obj_chg && (val ? val->needsScan() : needs_scan(*v)))
	 obj_chg = true;
      if (robj) {
	 robj->tRef();
	 obj_ref = true;
      }

      //printd(5, "LValueHelper::~LValueHelper() robj: %p before: %d obj_chg: %d (val: %s v: %s)\n", robj, before, obj_chg, val ? val->getTypeName() : "null", v ? get_type_name(*v) : "null");
   }

   // first free any locks
   vl.del();

   // now delete temporary values (if any)
   for (nvec_t::iterator i = tvec.begin(), e = tvec.end(); i != e; ++i)
      discard(*i, vl.xsink);

   delete lvid_set;

   if (robj) {
      // recalculate recursive references for objects if necessary
      if (obj_chg)
	 RSetHelper rsh(*robj);
      if (obj_ref)
	 robj->tDeref();
   }
}

void LValueHelper::saveTemp(QoreValue& n) {
   saveTemp(n.takeIfNode());
}

void LValueHelper::saveTemp(AbstractQoreNode* n) {
   if (!n || !n->isReferenceCounted())
      return;
   // save for dereferencing later
   tvec.push_back(n);
}

void LValueHelper::setValue(QoreLValueGeneric& nv) {
   assert(!v);
   assert(!val);
   val = &nv;
}

static int var_type_err(const QoreTypeInfo* typeInfo, const char* type, ExceptionSink* xsink) {
   xsink->raiseException("RUNTIME-TYPE-ERROR", "cannot convert lvalue declared as %s to a %s", typeInfo->getName(), type);
   return -1;
}

int LValueHelper::doListLValue(const QoreSquareBracketsOperatorNode* op, bool for_remove) {
   // first get index
   ValueEvalRefHolder rh(op->getRight(), vl.xsink);
   if (*vl.xsink)
      return -1;

   int64 ind = rh->getAsBigInt();
   if (ind < 0) {
      vl.xsink->raiseException("NEGATIVE-LIST-INDEX", "list index " QLLD " is invalid (index must evaluate to a non-negative integer)", ind);
      return -1;
   }

   // now get left hand side
   if (doLValue(op->getLeft(), for_remove))
      return -1;

   QoreListNode* l;
   if (getType() == NT_LIST) {
      ensureUnique();
      l = reinterpret_cast<QoreListNode*>(getValue());

      ocvec.push_back(ObjCountRec(l));
   }
   else {
      if (for_remove)
         return -1;

      // if the lvalue is not already a list, then make it one
      // but first make sure the lvalue can be converted to a list
      if (!typeInfo->parseAcceptsReturns(NT_LIST))
         return var_type_err(typeInfo, "list", vl.xsink);

      // save the old value for dereferencing outside any locks that may have been acquired
      //printd(5, "LValueHelper::doListLValue() this: %p saving old value: %p '%s'\n", this, vp, get_type_name(vp));
      saveTemp(getValue());
      assignIntern((l = new QoreListNode));
      ocvec.push_back(ObjCountRec(l));
   }

   resetPtr(l->get_entry_ptr(ind));
   return 0;
}

int LValueHelper::doHashObjLValue(const QoreTreeNode* tree, bool for_remove) {
   QoreNodeEvalOptionalRefHolder member(tree->right, vl.xsink);
   if (*vl.xsink)
      return -1;

   // convert to default character encoding
   QoreStringValueHelper mem(*member, QCS_DEFAULT, vl.xsink);
   if (*vl.xsink)
      return -1;

   if (doLValue(tree->left, for_remove))
      return -1;

   /*
   if (!isNode())
      return for_remove ? -1 : var_type_err(typeInfo, "hash", vl.xsink);
   */

   qore_type_t t = getType();
   QoreObject* o = t == NT_OBJECT ? reinterpret_cast<QoreObject*>(getValue()) : 0;
   QoreHashNode* h = 0;

   //printd(5, "LValueHelper::doHashObjLValue() h: %p v: %p ('%s', refs: %d)\n", h, getTypeName(), getValue() ? getValue()->reference_count() : 0);

   if (!o) {
      if (t == NT_HASH) {
         ensureUnique();
         h = reinterpret_cast<QoreHashNode*>(getValue());

	 ocvec.push_back(ObjCountRec(h));
      }
      else {
         if (for_remove)
            return -1;

         // if the variable's value is not already a hash or an object, then make it a hash
         // but first make sure the lvalue can be converted to a hash
         if (!typeInfo->parseAcceptsReturns(NT_HASH))
            return var_type_err(typeInfo, "hash", vl.xsink);

         //printd(5, "LValueHelper::doHashObjLValue() this: %p saving value to dereference before making hash: %p '%s'\n", this, vp, get_type_name(vp));
         saveTemp(getValue());
         assignIntern((h = new QoreHashNode));

	 ocvec.push_back(ObjCountRec(h));
      }

      //printd(5, "LValueHelper::doHashObjLValue() def: %s member %s \"%s\"\n", QCS_DEFAULT->getCode(), mem->getEncoding()->getCode(), mem->getBuffer());
      AbstractQoreNode** ptr = for_remove ? h->getExistingValuePtr(mem->getBuffer()) : h->getKeyValuePtr(mem->getBuffer());
      if (!ptr) {
	 assert(for_remove);
	 return -1;
      }

      resetPtr(ptr);
      return 0;
   }

   //printd(5, "LValueHelper::doHashObjLValue() obj: %p member: '%s'\n", o, mem->getBuffer());

   // clear ocvec when we get to an object
   ocvec.clear();
   clearPtr();

   bool intern = qore_class_private::runtimeCheckPrivateClassAccess(*o->getClass());
   if (!qore_object_private::getLValue(*o, mem->getBuffer(), *this, intern, for_remove, vl.xsink)) {
      if (!intern)
         vl.addMemberNotification(o, mem->getBuffer()); // add member notification for external updates
   }
   if (*vl.xsink)
      return -1;

   robj = qore_object_private::get(*o);
   ocvec.push_back(ObjCountRec(o));

   return 0;
}

int LValueHelper::doLValue(const ReferenceNode* ref, bool for_remove) {
   //RuntimeReferenceHelper rh(*ref, vl.xsink);
   const lvalue_ref* r = lvalue_ref::get(ref);
   if (!lvid_set)
      lvid_set = new lvid_set_t;
#ifdef DEBUG
   else
      assert(lvid_set->find(r->lvalue_id) == lvid_set->end());
#endif
   lvid_set->insert(r->lvalue_id);
   return doLValue(r->vexp, for_remove);
}

int LValueHelper::doLValue(const AbstractQoreNode* n, bool for_remove) {
   // if we are already locked, then save the value and unlock before processing
   if (vl) {
      saveTemp(n->refSelf());
      vl.del();
   }
   qore_type_t ntype = n->getType();
   //printd(5, "LValueHelper::doLValue(exp: %p) %s %d\n", n, get_type_name(n), get_node_type(n));
   if (ntype == NT_VARREF) {
      const VarRefNode* v = reinterpret_cast<const VarRefNode*>(n);
      //printd(5, "LValueHelper::doLValue(): vref: %s (%p) type: %d\n", v->getName(), v, v->getType());
      if (v->getLValue(*this, for_remove))
         return -1;
   }
   else if (ntype == NT_SELF_VARREF) {
      const SelfVarrefNode* v = reinterpret_cast<const SelfVarrefNode*>(n);
      // note that getStackObject() is guaranteed to return a value here (self varref is only valid in a method)
      QoreObject* obj = runtime_get_stack_object();
      assert(obj);
      // true is for "internal"
      if (qore_object_private::getLValue(*obj, v->str, *this, true, for_remove, vl.xsink))
         return -1;

      robj = qore_object_private::get(*obj);
      ocvec.push_back(ObjCountRec(obj));
   }
   else if (ntype == NT_CLASS_VARREF)
      reinterpret_cast<const StaticClassVarRefNode*>(n)->getLValue(*this);
   else if (ntype == NT_REFERENCE) {
      if (doLValue(reinterpret_cast<const ReferenceNode*>(n), for_remove))
	 return -1;
   }
   else if (ntype == NT_OPERATOR) {
      const QoreSquareBracketsOperatorNode* op = dynamic_cast<const QoreSquareBracketsOperatorNode*>(n);
      assert(op);
      if (doListLValue(op, for_remove))
	 return -1;
   }
   else {
      assert(n->getType() == NT_TREE);
      // it must be a tree
      const QoreTreeNode* tree = reinterpret_cast<const QoreTreeNode*>(n);
      assert(tree->getOp() == OP_OBJECT_REF);
      if (doHashObjLValue(tree, for_remove))
	 return -1;
   }

#if 0
   if (v && *v)
      printd(0, "LValueHelper::doLValue() v: %p %s %d\n", *v, get_type_name(*v), get_node_type(*v));
   else if (val)
      printd(0, "LValueHelper::doLValue() val: %s %d\n", val->getTypeName(), val->getType());
#endif

   AbstractQoreNode* current_value = getValue();
   if (current_value && current_value->getType() == NT_REFERENCE) {
      const ReferenceNode* ref = reinterpret_cast<const ReferenceNode*>(current_value);
      if (val)
	 val = 0;
      if (v)
	 v = 0;
      return doLValue(ref, for_remove);
   }

   return 0;
}

void LValueHelper::setAndLock(QoreVarRWLock& rwl) {
   rwl.wrlock();
   vl.set(&rwl);
}

void LValueHelper::set(QoreVarRWLock& rwl) {
   vl.set(&rwl);
}

QoreValue LValueHelper::getReferencedValue() const {
   if (val)
      return val->getReferencedValue();

   return QoreValue(*v ? (*v)->refSelf() : 0);
}

AbstractQoreNode* LValueHelper::getReferencedNodeValue() const {
   if (val)
      return val->getReferencedNodeValue();

   return *v ? (*v)->refSelf() : 0;
}

int64 LValueHelper::getAsBigInt() const {
   if (val) return val->getAsBigInt();
   return (*v) ? (*v)->getAsBigInt() : 0;
}

bool LValueHelper::getAsBool() const {
   if (val) return val->getAsBool();
   return (*v) ? (*v)->getAsBool() : 0;
}

double LValueHelper::getAsFloat() const {
   if (val) return val->getAsFloat();
   return (*v) ? (*v)->getAsFloat() : 0;
}

int LValueHelper::assign(QoreValue n, const char* desc) {
   if (n.type == QV_Node && n.v.n == &Nothing)
      n.v.n = 0;

   // check type for assignment
   typeInfo->acceptAssignment(desc, n, vl.xsink);
   if (*vl.xsink) {
      //printd(5, "LValueHelper::assign() this: %p saving type-rejected value: %p '%s'\n", this, n, get_type_name(n));
      saveTemp(n);
      return -1;
   }

   if (lvid_set && n.getType() == NT_REFERENCE && (lvid_set->find(lvalue_ref::get(reinterpret_cast<const ReferenceNode*>(n.getInternalNode()))->lvalue_id) != lvid_set->end())) {
      vl.xsink->raiseException("REFERENCE-ERROR", "recursive reference detected in assignment");
      saveTemp(n);
      return -1;
   }

   if (val) {
      saveTemp(val->assignAssume(n));
      return 0;
   }

   //printd(5, "LValueHelper::assign() this: %p saving old value: %p '%s'\n", this, *v, get_type_name(*v));
   saveTemp(*v);
   *v = n.takeNode();
   return 0;
}

int LValueHelper::makeInt(const char* desc) {
   assert(val);
   if (val->isInt())
      return 0;

   if (typeInfo && !typeInfo->parseAccepts(bigIntTypeInfo)) {
      typeInfo->doTypeException(0, desc, bigIntTypeInfo->getName(), vl.xsink);
      assert(*vl.xsink);
      return -1;
   }

   saveTemp(val->makeInt());
   return 0;
}

int LValueHelper::makeFloat(const char* desc) {
   assert(val);
   if (val->isFloat())
      return 0;

   if (typeInfo && !typeInfo->parseAccepts(floatTypeInfo)) {
      typeInfo->doTypeException(0, desc, floatTypeInfo->getName(), vl.xsink);
      return -1;
   }

   saveTemp(val->makeFloat());
   return 0;
}

int LValueHelper::makeNumber(const char* desc) {
   assert(val);
   if (val->getType() == NT_NUMBER)
      return 0;

   if (typeInfo && !typeInfo->parseAccepts(numberTypeInfo)) {
      typeInfo->doTypeException(0, desc, numberTypeInfo->getName(), vl.xsink);
      return -1;
   }

   saveTemp(val->makeNumber());
   return 0;
}

int64 LValueHelper::plusEqualsBigInt(int64 va, const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->plusEqualsBigInt(va, getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val += va;
   return i->val;
}

int64 LValueHelper::minusEqualsBigInt(int64 va, const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->minusEqualsBigInt(va, getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val -= va;
   return i->val;
}

int64 LValueHelper::multiplyEqualsBigInt(int64 va, const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->multiplyEqualsBigInt(va, getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val *= va;
   return i->val;
}

int64 LValueHelper::divideEqualsBigInt(int64 va, const char* desc) {
   assert(va);

   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->divideEqualsBigInt(va, getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val /= va;
   return i->val;
}

int64 LValueHelper::orEqualsBigInt(int64 va, const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->orEqualsBigInt(va, getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val |= va;
   return i->val;
}

int64 LValueHelper::xorEqualsBigInt(int64 va, const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->xorEqualsBigInt(va, getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val ^= va;
   return i->val;
}

int64 LValueHelper::modulaEqualsBigInt(int64 va, const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->modulaEqualsBigInt(va, getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val %= va;
   return i->val;
}

int64 LValueHelper::andEqualsBigInt(int64 va, const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->andEqualsBigInt(va, getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val &= va;
   return i->val;
}

int64 LValueHelper::shiftLeftEqualsBigInt(int64 va, const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->shiftLeftEqualsBigInt(va, getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val <<= va;
   return i->val;
}

int64 LValueHelper::shiftRightEqualsBigInt(int64 va, const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->shiftRightEqualsBigInt(va, getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val >>= va;
   return i->val;
}

int64 LValueHelper::preIncrementBigInt(const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->preIncrementBigInt(getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   return ++i->val;
}

int64 LValueHelper::preDecrementBigInt(const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->preDecrementBigInt(getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   return --i->val;
}

int64 LValueHelper::postIncrementBigInt(const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      assert(val->isInt());
      return val->postIncrementBigInt(getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   return i->val++;
}

int64 LValueHelper::postDecrementBigInt(const char* desc) {
   if (val) {
      if (makeInt(desc))
	 return 0;
      return val->postDecrementBigInt(getTempRef());
   }

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   return i->val--;
}

double LValueHelper::preIncrementFloat(const char* desc) {
   if (val) {
      if (makeFloat(desc))
	 return 0;
      return val->preIncrementFloat(getTempRef());
   }

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0.0;
   return ++f->f;
}

double LValueHelper::preDecrementFloat(const char* desc) {
   if (val) {
      if (makeFloat(desc))
	 return 0;
      return val->preDecrementFloat(getTempRef());
   }

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0.0;
   return --f->f;
}

double LValueHelper::postIncrementFloat(const char* desc) {
   if (val) {
      if (makeFloat(desc))
	 return 0;
      return val->postIncrementFloat(getTempRef());
   }

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0.0;
   return f->f++;
}

double LValueHelper::postDecrementFloat(const char* desc) {
   if (val) {
      if (makeFloat(desc))
	 return 0;
      return val->postDecrementFloat(getTempRef());
   }

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0.0;
   return f->f--;
}

double LValueHelper::plusEqualsFloat(double va, const char* desc) {
   if (val) {
      if (makeFloat(desc))
	 return 0;
      return val->plusEqualsFloat(va, getTempRef());
   }

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0.0;
   f->f += va;
   return f->f;
}

double LValueHelper::minusEqualsFloat(double va, const char* desc) {
   if (val) {
      if (makeFloat(desc))
	 return 0;
      return val->minusEqualsFloat(va, getTempRef());
   }

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0.0;
   f->f -= va;
   return f->f;
}

double LValueHelper::multiplyEqualsFloat(double va, const char* desc) {
   if (val) {
      if (makeFloat(desc))
	 return 0;
      return val->multiplyEqualsFloat(va, getTempRef());
   }

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0.0;
   f->f *= va;
   return f->f;
}

double LValueHelper::divideEqualsFloat(double va, const char* desc) {
   assert(va);
   if (val) {
      if (makeFloat(desc))
	 return 0;
      return val->divideEqualsFloat(va, getTempRef());
   }

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0.0;
   f->f /= va;
   return f->f;
}

void LValueHelper::preIncrementNumber(const char* desc) {
   QoreNumberNode* n = ensureUniqueNumber(desc);
   if (n)
      qore_number_private::inc(*n);
}

void LValueHelper::preDecrementNumber(const char* desc) {
   QoreNumberNode* n = ensureUniqueNumber(desc);
   if (n)
      qore_number_private::dec(*n);
}

QoreNumberNode* LValueHelper::postIncrementNumber(bool ref_rv, const char* desc) {
   QoreNumberNode* n = ensureUniqueNumber(desc);
   if (!n)
      return 0;
   QoreNumberNode* rv = ref_rv ? new QoreNumberNode(*n) : 0;
   qore_number_private::inc(*n);
   return rv;
}

QoreNumberNode* LValueHelper::postDecrementNumber(bool ref_rv, const char* desc) {
   QoreNumberNode* n = ensureUniqueNumber(desc);
   if (!n)
      return 0;
   QoreNumberNode* rv = ref_rv ? new QoreNumberNode(*n) : 0;
   qore_number_private::dec(*n);
   return rv;
}

void LValueHelper::plusEqualsNumber(const AbstractQoreNode* r, const char* desc) {
   SimpleRefHolder<QoreNumberNode> rn_holder;
   QoreNumberNode* rn;
   if (get_node_type(r) == NT_NUMBER)
      rn = const_cast<QoreNumberNode*>(reinterpret_cast<const QoreNumberNode*>(r));
   else
      rn_holder = (rn = new QoreNumberNode(r));

   QoreNumberNode* n = ensureUniqueNumber(desc);
   if (n)
      qore_number_private::plusEquals(*n, *rn);
}

void LValueHelper::minusEqualsNumber(const AbstractQoreNode* r, const char* desc) {
   SimpleRefHolder<QoreNumberNode> rn_holder;
   QoreNumberNode* rn;
   if (get_node_type(r) == NT_NUMBER)
      rn = const_cast<QoreNumberNode*>(reinterpret_cast<const QoreNumberNode*>(r));
   else
      rn_holder = (rn = new QoreNumberNode(r));

   QoreNumberNode* n = ensureUniqueNumber(desc);
   if (n)
      qore_number_private::minusEquals(*n, *rn);
}

void LValueHelper::multiplyEqualsNumber(const AbstractQoreNode* r, const char* desc) {
   SimpleRefHolder<QoreNumberNode> rn_holder;
   QoreNumberNode* rn;
   if (get_node_type(r) == NT_NUMBER)
      rn = const_cast<QoreNumberNode*>(reinterpret_cast<const QoreNumberNode*>(r));
   else
      rn_holder = (rn = new QoreNumberNode(r));

   QoreNumberNode* n = ensureUniqueNumber(desc);
   if (n)
      qore_number_private::multiplyEquals(*n, *rn);
}

void LValueHelper::divideEqualsNumber(const AbstractQoreNode* r, const char* desc) {
   SimpleRefHolder<QoreNumberNode> rn_holder;
   QoreNumberNode* rn;
   if (get_node_type(r) == NT_NUMBER)
      rn = const_cast<QoreNumberNode*>(reinterpret_cast<const QoreNumberNode*>(r));
   else
      rn_holder = (rn = new QoreNumberNode(r));

   QoreNumberNode* n = ensureUniqueNumber(desc);
   if (n)
      qore_number_private::divideEquals(*n, *rn);
}

AbstractQoreNode* LValueHelper::removeNode(bool for_del) {
   if (val)
      return val->removeNode(for_del);

   AbstractQoreNode* rv = *v;
   *v = 0;
   return rv;
}

QoreValue LValueHelper::remove(bool& static_assignment) {
   assert(!static_assignment);
   if (val)
      return val->remove(static_assignment);

   AbstractQoreNode* rv = *v;
   *v = 0;
   return rv;
}

LValueRemoveHelper::LValueRemoveHelper(const AbstractQoreNode* exp, ExceptionSink* n_xsink, bool fd) : xsink(n_xsink), for_del(fd) {
   doRemove(const_cast<AbstractQoreNode*>(exp));
}

LValueRemoveHelper::LValueRemoveHelper(const ReferenceNode& ref, ExceptionSink* n_xsink, bool fd) : xsink(n_xsink), for_del(fd) {
   RuntimeReferenceHelper rrh(ref, xsink);
   if (rrh)
      doRemove(const_cast<AbstractQoreNode*>(lvalue_ref::get(&ref)->vexp));
}

AbstractQoreNode* LValueRemoveHelper::removeNode() {
   assert(!*xsink);
   return rv.removeNode(for_del);
}

QoreValue LValueRemoveHelper::remove(bool& static_assignment) {
   assert(!*xsink);
   return rv.remove(static_assignment);
}

void LValueRemoveHelper::deleteLValue() {
   assert(!*xsink);
   assert(for_del);

   bool static_assignment = false;
   ValueOptionalRefHolder v(remove(static_assignment), true, xsink);
   if (!v) {
      assert(!static_assignment);
      return;
   }
   if (static_assignment)
      v.setTemp();

   qore_type_t t = v->getType();
   if (t != NT_OBJECT)
      return;

   QoreObject* o = reinterpret_cast<QoreObject*>(v->getInternalNode());
   if (o->isSystemObject()) {
      xsink->raiseException("SYSTEM-OBJECT-ERROR", "you cannot delete a system constant object");
      return;
   }

   o->doDelete(xsink);
}

void LValueRemoveHelper::doRemove(AbstractQoreNode* lvalue) {
   assert(lvalue);
   qore_type_t t = lvalue->getType();
   if (t == NT_VARREF) {
      reinterpret_cast<VarRefNode*>(lvalue)->remove(*this);
      return;
   }

   if (t == NT_SELF_VARREF) {
      rv.assignInitial(qore_object_private::takeMember(*(runtime_get_stack_object()), xsink, reinterpret_cast<SelfVarrefNode*>(lvalue)->str, false));
      return;
   }

   if (t == NT_CLASS_VARREF) {
      reinterpret_cast<StaticClassVarRefNode*>(lvalue)->remove(*this);
      return;
   }

   if (t == NT_OPERATOR) {
      const QoreSquareBracketsOperatorNode* op = dynamic_cast<const QoreSquareBracketsOperatorNode*>(lvalue);
      if (op) {
         LValueHelper lvhb(op, xsink, true);
         if (!lvhb)
            return;

         bool static_assignment = false;
         QoreValue tmp = lvhb.remove(static_assignment);
         if (static_assignment)
            tmp.ref();
         rv.assignAssumeInitial(tmp);
         return;
      }
   }

   // could be any type if in a background expression
   if (t != NT_TREE) {
      rv.assignInitial(lvalue ? lvalue->refSelf() : 0);
      return;
   }

   // can be only a list reference
   // must be a tree
   assert(t == NT_TREE);
   QoreTreeNode* tree = reinterpret_cast<QoreTreeNode*>(lvalue);

   assert(tree->getOp() == OP_OBJECT_REF);

   // get the member name or names
   QoreNodeEvalOptionalRefHolder member(tree->right, xsink);
   if (*xsink)
      return;

   // find variable ptr, exit if doesn't exist anyway
   LValueHelper lvh(tree->left, xsink, true);
   if (!lvh)
      return;

   t = lvh.getType();
   if (t == NT_HASH)
      lvh.ensureUnique();

   QoreObject* o = t == NT_OBJECT ? reinterpret_cast<QoreObject*>(lvh.getValue()) : 0;
   QoreHashNode* h = !o && t == NT_HASH ? reinterpret_cast<QoreHashNode*>(lvh.getValue()) : 0;
   if (!o && !h)
      return;

   // remove a slice of the hash or object
   if (get_node_type(*member) == NT_LIST) {
      const QoreListNode* l = reinterpret_cast<const QoreListNode*>(*member);

      if (o)
	 qore_object_private::takeMembers(*o, rv, lvh, l);
      else {
	 unsigned old_count = qore_hash_private::getScanCount(*h);

	 QoreHashNode* rvh = new QoreHashNode;
	 rv.assignInitial(rvh);

	 ConstListIterator li(l);
	 while (li.next()) {
	    QoreStringValueHelper mem(li.getValue(), QCS_DEFAULT, xsink);
	    if (*xsink)
	       return;

	    AbstractQoreNode* n = h->takeKeyValue(mem->getBuffer());
	    if (*xsink)
	       return;

	    // note that no exception can occur here
	    rvh->setKeyValue(mem->getBuffer(), n, xsink);
	    assert(!*xsink);
	 }

	 if (old_count && !qore_hash_private::getScanCount(*h))
	    lvh.setDelta(-1);
      }

      return;
   }

   QoreStringValueHelper mem(*member, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   AbstractQoreNode* v;
   if (o)
      v = qore_object_private::takeMember(*o, lvh, mem->getBuffer());
   else {
      v = h->takeKeyValue(mem->getBuffer());
      if (needs_scan(v)) {
	 if (!qore_hash_private::getScanCount(*h))
	    lvh.setDelta(-1);
      }
   }

   rv.assignInitial(v);
}

int LocalVarValue::getLValue(LValueHelper& lvh, bool for_remove) const {
   //printd(5, "LocalVarValue::getLValue() this: %p type: '%s' %d\n", this, val.getTypeName(), val.getType());
   if (val.getType() == NT_REFERENCE) {
      ReferenceNode* ref = reinterpret_cast<ReferenceNode*>(val.v.n);
      LocalRefHelper<LocalVarValue> helper(this, *ref, lvh.vl.xsink);
      return helper ? lvh.doLValue(ref, for_remove) : -1;
   }

   lvh.setValue((QoreLValueGeneric&)val);
   return 0;
}

void LocalVarValue::remove(LValueRemoveHelper& lvrh, const QoreTypeInfo* typeInfo) {
   if (val.getType() == NT_REFERENCE) {
      VarStackPointerHelper<LocalVarValue> helper(const_cast<LocalVarValue*>(this));
      ReferenceNode* ref = reinterpret_cast<ReferenceNode*>(val.v.n);
      lvrh.doRemove(lvalue_ref::get(ref)->vexp);
      return;
   }

   lvrh.doRemove((QoreLValueGeneric&)val, typeInfo);
}

int ClosureVarValue::getLValue(LValueHelper& lvh, bool for_remove) const {
   //printd(5, "ClosureVarValue::getLValue() this: %p type: '%s' %d\n", this, val.getTypeName(), val.getType());

   if (typeInfo->needsScan())
      lvh.setClosure(const_cast<ClosureVarValue*>(this));

   QoreSafeVarRWWriteLocker sl(rml);
   if (val.getType() == NT_REFERENCE) {
      ReferenceHolder<ReferenceNode> ref(reinterpret_cast<ReferenceNode*>(val.v.n->refSelf()), lvh.vl.xsink);
      sl.unlock();
      LocalRefHelper<ClosureVarValue> helper(this, **ref, lvh.vl.xsink);
      return helper ? lvh.doLValue(*ref, for_remove) : -1;
   }

   lvh.setTypeInfo(typeInfo);
   lvh.set(rml);
   sl.stay_locked();
   lvh.setValue((QoreLValueGeneric&)val);
   return 0;
}

void ClosureVarValue::remove(LValueRemoveHelper& lvrh) {
   QoreSafeVarRWWriteLocker sl(rml);
   if (val.getType() == NT_REFERENCE) {
      ReferenceHolder<ReferenceNode> ref(reinterpret_cast<ReferenceNode*>(val.v.n->refSelf()), lvrh.getExceptionSink());
      sl.unlock();
      // skip this entry in case it's a recursive reference
      VarStackPointerHelper<ClosureVarValue> helper(const_cast<ClosureVarValue*>(this));
      lvrh.doRemove(lvalue_ref::get(*ref)->vexp);
      return;
   }

   lvrh.doRemove((QoreLValueGeneric&)val, typeInfo);
}

/*
static bool check_closure_loop_closure(ClosureVarValue* cvv, const QoreClosureBase* c) {
   //printd(5, "check_closure_loop_closure() c: %p cvv: %p rv: %d\n", c, cvv, c->hasVar(cvv));
   return c->hasVar(cvv);
}

static bool check_closure_loop(ClosureVarValue* cvv, const AbstractQoreNode* n) {
   switch (get_node_type(n)) {
      case NT_RUNTIME_CLOSURE: return check_closure_loop_closure(cvv, reinterpret_cast<const QoreClosureBase*>(n));
      case NT_HASH: {
	 ConstHashIterator hi(reinterpret_cast<const QoreHashNode*>(n));
	 while (hi.next())
	    if (check_closure_loop(cvv, hi.getValue()))
	       return true;
	 break;
      }
      case NT_LIST: {
	 ConstListIterator li(reinterpret_cast<const QoreListNode*>(n));
	 while (li.next())
	    if (check_closure_loop(cvv, li.getValue()))
	       return true;
	 break;
      }
   }
   return false;
}
*/

void ClosureVarValue::ref() const {
   AutoLocker al(rlck);
   //printd(5, "ClosureVarValue::ref() this: %p refs: %d -> %d val: %s\n", this, references, references + 1, val.getTypeName());
   ++references;
}

void ClosureVarValue::deref(ExceptionSink* xsink) {
   printd(QORE_DEBUG_OBJ_REFS, "ClosureVarValue::deref() this: %p refs: %d -> %d rcount: %d rset: %p val: %s\n", this, references, references - 1, rcount, rset, val.getTypeName());

   int ref_copy;
   {
      AutoLocker al(rlck);
      ref_copy = --references;
   }

   if (!ref_copy) {
      // first invalidate any rset
      {
	 QoreAutoVarRWWriteLocker al(rml);
	 removeInvalidateRSet();
      }
      del(xsink);
      printd(QORE_DEBUG_OBJ_REFS, "ClosureVarValue::deref() this: %p deleting\n", this);
      delete this;
      return;
   }

   bool do_del = false;

   while (true) {
      if (!rset) {
         if (ref_copy == rcount)
            do_del = true;
         break;
      }
      int rc = rset->canDelete(ref_copy, rcount);
      if (rc == 1) {
	 printd(QORE_DEBUG_OBJ_REFS, "ClosureVarValue::deref() this: %p found recursive reference; deleting value\n", this);
	 do_del = true;
	 break;
      }
      if (!rc)
	 break;
      assert(rc == -1);
      // need to recalculate references
      RSetHelper rsh(*this);
   }

   if (do_del) {
      // first invalidate any rset
      {
	 QoreAutoVarRWWriteLocker al(rml);
	 removeInvalidateRSet();
      }
      // now delete the value which should cause the entire chain to be destroyed
      del(xsink);
   }
}

bool ClosureVarValue::scanMembers(RSetHelper& rsh) {
   return scanCheck(rsh, val.getInternalNode());
}
