/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreClassIntern.h

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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

#include <qore/safe_dslist>
#include <qore/intern/ConstantList.h>
#include <qore/intern/QoreValue.h>

#include <string.h>

#include <list>
#include <map>
#include <string>

#define OTF_USER    CT_USER
#define OTF_BUILTIN CT_BUILTIN

#ifdef HAVE_QORE_HASH_MAP
//#warning compiling with hash_map
#include <qore/hash_map_include.h>
#include <qore/intern/xxhash.h>

typedef HASH_MAP<std::string, QoreMethod*> hm_method_t;
#else
typedef std::map<std::string, QoreMethod*> hm_method_t;
#endif

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

   DLLLOCAL static void parseCheckAbstract(const char* cname, const char* mname, vmap_t& vlist, QoreStringNode*& desc);

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

   // we check if there are any abstract method variants still in the committed lists
   DLLLOCAL void parseCheckAbstractNew(const char* name) const;
};

class SignatureHash;

static inline const char* privpub(bool priv) { return priv ? "private" : "public"; }

// forward reference for base class (constructor) argument list
class BCAList;
// forward reference for base class list
class BCList;
// forward reference for base class (constructor) evaluated argument list
class BCEAList;

// forward reference to private class implementation
class qore_class_private;

class MethodVariantBase : public AbstractQoreFunctionVariant {
protected:
   const QoreMethod* qmethod;    // pointer to method that owns the variant
   bool priv_flag,               // is the variant private or not
      final,                     // is the variant final or not
      abstract;                  // is the variant abstract or not
   std::string asig;             // abstract signature, only set for abstract method variants

public:
   // add QC_USES_EXTRA_ARGS to abstract methods by default as derived methods could use extra arguments
   DLLLOCAL MethodVariantBase(bool n_priv_flag, bool n_final, int64 n_flags, bool n_is_user = false, bool n_is_abstract = false) :
      AbstractQoreFunctionVariant(n_flags | (n_is_abstract ? QC_USES_EXTRA_ARGS : 0), n_is_user), qmethod(0), priv_flag(n_priv_flag), final(n_final), abstract(n_is_abstract) {
   }

   DLLLOCAL bool isAbstract() const {
      return abstract;
   }

   DLLLOCAL bool isPrivate() const {
      return priv_flag;
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
   DLLLOCAL MethodVariant(bool n_priv_flag, bool n_final, int64 n_flags, bool n_is_user = false, bool is_abstract = false) : MethodVariantBase(n_priv_flag, n_final, n_flags, n_is_user, is_abstract) {
   }
   DLLLOCAL virtual AbstractQoreNode* evalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const = 0;
   DLLLOCAL virtual int64 bigIntEvalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalMethod(self, ceh, xsink), xsink);
      return rv ? rv->getAsBigInt() : 0;
   }
   DLLLOCAL virtual int intEvalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalMethod(self, ceh, xsink), xsink);
      return rv ? rv->getAsInt() : 0;
   }
   DLLLOCAL virtual bool boolEvalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalMethod(self, ceh, xsink), xsink);
      return rv ? rv->getAsBool() : false;
   }
   DLLLOCAL virtual double floatEvalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalMethod(self, ceh, xsink), xsink);
      return rv ? rv->getAsFloat() : 0.0;
   }
   DLLLOCAL virtual AbstractQoreNode* evalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      assert(false);
      return 0;
   }
   DLLLOCAL virtual int64 bigIntEvalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalPseudoMethod(n, ceh, xsink), xsink);
      return rv ? rv->getAsBigInt() : 0;
   }
   DLLLOCAL virtual int intEvalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalPseudoMethod(n, ceh, xsink), xsink);
      return rv ? rv->getAsInt() : 0;
   }
   DLLLOCAL virtual bool boolEvalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalPseudoMethod(n, ceh, xsink), xsink);
      return rv ? rv->getAsBool() : false;
   }
   DLLLOCAL virtual double floatEvalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalPseudoMethod(n, ceh, xsink), xsink);
      return rv ? rv->getAsFloat() : 0.0;
   }
};

#define METHV(f) (reinterpret_cast<MethodVariant*>(f))
#define METHV_const(f) (reinterpret_cast<const MethodVariant*>(f))

class ConstructorMethodVariant : public MethodVariantBase {
protected:
   // evaluates base class constructors and initializes members
   DLLLOCAL int constructorPrelude(const QoreClass &thisclass, CodeEvaluationHelper &ceh, QoreObject *self, BCList *bcl, BCEAList *bceal, ExceptionSink* xsink) const;

public:
   DLLLOCAL ConstructorMethodVariant(bool n_priv_flag, int64 n_flags, bool n_is_user = false) : MethodVariantBase(n_priv_flag, false, n_flags, n_is_user) {
   }
   DLLLOCAL virtual const BCAList *getBaseClassArgumentList() const = 0;
   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject *self, CodeEvaluationHelper &ceh, BCList *bcl, BCEAList *bceal, ExceptionSink* xsink) const = 0;
};

#define CONMV(f) (reinterpret_cast<ConstructorMethodVariant*>(f))
#define CONMV_const(f) (reinterpret_cast<const ConstructorMethodVariant*>(f))

class DestructorMethodVariant : public MethodVariantBase {
protected:
public:
   DLLLOCAL DestructorMethodVariant(bool n_is_user = false) : MethodVariantBase(false, QC_NO_FLAGS, n_is_user) {
   }

   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink* xsink) const = 0;
};

#define DESMV(f) (reinterpret_cast<DestructorMethodVariant*>(f))
#define DESMV_const(f) (reinterpret_cast<const DestructorMethodVariant*>(f))

class CopyMethodVariant : public MethodVariantBase {
protected:
public:
   DLLLOCAL CopyMethodVariant(bool n_priv_flag, bool n_is_user = false) : MethodVariantBase(n_priv_flag, QC_NO_FLAGS, n_is_user) {
   }

   DLLLOCAL virtual void evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, CodeEvaluationHelper &ceh, BCList *scl, ExceptionSink* xsink) const = 0;
};

#define COPYMV(f) (reinterpret_cast<CopyMethodVariant*>(f))
#define COPYMV_const(f) (reinterpret_cast<const CopyMethodVariant*>(f))

class UserMethodVariant : public MethodVariant, public UserVariantBase {
public:
   DLLLOCAL UserMethodVariant(bool n_priv_flag, bool n_final, StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced, int64 n_flags, bool is_abstract) : MethodVariant(n_priv_flag, n_final, n_flags, true, is_abstract), UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, rv, synced) {
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

   DLLLOCAL virtual AbstractQoreNode* evalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      return eval(qmethod->getName(), &ceh, self, xsink, getClassPriv());
   }
};

#define UMV(f) (reinterpret_cast<UserMethodVariant*>(f))
#define UMV_const(f) (reinterpret_cast<const UserMethodVariant*>(f))

class UserConstructorVariant : public ConstructorMethodVariant, public UserVariantBase {
protected:
   // base class argument list for constructors
   BCAList *bcal;   

   DLLLOCAL virtual ~UserConstructorVariant();

public:
   DLLLOCAL UserConstructorVariant(bool n_priv_flag, StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, BCAList *n_bcal, int64 n_flags = QC_NO_FLAGS) : ConstructorMethodVariant(n_priv_flag, n_flags, true), UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, 0, false), bcal(n_bcal) {
   }

   // the following defines the pure virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL virtual const BCAList *getBaseClassArgumentList() const {
      return bcal;
   }

   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject *self, CodeEvaluationHelper &ceh, BCList *bcl, BCEAList *bceal, ExceptionSink* xsink) const {
      // in case this method is called from a subclass, switch to the program where the class was created
      ProgramThreadCountContextHelper pch(xsink, pgm, true);
      if (*xsink)
         return;

      UserVariantExecHelper uveh(this, &ceh, false, xsink);
      if (!uveh)
	 return;

      CodeContextHelper cch("constructor", self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack in debugging mode
      CallStackHelper csh("constructor", CT_USER, self, xsink);
#endif

      if (constructorPrelude(thisclass, ceh, self, bcl, bceal, xsink))
	 return;

      discard(evalIntern(uveh.getArgv(), self, xsink), xsink);
   }

   DLLLOCAL virtual void parseInit(QoreFunction* f);
};

#define UCONV(f) (reinterpret_cast<UserConstructorVariant*>(f))
#define UCONV_const(f) (reinterpret_cast<const UserConstructorVariant*>(f))

class UserDestructorVariant : public DestructorMethodVariant, public UserVariantBase {
protected:
public:
   DLLLOCAL UserDestructorVariant(StatementBlock *b, int n_sig_first_line, int n_sig_last_line) : DestructorMethodVariant(true), UserVariantBase(b, n_sig_first_line, n_sig_last_line, 0, 0, false) {
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

   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink* xsink) const {
      // there cannot be any params
      assert(!signature.numParams());
      assert(!signature.getReturnTypeInfo() || signature.getReturnTypeInfo() == nothingTypeInfo);
      ProgramThreadCountContextHelper pch(xsink, pgm, true);
      discard(eval("destructor", 0, self, xsink, getClassPriv()), xsink);
   }
};

#define UDESV(f) (reinterpret_cast<UserDestructorVariant*>(f))
#define UDESV_const(f) (reinterpret_cast<const UserDestructorVariant*>(f))

class UserCopyVariant : public CopyMethodVariant, public UserVariantBase {
protected:
public:
   DLLLOCAL UserCopyVariant(bool n_priv_flag, StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced) : CopyMethodVariant(n_priv_flag, true), UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, rv, synced) {
   }

   // the following defines the pure virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL virtual void parseInit(QoreFunction* f);

   DLLLOCAL virtual void evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, CodeEvaluationHelper &ceh, BCList *scl, ExceptionSink* xsink) const;
};

#define UCOPYV(f) (reinterpret_cast<UserCopyVariant*>(f))

class BuiltinMethodVariant : public MethodVariant, public BuiltinFunctionVariantBase {
public:
   DLLLOCAL BuiltinMethodVariant(bool n_priv_flag, bool n_final, int64 n_flags, int64 n_functionality, const QoreTypeInfo* n_returnTypeInfo, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : MethodVariant(n_priv_flag, n_final, n_flags), BuiltinFunctionVariantBase(n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names) {}

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS
};

class BuiltinAbstractMethodVariant : public BuiltinMethodVariant {
public:
   DLLLOCAL BuiltinAbstractMethodVariant(bool n_priv_flag, int64 n_flags, const QoreTypeInfo* n_returnTypeInfo, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_priv_flag, false, n_flags, QDOM_DEFAULT, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names) {
      abstract = true;
   }

   DLLLOCAL virtual AbstractQoreNode* evalMethod(QoreObject* self, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      assert(false);
      return 0;
   }
};

class BuiltinNormalMethodVariantBase : public BuiltinMethodVariant {
public:
   DLLLOCAL BuiltinNormalMethodVariantBase(bool n_priv_flag, bool n_final, int64 n_flags, int64 n_functionality, const QoreTypeInfo* n_returnTypeInfo, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names) {}

   DLLLOCAL virtual AbstractQoreNode* evalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), self, xsink);

      return self->evalBuiltinMethodWithPrivateData(*qmethod, this, ceh.getArgs(), xsink);
   }

   DLLLOCAL virtual int64 bigIntEvalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), self, xsink);

      return self->bigIntEvalBuiltinMethodWithPrivateData(*qmethod, this, ceh.getArgs(), xsink);
   }

   DLLLOCAL virtual int intEvalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), self, xsink);

      return self->intEvalBuiltinMethodWithPrivateData(*qmethod, this, ceh.getArgs(), xsink);
   }

   DLLLOCAL virtual bool boolEvalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), self, xsink);

      return self->boolEvalBuiltinMethodWithPrivateData(*qmethod, this, ceh.getArgs(), xsink);
   }

   DLLLOCAL virtual double floatEvalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), self, xsink);

      return self->floatEvalBuiltinMethodWithPrivateData(*qmethod, this, ceh.getArgs(), xsink);
   }

   DLLLOCAL virtual AbstractQoreNode* evalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const;
   DLLLOCAL virtual int64 bigIntEvalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const;
   DLLLOCAL virtual int intEvalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const;
   DLLLOCAL virtual bool boolEvalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const;
   DLLLOCAL virtual double floatEvalPseudoMethod(const AbstractQoreNode* n, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const;

   DLLLOCAL virtual AbstractQoreNode* evalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const = 0;
   DLLLOCAL virtual int64 bigIntEvalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalImpl(self, private_data, args, xsink), xsink);
      return rv ? rv->getAsBigInt() : 0;
   }
   DLLLOCAL virtual int intEvalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalImpl(self, private_data, args, xsink), xsink);
      return rv ? rv->getAsInt() : 0;
   }
   DLLLOCAL virtual bool boolEvalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalImpl(self, private_data, args, xsink), xsink);
      return rv ? rv->getAsBool() : false;
   }
   DLLLOCAL virtual double floatEvalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalImpl(self, private_data, args, xsink), xsink);
      return rv ? rv->getAsFloat() : 0.0;
   }
};

class BuiltinNormalMethodVariant : public BuiltinNormalMethodVariantBase {
protected:
   q_method_t method;

public:
   DLLLOCAL BuiltinNormalMethodVariant(q_method_t m, bool n_priv_flag, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinNormalMethodVariantBase(n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), method(m) {
   }
   DLLLOCAL virtual AbstractQoreNode* evalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      return method(self, private_data, args, xsink);
   }
};

template <typename F>
class BuiltinNormalMethodTypeVariantBase : public BuiltinNormalMethodVariantBase {
protected:
   F method;

public:
   DLLLOCAL BuiltinNormalMethodTypeVariantBase(F m, bool n_priv_flag, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinNormalMethodVariantBase(n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), method(m) {
   }
   DLLLOCAL virtual int64 bigIntEvalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      return (int64)method(self, private_data, args, xsink);
   }
   DLLLOCAL virtual int intEvalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      return (int)method(self, private_data, args, xsink);
   }
   DLLLOCAL virtual bool boolEvalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      return (bool)method(self, private_data, args, xsink);
   }
   DLLLOCAL virtual double floatEvalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      return (double)method(self, private_data, args, xsink);
   }
};

template <typename B, typename F, class Q>
class BuiltinNormalMethodTypeVariant : public BuiltinNormalMethodTypeVariantBase<F> {
public:
   DLLLOCAL BuiltinNormalMethodTypeVariant(F m, bool n_priv_flag, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinNormalMethodTypeVariantBase<F>(m, n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names) {
   }
   DLLLOCAL virtual AbstractQoreNode* evalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      B rv = BuiltinNormalMethodTypeVariantBase<F>::method(self, private_data, args, xsink);
      return *xsink ? 0 : new Q(rv);
   }
};

typedef BuiltinNormalMethodTypeVariant<int64, q_method_int64_t, QoreBigIntNode> BuiltinNormalMethodBigIntVariant;
typedef BuiltinNormalMethodTypeVariant<int, q_method_int_t, QoreBigIntNode> BuiltinNormalMethodIntVariant;
typedef BuiltinNormalMethodTypeVariant<double, q_method_double_t, QoreFloatNode> BuiltinNormalMethodFloatVariant;

class BuiltinNormalMethodBoolVariant : public BuiltinNormalMethodTypeVariantBase<q_method_bool_t> {
public:
   DLLLOCAL BuiltinNormalMethodBoolVariant(q_method_bool_t m, bool n_priv_flag, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinNormalMethodTypeVariantBase<q_method_bool_t>(m, n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names) {
   }
   DLLLOCAL virtual AbstractQoreNode* evalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      bool rv = method(self, private_data, args, xsink);
      return *xsink ? 0 : get_bool_node(rv);
   }
};

class BuiltinNormalMethod2Variant : public BuiltinNormalMethodVariantBase {
protected:
   q_method2_t method;

public:
   DLLLOCAL BuiltinNormalMethod2Variant(q_method2_t m, bool n_priv_flag, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinNormalMethodVariantBase(n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), method(m) {
   }
   DLLLOCAL virtual AbstractQoreNode* evalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      return method(*qmethod, self, private_data, args, xsink);
   }
};

class BuiltinNormalMethod3Variant : public BuiltinNormalMethodVariantBase {
protected:
   q_method3_t method;
   const void *ptr;

public:
   DLLLOCAL BuiltinNormalMethod3Variant(const void *n_ptr, q_method3_t m, bool n_priv_flag, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinNormalMethodVariantBase(n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), method(m), ptr(n_ptr) {
   }
   DLLLOCAL virtual AbstractQoreNode* evalImpl(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode* args, ExceptionSink* xsink) const {
      return method(*qmethod, signature.getTypeList(), ptr, self, private_data, args, xsink);
   }
};

class BuiltinStaticMethodVariant : public BuiltinMethodVariant {
protected:
   q_func_t static_method;

public:
   DLLLOCAL BuiltinStaticMethodVariant(q_func_t m, bool n_priv_flag, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), static_method(m) {
   }

   DLLLOCAL virtual AbstractQoreNode* evalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), ClassObj(getClassPriv()), xsink);

      return static_method(ceh.getArgs(), xsink);
   }
};

template <typename F>
class BuiltinStaticMethodTypeVariantBase : public BuiltinMethodVariant {
protected:
   F static_method;

public:
   DLLLOCAL BuiltinStaticMethodTypeVariantBase(F m, bool n_priv_flag, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), static_method(m) {
   }
   DLLLOCAL virtual int64 bigIntEvalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), ClassObj(getClassPriv()), xsink);

      return (int64)static_method(ceh.getArgs(), xsink);
   }

   DLLLOCAL virtual int intEvalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), ClassObj(getClassPriv()), xsink);

      return (int)static_method(ceh.getArgs(), xsink);
   }

   DLLLOCAL virtual bool boolEvalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), ClassObj(getClassPriv()), xsink);

      return (bool)static_method(ceh.getArgs(), xsink);
   }

   DLLLOCAL virtual double floatEvalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), ClassObj(getClassPriv()), xsink);

      return (double)static_method(ceh.getArgs(), xsink);
   }
};

template <typename B, typename F, class Q>
class BuiltinStaticMethodTypeVariant : public BuiltinStaticMethodTypeVariantBase<F> {
public:
   DLLLOCAL BuiltinStaticMethodTypeVariant(F m, bool n_priv_flag, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinStaticMethodTypeVariantBase<F>(m, n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names) {
   }
   DLLLOCAL virtual AbstractQoreNode* evalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      B rv = BuiltinStaticMethodTypeVariantBase<F>::static_method(ceh.getArgs(), xsink);
      return *xsink ? 0 : new Q(rv);
   }
};

typedef BuiltinStaticMethodTypeVariant<int64, q_func_int64_t, QoreBigIntNode> BuiltinStaticMethodBigIntVariant;
typedef BuiltinStaticMethodTypeVariant<double, q_func_double_t, QoreFloatNode> BuiltinStaticMethodFloatVariant;

class BuiltinStaticMethodBoolVariant : public BuiltinStaticMethodTypeVariantBase<q_func_bool_t> {
public:
   DLLLOCAL BuiltinStaticMethodBoolVariant(q_func_bool_t m, bool n_priv_flag, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinStaticMethodTypeVariantBase<q_func_bool_t>(m, n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names) {
   }
   DLLLOCAL virtual AbstractQoreNode* evalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      bool rv = BuiltinStaticMethodTypeVariantBase<q_func_bool_t>::static_method(ceh.getArgs(), xsink);
      return *xsink ? 0 : get_bool_node(rv);
   }
};

class BuiltinStaticMethod2Variant : public BuiltinMethodVariant {
protected:
   q_static_method2_t static_method;

public:
   DLLLOCAL BuiltinStaticMethod2Variant(q_static_method2_t m, bool n_priv_flag, bool n_final = false, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), static_method(m) {
   }
   DLLLOCAL AbstractQoreNode* evalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), ClassObj(getClassPriv()), xsink);

      return static_method(*qmethod, ceh.getArgs(), xsink);
   }
};

class BuiltinStaticMethod3Variant : public BuiltinMethodVariant {
protected:
   q_static_method3_t static_method;
   const void *ptr;

public:
   DLLLOCAL BuiltinStaticMethod3Variant(const void *n_ptr, q_static_method3_t m, bool n_priv_flag, bool n_final, int64 n_flags, int64 n_functionality = QDOM_DEFAULT, const QoreTypeInfo* n_returnTypeInfo = 0, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinMethodVariant(n_priv_flag, n_final, n_flags, n_functionality, n_returnTypeInfo, n_typeList, n_defaultArgList, n_names), static_method(m), ptr(n_ptr) {
   }
   DLLLOCAL AbstractQoreNode* evalMethod(QoreObject *self, CodeEvaluationHelper &ceh, ExceptionSink* xsink) const {
      CODE_CONTEXT_HELPER(CT_BUILTIN, qmethod->getName(), ClassObj(getClassPriv()), xsink);

      return static_method(*qmethod, signature.getTypeList(), ptr, ceh.getArgs(), xsink);
   }
};

class BuiltinConstructorVariantBase : public ConstructorMethodVariant, public BuiltinFunctionVariantBase {
public:
   // return type info is set to 0 because the new operator actually returns the new object, not the constructor
   DLLLOCAL BuiltinConstructorVariantBase(bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : ConstructorMethodVariant(n_priv_flag, n_flags), BuiltinFunctionVariantBase(n_functionality, 0, n_typeList, n_defaultArgList, n_names) {
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
   DLLLOCAL BuiltinConstructorVariant(q_constructor_t m, bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinConstructorVariantBase(n_priv_flag, n_flags, n_functionality, n_typeList, n_defaultArgList, n_names), constructor(m) {
   }

   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject *self, CodeEvaluationHelper &ceh, BCList *bcl, BCEAList *bceal, ExceptionSink* xsink) const {
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
   DLLLOCAL BuiltinConstructor2Variant(q_constructor2_t m, bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinConstructorVariantBase(n_priv_flag, n_flags, n_functionality, n_typeList, n_defaultArgList, n_names), constructor(m) {
   }
   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject *self, CodeEvaluationHelper &ceh, BCList *bcl, BCEAList *bceal, ExceptionSink* xsink) const {
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
   DLLLOCAL BuiltinConstructor3Variant(const void *n_ptr, q_constructor3_t m, bool n_priv_flag, int64 n_flags = QC_USES_EXTRA_ARGS, int64 n_functionality = QDOM_DEFAULT, const type_vec_t &n_typeList = type_vec_t(), const arg_vec_t &n_defaultArgList = arg_vec_t(), const name_vec_t& n_names = name_vec_t()) : BuiltinConstructorVariantBase(n_priv_flag, n_flags, n_functionality, n_typeList, n_defaultArgList, n_names), constructor(m), ptr(n_ptr) {
   }
   DLLLOCAL virtual void evalConstructor(const QoreClass &thisclass, QoreObject *self, CodeEvaluationHelper &ceh, BCList *bcl, BCEAList *bceal, ExceptionSink* xsink) const {
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

   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink* xsink) const {
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
   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink* xsink) const {
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
   DLLLOCAL virtual void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink* xsink) const {
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
   DLLLOCAL BuiltinCopyVariantBase(const QoreClass* c) : CopyMethodVariant(false), BuiltinFunctionVariantBase(QDOM_DEFAULT, c->getTypeInfo()) {
   }

   // the following defines the pure virtual functions that are common to all builtin variants
   COMMON_BUILTIN_VARIANT_FUNCTIONS

   DLLLOCAL virtual void evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, CodeEvaluationHelper &ceh, BCList *scl, ExceptionSink* xsink) const;
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink* xsink) const = 0;
};

class BuiltinCopyVariant : public BuiltinCopyVariantBase {
protected:
   q_copy_t copy;

public:
   DLLLOCAL BuiltinCopyVariant(QoreClass* c, q_copy_t m) : BuiltinCopyVariantBase(c), copy(m) {
   }
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink* xsink) const {
      copy(self, old, private_data, xsink);
   }
};

class BuiltinCopy2Variant : public BuiltinCopyVariantBase {
protected:
   q_copy2_t copy;

public:
   DLLLOCAL BuiltinCopy2Variant(QoreClass* c, q_copy2_t m) : BuiltinCopyVariantBase(c), copy(m) {
   }
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink* xsink) const {
      copy(thisclass, self, old, private_data, xsink);
   }
};

class BuiltinCopy3Variant : public BuiltinCopyVariantBase {
protected:
   q_copy3_t copy;
   const void *ptr;

public:
   DLLLOCAL BuiltinCopy3Variant(const void *n_ptr, QoreClass* c, q_copy3_t m) : BuiltinCopyVariantBase(c), copy(m), ptr(n_ptr) {
   }
   DLLLOCAL virtual void evalImpl(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink* xsink) const {
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
   DLLLOCAL AbstractQoreNode* evalMethod(const AbstractQoreFunctionVariant* variant, QoreObject *self, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL int64 bigIntEvalMethod(const AbstractQoreFunctionVariant* variant, QoreObject *self, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL int intEvalMethod(const AbstractQoreFunctionVariant* variant, QoreObject *self, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL bool boolEvalMethod(const AbstractQoreFunctionVariant* variant, QoreObject *self, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL double floatEvalMethod(const AbstractQoreFunctionVariant* variant, QoreObject *self, const QoreListNode* args, ExceptionSink* xsink) const;

   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL AbstractQoreNode* evalPseudoMethod(const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL int64 bigIntEvalPseudoMethod(const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL int intEvalPseudoMethod(const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL bool boolEvalPseudoMethod(const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL double floatEvalPseudoMethod(const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const;
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
   DLLLOCAL AbstractQoreNode* evalMethod(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL int64 bigIntEvalMethod(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL int intEvalMethod(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL bool boolEvalMethod(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL double floatEvalMethod(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, ExceptionSink* xsink) const;
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
   DLLLOCAL void evalConstructor(const AbstractQoreFunctionVariant* variant, const QoreClass &thisclass, QoreObject *self, const QoreListNode* args, BCList *bcl, BCEAList *bceal, ExceptionSink* xsink) const;

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass* n_qc) const {
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
   DLLLOCAL void evalDestructor(const QoreClass &thisclass, QoreObject *self, ExceptionSink* xsink) const;

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass* n_qc) const {
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
   DLLLOCAL void evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, BCList *scl, ExceptionSink* xsink) const;

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass* n_qc) const {
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
   DLLLOCAL virtual void eval(const QoreClass &thisclass, QoreObject *self, int code, va_list args) const = 0;

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass* n_qc) const = 0;
};

#define BSYSCONB(f) (reinterpret_cast<BuiltinSystemConstructorBase *>(f))

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

   DLLLOCAL virtual void eval(const QoreClass &thisclass, QoreObject *self, int code, va_list args) const {
      system_constructor(self, code, args);
   }

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass* n_qc) const {
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

   DLLLOCAL virtual void eval(const QoreClass &thisclass, QoreObject *self, int code, va_list args) const {
      system_constructor(thisclass, self, code, args);
   }

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass* n_qc) const {
      return new BuiltinSystemConstructor2(*this, n_qc);
   }
};

class BuiltinNormalMethod : public NormalMethodFunction {
public:
   DLLLOCAL BuiltinNormalMethod(const QoreClass* n_qc, const char* mname) : NormalMethodFunction(mname, n_qc) {
   }

   DLLLOCAL BuiltinNormalMethod(const BuiltinNormalMethod &old, const QoreClass* n_qc) : NormalMethodFunction(old, n_qc) {
   }

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass* n_qc) const {
      return new BuiltinNormalMethod(*this, n_qc);
   }
};

class BuiltinStaticMethod : public StaticMethodFunction {
public:
   DLLLOCAL BuiltinStaticMethod(const QoreClass* n_qc, const char* mname) : StaticMethodFunction(mname, n_qc) {
   }

   DLLLOCAL BuiltinStaticMethod(const BuiltinStaticMethod &old, const QoreClass* n_qc) : StaticMethodFunction(old, n_qc) {
   }

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass* n_qc) const {
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

   DLLLOCAL BuiltinDeleteBlocker(const BuiltinDeleteBlocker &old, const QoreClass* n_qc) : BuiltinNormalMethod(old, n_qc), delete_blocker(old.delete_blocker) {
   }

   DLLLOCAL bool eval(QoreObject *self, AbstractPrivateData *private_data) const {
      return delete_blocker(self, private_data);
   }

   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass* n_qc) const {
      return new BuiltinDeleteBlocker(*this, n_qc);
   }
};

#define BDELB(f) (reinterpret_cast<BuiltinDeleteBlocker *>(f))

class NormalUserMethod : public NormalMethodFunction {
public:
   DLLLOCAL NormalUserMethod(const QoreClass* n_qc, const char* mname) : NormalMethodFunction(mname, n_qc) {
   }
   DLLLOCAL NormalUserMethod(const NormalUserMethod &old, const QoreClass* n_qc) : NormalMethodFunction(old, n_qc) {
   }
   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass* n_qc) const {
      return new NormalUserMethod(*this, n_qc);
   }
};

class StaticUserMethod : public StaticMethodFunction {
public:
   DLLLOCAL StaticUserMethod(const QoreClass* n_qc, const char* mname) : StaticMethodFunction(mname, n_qc) {
   }
   DLLLOCAL StaticUserMethod(const StaticUserMethod &old, const QoreClass* n_qc) : StaticMethodFunction(old, n_qc) {
   }
   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass* n_qc) const {
      return new StaticUserMethod(*this, n_qc);
   }
};

class QoreMemberInfo {
protected:
   const QoreTypeInfo* typeInfo;

   DLLLOCAL QoreMemberInfo(const QoreMemberInfo& old) : typeInfo(old.typeInfo), exp(old.exp ? old.exp->refSelf() : 0), loc(old.loc), parseTypeInfo(0) {
      assert(!old.parseTypeInfo);
   }

public:
   AbstractQoreNode* exp;
   // store parse location in case of errors
   QoreProgramLocation loc;
   QoreParseTypeInfo* parseTypeInfo;

   DLLLOCAL QoreMemberInfo(int nfl, int nll, const QoreTypeInfo* n_typeinfo, QoreParseTypeInfo* n_parseTypeInfo, AbstractQoreNode* e = 0) :
      typeInfo(n_typeinfo), exp(e), loc(nfl, nll), parseTypeInfo(n_parseTypeInfo) {
   }

   DLLLOCAL QoreMemberInfo(int nfl, int nll, const QoreTypeInfo* n_typeinfo, AbstractQoreNode* e = 0) : typeInfo(n_typeinfo), exp(e),
         loc(nfl, nll), parseTypeInfo(0) {
   }

   DLLLOCAL QoreMemberInfo(int nfl, int nll, char* n, AbstractQoreNode* e = 0) : typeInfo(0), exp(e), loc(nfl, nll),
         parseTypeInfo(new QoreParseTypeInfo(n)) {
   }

   DLLLOCAL QoreMemberInfo(int nfl, int nll, const QoreClass* qc, AbstractQoreNode* e) : typeInfo(qc->getTypeInfo()), exp(e),
         loc(nfl, nll), parseTypeInfo(0) {
   }
   DLLLOCAL QoreMemberInfo(int nfl, int nll, AbstractQoreNode* e) : typeInfo(0), exp(e), loc(nfl, nll),
         parseTypeInfo(0) {
   }
   DLLLOCAL QoreMemberInfo(int nfl, int nll) : typeInfo(0), exp(0), loc(nfl, nll),
         parseTypeInfo(0) {
   }
   DLLLOCAL ~QoreMemberInfo() {
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
      return this ? typeInfo : 0;
   }

   DLLLOCAL bool parseHasTypeInfo() const {
      return this && (typeInfo || parseTypeInfo);
   }

   DLLLOCAL QoreMemberInfo* copy() const {
      if (!this)
         return 0;

      return new QoreMemberInfo(*this);
   }

   DLLLOCAL void parseInit(const char* name, bool priv);
};

class QoreVarInfo : public QoreMemberInfo {
protected:
   DLLLOCAL QoreVarInfo(const QoreVarInfo& old) : QoreMemberInfo(old), val(old.val), finalized(old.finalized) {
   }

   DLLLOCAL int checkFinalized(ExceptionSink* xsink) const {
         if (finalized) {
            xsink->raiseException("DESTRUCTOR-ERROR", "illegal class static variable assignment after second phase of variable destruction");
            return -1;
         }
         return 0;
      }

public:
   mutable QoreRWLock rwl;
   QoreLValueGeneric val;
   bool finalized;

   DLLLOCAL QoreVarInfo(int nfl, int nll, const QoreTypeInfo* n_typeinfo, QoreParseTypeInfo* n_parseTypeInfo, AbstractQoreNode* e = 0) :
      QoreMemberInfo(nfl, nll, n_typeinfo, n_parseTypeInfo, e), finalized(false) {
   }

   DLLLOCAL QoreVarInfo(int nfl, int nll) : QoreMemberInfo(nfl, nll), finalized(false) {
   }

   DLLLOCAL QoreVarInfo(int nfl, int nll, AbstractQoreNode* e) : QoreMemberInfo(nfl, nll, e), finalized(false) {
   }
   
   DLLLOCAL ~QoreVarInfo() {
      assert(!val.hasValue());
   }

   DLLLOCAL void clear(ExceptionSink* xsink) {
      ReferenceHolder<> tmp(xsink);
      QoreAutoRWWriteLocker al(rwl);
      if (!finalized)
         finalized = true;
      tmp = val.remove(true);
   }

   DLLLOCAL void delVar(ExceptionSink* xsink) {
      del();
      discard(val.remove(true), xsink);
   }

   DLLLOCAL QoreVarInfo* copy() const {
      if (!this)
         return 0;

      return new QoreVarInfo(*this);
   }

   DLLLOCAL void assignInit(AbstractQoreNode* v) {
      // try to set an optimized value type for the value holder if possible
      val.set(getTypeInfo());
      val.assignInitial(v);
   }

   DLLLOCAL void getLValue(LValueHelper& lvh) {
      lvh.setTypeInfo(getTypeInfo());
      lvh.setAndLock(rwl);
      if (checkFinalized(lvh.vl.xsink))
         return;
      lvh.setValue(val);
   }

   DLLLOCAL void init() {
#ifdef QORE_ENFORCE_DEFAULT_LVALUE
      // try to set an optimized value type for the value holder if possible
      val.set(getTypeInfo());
      val.assignInitial(typeInfo->getDefaultQoreValue());
#endif
   }

   DLLLOCAL AbstractQoreNode* getReferencedValue() const {
      QoreAutoRWReadLocker al(rwl);
      return val.getReferencedValue();
   }

   DLLLOCAL int64 getAsBigInt() const {
      QoreAutoRWReadLocker al(rwl);
      return val.getAsBigInt();
   }

   DLLLOCAL double getAsFloat() const {
      QoreAutoRWReadLocker al(rwl);
      return val.getAsFloat();
   }

   DLLLOCAL bool getAsBool() const {
      QoreAutoRWReadLocker al(rwl);
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

class QoreMemberMap : public member_map_t {
public:
   DLLLOCAL ~QoreMemberMap() {
      del();
   }

   DLLLOCAL void del() {
      for (member_map_t::iterator i = begin(), e = end(); i != e; ++i) {
         //printd(5, "QoreMemberMap::~QoreMemberMap() this=%p freeing pending private member %p '%s'\n", this, i->second, i->first);
         delete i->second;
         free(i->first);
      }
      clear();
   }

   DLLLOCAL bool inList(const char* name) const {
      return find((char* )name) != end();
   }
};

class QoreVarMap : public var_map_t {
public:
   DLLLOCAL ~QoreVarMap() {
      for (var_map_t::iterator i = begin(), e = end(); i != e; ++i) {
         //printd(5, "QoreVarMap::~QoreVarMap() deleting static var %s\n", i->first);
         assert(i->second->empty());
         i->second->del();
         free(i->first);
      }
   }

   DLLLOCAL void clear(ExceptionSink* xsink) {
      for (var_map_t::iterator i = begin(), e = end(); i != e; ++i) {
         i->second->clear(xsink);
      }
   }

   DLLLOCAL void del(ExceptionSink* xsink) {
      for (var_map_t::iterator i = begin(), e = end(); i != e; ++i) {
         i->second->delVar(xsink);
         free(i->first);
         delete i->second;
      }
      var_map_t::clear();
   }

   DLLLOCAL void del() {
      for (var_map_t::iterator i = begin(), e = end(); i != e; ++i) {
         assert(!i->second->val.hasValue());
         free(i->first);
         delete i->second;
      }
      var_map_t::clear();
   }

   DLLLOCAL bool inList(const char* name) const {
      return var_map_t::find((char* )name) != end();
   }

   DLLLOCAL QoreVarInfo* find(const char* name) const {
      var_map_t::const_iterator i = var_map_t::find((char* )name);
      return i == end() ? 0 : i->second;
   }
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
   DLLLOCAL void parseInit(BCList *bcl, const char* classname);
};

//typedef safe_dslist<BCANode*> bcalist_t;
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
   DLLLOCAL int execBaseClassConstructorArgs(BCEAList *bceal, ExceptionSink* xsink) const;
};

typedef std::pair<QoreClass* , bool> class_virt_pair_t;
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
   DLLLOCAL int add(QoreClass* thisclass, QoreClass* qc, bool is_virtual);
   DLLLOCAL int addBaseClassesToSubclass(QoreClass* thisclass, QoreClass* sc, bool is_virtual);
   DLLLOCAL bool isBaseClass(QoreClass* qc) const;
   DLLLOCAL QoreClass* getClass(qore_classid_t cid) const;
   //DLLLOCAL void execConstructors(QoreObject *o, BCEAList *bceal, ExceptionSink* xsink) const;
   DLLLOCAL void execDestructors(QoreObject *o, ExceptionSink* xsink) const;
   DLLLOCAL void execSystemDestructors(QoreObject *o, ExceptionSink* xsink) const;
   DLLLOCAL void execCopyMethods(QoreObject *self, QoreObject *old, ExceptionSink* xsink) const;

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
   NamedScope *cname;
   char* cstr;
   QoreClass* sclass;
   bool priv : 1;
   bool is_virtual : 1;
   
   DLLLOCAL BCNode(NamedScope *c, bool p) : loc(ParseLocation), cname(c), cstr(0), sclass(0), priv(p), is_virtual(false) {
   }

   // this method takes ownership of *str
   DLLLOCAL BCNode(char* str, bool p) : loc(ParseLocation), cname(0), cstr(str), sclass(0), priv(p), is_virtual(false) {
   }
   
   // for builtin base classes
   DLLLOCAL BCNode(QoreClass* qc, bool n_virtual = false)
      : loc(ParseLocation), cname(0), cstr(0), sclass(qc), priv(false), is_virtual(n_virtual) {
   }

   // called at runtime with committed classes
   DLLLOCAL BCNode(const BCNode &old) : loc(old.loc), cname(0), cstr(0), sclass(old.sclass), priv(old.priv), is_virtual(old.is_virtual) {
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
   // returns -1 if a recursive reference is found, 0 if not
   DLLLOCAL int initialize(QoreClass* cls, bool &has_delete_blocker, qcp_set_t& qcp_set);
   DLLLOCAL const QoreClass* getClass(qore_classid_t cid, bool &n_priv) const {
      // sclass can be 0 if the class could not be found during parse initialization
      if (!sclass)
         return 0;

      const QoreClass* qc = (sclass->getID() == cid) ? sclass : sclass->getClassIntern(cid, n_priv);
      if (qc && !n_priv && priv)
	 n_priv = true;
      return qc;
   }

   DLLLOCAL const QoreClass* getClass(const qore_class_private& qc, bool& n_priv) const;
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
   bool valid;

   DLLLOCAL BCList(BCNode* n) : valid(true) {
      push_back(n);
   }

   DLLLOCAL BCList() : valid(true) {
   }

   DLLLOCAL BCList(const BCList& old) : sml(old.sml), valid(true) {
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
   DLLLOCAL const QoreMethod* parseResolveSelfMethod(const char* name);

   // only looks in committed method lists
   DLLLOCAL const QoreMethod* parseFindCommittedMethod(const char* name);
   //DLLLOCAL const QoreMethod* parseFindCommittedStaticMethod(const char* name);

   // looks in committed and pending method lists
   DLLLOCAL const QoreMethod* parseFindMethodTree(const char* name);
   DLLLOCAL const QoreMethod* parseFindStaticMethodTree(const char* name);
   DLLLOCAL const QoreMethod* parseFindAnyMethodTree(const char* name);

   DLLLOCAL const QoreMethod* findCommittedMethod(const char* name, bool &priv_flag) const;
   DLLLOCAL const QoreMethod* findCommittedStaticMethod(const char* name, bool &priv_flag) const;

   DLLLOCAL bool match(const QoreClass* cls);
   DLLLOCAL void execConstructors(QoreObject *o, BCEAList *bceal, ExceptionSink* xsink) const;
   DLLLOCAL bool execDeleteBlockers(QoreObject *o, ExceptionSink* xsink) const;
   DLLLOCAL bool parseCheckHierarchy(const QoreClass* cls) const;
   DLLLOCAL bool isPrivateMember(const char* str) const;
   // member_has_type_info could return true while typeInfo is 0 if it has unresolved parse type information
   DLLLOCAL const QoreClass* parseFindPublicPrivateMember(const QoreProgramLocation*& loc, const char* mem, const QoreTypeInfo*& memberTypeInfo, bool &has_type_info, bool &priv) const;
   DLLLOCAL const QoreClass* parseFindPublicPrivateVar(const QoreProgramLocation*& loc, const char* mem, const QoreTypeInfo*& varTypeInfo, bool &has_type_info, bool &priv) const;
   DLLLOCAL bool runtimeGetMemberInfo(const char* mem, const QoreTypeInfo*& memberTypeInfo, bool &priv) const;
   DLLLOCAL bool parseHasPublicMembersInHierarchy() const;
   DLLLOCAL const qore_class_private* isPublicOrPrivateMember(const char* mem, bool &priv) const;
   DLLLOCAL const QoreClass* getClass(qore_classid_t cid, bool &priv) const {
      for (bclist_t::const_iterator i = begin(), e = end(); i != e; ++i) {
	 const QoreClass* qc = (*i)->getClass(cid, priv);
	 if (qc)
	    return qc;
      }
	 
      return 0;
   }

   DLLLOCAL const QoreClass* getClass(const qore_class_private& qc, bool &priv) const;

   DLLLOCAL void addNewAncestors(QoreMethod* m);
   DLLLOCAL void addAncestors(QoreMethod* m);
   DLLLOCAL void addNewStaticAncestors(QoreMethod* m);
   DLLLOCAL void addStaticAncestors(QoreMethod* m);
   DLLLOCAL void parseAddAncestors(QoreMethod* m);
   DLLLOCAL void parseAddStaticAncestors(QoreMethod* m);
   DLLLOCAL AbstractQoreNode* parseFindConstantValue(const char* cname, const QoreTypeInfo*& typeInfo, bool check);
   DLLLOCAL QoreVarInfo* parseFindStaticVar(const char* vname, const QoreClass*& qc, bool check) const;

   DLLLOCAL void resolveCopy();

   DLLLOCAL MethodVariantBase* matchNonAbstractVariant(const std::string& name, MethodVariantBase* v) const;
};

// BCEANode
// base constructor evaluated argument node; created locally at run time
class BCEANode {
public:
   QoreListNode* args;
   const AbstractQoreFunctionVariant* variant;
   bool execed;
      
   DLLLOCAL BCEANode(QoreListNode* n_args, const AbstractQoreFunctionVariant* n_variant) : args(n_args), variant(n_variant), execed(false) {}
   DLLLOCAL BCEANode() : args(0), variant(0), execed(true) {}
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
   DLLLOCAL ~BCEAList() { }
   
public:
   DLLLOCAL void deref(ExceptionSink* xsink);
   // evaluates arguments, returns -1 if an exception was thrown
   DLLLOCAL int add(qore_classid_t classid, const QoreListNode* arg, const AbstractQoreFunctionVariant* variant, ExceptionSink* xsink);
   DLLLOCAL QoreListNode* findArgs(qore_classid_t classid, bool *aexeced, const AbstractQoreFunctionVariant*& variant);
};

struct SelfInstantiatorHelper {
   LocalVar *selfid;
   ExceptionSink* xsink;
   DLLLOCAL SelfInstantiatorHelper(LocalVar *n_selfid, QoreObject *self, ExceptionSink* n_xsink) : selfid(n_selfid), xsink(n_xsink) {
      selfid->instantiate_object(self);
   }
   DLLLOCAL ~SelfInstantiatorHelper() {
      selfid->uninstantiate(xsink);
   }
};

// signature hash size - we use SHA1 for performance reasons (and because we don't necessarily need the best cryptographic security)
#define SH_SIZE 20

// class "signature" hash for comparing classes with the same name from different program objects at runtime
class SignatureHash {
private:
   // not implemented
   SignatureHash& operator=(const SignatureHash&);

protected:
   unsigned char buf[SH_SIZE];
   bool is_set;

   DLLLOCAL void set(const QoreString& str);

public:
   DLLLOCAL SignatureHash() : is_set(false) {
      memset(buf, 0, SH_SIZE);
   }

   DLLLOCAL SignatureHash(const SignatureHash& old) : is_set(old.is_set) {
      if (is_set)
         memcpy(buf, old.buf, SH_SIZE);
   }

   DLLLOCAL void update(const QoreString& str);

   DLLLOCAL bool operator==(const SignatureHash& other) const {
      // if either one of the hashes is not set, then the comparison always fails
      if (!is_set || !other.is_set)
         return false;
      return !memcmp(buf, other.buf, SH_SIZE);
   }

   DLLLOCAL operator bool() const {
      return is_set;
   }

   DLLLOCAL void toString(QoreString& str) const {
      for (unsigned i = 0; i < SH_SIZE; ++i) 
         str.sprintf("%02x", buf[i]);
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

   hm_method_t hm,               // "normal" (non-static) method map
      shm;                       // static method map

   AbstractMethodMap ahm;        // holds abstract variants with no implementation in the current class

   ConstantList pend_pub_const,  // pending public constants
      pend_priv_const,           // pending private constants
      pub_const,                 // committed public constants
      priv_const;                // committed private constants

   QoreMemberMap private_members, pending_private_members; // private member lists (maps)
   QoreMemberMap public_members, pending_public_members;   // public member lists (maps)

   QoreVarMap private_vars, pending_private_vars; // private static var lists (maps)
   QoreVarMap public_vars, pending_public_vars;   // public static var lists (maps)

   const QoreMethod* system_constructor, *constructor, *destructor,
      *copyMethod, *methodGate, *memberGate, *deleteBlocker,
      *memberNotification;

   qore_classid_t classID,          // class ID
      methodID;                     // for subclasses of builtin classes that will not have their own private data,
                                    // instead they will get the private data from this class

   bool sys,                        // system/builtin class?
      initialized,                  // is initialized? (only performed once)
      parse_init_called,            // has parseInit() been called? (performed once for each parseCommit())
      parse_init_partial_called,    // has parseInitPartial() been called? (performed once for each parseCommit())
      has_delete_blocker,           // has a delete_blocker function somewhere in the hierarchy?
      has_public_memdecl,           // has a public member declaration somewhere in the hierarchy?
      pending_has_public_memdecl,   // has a pending public member declaration in this class?
      owns_typeinfo,                // do we own the typeInfo data or not?
      resolve_copy_done,            // has the copy already been resolved
      has_new_user_changes,         // does the class have new user code that needs to be processed?
      owns_ornothingtypeinfo,       // do we own the "or nothing" type info
      pub,                          // is a public class (modules only)
      final                         // is the class "final" (cannot be inherited)
      ;

   int64 domain;                    // capabilities of builtin class to use in the context of parse restrictions
   QoreReferenceCounter nref;       // namespace references

   unsigned num_methods, num_user_methods, num_static_methods, num_static_user_methods;

   // type information for the class, may not have a pointer to the same QoreClass
   // as the actual owning class in case of a copy
   QoreTypeInfo* typeInfo,
      *orNothingTypeInfo;

   // common "self" local variable for all constructors
   mutable LocalVar selfid;

   // class "signature" hash for comparing classes with the same name from different program objects at runtime
   SignatureHash hash;

   // user-specific data
   const void* ptr;

   // pointer to new class when copying
   mutable QoreClass* new_copy;

   DLLLOCAL qore_class_private(QoreClass* n_cls, const char* nme, int64 dom = QDOM_DEFAULT, QoreTypeInfo* n_typeinfo = 0);

   // only called while the parse lock for the QoreProgram owning "old" is held
   DLLLOCAL qore_class_private(const qore_class_private &old, QoreClass* n_cls);

   DLLLOCAL ~qore_class_private();

   DLLLOCAL bool hasAbstract() const {
      return !ahm.empty();
   }

   DLLLOCAL void parseCheckAbstractNew() {
      parseInit();
      ahm.parseCheckAbstractNew(name.c_str());
   }

   DLLLOCAL void setNamespace(qore_ns_private* n) {
      ns = n;
   }

   DLLLOCAL void resolveCopy();

   DLLLOCAL void setUserData(const void *n_ptr) {
      ptr = n_ptr;
   }

   DLLLOCAL const void *getUserData() const {
      return ptr;
   }

   DLLLOCAL const QoreTypeInfo* getTypeInfo() const {
      return typeInfo;
   }

   DLLLOCAL const QoreTypeInfo* getOrNothingTypeInfo() const {
      return orNothingTypeInfo;
   }

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

   DLLLOCAL const QoreExternalMethodVariant* findUserMethodVariant(const char* name, const QoreMethod*& method, const type_vec_t &argTypeList) const;

   DLLLOCAL int parseCheckMemberAccess(const QoreProgramLocation& loc, const char* mem, const QoreTypeInfo*& memberTypeInfo, int pflag) const {
      const_cast<qore_class_private*>(this)->parseInitPartial();

      bool priv;
      bool has_type_info;
      const QoreProgramLocation* mloc = 0;
      const QoreClass* sclass = parseFindPublicPrivateMember(mloc, mem, memberTypeInfo, has_type_info, priv);
      
      if (!sclass) {
	 int rc = 0;
	 if (!parseHasMemberGate() || (pflag & PF_FOR_ASSIGNMENT)) {
	    if (parse_check_parse_option(PO_REQUIRE_TYPES)) {
	       parse_error(loc, "member '%s' of class '%s' referenced has no type information because it was not declared in a public or private member list, but parse options require type information for all declarations",
	             mem, name.c_str());
	       rc = -1;
	    }
	    if (parseHasPublicMembersInHierarchy()) {
	       //printd(5, "qore_class_private::parseCheckMemberAccess() %s %%.%s memberGate=%d pflag=%d\n", name.c_str(), mem, parseHasMemberGate(), pflag);
	       parse_error(loc, "illegal access to unknown member '%s' in class '%s' which hash a public member list (or inherited public member list)", mem, name.c_str());
	       rc = -1;
	    }
	 }
	 return rc;
      }

      // only raise a parse error for illegal access to private members if there is not memberGate function
      if (priv && !parseHasMemberGate() && !parseCheckPrivateClassAccess()) {
	 memberTypeInfo = 0;
         parse_error(loc, "illegal access to private member '%s' of class '%s'", mem, name.c_str());
	 return -1;
      }
      return 0;
   }

   DLLLOCAL int parseResolveInternalMemberAccess(const char* mem, const QoreTypeInfo*& memberTypeInfo) const {
      const_cast<qore_class_private *>(this)->parseInitPartial();

      bool priv, has_type_info;
      const QoreProgramLocation* loc = 0;
      const QoreClass* sclass = parseFindPublicPrivateMember(loc, mem, memberTypeInfo, has_type_info, priv);
      return sclass ? 0 : -1;
   }

   DLLLOCAL int parseCheckInternalMemberAccess(const char* mem, const QoreTypeInfo*& memberTypeInfo, const QoreProgramLocation& loc) const {
      const_cast<qore_class_private *>(this)->parseInitPartial();

      // throws a parse exception if there are public members and the name is not valid
      bool priv, has_type_info;
      const QoreProgramLocation* l = 0;
      const QoreClass* sclass = parseFindPublicPrivateMember(l, mem, memberTypeInfo, has_type_info, priv);
      int rc = 0;
      if (!sclass) {
	 if (parse_check_parse_option(PO_REQUIRE_TYPES)) {
	    parse_error(loc, "member '%s' of class '%s' referenced has no type information because it was not declared in a public or private member list, but parse options require type information for all declarations", mem, name.c_str());
	    rc = -1;
	 }
	 if (parseHasPublicMembersInHierarchy()) {
            parse_error(loc, "illegal access to unknown member '%s' in class '%s' which hash a public member list (or inherited public member list)", mem, name.c_str());
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
   DLLLOCAL bool runtimeGetMemberInfo(const char* mem, const QoreTypeInfo*& memberTypeInfo, bool &priv) const {
      member_map_t::const_iterator i = private_members.find(const_cast<char*>(mem));
      if (i != private_members.end()) {
	 priv = true;
	 memberTypeInfo = i->second->getTypeInfo();
	 return true;
      }
      
      i = public_members.find(const_cast<char*>(mem));
      if (i != public_members.end()) {
	 priv = false;
	 memberTypeInfo = i->second->getTypeInfo();
	 return true;
      }

      return scl ? scl->runtimeGetMemberInfo(mem, memberTypeInfo, priv) : false;
   }

   DLLLOCAL const QoreClass* parseFindPublicPrivateMember(const QoreProgramLocation*& loc, const char* mem, const QoreTypeInfo*& memberTypeInfo, bool &has_type_info, bool &priv) const {
      const_cast<qore_class_private*>(this)->initialize();
      return parseFindPublicPrivateMemberNoInit(loc, mem, memberTypeInfo, has_type_info, priv);
   }

   DLLLOCAL const QoreClass* parseFindPublicPrivateMemberNoInit(const QoreProgramLocation*& loc, const char* mem, const QoreTypeInfo*& memberTypeInfo, bool &has_type_info, bool &priv) const {
      bool found = false;
      member_map_t::const_iterator i = private_members.find(const_cast<char*>(mem));

      if (i != private_members.end())
	 found = true;
      else {
	 i = pending_private_members.find(const_cast<char*>(mem));
	 if (i != pending_private_members.end())
	    found = true;
      }

      if (found) {
         //printd(5, "qore_class_private:::parseFindPublicPrivateMember() this: %p %s.%s found private\n", this, name.c_str(), mem);

	 priv = true;
	 has_type_info = i->second->parseHasTypeInfo();
	 memberTypeInfo = i->second->getTypeInfo();
	 loc = &i->second->loc;
	 return cls;
      }

      i = public_members.find(const_cast<char*>(mem));
      if (i != public_members.end())
	 found = true;
      else {
	 i = pending_public_members.find(const_cast<char*>(mem));
	 if (i != pending_public_members.end())
	    found = true;
      }

      if (found) {
         //printd(5, "qore_class_private:::parseFindPublicPrivateMember() this: %p %s.%s found public\n", this, name.c_str(), mem);

	 priv = false;
	 has_type_info = i->second->parseHasTypeInfo();
	 memberTypeInfo = i->second->getTypeInfo();
         loc = &i->second->loc;
	 return cls;
      }

      return scl ? scl->parseFindPublicPrivateMember(loc, mem, memberTypeInfo, has_type_info, priv) : 0;
   }

   DLLLOCAL const QoreClass* parseFindPublicPrivateVar(const QoreProgramLocation*& loc, const char* dname, const QoreTypeInfo*& varTypeInfo, bool &var_has_type_info, bool &priv) const {
      //printd(5, "parseFindPublicPrivateVar() this=%p cls=%p (%s) scl=%p\n", this, cls, cls->getName(), scl);

      QoreVarInfo* vi = private_vars.find(const_cast<char*>(dname));
      if (!vi)
	 vi = pending_private_vars.find(const_cast<char*>(dname));

      if (vi) {
	 priv = true;
	 var_has_type_info = vi->parseHasTypeInfo();
	 varTypeInfo = vi->getTypeInfo();
	 loc = &vi->loc;
	 return cls;
      }

      vi = public_vars.find(const_cast<char*>(dname));
      if (!vi)
	 vi = pending_public_vars.find(const_cast<char*>(dname));

      if (vi) {
	 priv = false;
	 var_has_type_info = vi->parseHasTypeInfo();
	 varTypeInfo = vi->getTypeInfo();
         loc = &vi->loc;
	 return cls;
      }

      return scl ? scl->parseFindPublicPrivateVar(loc, dname, varTypeInfo, var_has_type_info, priv) : 0;
   }

   DLLLOCAL int checkExistingVarMember(const QoreProgramLocation& loc, char* dname, bool decl_has_type_info, bool priv, const QoreClass* sclass, bool has_type_info, bool is_priv, bool var = false) const;

   DLLLOCAL int parseCheckVar(char* dname, bool decl_has_type_info, bool priv) const {
      const QoreTypeInfo* varTypeInfo;
      bool has_type_info;
      bool is_priv;
      const QoreProgramLocation* loc = 0;
      const QoreClass* sclass = parseFindPublicPrivateVar(loc, dname, varTypeInfo, has_type_info, is_priv);
      //printd(5, "parseCheckVar() %s cls=%p (%s)\n", dname, sclass, sclass ? sclass->getName() : "n/a");
      if (!sclass) {
         if (parseHasConstant(dname)) {
            parse_error("'%s' has already been declared as a constant in class '%s' and therefore cannot be also declared as a static class variable in the same class with the same name", dname, name.c_str());
            return -1;
         }
	 return 0;
      }

      return checkExistingVarMember(*loc, dname, decl_has_type_info, priv, sclass, has_type_info, is_priv, true);
   }

   DLLLOCAL int parseCheckMember(char* mem, bool decl_has_type_info, bool priv) const {
      const QoreTypeInfo* memberTypeInfo;
      bool has_type_info;
      bool is_priv;
      const QoreProgramLocation* loc = 0;
      const QoreClass* sclass = parseFindPublicPrivateMemberNoInit(loc, mem, memberTypeInfo, has_type_info, is_priv);
      if (!sclass)
	 return 0;

      return checkExistingVarMember(*loc, mem, decl_has_type_info, priv, sclass, has_type_info, is_priv);
   }

   DLLLOCAL int parseCheckMemberInBaseClasses(char* mem, bool decl_has_type_info, bool priv) const {
      const QoreTypeInfo* memberTypeInfo;
      bool has_type_info;
      bool is_priv;
      const QoreProgramLocation* loc = 0;
      const QoreClass* sclass = scl ? scl->parseFindPublicPrivateMember(loc, mem, memberTypeInfo, has_type_info, is_priv) : 0;
      if (!sclass)
	 return 0;

      return checkExistingVarMember(*loc, mem, decl_has_type_info, priv, sclass, has_type_info, is_priv);
   }

   DLLLOCAL void parseAddPrivateMember(char* mem, QoreMemberInfo* MemberInfo) {
      if (!parseCheckMember(mem, MemberInfo->parseHasTypeInfo(), true)) {
	 if (!has_new_user_changes)
	    has_new_user_changes = true;

	 //printd(5, "qore_class_private::parseAddPrivateMember() this=%p %s adding %p %s\n", this, name.c_str(), mem, mem);
	 pending_private_members[mem] = MemberInfo;
	 return;
      }

      free(mem);
      delete MemberInfo;
   }

   DLLLOCAL void parseAddPrivateStaticVar(char* dname, QoreVarInfo* VarInfo) {
      if (!parseCheckVar(dname, VarInfo->parseHasTypeInfo(), true)) {
	 if (!has_new_user_changes)
	    has_new_user_changes = true;

	 //printd(5, "qore_class_private::parseAddPrivateStaticVar() this=%p %s adding %p %s\n", this, name.c_str(), mem, mem);
	 pending_private_vars[dname] = VarInfo;
	 return;
      }

      free(dname);
      delete VarInfo;
   }

   DLLLOCAL void parseAddPublicStaticVar(char* dname, QoreVarInfo* VarInfo) {
      if (!parseCheckVar(dname, VarInfo->parseHasTypeInfo(), false)) {
	 if (!has_new_user_changes)
	    has_new_user_changes = true;

	 //printd(5, "QoreClass::parseAddPublicStaticVar() this=%p %s adding %p %s\n", this, name.c_str(), mem, mem);
	 pending_public_vars[dname] = VarInfo;
	 return;
      }

      free(dname);
      delete VarInfo;
   }

   DLLLOCAL void addBuiltinConstant(const char* cname, AbstractQoreNode* value, bool priv = false, const QoreTypeInfo* cTypeInfo = 0) {
      assert(!pub_const.inList(cname));
      assert(!priv_const.inList(cname));
      if (priv)
         priv_const.add(cname, value, cTypeInfo);
      else
         pub_const.add(cname, value, cTypeInfo);
   }

   DLLLOCAL void addBuiltinStaticVar(const char* vname, AbstractQoreNode* value, bool priv = false, const QoreTypeInfo* vTypeInfo = 0) {
      assert(!public_vars.inList(vname));
      assert(!private_vars.inList(vname));

      QoreVarInfo* vi = new QoreVarInfo(0, 0, vTypeInfo, 0, value);

      if (priv)
         private_vars[strdup(vname)] = vi;
      else
         public_vars[strdup(vname)] = vi;
   }

   DLLLOCAL void parseAssimilatePublicConstants(ConstantList &cmap) {
      pend_pub_const.assimilate(cmap, pub_const, priv_const, pend_priv_const, false, name.c_str());
   }

   DLLLOCAL void parseAssimilatePrivateConstants(ConstantList &cmap) {
      pend_priv_const.assimilate(cmap, priv_const, pub_const, pend_pub_const, true, name.c_str());
   }

   DLLLOCAL void parseAddPublicConstant(const std::string &cname, AbstractQoreNode* val) {
      if (parseHasVar(cname.c_str())) {
         parse_error("'%s' has already been declared as a static variable in class '%s' and therefore cannot be also declared as a constant in the same class with the same name", cname.c_str(), name.c_str());
         val->deref(0);
         return;
      }
      //printd(5, "parseAddPublicConstant() this=%p cls=%p const=%s\n", this, cls, cname.c_str());
      
      pend_pub_const.parseAdd(cname, val, pub_const, priv_const, pend_priv_const, false, name.c_str());
   }

   DLLLOCAL bool parseHasVar(const char* vn) {
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

   DLLLOCAL AbstractQoreNode* parseFindLocalConstantValue(const char* cname, const QoreTypeInfo*& cTypeInfo) {
      parseInitPartial();

      // first check public constants
      AbstractQoreNode* rv = pub_const.find(cname, cTypeInfo);
      if (!rv) {
	 rv = pend_pub_const.find(cname, cTypeInfo);
	 if (!rv) {
	    // now check private constants
	    rv = priv_const.find(cname, cTypeInfo);
	    if (!rv)
	       rv = pend_priv_const.find(cname, cTypeInfo);
	 
	    // check for accessibility to private constants
	    if (rv && !parseCheckPrivateClassAccess()) {
	       rv = 0;
	       typeInfo = 0;
	    }
	 }
      }

      //printd(5, "qore_class_private::parseFindLocalConstantValue(%s) this=%p (cls=%p %s) rv=%p\n", cname, this, cls, name.c_str(), rv);
      return rv;
   }

   DLLLOCAL AbstractQoreNode* parseFindConstantValue(const char* cname, const QoreTypeInfo*& cTypeInfo, bool check = false) {
      parseInitPartial();

      bool priv = false;

      // first check public constants
      AbstractQoreNode* rv = pub_const.find(cname, cTypeInfo);
      if (!rv) {
	 rv = pend_pub_const.find(cname, cTypeInfo);
	 if (!rv) {
	    // now check private constants
	    rv = priv_const.find(cname, cTypeInfo);
	    if (!rv)
	       rv = pend_priv_const.find(cname, cTypeInfo);
            if (rv)
               priv = true;
	 }
      }

      // check for accessibility to private constants
      if (rv) {
         if (check && priv && !parseCheckPrivateClassAccess()) {
            cTypeInfo = 0;
            return 0;
         }

         return rv;
      }

      return scl ? scl->parseFindConstantValue(cname, cTypeInfo, check) : 0;
   }

   DLLLOCAL QoreVarInfo* parseFindLocalStaticVar(const char* vname) const {
      QoreVarInfo* vi = public_vars.find(vname);
      if (!vi) {
         vi = pending_public_vars.find(vname);
         if (!vi) {
            vi = private_vars.find(vname);
            if (!vi)
               vi = pending_private_vars.find(vname);
            if (vi && !parseCheckPrivateClassAccess())
               vi = 0;
         }
      }

      return vi;
   }

   DLLLOCAL QoreVarInfo* parseFindStaticVar(const char* vname, const QoreClass*& qc, bool check = false) const {
      bool priv = false;

      QoreVarInfo* vi = public_vars.find(vname);

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
         if (check && priv && vi && !parseCheckPrivateClassAccess()) {
            return 0;
         }
         qc = cls;
         return vi;
      }

      return scl ? scl->parseFindStaticVar(vname, qc, check) : 0;
   }

   DLLLOCAL void parseAddPublicMember(char* mem, QoreMemberInfo* MemberInfo) {
      if (!parseCheckMember(mem, MemberInfo->parseHasTypeInfo(), false)) {
	 if (!has_new_user_changes)
	    has_new_user_changes = true;

	 //printd(5, "QoreClass::parseAddPublicMember() this=%p %s adding %p %s\n", this, name.c_str(), mem, mem);
	 pending_public_members[mem] = MemberInfo;
	 if (!pending_has_public_memdecl)
	    pending_has_public_memdecl = true;
	 return;
      }

      free(mem);
      delete MemberInfo;
   }

   DLLLOCAL void addPublicMember(const char* mem, const QoreTypeInfo* n_typeinfo, AbstractQoreNode* initial_value) {
      assert(public_members.find((char*)name.c_str()) == public_members.end());
      public_members[strdup(mem)] = new QoreMemberInfo(0, 0, n_typeinfo, 0, initial_value);
      if (!has_public_memdecl)
	 has_public_memdecl = true;
   }

   DLLLOCAL void addPrivateMember(const char* mem, const QoreTypeInfo* n_typeinfo, AbstractQoreNode* initial_value) {
      assert(private_members.find((char*)name.c_str()) == private_members.end());
      private_members[strdup(mem)] = new QoreMemberInfo(0, 0, n_typeinfo, 0, initial_value);
   }

   DLLLOCAL void insertBuiltinStaticMethod(QoreMethod* m) {
      assert(m->isStatic());
      //printd(5, "QoreClass::insertBuiltinStaticMethod() %s::%s() size=%d\n", name.c_str(), m->getName(), numMethods());
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
      //printd(5, "QoreClass::insertBuiltinMethod() %s::%s() size=%d\n", name.c_str(), m->getName(), numMethods());
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

   DLLLOCAL const qore_class_private* isPublicOrPrivateMember(const char* mem, bool &priv) const {
      if (private_members.find(const_cast<char*>(mem)) != private_members.end()) {
	 priv = true;
	 return this;
      }

      if (public_members.find(const_cast<char*>(mem)) != public_members.end()) {
	 priv = false;
	 return this;
      }
      
      return scl ? scl->isPublicOrPrivateMember(mem, priv) : 0;
   }

   DLLLOCAL int initMembers(QoreObject *o, member_map_t::const_iterator i, member_map_t::const_iterator e, ExceptionSink* xsink) const;

   DLLLOCAL int initVar(const char* vname, QoreVarInfo& vi, ExceptionSink* xsink) const {
      if (vi.exp) {
         // evaluate expression
         ReferenceHolder<AbstractQoreNode> val(vi.exp->eval(xsink), xsink);
         if (*xsink)
            return -1;

         val = vi.getTypeInfo()->acceptInputMember(vname, val.release(), xsink);
         if (*xsink)
            return -1;

         vi.assignInit(val.release());
      }
      else
         vi.init();

      return 0;
   }

   DLLLOCAL void clearConstants(QoreListNode& l) {
      priv_const.clear(l);
      pub_const.clear(l);
   }

   DLLLOCAL void clear(ExceptionSink* xsink) {
      private_vars.clear(xsink);
      public_vars.clear(xsink);
   }

   DLLLOCAL void deleteClassData(ExceptionSink* xsink) {
      private_vars.del(xsink);
      public_vars.del(xsink);
      priv_const.deleteAll(xsink);
      pub_const.deleteAll(xsink);
   }

   DLLLOCAL int initMembers(QoreObject *o, ExceptionSink* xsink) const {
      if (public_members.empty() && private_members.empty())
	 return 0;

      SelfInstantiatorHelper sih(&selfid, o, xsink);

      if (initMembers(o, private_members.begin(), private_members.end(), xsink)
	  || initMembers(o, public_members.begin(), public_members.end(), xsink))
	 return -1;
      return 0;
   }

   DLLLOCAL const QoreMethod* getMethodForEval(const char* nme, ExceptionSink* xsink) const;

   DLLLOCAL QoreObject *execConstructor(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, ExceptionSink* xsink) const;

   DLLLOCAL void addBuiltinMethod(const char* mname, MethodVariantBase *variant);
   DLLLOCAL void addBuiltinStaticMethod(const char* mname, MethodVariantBase *variant);
   DLLLOCAL void addBuiltinConstructor(BuiltinConstructorVariantBase *variant);
   DLLLOCAL void addBuiltinDestructor(BuiltinDestructorVariantBase *variant);
   DLLLOCAL void addBuiltinCopyMethod(BuiltinCopyVariantBase *variant);
   DLLLOCAL void setDeleteBlocker(q_delete_blocker_t func);
   DLLLOCAL void setBuiltinSystemConstructor(BuiltinSystemConstructorBase *m);

   DLLLOCAL void execBaseClassConstructor(QoreObject *self, BCEAList *bceal, ExceptionSink* xsink) const;
   DLLLOCAL QoreObject *execSystemConstructor(QoreObject *self, int code, va_list args) const;
   DLLLOCAL bool execDeleteBlocker(QoreObject *self, ExceptionSink* xsink) const;
   DLLLOCAL QoreObject *execCopy(QoreObject *old, ExceptionSink* xsink) const;

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

   // returns a non-static method if it exists in class hierarchy and has been committed to the class, initializes base classes if necessary
   DLLLOCAL const QoreMethod* parseFindCommittedMethod(const char* nme) {
      const QoreMethod* m = findLocalCommittedMethod(nme);
      if (!m && scl)
	 m = scl->parseFindCommittedMethod(nme);
      return m;
   }

/*
   // returns a static method if it exists in the local class and has been committed to the class, initializes base classes if necessary
   DLLLOCAL const QoreMethod* parseFindCommittedStaticMethod(const char* nme) {
      const QoreMethod* m = findLocalCommittedStaticMethod(nme);
      if (!m && scl)
	 m = scl->parseFindCommittedStaticMethod(nme);
      return m;
   }
*/

   DLLLOCAL const QoreMethod* parseFindAnyMethodIntern(const char* mname) {
      const QoreMethod* m = parseFindAnyLocalMethod(mname);
      if (!m && scl)
	 m = scl->parseFindAnyMethodTree(mname);
      return m;
   }

   // finds a non-static method in the class hierarchy at parse time, optionally initializes classes
   DLLLOCAL const QoreMethod* parseFindMethod(const char* mname) {
      const QoreMethod* m = parseFindLocalMethod(mname);
      if (!m && scl)
	 m = scl->parseFindMethodTree(mname);
      return m;
   }

   // finds a static method in the class hierarchy at parse time, optionally initializes classes
   DLLLOCAL const QoreMethod* parseFindStaticMethod(const char* mname) {
      const QoreMethod* m = parseFindLocalStaticMethod(mname);
      if (!m && scl)
	 m = scl->parseFindStaticMethodTree(mname);
      return m;
   }

   // returns a non-static method if it exists in class hierarchy and has been committed to the class
   DLLLOCAL const QoreMethod* findCommittedStaticMethod(const char* nme, bool &p) const {
      const QoreMethod* w = findLocalCommittedStaticMethod(nme);
      if (!w && scl)
	 w = scl->findCommittedStaticMethod(nme, p);
      return w;
   }

   // returns a non-static method if it exists in class hierarchy and has been committed to the class
   DLLLOCAL const QoreMethod* findCommittedMethod(const char* nme, bool &p) const {
      const QoreMethod* w = findLocalCommittedMethod(nme);
      if (!w && scl)
	 w = scl->findCommittedMethod(nme, p);
      return w;
   }

   DLLLOCAL const QoreMethod* findStaticMethod(const char* nme, bool &priv_flag) const {
      const QoreMethod* w;
      if (!(w = findLocalCommittedStaticMethod(nme))) {
	 // search superclasses
	 if (scl)
	    w = scl->findCommittedStaticMethod(nme, priv_flag);
      }
      return w;
   }

   const QoreMethod* findMethod(const char* nme, bool &priv_flag) const {
      const QoreMethod* w;
      if (!(w = findLocalCommittedMethod(nme))) {
	 // search superclasses
	 if (scl)
	    w = scl->findCommittedMethod(nme, priv_flag);
      }
      return w;
   }

   DLLLOCAL bool hasCallableMethod(const char* m, int mask) const;

   DLLLOCAL void execDestructor(QoreObject *self, ExceptionSink* xsink) const;

   DLLLOCAL void execBaseClassDestructor(QoreObject *self, ExceptionSink* xsink) const;

   DLLLOCAL void execBaseClassSystemDestructor(QoreObject *self, ExceptionSink* xsink) const;

   DLLLOCAL void execBaseClassCopy(QoreObject *self, QoreObject *old, ExceptionSink* xsink) const;

   DLLLOCAL void parseInit();
   DLLLOCAL void parseCommit();
   DLLLOCAL void parseRollback();
   DLLLOCAL int addUserMethod(const char* mname, MethodVariantBase *f, bool n_static);

   DLLLOCAL AbstractQoreNode* evalPseudoMethod(const AbstractQoreNode* n, const char* name, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL int64 bigIntEvalPseudoMethod(const AbstractQoreNode* n, const char* name, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL int intEvalPseudoMethod(const AbstractQoreNode* n, const char* name, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL bool boolEvalPseudoMethod(const AbstractQoreNode* n, const char* name, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL double floatEvalPseudoMethod(const AbstractQoreNode* n, const char* name, const QoreListNode* args, ExceptionSink* xsink) const;

   DLLLOCAL AbstractQoreNode* evalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL int64 bigIntEvalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL int intEvalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL bool boolEvalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL double floatEvalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const;

   DLLLOCAL const QoreMethod* parseResolveSelfMethodIntern(const char* nme) {
      const QoreMethod* m = parseFindLocalMethod(nme);
      if (!m)
         m = parseFindLocalStaticMethod(nme);

      // if still not found now look in superclass methods
      if (!m && scl)
         m = scl->parseResolveSelfMethod(nme);

      return m;
   }
   
   const QoreMethod* findPseudoMethod(const AbstractQoreNode* n, const char* nme, ExceptionSink* xsink) const {
      const QoreMethod* w;
      bool priv_flag = false;
      if (!(w = findMethod(nme, priv_flag))) {
         qore_type_t t = get_node_type(n);
         // throw an exception
         if (t == NT_OBJECT) {
            const char* cname = reinterpret_cast<const QoreObject*>(n)->getClassName();
            xsink->raiseException("METHOD-DOES-NOT-EXIST", "no method %s::%s() has been defined and no pseudo-method %s::%s() is available", cname, nme, name.c_str(), nme);
         }
         else
            xsink->raiseException("PSEUDO-METHOD-DOES-NOT-EXIST", "no pseudo method <%s>::%s() has been defined", get_type_name(n), nme);
         return 0;
      }
      
      return w;
   }

   DLLLOCAL bool parseCheckPrivateClassAccess() const;
   DLLLOCAL bool runtimeCheckPrivateClassAccess() const;

   // this = class to find in "oc"
   DLLLOCAL qore_type_result_e parseCheckCompatibleClass(const qore_class_private& oc) const;
   // this = class to find in "oc"
   DLLLOCAL qore_type_result_e runtimeCheckCompatibleClass(const qore_class_private& oc) const;

   DLLLOCAL const QoreClass* getClassIntern(const qore_class_private& qc, bool &priv) const {
      // check hashes if names are the same
      // FIXME: check fully-qualified namespace name
      if (qc.classID == classID || (qc.name == name && qc.hash == hash))
         return cls;

#ifdef DEBUG_1
      if (qc.name == name) {
         QoreString lh, rh;
         hash.toString(lh);
         qc.hash.toString(rh);
         printd(0, "qore_class_private::getClassIntern() this: %p '%s' != '%s' scl: %p (hash: %s qc.hash: %s)\n", this, name.c_str(), qc.name.c_str(), scl, lh.getBuffer(), rh.getBuffer());
      }
#endif
      
      return scl ? scl->getClass(qc, priv) : 0;
   }

   DLLLOCAL const QoreClass* parseGetClass(const qore_class_private& qc, bool& cpriv) const;

   DLLLOCAL const QoreMethod* parseResolveSelfMethod(const char* nme);
   DLLLOCAL const QoreMethod* parseResolveSelfMethod(NamedScope* nme);

   DLLLOCAL int addBaseClassesToSubclass(QoreClass* sc, bool is_virtual);

   DLLLOCAL void setPublic();

   DLLLOCAL void parseSetBaseClassList(BCList *bcl) {
      assert(!scl);
      if (bcl) {
         scl = bcl;
         if (!has_new_user_changes)
            has_new_user_changes = true;
      }
   }

   // static methods
   //DLLLOCAL static

   DLLLOCAL static int parseCheckMemberAccess(const QoreClass& qc, const QoreProgramLocation& loc, const char* mem, const QoreTypeInfo*& memberTypeInfo, int pflag) {
      return qc.priv->parseCheckMemberAccess(loc, mem, memberTypeInfo, pflag);
   }

   DLLLOCAL static bool hasCallableMethod(const QoreClass& qc, const char* m) {
      return qc.priv->hasCallableMethod(m, QCCM_NORMAL | QCCM_STATIC);
   }

   DLLLOCAL static bool hasCallableNormalMethod(const QoreClass& qc, const char* m) {
      return qc.priv->hasCallableMethod(m, QCCM_NORMAL);
   }

   DLLLOCAL static bool hasCallableStaticMethod(const QoreClass& qc, const char* m) {
      return qc.priv->hasCallableMethod(m, QCCM_STATIC);
   }

   DLLLOCAL static const qore_class_private* isPublicOrPrivateMember(const QoreClass& qc, const char* mem, bool &priv) {
      return qc.priv->isPublicOrPrivateMember(mem, priv);
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

   DLLLOCAL static void parseRollback(QoreClass& qc) {
      qc.priv->parseRollback();
   }

   DLLLOCAL static void resolveCopy(QoreClass& qc) {
      qc.priv->resolveCopy();
   }

   DLLLOCAL static int addUserMethod(QoreClass& qc, const char* mname, MethodVariantBase *f, bool n_static) {
      return qc.priv->addUserMethod(mname, f, n_static);
   }

   DLLLOCAL static void initialize(QoreClass& qc) {
      qc.priv->initialize();
   }

   DLLLOCAL static void parseSetBaseClassList(QoreClass& qc, BCList *bcl) {
      qc.priv->parseSetBaseClassList(bcl);
   }

   DLLLOCAL static BCList* getBaseClassList(const QoreClass& qc) {
      return qc.priv->scl;
   }

   DLLLOCAL static void parseAddPrivateStaticVar(QoreClass* qc, char* dname, QoreVarInfo* VarInfo) {
      qc->priv->parseAddPrivateStaticVar(dname, VarInfo);
   }

   DLLLOCAL static void parseAddPublicStaticVar(QoreClass* qc, char* dname, QoreVarInfo* VarInfo) {
      qc->priv->parseAddPublicStaticVar(dname, VarInfo);
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
   DLLLOCAL static QoreVarInfo* parseFindLocalStaticVar(const QoreClass* qc, const char* vname, const QoreTypeInfo*& typeInfo) {
      QoreVarInfo* vi = qc->priv->parseFindLocalStaticVar(vname);
      if (vi)
         typeInfo = vi->getTypeInfo();
      return vi;
   }

   // searches this class and all superclasses, if check = false, then assumes parsing from within the class (getParseClass() == this class)
   DLLLOCAL static AbstractQoreNode* parseFindConstantValue(QoreClass* qc, const char* cname, const QoreTypeInfo*& typeInfo, bool check = false) {
      return qc->priv->parseFindConstantValue(cname, typeInfo, check);
   }

   // searches this class and all superclasses, if check = false, then assumes parsing from within the class (getParseClass() == this class)
   DLLLOCAL static QoreVarInfo* parseFindStaticVar(const QoreClass* qc, const char* vname, const QoreClass*& nqc, const QoreTypeInfo*& typeInfo, bool check = false) {
      QoreVarInfo* vi = qc->priv->parseFindStaticVar(vname, nqc, check);
      if (vi)
         typeInfo = vi->getTypeInfo();
      return vi;
   }

   DLLLOCAL static const QoreMethod* parseResolveSelfMethod(const QoreClass& qc, const char* nme) {
      return qc.priv->parseResolveSelfMethod(nme);
   }

   DLLLOCAL static const QoreMethod* parseResolveSelfMethod(const QoreClass& qc, NamedScope* nme) {
      return qc.priv->parseResolveSelfMethod(nme);
   }

   DLLLOCAL static int parseCheckInternalMemberAccess(const QoreClass* qc, const char* mem, const QoreTypeInfo*& memberTypeInfo, const QoreProgramLocation& loc) {
      return qc->priv->parseCheckInternalMemberAccess(mem, memberTypeInfo, loc);
   }

   DLLLOCAL static int parseResolveInternalMemberAccess(const QoreClass* qc, const char* mem, const QoreTypeInfo*& memberTypeInfo) {
      return qc->priv->parseResolveInternalMemberAccess(mem, memberTypeInfo);
   }

   DLLLOCAL static const QoreMethod* parseFindSelfMethod(QoreClass* qc, const char* mname) {
      qc->priv->initialize();
      const QoreMethod* m = qc->priv->parseResolveSelfMethodIntern(mname);
      if (!m)
         return 0;

      // make sure we're not calling a method that cannot be called directly
      if (!m->isStatic() && (!strcmp(mname, "constructor") || !strcmp(mname, "destructor") || !strcmp(mname, "copy")))
         return 0;

      return m;
   }

   DLLLOCAL static void parseAddPrivateMember(QoreClass& qc, char *nme, QoreMemberInfo *mInfo) {
      qc.priv->parseAddPrivateMember(nme, mInfo);
   }

   DLLLOCAL static void parseAddPublicMember(QoreClass& qc, char *nme, QoreMemberInfo *mInfo) {
      qc.priv->parseAddPublicMember(nme, mInfo);
   }

   DLLLOCAL static const QoreMethod* parseFindAnyMethodIntern(const QoreClass* qc, const char* mname) {
      return qc->priv->parseFindAnyMethodIntern(mname);
   }

   DLLLOCAL static AbstractQoreNode* evalPseudoMethod(const QoreClass* qc, const AbstractQoreNode* n, const char* name, const QoreListNode* args, ExceptionSink* xsink) {
      return qc->priv->evalPseudoMethod(n, name, args, xsink);
   }

   DLLLOCAL static int64 bigIntEvalPseudoMethod(const QoreClass* qc, const AbstractQoreNode* n, const char* name, const QoreListNode* args, ExceptionSink* xsink) {
      return qc->priv->bigIntEvalPseudoMethod(n, name, args, xsink);
   }

   DLLLOCAL static int intEvalPseudoMethod(const QoreClass* qc, const AbstractQoreNode* n, const char* name, const QoreListNode* args, ExceptionSink* xsink) {
      return qc->priv->intEvalPseudoMethod(n, name, args, xsink);
   }

   DLLLOCAL static bool boolEvalPseudoMethod(const QoreClass* qc, const AbstractQoreNode* n, const char* name, const QoreListNode* args, ExceptionSink* xsink) {
      return qc->priv->boolEvalPseudoMethod(n, name, args, xsink);
   }

   DLLLOCAL static double floatEvalPseudoMethod(const QoreClass* qc, const AbstractQoreNode* n, const char* name, const QoreListNode* args, ExceptionSink* xsink) {
      return qc->priv->floatEvalPseudoMethod(n, name, args, xsink);
   }

   DLLLOCAL static AbstractQoreNode* evalPseudoMethod(const QoreClass* qc, const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) {
      return qc->priv->evalPseudoMethod(m, variant, n, args, xsink);
   }

   DLLLOCAL static int64 bigIntEvalPseudoMethod(const QoreClass* qc, const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) {
      return qc->priv->bigIntEvalPseudoMethod(m, variant, n, args, xsink);
   }

   DLLLOCAL static int intEvalPseudoMethod(const QoreClass* qc, const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) {
      return qc->priv->intEvalPseudoMethod(m, variant, n, args, xsink);
   }

   DLLLOCAL static bool boolEvalPseudoMethod(const QoreClass* qc, const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) {
      return qc->priv->boolEvalPseudoMethod(m, variant, n, args, xsink);
   }

   DLLLOCAL static double floatEvalPseudoMethod(const QoreClass* qc, const QoreMethod* m, const AbstractQoreFunctionVariant* variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) {
      return qc->priv->floatEvalPseudoMethod(m, variant, n, args, xsink);
   }

   DLLLOCAL static void setNamespace(QoreClass* qc, qore_ns_private* n) {
      qc->priv->setNamespace(n);
   }

   DLLLOCAL static bool parseCheckPrivateClassAccess(const QoreClass& qc) {
      return qc.priv->parseCheckPrivateClassAccess();
   }

   DLLLOCAL static bool runtimeCheckPrivateClassAccess(const QoreClass& qc) {
      return qc.priv->runtimeCheckPrivateClassAccess();
   }

   DLLLOCAL static qore_type_result_e parseCheckCompatibleClass(const QoreClass& qc, const QoreClass& oc) {
      if (!&oc)
         return QTI_NOT_EQUAL;
      return qc.priv->parseCheckCompatibleClass(*oc.priv);
   }

   DLLLOCAL static qore_type_result_e runtimeCheckCompatibleClass(const QoreClass& qc, const QoreClass& oc) {
      return qc.priv->runtimeCheckCompatibleClass(*oc.priv);
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

class qore_method_private {
public:
   const QoreClass* parent_class;
   MethodFunctionBase *func;
   bool static_flag, all_user;

   DLLLOCAL qore_method_private(const QoreClass* n_parent_class, MethodFunctionBase *n_func, bool n_static) : parent_class(n_parent_class), func(n_func), static_flag(n_static), all_user(true) {
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

   DLLLOCAL int addUserVariant(MethodVariantBase *variant) {
      return func->parseAddUserMethodVariant(variant);
   }

   DLLLOCAL void addBuiltinVariant(MethodVariantBase *variant) {
      setBuiltin();
      func->addBuiltinMethodVariant(variant);
   }

   DLLLOCAL MethodFunctionBase *getFunction() const {
      return const_cast<MethodFunctionBase *>(func);
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

   DLLLOCAL const QoreTypeInfo *getUniqueReturnTypeInfo() const {
      return func->getUniqueReturnTypeInfo();
   }

   DLLLOCAL void evalConstructor(const AbstractQoreFunctionVariant *variant, QoreObject* self, const QoreListNode* args, BCEAList *bceal, ExceptionSink* xsink) {
      CONMF(func)->evalConstructor(variant, *parent_class, self, args, parent_class->priv->scl, bceal, xsink);
   }

   DLLLOCAL void evalCopy(QoreObject* self, QoreObject* old, ExceptionSink* xsink) const {
      COPYMF(func)->evalCopy(*parent_class, self, old, parent_class->priv->scl, xsink);
   }

   DLLLOCAL bool evalDeleteBlocker(QoreObject* self) const {
      // can only be builtin
      return self->evalDeleteBlocker(parent_class->priv->methodID, reinterpret_cast<BuiltinDeleteBlocker *>(func));
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

   DLLLOCAL AbstractQoreNode* eval(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) const {
      if (!static_flag) {
         assert(self);
         return NMETHF(func)->evalMethod(0, self, args, xsink);
      }
      return SMETHF(func)->evalMethod(0, args, xsink);
   }

   DLLLOCAL int64 bigIntEval(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) const {
      if (!static_flag) {
         assert(self);
         return NMETHF(func)->bigIntEvalMethod(0, self, args, xsink);
      }
      return SMETHF(func)->bigIntEvalMethod(0, args, xsink);
   }

   DLLLOCAL int intEval(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) const {
      if (!static_flag) {
         assert(self);
         return NMETHF(func)->intEvalMethod(0, self, args, xsink);
      }
      return SMETHF(func)->intEvalMethod(0, args, xsink);
   }

   DLLLOCAL bool boolEval(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) const {
      if (!static_flag) {
         assert(self);
         return NMETHF(func)->boolEvalMethod(0, self, args, xsink);
      }
      return SMETHF(func)->boolEvalMethod(0, args, xsink);
   }

   DLLLOCAL double floatEval(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) const {
      if (!static_flag) {
         assert(self);
         return NMETHF(func)->floatEvalMethod(0, self, args, xsink);
      }
      return SMETHF(func)->floatEvalMethod(0, args, xsink);
   }

   DLLLOCAL AbstractQoreNode* evalPseudoMethod(const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
      QORE_TRACE("qore_method_private::evalPseudoMethod()");

      assert(!static_flag);

      AbstractQoreNode* rv = NMETHF(func)->evalPseudoMethod(variant, n, args, xsink);
      printd(5, "qore_method_private::evalPseudoMethod() %s::%s() returning %p (type=%s, refs=%d)\n", parent_class->getName(), getName(), rv, rv ? rv->getTypeName() : "(null)", rv ? rv->reference_count() : 0);
      return rv;
   }

   DLLLOCAL int64 bigIntEvalPseudoMethod(const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
      QORE_TRACE("qore_method_private::bigIntEvalPseudoMethod()");

      assert(!static_flag);

      int64 rv = NMETHF(func)->bigIntEvalPseudoMethod(variant, n, args, xsink);
      printd(5, "qore_method_private::bigIntEvalPseudoMethod() %s::%s() returning " QLLD "\n", parent_class->getName(), getName(), rv);
      return rv;
   }

   DLLLOCAL int intEvalPseudoMethod(const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
      QORE_TRACE("qore_method_private::intEvalPseudoMethod()");

      assert(!static_flag);

      int rv = NMETHF(func)->intEvalPseudoMethod(variant, n, args, xsink);
      printd(5, "qore_method_private::intEvalPseudoMethod() %s::%s() returning %d\n", parent_class->getName(), getName(), rv);
      return rv;
   }

   DLLLOCAL bool boolEvalPseudoMethod(const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
      QORE_TRACE("qore_method_private::boolEvalPseudoMethod()");

      assert(!static_flag);

      bool rv = NMETHF(func)->boolEvalPseudoMethod(variant, n, args, xsink);
      printd(5, "qore_method_private::boolEvalPseudoMethod() %s::%s() returning %d\n", parent_class->getName(), getName(), rv);
      return rv;
   }

   DLLLOCAL double floatEvalPseudoMethod(const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) const {
      QORE_TRACE("qore_method_private::doubleEvalPseudoMethod()");

      assert(!static_flag);

      double rv = NMETHF(func)->floatEvalPseudoMethod(variant, n, args, xsink);
      printd(5, "qore_method_private::doubleEvalPseudoMethod() %s::%s() returning %d\n", parent_class->getName(), getName(), rv);
      return rv;
   }

   DLLLOCAL static AbstractQoreNode* evalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) {
      return m->priv->evalPseudoMethod(variant, n, args, xsink);
   }

   DLLLOCAL static int64 bigIntEvalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) {
      return m->priv->bigIntEvalPseudoMethod(variant, n, args, xsink);
   }

   DLLLOCAL static int intEvalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) {
      return m->priv->intEvalPseudoMethod(variant, n, args, xsink);
   }

   DLLLOCAL static bool boolEvalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) {
      return m->priv->boolEvalPseudoMethod(variant, n, args, xsink);
   }

   DLLLOCAL static double floatEvalPseudoMethod(const QoreMethod* m, const AbstractQoreFunctionVariant *variant, const AbstractQoreNode* n, const QoreListNode* args, ExceptionSink* xsink) {
      return m->priv->floatEvalPseudoMethod(variant, n, args, xsink);
   }

   DLLLOCAL static AbstractQoreNode* eval(const QoreMethod& m, QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) {
      return m.priv->eval(self, args, xsink);
   }

   DLLLOCAL static int64 bigIntEval(const QoreMethod& m, QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) {
      return m.priv->bigIntEval(self, args, xsink);
   }

   DLLLOCAL static int intEval(const QoreMethod& m, QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) {
      return m.priv->intEval(self, args, xsink);
   }

   DLLLOCAL static bool boolEval(const QoreMethod& m, QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) {
      return m.priv->boolEval(self, args, xsink);
   }

   DLLLOCAL static double floatEval(const QoreMethod& m, QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) {
      return m.priv->floatEval(self, args, xsink);
   }
};

#endif
