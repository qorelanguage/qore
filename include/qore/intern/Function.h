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

// these data structures are all private to the library

DLLLOCAL AbstractQoreNode *doPartialEval(class AbstractQoreNode *n, bool *is_self_ref, ExceptionSink *xsink);

class LocalVar;
class VarRefNode;
class BCAList;

class AbstractFunctionSignature {
protected:
   // number of parameters
   unsigned short num_params;
   // number of parameters that have type information
   unsigned short num_param_types;
   // parameter signature string
   std::string str;

public:
   DLLLOCAL AbstractFunctionSignature(unsigned n_num_params) : num_params(n_num_params), num_param_types(0) {
   }
   DLLLOCAL virtual ~AbstractFunctionSignature() {
   }
   DLLLOCAL virtual void resolve() = 0;
   DLLLOCAL virtual const QoreTypeInfo *getParamTypeInfoImpl(unsigned num) const = 0;
   // called at parse time to include optional type resolution
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const = 0;
   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const = 0;
   DLLLOCAL virtual const AbstractQoreNode **getDefaultArgList() const = 0;

   DLLLOCAL const char *getSignatureText() const {
      return str.c_str();
   }

   DLLLOCAL unsigned numParams() const {
      return num_params;
   }
   // number of params with type information
   DLLLOCAL unsigned getParamTypes() const {
      return num_param_types;
   }
   DLLLOCAL const QoreTypeInfo *getParamTypeInfo(unsigned num) const {
      return num >= num_params ? 0 : getParamTypeInfoImpl(num);
   }
};

class UserSignature : public AbstractFunctionSignature {
protected:
   QoreParseTypeInfo *returnTypeInfo;
   int first_line, last_line;
   const char *parse_file;

   DLLLOCAL void assignParam(int i, VarRefNode *v);

   DLLLOCAL void setSingleParamIntern(VarRefNode *v) {
      num_params = 1;
      names = new char *[1];
      typeList = new QoreParseTypeInfo *[1];
      assignParam(0, v);
   }

   DLLLOCAL static void param_error() {
      parse_error("parameter list contains non-variable reference expressions");
   }

public:
   char **names;
   QoreParseTypeInfo **typeList;
   LocalVar **lv;
   LocalVar *argvid;
   LocalVar *selfid;
   bool resolved;

   DLLLOCAL UserSignature(int n_first_line, int n_last_line, AbstractQoreNode *params, QoreParseTypeInfo *n_returnTypeInfo);

   DLLLOCAL virtual ~UserSignature() {
      for (unsigned i = 0; i < num_params; i++)
	 free(names[i]);
      delete [] names;
      if (typeList) {
	 for (unsigned i = 0; i < num_params; i++)
	    delete typeList[i];
	 delete [] typeList;
      }
      delete [] lv;
      delete returnTypeInfo;
   }

   DLLLOCAL void setSelfId(LocalVar *n_selfid) {
      assert(!selfid);
      selfid = n_selfid;
   }

   DLLLOCAL virtual const QoreTypeInfo *getParamTypeInfoImpl(unsigned num) const {
      return typeList[num];
   }

   DLLLOCAL const QoreParseTypeInfo *getParseParamTypeInfo(unsigned num) const {
      return num < num_params ? typeList[num] : 0;
   }
      
   DLLLOCAL virtual void resolve() {
      if (resolved)
	 return;
      
      resolved = true;
      if (!num_params)
	 return;

      for (unsigned i = 0; i < num_params; ++i)
	 if (typeList[i])
	    typeList[i]->resolve();			 
   }

   // called at parse time to include optional type resolution
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const {
      if (returnTypeInfo)
	 returnTypeInfo->resolve();

      return returnTypeInfo;
   }

   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const {
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

   DLLLOCAL virtual const AbstractQoreNode **getDefaultArgList() const {
      return 0;
   }

   DLLLOCAL void parseInitPushLocalVars(const QoreTypeInfo *classTypeInfo);

   DLLLOCAL void parseInitPopLocalVars();
};

class UserVariantBase;

// describes the details of the function variant
class AbstractQoreFunctionVariant {
protected:
public:
   DLLLOCAL AbstractQoreFunctionVariant() {}
   DLLLOCAL virtual ~AbstractQoreFunctionVariant() {}

   DLLLOCAL virtual AbstractFunctionSignature *getSignature() const = 0;
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const = 0;
   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const = 0;

   DLLLOCAL virtual unsigned numParams() const {
      AbstractFunctionSignature *sig = getSignature();
      return sig ? sig->numParams() : 0;
   }

   DLLLOCAL virtual qore_call_t getCallType() const = 0;
   DLLLOCAL virtual int getFunctionality() const = 0;

   DLLLOCAL virtual UserVariantBase *getUserVariantBase() = 0;
   DLLLOCAL const UserVariantBase *getConstUserVariantBase() const {
      return const_cast<AbstractQoreFunctionVariant *>(this)->getUserVariantBase();
   }

   DLLLOCAL virtual AbstractQoreNode *evalFunction(const char *name, const QoreListNode *args, ExceptionSink *xsink) const {
      assert(false);
      return 0;
   }
};

class VRMutex;

// base implementation shared between all user variants
class UserVariantBase {
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
   DLLLOCAL UserVariantBase(StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, QoreParseTypeInfo *rv, bool synced);
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
   DLLLOCAL virtual int getFunctionality() const { return QDOM_DEFAULT; } \
   DLLLOCAL virtual UserVariantBase *getUserVariantBase() { return static_cast<UserVariantBase *>(this); } \
   DLLLOCAL virtual AbstractFunctionSignature *getSignature() const { return const_cast<UserSignature *>(&signature); } \
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const { return signature.parseGetReturnTypeInfo(); } \
   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const { return signature.getReturnTypeInfo(); }

class UserFunctionVariant : public AbstractQoreFunctionVariant, public UserVariantBase {
protected:

public:
   DLLLOCAL UserFunctionVariant(StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, QoreParseTypeInfo *rv, bool synced) : UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, rv, synced) {
   }
   DLLLOCAL virtual ~UserFunctionVariant() {
   }

   // the following defines the pure virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL virtual AbstractQoreNode *evalFunction(const char *name, const QoreListNode *args, ExceptionSink *xsink) const {
      return eval(name, args, 0, xsink);
   }
   DLLLOCAL void parseInit();
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
   
public:
   // saves current program location in case there's an exception
   DLLLOCAL CodeEvaluationHelper(ExceptionSink *n_xsink, const char *n_name, const QoreListNode *args = 0, const char *n_class_name = 0, qore_call_t n_ct = CT_UNUSED)
      : ct(n_ct), name(n_name), xsink(n_xsink), class_name(n_class_name), o_fn(get_pgm_file()), tmp(n_xsink) {
      get_pgm_counter(o_ln, o_eln);
      tmp.assignEval(args);
      // reset program position if arguments were evaluated
      if (tmp.needsDeref())
	 update_pgm_counter_pgm_file(o_ln, o_eln, o_fn);   
   }
   DLLLOCAL ~CodeEvaluationHelper() {
      if (ct != CT_UNUSED && xsink->isException())
	 xsink->addStackInfo(ct, class_name, name, o_fn, o_ln, o_eln);
   }
   // once this is set, exception information will be raised in the destructor if an exception has been raised
   DLLLOCAL void setCallType(qore_call_t n_ct) {
      ct = n_ct;
   }
   DLLLOCAL const QoreListNode *getArgs() const {
      return *tmp;
   }
   DLLLOCAL void restorePosition() const {
      update_pgm_counter_pgm_file(o_ln, o_eln, o_fn);   
   }
};

// type for lists of function variants
typedef safe_dslist<AbstractQoreFunctionVariant *> vlist_t;

class VList : public vlist_t {
public:
   DLLLOCAL ~VList() {
      del();
   }
   DLLLOCAL void del() {
      // delete all variants
      for (vlist_t::iterator i = begin(), e = end(); i != e; ++i)
	 delete *i;
      clear();
   }
};

class AbstractQoreFunction {
protected:
   // list of function variants
   VList vlist;
   // list of pending user-code function variants
   VList pending_vlist;
   // if true means all variants have the same return value
   bool same_return_type;
   int unique_functionality;

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

   // convenience function for returning the first variant in the list
   DLLLOCAL AbstractQoreFunctionVariant *pending_first() {
      assert(!pending_vlist.empty());
      return *(pending_vlist.begin());
   }

   // returns 0 for OK, -1 for error
   DLLLOCAL int parseCheckDuplicateSignatureCommitted(UserVariantBase *variant);

   // returns 0 for OK, -1 for error
   DLLLOCAL int parseCheckDuplicateSignature(UserVariantBase *variant);

   // FIXME: does not check unparsed types properly
   DLLLOCAL void addVariant(AbstractQoreFunctionVariant *variant) {
      if (same_return_type && !vlist.empty() && variant->getReturnTypeInfo() != first()->getReturnTypeInfo())
	 same_return_type = false;

      int vf = variant->getFunctionality();
      if (vlist.empty())
	 unique_functionality = vf;
      else {
	 if (vf != unique_functionality)
	    unique_functionality = QDOM_DEFAULT;
      }

      vlist.push_back(variant);
   }

   // find variant at runtime
   DLLLOCAL const AbstractQoreFunctionVariant *findVariant(const QoreListNode *args, ExceptionSink *xsink, const char *class_name = 0) const;

public:
   DLLLOCAL AbstractQoreFunction() : same_return_type(true), unique_functionality(QDOM_DEFAULT) {
   }
   DLLLOCAL virtual ~AbstractQoreFunction() {
   }

   DLLLOCAL virtual const char *getName() const = 0;

   DLLLOCAL virtual void ref() = 0;
   DLLLOCAL virtual void deref() = 0;

   DLLLOCAL AbstractFunctionSignature *getUniqueSignature() const {
      return vlist.singular() ? first()->getSignature() : 0;
   }

   DLLLOCAL int getUniqueFunctionality() const {
      return unique_functionality;
   }

   // object takes ownership of variant or deletes it if it can't be added
   DLLLOCAL int parseAddVariant(AbstractQoreFunctionVariant *variant);

   DLLLOCAL void parseCommit();
   DLLLOCAL void parseRollback();

   DLLLOCAL const QoreTypeInfo *parseGetUniqueReturnTypeInfo() const {
      if (!same_return_type)
	 return 0;

      const QoreTypeInfo *rv = 0;
      for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
	 rv = (*i)->parseGetReturnTypeInfo();
      }
      return rv;
   }

   DLLLOCAL const QoreTypeInfo *getUniqueReturnTypeInfo() const {
      return same_return_type && !vlist.empty() ? first()->getReturnTypeInfo() : 0;
   }

   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL AbstractQoreNode *evalFunction(const AbstractQoreFunctionVariant *variant, const QoreListNode *args, ExceptionSink *xsink) const;

   // finds a variant and checks variant capabilities against current program parse options and executes the variant
   DLLLOCAL AbstractQoreNode *evalDynamic(const QoreListNode *args, ExceptionSink *xsink) const;

   // find variant at parse time, throw parse exception if no variant can be matched
   DLLLOCAL const AbstractQoreFunctionVariant *parseFindVariant(unsigned num_args, const QoreTypeInfo **argTypeInfo, const char *class_name = 0);

   // returns true if there are no committed variants in the function
   DLLLOCAL bool committedEmpty() const {
      return vlist.empty();
   }

   DLLLOCAL void addBuiltinVariant(AbstractQoreFunctionVariant *variant);

   DLLLOCAL bool existsVariant(unsigned p_num_params, const QoreTypeInfo **paramTypeInfo) const;
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

class MethodFunctionBase : public AbstractReferencedFunction {
protected:
   bool all_private, pending_all_private;

public:
   DLLLOCAL MethodFunctionBase() : all_private(true), pending_all_private(true) {
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
};

#define METHFB(f) (reinterpret_cast<MethodFunctionBase *>(f))

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
   DLLLOCAL UserClosureVariant(StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, QoreParseTypeInfo *rv, bool synced = false) : UserFunctionVariant(b, n_sig_first_line, n_sig_last_line, params, rv, synced) {
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
   DLLLOCAL UserClosureFunction(StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, QoreParseTypeInfo *rv, bool synced = false) {
      parseAddVariant(new UserClosureVariant(b, n_sig_first_line, n_sig_last_line, params, rv, synced));
   }

   DLLLOCAL virtual const char *getName() const {
      return "<anonymous closure>";
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
