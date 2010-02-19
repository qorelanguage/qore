/* -*- mode: c++; indent-tabs-mode: nil -*- */
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

#include <vector>

class BCList;
class BCEAList;

class BuiltinSignature : public AbstractFunctionSignature {
public:
   DLLLOCAL BuiltinSignature(const QoreTypeInfo *n_returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) : AbstractFunctionSignature(n_returnTypeInfo, n_typeList, n_defaultArgList) {
      for (unsigned i = 0; i < typeList.size(); ++i) {
	 if (typeList[i])
	    ++num_param_types;

	 typeList[i]->concatName(str);

	 // add a comma to the signature string if it's not the last parameter
	 if (i != (typeList.size() - 1))
	    str.append(", ");
      }
   }
   DLLLOCAL virtual ~BuiltinSignature() {
   }
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const {
      return returnTypeInfo;
   }
};

// the following defines the pure virtual functions that are common to all builtin variants
#define COMMON_BUILTIN_VARIANT_FUNCTIONS DLLLOCAL virtual qore_call_t getCallType() const { return CT_BUILTIN; } \
   DLLLOCAL virtual int getFunctionality() const { return functionality; } \
   DLLLOCAL virtual AbstractFunctionSignature *getSignature() const { return const_cast<BuiltinSignature *>(&signature); } \
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const { return signature.getReturnTypeInfo(); } \
   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const { return signature.getReturnTypeInfo(); } \
   DLLLOCAL virtual UserVariantBase *getUserVariantBase() { return 0; }

class BuiltinFunctionVariantBase {
public:
   BuiltinSignature signature;
   // functionality bitmap for parse restrictions
   int functionality;

   DLLLOCAL BuiltinFunctionVariantBase(int n_functionality = QDOM_DEFAULT, const QoreTypeInfo *n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) :
      signature(n_returnTypeInfo, n_typeList, n_defaultArgList), functionality(n_functionality) {
   }
};

class BuiltinFunctionVariant : public AbstractQoreFunctionVariant, public BuiltinFunctionVariantBase {
protected:
   q_func_t func;

public:
   DLLLOCAL BuiltinFunctionVariant(q_func_t m, int n_functionality, const QoreTypeInfo *n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinFunctionVariantBase(n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList), func(m) {
   }

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS

   DLLLOCAL AbstractQoreNode *evalFunction(const char *name, const QoreListNode *args, ExceptionSink *xsink) const {
      CodeContextHelper cch(name, 0, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack in debugging mode
      CallStackHelper csh(name, CT_BUILTIN, 0, xsink);
#endif

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

