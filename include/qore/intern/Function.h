/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  Function.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

#ifndef _QORE_FUNCTION_H

#define _QORE_FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

// these data structures are all private to the library

class LocalVar;
class VarRefNode;
class BCAList;
class QoreOperatorNode;
class BarewordNode;
class QoreFunction;
class qore_class_private;
class qore_ns_private;

typedef std::vector<QoreParseTypeInfo*> ptype_vec_t;
typedef std::vector<LocalVar*> lvar_vec_t;

class AbstractFunctionSignature {
protected:
   unsigned short num_param_types,    // number of parameters that have type information
      min_param_types;                // minimum number of parameters with type info (without default args)

   const QoreTypeInfo* returnTypeInfo;
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   name_vec_t names;

   // parameter signature string
   std::string str;

public:
   DLLLOCAL AbstractFunctionSignature(const QoreTypeInfo* n_returnTypeInfo = 0) : num_param_types(0), min_param_types(0), returnTypeInfo(n_returnTypeInfo) {
   }
   DLLLOCAL AbstractFunctionSignature(const QoreTypeInfo* n_returnTypeInfo, const type_vec_t& n_typeList, const arg_vec_t& n_defaultArgList, const name_vec_t& n_names) : num_param_types(0), min_param_types(0), returnTypeInfo(n_returnTypeInfo), typeList(n_typeList), defaultArgList(n_defaultArgList), names(n_names) {
   }
   DLLLOCAL virtual ~AbstractFunctionSignature() {
      // delete all default argument expressions
      for (arg_vec_t::iterator i = defaultArgList.begin(), e = defaultArgList.end(); i != e; ++i)
         if (*i)
            (*i)->deref(0);
   }
   // called at parse time to include optional type resolution
   DLLLOCAL virtual const QoreTypeInfo* parseGetReturnTypeInfo() const = 0;

   DLLLOCAL virtual const QoreParseTypeInfo* getParseParamTypeInfo(unsigned num) const = 0;

   DLLLOCAL const QoreTypeInfo* getReturnTypeInfo() const {
      return returnTypeInfo;
   }

   DLLLOCAL const arg_vec_t& getDefaultArgList() const {
      return defaultArgList;
   }

   DLLLOCAL const type_vec_t& getTypeList() const {
      return typeList;
   }

   DLLLOCAL AbstractQoreNode* evalDefaultArg(unsigned i, ExceptionSink* xsink) const {
      assert(i < defaultArgList.size());
      return defaultArgList[i] ? defaultArgList[i]->eval(xsink) : 0;
   }

   DLLLOCAL const char* getSignatureText() const {
      return str.c_str();
   }

   DLLLOCAL virtual void addAbstractParameterSignature(std::string& str) const {
      for (unsigned i = 0; i < typeList.size(); ++i) {
         str.append(typeList[i]->getName());
         if (i != typeList.size() - 1)
            str.append(",");
      }
   }

   DLLLOCAL unsigned numParams() const {
      return (unsigned)typeList.size();
   }

   // number of params with type information
   DLLLOCAL unsigned getParamTypes() const {
      return num_param_types;
   }

   DLLLOCAL unsigned getMinParamTypes() const {
      return min_param_types;
   }

   DLLLOCAL const QoreTypeInfo* getParamTypeInfo(unsigned num) const {
      return num >= typeList.size() ? 0 : typeList[num];
   }

   DLLLOCAL bool hasDefaultArg(unsigned i) const {
      return i >= defaultArgList.size() || !defaultArgList[i] ? false : true;
   }

   // adds a description of the given default argument to the signature string
   DLLLOCAL void addDefaultArgument(const AbstractQoreNode* arg);

   DLLLOCAL const char* getName(unsigned i) const {
      return i < names.size() ? names[i].c_str() : 0;
   }

   DLLLOCAL bool operator==(const AbstractFunctionSignature& sig) const;
};

// used to store return type info during parsing for user code
class RetTypeInfo {
   QoreParseTypeInfo* parseTypeInfo;
   const QoreTypeInfo* typeInfo;

public:
   DLLLOCAL RetTypeInfo(QoreParseTypeInfo* n_parseTypeInfo, const QoreTypeInfo* n_typeInfo) : parseTypeInfo(n_parseTypeInfo), typeInfo(n_typeInfo) {
   }
   DLLLOCAL ~RetTypeInfo() {
      delete parseTypeInfo;
   }
   DLLLOCAL const QoreTypeInfo* getTypeInfo() const {
      return typeInfo;
   }
   DLLLOCAL QoreParseTypeInfo* takeParseTypeInfo() {
      QoreParseTypeInfo* rv = parseTypeInfo;
      parseTypeInfo = 0;
      return rv;
   }
};

class UserSignature : public AbstractFunctionSignature {
protected:
   ptype_vec_t parseTypeList;
   QoreParseTypeInfo* parseReturnTypeInfo;

   QoreProgramLocation loc;

   DLLLOCAL void pushParam(BarewordNode* b, bool needs_types, bool bare_refs);
   DLLLOCAL void pushParam(QoreOperatorNode* t, bool needs_types);
   DLLLOCAL void pushParam(VarRefNode* v, AbstractQoreNode* defArg, bool needs_types);

   DLLLOCAL static void param_error() {
      parse_error("parameter list contains non-variable reference expressions");
   }

public:
   lvar_vec_t lv;
   LocalVar* argvid;
   LocalVar* selfid;
   bool resolved;

   DLLLOCAL UserSignature(int n_first_line, int n_last_line, AbstractQoreNode* params, RetTypeInfo* retTypeInfo, int64 po);

   DLLLOCAL virtual ~UserSignature() {
      for (ptype_vec_t::iterator i = parseTypeList.begin(), e = parseTypeList.end(); i != e; ++i)
	 delete* i;
      delete parseReturnTypeInfo;
   }

   DLLLOCAL void setFirstParamType(const QoreTypeInfo* typeInfo) {
      assert(!typeList.empty());
      typeList[0] = typeInfo;
   }

   DLLLOCAL void setSelfId(LocalVar* n_selfid) {
      assert(!selfid);
      selfid = n_selfid;
   }

   DLLLOCAL virtual const QoreParseTypeInfo* getParseParamTypeInfo(unsigned num) const {
      return num < parseTypeList.size() ? parseTypeList[num] : 0;
   }

   // resolves all parse types to the final types
   DLLLOCAL void resolve();

   DLLLOCAL virtual void addAbstractParameterSignature(std::string& str) const {
      if (resolved) {
         AbstractFunctionSignature::addAbstractParameterSignature(str);
         return;
      }

      for (unsigned i = 0; i < parseTypeList.size(); ++i) {
         if (!parseTypeList[i] && typeList.size() > i && typeList[i])
            str.append(typeList[i]->getName());
         else
            str.append(parseTypeList[i]->getName());
         if (i != parseTypeList.size() - 1)
            str.append(",");
      }
   }

   // called at parse time to ensure types are resolved
   DLLLOCAL virtual const QoreTypeInfo* parseGetReturnTypeInfo() const {
      const_cast<UserSignature*>(this)->resolve();
      return returnTypeInfo;
   }

   DLLLOCAL const QoreProgramLocation& getParseLocation() const {
      return loc;
   }

   DLLLOCAL bool hasReturnTypeInfo() const {
      return parseReturnTypeInfo || returnTypeInfo;
   }

   DLLLOCAL void parseInitPushLocalVars(const QoreTypeInfo* classTypeInfo);

   // returns the $argv reference count
   DLLLOCAL void parseInitPopLocalVars();
};

class AbstractQoreFunctionVariant;

class CodeEvaluationHelper {
protected:
   qore_call_t ct;
   const char* name;
   ExceptionSink* xsink;
   const char* class_name;
   QoreProgramLocation loc;
   QoreListNodeEvalOptionalRefHolder tmp;
   const QoreTypeInfo* returnTypeInfo; // saved return type info
   QoreProgram* pgm; // program used when evaluated (to find stacks for references)

public:
   // saves current program location in case there's an exception
   DLLLOCAL CodeEvaluationHelper(ExceptionSink* n_xsink, const QoreFunction* func, const AbstractQoreFunctionVariant*& variant, const char* n_name, const QoreListNode* args = 0, const char* n_class_name = 0, qore_call_t n_ct = CT_UNUSED, bool is_copy = false);

   DLLLOCAL ~CodeEvaluationHelper();

   DLLLOCAL void setReturnTypeInfo(const QoreTypeInfo* n_returnTypeInfo) {
      returnTypeInfo = saveReturnTypeInfo(n_returnTypeInfo);
   }
   // once this is set, exception information will be raised in the destructor if an exception has been raised
   DLLLOCAL void setCallType(qore_call_t n_ct) {
      ct = n_ct;
   }
   DLLLOCAL int processDefaultArgs(const QoreFunction* func, const AbstractQoreFunctionVariant* variant, bool check_args, bool is_copy = false);

   DLLLOCAL void setArgs(QoreListNode* n_args) {
      assert(!*tmp);
      tmp.assign(true, n_args);
   }

   DLLLOCAL const QoreListNode* getArgs() const {
      return *tmp;
   }
   // returns the QoreProgram object where the call originated
   DLLLOCAL QoreProgram* getSourceProgram() const {
      return pgm;
   }
   DLLLOCAL void restorePosition() const {
      update_runtime_location(loc);
   }
};

class UserVariantBase;

// describes the details of the function variant
class AbstractQoreFunctionVariant : protected QoreReferenceCounter {
private:
   // not implemented
   DLLLOCAL AbstractQoreFunctionVariant(const AbstractQoreFunctionVariant& old);
   DLLLOCAL AbstractQoreFunctionVariant& operator=(AbstractQoreFunctionVariant& orig);

protected:
   // code flags
   int64 flags;
   bool is_user;

   DLLLOCAL virtual ~AbstractQoreFunctionVariant() {}

public:
   DLLLOCAL AbstractQoreFunctionVariant(int64 n_flags, bool n_is_user = false) : flags(n_flags), is_user(n_is_user) {}

   DLLLOCAL virtual AbstractFunctionSignature* getSignature() const = 0;
   DLLLOCAL virtual const QoreTypeInfo* parseGetReturnTypeInfo() const = 0;

   DLLLOCAL const QoreTypeInfo* getReturnTypeInfo() const {
      return getSignature()->getReturnTypeInfo();
   }

   DLLLOCAL unsigned numParams() const {
      AbstractFunctionSignature* sig = getSignature();
      return sig ? sig->numParams() : 0;
   }

   DLLLOCAL qore_call_t getCallType() const {
      return is_user ? CT_USER : CT_BUILTIN;
   }

   DLLLOCAL int64 getFlags() const {
      return flags;
   }

   DLLLOCAL virtual int64 getFunctionality() const = 0;

   // set flag to recheck params against committed variants in stage 2 parsing after type resolution (only for user variants); should never be called with builtin variants
   DLLLOCAL virtual void setRecheck() {
      assert(false);
   }

   DLLLOCAL void parseResolveUserSignature();

   DLLLOCAL virtual UserVariantBase* getUserVariantBase() {
      return 0;
   }

   DLLLOCAL const UserVariantBase* getUserVariantBase() const {
      // avoid the virtual function call if possible
      return is_user ? const_cast<AbstractQoreFunctionVariant*>(this)->getUserVariantBase() : 0;
   }

   DLLLOCAL virtual QoreValue evalFunction(const char* name, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      assert(false);
      return QoreValue();
   }

   DLLLOCAL virtual const QoreClass* getClass() const {
      return 0;
   }

   DLLLOCAL const char* className() const {
      const QoreClass* qc = getClass();
      return qc ? qc->getName() : 0;
   }

   DLLLOCAL bool isSignatureIdentical(const AbstractFunctionSignature& sig) const {
      //printd(5, "AbstractQoreFunctionVariant::isSignatureIdentical() this: %p '%s' == '%s': %d\n", this, getSignature()->getSignatureText(), sig.getSignatureText(), *(getSignature()) == sig);
      return *(getSignature()) == sig;
   }

   DLLLOCAL AbstractQoreFunctionVariant* ref() {
      ROreference();
      return this;
   }

   DLLLOCAL void deref() {
      if (ROdereference()) {
         delete this;
      }
   }

   DLLLOCAL bool isUser() const {
      return is_user;
   }

   DLLLOCAL bool hasBody() const;

   DLLLOCAL virtual bool isModulePublic() const {
      return false;
   }

   // the default implementation of this function does nothing
   DLLLOCAL virtual void parseInit(QoreFunction* f) {
   }
};

class VRMutex;
class UserVariantExecHelper;

// base implementation shared between all user variants
class UserVariantBase {
   friend class UserVariantExecHelper;

protected:
   UserSignature signature;
   StatementBlock* statements;
   // for "synchronized" functions
   VRMutex* gate;

public:
   QoreProgram* pgm;

protected:
   // flag to recheck params against committed after type resolution
   bool recheck;
   // flag to tell if variant has been initialized or not (still in pending list)
   bool init;

   DLLLOCAL QoreValue evalIntern(ReferenceHolder<QoreListNode>& argv, QoreObject* self, ExceptionSink* xsink) const;
   DLLLOCAL QoreValue eval(const char* name, CodeEvaluationHelper* ceh, QoreObject* self, ExceptionSink* xsink, const qore_class_private* qc = 0) const;
   DLLLOCAL int setupCall(CodeEvaluationHelper* ceh, ReferenceHolder<QoreListNode>& argv, ExceptionSink* xsink) const;

public:
   DLLLOCAL UserVariantBase(StatementBlock* b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced);
   DLLLOCAL virtual ~UserVariantBase();
   DLLLOCAL UserSignature* getUserSignature() const {
      return const_cast<UserSignature*>(&signature);
   }

   DLLLOCAL void setInit() {
      init = true;
   }

   DLLLOCAL bool getInit() const {
      return init;
   }

   DLLLOCAL bool hasBody() const {
      return (bool)statements;
   }

   DLLLOCAL void parseInitPushLocalVars(const QoreTypeInfo* classTypeInfo);

   DLLLOCAL void parseInitPopLocalVars();
};

// the following defines the pure virtual functions that are common to all user variants
#define COMMON_USER_VARIANT_FUNCTIONS DLLLOCAL virtual int64 getFunctionality() const { return QDOM_DEFAULT; } \
   using AbstractQoreFunctionVariant::getUserVariantBase; \
   DLLLOCAL virtual UserVariantBase* getUserVariantBase() { return static_cast<UserVariantBase*>(this); } \
   DLLLOCAL virtual AbstractFunctionSignature* getSignature() const { return const_cast<UserSignature*>(&signature); } \
   DLLLOCAL virtual const QoreTypeInfo* parseGetReturnTypeInfo() const { return signature.parseGetReturnTypeInfo(); } \
   DLLLOCAL virtual void setRecheck() { recheck = true; }

// this class ensures that instantiated variables in user code are uninstantiated, even if an exception occurs
class UserVariantExecHelper {
protected:
   const UserVariantBase* uvb;
   ReferenceHolder<QoreListNode> argv;
   ExceptionSink* xsink;

public:
   DLLLOCAL UserVariantExecHelper(const UserVariantBase* n_uvb, CodeEvaluationHelper* ceh, ExceptionSink* n_xsink) : uvb(n_uvb), argv(n_xsink), xsink(n_xsink) {
      if (uvb->setupCall(ceh, argv, xsink))
	 uvb = 0;
   }
   DLLLOCAL ~UserVariantExecHelper();
   DLLLOCAL operator bool() const {
      return uvb;
   }
   DLLLOCAL ReferenceHolder<QoreListNode>& getArgv() {
      return argv;
   }
};

class UserFunctionVariant : public AbstractQoreFunctionVariant, public UserVariantBase {
protected:
   bool mod_pub; // is public in module

   DLLLOCAL virtual ~UserFunctionVariant() {
   }

public:
   DLLLOCAL UserFunctionVariant(StatementBlock* b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced, int64 n_flags = QC_NO_FLAGS) :
      AbstractQoreFunctionVariant(n_flags, true), UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, rv, synced), mod_pub(false) {
   }

   // the following defines the virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL virtual QoreValue evalFunction(const char* name, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      return eval(name, &ceh, 0, xsink);
   }

   DLLLOCAL virtual void parseInit(QoreFunction* f);

   DLLLOCAL virtual bool isModulePublic() const {
      return mod_pub;
   }

   DLLLOCAL void setModulePublic() {
      assert(!mod_pub);
      mod_pub = true;
   }
};

#define UFV(f) (reinterpret_cast<UserFunctionVariant*>(f))
#define UFV_const(f) (reinterpret_cast<const UserFunctionVariant*>(f))

// type for lists of function variants
// this type will be read at runtime and could be appended to simultaneously at parse time (under a lock)
typedef safe_dslist<AbstractQoreFunctionVariant*> vlist_t;

class VList : public vlist_t {
public:
   DLLLOCAL ~VList() {
      del();
   }
   DLLLOCAL void del() {
      // dereference all variants
      for (vlist_t::iterator i = begin(), e = end(); i != e; ++i)
	 (*i)->deref();
      clear();
   }
};

class QoreFunction : protected QoreReferenceCounter {
protected:
   std::string name;
   qore_ns_private* ns;

   // inheritance list type
   typedef std::vector<QoreFunction*> ilist_t;

   // list of function variants
   VList vlist;

   // list of pending user-code function variants
   VList pending_vlist;

   // list of inherited methods for variant matching; the first pointer is always a pointer to "this"
   ilist_t ilist;

   // if true means all variants have the same return value
   bool same_return_type, parse_same_return_type;
   int64 unique_functionality;
   int64 unique_flags;

   // same as above but for variants without QC_RUNTIME_NOOP
   bool nn_same_return_type;
   int64 nn_unique_functionality;
   int64 nn_unique_flags;
   int nn_count;
   bool parse_rt_done;
   bool parse_init_done;
   bool has_user;                   // has at least 1 committed user variant
   bool has_builtin;                // has at least 1 committed builtin variant
   bool has_mod_pub;                // has at least 1 committed user variant with public visibility
   bool inject;

   const QoreTypeInfo* nn_uniqueReturnType;

   DLLLOCAL void parseCheckReturnType() {
      if (parse_rt_done)
         return;

      parse_rt_done = true;

      if (!same_return_type || pending_vlist.empty())
         return;

      for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
         reinterpret_cast<UserSignature*>((*i)->getUserVariantBase()->getUserSignature())->resolve();
         const QoreTypeInfo* rti = (*i)->getReturnTypeInfo();

         if (i == pending_vlist.begin()) {
            if (!vlist.empty()) {
               if (!rti->isOutputIdentical(first()->getReturnTypeInfo())) {
                  parse_same_return_type = false;
                  break;
               }
            }
            continue;
         }

         if (!rti->isOutputIdentical(pending_first()->getReturnTypeInfo())) {
            parse_same_return_type = false;
            break;
         }
      }
   }

   // convenience function for returning the first variant in the list
   DLLLOCAL const AbstractQoreFunctionVariant* first() const {
      assert(!vlist.empty());
      return *(vlist.begin());
   }

   // convenience function for returning the first variant in the list
   DLLLOCAL AbstractQoreFunctionVariant* first() {
      assert(!vlist.empty());
      return *(vlist.begin());
   }

   // convenience function for returning the first variant in the list
   DLLLOCAL const AbstractQoreFunctionVariant* pending_first() const {
      assert(!pending_vlist.empty());
      return *(pending_vlist.begin());
   }

   // returns QTI_NOT_EQUAL, QTI_AMBIGUOUS, or QTI_IDENT
   DLLLOCAL static int parseCompareResolvedSignature(const VList& vlist, const AbstractFunctionSignature* sig, const AbstractFunctionSignature*& vs);

   // returns 0 for OK (not a duplicate), -1 for error (duplicate) - parse exceptions are raised if a duplicate is found
   DLLLOCAL int parseCheckDuplicateSignature(AbstractQoreFunctionVariant* variant);

   // FIXME: does not check unparsed types properly
   DLLLOCAL void addVariant(AbstractQoreFunctionVariant* variant) {
      const QoreTypeInfo* rti = variant->getReturnTypeInfo();
      if (same_return_type && !vlist.empty() && !rti->isOutputIdentical(first()->getReturnTypeInfo()))
	 same_return_type = false;

      int64 vf = variant->getFunctionality();
      int64 vflags = variant->getFlags();

      bool rtn = (bool)(vflags & QC_RUNTIME_NOOP);

      if (vlist.empty()) {
	 unique_functionality = vf;
         unique_flags = vflags;
      }
      else {
         unique_functionality &= vf;
         unique_flags &= vflags;
      }

      if (!rtn) {
         if (!nn_count) {
            nn_unique_functionality = vf;
            nn_unique_flags = vflags;
            nn_uniqueReturnType = rti;
            ++nn_count;
         }
         else {
            nn_unique_functionality &= vf;
            nn_unique_flags &= vflags;
            if (nn_uniqueReturnType && !rti->isOutputIdentical(nn_uniqueReturnType))
               nn_uniqueReturnType = 0;
            ++nn_count;
         }
      }

      vlist.push_back(variant);
   }

   DLLLOCAL virtual ~QoreFunction() {
      //printd(5, "QoreFunction::~QoreFunction() this: %p %s\n", this, name.c_str());
   }

public:
   DLLLOCAL QoreFunction(const char* n_name, qore_ns_private* n = 0)
      : name(n_name), ns(n), same_return_type(true), parse_same_return_type(true),
        unique_functionality(QDOM_DEFAULT), unique_flags(QC_NO_FLAGS),
        nn_same_return_type(true), nn_unique_functionality(QDOM_DEFAULT),
        nn_unique_flags(QC_NO_FLAGS), nn_count(0), parse_rt_done(true),
        parse_init_done(true), has_user(false), has_builtin(false), has_mod_pub(false), inject(false),
        nn_uniqueReturnType(0) {
      ilist.push_back(this);
      //printd(5, "QoreFunction::QoreFunction() this: %p %s\n", this, name.c_str());
   }

   // copy constructor (used by method functions when copied)
   DLLLOCAL QoreFunction(const QoreFunction& old, int64 po = 0, qore_ns_private* n = 0, bool copy_all = false, bool n_inject = false)
      : name(old.name), ns(n), same_return_type(old.same_return_type),
        parse_same_return_type(true),
        unique_functionality(old.unique_functionality),
        unique_flags(old.unique_flags),
        nn_same_return_type(old.nn_same_return_type),
        nn_unique_functionality(old.nn_unique_functionality),
        nn_unique_flags(old.nn_unique_flags),
        nn_count(old.nn_count),
        parse_rt_done(true), parse_init_done(true),
        has_user(old.has_user), has_builtin(old.has_builtin), has_mod_pub(false), inject(n_inject),
        nn_uniqueReturnType(old.nn_uniqueReturnType) {
      bool no_user = po & PO_NO_INHERIT_USER_FUNC_VARIANTS;
      bool no_builtin = po & PO_NO_SYSTEM_FUNC_VARIANTS;

      // copy variants by reference
      for (vlist_t::const_iterator i = old.vlist.begin(), e = old.vlist.end(); i != e; ++i) {
         if (!copy_all) {
            if ((*i)->isUser()) {
               if (no_user || !(*i)->isModulePublic())
                  continue;
            }
            else
               if (no_builtin)
                  continue;
         }

         vlist.push_back((*i)->ref());
      }

      if (no_user && has_user)
         has_user = false;
      if (no_builtin && has_builtin)
         has_builtin = false;

      // make sure the new variant list is not empty if the parent also wasn't
      assert(old.vlist.empty() || !vlist.empty());

      assert(!old.ilist.empty());
      assert(old.ilist.front() == &old);

      // resolve initial ilist entry to this function
      ilist.push_back(this);

      // the rest of ilist is copied in method base class
      // do not copy pending variants
      //printd(5, "QoreFunction::QoreFunction() this: %p %s\n", this, name.c_str());
   }

#if 0
   // copy constructor when importing public user variants from user modules into Program objects
   DLLLOCAL QoreFunction(bool ignore, const QoreFunction& old, qore_ns_private* nns)
      : name(old.name), ns(nns), same_return_type(old.same_return_type),
        parse_same_return_type(true),
        unique_functionality(old.unique_functionality),
        unique_flags(old.unique_flags),
        nn_same_return_type(old.nn_same_return_type),
        nn_unique_functionality(old.nn_unique_functionality),
        nn_unique_flags(old.nn_unique_flags),
        nn_count(old.nn_count),
        parse_rt_done(true), parse_init_done(true),
        has_user(true), has_builtin(false), has_mod_pub(false /*old.has_mod_pub*/), inject(false),
        nn_uniqueReturnType(old.nn_uniqueReturnType) {
      assert(!ignore);
      assert(old.has_mod_pub);

      // copy variants by reference
      for (vlist_t::const_iterator i = old.vlist.begin(), e = old.vlist.end(); i != e; ++i) {
         if (!(*i)->isModulePublic())
            continue;
         vlist.push_back((*i)->ref());
      }

      // make sure the new variant list is not empty if the parent also wasn't
      assert(old.vlist.empty() || !vlist.empty());

      assert(!old.ilist.empty());
      assert(old.ilist.front() == &old);

      // resolve initial ilist entry to this function
      ilist.push_back(this);

      // the rest of ilist is copied in method base class
      // do not copy pending variants
      //printd(5, "QoreFunction::QoreFunction() this: %p %s\n", this, name.c_str());
   }
#endif

   DLLLOCAL qore_ns_private* getNamespace() const {
      return ns;
   }

   // used when merging namespaces at parse-time
   DLLLOCAL void updateNs(qore_ns_private* nns) {
      ns = nns;
   }

   // returns 0 for OK, -1 for error
   DLLLOCAL int parseCheckDuplicateSignatureCommitted(AbstractFunctionSignature* sig);

   DLLLOCAL const char* getName() const {
      return name.c_str();
   }

   DLLLOCAL virtual const QoreClass* getClass() const {
      return 0;
   }

   DLLLOCAL void ref() {
      ROreference();
   }

   DLLLOCAL void deref() {
      if (ROdereference())
	 delete this;
   }

   DLLLOCAL const char* className() const {
      const QoreClass* qc = getClass();
      return qc ? qc->getName() : 0;
   }

   DLLLOCAL void addAncestor(QoreFunction* ancestor) {
      ilist.push_back(ancestor);
   }

   DLLLOCAL void addNewAncestor(QoreFunction* ancestor) {
      for (ilist_t::iterator i = ilist.begin(), e = ilist.end(); i != e; ++i)
         if (*i == ancestor)
            return;
      ilist.push_back(ancestor);
   }

   // resolves all types in signatures and return types in pending variants; called during the "parseInit" phase
   DLLLOCAL void resolvePendingSignatures();

   DLLLOCAL AbstractFunctionSignature* getUniqueSignature() const {
      return vlist.singular() ? first()->getSignature() : 0;
   }

   DLLLOCAL AbstractFunctionSignature* parseGetUniqueSignature() const;

   DLLLOCAL int64 parseGetUniqueFunctionality() const {
      if (parse_get_parse_options() & PO_REQUIRE_TYPES)
         return nn_unique_functionality;
      return unique_functionality;
   }

   DLLLOCAL int64 parseGetUniqueFlags() const {
      if (parse_get_parse_options() & PO_REQUIRE_TYPES)
         return nn_unique_flags;
      return unique_flags;
   }

   // object takes ownership of variant or deletes it if it can't be added
   DLLLOCAL int addPendingVariant(AbstractQoreFunctionVariant* variant);

   DLLLOCAL void addBuiltinVariant(AbstractQoreFunctionVariant* variant);

   DLLLOCAL void parseInit();
   DLLLOCAL void parseCommit();
   DLLLOCAL void parseRollback();

   DLLLOCAL const QoreTypeInfo* getUniqueReturnTypeInfo() const {
      if (runtime_get_parse_options() & PO_REQUIRE_TYPES)
         return nn_uniqueReturnType;

      return same_return_type && !vlist.empty() ? first()->getReturnTypeInfo() : 0;
   }

   DLLLOCAL const QoreTypeInfo* parseGetUniqueReturnTypeInfo() {
      parseCheckReturnType();

      if (parse_get_parse_options() & PO_REQUIRE_TYPES) {
         if (!nn_same_return_type || !parse_same_return_type)
            return 0;

         return nn_count ? nn_uniqueReturnType : (!pending_vlist.empty() ? pending_first()->getReturnTypeInfo() : 0);
      }

      if (!same_return_type || !parse_same_return_type)
         return 0;

      if (!vlist.empty())
         return first()->getReturnTypeInfo();

      assert(!pending_vlist.empty());
      return pending_first()->getReturnTypeInfo();
   }

   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL virtual QoreValue evalFunction(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, QoreProgram* pgm, ExceptionSink* xsink) const;

   // finds a variant and checks variant capabilities against current program parse options and executes the variant
   DLLLOCAL QoreValue evalDynamic(const QoreListNode* args, ExceptionSink* xsink) const;

   // find variant at parse time, throw parse exception if no variant can be matched
   DLLLOCAL const AbstractQoreFunctionVariant* parseFindVariant(const QoreProgramLocation& loc, const type_vec_t& argTypeInfo);

   // returns true if there are no committed variants in the function
   DLLLOCAL bool committedEmpty() const {
      return vlist.empty();
   }

   // returns true if there are no pending variants in the function
   DLLLOCAL bool pendingEmpty() const {
      return pending_vlist.empty();
   }

   DLLLOCAL bool existsVariant(const type_vec_t& paramTypeInfo) const;

   // find variant at runtime
   // if only_user is set, then no exception is raised if the user variant is not found
   DLLLOCAL const AbstractQoreFunctionVariant* findVariant(const QoreListNode* args, bool only_user, ExceptionSink* xsink) const;

   DLLLOCAL const AbstractQoreFunctionVariant* runtimeFindVariant(const type_vec_t& argTypeList, bool only_user = false) const;

   DLLLOCAL void parseAssimilate(QoreFunction& other) {
      // ensure there are no committed variants
      assert(other.vlist.empty());

      while (!other.pending_vlist.empty()) {
         addPendingVariant(*(other.pending_vlist.begin()));
         other.pending_vlist.pop_front();
      }
   }

   // convenience function for returning the first variant in the list
   DLLLOCAL AbstractQoreFunctionVariant* pending_first() {
      assert(!pending_vlist.empty());
      return *(pending_vlist.begin());
   }

   DLLLOCAL bool hasUser() const {
      return has_user;
   }

   DLLLOCAL bool hasBuiltin() const {
      return has_builtin;
   }

   DLLLOCAL bool hasPublic() const {
      return has_mod_pub;
   }

   DLLLOCAL bool hasUserPublic() const {
      return has_mod_pub && has_user;
   }

   DLLLOCAL bool injected() const {
      return inject;
   }

   DLLLOCAL const std::string& getNameStr() const {
      return name;
   }
};

class MethodVariantBase;
class MethodFunctionBase;
#define METHFB(f) (reinterpret_cast<MethodFunctionBase*>(f))
#define METHFB_const(f) (reinterpret_cast<const MethodFunctionBase*>(f))

class MethodFunctionBase : public QoreFunction {
friend struct AbstractMethod;
protected:
   bool all_private,
      pending_all_private,
      is_static,
      has_final,
      pending_has_final;
   const QoreClass* qc;

   // for concrete variants for local abstract variants inherited from base classes
   VList pending_save;

   // pointer to copy, only valid during copy
   mutable MethodFunctionBase* new_copy;

   DLLLOCAL int checkFinalVariant(const MethodFunctionBase* m, const MethodVariantBase* v) const;

   DLLLOCAL void replaceAbstractVariantIntern(MethodVariantBase* variant);

public:
   DLLLOCAL MethodFunctionBase(const char* nme, const QoreClass* n_qc, bool n_is_static) : QoreFunction(nme), all_private(true), pending_all_private(true), is_static(n_is_static), has_final(false), pending_has_final(false), qc(n_qc), new_copy(0) {
   }

   // copy constructor, only copies committed variants
   DLLLOCAL MethodFunctionBase(const MethodFunctionBase& old, const QoreClass* n_qc)
      : QoreFunction(old, 0, 0, true),
        all_private(old.all_private),
        pending_all_private(true),
        is_static(old.is_static),
        has_final(old.has_final),
        pending_has_final(false),
        qc(n_qc) {
      //printd(5, "MethodFunctionBase() copying old=%p -> new=%p %p %s::%s() %p %s::%s()\n",& old, this, old.qc, old.qc->getName(), old.getName(), qc, qc->getName(), old.getName());

      // set a pointer to the new function
      old.new_copy = this;

      // copy ilist, will be adjusted for new class pointers after all classes have been copied
      ilist.reserve(old.ilist.size());
      ilist_t::const_iterator i = old.ilist.begin(), e = old.ilist.end();
      ++i;
      for (; i != e; ++i)
         ilist.push_back(*i);
   }

   DLLLOCAL void resolveCopy() {
      ilist_t::iterator i = ilist.begin(), e = ilist.end();
      ++i;
      for (; i != e; ++i) {
         MethodFunctionBase* mfb = METHFB(*i);
#ifdef DEBUG
         if (!mfb->new_copy)
            printd(0, "error resolving %p %s::%s() base method %p %s::%s() nas no new method pointer\n", qc, qc->getName(), getName(), mfb->qc, mfb->qc->getName(), getName());
         assert(mfb->new_copy);
         //printd(5, "resolving %p %s::%s() base method %p %s::%s() from %p -> %p\n", qc, qc->getName(), getName(), mfb->qc, mfb->qc->getName(), getName(), mfb, mfb->new_copy);
#endif
        *i = mfb->new_copy;
      }
   }

   DLLLOCAL void parseInit();
   DLLLOCAL void parseCommit();
   DLLLOCAL void parseRollback();

   // returns -1 for error, 0 = OK
   DLLLOCAL int parseAddUserMethodVariant(MethodVariantBase* variant);
   // maintains all_private flag and commits the builtin variant
   DLLLOCAL void addBuiltinMethodVariant(MethodVariantBase* variant);
   // maintains all_private flag and commits user variants
   DLLLOCAL void parseCommitMethod(QoreString& csig, const char* mod);
   DLLLOCAL void parseCommitMethod();
   // processes method signatures while parsing classes for pending variants
   DLLLOCAL void parsePendingSignatures(QoreString& csig, const char* mod) const;
   // processes method signatures for committed variants
   DLLLOCAL void parseCommittedSignatures(QoreString& csig, const char* mod) const;

   // if an identical signature is found to the passed variant, then it is removed from the abstract list
   DLLLOCAL MethodVariantBase* parseHasVariantWithSignature(MethodVariantBase* v) const;

   DLLLOCAL void replaceAbstractVariant(MethodVariantBase* variant);

   DLLLOCAL void parseRollbackMethod();
   DLLLOCAL bool isUniquelyPrivate() const {
      return all_private;
   }
   DLLLOCAL bool parseIsUniquelyPrivate() const {
      return all_private && pending_all_private;
   }

   DLLLOCAL virtual const QoreClass* getClass() const {
      return qc;
   }

   DLLLOCAL const char* getClassName() const {
      return qc->getName();
   }

   DLLLOCAL bool isStatic() const {
      return is_static;
   }

   DLLLOCAL void checkFinal() const;

   // virtual copy constructor
   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const = 0;
};

class UserParamListLocalVarHelper {
protected:
   UserVariantBase* uvb;

public:
   DLLLOCAL UserParamListLocalVarHelper(UserVariantBase* n_uvb, const QoreTypeInfo* classTypeInfo = 0) : uvb(n_uvb) {
      uvb->parseInitPushLocalVars(classTypeInfo);
   }

   DLLLOCAL ~UserParamListLocalVarHelper() {
      uvb->parseInitPopLocalVars();
   }
};

class UserClosureVariant : public UserFunctionVariant {
protected:
public:
   DLLLOCAL UserClosureVariant(StatementBlock* b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced = false, int64 n_flags = QC_NO_FLAGS) : UserFunctionVariant(b, n_sig_first_line, n_sig_last_line, params, rv, synced, n_flags) {
   }

   DLLLOCAL virtual void parseInit(QoreFunction* f);

   DLLLOCAL QoreValue evalClosure(CodeEvaluationHelper& ceh, QoreObject* self, ExceptionSink* xsink) const {
      return eval("<anonymous closure>", &ceh, self, xsink);
   }
};

#define UCLOV(f) (reinterpret_cast<UserClosureVariant*>(f))
#define UCLOV_const(f) (reinterpret_cast<const UserClosureVariant*>(f))

class UserClosureFunction : public QoreFunction {
protected:
   lvar_set_t varlist;  // closure local variable environment
   const QoreTypeInfo* classTypeInfo;

public:
   DLLLOCAL UserClosureFunction(StatementBlock* b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced = false, int64 n_flags = QC_NO_FLAGS) : QoreFunction("<anonymous closure>"), classTypeInfo(0) {
      addPendingVariant(new UserClosureVariant(b, n_sig_first_line, n_sig_last_line, params, rv, synced, n_flags));
   }

   DLLLOCAL bool parseStage1HasReturnTypeInfo() const {
      return reinterpret_cast<const UserClosureVariant*>(pending_first())->getUserSignature()->hasReturnTypeInfo();
   }

   DLLLOCAL QoreValue evalClosure(const QoreClosureBase& closure_base, QoreProgram* pgm, const QoreListNode* args, QoreObject* self, ExceptionSink* xsink) const;

   DLLLOCAL void setClassType(const QoreTypeInfo* cti) {
      classTypeInfo = cti;
   }

   DLLLOCAL const QoreTypeInfo* getClassType() const {
      return classTypeInfo;
   }

   DLLLOCAL lvar_set_t* getVList() {
      return &varlist;
   }
};

#endif // _QORE_FUNCTION_H
