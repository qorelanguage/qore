/*
  Variable.cc

  Qore programming language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
class QoreHash *ENV;

#include <qore/QoreType.h>

Var::Var(const char *nme, AbstractQoreNode *val)
{
   type = GV_VALUE;
   v.val.value = val;
   v.val.name = strdup(nme);
   //next = NULL;
}

Var::Var(class Var *ref, bool ro)
{
   type = GV_IMPORT;
   v.ivar.refptr = ref;
   v.ivar.readonly = ro;
   ref->ROreference();
   //next = NULL;
}

void Var::del(class ExceptionSink *xsink)
{
   if (type == GV_IMPORT)
   {
      printd(4, "Var::~Var() refptr=%08p\n", v.ivar.refptr);
      v.ivar.refptr->deref(xsink);
      // clear type so no further deleting will be done
   }
   else
   {
      printd(4, "Var::~Var() name=%s value=%08p type=%s refs=%d\n", v.val.name,
	     v.val.value, v.val.value ? v.val.value->getTypeName() : "null", 
	     v.val.value ? v.val.value->reference_count() : 0);
 
      free(v.val.name);
#ifdef DEBUG
      v.val.name = NULL;
#endif
      if (v.val.value)
	 v.val.value->deref(xsink);
      // clear type so no further deleting will be done
   }
}

bool Var::isImported() const
{
   return type == GV_IMPORT;
}

const char *Var::getName() const
{
   if (type == GV_IMPORT)
      return v.ivar.refptr->getName();
   return v.val.name;
}

/*
AbstractQoreNode *Var::getValue()
{
   if (refptr)
      return refptr->getValue();
   return value;
}
*/

AbstractQoreNode *Var::eval(class ExceptionSink *xsink)
{
   AbstractQoreNode *rv;

   if (gate.enter(xsink) < 0)
      return NULL;
   if (type == GV_IMPORT)
      rv = v.ivar.refptr->eval(xsink);
   else
   {
      rv = v.val.value;
      if (rv)
	 rv->ref();
      //printd(5, "Var::eval() this=%08p val=%08p (%s)\n", this, rv, rv ? rv->getTypeName() : "(null)");
   }
   gate.exit();
   return rv;
}

// note: the caller must exit the gate!
AbstractQoreNode **Var::getValuePtr(class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (gate.enter(xsink) < 0)
      return NULL;

   if (type == GV_IMPORT)
   {
      if (v.ivar.readonly)
      {
	 gate.exit();
	 xsink->raiseException("ACCESS-ERROR", "attempt to write to read-only variable $%s", v.ivar.refptr->getName());
	 return NULL;
      }
      AbstractQoreNode **rv = v.ivar.refptr->getValuePtr(vl, xsink);
      gate.exit();
      return rv;
   }
   vl->push(&gate);
   return &v.val.value;
}

// note: the caller must exit the gate!
AbstractQoreNode *Var::getValue(class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (gate.enter(xsink) < 0)
      return NULL;

   if (type == GV_IMPORT)
   {
      AbstractQoreNode *rv = v.ivar.refptr->getValue(vl, xsink);
      gate.exit();
      return rv;
   }
   vl->push(&gate);
   return v.val.value;
}

void Var::setValue(AbstractQoreNode *val, class ExceptionSink *xsink)
{
   if (type == GV_IMPORT)
   {
      if (v.ivar.readonly)
      {
	 xsink->raiseException("ACCESS-ERROR", "attempt to write to read-only variable $%s", v.ivar.refptr->getName());
	 return;
      }
      if (gate.enter(xsink) < 0)
	 return;
      v.ivar.refptr->setValue(val, xsink);
      gate.exit();
      return;
   }

   if (gate.enter(xsink) < 0)
      return;
   if (v.val.value)
      v.val.value->deref(xsink);
   v.val.value = val;
   gate.exit();
}

void Var::makeReference(class Var *pvar, class ExceptionSink *xsink, bool ro)
{
   if (gate.enter(xsink) < 0)
      return;
   if (type == GV_IMPORT)
      v.ivar.refptr->deref(xsink);
   else
   {
      if (v.val.value)
	 v.val.value->deref(xsink);
      free(v.val.name);
   }
   type = GV_IMPORT;
   v.ivar.refptr = pvar;
   v.ivar.readonly = ro;
   pvar->ROreference();
   gate.exit();
}

void Var::deref(class ExceptionSink *xsink)
{
   if (ROdereference())
   {
      del(xsink);
      delete this;
   }
}

/*
LVar::LVar(lvh_t nid, AbstractQoreNode *nvalue) 
{
   id = nid; 
   value = nvalue; 
   vexp = NULL;
   obj = NULL;
}

LVar::LVar(lvh_t nid, AbstractQoreNode *ve, class QoreObject *o) 
{
   id = nid; 
   value = NULL;
   vexp = ve;
   obj = o;
   if (o)
      o->tRef();
}
*/

void LVar::set(lvh_t nid, AbstractQoreNode *nvalue) 
{
   id = nid; 
   value = nvalue; 
   vexp = NULL;
   obj = NULL;
}

void LVar::set(lvh_t nid, AbstractQoreNode *ve, class QoreObject *o) 
{
   id = nid; 
   value = NULL;
   vexp = ve;
   obj = o;
   if (o)
      o->tRef();
}

AbstractQoreNode *LVar::evalReference(class ExceptionSink *xsink)
{
   AbstractQoreNode *rv;
   ObjectSubstitutionHelper osh(obj);
   // mask the ID in case it's a recursive reference
   lvh_t save = id;
   id = NULL;
   rv = vexp->eval(xsink);
   //printd(5, "LVar::eval() this=%08p obj=%08p (%s) reference expression %08p (%s) evaluated to %08p (%s)\n", this, obj, obj ? obj->getClass()->name : "NULL", vexp, vexp->getTypeName(), rv, rv ? rv->getTypeName() : "NULL");
   id = save;
   return rv;
}

AbstractQoreNode *LVar::eval(class ExceptionSink *xsink)
{
   if (vexp)
      return evalReference(xsink);
   else
      return value ? value->RefSelf() : NULL;
}

AbstractQoreNode *LVar::eval(bool &needs_deref, class ExceptionSink *xsink)
{
   AbstractQoreNode *rv;

   if (vexp)
   {
      needs_deref = true;
      rv = evalReference(xsink);
   }
   else
   {
      needs_deref = false;
      rv = value;
   }
   
   return rv;
}

AbstractQoreNode **LVar::getValuePtr(class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (vexp)
   {
      // mask the ID in case it's a recursive reference
      lvh_t save = id;
      id = NULL;
      AbstractQoreNode **rv;
      if (obj)
      {
	 ObjectSubstitutionHelper osh(obj);
	 rv = get_var_value_ptr(vexp, vl, xsink);
      }
      else
	 rv = get_var_value_ptr(vexp, vl, xsink);
      id = save;
      return rv;
   }
   return &value;
}

AbstractQoreNode *LVar::getValue(class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (vexp)
   {
      // mask the ID in case it's a recursive reference
      lvh_t save = id;
      id = NULL;
      AbstractQoreNode *rv;
      if (obj)
      {
	 ObjectSubstitutionHelper osh(obj);
	 rv = getNoEvalVarValue(vexp, vl, xsink);
      }
      else
	 rv = getNoEvalVarValue(vexp, vl, xsink);
      id = save;
      return rv;
   }
   return value;
}

void LVar::setValue(AbstractQoreNode *val, class ExceptionSink *xsink)
{
   if (vexp)
   {
      ObjectSubstitutionHelper osh(obj);
      AutoVLock vl;

      // mask the ID in case it's a recursive reference
      lvh_t save = id;
      id = NULL;
      AbstractQoreNode **valp = get_var_value_ptr(vexp, &vl, xsink);
      id = save;

      if (!xsink->isEvent())
      {
	 discard(*valp, xsink);
	 *valp = val;
	 vl.del();
      }
      else
      {
	 vl.del();
	 discard(val, xsink);
      }
   }
   else 
   {
      if (value)
	 value->deref(xsink);
      value = val;
   }
}

void LVar::deref(ExceptionSink *xsink)
{
   // if there is a reference expression, decrement the reference counter
   if (vexp)
   {
      vexp->deref(xsink);
      if (obj)
	 obj->tDeref();
   }
   else
      discard(value, xsink);
   //delete this;
}

static inline AbstractQoreNode **do_list_val_ptr(QoreTreeNode *tree, class AutoVLock *vlp, ExceptionSink *xsink)
{
   // first get index
   int ind = tree->right->integerEval(xsink);
   if (xsink->isEvent())
      return NULL;
   if (ind < 0)
   {
      xsink->raiseException("NEGATIVE-LIST-INDEX", "list index %d is invalid (index must evaluate to a non-negative integer)", ind);
      return NULL;
   }

   // now get left hand side
   AbstractQoreNode **val = get_var_value_ptr(tree->left, vlp, xsink);
   if (xsink->isEvent())
      return NULL;

   // if the variable's value is not already a list, then make it one
   // printd(0, "index=%d val=%08p (%s)\n", ind, *val, *val ? (*val)->getTypeName() : "(null)");
   QoreListNode *l;
   if (!(*val))
      l = new QoreListNode();
   else {
      l = dynamic_cast<QoreListNode *>(*val);
      if (!l)
      {
	 (*val)->deref(xsink);
	 l = new QoreListNode();
      }
      else if (l->reference_count() > 1) // otherwise if it's a list and the reference_count > 1, then duplicate it
      {
	 AbstractQoreNode *old = l;
	 l = l->copy();
	 old->deref(xsink);	 
      }
   }
   (*val) = l;

   return l->get_entry_ptr(ind);
}

// for objects and hashes
static inline AbstractQoreNode **do_object_val_ptr(QoreTreeNode *tree, class AutoVLock *vlp, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder member(tree->right, xsink);
   if (*xsink)
      return 0;

   QoreStringValueHelper mem(*member);

   AbstractQoreNode **val = get_var_value_ptr(tree->left, vlp, xsink);
   if (*xsink)
      return 0;

   //printd(0, "index=%d val=%08p (%s)\n", ind, *val, *val ? (*val)->getTypeName() : "(null)");

   QoreHashNode *h = dynamic_cast<QoreHashNode *>(*val);
   QoreObject *o = 0;

   // if the variable's value is not already a hash or an object, then make it a hash
   if (h) {
      if ((*val)->reference_count() > 1) {
	 // otherwise if the reference_count > 1 (and it's not an object), then duplicate it.
	 QoreHashNode *o = h;
	 h = h->copy();
	 o->deref(xsink);
	 (*val) = h;
      }
   }
   else {
      o = dynamic_cast<QoreObject *>(*val);
      if (!o) {
	 if (*val)
	    (*val)->deref(xsink);
	 h = new QoreHashNode();
	 (*val) = h;
      }
   }

   if (h) {
      //printd(0, "do_object_val_ptr() def=%s member %s \"%s\"\n", QCS_DEFAULT->getCode(), mem->getEncoding()->getCode(), mem->getBuffer());
      return h->getKeyValuePtr(*mem, xsink);
   }

   //printd(5, "do_object_val_ptr() member=%s\n", mem->getBuffer());

   AbstractQoreNode **rv;
   // if object has been deleted, then dereference, make into a hash, and get hash pointer
   if ((rv = o->getMemberValuePtr(*mem, vlp, xsink)))
      return rv;

   if (*xsink)
      return NULL;

   (*val)->deref(xsink);
   h = new QoreHashNode();
   (*val) = h; 
   return h->getKeyValuePtr(*mem, xsink);
}

// this function will change the lvalue to the right type if needed (used for assignments)
AbstractQoreNode **get_var_value_ptr(AbstractQoreNode *n, AutoVLock *vlp, ExceptionSink *xsink)
{
   const QoreType *ntype = n->getType();
   //printd(5, "get_var_value_ptr(%08p) %s\n", n, n->getTypeName());
   if (ntype == NT_VARREF)
   {
      VarRefNode *v = reinterpret_cast<VarRefNode *>(n);
      //printd(5, "get_var_value_ptr(): vref=%s (%08p)\n", v->name, v);
      return v->getValuePtr(vlp, xsink);
   }
   else if (ntype == NT_SELF_VARREF)
   {
      SelfVarrefNode *v = reinterpret_cast<SelfVarrefNode *>(n);
      // need to check for deleted objects
      // note that getStackObject() is guaranteed to return a value here (self varref is only valid in a method)
      AbstractQoreNode **rv = getStackObject()->getMemberValuePtr(v->str, vlp, xsink);
      if (!rv && !xsink->isException())
	 xsink->raiseException("OBJECT-ALREADY-DELETED", "write attempted to member \"%s\" in an already-deleted object", v->str);
      return rv;
   }

   // it must be a tree
   QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(n);
   if (tree->op == OP_LIST_REF)
      return do_list_val_ptr(tree, vlp, xsink);
   return do_object_val_ptr(tree, vlp, xsink);
}

class QoreStringNode **get_string_var_value_ptr(AbstractQoreNode *n, class AutoVLock *vlp, class ExceptionSink *xsink)
{
   AbstractQoreNode **v = get_var_value_ptr(n, vlp, xsink);
   if (!v || *xsink)
      return 0;
   if (dynamic_cast<QoreStringNode *>(*v))
      return reinterpret_cast<QoreStringNode **>(v);
   return 0;
}

// finds value of partially evaluated lvalue expressions (for use with references)
// will not do any evaluations, however, because local variables could hold imported
// object references, exceptions could be thrown
AbstractQoreNode *getNoEvalVarValue(AbstractQoreNode *n, class AutoVLock *vl, class ExceptionSink *xsink)
{
   printd(5, "getNoEvalVarValue(%08p) %s\n", n, n->getTypeName());
   if (n->type == NT_VARREF)
      return reinterpret_cast<VarRefNode *>(n)->getValue(vl, xsink);

   if (n->type == NT_SELF_VARREF)
      return getStackObject()->getMemberValueNoMethod(reinterpret_cast<SelfVarrefNode *>(n)->str, vl, xsink);

   // it's a variable reference tree
   QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(n);
   AbstractQoreNode *val = getNoEvalVarValue(tree->left, vl, xsink);
   if (!val)
      return NULL;

   // if it's a list reference
   if (tree->op == OP_LIST_REF)
   {
      // if it's not a list then return NULL
      QoreListNode *l = dynamic_cast<QoreListNode *>(val);
      if (!l)
	 return 0;
      
      // otherwise return value
      int i;
      if (tree->right)
	 i = tree->right->getAsInt();
      else
	 i = 0;
      return l->retrieve_entry(i);
   }
      
   // it's an object reference
   // if not an object or a hash, return NULL
   QoreHashNode *h = dynamic_cast<QoreHashNode *>(val);
   QoreObject *o = h ? 0 : dynamic_cast<QoreObject *>(val);
   if (!o && !h)
      return 0;
      
   // otherwise get member name
   QoreStringValueHelper key(tree->right);

   if (h)
      return h->getKeyValue(*key, xsink);
   return o->getMemberValueNoMethod(*key, vl, xsink);
}

// finds object value pointers without making any changes to the referenced structures
// will *not* execute memberGate methods
AbstractQoreNode *getExistingVarValue(AbstractQoreNode *n, ExceptionSink *xsink, class AutoVLock *vl, AbstractQoreNode **pt)
{
   printd(5, "getExistingVarValue(%08p) %s\n", n, n->getTypeName());
   const QoreType *ntype = n->getType();
   if (ntype == NT_VARREF)
      return reinterpret_cast<VarRefNode *>(n)->getValue(vl, xsink);

   if (ntype == NT_SELF_VARREF)
      return getStackObject()->getMemberValueNoMethod(reinterpret_cast<SelfVarrefNode *>(n)->str, vl, xsink);

   // it's a variable reference tree
   QoreTreeNode *tree = ntype == NT_TREE ? reinterpret_cast<QoreTreeNode *>(n) : 0;
   if (tree && (tree->op == OP_LIST_REF || tree->op == OP_OBJECT_REF))
   {
      AbstractQoreNode *val = getExistingVarValue(tree->left, xsink, vl, pt);
      if (!val)
	 return 0;

      // if it's a list reference
      if (tree->op == OP_LIST_REF)
      {
	 QoreListNode *l = dynamic_cast<QoreListNode *>(val);
	 // if it's not a list then return NULL
	 if (!l)
	    return 0;

	 // otherwise return value
	 return l->retrieve_entry(tree->right->integerEval(xsink));
      }
      
      // if it's an object reference
      if (tree->op == OP_OBJECT_REF)
      {
	 QoreHashNode *h = dynamic_cast<QoreHashNode *>(val);
	 QoreObject *o = h ? 0 : dynamic_cast<QoreObject *>(val);

	 // if not an object or a hash, return NULL
	 if (!o && !h)
	    return NULL;
	 
	 // otherwise evaluate member
	 QoreNodeEvalOptionalRefHolder member(tree->right, xsink);
	 if (*xsink)
	    return 0;

	 QoreStringValueHelper mem(*member);

	 AbstractQoreNode *rv;
	 if (h)
	    rv = h->getKeyValue(*mem, xsink);
	 else
	    rv = o->getMemberValueNoMethod(*mem, vl, xsink);
	 
	 //traceout("getExistingVarValue()");
	 return rv;
      }
   }
   
   // otherwise need to evaluate
   AbstractQoreNode *t = n->eval(xsink);
   if (xsink->isEvent())
   {
      if (t)
	 (t)->deref(xsink);
      return NULL;
   }

   *pt = t;
   return t;
}

// needed for deletes
static AbstractQoreNode **getUniqueExistingVarValuePtr(AbstractQoreNode *n, ExceptionSink *xsink, class AutoVLock *vl, AbstractQoreNode **pt)
{
   printd(5, "getUniqueExistingVarValuePtr(%08p) %s\n", n, n->getTypeName());
   const QoreType *ntype = n->getType();
   if (ntype == NT_VARREF)
      return reinterpret_cast<VarRefNode *>(n)->getValuePtr(vl, xsink);

   // getStackObject() will always return a value here (self refs are only legal in methods)
   if (ntype == NT_SELF_VARREF)
      return getStackObject()->getExistingValuePtr(reinterpret_cast<SelfVarrefNode *>(n)->str, vl, xsink);

   // it's a variable reference tree
   QoreTreeNode *tree = ntype == NT_TREE ? reinterpret_cast<QoreTreeNode *>(n) : 0;
   if (tree && (tree->op == OP_LIST_REF || tree->op == OP_OBJECT_REF)) {
      AbstractQoreNode **val = getUniqueExistingVarValuePtr(tree->left, xsink, vl, pt);
      if (!val || !(*val))
         return NULL;

      // if it's a list reference
      if (tree->op == OP_LIST_REF) {
	 QoreListNode *l = dynamic_cast<QoreListNode *>(*val);
         // if it's not a list then return NULL
         if (!l)
            return NULL;

         // otherwise return value
         return l->getExistingEntryPtr(tree->right->integerEval(xsink));
      }
      
      QoreHashNode *h = dynamic_cast<QoreHashNode *>(*val);
      QoreObject *o = h ? 0 : dynamic_cast<QoreObject *>(*val);

      // must be an object reference
      // if not an object or a hash, return NULL
      if (!o && !h)
	 return 0;
      
      // otherwise evaluate member
      QoreNodeEvalOptionalRefHolder member(tree->right, xsink);
      if (*xsink)
	 return 0;
   
      QoreStringValueHelper mem(*member);
         
      if (h)
	 return h->getExistingValuePtr(*mem, xsink); 
      else 
	 return o->getExistingValuePtr(*mem, vl, xsink);
   }
   
   // otherwise need to evaluate
   AbstractQoreNode *t = n->eval(xsink);
   if (xsink->isEvent())
   {
      if (t)
         (t)->deref(xsink);
      return NULL;
   }

   *pt = t;
   return pt;
}

void delete_var_node(AbstractQoreNode *lvalue, ExceptionSink *xsink)
{
   AutoVLock vl;
   AbstractQoreNode **val;
   
   const QoreType *lvtype = lvalue->getType();

   // if the node is a variable reference, then find value
   // ptr, dereference it, and return
   if (lvtype == NT_VARREF)
   {
      val = reinterpret_cast<VarRefNode *>(lvalue)->getValuePtr(&vl, xsink);
      if (val && *val)
      {
	 printd(5, "delete_var_node() setting ptr %08p (val=%08p) to NULL\n", val, (*val));
	 QoreObject *o = dynamic_cast<QoreObject *>(*val);
	 if (o) {
	    if (o->isSystemObject())
	       xsink->raiseException("SYSTEM-OBJECT-ERROR", "you cannot delete a system constant object");
	    else
	    {
	       (*val) = NULL;
	       vl.del();
	       o->doDelete(xsink);
	       o->deref(xsink);
	    }
	 }
	 else
	 {
	    (*val)->deref(xsink);
	    *val = NULL;
	 }
      }
      return;
   }

   // delete key if exists and resize hash if necessary
   if (lvtype == NT_SELF_VARREF)
   {
      getStackObject()->deleteMemberValue(reinterpret_cast<SelfVarrefNode *>(lvalue)->str, xsink);
      return;
   }

   // must be a tree
   QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(lvalue);

   // otherwise it is a list or object (hash) reference
   // find variable ptr, exit if doesn't exist anyway
   val = getUniqueExistingVarValuePtr(tree->left, xsink, &vl, NULL);

   if (!val || !(*val) || xsink->isEvent())
      return;

   // if it's a list reference, see if the reference exists
   // if so, then delete it; if it's the last element in the
   // list, then resize the list...
   if (tree->op == OP_LIST_REF)
   {
      QoreListNode *l = dynamic_cast<QoreListNode *>(*val);
      // if it's not a list then return
      if (!l)
	 return;
      // delete the value if it exists and resize the list if necessary
      if (l->reference_count() > 1)
      {
	 l = l->copy();
	 (*val)->deref(xsink);
	 (*val) = l;
      }
      l->delete_entry(tree->right->integerEval(xsink), xsink);
      return;
   }

   QoreHashNode *h = dynamic_cast<QoreHashNode *>(*val);
   QoreObject *o = h ? 0 : dynamic_cast<QoreObject *>(*val);
   // otherwise if not a hash or object then exit
   if (!h && !o)
      return;

   // otherwise get the member name
   QoreNodeEvalOptionalRefHolder member(tree->right, xsink);
   if (*xsink)
      return;

   QoreStringValueHelper mem(*member);

   // get unique value if necessary
   if (h && (*val)->reference_count() > 1)
   {
      QoreHashNode *s = h->copy();
      h->deref(xsink);
      h = s;
      (*val) = h;
   }

   // if it's a hash reference, then delete the key
   if (h)
      h->deleteKey(*mem, xsink);
   else     // must be an object reference
      o->deleteMemberValue(*mem, xsink);

   // release lock(s)
   vl.del();
}

// pops local variable off stack
void uninstantiateLVar(class ExceptionSink *xsink)
{
   thread_uninstantiate_lvar(xsink);
}

// pushes local variable on stack by value
class LVar *instantiateLVar(lvh_t id, AbstractQoreNode *value)
{
   printd(3, "instantiating lvar '%s' by value (val=%08p)\n", id, value);
   // allocate new local variable structure
   class LVar *lvar = thread_instantiate_lvar();
   lvar->set(id, value);

   return lvar;
}

class LVar *instantiateLVar(lvh_t id, AbstractQoreNode *ve, class QoreObject *o)
{
   printd(3, "instantiating lvar %08p '%s' by reference (ve=%08p, o=%08p)\n", id, id, ve, o);
   // if we're instantiating the same variable recursively, then don't instantiate it at all
   // allocate new local variable structure
   class LVar *lvar = thread_instantiate_lvar();
   lvar->set(id, ve, o);
   return lvar;
}

#if 0
static inline void show_lvstack()
{
   class LVar *lvar = get_thread_stack();

   printd(0, "show_lvstack():\n");
   while (lvar)
   {
      AutoVLock vl;
      AbstractQoreNode *n = lvar->getValue(&vl, NULL);
      printd(0, "\t%08p: \"%s\" value=%08p (type=%s)\n", lvar, lvar->id, n, n ? n->getTypeName() : "<NOTHING>");
      vl.del();
      lvar = lvar->next;
   }
}
#endif

// find_lvar() finds local variables on the local variable stack
class LVar *find_lvar(lvh_t id)
{
   return thread_find_lvar(id);
}
