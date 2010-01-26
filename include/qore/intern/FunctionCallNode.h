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

class ImportedFunctionCall {
  public:
   QoreProgram *pgm;
   const UserFunction *func;

   DLLLOCAL ImportedFunctionCall(QoreProgram *p, const UserFunction *f) { pgm = p; func = f; }
   DLLLOCAL AbstractQoreNode *eval(const QoreListNode *args, ExceptionSink *xsink) const;
};

class AbstractFunctionCallNode : public ParseNode {
protected:
   QoreListNode *args;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *) const = 0;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

   DLLLOCAL virtual bool existsUserParam(unsigned i) const {
      return true;
   }

public:
   DLLLOCAL AbstractFunctionCallNode(qore_type_t t, QoreListNode *n_args, bool needs_eval = true) : ParseNode(t, needs_eval), args(n_args) {}
   DLLLOCAL virtual ~AbstractFunctionCallNode() {
      // there could be an object here in the case of a background expression
      if (args) {
	 ExceptionSink xsink;
	 args->deref(&xsink);
      }
   }

   DLLLOCAL virtual const char *getName() const = 0;

   DLLLOCAL const QoreListNode *getArgs() const { return args; }

   DLLLOCAL int parseArgs(LocalVar *oflag, int pflag, ParamList *params) {
      int lvids = 0;
   
      if (params)
	 params->resolve();
   
      pflag &= ~PF_REFERENCE_OK;
      bool needs_eval = args ? args->needs_eval() : false;
   
      unsigned max = QORE_MAX(args ? args->size() : 0, params ? params->numParams() : 0);
   
      for (unsigned i = 0; i < max; ++i) {
	 AbstractQoreNode **n = args && i < args->size() ? args->get_entry_ptr(i) : 0;
	 const QoreTypeInfo *argTypeInfo = 0;
	 if (n && *n) {
	    if ((*n)->getType() == NT_REFERENCE) {
	       if (!existsUserParam(i))
		  parse_error("not enough parameters in '%s()' to accept reference expression", getName());
	       (*n) = (*n)->parseInit(oflag, pflag | PF_REFERENCE_OK, lvids, argTypeInfo);
	    }
	    else
	       (*n) = (*n)->parseInit(oflag, pflag, lvids, argTypeInfo);
	    if (!needs_eval && (*n)->needs_eval()) {
	       args->setNeedsEval();
	       needs_eval = true;
	    }
	 }
	 else
	    argTypeInfo = &nothingTypeInfo;
      
	 // check for compatible types
	 if (params) {
	    const QoreTypeInfo *paramTypeInfo = params->getParamTypeInfo(i);
	    // note that QoreTypeInfo::parseEqual() can be called when this = 0
	    if (!paramTypeInfo->parseEqual(argTypeInfo)) {
	       // raise a parse exception if parse exceptions are enabled
	       if (getProgram()->getParseExceptionSink()) {
		  QoreStringNode *desc = new QoreStringNode("argument ");
		  desc->sprintf("%d to '%s()' expects ", i + 1, getName());
		  paramTypeInfo->getThisType(*desc);
		  desc->concat(", but call supplies ");
		  argTypeInfo->getThisType(*desc);
		  getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
	       }
	    }
	 }
      }
   
      return lvids;
   }
};

// FIXME: split this into different function call subclasses
class FunctionCallNode : public AbstractFunctionCallNode {
  protected:
   // eval(): return value requires a deref(xsink)
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *) const;

   DLLLOCAL virtual bool existsUserParam(unsigned i) const;

  public:
   union uFCall {
      const AbstractQoreFunction *func;
      ImportedFunctionCall *ifunc;
      char *c_str;
   } f;
   int ftype;

   //DLLLOCAL FunctionCallNode(const UserFunction *u, QoreListNode *a);
   //DLLLOCAL FunctionCallNode(const BuiltinFunction *b, QoreListNode *a);
   DLLLOCAL FunctionCallNode(const AbstractQoreFunction *f, QoreListNode *a);

   // normal function call constructor
   DLLLOCAL FunctionCallNode(char *name, QoreListNode *a);
      
   DLLLOCAL FunctionCallNode(QoreProgram *p, const UserFunction *u, QoreListNode *a);

   DLLLOCAL virtual ~FunctionCallNode();

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const;

   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL virtual const char *getName() const;

   DLLLOCAL AbstractQoreNode *parseMakeNewObject();
   DLLLOCAL int getFunctionType() const;

   // FIXME: delete when unresolved function call node implemented properly
   DLLLOCAL char *takeName();

   // FIXME: delete when unresolved function call node implemented properly
   DLLLOCAL QoreListNode *take_args() {
      QoreListNode *rv = args;
      args = 0;
      return rv;
   }
};

class SelfFunctionCallNode : public AbstractFunctionCallNode {
protected:
   char *name;
   NamedScope *ns;
   const QoreMethod *func;

   DLLLOCAL char *takeName() {
      char *n = name;
      name = 0;
      return n;
   }
   DLLLOCAL NamedScope *takeNScope() {
      NamedScope *rns = ns;
      ns = 0;
      return rns;
   }

public:
   DLLLOCAL SelfFunctionCallNode(char *n, QoreListNode *n_args) : AbstractFunctionCallNode(NT_SELF_CALL, n_args), name(n), ns(0), func(0) {
   }
   DLLLOCAL SelfFunctionCallNode(NamedScope *n_ns, QoreListNode *n_args) : AbstractFunctionCallNode(NT_SELF_CALL, n_args), name(0), ns(n_ns), func(0) {}
   DLLLOCAL SelfFunctionCallNode(const QoreMethod *f, QoreListNode *n_args) : AbstractFunctionCallNode(NT_SELF_CALL, n_args), name(0), ns(0), func(f) {}
   DLLLOCAL virtual ~SelfFunctionCallNode() {
      if (name)
	 free(name);
      delete ns;
   }
   DLLLOCAL virtual const char *getTypeName() const {
      return "in-object method call";
   }
   DLLLOCAL virtual const char *getName() const {
      return name ? name : (ns ? ns->ostr : func->getName());
   }
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo);

   DLLLOCAL const QoreMethod *getMethod() const {
      return func;
   }
   DLLLOCAL AbstractQoreNode *makeReferenceNodeAndDeref();
};

class MethodCallNode : public AbstractFunctionCallNode {
protected:
   char *c_str;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *) const {
      assert(false);
      return 0;
   }

public:
   DLLLOCAL MethodCallNode(char *name, QoreListNode *n_args) : AbstractFunctionCallNode(NT_METHOD_CALL, n_args), c_str(name) {
      //printd(0, "MethodCallNode::MethodCallNode() this=%08p name='%s' args=%08p (len=%d)\n", this, c_str, args, args ? args->size() : -1);
   }

   DLLLOCAL virtual ~MethodCallNode() {
      if (c_str)
	 free(c_str);
   }

   DLLLOCAL virtual const char *getName() const {
      return c_str ? c_str : "copy";
   }

   DLLLOCAL const char *getRawName() const {
      return c_str;
   }

   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      lvids += parseArgs(oflag, pflag, 0);
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
};

class StaticMethodCallNode : public AbstractFunctionCallNode {
  protected:
   NamedScope *scope;
   const QoreMethod *method;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const {
      return method->eval(0, args, xsink);
   }

   DLLLOCAL virtual bool existsUserParam(unsigned i) const {
      return method->existsUserParam(i);
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

      if (method->isPrivate()) {
	 const QoreClass *cls = getParseClass();
	 if (!cls || !cls->parseCheckHierarchy(qc)) {
	    parseException("PRIVATE-METHOD", "method %s::%s() is private and cannot be accessed outside of the class", qc->getName(), method->getName());
	    return this;
	 }
      }

      // check class capabilities against parse options
      if (qc->getDomain() & getProgram()->getParseOptions()) {
	 parseException("class '%s' implements capabilities that are not allowed by current parse options", qc->getName());
	 return this;
      }

      lvids += parseArgs(oflag, pflag, method->getParams());
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
