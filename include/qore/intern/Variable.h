/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  Variable.h

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
   unsigned char type;
   // holds the value of the variable or a pointer to the imported variable
   union VarValue v;
   std::string name;
   mutable QoreThreadLock m;
   QoreParseTypeInfo *parseTypeInfo;
   const QoreTypeInfo *typeInfo;

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

protected:
   DLLLOCAL ~Var() { delete parseTypeInfo; }

public:
   DLLLOCAL Var(const char *n_name) : type(GV_VALUE), v(0), name(n_name), parseTypeInfo(0), typeInfo(0) {
   }

   DLLLOCAL Var(const char *n_name, QoreParseTypeInfo *n_parseTypeInfo) : type(GV_VALUE), v(0), name(n_name), parseTypeInfo(n_parseTypeInfo), typeInfo(0) {
   }

   DLLLOCAL Var(const char *n_name, const QoreTypeInfo *n_typeInfo) : type(GV_VALUE), v(0), name(n_name), parseTypeInfo(0), typeInfo(n_typeInfo) {
   }

   DLLLOCAL Var(Var *ref, bool ro = false) : type(GV_IMPORT), v(ref, ro), name(ref->name), parseTypeInfo(0), typeInfo(0) {
   }

   DLLLOCAL const char *getName() const;
   DLLLOCAL void setValue(AbstractQoreNode *val, ExceptionSink *xsink);
   DLLLOCAL void makeReference(Var *v, ExceptionSink *xsink, bool ro = false);
   DLLLOCAL bool isImported() const;
   DLLLOCAL void deref(ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ExceptionSink *xsink) const;
   DLLLOCAL AbstractQoreNode *getReferencedValue() const;
   DLLLOCAL ScopedObjectCallNode *makeNewCall(AbstractQoreNode *args) const;
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
      parseInit();
      return typeInfo;
   }

   DLLLOCAL const QoreTypeInfo *getTypeInfo() const {
      assert(!parseTypeInfo);
      return typeInfo;
   }

   DLLLOCAL bool hasTypeInfo() const {
      return parseTypeInfo || typeInfo;
   }

   // only called with a new object declaration expression (ie our <class> $x())
   DLLLOCAL const char *getClassName() const {
      if (typeInfo) {
         assert(typeInfo->getUniqueReturnClass());
         return typeInfo->getUniqueReturnClass()->getName();
      }
      assert(parseTypeInfo);
      assert(parseTypeInfo->cscope);
      return parseTypeInfo->cscope->getIdentifier();
   }
};

DLLLOCAL AbstractQoreNode *getExistingVarValue(const AbstractQoreNode *n, ExceptionSink *xsink);

// deletes the value from an lvalue expression
DLLLOCAL void delete_lvalue(AbstractQoreNode *node, ExceptionSink *xsink);
// like delete_lvalue, but returns the value removed from the lvalue
DLLLOCAL AbstractQoreNode *remove_lvalue(AbstractQoreNode *node, ExceptionSink *xsink);

DLLLOCAL void delete_global_variables();

// for retrieving a pointer to a pointer to an lvalue expression
DLLLOCAL AbstractQoreNode **get_var_value_ptr(const AbstractQoreNode *lvalue, AutoVLock *vl, const QoreTypeInfo *&typeInfo, obj_map_t &omap, ExceptionSink *xsink);

DLLLOCAL extern QoreHashNode *ENV;

// this class grabs global variable or object locks for the duration of the scope of the object
// no evaluations can be done while this object is in scope or a deadlock may result
class LValueHelper {
private:
   AbstractQoreNode **v;
   ExceptionSink *xsink;
   AutoVLock vl;
   const QoreTypeInfo *typeInfo;
   obj_map_t omap;
   
public:
   DLLLOCAL LValueHelper(const AbstractQoreNode *exp, ExceptionSink *n_xsink) : xsink(n_xsink), vl(n_xsink), typeInfo(0) {
      v = get_var_value_ptr(exp, &vl, typeInfo, omap, xsink);
   }
   DLLLOCAL ~LValueHelper() {
      if (v && !*xsink)
         qoreCheckContainer(*v, omap, vl, xsink);
   }
   DLLLOCAL operator bool() const { return v != 0; }
   DLLLOCAL const QoreTypeInfo *get_type_info() const {
      return typeInfo;
   }
   DLLLOCAL const qore_type_t get_type() const { return *v ? (*v)->getType() : NT_NOTHING; }
   DLLLOCAL bool check_type(const qore_type_t t) const {
      if (!(*v))
         return (t == NT_NOTHING) ? true : false;
      return t == (*v)->getType();
   }
   DLLLOCAL bool is_nothing() const { return ::is_nothing(*v); }
   DLLLOCAL AbstractQoreNode *get_value() { return *v; }
   DLLLOCAL AbstractQoreNode *take_value() { AbstractQoreNode *rv = *v; *v = 0; return rv; }

   DLLLOCAL int assign(AbstractQoreNode *val) {
      //printd(0, "LValueHelper::assign() this=%p val=%p (%s) typeInfo=%s calling checkType()\n", this, val, val ? val->getTypeName() : "NOTHING", typeInfo->getName());
      val = typeInfo->acceptAssignment("<lvalue>", val, xsink);
      if (*xsink) {
         discard(val, xsink);
         return -1;
      }

      if (*v) {
         (*v)->deref(xsink);
         if (*xsink) {
            (*v) = 0;
            discard(val, xsink);
            return -1;
         }
      }
      (*v) = val;
      return 0;
   }

   DLLLOCAL int ensure_unique() {
      assert((*v) && (*v)->getType() != NT_OBJECT);

      if (!(*v)->is_unique()) {
         AbstractQoreNode *old = *v;
         (*v) = old->realCopy();
         old->deref(xsink);
         return *xsink; 
      }
      return 0;
   }

   DLLLOCAL int ensure_unique_int() {
      if (!(*v)) {
         (*v) = new QoreBigIntNode;
         return 0;
      }

      if ((*v)->getType() != NT_INT) {
         int64 i = (*v)->getAsBigInt();
         (*v)->deref(xsink);
         if (*xsink) {
            (*v) = 0;
            return -1;
         }
         (*v) = new QoreBigIntNode(i);
         return 0;
      }

      if ((*v)->is_unique())
         return 0;

      QoreBigIntNode *old = reinterpret_cast<QoreBigIntNode *>(*v);
      (*v) = old->realCopy();
      old->deref();
      return 0;
   }

   DLLLOCAL int ensure_unique_float() {
      if (!(*v)) {
         (*v) = new QoreFloatNode;
         return 0;
      }

      if ((*v)->getType() != NT_FLOAT) {
         double f = (*v)->getAsFloat();
         (*v)->deref(xsink);
         if (*xsink) {
            (*v) = 0;
            return -1;
         }
         (*v) = new QoreFloatNode(f);
         return 0;
      }
      
      if ((*v)->is_unique())
         return 0;

      QoreFloatNode *old = reinterpret_cast<QoreFloatNode *>(*v);
      (*v) = old->realCopy();
      old->deref();
      return 0;
   }
};

#endif // _QORE_VARIABLE_H
