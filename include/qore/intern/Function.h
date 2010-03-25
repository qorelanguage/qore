/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  Function.h

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

#ifndef _QORE_FUNCTION_H

#define _QORE_FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

// these data structures are all private to the library

DLLLOCAL AbstractQoreNode *doPartialEval(class AbstractQoreNode *n, bool *is_self_ref, ExceptionSink *xsink);

DLLLOCAL AbstractQoreNode *f_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_bool_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_bool_true_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_string_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_float_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_float_one_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_float_minus_infinity_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_int_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_int_minus_one_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_int_one_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_list_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_date_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_reldate_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *f_binary_noop(const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *class_noop(QoreObject *self, AbstractPrivateData *ptr, const QoreListNode *args, ExceptionSink *xsink);

class LocalVar;
class VarRefNode;
class BCAList;
class QoreTreeNode;

class AbstractFunctionSignature {
protected:
   unsigned short num_param_types,    // number of parameters that have type information
      min_param_types;                // minimum number of parameters with type info (without default args)

   const QoreTypeInfo *returnTypeInfo;
   type_vec_t typeList;
   arg_vec_t defaultArgList;

   // parameter signature string
   std::string str;

public:
   DLLLOCAL AbstractFunctionSignature(const QoreTypeInfo *n_returnTypeInfo = 0) : num_param_types(0), min_param_types(0), returnTypeInfo(n_returnTypeInfo) {
   }
   DLLLOCAL AbstractFunctionSignature(const QoreTypeInfo *n_returnTypeInfo, const type_vec_t &n_typeList, const arg_vec_t &n_defaultArgList) : num_param_types(0), min_param_types(0), returnTypeInfo(n_returnTypeInfo), typeList(n_typeList), defaultArgList(n_defaultArgList) {
   }
   DLLLOCAL virtual ~AbstractFunctionSignature() {
      // delete all default argument expressions
      for (arg_vec_t::iterator i = defaultArgList.begin(), e = defaultArgList.end(); i != e; ++i)
         if (*i)
            (*i)->deref(0);
   }
   // called at parse time to include optional type resolution
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const = 0;

   DLLLOCAL const QoreTypeInfo *getReturnTypeInfo() const {
      return returnTypeInfo;
   }
   DLLLOCAL const arg_vec_t &getDefaultArgList() const {
      return defaultArgList;
   }
   DLLLOCAL const type_vec_t &getTypeList() const {
      return typeList;
   }   
   DLLLOCAL AbstractQoreNode *evalDefaultArg(unsigned i, ExceptionSink *xsink) const {
      assert(i < defaultArgList.size());
      return defaultArgList[i] ? defaultArgList[i]->eval(xsink) : 0;
   }

   DLLLOCAL const char *getSignatureText() const {
      return str.c_str();
   }
   DLLLOCAL unsigned numParams() const {
      return typeList.size();
   }
   // number of params with type information
   DLLLOCAL unsigned getParamTypes() const {
      return num_param_types;
   }
   DLLLOCAL unsigned getMinParamTypes() const {
      return min_param_types;
   }
   DLLLOCAL const QoreTypeInfo *getParamTypeInfo(unsigned num) const {
      return num >= typeList.size() ? 0 : typeList[num];
   }
   DLLLOCAL bool hasDefaultArg(unsigned i) const {
      return i >= defaultArgList.size() || !defaultArgList[i] ? false : true;
   }
   // adds a description of the given default argument to the signature string
   DLLLOCAL void addDefaultArgument(const AbstractQoreNode *arg);

   DLLLOCAL virtual const char *getName(unsigned i) const = 0;
};

// used to store return type info during parsing for user code
class RetTypeInfo {
   QoreParseTypeInfo *parseTypeInfo;
   const QoreTypeInfo *typeInfo;

public:

   DLLLOCAL RetTypeInfo(QoreParseTypeInfo *n_parseTypeInfo, const QoreTypeInfo *n_typeInfo) : parseTypeInfo(n_parseTypeInfo), typeInfo(n_typeInfo) {
   }
   DLLLOCAL ~RetTypeInfo() {
      delete parseTypeInfo;      
   }
   DLLLOCAL const QoreTypeInfo *getTypeInfo() const {
      return typeInfo;
   }
   DLLLOCAL QoreParseTypeInfo *takeParseTypeInfo() {
      QoreParseTypeInfo *rv = parseTypeInfo;
      parseTypeInfo = 0;
      return rv;
   }
};

typedef std::vector<QoreParseTypeInfo *> ptype_vec_t;
typedef std::vector<std::string> name_vec_t;
typedef std::vector<LocalVar *> lvar_vec_t;

class UserSignature : public AbstractFunctionSignature {
protected:
   ptype_vec_t parseTypeList;
   QoreParseTypeInfo *parseReturnTypeInfo;
   name_vec_t names;

   int first_line, last_line;
   const char *parse_file;

   DLLLOCAL void pushParam(QoreTreeNode *t, bool needs_types);
   DLLLOCAL void pushParam(VarRefNode *v, AbstractQoreNode *defArg, bool needs_types);

   DLLLOCAL static void param_error() {
      parse_error("parameter list contains non-variable reference expressions");
   }

public:
   lvar_vec_t lv;
   LocalVar *argvid;
   LocalVar *selfid;
   bool resolved;

   DLLLOCAL UserSignature(int n_first_line, int n_last_line, AbstractQoreNode *params, RetTypeInfo *retTypeInfo);

   DLLLOCAL virtual ~UserSignature() {
      for (ptype_vec_t::iterator i = parseTypeList.begin(), e = parseTypeList.end(); i != e; ++i)
	 delete *i;
      delete parseReturnTypeInfo;
   }

   DLLLOCAL virtual const char *getName(unsigned i) const {
      assert(i < names.size());
      return names[i].c_str();
   }

   DLLLOCAL void setFirstParamType(const QoreTypeInfo *typeInfo) {
      assert(!typeList.empty());
      typeList[0] = typeInfo;
   }

   DLLLOCAL void setSelfId(LocalVar *n_selfid) {
      assert(!selfid);
      selfid = n_selfid;
   }

   DLLLOCAL const QoreParseTypeInfo *getParseParamTypeInfo(unsigned num) const {
      return num < parseTypeList.size() ? parseTypeList[num] : 0;
   }
      
   // resolves all parse types to the final types
   DLLLOCAL void resolve();

   // called at parse time to ensure types are resolved
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const {
      const_cast<UserSignature *>(this)->resolve();
      return returnTypeInfo;
   }

   DLLLOCAL const QoreTypeInfo *getReturnTypeInfo() const {
      assert(resolved);
      return returnTypeInfo;
   }

   DLLLOCAL int firstLine() const {
      return first_line;
   }
   
   DLLLOCAL int lastLine() const {
      return last_line;
   }
   
   DLLLOCAL const char *getParseFile() const {
      return parse_file;
   }

   DLLLOCAL bool hasReturnTypeInfo() const {
      return parseReturnTypeInfo || returnTypeInfo;
   }

   DLLLOCAL void parseInitPushLocalVars(const QoreTypeInfo *classTypeInfo);

   DLLLOCAL void parseInitPopLocalVars();
};

class UserVariantBase;

// describes the details of the function variant
class AbstractQoreFunctionVariant : protected QoreReferenceCounter {
protected:
   DLLLOCAL virtual ~AbstractQoreFunctionVariant() {}

public:
   DLLLOCAL AbstractQoreFunctionVariant() {}

   DLLLOCAL virtual AbstractFunctionSignature *getSignature() const = 0;
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const = 0;
   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const = 0;

   DLLLOCAL virtual unsigned numParams() const {
      AbstractFunctionSignature *sig = getSignature();
      return sig ? sig->numParams() : 0;
   }

   DLLLOCAL virtual qore_call_t getCallType() const = 0;
   DLLLOCAL virtual int64 getFlags() const = 0;
   DLLLOCAL virtual int64 getFunctionality() const = 0;

   DLLLOCAL virtual UserVariantBase *getUserVariantBase() = 0;
   DLLLOCAL const UserVariantBase *getUserVariantBase() const {
      return const_cast<AbstractQoreFunctionVariant *>(this)->getUserVariantBase();
   }

   DLLLOCAL virtual AbstractQoreNode *evalFunction(const char *name, const QoreListNode *args, ExceptionSink *xsink) const {
      assert(false);
      return 0;
   }

   DLLLOCAL AbstractQoreFunctionVariant *ref() { ROreference(); return this; }
   DLLLOCAL void deref() { if (ROdereference()) { delete this; } }

   DLLLOCAL virtual bool isUser() const = 0;
};

class VRMutex;
class UserVariantExecHelper;

// base implementation shared between all user variants
class UserVariantBase {
   friend class UserVariantExecHelper;

protected:
   StatementBlock *statements;
   UserSignature signature;
   bool synchronized;
   // for "synchronized" functions
   VRMutex *gate;
   // flag to recheck params against committed after type resolution
   bool recheck;

   DLLLOCAL AbstractQoreNode *evalIntern(ReferenceHolder<QoreListNode> &argv, QoreObject *self, ExceptionSink *xsink, const char *class_name) const;
   DLLLOCAL AbstractQoreNode *eval(const char *name, const QoreListNode *args, QoreObject *self, ExceptionSink *xsink, const char *class_name = 0) const;
   DLLLOCAL int setupCall(const QoreListNode *args, ReferenceHolder<QoreListNode> &argv, ExceptionSink *xsink) const;

public:
   DLLLOCAL UserVariantBase(StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, RetTypeInfo *rv, bool synced);
   DLLLOCAL virtual ~UserVariantBase();
   DLLLOCAL UserSignature *getUserSignature() const {
      return const_cast<UserSignature *>(&signature);
   }
   // set flag to recheck params against committed variants in stage 2 parsing after type resolution
   DLLLOCAL void setRecheck() {
      recheck = true;
   }
   DLLLOCAL bool getRecheck() const {
      return recheck;
   }
};

// the following defines the pure virtual functions that are common to all user variants
#define COMMON_USER_VARIANT_FUNCTIONS DLLLOCAL virtual qore_call_t getCallType() const { return CT_USER; } \
   DLLLOCAL virtual int64 getFlags() const { return QC_NO_FLAGS; } \
   DLLLOCAL virtual int64 getFunctionality() const { return QDOM_DEFAULT; } \
   DLLLOCAL virtual UserVariantBase *getUserVariantBase() { return static_cast<UserVariantBase *>(this); } \
   DLLLOCAL virtual AbstractFunctionSignature *getSignature() const { return const_cast<UserSignature *>(&signature); } \
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const { return signature.parseGetReturnTypeInfo(); } \
   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const { return signature.getReturnTypeInfo(); } \
   DLLLOCAL virtual bool isUser() const { return true; }

// this class ensures that instantiated variables in user code are uninstantiated, even if an exception occurs
class UserVariantExecHelper {
protected:
   const UserVariantBase *uvb;
   ReferenceHolder<QoreListNode> argv;
   ExceptionSink *xsink;

public:
   DLLLOCAL UserVariantExecHelper(const UserVariantBase *n_uvb, const QoreListNode *args, ExceptionSink *n_xsink) : uvb(n_uvb), argv(n_xsink), xsink(n_xsink) {
      if (uvb->setupCall(args, argv, xsink))
	 uvb = 0;
   }
   DLLLOCAL ~UserVariantExecHelper();
   DLLLOCAL operator bool() const {
      return uvb;
   }
   DLLLOCAL ReferenceHolder<QoreListNode> &getArgv() {
      return argv;
   }
};

class UserFunctionVariant : public AbstractQoreFunctionVariant, public UserVariantBase {
protected:
   DLLLOCAL virtual ~UserFunctionVariant() {
   }

public:
   DLLLOCAL UserFunctionVariant(StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, RetTypeInfo *rv, bool synced) : UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, rv, synced) {
   }

   // the following defines the pure virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL virtual AbstractQoreNode *evalFunction(const char *name, const QoreListNode *args, ExceptionSink *xsink) const {
      return eval(name, args, 0, xsink);
   }
   DLLLOCAL void parseInit(const char *fname);
};

#define UFV(f) (reinterpret_cast<UserFunctionVariant *>(f))
#define UFV_const(f) (reinterpret_cast<const UserFunctionVariant *>(f))

class CodeEvaluationHelper {
protected:
   qore_call_t ct;
   const char *name;
   ExceptionSink *xsink;
   const char *class_name, *o_fn;
   int o_ln, o_eln;
   QoreListNodeEvalOptionalRefHolder tmp;
   const QoreTypeInfo *returnTypeInfo; // saved return type info

public:
   // saves current program location in case there's an exception
   DLLLOCAL CodeEvaluationHelper(ExceptionSink *n_xsink, const char *n_name, const QoreListNode *args = 0, const char *n_class_name = 0, qore_call_t n_ct = CT_UNUSED)
      : ct(n_ct), name(n_name), xsink(n_xsink), class_name(n_class_name), tmp(n_xsink), returnTypeInfo((const QoreTypeInfo *)-1) {
      o_fn = get_pgm_counter(o_ln, o_eln);
      tmp.assignEval(args);
      // reset program position if arguments were evaluated
      if (tmp.needsDeref())
	 update_pgm_counter_pgm_file(o_ln, o_eln, o_fn);      
   }
   DLLLOCAL ~CodeEvaluationHelper() {
      if (returnTypeInfo != (const QoreTypeInfo *)-1)
         saveReturnTypeInfo(returnTypeInfo);
      if (ct != CT_UNUSED && xsink->isException())
	 xsink->addStackInfo(ct, class_name, name, o_fn, o_ln, o_eln);
   }
   DLLLOCAL void setReturnTypeInfo(const QoreTypeInfo *n_returnTypeInfo) {
      returnTypeInfo = saveReturnTypeInfo(n_returnTypeInfo);
   }
   DLLLOCAL void setClassName(const char *n_class_name) {
      class_name = n_class_name;
   }
   // once this is set, exception information will be raised in the destructor if an exception has been raised
   DLLLOCAL void setCallType(qore_call_t n_ct) {
      ct = n_ct;
   }
   DLLLOCAL int processDefaultArgs(const AbstractQoreFunctionVariant *variant, ExceptionSink *xsink);

   DLLLOCAL const QoreListNode *getArgs() const {
      return *tmp;
   }
   DLLLOCAL void restorePosition() const {
      update_pgm_counter_pgm_file(o_ln, o_eln, o_fn);   
   }
};

// type for lists of function variants
// this type will be read at runtime and could be appended to simultaneously at parse time (under a lock)
typedef safe_dslist<AbstractQoreFunctionVariant *> vlist_t;

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

class AbstractQoreFunction {
protected:
   // inheritance list type
   typedef std::vector<AbstractQoreFunction *> ilist_t;

   // list of function variants
   VList vlist;

   // list of pending user-code function variants
   VList pending_vlist;

   // list of inherited methods for variant matching; the first pointer is always a pointer to "this"
   ilist_t ilist;

   // if true means all variants have the same return value
   bool same_return_type, parse_same_return_type;
   int64 unique_functionality;

   // convenience function for returning the first variant in the list
   DLLLOCAL const AbstractQoreFunctionVariant *first() const {
      assert(!vlist.empty());
      return *(vlist.begin());
   }

   // convenience function for returning the first variant in the list
   DLLLOCAL AbstractQoreFunctionVariant *first() {
      assert(!vlist.empty());
      return *(vlist.begin());
   }

   // convenience function for returning the first variant in the list
   DLLLOCAL const AbstractQoreFunctionVariant *pending_first() const {
      assert(!pending_vlist.empty());
      return *(pending_vlist.begin());
   }

   // returns 0 for OK, -1 for error
   DLLLOCAL int parseCheckDuplicateSignatureCommitted(UserVariantBase *variant);

   // returns 0 for OK, -1 for error
   DLLLOCAL int parseCheckDuplicateSignature(UserVariantBase *variant);

   // FIXME: does not check unparsed types properly
   DLLLOCAL void addVariant(AbstractQoreFunctionVariant *variant) {
      if (same_return_type && !vlist.empty() && !variant->getReturnTypeInfo()->checkIdentical(first()->getReturnTypeInfo()))
	 same_return_type = false;

      int64 vf = variant->getFunctionality();
      if (vlist.empty())
	 unique_functionality = vf;
      else {
	 if (vf != unique_functionality)
	    unique_functionality = QDOM_DEFAULT;
      }

      vlist.push_back(variant);
   }

public:
   DLLLOCAL AbstractQoreFunction() : same_return_type(true), parse_same_return_type(true), unique_functionality(QDOM_DEFAULT) {
      ilist.push_back(this);
   }

   // copy constructor (used by method functions when copied)
   DLLLOCAL AbstractQoreFunction(const AbstractQoreFunction &old) : same_return_type(old.same_return_type), 
                                                                    parse_same_return_type(old.parse_same_return_type), 
                                                                    unique_functionality(old.unique_functionality) {      
      // copy variants by reference
      for (vlist_t::const_iterator i = old.vlist.begin(), e = old.vlist.end(); i != e; ++i)
         vlist.push_back((*i)->ref());

      // do not copy pending variants
      // ilist is copied in method base class
   }

   DLLLOCAL virtual ~AbstractQoreFunction() {
   }

   DLLLOCAL virtual const char *getName() const = 0;

   DLLLOCAL virtual const QoreClass *getClass() const = 0;

   DLLLOCAL virtual void ref() = 0;
   DLLLOCAL virtual void deref() = 0;

   DLLLOCAL const char *className() const {
      const QoreClass *qc = getClass();
      return qc ? qc->getName() : 0;
   }

   DLLLOCAL void addAncestor(AbstractQoreFunction *ancestor) {
      ilist.push_back(ancestor);
   }
   
   DLLLOCAL void addNewAncestor(AbstractQoreFunction *ancestor) {
      for (ilist_t::iterator i = ilist.begin(), e = ilist.end(); i != e; ++i)
         if (*i == ancestor)
            return;
      ilist.push_back(ancestor);
   }

   // resolves all types in signatures and return types in pending variants; called during the "parseInit" phase
   DLLLOCAL void resolvePendingSignatures();

   DLLLOCAL AbstractFunctionSignature *getUniqueSignature() const {
      return vlist.singular() ? first()->getSignature() : 0;
   }

   DLLLOCAL AbstractFunctionSignature *parseGetUniqueSignature() const;

   DLLLOCAL int64 getUniqueFunctionality() const {
      return unique_functionality;
   }

   // object takes ownership of variant or deletes it if it can't be added
   DLLLOCAL int parseAddVariant(AbstractQoreFunctionVariant *variant);

   DLLLOCAL void parseCommit();
   DLLLOCAL void parseRollback();

   DLLLOCAL const QoreTypeInfo *getUniqueReturnTypeInfo() const {
      return same_return_type && !vlist.empty() ? first()->getReturnTypeInfo() : 0;
   }

   DLLLOCAL const QoreTypeInfo *parseGetUniqueReturnTypeInfo() const {
      if (!same_return_type || !parse_same_return_type)
         return 0;

      if (!vlist.empty())
         return first()->getReturnTypeInfo();

      assert(!pending_vlist.empty());
      return pending_first()->getReturnTypeInfo();
   }

   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL AbstractQoreNode *evalFunction(const AbstractQoreFunctionVariant *variant, const QoreListNode *args, ExceptionSink *xsink) const;

   // finds a variant and checks variant capabilities against current program parse options and executes the variant
   DLLLOCAL AbstractQoreNode *evalDynamic(const QoreListNode *args, ExceptionSink *xsink) const;

   // find variant at parse time, throw parse exception if no variant can be matched
   DLLLOCAL const AbstractQoreFunctionVariant *parseFindVariant(const type_vec_t &argTypeInfo);

   // returns true if there are no committed variants in the function
   DLLLOCAL bool committedEmpty() const {
      return vlist.empty();
   }

   DLLLOCAL bool pendingEmpty() const {
      return pending_vlist.empty();
   }

   DLLLOCAL void addBuiltinVariant(AbstractQoreFunctionVariant *variant);

   DLLLOCAL bool existsVariant(const type_vec_t &paramTypeInfo) const;

   // find variant at runtime
   // if only_user is set, then no exception is raised if the user variant is not found
   DLLLOCAL const AbstractQoreFunctionVariant *findVariant(const QoreListNode *args, bool only_user, ExceptionSink *xsink) const;

   DLLLOCAL const AbstractQoreFunctionVariant *runtimeFindVariant(const type_vec_t &argTypeList, bool only_user = false) const;

   // convenience function for returning the first variant in the list
   DLLLOCAL AbstractQoreFunctionVariant *pending_first() {
      assert(!pending_vlist.empty());
      return *(pending_vlist.begin());
   }
};

class AbstractReferencedFunction : public AbstractQoreFunction, protected QoreReferenceCounter {
public:
   DLLLOCAL virtual void ref() {
      ROreference();
   }
   DLLLOCAL virtual void deref() {
      if (ROdereference())
	 delete this;
   }
};

class MethodVariantBase;
class MethodFunctionBase;
#define METHFB(f) (reinterpret_cast<MethodFunctionBase *>(f))

class MethodFunctionBase : public AbstractQoreFunction {
protected:
   bool all_private, 
      pending_all_private;
   const QoreClass *qc;   

   // pointer to copy, only valid during copy
   mutable MethodFunctionBase *new_copy;

public:
   DLLLOCAL MethodFunctionBase(const QoreClass *n_qc) : all_private(true), pending_all_private(true), qc(n_qc), new_copy(0) {
   }

   // copy constructor, only copies comitted variants
   DLLLOCAL MethodFunctionBase(const MethodFunctionBase &old, const QoreClass *n_qc) 
      : AbstractQoreFunction(old), 
        all_private(old.all_private), 
        pending_all_private(true),
        qc(n_qc) {
      //printd(5, "MethodFunctionBase() copying old=%p -> new=%p %p %s::%s() %p %s::%s()\n", &old, this, old.qc, old.qc->getName(), old.getName(), qc, qc->getName(), old.getName());

      // set a pointer to the new function
      old.new_copy = this;

      // copy ilist, will be adjusted for new class pointers after all classes have been copied
      ilist.reserve(old.ilist.size());
      for (ilist_t::const_iterator i = old.ilist.begin(), e = old.ilist.end(); i != e; ++i)
         ilist.push_back(*i);
   }

   DLLLOCAL void resolveCopy() {
      for (ilist_t::iterator i = ilist.begin(), e = ilist.end(); i != e; ++i) {
         MethodFunctionBase *mfb = METHFB(*i);
#ifdef DEBUG
         if (!mfb->new_copy)
            printd(0, "error resolving %p %s::%s() base method %p %s::%s() nas no new method pointer\n", qc, qc->getName(), getName(), mfb->qc, mfb->qc->getName(), getName());
         assert(mfb->new_copy);
         //printd(5, "resolving %p %s::%s() base method %p %s::%s() from %p -> %p\n", qc, qc->getName(), getName(), mfb->qc, mfb->qc->getName(), getName(), mfb, mfb->new_copy);
#endif
         *i = mfb->new_copy;         
      }
   }

   // returns -1 for error, 0 = OK
   DLLLOCAL int parseAddUserMethodVariant(MethodVariantBase *variant);
   // maintains all_private flag and commits the builtin variant
   DLLLOCAL void addBuiltinMethodVariant(MethodVariantBase *variant);
   // maintains all_private flag and commits user variants
   DLLLOCAL void parseCommitMethod();
   DLLLOCAL void parseRollbackMethod();
   DLLLOCAL bool isUniquelyPrivate() const {
      return all_private;
   }
   DLLLOCAL bool parseIsUniquelyPrivate() const {
      return all_private && pending_all_private;
   }
   DLLLOCAL virtual const QoreClass *getClass() const {
      return qc;
   }

   // virtual copy constructor
   DLLLOCAL virtual MethodFunctionBase *copy(const QoreClass *n_qc) const = 0;

   DLLLOCAL virtual void ref() {
      assert(false);
   }
   DLLLOCAL virtual void deref() {
      delete this;
   }
};

class UserFunction : public AbstractReferencedFunction {
protected:
   char *name;

   DLLLOCAL virtual ~UserFunction();

public:
   // the object owns the memory for "n_name", name is 0 for anonymous closures
   DLLLOCAL UserFunction(char *n_name);

   DLLLOCAL virtual const char *getName() const {
      return name;
   }

   DLLLOCAL void parseInit();
   DLLLOCAL virtual const QoreClass *getClass() const {
      return 0;
   }
};

class UserParamListLocalVarHelper {
protected:
   UserSignature *l;

public:
   DLLLOCAL UserParamListLocalVarHelper(UserSignature *n_l, const QoreTypeInfo *classTypeInfo = 0) : l(n_l) {
      l->parseInitPushLocalVars(classTypeInfo);
   }
   DLLLOCAL ~UserParamListLocalVarHelper() {
      l->parseInitPopLocalVars();
   }
};

class UserClosureVariant : public UserFunctionVariant {
protected:
public:
   DLLLOCAL UserClosureVariant(StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, RetTypeInfo *rv, bool synced = false) : UserFunctionVariant(b, n_sig_first_line, n_sig_last_line, params, rv, synced) {
   }
   DLLLOCAL void parseInitClosure(const QoreTypeInfo *classTypeInfo, lvar_set_t *vlist);
   DLLLOCAL AbstractQoreNode *evalClosure(const QoreListNode *args, QoreObject *self, ExceptionSink *xsink) const {
      return eval("<anonymous closure>", args, self, xsink);
   }
};

#define UCLOV(f) (reinterpret_cast<UserClosureVariant *>(f))
#define UCLOV_const(f) (reinterpret_cast<const UserClosureVariant *>(f))

class UserClosureFunction : public AbstractQoreFunction {
protected:

public:
   DLLLOCAL UserClosureFunction(StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, RetTypeInfo *rv, bool synced = false) {
      parseAddVariant(new UserClosureVariant(b, n_sig_first_line, n_sig_last_line, params, rv, synced));
   }

   DLLLOCAL virtual const char *getName() const {
      return "<anonymous closure>";
   }

   DLLLOCAL virtual const QoreClass *getClass() const {
      return 0;
   }

   DLLLOCAL bool parseStage1HasReturnTypeInfo() const {
      return reinterpret_cast<const UserClosureVariant *>(pending_first())->getUserSignature()->hasReturnTypeInfo();
   }
   
   DLLLOCAL AbstractQoreNode *evalClosure(const QoreListNode *args, QoreObject *self, ExceptionSink *xsink) const;

   DLLLOCAL void parseInitClosure(const QoreTypeInfo *classTypeInfo, lvar_set_t *vlist);

   DLLLOCAL virtual void ref() {
      assert(false);
   }
   DLLLOCAL virtual void deref() {
      assert(false);
   }
};

#endif // _QORE_FUNCTION_H
