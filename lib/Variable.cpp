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
QoreHashNode *ENV;

#include <qore/QoreType.h>
#include <qore/intern/ParserSupport.h>
#include <qore/intern/QoreObjectIntern.h>

int qore_gvar_ref_u::write(ExceptionSink* xsink) const {
   if (_refptr & 1) {
      xsink->raiseException("ACCESS-ERROR", "attempt to write to read-only imported global variable '%s'", getPtr()->getName());
      return -1;
   }
   return 0;
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

const char *Var::getName() const {
   return name.c_str();
}

AbstractQoreNode *Var::eval() const {
   if (val.type == QV_Ref)
      return val.v.getPtr()->eval();

   AutoLocker al(m);
   return val.eval();
}

AbstractQoreNode *Var::eval(bool &needs_deref) const {
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

int64 Var::removeBigInt(ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;

      return val.v.getPtr()->removeBigInt(xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.removeBigInt(old.getRef());
}

double Var::removeFloat(ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;

      return val.v.getPtr()->removeFloat(xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.removeFloat(old.getRef());
}

AbstractQoreNode* Var::remove(ExceptionSink* xsink, bool for_del) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;

      return val.v.getPtr()->remove(xsink, for_del);
   }

   AutoLocker al(m);
   return val.remove(for_del);
}

AbstractQoreNode **Var::getValuePtr(AutoVLock *vl, const QoreTypeInfo *&varTypeInfo, ObjMap &omap, ExceptionSink* xsink) const {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;

      return val.v.getPtr()->getValuePtr(vl, varTypeInfo, omap, xsink);
   }

   // note: unlocking the lock is managed with the AutoVLock object
   m.lock();
   vl->set(&m);
   varTypeInfo = typeInfo;
   return val.getValuePtr(xsink);
}

AbstractQoreNode **Var::getContainerValuePtr(AutoVLock *vl, const QoreTypeInfo *&varTypeInfo, ObjMap &omap, ExceptionSink* xsink) const {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;

      return val.v.getPtr()->getContainerValuePtr(vl, varTypeInfo, omap, xsink);
   }

   // note: unlocking the lock is managed with the AutoVLock object
   m.lock();
   vl->set(&m);
   varTypeInfo = typeInfo;
   return val.getContainerValuePtr();
}

void Var::assign(AbstractQoreNode *v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return;

      val.v.getPtr()->assign(v, xsink);
      return;
   }

   // must dereference old value outside the lock
   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   old = val.assign(v);
}

void Var::assignBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return;

      val.v.getPtr()->assignBigInt(v, xsink);
      return;
   }

   // must dereference old value outside the lock
   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   old = val.assign(v);
   return;
}

void Var::assignFloat(double v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return;

      val.v.getPtr()->assignFloat(v, xsink);
      return;
   }

   // must dereference old value outside the lock
   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   old = val.assign(v);
}

int64 Var::plusEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->plusEqualsBigInt(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.plusEqualsBigInt(v, old.getRef());
}

double Var::plusEqualsFloat(double v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->plusEqualsFloat(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.plusEqualsFloat(v, old.getRef());
}

int64 Var::minusEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->minusEqualsBigInt(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.minusEqualsBigInt(v, old.getRef());
}

double Var::minusEqualsFloat(double v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->minusEqualsFloat(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.minusEqualsFloat(v, old.getRef());
}

int64 Var::orEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->orEqualsBigInt(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.orEqualsBigInt(v, old.getRef());
}

int64 Var::andEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->andEqualsBigInt(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.andEqualsBigInt(v, old.getRef());
}

int64 Var::modulaEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->modulaEqualsBigInt(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.modulaEqualsBigInt(v, old.getRef());
}

int64 Var::multiplyEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->multiplyEqualsBigInt(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.multiplyEqualsBigInt(v, old.getRef());
}

int64 Var::divideEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->divideEqualsBigInt(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.divideEqualsBigInt(v, old.getRef());
}

int64 Var::xorEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->xorEqualsBigInt(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.xorEqualsBigInt(v, old.getRef());
}

int64 Var::shiftLeftEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->shiftLeftEqualsBigInt(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.shiftLeftEqualsBigInt(v, old.getRef());
}

int64 Var::shiftRightEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->shiftRightEqualsBigInt(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.shiftRightEqualsBigInt(v, old.getRef());
}

double Var::multiplyEqualsFloat(double v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->multiplyEqualsFloat(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.multiplyEqualsFloat(v, old.getRef());
}

double Var::divideEqualsFloat(double v, ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->divideEqualsFloat(v, xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.divideEqualsFloat(v, old.getRef());
}

int64 Var::postIncrement(ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->postIncrement(xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.postIncrement(old.getRef());
}

int64 Var::preIncrement(ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->preIncrement(xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.preIncrement(old.getRef());
}

int64 Var::postDecrement(ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->postDecrement(xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.postDecrement(old.getRef());
}

int64 Var::preDecrement(ExceptionSink* xsink) {
   if (val.type == QV_Ref) {
      if (val.v.write(xsink))
	 return 0;
      return val.v.getPtr()->preDecrement(xsink);
   }

   ReferenceHolder<> old(xsink);
   AutoLocker al(m);
   return val.preDecrement(old.getRef());
}

void Var::deref(ExceptionSink* xsink) {
   if (ROdereference()) {
      del(xsink);
      delete this;
   }
}

static AbstractQoreNode **do_list_val_ptr(const QoreTreeNode *tree, AutoVLock *vlp, ObjMap &omap, ExceptionSink* xsink) {
   // first get index
   int ind = tree->right->integerEval(xsink);
   if (xsink->isEvent())
      return 0;
   if (ind < 0) {
      xsink->raiseException("NEGATIVE-LIST-INDEX", "list index %d is invalid (index must evaluate to a non-negative integer)", ind);
      return 0;
   }

   const QoreTypeInfo *typeInfo = 0;

   // now get left hand side
   AbstractQoreNode **val = get_var_value_ptr(tree->left, vlp, typeInfo, omap, xsink);
   if (xsink->isEvent())
      return 0;

   // xxx check type

   // if the variable's value is not already a list, then make it one
   // printd(0, "index=%d val=%p (%s)\n", ind, *val, *val ? (*val)->getTypeName() : "(null)");
   QoreListNode *l;
   if (!(*val))
      l = new QoreListNode();
   else {
      if ((*val)->getType() != NT_LIST) {
	 (*val)->deref(xsink);
	 l = new QoreListNode();
      }
      else { 
	 if ((*val)->reference_count() > 1) { // otherwise if it's a list and the reference_count > 1, then duplicate it
	    l = reinterpret_cast<QoreListNode *>(*val);
	    QoreListNode *old = l;
	    l = l->copy();
	    old->deref(xsink);
	 }
	 else
	    l = reinterpret_cast<QoreListNode *>(*val);
      }
   }
   (*val) = l;

   return l->get_entry_ptr(ind);
}

// for objects and hashes
static AbstractQoreNode **do_object_val_ptr(const QoreTreeNode *tree, AutoVLock *vlp, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink* xsink) {
   QoreNodeEvalOptionalRefHolder member(tree->right, xsink);
   if (*xsink)
      return 0;

   QoreStringValueHelper mem(*member, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   const QoreTypeInfo *leftTypeInfo = 0;
   AbstractQoreNode **val = get_var_value_ptr(tree->left, vlp, leftTypeInfo, omap, xsink);
   if (*xsink)
      return 0;

   // xxx check type

   QoreHashNode *h = (*val && (*val)->getType() == NT_HASH) ? reinterpret_cast<QoreHashNode *>(*val) : 0;
   QoreObject *o = 0;

   //printd(0, "do_object_val_ptr() h=%p val=%p (%s, refs=%d)\n", h, *val, *val ? (*val)->getTypeName() : "(null)", *val ? (*val)->reference_count() : 0);

   // if the variable's value is not already a hash or an object, then make it a hash
   if (h) {
      if (h->reference_count() > 1) {
	 // otherwise if the reference_count > 1 (and it's not an object), then duplicate it.
	 QoreHashNode *oh = h;
	 h = h->copy();
	 oh->deref(xsink);
	 (*val) = h;
      }
   }
   else {
      o = (*val && (*val)->getType() == NT_OBJECT) ? reinterpret_cast<QoreObject *>(*val) : 0;
      if (!o) {
	 if (*val)
	    (*val)->deref(xsink);
	 // check assignment here against leftTypeInfo
	 if (!leftTypeInfo->parseAcceptsReturns(NT_HASH)) {
	    (*val) = 0;
	    xsink->raiseException("RUNTIME-TYPE-ERROR", "cannot convert lvalue declared as %s to a hash", leftTypeInfo->getName());
	    return 0;
	 }
	 h = new QoreHashNode;
	 (*val) = h;
      }
   }

   if (h) {
      //printd(0, "do_object_val_ptr() def=%s member %s \"%s\"\n", QCS_DEFAULT->getCode(), mem->getEncoding()->getCode(), mem->getBuffer());
      return h->getKeyValuePtr(mem->getBuffer());
   }

   //printd(5, "do_object_val_ptr() member=%s\n", mem->getBuffer());

   AbstractQoreNode **rv;
   if ((rv = qore_object_private::getMemberValuePtr(o, mem->getBuffer(), vlp, typeInfo, omap, xsink))) {
      vlp->addMemberNotification(o, mem->getBuffer());
      return rv;
   }

   if (*xsink)
      return 0;

   // if object has been deleted, then dereference, make into a hash, and get hash pointer

   (*val)->deref(xsink);
   // check assignment here against leftTypeInfo
   if (!leftTypeInfo->parseAcceptsReturns(NT_HASH)) {
      (*val) = 0;
      xsink->raiseException("RUNTIME-TYPE-ERROR", "cannot convert lvalue declared as %s to a hash", leftTypeInfo->getName());
      return 0;
   }

   h = new QoreHashNode;
   (*val) = h; 
   return h->getKeyValuePtr(mem->getBuffer());
}

// this function will change the lvalue to the right type if needed (used for assignments)
AbstractQoreNode **get_var_value_ptr(const AbstractQoreNode *n, AutoVLock *vlp, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink* xsink) {
   qore_type_t ntype = n->getType();
   //printd(0, "get_var_value_ptr(%p) %s\n", n, n->getTypeName());
   if (ntype == NT_VARREF) {
      const VarRefNode *v = reinterpret_cast<const VarRefNode *>(n);
      //printd(5, "get_var_value_ptr(): vref=%s (%p)\n", v->name, v);
      return v->getValuePtr(vlp, typeInfo, omap, xsink);
   }
   else if (ntype == NT_SELF_VARREF) {
      const SelfVarrefNode *v = reinterpret_cast<const SelfVarrefNode *>(n);
      // need to check for deleted objects
      // note that getStackObject() is guaranteed to return a value here (self varref is only valid in a method)

      // xxx add type info to call
      QoreObject *obj = getStackObject();
      AbstractQoreNode **rv = qore_object_private::getMemberValuePtr(obj, v->str, vlp, typeInfo, omap, xsink);
      if (!rv && !xsink->isException())
	 xsink->raiseException("OBJECT-ALREADY-DELETED", "write attempted to member \"%s\" in an already-deleted object", v->str);
      return rv;
   }
   else if (ntype == NT_CLASS_VARREF) {
      const StaticClassVarRefNode *v = reinterpret_cast<const StaticClassVarRefNode *>(n);
      return v->getValuePtr(*vlp, typeInfo);
   }

   assert(n->getType() == NT_TREE);
   // it must be a tree
   const QoreTreeNode *tree = reinterpret_cast<const QoreTreeNode *>(n);
   if (tree->getOp() == OP_LIST_REF)
      return do_list_val_ptr(tree, vlp, omap, xsink);
   return do_object_val_ptr(tree, vlp, typeInfo, omap, xsink);
}

// finds object value pointers without making any changes to the referenced structures
// will *not* execute memberGate methods
AbstractQoreNode *getExistingVarValue(const AbstractQoreNode *n, ExceptionSink* xsink) {
   printd(5, "getExistingVarValue(%p) %s\n", n, n->getTypeName());
   qore_type_t ntype = n->getType();
   if (ntype == NT_VARREF)
      return reinterpret_cast<const VarRefNode *>(n)->eval(xsink);

   if (ntype == NT_SELF_VARREF)
      return getStackObject()->getReferencedMemberNoMethod(reinterpret_cast<const SelfVarrefNode *>(n)->str, xsink);

   // it's a variable reference tree
   const QoreTreeNode *tree = ntype == NT_TREE ? reinterpret_cast<const QoreTreeNode *>(n) : 0;
   if (tree && (tree->getOp() == OP_LIST_REF || tree->getOp() == OP_OBJECT_REF)) {
      ReferenceHolder<AbstractQoreNode> val(getExistingVarValue(tree->left, xsink), xsink);
      if (!val)
	 return 0;

      // if it's a list reference
      if (tree->getOp() == OP_LIST_REF) {
	 // if it's not a list then return 0
	 if (val->getType() != NT_LIST)
	    return 0;

	 // otherwise return value
	 return reinterpret_cast<QoreListNode *>(*val)->get_referenced_entry(tree->right->integerEval(xsink));
      }
      
      // if it's an object reference
      assert(tree->getOp() == OP_OBJECT_REF);

      QoreHashNode *h = val->getType() == NT_HASH ? reinterpret_cast<QoreHashNode *>(*val) : 0;
      QoreObject *o;
      if (!h)
	 o = val->getType() == NT_OBJECT ? reinterpret_cast<QoreObject *>(*val) : 0;
      else
	 o = 0;
      
      // if not an object or a hash, return 0
      if (!o && !h)
	 return 0;
      
      // otherwise evaluate member
      QoreNodeEvalOptionalRefHolder member(tree->right, xsink);
      if (*xsink)
	 return 0;
      
      QoreStringValueHelper mem(*member, QCS_DEFAULT, xsink);
      if (*xsink)
	 return 0;
      
      return h ? h->getReferencedKeyValue(mem->getBuffer()) : o->getReferencedMemberNoMethod(mem->getBuffer(), xsink);
   }
   
   // otherwise need to evaluate
   return n->eval(xsink);
}

static AbstractQoreNode **check_unique(AbstractQoreNode **p, ExceptionSink* xsink) {
   if (p && *p && (*p)->reference_count() > 1 && (*p)->getType() != NT_OBJECT) {
      AbstractQoreNode *op = *p;
      *p = (*p)->realCopy();
      op->deref(xsink);
   }
   return p;
}

// only returns a value ptr if the expression points to an already-existing value
// (i.e. does not create entries anywhere)
// needed for deletes
static AbstractQoreNode **get_unique_existing_container_value_ptr(AbstractQoreNode *n, ExceptionSink* xsink, AutoVLock *vl, ObjMap &omap) {
   printd(5, "get_unique_existing_container_value_ptr(%p) %s\n", n, n->getTypeName());
   qore_type_t ntype = n->getType();
   const QoreTypeInfo *typeInfo = 0;
   if (ntype == NT_VARREF)
      return check_unique(reinterpret_cast<VarRefNode *>(n)->getContainerValuePtr(vl, typeInfo, omap, xsink), xsink);

   // getStackObject() will always return a value here (self refs are only legal in methods)
   if (ntype == NT_SELF_VARREF) {
      QoreObject *obj = getStackObject();
      return check_unique(obj->getExistingValuePtr(reinterpret_cast<SelfVarrefNode *>(n)->str, vl, xsink), xsink);
   }

   // it's a variable reference tree
   QoreTreeNode *tree = ntype == NT_TREE ? reinterpret_cast<QoreTreeNode *>(n) : 0;

   assert(tree && (tree->getOp() == OP_LIST_REF || tree->getOp() == OP_OBJECT_REF));

   AbstractQoreNode **val = get_unique_existing_container_value_ptr(tree->left, xsink, vl, omap);
   if (!val || !(*val))
      return 0;

   // if it's a list reference
   if (tree->getOp() == OP_LIST_REF) {
      if ((*val)->getType() != NT_LIST)  // if it's not a list then return 0
	 return 0;
      // otherwise return value
      return check_unique(reinterpret_cast<QoreListNode *>(*val)->getExistingEntryPtr(tree->right->integerEval(xsink)), xsink);
   }
   
   QoreHashNode *h = (*val)->getType() == NT_HASH   ? reinterpret_cast<QoreHashNode *>(*val) : 0;
   QoreObject *o   = (*val)->getType() == NT_OBJECT ? reinterpret_cast<QoreObject *>(*val)   : 0;
   
   // must be an object reference
   // if not an object or a hash, return 0
   if (!o && !h)
      return 0;
   
   // otherwise evaluate member
   QoreNodeEvalOptionalRefHolder member(tree->right, xsink);
   if (*xsink)
      return 0;
   
   QoreStringValueHelper mem(*member, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   if (h)
      return check_unique(h->getExistingValuePtr(mem->getBuffer()), xsink);

   AbstractQoreNode **rv = check_unique(o->getExistingValuePtr(mem->getBuffer(), vl, xsink), xsink);
   if (rv)
      vl->addMemberNotification(o, mem->getBuffer());

   return rv;
}

static AbstractQoreNode *remove_hash_object_value(QoreHashNode *h, QoreObject *o, const char *mem, ExceptionSink* xsink) {
   // otherwise if not a hash or object then exit
   if (o)
      return o->takeMember(mem, xsink);

   // if it's a hash reference, then remove the key
   return h->takeKeyValue(mem);
}

AbstractQoreNode *remove_lvalue_intern(ExceptionSink* xsink, AbstractQoreNode *lvalue, AutoVLock& vl, qore_type_t lvtype, ObjMap& omap) {
   if (lvtype == NT_SELF_VARREF)
      return getStackObject()->takeMember(reinterpret_cast<SelfVarrefNode *>(lvalue)->str, xsink);

   // must be a tree
   assert(lvtype == NT_TREE);
   QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(lvalue);

   // can be only a list or object (hash) reference

   // if it's a list reference, see if the reference exists, if so, then remove it
   if (tree->getOp() == OP_LIST_REF) {
      int offset = tree->right->integerEval(xsink);
      if (*xsink)
	 return 0;

      // find variable ptr, exit if doesn't exist anyway
      AbstractQoreNode **val = get_unique_existing_container_value_ptr(tree->left, xsink, &vl, omap);
      
      if (!val || !(*val) || (*val)->getType() != NT_LIST || *xsink)
	 return 0;

      QoreListNode *l = reinterpret_cast<QoreListNode *>(*val);
      // delete the value if it exists
      return l->swap(offset, 0);
   }
   assert(tree->getOp() == OP_OBJECT_REF);

   // get the member name or names
   QoreNodeEvalOptionalRefHolder member(tree->right, xsink);
   if (*xsink)
      return 0;

   // find variable ptr, exit if doesn't exist anyway
   AbstractQoreNode **val = get_unique_existing_container_value_ptr(tree->left, xsink, &vl, omap);      
   if (!val || !(*val) || *xsink)
      return 0;

   QoreObject *o = (*val)->getType() == NT_OBJECT ? reinterpret_cast<QoreObject *>(*val) : 0;
   QoreHashNode *h = !o && (*val)->getType() == NT_HASH ? reinterpret_cast<QoreHashNode *>(*val) : 0;
   if (!o && !h)
      return 0;

   if (get_node_type(*member) == NT_LIST) {
      ReferenceHolder<QoreHashNode> rv(new QoreHashNode, xsink);

      ConstListIterator li(reinterpret_cast<const QoreListNode *>(*member));
      while (li.next()) {
	 QoreStringValueHelper mem(li.getValue(), QCS_DEFAULT, xsink);
	 if (*xsink)
	    return 0;

	 AbstractQoreNode *n = remove_hash_object_value(h, o, mem->getBuffer(), xsink);
	 if (*xsink)
	    return 0;

	 rv->setKeyValue(mem->getBuffer(), n, xsink);
      }

      return rv.release();
   }

   QoreStringValueHelper mem(*member, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   return remove_hash_object_value(h, o, mem->getBuffer(), xsink);
}

AbstractQoreNode *remove_lvalue(AbstractQoreNode *lvalue, ExceptionSink* xsink, bool for_del) {
   AutoVLock vl(xsink);
   qore_type_t lvtype = lvalue->getType();

   // FIXME: add logic to detect breaking a recursive reference
   ObjMap omap;

   // if the node is a variable reference, then find value ptr, set it to 0, and return the value
   if (lvtype == NT_VARREF)
      return reinterpret_cast<VarRefNode *>(lvalue)->remove(xsink, for_del);

   return remove_lvalue_intern(xsink, lvalue, vl, lvtype, omap);
}

DLLLOCAL int64 remove_lvalue_bigint(AbstractQoreNode *lvalue, ExceptionSink* xsink) {
   AutoVLock vl(xsink);
   qore_type_t lvtype = lvalue->getType();

   // FIXME: add logic to detect breaking a recursive reference
   ObjMap omap;

   // if the node is a variable reference, then find value ptr, set it to 0, and return the value
   if (lvtype == NT_VARREF) 
      reinterpret_cast<VarRefNode *>(lvalue)->removeBigInt(xsink);

   ReferenceHolder<> rv(remove_lvalue_intern(xsink, lvalue, vl, lvtype, omap), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

DLLLOCAL double remove_lvalue_float(AbstractQoreNode *lvalue, ExceptionSink* xsink) {
   AutoVLock vl(xsink);
   qore_type_t lvtype = lvalue->getType();

   // FIXME: add logic to detect breaking a recursive reference
   ObjMap omap;

   // if the node is a variable reference, then find value ptr, set it to 0, and return the value
   if (lvtype == NT_VARREF) 
      reinterpret_cast<VarRefNode *>(lvalue)->removeFloat(xsink);

   ReferenceHolder<> rv(remove_lvalue_intern(xsink, lvalue, vl, lvtype, omap), xsink);
   return rv ? rv->getAsFloat() : 0.0;
}

void delete_lvalue(AbstractQoreNode *lvalue, ExceptionSink* xsink) {
   ReferenceHolder<AbstractQoreNode> v(remove_lvalue(lvalue, xsink, true), xsink);
   if (!v)
      return;

   qore_type_t t = v->getType();
   if (t == NT_OBJECT) {
      QoreObject *o = reinterpret_cast<QoreObject *>(*v);
      if (o->isSystemObject())
	 xsink->raiseException("SYSTEM-OBJECT-ERROR", "you cannot delete a system constant object");
      else
	 o->doDelete(xsink);
      return;
   }
}

LValueHelper::LValueHelper(const AbstractQoreNode *exp, ExceptionSink *n_xsink) : xsink(n_xsink), typeInfo(0), lvt(LVT_Unknown)  {
   qore_type_t t = exp->getType();
   // make sure it's an optimized var
   if (t == NT_VARREF && (lv.v = reinterpret_cast<const VarRefNode *>(exp)->isOptimized(typeInfo))) {
      //lv.v = (VarRefNode*)exp;
      //typeInfo = lv.v->getTypeInfo();
      lvt = LVT_OptVarRef;
   }
   else {
      lvt = LVT_Expr;
      lv.n = new LValueExpressionHelper(exp, typeInfo, xsink);
   }         
}

AbstractQoreNode *LValueHelper::getReferencedValue() const {
   return lvt == LVT_OptVarRef ? lv.v->eval(xsink) : lv.n->getReferencedValue();
}

const qore_type_t LValueHelper::get_type() const {
   return lvt == LVT_OptVarRef ? lv.v->getValueType() : *(lv.n->v) ? (*(lv.n->v))->getType() : NT_NOTHING; 
}

const char* LValueHelper::get_type_name() const {
   return lvt == LVT_OptVarRef ? lv.v->getValueTypeName() : ::get_type_name(*(lv.n->v)); 
}

int LValueHelper::assign(AbstractQoreNode *val, const char *desc) {
   // check type for assignment
   val = typeInfo->acceptAssignment(desc, val, xsink);
   if (*xsink) {
      discard(val, xsink);
      return -1;
   }

   if (lvt == LVT_OptVarRef) {
      lv.v->assign(val, xsink);
      return *xsink;
   }

   return lv.n->assign(val, xsink);
}

int LValueHelper::assignBigInt(int64 v, const char *desc) {
   // check type for assignment
   if (!typeInfo->parseAccepts(bigIntTypeInfo)) {
      typeInfo->doAcceptError(false, false, -1, desc, Zero, xsink);
      return -1;
   }

   // type compatibility must have been checked as parse time
   if (lvt == LVT_OptVarRef) {
      lv.v->assignBigInt(v, xsink);
      return *xsink;
   }

   AbstractQoreNode *val = new QoreBigIntNode(v);
   val = typeInfo->acceptAssignment(desc, val, xsink);
   if (*xsink) {
      discard(val, xsink);
      return -1;
   }

   return lv.n->assign(val, xsink);
}

int LValueHelper::assignFloat(double v, const char *desc) {
   // check type for assignment
   if (!typeInfo->parseAccepts(floatTypeInfo)) {
      typeInfo->doAcceptError(false, false, -1, desc, ZeroFloat, xsink);
      return -1;
   }

   if (lvt == LVT_OptVarRef) {
      // since we are only dealing with optimized local vars, it's not possible
      // to have an exception here on the assignment
      lv.v->assignFloat(v, xsink);
      assert(!*xsink);
      return 0;
   }

   AbstractQoreNode *val = new QoreFloatNode(v);
   val = typeInfo->acceptAssignment(desc, val, xsink);
   if (*xsink) {
      discard(val, xsink);
      return -1;
   }

   return lv.n->assign(val, xsink);
}

int64 LValueHelper::plusEqualsBigInt(int64 v) {
   if (lvt == LVT_OptVarRef)
      return lv.v->plusEqualsBigInt(v, xsink);

   // get new value if necessary
   if (ensure_unique_int())
      return 0;
   QoreBigIntNode *i = reinterpret_cast<QoreBigIntNode *>(get_value());
   // increment current value
   i->val += v;
   return i->val;
}

int64 LValueHelper::minusEqualsBigInt(int64 v) {
   if (lvt == LVT_OptVarRef)
      return lv.v->minusEqualsBigInt(v, xsink);

   // get new value if necessary
   if (ensure_unique_int())
      return 0;
   QoreBigIntNode *i = reinterpret_cast<QoreBigIntNode *>(get_value());
   // increment current value
   i->val -= v;
   return i->val;
}

int64 LValueHelper::multiplyEqualsBigInt(int64 v) {
   if (lvt == LVT_OptVarRef)
      return lv.v->multiplyEqualsBigInt(v, xsink);

   // get new value if necessary
   if (ensure_unique_int())
      return 0;
   QoreBigIntNode *i = reinterpret_cast<QoreBigIntNode *>(get_value());
   // increment current value
   i->val *= v;
   return i->val;
}

int64 LValueHelper::divideEqualsBigInt(int64 v) {
   assert(v);
   if (lvt == LVT_OptVarRef)
      return lv.v->divideEqualsBigInt(v, xsink);

   // get new value if necessary
   if (ensure_unique_int())
      return 0;
   QoreBigIntNode *i = reinterpret_cast<QoreBigIntNode *>(get_value());
   // increment current value
   i->val /= v;
   return i->val;
}

double LValueHelper::plusEqualsFloat(double v) {
   if (lvt == LVT_OptVarRef)
      return lv.v->plusEqualsFloat(v, xsink);

   // get new value if necessary
   if (ensure_unique_float())
      return 0.0;
   QoreFloatNode *i = reinterpret_cast<QoreFloatNode *>(get_value());
   // increment current value
   i->f += v;
   return i->f;
}

double LValueHelper::minusEqualsFloat(double v) {
   if (lvt == LVT_OptVarRef)
      return lv.v->minusEqualsFloat(v, xsink);

   // get new value if necessary
   if (ensure_unique_float())
      return 0.0;
   QoreFloatNode *i = reinterpret_cast<QoreFloatNode *>(get_value());
   // increment current value
   i->f -= v;
   return i->f;
}

double LValueHelper::multiplyEqualsFloat(double v) {
   if (lvt == LVT_OptVarRef)
      return lv.v->multiplyEqualsFloat(v, xsink);

   // get new value if necessary
   if (ensure_unique_float())
      return 0;
   QoreFloatNode *i = reinterpret_cast<QoreFloatNode *>(get_value());
   // increment current value
   i->f *= v;
   return i->f;
}

double LValueHelper::divideEqualsFloat(double v) {
   assert(v);
   if (lvt == LVT_OptVarRef)
      return lv.v->divideEqualsFloat(v, xsink);

   // get new value if necessary
   if (ensure_unique_float())
      return 0;
   QoreFloatNode *i = reinterpret_cast<QoreFloatNode *>(get_value());
   // increment current value
   i->f /= v;
   return i->f;
}

lvar_ref::lvar_ref(AbstractQoreNode *n_vexp, QoreObject *n_obj, QoreProgram *n_pgm) 
  : vexp(n_vexp), obj(n_obj), pgm(n_pgm), 
    is_vref(vexp->getType() == NT_VARREF && !reinterpret_cast<VarRefNode*>(vexp)->isGlobalVar()) {
   if (n_obj)
      n_obj->tRef();
}

template <class T>
int64 lvar_ref::postIncrement(T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->postIncrement(xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   int64 rv = n->val;
   ++n->val;
   return rv;
}

template <class T>
int64 lvar_ref::preIncrement(T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->preIncrement(xsink);
   }   
   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
   
   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   return ++n->val;
}

template <class T>
int64 lvar_ref::postDecrement(T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->postDecrement(xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;

   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   int64 rv = n->val;
   --n->val;
   return rv;
}

template <class T>
int64 lvar_ref::preDecrement(T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->preDecrement(xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;

   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   return --n->val;
}

template <class T>
int64 lvar_ref::plusEqualsBigInt(int64 v, T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->plusEqualsBigInt(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;

   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   n->val += v;
   return n->val;
}

template <class T>
double lvar_ref::plusEqualsFloat(double v, T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->plusEqualsFloat(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;

   valp->ensure_unique_float();
   QoreFloatNode *n = reinterpret_cast<QoreFloatNode *>(valp->get_value());
   n->f += v;
   return n->f;
}

template <class T>
int64 lvar_ref::minusEqualsBigInt(int64 v, T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->minusEqualsBigInt(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   n->val -= v;
   return n->val;
}

template <class T>
double lvar_ref::minusEqualsFloat(double v, T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->minusEqualsFloat(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;

   valp->ensure_unique_float();
   QoreFloatNode *n = reinterpret_cast<QoreFloatNode *>(valp->get_value());
   n->f -= v;
   return n->f;
}

template <class T>
int64 lvar_ref::orEqualsBigInt(int64 v, T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->orEqualsBigInt(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   n->val |= v;
   return n->val;
}

template <class T>
int64 lvar_ref::andEqualsBigInt(int64 v, T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->andEqualsBigInt(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   n->val &= v;
   return n->val;
}

template <class T>
int64 lvar_ref::modulaEqualsBigInt(int64 v, T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->modulaEqualsBigInt(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   if (v)
      n->val %= v;
   else
      n->val = 0;
   return n->val;
}

template <class T>
int64 lvar_ref::multiplyEqualsBigInt(int64 v, T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->multiplyEqualsBigInt(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   n->val *= v;
   return n->val;
}

template <class T>
int64 lvar_ref::divideEqualsBigInt(int64 v, T *vv, ExceptionSink* xsink) {
   assert(v);
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->divideEqualsBigInt(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   n->val /= v;
   return n->val;
}

template <class T>
int64 lvar_ref::xorEqualsBigInt(int64 v, T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->xorEqualsBigInt(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   n->val ^= v;
   return n->val;
}

template <class T>
int64 lvar_ref::shiftLeftEqualsBigInt(int64 v, T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->shiftLeftEqualsBigInt(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   n->val <<= v;
   return n->val;
}

template <class T>
int64 lvar_ref::shiftRightEqualsBigInt(int64 v, T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->shiftRightEqualsBigInt(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   n->val >>= v;
   return n->val;
}

template <class T>
double lvar_ref::multiplyEqualsFloat(double v, T *vv, ExceptionSink* xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->multiplyEqualsFloat(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_float();
   QoreFloatNode *n = reinterpret_cast<QoreFloatNode *>(valp->get_value());
   n->f *= v;
   return n->f;
}

template <class T>
double lvar_ref::divideEqualsFloat(double v, T *vv, ExceptionSink* xsink) {
   assert(v);
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->divideEqualsFloat(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_float();
   QoreFloatNode *n = reinterpret_cast<QoreFloatNode *>(valp->get_value());
   n->f /= v;
   return n->f;
}

bool LocalVarValue::isOptimized(const QoreTypeInfo*& varTypeInfo) const {
   if (val.type == QV_Ref)
      return val.v.ref->vexp->getType() == NT_VARREF ? reinterpret_cast<VarRefNode*>(val.v.ref->vexp)->isOptimized(varTypeInfo) : false;
   
   return val.optimized() ? true : false;
}

int64 LocalVarValue::plusEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->plusEqualsBigInt<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.plusEqualsBigInt(v, old.getRef());
}

double LocalVarValue::plusEqualsFloat(double v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->plusEqualsFloat<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.plusEqualsFloat(v, old.getRef());
}

int64 LocalVarValue::minusEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->minusEqualsBigInt<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.minusEqualsBigInt(v, old.getRef());
}

double LocalVarValue::minusEqualsFloat(double v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->minusEqualsFloat<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.minusEqualsFloat(v, old.getRef());
}

int64 LocalVarValue::orEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->orEqualsBigInt<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.orEqualsBigInt(v, old.getRef());
}

int64 LocalVarValue::andEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->andEqualsBigInt<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.andEqualsBigInt(v, old.getRef());
}

int64 LocalVarValue::modulaEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->modulaEqualsBigInt<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.modulaEqualsBigInt(v, old.getRef());
}

int64 LocalVarValue::multiplyEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->multiplyEqualsBigInt<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.multiplyEqualsBigInt(v, old.getRef());
}

int64 LocalVarValue::divideEqualsBigInt(int64 v, ExceptionSink* xsink) {
   assert(v);
   if (val.type == QV_Ref)
      return val.v.ref->divideEqualsBigInt<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.divideEqualsBigInt(v, old.getRef());
}

int64 LocalVarValue::xorEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->xorEqualsBigInt<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.xorEqualsBigInt(v, old.getRef());
}

int64 LocalVarValue::shiftLeftEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->shiftLeftEqualsBigInt<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.shiftLeftEqualsBigInt(v, old.getRef());
}

int64 LocalVarValue::shiftRightEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->shiftRightEqualsBigInt<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.shiftRightEqualsBigInt(v, old.getRef());
}

int64 LocalVarValue::postIncrement(ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->postIncrement<LocalVarValue>(this, xsink);

   ReferenceHolder<> old(xsink);
   return val.postIncrement(old.getRef());
}

int64 LocalVarValue::preIncrement(ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->preIncrement<LocalVarValue>(this, xsink);

   ReferenceHolder<> old(xsink);
   return val.preIncrement(old.getRef());
}

int64 LocalVarValue::postDecrement(ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->postDecrement<LocalVarValue>(this, xsink);

   ReferenceHolder<> old(xsink);
   return val.postDecrement(old.getRef());
}

int64 LocalVarValue::preDecrement(ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->preDecrement<LocalVarValue>(this, xsink);

   ReferenceHolder<> old(xsink);
   return val.preDecrement(old.getRef());
}

double LocalVarValue::multiplyEqualsFloat(double v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->multiplyEqualsFloat<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.multiplyEqualsFloat(v, old.getRef());
}

double LocalVarValue::divideEqualsFloat(double v, ExceptionSink* xsink) {
   assert(v);
   if (val.type == QV_Ref)
      return val.v.ref->divideEqualsFloat<LocalVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   return val.divideEqualsFloat(v, old.getRef());
}

bool ClosureVarValue::isOptimized(const QoreTypeInfo*& varTypeInfo) const {
   if (val.type == QV_Ref)
      return val.v.ref->vexp->getType() == NT_VARREF ? reinterpret_cast<VarRefNode*>(val.v.ref->vexp)->isOptimized(varTypeInfo) : false;

   if (val.optimized()) {
      varTypeInfo = typeInfo;
      return true;
   }
   return false;
}

int64 ClosureVarValue::plusEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->plusEqualsBigInt<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.plusEqualsBigInt(v, old.getRef());
}

double ClosureVarValue::plusEqualsFloat(double v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->plusEqualsFloat<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.plusEqualsFloat(v, old.getRef());
}

int64 ClosureVarValue::minusEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->minusEqualsBigInt<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.minusEqualsBigInt(v, old.getRef());
}

double ClosureVarValue::minusEqualsFloat(double v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->minusEqualsFloat<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.minusEqualsFloat(v, old.getRef());
}

int64 ClosureVarValue::orEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->orEqualsBigInt<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.orEqualsBigInt(v, old.getRef());
}

int64 ClosureVarValue::andEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->andEqualsBigInt<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.andEqualsBigInt(v, old.getRef());
}

int64 ClosureVarValue::modulaEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->modulaEqualsBigInt<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.modulaEqualsBigInt(v, old.getRef());
}

int64 ClosureVarValue::multiplyEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->multiplyEqualsBigInt<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.multiplyEqualsBigInt(v, old.getRef());
}

int64 ClosureVarValue::divideEqualsBigInt(int64 v, ExceptionSink* xsink) {
   assert(v);
   if (val.type == QV_Ref)
      return val.v.ref->divideEqualsBigInt<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.divideEqualsBigInt(v, old.getRef());
}

int64 ClosureVarValue::xorEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->xorEqualsBigInt<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.xorEqualsBigInt(v, old.getRef());
}

int64 ClosureVarValue::shiftLeftEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->shiftLeftEqualsBigInt<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.shiftLeftEqualsBigInt(v, old.getRef());
}

int64 ClosureVarValue::shiftRightEqualsBigInt(int64 v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->shiftRightEqualsBigInt<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.shiftRightEqualsBigInt(v, old.getRef());
}

int64 ClosureVarValue::postIncrement(ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->postIncrement<ClosureVarValue>(this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.postIncrement(old.getRef());
}

int64 ClosureVarValue::preIncrement(ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->preIncrement<ClosureVarValue>(this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.preIncrement(old.getRef());
}

int64 ClosureVarValue::postDecrement(ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->postDecrement<ClosureVarValue>(this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.postDecrement(old.getRef());
}

int64 ClosureVarValue::preDecrement(ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->preDecrement<ClosureVarValue>(this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.preDecrement(old.getRef());
}

double ClosureVarValue::multiplyEqualsFloat(double v, ExceptionSink* xsink) {
   if (val.type == QV_Ref)
      return val.v.ref->multiplyEqualsFloat<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.multiplyEqualsFloat(v, old.getRef());
}

double ClosureVarValue::divideEqualsFloat(double v, ExceptionSink* xsink) {
   assert(v);
   if (val.type == QV_Ref)
      return val.v.ref->divideEqualsFloat<ClosureVarValue>(v, this, xsink);

   ReferenceHolder<> old(xsink);
   AutoLocker al(this);
   return val.divideEqualsFloat(v, old.getRef());
}
