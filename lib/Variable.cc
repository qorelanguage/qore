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
class Hash *ENV;

#include <qore/QoreType.h>

Var::Var(const char *nme, QoreNode *val)
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
      printd(5, "Var::~Var() refptr=%08p\n", v.ivar.refptr);
      v.ivar.refptr->deref(xsink);
      // clear type so no further deleting will be done
   }
   else
   {
      printd(5, "Var::~Var() name=%s value=%08p type=%s refs=%d\n", v.val.name,
	     v.val.value, v.val.value ? v.val.value->type->getName() : "null", 
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
class QoreNode *Var::getValue()
{
   if (refptr)
      return refptr->getValue();
   return value;
}
*/

class QoreNode *Var::eval(class ExceptionSink *xsink)
{
   class QoreNode *rv;

   if (gate.enter(xsink) < 0)
      return NULL;
   if (type == GV_IMPORT)
      rv = v.ivar.refptr->eval(xsink);
   else
   {
      rv = v.val.value;
      if (rv)
	 rv->ref();
      //printd(5, "Var::eval() this=%08p val=%08p (%s)\n", this, rv, rv ? rv->type->getName() : "(null)");
   }
   gate.exit();
   return rv;
}

// note: the caller must exit the gate!
class QoreNode **Var::getValuePtr(class AutoVLock *vl, class ExceptionSink *xsink)
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
      class QoreNode **rv = v.ivar.refptr->getValuePtr(vl, xsink);
      gate.exit();
      return rv;
   }
   vl->push(&gate);
   return &v.val.value;
}

// note: the caller must exit the gate!
class QoreNode *Var::getValue(class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (gate.enter(xsink) < 0)
      return NULL;

   if (type == GV_IMPORT)
   {
      class QoreNode *rv = v.ivar.refptr->getValue(vl, xsink);
      gate.exit();
      return rv;
   }
   vl->push(&gate);
   return v.val.value;
}

void Var::setValue(class QoreNode *val, class ExceptionSink *xsink)
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

LVar::LVar(lvh_t nid, QoreNode *nvalue) 
{
   id = nid; 
   value = nvalue; 
   vexp = NULL;
   obj = NULL;
}

LVar::LVar(lvh_t nid, QoreNode *ve, class Object *o) 
{
   id = nid; 
   value = NULL;
   vexp = ve;
   obj = o;
   if (o)
      o->tRef();
}

class QoreNode *LVar::evalReference(class ExceptionSink *xsink)
{
   class QoreNode *rv;
   ObjectSubstitutionHelper osh(obj);
   // mask the ID in case it's a recursive reference
   lvh_t save = id;
   id = NULL;
   rv = vexp->eval(xsink);
   //printd(5, "LVar::eval() this=%08p obj=%08p (%s) reference expression %08p (%s) evaluated to %08p (%s)\n", this, obj, obj ? obj->getClass()->name : "NULL", vexp, vexp->type->getName(), rv, rv ? rv->type->getName() : "NULL");
   id = save;
   return rv;
}

class QoreNode *LVar::eval(class ExceptionSink *xsink)
{
   if (vexp)
      return evalReference(xsink);
   else
      return value ? value->RefSelf() : NULL;
}

class QoreNode *LVar::eval(bool &needs_deref, class ExceptionSink *xsink)
{
   class QoreNode *rv;

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

class QoreNode **LVar::getValuePtr(class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (vexp)
   {
      // mask the ID in case it's a recursive reference
      lvh_t save = id;
      id = NULL;
      class QoreNode **rv;
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

class QoreNode *LVar::getValue(class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (vexp)
   {
      // mask the ID in case it's a recursive reference
      lvh_t save = id;
      id = NULL;
      class QoreNode *rv;
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

void LVar::setValue(class QoreNode *val, class ExceptionSink *xsink)
{
   if (vexp)
   {
      ObjectSubstitutionHelper osh(obj);
      AutoVLock vl;

      // mask the ID in case it's a recursive reference
      lvh_t save = id;
      id = NULL;
      QoreNode **valp = get_var_value_ptr(vexp, &vl, xsink);
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
   delete this;
}

VarRef::VarRef(char *nme, int typ)
{
   name = nme;
   type = typ;
}

VarRef::~VarRef()
{
   if (name)
   {
      printd(3, "VarRef::~VarRef() deleting variable reference %08p %s\n", name, name);
      free(name);
   }
}

void VarRef::resolve()
{
   lvh_t id;
   if ((id = find_local_var(name)))
   {
      type = VT_LOCAL;
      ref.id = id;
      printd(5, "VarRef::resolve(): local var %s resolved (id=%08p)\n", name, ref.id);
   }
   else
   {
      ref.var = getProgram()->checkVar(name);
      type = VT_GLOBAL;
      printd(5, "VarRef::resolve(): global var %s resolved (var=%08p)\n", name, ref.var);
   }
}

// returns 0 for OK, 1 for would be a new variable
int VarRef::resolveExisting()
{
   lvh_t id;
   if ((id = find_local_var(name)))
   {
      type = VT_LOCAL;
      ref.id = id;
      printd(5, "VarRef::resolveExisting(): local var %s resolved (id=%08p)\n", name, ref.id);
      return 0;
   }
   
   ref.var = getProgram()->findVar(name);
   type = VT_GLOBAL;
   printd(5, "VarRef::resolveExisting(): global var %s resolved (var=%08p)\n", name, ref.var);
   return !ref.var;
}

VarRef *VarRef::copy()
{
   class VarRef *v = new VarRef();
   memcpy(v, this, sizeof(class VarRef));
   v->name = strdup(name);
   return v;
}

class QoreNode *VarRef::eval(class ExceptionSink *xsink)
{
   if (type == VT_LOCAL)
   {
      printd(5, "VarRef::eval() lvar %08p (%s)\n", ref.id, ref.id);
      return find_lvar(ref.id)->eval(xsink);
   }
   printd(5, "VarRef::eval() global var=%08p\n", ref.var);
   return ref.var->eval(xsink);
}

class QoreNode *VarRef::eval(bool &needs_deref, class ExceptionSink *xsink)
{
   if (type == VT_LOCAL)
      return find_lvar(ref.id)->eval(needs_deref, xsink);
   needs_deref = true;
   return ref.var->eval(xsink);
}

class QoreNode **VarRef::getValuePtr(class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (type == VT_LOCAL)
      return find_lvar(ref.id)->getValuePtr(vl, xsink);
   return ref.var->getValuePtr(vl, xsink);
}

class QoreNode *VarRef::getValue(class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (type == VT_LOCAL)
      return find_lvar(ref.id)->getValue(vl, xsink);
   return ref.var->getValue(vl, xsink);
}

void VarRef::setValue(class QoreNode *val, class ExceptionSink *xsink)
{
   if (type == VT_LOCAL)
      find_lvar(ref.id)->setValue(val, xsink);
   else
      ref.var->setValue(val, xsink);
}

static inline class QoreNode **do_list_val_ptr(Tree *tree, class AutoVLock *vlp, ExceptionSink *xsink)
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
   QoreNode **val = get_var_value_ptr(tree->left, vlp, xsink);
   if (xsink->isEvent())
      return NULL;

   // if the variable's value is not already a list, then make it one
   // printd(0, "index=%d val=%08p (%s)\n", ind, *val, *val ? (*val)->type->getName() : "(null)");
   if (!(*val))
   {
      (*val) = new QoreNode(NT_LIST);
      (*val)->val.list = new List();
   }
   else if ((*val)->type != NT_LIST)
   {
      (*val)->deref(xsink);
      (*val) = new QoreNode(NT_LIST);
      (*val)->val.list = new List();
   }
   else if ((*val)->reference_count() > 1) // otherwise if it's a list and the reference_count > 1, then duplicate it
   {
      QoreNode *old = (*val);
      (*val) = (*val)->realCopy(xsink);
      old->deref(xsink);
   }

   return (*val)->val.list->get_entry_ptr(ind);
}

// for objects and hashes
static inline class QoreNode **do_object_val_ptr(Tree *tree, class AutoVLock *vlp, ExceptionSink *xsink)
{
   class QoreNode *member;

   if (!(member = tree->right->eval(xsink)))
      member = null_string();
   else if (member->type != NT_STRING)
   {
      class QoreNode *nm = member->convert(NT_STRING);
      member->deref(xsink);
      member = nm;
   }

   QoreNode **val = get_var_value_ptr(tree->left, vlp, xsink);
   if (xsink->isEvent())
   {
      member->deref(xsink);
      return NULL;
   }

   // if the variable's value is not already a hash or an object, then make it a hash
   //printd(0, "index=%d val=%08p (%s)\n", ind, *val, *val ? (*val)->type->getName() : "(null)");
   if (!(*val))
      (*val) = new QoreNode(new Hash());
   else if ((*val)->type != NT_OBJECT && (*val)->type != NT_HASH)
   {
      (*val)->deref(xsink);
      (*val) = new QoreNode(new Hash());
   }
   // otherwise if the reference_count > 1, then duplicate it.
   else if ((*val)->reference_count() > 1)
   {
      QoreNode *old = (*val);
      (*val) = (*val)->realCopy(xsink);
      old->deref(xsink);
   }

   class QoreNode **rv;
   if ((*val)->type == NT_HASH)
   {
      //printd(0, "do_object_val_ptr() def=%s member %s \"%s\"\n", QCS_DEFAULT->getCode(), member->val.String->getEncoding()->getCode(), member->val.String->getBuffer());
      rv = (*val)->val.hash->getKeyValuePtr(member->val.String, xsink);
   }
   else
   {
      // if object has been deleted, then dereference, make into a hash, and get hash pointer
      if (!(rv = (*val)->val.object->getMemberValuePtr(member->val.String, vlp, xsink)))
      {
	 if (xsink->isException())
	    rv = NULL;
	 else
	 {
	    (*val)->deref(xsink);
	    (*val) = new QoreNode(new Hash());
	    rv = (*val)->val.hash->getKeyValuePtr(member->val.String, xsink);
	 }
      }
   }
   //printd(5, "do_object_val_ptr() member=%s\n", member->val.String->getBuffer());
   member->deref(xsink);

   return rv;
}

// this function will change the lvalue to the right type if needed (used for assignments)
class QoreNode **get_var_value_ptr(class QoreNode *n, class AutoVLock *vlp, class ExceptionSink *xsink)
{
   //printd(5, "get_var_value_ptr(%08p) %s\n", n, n->type->getName());
   if (n->type == NT_VARREF)
   {
      //printd(5, "get_var_value_ptr(): vref=%s (%08p)\n", n->val.vref->name, n->val.vref);
      return n->val.vref->getValuePtr(vlp, xsink);
   }
   else if (n->type == NT_SELF_VARREF)
   {
      // need to check for deleted objects
      // note that getStackObject() is guaranteed to return a value here (self varref is only valid in a method)
      QoreNode **rv = getStackObject()->getMemberValuePtr(n->val.c_str, vlp, xsink);
      if (!rv && !xsink->isException())
	 xsink->raiseException("OBJECT-ALREADY-DELETED", "write attempted to member \"%s\" in an already-deleted object", n->val.c_str);
      return rv;
   }

   // it's a tree
   if (n->val.tree->op == OP_LIST_REF)
      return do_list_val_ptr(n->val.tree, vlp, xsink);
   return do_object_val_ptr(n->val.tree, vlp, xsink);
}

// finds value of partially evaluated lvalue expressions (for use with references)
// will not do any evaluations, however, because local variables could hold imported
// object references, exceptions could be thrown
class QoreNode *getNoEvalVarValue(class QoreNode *n, class AutoVLock *vl, class ExceptionSink *xsink)
{
   printd(5, "getNoEvalVarValue(%08p) %s\n", n, n->type->getName());
   if (n->type == NT_VARREF)
      return n->val.vref->getValue(vl, xsink);

   if (n->type == NT_SELF_VARREF)
      return getStackObject()->getMemberValueNoMethod(n->val.c_str, vl, xsink);

   // it's a variable reference tree
   class QoreNode *val = getNoEvalVarValue(n->val.tree->left, vl, xsink);
   if (!val)
      return NULL;

   // if it's a list reference
   if (n->val.tree->op == OP_LIST_REF)
   {
      // if it's not a list then return NULL
      if (val->type != NT_LIST)
	 return NULL;
      
      // otherwise return value
      int i;
      if (n->val.tree->right)
	 i = n->val.tree->right->getAsInt();
      else
	 i = 0;
      return val->val.list->retrieve_entry(i);
   }
      
   // it's an object reference
   // if not an object or a hash, return NULL
   if (val->type != NT_OBJECT && val->type != NT_HASH)
      return NULL;
      
   // otherwise get member name
   if (!n->val.tree->right)
   {
      if (val->type == NT_HASH)
	 return val->val.hash->getKeyValue("");
      return val->val.object->getMemberValueNoMethod("", vl, xsink);
   }
   
   if (n->val.tree->right->type == NT_STRING)
   {
      if (val->type == NT_HASH)
	 return val->val.hash->getKeyValue(n->val.tree->right->val.String, xsink);
      return val->val.object->getMemberValueNoMethod(n->val.tree->right->val.String, vl, xsink);
   }
   
   class QoreNode *nm = n->val.tree->right->convert(NT_STRING);
   
   class QoreNode *rv;
   if (val->type == NT_HASH)
      rv = val->val.hash->getKeyValue(nm->val.String, xsink);
   else
      rv = val->val.object->getMemberValueNoMethod(nm->val.String, vl, xsink);
   nm->deref(xsink);
   
   return rv;
}

// finds object value pointers without making any changes to the referenced structures
// will *not* execute memberGate methods
class QoreNode *getExistingVarValue(class QoreNode *n, ExceptionSink *xsink, class AutoVLock *vl, class QoreNode **pt)
{
   printd(5, "getExistingVarValue(%08p) %s\n", n, n->type->getName());
   if (n->type == NT_VARREF)
      return n->val.vref->getValue(vl, xsink);

   if (n->type == NT_SELF_VARREF)
      return getStackObject()->getMemberValueNoMethod(n->val.c_str, vl, xsink);

   // it's a variable reference tree
   if (n->type == NT_TREE && (n->val.tree->op == OP_LIST_REF || n->val.tree->op == OP_OBJECT_REF))
   {
      class QoreNode *val = getExistingVarValue(n->val.tree->left, xsink, vl, pt);
      if (!val)
	 return NULL;

      // if it's a list reference
      if (n->val.tree->op == OP_LIST_REF)
      {
	 // if it's not a list then return NULL
	 if (val->type != NT_LIST)
	    return NULL;

	 // otherwise return value
	 return val->val.list->retrieve_entry(n->val.tree->right->integerEval(xsink));
      }
      
      // if it's an object reference
      if (n->val.tree->op == OP_OBJECT_REF)
      {
	 // if not an object or a hash, return NULL
	 if (val->type != NT_OBJECT && val->type != NT_HASH)
	    return NULL;
	 
	 // otherwise evaluate member
	 class QoreNode *member;
	 if (!(member = n->val.tree->right->eval(xsink)))
	    member = null_string();
	 else if (member->type != NT_STRING)
	 {
	    class QoreNode *nm = member->convert(NT_STRING);
	    member->deref(xsink);
	    member = nm;
	 }
	 
	 class QoreNode *rv;
	 if (val->type == NT_HASH)
	    rv = val->val.hash->getKeyValue(member->val.String, xsink);
	 else
	    rv = val->val.object->getMemberValueNoMethod(member->val.String, vl, xsink);
	 member->deref(xsink);
	 
	 //traceout("getExistingVarValue()");
	 return rv;
      }
   }
   
   // otherwise need to evaluate
   class QoreNode *t = n->eval(xsink);
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
static class QoreNode **getUniqueExistingVarValuePtr(class QoreNode *n, ExceptionSink *xsink, class AutoVLock *vl, class QoreNode **pt)
{
   printd(5, "getUniqueExistingVarValuePtr(%08p) %s\n", n, n->type->getName());
   if (n->type == NT_VARREF)
      return n->val.vref->getValuePtr(vl, xsink);

   // getStackObjecT() will always return a value here (self refs are only legal in methods)
   if (n->type == NT_SELF_VARREF)
      return getStackObject()->getExistingValuePtr(n->val.c_str, vl, xsink);

   // it's a variable reference tree
   if (n->type == NT_TREE && (n->val.tree->op == OP_LIST_REF || n->val.tree->op == OP_OBJECT_REF))
   {
      class QoreNode **val = getUniqueExistingVarValuePtr(n->val.tree->left, xsink, vl, pt);
      if (!val || !(*val))
         return NULL;

      // get a unique object if necessary
      if ((*val)->reference_count() > 1)
      {
	 QoreNode *cp = (*val)->realCopy(xsink);
	 (*val)->deref(xsink);
	 (*val) = cp;
	 if (xsink->isEvent())
	    return NULL;
      }

      // if it's a list reference
      if (n->val.tree->op == OP_LIST_REF)
      {
         // if it's not a list then return NULL
         if ((*val)->type != NT_LIST)
            return NULL;

         // otherwise return value
         return (*val)->val.list->getExistingEntryPtr(n->val.tree->right->integerEval(xsink));
      }
      
      // if it's an object reference
      if (n->val.tree->op == OP_OBJECT_REF)
      {
         // if not an object or a hash, return NULL
         if ((*val)->type != NT_OBJECT && (*val)->type != NT_HASH)
            return NULL;
         
         // otherwise evaluate member
         class QoreNode *member;
         if (!(member = n->val.tree->right->eval(xsink)))
            member = null_string();
         else if (member->type != NT_STRING)
         {
            class QoreNode *nm = member->convert(NT_STRING);
            member->deref(xsink);
            member = nm;
         }
         
         class QoreNode **rv;
         if ((*val)->type == NT_HASH)
            rv = (*val)->val.hash->getExistingValuePtr(member->val.String, xsink);
         else
            rv = (*val)->val.object->getExistingValuePtr(member->val.String, vl, xsink);
         member->deref(xsink);
         
         //traceout("getUniqueExistingVarValuePtr()");
         return rv;
      }
   }
   
   // otherwise need to evaluate
   class QoreNode *t = n->eval(xsink);
   if (xsink->isEvent())
   {
      if (t)
         (t)->deref(xsink);
      return NULL;
   }

   *pt = t;
   return pt;
}

void delete_var_node(class QoreNode *lvalue, ExceptionSink *xsink)
{
   class AutoVLock vl;
   class QoreNode **val;

   //tracein("delete_var_node()");

   // if the node is a variable reference, then find value
   // ptr, dereference it, and return
   if (lvalue->type == NT_VARREF)
   {
      val = lvalue->val.vref->getValuePtr(&vl, xsink);
      if (val && *val)
      {
	 printd(5, "delete_var_node() setting ptr %08p (val=%08p) to NULL\n", val, (*val));
	 if ((*val)->type == NT_OBJECT)
	 {
	    if ((*val)->val.object->isSystemObject())
	       xsink->raiseException("SYSTEM-OBJECT-ERROR", "you cannot delete a system constant object");
	    else
	    {
	       QoreNode *n = (*val);
	       (*val) = NULL;
	       vl.del();
	       n->val.object->doDelete(xsink);
	       n->deref(xsink);
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
   if (lvalue->type == NT_SELF_VARREF)
   {
      getStackObject()->deleteMemberValue(lvalue->val.c_str, xsink);
      return;
   }

   // otherwise it is a list or object (hash) reference
   // find variable ptr, exit if doesn't exist anyway
   //val = get_var_value_ptr(lvalue->val.tree->left, vl, xsink);
   val = getUniqueExistingVarValuePtr(lvalue->val.tree->left, xsink, &vl, NULL);

   if (!val || !(*val) || xsink->isEvent())
      return;

   // if it's a list reference, see if the reference exists
   // if so, then delete it; if it's the last element in the
   // list, then resize the list...
   if (lvalue->val.tree->op == OP_LIST_REF)
   {
      // if it's not a list then return
      if ((*val)->type != NT_LIST)
	 return;
      // delete the value if it exists and resize the list if necessary
      if ((*val)->reference_count() > 1)
      {
	 QoreNode *s = *val;
	 *val = (*val)->realCopy(xsink);
	 s->deref(xsink);
      }
      (*val)->val.list->delete_entry(lvalue->val.tree->right->integerEval(xsink), xsink);
      return;
   }

   // otherwise if not a hash or object then exit
   if ((*val)->type != NT_HASH && (*val)->type != NT_OBJECT)
      return;

   // otherwise get the member name
   class QoreNode *member;
   if (!(member = lvalue->val.tree->right->eval(xsink)))
      member = null_string();
   else if (member->type != NT_STRING)
   {
      class QoreNode *nm = member->convert(NT_STRING);
      member->deref(xsink);
      member = nm;
   }

   // get unique value if necessary
   if ((*val)->reference_count() > 1)
   {
      QoreNode *s = *val;
      *val = (*val)->realCopy(xsink);
      s->deref(xsink);
   }

   // if it's a hash reference, then delete the key
   if ((*val)->type == NT_HASH)
      (*val)->val.hash->deleteKey(member->val.String, xsink);
   else     // must be an object reference
      (*val)->val.object->deleteMemberValue(member->val.String, xsink);

   // release lock(s)
   vl.del();

   // dereference member node
   member->deref(xsink);
   
   // traceout("delete_var_node()");
}

// pops local variable off stack
void uninstantiateLVar(class ExceptionSink *xsink)
{
   class LVar *lvs = get_thread_stack();
   class LVar *lvar = lvs;

   //tracein("uninstantiateLVar()");
   assert(lvs);
   printd(5, "uninstantiating lvar \"%s\"\n", lvs->id);
   update_thread_stack(lvs->next);

   // the following call will delete the local variable object
   lvar->deref(xsink);
   //traceout("uninstantiateLVar()");
}

// pushes local variable on stack by value
class LVar *instantiateLVar(lvh_t id, class QoreNode *value)
{
   printd(3, "instantiating lvar '%s' by value (val=%08p)\n", id, value);
   // allocate new local variable structure
   class LVar *lvar = new LVar(id, value);
   // push on stack
   lvar->next = get_thread_stack();
   update_thread_stack(lvar);

   return lvar;
}

class LVar *instantiateLVar(lvh_t id, class QoreNode *ve, class Object *o)
{
   class LVar *lvar;

   printd(3, "instantiating lvar %08p '%s' by reference (ve=%08p, o=%08p)\n", id, id, ve, o);
   // if we're instantiating the same variable recursively, then don't instantiate it at all
   // allocate new local variable structure
   lvar = new LVar(id, ve, o);
   // push on stack
   lvar->next = get_thread_stack();
   update_thread_stack(lvar);

   return lvar;
}

/*
// pushes local variable on stack by reference
static inline class LVar *instantiateLVarRef(lvh_t id, class QoreNode **ptr, class VRMutex *eg)
{
   printd(3, "instantiating lvar \"%s\" by reference (ptr=%08p val=%08p)\n", id, ptr, *ptr);
   // allocate new local variable structure
   class LVar *lvar = new LVar(id, ptr, eg);
   // push on stack
   lvar->next = get_thread_stack();
   update_thread_stack(lvar);

   return lvar;
}
*/

#ifdef DEBUG
static inline void show_lvstack()
{
   class LVar *lvar = get_thread_stack();

   printd(0, "show_lvstack():\n");
   while (lvar)
   {
      AutoVLock vl;
      QoreNode *n = lvar->getValue(&vl, NULL);
      printd(0, "\t%08p: \"%s\" value=%08p (type=%s)\n", lvar, lvar->id, n, n ? n->type->getName() : "<NOTHING>");
      vl.del();
      lvar = lvar->next;
   }
}
#endif

// find_lvar() finds local variables on the local variable stack
class LVar *find_lvar(lvh_t id)
{
   class LVar *lvar = get_thread_stack();

   while (lvar)
   {
      //printd(5, "find_lvar(%s) 0x%08p \"%s\" (%08p == %08p) (0x%08p %s) (next=0x%08p)\n", id, lvar, lvar->id, lvar->id, id, lvar->getValue(), lvar->getValue() ? lvar->getValue()->type->getName() : "(null)", lvar->next);
      if (lvar->id == id)
         break;
      lvar = lvar->next;
   }
#ifdef DEBUG
   if (!lvar)
   {
      show_lvstack();
      printd(0, "find_lvar(): local variable %08p (%s) not found on stack!", id, id);
      assert(false);
   }
#endif
   return lvar;
}
