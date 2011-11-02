/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  LocalVar.h

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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

#ifndef _QORE_LOCALVAR_H

#define _QORE_LOCALVAR_H

#include <qore/intern/qore_thread_intern.h>

struct lvar_ref {
   AbstractQoreNode *vexp;  // partially evaluated lvalue expression for references
   QoreObject *obj;         // for references to object members
   QoreProgram *pgm;
};

union lvar_u {
   AbstractQoreNode *value;
   lvar_ref ref;
};

class LocalVarValue;

class VarStackPointerHelper {
   LocalVarValue *orig;

public:
   DLLLOCAL VarStackPointerHelper(LocalVarValue *v);
   DLLLOCAL ~VarStackPointerHelper();
};

class VarStackPointerClosureHelper {
   ClosureVarValue *orig;

public:
   DLLLOCAL VarStackPointerClosureHelper(ClosureVarValue *v);
   DLLLOCAL ~VarStackPointerClosureHelper();
};

class VarValueBase {
protected:
   DLLLOCAL int checkFinalized(ExceptionSink *xsink) {
      if (finalized) {
         xsink->raiseException("DESTRUCTOR-ERROR", "illegal variable assignment after second phase of variable destruction");
         return -1;
      }
      return 0;
   }

public:
   union lvar_u val;
   const char *id;
   bool is_ref : 1;
   bool skip : 1;
   bool finalized : 1;

   DLLLOCAL VarValueBase(const char *n_id, bool n_is_ref = false, bool n_skip = false) : id(n_id), is_ref(n_is_ref), skip(n_skip), finalized(false) {
   }
   DLLLOCAL VarValueBase() : finalized(false) {
   }
};

class LocalVarValue : public VarValueBase {
public:
   DLLLOCAL void set(const char *n_id, AbstractQoreNode *value) {
      is_ref = false;
      skip = false;
      id = n_id;
      val.value = value;
   }

   DLLLOCAL void set(const char *n_id, AbstractQoreNode *vexp, QoreObject *obj, QoreProgram *pgm) {
      assert(pgm);
      is_ref = true;
      skip = false;
      id = n_id;
      val.ref.vexp = vexp;
      val.ref.obj = obj;
      val.ref.pgm = pgm;
      if (obj)
         obj->tRef();
   }

   DLLLOCAL void uninstantiate(ExceptionSink *xsink) {
      if (!is_ref) {
         //printd(5, "LocalVarValue::uninstantiate() this=%p uninstantiating local variable '%s' val=%p\n", this, id, val.value);
         discard(val.value, xsink);
         return;
      }

      //printd(5, "LocalVarValue::uninstantiate() this=%p uninstantiating local variable '%s' reference expression vexp=%p\n", this, id, val.ref.vexp);
      val.ref.vexp->deref(xsink);
      if (val.ref.obj)
         val.ref.obj->tDeref();
   }

   DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) {
      if (checkFinalized(xsink))
         return 0;

      if (!is_ref)
         return const_cast<AbstractQoreNode **>(&val.value);

      ProgramContextHelper pch(val.ref.pgm);

      // skip this entry in case it's a recursive reference
      VarStackPointerHelper helper(this);

      if (val.ref.obj) {
         ObjectSubstitutionHelper osh(val.ref.obj);
         return get_var_value_ptr(val.ref.vexp, vl, typeInfo, omap, xsink);
      }

      return get_var_value_ptr(val.ref.vexp, vl, typeInfo, omap, xsink);
   }

   DLLLOCAL void finalize(ExceptionSink *xsink) {
      assert(!finalized);
      if (!is_ref) {
         if (val.value) {
            discard(val.value, xsink);
            val.value = 0;
         }
         finalized = true;
      }
   }

   // value is already referenced for assignment
   DLLLOCAL void setValue(AbstractQoreNode *value, ExceptionSink *xsink) {
      ReferenceHolder<AbstractQoreNode> value_holder(value, xsink);

      if (!is_ref) {
         if (checkFinalized(xsink))
            return;

         if (val.value)
            val.value->deref(xsink);
         val.value = value_holder.release();
         return;
      }

      ObjectSubstitutionHelper osh(val.ref.obj);

      ProgramContextHelper pch(val.ref.pgm);

      AutoVLock vl(xsink);

      // skip this entry in case it's a recursive reference
      VarStackPointerHelper helper(this);

      LValueHelper valp(val.ref.vexp, xsink);
      if (!valp)
         return;

      valp.assign(value_holder.release());
   }

   DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink) {
      if (!is_ref)
         return val.value ? val.value->refSelf() : 0;

      ObjectSubstitutionHelper osh(val.ref.obj);

      //printd(5, "LocalVarValue::eval() this=%p (%s) current pgm=%p new pgm=%p\n", this, id, getProgram(), val.ref.pgm);
      ProgramContextHelper pch(val.ref.pgm);

      // push stack pointer back one in case the expression is a recursive reference
      VarStackPointerHelper helper(this);
      return val.ref.vexp->eval(xsink);
   }

   DLLLOCAL AbstractQoreNode *eval(bool &needs_deref, ExceptionSink *xsink) {
      if (!is_ref) {
         needs_deref = false;
         return val.value;
      }

      needs_deref = true;

      ObjectSubstitutionHelper osh(val.ref.obj);

      ProgramContextHelper pch(val.ref.pgm);

      // push stack pointer back one in case the expression is a recursive reference
      VarStackPointerHelper helper(this);
      return val.ref.vexp->eval(xsink);
   }
};

/* NOTE: the proper threading behavior of this class depends on the fact that the
   type (reference or value) can never change.  also the reference expression
   and object will also not change.
   only the value operations are locked.
*/
struct ClosureVarValue : public VarValueBase, public QoreReferenceCounter, public QoreThreadLock {
private:
   DLLLOCAL void del(ExceptionSink *xsink) {
      if (!is_ref) {
         //printd(5, "ClosureVarValue::del() this=%p uninstantiating closure variable '%s' val=%p\n", this, id, val->val.value);
         discard(val.value, xsink);
         return;
      }
      else {
         //printd(5, "ClosureVarValue::del() this=%p uninstantiating closure variable '%s' reference expression vexp=%p\n", this, id, val->val.ref.vexp);
         val.ref.vexp->deref(xsink);
         if (val.ref.obj)
            val.ref.obj->tDeref();
      }
   }

   ///DLLLOCAL VarValueBase(const char *n_id, bool n_is_ref, bool n_skip) : id(n_id), is_ref(n_is_ref), skip(n_skip), finalized(false) {

public:
   DLLLOCAL ClosureVarValue(const char *n_id, AbstractQoreNode *value) : VarValueBase(n_id) {
      val.value = value;
   }

   DLLLOCAL ClosureVarValue(const char *n_id, AbstractQoreNode *vexp, QoreObject *obj, QoreProgram *pgm) : VarValueBase(n_id, true) {
      val.ref.vexp = vexp;
      val.ref.obj = obj;
      val.ref.pgm = pgm;
      if (obj)
	 obj->tRef();
   }

   DLLLOCAL void ref() { ROreference(); }

   DLLLOCAL void deref(ExceptionSink *xsink) { if (ROdereference()) { del(xsink); delete this; } }

   DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) {
      if (!is_ref) {
         lock();

         if (checkFinalized(xsink))
            return 0;

         vl->set(this);
         return const_cast<AbstractQoreNode **>(&val.value);
      }

      ProgramContextHelper pch(val.ref.pgm);

      // skip this entry in case it's a recursive reference
      VarStackPointerClosureHelper helper(this);

      if (val.ref.obj) {
         ObjectSubstitutionHelper osh(val.ref.obj);
         return get_var_value_ptr(val.ref.vexp, vl, typeInfo, omap, xsink);
      }

      return get_var_value_ptr(val.ref.vexp, vl, typeInfo, omap, xsink);
   }

   DLLLOCAL void finalize(ExceptionSink *xsink) {
      SafeLocker sl(this);
      if (!finalized && !is_ref) {
         AbstractQoreNode *dr = val.value;
         val.value = 0;
         finalized = true;
         sl.unlock();
         discard(dr, xsink);         
      }
   }

   // value is already referenced for assignment
   DLLLOCAL void setValue(AbstractQoreNode *value, ExceptionSink *xsink) {
      ReferenceHolder<AbstractQoreNode> value_holder(value, xsink);

      if (!is_ref) {
         // make sure and dereference the old value outside the lock
         AbstractQoreNode *dr = 0;
         {
            AutoLocker al(this);

            if (checkFinalized(xsink))
               return;

            if (val.value)
               dr = val.value;
            val.value = value_holder.release();
         }
         discard(dr, xsink);
         return;
      }

      ObjectSubstitutionHelper osh(val.ref.obj);
      ProgramContextHelper pch(val.ref.pgm);

      // skip this entry in case it's a recursive reference
      VarStackPointerClosureHelper helper(this);

      LValueHelper valp(val.ref.vexp, xsink);
      if (!valp)
         return;

      valp.assign(value_holder.release());
   }

   DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink) {
      if (!is_ref) {
         AutoLocker al(this);
         return val.value ? val.value->refSelf() : 0;
      }

      ObjectSubstitutionHelper osh(val.ref.obj);
 
      ProgramContextHelper pch(val.ref.pgm);

      // push stack pointer back one in case the expression is a recursive reference
      VarStackPointerClosureHelper helper(this);
      return val.ref.vexp->eval(xsink);
   }

   DLLLOCAL AbstractQoreNode *eval(bool &needs_deref, ExceptionSink *xsink) {
      needs_deref = true;
      return eval(xsink);
   }
};

// now shared between parent and child Program objects for top-level local variables with global scope
class LocalVar {
private:
   std::string name;
   bool closure_use, needs_value_instantiation;
   const QoreTypeInfo *typeInfo;

   DLLLOCAL LocalVarValue *get_var() const {
      return thread_find_lvar(name.c_str());
   }

public:
   DLLLOCAL LocalVar(const char *n_name, const QoreTypeInfo *ti) : name(n_name), closure_use(false), needs_value_instantiation(ti->hasType() ? true : false), typeInfo(ti) {
   }

   DLLLOCAL LocalVar(const LocalVar &old) : name(old.name), closure_use(old.closure_use), needs_value_instantiation(old.needs_value_instantiation), typeInfo(old.typeInfo) {
   }

   DLLLOCAL ~LocalVar() {
   }

   DLLLOCAL void instantiate() const {
      //printd(5, "LocalVar::instantiate(%p) this=%p '%s'\n", value, this, name.c_str());
      //instantiate(typeInfo->getDefaultValue());
      instantiate(0);
   }

   DLLLOCAL void instantiate(AbstractQoreNode *value) const {
      //printd(5, "LocalVar::instantiate(%p) this=%p '%s' value type=%s closure_use=%s pgm=%p\n", value, this, name.c_str(), get_type_name(value), closure_use ? "true" : "false", getProgram());

      if (!closure_use) {
         LocalVarValue *val = thread_instantiate_lvar();
         val->set(name.c_str(), value);
      }
      else
         thread_instantiate_closure_var(name.c_str(), value);
   }

   DLLLOCAL void instantiate_object(QoreObject *value) const {
      //printd(5, "LocalVar::instantiate_object(%p) this=%p '%s'\n", value, this, name.c_str());
      instantiate(value);
      value->ref();
   }

   DLLLOCAL void instantiate(AbstractQoreNode *vexp, QoreObject *obj, QoreProgram *pgm) {
      //printd(5, "LocalVar::instantiate(%p, %p) this=%p '%s'\n", vexp, obj, this, name.c_str());

      if (!closure_use) {
         LocalVarValue *val = thread_instantiate_lvar();
         val->set(name.c_str(), vexp, obj, pgm);
      }
      else
         thread_instantiate_closure_var(name.c_str(), vexp, obj, pgm);
   }

   DLLLOCAL void uninstantiate(ExceptionSink *xsink) const  {
      //printd(5, "LocalVar::uninstantiate() this=%p '%s' closure_use=%s pgm=%p\n", this, name.c_str(), closure_use ? "true" : "false", getProgram());

      if (!closure_use) {
         thread_uninstantiate_lvar(xsink);
      }
      else
         thread_uninstantiate_closure_var(xsink);
   }

   DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, const QoreTypeInfo *&n_typeInfo, ObjMap &omap, ExceptionSink *xsink) const {
      // typeInfo is set here, but in case the variable is a reference, the actual
      // typeInfo structure will be set from the referenced value
      n_typeInfo = typeInfo;
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->getValuePtr(vl, n_typeInfo, omap, xsink);
      }
      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->getValuePtr(vl, n_typeInfo, omap, xsink);
   }

   // value is already referenced for assignment
   DLLLOCAL void setValue(AbstractQoreNode *value, ExceptionSink *xsink) const {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         val->setValue(value, xsink);
         return;
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      val->setValue(value, xsink);
   }

   DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink) const {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->eval(xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->eval(xsink);
   }

   DLLLOCAL AbstractQoreNode *eval(bool &needs_deref, ExceptionSink *xsink) const {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->eval(needs_deref, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->eval(needs_deref, xsink);
   }

/*
   DLLLOCAL int64 bigIntEval(ExceptionSink *xsink) const {
      xxx
   }
*/
   DLLLOCAL const char *getName() const {
      return name.c_str();
   }

   DLLLOCAL const std::string &getNameStr() const {
      return name;
   }

   DLLLOCAL void setClosureUse() {
      closure_use = true;
   }

   DLLLOCAL bool closureUse() const { 
      return closure_use;
   }

   DLLLOCAL const QoreTypeInfo *getTypeInfo() const {
      return typeInfo;
   }

   DLLLOCAL bool needsValueInstantiation() const {
      return needs_value_instantiation;
   }

   DLLLOCAL void unsetNeedsValueInstantiation() {
      if (needs_value_instantiation)
         needs_value_instantiation = false;
   }

   DLLLOCAL bool needsAssignmentAtInstantiation() const {
      return needs_value_instantiation && !typeInfo->hasDefaultValue() ? true : false;
   }
};

typedef LocalVar *lvar_ptr_t;

#endif
