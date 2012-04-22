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

VarValue::VarValue(Var *n_refptr, bool n_readonly) {
   ivar.refptr = n_refptr;
   ivar.readonly = n_readonly;
   ivar.refptr->ROreference();
}

/*
ScopedObjectCallNode *Var::makeNewCall(AbstractQoreNode *args) const {
   if (type == GV_IMPORT)
      return v.ivar.refptr->makeNewCall(args);

   const QoreClass *qc = typeInfo->getUniqueReturnClass();
   if (qc)
      return new ScopedObjectCallNode(qc, makeArgs(args));
   if (parseTypeInfo && parseTypeInfo->cscope)
      return new ScopedObjectCallNode(parseTypeInfo->cscope->copy(), makeArgs(args));
   return 0;
}
*/

void Var::del(ExceptionSink *xsink) {
   if (type == GV_IMPORT) {
      printd(4, "Var::~Var() refptr=%p\n", v.ivar.refptr);
      v.ivar.refptr->deref(xsink);
      // clear type so no further deleting will be done
   }
   else {
      printd(4, "Var::~Var() name=%s value=%p type=%s refs=%d\n", name.c_str(),
	     v.val.value, v.val.value ? v.val.value->getTypeName() : "null", 
	     v.val.value ? v.val.value->reference_count() : 0);
 
      if (v.val.value)
	 v.val.value->deref(xsink);
      // clear type so no further deleting will be done
   }
}

bool Var::isImported() const {
   return type == GV_IMPORT;
}

const char *Var::getName() const {
   return name.c_str();
}

AbstractQoreNode *Var::evalIntern(ExceptionSink *xsink) {
   if (type == GV_IMPORT) {
      // perform lock handoff
      m.unlock();
      v.ivar.refptr->m.lock();

      return v.ivar.refptr->evalIntern(xsink);
   }

   AbstractQoreNode *rv = v.val.value;
   if (rv)
      rv->ref();
   //printd(5, "Var::eval() this=%p val=%p (%s)\n", this, rv, rv ? rv->getTypeName() : "(null)");

   m.unlock();
   return rv;
}

AbstractQoreNode *Var::eval(ExceptionSink *xsink) {
   m.lock();

   return evalIntern(xsink);
}

// note: unlocking the lock is managed with the AutoVLock object
AbstractQoreNode **Var::getValuePtrIntern(AutoVLock *vl, const QoreTypeInfo *&varTypeInfo, ExceptionSink *xsink) const {
   if (type == GV_IMPORT) {
      if (v.ivar.readonly) {
	 m.unlock();
	 xsink->raiseException("ACCESS-ERROR", "attempt to write to read-only imported variable $%s", v.ivar.refptr->getName());
	 return 0;
      }

      // perform lock handoff
      m.unlock();
      v.ivar.refptr->m.lock();

      return v.ivar.refptr->getValuePtrIntern(vl, varTypeInfo, xsink);
   }

   vl->set(&m);
   varTypeInfo = typeInfo;
   return const_cast<AbstractQoreNode **>(&v.val.value);
}

AbstractQoreNode* Var::remove(ExceptionSink* xsink) {
   m.lock();
   if (type == GV_IMPORT) {
      if (v.ivar.readonly) {
	 m.unlock();
	 xsink->raiseException("ACCESS-ERROR", "attempt to write to read-only imported variable $%s", v.ivar.refptr->getName());
	 return 0;
      }

      // perform lock handoff
      m.unlock();
      return v.ivar.refptr->remove(xsink);
   }

   AbstractQoreNode* rv = v.val.value;
   v.val.value = 0;
   m.unlock();
   return rv;
}

// note: unlocking the lock is managed with the AutoVLock object
AbstractQoreNode **Var::getValuePtr(AutoVLock *vl, const QoreTypeInfo *&varTypeInfo, ExceptionSink *xsink) const {
   m.lock();
   return getValuePtrIntern(vl, varTypeInfo, xsink);
}

// note: unlocking the lock is managed with the AutoVLock object
AbstractQoreNode *Var::getValueIntern(AutoVLock *vl) {
   if (type == GV_IMPORT) {
      // perform lock handoff
      m.unlock();
      v.ivar.refptr->m.lock();

      return v.ivar.refptr->getValueIntern(vl);
   }

   vl->set(&m);
   return v.val.value;
}

// note: unlocking the lock is managed with the AutoVLock object
const AbstractQoreNode *Var::getValueIntern(AutoVLock *vl) const {
   if (type == GV_IMPORT) {
      // perform lock handoff
      m.unlock();
      v.ivar.refptr->m.lock();

      return v.ivar.refptr->getValueIntern(vl);
   }

   vl->set(&m);
   return v.val.value;
}

AbstractQoreNode *Var::getReferencedValue() const {
   AutoVLock vl(0);
   m.lock();
   AbstractQoreNode *rv = (AbstractQoreNode *)getValueIntern(&vl);
   return rv ? rv->refSelf() : 0;
}

// note: type enforcement is done at a higher level
void Var::setValueIntern(AbstractQoreNode *val, ExceptionSink *xsink) {
   if (type == GV_IMPORT) {
      if (v.ivar.readonly) {
	 m.unlock();
	 xsink->raiseException("ACCESS-ERROR", "attempt to write to read-only variable $%s", getName());
	 return;
      }

      // perform lock handoff
      m.unlock();
      v.ivar.refptr->m.lock();

      v.ivar.refptr->setValueIntern(val, xsink);
      return;
   }

   // save old value (if any) to dereference outside the lock
   // (because a deadlock could occur if the dereference causes
   // a destructor to run which references this variable)
   AbstractQoreNode *old = v.val.value;
   v.val.value = val;

   m.unlock();

   // dereference old value once lock has been released
   if (old)
      old->deref(xsink);
}

void Var::setValue(AbstractQoreNode *val, ExceptionSink *xsink) {
   m.lock();
   setValueIntern(val, xsink);
}

void Var::deref(ExceptionSink *xsink) {
   if (ROdereference()) {
      del(xsink);
      delete this;
   }
}

static AbstractQoreNode **do_list_val_ptr(const QoreTreeNode *tree, AutoVLock *vlp, ObjMap &omap, ExceptionSink *xsink) {
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
static AbstractQoreNode **do_object_val_ptr(const QoreTreeNode *tree, AutoVLock *vlp, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) {
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
AbstractQoreNode **get_var_value_ptr(const AbstractQoreNode *n, AutoVLock *vlp, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) {
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
AbstractQoreNode *getExistingVarValue(const AbstractQoreNode *n, ExceptionSink *xsink) {
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

static AbstractQoreNode **check_unique(AbstractQoreNode **p, ExceptionSink *xsink) {
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
static AbstractQoreNode **get_unique_existing_container_value_ptr(AbstractQoreNode *n, ExceptionSink *xsink, AutoVLock *vl, ObjMap &omap) {
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

static AbstractQoreNode *remove_hash_object_value(QoreHashNode *h, QoreObject *o, const char *mem, ExceptionSink *xsink) {
   // otherwise if not a hash or object then exit
   if (o)
      return o->takeMember(mem, xsink);

   // if it's a hash reference, then remove the key
   return h->takeKeyValue(mem);
}

AbstractQoreNode *remove_lvalue_intern(ExceptionSink *xsink, AbstractQoreNode *lvalue, AutoVLock& vl, qore_type_t lvtype, ObjMap& omap) {
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

AbstractQoreNode *remove_lvalue(AbstractQoreNode *lvalue, ExceptionSink *xsink, bool for_del) {
   AutoVLock vl(xsink);
   qore_type_t lvtype = lvalue->getType();

   // FIXME: add logic to detect breaking a recursive reference
   ObjMap omap;

   // if the node is a variable reference, then find value ptr, set it to 0, and return the value
   if (lvtype == NT_VARREF)
      return reinterpret_cast<VarRefNode *>(lvalue)->remove(xsink, for_del);

   return remove_lvalue_intern(xsink, lvalue, vl, lvtype, omap);
}

DLLLOCAL int64 remove_lvalue_bigint(AbstractQoreNode *lvalue, ExceptionSink *xsink) {
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

DLLLOCAL double remove_lvalue_float(AbstractQoreNode *lvalue, ExceptionSink *xsink) {
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

void delete_lvalue(AbstractQoreNode *lvalue, ExceptionSink *xsink) {
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
   // make sure it's an optimized (ie not global, not a reference, not a "normal" local var)
   if (t == NT_VARREF && (lv.v = reinterpret_cast<const VarRefNode *>(exp)->isLocalOptimized(typeInfo)))
      lvt = LVT_OptLocalVar;
   else {
      lvt = LVT_Normal;
      lv.n = new LValueExpressionHelper(exp, typeInfo, xsink);
   }         
}

AbstractQoreNode *LValueHelper::getReferencedValue() const {
   return lvt == LVT_OptLocalVar ? lv.v->eval(xsink) : lv.n->getReferencedValue();
}

const qore_type_t LValueHelper::get_type() const {
   return lvt == LVT_OptLocalVar ? lv.v->getValueType() : *(lv.n->v) ? (*(lv.n->v))->getType() : NT_NOTHING; 
}

const char* LValueHelper::get_type_name() const {
   return lvt == LVT_OptLocalVar ? lv.v->getValueTypeName() : ::get_type_name(*(lv.n->v)); 
}

int LValueHelper::assign(AbstractQoreNode *val, const char *desc) {
   // check type for assignment
   val = get_type_info()->acceptAssignment(desc, val, xsink);
   if (*xsink) {
      discard(val, xsink);
      return -1;
   }

   if (lvt == LVT_OptLocalVar) {
      // since we are only dealing with optimized local vars, it's not possible
      // to have an exception here on the assignment
      lv.v->setValue(val, xsink);
      assert(!*xsink);
      return 0;
   }

   return lv.n->assign(val, xsink);
}

int LValueHelper::assignBigInt(int64 v, const char *desc) {
   // check type for assignment
   if (!typeInfo->parseAccepts(bigIntTypeInfo)) {
      typeInfo->doAcceptError(false, false, -1, desc, Zero, xsink);
      return -1;
   }

   if (lvt == LVT_OptLocalVar) {
      // since we are only dealing with optimized local vars, it's not possible
      // to have an exception here on the assignment
      lv.v->assignBigInt(v, xsink);
      assert(!*xsink);
      return 0;
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

   if (lvt == LVT_OptLocalVar) {
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
   if (lvt == LVT_OptLocalVar)
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
   if (lvt == LVT_OptLocalVar)
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
   if (lvt == LVT_OptLocalVar)
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
   if (lvt == LVT_OptLocalVar)
      return lv.v->divideEqualsBigInt(v, xsink);

   // get new value if necessary
   if (ensure_unique_int())
      return 0;
   QoreBigIntNode *i = reinterpret_cast<QoreBigIntNode *>(get_value());
   // increment current value
   i->val /= v;
   return i->val;
}

double LValueHelper::multiplyEqualsFloat(double v) {
   if (lvt == LVT_OptLocalVar)
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
   if (lvt == LVT_OptLocalVar)
      return lv.v->divideEqualsFloat(v, xsink);

   // get new value if necessary
   if (ensure_unique_float())
      return 0;
   QoreFloatNode *i = reinterpret_cast<QoreFloatNode *>(get_value());
   // increment current value
   i->f /= v;
   return i->f;
}

void lvar_ref::assign(AbstractQoreNode *n_vexp, QoreObject *n_obj, QoreProgram *n_pgm) {
   vexp = n_vexp;
   obj = n_obj;
   pgm = n_pgm;
   if (n_obj)
      n_obj->tRef();

   is_vref = vexp->getType() == NT_VARREF && !reinterpret_cast<VarRefNode*>(vexp)->isGlobalVar();
}

template <class T>
int64 lvar_ref::postIncrement(T *vv, ExceptionSink *xsink) {
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
int64 lvar_ref::preIncrement(T *vv, ExceptionSink *xsink) {
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
int64 lvar_ref::postDecrement(T *vv, ExceptionSink *xsink) {
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
int64 lvar_ref::preDecrement(T *vv, ExceptionSink *xsink) {
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
int64 lvar_ref::plusEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink) {
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
double lvar_ref::plusEqualsFloat(double v, T *vv, ExceptionSink *xsink) {
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
int64 lvar_ref::minusEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink) {
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
double lvar_ref::minusEqualsFloat(double v, T *vv, ExceptionSink *xsink) {
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
int64 lvar_ref::orEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink) {
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
int64 lvar_ref::andEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink) {
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
int64 lvar_ref::modulaEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink) {
   if (is_vref) {
      LocalRefHelper<T> lrh(vv);
      return reinterpret_cast<VarRefNode*>(vexp)->modulaEqualsBigInt(v, xsink);
   }

   LValueRefHelper<T> valp(vv, xsink);
   if (!valp)
      return 0;
      
   valp->ensure_unique_int();
   QoreBigIntNode *n = reinterpret_cast<QoreBigIntNode *>(valp->get_value());
   n->val %= v;
   return n->val;
}

template <class T>
int64 lvar_ref::multiplyEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink) {
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
int64 lvar_ref::divideEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink) {
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
int64 lvar_ref::xorEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink) {
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
int64 lvar_ref::shiftLeftEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink) {
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
int64 lvar_ref::shiftRightEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink) {
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
double lvar_ref::multiplyEqualsFloat(double v, T *vv, ExceptionSink *xsink) {
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
double lvar_ref::divideEqualsFloat(double v, T *vv, ExceptionSink *xsink) {
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

LocalVarValue* LocalVarValue::optimized(const QoreTypeInfo *&varTypeInfo) const {
   if (vvt == VVT_Ref) {
      if (val.ref.vexp->getType() != NT_VARREF)
	 return 0;

      SkipHelper sh(const_cast<LocalVarValue*>(this));
      ProgramContextHelper pch(val.ref.pgm);

      return reinterpret_cast<const VarRefNode*>(val.ref.vexp)->isLocalOptimized(varTypeInfo);
   }
   
   return vvt != VVT_Normal ? const_cast<LocalVarValue*>(this) : 0;
}

int64 LocalVarValue::plusEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_plusEqualsBigInt(v, xsink);

      case VVT_Int:
	 return val.val_int += v;

      case VVT_Ref:
	 return val.ref.plusEqualsBigInt<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

double LocalVarValue::plusEqualsFloat(double v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_plusEqualsFloat(v, xsink);

      case VVT_Float:
	 return val.val_float += v;

      case VVT_Ref:
	 return val.ref.plusEqualsFloat<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0.0;
}

int64 LocalVarValue::minusEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_minusEqualsBigInt(v, xsink);

      case VVT_Int:
	 return val.val_int -= v;

      case VVT_Ref:
	 return val.ref.minusEqualsBigInt<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

double LocalVarValue::minusEqualsFloat(double v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_minusEqualsFloat(v, xsink);

      case VVT_Float:
	 return val.val_float += v;

      case VVT_Ref:
	 return val.ref.minusEqualsFloat<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0.0;
}

int64 LocalVarValue::orEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_orEqualsBigInt(v, xsink);

      case VVT_Int:
	 return val.val_int |= v;

      case VVT_Ref:
	 return val.ref.orEqualsBigInt<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

int64 LocalVarValue::andEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_andEqualsBigInt(v, xsink);

      case VVT_Int:
	 return val.val_int &= v;

      case VVT_Ref:
	 return val.ref.andEqualsBigInt<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

int64 LocalVarValue::modulaEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_modulaEqualsBigInt(v, xsink);

      case VVT_Int:
	 return val.val_int %= v;

      case VVT_Ref:
	 return val.ref.modulaEqualsBigInt<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

int64 LocalVarValue::multiplyEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_multiplyEqualsBigInt(v, xsink);

      case VVT_Int:
	 return val.val_int *= v;

      case VVT_Ref:
	 return val.ref.multiplyEqualsBigInt<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

int64 LocalVarValue::divideEqualsBigInt(int64 v, ExceptionSink *xsink) {
   assert(v);
   switch (vvt) {
      case VVT_Normal:
	 return val.value_minusEqualsBigInt(v, xsink);

      case VVT_Int:
	 return val.val_int /= v;

      case VVT_Ref:
	 return val.ref.minusEqualsBigInt<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

int64 LocalVarValue::xorEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_minusEqualsBigInt(v, xsink);

      case VVT_Int:
	 return val.val_int ^= v;

      case VVT_Ref:
	 return val.ref.minusEqualsBigInt<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

int64 LocalVarValue::shiftLeftEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_shiftLeftEqualsBigInt(v, xsink);

      case VVT_Int:
	 return val.val_int <<= v;

      case VVT_Ref:
	 return val.ref.shiftLeftEqualsBigInt<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

int64 LocalVarValue::shiftRightEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_shiftRightEqualsBigInt(v, xsink);

      case VVT_Int:
	 return val.val_int >>= v;

      case VVT_Ref:
	 return val.ref.shiftRightEqualsBigInt<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

int64 LocalVarValue::postIncrement(ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_postIncrement(xsink);

      case VVT_Int: {
	 int64 rv = val.val_int;
	 ++val.val_int;
	 return rv;
      }

      case VVT_Ref:
	 return val.ref.postIncrement<LocalVarValue>(this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      

   return 0;
}

int64 LocalVarValue::preIncrement(ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_preIncrement(xsink);

      case VVT_Int:
	 return ++val.val_int;

      case VVT_Ref:
	 return val.ref.preIncrement<LocalVarValue>(this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      

   return 0;
}

int64 LocalVarValue::postDecrement(ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_postDecrement(xsink);

      case VVT_Int: {
	 int64 rv = val.val_int;
	 --val.val_int;
	 return rv;
      }

      case VVT_Ref:
	 return val.ref.postDecrement<LocalVarValue>(this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      

   return 0;
}

int64 LocalVarValue::preDecrement(ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_preDecrement(xsink);

      case VVT_Int:
	 return --val.val_int;

      case VVT_Ref:
	 return val.ref.preDecrement<LocalVarValue>(this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      
   return 0;
}

double LocalVarValue::multiplyEqualsFloat(double v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal:
	 return val.value_multiplyEqualsFloat(v, xsink);

      case VVT_Int:
	 return val.val_int *= (int64)v;

      case VVT_Ref:
	 return val.ref.multiplyEqualsFloat<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      

   return 0;
}

double LocalVarValue::divideEqualsFloat(double v, ExceptionSink *xsink) {
   assert(v);

   switch (vvt) {
      case VVT_Normal:
	 return val.value_divideEqualsFloat(v, xsink);

      case VVT_Int:
	 return val.val_int /= (int64)v;

      case VVT_Ref:
	 return val.ref.divideEqualsFloat<LocalVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      

   return 0;
}


int64 ClosureVarValue::plusEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_plusEqualsBigInt(v, xsink);
      }
	 
      case VVT_Ref:
	 return val.ref.plusEqualsBigInt<ClosureVarValue>(v, this, xsink);
	 
         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }
   
   return 0;
}

double ClosureVarValue::plusEqualsFloat(double v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_plusEqualsFloat(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.plusEqualsFloat<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0.0;
}

int64 ClosureVarValue::minusEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_minusEqualsBigInt(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.minusEqualsBigInt<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

double ClosureVarValue::minusEqualsFloat(double v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_minusEqualsFloat(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.minusEqualsFloat<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0.0;
}

int64 ClosureVarValue::orEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_orEqualsBigInt(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.orEqualsBigInt<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

int64 ClosureVarValue::andEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_andEqualsBigInt(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.andEqualsBigInt<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      

   return 0;
}

int64 ClosureVarValue::modulaEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_modulaEqualsBigInt(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.modulaEqualsBigInt<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }

   return 0;
}

int64 ClosureVarValue::multiplyEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_multiplyEqualsBigInt(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.multiplyEqualsBigInt<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      
   return 0;
}

int64 ClosureVarValue::divideEqualsBigInt(int64 v, ExceptionSink *xsink) {
   assert(v);
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_divideEqualsBigInt(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.divideEqualsBigInt<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      
   return 0;
}

int64 ClosureVarValue::xorEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_xorEqualsBigInt(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.xorEqualsBigInt<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      
   return 0;
}

int64 ClosureVarValue::shiftLeftEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_shiftLeftEqualsBigInt(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.shiftLeftEqualsBigInt<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      
   return 0;
}

int64 ClosureVarValue::shiftRightEqualsBigInt(int64 v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_shiftRightEqualsBigInt(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.shiftRightEqualsBigInt<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      
   return 0;
}

int64 ClosureVarValue::postIncrement(ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_postIncrement(xsink);
      }

      case VVT_Ref:
	 return val.ref.postIncrement<ClosureVarValue>(this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      
   return 0;
}

int64 ClosureVarValue::preIncrement(ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_preIncrement(xsink);
      }

      case VVT_Ref:
	 return val.ref.preIncrement<ClosureVarValue>(this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      
   return 0;
}

int64 ClosureVarValue::postDecrement(ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_postDecrement(xsink);
      }

      case VVT_Ref:
	 return val.ref.postDecrement<ClosureVarValue>(this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      
   return 0;
}

int64 ClosureVarValue::preDecrement(ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_preDecrement(xsink);
      }

      case VVT_Ref:
	 return val.ref.preDecrement<ClosureVarValue>(this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }      
   return 0;
}

double ClosureVarValue::multiplyEqualsFloat(double v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_multiplyEqualsFloat(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.multiplyEqualsFloat<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }
   return 0.0;
}

double ClosureVarValue::divideEqualsFloat(double v, ExceptionSink *xsink) {
   switch (vvt) {
      case VVT_Normal: {
	 AutoLocker al(this);
	 return val.value_divideEqualsFloat(v, xsink);
      }

      case VVT_Ref:
	 return val.ref.divideEqualsFloat<ClosureVarValue>(v, this, xsink);

         // to avoid warnings about missing enum values
      default:
	 assert(false);
   }
   return 0.0;
}
