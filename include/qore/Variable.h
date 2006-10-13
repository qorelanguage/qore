/*
  Variable.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#ifndef _QORE_VARIABLE_H

#define _QORE_VARIABLE_H

#define VT_UNRESOLVED 1
#define VT_LOCAL      2
#define VT_GLOBAL     3
#define VT_OBJECT     4  // used for references only

#include <qore/common.h>
#include <qore/ReferenceObject.h>
#include <qore/LockedObject.h>
#include <qore/RMutex.h>

#include <string.h>
#include <stdlib.h>

// structure for local variables
class LVar {
   private:
      class QoreNode *value;
      class QoreNode *vexp;  // partially evaluated lvalue expression for references
      class Object *obj;     // for references to object members

   protected:
      inline ~LVar() {}

   public:
      lvh_t id;
      class LVar *next;

      inline LVar(lvh_t nid, class QoreNode *nvalue);
      inline LVar(lvh_t nid, class QoreNode *ve, class Object *o);

      inline class QoreNode **getValuePtr(class VLock *vl, class ExceptionSink *xsink);
      inline class QoreNode *getValue(class VLock *vl, class ExceptionSink *xsink);
      inline void setValue(class QoreNode *val, class ExceptionSink *xsink);
      inline class QoreNode *eval(class ExceptionSink *xsink);
      inline bool checkRecursiveReference(lvh_t nid);

      inline void deref(class ExceptionSink *xsink);
      //inline class LVar *copy();
      //inline class QoreNode *getValue();
};

#define GV_VALUE  1
#define GV_IMPORT 2

// structure for global variables
class Var : public ReferenceObject
{
   private:
      int type;
      // holds the value of the variable or a pointer to the imported variable
      union 
      {
	    // for value
	    class
	    {
	       public:
		  class QoreNode *value;
		  char *name;
	    } val;
	    // for imported variables
	    class 
	    {
	       public:
		  class Var *refptr;
		  bool readonly;
	    } ivar;
      } v;

      class RMutex gate;

      inline void del(class ExceptionSink *xsink);

   protected:
      inline ~Var() {}

   public:
      //class Var *next;

      inline Var(char *nme, class QoreNode *val = NULL);
      inline Var(class Var *ref, bool ro = false);
      inline char *getName();
      inline void setValue(class QoreNode *val, class ExceptionSink *xsink);
      inline void makeReference(class Var *v, class ExceptionSink *xsink, bool ro = false);
      inline bool isImported()
      {
	 return type == GV_IMPORT;
      }
      inline void deref(class ExceptionSink *xsink);
      inline class QoreNode *eval();

      inline class QoreNode **getValuePtr(class VLock *vl, class ExceptionSink *xsink);
      inline class QoreNode *getValue(class VLock *vl);
};

class VarRef {
   public:
      char *name;
      int type;
      union var_u {
	    lvh_t id;          // for local variables
	    class Var *var;    // for global variables
      } ref;
      inline VarRef(char *n, int t);
      inline VarRef() {}
      inline ~VarRef();
      inline void resolve();
      // returns -1 if the variable did not already exist
      int resolveExisting();
      inline class VarRef *copy();
      inline class QoreNode *eval(class ExceptionSink *xsink);
      inline void setValue(class QoreNode *val, class ExceptionSink *xsink);
      inline class QoreNode **getValuePtr(class VLock *vl, class ExceptionSink *xsink);
      inline class QoreNode *getValue(class VLock *vl, class ExceptionSink *xsink);
};

// VLNode and VLock are for nested locks when updating variables and objects
class VLNode {
   public:
      class RMutex *g;
      class VLNode *next;

      inline VLNode(class RMutex *gate) 
      {
	 g = gate;
	 next = NULL;
      }
};

// for locking
class VLock {
   private:
      class VLNode *head;
      class VLNode *tail;

   public:
      inline VLock()
      {
	 head = NULL;
	 tail = NULL;
      }
      inline ~VLock()
      {
	 del();
      }
      inline void add(class RMutex *g);
      inline void del()
      {
	 while (head)
	 {
	    tail = head->next;
	    head->g->exit();
	    delete head;
	    head = tail;
	 }
      }
};

void qore_setup_argv(int pos, int argc, char *argv[]);
void initProgramGlobalVars(char *env[]);
void delete_var_node(class QoreNode *node, class ExceptionSink *xsink);
void delete_global_variables();

class QoreNode **get_var_value_ptr(class QoreNode *lvalue, class VLock *vl, class ExceptionSink *xsink);
class QoreNode *getNoEvalVarValue(class QoreNode *n, class VLock *vl, class ExceptionSink *xsink);
class QoreNode *getExistingVarValue(class QoreNode *n, class ExceptionSink *xsink, class VLock *vl, class TempNode **pt);

#if 0
static inline class VarRef *getVarRefByName(char *name);
static inline class LVar *findLVar(char *name);
#endif

static inline class LVar *instantiateLVar(lvh_t id, class QoreNode *value);
static inline class LVar *instantiateLVar(lvh_t id, class QoreNode *ve, class Object *o);
static inline void uninstantiateLVar(class ExceptionSink *xsink);

#include <qore/QoreNode.h>
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/qore_thread.h>
#include <qore/QoreProgram.h>
#include <qore/Object.h>

inline void VLock::add(class RMutex *g)
{
   class VLNode *n = new VLNode(g);
   if (tail)
   {
      //run_time_error("VLock::add() > 1 lock count!");
      tail->next = n;
   }
   else
      head = n;
   tail = n;
}

class TempNode {
   public:
      QoreNode *val;
      inline TempNode(class QoreNode *v) 
      { 
	 val = v; 
      }
      inline void del(ExceptionSink *xsink) 
      { 
	 if (val) 
	    val->deref(xsink); 
	 delete this; 
      }
};

inline Var::Var(char *nme, QoreNode *val)
{
   type = GV_VALUE;
   v.val.value = val;
   v.val.name = strdup(nme);
   //next = NULL;
}

inline Var::Var(class Var *ref, bool ro)
{
   type = GV_IMPORT;
   v.ivar.refptr = ref;
   v.ivar.readonly = ro;
   ref->ROreference();
   //next = NULL;
}

#ifdef DEBUG
#include <qore/QoreType.h>
#endif

inline void Var::del(class ExceptionSink *xsink)
{
   if (type == GV_IMPORT)
   {
      printd(5, "Var::~Var() refptr=%08p\n", v.ivar.refptr);
      v.ivar.refptr->deref(xsink);
      // clear type so no further deleting will be done
   }
   else
   {
      printd(5, "Var::~Var() name=%s value=%08p type=%s refs=%d\n", v.val.name ? v.val.name : "(null)",
	     v.val.value, v.val.value ? v.val.value->type->name : "null", 
	     v.val.value ? v.val.value->reference_count() : 0);
   
      if (v.val.name)
	 free(v.val.name);
      if (v.val.value)
	 v.val.value->deref(xsink);
      // clear type so no further deleting will be done
   }
}

inline char *Var::getName()
{
   if (type == GV_IMPORT)
      return v.ivar.refptr->getName();
   return v.val.name;
}

/*
inline class QoreNode *Var::getValue()
{
   if (refptr)
      return refptr->getValue();
   return value;
}
*/

inline class QoreNode *Var::eval()
{
   class QoreNode *rv;

   gate.enter();
   if (type == GV_IMPORT)
      rv = v.ivar.refptr->eval();
   else
   {
      rv = v.val.value;
      if (rv)
	 rv->ref();
   }
   gate.exit();
   return rv;
}

// note: the caller must exit the gate!
inline class QoreNode **Var::getValuePtr(class VLock *vl, class ExceptionSink *xsink)
{
   gate.enter();
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
   vl->add(&gate);
   return &v.val.value;
}

// note: the caller must exit the gate!
inline class QoreNode *Var::getValue(class VLock *vl)
{
   gate.enter();
   if (type == GV_IMPORT)
   {
      class QoreNode *rv = v.ivar.refptr->getValue(vl);
      gate.exit();
      return rv;
   }
   vl->add(&gate);
   return v.val.value;
}

inline void Var::setValue(class QoreNode *val, class ExceptionSink *xsink)
{
   gate.enter();
   if (type == GV_IMPORT)
   {
      if (v.ivar.readonly)
      {
	 gate.exit();
	 xsink->raiseException("ACCESS-ERROR", "attempt to write to read-only variable $%s", v.ivar.refptr->getName());
	 return;
      }
      v.ivar.refptr->setValue(val, xsink);
   }
   else
   {
      if (v.val.value)
	 v.val.value->deref(xsink);
      v.val.value = val;
   }
   gate.exit();
}

inline void Var::makeReference(class Var *pvar, class ExceptionSink *xsink, bool ro)
{
   gate.enter();
   if (type == GV_IMPORT)
      v.ivar.refptr->deref(xsink);
   else
   {
      if (v.val.value)
	 v.val.value->deref(xsink);
      if (v.val.name)
	 free(v.val.name);
   }
   type = GV_IMPORT;
   v.ivar.refptr = pvar;
   v.ivar.readonly = ro;
   pvar->ROreference();
   gate.exit();
}

inline void Var::deref(class ExceptionSink *xsink)
{
   if (ROdereference())
   {
      del(xsink);
      delete this;
   }
}

inline LVar::LVar(lvh_t nid, QoreNode *nvalue) 
{
   id = nid; 
   value = nvalue; 
   vexp = NULL;
   obj = NULL;
}

inline LVar::LVar(lvh_t nid, QoreNode *ve, class Object *o) 
{
   id = nid; 
   value = NULL;
   vexp = ve;
   obj = o;
}

inline class QoreNode *LVar::eval(class ExceptionSink *xsink)
{
   class QoreNode *rv;

   if (vexp)
   {
      class Object *o = NULL;
      if (obj)
	 o = substituteObject(obj);
      // mask the ID in case it's a recursive reference
      lvh_t save = id;
      id = NULL;
      rv = vexp->eval(xsink);
      //printd(5, "LVar::eval() this=%08p obj=%08p (%s) reference expression %08p (%s) evaluated to %08p (%s)\n", this, obj, obj ? obj->getClass()->name : "NULL", vexp, vexp->type->name, rv, rv ? rv->type->name : "NULL");
      id = save;
      if (obj)
	 substituteObject(o);
   }
   else
      rv = value ? value->RefSelf() : NULL;

   return rv;
}

inline class QoreNode **LVar::getValuePtr(class VLock *vl, class ExceptionSink *xsink)
{
   if (vexp)
   {
      // mask the ID in case it's a recursive reference
      lvh_t save = id;
      id = NULL;
      class QoreNode **rv;
      if (obj)
      {
	 class Object *o = substituteObject(obj);
	 rv = get_var_value_ptr(vexp, vl, xsink);
	 substituteObject(o);
      }
      else
	 rv = get_var_value_ptr(vexp, vl, xsink);
      id = save;
      return rv;
   }
   return &value;
}

inline class QoreNode *LVar::getValue(class VLock *vl, class ExceptionSink *xsink)
{
   if (vexp)
   {
      // mask the ID in case it's a recursive reference
      lvh_t save = id;
      id = NULL;
      class QoreNode *rv;
      if (obj)
      {
	 class Object *o = substituteObject(obj);
	 rv = getNoEvalVarValue(vexp, vl, xsink);
	 substituteObject(o);
      }
      else
	 rv = getNoEvalVarValue(vexp, vl, xsink);
      id = save;
      return rv;
   }
   return value;
}

inline void LVar::setValue(class QoreNode *val, class ExceptionSink *xsink)
{
   if (vexp)
   {
      class Object *o = NULL;
      if (obj)
	 o = substituteObject(obj);
      VLock vl;

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
      if (obj)
	 substituteObject(o);
   }
   else 
   {
      if (value)
	 value->deref(xsink);
      value = val;
   }
}

inline void LVar::deref(ExceptionSink *xsink)
{
   // if the variable was passed by reference, then write the value back to the
   // vexp lvalue
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

// pops local variable off stack
static inline void uninstantiateLVar(class ExceptionSink *xsink)
{
   class LVar *lvs = get_thread_stack();
   class LVar *lvar = lvs;

   //tracein("uninstantiateLVar()");
#ifdef DEBUG
   if (!lvs) { run_time_error("uninstantiateLVar(): ERROR empty stack! aborting"); }
#endif
   printd(5, "uninstantiating lvar \"%s\"\n", lvs->id);
   update_thread_stack(lvs->next);

   // the following call will delete the local variable object
   lvar->deref(xsink);
   //traceout("uninstantiateLVar()");
}

// pushes local variable on stack by value
static inline class LVar *instantiateLVar(lvh_t id, class QoreNode *value)
{
   printd(3, "instantiating lvar '%s' by value (val=%08p)\n", id, value);
   // allocate new local variable structure
   class LVar *lvar = new LVar(id, value);
   // push on stack
   lvar->next = get_thread_stack();
   update_thread_stack(lvar);

   return lvar;
}

static inline class LVar *instantiateLVar(lvh_t id, class QoreNode *ve, class Object *o)
{
   class LVar *lvar;

   printd(3, "instantiating lvar %08p '%s' by reference (ve=%08p, o=%08p)\n", id, id, ve, o);
   // if we're instantiating the same variable recursively, then don't instantiate it at all
   // allocate new local variable structure
   lvar = new LVar(id, ve, o);
   if (o)
      o->tRef();
   // push on stack
   lvar->next = get_thread_stack();
   update_thread_stack(lvar);

   return lvar;
}

/*
// pushes local variable on stack by reference
static inline class LVar *instantiateLVarRef(lvh_t id, class QoreNode **ptr, class RMutex *eg)
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
      VLock vl;
      QoreNode *n = lvar->getValue(&vl, NULL);
      printd(0, "\t%08p: \"%s\" value=%08p (type=%s)\n", lvar, lvar->id, n, n ? n->type->name : "<NOTHING>");
      vl.del();
      lvar = lvar->next;
   }
}
#endif

// find_lvar() finds local variables on the local variable stack
static inline class LVar *find_lvar(lvh_t id)
{
   class LVar *lvar = get_thread_stack();

   while (lvar)
   {
      //printd(5, "find_lvar(%s) 0x%08p \"%s\" (%08p == %08p) (0x%08p %s) (next=0x%08p)\n", id, lvar, lvar->id, lvar->id, id, lvar->getValue(), lvar->getValue() ? lvar->getValue()->type->name : "(null)", lvar->next);
      if (lvar->id == id)
         break;
      lvar = lvar->next;
   }
#ifdef DEBUG
   if (!lvar)
   {
      show_lvstack();
      run_time_error("find_lvar(): local variable %08p (%s) not found on stack!", id, id);
   }
#endif
   return lvar;
}

inline VarRef::VarRef(char *nme, int typ)
{
   name = nme;
   type = typ;
}

inline VarRef::~VarRef()
{
   if (name)
   {
      printd(3, "deleting variable reference %08p %s\n", name, name);
      free(name);
   }
}

inline VarRef *VarRef::copy()
{
   class VarRef *v = new VarRef();
   memcpy(v, this, sizeof(class VarRef));
   v->name = strdup(name);
   return v;
}

inline class QoreNode *VarRef::eval(class ExceptionSink *xsink)
{
   if (type == VT_LOCAL)
   {
      printd(5, "VarRef::eval() lvar %08p (%s)\n", ref.id, ref.id);
      return find_lvar(ref.id)->eval(xsink);
   }
   printd(5, "VarRef::eval() global var=%08p\n", ref.var);
   return ref.var->eval();
}

inline class QoreNode **VarRef::getValuePtr(class VLock *vl, class ExceptionSink *xsink)
{
   if (type == VT_LOCAL)
      return find_lvar(ref.id)->getValuePtr(vl, xsink);
   return ref.var->getValuePtr(vl, xsink);
}

inline class QoreNode *VarRef::getValue(class VLock *vl, class ExceptionSink *xsink)
{
   if (type == VT_LOCAL)
      return find_lvar(ref.id)->getValue(vl, xsink);
   return ref.var->getValue(vl);
}

inline void VarRef::setValue(class QoreNode *val, class ExceptionSink *xsink)
{
   if (type == VT_LOCAL)
      find_lvar(ref.id)->setValue(val, xsink);
   else
      ref.var->setValue(val, xsink);
}

#if 0
static inline class VarRef *getVarRefByName(char *name)
{
   LVar *lvar = findLVar(name);   
   if (lvar)
   {
      VarRef *vr = new VarRef(strdup(name), VT_LOCAL);
      vr->ref.id = lvar->id;
      return vr;
   }

   Var *var = getProgram()->findVar(name);
   if (!var)
      return NULL;
   VarRef *vr = new VarRef(strdup(name), VT_GLOBAL);
   vr->ref.var = var;
   return vr;
}

static inline class LVar *findLVar(char *name)
{
   class LVar *lvar = get_thread_stack();

   while (lvar)
   {
      //printd(5, "findLVar(%s) %08p %s\n", name, lvar->id, lvar->id);
      if (!strcmp(lvar->id, name))
	 break;
      lvar = lvar->next;
   }
   return lvar;
}
#endif

#endif // _QORE_VARIABLE_H
