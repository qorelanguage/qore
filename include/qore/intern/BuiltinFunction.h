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

class BuiltinFunctionBase {
protected:
   const char *name;
   // functionality bitmap for parse restrictions
   int functionality;
   const QoreTypeInfo *returnTypeInfo;
   unsigned num_params;
   const QoreTypeInfo **typeList;
   const AbstractQoreNode **defaultArgList;

public:
   DLLLOCAL BuiltinFunctionBase(const char *n_name, int n_functionality, const QoreTypeInfo *n_returnTypeInfo, unsigned n_num_params, const QoreTypeInfo **n_typeList, const AbstractQoreNode **n_defaultArgList) : name(n_name), functionality(n_functionality), returnTypeInfo(n_returnTypeInfo), num_params(n_num_params), typeList(n_typeList), defaultArgList(n_defaultArgList) {}
   DLLLOCAL BuiltinFunctionBase() : name(0), returnTypeInfo(0), num_params(0), typeList(0), defaultArgList(0) {
      assert(false);
   }
   DLLLOCAL ~BuiltinFunctionBase() {
      delete [] typeList;
      delete [] defaultArgList;
   }
   DLLLOCAL int getType() const {
      return functionality;
   }
   DLLLOCAL const char *getName() const { 
      return name;
   }
   DLLLOCAL const QoreTypeInfo *getReturnTypeInfo() const {
      return returnTypeInfo;
   }
};

class BuiltinMethod : public BuiltinFunctionBase, public QoreReferenceCounter {
public:
   QoreClass *myclass;
   DLLLOCAL BuiltinMethod(QoreClass *n_myclass, const char *n_name, int n_functionality, const QoreTypeInfo *n_returnTypeInfo, unsigned n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinFunctionBase(n_name, n_functionality, n_returnTypeInfo, n_num_params, n_typeList, n_defaultArgList), myclass(n_myclass) {
   }
   DLLLOCAL void deref() {
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
   DLLLOCAL BuiltinConstructor(QoreClass *c, q_constructor_t m) : BuiltinMethod(c, "constructor", 0, 0), constructor(m) {
   }
   
   DLLLOCAL void eval(QoreObject *self, const QoreListNode *args, BCList *bcl, BCEAList *bceal, const char *class_name, ExceptionSink *xsink) const;
};

class BuiltinConstructor2 : public BuiltinMethod {
protected:
   q_constructor2_t constructor;

public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   // also with functionality - enforced by the new operator
   DLLLOCAL BuiltinConstructor2(QoreClass *c, q_constructor2_t m) : BuiltinMethod(c, "constructor", 0, 0), constructor(m) {
   }
   
   DLLLOCAL void eval(const QoreClass &thisclass, QoreObject *self, const QoreListNode *args, BCList *bcl, BCEAList *bceal, const char *class_name, ExceptionSink *xsink) const;
};

class BuiltinSystemConstructor : public BuiltinMethod {
protected:
   q_system_constructor_t system_constructor;

public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   // also with functionality - enforced by the new operator
   DLLLOCAL BuiltinSystemConstructor(QoreClass *c, q_system_constructor_t m) : BuiltinMethod(c, "constructor", 0, 0), system_constructor(m) {
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
   DLLLOCAL BuiltinSystemConstructor2(QoreClass *c, q_system_constructor2_t m) : BuiltinMethod(c, "constructor", 0, 0), system_constructor(m) {
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
   DLLLOCAL void eval(QoreObject *self, AbstractPrivateData *private_data, const char *class_name, ExceptionSink *xsink) const;
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
   DLLLOCAL void eval(const QoreClass &thisclass, QoreObject *self, AbstractPrivateData *private_data, const char *class_name, ExceptionSink *xsink) const;
   DLLLOCAL void evalSystem(const QoreClass &thisclass, QoreObject *self, AbstractPrivateData *private_data, ExceptionSink *xsink) const {
      destructor(thisclass, self, private_data, xsink);
   }
};

class BuiltinCopy : public BuiltinMethod {
protected:
   q_copy_t copy;

public:
   DLLLOCAL BuiltinCopy(QoreClass *c, q_copy_t m) : BuiltinMethod(c, "copy", 0, 0), copy(m) {
   }
   DLLLOCAL void eval(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink) const;
};

class BuiltinCopy2 : public BuiltinMethod {
protected:
   q_copy2_t copy;

public:
   DLLLOCAL BuiltinCopy2(QoreClass *c, q_copy2_t m) : BuiltinMethod(c, "copy", 0, 0), copy(m) {
   }
   DLLLOCAL void eval(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink) const;
};

// for non-static methods
class BuiltinNormalMethod : public BuiltinMethod {
protected:
   q_method_t method;

public:
   DLLLOCAL BuiltinNormalMethod(QoreClass *c, const char *n_name, q_method_t m, const QoreTypeInfo *returnTypeInfo, int n_functionality) : BuiltinMethod(c, n_name, n_functionality, returnTypeInfo), method(m) {
   }
   
   DLLLOCAL AbstractQoreNode *eval(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const;
};

// for non-static methods
class BuiltinNormalMethod2 : public BuiltinMethod {
protected:
   q_method2_t method;

public:
   DLLLOCAL BuiltinNormalMethod2(QoreClass *c, const char *n_name, q_method2_t m, const QoreTypeInfo *returnTypeInfo, int n_functionality) : BuiltinMethod(c, n_name, n_functionality, returnTypeInfo), method(m) {
   }
   
   DLLLOCAL AbstractQoreNode *eval(const QoreMethod &method, QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const;
};

// for static methods
class BuiltinStaticMethod : public BuiltinMethod {
protected:
   q_func_t static_method;

public:
   DLLLOCAL BuiltinStaticMethod(QoreClass *c, const char *n_name, q_func_t m, const QoreTypeInfo *returnTypeInfo, int n_functionality) : BuiltinMethod(c, n_name, n_functionality, returnTypeInfo), static_method(m) {
   }
   
   DLLLOCAL AbstractQoreNode *eval(const QoreMethod &method, const QoreListNode *args, ExceptionSink *xsink) const;
};

class BuiltinStaticMethod2 : public BuiltinMethod {
protected:
   q_static_method2_t static_method;

public:
   DLLLOCAL BuiltinStaticMethod2(QoreClass *c, const char *n_name, q_static_method2_t m, const QoreTypeInfo *returnTypeInfo, int n_functionality) : BuiltinMethod(c, n_name, n_functionality, returnTypeInfo), static_method(m) {
   }
   
   DLLLOCAL AbstractQoreNode *eval(const QoreMethod &method, const QoreListNode *args, ExceptionSink *xsink) const;
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

class BuiltinFunction : public BuiltinFunctionBase {
public:
   q_func_t func;

   DLLLOCAL BuiltinFunction(const char *nme, q_func_t f, int typ, const QoreTypeInfo *n_returnTypeInfo = 0, unsigned n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinFunctionBase(nme, typ, n_returnTypeInfo, n_num_params, n_typeList, n_defaultArgList), func(f) {
   }
   DLLLOCAL AbstractQoreNode *eval(const QoreListNode *args, ExceptionSink *xsink) const;
};

#endif // _QORE_BUILTIN_FUNCTION

