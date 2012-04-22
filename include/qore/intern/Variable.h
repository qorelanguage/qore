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

#define GV_VALUE  1
#define GV_IMPORT 2

#include <qore/intern/VRMutex.h>

#include <string.h>
#include <stdlib.h>

#include <string>
#include <memory>

#ifndef QORE_THREAD_STACK_BLOCK
#define QORE_THREAD_STACK_BLOCK 128
#endif

class Var;
class ScopedObjectCallNode;

union VarValue {
   // for value
   struct {
      AbstractQoreNode *value;
   } val;
   // for imported variables
   struct {
      Var *refptr;
      bool readonly;
   } ivar;

   DLLLOCAL VarValue(AbstractQoreNode *n_value) {
      val.value = n_value;
   }
   DLLLOCAL VarValue(Var *n_refptr, bool n_readonly);
};

// structure for global variables
class Var : public QoreReferenceCounter {
private:
   const QoreProgramLocation loc;           // location of the initial definition
   unsigned char type;                // holds the value of the variable or a pointer to the imported variable
   union VarValue v;
   std::string name;
   mutable QoreThreadLock m;
   QoreParseTypeInfo *parseTypeInfo;
   const QoreTypeInfo *typeInfo;
   bool pub;                          // is this global var public (valid and set for modules only)

   DLLLOCAL void del(ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode *evalIntern(ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode **getValuePtrIntern(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ExceptionSink *xsink) const;
   DLLLOCAL AbstractQoreNode *getValueIntern(AutoVLock *vl);
   DLLLOCAL const AbstractQoreNode *getValueIntern(AutoVLock *vl) const;
   DLLLOCAL void setValueIntern(AbstractQoreNode *val, ExceptionSink *xsink);
/*
   DLLLOCAL void assignInitialValue() {
      // assign default value
      if (!v.val.value)
         v.val.value = typeInfo->getDefaultValue();
   }
*/

   // not implemented
   DLLLOCAL Var(const Var&);

protected:
   DLLLOCAL ~Var() { delete parseTypeInfo; }

public:
   DLLLOCAL Var(const char *n_name) : type(GV_VALUE), v(0), name(n_name), parseTypeInfo(0), typeInfo(0), pub(false) {
   }

   DLLLOCAL Var(const char *n_name, QoreParseTypeInfo *n_parseTypeInfo) : type(GV_VALUE), v(0), name(n_name), parseTypeInfo(n_parseTypeInfo), typeInfo(0), pub(false) {
   }

   DLLLOCAL Var(const char *n_name, const QoreTypeInfo *n_typeInfo) : type(GV_VALUE), v(0), name(n_name), parseTypeInfo(0), typeInfo(n_typeInfo), pub(false) {
   }

   DLLLOCAL Var(Var *ref, bool ro = false) : loc(ref->loc), type(GV_IMPORT), v(ref, ro), name(ref->name), parseTypeInfo(0), typeInfo(ref->typeInfo), pub(false) {
   }

   DLLLOCAL const char *getName() const;
   DLLLOCAL void setValue(AbstractQoreNode *val, ExceptionSink *xsink);
   DLLLOCAL bool isImported() const;
   DLLLOCAL void deref(ExceptionSink *xsink);

   DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ExceptionSink *xsink) const;
   DLLLOCAL AbstractQoreNode *getReferencedValue() const;
   DLLLOCAL AbstractQoreNode* remove(ExceptionSink* xsink);

   //DLLLOCAL ScopedObjectCallNode *makeNewCall(AbstractQoreNode *args) const;
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

      if (type == GV_IMPORT) {
         v.ivar.refptr->parseCheckAssignType(n_parseTypeInfo);
         return;
      }

      // here we know that n_typeInfo is not null
      // if no previous type was declared, take the new type
      if (parseTypeInfo || typeInfo) {
         doDoubleDeclarationError();
         return;
      }

      parseTypeInfo = ti.release();
      assert(!v.val.value);
   }

   DLLLOCAL void checkAssignType(const QoreTypeInfo *n_typeInfo) {
      //printd(5, "Var::parseCheckAssignType() this=%p %s: type=%s %s new type=%s %s\n", this, name.c_str(), typeInfo->getTypeName(), typeInfo->getCID(), n_typeInfo->getTypeName(), n_typeInfo->getCID());
      // it is safe to call QoreTypeInfo::hasType() when this is 0
      if (!n_typeInfo->hasType())
         return;

      if (type == GV_IMPORT) {
         v.ivar.refptr->checkAssignType(n_typeInfo);
         return;
      }

      // here we know that n_typeInfo is not null
      // if no previous type was declared, take the new type
      if (parseTypeInfo || typeInfo) {
         doDoubleDeclarationError();
         return;
      }

      typeInfo = n_typeInfo;
      assert(!v.val.value);
   }

   DLLLOCAL void parseInit() {
      if (type == GV_IMPORT)
         return;

      if (parseTypeInfo) {
         typeInfo = parseTypeInfo->resolveAndDelete();
         parseTypeInfo = 0;
      }
      //assignInitialValue();
   }

   DLLLOCAL QoreParseTypeInfo *copyParseTypeInfo() const {
      return parseTypeInfo ? parseTypeInfo->copy() : 0;
   }

   DLLLOCAL const QoreTypeInfo *parseGetTypeInfo() {
      // imported variables have already been initialized
      if (type == GV_IMPORT)
         return v.ivar.refptr->getTypeInfo();

      parseInit();
      return typeInfo;
   }

   DLLLOCAL const QoreTypeInfo *getTypeInfo() const {
      assert(!parseTypeInfo);
      if (type == GV_IMPORT)
         return v.ivar.refptr->getTypeInfo();
      return typeInfo;
   }

   DLLLOCAL bool hasTypeInfo() const {
      if (type == GV_IMPORT)
         return v.ivar.refptr->hasTypeInfo();
      return parseTypeInfo || typeInfo;
   }

   DLLLOCAL bool isRef() const {
      return type == GV_IMPORT;
   }

   // only called with a new object declaration expression (ie our <class> $x())
   DLLLOCAL const char *getClassName() const {
      if (type == GV_IMPORT)
         return v.ivar.refptr->getClassName();

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

DLLLOCAL AbstractQoreNode *getExistingVarValue(const AbstractQoreNode *n, ExceptionSink *xsink);

// deletes the value from an lvalue expression
DLLLOCAL void delete_lvalue(AbstractQoreNode *node, ExceptionSink *xsink);
// like delete_lvalue, but returns the value removed from the lvalue
DLLLOCAL AbstractQoreNode *remove_lvalue(AbstractQoreNode *node, ExceptionSink *xsink, bool for_del = false);
DLLLOCAL int64 remove_lvalue_bigint(AbstractQoreNode *node, ExceptionSink *xsink);
DLLLOCAL double remove_lvalue_float(AbstractQoreNode *node, ExceptionSink *xsink);

DLLLOCAL void delete_global_variables();

// for retrieving a pointer to a pointer to an lvalue expression
DLLLOCAL AbstractQoreNode **get_var_value_ptr(const AbstractQoreNode *lvalue, AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink);

DLLLOCAL extern QoreHashNode *ENV;

enum lvt_e { 
   LVT_Unknown     = 0, //!< type not yet set
   LVT_Normal      = 1, //!< normal lvalue (ie anything except an optimized local variable)
   LVT_OptLocalVar = 2, //!< optimized local variable
};

class LValueExpressionHelper {
protected:
   DLLLOCAL ~LValueExpressionHelper() {
      assert(!v);
      assert(!dr);
   }

public:
   AbstractQoreNode **v,  // ptr to ptr for lvalue expression
      *dr;                // old value to dereference outside the lock   
   AutoVLock vl;
   ObjMap omap;
   bool already_checked : 1;

   DLLLOCAL LValueExpressionHelper(const AbstractQoreNode *exp, const QoreTypeInfo *&typeInfo, ExceptionSink *xsink) : v(0), dr(0), vl(xsink), already_checked(false) {
      v = get_var_value_ptr(exp, &vl, typeInfo, omap, xsink);
   }

   DLLLOCAL void del(ExceptionSink *xsink) {
#ifdef _QORE_CYCLE_CHECK
      printd(0, "LValueExpressionHelper::~LValueExpressionHelper() v=%p *v=%p *xsink=%d already_checked=%d\n", v, v ? *v : 0, (bool)*xsink, already_checked);
      if (v && !*xsink && !already_checked) {
         omap.mark();
         qoreCheckContainer(*v, omap, vl, xsink);
      }
#endif

      // release locks
      vl.del();

      // delete any value left over
      if (dr)
         dr->deref(xsink);

#ifdef DEBUG
      v = 0;
      dr = 0;
#endif

      delete this;
   }

   DLLLOCAL AbstractQoreNode *swapValue(AbstractQoreNode *nv) {
      AbstractQoreNode *rv = *v; 
      *v = nv; 
      return rv;
   }

   DLLLOCAL int ensureUnique(ExceptionSink *xsink) {
      assert((*v) && (*v)->getType() != NT_OBJECT);

      if (!(*v)->is_unique()) {
         AbstractQoreNode *old = *v;
         (*v) = old->realCopy();
         old->deref(xsink);
         return *xsink; 
      }
      return 0;
   }

   template <class T, typename t, int nt>
   DLLLOCAL int ensureUnique(ExceptionSink *xsink) {
      if (!(*v)) {
         (*v) = new T;
         return 0;
      }

      if ((*v)->getType() != nt) {
         t i = T::getValue(*v);
         (*v)->deref(xsink);
         if (*xsink) {
            (*v) = 0;
            return -1;
         }
         (*v) = new T(i);
         return 0;
      }

      if ((*v)->is_unique())
         return 0;

      T *old = reinterpret_cast<T *>(*v);
      (*v) = old->realCopy();
      old->deref();
      return 0;
   }

   DLLLOCAL AbstractQoreNode *getReferencedValue() const {
      return *v ? (*v)->refSelf() : 0;
   }

   DLLLOCAL int assign(AbstractQoreNode *val, ExceptionSink *xsink) {
      assert(!dr);
      dr = swapValue(val);
      return 0;
   }
};

class LocalVarValue;

// this class grabs global variable or object locks for the duration of the scope of the object
// no evaluations can be done while this object is in scope or a deadlock may result
class LValueHelper {
protected:
   ExceptionSink* xsink;
   union {
      LValueExpressionHelper* n;
      LocalVarValue* v;
   } lv;
   const QoreTypeInfo* typeInfo;
   lvt_e lvt : 2;

public:
   DLLLOCAL LValueHelper(const AbstractQoreNode *exp, ExceptionSink *n_xsink);

   DLLLOCAL ~LValueHelper() {
      if (lvt == LVT_Normal)
         lv.n->del(xsink);
   }

   DLLLOCAL operator bool() const {
      return lvt == LVT_OptLocalVar || lv.n->v;
   }

   DLLLOCAL bool isOptimized() const {
      return lvt == LVT_OptLocalVar;
   }

   const QoreTypeInfo *get_type_info() const {
      return typeInfo;
   }

   const qore_type_t get_type() const;
   const char* get_type_name() const;

   DLLLOCAL bool check_type(const qore_type_t t) const {
      return get_type() == t;
   }

   DLLLOCAL bool is_nothing() const {
      return check_type(NT_NOTHING);
   }

   DLLLOCAL AbstractQoreNode *getReferencedValue() const;

   DLLLOCAL AbstractQoreNode *get_value() const {
      assert(lvt == LVT_Normal);
      return *(lv.n->v);
   }

   DLLLOCAL AbstractQoreNode *take_value() {
      assert(lvt == LVT_Normal);
      return lv.n->swapValue(0);
   }

   DLLLOCAL AbstractQoreNode *swapValue(AbstractQoreNode *v) {
      assert(lvt == LVT_Normal);
      return lv.n->swapValue(v);
   }

   DLLLOCAL int64 plusEqualsBigInt(int64 v);
   DLLLOCAL int64 minusEqualsBigInt(int64 v);
   DLLLOCAL int64 multiplyEqualsBigInt(int64 v);
   DLLLOCAL int64 divideEqualsBigInt(int64 v);

   DLLLOCAL double multiplyEqualsFloat(double v);
   DLLLOCAL double divideEqualsFloat(double v);

   DLLLOCAL int assign(AbstractQoreNode *val, const char *desc = "<lvalue>");

   DLLLOCAL int assignBigInt(int64 v, const char *desc = "<lvalue>");
   DLLLOCAL int assignFloat(double v, const char *desc = "<lvalue>");

   DLLLOCAL int ensure_unique() {
      assert(lvt == LVT_Normal);
      return lv.n->ensureUnique(xsink);
   }

   DLLLOCAL int ensure_unique_int() {
      assert(lvt == LVT_Normal);
      return lv.n->ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink);
   }

   DLLLOCAL int ensure_unique_float() {
      assert(lvt == LVT_Normal);
      return lv.n->ensureUnique<QoreFloatNode, double, NT_FLOAT>(xsink);
   }

   DLLLOCAL AutoVLock &getAutoVLock() {
      assert(lvt == LVT_Normal);
      return lv.n->vl;
   }

   DLLLOCAL ObjMap &getObjMap() {
      assert(lvt == LVT_Normal);
      return lv.n->omap;
   }

   DLLLOCAL void alreadyChecked() {
      assert(lvt == LVT_Normal);
      lv.n->already_checked = true;
   }

   DLLLOCAL ExceptionSink *getExceptionSink() {
      return xsink;
   }
};

#endif // _QORE_VARIABLE_H
