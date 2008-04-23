/*
  LocalVar.h

  Qore Programming Language

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

struct LocalVarValue {
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
	    //printd(5, "LocalVarValue::uninstantiate() this=%08p uninstantiating local variable '%s' val=%08p\n", this, id, val->val.value);
	    discard(val.value, xsink);
	    return;
	 }
	 else {
	    //printd(5, "LocalVarValue::uninstantiate() this=%08p uninstantiating local variable '%s' reference expression vexp=%08p\n", this, id, val->val.ref.vexp);
	    val.ref.vexp->deref(xsink);
	    if (val.ref.obj)
	       val.ref.obj->tDeref();
	 }
      }
};

class VarStackPointerHelper {
      LocalVarValue *orig;

   public:
      DLLLOCAL VarStackPointerHelper(LocalVarValue *v) : orig(v)
      {
	 v->skip = true;
      }
      DLLLOCAL ~VarStackPointerHelper()
      {
	 orig->skip = false;
      }
};

class LocalVar {
   private:
      pthread_key_t var_key;
      std::string name;

      DLLLOCAL LocalVarValue *get_current_var() const
      {
#ifdef HAVE_UNLIMITED_THREAD_KEYS
	 return (LocalVarValue *)pthread_getspecific(var_key);
#else
	 return thread_find_current_lvar(name.c_str());
#endif
      }

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
      DLLLOCAL LocalVar(const char *n_name)
      {
#ifdef HAVE_UNLIMITED_THREAD_KEYS
	 pthread_key_create(&var_key, 0);
#endif
	 name = n_name;
      }

      DLLLOCAL ~LocalVar()
      {
	 //pthread_key_delete(var_key);
      }

      DLLLOCAL void instantiate(AbstractQoreNode *value)
      {
	 //printd(5, "LocalVar::instantiate(%08p) this=%08p '%s'\n", value, this, name.c_str());

	 LocalVarValue *val = thread_instantiate_lvar();
#ifdef HAVE_UNLIMITED_THREAD_KEYS
	 val->prev = get_current_var();
	 pthread_setspecific(var_key, val);
	 val->set(value);
#else
	 val->set(name.c_str(), value);
#endif
      }

      DLLLOCAL void instantiate_object(QoreObject *value)
      {
	 //printd(5, "LocalVar::instantiate_object(%08p) this=%08p '%s'\n", value, this, name.c_str());

	 LocalVarValue *val = thread_instantiate_lvar();
#ifdef HAVE_UNLIMITED_THREAD_KEYS
	 val->prev = get_current_var();
	 pthread_setspecific(var_key, val);
	 val->set(value);
#else
	 val->set(name.c_str(), value);
#endif
	 value->ref();
      }

      DLLLOCAL void instantiate(AbstractQoreNode *vexp, QoreObject *obj)
      {
	 //printd(5, "LocalVar::instantiate(%08p, %08p) this=%08p '%s'\n", vexp, obj, this, name.c_str());

	 LocalVarValue *val = thread_instantiate_lvar();
#ifdef HAVE_UNLIMITED_THREAD_KEYS
	 val->prev = get_current_var();
	 pthread_setspecific(var_key, val);
	 val->set(vexp, obj);
#else
	 val->set(name.c_str(), vexp, obj);
#endif
      }
      
      DLLLOCAL void uninstantiate(ExceptionSink *xsink)
      {
#ifdef HAVE_UNLIMITED_THREAD_KEYS
	 LocalVarValue *val = get_current_var();
	 assert(val);
	 pthread_setspecific(var_key, val->prev);
#endif
	 thread_uninstantiate_lvar(xsink);
      }

      DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, ExceptionSink *xsink) const
      {
	 LocalVarValue *val = get_var();
	 assert(val);

	 if (!val->is_ref)
	    return const_cast<AbstractQoreNode **>(&val->val.value);

	 // skip this entry in case it's a recursive reference
	 VarStackPointerHelper helper(val);

	 if (val->val.ref.obj) {
	    ObjectSubstitutionHelper osh(val->val.ref.obj);
	    return get_var_value_ptr(val->val.ref.vexp, vl, xsink);
	 }

	 return get_var_value_ptr(val->val.ref.vexp, vl, xsink);
      }

      DLLLOCAL AbstractQoreNode *getValue(AutoVLock *vl, ExceptionSink *xsink)
      {
	 LocalVarValue *val = get_var();
	 assert(val);

	 if (!val->is_ref)
	    return const_cast<AbstractQoreNode *>(val->val.value);

	 // skip this entry in case it's a recursive reference
	 VarStackPointerHelper helper(val);

	 if (val->val.ref.obj) {
	    ObjectSubstitutionHelper osh(val->val.ref.obj);
	    return getNoEvalVarValue(val->val.ref.vexp, vl, xsink);
	 }
	 return getNoEvalVarValue(val->val.ref.vexp, vl, xsink);
      }

      // value is already referenced for assignment
      DLLLOCAL void setValue(AbstractQoreNode *value, ExceptionSink *xsink)
      {
	 LocalVarValue *val = get_var();
	 assert(val);

	 if (!val->is_ref) {
	    if (val->val.value)
	       val->val.value->deref(xsink);
	    val->val.value = value;
	    return;
	 }

	 ReferenceHolder<AbstractQoreNode> value_holder(value, xsink);

	 ObjectSubstitutionHelper osh(val->val.ref.obj);
	 AutoVLock vl(xsink);

	 // skip this entry in case it's a recursive reference
	 VarStackPointerHelper helper(val);

	 LValueHelper valp(val->val.ref.vexp, xsink);
	 if (!valp)
	    return;

	 valp.assign(value_holder.release());
      }

      DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink)
      {
	 LocalVarValue *val = get_var();
	 assert(val);

	 if (!val->is_ref)
	    return val->val.value ? val->val.value->refSelf() : 0;

         ObjectSubstitutionHelper osh(val->val.ref.obj);

         // push stack pointer back one in case the expression is a recursive reference
         VarStackPointerHelper helper(val);
         return val->val.ref.vexp->eval(xsink);
      }

      DLLLOCAL AbstractQoreNode *eval(bool &needs_deref, ExceptionSink *xsink)
      {
	 LocalVarValue *val = get_var();
	 assert(val);

	 if (!val->is_ref) {
	    needs_deref = false;
	    return val->val.value;
	 }

	 needs_deref = true;

         ObjectSubstitutionHelper osh(val->val.ref.obj);

         // push stack pointer back one in case the expression is a recursive reference
         VarStackPointerHelper helper(val);
         return val->val.ref.vexp->eval(xsink);
      }

      const char *getName() const
      {
	 return name.c_str();
      }
};

#endif
