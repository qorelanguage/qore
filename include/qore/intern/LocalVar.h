/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  LocalVar.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_LOCALVAR_H

#define _QORE_LOCALVAR_H

#include "qore/intern/qore_thread_intern.h"
#include "qore/intern/QoreLValue.h"
#include "qore/intern/RSection.h"
#include "qore/intern/RSet.h"

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
   LValueHelper* valp;

public:
   DLLLOCAL LValueRefHelper(T* val, ExceptionSink* xsink) : LocalRefHelper<T>(val, xsink), valp(this->valid ? new LValueHelper(*((ReferenceNode*)val->v.n), xsink) : nullptr) {
   }

   DLLLOCAL ~LValueRefHelper() {
      delete valp;
   }

   DLLLOCAL operator bool() const {
      return valp;
   }

   DLLLOCAL LValueHelper* operator->() {
      return valp;
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
      discard(val.removeNode(true), xsink);
   }

   DLLLOCAL bool isRef() const {
      return val.getType() == NT_REFERENCE;
   }

   DLLLOCAL AbstractQoreNode* finalize() {
      if (finalized)
         return 0;

      finalized = true;

      return val.removeNode(true);
   }
};

struct SkipHelper {
   VarValueBase* vvb;

   DLLLOCAL SkipHelper(VarValueBase* n_vvb) : vvb(n_vvb) {
      assert(!vvb->skip);
      vvb->skip = true;
   }

   DLLLOCAL ~SkipHelper() {
      vvb->skip = false;
   }
};

class LocalVarValue : public VarValueBase {
public:
   DLLLOCAL void set(const char* n_id, const QoreTypeInfo* typeInfo, QoreValue nval, bool static_assignment = false) {
      //printd(5, "LocalVarValue::set() this: %p id: '%s' type: '%s' code: %d static_assignment: %d\n", this, n_id, typeInfo->getName(), nval.getType(), static_assignment);
      assert(!finalized);
      skip = false;
      id = n_id;

      // try to set an optimized value type for the value holder if possible
      val.set(typeInfo);

      // no exception is possible here as there was no previous value
      // also since only basic value types could be returned, no exceptions can occur with the value passed either
      discard(val.assignAssumeInitial(nval, static_assignment), 0);
   }

   DLLLOCAL void uninstantiate(ExceptionSink* xsink) {
      del(xsink);
   }

   DLLLOCAL void uninstantiateSelf() {
      val.unassignIgnore();
   }

   DLLLOCAL int getLValue(LValueHelper& lvh, bool for_remove) const;
   DLLLOCAL void remove(LValueRemoveHelper& lvrh, const QoreTypeInfo* typeInfo);

   DLLLOCAL QoreValue evalValue(bool& needs_deref, ExceptionSink* xsink) const {
      if (val.getType() == NT_REFERENCE) {
         ReferenceNode* ref = reinterpret_cast<ReferenceNode*>(val.v.n);
         LocalRefHelper<LocalVarValue> helper(this, *ref, xsink);
         if (!helper)
            return QoreValue();

         ValueEvalRefHolder erh(lvalue_ref::get(ref)->vexp, xsink);
         return erh.takeValue(needs_deref);
      }

      return val.getReferencedValue(needs_deref);
   }
};

struct ClosureVarValue : public VarValueBase, public RObject {
public:
   // reference count; access serialized with rlck from RObject
   mutable int references;
   const QoreTypeInfo* typeInfo; // type restriction for lvalue

   DLLLOCAL ClosureVarValue(const char* n_id, const QoreTypeInfo* varTypeInfo, QoreValue& nval) : VarValueBase(n_id, varTypeInfo), RObject(references), references(1), typeInfo(varTypeInfo) {
      //printd(5, "ClosureVarValue::ClosureVarValue() this: %p refs: 0 -> 1 val: %s\n", this, val.getTypeName());
      val.setClosure();

      // try to set an optimized value type for the value holder if possible
      val.set(varTypeInfo);

      //printd(5, "ClosureVarValue::ClosureVarValue() this: %p pgm: %p val: %s\n", this, getProgram(), nval.getTypeName());
      // also since only basic value types could be returned, no exceptions can occur with the value passed either
      discard(val.assignAssumeInitial(nval), 0);
   }

   DLLLOCAL virtual ~ClosureVarValue() {
      //printd(5, "ClosureVarValue::~ClosureVarValue() this: %p\n", this);
   }

   DLLLOCAL void ref() const;

   DLLLOCAL void deref(ExceptionSink* xsink);

   DLLLOCAL const void* getLValueId() const;

   // returns true if the value could contain an object or a closure
   DLLLOCAL virtual bool needsScan(bool scan_now) {
      return typeInfo->needsScan();
   }

   DLLLOCAL virtual bool scanMembers(RSetHelper& rsh);

   DLLLOCAL int getLValue(LValueHelper& lvh, bool for_remove) const;
   DLLLOCAL void remove(LValueRemoveHelper& lvrh);

   DLLLOCAL ClosureVarValue* refSelf() const {
      ref();
      return const_cast<ClosureVarValue*>(this);
   }

   // sets the current variable to finalized, sets the value to 0, and returns the value held (for dereferencing outside the lock)
   DLLLOCAL AbstractQoreNode* finalize() {
      QoreSafeVarRWWriteLocker sl(rml);
      return VarValueBase::finalize();
   }

   DLLLOCAL QoreValue evalValue(bool& needs_deref, ExceptionSink* xsink) {
      QoreSafeVarRWReadLocker sl(rml);
      if (val.getType() == NT_REFERENCE) {
         ReferenceHolder<ReferenceNode> ref(reinterpret_cast<ReferenceNode*>(val.v.n->refSelf()), xsink);
         sl.unlock();
         LocalRefHelper<ClosureVarValue> helper(this, **ref, xsink);
         return helper ? lvalue_ref::get(*ref)->vexp->eval(needs_deref, xsink) : QoreValue();
      }

      return val.getReferencedValue();
   }

   // deletes the object itself
   DLLLOCAL virtual void deleteObject() {
      delete this;
   }

   // returns the name of the object
   DLLLOCAL virtual const char* getName() const {
      return id;
   }
};

// now shared between parent and child Program objects for top-level local variables with global scope
class LocalVar {
private:
   std::string name;
   bool closure_use, parse_assigned;
   const QoreTypeInfo* typeInfo;

   DLLLOCAL LocalVarValue* get_var() const {
      return thread_find_lvar(name.c_str());
   }

public:
   DLLLOCAL LocalVar(const char* n_name, const QoreTypeInfo* ti) : name(n_name), closure_use(false), parse_assigned(false), typeInfo(ti) {
   }

   DLLLOCAL LocalVar(const LocalVar& old) : name(old.name), closure_use(old.closure_use), parse_assigned(old.parse_assigned), typeInfo(old.typeInfo) {
   }

   DLLLOCAL ~LocalVar() {
   }

   DLLLOCAL void parseAssigned() {
      if (!parse_assigned)
         parse_assigned = true;
   }

   DLLLOCAL void parseUnassigned() {
      if (parse_assigned)
         parse_assigned = false;
   }

   DLLLOCAL void instantiate() {
#ifdef QORE_ENFORCE_DEFAULT_LVALUE
      instantiate(typeInfo->getDefaultQoreValue());
#else
      instantiate(QoreValue());
#endif
   }

   DLLLOCAL void instantiate(QoreValue nval) const {
      //printd(5, "LocalVar::instantiate(%s) this: %p '%s' value closure_use: %s pgm: %p val: %s\n", nval.getTypeName(), this, name.c_str(), closure_use ? "true" : "false", getProgram(), nval.getTypeName());

      if (!closure_use) {
         LocalVarValue* val = thread_instantiate_lvar();
         val->set(name.c_str(), typeInfo, nval);
      }
      else
         thread_instantiate_closure_var(name.c_str(), typeInfo, nval);
   }

   DLLLOCAL void instantiateSelf(QoreObject* value) const {
      //printd(5, "LocalVar::instantiateSelf(%p) this: %p '%s'\n", value, this, name.c_str());
      if (!closure_use) {
         LocalVarValue* val = thread_instantiate_lvar();
         val->set(name.c_str(), typeInfo, value, true);
      }
      else {
         QoreValue val(value->refSelf());
         thread_instantiate_closure_var(name.c_str(), typeInfo, val);
      }
   }

   DLLLOCAL void uninstantiate(ExceptionSink* xsink) const  {
      //printd(5, "LocalVar::uninstantiate() this: %p '%s' closure_use: %s pgm: %p\n", this, name.c_str(), closure_use ? "true" : "false", getProgram());

      if (!closure_use)
         thread_uninstantiate_lvar(xsink);
      else
         thread_uninstantiate_closure_var(xsink);
   }

   DLLLOCAL void uninstantiateSelf() const  {
      if (!closure_use)
         thread_uninstantiate_self();
      else // cannot go out of scope here, so no destructor can be run, so we pass a NULL ExceptionSink ptr
         thread_uninstantiate_closure_var(0);
   }

   DLLLOCAL QoreValue evalValue(bool& needs_deref, ExceptionSink* xsink) const {
      if (!closure_use) {
         LocalVarValue* val = get_var();
         return val->evalValue(needs_deref, xsink);
      }

      ClosureVarValue* val = thread_find_closure_var(name.c_str());
      return val->evalValue(needs_deref, xsink);
   }

   // returns true if the value could contain an object or a closure
   DLLLOCAL bool needsScan() const {
      return typeInfo->needsScan();
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
      //printd(5, "LocalVar::getLValue() this: %p '%s' for_remove: %d closure_use: %d\n", this, getName(), for_remove, closure_use);
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

   DLLLOCAL const QoreTypeInfo* parseGetTypeInfo() const {
      return parse_assigned && (typeInfo == referenceTypeInfo || typeInfo == referenceOrNothingTypeInfo) ? anyTypeInfo : typeInfo;
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
