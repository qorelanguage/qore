/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreClassIntern.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_QORECLASSINTERN_H

#define _QORE_QORECLASSINTERN_H

#include "qore/safe_dslist"
#include "qore/intern/ConstantList.h"
#include "qore/intern/QoreLValue.h"
#include "qore/intern/qore_var_rwlock_priv.h"
#include "qore/intern/VRMutex.h"

#include <string.h>

#include <list>
#include <map>
#include <string>

#define OTF_USER    CT_USER
#define OTF_BUILTIN CT_BUILTIN

#ifdef HAVE_QORE_HASH_MAP
#include <qore/hash_map_include.h>
#include <qore/intern/xxhash.h>

typedef HASH_MAP<std::string, QoreMethod*> hm_method_t;
#else
typedef std::map<std::string, QoreMethod*> hm_method_t;
#endif

// forward reference to private class implementation
class qore_class_private;

// map from abstract signature to variant for fast tracking of abstract variants
typedef std::map<const char*, MethodVariantBase*, ltstr> vmap_t;

struct AbstractMethod {
   // committed abstract methods from this class and parent classes
   vmap_t vlist;
   // pending abstract methods from this class and parent classes
   vmap_t pending_vlist;
   // save temporarily removed committed variants while parsing; to be moved back to vlist on parse rollback or purged on parse commit
   vmap_t pending_save;

   DLLLOCAL AbstractMethod() {
   }

   DLLLOCAL AbstractMethod(const AbstractMethod& old);

   DLLLOCAL ~AbstractMethod();

   // merge changes from parent class method of the same name during parse initialization
   DLLLOCAL void parseMergeBase(AbstractMethod& m, bool committed = false);

   // merge changes from parent class method of the same name during parse initialization
   DLLLOCAL void parseMergeBase(AbstractMethod& m, MethodFunctionBase* f, bool committed = false);

   DLLLOCAL void parseAdd(MethodVariantBase* v);

   DLLLOCAL void parseOverride(MethodVariantBase* v);

   DLLLOCAL void parseInit(const char* cname, const char* mname);

   // delete/purge all saved variants in the pending_save list, returns 0 if the AbstractMethod still has abstract variants, -1 if not and therefore can be removed from the map
   DLLLOCAL int parseCommit();

   // move all saved variants back from the pending_save list to the committed list (vlist)
   DLLLOCAL void parseRollback() {
      assert(!pending_save.empty());
      for (vmap_t::iterator i = pending_save.begin(), e = pending_save.end(); i != e; ++i) {
         assert(vlist.find(i->first) == vlist.end());
         vlist.insert(vmap_t::value_type(i->first, i->second));
      }
      pending_save.clear();
      assert(!vlist.empty());
   }

   DLLLOCAL static void checkAbstract(const char* cname, const char* mname, vmap_t& vlist, QoreStringNode*& desc);

   DLLLOCAL void add(MethodVariantBase* v);
   DLLLOCAL void override(MethodVariantBase* v);

   DLLLOCAL bool empty() const {
      return vlist.empty() && pending_vlist.empty() && pending_save.empty();
   }
};

#ifdef HAVE_QORE_HASH_MAP
typedef HASH_MAP<std::string, AbstractMethod*> amap_t;
#else
typedef std::map<std::string, AbstractMethod*> amap_t;
#endif

struct AbstractMethodMap : amap_t {
   DLLLOCAL AbstractMethodMap(const AbstractMethodMap& old) {
      for (amap_t::const_iterator i = old.begin(), e = old.end(); i != e; ++i) {
         if (i->second->vlist.empty())
            continue;
         insert(amap_t::value_type(i->first, new AbstractMethod(*(i->second))));
      }
   }

   DLLLOCAL AbstractMethodMap() {
   }

   DLLLOCAL ~AbstractMethodMap() {
      for (amap_t::iterator i = begin(), e = end(); i != e; ++i)
         delete i->second;
   }

   DLLLOCAL AbstractMethod* findMethod(const std::string& name) {
      amap_t::iterator i = find(name);
      return i == end() ? 0 : i->second;
   }

   // adds a pending abstract variant if it doesn't already exist
   DLLLOCAL void parseAddAbstractVariant(const char* name, MethodVariantBase* f);

   DLLLOCAL void parseOverrideAbstractVariant(const char* name, MethodVariantBase* f);

   // adds a committed abstract variant if it doesn't already exist
   DLLLOCAL void addAbstractVariant(const char* name, MethodVariantBase* f);

   // adds a committed non-abstract variant
   DLLLOCAL void overrideAbstractVariant(const char* name, MethodVariantBase* f);

   DLLLOCAL void parseCommit() {
      for (amap_t::iterator i = begin(), e = end(); i != e;) {
         if (i->second->parseCommit()) {
            //printd(5, "AbstractMethodMap::parseCommit() removing abstract ???::%s()\n", i->first.c_str());
            delete i->second;
            erase(i++);
            continue;
         }
         ++i;
      }
   }

   DLLLOCAL void parseRollback() {
      for (amap_t::iterator i = begin(), e = end(); i != e; ++i)
         i->second->parseRollback();
   }

   DLLLOCAL void parseInit(qore_class_private& qc, BCList* scl);

   DLLLOCAL QoreStringNode* checkAbstract(const char* cname) const;

   // we check if there are any abstract method variants still in the committed lists
   DLLLOCAL void parseCheckAbstractNew(const char* name) const;

   // we check if there are any abstract method variants in the class at runtime (for use with exec-class)
   DLLLOCAL int runtimeCheckInstantiateClass(const char* name, ExceptionSink* xsink) const;
};

class SignatureHash;

static inline const char* privpub(ClassAccess access) {
   return access == Public
      ? "public"
      : (access == Private ? "private" : "private:internal");
}

// forward reference for base class (constructor) argument list
class BCAList;
// forward reference for base class list
class BCList;
// forward reference for base class (constructor) evaluated argument list
class BCEAList;

class MethodVariantBase : public AbstractQoreFunctionVariant {
protected:
   const QoreMethod* qmethod;    // pointer to method that owns the variant
   ClassAccess access;           // variant access code
   bool final,                   // is the variant final or not
      abstract;                  // is the variant abstract or not
   std::string asig;             // abstract signature, only set for abstract method variants

public:
   // add QC_USES_EXTRA_ARGS to abstract methods by default as derived methods could use extra arguments
   DLLLOCAL MethodVariantBase(ClassAccess n_access, bool n_final, int64 n_flags, bool n_is_user = false, bool n_is_abstract = false) :
      AbstractQoreFunctionVariant(n_flags | (n_is_abstract ? QC_USES_EXTRA_ARGS : 0), n_is_user), qmethod(0), access(n_access), final(n_final), abstract(n_is_abstract) {
   }

   DLLLOCAL bool isAbstract() const {
      return abstract;
   }

   DLLLOCAL bool isPrivate() const {
      return access > Public;
   }

   DLLLOCAL ClassAccess getAccess() const {
      return access;
   }

   DLLLOCAL bool isFinal() const {
      return final;
   }

   DLLLOCAL void clearAbstract() {
      assert(abstract);
      abstract = false;
   }

   DLLLOCAL void setMethod(QoreMethod* n_qm) {
      qmethod = n_qm;
   }

   DLLLOCAL const QoreMethod* method() const {
      assert(qmethod);
      return qmethod;
   }

   DLLLOCAL const QoreClass* getClass() const {
      return qmethod->getClass();
   }

   DLLLOCAL const char* getAbstractSignature();

   DLLLOCAL const qore_class_private* getClassPriv() const;

   DLLLOCAL MethodVariantBase* ref() {
      ROreference();
      return this;
   }

   DLLLOCAL void deref() {
      if (ROdereference()) {
         delete this;
      }
   }
};

#define METHVB(f) (reinterpret_cast<MethodVariantBase*>(f))
#define METHVB_const(f) (reinterpret_cast<const MethodVariantBase*>(f))

class MethodVariant : public MethodVariantBase {
public:
   DLLLOCAL MethodVariant(ClassAccess n_access, bool n_final, int64 n_flags, bool n_is_user = false, bool is_abstract = false) : MethodVariantBase(n_access, n_final, n_flags, n_is_user, is_abstract) {
   }
   DLLLOCAL virtual QoreValue evalMethod(QoreObject* self, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const = 0;
   DLLLOCAL virtual QoreValue evalPseudoMethod(const QoreValue n, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      assert(false);
      return QoreValue();
   }
};

#define METHV(f) (reinterpret_cast<MethodVariant*>(f))
#define METHV_const(f) (reinterpret_cast<const MethodVariant*>(f))

class ConstructorMethodVariant : public MethodVariantBase {
protected:
   // evaluates base class constructors and initializes members
   DLLLOCAL int constructorPrelude(const QoreClass &thisclass, CodeEvaluationHelper& ceh, QoreObject* self, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const;

public:
   DLLLOCAL ConstructorMethodVariant(ClassAccess n_access, int64 n_flags, bool n_is_user = false) : MethodVariantBase(n_access, false, n_flags, n_is_user) {
   }
   DLLLOCAL virtual const BCAList* getBaseClassArgumentList() const = 0;
   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject* self, CodeEvaluationHelper& ceh, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const = 0;
};

#define CONMV(f) (reinterpret_cast<ConstructorMethodVariant*>(f))
#define CONMV_const(f) (reinterpret_cast<const ConstructorMethodVariant*>(f))

class DestructorMethodVariant : public MethodVariantBase {
protected:
public:
   DLLLOCAL DestructorMethodVariant(bool n_is_user = false) : MethodVariantBase(Public, QC_NO_FLAGS, n_is_user) {
   }

   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject* self, ExceptionSink* xsink) const = 0;
};

#define DESMV(f) (reinterpret_cast<DestructorMethodVariant*>(f))
#define DESMV_const(f) (reinterpret_cast<const DestructorMethodVariant*>(f))

class CopyMethodVariant : public MethodVariantBase {
protected:
public:
   DLLLOCAL CopyMethodVariant(ClassAccess n_access, bool n_is_user = false) : MethodVariantBase(n_access, QC_NO_FLAGS, n_is_user) {
   }

   DLLLOCAL virtual void evalCopy(const QoreClass &thisclass, QoreObject* self, QoreObject* old, CodeEvaluationHelper& ceh, BCList* scl, ExceptionSink* xsink) const = 0;
};

#define COPYMV(f) (reinterpret_cast<CopyMethodVariant*>(f))
#define COPYMV_const(f) (reinterpret_cast<const CopyMethodVariant*>(f))

class UserMethodVariant : public MethodVariant, public UserVariantBase {
protected:
   bool synchronized;

public:
   DLLLOCAL UserMethodVariant(ClassAccess n_access, bool n_final, StatementBlock* b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced, int64 n_flags, bool is_abstract) : MethodVariant(n_access, n_final, n_flags, true, is_abstract), UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, rv, false), synchronized(synced) {
   }

   DLLLOCAL ~UserMethodVariant() {
   }

   // the following defines the pure virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL virtual void parseInit(QoreFunction* f) {
      MethodFunctionBase* mf = static_cast<MethodFunctionBase*>(f);

      signature.resolve();
      // parseResolve and push current return type on stack
      ParseCodeInfoHelper rtih(mf->getName(), signature.getReturnTypeInfo());

      // must be called even if "statements" is NULL
      if (!mf->isStatic()) {
         if (!isAbstract())
            statements->parseInitMethod(mf->MethodFunctionBase::getClass()->getTypeInfo(), this);
      }
      else
	 statements->parseInit(this);

      // recheck types against committed types if necessary
      if (recheck)
         f->parseCheckDuplicateSignatureCommitted(&signature);
   }

   DLLLOCAL virtual QoreValue evalMethod(QoreObject* self, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const;
};

#define UMV(f) (reinterpret_cast<UserMethodVariant*>(f))
#define UMV_const(f) (reinterpret_cast<const UserMethodVariant*>(f))

class UserConstructorVariant : public ConstructorMethodVariant, public UserVariantBase {
protected:
   // base class argument list for constructors
   BCAList* bcal;

   DLLLOCAL virtual ~UserConstructorVariant();

public:
   DLLLOCAL UserConstructorVariant(ClassAccess n_access, StatementBlock* b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, BCAList* n_bcal, int64 n_flags = QC_NO_FLAGS) : ConstructorMethodVariant(n_access, n_flags, true), UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, 0, false), bcal(n_bcal) {
   }

   // the following defines the pure virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL virtual const BCAList* getBaseClassArgumentList() const {
      return bcal;
   }

   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject* self, CodeEvaluationHelper& ceh, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const;

   DLLLOCAL virtual void parseInit(QoreFunction* f);
};

#define UCONV(f) (reinterpret_cast<UserConstructorVariant*>(f))
#define UCONV_const(f) (reinterpret_cast<const UserConstructorVariant*>(f))

class UserDestructorVariant : public DestructorMethodVariant, public UserVariantBase {
protected:
public:
   DLLLOCAL UserDestructorVariant(StatementBlock* b, int n_sig_first_line, int n_sig_last_line) : DestructorMethodVariant(true), UserVariantBase(b, n_sig_first_line, n_sig_last_line, 0, 0, false) {
   }

   // the following defines the pure virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL virtual void parseInit(QoreFunction* f) {
      MethodFunctionBase* mf = static_cast<MethodFunctionBase*>(f);

      signature.resolve();
      assert(!signature.getReturnTypeInfo() || signature.getReturnTypeInfo() == nothingTypeInfo);

      // push return type on stack (no return value can be used)
      ParseCodeInfoHelper rtih("destructor", nothingTypeInfo);

      // must be called even if statements is NULL
      statements->parseInitMethod(mf->MethodFunctionBase::getClass()->getTypeInfo(), this);

      // only 1 variant is possible, no need to recheck types
   }

   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject* self, ExceptionSink* xsink) const {
      // there cannot be any params
      assert(!signature.numParams());
      assert(!signature.getReturnTypeInfo() || signature.getReturnTypeInfo() == nothingTypeInfo);
      eval("destructor", 0, self, xsink, getClassPriv()).discard(xsink);
   }
};

#define UDESV(f) (reinterpret_cast<UserDestructorVariant*>(f))
#define UDESV_const(f) (reinterpret_cast<const UserDestructorVariant*>(f))

class UserCopyVariant : public CopyMethodVariant, public UserVariantBase {
protected:
public:
   DLLLOCAL UserCopyVariant(ClassAccess n_access, StatementBlock* b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced) : CopyMethodVariant(n_access, true), UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, rv, synced) {
   }

   // the following defines the pure virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL virtual void parseInit(QoreFunction* f);

   DLLLOCAL virtual void evalCopy(const QoreClass &thisclass, QoreObject* self, QoreObject* old, CodeEvaluationHelper& ceh, BCList* scl, ExceptionSink* xsink) const;
};

#define UCOPYV(f) (reinterpret_cast<UserCopyVariant*>(f))

class BuiltinMethodVariant : public MethodVariant, public BuiltinFunctionVariantBase {
public:
   DLLLOCAL BuiltinMethodVariant(ClassAccess n_access, bool n_final, int64 n_flags, int64 n_functionality, const QoreTypeInfo* n_returnTypeInfo, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : MethodVariant(n_access, n_final, n_flags), BuiltinFunctionVariantBase(n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names) {}

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS
};

class BuiltinAbstractMethodVariant : public BuiltinMethodVariant {
public:
   DLLLOCAL BuiltinAbstractMethodVariant(ClassAccess n_access, int64 n_flags, const QoreTypeInfo* n_returnTypeInfo, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_access, false, n_flags, QDOM_DEFAULT, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names) {
      abstract = true;
   }

   DLLLOCAL virtual QoreValue evalMethod(QoreObject* self, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      assert(false);
      return QoreValue();
   }
};

class BuiltinNormalMethodVariantBase : public BuiltinMethodVariant {
public:
   DLLLOCAL BuiltinNormalMethodVariantBase(ClassAccess n_access, bool n_final, int64 n_flags, int64 n_functionality, const QoreTypeInfo* n_returnTypeInfo, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_access, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names) {}

   DLLLOCAL virtual QoreValue evalMethod(QoreObject* self, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const;

   DLLLOCAL virtual QoreValue evalPseudoMethod(const QoreValue n, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const;

   DLLLOCAL virtual QoreValue evalImpl(QoreObject* self, AbstractPrivateData* private_data, const QoreValueList* args, q_rt_flags_t rtflags, ExceptionSink* xsink) const = 0;
};

template <typename F>
class BuiltinNormalMethodVariantTemplate : public BuiltinNormalMethodVariantBase {
protected:
   F method;

public:
   DLLLOCAL BuiltinNormalMethodVariantTemplate(F m, ClassAccess n_access, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinNormalMethodVariantBase(n_access, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), method(m) {
   }
   DLLLOCAL virtual QoreValue evalImpl(QoreObject* self, AbstractPrivateData* private_data, const QoreValueList* args, q_rt_flags_t rtflags, ExceptionSink* xsink) const {
      ReferenceHolder<QoreListNode> l(args ? args->getOldList() : 0, xsink);
      return method(self, private_data, *l, xsink);
   }
};

typedef BuiltinNormalMethodVariantTemplate<q_method_t> BuiltinNormalMethodVariant;
typedef BuiltinNormalMethodVariantTemplate<q_method_int64_t> BuiltinNormalMethodBigIntVariant;
typedef BuiltinNormalMethodVariantTemplate<q_method_int_t> BuiltinNormalMethodIntVariant;
typedef BuiltinNormalMethodVariantTemplate<q_method_double_t> BuiltinNormalMethodFloatVariant;
typedef BuiltinNormalMethodVariantTemplate<q_method_bool_t> BuiltinNormalMethodBoolVariant;

class BuiltinNormalMethodValueVariant : public BuiltinNormalMethodVariantBase {
protected:
   q_method_n_t method;

public:
   DLLLOCAL BuiltinNormalMethodValueVariant(q_method_n_t m, ClassAccess n_access, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinNormalMethodVariantBase(n_access, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), method(m) {
   }
   DLLLOCAL virtual QoreValue evalImpl(QoreObject* self, AbstractPrivateData* private_data, const QoreValueList* args, q_rt_flags_t rtflags, ExceptionSink* xsink) const {
      return method(self, private_data, args, rtflags, xsink);
   }
};

class BuiltinExternalNormalMethodValueVariant : public BuiltinNormalMethodVariantBase {
protected:
   q_external_method_t method;
   const void* ptr;

public:
   DLLLOCAL BuiltinExternalNormalMethodValueVariant(const void* n_ptr, q_external_method_t m, ClassAccess n_access, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinNormalMethodVariantBase(n_access, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), method(m), ptr(n_ptr) {
   }
   DLLLOCAL virtual QoreValue evalImpl(QoreObject* self, AbstractPrivateData* private_data, const QoreValueList* args, q_rt_flags_t rtflags, ExceptionSink* xsink) const {
      return method(*qmethod, ptr, self, private_data, args, rtflags, xsink);
   }
};

class BuiltinNormalMethod2Variant : public BuiltinNormalMethodVariantBase {
protected:
   q_method2_t method;

public:
   DLLLOCAL BuiltinNormalMethod2Variant(q_method2_t m, ClassAccess n_access, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinNormalMethodVariantBase(n_access, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), method(m) {
   }
   DLLLOCAL virtual QoreValue evalImpl(QoreObject* self, AbstractPrivateData* private_data, const QoreValueList* args, q_rt_flags_t rtflags, ExceptionSink* xsink) const {
      ReferenceHolder<QoreListNode> l(args ? args->getOldList() : 0, xsink);
      return method(*qmethod, self, private_data, *l, xsink);
   }
};

class BuiltinNormalMethod3Variant : public BuiltinNormalMethodVariantBase {
protected:
   q_method3_t method;
   const void* ptr;

public:
   DLLLOCAL BuiltinNormalMethod3Variant(const void* n_ptr, q_method3_t m, ClassAccess n_access, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinNormalMethodVariantBase(n_access, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), method(m), ptr(n_ptr) {
   }
   DLLLOCAL virtual QoreValue evalImpl(QoreObject* self, AbstractPrivateData* private_data, const QoreValueList* args, q_rt_flags_t rtflags, ExceptionSink* xsink) const {
      ReferenceHolder<QoreListNode> l(args ? args->getOldList() : 0, xsink);
      return method(*qmethod, signature.getTypeList(), ptr, self, private_data, *l, xsink);
   }
};

template <typename F>
class BuiltinStaticMethodVariantTemplate : public BuiltinMethodVariant {
protected:
   F static_method;

public:
   DLLLOCAL BuiltinStaticMethodVariantTemplate(F m, ClassAccess n_access, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_access, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), static_method(m) {
   }

   DLLLOCAL virtual QoreValue evalMethod(QoreObject* self, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      CodeContextHelper cch(xsink, CT_BUILTIN, qmethod->getName(), 0, getClassPriv());
      const QoreValueList* args = ceh.getArgs();
      ReferenceHolder<QoreListNode> l(args ? args->getOldList() : 0, xsink);
      return static_method(*l, xsink);
   }
};

typedef BuiltinStaticMethodVariantTemplate<q_func_t> BuiltinStaticMethodVariant;
typedef BuiltinStaticMethodVariantTemplate<q_func_int64_t> BuiltinStaticMethodBigIntVariant;
typedef BuiltinStaticMethodVariantTemplate<q_func_double_t> BuiltinStaticMethodFloatVariant;
typedef BuiltinStaticMethodVariantTemplate<q_func_bool_t> BuiltinStaticMethodBoolVariant;

class BuiltinStaticMethodValueVariant : public BuiltinMethodVariant {
protected:
   q_func_n_t static_method;

public:
   DLLLOCAL BuiltinStaticMethodValueVariant(q_func_n_t m, ClassAccess n_access, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_access, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), static_method(m) {
   }

   DLLLOCAL virtual QoreValue evalMethod(QoreObject* self, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      CodeContextHelper cch(xsink, CT_BUILTIN, qmethod->getName(), 0, getClassPriv());

      return static_method(ceh.getArgs(), ceh.getRuntimeFlags(), xsink);
   }
};

class BuiltinExternalStaticMethodValueVariant : public BuiltinMethodVariant {
protected:
   q_external_static_method_t static_method;
   const void* ptr;

public:
   DLLLOCAL BuiltinExternalStaticMethodValueVariant(const void* n_ptr, q_external_static_method_t m, ClassAccess n_access, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_access, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), static_method(m), ptr(n_ptr) {
   }

   DLLLOCAL virtual QoreValue evalMethod(QoreObject* self, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      CodeContextHelper cch(xsink, CT_BUILTIN, qmethod->getName(), 0, getClassPriv());

      return static_method(*qmethod, ptr, ceh.getArgs(), ceh.getRuntimeFlags(), xsink);
   }
};

// FIXME: deprecated
class BuiltinStaticMethod2Variant : public BuiltinMethodVariant {
protected:
   q_static_method2_t static_method;

public:
   DLLLOCAL BuiltinStaticMethod2Variant(q_static_method2_t m, ClassAccess n_access, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_access, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), static_method(m) {
   }
   DLLLOCAL QoreValue evalMethod(QoreObject* self, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      CodeContextHelper cch(xsink, CT_BUILTIN, qmethod->getName(), 0, getClassPriv());
      const QoreValueList* args = ceh.getArgs();
      ReferenceHolder<QoreListNode> l(args ? args->getOldList() : 0, xsink);
      return static_method(*qmethod, *l, xsink);
   }
};

// FIXME: deprecated
class BuiltinStaticMethod3Variant : public BuiltinMethodVariant {
protected:
   q_static_method3_t static_method;
   const void* ptr;

public:
   DLLLOCAL BuiltinStaticMethod3Variant(const void* n_ptr, q_static_method3_t m, ClassAccess n_access, bool n_final, int64 n_flags, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_access, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), static_method(m), ptr(n_ptr) {
   }
   DLLLOCAL QoreValue evalMethod(QoreObject* self, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      CodeContextHelper cch(xsink, CT_BUILTIN, qmethod->getName(), 0, getClassPriv());
      const QoreValueList* args = ceh.getArgs();
      ReferenceHolder<QoreListNode> l(args ? args->getOldList() : 0, xsink);
      return static_method(*qmethod, signature.getTypeList(), ptr, *l, xsink);
   }
};

class BuiltinConstructorVariantBase : public ConstructorMethodVariant, public BuiltinFunctionVariantBase {
public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   DLLLOCAL BuiltinConstructorVariantBase(ClassAccess n_access, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : ConstructorMethodVariant(n_access, n_flags), BuiltinFunctionVariantBase(n_functionality, 0, n_typeList, n_defaultArgList, n_names) {
   }

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS

   DLLLOCAL virtual const BCAList* getBaseClassArgumentList() const {
      return 0;
   }
};

class BuiltinConstructorValueVariant : public BuiltinConstructorVariantBase {
protected:
   q_constructor_n_t constructor;

public:
   DLLLOCAL BuiltinConstructorValueVariant(q_constructor_n_t m, ClassAccess n_access, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinConstructorVariantBase(n_access, n_flags, n_functionality, n_typeList, n_defaultArgList, n_names), constructor(m) {
   }

   DLLLOCAL virtual void evalConstructor(const QoreClass& thisclass, QoreObject* self, CodeEvaluationHelper& ceh, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const;
};

class BuiltinExternalConstructorValueVariant : public BuiltinConstructorVariantBase {
protected:
   q_external_constructor_t constructor;
   const void* ptr;

public:
   DLLLOCAL BuiltinExternalConstructorValueVariant(const void* n_ptr, q_external_constructor_t m, ClassAccess n_access, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinConstructorVariantBase(n_access, n_flags, n_functionality, n_typeList, n_defaultArgList, n_names), constructor(m), ptr(n_ptr) {
   }

   DLLLOCAL virtual void evalConstructor(const QoreClass& thisclass, QoreObject* self, CodeEvaluationHelper& ceh, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const;
};

class BuiltinConstructorVariant : public BuiltinConstructorVariantBase {
protected:
   q_constructor_t constructor;

public:
   DLLLOCAL BuiltinConstructorVariant(q_constructor_t m, ClassAccess n_access, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinConstructorVariantBase(n_access, n_flags, n_functionality, n_typeList, n_defaultArgList, n_names), constructor(m) {
   }

   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject* self, CodeEvaluationHelper& ceh, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const;
};

class BuiltinConstructor2Variant : public BuiltinConstructorVariantBase {
protected:
   q_constructor2_t constructor;

public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   DLLLOCAL BuiltinConstructor2Variant(q_constructor2_t m, ClassAccess n_access, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinConstructorVariantBase(n_access, n_flags, n_functionality, n_typeList, n_defaultArgList, n_names), constructor(m) {
   }
   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject* self, CodeEvaluationHelper& ceh, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const;
};

class BuiltinConstructor3Variant : public BuiltinConstructorVariantBase {
protected:
   q_constructor3_t constructor;
   const void* ptr;

public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   DLLLOCAL BuiltinConstructor3Variant(const void* n_ptr, q_constructor3_t m, ClassAccess n_access, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinConstructorVariantBase(n_access, n_flags, n_functionality, n_typeList, n_defaultArgList, n_names), constructor(m), ptr(n_ptr) {
   }

   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject* self, CodeEvaluationHelper& ceh, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const;
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

   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject* self, ExceptionSink* xsink) const;
};

class BuiltinDestructor2Variant : public BuiltinDestructorVariantBase {
protected:
   q_destructor2_t destructor;

public:
   DLLLOCAL BuiltinDestructor2Variant(q_destructor2_t n_destructor) : destructor(n_destructor) {
   }

   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject* self, ExceptionSink* xsink) const;
};

class BuiltinDestructor3Variant : public BuiltinDestructorVariantBase {
protected:
   q_destructor3_t destructor;
   const void* ptr;

public:
   DLLLOCAL BuiltinDestructor3Variant(const void* n_ptr, q_destructor3_t n_destructor) : destructor(n_destructor), ptr(n_ptr) {
   }

   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject* self, ExceptionSink* xsink) const;
};

class BuiltinCopyVariantBase : public CopyMethodVariant, public BuiltinFunctionVariantBase {
protected:
public:
   DLLLOCAL BuiltinCopyVariantBase(const QoreClass* c) : CopyMethodVariant(Public), BuiltinFunctionVariantBase(QDOM_DEFAULT, c->getTypeInfo()) {
   }

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS

   DLLLOCAL virtual void evalCopy(const QoreClass &thisclass, QoreObject* self, QoreObject* old, CodeEvaluationHelper& ceh, BCList* scl, ExceptionSink* xsink) const;
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject* self, QoreObject* old, AbstractPrivateData* private_data, ExceptionSink* xsink) const = 0;
};

class BuiltinCopyVariant : public BuiltinCopyVariantBase {
protected:
   q_copy_t copy;

public:
   DLLLOCAL BuiltinCopyVariant(QoreClass* c, q_copy_t m) : BuiltinCopyVariantBase(c), copy(m) {
   }
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject* self, QoreObject* old, AbstractPrivateData* private_data, ExceptionSink* xsink) const {
      copy(self, old, private_data, xsink);
   }
};

class BuiltinCopy2Variant : public BuiltinCopyVariantBase {
protected:
   q_copy2_t copy;

public:
   DLLLOCAL BuiltinCopy2Variant(QoreClass* c, q_copy2_t m) : BuiltinCopyVariantBase(c), copy(m) {
   }
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject* self, QoreObject* old, AbstractPrivateData* private_data, ExceptionSink* xsink) const {
      copy(thisclass, self, old, private_data, xsink);
   }
};

class BuiltinCopy3Variant : public BuiltinCopyVariantBase {
protected:
   q_copy3_t copy;
   const void* ptr;

public:
   DLLLOCAL BuiltinCopy3Variant(const void* n_ptr, QoreClass* c, q_copy3_t m) : BuiltinCopyVariantBase(c), copy(m), ptr(n_ptr) {
   }
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject* self, QoreObject* old, AbstractPrivateData* private_data, ExceptionSink* xsink) const {
      copy(thisclass, ptr, self, old, private_data, xsink);
   }
};

// abstract class for method functions (static and non-static)
class NormalMethodFunction : public MethodFunctionBase {
public:
   DLLLOCAL NormalMethodFunction(const char* nme, const QoreClass* n_qc) : MethodFunctionBase(nme, n_qc, false) {
   }

   DLLLOCAL NormalMethodFunction(const NormalMethodFunction &old, const QoreClass* n_qc) : MethodFunctionBase(old, n_qc) {
   }

   DLLLOCAL virtual ~NormalMethodFunction() {
   }

   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL QoreValue evalMethod(ExceptionSink* xsink, const AbstractQoreFunctionVariant* variant, QoreObject* self, const QoreListNode* args) const;

   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL QoreValue evalPseudoMethod(ExceptionSink* xsink, const AbstractQoreFunctionVariant* variant, const QoreValue n, const QoreListNode* args) const;
};

#define NMETHF(f) (reinterpret_cast<NormalMethodFunction*>(f))

// abstract class for method functions (static and non-static)
class StaticMethodFunction : public MethodFunctionBase {
public:
   DLLLOCAL StaticMethodFunction(const char* nme, const QoreClass* n_qc) : MethodFunctionBase(nme, n_qc, true) {
   }
   DLLLOCAL StaticMethodFunction(const StaticMethodFunction &old, const QoreClass* n_qc) : MethodFunctionBase(old, n_qc) {
   }
   DLLLOCAL virtual ~StaticMethodFunction() {
   }
   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL QoreValue evalMethod(ExceptionSink* xsink, const AbstractQoreFunctionVariant* variant, const QoreListNode* args) const;
};

#define SMETHF(f) (reinterpret_cast<StaticMethodFunction*>(f))

// abstract class for constructor method functions
class ConstructorMethodFunction : public MethodFunctionBase {
public:
   DLLLOCAL ConstructorMethodFunction(const QoreClass* n_qc) : MethodFunctionBase("constructor", n_qc, false) {
   }
   DLLLOCAL ConstructorMethodFunction(const ConstructorMethodFunction &old, const QoreClass* n_qc) : MethodFunctionBase(old, n_qc) {
   }
   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL void evalConstructor(const AbstractQoreFunctionVariant* variant, const QoreClass &thisclass, QoreObject* self, const QoreValueList* args, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const;

   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL void evalConstructor(const AbstractQoreFunctionVariant* variant, const QoreClass &thisclass, QoreObject* self, const QoreListNode* args, BCList* bcl, BCEAList* bceal, ExceptionSink* xsink) const;

   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const {
      return new ConstructorMethodFunction(*this, n_qc);
   }
};

#define CONMF(f) (reinterpret_cast<ConstructorMethodFunction*>(f))

// abstract class for destructor method functions
class DestructorMethodFunction : public MethodFunctionBase {
public:
   DLLLOCAL DestructorMethodFunction(const QoreClass* n_qc) : MethodFunctionBase("destructor", n_qc, false) {
   }
   DLLLOCAL DestructorMethodFunction(const DestructorMethodFunction &old, const QoreClass* n_qc) : MethodFunctionBase(old, n_qc) {
   }
   DLLLOCAL void evalDestructor(const QoreClass &thisclass, QoreObject* self, ExceptionSink* xsink) const;

   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const {
      return new DestructorMethodFunction(*this, n_qc);
   }
};

#define DESMF(f) (reinterpret_cast<DestructorMethodFunction*>(f))

// abstract class for copy method functions
class CopyMethodFunction : public MethodFunctionBase {
public:
   DLLLOCAL CopyMethodFunction(const QoreClass* n_qc) : MethodFunctionBase("copy", n_qc, false) {
   }
   DLLLOCAL CopyMethodFunction(const CopyMethodFunction &old, const QoreClass* n_qc) : MethodFunctionBase(old, n_qc) {
   }
   DLLLOCAL void evalCopy(const QoreClass &thisclass, QoreObject* self, QoreObject* old, BCList* scl, ExceptionSink* xsink) const;

   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const {
      return new CopyMethodFunction(*this, n_qc);
   }
};

#define COPYMF(f) (reinterpret_cast<CopyMethodFunction*>(f))

class BuiltinSystemConstructorBase : public MethodFunctionBase {
public:
   DLLLOCAL BuiltinSystemConstructorBase(const QoreClass* n_qc) : MethodFunctionBase("constructor", n_qc, false) {
   }
   DLLLOCAL BuiltinSystemConstructorBase(const BuiltinSystemConstructorBase &old, const QoreClass* n_qc) : MethodFunctionBase(old, n_qc) {
   }
   DLLLOCAL virtual void eval(const QoreClass &thisclass, QoreObject* self, int code, va_list args) const = 0;

   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const = 0;
};

#define BSYSCONB(f) (reinterpret_cast<BuiltinSystemConstructorBase* >(f))

// system constructors are not accessed from userspace so we don't need to conform
// to the abstract class structure
class BuiltinSystemConstructor : public BuiltinSystemConstructorBase {
protected:
   q_system_constructor_t system_constructor;

public:
   DLLLOCAL BuiltinSystemConstructor(const QoreClass* n_qc, q_system_constructor_t m) : BuiltinSystemConstructorBase(n_qc), system_constructor(m) {
   }

   DLLLOCAL BuiltinSystemConstructor(const BuiltinSystemConstructor &old, const QoreClass* n_qc) : BuiltinSystemConstructorBase(old, n_qc), system_constructor(old.system_constructor) {
   }

   DLLLOCAL virtual void eval(const QoreClass &thisclass, QoreObject* self, int code, va_list args) const {
      system_constructor(self, code, args);
   }

   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const {
      return new BuiltinSystemConstructor(*this, n_qc);
   }
};

class BuiltinSystemConstructor2 : public BuiltinSystemConstructorBase {
protected:
   q_system_constructor2_t system_constructor;

public:
   DLLLOCAL BuiltinSystemConstructor2(const QoreClass* n_qc, q_system_constructor2_t m) : BuiltinSystemConstructorBase(n_qc), system_constructor(m) {
   }

   DLLLOCAL BuiltinSystemConstructor2(const BuiltinSystemConstructor2 &old, const QoreClass* n_qc) : BuiltinSystemConstructorBase(old, n_qc), system_constructor(old.system_constructor) {
   }

   DLLLOCAL virtual void eval(const QoreClass &thisclass, QoreObject* self, int code, va_list args) const {
      system_constructor(thisclass, self, code, args);
   }

   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const {
      return new BuiltinSystemConstructor2(*this, n_qc);
   }
};

class BuiltinNormalMethod : public NormalMethodFunction {
public:
   DLLLOCAL BuiltinNormalMethod(const QoreClass* n_qc, const char* mname) : NormalMethodFunction(mname, n_qc) {
   }

   DLLLOCAL BuiltinNormalMethod(const BuiltinNormalMethod &old, const QoreClass* n_qc) : NormalMethodFunction(old, n_qc) {
   }

   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const {
      return new BuiltinNormalMethod(*this, n_qc);
   }
};

class BuiltinStaticMethod : public StaticMethodFunction {
public:
   DLLLOCAL BuiltinStaticMethod(const QoreClass* n_qc, const char* mname) : StaticMethodFunction(mname, n_qc) {
   }

   DLLLOCAL BuiltinStaticMethod(const BuiltinStaticMethod &old, const QoreClass* n_qc) : StaticMethodFunction(old, n_qc) {
   }

   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const {
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

   DLLLOCAL BuiltinDeleteBlocker(const BuiltinDeleteBlocker& old, const QoreClass* n_qc) : BuiltinNormalMethod(old, n_qc), delete_blocker(old.delete_blocker) {
   }

   DLLLOCAL bool eval(QoreObject* self, AbstractPrivateData* private_data) const {
      return delete_blocker(self, private_data);
   }

   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const {
      return new BuiltinDeleteBlocker(*this, n_qc);
   }
};

#define BDELB(f) (reinterpret_cast<BuiltinDeleteBlocker*>(f))

class NormalUserMethod : public NormalMethodFunction {
public:
   DLLLOCAL NormalUserMethod(const QoreClass* n_qc, const char* mname) : NormalMethodFunction(mname, n_qc) {
   }
   DLLLOCAL NormalUserMethod(const NormalUserMethod &old, const QoreClass* n_qc) : NormalMethodFunction(old, n_qc) {
   }
   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const {
      return new NormalUserMethod(*this, n_qc);
   }
};

class StaticUserMethod : public StaticMethodFunction {
public:
   DLLLOCAL StaticUserMethod(const QoreClass* n_qc, const char* mname) : StaticMethodFunction(mname, n_qc) {
   }
   DLLLOCAL StaticUserMethod(const StaticUserMethod &old, const QoreClass* n_qc) : StaticMethodFunction(old, n_qc) {
   }
   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const {
      return new StaticUserMethod(*this, n_qc);
   }
};

class QoreMemberInfoBase {
protected:
   const QoreTypeInfo* typeInfo;

   DLLLOCAL QoreMemberInfoBase(const QoreMemberInfoBase& old, ClassAccess n_access) : typeInfo(old.typeInfo), exp(old.exp ? old.exp->refSelf() : 0), loc(old.loc), parseTypeInfo(old.parseTypeInfo ? new QoreParseTypeInfo(*old.parseTypeInfo) : 0), access(old.access >= n_access ? old.access : n_access) {
   }

public:
   // initialization expression
   AbstractQoreNode* exp;

   // store parse location in case of errors
   QoreProgramLocation loc;
   QoreParseTypeInfo* parseTypeInfo;
   ClassAccess access;

   DLLLOCAL QoreMemberInfoBase(int nfl, int nll, const QoreTypeInfo* n_typeinfo, QoreParseTypeInfo* n_parseTypeInfo, AbstractQoreNode* e = 0, ClassAccess n_access = Public) :
      typeInfo(n_typeinfo), exp(e), loc(nfl, nll), parseTypeInfo(n_parseTypeInfo), access(n_access) {
   }
   DLLLOCAL QoreMemberInfoBase(int nfl, int nll, const QoreTypeInfo* n_typeinfo, AbstractQoreNode* e = 0, ClassAccess n_access = Public) : typeInfo(n_typeinfo), exp(e),
         loc(nfl, nll), parseTypeInfo(0), access(n_access) {
   }
   DLLLOCAL QoreMemberInfoBase(int nfl, int nll, char* n, AbstractQoreNode* e = 0, ClassAccess n_access = Public) : typeInfo(0), exp(e), loc(nfl, nll),
         parseTypeInfo(new QoreParseTypeInfo(n)), access(n_access) {
   }
   DLLLOCAL QoreMemberInfoBase(int nfl, int nll, const QoreClass* qc, AbstractQoreNode* e, ClassAccess n_access = Public) : typeInfo(qc->getTypeInfo()), exp(e),
         loc(nfl, nll), parseTypeInfo(0), access(n_access) {
   }
   DLLLOCAL QoreMemberInfoBase(int nfl, int nll, AbstractQoreNode* e, ClassAccess n_access = Public) : typeInfo(0), exp(e), loc(nfl, nll), parseTypeInfo(0), access(n_access) {
   }
   DLLLOCAL QoreMemberInfoBase(int nfl, int nll, ClassAccess n_access = Public) : typeInfo(0), exp(0), loc(nfl, nll), parseTypeInfo(0), access(n_access) {
   }

   DLLLOCAL ~QoreMemberInfoBase() {
      del();
   }

   DLLLOCAL void del() {
      if (exp) {
	 exp->deref(0);
         exp = 0;
      }
      if (parseTypeInfo) {
         delete parseTypeInfo;
         parseTypeInfo = 0;
      }
   }

   DLLLOCAL const QoreTypeInfo* getTypeInfo() const {
      return qore_check_this(this) ? typeInfo : 0;
   }

   DLLLOCAL bool parseHasTypeInfo() const {
      return qore_check_this(this) && (typeInfo || parseTypeInfo);
   }

   DLLLOCAL ClassAccess getAccess() const {
      return access;
   }
};

class QoreMemberInfo : public QoreMemberInfoBase {
   DLLLOCAL QoreMemberInfo(const QoreMemberInfo& old, const qore_class_private* n_qc, ClassAccess n_access = Public) : QoreMemberInfoBase(old, n_access), qc(old.qc ? old.qc : n_qc) {
   }

public:
   // class pointer in case member was imported from a base class
   const qore_class_private* qc;

   DLLLOCAL QoreMemberInfo(int nfl, int nll, const QoreTypeInfo* n_typeInfo, QoreParseTypeInfo* n_parseTypeInfo, AbstractQoreNode* e = 0, ClassAccess n_access = Public) : QoreMemberInfoBase(nfl, nll, n_typeInfo, n_parseTypeInfo, e, n_access), qc(0) {
   }

   DLLLOCAL QoreMemberInfo(int nfl, int nll, const QoreTypeInfo* n_typeInfo, AbstractQoreNode* e = 0, ClassAccess n_access = Public) : QoreMemberInfoBase(nfl, nll, n_typeInfo, e, n_access), qc(0) {
   }

   DLLLOCAL QoreMemberInfo(int nfl, int nll, char* n, AbstractQoreNode* e = 0, ClassAccess n_access = Public) : QoreMemberInfoBase(nfl, nll, n, e, n_access), qc(0) {
   }

   DLLLOCAL QoreMemberInfo(int nfl, int nll, const QoreClass* tqc, AbstractQoreNode* e, ClassAccess n_access = Public) : QoreMemberInfoBase(nfl, nll, tqc, e, n_access), qc(0) {
   }

   DLLLOCAL QoreMemberInfo(int nfl, int nll, AbstractQoreNode* e, ClassAccess n_access = Public) : QoreMemberInfoBase(nfl, nll, e, n_access), qc(0) {
   }

   DLLLOCAL QoreMemberInfo(int nfl, int nll, ClassAccess n_access = Public) : QoreMemberInfoBase(nfl, nll, n_access), qc(0) {
   }

   DLLLOCAL bool local() const {
      return qc ? false : true;
   }

   DLLLOCAL const qore_class_private* getClass(const qore_class_private* c) const {
      return qc ? qc : c;
   }

   DLLLOCAL QoreMemberInfo* copy(const qore_class_private* n_qc, ClassAccess n_access = Public) const {
      if (!qore_check_this(this))
         return 0;

      return new QoreMemberInfo(*this, n_qc, n_access);
   }

   DLLLOCAL void parseInit(const char* name, bool priv);
};

class QoreVarInfo : public QoreMemberInfoBase {
protected:
   DLLLOCAL QoreVarInfo(const QoreVarInfo& old, ClassAccess n_access = Public) : QoreMemberInfoBase(old, n_access), val(old.val), finalized(old.finalized) {
   }

   DLLLOCAL int checkFinalized(ExceptionSink* xsink) const {
         if (finalized) {
            xsink->raiseException("DESTRUCTOR-ERROR", "illegal class static variable assignment after second phase of variable destruction");
            return -1;
         }
         return 0;
      }

public:
   mutable QoreVarRWLock rwl;
   QoreLValueGeneric val;
   bool finalized;

   DLLLOCAL QoreVarInfo(int nfl, int nll, const QoreTypeInfo* n_typeinfo, QoreParseTypeInfo* n_parseTypeInfo, AbstractQoreNode* e = 0, ClassAccess n_access = Public) :
      QoreMemberInfoBase(nfl, nll, n_typeinfo, n_parseTypeInfo, e, n_access), finalized(false) {
   }

   DLLLOCAL QoreVarInfo(int nfl, int nll, ClassAccess n_access = Public) : QoreMemberInfoBase(nfl, nll, n_access), finalized(false) {
   }

   DLLLOCAL QoreVarInfo(int nfl, int nll, AbstractQoreNode* e, ClassAccess n_access = Public) : QoreMemberInfoBase(nfl, nll, e, n_access), finalized(false) {
   }

   DLLLOCAL ~QoreVarInfo() {
      assert(!val.hasValue());
   }

   DLLLOCAL void clear(ExceptionSink* xsink) {
      ReferenceHolder<> tmp(xsink);
      QoreAutoVarRWWriteLocker al(rwl);
      if (!finalized)
         finalized = true;
      tmp = val.removeNode(true);
   }

   DLLLOCAL void delVar(ExceptionSink* xsink) {
      del();
      discard(val.removeNode(true), xsink);
   }

   DLLLOCAL QoreVarInfo* copy() const {
      if (!qore_check_this(this))
         return 0;

      return new QoreVarInfo(*this);
   }

   DLLLOCAL AbstractQoreNode* assignInit(AbstractQoreNode* v) {
      // try to set an optimized value type for the value holder if possible
      val.set(getTypeInfo());
      return val.assignInitial(v);
   }

   DLLLOCAL void getLValue(LValueHelper& lvh) {
      lvh.setTypeInfo(getTypeInfo());
      lvh.setAndLock(rwl);
      if (checkFinalized(lvh.vl.xsink))
         return;
      lvh.setValue(val);
   }

   DLLLOCAL void init() {
      val.set(getTypeInfo());
#ifdef QORE_ENFORCE_DEFAULT_LVALUE
      // try to set an optimized value type for the value holder if possible
      discard(val.assignInitial(typeInfo->getDefaultQoreValue()), 0);
#endif
   }

   DLLLOCAL QoreValue getReferencedValue() const {
      QoreAutoVarRWReadLocker al(rwl);
      return val.getReferencedValue();
   }

   DLLLOCAL AbstractQoreNode* getReferencedNodeValue() const {
      QoreAutoVarRWReadLocker al(rwl);
      return val.getReferencedNodeValue();
   }

   DLLLOCAL int64 getAsBigInt() const {
      QoreAutoVarRWReadLocker al(rwl);
      return val.getAsBigInt();
   }

   DLLLOCAL double getAsFloat() const {
      QoreAutoVarRWReadLocker al(rwl);
      return val.getAsFloat();
   }

   DLLLOCAL bool getAsBool() const {
      QoreAutoVarRWReadLocker al(rwl);
      return val.getAsBool();
   }

   DLLLOCAL void parseInit(const char* name, bool priv);

#ifdef DEBUG
   DLLLOCAL bool empty() const {
      return !val.hasValue();
   }
#endif
};

#ifdef HAVE_QORE_HASH_MAP
typedef HASH_MAP<char*, QoreMemberInfo*, qore_hash_str, eqstr> member_map_t;
typedef HASH_MAP<char*, QoreVarInfo*, qore_hash_str, eqstr> var_map_t;
#else
typedef std::map<char*, QoreMemberInfo*, ltstr> member_map_t;
typedef std::map<char*, QoreVarInfo*, ltstr> var_map_t;
#endif

class QoreMemberMap {
public:
   typedef std::pair<char *, QoreMemberInfo*> list_element_t;
   typedef std::vector<list_element_t> member_list_t;
   typedef member_list_t::const_iterator DeclOrderIterator;
   typedef member_map_t::const_iterator SigOrderIterator;

public:
   DLLLOCAL QoreMemberMap() : inheritedCount(0) {
   }

   DLLLOCAL ~QoreMemberMap() {
      for (member_map_t::iterator i = map.begin(), e = map.end(); i != e; ++i) {
         //printd(5, "QoreMemberMap::~QoreMemberMap() this: %p freeing pending private member %p '%s'\n", this, i->second, i->first);
         delete i->second;
         free(i->first);
      }
      map.clear();
      list.clear();
   }

   DLLLOCAL bool inList(const char* name) const {
      return map.find(const_cast<char*>(name)) != map.end();
   }

   DLLLOCAL QoreMemberInfo *findByName(const char *name) const {
      member_map_t::const_iterator it = map.find(const_cast<char*>(name));
      return it == map.end() ? NULL : it->second;
   }

   DLLLOCAL bool empty() const {
      return map.empty();
   }

   DLLLOCAL void addNoCheck(char *name, QoreMemberInfo *info) {
      assert(name);
      assert(info);
      assert(!inList(name));
      map[name] = info;
      list.push_back(std::make_pair(name, info));
   }

   DLLLOCAL void addNoCheck(std::pair<char *, QoreMemberInfo *> pair) {
      addNoCheck(pair.first, pair.second);
   }

   DLLLOCAL void addInheritedNoCheck(char *name, QoreMemberInfo *info) {
      assert(name);
      assert(info);
      assert(!inList(name));
      map[name] = info;
      list.insert(list.begin() + inheritedCount++, std::make_pair(name, info));
   }

   DLLLOCAL void moveAllTo(QoreMemberMap &dest) {
      dest.map.insert(map.begin(), map.end());
      dest.list.insert(dest.list.end(), list.begin(), list.end());
      map.clear();
      list.clear();
   }

   DLLLOCAL void moveAllTo(QoreClass* qc, ClassAccess access);

   DLLLOCAL DeclOrderIterator beginDeclOrder() const {
      return list.begin();
   }

   DLLLOCAL DeclOrderIterator endDeclOrder() const {
      return list.end();
   }

   DLLLOCAL SigOrderIterator beginSigOrder() const {
      return map.begin();
   }

   DLLLOCAL SigOrderIterator endSigOrder() const {
      return map.end();
   }

private:
   member_list_t list;
   member_map_t map;
   member_list_t::size_type inheritedCount;
};

class QoreVarMap {
public:
   typedef std::pair<char *, QoreVarInfo*> list_element_t;
   typedef std::vector<list_element_t> var_list_t;
   typedef var_list_t::const_iterator DeclOrderIterator;
   typedef var_map_t::const_iterator SigOrderIterator;

public:
   DLLLOCAL ~QoreVarMap() {
      for (var_map_t::iterator i = map.begin(), e = map.end(); i != e; ++i) {
         //printd(5, "QoreVarMap::~QoreVarMap() deleting static var %s\n", i->first);
         assert(i->second->empty());
         i->second->del();
         free(i->first);
      }
   }

   DLLLOCAL void clear(ExceptionSink* xsink) {
      for (var_list_t::reverse_iterator i = list.rbegin(), e = list.rend(); i != e; ++i) {
         i->second->clear(xsink);
      }
   }

   DLLLOCAL void del(ExceptionSink* xsink) {
      for (var_list_t::reverse_iterator i = list.rbegin(), e = list.rend(); i != e; ++i) {
         i->second->delVar(xsink);
         free(i->first);
         delete i->second;
      }
      map.clear();
      list.clear();
   }

   DLLLOCAL void del() {
      for (var_map_t::iterator i = map.begin(), e = map.end(); i != e; ++i) {
         assert(!i->second->val.hasValue());
         free(i->first);
         delete i->second;
      }
      map.clear();
      list.clear();
   }

   DLLLOCAL bool inList(const char* name) const {
      return map.find(const_cast<char*>(name)) != map.end();
   }

   DLLLOCAL QoreVarInfo* find(const char* name) const {
      var_map_t::const_iterator i = map.find(const_cast<char*>(name));
      return i == map.end() ? 0 : i->second;
   }

   DLLLOCAL void addNoCheck(char* name, QoreVarInfo* info) {
      assert(name);
      assert(info);
      assert(!inList(name));
      map[name] = info;
      list.push_back(std::make_pair(name, info));
   }

   DLLLOCAL void addNoCheck(std::pair<char*, QoreVarInfo*> pair) {
      addNoCheck(pair.first, pair.second);
   }

   DLLLOCAL bool empty() const {
      return map.empty();
   }

   DLLLOCAL void clearNoFree() {
      map.clear();
      list.clear();
   }

   DLLLOCAL void moveAllTo(QoreClass* qc, ClassAccess access);

   DLLLOCAL DeclOrderIterator beginDeclOrder() const {
      return list.begin();
   }

   DLLLOCAL DeclOrderIterator endDeclOrder() const {
      return list.end();
   }

   DLLLOCAL SigOrderIterator beginSigOrder() const {
      return map.begin();
   }

   DLLLOCAL SigOrderIterator endSigOrder() const {
      return map.end();
   }

private:
   var_list_t list;
   var_map_t map;
};

/*
  BCANode
  base class constructor argument node
*/
class BCANode : public FunctionCallBase {
public:
   // set automatically when created
   QoreProgramLocation loc;
   qore_classid_t classid;
   //QoreClass* sclass;
   NamedScope* ns;
   char* name;

   // this function takes ownership of n and arg
   DLLLOCAL BCANode(NamedScope* n, QoreListNode* n_args) : FunctionCallBase(n_args), loc(ParseLocation), classid(0), ns(n), name(0) {
   }

   // this function takes ownership of n and arg
   DLLLOCAL BCANode(char* n, QoreListNode* n_args) : FunctionCallBase(n_args), loc(ParseLocation), classid(0), ns(0), name(n) {
   }

   DLLLOCAL ~BCANode() {
      delete ns;
      if (name)
	 free(name);
   }

   // resolves classes, parses arguments, and attempts to find constructor variant
   DLLLOCAL void parseInit(BCList* bcl, const char* classname);
};

typedef std::vector<BCANode*> bcalist_t;

// BCAList
// base class constructor argument list
// this data structure will not be modified even if the class is copied
// to a subprogram object
class BCAList : public bcalist_t {
public:
   DLLLOCAL BCAList(BCANode* n) {
      push_back(n);
   }

   DLLLOCAL ~BCAList() {
      for (bcalist_t::iterator i = begin(), e = end(); i != e; ++i)
         delete *i;
   }

   // returns 0 for no errors, -1 for exception raised
   DLLLOCAL int execBaseClassConstructorArgs(BCEAList* bceal, ExceptionSink* xsink) const;
};

typedef std::pair<QoreClass*, bool> class_virt_pair_t;
//typedef std::list<class_virt_pair_t> class_list_t;
typedef std::vector<class_virt_pair_t> class_list_t;

// BCSMList: Base Class Special Method List
// unique list of base classes for a class hierarchy to ensure that "special" methods, constructor(), destructor(), copy() - are executed only once
// this class also tracks virtual classes to ensure that they are not inserted into the list in a complex tree and executed here
class BCSMList : public class_list_t {
public:
   DLLLOCAL BCSMList() {
   }

   DLLLOCAL BCSMList(const BCSMList &old);

   DLLLOCAL ~BCSMList();

   DLLLOCAL int add(QoreClass* thisclass, QoreClass* qc, bool is_virtual);
   DLLLOCAL int addBaseClassesToSubclass(QoreClass* thisclass, QoreClass* sc, bool is_virtual);

   DLLLOCAL void alignBaseClassesInSubclass(QoreClass* thisclass, QoreClass* child, bool is_virtual);

   // returns 0 = can add, non-0 = cannot add
   DLLLOCAL void align(QoreClass* thisclass, QoreClass* qc, bool is_virtual);

   DLLLOCAL QoreClass* getClass(qore_classid_t cid) const;
   //DLLLOCAL void execConstructors(QoreObject* o, BCEAList* bceal, ExceptionSink* xsink) const;
   DLLLOCAL void execDestructors(QoreObject* o, ExceptionSink* xsink) const;
   DLLLOCAL void execSystemDestructors(QoreObject* o, ExceptionSink* xsink) const;
   DLLLOCAL void execCopyMethods(QoreObject* self, QoreObject* old, ExceptionSink* xsink) const;

   // parseResolve classes to the new class pointer after all namespaces and classes have been copied
   DLLLOCAL void resolveCopy();
};

// set of private class pointers; used when checking for recursive class inheritance lists
typedef std::set<qore_class_private*> qcp_set_t;

// BCNode
// base class pointer
class BCNode {
public:
   // populated automatically on creation
   QoreProgramLocation loc;
   NamedScope* cname;
   char* cstr;
   QoreClass* sclass;
   ClassAccess access;
   bool is_virtual : 1;

   DLLLOCAL BCNode(NamedScope* c, ClassAccess a) : loc(ParseLocation), cname(c), cstr(0), sclass(0), access(a), is_virtual(false) {
   }

   // this method takes ownership of *str
   DLLLOCAL BCNode(char* str, ClassAccess a) : loc(ParseLocation), cname(0), cstr(str), sclass(0), access(a), is_virtual(false) {
   }

   // for builtin base classes
   DLLLOCAL BCNode(QoreClass* qc, bool n_virtual = false)
      : loc(ParseLocation), cname(0), cstr(0), sclass(qc), access(Public), is_virtual(n_virtual) {
   }

   // called at runtime with committed classes
   DLLLOCAL BCNode(const BCNode &old) : loc(old.loc), cname(0), cstr(0), sclass(old.sclass), access(old.access), is_virtual(old.is_virtual) {
      assert(!old.cname);
      assert(!old.cstr);
      assert(old.sclass);
   }

   DLLLOCAL ~BCNode() {
      delete cname;
      if (cstr)
         free(cstr);
   }

   DLLLOCAL ClassAccess getAccess() const { return access; }

   // returns -1 if a recursive reference is found, 0 if not
   DLLLOCAL int initialize(QoreClass* cls, bool& has_delete_blocker, qcp_set_t& qcp_set);

   DLLLOCAL bool isBaseClass(QoreClass* qc, bool toplevel) const;

   DLLLOCAL const QoreMemberInfo* runtimeGetMemberInfo(const char* mem, ClassAccess& n_access, const qore_class_private* class_ctx, bool allow_internal) const;
   DLLLOCAL const qore_class_private* runtimeGetMemberClass(const char* mem, ClassAccess& n_access, const qore_class_private* class_ctx, bool allow_internal) const;

   DLLLOCAL const QoreMethod* runtimeFindCommittedMethod(const char* name, ClassAccess& n_access, const qore_class_private* class_ctx, bool allow_internal) const;
   DLLLOCAL const QoreMethod* runtimeFindCommittedStaticMethod(const char* name, ClassAccess& n_access, const qore_class_private* class_ctx, bool allow_internal) const;

   DLLLOCAL bool runtimeIsPrivateMember(const char* str, bool toplevel) const;

   DLLLOCAL int runtimeInitInternalMembers(QoreObject& o, bool& need_scan, ExceptionSink* xsink) const;

   DLLLOCAL void execConstructors(QoreObject* o, BCEAList* bceal, ExceptionSink* xsink) const;

   DLLLOCAL const QoreMemberInfo* parseFindMember(const char* mem, const qore_class_private*& qc, ClassAccess& n_access, bool toplevel) const;
   DLLLOCAL const QoreVarInfo* parseFindVar(const char* name, const qore_class_private*& qc, ClassAccess& n_access, bool toplevel) const;

   DLLLOCAL const QoreClass* getClass(qore_classid_t cid, ClassAccess& n_access, bool toplevel) const;
   DLLLOCAL const QoreClass* getClass(const qore_class_private& qc, ClassAccess& n_access, bool toplevel) const;
   DLLLOCAL const QoreClass* parseGetClass(const qore_class_private& qc, ClassAccess& n_access, bool toplevel) const;

   // inaccessible methods are ignored
   DLLLOCAL const QoreMethod* parseFindNormalMethod(const char* name, const qore_class_private* class_ctx, bool allow_internal) const;

   // inaccessible methods are ignored
   DLLLOCAL const QoreMethod* parseFindStaticMethod(const char* name, const qore_class_private* class_ctx, bool allow_internal) const;

   // inaccessible methods are ignored
   DLLLOCAL const QoreMethod* parseResolveSelfMethod(const char* name, const qore_class_private* class_ctx, bool allow_internal) const;

   DLLLOCAL bool parseCheckHierarchy(const QoreClass* cls, ClassAccess& n_access, bool toplevel) const;

   DLLLOCAL QoreVarInfo* parseFindStaticVar(const char* vname, const QoreClass*& qc, ClassAccess& n_access, bool check, bool toplevel) const;

   DLLLOCAL AbstractQoreNode* parseFindConstantValue(const char* cname, const QoreTypeInfo*& typeInfo, const qore_class_private* class_ctx, bool allow_internal) const;

   DLLLOCAL int addBaseClassesToSubclass(QoreClass* child, bool is_virtual);
};

typedef std::vector<BCNode*> bclist_t;

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
   bool valid = true,
      rescanned = false;

   DLLLOCAL BCList(BCNode* n) {
      push_back(n);
   }

   DLLLOCAL BCList() {
   }

   DLLLOCAL BCList(const BCList& old) : sml(old.sml) {
      assert(old.valid);
      reserve(old.size());
      for (bclist_t::const_iterator i = old.begin(), e = old.end(); i != e; ++i)
         push_back(new BCNode(*(*i)));
   }

   DLLLOCAL ~BCList() {
      for (bclist_t::iterator i = begin(), e = end(); i != e; ++i)
         delete *i;
   }

   DLLLOCAL int initialize(QoreClass* thisclass, bool& has_delete_blocker, qcp_set_t& qcp_set);

   // inaccessible methods are ignored
   DLLLOCAL const QoreMethod* parseResolveSelfMethod(const char* name, const qore_class_private* class_ctx, bool allow_internal);

   // inaccessible methods are ignored
   DLLLOCAL const QoreMethod* parseFindNormalMethod(const char* name, const qore_class_private* class_ctx, bool allow_internal);
   // inaccessible methods are ignored
   DLLLOCAL const QoreMethod* parseFindStaticMethod(const char* name, const qore_class_private* class_ctx, bool allow_internal);

   DLLLOCAL const QoreMethod* runtimeFindCommittedMethod(const char* name, ClassAccess& access, const qore_class_private* class_ctx, bool allow_internal) const;
   DLLLOCAL const QoreMethod* runtimeFindCommittedStaticMethod(const char* name, ClassAccess& access, const qore_class_private* class_ctx, bool allow_internal) const;

   DLLLOCAL const QoreMemberInfo* runtimeGetMemberInfo(const char* mem, ClassAccess& access, const qore_class_private* class_ctx, bool allow_internal) const;
   DLLLOCAL const qore_class_private* runtimeGetMemberClass(const char* mem, ClassAccess& access, const qore_class_private* class_ctx, bool allow_internal) const;

   DLLLOCAL int runtimeInitInternalMembers(QoreObject& o, bool& need_scan, ExceptionSink* xsink) const;

   DLLLOCAL bool match(const QoreClass* cls);
   DLLLOCAL void execConstructors(QoreObject* o, BCEAList* bceal, ExceptionSink* xsink) const;
   DLLLOCAL bool execDeleteBlockers(QoreObject* o, ExceptionSink* xsink) const;

   DLLLOCAL bool runtimeIsPrivateMember(const char* str, bool toplevel) const;

   DLLLOCAL bool parseCheckHierarchy(const QoreClass* cls, ClassAccess& access, bool toplevel) const;

   DLLLOCAL const QoreMemberInfo* parseFindMember(const char* mem, const qore_class_private*& qc, ClassAccess& n_access, bool toplevel) const;

   DLLLOCAL const QoreVarInfo* parseFindVar(const char* vname, const qore_class_private*& qc, ClassAccess& access, bool toplevel) const;

   DLLLOCAL bool parseHasPublicMembersInHierarchy() const;

   DLLLOCAL const QoreClass* getClass(qore_classid_t cid, ClassAccess& n_access, bool toplevel) const;
   DLLLOCAL const QoreClass* getClass(const qore_class_private& qc, ClassAccess& n_access, bool toplevel) const;

   DLLLOCAL const QoreClass* parseGetClass(const qore_class_private& qc, ClassAccess& n_access, bool toplevel) const;

   DLLLOCAL void addNewAncestors(QoreMethod* m);
   DLLLOCAL void addAncestors(QoreMethod* m);
   DLLLOCAL void addNewStaticAncestors(QoreMethod* m);
   DLLLOCAL void addStaticAncestors(QoreMethod* m);
   DLLLOCAL void parseAddAncestors(QoreMethod* m);
   DLLLOCAL void parseAddStaticAncestors(QoreMethod* m);

   DLLLOCAL AbstractQoreNode* parseFindConstantValue(const char* cname, const QoreTypeInfo*& typeInfo, const qore_class_private* class_ctx, bool allow_internal) const;

   DLLLOCAL QoreVarInfo* parseFindStaticVar(const char* vname, const QoreClass*& qc, ClassAccess& access, bool check, bool toplevel) const;

   DLLLOCAL void resolveCopy();

   DLLLOCAL MethodVariantBase* matchNonAbstractVariant(const std::string& name, MethodVariantBase* v) const;

   DLLLOCAL bool isBaseClass(QoreClass* qc, bool toplevel) const;

   DLLLOCAL int addBaseClassesToSubclass(QoreClass* thisparent, QoreClass* child, bool is_virtual) {
      for (auto& i : *this) {
         if ((*i).addBaseClassesToSubclass(child, is_virtual))
            return -1;
      }
      return sml.addBaseClassesToSubclass(thisparent, child, is_virtual);
   }

   DLLLOCAL void rescanParents(QoreClass* cls);
};

// BCEANode
// base constructor evaluated argument node; created locally at run time
class BCEANode {
public:
   QoreListNode* args;
   const AbstractQoreFunctionVariant* variant;
   bool execed;
   bool member_init_done;

   DLLLOCAL BCEANode(QoreListNode* n_args, const AbstractQoreFunctionVariant* n_variant) : args(n_args), variant(reinterpret_cast<const MethodVariant*>(n_variant)), execed(false), member_init_done(false) {}
   DLLLOCAL BCEANode(bool n_execed = true, bool mid = true) : args(0), variant(0), execed(n_execed), member_init_done(mid) {}
};

/*
struct ltqc {
   bool operator()(const QoreClass* qc1, const QoreClass* qc2) const {
      return qc1 < qc2;
   }
};
*/

typedef std::map<qore_classid_t, BCEANode*> bceamap_t;

/*
  BCEAList
  base constructor evaluated argument list
*/
class BCEAList : public bceamap_t {
protected:
   DLLLOCAL ~BCEAList() {
      assert(empty());
   }

public:
   DLLLOCAL void deref(ExceptionSink* xsink);
   // evaluates arguments, returns -1 if an exception was thrown
   DLLLOCAL int add(qore_classid_t classid, const QoreListNode* arg, const AbstractQoreFunctionVariant* variant, QoreProgramLocation& loc, ExceptionSink* xsink);
   DLLLOCAL QoreListNode* findArgs(qore_classid_t classid, bool* aexeced, const AbstractQoreFunctionVariant*& variant);
   /*
   DLLLOCAL bool initMembers(qore_classid_t classid) {
      bceamap_t::iterator i = lower_bound(classid);
      if (i == end() || i->first != classid) {
         insert(i, bceamap_t::value_type(classid, new BCEANode(false, true)));
         return false;
      }
      if (!i->second->member_init_done) {
         i->second->member_init_done = true;
         return false;
      }
      return true;
   }
   */
};

struct SelfInstantiatorHelper {
   LocalVar* selfid;
   DLLLOCAL SelfInstantiatorHelper(LocalVar* n_selfid, QoreObject* self) : selfid(n_selfid) {
      selfid->instantiateSelf(self);
   }
   DLLLOCAL ~SelfInstantiatorHelper() {
      selfid->uninstantiateSelf();
   }
};

// signature hash size - we use SHA1 for performance reasons (and because we don't necessarily need the best cryptographic security)
#define SH_SIZE 20

// class "signature" hash for comparing classes with the same name from different program objects at runtime
class SignatureHash {
protected:
   unsigned char buf[SH_SIZE];
   bool is_set;

   DLLLOCAL void set(const QoreString& str);

   DLLLOCAL void clearHash() {
      memset(buf, 0, SH_SIZE);
   }

   DLLLOCAL void copyHash(const SignatureHash& other) {
      memcpy(buf, other.buf, SH_SIZE);
   }

public:
   DLLLOCAL SignatureHash() : is_set(false) {
      clearHash();
   }

   DLLLOCAL SignatureHash(const SignatureHash& old) : is_set(old.is_set) {
      if (is_set)
         copyHash(old);
   }

   DLLLOCAL void update(const QoreString& str);

   DLLLOCAL bool operator==(const SignatureHash& other) const {
      // if either one of the hashes is not set, then the comparison always fails
      if (!is_set || !other.is_set)
         return false;
      return !memcmp(buf, other.buf, SH_SIZE);
   }

   DLLLOCAL SignatureHash& operator=(const SignatureHash& other) {
      if (!other.is_set) {
         assert(false);
         clear();
      }
      else {
         if (!is_set)
            is_set = true;
         copyHash(other);
      }
      return *this;
   }

   DLLLOCAL operator bool() const {
      return is_set;
   }

   // appends the hash to the string
   DLLLOCAL void toString(QoreString& str) const {
      for (unsigned i = 0; i < SH_SIZE; ++i)
         str.sprintf("%02x", buf[i]);
   }

   DLLLOCAL char* getHash() const {
      assert(is_set);
      return (char*)buf;
   }

   DLLLOCAL void clear() {
      if (is_set) {
         is_set = false;
         clearHash();
      }
   }
};

#define QCCM_NORMAL (1 << 0)
#define QCCM_STATIC (1 << 1)

// private QoreClass implementation
class qore_class_private {
public:
   std::string name;             // the name of the class
   QoreClass* cls;               // parent class
   qore_ns_private* ns;          // parent namespace
   BCList* scl;                  // base class list

   mutable VRMutex gate;                 // for synchronized static methods

   hm_method_t hm,               // "normal" (non-static) method map
      shm;                       // static method map

   AbstractMethodMap ahm;        // holds abstract variants with no implementation in the current class

   ConstantList pend_constlist,  // pending constants
      constlist;                 // committed constants

   // member lists (maps)
   QoreMemberMap members, pending_members;

   // static var lists (maps)
   QoreVarMap vars, pending_vars;

   const QoreMethod* system_constructor, *constructor, *destructor,
      *copyMethod, *methodGate, *memberGate, *deleteBlocker,
      *memberNotification;

   qore_classid_t classID,          // class ID
      methodID;                     // for subclasses of builtin classes that will not have their own private data,
                                    // instead they will get the private data from this class

   bool sys : 1,                        // system/builtin class?
      initialized : 1,                  // is initialized? (only performed once)
      parse_init_called : 1,            // has parseInit() been called? (performed once for each parseCommit())
      parse_init_partial_called : 1,    // has parseInitPartial() been called? (performed once for each parseCommit())
      has_delete_blocker : 1,           // has a delete_blocker function somewhere in the hierarchy?
      has_public_memdecl : 1,           // has a public member declaration somewhere in the hierarchy?
      pending_has_public_memdecl : 1,   // has a pending public member declaration in this class?
      owns_typeinfo : 1,                // do we own the typeInfo data or not?
      resolve_copy_done : 1,            // has the copy already been resolved
      has_new_user_changes : 1,         // does the class have new user code that needs to be processed?
      has_sig_changes : 1,              // does the class have code affecting the signature to be processed?
      owns_ornothingtypeinfo : 1,       // do we own the "or nothing" type info
      pub : 1,                          // is a public class (modules only)
      final : 1,                        // is the class "final" (cannot be inherited)
      inject : 1                        // has the class been injected
      ;

   int64 domain;                    // capabilities of builtin class to use in the context of parse restrictions
   mutable QoreReferenceCounter refs;       // references

   unsigned num_methods, num_user_methods, num_static_methods, num_static_user_methods;

   // type information for the class, may not have a pointer to the same QoreClass
   // as the actual owning class in case of a copy
   QoreTypeInfo* typeInfo,
      *orNothingTypeInfo;

   // common "self" local variable for all constructors
   mutable LocalVar selfid;

   // class "signature" hash for comparing classes with the same name from different program objects at runtime
   SignatureHash hash, pend_hash;

   // user-specific data
   const void* ptr;

   // managed user-specific data
   AbstractQoreClassUserData* mud;

   // pointer to new class when copying
   mutable QoreClass* new_copy;

   // pointer to owning program for imported classes
   QoreProgram* spgm;

   DLLLOCAL qore_class_private(QoreClass* n_cls, const char* nme, int64 dom = QDOM_DEFAULT, QoreTypeInfo* n_typeinfo = 0);

   // only called while the parse lock for the QoreProgram owning "old" is held
   DLLLOCAL qore_class_private(const qore_class_private &old, QoreClass* n_cls);

   DLLLOCAL ~qore_class_private();

   DLLLOCAL void ref() const {
      refs.ROreference();
   }

   DLLLOCAL bool deref() {
      if (refs.ROdereference()) {
         delete cls;
         return true;
      }
      return false;
   }

   DLLLOCAL bool hasAbstract() const {
      return !ahm.empty();
   }

   DLLLOCAL int runtimeCheckInstantiateClass(ExceptionSink* xsink) const {
      return ahm.runtimeCheckInstantiateClass(name.c_str(), xsink);
   }

   DLLLOCAL void parseCheckAbstractNew() {
      parseInit();
      ahm.parseCheckAbstractNew(name.c_str());
   }

   DLLLOCAL void setNamespace(qore_ns_private* n) {
      ns = n;
   }

   DLLLOCAL void resolveCopy();

   DLLLOCAL void setUserData(const void* n_ptr) {
      assert(!ptr);
      ptr = n_ptr;
   }

   DLLLOCAL const void* getUserData() const {
      return ptr;
   }

   DLLLOCAL void setManagedUserData(AbstractQoreClassUserData* n_mud) {
      assert(!mud);
      mud = n_mud;
   }

   DLLLOCAL AbstractQoreClassUserData* getManagedUserData() const {
      return mud;
   }

   DLLLOCAL const QoreTypeInfo* getTypeInfo() const {
      return typeInfo;
   }

   DLLLOCAL const QoreTypeInfo* getOrNothingTypeInfo() const {
      return orNothingTypeInfo;
   }

   DLLLOCAL bool runtimeIsPrivateMemberIntern(const char* str, bool toplevel) const;

   DLLLOCAL void parseImportMembers(qore_class_private& qc, ClassAccess access);

   DLLLOCAL bool parseHasMemberGate() const {
      return memberGate || hm.find("memberGate") != hm.end();
   }

   DLLLOCAL bool parseHasMethodGate() const {
      return methodGate || hm.find("methodGate") != hm.end();
   }

   // checks for all special methods except constructor & destructor
   DLLLOCAL bool checkAssignSpecialIntern(const QoreMethod* m) {
      // set quick pointers
      if (!methodGate && !strcmp(m->getName(), "methodGate")) {
	 methodGate = m;
	 return true;
      }

      if (!memberGate && !strcmp(m->getName(), "memberGate")) {
	 memberGate = m;
	 //printd(5, "qore_class_private::checkAssignSpecialIntern() this: %p got %s::%s()\n", this, name.c_str(), m->getName());
	 return true;
      }

      if (!memberNotification && !strcmp(m->getName(), "memberNotification")) {
	 memberNotification = m;
	 return true;
      }

      return false;
   }

   // checks for all special methods except constructor, destructor, and copy
   DLLLOCAL bool checkSpecialStaticIntern(const char* mname) {
      // set quick pointers
      if ((!methodGate && !strcmp(mname, "methodGate"))
	  || (!memberGate && !strcmp(mname, "memberGate"))
	  || (!memberNotification && !strcmp(mname, "memberNotification")))
	 return true;
      return false;
   }

   // checks for all special methods
   DLLLOCAL bool checkSpecial(const char* mname) {
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
   DLLLOCAL bool checkAssignSpecial(const QoreMethod* m) {
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

   // returns -1 if a recursive inheritance list was found, 0 if not
   DLLLOCAL int initializeIntern(qcp_set_t& qcp_set);
   DLLLOCAL void initialize();

   DLLLOCAL void parseInitPartial();
   DLLLOCAL void parseInitPartialIntern();

   DLLLOCAL int parseCheckMemberAccess(const QoreProgramLocation& loc, const char* mem, const QoreTypeInfo*& memberTypeInfo, int pflag) const {
      const_cast<qore_class_private*>(this)->parseInitPartial();

      const qore_class_private* qc = 0;
      ClassAccess access;
      const QoreMemberInfo* omi = parseFindMember(mem, qc, access);

      if (!omi) {
	 int rc = 0;
	 if (!parseHasMemberGate() || (pflag & PF_FOR_ASSIGNMENT)) {
	    if (parse_check_parse_option(PO_REQUIRE_TYPES)) {
	       parse_error(loc, "member '%s' of class '%s' referenced has no type information because it was not declared in a public or private member list, but parse options require type information for all declarations",
	             mem, name.c_str());
	       rc = -1;
	    }
	    if (parseHasPublicMembersInHierarchy()) {
	       //printd(5, "qore_class_private::parseCheckMemberAccess() %s %%.%s memberGate: %d pflag: %d\n", name.c_str(), mem, parseHasMemberGate(), pflag);
	       parse_error(loc, "illegal access to unknown member '%s' in class '%s' which has a public member list (or inherited public member list)", mem, name.c_str());
	       rc = -1;
	    }
	 }
	 return rc;
      }

      memberTypeInfo = omi->getTypeInfo();

      // only raise a parse error for illegal access to private members if there is not memberGate function
      if ((access > Public) && !parseHasMemberGate() && !parseCheckPrivateClassAccess()) {
	 memberTypeInfo = 0;
         parse_error(loc, "illegal access to private member '%s' of class '%s'", mem, name.c_str());
	 return -1;
      }
      return 0;
   }

   DLLLOCAL int parseResolveInternalMemberAccess(const char* mem, const QoreTypeInfo*& memberTypeInfo) const {
      const_cast<qore_class_private*>(this)->parseInitPartial();

      const qore_class_private* qc = 0;
      ClassAccess access;
      const QoreMemberInfo* omi = parseFindMember(mem, qc, access);
      if (omi)
         memberTypeInfo = omi->getTypeInfo();

      return omi ? 0 : -1;
   }

   DLLLOCAL int parseCheckInternalMemberAccess(const char* mem, const QoreTypeInfo*& memberTypeInfo, const QoreProgramLocation& loc) const {
      const_cast<qore_class_private*>(this)->parseInitPartial();

      // throws a parse exception if there are public members and the name is not valid
      const qore_class_private* qc = 0;
      ClassAccess access;
      const QoreMemberInfo* omi = parseFindMember(mem, qc, access);
      if (omi)
         memberTypeInfo = omi->getTypeInfo();

      int rc = 0;
      if (!omi) {
	 if (parse_check_parse_option(PO_REQUIRE_TYPES)) {
	    parse_error(loc, "member '%s' of class '%s' referenced has no type information because it was not declared in a public or private member list, but parse options require type information for all declarations", mem, name.c_str());
	    rc = -1;
	 }
	 if (parseHasPublicMembersInHierarchy()) {
            parse_error(loc, "illegal access to unknown member '%s' in class '%s' which has a public member list (or inherited public member list)", mem, name.c_str());
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

   DLLLOCAL bool runtimeIsMemberInternal(const char* mem) const {
      QoreMemberInfo* info = members.findByName(mem);
      return info && info->getAccess() == Internal ? true : false;
   }

   // class_ctx is only set if it is present and accessible, so we only need to check for internal access here
   DLLLOCAL const QoreMemberInfo* runtimeGetMemberInfo(const char* mem, ClassAccess& access, const qore_class_private* class_ctx, bool& internal_member) const {
      if (class_ctx) {
         QoreMemberInfo *info = class_ctx->members.findByName(mem);
         if (info && info->getAccess() == Internal) {
            internal_member = true;
            access = Internal;
            return info;
         }
      }
      access = Public;
      internal_member = false;

      return runtimeGetMemberInfoIntern(mem, access, class_ctx);
   }

   // class_ctx is only set if it is present and accessible, so we only need to check for internal access here
   DLLLOCAL const QoreMemberInfo* runtimeGetMemberInfoIntern(const char* mem, ClassAccess& access, const qore_class_private* class_ctx) const {
      QoreMemberInfo *info = members.findByName(mem);
      if (info) {
         ClassAccess ma = info->getAccess();
         if (ma != Internal) {
            if (access < ma)
               access = ma;
            return info;
         }
      }

      return scl ? scl->runtimeGetMemberInfo(mem, access, class_ctx, class_ctx && is_equal(*class_ctx)) : 0;
   }

   DLLLOCAL const QoreMemberInfo* parseFindMember(const char* mem, const qore_class_private*& qc, ClassAccess& access) const {
      access = Public;
      const_cast<qore_class_private*>(this)->initialize();
      return parseFindMemberNoInit(mem, qc, access, true);
   }

   DLLLOCAL const QoreMemberInfo* parseFindLocalPublicPrivateMemberNoInit(const char* mem) const {
      QoreMemberInfo *info = members.findByName(mem);
      if (!info)
         info = pending_members.findByName(mem);

      return info;
   }

   DLLLOCAL const QoreMemberInfo* parseFindMemberNoInit(const char* mem, const qore_class_private*& qc, ClassAccess& access, bool toplevel) const {
      const QoreMemberInfo* mi = parseFindLocalPublicPrivateMemberNoInit(mem);
      if (mi) {
         ClassAccess ma = mi->getAccess();
         if (toplevel || ma != Internal) {
            if (access < ma)
               access = ma;
            qc = mi->getClass(this);
            return mi;
         }
      }

      return scl ? scl->parseFindMember(mem, qc, access, true) : 0;
   }

   DLLLOCAL const QoreVarInfo* parseFindVar(const char* vname, const qore_class_private*& qc, ClassAccess& access, bool toplevel) const {
      //printd(5, "parseFindVar() this: %p cls: %p (%s) scl: %p\n", this, cls, cls->getName(), scl);

      QoreVarInfo* vi = vars.find(const_cast<char*>(vname));
      if (!vi)
	 vi = pending_vars.find(const_cast<char*>(vname));

      if (vi) {
         qc = this;
         access = vi->getAccess();
	 return vi;
      }

      return scl ? scl->parseFindVar(vname, qc, access, toplevel) : 0;
   }

   DLLLOCAL int parseCheckClassHierarchyMembers(const char* mname, const QoreMemberInfo& l_mi, const qore_class_private& b_qc, const QoreMemberInfo& b_mi);

   DLLLOCAL int checkExistingVarMember(const char* dname, const QoreMemberInfoBase* mi, const QoreMemberInfoBase* omi, const qore_class_private* qc, ClassAccess oaccess, bool var = false) const;

   DLLLOCAL int parseCheckVar(const char* dname, const QoreVarInfo* vi) const {
      const qore_class_private* qc = 0;
      ClassAccess access;
      const QoreVarInfo* ovi = parseFindVar(dname, qc, access, true);
      //printd(5, "parseCheckVar() %s cls: %p (%s)\n", dname, sclass, sclass ? sclass->getName() : "n/a");
      if (!ovi) {
         if (parseHasConstant(dname)) {
            parse_error("'%s' has already been declared as a constant in class '%s' and therefore cannot be also declared as a static class variable in the same class with the same name", dname, name.c_str());
            return -1;
         }
	 return 0;
      }

      return checkExistingVarMember(dname, vi, ovi, qc, access, true);
   }

   DLLLOCAL int parseCheckMember(const char* mem, const QoreMemberInfo* mi) const {
      const qore_class_private* qc = 0;
      ClassAccess access = Public;
      const QoreMemberInfo* omi = parseFindMemberNoInit(mem, qc, access, true);
      if (!omi)
         return 0;

      return checkExistingVarMember(mem, mi, omi, qc, omi->access);
   }

   DLLLOCAL int parseCheckMemberInBaseClasses(const char* mem, const QoreMemberInfo* mi) const {
      const qore_class_private* qc = 0;
      ClassAccess access = Public;
      const QoreMemberInfo* omi = scl ? scl->parseFindMember(mem, qc, access, true) : 0;
      if (!omi || (omi->getClass(qc) == mi->getClass(this)))
	 return 0;

      return checkExistingVarMember(mem, mi, omi, qc, omi->access);
   }

   DLLLOCAL void parseAddMember(char* mem, ClassAccess access, QoreMemberInfo* MemberInfo) {
      MemberInfo->access = access;
      if (!parseCheckMember(mem, MemberInfo)) {
         if (!has_new_user_changes)
            has_new_user_changes = true;
         if (!has_sig_changes)
            has_sig_changes = true;
         //printd(5, "qore_class_private::parseAddMember() this: %p %s adding %s %p %s\n", this, name.c_str(), privpub(access), mem, mem);
         pending_members.addNoCheck(mem, MemberInfo);
         return;
      }

      free(mem);
      delete MemberInfo;
   }

   DLLLOCAL void parseAddStaticVar(char* dname, ClassAccess access, QoreVarInfo* VarInfo) {
      VarInfo->access = access;
      if (!parseCheckVar(dname, VarInfo)) {
         //printd(5, "qore_class_private::parseAddStaticVar() this: %p %s adding %p %s\n", this, name.c_str(), mem, mem);
         pending_vars.addNoCheck(dname, VarInfo);
         return;
      }

      free(dname);
      delete VarInfo;
   }

   DLLLOCAL void addBuiltinConstant(const char* cname, AbstractQoreNode* value, ClassAccess access = Public, const QoreTypeInfo* cTypeInfo = 0) {
      assert(!constlist.inList(cname));
      assert(!pend_constlist.inList(cname));
      constlist.add(cname, value, cTypeInfo, access);
   }

   DLLLOCAL void addBuiltinStaticVar(const char* vname, AbstractQoreNode* value, ClassAccess access = Public, const QoreTypeInfo* vTypeInfo = 0) {
      assert(!vars.inList(vname));

      vars.addNoCheck(strdup(vname), new QoreVarInfo(0, 0, vTypeInfo, 0, value, access));
   }

   DLLLOCAL void parseAssimilateConstants(ConstantList &cmap, ClassAccess access) {
      if (!has_new_user_changes)
         has_new_user_changes = true;
      if (!has_sig_changes)
         has_sig_changes = true;

      // set access if necessary
      cmap.setAccess(access);
      pend_constlist.assimilate(cmap, constlist, "class", name.c_str());
   }

   DLLLOCAL void parseAddConstant(const std::string &cname, AbstractQoreNode* val, ClassAccess access) {
      if (parseHasVar(cname.c_str())) {
         parse_error("'%s' has already been declared as a static variable in class '%s' and therefore cannot be also declared as a constant in the same class with the same name", cname.c_str(), name.c_str());
         val->deref(0);
         return;
      }
      if (!has_new_user_changes)
         has_new_user_changes = true;
      if (!has_sig_changes)
         has_sig_changes = true;

      //printd(5, "parseAddConstant() this: %p cls: %p const: %s access: %d\n", this, cls, cname.c_str(), access);

      pend_constlist.parseAdd(cname, val, constlist, access, name.c_str());
   }

   DLLLOCAL bool parseHasVar(const char* vn) {
      return vars.inList(vn) || pending_vars.inList(vn)
         ? true
         : false;
   }

   DLLLOCAL bool parseHasConstant(const std::string &cname) const {
      return constlist.inList(cname) || pend_constlist.inList(cname) ? true : false;
   }

   DLLLOCAL AbstractQoreNode* parseFindLocalConstantValue(const char* cname, const QoreTypeInfo*& cTypeInfo) {
      parseInitPartial();

      // first check committed constants
      ClassAccess access = Public;
      AbstractQoreNode* rv = constlist.find(cname, cTypeInfo, access);
      if (!rv)
	 rv = pend_constlist.parseFind(cname, cTypeInfo, access);

      // check for accessibility to private constants
      if (rv && (access > Public)) {
         qore_class_private* class_ctx = parse_get_class_priv();
         if ((access == Internal && class_ctx != this) || !parseCheckPrivateClassAccess(class_ctx)) {
            rv = 0;
            cTypeInfo = 0;
         }
      }

      //printd(5, "qore_class_private::parseFindLocalConstantValue(%s) this: %p (cls: %p %s) rv: %p\n", cname, this, cls, name.c_str(), rv);
      return rv;
   }

   DLLLOCAL AbstractQoreNode* parseFindConstantValue(const char* cname, const QoreTypeInfo*& cTypeInfo, const qore_class_private* class_ctx) {
      return parseFindConstantValueIntern(cname, cTypeInfo, class_ctx);
   }

   DLLLOCAL AbstractQoreNode* parseFindConstantValueIntern(const char* cname, const QoreTypeInfo*& cTypeInfo, const qore_class_private* class_ctx) {
      parseInitPartial();

      // first check committed constants
      ClassAccess access = Public;
      AbstractQoreNode* rv = constlist.find(cname, cTypeInfo, access);
      if (!rv)
	 rv = pend_constlist.parseFind(cname, cTypeInfo, access);

      // check for accessibility to private constants
      if (rv) {
         if (access == Internal) {
            if (class_ctx == this)
               return rv;
            else
               cTypeInfo = 0;
         }
         else if (access == Private && !parseCheckPrivateClassAccess(class_ctx))
            cTypeInfo = 0;
         else
            return rv;
      }

      return scl ? scl->parseFindConstantValue(cname, cTypeInfo, class_ctx, class_ctx == this) : 0;
   }

   DLLLOCAL QoreVarInfo* parseFindLocalStaticVar(const char* vname) const {
      QoreVarInfo* vi = vars.find(vname);
      if (!vi)
         vi = pending_vars.find(vname);

      if (vi && (vi->access > Public) && !parseCheckPrivateClassAccess())
         vi = 0;

      return vi;
   }

   DLLLOCAL QoreVarInfo* parseFindStaticVar(const char* vname, const QoreClass*& qc, ClassAccess& access, bool check = false) const {
      access = Public;
      return parseFindStaticVarIntern(vname, qc, access, check, true);
   }

   DLLLOCAL QoreVarInfo* parseFindStaticVarIntern(const char* vname, const QoreClass*& qc, ClassAccess& access, bool check, bool toplevel) const {
      QoreVarInfo* vi = vars.find(vname);

      if (!vi)
         vi = pending_vars.find(vname);

      if (vi) {
         ClassAccess va = vi->getAccess();
         if (toplevel || va != Internal) {
            if (access < va)
               access = va;

            // return null and stop searching in this class if we should verify access, and the var is not accessible
            if (check && (access > Public) && !parseCheckPrivateClassAccess())
               return 0;

            qc = cls;
            return vi;
         }
      }

      return scl ? scl->parseFindStaticVar(vname, qc, access, check, toplevel) : 0;
   }

   DLLLOCAL void addMember(const char* mem, ClassAccess access, const QoreTypeInfo* n_typeinfo, AbstractQoreNode* initial_value) {
      assert(!members.inList(mem));
      members.addNoCheck(strdup(mem), new QoreMemberInfo(0, 0, n_typeinfo, 0, initial_value, access));
      if (access == Public && !has_public_memdecl)
         has_public_memdecl = true;
   }

   DLLLOCAL void insertBuiltinStaticMethod(QoreMethod* m) {
      assert(m->isStatic());
      //printd(5, "QoreClass::insertBuiltinStaticMethod() %s::%s() size: %d\n", name.c_str(), m->getName(), numMethods());
      shm[m->getName()] = m;
      // maintain method counts (safely inside parse lock)
      ++num_static_methods;
      if (!sys) { sys = true; }
      // check for special methods (except constructor and destructor) and abort if found
      assert(!checkSpecialStaticIntern(m->getName()));
      // add ancestors
      addStaticAncestors(m);
   }

   DLLLOCAL void insertBuiltinMethod(QoreMethod* m, bool special_method = false) {
      assert(!m->isStatic());
      //printd(5, "QoreClass::insertBuiltinMethod() %s::%s() size: %d\n", name.c_str(), m->getName(), numMethods());
      hm[m->getName()] = m;
      // maintain method counts (safely inside parse lock)
      ++num_methods;
      if (!sys) { sys = true; }
      // check for special methods (except constructor and destructor)
      if (!special_method && !checkAssignSpecialIntern(m))
	 // add ancestors
	 addAncestors(m);
   }

   DLLLOCAL void recheckBuiltinMethodHierarchy();

   DLLLOCAL void addNewAncestors(QoreMethod* m) {
      if (!scl)
	 return;

      scl->addNewAncestors(m);
   }

   DLLLOCAL void addNewStaticAncestors(QoreMethod* m) {
      if (!scl)
	 return;

      scl->addNewStaticAncestors(m);
   }

   DLLLOCAL void addStaticAncestors(QoreMethod* m) {
      if (!scl)
	 return;

      scl->addStaticAncestors(m);
   }

   DLLLOCAL void addAncestors(QoreMethod* m) {
      assert(strcmp(m->getName(), "constructor"));

      if (!scl)
	 return;

      scl->addAncestors(m);
   }

   DLLLOCAL void parseAddStaticAncestors(QoreMethod* m) {
      if (!scl)
	 return;

      scl->parseAddStaticAncestors(m);
   }

   DLLLOCAL void parseAddAncestors(QoreMethod* m) {
      //printd(5, "qore_class_private::parseAddAncestors(%p %s) this: %p cls: %p %s scl: %p\n", m, m->getName(), this, cls, name.c_str(), scl);
      assert(strcmp(m->getName(), "constructor"));

      if (!scl)
	 return;

      scl->parseAddAncestors(m);
   }

   // class_ctx is only set if it is present and accessible, so we only need to check for internal access here
   DLLLOCAL const qore_class_private* runtimeGetMemberClass(const char* mem, ClassAccess& access, const qore_class_private* class_ctx, bool& internal_member) const {
      if (class_ctx) {
         QoreMemberInfo *info = class_ctx->members.findByName(mem);
         if (info && info->getAccess() == Internal) {
            internal_member = true;
            access = Internal;
            return this;
         }
      }

      access = Public;
      internal_member = false;

      return runtimeGetMemberClassIntern(mem, access, class_ctx);
   }

   DLLLOCAL const qore_class_private* runtimeGetMemberClassIntern(const char* mem, ClassAccess& access, const qore_class_private* class_ctx) const {
      QoreMemberInfo *info = members.findByName(mem);
      if (info) {
         ClassAccess ma = info->getAccess();
         if (ma != Internal) {
            if (access < ma)
               access = ma;
            return this;
         }
      }

      return scl ? scl->runtimeGetMemberClass(mem, access, class_ctx, class_ctx && is_equal(*class_ctx)) : 0;
   }

   DLLLOCAL int runtimeInitMembers(QoreObject& o, bool& need_scan, bool internal_only, ExceptionSink* xsink) const;

   DLLLOCAL int initMembers(QoreObject& o, bool& need_scan, ExceptionSink* xsink) const;

   DLLLOCAL int initVar(const char* vname, QoreVarInfo& vi, ExceptionSink* xsink) const {
      if (vi.exp) {
         // evaluate expression
         ReferenceHolder<AbstractQoreNode> val(vi.exp->eval(xsink), xsink);
         if (*xsink)
            return -1;

         val = vi.getTypeInfo()->acceptInputMember(vname, val.release(), xsink);
         if (*xsink)
            return -1;

         discard(vi.assignInit(val.release()), xsink);
      }
      else
         vi.init();

      return 0;
   }

   DLLLOCAL void clearConstants(QoreListNode& l) {
      constlist.clear(l);
   }

   DLLLOCAL void clear(ExceptionSink* xsink) {
      vars.clear(xsink);
   }

   DLLLOCAL void deleteClassData(ExceptionSink* xsink) {
      vars.del(xsink);
      constlist.deleteAll(xsink);
      if (spgm) {
         spgm->deref(xsink);
         spgm = 0;
      }
   }

   /*
   DLLLOCAL int initMembers(QoreObject& o, BCEAList* bceal, ExceptionSink* xsink) const {
      if (scl && scl->initMembers(o, bceal, xsink))
         return -1;

      if (members.empty())
	 return 0;

      // ensure class is only initialized once
      if (bceal && bceal->initMembers(cls->getID()))
         return 0;

      SelfInstantiatorHelper sih(&selfid, &o, xsink);

      if (initMembers(o, members.begin(), members.end(), xsink))
	 return -1;
      return 0;
   }
   */

   DLLLOCAL const QoreMethod* getMethodForEval(const char* nme, QoreProgram* pgm, ExceptionSink* xsink) const;

   DLLLOCAL QoreObject* execConstructor(const AbstractQoreFunctionVariant* variant, const QoreValueList* args, ExceptionSink* xsink) const;
   DLLLOCAL QoreObject* execConstructor(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, ExceptionSink* xsink) const;

   DLLLOCAL void addBuiltinMethod(const char* mname, MethodVariantBase* variant);
   DLLLOCAL void addBuiltinStaticMethod(const char* mname, MethodVariantBase* variant);
   DLLLOCAL void addBuiltinConstructor(BuiltinConstructorVariantBase* variant);
   DLLLOCAL void addBuiltinDestructor(BuiltinDestructorVariantBase* variant);
   DLLLOCAL void addBuiltinCopyMethod(BuiltinCopyVariantBase* variant);
   DLLLOCAL void setDeleteBlocker(q_delete_blocker_t func);
   DLLLOCAL void setBuiltinSystemConstructor(BuiltinSystemConstructorBase* m);

   DLLLOCAL void execBaseClassConstructor(QoreObject* self, BCEAList* bceal, ExceptionSink* xsink) const;
   DLLLOCAL QoreObject* execSystemConstructor(QoreObject* self, int code, va_list args) const;
   DLLLOCAL bool execDeleteBlocker(QoreObject* self, ExceptionSink* xsink) const;
   DLLLOCAL QoreObject* execCopy(QoreObject* old, ExceptionSink* xsink) const;

   // returns a non-static method if it exists in the local class
   DLLLOCAL QoreMethod* parseFindLocalMethod(const char* nme) {
      hm_method_t::iterator i = hm.find(nme);
      return (i != hm.end()) ? i->second : 0;
   }
   // returns a non-static method if it exists in the local class
   DLLLOCAL const QoreMethod* parseFindLocalMethod(const char* nme) const {
      hm_method_t::const_iterator i = hm.find(nme);
      return (i != hm.end()) ? i->second : 0;
   }

   DLLLOCAL QoreMethod* parseFindLocalMethod(const std::string& nme) {
      hm_method_t::iterator i = hm.find(nme);
      return (i != hm.end()) ? i->second : 0;
   }
   // returns a non-static method if it exists in the local class
   DLLLOCAL const QoreMethod* parseFindLocalMethod(const std::string& nme) const {
      hm_method_t::const_iterator i = hm.find(nme);
      return (i != hm.end()) ? i->second : 0;
   }

   // returns any method if it exists in the local class
   DLLLOCAL const QoreMethod* parseFindAnyLocalMethod(const char* nme) const {
      const QoreMethod* m = parseFindLocalMethod(nme);
      return m ? m : parseFindLocalStaticMethod(nme);
   }

   // returns a static method if it exists in the local class
   DLLLOCAL QoreMethod* parseFindLocalStaticMethod(const char* nme) {
      hm_method_t::iterator i = shm.find(nme);
      return (i != shm.end()) ? i->second : 0;
   }
   // returns a static method if it exists in the local class
   DLLLOCAL const QoreMethod* parseFindLocalStaticMethod(const char* nme) const {
      hm_method_t::const_iterator i = shm.find(nme);
      return (i != shm.end()) ? i->second : 0;
   }

   // returns a non-static method if it exists in the local class and has been committed to the class
   DLLLOCAL QoreMethod* findLocalCommittedMethod(const char* nme);
   // returns a non-static method if it exists in the local class and has been committed to the class
   DLLLOCAL const QoreMethod* findLocalCommittedMethod(const char* nme) const;

   // returns a static method if it exists in the local class and has been committed to the class
   DLLLOCAL QoreMethod* findLocalCommittedStaticMethod(const char* nme);
   // returns a static method if it exists in the local class and has been committed to the class
   DLLLOCAL const QoreMethod* findLocalCommittedStaticMethod(const char* nme) const;

   DLLLOCAL void finalizeBuiltin(const char* nspath);
   DLLLOCAL void generateBuiltinSignature(const char* nspath);

   DLLLOCAL static const QoreMethod* doParseMethodAccess(const QoreMethod* m, const qore_class_private* class_ctx);

   DLLLOCAL static const QoreMethod* doMethodAccess(const QoreMethod* m, ClassAccess ma, const qore_class_private* class_ctx) {
      assert(m);
      return ((ma == Public) || ((ma == Private && class_ctx))) ? m : 0;
   }

   DLLLOCAL static const QoreMethod* doMethodAccess(const QoreMethod* m, ClassAccess& access, ClassAccess ma) {
      assert(m);

      if (ma == Internal)
         m = 0;
      else if (access < ma)
         access = ma;

      return m;
   }

   DLLLOCAL const QoreMethod* doRuntimeMethodAccess(const QoreMethod* m, ClassAccess& access, ClassAccess ma, const qore_class_private* class_ctx) const {
      assert(m);

      if (ma == Internal && (!class_ctx || !is_equal(*class_ctx)))
         m = 0;
      else if (access < ma)
         access = ma;

      return m;
   }

   // performs class initialization; find a static method in the class hierarchy at parse time; inaccessible methods are ignored
   DLLLOCAL const QoreMethod* parseFindNormalMethod(const char* mname, const qore_class_private* class_ctx);

   // performs class initialization; find a static method in the class hierarchy at parse time; inaccessible methods are ignored
   DLLLOCAL const QoreMethod* parseFindStaticMethod(const char* mname, const qore_class_private* class_ctx);

   // performs class initialization; finds a non-static method in the class hierarchy at parse time, optionally initializes classes
   DLLLOCAL const QoreMethod* parseFindNormalMethodIntern(const char* mname, const qore_class_private* class_ctx);

   // performs class initialization; finds a static method in the class hierarchy at parse time, optionally initializes classes
   DLLLOCAL const QoreMethod* parseFindStaticMethodIntern(const char* mname, const qore_class_private* class_ctx);

   DLLLOCAL const QoreMethod* parseResolveSelfMethodIntern(const char* nme, const qore_class_private* class_ctx);

   // returns a non-static method if it exists in class hierarchy and has been committed to the class
   // class_ctx is only set if it is present and accessible, so we only need to check for internal access here
   DLLLOCAL const QoreMethod* runtimeFindCommittedStaticMethodIntern(const char* nme, ClassAccess& access, const qore_class_private* class_ctx) const {
      const QoreMethod* m = findLocalCommittedStaticMethod(nme);
      if (m && doRuntimeMethodAccess(m, access, m->getAccess(), class_ctx))
         return m;
      if (!scl)
         return 0;
      m = scl->runtimeFindCommittedStaticMethod(nme, access, class_ctx, class_ctx == this);
      return m ? doRuntimeMethodAccess(m, access, m->getAccess(), class_ctx) : 0;
   }

   // returns a non-static method if it exists in class hierarchy and has been committed to the class
   // class_ctx is only set if it is present and accessible, so we only need to check for internal access here
   DLLLOCAL const QoreMethod* runtimeFindCommittedMethodIntern(const char* nme, ClassAccess& access, const qore_class_private* class_ctx) const {
      const QoreMethod* m = findLocalCommittedMethod(nme);
      if (m && doRuntimeMethodAccess(m, access, m->getAccess(), class_ctx))
         return m;
      if (!scl)
         return 0;
      m = scl->runtimeFindCommittedMethod(nme, access, class_ctx, class_ctx == this);
      return m ? doRuntimeMethodAccess(m, access, m->getAccess(), class_ctx) : 0;
   }

   DLLLOCAL const QoreMethod* runtimeFindCommittedStaticMethod(const char* nme, ClassAccess& access, const qore_class_private* class_ctx) const {
      access = Public;
      return runtimeFindCommittedStaticMethodIntern(nme, access, class_ctx);
   }

   DLLLOCAL const QoreMethod* runtimeFindCommittedMethod(const char* nme, ClassAccess& access, const qore_class_private* class_ctx) const {
      access = Public;
      return runtimeFindCommittedMethodIntern(nme, access, class_ctx);
   }

   DLLLOCAL const QoreMethod* findMethod(const char* nme, ClassAccess& access) const {
      CurrentProgramRuntimeParseContextHelper pch;
      const qore_class_private* class_ctx = runtime_get_class();
      if (class_ctx && !runtimeCheckPrivateClassAccess(class_ctx))
         class_ctx = 0;
      return runtimeFindCommittedMethod(nme, access, class_ctx);
   }

   DLLLOCAL bool runtimeHasCallableMethod(const char* m, int mask) const;

   DLLLOCAL void execDestructor(QoreObject* self, ExceptionSink* xsink) const;

   DLLLOCAL void execBaseClassDestructor(QoreObject* self, ExceptionSink* xsink) const;

   DLLLOCAL void execBaseClassSystemDestructor(QoreObject* self, ExceptionSink* xsink) const;

   DLLLOCAL void execBaseClassCopy(QoreObject* self, QoreObject* old, ExceptionSink* xsink) const;

   DLLLOCAL void parseInit();
   DLLLOCAL void parseCommit();
   DLLLOCAL void parseCommitRuntimeInit(ExceptionSink* xsink);
   DLLLOCAL void parseRollback();
   DLLLOCAL int addUserMethod(const char* mname, MethodVariantBase* f, bool n_static);

   DLLLOCAL QoreValue evalPseudoMethod(const QoreValue n, const char* name, const QoreListNode* args, ExceptionSink* xsink) const;

   DLLLOCAL QoreValue evalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const QoreValue n, const QoreListNode* args, ExceptionSink* xsink) const;

   const QoreMethod* runtimeFindPseudoMethod(const QoreValue n, const char* nme, ExceptionSink* xsink) const {
      const QoreMethod* w;

      const qore_class_private* class_ctx = runtime_get_class();
      if (class_ctx && !runtimeCheckPrivateClassAccess(class_ctx))
         class_ctx = 0;
      ClassAccess access;
      if (!(w = runtimeFindCommittedMethod(nme, access, class_ctx))) {
         qore_type_t t = n.getType();
         // throw an exception
         if (t == NT_OBJECT) {
            const char* cname = n.get<const QoreObject>()->getClassName();
            xsink->raiseException("METHOD-DOES-NOT-EXIST", "no method %s::%s() or pseudo-method %s::%s() is available", cname, nme, name.c_str(), nme);
         }
         else
            xsink->raiseException("PSEUDO-METHOD-DOES-NOT-EXIST", "no pseudo method <%s>::%s() has been defined", n.getTypeName(), nme);
         return 0;
      }

      return w;
   }

   DLLLOCAL bool parseCheckPrivateClassAccess(const qore_class_private* qc = parse_get_class_priv()) const;
   DLLLOCAL bool runtimeCheckPrivateClassAccess(const qore_class_private* qc = runtime_get_class()) const;

   // this = class to find in "oc"
   DLLLOCAL qore_type_result_e parseCheckCompatibleClass(const qore_class_private& oc) const;
   // this = class to find in "oc"
   DLLLOCAL qore_type_result_e runtimeCheckCompatibleClass(const qore_class_private& oc) const;

   DLLLOCAL const QoreClass* getClassIntern(qore_classid_t cid, ClassAccess& n_access, bool toplevel) const {
      if (cid == classID)
         return cls;
      return scl ? scl->getClass(cid, n_access, toplevel) : 0;
   }

   DLLLOCAL const QoreClass* getClass(const qore_class_private& qc, ClassAccess& n_access) const {
      n_access = Public;
      return getClassIntern(qc, n_access, true);
   }

   DLLLOCAL bool is_equal(const qore_class_private& qc) const {
      // check hashes if names are the same
      // FIXME: check fully-qualified namespace name
      return (qc.classID == classID || (qc.name == name && hash == qc.hash));
   }

   DLLLOCAL const QoreClass* getClassIntern(const qore_class_private& qc, ClassAccess& n_access, bool toplevel) const {
      if (is_equal(qc))
         return cls;

#ifdef DEBUG_1
      if (qc.name == name) {
         QoreString lh, rh;
         hash.toString(lh);
         qc.hash.toString(rh);
         printd(0, "qore_class_private::getClassIntern() this: %p '%s' != '%s' scl: %p (hash: %s qc.hash: %s)\n", this, name.c_str(), qc.name.c_str(), scl, lh.getBuffer(), rh.getBuffer());
      }
#endif

      return scl ? scl->getClass(qc, n_access, toplevel) : 0;
   }

   DLLLOCAL const QoreClass* parseGetClassIntern(const qore_class_private& qc, ClassAccess& n_access, bool toplevel) const {
      // check hashes if names are the same
      // FIXME: check fully-qualified namespace name
      if (qc.classID == classID || (qc.name == name && parseCheckEqualHash(qc)))
         return cls;

#ifdef DEBUG_SKIP
      if (qc.name == name) {
         printd(5, "qore_class_private::parseGetClassIntern() this: %p '%s' != '%s' scl: %p\n", this, name.c_str(), qc.name.c_str(), scl);
         parseShowHashes();
         qc.parseShowHashes();
      }
#endif

      return scl ? scl->parseGetClass(qc, n_access, toplevel) : 0;
   }

#ifdef DEBUG_SKIP
   DLLLOCAL void parseShowHashes() const {
      QoreString ch, ph;
      hash.toString(ch);
      pend_hash.toString(ph);
      printd(5, " + %p %s committed: %s\n", this, name.c_str(), ch.getBuffer());
      printd(5, " + %p %s pending  : %s\n", this, name.c_str(), ph.getBuffer());
   }
#endif

   DLLLOCAL bool parseCheckEqualHash(const qore_class_private& qc) const {
#ifdef DEBUG_SKIP
      printd(5, "qore_class_private::parseCheckEqualHash() %s == %s\n", name.c_str(), qc.name.c_str());
      parseShowHashes();
      qc.parseShowHashes();
#endif
      if (pend_hash)
         return qc.pend_hash ? pend_hash == qc.pend_hash : pend_hash == qc.hash;
      return qc.pend_hash ? hash == qc.pend_hash : hash == qc.hash;
   }

   DLLLOCAL bool equal(const qore_class_private& qc) const {
      if (&qc == this)
         return true;
      if (qc.classID == classID || (qc.name == name && qc.hash == hash))
         return true;

      return false;
   }

   DLLLOCAL const QoreClass* parseGetClass(const qore_class_private& qc, ClassAccess& n_access) const;

   DLLLOCAL int addBaseClassesToSubclass(QoreClass* sc, bool is_virtual);

   DLLLOCAL void setPublic();

   DLLLOCAL void parseSetBaseClassList(BCList* bcl) {
      assert(!scl);
      if (bcl) {
         scl = bcl;
         if (!has_new_user_changes)
            has_new_user_changes = true;
      }
   }

   DLLLOCAL bool parseHasPendingChanges() const {
      return has_new_user_changes;
   }

   DLLLOCAL bool parseCheckHierarchy(const QoreClass* n_cls, ClassAccess& access) const {
      access = Public;
      return parseCheckHierarchyIntern(n_cls, access, true);
   }

   DLLLOCAL bool parseCheckHierarchyIntern(const QoreClass* n_cls, ClassAccess& access, bool toplevel) const {
      if (cls == n_cls || (name == n_cls->priv->name && parseCheckEqualHash(*n_cls->priv)))
         return true;

      return scl ? scl->parseCheckHierarchy(n_cls, access, toplevel) : false;
   }

   DLLLOCAL VRMutex* getGate() const {
      return &gate;
   }

   // class initialization; inaccessible methods are ignored
   DLLLOCAL const QoreMethod* parseFindAnyMethod(const char* nme, const qore_class_private* class_ctx);

   // class initialization; inaccessible methods are ignored
   DLLLOCAL const QoreMethod* parseFindAnyMethodStaticFirst(const char* nme, const qore_class_private* class_ctx);

   // class initialization
   DLLLOCAL const QoreMethod* parseResolveSelfMethod(const char* nme, const qore_class_private* class_ctx);
   DLLLOCAL const QoreMethod* parseResolveSelfMethod(NamedScope* nme);

   // class initialization
   DLLLOCAL const QoreMethod* parseFindSelfMethod(const char* nme);

   DLLLOCAL char* getHash() const {
      return hash.getHash();
   }

   // static methods
   //DLLLOCAL static

   DLLLOCAL static char* getHash(const QoreClass& qc) {
      return qc.priv->getHash();
   }

   DLLLOCAL static void parseAddConstant(QoreClass& qc, const std::string &cname, AbstractQoreNode* val, ClassAccess access) {
      qc.priv->parseAddConstant(cname, val, access);
   }

   DLLLOCAL static const QoreMemberInfo* runtimeGetMemberInfo(const QoreClass& qc, const char* mem, ClassAccess& access, const qore_class_private* class_ctx, bool& internal_member) {
      return qc.priv->runtimeGetMemberInfo(mem, access, class_ctx, internal_member);
   }

   DLLLOCAL static LocalVar* getSelfId(const QoreClass& qc) {
      return &qc.priv->selfid;
   }

   DLLLOCAL static QoreObject* execConstructor(const QoreClass& qc, const AbstractQoreFunctionVariant* variant, const QoreListNode* args, ExceptionSink* xsink) {
      return qc.priv->execConstructor(variant, args, xsink);
   }

   DLLLOCAL static bool injected(const QoreClass& qc) {
      return qc.priv->inject;
   }

   DLLLOCAL static QoreClass* makeImportClass(const QoreClass& qc, QoreProgram* spgm, const char* nme, bool inject) {
      QoreClass* rv = new QoreClass(qc);
      if (nme)
         rv->priv->name = nme;
      rv->priv->inject = inject;
      if (qc.priv->pub)
         rv->priv->pub = true;
      // reference source program and save in copied class
      rv->priv->spgm = spgm->programRefSelf();
      printd(5, "qore_program_private::makeImportClass() name: '%s' (%s) rv: %p inject: %d\n", qc.getName(), nme ? nme : "n/a", rv, rv->priv->inject);
      return rv;
   }

   DLLLOCAL static const QoreMethod* runtimeFindCommittedStaticMethod(const QoreClass& qc, const char* nme, ClassAccess& access, const qore_class_private* class_ctx) {
      return qc.priv->runtimeFindCommittedStaticMethod(nme, access, class_ctx);
   }

   DLLLOCAL static const QoreMethod* parseFindLocalMethod(const QoreClass& qc, const char* mname) {
      return qc.priv->parseFindLocalMethod(mname);
   }

   DLLLOCAL static bool parseHasPendingChanges(const QoreClass& qc) {
      return qc.priv->parseHasPendingChanges();
   }

   DLLLOCAL static int parseCheckMemberAccess(const QoreClass& qc, const QoreProgramLocation& loc, const char* mem, const QoreTypeInfo*& memberTypeInfo, int pflag) {
      return qc.priv->parseCheckMemberAccess(loc, mem, memberTypeInfo, pflag);
   }

   DLLLOCAL static bool runtimeHasCallableMethod(const QoreClass& qc, const char* m) {
      return qc.priv->runtimeHasCallableMethod(m, QCCM_NORMAL | QCCM_STATIC);
   }

   DLLLOCAL static bool runtimeHasCallableNormalMethod(const QoreClass& qc, const char* m) {
      return qc.priv->runtimeHasCallableMethod(m, QCCM_NORMAL);
   }

   DLLLOCAL static bool runtimeHasCallableStaticMethod(const QoreClass& qc, const char* m) {
      return qc.priv->runtimeHasCallableMethod(m, QCCM_STATIC);
   }

   DLLLOCAL static const qore_class_private* runtimeGetMemberClass(const QoreClass& qc, const char* mem, ClassAccess& access, const qore_class_private* class_ctx, bool& internal_member) {
      access = Public;
      return qc.priv->runtimeGetMemberClass(mem, access, class_ctx, internal_member);
   }

   DLLLOCAL static int runtimeCheckInstantiateClass(const QoreClass& qc, ExceptionSink* xsink) {
      return qc.priv->runtimeCheckInstantiateClass(xsink);
   }

   DLLLOCAL static void parseCheckAbstractNew(QoreClass& qc) {
      qc.priv->parseCheckAbstractNew();
   }

   DLLLOCAL static void parseInit(QoreClass& qc) {
      qc.priv->parseInit();
   }

   DLLLOCAL static void parseInitPartial(QoreClass& qc) {
      qc.priv->parseInitPartial();
   }

   DLLLOCAL static void parseCommit(QoreClass& qc) {
      qc.priv->parseCommit();
   }

   DLLLOCAL static void parseCommitRuntimeInit(QoreClass& qc, ExceptionSink* xsink) {
      qc.priv->parseCommitRuntimeInit(xsink);
   }

   DLLLOCAL static void parseRollback(QoreClass& qc) {
      qc.priv->parseRollback();
   }

   DLLLOCAL static void resolveCopy(QoreClass& qc) {
      qc.priv->resolveCopy();
   }

   DLLLOCAL static int addUserMethod(QoreClass& qc, const char* mname, MethodVariantBase* f, bool n_static) {
      return qc.priv->addUserMethod(mname, f, n_static);
   }

   DLLLOCAL static void initialize(QoreClass& qc) {
      qc.priv->initialize();
   }

   DLLLOCAL static void parseSetBaseClassList(QoreClass& qc, BCList* bcl) {
      qc.priv->parseSetBaseClassList(bcl);
   }

   DLLLOCAL static BCList* getBaseClassList(const QoreClass& qc) {
      return qc.priv->scl;
   }

   DLLLOCAL static void parseAddStaticVar(QoreClass* qc, char* dname, ClassAccess access, QoreVarInfo* VarInfo) {
      qc->priv->parseAddStaticVar(dname, access, VarInfo);
   }

   DLLLOCAL static void clearConstants(QoreClass* qc, QoreListNode& l) {
      qc->priv->clearConstants(l);
   }

   DLLLOCAL static void clear(QoreClass* qc, ExceptionSink* xsink) {
      qc->priv->clear(xsink);
   }

   DLLLOCAL static void deleteClassData(QoreClass* qc, ExceptionSink* xsink) {
      qc->priv->deleteClassData(xsink);
   }

   // searches only the current class, returns 0 if private found and not accessible in the current parse context
   DLLLOCAL static AbstractQoreNode* parseFindLocalConstantValue(QoreClass* qc, const char* cname, const QoreTypeInfo*& typeInfo) {
      return qc->priv->parseFindLocalConstantValue(cname, typeInfo);
   }

   // searches only the current class, returns 0 if private found and not accessible in the current parse context
   DLLLOCAL static QoreVarInfo* parseFindLocalStaticVar(const QoreClass* qc, const char* vname) {
      return qc->priv->parseFindLocalStaticVar(vname);
   }

   // searches this class and all superclasses
   DLLLOCAL static AbstractQoreNode* parseFindConstantValue(QoreClass* qc, const char* cname, const QoreTypeInfo*& typeInfo, const qore_class_private* class_ctx) {
      return qc->priv->parseFindConstantValue(cname, typeInfo, class_ctx);
   }

   // searches this class and all superclasses, if check = false, then assumes parsing from within the class (parse_get_class() == this class)
   DLLLOCAL static QoreVarInfo* parseFindStaticVar(const QoreClass* qc, const char* vname, const QoreClass*& nqc, ClassAccess& access, bool check = false) {
      return qc->priv->parseFindStaticVar(vname, nqc, access, check);
   }

   DLLLOCAL static const QoreMethod* parseResolveSelfMethod(qore_class_private& qc, const char* nme) {
      return qc.parseResolveSelfMethod(nme, &qc);
   }

   DLLLOCAL static const QoreMethod* parseResolveSelfMethod(qore_class_private& qc, NamedScope* nme) {
      return qc.parseResolveSelfMethod(nme);
   }

   DLLLOCAL static int parseCheckInternalMemberAccess(const QoreClass* qc, const char* mem, const QoreTypeInfo*& memberTypeInfo, const QoreProgramLocation& loc) {
      return qc->priv->parseCheckInternalMemberAccess(mem, memberTypeInfo, loc);
   }

   DLLLOCAL static int parseResolveInternalMemberAccess(const QoreClass* qc, const char* mem, const QoreTypeInfo*& memberTypeInfo) {
      return qc->priv->parseResolveInternalMemberAccess(mem, memberTypeInfo);
   }

   DLLLOCAL static const QoreMethod* parseFindSelfMethod(QoreClass* qc, const char* mname) {
      return qc->priv->parseFindSelfMethod(mname);
   }

   DLLLOCAL static void parseAddMember(QoreClass& qc, char* nme, ClassAccess access, QoreMemberInfo* mInfo) {
      qc.priv->parseAddMember(nme, access, mInfo);
   }

   DLLLOCAL static QoreValue evalPseudoMethod(const QoreClass* qc, const QoreValue n, const char* name, const QoreListNode* args, ExceptionSink* xsink) {
      return qc->priv->evalPseudoMethod(n, name, args, xsink);
   }

   DLLLOCAL static QoreValue evalPseudoMethod(const QoreClass* qc, const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const QoreValue n, const QoreListNode* args, ExceptionSink* xsink) {
      return qc->priv->evalPseudoMethod(m, variant, n, args, xsink);
   }

   DLLLOCAL static void setNamespace(QoreClass* qc, qore_ns_private* n) {
      qc->priv->setNamespace(n);
   }

   DLLLOCAL static bool parseCheckPrivateClassAccess(const QoreClass& qc, const qore_class_private* oqc = parse_get_class_priv()) {
      return qc.priv->parseCheckPrivateClassAccess(oqc);
   }

   DLLLOCAL static bool runtimeCheckPrivateClassAccess(const QoreClass& qc, const qore_class_private* oqc = runtime_get_class()) {
      return qc.priv->runtimeCheckPrivateClassAccess(oqc);
   }

   DLLLOCAL static qore_type_result_e parseCheckCompatibleClass(const QoreClass& qc, const QoreClass& oc) {
      if (!qore_check_this(&oc))
         return QTI_NOT_EQUAL;
      return qc.priv->parseCheckCompatibleClass(*oc.priv);
   }

   DLLLOCAL static qore_type_result_e runtimeCheckCompatibleClass(const QoreClass& qc, const QoreClass& oc) {
      return qc.priv->runtimeCheckCompatibleClass(*oc.priv);
   }

   DLLLOCAL static qore_class_private* get(QoreClass& qc) {
      return qc.priv;
   }

   DLLLOCAL static const qore_class_private* get(const QoreClass& qc) {
      return qc.priv;
   }

   DLLLOCAL static bool isPublic(const QoreClass& qc) {
      return qc.priv->pub;
   }

   DLLLOCAL static bool isUserPublic(const QoreClass& qc) {
      return qc.priv->pub && !qc.priv->sys;
   }

   DLLLOCAL static bool isFinal(const QoreClass& qc) {
      return qc.priv->final;
   }

   DLLLOCAL static void setPublic(QoreClass& qc) {
      qc.priv->setPublic();
   }

   DLLLOCAL static void setFinal(QoreClass& qc) {
      assert(!qc.priv->final);
      qc.priv->final = true;
   }
};

class qore_class_private_holder {
   qore_class_private* c;

public:
   DLLLOCAL qore_class_private_holder(QoreClass* n_c) : c(qore_class_private::get(*n_c)) {
   }

   DLLLOCAL qore_class_private_holder(qore_class_private* n_c) : c(n_c) {
   }

   DLLLOCAL ~qore_class_private_holder() {
      if (c)
         c->deref();
   }

   DLLLOCAL qore_class_private* operator*() {
      return c;
   }

   DLLLOCAL QoreClass* release() {
      if (c) {
         QoreClass* rv = c->cls;
         c = 0;
         return rv;
      }
      return 0;
   }
};

class qore_method_private {
public:
   const QoreClass* parent_class;
   MethodFunctionBase* func;
   bool static_flag, all_user;

   DLLLOCAL qore_method_private(const QoreClass* n_parent_class, MethodFunctionBase* n_func, bool n_static) : parent_class(n_parent_class), func(n_func), static_flag(n_static), all_user(true) {
      assert(parent_class == n_func->getClass());
   }

   DLLLOCAL ~qore_method_private() {
      func->deref();
   }

   DLLLOCAL void setBuiltin() {
      if (all_user)
         all_user = false;
   }

   DLLLOCAL bool isUniquelyUser() const {
      return all_user;
   }

   DLLLOCAL int addUserVariant(MethodVariantBase* variant) {
      return func->parseAddUserMethodVariant(variant);
   }

   DLLLOCAL void addBuiltinVariant(MethodVariantBase* variant) {
      setBuiltin();
      func->addBuiltinMethodVariant(variant);
   }

   DLLLOCAL MethodFunctionBase* getFunction() const {
      return const_cast<MethodFunctionBase* >(func);
   }

   DLLLOCAL const char* getName() const {
      return func->getName();
   }

   DLLLOCAL const std::string& getNameStr() const {
      return func->getNameStr();
   }

   DLLLOCAL void parseInit();

   DLLLOCAL void parseInitStatic() {
      assert(static_flag);
      func->parseInit();
      // make sure the method doesn't override a "final" method in a base class
      func->checkFinal();
   }

   DLLLOCAL const QoreTypeInfo* getUniqueReturnTypeInfo() const {
      return func->getUniqueReturnTypeInfo();
   }

   DLLLOCAL void evalConstructor(const AbstractQoreFunctionVariant* variant, QoreObject* self, const QoreValueList* args, BCEAList* bceal, ExceptionSink* xsink) {
      CONMF(func)->evalConstructor(variant, *parent_class, self, args, parent_class->priv->scl, bceal, xsink);
   }

   DLLLOCAL void evalConstructor(const AbstractQoreFunctionVariant* variant, QoreObject* self, const QoreListNode* args, BCEAList* bceal, ExceptionSink* xsink) {
      CONMF(func)->evalConstructor(variant, *parent_class, self, args, parent_class->priv->scl, bceal, xsink);
   }

   DLLLOCAL void evalCopy(QoreObject* self, QoreObject* old, ExceptionSink* xsink) const {
      COPYMF(func)->evalCopy(*parent_class, self, old, parent_class->priv->scl, xsink);
   }

   DLLLOCAL bool evalDeleteBlocker(QoreObject* self) const {
      // can only be builtin
      return self->evalDeleteBlocker(parent_class->priv->methodID, reinterpret_cast<BuiltinDeleteBlocker*>(func));
   }

   DLLLOCAL void evalDestructor(QoreObject* self, ExceptionSink* xsink) const {
      DESMF(func)->evalDestructor(*parent_class, self, xsink);
   }

   DLLLOCAL void evalSystemDestructor(QoreObject* self, ExceptionSink* xsink) const {
      // execute function directly
      DESMF(func)->evalDestructor(*parent_class, self, xsink);
   }

   DLLLOCAL void evalSystemConstructor(QoreObject* self, int code, va_list args) const {
      BSYSCONB(func)->eval(*parent_class, self, code, args);
   }

   DLLLOCAL QoreValue eval(ExceptionSink* xsink, QoreObject* self, const QoreListNode* args) const {
      if (!static_flag) {
         assert(self);
         return NMETHF(func)->evalMethod(xsink, 0, self, args);
      }
      return SMETHF(func)->evalMethod(xsink, 0, args);
   }

   DLLLOCAL QoreValue evalPseudoMethod(const AbstractQoreFunctionVariant* variant, const QoreValue n, const QoreListNode* args, ExceptionSink* xsink) const {
      QORE_TRACE("qore_method_private::evalPseudoMethod()");

      assert(!static_flag);

      QoreValue rv = NMETHF(func)->evalPseudoMethod(xsink, variant, n, args);
      printd(5, "qore_method_private::evalPseudoMethod() %s::%s() returning type: %s\n", parent_class->getName(), getName(), rv.getTypeName());
      return rv;
   }

   DLLLOCAL QoreValue evalNormalVariant(QoreObject* self, const QoreExternalMethodVariant* ev, const QoreListNode* args, ExceptionSink* xsink) const;

   // returns the lowest access code for all variants including uncommitted variants
   DLLLOCAL ClassAccess parseGetAccess() const;

   // returns the lowest access code for all variants including uncommitted variants
   DLLLOCAL static ClassAccess parseGetAccess(const QoreMethod& m) {
      return m.priv->parseGetAccess();
   }

   DLLLOCAL static QoreValue evalNormalVariant(const QoreMethod& m, ExceptionSink* xsink, QoreObject* self, const QoreExternalMethodVariant* ev, const QoreListNode* args) {
      return m.priv->evalNormalVariant(self, ev, args, xsink);
   }

   DLLLOCAL static QoreValue evalPseudoMethod(const QoreMethod& m, ExceptionSink* xsink, const AbstractQoreFunctionVariant* variant, const QoreValue n, const QoreListNode* args) {
      return m.priv->evalPseudoMethod(variant, n, args, xsink);
   }

   DLLLOCAL static QoreValue eval(const QoreMethod& m, ExceptionSink* xsink, QoreObject* self, const QoreListNode* args) {
      return m.priv->eval(xsink, self, args);
   }
};

#endif
