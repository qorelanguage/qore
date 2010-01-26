/*
  BuiltinFunction.h

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

#ifndef _QORE_BUILTINFUNCTION_H

#define _QORE_BUILTINFUNCTION_H
class BCList;
class BCEAList;

class BuiltinSignature : public AbstractFunctionSignature {
public:
   const QoreTypeInfo **typeList;
   const QoreTypeInfo *returnTypeInfo;

   DLLLOCAL BuiltinSignature(const QoreTypeInfo *n_returnTypeInfo, unsigned n_num_params, const QoreTypeInfo **n_typeList) : AbstractFunctionSignature(n_num_params), typeList(n_typeList), returnTypeInfo(n_returnTypeInfo) {
   }
   DLLLOCAL virtual ~BuiltinSignature() {
      delete [] typeList;
   }
   DLLLOCAL virtual void resolve() {}
   DLLLOCAL virtual const QoreTypeInfo *getParamTypeInfoImpl(unsigned num) const {
      return typeList[num];
   }
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const {
      return returnTypeInfo;
   }
   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const {
      return returnTypeInfo;
   }
   //DLLLOCAL virtual const AbstractQoreNode **getDefaultArgList() const = 0;
};

class BuiltinFunctionBase : public AbstractQoreFunction {
protected:
   const char *name;
   // functionality bitmap for parse restrictions
   int functionality;
   BuiltinSignature params;
   const AbstractQoreNode **defaultArgList;

public:
   DLLLOCAL BuiltinFunctionBase(const char *n_name, int n_functionality, const QoreTypeInfo *n_returnTypeInfo, unsigned n_num_params, const QoreTypeInfo **n_typeList, const AbstractQoreNode **n_defaultArgList) : name(n_name), functionality(n_functionality), params(n_returnTypeInfo, n_num_params, n_typeList), defaultArgList(n_defaultArgList) {
   }
   DLLLOCAL BuiltinFunctionBase() : name(0), params(0, 0, 0), defaultArgList(0) {
      assert(false);
   }
   DLLLOCAL virtual ~BuiltinFunctionBase() {
      delete [] defaultArgList;
   }
   DLLLOCAL virtual const char *getName() const { 
      return name;
   }
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const {
      return params.parseGetReturnTypeInfo();
   }
   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const {
      return params.getReturnTypeInfo();
   }
   DLLLOCAL virtual AbstractFunctionSignature *getSignature() const {
      return const_cast<BuiltinSignature *>(&params);
   }
   DLLLOCAL int getType() const {
      return functionality;
   }
   DLLLOCAL virtual bool isUserCode() const {
      return false;
   }
};

class BuiltinMethod : public BuiltinFunctionBase, protected QoreReferenceCounter {
public:
   QoreClass *myclass;
   DLLLOCAL BuiltinMethod(QoreClass *n_myclass, const char *n_name, int n_functionality, const QoreTypeInfo *n_returnTypeInfo, unsigned n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinFunctionBase(n_name, n_functionality, n_returnTypeInfo, n_num_params, n_typeList, n_defaultArgList), myclass(n_myclass) {
   }
   DLLLOCAL virtual void ref() {
      ROreference();
   }
   DLLLOCAL virtual void deref() {
      if (ROdereference())
	 delete this;
   }
};

class BuiltinConstructor : public BuiltinMethod {
protected:
   q_constructor_t constructor;

public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   // also with functionality - enforced by the new operator
   DLLLOCAL BuiltinConstructor(QoreClass *c, q_constructor_t m, int n_functionality = QDOM_DEFAULT, unsigned n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinMethod(c, "constructor", n_functionality, 0, n_num_params, n_typeList, n_defaultArgList), constructor(m) {
   }
   
   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject *self, const QoreListNode *args, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const;
};

class BuiltinConstructor2 : public BuiltinMethod {
protected:
   q_constructor2_t constructor;

public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   // also with functionality - enforced by the new operator
   DLLLOCAL BuiltinConstructor2(QoreClass *c, q_constructor2_t m, int n_functionality = QDOM_DEFAULT, unsigned n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinMethod(c, "constructor", n_functionality, 0, n_num_params, n_typeList, n_defaultArgList), constructor(m) {
   }
   
   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject *self, const QoreListNode *args, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const;
};

class BuiltinSystemConstructor : public BuiltinMethod {
protected:
   q_system_constructor_t system_constructor;

public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   // also with functionality - enforced by the new operator
   DLLLOCAL BuiltinSystemConstructor(QoreClass *c, q_system_constructor_t m, unsigned n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinMethod(c, "constructor", 0, 0, n_num_params, n_typeList, n_defaultArgList), system_constructor(m) {
   }
   
   DLLLOCAL void eval(QoreObject *self, int code, va_list args) const {
      system_constructor(self, code, args);
   }
};

class BuiltinSystemConstructor2 : public BuiltinMethod {
protected:
   q_system_constructor2_t system_constructor;

public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   // also with functionality - enforced by the new operator
   DLLLOCAL BuiltinSystemConstructor2(QoreClass *c, q_system_constructor2_t m, unsigned n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinMethod(c, "constructor", 0, 0, n_num_params, n_typeList, n_defaultArgList), system_constructor(m) {
   }

   DLLLOCAL void eval(const QoreClass &thisclass, QoreObject *self, int code, va_list args) const {
      system_constructor(thisclass, self, code, args);
   }
};

class BuiltinDestructor : public BuiltinMethod {
protected:
   q_destructor_t destructor;

public:
   DLLLOCAL BuiltinDestructor(QoreClass *c, q_destructor_t m) : BuiltinMethod(c, "destructor", 0, 0), destructor(m) {
   }
   DLLLOCAL void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink *xsink) const;
   DLLLOCAL void evalSystem(QoreObject *self, AbstractPrivateData *private_data, ExceptionSink *xsink) const {
      destructor(self, private_data, xsink);
   }
};

class BuiltinDestructor2 : public BuiltinMethod {
protected:
   q_destructor2_t destructor;

public:
   DLLLOCAL BuiltinDestructor2(QoreClass *c, q_destructor2_t m) : BuiltinMethod(c, "destructor", 0, 0), destructor(m) {
   }
   DLLLOCAL void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink *xsink) const;
   DLLLOCAL void evalSystem(const QoreClass &thisclass, QoreObject *self, AbstractPrivateData *private_data, ExceptionSink *xsink) const {
      destructor(thisclass, self, private_data, xsink);
   }
};

class BuiltinCopyBase : public BuiltinMethod {
public:
   DLLLOCAL BuiltinCopyBase(QoreClass *c, const char *n_name, int n_functionality, const QoreTypeInfo *n_returnTypeInfo) : BuiltinMethod(c, n_name, n_functionality, n_returnTypeInfo) {} 
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink) const = 0;
   DLLLOCAL virtual void evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, ExceptionSink *xsink) const {      
      old->evalCopyMethodWithPrivateData(thisclass, this, self, xsink);
   }
};

class BuiltinCopy : public BuiltinCopyBase {
protected:
   q_copy_t copy;

public:
   DLLLOCAL BuiltinCopy(QoreClass *c, q_copy_t m) : BuiltinCopyBase(c, "copy", 0, 0), copy(m) {
   }
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink) const;
};

class BuiltinCopy2 : public BuiltinCopyBase {
protected:
   q_copy2_t copy;

public:
   DLLLOCAL BuiltinCopy2(QoreClass *c, q_copy2_t m) : BuiltinCopyBase(c, "copy", 0, c->getTypeInfo()), copy(m) {
   }
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink) const;
};

class BuiltinNormalMethodBase : public BuiltinMethod {
public:
   DLLLOCAL BuiltinNormalMethodBase(QoreClass *c, const char *n_name, int n_functionality, const QoreTypeInfo *returnTypeInfo, unsigned n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinMethod(c, n_name, n_functionality, returnTypeInfo, n_num_params, n_typeList, n_defaultArgList) {}

   DLLLOCAL virtual AbstractQoreNode *evalNormalMethod(const QoreMethod &method, QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(const QoreMethod &method, QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const = 0;
};

// for non-static methods
class BuiltinNormalMethod : public BuiltinNormalMethodBase {
protected:
   q_method_t method;

public:
   DLLLOCAL BuiltinNormalMethod(QoreClass *c, const char *n_name, q_method_t m, int n_functionality, const QoreTypeInfo *returnTypeInfo, unsigned n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinNormalMethodBase(c, n_name, n_functionality, returnTypeInfo, n_num_params, n_typeList, n_defaultArgList), method(m) {
   }
   
   DLLLOCAL virtual AbstractQoreNode *evalImpl(const QoreMethod &method, QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const;
};

// for non-static methods
class BuiltinNormalMethod2 : public BuiltinNormalMethodBase {
protected:
   q_method2_t method;

public:
   DLLLOCAL BuiltinNormalMethod2(QoreClass *c, const char *n_name, q_method2_t m, int n_functionality, const QoreTypeInfo *returnTypeInfo, int n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinNormalMethodBase(c, n_name, n_functionality, returnTypeInfo, n_num_params, n_typeList, n_defaultArgList), method(m) {
   }
   
   DLLLOCAL virtual AbstractQoreNode *evalImpl(const QoreMethod &method, QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const;
};

// for static methods
class BuiltinStaticMethod : public BuiltinMethod {
protected:
   q_func_t static_method;

public:
   DLLLOCAL BuiltinStaticMethod(QoreClass *c, const char *n_name, q_func_t m, int n_functionality, const QoreTypeInfo *returnTypeInfo, int n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinMethod(c, n_name, n_functionality, returnTypeInfo, n_num_params, n_typeList, n_defaultArgList), static_method(m) {
   }
   
   DLLLOCAL AbstractQoreNode *evalStaticMethod(const QoreMethod &method, const QoreListNode *args, ExceptionSink *xsink) const;
};

class BuiltinStaticMethod2 : public BuiltinMethod {
protected:
   q_static_method2_t static_method;

public:
   DLLLOCAL BuiltinStaticMethod2(QoreClass *c, const char *n_name, q_static_method2_t m, int n_functionality, const QoreTypeInfo *returnTypeInfo, int n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinMethod(c, n_name, n_functionality, returnTypeInfo, n_num_params, n_typeList, n_defaultArgList), static_method(m) {
   }
   
   DLLLOCAL AbstractQoreNode *evalStaticMethod(const QoreMethod &method, const QoreListNode *args, ExceptionSink *xsink) const;
};

class BuiltinDeleteBlocker : public BuiltinMethod {
protected:
   q_delete_blocker_t delete_blocker;

public:
   DLLLOCAL BuiltinDeleteBlocker(QoreClass *c, q_delete_blocker_t m) : BuiltinMethod(c, "<delete_blocker>", 0, 0), delete_blocker(m) {
   }
   
   DLLLOCAL bool eval(QoreObject *self, AbstractPrivateData *private_data) const {
      return delete_blocker(self, private_data);
   }
};

/*
struct BuiltinFunctionVariant {
   q_func_t func;
};

struct BuiltinFunctionVariantElement : public BuiltinFunctionVariant {
   BuiltinFunctionVariant *next;
};
*/

class BuiltinFunction : public BuiltinFunctionBase {
protected:
   // variant list type
   typedef safe_dslist<q_func_t> flist_t;

   // default function without argument information
   q_func_t func;
   // variant list
   flist_t vlist;

public:
   DLLLOCAL BuiltinFunction(const char *nme, q_func_t f, int typ, const QoreTypeInfo *n_returnTypeInfo = 0, int n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinFunctionBase(nme, typ, n_returnTypeInfo, n_num_params, n_typeList, n_defaultArgList), func(f) {
   }
   DLLLOCAL AbstractQoreNode *evalFunction(const QoreListNode *args, ExceptionSink *xsink) const;
   DLLLOCAL virtual void ref() {
      assert(false);
   }
   DLLLOCAL virtual void deref() {
      assert(false);
   }
};

#endif // _QORE_BUILTIN_FUNCTION

