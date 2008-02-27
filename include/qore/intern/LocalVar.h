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
   public:
      bool is_ref : 1;
      bool skip : 1;
      union lvar_u val;
      LocalVarValue *prev;  // link to previous instantiation of this local variable

      DLLLOCAL void set(AbstractQoreNode *value)
      {
	 is_ref = false;
	 skip = false;
	 val.value = value;
      }

      DLLLOCAL void set(AbstractQoreNode *vexp, QoreObject *obj)
      {
	 is_ref = true;
	 skip = false;
	 val.ref.vexp = vexp;
	 val.ref.obj = obj;
	 if (obj)
	    obj->tRef();
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
	 return (LocalVarValue *)pthread_getspecific(var_key);
      }

      DLLLOCAL LocalVarValue *get_var() const
      {
	 LocalVarValue *v = get_current_var();
	 while (v->skip)
	    v = v->prev;
	 return v;
      }

   public:
      DLLLOCAL LocalVar(const char *n_name)
      {
	 pthread_key_create(&var_key, 0);
	 name = n_name;
      }

      DLLLOCAL ~LocalVar()
      {
	 pthread_key_delete(var_key);
      }

      DLLLOCAL void instantiate(AbstractQoreNode *value)
      {
	 LocalVarValue *val = thread_instantiate_lvar();
	 val->prev = get_current_var();
	 pthread_setspecific(var_key, val);

	 //printd(5, "LocalVar::instantiate(%08p) this=%08p '%s'\n", value, this, name.c_str());
	 val->set(value);
      }

      DLLLOCAL void instantiate_object(QoreObject *value)
      {
	 LocalVarValue *val = thread_instantiate_lvar();
	 val->prev = get_current_var();
	 pthread_setspecific(var_key, val);

	 //printd(5, "LocalVar::instantiate_object(%08p) this=%08p '%s'\n", value, this, name.c_str());
	 value->ref();
	 val->set(value);
      }

      DLLLOCAL void instantiate(AbstractQoreNode *vexp, QoreObject *obj)
      {
	 LocalVarValue *val = thread_instantiate_lvar();
	 val->prev = get_current_var();
	 pthread_setspecific(var_key, val);

	 //printd(5, "LocalVar::instantiate(%08p, %08p) this=%08p '%s'\n", vexp, obj, this, name.c_str());
	 val->set(vexp, obj);
      }
      
      DLLLOCAL void uninstantiate(ExceptionSink *xsink)
      {
	 LocalVarValue *val = get_current_var();
	 assert(val);
	 pthread_setspecific(var_key, val->prev);
	 thread_uninstantiate_lvar();

	 //printd(5, "LocalVar::uninstantiate() this=%08p stack=%08p uninstantiating local variable '%s' (type %d)\n", this, stack, name.c_str(), val->type);
	 
	 // if there is a reference expression, decrement the reference counter
	 if (!val->is_ref) {
	    //printd(5, "LocalVar::uninstantiate() this=%08p uninstantiating local variable '%s' val=%08p\n", this, name.c_str(), val->val.value);
	    discard(val->val.value, xsink);
	    return;
	 }
	 else {
	    //printd(5, "LocalVar::uninstantiate() this=%08p uninstantiating local variable '%s' reference expression vexp=%08p\n", this, name.c_str(), val->val.ref.vexp);
	    val->val.ref.vexp->deref(xsink);
	    if (val->val.ref.obj)
	       val->val.ref.obj->tDeref();
	 }
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

	 ObjectSubstitutionHelper osh(val->val.ref.obj);
	 AutoVLock vl;

	 AbstractQoreNode **valp;
	 {
	    // skip this entry in case it's a recursive reference
	    VarStackPointerHelper helper(val);

	    valp = get_var_value_ptr(val->val.ref.vexp, &vl, xsink);
	 }

	 if (!*xsink) {
	    discard(*valp, xsink);
	    *valp = value;
	    vl.del();
	 }
	 else {
	    vl.del();
	    discard(value, xsink);
	 }
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
