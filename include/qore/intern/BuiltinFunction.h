/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  BuiltinFunction.h

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

#ifndef _QORE_BUILTINFUNCTION_H

#define _QORE_BUILTINFUNCTION_H

#include <vector>
#include <string>

class BCList;
class BCEAList;

class BuiltinSignature : public AbstractFunctionSignature {
public:
   DLLLOCAL BuiltinSignature(const QoreTypeInfo *n_returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) : AbstractFunctionSignature(n_returnTypeInfo, n_typeList, n_defaultArgList) {
      for (unsigned i = 0; i < typeList.size(); ++i) {
         bool hasDefaultArg = i < defaultArgList.size() && defaultArgList[i];
	 if (typeList[i]) {
	    ++num_param_types;
            if (!hasDefaultArg)
               ++min_param_types;
         }

	 typeList[i]->concatName(str);

         if (hasDefaultArg)
            addDefaultArgument(defaultArgList[i]);

	 // add a comma to the signature string if it's not the last parameter
	 if (i != (typeList.size() - 1))
	    str.append(", ");
      }
      if (!typeList.size())
         str = NO_TYPE_INFO;
   }
   DLLLOCAL virtual ~BuiltinSignature() {
   }
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const {
      return returnTypeInfo;
   }
   DLLLOCAL virtual const char *getName(unsigned i) const {
      return 0;
   }
};

// the following defines the pure virtual functions that are common to all builtin variants
#define COMMON_BUILTIN_VARIANT_FUNCTIONS DLLLOCAL virtual qore_call_t getCallType() const { return CT_BUILTIN; } \
   DLLLOCAL virtual int64 getFlags() const { return flags; } \
   DLLLOCAL virtual int64 getFunctionality() const { return functionality; } \
   DLLLOCAL virtual AbstractFunctionSignature *getSignature() const { return const_cast<BuiltinSignature *>(&signature); } \
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const { return signature.getReturnTypeInfo(); } \
   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const { return signature.getReturnTypeInfo(); } \
   using AbstractQoreFunctionVariant::getUserVariantBase; \
   DLLLOCAL virtual UserVariantBase *getUserVariantBase() { return 0; } \
   DLLLOCAL virtual bool isUser() const { return false; }

class BuiltinFunctionVariantBase {
public:
   BuiltinSignature signature;
   // code flags for function information
   int64 flags;
   // functionality bitmap for parse restrictions
   int64 functionality;

   DLLLOCAL BuiltinFunctionVariantBase(int64 n_flags = QC_NO_FLAGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo *n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) :
      signature(n_returnTypeInfo, n_typeList, n_defaultArgList), flags(n_flags), functionality(n_functionality) {
      //printd(0, "BuiltinFunctionVariantBase::BuiltinFunctionVariantBase() this=%p flags=%lld (%lld) functionality=%lld\n", this, flags, n_flags, functionality);
   }
};

class BuiltinFunctionVariant : public AbstractQoreFunctionVariant, public BuiltinFunctionVariantBase {
protected:
   q_func_t func;

public:
   DLLLOCAL BuiltinFunctionVariant(q_func_t m, int64 n_flags, int64 n_functionality, const QoreTypeInfo *n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinFunctionVariantBase(n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList), func(m) {
   }

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS

   DLLLOCAL virtual AbstractQoreNode *evalFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      CodeContextHelper cch(name, 0, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack in debugging mode
      CallStackHelper csh(name, CT_BUILTIN, 0, xsink);
#endif

      return func(ceh.getArgs(), xsink);
   }
};

template <typename B, typename F, class Q>
class BuiltinFunctionTypeVariant : public AbstractQoreFunctionVariant, public BuiltinFunctionVariantBase {
protected:
   F func;

public:
   DLLLOCAL BuiltinFunctionTypeVariant(F m, int64 n_flags, int64 n_functionality, const QoreTypeInfo *n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinFunctionVariantBase(n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList), func(m) {
   }

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS

   DLLLOCAL virtual AbstractQoreNode *evalFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      CodeContextHelper cch(name, 0, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack in debugging mode
      CallStackHelper csh(name, CT_BUILTIN, 0, xsink);
#endif

      B rv = func(ceh.getArgs(), xsink);
      return *xsink ? 0 : new Q(rv);
   }

   DLLLOCAL B evalNativeFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      CodeContextHelper cch(name, 0, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack in debugging mode
      CallStackHelper csh(name, CT_BUILTIN, 0, xsink);
#endif

      return func(ceh.getArgs(), xsink);
   }

   DLLLOCAL virtual int64 bigIntEvalFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      return (int64)evalNativeFunction(name, ceh, xsink);
   }

   DLLLOCAL virtual int intEvalFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      return (int)evalNativeFunction(name, ceh, xsink);
   }

   DLLLOCAL virtual bool boolEvalFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      return (bool)evalNativeFunction(name, ceh, xsink);
   }

   DLLLOCAL virtual double floatEvalFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      return (double)evalNativeFunction(name, ceh, xsink);
   }
};

typedef BuiltinFunctionTypeVariant<int64, q_func_int64_t, QoreBigIntNode> BuiltinFunctionBigIntVariant;
//typedef BuiltinFunctionTypeVariant<int, q_func_int_t, QoreBigIntNode> BuiltinFunctionIntVariant;
typedef BuiltinFunctionTypeVariant<double, q_func_double_t, QoreFloatNode> BuiltinFunctionFloatVariant;

class BuiltinFunctionBoolVariant : public AbstractQoreFunctionVariant, public BuiltinFunctionVariantBase {
protected:
   q_func_bool_t func;

public:
   DLLLOCAL BuiltinFunctionBoolVariant(q_func_bool_t m, int64 n_flags, int64 n_functionality, const QoreTypeInfo *n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinFunctionVariantBase(n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList), func(m) {
   }

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS

   DLLLOCAL virtual AbstractQoreNode *evalFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      CodeContextHelper cch(name, 0, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack in debugging mode
      CallStackHelper csh(name, CT_BUILTIN, 0, xsink);
#endif

      bool rv = func(ceh.getArgs(), xsink);
      return *xsink ? 0 : get_bool_node(rv);
   }

   DLLLOCAL bool evalNativeFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      CodeContextHelper cch(name, 0, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack in debugging mode
      CallStackHelper csh(name, CT_BUILTIN, 0, xsink);
#endif

      return func(ceh.getArgs(), xsink);
   }

   DLLLOCAL virtual int64 bigIntEvalFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      return (int64)evalNativeFunction(name, ceh, xsink);
   }

   DLLLOCAL virtual int intEvalFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      return (int)evalNativeFunction(name, ceh, xsink);
   }

   DLLLOCAL virtual bool boolEvalFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      return (bool)evalNativeFunction(name, ceh, xsink);
   }

   DLLLOCAL virtual double floatEvalFunction(const char *name, CodeEvaluationHelper &ceh, ExceptionSink *xsink) const {
      return (double)evalNativeFunction(name, ceh, xsink);
   }
};

class BuiltinFunctionBase {
protected:
   std::string name;

public:
   DLLLOCAL BuiltinFunctionBase(const char *n_name) : name(n_name) {
   }

   DLLLOCAL BuiltinFunctionBase(const BuiltinFunctionBase &old) : name(old.name) {
   }
};

class BuiltinFunction : public AbstractQoreFunction, public BuiltinFunctionBase {
protected:
public:
   DLLLOCAL BuiltinFunction(const char *nme) : BuiltinFunctionBase(nme) {
   }
   DLLLOCAL BuiltinFunction(const BuiltinFunction &old) : BuiltinFunctionBase(old) {
   }
   DLLLOCAL virtual const char *getName() const { 
      return name.c_str();
   }
   DLLLOCAL virtual void ref() {
      assert(false);
   }
   DLLLOCAL virtual void deref() {
      assert(false);
   }
   DLLLOCAL virtual const QoreClass *getClass() const {
      return 0;
   }

   DLLLOCAL virtual void parseInit() {
      // FIXME
      assert(pendingEmpty());
   }
};

#endif // _QORE_BUILTIN_FUNCTION
