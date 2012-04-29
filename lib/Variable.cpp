/*
  Variable.cpp

  Qore programming language

  Copyright 2003 - 2012 David Nichols

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

// global environment hash
QoreHashNode* ENV;

#include <qore/QoreType.h>
#include <qore/intern/ParserSupport.h>
#include <qore/intern/QoreObjectIntern.h>
#include <qore/intern/QoreValue.h>

int qore_gvar_ref_u::write(ExceptionSink* xsink) const {
   if (_refptr & 1) {
      xsink->raiseException("ACCESS-ERROR", "attempt to write to read-only imported global variable '%s'", getPtr()->getName());
      return -1;
   }
   return 0;
}

int Var::getLValue(ExceptionSink* xsink, LValueHelper& lvh, bool for_remove) const {
   if (val.type == QV_Ref)
      return val.v.getPtr()->getLValue(xsink, lvh, for_remove);

   lvh.setTypeInfo(typeInfo);
   lvh.setAndLock(m);
   lvh.setValue((QoreValueGeneric&)val);
   return 0;
}

void Var::remove(LValueRemoveHelper& lvrh) {
   if (val.type == QV_Ref) {
      val.v.getPtr()->remove(lvrh);
      return;
   }

   AutoLocker al(m);
   lvrh.setRemove((QoreValueGeneric&)val);
}

void Var::del(ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      printd(4, "Var::~Var() refptr=%p\n", val.v.getPtr());
      val.v.getPtr()->deref(xsink);
      // clear type so no further deleting will be done
   }
   else
      discard(val.remove(true), xsink);
}

bool Var::isImported() const {
   return val.type == QV_Ref;
}

const char* Var::getName() const {
   return name.c_str();
}

AbstractQoreNode* Var::eval() const {
   if (val.type == QV_Ref)
      return val.v.getPtr()->eval();

   AutoLocker al(m);
   return val.eval();
}

AbstractQoreNode* Var::eval(bool &needs_deref) const {
   if (val.type == QV_Ref)
      return val.v.getPtr()->eval(needs_deref);

   
   AutoLocker al(m);
   return val.eval(needs_deref);
}

int64 Var::bigIntEval() const {
   if (val.type == QV_Ref)
      return val.v.getPtr()->bigIntEval();

   AutoLocker al(m);
   return val.getAsBigInt();
}

double Var::floatEval() const {
   if (val.type == QV_Ref)
      return val.v.getPtr()->floatEval();

   AutoLocker al(m);
   return val.getAsFloat();
}

void Var::deref(ExceptionSink* xsink) {
   if (ROdereference()) {
      del(xsink);
      delete this;
   }
}

LValueHelper::LValueHelper(const AbstractQoreNode* exp, ExceptionSink* xsink, bool for_remove) : vl(xsink), v(0), val(0), typeInfo(0) {
   doLValue(exp, for_remove);
}

static int var_type_err(const QoreTypeInfo* typeInfo, const char* type, ExceptionSink* xsink) {
   xsink->raiseException("RUNTIME-TYPE-ERROR", "cannot convert lvalue declared as %s to a %s", typeInfo->getName(), type);
   return -1;
}

int LValueHelper::doListLValue(const QoreTreeNode* tree, bool for_remove) {
   // first get index
   int ind = tree->right->integerEval(vl.xsink);
   if (*vl.xsink)
      return -1;
   if (ind < 0) {
      vl.xsink->raiseException("NEGATIVE-LIST-INDEX", "list index %d is invalid (index must evaluate to a non-negative integer)", ind);
      return -1;
   }

   // now get left hand side
   if (doLValue(tree->left, for_remove))
      return -1;

   if (!isNode())
      return for_remove ? -1 : var_type_err(typeInfo, "list", vl.xsink);

   AbstractQoreNode*& vp = getPtrRef();

   QoreListNode* l;
   if (get_node_type(vp) == NT_LIST) {
      l = reinterpret_cast<QoreListNode*>(vp);
      if (l->reference_count() > 1) {
         // otherwise if it's a list and the reference_count > 1, then duplicate it
         // save the old value for dereferencing outside any locks that may have been acquired
         //printd(5, "LValueHelper::doListLValue() this: %p saving old list: %p '%s'\n", this, l, get_type_name(l));
         saveTemp(l);
         l = l->copy();
      }
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
      saveTemp(vp);
      l = new QoreListNode;
   }

   vp = l;

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

   if (!isNode())
      return for_remove ? -1 : var_type_err(typeInfo, "hash", vl.xsink);

   AbstractQoreNode*& vp = getPtrRef();

   qore_type_t t = get_node_type(vp);
   QoreObject* o = t == NT_OBJECT ? reinterpret_cast<QoreObject*>(vp) : 0;
   QoreHashNode* h = 0;

   //printd(0, "LValueHelper::doHashObjLValue() h=%p v: %p ('%s', refs: %d)\n", h, v, v ? v->getTypeName() : "(null)", v ? v->reference_count() : 0);

   if (!o) {
      if (t == NT_HASH) {
         h = reinterpret_cast<QoreHashNode*>(vp);
         if (h->reference_count() > 1) {
            //printd(5, "LValueHelper::doHashObjLValue() this: %p saving hash to dereference: %p\n", this, h);
            // if the reference_count > 1 then duplicate it.
            saveTemp(h);
            h = h->copy();
         }
      }
      else {
         if (for_remove)
            return -1;

         // if the variable's value is not already a hash or an object, then make it a hash
         // but first make sure the lvalue can be converted to a hash
         if (!typeInfo->parseAcceptsReturns(NT_HASH))
            return var_type_err(typeInfo, "hash", vl.xsink);

         //printd(5, "LValueHelper::doHashObjLValue() this: %p saving value to dereference before making hash: %p '%s'\n", this, vp, get_type_name(vp));
         saveTemp(vp);
         h = new QoreHashNode;
      }
      vp = h;

      //printd(0, "LValueHelper::doHashObjLValue() def=%s member %s \"%s\"\n", QCS_DEFAULT->getCode(), mem->getEncoding()->getCode(), mem->getBuffer());
      resetPtr(h->getKeyValuePtr(mem->getBuffer()));
      return 0;
   }

   //printd(5, "LValueHelper::doHashObjLValue() obj: %p member: %s\n", o, mem->getBuffer());

   clearPtr();

   // true is for "internal" so that the member notification will be queued
   qore_object_private::getLValue(*o, mem->getBuffer(), *this, true, for_remove, vl.xsink);
   return *vl.xsink ? -1 : 0;
}

int LValueHelper::doLValue(const AbstractQoreNode* n, bool for_remove) {
   qore_type_t ntype = n->getType();
   //printd(0, "setup_lvalue(exp: %p) %s\n", n, n->getTypeName());
   if (ntype == NT_VARREF) {
      const VarRefNode* v = reinterpret_cast<const VarRefNode*>(n);
      //printd(5, "get_var_value_ptr(): vref=%s (%p)\n", v->name, v);
      return v->getLValue(vl.xsink, *this, for_remove);
   }
   else if (ntype == NT_SELF_VARREF) {
      const SelfVarrefNode* v = reinterpret_cast<const SelfVarrefNode*>(n);
      // note that getStackObject() is guaranteed to return a value here (self varref is only valid in a method)
      QoreObject* obj = getStackObject();
      assert(obj);
      // false is for "internal" so that no member notification will be queued
      qore_object_private::getLValue(*obj, v->str, *this, false, for_remove, vl.xsink);
      return *vl.xsink ? -1 : 0;
   }
   else if (ntype == NT_CLASS_VARREF) {
      const StaticClassVarRefNode* v = reinterpret_cast<const StaticClassVarRefNode*>(n);
      v->getLValue(*this);
      return 0;
   }

   assert(n->getType() == NT_TREE);
   // it must be a tree
   const QoreTreeNode* tree = reinterpret_cast<const QoreTreeNode*>(n);
   if (tree->getOp() == OP_LIST_REF)
      return doListLValue(tree, for_remove);
   return doHashObjLValue(tree, for_remove);
}

void LValueHelper::setAndLock(QoreThreadLock& m) {
   m.lock();
   vl.set(&m);
}

AbstractQoreNode* LValueHelper::getReferencedValue() const {
   if (val)
      return val->eval();
   return *v ? (*v)->refSelf() : 0;
}

const qore_type_t LValueHelper::getType() const {
   if (val)
      return val->getType();
   return get_node_type(*v);
}

const char* LValueHelper::getTypeName() const {
   if (val)
      return val->getTypeName();
   return get_type_name(*v);
}

int LValueHelper::assign(AbstractQoreNode* n, const char* desc) {
   // check type for assignment
   n = typeInfo->acceptAssignment(desc, n, vl.xsink);
   if (*vl.xsink) {
      //printd(5, "LValueHelper::assign() this: %p saving type-rejected value: %p '%s'\n", this, n, get_type_name(n));
      saveTemp(n);
      return -1;
   }

   if (val) {
      saveTemp(val->assign(n));
      return 0;
   }

   //printd(5, "LValueHelper::assign() this: %p saving old value: %p '%s'\n", this, *v, get_type_name(*v));
   saveTemp(*v);
   *v = n;
   return 0;
}

int LValueHelper::assignBigInt(int64 va, const char* desc) {
   // check type for assignment
   if (!typeInfo->parseAccepts(bigIntTypeInfo)) {
      typeInfo->doAcceptError(false, false, -1, desc, Zero, vl.xsink);
      return -1;
   }

   // type compatibility must have been checked at parse time
   if (val) {
      saveTemp(val->assign(va));
      return 0;
   }

   AbstractQoreNode* n = typeInfo->acceptAssignment(desc, new QoreBigIntNode(va), vl.xsink);
   if (*vl.xsink) {
      saveTemp(n);
      return -1;
   }

   saveTemp(*v);
   *v = n;
   return 0;
}

int LValueHelper::assignFloat(double va, const char* desc) {
   // check type for assignment
   if (!typeInfo->parseAccepts(floatTypeInfo)) {
      typeInfo->doAcceptError(false, false, -1, desc, ZeroFloat, vl.xsink);
      return -1;
   }

   // type compatibility must have been checked at parse time
   if (val) {
      saveTemp(val->assign(va));
      return 0;
   }

   AbstractQoreNode* n = typeInfo->acceptAssignment(desc, new QoreFloatNode(va), vl.xsink);
   if (*vl.xsink) {
      saveTemp(n);
      return -1;
   }

   saveTemp(*v);
   *v = n;
   return 0;
}

int64 LValueHelper::plusEqualsBigInt(int64 va, const char* desc) {
   if (val)
      return val->plusEqualsBigInt(va, getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val += va;
   return i->val;
}

int64 LValueHelper::minusEqualsBigInt(int64 va, const char* desc) {
   if (val)
      return val->minusEqualsBigInt(va, getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val -= va;
   return i->val;
}

int64 LValueHelper::multiplyEqualsBigInt(int64 va, const char* desc) {
   if (val)
      return val->multiplyEqualsBigInt(va, getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val *= va;
   return i->val;
}

int64 LValueHelper::divideEqualsBigInt(int64 va, const char* desc) {
   assert(va);

   if (val)
      return val->divideEqualsBigInt(va, getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val /= va;
   return i->val;
}

int64 LValueHelper::orEqualsBigInt(int64 va, const char* desc) {
   if (val)
      return val->orEqualsBigInt(va, getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val |= va;
   return i->val;
}

int64 LValueHelper::xorEqualsBigInt(int64 va, const char* desc) {
   if (val)
      return val->xorEqualsBigInt(va, getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val ^= va;
   return i->val;
}

int64 LValueHelper::modulaEqualsBigInt(int64 va, const char* desc) {
   if (val)
      return val->modulaEqualsBigInt(va, getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val %= va;
   return i->val;
}

int64 LValueHelper::andEqualsBigInt(int64 va, const char* desc) {
   if (val)
      return val->andEqualsBigInt(va, getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val &= va;
   return i->val;
}

int64 LValueHelper::shiftLeftEqualsBigInt(int64 va, const char* desc) {
   if (val)
      return val->shiftLeftEqualsBigInt(va, getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val <<= va;
   return i->val;
}

int64 LValueHelper::shiftRightEqualsBigInt(int64 va, const char* desc) {
   if (val)
      return val->shiftRightEqualsBigInt(va, getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   i->val >>= va;
   return i->val;
}

int64 LValueHelper::preIncrementBigInt(const char* desc) {
   if (val)
      return val->preIncrementBigInt(getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   return ++i->val;
}

int64 LValueHelper::preDecrementBigInt(const char* desc) {
   if (val)
      return val->preDecrementBigInt(getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   return --i->val;
}

int64 LValueHelper::postIncrementBigInt(const char* desc) {
   if (val)
      return val->postIncrementBigInt(getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   return i->val++;
}

int64 LValueHelper::postDecrementBigInt(const char* desc) {
   if (val)
      return val->postDecrementBigInt(getTempRef());

   // increment current value
   QoreBigIntNode* i = ensureUnique<QoreBigIntNode, int64, NT_INT>(bigIntTypeInfo, desc);
   if (!i)
      return 0;
   return i->val--;
}

double LValueHelper::preIncrementFloat(const char* desc) {
   if (val)
      return val->preIncrementFloat(getTempRef());

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0;
   return ++f->f;
}

double LValueHelper::preDecrementFloat(const char* desc) {
   if (val)
      return val->preDecrementFloat(getTempRef());

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0;
   return --f->f;
}

double LValueHelper::postIncrementFloat(const char* desc) {
   if (val)
      return val->postIncrementFloat(getTempRef());

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0;
   return f->f++;
}

double LValueHelper::postDecrementFloat(const char* desc) {
   if (val)
      return val->postDecrementFloat(getTempRef());

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0;
   return f->f--;
}

double LValueHelper::plusEqualsFloat(double va, const char* desc) {
   if (val)
      return val->plusEqualsFloat(va, getTempRef());

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0.0;
   f->f += va;
   return f->f;
}

double LValueHelper::minusEqualsFloat(double va, const char* desc) {
   if (val)
      return val->minusEqualsFloat(va, getTempRef());

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0.0;
   f->f -= va;
   return f->f;
}

double LValueHelper::multiplyEqualsFloat(double va, const char* desc) {
   if (val)
      return val->multiplyEqualsFloat(va, getTempRef());

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0.0;
   f->f *= va;
   return f->f;
}

double LValueHelper::divideEqualsFloat(double va, const char* desc) {
   assert(va);
   if (val)
      return val->divideEqualsFloat(va, getTempRef());

   // increment current value
   QoreFloatNode* f = ensureUnique<QoreFloatNode, double, NT_FLOAT>(floatTypeInfo, desc);
   if (!f)
      return 0.0;
   f->f /= va;
   return f->f;
}

int64 LValueHelper::removeBigInt() {
   if (val)
      return val->removeBigInt(getTempRef());

   int64 rv = *v ? (*v)->getAsBigInt() : 0;
   saveTemp(*v);
   *v = 0;
   return rv;
}

double LValueHelper::removeFloat() {
   if (val)
      return val->removeFloat(getTempRef());

   double rv = *v ? (*v)->getAsFloat() : 0;
   saveTemp(*v);
   *v = 0;
   return rv;
}

AbstractQoreNode* LValueHelper::remove(bool for_del) {
   if (val)
      return val->remove(for_del);

   AbstractQoreNode* rv = *v;
   *v = 0;
   return rv;
}

LValueRemoveHelper::LValueRemoveHelper(const AbstractQoreNode* exp, ExceptionSink* n_xsink, bool fd) : xsink(n_xsink), for_del(fd) {
   doRemove(const_cast<AbstractQoreNode*>(exp));
}

int64 LValueRemoveHelper::removeBigInt() {
   assert(!*xsink);
   ReferenceHolder<> dr(xsink);
   return rv.removeBigInt(dr.getRef());
}

double LValueRemoveHelper::removeFloat() {
   assert(!*xsink);
   ReferenceHolder<> dr(xsink);
   return rv.removeFloat(dr.getRef());
}

AbstractQoreNode* LValueRemoveHelper::remove() {
   assert(!*xsink);
   return rv.remove(for_del);
}

void LValueRemoveHelper::deleteLValue() {
   assert(!*xsink);
   assert(for_del);

   ReferenceHolder<> v(remove(), xsink);
   if (!v)
      return;

   qore_type_t t = v->getType();
   if (t != NT_OBJECT)
      return;

   QoreObject* o = reinterpret_cast<QoreObject* >(*v);
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
      rv.assignInitial(getStackObject()->takeMember(reinterpret_cast<SelfVarrefNode*>(lvalue)->str, xsink));
      return;
   }

   // must be a tree
   assert(t == NT_TREE);
   QoreTreeNode* tree = reinterpret_cast<QoreTreeNode*>(lvalue);

   // can be only a list or object (hash) reference

   // if it's a list reference, see if the reference exists, if so, then remove it
   if (tree->getOp() == OP_LIST_REF) {
      int offset = tree->right->integerEval(xsink);
      if (*xsink)
         return;

      LValueHelper lvhb(tree->left, xsink, true);
      if (!lvhb || lvhb.getType() != NT_LIST)
         return;

      lvhb.ensureUnique();
      QoreListNode* l = reinterpret_cast<QoreListNode*>(lvhb.getValue());
      // delete the value if it exists
      rv.assignInitial(l->swap(offset, 0));
      return;
   }
   assert(tree->getOp() == OP_OBJECT_REF);

   // get the member name or names
   QoreNodeEvalOptionalRefHolder member(tree->right, xsink);
   if (*xsink)
      return;

   // find variable ptr, exit if doesn't exist anyway
   LValueHelper lvhb(tree->left, xsink, true);
   if (!lvhb)
      return;

   t = lvhb.getType();
   QoreObject* o = t == NT_OBJECT ? reinterpret_cast<QoreObject* >(lvhb.getValue()) : 0;
   QoreHashNode* h = !o && t == NT_HASH ? reinterpret_cast<QoreHashNode*>(lvhb.getValue()) : 0;
   if (!o && !h)
      return;

   if (h)
      lvhb.ensureUnique();

   // remove a slice of the hash or object
   if (get_node_type(*member) == NT_LIST) {
      QoreHashNode* rvh = new QoreHashNode;
      rv.assignInitial(rvh);

      ConstListIterator li(reinterpret_cast<const QoreListNode*>(*member));
      while (li.next()) {
         QoreStringValueHelper mem(li.getValue(), QCS_DEFAULT, xsink);
         if (*xsink)
            return;

         AbstractQoreNode* n = o ? o->takeMember(mem->getBuffer(), xsink) : h->takeKeyValue(mem->getBuffer());
         if (*xsink)
            return;

         // note that no exception can occur here
         rvh->setKeyValue(mem->getBuffer(), n, xsink);
         assert(!*xsink);
      }
      return;
   }

   QoreStringValueHelper mem(*member, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   rv.assignInitial(o ? o->takeMember(mem->getBuffer(), xsink) : h->takeKeyValue(mem->getBuffer()));
}

lvar_ref::lvar_ref(AbstractQoreNode* n_vexp, QoreObject* n_obj, QoreProgram *n_pgm)
  : vexp(n_vexp), obj(n_obj), pgm(n_pgm), 
    is_vref(vexp->getType() == NT_VARREF && !reinterpret_cast<VarRefNode*>(vexp)->isGlobalVar()) {
   if (n_obj)
      n_obj->tRef();
}

int LocalVarValue::getLValue(ExceptionSink* xsink, LValueHelper& lvh, bool for_remove) const {
   if (val.type == QV_Ref) {
      if (val.v.ref->vexp->getType() == NT_VARREF) {
         // skip this entry in case it's a recursive reference
         VarStackPointerHelper<LocalVarValue> helper(const_cast<LocalVarValue*>(this));
         return lvh.doLValue(val.v.ref->vexp, for_remove);
      }
      return lvh.doLValue(val.v.ref->vexp, for_remove);
   }

   lvh.setValue((QoreValueGeneric&)val);
   return 0;
}

void LocalVarValue::remove(LValueRemoveHelper& lvrh) {
   if (val.type == QV_Ref) {
      if (val.v.ref->vexp->getType() == NT_VARREF) {
         // skip this entry in case it's a recursive reference
         VarStackPointerHelper<LocalVarValue> helper(const_cast<LocalVarValue*>(this));
         lvrh.doRemove(val.v.ref->vexp);
      }
      else
         lvrh.doRemove(val.v.ref->vexp);
   }
   else
      lvrh.setRemove((QoreValueGeneric&)val);
}

int ClosureVarValue::getLValue(ExceptionSink* xsink, LValueHelper& lvh, bool for_remove) const {
   if (val.type == QV_Ref) {
      if (val.v.ref->vexp->getType() == NT_VARREF) {
         assert(reinterpret_cast<VarRefNode*>(val.v.ref->vexp)->getType() != VT_LOCAL);
         // skip this entry in case it's a recursive reference
         VarStackPointerHelper<ClosureVarValue> helper(const_cast<ClosureVarValue*>(this));
         return lvh.doLValue(val.v.ref->vexp, for_remove);
      }
      return lvh.doLValue(val.v.ref->vexp, for_remove);
   }

   lvh.setAndLock(*const_cast<ClosureVarValue*>(this));
   lvh.setValue((QoreValueGeneric&)val);
   return 0;
}

void ClosureVarValue::remove(LValueRemoveHelper& lvrh) {
   if (val.type == QV_Ref) {
      if (val.v.ref->vexp->getType() == NT_VARREF) {
         // skip this entry in case it's a recursive reference
         VarStackPointerHelper<ClosureVarValue> helper(const_cast<ClosureVarValue*>(this));
         lvrh.doRemove(val.v.ref->vexp);
      }
      else
         lvrh.doRemove(val.v.ref->vexp);
   }
   else {
      AutoLocker al(this);
      lvrh.setRemove((QoreValueGeneric&)val);
   }
}
