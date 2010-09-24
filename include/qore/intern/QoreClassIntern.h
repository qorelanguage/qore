/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreClassIntern.h

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

#ifndef _QORE_QORECLASSINTERN_H

#define _QORE_QORECLASSINTERN_H

#include <qore/safe_dslist>

#include <list>
#include <map>
#include <string>

#define OTF_USER    CT_USER
#define OTF_BUILTIN CT_BUILTIN

// forward reference for base class (constructor) argument list
class BCAList;
// forward reference for base class list
class BCList;
// forward reference for base class (constructor) evaluated argument list
class BCEAList;

class MethodVariantBase : public AbstractQoreFunctionVariant {
protected:
   // is the method private or not
   bool priv_flag;
   // pointer to method that owns the variant
   const QoreMethod *qmethod;

public:
   DLLLOCAL MethodVariantBase(bool n_priv_flag) : priv_flag(n_priv_flag), qmethod(0) {
   }
   DLLLOCAL bool isPrivate() const {
      return priv_flag;
   }
   DLLLOCAL void setMethod(QoreMethod *n_qm) {
      qmethod = n_qm;
   }
   DLLLOCAL const QoreMethod *method() const {
      assert(qmethod);
      return qmethod;
   }
   DLLLOCAL const char *className() const {
      return qmethod->getClassName();
   }
   DLLLOCAL const QoreClass *getClass() const {
      return qmethod->getClass();
   }
};

#define METHVB(f) (reinterpret_cast<MethodVariantBase *>(f))
#define METHVB_const(f) (reinterpret_cast<const MethodVariantBase *>(f))

class MethodVariant : public MethodVariantBase {
public:
   DLLLOCAL MethodVariant(bool n_priv_flag) : MethodVariantBase(n_priv_flag) {
   }
   DLLLOCAL virtual AbstractQoreNode *evalMethod(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const = 0;
};

#define METHV(f) (reinterpret_cast<MethodVariant *>(f))
#define METHV_const(f) (reinterpret_cast<const MethodVariant *>(f))

class ConstructorMethodVariant : public MethodVariantBase {
protected:
   // evaluates base class constructors and initializes members
   DLLLOCAL int constructorPrelude(const QoreClass &thisclass, CodeEvaluationHelper &ceh, QoreObject *self, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const;

public:
   DLLLOCAL ConstructorMethodVariant(bool n_priv_flag) : MethodVariantBase(n_priv_flag) {
   }
   DLLLOCAL virtual const BCAList *getBaseClassArgumentList() const = 0;
   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject *self, CodeEvaluationHelper &ceh, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const = 0;
};

#define CONMV(f) (reinterpret_cast<ConstructorMethodVariant *>(f))
#define CONMV_const(f) (reinterpret_cast<const ConstructorMethodVariant *>(f))

class DestructorMethodVariant : public MethodVariantBase {
protected:
public:
   DLLLOCAL DestructorMethodVariant() : MethodVariantBase(false) {
   }
   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink *xsink) const = 0;
};

#define DESMV(f) (reinterpret_cast<DestructorMethodVariant *>(f))
#define DESMV_const(f) (reinterpret_cast<const DestructorMethodVariant *>(f))

class CopyMethodVariant : public MethodVariantBase {
protected:
public:
   DLLLOCAL CopyMethodVariant(bool n_priv_flag) : MethodVariantBase(n_priv_flag) {
   }
   DLLLOCAL virtual void evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, CodeEvaluationHelper &ceh, BCList *scl, ExceptionSink *xsink) const = 0;
};

#define COPYMV(f) (reinterpret_cast<CopyMethodVariant *>(f))
#define COPYMV_const(f) (reinterpret_cast<const CopyMethodVariant *>(f))

class UserMethodVariant : public MethodVariant, public UserVariantBase {
public:
   DLLLOCAL UserMethodVariant(bool n_priv_flag, StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, RetTypeInfo *rv, bool synced, int64 n_flags) : MethodVariant(n_priv_flag), UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, rv, synced, n_flags) {
   }

   // the following defines the pure virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL void parseInitMethod(const QoreClass &parent_class, bool static_flag) {
      signature.resolve();
      // resolve and push current return type on stack
      ParseCodeInfoHelper rtih(qmethod->getName(), signature.getReturnTypeInfo());
      
      // must be called even if statements is NULL
      if (!static_flag)
	 statements->parseInitMethod(parent_class.getTypeInfo(), this);
      else
	 statements->parseInit(this);
   }
   DLLLOCAL virtual AbstractQoreNode *evalMethod(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const {
      return eval(qmethod->getName(), args, self, xsink, qmethod->getClass()->getName());
   }
};

#define UMV(f) (reinterpret_cast<UserMethodVariant *>(f))
#define UMV_const(f) (reinterpret_cast<const UserMethodVariant *>(f))

class UserConstructorVariant : public ConstructorMethodVariant, public UserVariantBase {
protected:
   // base class argument list for constructors
   BCAList *bcal;

   DLLLOCAL virtual ~UserConstructorVariant();

public:
      DLLLOCAL UserConstructorVariant(bool n_priv_flag, StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, BCAList *n_bcal, int64 n_flags = QC_NO_FLAGS) : ConstructorMethodVariant(n_priv_flag), UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, 0, false, n_flags), bcal(n_bcal) {
   }

   // the following defines the pure virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL virtual const BCAList *getBaseClassArgumentList() const {
      return bcal;
   }

   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject *self, CodeEvaluationHelper &ceh, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const {
      UserVariantExecHelper uveh(this, ceh.getArgs(), xsink);
      if (!uveh)
	 return;

      CodeContextHelper cch("constructor", self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack in debugging mode
      CallStackHelper csh("constructor", CT_USER, self, xsink);
#endif

      if (constructorPrelude(thisclass, ceh, self, bcl, bceal, xsink))
	 return;

      discard(evalIntern(uveh.getArgv(), self, xsink, thisclass.getName()), xsink);
   }

   DLLLOCAL void parseInitConstructor(const QoreClass &parent_class, BCList *bcl);
};

#define UCONV(f) (reinterpret_cast<UserConstructorVariant *>(f))
#define UCONV_const(f) (reinterpret_cast<const UserConstructorVariant *>(f))

class UserDestructorVariant : public DestructorMethodVariant, public UserVariantBase {
protected:
public:
   DLLLOCAL UserDestructorVariant(StatementBlock *b, int n_sig_first_line, int n_sig_last_line) : DestructorMethodVariant(), UserVariantBase(b, n_sig_first_line, n_sig_last_line, 0, 0, false) {
   }

   // the following defines the pure virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL void parseInitDestructor(const QoreClass &parent_class) {
      signature.resolve();
      assert(!signature.getReturnTypeInfo() || signature.getReturnTypeInfo() == nothingTypeInfo);

      // push return type on stack (no return value can be used)
      ParseCodeInfoHelper rtih("destructor", nothingTypeInfo);

      // must be called even if statements is NULL
      statements->parseInitMethod(parent_class.getTypeInfo(), this);
   }

   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink *xsink) const {
      // there cannot be any params
      assert(!signature.numParams());
      assert(!signature.getReturnTypeInfo() || signature.getReturnTypeInfo() == nothingTypeInfo);
      discard(eval("destructor", 0, self, xsink, thisclass.getName()), xsink);
   }
};

#define UDESV(f) (reinterpret_cast<UserDestructorVariant *>(f))
#define UDESV_const(f) (reinterpret_cast<const UserDestructorVariant *>(f))

class UserCopyVariant : public CopyMethodVariant, public UserVariantBase {
protected:
public:
   DLLLOCAL UserCopyVariant(bool n_priv_flag, StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, RetTypeInfo *rv, bool synced) : CopyMethodVariant(n_priv_flag), UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, rv, synced) {
   }

   // the following defines the pure virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL void parseInitCopy(const QoreClass &parent_class);
   DLLLOCAL virtual void evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, CodeEvaluationHelper &ceh, BCList *scl, ExceptionSink *xsink) const;
};

#define UCOPYV(f) (reinterpret_cast<UserCopyVariant *>(f))

class BuiltinMethodVariant : public MethodVariant, public BuiltinFunctionVariantBase {
public:
   DLLLOCAL BuiltinMethodVariant(bool n_priv_flag, int64 n_flags, int64 n_functionality, const QoreTypeInfo *n_returnTypeInfo, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : MethodVariant(n_priv_flag), BuiltinFunctionVariantBase(n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList) {}

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS
};

class BuiltinNormalMethodVariantBase : public BuiltinMethodVariant {
public:
   DLLLOCAL BuiltinNormalMethodVariantBase(bool n_priv_flag, int64 n_flags,  int64 n_functionality, const QoreTypeInfo *n_returnTypeInfo, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinMethodVariant(n_priv_flag, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList) {}

   DLLLOCAL virtual AbstractQoreNode *evalMethod(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), self, xsink);

      return self->evalBuiltinMethodWithPrivateData(*qmethod, this, args, xsink);
   }
   DLLLOCAL virtual AbstractQoreNode *evalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const = 0;
};

class BuiltinNormalMethodVariant : public BuiltinNormalMethodVariantBase {
protected:
   q_method_t method;

public:
   DLLLOCAL BuiltinNormalMethodVariant(q_method_t m, bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo *n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinNormalMethodVariantBase(n_priv_flag, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList), method(m) {
   }
   DLLLOCAL virtual AbstractQoreNode *evalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const {
      return method(self, private_data, args, xsink);
   }
};

class BuiltinNormalMethod2Variant : public BuiltinNormalMethodVariantBase {
protected:
   q_method2_t method;

public:
   DLLLOCAL BuiltinNormalMethod2Variant(q_method2_t m, bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo *n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinNormalMethodVariantBase(n_priv_flag, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList), method(m) {
   }
   DLLLOCAL virtual AbstractQoreNode *evalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const {
      return method(*qmethod, self, private_data, args, xsink);
   }
};

class BuiltinNormalMethod3Variant : public BuiltinNormalMethodVariantBase {
protected:
   q_method3_t method;
   const void *ptr;

public:
   DLLLOCAL BuiltinNormalMethod3Variant(const void *n_ptr, q_method3_t m, bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo *n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinNormalMethodVariantBase(n_priv_flag, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList), method(m), ptr(n_ptr) {
   }
   DLLLOCAL virtual AbstractQoreNode *evalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const {
      return method(*qmethod, signature.getTypeList(), ptr, self, private_data, args, xsink);
   }
};

class BuiltinStaticMethodVariant : public BuiltinMethodVariant {
protected:
   q_func_t static_method;

public:
   DLLLOCAL BuiltinStaticMethodVariant(q_func_t m, bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo *n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinMethodVariant(n_priv_flag, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList), static_method(m) {
   }
   DLLLOCAL virtual AbstractQoreNode *evalMethod(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), 0, xsink);

      return static_method(args, xsink);
   }
};

class BuiltinStaticMethod2Variant : public BuiltinMethodVariant {
protected:
   q_static_method2_t static_method;

public:
   DLLLOCAL BuiltinStaticMethod2Variant(q_static_method2_t m, bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo *n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinMethodVariant(n_priv_flag, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList), static_method(m) {
   }
   DLLLOCAL AbstractQoreNode *evalMethod(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const {      
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), 0, xsink);

      return static_method(*qmethod, args, xsink);
   }
};

class BuiltinStaticMethod3Variant : public BuiltinMethodVariant {
protected:
   q_static_method3_t static_method;
   const void *ptr;

public:
   DLLLOCAL BuiltinStaticMethod3Variant(const void *n_ptr, q_static_method3_t m, bool n_priv_flag, int64 n_flags, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo *n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinMethodVariant(n_priv_flag, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList), static_method(m), ptr(n_ptr) {
   }
   DLLLOCAL AbstractQoreNode *evalMethod(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), 0, xsink);

      return static_method(*qmethod, signature.getTypeList(), ptr, args, xsink);
   }
};

class BuiltinConstructorVariantBase : public ConstructorMethodVariant, public BuiltinFunctionVariantBase {
public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   DLLLOCAL BuiltinConstructorVariantBase(bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : ConstructorMethodVariant(n_priv_flag), BuiltinFunctionVariantBase(n_flags, n_functionality, 0, n_typeList, n_defaultArgList) {
   }

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS

   DLLLOCAL virtual const BCAList *getBaseClassArgumentList() const {
      return 0;
   }
};

class BuiltinConstructorVariant : public BuiltinConstructorVariantBase {
protected:
   q_constructor_t constructor;

public:
   DLLLOCAL BuiltinConstructorVariant(q_constructor_t m, bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinConstructorVariantBase(n_priv_flag, n_flags, n_functionality, n_typeList, n_defaultArgList), constructor(m) {
   }

   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject *self, CodeEvaluationHelper &ceh, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const {
      CodeContextHelper cch("constructor", self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh("constructor", CT_BUILTIN, self, xsink);
#endif

      if (constructorPrelude(thisclass, ceh, self, bcl, bceal, xsink))
	 return;

      constructor(self, ceh.getArgs(), xsink);
   }
};

class BuiltinConstructor2Variant : public BuiltinConstructorVariantBase {
protected:
   q_constructor2_t constructor;

public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   DLLLOCAL BuiltinConstructor2Variant(q_constructor2_t m, bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinConstructorVariantBase(n_priv_flag, n_flags, n_functionality, n_typeList, n_defaultArgList), constructor(m) {
   }
   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject *self, CodeEvaluationHelper &ceh, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const { 
      CodeContextHelper cch("constructor", self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh("constructor", CT_BUILTIN, self, xsink);
#endif

      if (constructorPrelude(thisclass, ceh, self, bcl, bceal, xsink))
	 return;

      constructor(thisclass, self, ceh.getArgs(), xsink);
   }
};

class BuiltinConstructor3Variant : public BuiltinConstructorVariantBase {
protected:
   q_constructor3_t constructor;
   const void *ptr;

public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   DLLLOCAL BuiltinConstructor3Variant(const void *n_ptr, q_constructor3_t m, bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t()) : BuiltinConstructorVariantBase(n_priv_flag, n_flags, n_functionality, n_typeList, n_defaultArgList), constructor(m), ptr(n_ptr) {
   }
   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject *self, CodeEvaluationHelper &ceh, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const { 
      CodeContextHelper cch("constructor", self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh("constructor", CT_BUILTIN, self, xsink);
#endif

      if (constructorPrelude(thisclass, ceh, self, bcl, bceal, xsink))
	 return;

      constructor(thisclass, signature.getTypeList(), ptr, self, ceh.getArgs(), xsink);
   }
};

class BuiltinDestructorVariantBase : public DestructorMethodVariant, public BuiltinFunctionVariantBase {
public:
   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS
};

class BuiltinDestructorVariant : public BuiltinDestructorVariantBase {
protected:
   q_destructor_t destructor;

public:
   DLLLOCAL BuiltinDestructorVariant(q_destructor_t n_destructor) : destructor(n_destructor) {
   }

   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink *xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, "destructor", self, xsink);

      AbstractPrivateData *private_data = self->getAndClearPrivateData(thisclass.getID(), xsink);
      if (!private_data)
	 return;
      destructor(self, private_data, xsink);
   }
};

class BuiltinDestructor2Variant : public BuiltinDestructorVariantBase {
protected:
   q_destructor2_t destructor;

public:
   DLLLOCAL BuiltinDestructor2Variant(q_destructor2_t n_destructor) : destructor(n_destructor) {
   }
   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink *xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, "destructor", self, xsink);

      AbstractPrivateData *private_data = self->getAndClearPrivateData(thisclass.getID(), xsink);
      if (!private_data)
	 return;
      destructor(thisclass, self, private_data, xsink);
   }
};

class BuiltinDestructor3Variant : public BuiltinDestructorVariantBase {
protected:
   q_destructor3_t destructor;
   const void *ptr;

public:
   DLLLOCAL BuiltinDestructor3Variant(const void *n_ptr, q_destructor3_t n_destructor) : destructor(n_destructor), ptr(n_ptr) {
   }
   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink *xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, "destructor", self, xsink);

      AbstractPrivateData *private_data = self->getAndClearPrivateData(thisclass.getID(), xsink);
      if (!private_data)
	 return;
      destructor(thisclass, ptr, self, private_data, xsink);
   }
};

class BuiltinCopyVariantBase : public CopyMethodVariant, public BuiltinFunctionVariantBase {
protected:
public:
   DLLLOCAL BuiltinCopyVariantBase(const QoreClass *c) : CopyMethodVariant(false), BuiltinFunctionVariantBase(QC_NO_FLAGS, QDOM_DEFAULT, c->getTypeInfo()) {
   }

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS

   DLLLOCAL virtual void evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, CodeEvaluationHelper &ceh, BCList *scl, ExceptionSink *xsink) const;
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink) const = 0;
};

class BuiltinCopyVariant : public BuiltinCopyVariantBase {
protected:
   q_copy_t copy;

public:
   DLLLOCAL BuiltinCopyVariant(QoreClass *c, q_copy_t m) : BuiltinCopyVariantBase(c), copy(m) {
   }
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink) const {
      copy(self, old, private_data, xsink);
   }
};

class BuiltinCopy2Variant : public BuiltinCopyVariantBase {
protected:
   q_copy2_t copy;

public:
   DLLLOCAL BuiltinCopy2Variant(QoreClass *c, q_copy2_t m) : BuiltinCopyVariantBase(c), copy(m) {
   }
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink) const {
      copy(thisclass, self, old, private_data, xsink);
   }
};

class BuiltinCopy3Variant : public BuiltinCopyVariantBase {
protected:
   q_copy3_t copy;
   const void *ptr;

public:
   DLLLOCAL BuiltinCopy3Variant(const void *n_ptr, QoreClass *c, q_copy3_t m) : BuiltinCopyVariantBase(c), copy(m), ptr(n_ptr) {
   }
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink) const {
      copy(thisclass, ptr, self, old, private_data, xsink);
   }
};

// abstract class for method functions (static and non-static)
class MethodFunction : public MethodFunctionBase {
public:
   DLLLOCAL MethodFunction(const QoreClass *n_qc) : MethodFunctionBase(n_qc) {
   }
   DLLLOCAL MethodFunction(const MethodFunction &old, const QoreClass *n_qc) : MethodFunctionBase(old, n_qc) {
   }
   DLLLOCAL virtual ~MethodFunction() {
   }
   DLLLOCAL virtual void parseInitMethod(const QoreClass &parent_class, bool static_flag);

   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL AbstractQoreNode *evalNormalMethod(const AbstractQoreFunctionVariant *variant, const char *class_name, QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const;

   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL AbstractQoreNode *evalStaticMethod(const AbstractQoreFunctionVariant *variant, const char *class_name, const QoreListNode *args, ExceptionSink *xsink) const;
};

#define METHF(f) (reinterpret_cast<MethodFunction *>(f))

// abstract class for constructor method functions
class ConstructorMethodFunction : public MethodFunctionBase {
public:
   DLLLOCAL ConstructorMethodFunction(const QoreClass *n_qc) : MethodFunctionBase(n_qc) {
   }
   DLLLOCAL ConstructorMethodFunction(const ConstructorMethodFunction &old, const QoreClass *n_qc) : MethodFunctionBase(old, n_qc) {
   }
   DLLLOCAL void parseInitConstructor(const QoreClass &parent_class, BCList *bcl);
   DLLLOCAL virtual const char *getName() const {
      return "constructor";
   }
   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL void evalConstructor(const AbstractQoreFunctionVariant *variant, const QoreClass &thisclass, QoreObject *self, const QoreListNode *args, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const;

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const {
      return new ConstructorMethodFunction(*this, n_qc);
   }
};

#define CONMF(f) (reinterpret_cast<ConstructorMethodFunction *>(f))

// abstract class for destructor method functions
class DestructorMethodFunction : public MethodFunctionBase {
public:
   DLLLOCAL DestructorMethodFunction(const QoreClass *n_qc) : MethodFunctionBase(n_qc) {
   }
   DLLLOCAL DestructorMethodFunction(const DestructorMethodFunction &old, const QoreClass *n_qc) : MethodFunctionBase(old, n_qc) {
   }
   DLLLOCAL void parseInitDestructor(const QoreClass &parent_class);
   DLLLOCAL virtual const char *getName() const {
      return "destructor";
   }
   DLLLOCAL void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink *xsink) const;

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const {
      return new DestructorMethodFunction(*this, n_qc);
   }
};

#define DESMF(f) (reinterpret_cast<DestructorMethodFunction *>(f))

// abstract class for copy method functions
class CopyMethodFunction : public MethodFunctionBase {
public:
   DLLLOCAL CopyMethodFunction(const QoreClass *n_qc) : MethodFunctionBase(n_qc) {
   }
   DLLLOCAL CopyMethodFunction(const CopyMethodFunction &old, const QoreClass *n_qc) : MethodFunctionBase(old, n_qc) {
   }
   DLLLOCAL void parseInitCopy(const QoreClass &parent_class);
   DLLLOCAL virtual const char *getName() const {
      return "copy";
   }
   DLLLOCAL void evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, BCList *scl, ExceptionSink *xsink) const;

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const {
      return new CopyMethodFunction(*this, n_qc);
   }
};

#define COPYMF(f) (reinterpret_cast<CopyMethodFunction *>(f))

class BuiltinSystemConstructorBase : public MethodFunctionBase {
public:
   DLLLOCAL BuiltinSystemConstructorBase(const QoreClass *n_qc) : MethodFunctionBase(n_qc) {
   }
   DLLLOCAL BuiltinSystemConstructorBase(const BuiltinSystemConstructorBase &old, const QoreClass *n_qc) : MethodFunctionBase(old, n_qc) {
   }
   DLLLOCAL const char *getName() const {
      return "<system_constructor>";
   }
   DLLLOCAL virtual void eval(const QoreClass &thisclass, QoreObject *self, int code, va_list args) const = 0;

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const = 0;
};

#define BSYSCONB(f) (reinterpret_cast<BuiltinSystemConstructorBase *>(f))

// system constructors are not accessed from userspace so we don't need to conform
// to the abstract class structure
class BuiltinSystemConstructor : public BuiltinSystemConstructorBase {
protected:
   q_system_constructor_t system_constructor;

public:
   DLLLOCAL BuiltinSystemConstructor(const QoreClass *n_qc, q_system_constructor_t m) : BuiltinSystemConstructorBase(n_qc), system_constructor(m) {
   }

   DLLLOCAL BuiltinSystemConstructor(const BuiltinSystemConstructor &old, const QoreClass *n_qc) : BuiltinSystemConstructorBase(old, n_qc), system_constructor(old.system_constructor) {
   }

   DLLLOCAL virtual void eval(const QoreClass &thisclass, QoreObject *self, int code, va_list args) const {
      system_constructor(self, code, args);
   }

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const {
      return new BuiltinSystemConstructor(*this, n_qc);
   }
};

class BuiltinSystemConstructor2 : public BuiltinSystemConstructorBase {
protected:
   q_system_constructor2_t system_constructor;

public:
   DLLLOCAL BuiltinSystemConstructor2(const QoreClass *n_qc, q_system_constructor2_t m) : BuiltinSystemConstructorBase(n_qc), system_constructor(m) {
   }

   DLLLOCAL BuiltinSystemConstructor2(const BuiltinSystemConstructor2 &old, const QoreClass *n_qc) : BuiltinSystemConstructorBase(old, n_qc), system_constructor(old.system_constructor) {
   }

   DLLLOCAL virtual void eval(const QoreClass &thisclass, QoreObject *self, int code, va_list args) const {
      system_constructor(thisclass, self, code, args);
   }

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const {
      return new BuiltinSystemConstructor2(*this, n_qc);
   }
};

class BuiltinMethod : public MethodFunction, public BuiltinFunctionBase {
public:
   DLLLOCAL BuiltinMethod(const QoreClass *n_qc, const char *mname) : MethodFunction(n_qc), BuiltinFunctionBase(mname) {
   }

   DLLLOCAL BuiltinMethod(const BuiltinMethod &old, const QoreClass *n_qc) : MethodFunction(old, n_qc), BuiltinFunctionBase(old) {
   }

   DLLLOCAL virtual const char *getName() const { 
      return name.c_str();
   }

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const {
      return new BuiltinMethod(*this, n_qc);
   }
};

// not visible to user code, does not follow abstract class pattern
class BuiltinDeleteBlocker : public BuiltinMethod {
protected:
   q_delete_blocker_t delete_blocker;

public:
   DLLLOCAL BuiltinDeleteBlocker(q_delete_blocker_t m) : BuiltinMethod(0, "<delete_blocker>"), delete_blocker(m) {
   }   

   DLLLOCAL BuiltinDeleteBlocker(const BuiltinDeleteBlocker &old, const QoreClass *n_qc) : BuiltinMethod(old, n_qc), delete_blocker(old.delete_blocker) {
   }

   DLLLOCAL bool eval(QoreObject *self, AbstractPrivateData *private_data) const {
      return delete_blocker(self, private_data);
   }

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const {
      return new BuiltinDeleteBlocker(*this, n_qc);
   }
};

#define BDELB(f) (reinterpret_cast<BuiltinDeleteBlocker *>(f))

class UserMethodBase {
protected:
   std::string name;

   DLLLOCAL UserMethodBase(const char *mname) : name(mname) {
   }

   DLLLOCAL UserMethodBase(const UserMethodBase &old) : name(old.name) {
   }
};

class UserMethod : public MethodFunction, public UserMethodBase {
public:
   DLLLOCAL UserMethod(const QoreClass *n_qc, const char *mname) : MethodFunction(n_qc), UserMethodBase(mname) {
   }
   DLLLOCAL UserMethod(const UserMethod &old, const QoreClass *n_qc) : MethodFunction(old, n_qc), UserMethodBase(old) {
   }
   DLLLOCAL virtual const char *getName() const {
      return name.c_str();
   }

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const {
      return new UserMethod(*this, n_qc);
   }
};

class QoreMemberInfo {
protected:
   const QoreTypeInfo *typeInfo;

   DLLLOCAL QoreMemberInfo(const QoreMemberInfo &old) : typeInfo(old.typeInfo), exp(old.exp ? old.exp->refSelf() : 0), first_line(old.first_line),
							last_line(old.last_line), file(old.file), parseTypeInfo(0) {
      assert(!old.parseTypeInfo);
   }

public:
   AbstractQoreNode *exp;
   // store parse location in case of errors
   int first_line, last_line;
   const char *file;
   QoreParseTypeInfo *parseTypeInfo;

   DLLLOCAL QoreMemberInfo(int nfl, int nll, const QoreTypeInfo *n_typeInfo, QoreParseTypeInfo *n_parseTypeInfo, AbstractQoreNode *e = 0) :
      typeInfo(n_typeInfo), exp(e), first_line(nfl), last_line(nll), 
      file(get_parse_file()), parseTypeInfo(n_parseTypeInfo) {
   }

   DLLLOCAL QoreMemberInfo(int nfl, int nll, const QoreTypeInfo *n_typeInfo, AbstractQoreNode *e = 0) : typeInfo(n_typeInfo), exp(e), first_line(nfl), last_line(nll), 
                                                                                                        file(get_parse_file()), parseTypeInfo(0) {
   }

   DLLLOCAL QoreMemberInfo(int nfl, int nll, char *n, AbstractQoreNode *e = 0) : typeInfo(0), exp(e), first_line(nfl), last_line(nll), file(get_parse_file()),
										 parseTypeInfo(new QoreParseTypeInfo(n)) {
   }

   DLLLOCAL QoreMemberInfo(int nfl, int nll, const QoreClass *qc, AbstractQoreNode *e) : typeInfo(qc->getTypeInfo()), exp(e), first_line(nfl), 
                                                                                         last_line(nll), file(get_parse_file()),
											 parseTypeInfo(0) {
   }
   DLLLOCAL QoreMemberInfo(int nfl, int nll, AbstractQoreNode *e) : typeInfo(0), exp(e), first_line(nfl), 
                                                                    last_line(nll), file(get_parse_file()),
                                                                    parseTypeInfo(0) {
   }
   DLLLOCAL ~QoreMemberInfo() {
      if (exp)
	 exp->deref(0);
      delete parseTypeInfo;
   }
   DLLLOCAL const QoreTypeInfo *getTypeInfo() const {
      return this ? typeInfo : 0;
   }
   DLLLOCAL bool parseHasTypeInfo() const {
      return this && (typeInfo || parseTypeInfo);
   }

   DLLLOCAL QoreMemberInfo *copy() const {
      if (!this)
         return 0;

      return new QoreMemberInfo(*this);
   }

   DLLLOCAL void parseInit(const char *name, bool priv) {
      if (!typeInfo) {
         typeInfo = parseTypeInfo->resolveAndDelete();
         parseTypeInfo = 0;
      }
#ifdef DEBUG
      else assert(!parseTypeInfo);
#endif

      if (exp) {
	 const QoreTypeInfo *argTypeInfo = 0;
	 int lvids = 0;
	 exp = exp->parseInit(0, 0, lvids, argTypeInfo);
	 if (lvids) {
	    update_parse_location(first_line, last_line, file);
	    parse_error("illegal local variable declaration in member initialization expression");
	    while (lvids)
	       pop_local_var();
	 }
	 // throw a type exception only if parse exceptions are enabled
	 if (!typeInfo->parseAccepts(argTypeInfo) && getProgram()->getParseExceptionSink()) {
            QoreStringNode *desc = new QoreStringNode("initialization expression for ");
	    desc->sprintf("%s member '$.%s' returns ", priv ? "private" : "public", name);
            argTypeInfo->getThisType(*desc);
            desc->concat(", but the member was declared as ");
            typeInfo->getThisType(*desc);
	    update_parse_location(first_line, last_line, file);
            getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
         }
      }
#if 0
      else if (hasType() && qt == NT_OBJECT) {
	 update_parse_location(first_line, last_line, file);
	 parseException("PARSE-TYPE-ERROR", "%s member '$.%s' has been defined with a complex type and must be assigned when instantiated", priv ? "private" : "public", name);
      }
#endif
   }
};

typedef std::map<char *, QoreMemberInfo *, ltstr> member_map_t;

class QoreMemberMap : public member_map_t {
public:
   DLLLOCAL ~QoreMemberMap() {
      member_map_t::iterator j;
      while ((j = begin()) != end()) {
         char *n = j->first;
         delete j->second;
         erase(j);
         //printd(5, "QoreMemberMap::~QoreMemberMap() this=%p freeing pending private member %p '%s'\n", this, n, n);
         free(n);
      }
   }
};

/*
  BCANode
  base class constructor argument node
*/
class BCANode : public FunctionCallBase {
public:
   qore_classid_t classid;
   //QoreClass *sclass;
   NamedScope *ns;
   char *name;

   // this function takes ownership of n and arg
   DLLLOCAL BCANode(NamedScope *n, QoreListNode *n_args) : FunctionCallBase(n_args), classid(0), ns(n), name(0) {
   }

   // this function takes ownership of n and arg
   DLLLOCAL BCANode(char *n, QoreListNode *n_args) : FunctionCallBase(n_args), classid(0), ns(0), name(n) {
   }

   DLLLOCAL ~BCANode() {
      delete ns;
      if (name)
	 free(name);
   }

   // resolves classes, parses arguments, and attempts to find constructor variant
   DLLLOCAL void parseInit(BCList *bcl, const char *classname);
};

//typedef safe_dslist<BCANode *> bcalist_t;
typedef std::vector<BCANode *> bcalist_t;

// BCAList
// base class constructor argument list
// this data structure will not be modified even if the class is copied
// to a subprogram object
class BCAList : public bcalist_t {
public:
   DLLLOCAL BCAList(BCANode *n) {
      push_back(n);
   }

   DLLLOCAL ~BCAList() {
      for (bcalist_t::iterator i = begin(), e = end(); i != e; ++i)
         delete *i;
   }

   // returns 0 for no errors, -1 for exception raised
   DLLLOCAL int execBaseClassConstructorArgs(BCEAList *bceal, ExceptionSink *xsink) const;
};

typedef std::pair<QoreClass *, bool> class_virt_pair_t;
//typedef std::list<class_virt_pair_t> class_list_t;
typedef std::vector<class_virt_pair_t> class_list_t;

// BCSMList: Base Class Special Method List
// unique list of base classes for a class hierarchy to ensure that "special" methods, constructor(), destructor(), copy() - are executed only once
// this class also tracks virtual classes to ensure that they are not inserted into the list in a complex tree and executed here
class BCSMList : public class_list_t {
public:
   DLLLOCAL BCSMList() {
   }
   DLLLOCAL BCSMList(const BCSMList &old) {
      reserve(old.size());
      for (class_list_t::const_iterator i = old.begin(), e = old.end(); i != e; ++i)
         push_back(*i);
   }
   DLLLOCAL void add(QoreClass *thisclass, QoreClass *qc, bool is_virtual);
   DLLLOCAL void addBaseClassesToSubclass(QoreClass *thisclass, QoreClass *sc, bool is_virtual);
   DLLLOCAL bool isBaseClass(QoreClass *qc) const;
   DLLLOCAL QoreClass *getClass(qore_classid_t cid) const;
   //DLLLOCAL void execConstructors(QoreObject *o, BCEAList *bceal, ExceptionSink *xsink) const;
   DLLLOCAL void execDestructors(QoreObject *o, ExceptionSink *xsink) const;
   DLLLOCAL void execSystemDestructors(QoreObject *o, ExceptionSink *xsink) const;
   DLLLOCAL void execCopyMethods(QoreObject *self, QoreObject *old, ExceptionSink *xsink) const;

   // resolve classes to the new class pointer after all namespaces and classes have been copied
   DLLLOCAL void resolveCopy();
};

// BCNode 
// base class pointer, also stores arguments for base class constructors
class BCNode {
public:
   NamedScope *cname;
   char *cstr;
   QoreClass *sclass;
   bool priv : 1;
   bool is_virtual : 1;
   
   DLLLOCAL BCNode(NamedScope *c, bool p) : cname(c), cstr(0), sclass(0), priv(p), is_virtual(false) {
   }

   // this method takes ownership of *str
   DLLLOCAL BCNode(char *str, bool p) : cname(0), cstr(str), sclass(0), priv(p), is_virtual(false) {
   }
   
   // for builtin base classes
   DLLLOCAL BCNode(QoreClass *qc, bool n_virtual = false) 
      : cname(0), cstr(0), sclass(qc), priv(false), is_virtual(n_virtual) {
   }

   // called at runtime with committed classes
   DLLLOCAL BCNode(const BCNode &old) : cname(0), cstr(0), sclass(old.sclass), priv(old.priv), is_virtual(old.is_virtual) {
      assert(!old.cname);
      assert(!old.cstr);
      assert(old.sclass);
   }

   DLLLOCAL ~BCNode() {
      delete cname;
      if (cstr)
         free(cstr);
   }

   DLLLOCAL bool isPrivate() const { return priv; }
   DLLLOCAL void parseInit(QoreClass *cls, bool &has_delete_blocker);
   DLLLOCAL const QoreClass *getClass(qore_classid_t cid, bool &n_priv) const {
      // sclass can be 0 if the class could not be found during parse initialization
      if (!sclass)
         return 0;

      const QoreClass *qc = (sclass->getID() == cid) ? sclass : sclass->getClassIntern(cid, n_priv);
      if (qc && !n_priv && priv)
	 n_priv = true;
      return qc;
   }
};

//typedef safe_dslist<BCNode *> bclist_t;
typedef std::vector<BCNode *> bclist_t;

//  BCList
//  linked list of base classes, constructors called head->tail, 
//  destructors called in reverse order (tail->head) (stored in BCSMList)
//  note that this data structure cannot be modified even if the class is
//  copied to a subprogram object and extended
//  this class is a QoreReferenceCounter so it won't be copied when the class is copied
class BCList : public bclist_t {
protected:
public:
   // special method (constructor, destructor, copy) list for superclasses
   BCSMList sml;

   DLLLOCAL BCList(BCNode *n) {
      push_back(n);
   }

   DLLLOCAL BCList() {
   }

   DLLLOCAL BCList(const BCList &old) : sml(old.sml) {
      reserve(old.size());
      for (bclist_t::const_iterator i = old.begin(), e = old.end(); i != e; ++i)
         push_back(new BCNode(*(*i)));
   }

   DLLLOCAL ~BCList() {
      for (bclist_t::iterator i = begin(), e = end(); i != e; ++i)
         delete *i;
   }

   DLLLOCAL void parseInit(QoreClass *thisclass, bool &has_delete_blocker);
   DLLLOCAL const QoreMethod *parseResolveSelfMethod(const char *name);

   // only looks in committed method lists
   DLLLOCAL const QoreMethod *parseFindCommittedMethod(const char *name);
   //DLLLOCAL const QoreMethod *parseFindCommittedStaticMethod(const char *name);

   // looks in committed and pending method lists
   DLLLOCAL const QoreMethod *parseFindMethodTree(const char *name);
   DLLLOCAL const QoreMethod *parseFindStaticMethodTree(const char *name);

   DLLLOCAL const QoreMethod *findCommittedMethod(const char *name, bool &priv_flag) const;
   DLLLOCAL const QoreMethod *findCommittedStaticMethod(const char *name, bool &priv_flag) const;

   DLLLOCAL bool match(const QoreClass *cls);
   DLLLOCAL void execConstructors(QoreObject *o, BCEAList *bceal, ExceptionSink *xsink) const;
   DLLLOCAL bool execDeleteBlockers(QoreObject *o, ExceptionSink *xsink) const;
   DLLLOCAL bool parseCheckHierarchy(const QoreClass *cls) const;
   DLLLOCAL bool isPrivateMember(const char *str) const;
   // member_has_type_info could return true while typeInfo is 0 if it has unresolved parse type information
   DLLLOCAL const QoreClass *parseFindPublicPrivateMember(const char *mem, const QoreTypeInfo *&memberTypeInfo, bool &member_has_type_info, bool &priv) const;
   DLLLOCAL bool runtimeGetMemberInfo(const char *mem, const QoreTypeInfo *&memberTypeInfo, bool &priv) const;
   DLLLOCAL bool parseHasPublicMembersInHierarchy() const;
   DLLLOCAL bool isPublicOrPrivateMember(const char *mem, bool &priv) const;
   DLLLOCAL const QoreClass *getClass(qore_classid_t cid, bool &priv) const {
      for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
	 const QoreClass *qc = (*i)->getClass(cid, priv);
	 if (qc)
	    return qc;
      }
	 
      return 0;
   }
   DLLLOCAL void addNewAncestors(QoreMethod *m);
   DLLLOCAL void addAncestors(QoreMethod *m);
   DLLLOCAL void addNewStaticAncestors(QoreMethod *m);
   DLLLOCAL void addStaticAncestors(QoreMethod *m);
   DLLLOCAL void parseAddAncestors(QoreMethod *m);
   DLLLOCAL void parseAddStaticAncestors(QoreMethod *m);

   DLLLOCAL void resolveCopy();
};

// BCEANode
// base constructor evaluated argument node; created locally at run time
class BCEANode {
public:
   QoreListNode *args;
   const AbstractQoreFunctionVariant *variant;
   bool execed;
      
   DLLLOCAL BCEANode(QoreListNode *n_args, const AbstractQoreFunctionVariant *n_variant) : args(n_args), variant(n_variant), execed(false) {}
   DLLLOCAL BCEANode() : args(0), variant(0), execed(true) {}
};

/*
struct ltqc {
   bool operator()(const QoreClass *qc1, const QoreClass *qc2) const {
      return qc1 < qc2;
   }
};
*/

typedef std::map<qore_classid_t, BCEANode *> bceamap_t;

/*
  BCEAList
  base constructor evaluated argument list
*/
class BCEAList : public bceamap_t {
protected:
   DLLLOCAL ~BCEAList() { }
   
public:
   DLLLOCAL void deref(ExceptionSink *xsink);
   // evaluates arguments, returns -1 if an exception was thrown
   DLLLOCAL int add(qore_classid_t classid, const QoreListNode *arg, const AbstractQoreFunctionVariant *variant, ExceptionSink *xsink);
   DLLLOCAL QoreListNode *findArgs(qore_classid_t classid, bool *aexeced, const AbstractQoreFunctionVariant *&variant);
};

#endif
