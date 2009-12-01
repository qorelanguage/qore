/*
  Variable.h

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

#ifndef QORE_THREAD_STACK_BLOCK
#define QORE_THREAD_STACK_BLOCK 128
#endif

class Var;

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
};

// structure for global variables
class Var : public QoreReferenceCounter {
private:
   unsigned char type;
   // holds the value of the variable or a pointer to the imported variable
   union VarValue v;
   std::string name;
   mutable QoreThreadLock m;
   const QoreParseTypeInfo *typeInfo;

   DLLLOCAL void del(ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode *evalIntern(ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode **getValuePtrIntern(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ExceptionSink *xsink) const;
   DLLLOCAL AbstractQoreNode *getValueIntern(AutoVLock *vl);
   DLLLOCAL const AbstractQoreNode *getValueIntern(AutoVLock *vl) const;
   DLLLOCAL void setValueIntern(AbstractQoreNode *val, ExceptionSink *xsink);
   DLLLOCAL void assignInitialValue() {
      assert(type == GV_VALUE);
      // assign default value
      if (typeInfo->hasType()) {
	 qore_type_t t = typeInfo->getType();
	 if (t >= 0 && t < NT_OBJECT) {
	    v.val.value = getDefaultValueForBuitinValueType(t);
	    return;
	 }
      }
      v.val.value = 0;
   }

protected:
   DLLLOCAL ~Var() {}

public:
   DLLLOCAL Var(const char *n_name) : type(GV_VALUE), name(n_name), typeInfo(0) {
      v.val.value = 0;
   }

   DLLLOCAL Var(const char *n_name, const QoreParseTypeInfo *n_typeInfo) : type(GV_VALUE), name(n_name), typeInfo(n_typeInfo) {
      assignInitialValue();
   }

   DLLLOCAL Var(Var *ref, bool ro = false) : type(GV_IMPORT), name(ref->name), typeInfo(0) {
      v.ivar.refptr = ref;
      v.ivar.readonly = ro;
      ref->ROreference();
   }

   DLLLOCAL const char *getName() const;
   DLLLOCAL void setValue(AbstractQoreNode *val, ExceptionSink *xsink);
   DLLLOCAL void makeReference(Var *v, ExceptionSink *xsink, bool ro = false);
   DLLLOCAL bool isImported() const;
   DLLLOCAL void deref(ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ExceptionSink *xsink) const;
   DLLLOCAL AbstractQoreNode *getValue(AutoVLock *vl);
   DLLLOCAL AbstractQoreNode *getReferencedValue() const;
   DLLLOCAL void parseCheckAssignType(const QoreParseTypeInfo *n_typeInfo) {
      //printd(5, "Var::parseCheckAssignType() this=%p %s: type=%s %s new type=%s %s\n", this, name.c_str(), typeInfo->getTypeName(), typeInfo->getCID(), n_typeInfo->getTypeName(), n_typeInfo->getCID());
      // it is safe to call QoreTypeInfo::hasType() when this is 0
      if (!n_typeInfo->hasType())
	 return;

      // here we know that n_typeInfo is not null
      // if no previous type was declared, take the new type
      if (!typeInfo) {
	 typeInfo = n_typeInfo;
	 assert(!v.val.value);
	 assignInitialValue();
	 return;
      }

      // make sure types are identical or throw an exception
      if (!typeInfo->parseStageOneEqual(n_typeInfo))
	 parse_error("global variable '%s' declared previously with an incompatible type", name.c_str());
   }

   DLLLOCAL const QoreTypeInfo *getTypeInfo() const {
      return typeInfo;
   }
};

DLLLOCAL AbstractQoreNode *getNoEvalVarValue(AbstractQoreNode *n, AutoVLock *vl, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *getExistingVarValue(const AbstractQoreNode *n, ExceptionSink *xsink, AutoVLock *vl, ReferenceHolder<AbstractQoreNode> &pt);
DLLLOCAL void delete_var_node(AbstractQoreNode *node, ExceptionSink *xsink);
DLLLOCAL void delete_global_variables();

// for retrieving a pointer to a pointer to an lvalue expression
DLLLOCAL AbstractQoreNode **get_var_value_ptr(const AbstractQoreNode *lvalue, AutoVLock *vl, const QoreTypeInfo *&typeInfo, ExceptionSink *xsink);

DLLLOCAL extern QoreHashNode *ENV;

// this class grabs global variable or object locks for the duration of the scope of the object
// no evaluations can be done while this object is in scope or a deadlock may result
class LValueHelper {
private:
   AbstractQoreNode **v;
   ExceptionSink *xsink;
   AutoVLock vl;
   const QoreTypeInfo *typeInfo;

public:
   DLLLOCAL LValueHelper(const AbstractQoreNode *exp, ExceptionSink *n_xsink) : xsink(n_xsink), vl(n_xsink), typeInfo(0) {
      v = get_var_value_ptr(exp, &vl, typeInfo, xsink);
   }
   DLLLOCAL operator bool() const { return v != 0; }
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
      if (typeInfo->checkType(val, xsink)) {
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
	 (*v) = new QoreBigIntNode();
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
	 (*v) = new QoreFloatNode();
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
