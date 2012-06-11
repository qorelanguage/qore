/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  LocalVar.h

  Qore Programming Language

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

#ifndef _QORE_LOCALVAR_H

#define _QORE_LOCALVAR_H

#include <qore/intern/qore_thread_intern.h>

#include <qore/intern/QoreValue.h>

template <class T>
class VarStackPointerHelper {
   const T* orig;

public:
   DLLLOCAL VarStackPointerHelper(const T* v) : orig(v) {
      v->skip = true;
   }
   DLLLOCAL ~VarStackPointerHelper() {
      orig->skip = false;
   }
};

template <class T>
class LocalRefHelper : public RuntimeReferenceHelper {
protected:
   // used to skip the var entry in case it's a recursive reference
   VarStackPointerHelper<T> helper;
   bool valid;

public:
   DLLLOCAL LocalRefHelper(const T* val, ReferenceNode& ref, ExceptionSink* xsink) : RuntimeReferenceHelper(ref, xsink), helper(val), valid(!*xsink) {
   }

   DLLLOCAL operator bool() const {
      return valid;
   }
};

template <class T>
class LValueRefHelper : public LocalRefHelper<T> {
protected:
   LValueHelper valp;

public:
   DLLLOCAL LValueRefHelper(T* val, ExceptionSink* xsink) : LocalRefHelper<T>(val, xsink), valp(this->valid ? *((ReferenceNode*)val->v.n) : 0, xsink) {
   }

   DLLLOCAL operator bool() const {
      return valp;
   }

   DLLLOCAL LValueHelper *operator->() {
      return &valp;
   }
};

class VarValueBase {
protected:
   DLLLOCAL int checkFinalized(ExceptionSink* xsink) const {
      if (finalized) {
         xsink->raiseException("DESTRUCTOR-ERROR", "illegal variable assignment after second phase of variable destruction");
         return -1;
      }
      return 0;
   }

public:
   QoreLValueGeneric val;
   const char* id;
   mutable bool skip : 1;
   bool finalized : 1;

   DLLLOCAL VarValueBase(const char* n_id, valtype_t t = QV_Node, bool n_skip = false) : val(t), id(n_id), skip(n_skip), finalized(false) {
   }

   DLLLOCAL VarValueBase(const char* n_id, const QoreTypeInfo* varTypeInfo) : val(varTypeInfo), id(n_id), skip(false), finalized(false) {
   }

   DLLLOCAL VarValueBase() : val(QV_Bool), finalized(false) {
   }

   DLLLOCAL void del(ExceptionSink* xsink) {
      discard(val.remove(true), xsink);
   }

   DLLLOCAL bool isRef() const {
      return val.getType() == NT_REFERENCE;
   }
};

struct SkipHelper {
   VarValueBase *vvb;

   DLLLOCAL SkipHelper(VarValueBase *n_vvb) : vvb(n_vvb) {
      assert(!vvb->skip);
      vvb->skip = true;
   }

   DLLLOCAL ~SkipHelper() {
      vvb->skip = false;
   }
};

class LocalVarValue : public VarValueBase {
public:
   DLLLOCAL void set(const char* n_id, const QoreTypeInfo* typeInfo, QoreValue nval) {
      //printd(5, "LocalVarValue::set() this: %p id: '%s' type: '%s' code: %d\n", this, n_id, typeInfo->getName(), nval.getType());
      skip = false;
      id = n_id;

      if (typeInfo == bigIntTypeInfo || typeInfo == softBigIntTypeInfo)
         val.set(QV_Int);
      else if (typeInfo == floatTypeInfo || typeInfo == softFloatTypeInfo)
         val.set(QV_Float);
      else if (typeInfo == boolTypeInfo || typeInfo == softBoolTypeInfo)
         val.set(QV_Bool);
      else
         val.set(QV_Node);

      // no exception is possible here as there was no previous value
      // also since only basic value types could be returned, no exceptions can occur with the value passed either
      discard(val.assignInitial(nval), 0);
   }

   DLLLOCAL void uninstantiate(ExceptionSink* xsink) {
      del(xsink);
   }

   DLLLOCAL bool optimizedLocal() const {
      return val.optimized();
   }

   DLLLOCAL int getLValue(LValueHelper& lvh, bool for_remove) const;
   DLLLOCAL void remove(LValueRemoveHelper& lvrh, const QoreTypeInfo* typeInfo);

   DLLLOCAL qore_type_t getValueType() const {
      assert(optimizedLocal());

      return val.getType();
   }

   DLLLOCAL const char* getValueTypeName() const {
      assert(optimizedLocal());

      return val.getTypeName();
   }

   DLLLOCAL void finalize(ExceptionSink* xsink) {
      //assert(!finalized);
      if (val.type == QV_Node) {
         discard(val.remove(true), xsink);
         finalized = true;
      }
   }

   DLLLOCAL AbstractQoreNode* eval(ExceptionSink* xsink) {
      if (val.getType() == NT_REFERENCE) {
         ReferenceNode* ref = reinterpret_cast<ReferenceNode*>(val.v.n);
         LocalRefHelper<LocalVarValue> helper(this, *ref, xsink);
         return helper ? lvalue_ref::get(ref)->vexp->eval(xsink) : 0;
      }

      return val.eval();
   }

   DLLLOCAL AbstractQoreNode* eval(bool& needs_deref, ExceptionSink* xsink) {
      if (val.getType() == NT_REFERENCE) {
         needs_deref = true;
         ReferenceNode* ref = reinterpret_cast<ReferenceNode*>(val.v.n);
         LocalRefHelper<LocalVarValue> helper(this, *ref, xsink);
         return helper ? lvalue_ref::get(ref)->vexp->eval(xsink) : 0;
      }
 
      return val.eval(needs_deref);
   }

   DLLLOCAL int64 bigIntEval(ExceptionSink* xsink) {
      if (val.getType() == NT_REFERENCE) {
         ReferenceNode* ref = reinterpret_cast<ReferenceNode*>(val.v.n);
         LocalRefHelper<LocalVarValue> helper(this, *ref, xsink);
         return helper ? lvalue_ref::get(ref)->vexp->bigIntEval(xsink) : 0;
      }

      return val.getAsBigInt();
   }

   DLLLOCAL int intEval(ExceptionSink* xsink) {
      if (val.getType() == NT_REFERENCE) {
         ReferenceNode* ref = reinterpret_cast<ReferenceNode*>(val.v.n);
         LocalRefHelper<LocalVarValue> helper(this, *ref, xsink);
         return helper ? lvalue_ref::get(ref)->vexp->integerEval(xsink) : 0;
      }

      return (int)val.getAsBigInt();
   }

   DLLLOCAL bool boolEval(ExceptionSink* xsink) {
      if (val.getType() == NT_REFERENCE) {
         ReferenceNode* ref = reinterpret_cast<ReferenceNode*>(val.v.n);
         LocalRefHelper<LocalVarValue> helper(this, *ref, xsink);
         return helper ? lvalue_ref::get(ref)->vexp->boolEval(xsink) : 0;
      }

      return val.getAsBool();
   }

   DLLLOCAL double floatEval(ExceptionSink* xsink) {
      if (val.getType() == NT_REFERENCE) {
         ReferenceNode* ref = reinterpret_cast<ReferenceNode*>(val.v.n);
         LocalRefHelper<LocalVarValue> helper(this, *ref, xsink);
         return helper ? lvalue_ref::get(ref)->vexp->floatEval(xsink) : 0;
      }

      return val.getAsFloat();
   }
};

struct ClosureVarValue : public VarValueBase, public QoreReferenceCounter, public QoreThreadLock {
public:
   const QoreTypeInfo* typeInfo; // type restriction for lvalue

   DLLLOCAL ClosureVarValue(const char* n_id, const QoreTypeInfo* varTypeInfo, QoreValue& nval) : VarValueBase(n_id, varTypeInfo), typeInfo(varTypeInfo) {
      //printd(5, "ClosureVarValue::ClosureVarValue() this: %p pgm: %p\n", this, getProgram());
      // also since only basic value types could be returned, no exceptions can occur with the value passed either
      discard(val.assignInitial(nval), 0);
   }

#if 0
   DLLLOCAL ~ClosureVarValue() {
      //printd(5, "ClosureVarValue::~ClosureVarValue() this: %p\n", this);
   }
#endif

   DLLLOCAL void ref() { ROreference(); }

   DLLLOCAL void deref(ExceptionSink* xsink) { if (ROdereference()) { del(xsink); delete this; } }

   DLLLOCAL int getLValue(LValueHelper& lvh, bool for_remove) const;
   DLLLOCAL void remove(LValueRemoveHelper& lvrh);

   DLLLOCAL void finalize(ExceptionSink* xsink) {
      SafeLocker sl(this);
      if (!finalized) {
         AbstractQoreNode* dr = val.remove(true);
         finalized = true;
         sl.unlock();
         discard(dr, xsink);         
      }
   }

   DLLLOCAL AbstractQoreNode* eval(ExceptionSink* xsink) {
      SafeLocker sl(this);
      if (val.getType() == NT_REFERENCE) {
         ReferenceHolder<ReferenceNode> ref(reinterpret_cast<ReferenceNode*>(val.v.n->refSelf()), xsink);
         sl.unlock();
         LocalRefHelper<ClosureVarValue> helper(this, **ref, xsink);
         return helper ? lvalue_ref::get(*ref)->vexp->eval(xsink) : 0;
      }

      return val.eval();
   }

   DLLLOCAL AbstractQoreNode* eval(bool& needs_deref, ExceptionSink* xsink) {
      SafeLocker sl(this);
      if (val.getType() == NT_REFERENCE) {
         ReferenceHolder<ReferenceNode> ref(reinterpret_cast<ReferenceNode*>(val.v.n->refSelf()), xsink);
         sl.unlock();
         LocalRefHelper<ClosureVarValue> helper(this, **ref, xsink);
         return helper ? lvalue_ref::get(*ref)->vexp->eval(needs_deref, xsink) : 0;
      }

      return val.eval(needs_deref);
   }

   DLLLOCAL int64 bigIntEval(ExceptionSink* xsink) {
      SafeLocker sl(this);
      if (val.getType() == NT_REFERENCE) {
         ReferenceHolder<ReferenceNode> ref(reinterpret_cast<ReferenceNode*>(val.v.n->refSelf()), xsink);
         sl.unlock();
         LocalRefHelper<ClosureVarValue> helper(this, **ref, xsink);
         return helper ? lvalue_ref::get(*ref)->vexp->bigIntEval(xsink) : 0;
      }

      return val.getAsBigInt();
   }

   DLLLOCAL int intEval(ExceptionSink* xsink) {
      SafeLocker sl(this);
      if (val.getType() == NT_REFERENCE) {
         ReferenceHolder<ReferenceNode> ref(reinterpret_cast<ReferenceNode*>(val.v.n->refSelf()), xsink);
         sl.unlock();
         LocalRefHelper<ClosureVarValue> helper(this, **ref, xsink);
         return helper ? lvalue_ref::get(*ref)->vexp->integerEval(xsink) : 0;
      }

      return (int)val.getAsBigInt();
   }

   DLLLOCAL bool boolEval(ExceptionSink* xsink) {
      SafeLocker sl(this);
      if (val.getType() == NT_REFERENCE) {
         ReferenceHolder<ReferenceNode> ref(reinterpret_cast<ReferenceNode*>(val.v.n->refSelf()), xsink);
         sl.unlock();
         LocalRefHelper<ClosureVarValue> helper(this, **ref, xsink);
         return helper ? lvalue_ref::get(*ref)->vexp->boolEval(xsink) : 0;
      }

      return val.getAsBool();
   }

   DLLLOCAL double floatEval(ExceptionSink* xsink) {
      SafeLocker sl(this);
      if (val.getType() == NT_REFERENCE) {
         ReferenceHolder<ReferenceNode> ref(reinterpret_cast<ReferenceNode*>(val.v.n->refSelf()), xsink);
         sl.unlock();
         LocalRefHelper<ClosureVarValue> helper(this, **ref, xsink);
         return helper ? lvalue_ref::get(*ref)->vexp->floatEval(xsink) : 0;
      }

      return val.getAsFloat();
   }
};

// now shared between parent and child Program objects for top-level local variables with global scope
class LocalVar {
private:
   std::string name;
   bool closure_use;
   const QoreTypeInfo* typeInfo;

   DLLLOCAL LocalVarValue* get_var() const {
      return thread_find_lvar(name.c_str());
   }

public:
   DLLLOCAL LocalVar(const char* n_name, const QoreTypeInfo* ti) : name(n_name), closure_use(false), typeInfo(ti) {
   }

   DLLLOCAL LocalVar(const LocalVar& old) : name(old.name), closure_use(old.closure_use), typeInfo(old.typeInfo) {
   }

   DLLLOCAL ~LocalVar() {
   }

   DLLLOCAL void instantiate() {
#ifdef QORE_ENFORCE_DEFAULT_LVALUE
      instantiate(typeInfo->getDefaultQoreValue());
#else
      instantiate(QoreValue());
#endif
   }

   DLLLOCAL void instantiate(QoreValue nval) const {
      //printd(5, "LocalVar::instantiate(%p) this=%p '%s' value type=%s closure_use=%s pgm=%p\n", value, this, name.c_str(), get_type_name(value), closure_use ? "true" : "false", getProgram());

      if (!closure_use) {
         LocalVarValue* val = thread_instantiate_lvar();
         val->set(name.c_str(), typeInfo, nval);
      }
      else
         thread_instantiate_closure_var(name.c_str(), typeInfo, nval);
   }

   DLLLOCAL void instantiate_object(QoreObject* value) const {
      //printd(5, "LocalVar::instantiate_object(%p) this=%p '%s'\n", value, this, name.c_str());
      instantiate(value);
      value->ref();
   }

   DLLLOCAL void uninstantiate(ExceptionSink* xsink) const  {
      //printd(5, "LocalVar::uninstantiate() this=%p '%s' closure_use=%s pgm=%p\n", this, name.c_str(), closure_use ? "true" : "false", getProgram());

      if (!closure_use)
         thread_uninstantiate_lvar(xsink);
      else
         thread_uninstantiate_closure_var(xsink);
   }

   DLLLOCAL AbstractQoreNode* eval(ExceptionSink* xsink) const {
      if (!closure_use) {
         LocalVarValue* val = get_var();
         return val->eval(xsink);
      }

      ClosureVarValue* val = thread_find_closure_var(name.c_str());
      return val->eval(xsink);
   }

   DLLLOCAL AbstractQoreNode* eval(bool& needs_deref, ExceptionSink* xsink) const {
      if (!closure_use) {
         LocalVarValue* val = get_var();
         return val->eval(needs_deref, xsink);
      }

      ClosureVarValue* val = thread_find_closure_var(name.c_str());
      return val->eval(needs_deref, xsink);
   }

   DLLLOCAL int64 bigIntEval(ExceptionSink* xsink) const {
      if (!closure_use) {
         LocalVarValue* val = get_var();
         return val->bigIntEval(xsink);
      }

      ClosureVarValue* val = thread_find_closure_var(name.c_str());
      return val->bigIntEval(xsink);
   }

   DLLLOCAL int intEval(ExceptionSink* xsink) const {
      if (!closure_use) {
         LocalVarValue* val = get_var();
         return val->intEval(xsink);
      }

      ClosureVarValue* val = thread_find_closure_var(name.c_str());
      return val->intEval(xsink);
   }

   DLLLOCAL bool boolEval(ExceptionSink* xsink) const {
      if (!closure_use) {
         LocalVarValue* val = get_var();
         return val->boolEval(xsink);
      }

      ClosureVarValue* val = thread_find_closure_var(name.c_str());
      return val->boolEval(xsink);
   }

   DLLLOCAL double floatEval(ExceptionSink* xsink) const {
      if (!closure_use) {
         LocalVarValue* val = get_var();
         return val->floatEval(xsink);
      }

      ClosureVarValue* val = thread_find_closure_var(name.c_str());
      return val->floatEval(xsink);
   }

   DLLLOCAL const char* getName() const {
      return name.c_str();
   }

   DLLLOCAL const std::string& getNameStr() const {
      return name;
   }

   DLLLOCAL void setClosureUse() {
      closure_use = true;
   }

   DLLLOCAL bool closureUse() const { 
      return closure_use;
   }

   DLLLOCAL bool isRef() const {
      return !closure_use ? get_var()->isRef() : thread_find_closure_var(name.c_str())->isRef();
   }

   DLLLOCAL int getLValue(LValueHelper& lvh, bool for_remove) const {
      //printd(5, "LocalVar::getLValue() this: %p '%s' closure_use: %d\n", this, getName(), closure_use);
      if (!closure_use) {
         lvh.setTypeInfo(typeInfo);
         return get_var()->getLValue(lvh, for_remove);
      }

      return thread_find_closure_var(name.c_str())->getLValue(lvh, for_remove);
   }

   DLLLOCAL void remove(LValueRemoveHelper& lvrh) {
      if (!closure_use)
         return get_var()->remove(lvrh, typeInfo);

      return thread_find_closure_var(name.c_str())->remove(lvrh);
   }

   DLLLOCAL const QoreTypeInfo* getTypeInfo() const {
      return typeInfo;
   }

   DLLLOCAL qore_type_t getValueType() const {
      return !closure_use ? get_var()->val.getType() : thread_find_closure_var(name.c_str())->val.getType();
   }

   DLLLOCAL const char* getValueTypeName() const {
      return !closure_use ? get_var()->val.getTypeName() : thread_find_closure_var(name.c_str())->val.getTypeName();
   }
};

typedef LocalVar* lvar_ptr_t;

#endif
