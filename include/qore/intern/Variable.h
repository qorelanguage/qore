/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  Variable.h

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

#ifndef _QORE_VARIABLE_H
#define _QORE_VARIABLE_H

enum qore_var_t {
   VT_UNRESOLVED = 1,
   VT_LOCAL      = 2,
   VT_GLOBAL     = 3,
   VT_CLOSURE    = 4,
   VT_LOCAL_TS   = 5,  // thread-safe variables, not closure-bound
   VT_IMMEDIATE  = 6   // used in references with immediate variable storage
};

#include "qore/intern/RSet.h"
#include "qore/intern/VRMutex.h"
#include "qore/intern/QoreLValue.h"
#include "qore/intern/qore_var_rwlock_priv.h"

#include <string.h>
#include <stdlib.h>

#include <string>
#include <memory>
#include <set>

class Var;
class ScopedObjectCallNode;
class QoreSquareBracketsOperatorNode;
class QoreHashObjectDereferenceOperatorNode;

union qore_gvar_ref_u {
   bool b;
   int64 i;
   double f;
   AbstractQoreNode* n;
   // note that the "readonly" flag is stored in bit 0 of this pointer - do not read directly
   size_t _refptr;

   DLLLOCAL void setPtr(Var* refptr, bool readonly = false) {
      _refptr = (size_t)refptr;
      if (readonly)
         _refptr |= 1;
   }

   DLLLOCAL Var* getPtr() const {
#ifndef HAVE_LLVM_BUG_22050
      // there is a bug in clang++ 3.5.[0|1] where the conditional expression below is executed with the opposite expressions
      // when compiled with -O1 or greater: http://llvm.org/bugs/show_bug.cgi?id=22050
      return (Var*)((_refptr & 1L) ? (_refptr ^ 1L) : _refptr);
#else
      return (Var*)(_refptr & (~1L));
#endif
   }

   DLLLOCAL bool isReadOnly() const {
      return _refptr & 1;
   }

   // checks if the reference can be written to, returns -1 if an exception was thrown
   DLLLOCAL int write(ExceptionSink* xsink) const;
};

class LValueHelper;
class LValueRemoveHelper;

// structure for global variables
class Var : protected QoreReferenceCounter {
private:
   const QoreProgramLocation loc;      // location of the initial definition
   unsigned char type;
   QoreLValue<qore_gvar_ref_u> val;
   std::string name;
   mutable QoreVarRWLock rwl;
   QoreParseTypeInfo *parseTypeInfo;
   const QoreTypeInfo *typeInfo;
   bool pub,                          // is this global var public (valid and set for modules only)
      finalized;                      // has this var already been cleared during Program destruction?

   DLLLOCAL void del(ExceptionSink* xsink);

   // not implemented
   DLLLOCAL Var(const Var&);

protected:
   DLLLOCAL ~Var() { delete parseTypeInfo; }

   DLLLOCAL int checkFinalized(ExceptionSink* xsink) const {
         if (finalized) {
            xsink->raiseException("DESTRUCTOR-ERROR", "illegal variable assignment after second phase of variable destruction");
            return -1;
         }
         return 0;
      }

public:
   DLLLOCAL Var(const char* n_name) : loc(ParseLocation), val(QV_Node), name(n_name), parseTypeInfo(0), typeInfo(0), pub(false), finalized(false) {
   }

   DLLLOCAL Var(const char* n_name, QoreParseTypeInfo *n_parseTypeInfo) : loc(ParseLocation), val(QV_Node), name(n_name), parseTypeInfo(n_parseTypeInfo), typeInfo(0), pub(false), finalized(false) {
   }

   DLLLOCAL Var(const char* n_name, const QoreTypeInfo *n_typeInfo) : loc(ParseLocation), val(n_typeInfo), name(n_name), parseTypeInfo(0), typeInfo(n_typeInfo), pub(false), finalized(false) {
   }

   DLLLOCAL Var(Var* ref, bool ro = false) : loc(ref->loc), val(QV_Ref), name(ref->name), parseTypeInfo(0), typeInfo(ref->typeInfo), pub(false), finalized(false) {
      ref->ROreference();
      val.v.setPtr(ref, ro);
   }

   DLLLOCAL const char* getName() const;

   DLLLOCAL int getLValue(LValueHelper& lvh, bool for_remove) const;
   DLLLOCAL void remove(LValueRemoveHelper& lvrh);

   DLLLOCAL void clearLocal(ExceptionSink* xsink) {
      if (val.type != QV_Ref) {
         ReferenceHolder<> h(xsink);
         QoreAutoVarRWWriteLocker al(rwl);
         if (!finalized)
            finalized = true;
         printd(5, "Var::clearLocal() clearing '%s' %p\n", name.c_str(), this);
#ifdef QORE_ENFORCE_DEFAULT_LVALUE
         h = val.assign(typeInfo->getDefaultQoreValue());
#else
         h = val.removeNode(true);
#endif
      }
#ifdef DEBUG
      else
         printd(5, "Var::clearLocal() skipping imported var '%s' %p\n", name.c_str(), this);
#endif
   }

   DLLLOCAL void setInitial(AbstractQoreNode* v) {
      assert(val.type == QV_Node);
      // try to set an optimized value type for the value holder if possible
      val.set(typeInfo);
      discard(val.assignInitial(v), 0);
   }

   DLLLOCAL bool isImported() const;

   DLLLOCAL void deref(ExceptionSink* xsink);

   DLLLOCAL QoreValue eval() const;

   DLLLOCAL void doDoubleDeclarationError() {
      // make sure types are identical or throw an exception
      if (parseTypeInfo) {
         parse_error("global variable '%s' previously declared with type '%s'", name.c_str(), parseTypeInfo->getName());
         assert(!typeInfo);
      }
      if (typeInfo) {
         parse_error("global variable '%s' previously declared with type '%s'", name.c_str(), typeInfo->getName());
         assert(!parseTypeInfo);
      }
   }

   DLLLOCAL void parseCheckAssignType(QoreParseTypeInfo *n_parseTypeInfo) {
      std::unique_ptr<QoreParseTypeInfo> ti(n_parseTypeInfo);

      //printd(5, "Var::parseCheckAssignType() this=%p %s: type=%s %s new type=%s %s\n", this, name.c_str(), typeInfo->getTypeName(), typeInfo->getCID(), n_typeInfo->getTypeName(), n_typeInfo->getCID());
      // it is safe to call QoreTypeInfo::hasType() when this is 0
      if (!n_parseTypeInfo)
         return;

      if (val.type == QV_Ref) {
         val.v.getPtr()->parseCheckAssignType(n_parseTypeInfo);
         return;
      }

      // here we know that n_typeInfo is not null
      // if no previous type was declared, take the new type
      if (parseTypeInfo || typeInfo) {
         doDoubleDeclarationError();
         return;
      }

      parseTypeInfo = ti.release();
      assert(!val.removeNode(true));
   }

   DLLLOCAL void checkAssignType(const QoreTypeInfo *n_typeInfo) {
      //printd(5, "Var::parseCheckAssignType() this=%p %s: type=%s %s new type=%s %s\n", this, name.c_str(), typeInfo->getTypeName(), typeInfo->getCID(), n_typeInfo->getTypeName(), n_typeInfo->getCID());
      // it is safe to call QoreTypeInfo::hasType() when this is 0
      if (!n_typeInfo->hasType())
         return;

      if (val.type == QV_Ref) {
         val.v.getPtr()->checkAssignType(n_typeInfo);
         return;
      }

      // here we know that n_typeInfo is not null
      // if no previous type was declared, take the new type
      if (parseTypeInfo || typeInfo) {
         doDoubleDeclarationError();
         return;
      }

      typeInfo = n_typeInfo;

      assert(!val.removeNode(true));
   }

   DLLLOCAL void parseInit() {
      if (val.type == QV_Ref)
         return;

      if (parseTypeInfo) {
         typeInfo = parseTypeInfo->resolveAndDelete(loc);
         parseTypeInfo = 0;

         val.set(typeInfo);
      }

#ifdef QORE_ENFORCE_DEFAULT_LVALUE
      if (!val.hasValue())
         discard(val.assignInitial(typeInfo->getDefaultQoreValue()), 0);
#endif
   }

   DLLLOCAL QoreParseTypeInfo *copyParseTypeInfo() const {
      return parseTypeInfo ? parseTypeInfo->copy() : 0;
   }

   DLLLOCAL const QoreTypeInfo *parseGetTypeInfo() {
      // imported variables have already been initialized
      if (val.type == QV_Ref)
         return val.v.getPtr()->getTypeInfo();

      parseInit();
      // we cannot enforce the first reference assignment of global variables, so we return type any
      return typeInfo == referenceTypeInfo || typeInfo == referenceOrNothingTypeInfo ? anyTypeInfo : typeInfo;
   }

   DLLLOCAL const QoreTypeInfo *getTypeInfo() const {
      assert(!parseTypeInfo);
      if (val.type == QV_Ref)
         return val.v.getPtr()->getTypeInfo();

      return typeInfo;
   }

   DLLLOCAL bool hasTypeInfo() const {
      if (val.type == QV_Ref)
         return val.v.getPtr()->hasTypeInfo();

      return parseTypeInfo || typeInfo;
   }

   DLLLOCAL bool isRef() const {
      return val.type == QV_Ref;
   }

   // only called with a new object declaration expression (ie our <class> $x())
   DLLLOCAL const char* getClassName() const {
      if (val.type == QV_Ref)
         return val.v.getPtr()->getClassName();

      if (typeInfo) {
         assert(typeInfo->getUniqueReturnClass());
         return typeInfo->getUniqueReturnClass()->getName();
      }
      assert(parseTypeInfo);
      assert(parseTypeInfo->cscope);
      return parseTypeInfo->cscope->getIdentifier();
   }

   DLLLOCAL bool isPublic() const {
      return pub;
   }

   DLLLOCAL void setPublic() {
      assert(!pub);
      pub = true;
   }

   DLLLOCAL const QoreProgramLocation& getParseLocation() const {
      return loc;
   }
};

DLLLOCAL void delete_global_variables();

DLLLOCAL extern QoreHashNode *ENV;

typedef std::set<const void*> lvid_set_t;

// track obj count changes
struct ObjCountRec {
   // container
   const AbstractQoreNode* con;
   // initial count (true = possible recursive cycle, false = no cycle possible)
   bool before;

   DLLLOCAL ObjCountRec(const QoreListNode* c);
   DLLLOCAL ObjCountRec(const QoreHashNode* c);
   DLLLOCAL ObjCountRec(const QoreObject* c);
   DLLLOCAL int getDifference();
};

typedef std::vector<ObjCountRec> ocvec_t;

// this class grabs global variable or object locks for the duration of the scope of the object
// no evaluations can be done while this object is in scope or a deadlock may result
class LValueHelper {
   friend class LValueRemoveHelper;
   friend class LValueLockHandoffHelper;

private:
   // not implemented
   DLLLOCAL LValueHelper(const LValueHelper&) = delete;
   DLLLOCAL LValueHelper& operator=(const LValueHelper&) = delete;

protected:
   template <class T, typename t, int nt>
   DLLLOCAL T* ensureUnique(const QoreTypeInfo* typeTypeInfo, const char* desc) {
      assert(v);
      if (nt == get_node_type(*v)) {
         if (!(*v)->is_unique()) {
            //printd(5, "LValueHelper::ensureUnique() this: %p saving old value: %p '%s'\n", this, *v, get_type_name(*v));
            AbstractQoreNode* old = *v;
            (*v) = (*v)->realCopy();
            saveTemp(old);
         }
      }
      else {
         if (!typeInfo->parseAccepts(typeTypeInfo)) {
            typeInfo->doTypeException(0, desc, typeTypeInfo->getName(), vl.xsink);
            return 0;
         }
         if (!(*v))
            (*v) = new T;
         else {
            t i = T::getValue(*v);
            //printd(5, "LValueHelper::ensureUnique() this: %p saving old value: %p '%s'\n", this, *v, get_type_name(*v));
            saveTemp(*v);
            (*v) = new T(i);
         }
      }

      return reinterpret_cast<T*>(*v);
   }

   DLLLOCAL void assignIntern(AbstractQoreNode* n) {
      if (val)
         val->assign(n);
      else
         *v = n;
   }

   DLLLOCAL int doListLValue(const QoreSquareBracketsOperatorNode* op, bool for_remove);
   DLLLOCAL int doHashObjLValue(const QoreHashObjectDereferenceOperatorNode* op, bool for_remove);

   DLLLOCAL int makeInt(const char* desc);
   DLLLOCAL int makeFloat(const char* desc);
   DLLLOCAL int makeNumber(const char* desc);

   DLLLOCAL int doRecursiveException() {
      vl.xsink->raiseException("REFERENCE-ERROR", "recursive reference detected in assignment");
      return -1;
   }

public:
   AutoVLock vl;
   AbstractQoreNode** v;     // ptr to ptr for lvalue expression
private:
   typedef std::vector<AbstractQoreNode*> nvec_t;
   nvec_t tvec;
   lvid_set_t* lvid_set;
   ocvec_t ocvec;

   // flag if the changed value was a container before the assignment
   bool before;

   // recursive delta: change to recursive reference count
   int rdt;

   RObject* robj;

public:
   QoreLValueGeneric* val;
   const QoreTypeInfo* typeInfo;

   DLLLOCAL LValueHelper(const ReferenceNode& ref, ExceptionSink* xsink, bool for_remove = false);
   DLLLOCAL LValueHelper(const AbstractQoreNode* exp, ExceptionSink* xsink, bool for_remove = false);

   DLLLOCAL LValueHelper(LValueHelper&& o);

   // to scan objects after initialization
   DLLLOCAL LValueHelper(QoreObject& obj, ExceptionSink* xsink);

   DLLLOCAL ~LValueHelper();

   DLLLOCAL void setClosure(RObject* c) {
      robj = c;
   }

   DLLLOCAL void saveTemp(QoreValue& n);
   DLLLOCAL void saveTemp(AbstractQoreNode* n);

   DLLLOCAL AbstractQoreNode*& getTempRef() {
      tvec.push_back(0);
      return tvec[tvec.size() - 1];
   }

   DLLLOCAL int doLValue(const AbstractQoreNode* exp, bool for_remove);

   DLLLOCAL int doLValue(const ReferenceNode* ref, bool for_remove);

   DLLLOCAL void setAndLock(QoreVarRWLock& rwl);
   DLLLOCAL void set(QoreVarRWLock& rwl);

   DLLLOCAL AutoVLock& getAutoVLock() {
      return vl;
   }

   DLLLOCAL void setTypeInfo(const QoreTypeInfo* ti) {
      typeInfo = ti == referenceTypeInfo || ti == referenceOrNothingTypeInfo ? 0 : ti;
   }

   DLLLOCAL void setPtr(AbstractQoreNode*& ptr) {
      assert(!v);
      assert(!val);
      v = &ptr;
      before = needs_scan(ptr);
   }

   DLLLOCAL void setValue(QoreLValueGeneric& nv);

   DLLLOCAL bool isNode() const {
      return (bool)v;
   }

   DLLLOCAL void resetPtr(AbstractQoreNode** ptr, const QoreTypeInfo* ti = 0) {
      if (val) {
         assert(!v);
         val = 0;
      }
      else
         assert(v);
      v = ptr;
      typeInfo = ti;

      before = needs_scan(*ptr);
   }

   DLLLOCAL void clearPtr() {
      if (val)
         val = 0;
      v = 0;
      typeInfo = 0;
      before = 0;
   }

   DLLLOCAL operator bool() const {
      return val || v;
   }

   DLLLOCAL bool isOptimized() const {
      return val && val->optimized();
   }

   DLLLOCAL const QoreTypeInfo* getTypeInfo() const {
      return typeInfo;
   }

   DLLLOCAL qore_type_t getType() const {
      return val ? val->getType() : get_node_type(*v);
   }

   DLLLOCAL AbstractQoreNode* getValue() {
      return val ? val->getInternalNode() : *v;
   }

   DLLLOCAL const AbstractQoreNode* getValue() const {
      return val ? val->getInternalNode() : *v;
   }

   DLLLOCAL const char* getTypeName() const {
      return val ? val->getTypeName() : get_type_name(*v);
   }

   DLLLOCAL bool checkType(const qore_type_t t) const {
      return getType() == t;
   }

   DLLLOCAL bool isNothing() const {
      return checkType(NT_NOTHING);
   }

   DLLLOCAL QoreValue getReferencedValue() const;

   DLLLOCAL AbstractQoreNode* getReferencedNodeValue() const;

   // only call if there is a value in place
   // FIXME: port operators to LValueHelper instead and remove this function
   DLLLOCAL void ensureUnique() {
      AbstractQoreNode* current_value = getValue();
      assert(current_value && current_value->getType() != NT_OBJECT);

      if (!current_value->is_unique()) {
         //printd(5, "LValueHelper::ensureUnique() this: %p saving old value: %p '%s'\n", this, *v, get_type_name(*v));
         AbstractQoreNode* old = current_value;
         assignIntern(current_value->realCopy());
         saveTemp(old);
      }
   }

   DLLLOCAL int64 getAsBigInt() const;
   DLLLOCAL bool getAsBool() const;
   DLLLOCAL double getAsFloat() const;

   DLLLOCAL int64 plusEqualsBigInt(int64 v, const char* desc = "<lvalue>");
   DLLLOCAL int64 minusEqualsBigInt(int64 v, const char* desc = "<lvalue>");
   DLLLOCAL int64 multiplyEqualsBigInt(int64 v, const char* desc = "<lvalue>");
   DLLLOCAL int64 divideEqualsBigInt(int64 v, const char* desc = "<lvalue>");
   DLLLOCAL int64 orEqualsBigInt(int64 v, const char* desc = "<lvalue>");
   DLLLOCAL int64 andEqualsBigInt(int64 v, const char* desc = "<lvalue>");
   DLLLOCAL int64 xorEqualsBigInt(int64 v, const char* desc = "<lvalue>");
   DLLLOCAL int64 modulaEqualsBigInt(int64 v, const char* desc = "<lvalue>");
   DLLLOCAL int64 shiftLeftEqualsBigInt(int64 v, const char* desc = "<lvalue>");
   DLLLOCAL int64 shiftRightEqualsBigInt(int64 v, const char* desc = "<lvalue>");
   DLLLOCAL int64 preIncrementBigInt(const char* desc = "<lvalue>");
   DLLLOCAL int64 preDecrementBigInt(const char* desc = "<lvalue>");
   DLLLOCAL int64 postIncrementBigInt(const char* desc = "<lvalue>");
   DLLLOCAL int64 postDecrementBigInt(const char* desc = "<lvalue>");

   DLLLOCAL double plusEqualsFloat(double v, const char* desc = "<lvalue>");
   DLLLOCAL double minusEqualsFloat(double v, const char* desc = "<lvalue>");
   DLLLOCAL double multiplyEqualsFloat(double v, const char* desc = "<lvalue>");
   DLLLOCAL double divideEqualsFloat(double v, const char* desc = "<lvalue>");
   DLLLOCAL double preIncrementFloat(const char* desc = "<lvalue>");
   DLLLOCAL double preDecrementFloat(const char* desc = "<lvalue>");
   DLLLOCAL double postIncrementFloat(const char* desc = "<lvalue>");
   DLLLOCAL double postDecrementFloat(const char* desc = "<lvalue>");

   DLLLOCAL void plusEqualsNumber(const AbstractQoreNode* r, const char* desc = "<lvalue>");
   DLLLOCAL void minusEqualsNumber(const AbstractQoreNode* r, const char* desc = "<lvalue>");
   DLLLOCAL void multiplyEqualsNumber(const AbstractQoreNode* r, const char* desc = "<lvalue>");
   DLLLOCAL void divideEqualsNumber(const AbstractQoreNode* r, const char* desc = "<lvalue>");
   DLLLOCAL void preIncrementNumber(const char* desc = "<lvalue>");
   DLLLOCAL void preDecrementNumber(const char* desc = "<lvalue>");
   DLLLOCAL QoreNumberNode* postIncrementNumber(bool ref_rv, const char* desc = "<lvalue>");
   DLLLOCAL QoreNumberNode* postDecrementNumber(bool ref_rv, const char* desc = "<lvalue>");

   DLLLOCAL QoreNumberNode* ensureUniqueNumber(const char* desc = "<lvalue>") {
      AbstractQoreNode** p;
      if (val) {
         if (makeNumber(desc))
            return 0;
         p = &val->v.n;
      }
      else {
         assert(v);
         p = v;
      }

      // if we converted from val above, then we already have a QoreNumberNode
      if (get_node_type(*p) == NT_NUMBER) {
         if (!(*p)->is_unique()) {
            AbstractQoreNode* old = (*p);
            (*p) = (*p)->realCopy();
            saveTemp(old);
         }
      }
      else {
         if (!typeInfo->parseAccepts(numberTypeInfo)) {
            typeInfo->doTypeException(0, desc, numberTypeInfo->getName(), vl.xsink);
            return 0;
         }

         saveTemp(*p);
         *p = new QoreNumberNode(*p);
      }
      return reinterpret_cast<QoreNumberNode*>(*p);
   }

   DLLLOCAL int assign(QoreValue val, const char* desc = "<lvalue>");
   /*
   DLLLOCAL int assignBigInt(int64 v, const char* desc = "<lvalue>");
   DLLLOCAL int assignFloat(double v, const char* desc = "<lvalue>");
   */

   DLLLOCAL AbstractQoreNode* removeNode(bool for_del);
   DLLLOCAL QoreValue remove(bool& static_assignment);

   DLLLOCAL void setDelta(int dt) {
      assert(!rdt);
      rdt = dt;
   }
};

class LValueRemoveHelper {
private:
   // not implemented
   DLLLOCAL LValueRemoveHelper(const LValueRemoveHelper&);
   DLLLOCAL LValueRemoveHelper& operator=(const LValueRemoveHelper&);
   DLLLOCAL void* operator new(size_t);

protected:
   ExceptionSink* xsink;
   QoreLValueGeneric rv;
   bool for_del;

public:
   DLLLOCAL LValueRemoveHelper(const ReferenceNode& ref, ExceptionSink* n_xsink, bool fd);
   DLLLOCAL LValueRemoveHelper(const AbstractQoreNode* exp, ExceptionSink* n_xsink, bool fd);

   DLLLOCAL void doRemove(AbstractQoreNode* exp);

   DLLLOCAL operator bool() const {
      return !*xsink;
   }

   DLLLOCAL ExceptionSink* getExceptionSink() const {
      return xsink;
   }

   DLLLOCAL bool forDel() const {
      return for_del;
   }

   DLLLOCAL void doRemove(QoreLValueGeneric& qv, const QoreTypeInfo* ti) {
#ifdef QORE_ENFORCE_DEFAULT_LVALUE
      rv.assignSetInitialSwap(qv, ti->getDefaultQoreValue());
#else
      rv.assignSetTakeInitial(qv);
#endif
   }

   DLLLOCAL AbstractQoreNode* removeNode();
   DLLLOCAL QoreValue remove(bool& static_assignment);

   DLLLOCAL void deleteLValue();
};

#endif // _QORE_VARIABLE_H
