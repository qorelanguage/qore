/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  Variable.h

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

#ifndef _QORE_VARIABLE_H
#define _QORE_VARIABLE_H

enum qore_var_t { 
   VT_UNRESOLVED = 1, 
   VT_LOCAL      = 2, 
   VT_GLOBAL     = 3,
   VT_CLOSURE    = 4,
   VT_OBJECT     = 5 // used for references only
};

#include <qore/intern/VRMutex.h>
#include <qore/intern/QoreValue.h>

#include <string.h>
#include <stdlib.h>

#include <string>
#include <memory>

#ifndef QORE_THREAD_STACK_BLOCK
#define QORE_THREAD_STACK_BLOCK 128
#endif

class Var;
class ScopedObjectCallNode;

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
      return _refptr & 1 ? (Var*)(_refptr ^ 1) : (Var*)_refptr;
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
class Var : public QoreReferenceCounter {
private:
   const QoreProgramLocation loc;      // location of the initial definition
   unsigned char type;              
   QoreValue<qore_gvar_ref_u> val;
   std::string name;
   mutable QoreThreadLock m;
   QoreParseTypeInfo *parseTypeInfo;
   const QoreTypeInfo *typeInfo;
   bool pub;                          // is this global var public (valid and set for modules only)

   DLLLOCAL void del(ExceptionSink* xsink);

   // not implemented
   DLLLOCAL Var(const Var&);

protected:
   DLLLOCAL ~Var() { delete parseTypeInfo; }

public:
   DLLLOCAL Var(const char* n_name) : val(QV_Node), name(n_name), parseTypeInfo(0), typeInfo(0), pub(false) {
   }

   DLLLOCAL Var(const char* n_name, QoreParseTypeInfo *n_parseTypeInfo) : val(QV_Node), name(n_name), parseTypeInfo(n_parseTypeInfo), typeInfo(0), pub(false) {
   }

   DLLLOCAL Var(const char* n_name, const QoreTypeInfo *n_typeInfo) : val(n_typeInfo), name(n_name), parseTypeInfo(0), typeInfo(n_typeInfo), pub(false) {
   }

   DLLLOCAL Var(Var *ref, bool ro = false) : loc(ref->loc), val(QV_Ref), name(ref->name), parseTypeInfo(0), typeInfo(ref->typeInfo), pub(false) {
      ref->ROreference();
      val.v.setPtr(ref, ro);
   }

   DLLLOCAL const char* getName() const;

   DLLLOCAL int getLValue(ExceptionSink* xsink, LValueHelper& lvh, bool for_remove) const;
   DLLLOCAL void remove(LValueRemoveHelper& lvrh);

   DLLLOCAL void clearLocal(ExceptionSink* xsink) {
      if (val.type != QV_Ref) {
         printd(5, "Var::clearLocal() clearing '%s' %p\n", name.c_str(), this);
         discard(val.remove(true), xsink);
      }
#ifdef DEBUG
      else
         printd(5, "Var::clearLocal() skipping imported var '%s' %p\n", name.c_str(), this);
#endif

   }

   DLLLOCAL qore_type_t getValueType() const {
      return val.type == QV_Ref ? val.v.getPtr()->getValueType() : val.getType();
   }

   DLLLOCAL const char* getValueTypeName() const {
      return val.type == QV_Ref ? val.v.getPtr()->getValueTypeName() : val.getTypeName();
   }

   DLLLOCAL void setInitial(AbstractQoreNode* v) {
      assert(val.type == QV_Node);
      assert(!val.v.n);
      val.v.n = v;
   }

   DLLLOCAL bool isImported() const;
   DLLLOCAL void deref(ExceptionSink* xsink);

   DLLLOCAL AbstractQoreNode *eval() const;
   DLLLOCAL AbstractQoreNode* eval(bool &needs_deref) const;
   DLLLOCAL int64 bigIntEval() const;
   DLLLOCAL double floatEval() const;

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
      std::auto_ptr<QoreParseTypeInfo> ti(n_parseTypeInfo);

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
      assert(!val.remove(true));
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
      assert(!val.remove(true));
   }

   DLLLOCAL void parseInit() {
      if (val.type == QV_Ref)
         return;

      if (parseTypeInfo) {
         typeInfo = parseTypeInfo->resolveAndDelete(loc);
         parseTypeInfo = 0;

         val.set(typeInfo);
      }
   }

   DLLLOCAL QoreParseTypeInfo *copyParseTypeInfo() const {
      return parseTypeInfo ? parseTypeInfo->copy() : 0;
   }

   DLLLOCAL const QoreTypeInfo *parseGetTypeInfo() {
      // imported variables have already been initialized
      if (val.type == QV_Ref)
         return val.v.getPtr()->getTypeInfo();

      parseInit();
      return typeInfo;
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

class QoreTreeNode;

// this class grabs global variable or object locks for the duration of the scope of the object
// no evaluations can be done while this object is in scope or a deadlock may result
class LValueHelper {
   friend class LValueRemoveHelper;

private:
   // not implemented
   DLLLOCAL LValueHelper(const LValueHelper&);
   DLLLOCAL LValueHelper& operator=(const LValueHelper&);

protected:
   template <class T, typename t, int nt>
   DLLLOCAL T* ensureUnique(const QoreTypeInfo* typeTypeInfo, const char* desc) {
      if (nt == get_node_type(*v)) {
         if (!(*v)->is_unique()) {
            //printd(5, "LValueHelper::ensureUnique() this: %p saving old value: %p '%s'\n", this, *v, get_type_name(*v));
            saveTemp(*v);
            (*v) = (*v)->realCopy();
         }
      }
      else {
         if (!typeInfo->parseAccepts(typeTypeInfo)) {
            typeInfo->doTypeException(-1, desc, typeTypeInfo->getName(), vl.xsink);
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

   DLLLOCAL int doListLValue(const QoreTreeNode* tree, bool for_remove);
   DLLLOCAL int doHashObjLValue(const QoreTreeNode* tree, bool for_remove);

public:
   AutoVLock vl;
   AbstractQoreNode** v;     // ptr to ptr for lvalue expression
private:
   typedef std::vector<AbstractQoreNode*> nvec_t;
   nvec_t tvec;
public:
   QoreValueGeneric* val;
   const QoreTypeInfo* typeInfo;

   DLLLOCAL LValueHelper(const AbstractQoreNode* exp, ExceptionSink* xsink, bool for_remove = false);

   DLLLOCAL ~LValueHelper() {
      // first free any locks
      vl.del();

      // now delete temporary value (if any)
      for (nvec_t::iterator i = tvec.begin(), e = tvec.end(); i != e; ++i)
         discard(*i, vl.xsink);
   }

   DLLLOCAL unsigned saveTemp(AbstractQoreNode* n) {
#ifdef DEBUG
      if (!n) n = &True;
#else
      if (!n || !n->isReferenceCounted())
         return;
#endif
      tvec.push_back(n);
      return tvec.size() - 1;
   }

   DLLLOCAL AbstractQoreNode*& getTempRef() {
      tvec.push_back(0);
      return tvec[tvec.size() - 1];
   }

   DLLLOCAL int doLValue(const AbstractQoreNode* exp, bool for_remove);

   DLLLOCAL void setAndLock(QoreThreadLock& m);

   DLLLOCAL AutoVLock& getAutoVLock() {
      return vl;
   }

   DLLLOCAL void setTypeInfo(const QoreTypeInfo* ti) {
      typeInfo = ti;
   }

   DLLLOCAL void setPtr(AbstractQoreNode*& ptr) {
      assert(!v);
      assert(!val);
      v = &ptr;
   }

   DLLLOCAL void setValue(QoreValueGeneric& nv) {
      assert(!v);
      assert(!val);
      if (nv.type == QV_Node)
         v = &(nv.v.n);
      else
         val = &nv;
   }

   DLLLOCAL bool isNode() const {
      return (bool)v;
   }

   DLLLOCAL void resetPtr(AbstractQoreNode** ptr, const QoreTypeInfo* ti = 0) {
      assert(v);
      assert(!val);
      v = ptr;
      typeInfo = ti;
   }

   DLLLOCAL void clearPtr() {
      assert(v);
      v = 0;
      typeInfo = 0;
   }

   DLLLOCAL AbstractQoreNode*& getPtrRef() {
      assert(v);
      return *v;
   }

   DLLLOCAL operator bool() const {
      return val || v;
   }

   DLLLOCAL bool isOptimized() const {
      return (bool)val;
   }

   const QoreTypeInfo *getTypeInfo() const {
      return typeInfo;
   }

   const qore_type_t getType() const;
   const char* getTypeName() const;

   DLLLOCAL bool checkType(const qore_type_t t) const {
      return getType() == t;
   }

   DLLLOCAL bool isNothing() const {
      return checkType(NT_NOTHING);
   }

   DLLLOCAL AbstractQoreNode *getReferencedValue() const;

   // only call after calling checkType() to ensure the type is correct and cannot be optimized
   // FIXME: port operators to LValueHelper instead and remove this function
   DLLLOCAL AbstractQoreNode *getValue() {
      assert(*v);
      return *v;
   }

   // only call after calling checkType() to ensure the type is correct and cannot be optimized
   // FIXME: port operators to LValueHelper instead and remove this function
   DLLLOCAL const AbstractQoreNode *getValue() const {
      assert(*v);
      return *v;
   }

   // only call if there is a value in place
   // FIXME: port operators to LValueHelper instead and remove this function
   DLLLOCAL void ensureUnique() {
      assert((*v) && (*v)->getType() != NT_OBJECT);

      if (!(*v)->is_unique()) {
         //printd(5, "LValueHelper::ensureUnique() this: %p saving old value: %p '%s'\n", this, *v, get_type_name(*v));
         saveTemp(*v);
         (*v) = (*v)->realCopy();
      }
   }

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

   DLLLOCAL int assign(AbstractQoreNode *val, const char* desc = "<lvalue>");

   DLLLOCAL int assignBigInt(int64 v, const char* desc = "<lvalue>");
   DLLLOCAL int assignFloat(double v, const char* desc = "<lvalue>");

   DLLLOCAL int64 removeBigInt();
   DLLLOCAL double removeFloat();
   DLLLOCAL AbstractQoreNode* remove(bool for_del);

   /*
   DLLLOCAL int ensure_unique_int() {
      assert(lvt == LVT_Expr);
      return lv.n->ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink);
   }

   DLLLOCAL int ensure_unique_float() {
      assert(lvt == LVT_Expr);
      return lv.n->ensureUnique<QoreFloatNode, double, NT_FLOAT>(xsink);
   }
    */

   DLLLOCAL ExceptionSink* getExceptionSink() {
      return vl.xsink;
   }
};

class LValueRemoveHelper {
private:
   // not implemented
   DLLLOCAL LValueRemoveHelper(const LValueRemoveHelper&);
   DLLLOCAL LValueRemoveHelper& operator=(const LValueRemoveHelper&);
   DLLLOCAL void *operator new(size_t);

protected:
   ExceptionSink* xsink;
   QoreValueGeneric rv;
   bool for_del;

public:
   DLLLOCAL LValueRemoveHelper(const AbstractQoreNode* exp, ExceptionSink* n_xsink, bool fd);

   DLLLOCAL void doRemove(AbstractQoreNode* exp);

   DLLLOCAL operator bool() const {
      return !*xsink;
   }

   DLLLOCAL bool forDel() const {
      return for_del;
   }

   DLLLOCAL void setRemove(AbstractQoreNode* n) {
      rv.assignInitial(n);
   }

   DLLLOCAL void setRemove(QoreValueGeneric& qv) {
      rv.assignTakeInitial(qv);
   }

   DLLLOCAL int64 removeBigInt();
   DLLLOCAL double removeFloat();
   DLLLOCAL AbstractQoreNode* remove();

   DLLLOCAL void deleteLValue();
};

#endif // _QORE_VARIABLE_H
