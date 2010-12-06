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

typedef std::map<std::string, QoreMethod *> hm_method_t;

static inline const char *privpub(bool priv) { return priv ? "private" : "public"; }

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
class NormalMethodFunction : public MethodFunctionBase {
public:
   DLLLOCAL NormalMethodFunction(const QoreClass *n_qc) : MethodFunctionBase(n_qc) {
   }
   DLLLOCAL NormalMethodFunction(const NormalMethodFunction &old, const QoreClass *n_qc) : MethodFunctionBase(old, n_qc) {
   }
   DLLLOCAL virtual ~NormalMethodFunction() {
   }
   DLLLOCAL virtual void parseInit();

   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL AbstractQoreNode *evalMethod(const AbstractQoreFunctionVariant *variant, QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const;
};

#define NMETHF(f) (reinterpret_cast<NormalMethodFunction *>(f))

// abstract class for method functions (static and non-static)
class StaticMethodFunction : public MethodFunctionBase {
public:
   DLLLOCAL StaticMethodFunction(const QoreClass *n_qc) : MethodFunctionBase(n_qc) {
   }
   DLLLOCAL StaticMethodFunction(const StaticMethodFunction &old, const QoreClass *n_qc) : MethodFunctionBase(old, n_qc) {
   }
   DLLLOCAL virtual ~StaticMethodFunction() {
   }
   DLLLOCAL virtual void parseInit();

   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL AbstractQoreNode *evalMethod(const AbstractQoreFunctionVariant *variant, const QoreListNode *args, ExceptionSink *xsink) const;
};

#define SMETHF(f) (reinterpret_cast<StaticMethodFunction *>(f))

// abstract class for constructor method functions
class ConstructorMethodFunction : public MethodFunctionBase {
public:
   DLLLOCAL ConstructorMethodFunction(const QoreClass *n_qc) : MethodFunctionBase(n_qc) {
   }
   DLLLOCAL ConstructorMethodFunction(const ConstructorMethodFunction &old, const QoreClass *n_qc) : MethodFunctionBase(old, n_qc) {
   }
   DLLLOCAL virtual void parseInit();
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
   DLLLOCAL virtual void parseInit();
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
   DLLLOCAL virtual void parseInit();
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

   // cannot add user variants to a builtin constructor
   DLLLOCAL virtual void parseInit() {}
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

   // cannot add user variants to a builtin constructor
   DLLLOCAL virtual void parseInit() {}
};

class BuiltinNormalMethod : public NormalMethodFunction, public BuiltinFunctionBase {
public:
   DLLLOCAL BuiltinNormalMethod(const QoreClass *n_qc, const char *mname) : NormalMethodFunction(n_qc), BuiltinFunctionBase(mname) {
   }

   DLLLOCAL BuiltinNormalMethod(const BuiltinNormalMethod &old, const QoreClass *n_qc) : NormalMethodFunction(old, n_qc), BuiltinFunctionBase(old) {
   }

   DLLLOCAL virtual const char *getName() const { 
      return name.c_str();
   }

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const {
      return new BuiltinNormalMethod(*this, n_qc);
   }
};

class BuiltinStaticMethod : public StaticMethodFunction, public BuiltinFunctionBase {
public:
   DLLLOCAL BuiltinStaticMethod(const QoreClass *n_qc, const char *mname) : StaticMethodFunction(n_qc), BuiltinFunctionBase(mname) {
   }

   DLLLOCAL BuiltinStaticMethod(const BuiltinStaticMethod &old, const QoreClass *n_qc) : StaticMethodFunction(old, n_qc), BuiltinFunctionBase(old) {
   }

   DLLLOCAL virtual const char *getName() const { 
      return name.c_str();
   }

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const {
      return new BuiltinStaticMethod(*this, n_qc);
   }
};

// not visible to user code, does not follow abstract class pattern
class BuiltinDeleteBlocker : public BuiltinNormalMethod {
protected:
   q_delete_blocker_t delete_blocker;

public:
   DLLLOCAL BuiltinDeleteBlocker(q_delete_blocker_t m) : BuiltinNormalMethod(0, "<delete_blocker>"), delete_blocker(m) {
   }   

   DLLLOCAL BuiltinDeleteBlocker(const BuiltinDeleteBlocker &old, const QoreClass *n_qc) : BuiltinNormalMethod(old, n_qc), delete_blocker(old.delete_blocker) {
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

class NormalUserMethod : public NormalMethodFunction, public UserMethodBase {
public:
   DLLLOCAL NormalUserMethod(const QoreClass *n_qc, const char *mname) : NormalMethodFunction(n_qc), UserMethodBase(mname) {
   }
   DLLLOCAL NormalUserMethod(const NormalUserMethod &old, const QoreClass *n_qc) : NormalMethodFunction(old, n_qc), UserMethodBase(old) {
   }
   DLLLOCAL virtual const char *getName() const {
      return name.c_str();
   }
   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const {
      return new NormalUserMethod(*this, n_qc);
   }
};

class StaticUserMethod : public StaticMethodFunction, public UserMethodBase {
public:
   DLLLOCAL StaticUserMethod(const QoreClass *n_qc, const char *mname) : StaticMethodFunction(n_qc), UserMethodBase(mname) {
   }
   DLLLOCAL StaticUserMethod(const StaticUserMethod &old, const QoreClass *n_qc) : StaticMethodFunction(old, n_qc), UserMethodBase(old) {
   }
   DLLLOCAL virtual const char *getName() const {
      return name.c_str();
   }
   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const {
      return new StaticUserMethod(*this, n_qc);
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
   DLLLOCAL QoreMemberInfo(int nfl, int nll) : typeInfo(0), exp(0), first_line(nfl), 
                                               last_line(nll), file(get_parse_file()),
                                               parseTypeInfo(0) {
   }
   DLLLOCAL ~QoreMemberInfo() {
      del();
   }

   DLLLOCAL void del() {
      if (exp)
	 exp->deref(0);
      delete parseTypeInfo;
#ifdef DEBUG
      exp = 0;
      parseTypeInfo = 0;
#endif
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

class QoreVarInfo : public QoreMemberInfo {
protected:
   DLLLOCAL QoreVarInfo(const QoreVarInfo &old) : QoreMemberInfo(old), val(old.val ? old.val->refSelf() : 0) {
   }

public:
   QoreThreadLock l;
   AbstractQoreNode *val;

   DLLLOCAL QoreVarInfo(int nfl, int nll, const QoreTypeInfo *n_typeInfo, QoreParseTypeInfo *n_parseTypeInfo, AbstractQoreNode *e = 0) :
      QoreMemberInfo(nfl, nll, n_typeInfo, n_parseTypeInfo, e), val(0) {
   }

   DLLLOCAL QoreVarInfo(int nfl, int nll) : QoreMemberInfo(nfl, nll), val(0) {
   }

   DLLLOCAL QoreVarInfo(int nfl, int nll, AbstractQoreNode *e) : QoreMemberInfo(nfl, nll, e), val(0) {
   }
   
   DLLLOCAL ~QoreVarInfo() {
      assert(!val);
   }

   DLLLOCAL void delVar(ExceptionSink *xsink) {
      del();
      if (val) {
         val->deref(xsink);
#ifdef DEBUG
         val = 0;
#endif
      }
   }

   DLLLOCAL QoreVarInfo *copy() const {
      if (!this)
         return 0;

      return new QoreVarInfo(*this);
   }

   DLLLOCAL void assignInit(AbstractQoreNode *v) {
      assert(!val);
      val = v;
   }

   DLLLOCAL void assign(AbstractQoreNode *v, ExceptionSink *xsink) {
      AutoLocker al(l);

      if (val)
         val->deref(xsink);
      val = v;
   }

   DLLLOCAL AbstractQoreNode *getReferencedValue() {
      AutoLocker al(l);
      return val ? val->refSelf() : 0;
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
	    parse_error("illegal local variable declaration in class static variable initialization expression");
	    while (lvids)
	       pop_local_var();
	 }
	 // throw a type exception only if parse exceptions are enabled
	 if (!typeInfo->parseAccepts(argTypeInfo) && getProgram()->getParseExceptionSink()) {
            QoreStringNode *desc = new QoreStringNode("initialization expression for ");
	    desc->sprintf("%s class static variable '%s' returns ", priv ? "private" : "public", name);
            argTypeInfo->getThisType(*desc);
            desc->concat(", but the variable was declared as ");
            typeInfo->getThisType(*desc);
	    update_parse_location(first_line, last_line, file);
            getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
         }
      }
   }

#ifdef DEBUG
   DLLLOCAL bool empty() const {
      return !val;
   }
#endif
};

typedef std::map<char *, QoreMemberInfo *, ltstr> member_map_t;
typedef std::map<char *, QoreVarInfo *, ltstr> var_map_t;

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
   DLLLOCAL bool inList(const char *name) const {
      return find((char *)name) != end();
   }
};

class QoreVarMap : public var_map_t {
public:
   DLLLOCAL ~QoreVarMap() {
      for (var_map_t::iterator i = begin(), e = end(); i != e; ++i) {
         //printd(0, "QoreVarMap::~QoreVarMap() deleting static var %s\n", i->first);
         assert(i->second->empty());
         i->second->del();
         free(i->first);
      }
   }

   DLLLOCAL void del(ExceptionSink *xsink) {
      for (var_map_t::iterator i = begin(), e = end(); i != e; ++i) {
         i->second->delVar(xsink);
         free(i->first);
         delete i->second;
      }
      clear();
   }

   DLLLOCAL bool inList(const char *name) const {
      return var_map_t::find((char *)name) != end();
   }

   DLLLOCAL QoreVarInfo *find(const char *name) const {
      var_map_t::const_iterator i = var_map_t::find((char *)name);
      return i == end() ? 0 : i->second;
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
// base class pointer
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
   DLLLOCAL const QoreClass *parseFindPublicPrivateMember(const char *mem, const QoreTypeInfo *&memberTypeInfo, bool &has_type_info, bool &priv) const;
   DLLLOCAL const QoreClass *parseFindPublicPrivateVar(const char *mem, const QoreTypeInfo *&varTypeInfo, bool &has_type_info, bool &priv) const;
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
   DLLLOCAL AbstractQoreNode *parseFindConstantValue(const char *cname, const QoreTypeInfo *&typeInfo, bool check);
   DLLLOCAL QoreVarInfo *parseFindStaticVar(const char *vname, const QoreClass *&qc, bool check) const;

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

struct SelfInstantiatorHelper {
   LocalVar *selfid;
   ExceptionSink *xsink;
   DLLLOCAL SelfInstantiatorHelper(LocalVar *n_selfid, QoreObject *self, ExceptionSink *n_xsink) : selfid(n_selfid), xsink(n_xsink) {
      selfid->instantiate_object(self);
   }
   DLLLOCAL ~SelfInstantiatorHelper() {
      selfid->uninstantiate(xsink);
   }
};

// private QoreClass implementation
struct qore_class_private {
   char *name;                   // the name of the class
   QoreClass *cls;               // parent class
   BCList *scl;                  // base class list
   hm_method_t hm,               // "normal" (non-static) method map
      shm;                       // static method map
   ConstantList pend_pub_const,  // pending public constants
      pend_priv_const,           // pending private constants
      pub_const,                 // committed public constants
      priv_const;                // committed private constants

   QoreMemberMap private_members, pending_private_members; // private member lists (maps)
   QoreMemberMap public_members, pending_public_members;   // public member lists (maps)

   QoreVarMap private_vars, pending_private_vars; // private static var lists (maps)
   QoreVarMap public_vars, pending_public_vars;   // public static var lists (maps)

   const QoreMethod *system_constructor, *constructor, *destructor,
      *copyMethod, *methodGate, *memberGate, *deleteBlocker,
      *memberNotification;

   qore_classid_t classID,          // class ID
      methodID;                     // for subclasses of builtin classes that will not have their own private data,
                                    // instead they will get the private data from this class
   bool sys,                        // system class?
      initialized,                  // is initialized? (only performed once)
      parse_init_called,            // has parseInit() been called? (performed once for each parseCommit())
      parse_init_partial_called,    // has parseInitPartial() been called? (performed once for each parseCommit())
      has_delete_blocker,           // has a delete_blocker function somewhere in the hierarchy?
      has_public_memdecl,           // has a public member declaration somewhere in the hierarchy?
      pending_has_public_memdecl,   // has a pending public member declaration in this class?
      owns_typeinfo,                // do we own the typeinfo data or not?
      resolve_copy_done,            // has the copy already been resolved
      has_new_user_changes,         // does the class have new user code that needs to be processed?
      owns_ornothingtypeinfo        // do we own the "or nothing" type info
      ;
   int64 domain;                      // capabilities of builtin class to use in the context of parse restrictions
   QoreReferenceCounter nref;       // namespace references
   int num_methods, num_user_methods, num_static_methods, num_static_user_methods;
   // type information for the class, may not have a pointer to the same QoreClass
   // as the actual owning class in case of a copy
   QoreTypeInfo *typeInfo;
   QoreTypeInfo *orNothingTypeInfo;
   // common "self" local variable for all constructors
   mutable LocalVar selfid;
   // user-specific data
   const void *ptr;
   // pointer to new class when copying
   mutable QoreClass *new_copy;

   DLLLOCAL qore_class_private(QoreClass *n_cls, const char *nme, int64 dom = QDOM_DEFAULT, QoreTypeInfo *n_typeInfo = 0);

   // only called while the parse lock for the QoreProgram owning "old" is held
   DLLLOCAL qore_class_private(const qore_class_private &old, QoreClass *n_cls);

   DLLLOCAL ~qore_class_private();

   DLLLOCAL void resolveCopy();

   DLLLOCAL void setUserData(const void *n_ptr) {
      ptr = n_ptr;
   }

   DLLLOCAL const void *getUserData() const {
      return ptr;
   }

   DLLLOCAL const QoreTypeInfo *getTypeInfo() const {
      return typeInfo;
   }

   DLLLOCAL const QoreTypeInfo *getOrNothingTypeInfo() const {
      return orNothingTypeInfo;
   }

   DLLLOCAL bool parseHasMemberGate() const {
      return memberGate || hm.find("memberGate") != hm.end();
   }

   DLLLOCAL bool parseHasMethodGate() const {
      return methodGate || hm.find("methodGate") != hm.end();
   }

   // checks for all special methods except constructor & destructor
   DLLLOCAL bool checkAssignSpecialIntern(const QoreMethod *m) {
      // set quick pointers
      if (!methodGate && !strcmp(m->getName(), "methodGate")) {
	 methodGate = m;
	 return true;
      }
      
      if (!memberGate && !strcmp(m->getName(), "memberGate")) {
	 memberGate = m;
	 return true;
      }
      
      if (!memberNotification && !strcmp(m->getName(), "memberNotification")) {
	 memberNotification = m;
	 return true;
      }

      return false;
   }

   // checks for all special methods except constructor, destructor, and copy
   DLLLOCAL bool checkSpecialStaticIntern(const char *mname) {
      // set quick pointers
      if ((!methodGate && !strcmp(mname, "methodGate"))
	  || (!memberGate && !strcmp(mname, "memberGate"))
	  || (!memberNotification && !strcmp(mname, "memberNotification")))
	 return true;
      return false;
   }

   // checks for all special methods
   DLLLOCAL bool checkSpecial(const char *mname) {
      // set quick pointers
      if ((!methodGate && !strcmp(mname, "methodGate"))
	  || (!memberGate && !strcmp(mname, "memberGate"))
	  || (!memberNotification && !strcmp(mname, "memberNotification"))
	  || (!constructor && !strcmp(mname, "constructor"))
	  || (!destructor && !strcmp(mname, "destructor"))
	  || (!copyMethod && !strcmp(mname, "copy")))
	 return true;
      return false;
   }

   // checks for all special methods
   DLLLOCAL bool checkAssignSpecial(const QoreMethod *m) {
      // set quick pointers
      if (!constructor && !strcmp(m->getName(), "constructor")) {
	 constructor = m;
	 return true;
      }

      if (!destructor && !strcmp(m->getName(), "destructor")) {
	 destructor = m;
	 return true;
      }

      if (!copyMethod && !strcmp(m->getName(), "copy")) {
	 copyMethod = m;
	 return true;
      }

      return checkAssignSpecialIntern(m);
   }

   DLLLOCAL void initialize();

   DLLLOCAL void parseInitPartial();

   DLLLOCAL const QoreExternalMethodVariant *findUserMethodVariant(const char *name, const QoreMethod *&method, const type_vec_t &argTypeList) const;

   DLLLOCAL const int parseCheckMemberAccess(const char *mem, const QoreTypeInfo *&memberTypeInfo, int pflag) const {
      const_cast<qore_class_private *>(this)->parseInitPartial();

      bool priv;
      bool has_type_info;
      const QoreClass *sclass = parseFindPublicPrivateMember(mem, memberTypeInfo, has_type_info, priv);
      
      if (!sclass) {
	 int rc = 0;
	 if (!parseHasMemberGate() || pflag & PF_FOR_ASSIGNMENT) {
	    if (getProgram()->getParseOptions() & PO_REQUIRE_TYPES) {
	       parse_error("member $.%s referenced has no type information because it was not declared in a public or private member list, but parse options require type information for all declarations", mem);
	       rc = -1;
	    }
	    if (parseHasPublicMembersInHierarchy()) {
	       //printd(5, "qore_class_private::parseCheckMemberAccess() %s %%.%s memberGate=%d pflag=%d\n", name, mem, parseHasMemberGate(), pflag);
	       parse_error("illegal access to unknown member '%s' in a class with a public member list (or inherited public member list)", mem);
	       rc = -1;
	    }
	 }
	 return rc;
      }

      // only raise a parse error for illegal access to private members if there is not memberGate function
      if (priv && !parseHasMemberGate() && !parseCheckPrivateClassAccess(cls)) {
	 memberTypeInfo = 0;
	 if (name)
	    parse_error("illegal access to private member '%s' of class '%s'", mem, name);
	 else
	    parse_error("illegal access to private member '%s'", mem);
	 return -1;
      }
      return 0;
   }

   DLLLOCAL const int parseCheckInternalMemberAccess(const char *mem, const QoreTypeInfo *&memberTypeInfo) const {
      const_cast<qore_class_private *>(this)->parseInitPartial();

      // throws a parse exception if there are public members and the name is not valid
      bool priv;
      bool has_type_info;
      const QoreClass *sclass = parseFindPublicPrivateMember(mem, memberTypeInfo, has_type_info, priv);
      int rc = 0;
      if (!sclass) {
	 if (getProgram()->getParseOptions() & PO_REQUIRE_TYPES) {
	    parse_error("member $.%s referenced has no type information because it was not declared in a public or private member list, but parse options require type information for all declarations", mem);
	    rc = -1;
	 }
	 if (parseHasPublicMembersInHierarchy()) {
	    parse_error("illegal access to unknown member '%s' (class has a public member list or inherited public member list)", mem);
	    rc = -1;
	 }
      }
      return rc;
   }

   DLLLOCAL bool parseHasPublicMembersInHierarchy() const {
      if (has_public_memdecl || pending_has_public_memdecl)
	 return true;

      return scl ? scl->parseHasPublicMembersInHierarchy() : false;
   }

   // returns true = found, false = not found
   DLLLOCAL bool runtimeGetMemberInfo(const char *mem, const QoreTypeInfo *&memberTypeInfo, bool &priv) const {
      member_map_t::const_iterator i = private_members.find(const_cast<char *>(mem));
      if (i != private_members.end()) {
	 priv = true;
	 memberTypeInfo = i->second->getTypeInfo();
	 return true;
      }
      
      i = public_members.find(const_cast<char *>(mem));
      if (i != public_members.end()) {
	 priv = false;
	 memberTypeInfo = i->second->getTypeInfo();
	 return true;
      }

      return scl ? scl->runtimeGetMemberInfo(mem, memberTypeInfo, priv) : false;
   }

   DLLLOCAL const QoreClass *parseFindPublicPrivateMember(const char *mem, const QoreTypeInfo *&memberTypeInfo, bool &has_type_info, bool &priv) const {
      bool found = false;
      member_map_t::const_iterator i = private_members.find(const_cast<char *>(mem));
      if (i != private_members.end())
	 found = true;
      else {
	 i = pending_private_members.find(const_cast<char *>(mem));
	 if (i != pending_private_members.end())
	    found = true;
      }
      if (found) {
	 priv = true;
	 has_type_info = i->second->parseHasTypeInfo();
	 memberTypeInfo = i->second->getTypeInfo();
	 return cls;
      }

      i = public_members.find(const_cast<char *>(mem));
      if (i != public_members.end())
	 found = true;
      else {
	 i = pending_public_members.find(const_cast<char *>(mem));
	 if (i != pending_public_members.end())
	    found = true;
      }

      if (found) {
	 priv = false;
	 has_type_info = i->second->parseHasTypeInfo();
	 memberTypeInfo = i->second->getTypeInfo();
	 return cls;
      }

      return scl ? scl->parseFindPublicPrivateMember(mem, memberTypeInfo, has_type_info, priv) : 0;
   }

   DLLLOCAL const QoreClass *parseFindPublicPrivateVar(const char *dname, const QoreTypeInfo *&varTypeInfo, bool &var_has_type_info, bool &priv) const {
      //printd(0, "parseFindPublicPrivateVar() this=%p cls=%p (%s) scl=%p\n", this, cls, cls->getName(), scl);

      QoreVarInfo *vi = private_vars.find(const_cast<char *>(dname));
      if (!vi)
	 vi = pending_private_vars.find(const_cast<char *>(dname));

      if (vi) {
	 priv = true;
	 var_has_type_info = vi->parseHasTypeInfo();
	 varTypeInfo = vi->getTypeInfo();
	 return cls;
      }

      vi = public_vars.find(const_cast<char *>(dname));
      if (!vi)
	 vi = pending_public_vars.find(const_cast<char *>(dname));

      if (vi) {
	 priv = false;
	 var_has_type_info = vi->parseHasTypeInfo();
	 varTypeInfo = vi->getTypeInfo();
	 return cls;
      }

      return scl ? scl->parseFindPublicPrivateVar(dname, varTypeInfo, var_has_type_info, priv) : 0;
   }

   DLLLOCAL int checkExistingVarMember(char *dname, bool decl_has_type_info, bool priv, const QoreClass *sclass, bool has_type_info, bool is_priv, bool var = false) const {
      //printd(5, "checkExistingVarMember() name=%s priv=%d is_priv=%d sclass=%s\n", name, priv, is_priv, sclass->getName());

      // here we know that the member or var already exists, so either it will be a
      // duplicate declaration, in which case it is ignored, or it is a
      // contradictory declaration, in which case a parse exception is raised

      // if the var was previously declared public
      if (priv != is_priv) {
	 // raise an exception only if parse exceptions are enabled
	 if (getProgram()->getParseExceptionSink()) {
	    QoreStringNode *desc = new QoreStringNode;
	    if (name)
	       desc->sprintf("class '%s' ", name);
	    desc->concat("cannot declare ");
	    desc->sprintf("%s %s ", privpub(priv), var ? "static variable" : "member");
	    desc->sprintf("'%s' when ", dname);
	    if (sclass == cls)
	       desc->concat("this class");
	    else
	       desc->sprintf("base class '%s'", sclass->getName());
	    desc->sprintf(" already declared this %s as %s", var ? "variable" : "member", privpub(is_priv));
	    getProgram()->makeParseException("PARSE-ERROR", desc);
	 }
	 return -1;
      }
      else if (decl_has_type_info || has_type_info) {
	 if (getProgram()->getParseExceptionSink()) {
	    QoreStringNode *desc = new QoreStringNode;
	    desc->sprintf("%s %s ", privpub(priv), var ? "static variable" : "member");
	    desc->sprintf("'%s' was already declared in ", dname);
	    if (sclass == cls)
	       desc->concat("this class");
	    else
	       desc->sprintf("base class '%s'", sclass->getName());
	    if (has_type_info)
	       desc->sprintf(" with a type definition");
	    desc->concat(" and cannot be declared again");
	    if (name)
	       desc->sprintf(" in class '%s'", name);
	    desc->concat(" if the declaration has a type definition");
	    
	    getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
	 }
	 return -1;
      }
      
      return 0;
   }

   DLLLOCAL int parseCheckVar(char *dname, bool decl_has_type_info, bool priv) const {
      const QoreTypeInfo *varTypeInfo;
      bool has_type_info;
      bool is_priv;
      const QoreClass *sclass = parseFindPublicPrivateVar(dname, varTypeInfo, has_type_info, is_priv);
      //printd(0, "parseCheckVar() %s cls=%p (%s)\n", dname, sclass, sclass ? sclass->getName() : "n/a");
      if (!sclass) {
         if (parseHasConstant(dname)) {
            parse_error("'%s' has already been declared as a constant in this class and therefore cannot be also declared as a static class variable in the same class with the same name", dname);
            return -1;
         }
	 return 0;
      }

      return checkExistingVarMember(dname, decl_has_type_info, priv, sclass, has_type_info, is_priv, true);
   }

   DLLLOCAL int parseCheckMember(char *mem, bool decl_has_type_info, bool priv) const {
      const QoreTypeInfo *memberTypeInfo;
      bool has_type_info;
      bool is_priv;
      const QoreClass *sclass = parseFindPublicPrivateMember(mem, memberTypeInfo, has_type_info, is_priv);
      if (!sclass)
	 return 0;

      return checkExistingVarMember(mem, decl_has_type_info, priv, sclass, has_type_info, is_priv);
   }

   DLLLOCAL int parseCheckMemberInBaseClasses(char *mem, bool decl_has_type_info, bool priv) const {
      const QoreTypeInfo *memberTypeInfo;
      bool has_type_info;
      bool is_priv;
      const QoreClass *sclass = scl ? scl->parseFindPublicPrivateMember(mem, memberTypeInfo, has_type_info, is_priv) : 0;
      if (!sclass)
	 return 0;

      return checkExistingVarMember(mem, decl_has_type_info, priv, sclass, has_type_info, is_priv);
   }

   DLLLOCAL void parseAddPrivateMember(char *mem, QoreMemberInfo *memberInfo) {
      if (!parseCheckMember(mem, memberInfo->parseHasTypeInfo(), true)) {
	 if (!has_new_user_changes)
	    has_new_user_changes = true;

	 //printd(5, "qore_class_private::parseAddPrivateMember() this=%p %s adding %p %s\n", this, name, mem, mem);
	 pending_private_members[mem] = memberInfo;
	 return;
      }

      free(mem);
      delete memberInfo;
   }

   DLLLOCAL void parseAddPrivateStaticVar(char *dname, QoreVarInfo *varInfo) {
      if (!parseCheckVar(dname, varInfo->parseHasTypeInfo(), true)) {
	 if (!has_new_user_changes)
	    has_new_user_changes = true;

	 //printd(5, "qore_class_private::parseAddPrivateStaticVar() this=%p %s adding %p %s\n", this, name, mem, mem);
	 pending_private_vars[dname] = varInfo;
	 return;
      }

      free(dname);
      delete varInfo;
   }

   DLLLOCAL void parseAddPublicStaticVar(char *dname, QoreVarInfo *varInfo) {
      if (!parseCheckVar(dname, varInfo->parseHasTypeInfo(), false)) {
	 if (!has_new_user_changes)
	    has_new_user_changes = true;

	 //printd(5, "QoreClass::parseAddPublicStaticVar() this=%p %s adding %p %s\n", this, name, mem, mem);
	 pending_public_vars[dname] = varInfo;
	 return;
      }

      free(dname);
      delete varInfo;
   }

   DLLLOCAL void addBuiltinConstant(const char *name, AbstractQoreNode *value, bool priv = false, const QoreTypeInfo *typeInfo = 0) {
      assert(!pub_const.inList(name));
      assert(!priv_const.inList(name));
      if (priv)
         priv_const.add(name, value, typeInfo);
      else
         pub_const.add(name, value, typeInfo);
   }

   DLLLOCAL void addBuiltinStaticVar(const char *name, AbstractQoreNode *value, bool priv = false, const QoreTypeInfo *typeInfo = 0) {
      assert(!public_vars.inList(name));
      assert(!private_vars.inList(name));

      QoreVarInfo *vi = new QoreVarInfo(0, 0, typeInfo, 0, value);

      if (priv)
         private_vars[strdup(name)] = vi;
      else
         public_vars[strdup(name)] = vi;
   }

   DLLLOCAL void parseAssimilatePublicConstants(ConstantList &cmap) {
      pend_pub_const.assimilate(cmap, pub_const, priv_const, pend_priv_const, false, name);
   }

   DLLLOCAL void parseAssimilatePrivateConstants(ConstantList &cmap) {
      pend_priv_const.assimilate(cmap, priv_const, pub_const, pend_pub_const, true, name);
   }

   DLLLOCAL void parseAddPublicConstant(const std::string &cname, AbstractQoreNode *val) {
      if (parseHasVar(cname.c_str())) {
         parse_error("'%s' has already been declared as a static variable in this class and therefore cannot be also declared as a constant in the same class with the same name", cname.c_str());
         val->deref(0);
         return;
      }
      //printd(0, "parseAddPublicConstant() this=%p cls=%p const=%s\n", this, cls, cname.c_str());
      
      pend_pub_const.parseAdd(cname, val, pub_const, priv_const, pend_priv_const, false, name);
   }

   DLLLOCAL bool parseHasVar(const char *vn) {
      return public_vars.inList(vn) || pending_public_vars.inList(vn)
         || private_vars.inList(vn) || pending_private_vars.inList(vn)
         ? true
         : false;
   }   

   DLLLOCAL bool parseHasConstant(const std::string &cname) const {
      return pub_const.inList(cname) || pend_pub_const.inList(cname)
         || priv_const.inList(cname) || pend_priv_const.inList(cname)
         ? true
         : false;
   }

   DLLLOCAL AbstractQoreNode *parseFindLocalConstantValue(const char *cname, const QoreTypeInfo *&typeInfo) {
      // first check public constants
      AbstractQoreNode *rv = pub_const.find(cname, typeInfo);
      if (!rv) {
	 rv = pend_pub_const.find(cname, typeInfo);
	 if (!rv) {
	    // now check private constants
	    rv = priv_const.find(cname, typeInfo);
	    if (!rv)
	       rv = pend_priv_const.find(cname, typeInfo);
	 
	    // check for accessibility to private constants
	    if (rv && !parseCheckPrivateClassAccess(cls)) {
	       rv = 0;
	       typeInfo = 0;
	    }
	 }
      }

      //printd(0, "qore_class_private::parseFindLocalConstantValue(%s) this=%p (cls=%p %s) rv=%p\n", cname, this, cls, name, rv);      
      return rv;
   }

   DLLLOCAL AbstractQoreNode *parseFindConstantValue(const char *cname, const QoreTypeInfo *&typeInfo, bool check = false) {
      bool priv = false;

      // first check public constants
      AbstractQoreNode *rv = pub_const.find(cname, typeInfo);
      if (!rv) {
	 rv = pend_pub_const.find(cname, typeInfo);
	 if (!rv) {
            priv = true;

	    // now check private constants
	    rv = priv_const.find(cname, typeInfo);
	    if (!rv) {
	       rv = pend_priv_const.find(cname, typeInfo);
            }
	 }
      }

      // check for accessibility to private constants
      if (rv) {
         if (check && priv && !parseCheckPrivateClassAccess(cls)) {
            typeInfo = 0;
            return 0;
         }

         return rv;
      }

      return scl ? scl->parseFindConstantValue(cname, typeInfo, check) : 0;
   }

   DLLLOCAL QoreVarInfo *parseFindLocalStaticVar(const char *vname) const {
      QoreVarInfo *vi = public_vars.find(vname);
      if (!vi) {
         vi = pending_public_vars.find(vname);
         if (!vi) {
            vi = private_vars.find(vname);
            if (!vi)
               vi = pending_private_vars.find(vname);
            if (vi && !parseCheckPrivateClassAccess(cls))
               vi = 0;
         }
      }

      return vi;
   }

   DLLLOCAL QoreVarInfo *parseFindStaticVar(const char *vname, const QoreClass *&qc, bool check = false) const {
      bool priv = false;

      QoreVarInfo *vi = public_vars.find(vname);

      if (!vi) {
         vi = pending_public_vars.find(vname);
         if (!vi) {
            priv = true;
            vi = private_vars.find(vname);
            if (!vi) {
               vi = pending_private_vars.find(vname);
            }
         }
      }

      if (vi) {

         // check accessibility to private data
         if (check && priv && vi && !parseCheckPrivateClassAccess(cls)) {
            return 0;
         }
         qc = cls;
         return vi;
      }

      return scl ? scl->parseFindStaticVar(vname, qc, check) : 0;
   }

   DLLLOCAL void parseAddPublicMember(char *mem, QoreMemberInfo *memberInfo) {
      if (!parseCheckMember(mem, memberInfo->parseHasTypeInfo(), false)) {
	 if (!has_new_user_changes)
	    has_new_user_changes = true;

	 //printd(5, "QoreClass::parseAddPublicMember() this=%p %s adding %p %s\n", this, name, mem, mem);
	 pending_public_members[mem] = memberInfo;
	 if (!pending_has_public_memdecl)
	    pending_has_public_memdecl = true;
	 return;
      }

      free(mem);
      delete memberInfo;
   }

   DLLLOCAL void addPublicMember(const char *mem, const QoreTypeInfo *n_typeInfo, AbstractQoreNode *initial_value) {
      assert(public_members.find(name) == public_members.end());
      public_members[strdup(mem)] = new QoreMemberInfo(0, 0, n_typeInfo, 0, initial_value);
      if (!has_public_memdecl)
	 has_public_memdecl = true;
   }

   DLLLOCAL void addPrivateMember(const char *mem, const QoreTypeInfo *n_typeInfo, AbstractQoreNode *initial_value) {
      assert(private_members.find(name) == private_members.end());
      private_members[strdup(mem)] = new QoreMemberInfo(0, 0, n_typeInfo, 0, initial_value);
   }

   DLLLOCAL void insertBuiltinStaticMethod(QoreMethod *m) {
      assert(m->isStatic());
      //printd(5, "QoreClass::insertBuiltinStaticMethod() %s::%s() size=%d\n", name, m->getName(), numMethods());
      shm[m->getName()] = m;
      // maintain method counts (safely inside parse lock)
      ++num_static_methods;
      if (!sys) sys = true;
      // check for special methods (except constructor and destructor) and abort if found
      assert(!checkSpecialStaticIntern(m->getName()));
      // add ancestors
      addStaticAncestors(m);
   }

   DLLLOCAL void insertBuiltinMethod(QoreMethod *m, bool special_method = false) {
      assert(!m->isStatic());
      //printd(5, "QoreClass::insertBuiltinMethod() %s::%s() size=%d\n", name, m->getName(), numMethods());
      hm[m->getName()] = m;      
      // maintain method counts (safely inside parse lock)
      ++num_methods;
      if (!sys) sys = true;
      // check for special methods (except constructor and destructor)
      if (!special_method && !checkAssignSpecialIntern(m))
	 // add ancestors
	 addAncestors(m);
   }

   DLLLOCAL void recheckBuiltinMethodHierarchy();

   DLLLOCAL void addNewAncestors(QoreMethod *m) {
      if (!scl)
	 return;

      scl->addNewAncestors(m);
   }

   DLLLOCAL void addNewStaticAncestors(QoreMethod *m) {
      if (!scl)
	 return;

      scl->addNewStaticAncestors(m);
   }

   DLLLOCAL void addStaticAncestors(QoreMethod *m) {
      if (!scl)
	 return;

      scl->addStaticAncestors(m);
   }

   DLLLOCAL void addAncestors(QoreMethod *m) {
      assert(strcmp(m->getName(), "constructor"));

      if (!scl)
	 return;

      scl->addAncestors(m);
   }

   DLLLOCAL void parseAddStaticAncestors(QoreMethod *m) {
      if (!scl)
	 return;

      scl->parseAddStaticAncestors(m);
   }

   DLLLOCAL void parseAddAncestors(QoreMethod *m) {
      //printd(5, "qore_class_private::parseAddAncestors(%p %s) this=%p cls=%p %s scl=%p\n", m, m->getName(), this, cls, name, scl);
      assert(strcmp(m->getName(), "constructor"));

      if (!scl)
	 return;

      scl->parseAddAncestors(m);
   }

   DLLLOCAL bool isPublicOrPrivateMember(const char *mem, bool &priv) const {
      if (private_members.find(const_cast<char *>(mem)) != private_members.end()) {
	 priv = true;
	 return true;
      }

      if (public_members.find(const_cast<char *>(mem)) != public_members.end()) {
	 priv = false;
	 return true;
      }

      return scl ? scl->isPublicOrPrivateMember(mem, priv) : false;
   }

   DLLLOCAL int initMembers(QoreObject *o, member_map_t::const_iterator i, member_map_t::const_iterator e, ExceptionSink *xsink) const {
      for (; i != e; ++i) {
	 if (i->second) {
	    AbstractQoreNode **v = o->getMemberValuePtrForInitialization(i->first);
	    if (i->second->exp) {
	       ReferenceHolder<AbstractQoreNode> val(i->second->exp->eval(xsink), xsink);
	       if (*xsink)
		  return -1;
	       // check types
	       AbstractQoreNode *nv = i->second->getTypeInfo()->acceptInputMember(i->first, *val, xsink);
	       if (*xsink)
		  return -1;
	       *v = nv;
	       val.release();
	    }
	    else {
	       //*v = i->second->getTypeInfo()->getDefaultValue();
	       *v = 0;
	    }
	 }
      } 
      return 0;
   }

   DLLLOCAL int initVar(const char *name, QoreVarInfo &vi, ExceptionSink *xsink) const {
      if (vi.exp) {
         // evaluate expression
         ReferenceHolder<AbstractQoreNode> val(vi.exp->eval(xsink), xsink);
         if (*xsink)
            return -1;

         val = vi.getTypeInfo()->acceptInputMember(name, val.release(), xsink);
         if (*xsink)
            return -1;

         vi.assignInit(val.release());
      }

      return 0;
   }

   DLLLOCAL void deleteClassStaticVars(ExceptionSink *xsink) {
      private_vars.del(xsink);
      public_vars.del(xsink);
   }

   DLLLOCAL int initMembers(QoreObject *o, ExceptionSink *xsink) const {
      if (public_members.empty() && private_members.empty())
	 return 0;

      SelfInstantiatorHelper sih(&selfid, o, xsink);

      if (initMembers(o, private_members.begin(), private_members.end(), xsink)
	  || initMembers(o, public_members.begin(), public_members.end(), xsink))
	 return -1;
      return 0;
   }

   DLLLOCAL QoreObject *execConstructor(const AbstractQoreFunctionVariant *variant, const QoreListNode *args, ExceptionSink *xsink) const;

   DLLLOCAL void addBuiltinMethod(const char *mname, MethodVariantBase *variant);
   DLLLOCAL void addBuiltinStaticMethod(const char *mname, MethodVariantBase *variant);
   DLLLOCAL void addBuiltinConstructor(BuiltinConstructorVariantBase *variant);
   DLLLOCAL void addBuiltinDestructor(BuiltinDestructorVariantBase *variant);
   DLLLOCAL void addBuiltinCopyMethod(BuiltinCopyVariantBase *variant);
   DLLLOCAL void setDeleteBlocker(q_delete_blocker_t func);
   DLLLOCAL void setBuiltinSystemConstructor(BuiltinSystemConstructorBase *m);

   DLLLOCAL void execBaseClassConstructor(QoreObject *self, BCEAList *bceal, ExceptionSink *xsink) const;
   DLLLOCAL QoreObject *execSystemConstructor(QoreObject *self, int code, va_list args) const;
   DLLLOCAL bool execDeleteBlocker(QoreObject *self, ExceptionSink *xsink) const;
   DLLLOCAL QoreObject *execCopy(QoreObject *old, ExceptionSink *xsink) const;

   // returns a non-static method if it exists in the local class
   DLLLOCAL QoreMethod *parseFindLocalMethod(const char *nme) {
      hm_method_t::iterator i = hm.find(nme);
      return (i != hm.end()) ? i->second : 0;
   }
   // returns a non-static method if it exists in the local class
   DLLLOCAL const QoreMethod *parseFindLocalMethod(const char *nme) const {
      hm_method_t::const_iterator i = hm.find(nme);
      return (i != hm.end()) ? i->second : 0;
   }

   // returns a static method if it exists in the local class
   DLLLOCAL QoreMethod *parseFindLocalStaticMethod(const char *nme) {
      hm_method_t::iterator i = shm.find(nme);
      return (i != shm.end()) ? i->second : 0;
   }
   // returns a static method if it exists in the local class
   DLLLOCAL const QoreMethod *parseFindLocalStaticMethod(const char *nme) const {
      hm_method_t::const_iterator i = shm.find(nme);
      return (i != shm.end()) ? i->second : 0;
   }

   // returns a non-static method if it exists in the local class and has been committed to the class
   DLLLOCAL QoreMethod *findLocalCommittedMethod(const char *nme);
   // returns a non-static method if it exists in the local class and has been committed to the class
   DLLLOCAL const QoreMethod *findLocalCommittedMethod(const char *nme) const;

   // returns a static method if it exists in the local class and has been committed to the class
   DLLLOCAL QoreMethod *findLocalCommittedStaticMethod(const char *nme);
   // returns a static method if it exists in the local class and has been committed to the class
   DLLLOCAL const QoreMethod *findLocalCommittedStaticMethod(const char *nme) const;

   // returns a non-static method if it exists in class hierarchy and has been committed to the class, initializes base classes if necessary
   DLLLOCAL const QoreMethod *parseFindCommittedMethod(const char *nme) {
      const QoreMethod *m = findLocalCommittedMethod(nme);
      if (!m && scl)
	 m = scl->parseFindCommittedMethod(nme);
      return m;
   }

/*
   // returns a static method if it exists in the local class and has been committed to the class, initializes base classes if necessary
   DLLLOCAL const QoreMethod *parseFindCommittedStaticMethod(const char *nme) {
      const QoreMethod *m = findLocalCommittedStaticMethod(nme);
      if (!m && scl)
	 m = scl->parseFindCommittedStaticMethod(nme);
      return m;
   }
*/

   // finds a non-static method in the class hierarchy at parse time, optionally initializes classes
   DLLLOCAL const QoreMethod *parseFindMethod(const char *mname) {
      const QoreMethod *m = parseFindLocalMethod(mname);      
      if (!m && scl)
	 m = scl->parseFindMethodTree(mname);
      return m;
   }

   // finds a static method in the class hierarchy at parse time, optionally initializes classes
   DLLLOCAL const QoreMethod *parseFindStaticMethod(const char *mname) {
      const QoreMethod *m = parseFindLocalStaticMethod(mname);      
      if (!m && scl)
	 m = scl->parseFindStaticMethodTree(mname);
      return m;
   }

   // returns a non-static method if it exists in class hierarchy and has been committed to the class
   DLLLOCAL const QoreMethod *findCommittedStaticMethod(const char *nme, bool &p) const {
      const QoreMethod *w = findLocalCommittedStaticMethod(nme);
      if (!w && scl)
	 w = scl->findCommittedStaticMethod(nme, p);
      return w;
   }

   // returns a non-static method if it exists in class hierarchy and has been committed to the class
   DLLLOCAL const QoreMethod *findCommittedMethod(const char *nme, bool &p) const {
      const QoreMethod *w = findLocalCommittedMethod(nme);
      if (!w && scl)
	 w = scl->findCommittedMethod(nme, p);
      return w;
   }

   DLLLOCAL const QoreMethod *findStaticMethod(const char *nme, bool &priv_flag) const {
      const QoreMethod *w;
      if (!(w = findLocalCommittedStaticMethod(nme))) {
	 // search superclasses
	 if (scl)
	    w = scl->findCommittedStaticMethod(nme, priv_flag);
      }
      return w;
   }

   const QoreMethod *findMethod(const char *nme, bool &priv_flag) const {
      const QoreMethod *w;
      if (!(w = findLocalCommittedMethod(nme))) {
	 // search superclasses
	 if (scl)
	    w = scl->findCommittedMethod(nme, priv_flag);
      }
      return w;
   }

   DLLLOCAL void execDestructor(QoreObject *self, ExceptionSink *xsink) const;

   DLLLOCAL void execBaseClassDestructor(QoreObject *self, ExceptionSink *xsink) const;

   DLLLOCAL void execBaseClassSystemDestructor(QoreObject *self, ExceptionSink *xsink) const;

   DLLLOCAL void execBaseClassCopy(QoreObject *self, QoreObject *old, ExceptionSink *xsink) const;

   DLLLOCAL void parseInit();
   DLLLOCAL void parseCommit();
   DLLLOCAL void parseRollback();
   DLLLOCAL int addUserMethod(const char *mname, MethodVariantBase *f, bool n_static);

   // static methods
   //DLLLOCAL void 
   DLLLOCAL static const QoreClass *parseFindPublicPrivateVar(const QoreClass *qc, const char *name, const QoreTypeInfo *&varTypeInfo, bool &has_type_info, bool &priv) {
      return qc->priv->parseFindPublicPrivateVar(name, varTypeInfo, has_type_info, priv);
   }

   DLLLOCAL static void parseAddPrivateStaticVar(QoreClass *qc, char *dname, QoreVarInfo *varInfo) {
      qc->priv->parseAddPrivateStaticVar(dname, varInfo);
   }

   DLLLOCAL static void parseAddPublicStaticVar(QoreClass *qc, char *dname, QoreVarInfo *varInfo) {
      qc->priv->parseAddPublicStaticVar(dname, varInfo);
   }

   DLLLOCAL static void deleteClassStaticVars(QoreClass *qc, ExceptionSink *xsink) {
      qc->priv->deleteClassStaticVars(xsink);
   }

   // searches only the current class, returns 0 if private found and not accessible in the current parse context
   DLLLOCAL static AbstractQoreNode *parseFindLocalConstantValue(QoreClass *qc, const char *cname, const QoreTypeInfo *&typeInfo) {
      return qc->priv->parseFindLocalConstantValue(cname, typeInfo);
   }

   // searches only the current class, returns 0 if private found and not accessible in the current parse context
   DLLLOCAL static QoreVarInfo *parseFindLocalStaticVar(const QoreClass *qc, const char *vname, const QoreTypeInfo *&typeInfo) {
      QoreVarInfo *vi = qc->priv->parseFindLocalStaticVar(vname);
      if (vi)
         typeInfo = vi->getTypeInfo();
      return vi;
   }

   // searches this class and all superclasses, if check = false, then assumes parsing from within the class (getParseClass() == this class)
   DLLLOCAL static AbstractQoreNode *parseFindConstantValue(QoreClass *qc, const char *cname, const QoreTypeInfo *&typeInfo, bool check = false) {
      return qc->priv->parseFindConstantValue(cname, typeInfo, check);
   }

   // searches this class and all superclasses, if check = false, then assumes parsing from within the class (getParseClass() == this class)
   DLLLOCAL static QoreVarInfo *parseFindStaticVar(const QoreClass *qc, const char *vname, const QoreClass *&nqc, const QoreTypeInfo *&typeInfo, bool check = false) {
      QoreVarInfo *vi = qc->priv->parseFindStaticVar(vname, nqc, check);
      if (vi)
         typeInfo = vi->getTypeInfo();
      return vi;
   }
};

#endif
