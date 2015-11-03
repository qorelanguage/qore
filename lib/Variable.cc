/*
  Variable.cc

  Qore programming language

  Copyright 2003 - 2009 David Nichols

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

VarStackPointerHelper::VarStackPointerHelper(LocalVarValue *v) : orig(v) {
   v->skip = true;
}

VarStackPointerHelper::~VarStackPointerHelper() {
   orig->skip = false;
}

VarStackPointerClosureHelper::VarStackPointerClosureHelper(ClosureVarValue *v) : orig(v) {
   v->skip = true;
}

VarStackPointerClosureHelper::~VarStackPointerClosureHelper() {
   orig->skip = false;
}

Var::Var(const char *n_name, AbstractQoreNode *val) : type(GV_VALUE), name(n_name) {
   v.val.value = val;
}

Var::Var(class Var *ref, bool ro) : type(GV_IMPORT), name(ref->name) {
   v.ivar.refptr = ref;
   v.ivar.readonly = ro;
   ref->ROreference();
}

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
AbstractQoreNode **Var::getValuePtrIntern(AutoVLock *vl, ExceptionSink *xsink) const {
   if (type == GV_IMPORT) {
      if (v.ivar.readonly) {
	 m.unlock();
	 xsink->raiseException("ACCESS-ERROR", "attempt to write to read-only imported variable $%s", v.ivar.refptr->getName());
	 return 0;
      }

      // perform lock handoff
      m.unlock();
      v.ivar.refptr->m.lock();

      return v.ivar.refptr->getValuePtrIntern(vl, xsink);
   }

   vl->set(&m);
   return const_cast<AbstractQoreNode **>(&v.val.value);
}

// note: unlocking the lock is managed with the AutoVLock object
AbstractQoreNode **Var::getValuePtr(AutoVLock *vl, ExceptionSink *xsink) const {
   m.lock();

   return getValuePtrIntern(vl, xsink);
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

// note: unlocking the lock is managed with the AutoVLock object
AbstractQoreNode *Var::getValue(AutoVLock *vl) {
   m.lock();

   return getValueIntern(vl);
}

AbstractQoreNode *Var::getReferencedValue() const {
   AutoVLock vl(0);

   AbstractQoreNode *rv;
   { 
      m.lock();

      rv = (AbstractQoreNode *)getValueIntern(&vl);
      if (rv) rv->ref();
   }
   return rv;
}

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

   if (v.val.value)
      v.val.value->deref(xsink);
   v.val.value = val;

   m.unlock();
}

void Var::setValue(AbstractQoreNode *val, ExceptionSink *xsink) {
   m.lock();

   setValueIntern(val, xsink);
}

void Var::makeReference(Var *pvar, ExceptionSink *xsink, bool ro) {
   assert(name == pvar->name);

   AutoLocker al(m);

   if (type == GV_IMPORT)
      v.ivar.refptr->deref(xsink);
   else {
      if (v.val.value)
	 v.val.value->deref(xsink);

      type = GV_IMPORT;
   }

   v.ivar.refptr = pvar;
   v.ivar.readonly = ro;
   pvar->ROreference();
}

void Var::deref(ExceptionSink *xsink) {
   if (ROdereference()) {
      del(xsink);
      delete this;
   }
}

static AbstractQoreNode **do_list_val_ptr(const QoreTreeNode *tree, AutoVLock *vlp, ExceptionSink *xsink) {
   // first get index
   int ind = tree->right->integerEval(xsink);
   if (xsink->isEvent())
      return 0;
   if (ind < 0) {
      xsink->raiseException("NEGATIVE-LIST-INDEX", "list index %d is invalid (index must evaluate to a non-negative integer)", ind);
      return 0;
   }

   // now get left hand side
   AbstractQoreNode **val = get_var_value_ptr(tree->left, vlp, xsink);
   if (xsink->isEvent())
      return 0;

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
static AbstractQoreNode **do_object_val_ptr(const QoreTreeNode *tree, AutoVLock *vlp, ExceptionSink *xsink) {
   QoreNodeEvalOptionalRefHolder member(tree->right, xsink);
   if (*xsink)
      return 0;

   QoreStringValueHelper mem(*member, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   AbstractQoreNode **val = get_var_value_ptr(tree->left, vlp, xsink);
   if (*xsink)
      return 0;

   QoreHashNode *h = (*val && (*val)->getType() == NT_HASH) ? reinterpret_cast<QoreHashNode *>(*val) : 0;
   QoreObject *o = 0;

   //printd(0, "do_object_val_ptr() h=%p val=%p (%s, refs=%d)\n", h, *val, *val ? (*val)->getTypeName() : "(null)", *val ? (*val)->reference_count() : 0);

   // if the variable's value is not already a hash or an object, then make it a hash
   if (h) {
      if (h->reference_count() > 1) {
	 // otherwise if the reference_count > 1 (and it's not an object), then duplicate it.
	 QoreHashNode *o = h;
	 h = h->copy();
	 o->deref(xsink);
	 (*val) = h;
      }
   }
   else {
      o = (*val && (*val)->getType() == NT_OBJECT) ? reinterpret_cast<QoreObject *>(*val) : 0;
      if (!o) {
	 if (*val)
	    (*val)->deref(xsink);
	 h = new QoreHashNode();
	 (*val) = h;
      }
   }

   if (h) {
      //printd(0, "do_object_val_ptr() def=%s member %s \"%s\"\n", QCS_DEFAULT->getCode(), mem->getEncoding()->getCode(), mem->getBuffer());
      return h->getKeyValuePtr(mem->getBuffer());
   }

   //printd(5, "do_object_val_ptr() member=%s\n", mem->getBuffer());

   AbstractQoreNode **rv;
   // if object has been deleted, then dereference, make into a hash, and get hash pointer
   if ((rv = o->getMemberValuePtr(mem->getBuffer(), vlp, xsink))) {
      vlp->addMemberNotification(o, mem->getBuffer());
      return rv;
   }

   if (*xsink)
      return 0;

   (*val)->deref(xsink);
   h = new QoreHashNode();
   (*val) = h; 
   return h->getKeyValuePtr(mem->getBuffer());
}

// this function will change the lvalue to the right type if needed (used for assignments)
AbstractQoreNode **get_var_value_ptr(const AbstractQoreNode *n, AutoVLock *vlp, ExceptionSink *xsink) {
   qore_type_t ntype = n->getType();
   //printd(0, "get_var_value_ptr(%p) %s\n", n, n->getTypeName());
   if (ntype == NT_VARREF) {
      const VarRefNode *v = reinterpret_cast<const VarRefNode *>(n);
      //printd(5, "get_var_value_ptr(): vref=%s (%p)\n", v->name, v);
      return v->getValuePtr(vlp, xsink);
   }
   else if (ntype == NT_SELF_VARREF) {
      const SelfVarrefNode *v = reinterpret_cast<const SelfVarrefNode *>(n);
      // need to check for deleted objects
      // note that getStackObject() is guaranteed to return a value here (self varref is only valid in a method)
      AbstractQoreNode **rv = getStackObject()->getMemberValuePtr(v->str, vlp, xsink);
      if (!rv && !xsink->isException())
	 xsink->raiseException("OBJECT-ALREADY-DELETED", "write attempted to member \"%s\" in an already-deleted object", v->str);
      return rv;
   }

   // it must be a tree
   const QoreTreeNode *tree = reinterpret_cast<const QoreTreeNode *>(n);
   if (tree->op == OP_LIST_REF)
      return do_list_val_ptr(tree, vlp, xsink);
   return do_object_val_ptr(tree, vlp, xsink);
}

// finds value of partially evaluated lvalue expressions (for use with references)
// will not do any evaluations, however, because local variables could hold imported
// object references, exceptions could be thrown
AbstractQoreNode *getNoEvalVarValue(AbstractQoreNode *n, AutoVLock *vl, ExceptionSink *xsink) {
   printd(5, "getNoEvalVarValue(%p) %s\n", n, n->getTypeName());
   if (n->getType() == NT_VARREF)
      return reinterpret_cast<VarRefNode *>(n)->getValue(vl, xsink);

   if (n->getType() == NT_SELF_VARREF)
      return getStackObject()->getMemberValueNoMethod(reinterpret_cast<SelfVarrefNode *>(n)->str, vl, xsink);

   // it's a variable reference tree
   QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(n);
   AbstractQoreNode *val = getNoEvalVarValue(tree->left, vl, xsink);
   if (!val)
      return 0;

   // if it's a list reference
   if (tree->op == OP_LIST_REF) {
      if (val->getType() != NT_LIST)
	 return 0;
      // if it's not a list then return 0
      QoreListNode *l = reinterpret_cast<QoreListNode *>(val);
      
      // otherwise return value
      int i;
      if (tree->right)
	 i = tree->right->getAsInt();
      else
	 i = 0;
      return l->retrieve_entry(i);
   }
      
   // it's an object reference
   // if not an object or a hash, return 0
   QoreHashNode *h = val->getType() == NT_HASH ? reinterpret_cast<QoreHashNode *>(val) : 0;
   QoreObject *o;
   if (!h)
      o = val->getType() == NT_OBJECT ? reinterpret_cast<QoreObject *>(val) : 0;
   else
      o = 0;
   if (!o && !h)
      return 0;
      
   // otherwise get member name
   QoreStringValueHelper key(tree->right, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   if (h)
      return h->getKeyValue(key->getBuffer());
   return o->getMemberValueNoMethod(key->getBuffer(), vl, xsink);
}

// finds object value pointers without making any changes to the referenced structures
// will *not* execute memberGate methods
AbstractQoreNode *getExistingVarValue(const AbstractQoreNode *n, ExceptionSink *xsink, AutoVLock *vl, ReferenceHolder<AbstractQoreNode> &pt) {
   printd(5, "getExistingVarValue(%p) %s\n", n, n->getTypeName());
   qore_type_t ntype = n->getType();
   if (ntype == NT_VARREF)
      return reinterpret_cast<const VarRefNode *>(n)->getValue(vl, xsink);

   if (ntype == NT_SELF_VARREF)
      return getStackObject()->getMemberValueNoMethod(reinterpret_cast<const SelfVarrefNode *>(n)->str, vl, xsink);

   // it's a variable reference tree
   const QoreTreeNode *tree = ntype == NT_TREE ? reinterpret_cast<const QoreTreeNode *>(n) : 0;
   if (tree && (tree->op == OP_LIST_REF || tree->op == OP_OBJECT_REF)) {
      AbstractQoreNode *val = getExistingVarValue(tree->left, xsink, vl, pt);
      if (!val)
	 return 0;

      // if it's a list reference
      if (tree->op == OP_LIST_REF) {
	 // if it's not a list then return 0
	 if (val->getType() != NT_LIST)
	    return 0;

	 // otherwise return value
	 return reinterpret_cast<QoreListNode *>(val)->retrieve_entry(tree->right->integerEval(xsink));
      }
      
      // if it's an object reference
      if (tree->op == OP_OBJECT_REF) {
	 QoreHashNode *h = val->getType() == NT_HASH ? reinterpret_cast<QoreHashNode *>(val) : 0;
	 QoreObject *o;
	 if (!h)
	    o = val->getType() == NT_OBJECT ? reinterpret_cast<QoreObject *>(val) : 0;
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

	 return h ? h->getKeyValue(mem->getBuffer()) : o->getMemberValueNoMethod(mem->getBuffer(), vl, xsink);
      }
   }
   
   // otherwise need to evaluate
   ReferenceHolder<AbstractQoreNode> t(n->eval(xsink), xsink);
   if (*xsink)
      return 0;

   pt = t.release();
   return *pt;
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
static AbstractQoreNode **getUniqueExistingVarValuePtr(AbstractQoreNode *n, ExceptionSink *xsink, AutoVLock *vl) {
   printd(5, "getUniqueExistingVarValuePtr(%p) %s\n", n, n->getTypeName());
   qore_type_t ntype = n->getType();
   if (ntype == NT_VARREF)
      return check_unique(reinterpret_cast<VarRefNode *>(n)->getValuePtr(vl, xsink), xsink);

   // getStackObject() will always return a value here (self refs are only legal in methods)
   if (ntype == NT_SELF_VARREF) {
      QoreObject *obj = getStackObject();
      return check_unique(obj->getExistingValuePtr(reinterpret_cast<SelfVarrefNode *>(n)->str, vl, xsink), xsink);
   }

   // it's a variable reference tree
   QoreTreeNode *tree = ntype == NT_TREE ? reinterpret_cast<QoreTreeNode *>(n) : 0;

   assert(tree && (tree->op == OP_LIST_REF || tree->op == OP_OBJECT_REF));

   AbstractQoreNode **val = getUniqueExistingVarValuePtr(tree->left, xsink, vl);
   if (!val || !(*val))
      return 0;

   // if it's a list reference
   if (tree->op == OP_LIST_REF) {
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

void delete_var_node(AbstractQoreNode *lvalue, ExceptionSink *xsink) {
   AutoVLock vl(xsink);
   AbstractQoreNode **val;
   
   qore_type_t lvtype = lvalue->getType();

   // if the node is a variable reference, then find value ptr, dereference it, and return
   if (lvtype == NT_VARREF) {
      val = reinterpret_cast<VarRefNode *>(lvalue)->getValuePtr(&vl, xsink);
      if (val && *val) {
	 printd(5, "delete_var_node() setting ptr %p (val=%p) to NULL\n", val, (*val));
	 if ((*val)->getType() == NT_OBJECT) {
	    QoreObject *o = reinterpret_cast<QoreObject *>(*val);
	    if (o->isSystemObject())
	       xsink->raiseException("SYSTEM-OBJECT-ERROR", "you cannot delete a system constant object");
	    else {
	       (*val) = 0;
	       vl.del();
	       o->doDelete(xsink);
	       o->deref(xsink);
	    }
	 }
	 else {
	    (*val)->deref(xsink);
	    *val = 0;
	 }
      }
      return;
   }

   // delete key if exists and resize hash if necessary
   if (lvtype == NT_SELF_VARREF) {
      getStackObject()->deleteMemberValue(reinterpret_cast<SelfVarrefNode *>(lvalue)->str, xsink);
      return;
   }

   // must be a tree
   QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(lvalue);

   // otherwise it is a list or object (hash) reference

   // if it's a list reference, see if the reference exists, if so, then delete it;
   // if it's the last element in the list, then resize the list...
   if (tree->op == OP_LIST_REF) {
      int offset = tree->right->integerEval(xsink);
      if (*xsink)
	 return;

      // find variable ptr, exit if doesn't exist anyway
      val = getUniqueExistingVarValuePtr(tree->left, xsink, &vl);
      
      if (!val || !(*val) || (*val)->getType() != NT_LIST || *xsink)
	 return;

      QoreListNode *l = reinterpret_cast<QoreListNode *>(*val);
      // delete the value if it exists
      l->delete_entry(offset, xsink);
      return;
   }

   // get the member name
   QoreNodeEvalOptionalRefHolder member(tree->right, xsink);
   if (*xsink)
      return;

   QoreStringValueHelper mem(*member, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   // find variable ptr, exit if doesn't exist anyway
   val = getUniqueExistingVarValuePtr(tree->left, xsink, &vl);
      
   if (!val || !(*val) || *xsink)
      return;

   QoreObject *o = (*val)->getType() == NT_OBJECT ? reinterpret_cast<QoreObject *>(*val) : 0;

   // otherwise if not a hash or object then exit
   if (o) {
      // to avoid a deadlock in the case where an object is deleting a member
      // and the member refers to the parent object in the member's
      // destructor, we reference the object, free all locks, then
      // delete the member, and then dereference the object

      o->tRef();
      vl.del();
      o->deleteMemberValue(mem->getBuffer(), xsink);
      vl.addMemberNotification(o, mem->getBuffer());
      o->tDeref();
      return;
   }

   // if it's a hash reference, then delete the key
   QoreHashNode *h = (*val)->getType() == NT_HASH ? reinterpret_cast<QoreHashNode *>(*val) : 0;
   if (h) {
      // 1: take the value from the hash
      AbstractQoreNode *val = h->takeKeyValue(mem->getBuffer());
      // 2: release all locks
      vl.del();
      // 3: perform any necessary deletes
      if (val) {
	 if (val->getType() == NT_OBJECT) {
	    reinterpret_cast<QoreObject *>(val)->doDelete(xsink);
	 }
	 val->deref(xsink);
      }
   }
}
