/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  FunctionCallNode.h

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

#ifndef _QORE_FUNCTIONCALLNODE_H

#define _QORE_FUNCTIONCALLNODE_H

#include <qore/Qore.h>

class FunctionCallBase {
protected:
   QoreListNode *args;
   const AbstractQoreFunctionVariant *variant;

public:
   DLLLOCAL FunctionCallBase(QoreListNode *n_args) : args(n_args), variant(0) {
   }

   DLLLOCAL FunctionCallBase(const FunctionCallBase &old) : args(old.args ? old.args->listRefSelf() : 0), variant(old.variant) {
   }

   DLLLOCAL ~FunctionCallBase() {
      if (args)
	 args->deref(0);
   }
   DLLLOCAL const QoreListNode *getArgs() const { return args; }
   DLLLOCAL int parseArgsVariant(LocalVar *oflag, int pflag, AbstractQoreFunction *func, const QoreTypeInfo *&returnTypeInfo);
   DLLLOCAL const AbstractQoreFunctionVariant *getVariant() const {
      return variant;
   }
};

class AbstractFunctionCallNode : public ParseNode, public FunctionCallBase {
protected:
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *) const = 0;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

   DLLLOCAL void doFlags(int64 flags) {
      if (flags & QC_RET_VALUE_ONLY)
         set_effect(false);
      if (flags & QC_CONSTANT)
         set_const_ok(true);
   }

public:
   DLLLOCAL AbstractFunctionCallNode(qore_type_t t, QoreListNode *n_args, bool needs_eval = true) : ParseNode(t, needs_eval), FunctionCallBase(n_args) {}

   DLLLOCAL AbstractFunctionCallNode(const AbstractFunctionCallNode &old) : ParseNode(old), FunctionCallBase(old) {
   }

   DLLLOCAL virtual ~AbstractFunctionCallNode() {
      // there could be an object here in the case of a background expression
      if (args) {
	 ExceptionSink xsink;
	 args->deref(&xsink);
	 args = 0;
      }
   }

   DLLLOCAL int parseArgs(LocalVar *oflag, int pflag, AbstractQoreFunction *func, const QoreTypeInfo *&returnTypeInfo) {
      int lvids = parseArgsVariant(oflag, pflag, func, returnTypeInfo);
      // clear "effect" flag if possible, only if QC_CONSTANT is set on the variant or function
      if (variant)
         doFlags(variant->getFlags());
      else if (func)
         doFlags(func->getUniqueFlags());
      return lvids;
   }

   DLLLOCAL virtual const char *getName() const = 0;
};

class FunctionCallNode : public AbstractFunctionCallNode {
protected:
   const AbstractQoreFunction *func;
   QoreProgram *pgm;
   char *c_str;

   // eval(): return value requires a deref(xsink)
      using AbstractFunctionCallNode::evalImpl;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *) const;

public:
   DLLLOCAL FunctionCallNode(const AbstractQoreFunction *f, QoreListNode *a, QoreProgram *n_pgm) : AbstractFunctionCallNode(NT_FUNCTION_CALL, a), func(f), pgm(n_pgm), c_str(0) {
   }

   // normal function call constructor
   DLLLOCAL FunctionCallNode(char *name, QoreListNode *a) : AbstractFunctionCallNode(NT_FUNCTION_CALL, a), func(0), pgm(0), c_str(name) {
   }
      
   DLLLOCAL virtual ~FunctionCallNode() {
      printd(5, "FunctionCallNode::~FunctionCallNode(): func=%p c_str=%p (%s) args=%p\n", func, c_str, c_str ? c_str : "n/a", args);
      if (c_str)
	 free(c_str);
   }

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const {
      return "function call";
   }

   DLLLOCAL QoreProgram *getProgram() const {
      return const_cast<QoreProgram *>(pgm);
   }

   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL virtual const char *getName() const {
      return func ? func->getName() : c_str;
   }

   DLLLOCAL const AbstractQoreFunction *getFunction() const {
      return func;
   }

   // FIXME: delete when unresolved function call node implemented properly
   DLLLOCAL char *takeName() {
      char *str = c_str;
      c_str = 0;
      return str;
   }

   // FIXME: delete when unresolved function call node implemented properly
   DLLLOCAL QoreListNode *take_args() {
      QoreListNode *rv = args;
      args = 0;
      return rv;
   }
};

class AbstractMethodCallNode : public AbstractFunctionCallNode {
protected:
   // if a method pointer can be resolved at parse time, then the class
   // is used to compare the runtime class; if they are equal, then no search
   // is needed
   const QoreClass *qc;
   const QoreMethod *method;

public:
   DLLLOCAL AbstractMethodCallNode(qore_type_t t, QoreListNode *n_args, const QoreClass *n_qc = 0) : AbstractFunctionCallNode(t, n_args), qc(n_qc), method(0) {
   }

   DLLLOCAL AbstractMethodCallNode(const AbstractMethodCallNode &old) : AbstractFunctionCallNode(old), qc(old.qc), method(old.method) {
   }

   DLLLOCAL AbstractQoreNode *exec(QoreObject *o, const char *cstr, ExceptionSink *xsink) const;

   DLLLOCAL const QoreClass *getClass() const {
      return qc;
   }

   DLLLOCAL const QoreMethod *getMethod() const {
      return method;
   }
};

class MethodCallNode : public AbstractMethodCallNode {
protected:
   char *c_str;

   using AbstractFunctionCallNode::evalImpl;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *) const {
      assert(false);
      return 0;
   }

public:
   DLLLOCAL MethodCallNode(char *name, QoreListNode *n_args) : AbstractMethodCallNode(NT_METHOD_CALL, n_args), c_str(name) {
      //printd(0, "MethodCallNode::MethodCallNode() this=%08p name='%s' args=%08p (len=%d)\n", this, c_str, args, args ? args->size() : -1);
   }

   DLLLOCAL MethodCallNode(const MethodCallNode &old, QoreListNode *n_args) : AbstractMethodCallNode(NT_METHOD_CALL, n_args), c_str(old.c_str ? strdup(old.c_str) : 0) {
   }

   DLLLOCAL virtual ~MethodCallNode() {
      if (c_str)
	 free(c_str);
   }
   
   DLLLOCAL AbstractQoreNode *exec(QoreObject *o, ExceptionSink *xsink) const {
      return AbstractMethodCallNode::exec(o, c_str, xsink);
   }

   DLLLOCAL virtual const char *getName() const {
      return c_str ? c_str : "copy";
   }

   DLLLOCAL const char *getRawName() const {
      return c_str;
   }

   // note that the class and method are set in Operator.cpp:check_op_object_func_ref()
   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      typeInfo = 0;
      lvids += parseArgs(oflag, pflag, 0, typeInfo);
      return this;
   }

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
      str.sprintf("'%s' method call (0x%08p)", getName(), this);
      return 0;
   }

   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const {
      del = true;
      QoreString *rv = new QoreString();
      getAsString(*rv, foff, xsink);
      return rv;
   }

   DLLLOCAL char *takeName() {
      char *rv = c_str;
      c_str = 0;
      return rv;
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const {
      return getStaticTypeName();
   }

   DLLLOCAL static const char *getStaticTypeName() {
      return "method call";
   }

   DLLLOCAL void parseSetClassAndMethod(const QoreClass *n_qc, const QoreMethod *n_method) {
      assert(!qc);
      assert(!method);
      qc = n_qc;
      method = n_method;
      assert(qc);
      assert(method);
   }
};

class SelfFunctionCallNode : public AbstractMethodCallNode {
protected:
   NamedScope ns;
   bool is_copy;

public:
   DLLLOCAL SelfFunctionCallNode(char *n, QoreListNode *n_args) : AbstractMethodCallNode(NT_SELF_CALL, n_args, getParseClass()), ns(n), is_copy(false) {
   }

   DLLLOCAL SelfFunctionCallNode(NamedScope *n_ns, QoreListNode *n_args) : AbstractMethodCallNode(NT_SELF_CALL, n_args, getParseClass()), ns(n_ns), is_copy(false) {
   }

   DLLLOCAL SelfFunctionCallNode(const SelfFunctionCallNode &old, QoreListNode *n_args) : AbstractMethodCallNode(old), ns(old.ns), is_copy(old.is_copy) {
   }

   DLLLOCAL virtual ~SelfFunctionCallNode() {
   }

   DLLLOCAL virtual const char *getTypeName() const {
      return "in-object method call";
   }

   DLLLOCAL virtual const char *getName() const {
      return ns.ostr;
   }

   using AbstractFunctionCallNode::evalImpl;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo);

   DLLLOCAL AbstractQoreNode *makeReferenceNodeAndDeref();
};

class StaticMethodCallNode : public AbstractFunctionCallNode {
protected:
   NamedScope *scope;
   const QoreMethod *method;

   using AbstractFunctionCallNode::evalImpl;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const {
      return method->eval(0, args, xsink);
   }

public:
   DLLLOCAL StaticMethodCallNode(NamedScope *n_scope, QoreListNode *args) : AbstractFunctionCallNode(NT_STATIC_METHOD_CALL, args), scope(n_scope), method(0) {
   }

   // used when copying (for background expressions with processed arguments)
   DLLLOCAL StaticMethodCallNode(const QoreMethod *m, QoreListNode *args) : AbstractFunctionCallNode(NT_STATIC_METHOD_CALL, args), scope(0), method(m) {
   }

   DLLLOCAL virtual ~StaticMethodCallNode() {
      delete scope;
   }

   DLLLOCAL const QoreMethod *getMethod() const {
      return method;
   }
      
   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
      str.sprintf("static method call %s::%s() (0x%08p)", method->getClass()->getName(), method->getName(), this);
      return 0;
   }

   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const {
      del = true;
      QoreString *rv = new QoreString();
      getAsString(*rv, foff, xsink);
      return rv;
   }

   DLLLOCAL virtual const char *getName() const {
      return method->getName();
   }

   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      QoreClass *qc = getRootNS()->parseFindScopedClassWithMethod(scope);
      if (!qc)
	 return this;

      method = qc->parseFindStaticMethodTree(scope->getIdentifier());
      if (!method) {
	 parseException("INVALID-METHOD", "class '%s' has no static method '%s()'", qc->getName(), scope->getIdentifier());
	 return this;
      }

      assert(method->isStatic());

      delete scope;
      scope = 0;

      if (method->parseIsPrivate()) {
	 const QoreClass *cls = getParseClass();
	 if (!cls || !cls->parseCheckHierarchy(qc)) {
	    parseException("PRIVATE-METHOD", "method %s::%s() is private and cannot be accessed outside of the class", qc->getName(), method->getName());
	    return this;
	 }
      }

      // check class capabilities against parse options
      if (qc->getDomain() & getProgram()->getParseOptions()) {
	 parseException("INVALID-METHOD", "class '%s' implements capabilities that are not allowed by current parse options", qc->getName());
	 return this;
      }

      lvids += parseArgs(oflag, pflag, method->getFunction(), typeInfo);
      return this;
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const {
      return getStaticTypeName();
   }

   DLLLOCAL static const char *getStaticTypeName() {
      return "static method call";
   }

   DLLLOCAL NamedScope *takeScope() {
      NamedScope *rv = scope;
      scope = 0;
      return rv;
   }

   DLLLOCAL QoreListNode *takeArgs() {
      QoreListNode *rv = args;
      args = 0;
      return rv;
   }
};

#endif
