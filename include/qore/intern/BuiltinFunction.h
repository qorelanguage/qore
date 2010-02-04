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
   const AbstractQoreNode **defaultArgList;

   DLLLOCAL BuiltinSignature(const QoreTypeInfo *n_returnTypeInfo, unsigned n_num_params, const QoreTypeInfo **n_typeList, const AbstractQoreNode **n_defaultArgList) : AbstractFunctionSignature(n_num_params), typeList(n_typeList), returnTypeInfo(n_returnTypeInfo), defaultArgList(n_defaultArgList) {
      for (unsigned i = 0; i < num_params; ++i) {
	 if (typeList[i])
	    ++num_param_types;
      }
   }
   DLLLOCAL virtual ~BuiltinSignature() {
      delete [] typeList;
      delete [] defaultArgList;
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
   DLLLOCAL virtual const AbstractQoreNode **getDefaultArgList() const {
      return defaultArgList;
   }
};

// the following defines the pure virtual functions that are common to all builtin variants
#define COMMON_BUILTIN_VARIANT_FUNCTIONS DLLLOCAL virtual qore_call_t getCallType() const { return CT_BUILTIN; } \
   DLLLOCAL virtual int getFunctionality() const { return functionality; } \
   DLLLOCAL virtual AbstractFunctionSignature *getSignature() const { return const_cast<BuiltinSignature *>(&signature); } \
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const { return signature.returnTypeInfo; } \
   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const { return signature.returnTypeInfo; } \
   DLLLOCAL virtual UserVariantBase *getUserVariantBase() { return 0; }

class BuiltinFunctionVariantBase {
public:
   BuiltinSignature signature;
   // functionality bitmap for parse restrictions
   int functionality;

   DLLLOCAL BuiltinFunctionVariantBase(int n_functionality = QDOM_DEFAULT, const QoreTypeInfo *n_returnTypeInfo = 0, unsigned n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) :
      signature(n_returnTypeInfo, n_num_params, n_typeList, n_defaultArgList), functionality(n_functionality) {
   }
};

class BuiltinFunctionVariant : public AbstractQoreFunctionVariant, public BuiltinFunctionVariantBase {
protected:
   q_func_t func;

public:
   DLLLOCAL BuiltinFunctionVariant(q_func_t m, int n_functionality, const QoreTypeInfo *n_returnTypeInfo = 0, unsigned n_num_params = 0, const QoreTypeInfo **n_typeList = 0, const AbstractQoreNode **n_defaultArgList = 0) : BuiltinFunctionVariantBase(n_functionality, n_returnTypeInfo, n_num_params, n_typeList, n_defaultArgList), func(m) {
   }

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS

   DLLLOCAL AbstractQoreNode *evalFunction(const QoreListNode *args, ExceptionSink *xsink) const {
      return func(args, xsink);
   }
};

class BuiltinFunctionBase {
protected:
   const char *name;

public:
   DLLLOCAL BuiltinFunctionBase(const char *n_name) : name(n_name) {
   }
};

class BuiltinFunction : public AbstractQoreFunction, public BuiltinFunctionBase {
protected:
public:
   DLLLOCAL BuiltinFunction(const char *nme) : BuiltinFunctionBase(nme) {
   }
   DLLLOCAL virtual const char *getName() const { 
      return name;
   }
   DLLLOCAL virtual void ref() {
      assert(false);
   }
   DLLLOCAL virtual void deref() {
      assert(false);
   }
};

#endif // _QORE_BUILTIN_FUNCTION

