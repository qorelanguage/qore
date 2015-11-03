/*
  LocalVar.h

  Qore Programming Language

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

#ifndef _QORE_LOCALVAR_H

#define _QORE_LOCALVAR_H

#include <qore/intern/qore_thread_intern.h>

struct lvar_ref {
      AbstractQoreNode *vexp;  // partially evaluated lvalue expression for references
      QoreObject *obj;         // for references to object members
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

class LocalVarValue {
   public:
      union lvar_u val;
#ifdef HAVE_UNLIMITED_THREAD_KEYS
      LocalVarValue *prev;
#else
      const char *id;
#endif
      bool is_ref : 1;
      bool skip : 1;

#ifdef HAVE_UNLIMITED_THREAD_KEYS
      DLLLOCAL void set(AbstractQoreNode *value)
      {
	 is_ref = false;
	 skip = false;
	 val.value = value;
      }
#else
      DLLLOCAL void set(const char *n_id, AbstractQoreNode *value)
      {
	 is_ref = false;
	 skip = false;
	 id = n_id;
	 val.value = value;
      }
#endif

#ifdef HAVE_UNLIMITED_THREAD_KEYS
      DLLLOCAL void set(AbstractQoreNode *vexp, QoreObject *obj)
      {
	 is_ref = true;
	 skip = false;
	 val.ref.vexp = vexp;
	 val.ref.obj = obj;
	 if (obj)
	    obj->tRef();
      }
#else
      DLLLOCAL void set(const char *n_id, AbstractQoreNode *vexp, QoreObject *obj)
      {
	 is_ref = true;
	 skip = false;
	 id = n_id;
	 val.ref.vexp = vexp;
	 val.ref.obj = obj;
	 if (obj)
	    obj->tRef();
      }
#endif

      DLLLOCAL void uninstantiate(ExceptionSink *xsink)
      {
	 if (!is_ref) {
	    //printd(5, "LocalVarValue::uninstantiate() this=%08p uninstantiating local variable '%s' val=%08p\n", this, id, val.value);
	    discard(val.value, xsink);
	    return;
	 }

	 //printd(5, "LocalVarValue::uninstantiate() this=%08p uninstantiating local variable '%s' reference expression vexp=%08p\n", this, id, val.ref.vexp);
	 val.ref.vexp->deref(xsink);
	 if (val.ref.obj)
	    val.ref.obj->tDeref();
      }

      DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, ExceptionSink *xsink)
      {
	 if (!is_ref)
	    return const_cast<AbstractQoreNode **>(&val.value);

	 // skip this entry in case it's a recursive reference
	 VarStackPointerHelper helper(this);

	 if (val.ref.obj) {
	    ObjectSubstitutionHelper osh(val.ref.obj);
	    return get_var_value_ptr(val.ref.vexp, vl, xsink);
	 }

	 return get_var_value_ptr(val.ref.vexp, vl, xsink);
      }

      DLLLOCAL AbstractQoreNode *getValue(AutoVLock *vl, ExceptionSink *xsink)
      {
	 if (!is_ref)
	    return const_cast<AbstractQoreNode *>(val.value);

	 // skip this entry in case it's a recursive reference
	 VarStackPointerHelper helper(this);

	 if (val.ref.obj) {
	    ObjectSubstitutionHelper osh(val.ref.obj);
	    return getNoEvalVarValue(val.ref.vexp, vl, xsink);
	 }
	 return getNoEvalVarValue(val.ref.vexp, vl, xsink);
      }

      // value is already referenced for assignment
      DLLLOCAL void setValue(AbstractQoreNode *value, ExceptionSink *xsink)
      {
	 if (!is_ref) {
	    if (val.value)
	       val.value->deref(xsink);
	    val.value = value;
	    return;
	 }

	 ReferenceHolder<AbstractQoreNode> value_holder(value, xsink);

	 ObjectSubstitutionHelper osh(val.ref.obj);
	 AutoVLock vl(xsink);

	 // skip this entry in case it's a recursive reference
	 VarStackPointerHelper helper(this);

	 LValueHelper valp(val.ref.vexp, xsink);
	 if (!valp)
	    return;

	 valp.assign(value_holder.release());
      }

      DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink)
      {
	 if (!is_ref)
	    return val.value ? val.value->refSelf() : 0;

         ObjectSubstitutionHelper osh(val.ref.obj);

         // push stack pointer back one in case the expression is a recursive reference
         VarStackPointerHelper helper(this);
         return val.ref.vexp->eval(xsink);
      }

      DLLLOCAL AbstractQoreNode *eval(bool &needs_deref, ExceptionSink *xsink)
      {
	 if (!is_ref) {
	    needs_deref = false;
	    return val.value;
	 }

	 needs_deref = true;

         ObjectSubstitutionHelper osh(val.ref.obj);

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
struct ClosureVarValue : public QoreReferenceCounter, public QoreThreadLock
{
   private:
      DLLLOCAL void del(ExceptionSink *xsink)
      {
	 if (!is_ref) {
	    //printd(5, "ClosureVarValue::del() this=%08p uninstantiating closure variable '%s' val=%08p\n", this, id, val->val.value);
	    discard(val.value, xsink);
	    return;
	 }
	 else {
	    //printd(5, "ClosureVarValue::del() this=%08p uninstantiating closure variable '%s' reference expression vexp=%08p\n", this, id, val->val.ref.vexp);
	    val.ref.vexp->deref(xsink);
	    if (val.ref.obj)
	       val.ref.obj->tDeref();
	 }
      }

   public:
      union lvar_u val;
      const char *id;
      bool is_ref : 1;
      bool skip : 1;

      DLLLOCAL ClosureVarValue(const char *n_id, AbstractQoreNode *value)
      {
	 is_ref = false;
	 skip = false;
	 id = n_id;
	 val.value = value;
      }

      DLLLOCAL ClosureVarValue(const char *n_id, AbstractQoreNode *vexp, QoreObject *obj)
      {
	 is_ref = true;
	 skip = false;
	 id = n_id;
	 val.ref.vexp = vexp;
	 val.ref.obj = obj;
	 if (obj)
	    obj->tRef();
      }

      DLLLOCAL void ref() { ROreference(); }

      DLLLOCAL void deref(ExceptionSink *xsink) { if (ROdereference()) { del(xsink); delete this; } }

      DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, ExceptionSink *xsink)
      {
	 if (!is_ref) {
	    lock();
	    vl->set(this);
	    return const_cast<AbstractQoreNode **>(&val.value);
	 }

	 // skip this entry in case it's a recursive reference
	 VarStackPointerClosureHelper helper(this);

	 if (val.ref.obj) {
	    ObjectSubstitutionHelper osh(val.ref.obj);
	    return get_var_value_ptr(val.ref.vexp, vl, xsink);
	 }

	 return get_var_value_ptr(val.ref.vexp, vl, xsink);
      }

      DLLLOCAL AbstractQoreNode *getValue(AutoVLock *vl, ExceptionSink *xsink)
      {
	 if (!is_ref) {
	    lock();
	    vl->set(this);
	    return const_cast<AbstractQoreNode *>(val.value);
	 }

	 // skip this entry in case it's a recursive reference
	 VarStackPointerClosureHelper helper(this);

	 if (val.ref.obj) {
	    ObjectSubstitutionHelper osh(val.ref.obj);
	    return getNoEvalVarValue(val.ref.vexp, vl, xsink);
	 }
	 return getNoEvalVarValue(val.ref.vexp, vl, xsink);
      }

      // value is already referenced for assignment
      DLLLOCAL void setValue(AbstractQoreNode *value, ExceptionSink *xsink)
      {
	 if (!is_ref) {
	    AutoLocker al(this);
	    if (val.value)
	       val.value->deref(xsink);
	    val.value = value;
	    return;
	 }

	 ReferenceHolder<AbstractQoreNode> value_holder(value, xsink);

	 ObjectSubstitutionHelper osh(val.ref.obj);
	 AutoVLock vl(xsink);

	 // skip this entry in case it's a recursive reference
	 VarStackPointerClosureHelper helper(this);

	 LValueHelper valp(val.ref.vexp, xsink);
	 if (!valp)
	    return;

	 valp.assign(value_holder.release());
      }

      DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink)
      {
	 if (!is_ref) {
	    AutoLocker al(this);
	    return val.value ? val.value->refSelf() : 0;
	 }

         ObjectSubstitutionHelper osh(val.ref.obj);

         // push stack pointer back one in case the expression is a recursive reference
         VarStackPointerClosureHelper helper(this);
         return val.ref.vexp->eval(xsink);
      }

      DLLLOCAL AbstractQoreNode *eval(bool &needs_deref, ExceptionSink *xsink)
      {
	 needs_deref = true;
	 return eval(xsink);
      }
};

class LocalVar {
   private:
#ifdef HAVE_UNLIMITED_THREAD_KEYS
      QoreThreadLocalStorage<LocalVarValue> var_key;
#endif
      std::string name;
      bool closure_use;

#ifdef HAVE_UNLIMITED_THREAD_KEYS
      DLLLOCAL LocalVarValue *get_current_var() const
      {
	 return var_key.get();
      }
#endif

      DLLLOCAL LocalVarValue *get_var() const
      {
#ifdef HAVE_UNLIMITED_THREAD_KEYS
	 LocalVarValue *v = get_current_var();
	 while (v->skip)
	    v = v->prev;
	 return v;
#else
	 return thread_find_lvar(name.c_str());
#endif
      }

   public:
      DLLLOCAL LocalVar(const char *n_name) : name(n_name), closure_use(false)
      {
      }

      DLLLOCAL ~LocalVar()
      {
      }

      DLLLOCAL void instantiate(AbstractQoreNode *value) const {
	 //printd(5, "LocalVar::instantiate(%08p) this=%08p '%s'\n", value, this, name.c_str());

	 if (!closure_use) {
	    LocalVarValue *val = thread_instantiate_lvar();
#ifdef HAVE_UNLIMITED_THREAD_KEYS
	    val->prev = get_current_var();
	    var_key.set(val);
	    val->set(value);
#else
	    val->set(name.c_str(), value);
#endif
	    return;
	 }
	 thread_instantiate_closure_var(name.c_str(), value);
      }

      DLLLOCAL void instantiate_object(QoreObject *value) const {
	 //printd(5, "LocalVar::instantiate_object(%08p) this=%08p '%s'\n", value, this, name.c_str());

	 if (!closure_use) {
	    LocalVarValue *val = thread_instantiate_lvar();
#ifdef HAVE_UNLIMITED_THREAD_KEYS
	    val->prev = get_current_var();
	    var_key.set(val);
	    val->set(value);
#else
	    val->set(name.c_str(), value);
#endif
	 }
	 else
	    thread_instantiate_closure_var(name.c_str(), value);

	 value->ref();
      }

      DLLLOCAL void instantiate(AbstractQoreNode *vexp, QoreObject *obj) {
	 //printd(5, "LocalVar::instantiate(%08p, %08p) this=%08p '%s'\n", vexp, obj, this, name.c_str());

	 if (!closure_use) {
	    LocalVarValue *val = thread_instantiate_lvar();
#ifdef HAVE_UNLIMITED_THREAD_KEYS
	    val->prev = get_current_var();
	    var_key.set(val);
	    val->set(vexp, obj);
#else
	    val->set(name.c_str(), vexp, obj);
#endif
	    return;
	 }
	 thread_instantiate_closure_var(name.c_str(), vexp, obj);
      }
      
      DLLLOCAL void uninstantiate(ExceptionSink *xsink) const  {
	 if (!closure_use) {
#ifdef HAVE_UNLIMITED_THREAD_KEYS
	    LocalVarValue *val = get_current_var();
	    var_key.set(val->prev);
#endif
	    thread_uninstantiate_lvar(xsink);
	    return;
	 }
	 thread_uninstantiate_closure_var(xsink);
      }

      DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, ExceptionSink *xsink) const {
	 if (!closure_use) {
	    LocalVarValue *val = get_var();
	    return val->getValuePtr(vl, xsink);
	 }
	 ClosureVarValue *val = thread_find_closure_var(name.c_str());
	 return val->getValuePtr(vl, xsink);
      }

      DLLLOCAL AbstractQoreNode *getValue(AutoVLock *vl, ExceptionSink *xsink) const {
	 if (!closure_use) {
	    LocalVarValue *val = get_var();
	    return val->getValue(vl, xsink);
	 }

	 ClosureVarValue *val = thread_find_closure_var(name.c_str());
	 return val->getValue(vl, xsink);
      }

      // value is already referenced for assignment
      DLLLOCAL void setValue(AbstractQoreNode *value, ExceptionSink *xsink) const {
	 if (!closure_use) {
	    LocalVarValue *val = get_var();
	    val->setValue(value, xsink);
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

      DLLLOCAL AbstractQoreNode *eval(bool &needs_deref, ExceptionSink *xsink) const  {
	 if (!closure_use) {
	    LocalVarValue *val = get_var();
	    return val->eval(needs_deref, xsink);
	 }

	 ClosureVarValue *val = thread_find_closure_var(name.c_str());
	 return val->eval(needs_deref, xsink);
      }

      DLLLOCAL const char *getName() const
      {
	 return name.c_str();
      }

      DLLLOCAL void setClosureUse()
      {
	 closure_use = true;
      }

      DLLLOCAL bool closureUse() const
      { 
	 return closure_use;
      }
};


#endif
