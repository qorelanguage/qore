/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 CallReferenceNode.h
 
 Qore Programming Language
 
 Copyright 2003 - 2011 David Nichols
 
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

#ifndef _QORE_INTERN_FUNCTIONREFERENCENODE_H

#define _QORE_INTERN_FUNCTIONREFERENCENODE_H

class LocalUserCallReferenceNode;

class AbstractUnresolvedCallReferenceNode : public AbstractCallReferenceNode {
public:
   DLLLOCAL AbstractUnresolvedCallReferenceNode(bool n_needs_eval) : AbstractCallReferenceNode(n_needs_eval, false) {
   }
};

//! an unresolved call reference, only present temporarily in the parse tree
class UnresolvedProgramCallReferenceNode : public AbstractUnresolvedCallReferenceNode {
public:
   char *str;

   DLLLOCAL UnresolvedProgramCallReferenceNode(char *n_str);
   DLLLOCAL virtual ~UnresolvedProgramCallReferenceNode();
   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);
};

class UnresolvedCallReferenceNode : public UnresolvedProgramCallReferenceNode {
public:
   DLLLOCAL UnresolvedCallReferenceNode(char *n_str) : UnresolvedProgramCallReferenceNode(n_str) {
   }
   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);
};

//! a call reference to a static user method
class LocalStaticMethodCallReferenceNode : public ResolvedCallReferenceNode {
protected:
   const QoreMethod *method;

   // constructor for subclasses
   DLLLOCAL LocalStaticMethodCallReferenceNode(const QoreMethod *n_method, bool n_needs_eval) : ResolvedCallReferenceNode(n_needs_eval), method(n_method) {
   }

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;
   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const {
      return 0;
   }
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const {
      return 0;
   }
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const {
      return false;
   }
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const {
      return 0.0;
   }

public:
   DLLLOCAL LocalStaticMethodCallReferenceNode(const QoreMethod *n_method) : ResolvedCallReferenceNode(true), method(n_method) {
      //printd(5, "LocalStaticMethodCallReferenceNode::LocalStaticMethodCallReferenceNode() this=%p %s::%s() pgm=%p\n", this, method->getClass()->getName(), method->getName(), pgm);
   }
   DLLLOCAL virtual ~LocalStaticMethodCallReferenceNode() {
   }
   DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
      return LocalStaticMethodCallReferenceNode::is_equal_hard(v, xsink);
   }
   DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;

   DLLLOCAL virtual AbstractQoreFunction *getFunction() {
      return method ? method->getFunction() : 0;
   }
};

class StaticMethodCallReferenceNode : public LocalStaticMethodCallReferenceNode {
protected:   
   QoreProgram *pgm;

   DLLLOCAL virtual bool derefImpl(ExceptionSink *xsink);

public:
   DLLLOCAL StaticMethodCallReferenceNode(const QoreMethod *n_method, QoreProgram *n_pgm);
   DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
};

//! a call reference to a static user method
class LocalMethodCallReferenceNode : public LocalStaticMethodCallReferenceNode {
protected:
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

   DLLLOCAL LocalMethodCallReferenceNode(const QoreMethod *n_method, bool n_needs_eval) : LocalStaticMethodCallReferenceNode(n_method, n_needs_eval) {
      //printd(5, "LocalMethodCallReferenceNode::LocalStaticMethodCallReferenceNode() this=%p %s::%s() pgm=%p\n", this, method->getClass()->getName(), method->getName(), pgm);
   }

public:
   DLLLOCAL LocalMethodCallReferenceNode(const QoreMethod *n_method) : LocalStaticMethodCallReferenceNode(n_method) {
      //printd(5, "LocalMethodCallReferenceNode::LocalStaticMethodCallReferenceNode() this=%p %s::%s() pgm=%p\n", this, method->getClass()->getName(), method->getName(), pgm);
   }
   DLLLOCAL virtual ~LocalMethodCallReferenceNode() {
   }
   DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
      return LocalMethodCallReferenceNode::is_equal_hard(v, xsink);
   }
   DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
};

class MethodCallReferenceNode : public LocalMethodCallReferenceNode {
protected:   
   QoreProgram *pgm;

   DLLLOCAL virtual bool derefImpl(ExceptionSink *xsink);

public:
   DLLLOCAL MethodCallReferenceNode(const QoreMethod *n_method, QoreProgram *n_pgm);
   DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
};

//! a call reference to a user function from within the same QoreProgram object
class LocalUserCallReferenceNode : public ResolvedCallReferenceNode {
protected:
   const UserFunction *uf;

   // constructor for subclasses
   DLLLOCAL LocalUserCallReferenceNode(const UserFunction *n_uf, bool n_needs_eval);

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;
   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const {
      return 0;
   }
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const {
      return 0;
   }
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const {
      return false;
   }
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const {
      return 0.0;
   }

public:
   DLLLOCAL LocalUserCallReferenceNode(const UserFunction *n_uf);
   DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
   DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
      return LocalUserCallReferenceNode::is_equal_hard(v, xsink);
   }
   DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreFunction *getFunction() {
      return const_cast<UserFunction *>(uf);
   }
};

//! a call reference to a user function
class UserCallReferenceNode : public LocalUserCallReferenceNode {
protected:
   QoreProgram *pgm;

   DLLLOCAL virtual bool derefImpl(ExceptionSink *xsink);

public:
   DLLLOCAL UserCallReferenceNode(const UserFunction *n_uf, QoreProgram *n_pgm) : LocalUserCallReferenceNode(n_uf, false), pgm(n_pgm) {      
      assert(pgm);
      pgm->depRef();
   }
   DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
};

//! an unresolved static method call reference, only present temporarily in the parse tree
class UnresolvedStaticMethodCallReferenceNode : public AbstractUnresolvedCallReferenceNode {
protected:
   NamedScope *scope;

public:
   DLLLOCAL UnresolvedStaticMethodCallReferenceNode(NamedScope *n_scope);
   DLLLOCAL virtual ~UnresolvedStaticMethodCallReferenceNode();
   using AbstractQoreNode::deref;
   DLLLOCAL void deref() {
      if (ROdereference())
	 delete this;
   }
   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);
};

//! a call reference to a builtin function
class BuiltinCallReferenceNode : public ResolvedCallReferenceNode {
   const BuiltinFunction *bf;

public:
   DLLLOCAL BuiltinCallReferenceNode(const BuiltinFunction *n_bf);
   DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
   DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
      return BuiltinCallReferenceNode::is_equal_hard(v, xsink);
   }
   DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreFunction *getFunction();
};

class ImportedFunctionEntry;

//! a run-time call reference to a method of a particular object
class RunTimeObjectMethodReferenceNode : public ResolvedCallReferenceNode {
private:
   QoreObject *obj;
   char *method;

   DLLLOCAL virtual ~RunTimeObjectMethodReferenceNode();
   
public:
   DLLLOCAL RunTimeObjectMethodReferenceNode(QoreObject *n_obj, char *n_method);
   DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
   DLLLOCAL virtual QoreProgram *getProgram() const;
   DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
      return RunTimeObjectMethodReferenceNode::is_equal_hard(v, xsink);
   }
   DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

   DLLLOCAL virtual AbstractQoreFunction *getFunction() {
      // FIXME: implement type checking and method matching in ParseObjectMethodReferenceNode::parseInit()
      return 0;
   }
};

//! a run-time call reference to a method of a particular object where the method's class
// is known when the method reference node object is created
class RunTimeResolvedMethodReferenceNode : public ResolvedCallReferenceNode {
private:
   QoreObject *obj;
   const QoreMethod *method;

   DLLLOCAL virtual ~RunTimeResolvedMethodReferenceNode();

public:
   DLLLOCAL RunTimeResolvedMethodReferenceNode(QoreObject *n_obj, const QoreMethod *n_method);
   DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
   DLLLOCAL virtual QoreProgram *getProgram() const;
   DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
      return RunTimeResolvedMethodReferenceNode::is_equal_hard(v, xsink);
   }
   DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreFunction *getFunction() {
      return method ? method->getFunction() : 0;
   }
};

#endif
