/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  FunctionCallNode.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_FUNCTIONCALLNODE_H

#define _QORE_FUNCTIONCALLNODE_H

#include <qore/Qore.h>

class FunctionCallBase {
protected:
   QoreListNode* args;
   const AbstractQoreFunctionVariant* variant;

public:
   DLLLOCAL FunctionCallBase(QoreListNode* n_args) : args(n_args), variant(0) {
   }

   DLLLOCAL FunctionCallBase(const FunctionCallBase &old) : args(old.args ? old.args->listRefSelf() : 0), variant(old.variant) {
   }

   DLLLOCAL FunctionCallBase(const FunctionCallBase &old, QoreListNode* n_args) : args(n_args), variant(old.variant) {
   }

   DLLLOCAL ~FunctionCallBase() {
      if (args)
         args->deref(0);
   }

   DLLLOCAL const QoreListNode* getArgs() const { return args; }

   DLLLOCAL int parseArgsVariant(const QoreProgramLocation& loc, LocalVar* oflag, int pflag, QoreFunction* func, const QoreTypeInfo*& returnTypeInfo);

   DLLLOCAL const AbstractQoreFunctionVariant* getVariant() const {
      return variant;
   }
};

class AbstractFunctionCallNode : public ParseNode, public FunctionCallBase {
protected:
   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const = 0;

   DLLLOCAL void doFlags(int64 flags) {
      if (flags & QC_RET_VALUE_ONLY)
         set_effect(false);
   }

public:
   DLLLOCAL AbstractFunctionCallNode(const QoreProgramLocation& loc, qore_type_t t, QoreListNode* n_args, bool needs_eval = true) : ParseNode(loc, t, needs_eval), FunctionCallBase(n_args) {
      has_value_api = true;
   }

   DLLLOCAL AbstractFunctionCallNode(const AbstractFunctionCallNode &old) : ParseNode(old), FunctionCallBase(old) {
      has_value_api = true;
   }

   DLLLOCAL AbstractFunctionCallNode(const AbstractFunctionCallNode &old, QoreListNode* n_args) : ParseNode(old), FunctionCallBase(old, n_args) {
      has_value_api = true;
   }

   DLLLOCAL virtual ~AbstractFunctionCallNode() {
      // there could be an object here in the case of a background expression
      if (args) {
         ExceptionSink xsink;
         args->deref(&xsink);
         args = 0;
      }
   }

   DLLLOCAL int parseArgs(LocalVar* oflag, int pflag, QoreFunction* func, const QoreTypeInfo*& returnTypeInfo) {
      int lvids = parseArgsVariant(loc, oflag, pflag, func, returnTypeInfo);
      // clear "effect" flag if possible, only if QC_CONSTANT is set on the variant or function
      if (variant)
         doFlags(variant->getFlags());
      else if (func)
         doFlags(func->parseGetUniqueFlags());
      return lvids;
   }

   DLLLOCAL virtual const char* getName() const = 0;
};

class FunctionCallNode : public AbstractFunctionCallNode {
protected:
   const QoreFunction* func;
   QoreProgram* pgm;
   char* c_str;
   // was this call enclosed in parentheses (in which case it will not be converted to a method call)
   bool finalized;

   using AbstractFunctionCallNode::evalImpl;
   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL FunctionCallNode(const QoreProgramLocation& loc, char* name, QoreListNode* a, qore_type_t n_type) : AbstractFunctionCallNode(loc, n_type, a), func(nullptr), pgm(nullptr), c_str(name), finalized(false) {
   }

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return variant ? variant->parseGetReturnTypeInfo() : (func ? const_cast<QoreFunction*>(func)->parseGetUniqueReturnTypeInfo() : nullptr);
   }

public:
   DLLLOCAL FunctionCallNode(const QoreProgramLocation& loc, const QoreFunction* f, QoreListNode* a, QoreProgram* n_pgm) : AbstractFunctionCallNode(loc, NT_FUNCTION_CALL, a), func(f), pgm(n_pgm), c_str(nullptr), finalized(false) {
   }

   // normal function call constructor
   DLLLOCAL FunctionCallNode(const QoreProgramLocation& loc, char* name, QoreListNode* a) : AbstractFunctionCallNode(loc, NT_FUNCTION_CALL, a), func(nullptr), pgm(nullptr), c_str(name), finalized(false) {
   }

   DLLLOCAL virtual ~FunctionCallNode() {
      printd(5, "FunctionCallNode::~FunctionCallNode(): func=%p c_str=%p (%s) args=%p\n", func, c_str, c_str ? c_str : "n/a", args);
      if (c_str)
         free(c_str);
   }

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink* xsink) const;
   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const;

   // returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const {
      return "function call";
   }

   DLLLOCAL QoreProgram* getProgram() const {
      return const_cast<QoreProgram*>(pgm);
   }

   DLLLOCAL AbstractQoreNode* parseInitCall(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

   DLLLOCAL void parseInitFinalizedCall(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

   DLLLOCAL virtual const char* getName() const {
      return func ? func->getName() : c_str;
   }

   DLLLOCAL const QoreFunction* getFunction() const {
      return func;
   }

   // FIXME: delete when unresolved function call node implemented properly
   DLLLOCAL char* takeName() {
      char* str = c_str;
      c_str = 0;
      return str;
   }

   // FIXME: delete when unresolved function call node implemented properly
   DLLLOCAL QoreListNode* takeArgs() {
      QoreListNode* rv = args;
      args = 0;
      return rv;
   }

   DLLLOCAL AbstractQoreNode* makeReferenceNodeAndDeref() {
      if (args) {
         parse_error(loc, "argument given to call reference");
         return this;
      }

      assert(!func);
      AbstractQoreNode* rv = makeReferenceNodeAndDerefImpl();
      deref();
      return rv;
   }

   DLLLOCAL virtual AbstractQoreNode* makeReferenceNodeAndDerefImpl();

   DLLLOCAL bool isFinalized() const {
      return finalized;
   }

   DLLLOCAL void setFinalized() {
      finalized = true;
   }
};

class ProgramFunctionCallNode : public FunctionCallNode {
public:
   DLLLOCAL ProgramFunctionCallNode(const QoreProgramLocation& loc, char* name, QoreListNode* a) : FunctionCallNode(loc, name, a, NT_PROGRAM_FUNC_CALL) {
   }

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
      return parseInitCall(oflag, pflag, lvids, typeInfo);
   }

   DLLLOCAL virtual AbstractQoreNode* makeReferenceNodeAndDerefImpl();
};

class AbstractMethodCallNode : public AbstractFunctionCallNode {
protected:
   // if a method pointer can be resolved at parse time, then the class
   // is used to compare the runtime class; if they are equal, then no search
   // is needed
   const QoreClass* qc;
   const QoreMethod* method;

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) = 0;
   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return variant ? variant->parseGetReturnTypeInfo() : (method ? method->getFunction()->parseGetUniqueReturnTypeInfo() : 0);
   }

public:
   DLLLOCAL AbstractMethodCallNode(const QoreProgramLocation& loc, qore_type_t t, QoreListNode* n_args, const QoreClass* n_qc = nullptr, const QoreMethod* m = nullptr) : AbstractFunctionCallNode(loc, t, n_args), qc(n_qc), method(m) {
   }

   DLLLOCAL AbstractMethodCallNode(const AbstractMethodCallNode &old) : AbstractFunctionCallNode(old), qc(old.qc), method(old.method) {
   }

   DLLLOCAL AbstractMethodCallNode(const AbstractMethodCallNode &old, QoreListNode* n_args) : AbstractFunctionCallNode(old, n_args), qc(old.qc), method(old.method) {
   }

   DLLLOCAL QoreValue exec(QoreObject* o, const char* cstr, ExceptionSink* xsink) const;

   DLLLOCAL const QoreClass* getClass() const {
      return qc;
   }

   DLLLOCAL const QoreMethod* getMethod() const {
      return method;
   }
};

class MethodCallNode : public AbstractMethodCallNode {
protected:
   char* c_str;
   bool pseudo;

   using AbstractFunctionCallNode::evalImpl;
   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
      assert(false);
      return QoreValue();
   }

   // note that the class and method are set in QoreDotEvalOperatorNode::parseInitImpl()
   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
      typeInfo = 0;
      lvids += parseArgs(oflag, pflag, 0, typeInfo);
      return this;
   }

public:
   DLLLOCAL MethodCallNode(const QoreProgramLocation& loc, char* name, QoreListNode* n_args) : AbstractMethodCallNode(loc, NT_METHOD_CALL, n_args), c_str(name), pseudo(false) {
      //printd(0, "MethodCallNode::MethodCallNode() this=%p name='%s' args=%p (len=%d)\n", this, c_str, args, args ? args->size() : -1);
   }

   DLLLOCAL MethodCallNode(const MethodCallNode &old, QoreListNode* n_args) : AbstractMethodCallNode(old.loc, NT_METHOD_CALL, n_args), c_str(old.c_str ? strdup(old.c_str) : nullptr), pseudo(old.pseudo) {
   }

   DLLLOCAL virtual ~MethodCallNode() {
      if (c_str)
         free(c_str);
   }

   using AbstractMethodCallNode::exec;
   DLLLOCAL QoreValue exec(QoreObject* o, ExceptionSink* xsink) const {
      return AbstractMethodCallNode::exec(o, c_str, xsink);
   }

   DLLLOCAL QoreValue execPseudo(const AbstractQoreNode* n, ExceptionSink* xsink) const;

   DLLLOCAL virtual const char* getName() const {
      return c_str ? c_str : "copy";
   }

   DLLLOCAL const char* getRawName() const {
      return c_str;
   }

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink* xsink) const {
      str.sprintf("'%s' %smethod call (%p)", getName(), pseudo ? "pseudo " : "", this);
      return 0;
   }

   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const {
      del = true;
      QoreString* rv = new QoreString();
      getAsString(*rv, foff, xsink);
      return rv;
   }

   DLLLOCAL char* takeName() {
      char* rv = c_str;
      c_str = 0;
      return rv;
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const {
      return getStaticTypeName();
   }

   DLLLOCAL static const char* getStaticTypeName() {
      return "method call";
   }

   DLLLOCAL void parseSetClassAndMethod(const QoreClass* n_qc, const QoreMethod* n_method) {
      assert(!qc);
      assert(!method);
      qc = n_qc;
      method = n_method;
      assert(qc);
      assert(method);
   }

   DLLLOCAL void setPseudo() {
      assert(!pseudo);
      pseudo = true;
   }

   DLLLOCAL bool isPseudo() const {
      return pseudo;
   }
};

class SelfFunctionCallNode : public AbstractMethodCallNode {
protected:
   NamedScope ns;
   bool is_copy;

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& returnTypeInfo);

public:
   DLLLOCAL SelfFunctionCallNode(const QoreProgramLocation& loc, char* n, QoreListNode* n_args) : AbstractMethodCallNode(loc, NT_SELF_CALL, n_args, parse_get_class()), ns(n), is_copy(false) {
   }

   DLLLOCAL SelfFunctionCallNode(const QoreProgramLocation& loc, char* n, QoreListNode* n_args, const QoreMethod* m, const QoreClass* n_qc) : AbstractMethodCallNode(loc, NT_SELF_CALL, n_args, n_qc, m), ns(n), is_copy(false) {
   }

   DLLLOCAL SelfFunctionCallNode(const QoreProgramLocation& loc, char* n, QoreListNode* n_args, const QoreClass* n_qc) : AbstractMethodCallNode(loc, NT_SELF_CALL, n_args, n_qc), ns(n), is_copy(false) {
   }

   DLLLOCAL SelfFunctionCallNode(const QoreProgramLocation& loc, NamedScope* n_ns, QoreListNode* n_args) : AbstractMethodCallNode(loc, NT_SELF_CALL, n_args, parse_get_class()), ns(n_ns), is_copy(false) {
   }

   DLLLOCAL SelfFunctionCallNode(const SelfFunctionCallNode& old, QoreListNode* n_args) : AbstractMethodCallNode(old, n_args), ns(old.ns), is_copy(old.is_copy) {
   }

   DLLLOCAL void parseInitCall(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& returnTypeInfo);

   DLLLOCAL virtual ~SelfFunctionCallNode() {
   }

   DLLLOCAL virtual const char* getTypeName() const {
      return "in-object method call";
   }

   DLLLOCAL virtual const char* getName() const {
      return ns.ostr;
   }

   using AbstractFunctionCallNode::evalImpl;
   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink* xsink) const;
   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const;

   DLLLOCAL AbstractQoreNode* makeReferenceNodeAndDeref();
};

class StaticMethodCallNode : public AbstractFunctionCallNode {
protected:
   NamedScope* scope;
   const QoreMethod* method;

   using AbstractFunctionCallNode::evalImpl;
   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);
   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return variant ? variant->parseGetReturnTypeInfo() : (method ? method->getFunction()->parseGetUniqueReturnTypeInfo() : 0);
   }

public:
   DLLLOCAL StaticMethodCallNode(const QoreProgramLocation& loc, NamedScope* n_scope, QoreListNode* args) : AbstractFunctionCallNode(loc, NT_STATIC_METHOD_CALL, args), scope(n_scope), method(0) {
   }

   // used when copying (for background expressions with processed arguments)
   DLLLOCAL StaticMethodCallNode(const QoreProgramLocation& loc, const QoreMethod* m, QoreListNode* args) : AbstractFunctionCallNode(loc, NT_STATIC_METHOD_CALL, args), scope(0), method(m) {
   }

   DLLLOCAL virtual ~StaticMethodCallNode() {
      delete scope;
   }

   DLLLOCAL const QoreMethod* getMethod() const {
      return method;
   }

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink* xsink) const {
      str.sprintf("static method call %s::%s() (%p)", method->getClass()->getName(), method->getName(), this);
      return 0;
   }

   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const {
      del = true;
      QoreString* rv = new QoreString();
      getAsString(*rv, foff, xsink);
      return rv;
   }

   DLLLOCAL virtual const char* getName() const {
      return method->getName();
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const {
      return getStaticTypeName();
   }

   DLLLOCAL AbstractQoreNode* makeReferenceNodeAndDeref();

   DLLLOCAL static const char* getStaticTypeName() {
      return "static method call";
   }

   DLLLOCAL NamedScope* takeScope() {
      NamedScope* rv = scope;
      scope = 0;
      return rv;
   }

   DLLLOCAL QoreListNode* takeArgs() {
      QoreListNode* rv = args;
      args = 0;
      return rv;
   }
};

#endif
