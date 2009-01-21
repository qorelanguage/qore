/*
 CallReferenceNode.h
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
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

class StaticUserCallReferenceNode;
class UserCallReferenceNode;

class AbstractUnresolvedCallReferenceNode : public AbstractCallReferenceNode {
   public:
      DLLLOCAL AbstractUnresolvedCallReferenceNode(bool n_needs_eval, bool n_there_can_be_only_one) : AbstractCallReferenceNode(n_needs_eval, n_there_can_be_only_one) {
      }
      DLLLOCAL virtual AbstractCallReferenceNode *resolve() = 0;
};

//! an unresolved call reference, only present temporarily in the parse tree
class UnresolvedCallReferenceNode : public AbstractUnresolvedCallReferenceNode {
   public:
      char *str;

      DLLLOCAL UnresolvedCallReferenceNode(char *n_str);
      DLLLOCAL virtual ~UnresolvedCallReferenceNode();
      DLLLOCAL virtual AbstractCallReferenceNode *resolve();
      DLLLOCAL void deref();
};

//! a call reference to a user function
class UserCallReferenceNode : public ResolvedCallReferenceNode {
   friend class StaticUserCallReferenceNode;

   private:
      const UserFunction *uf;
      QoreProgram *pgm;

   protected:
      DLLLOCAL virtual bool derefImpl(ExceptionSink *xsink);

   public:
      DLLLOCAL UserCallReferenceNode(const UserFunction *n_uf, QoreProgram *n_pgm);
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL virtual QoreProgram *getProgram() const;

      DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
	 return UserCallReferenceNode::is_equal_hard(v, xsink);
      }
      DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;
};

//! an unresolved static method call reference, only present temporarily in the parse tree
class UnresolvedStaticMethodCallReferenceNode : public AbstractUnresolvedCallReferenceNode {
   protected:
      NamedScope *scope;

   public:
      DLLLOCAL UnresolvedStaticMethodCallReferenceNode(NamedScope *n_scope);
      DLLLOCAL virtual ~UnresolvedStaticMethodCallReferenceNode();
      DLLLOCAL virtual AbstractCallReferenceNode *resolve();
      DLLLOCAL void deref();
};

//! a call reference to a user function from within the same QoreProgram object
class StaticUserCallReferenceNode : public ResolvedCallReferenceNode {
   friend class UserCallReferenceNode;
    
      const UserFunction *uf;
      QoreProgram *pgm;

   protected:
      DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

      //! optionally evaluates the argument
      /** return value requires a deref(xsink) if needs_deref is true
	  @see AbstractQoreNode::eval()
      */
      DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, class ExceptionSink *xsink) const;

      DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

   public:
      DLLLOCAL StaticUserCallReferenceNode(const UserFunction *n_uf, class QoreProgram *n_pgm);
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;

      DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
	 return StaticUserCallReferenceNode::is_equal_hard(v, xsink);
      }

      DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;
};

//! a call reference to a builtin function
class BuiltinCallReferenceNode : public ResolvedCallReferenceNode {
      const BuiltinFunction *bf;

   public:
      DLLLOCAL BuiltinCallReferenceNode(const BuiltinFunction *n_bf);
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
      {
	 return BuiltinCallReferenceNode::is_equal_hard(v, xsink);
      }
      DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;
};

class ImportedFunctionCall;

//! a call reference to an imported function
class ImportedCallReferenceNode :  public ResolvedCallReferenceNode
{
      ImportedFunctionCall *ifunc;

   public:
      DLLLOCAL ImportedCallReferenceNode(ImportedFunctionCall *n_ifunc);
      DLLLOCAL virtual ~ImportedCallReferenceNode();
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL virtual QoreProgram *getProgram() const;
      DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
      {
	 return ImportedCallReferenceNode::is_equal_hard(v, xsink);
      }
      DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;
};

//! a run-time call reference to a method of a particular object
class RunTimeObjectMethodReferenceNode : public ResolvedCallReferenceNode
{
   private:
      QoreObject *obj;
      char *method;

      DLLLOCAL virtual ~RunTimeObjectMethodReferenceNode();
   
   public:
      DLLLOCAL RunTimeObjectMethodReferenceNode(class QoreObject *n_obj, char *n_method);
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual QoreProgram *getProgram() const;
      DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
      {
	 return RunTimeObjectMethodReferenceNode::is_equal_hard(v, xsink);
      }
      DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;
};

//! a run-time call reference to a method of a particular object where the method's class is explicitly specified
class RunTimeObjectScopedMethodReferenceNode : public ResolvedCallReferenceNode
{
   private:
      QoreObject *obj;
      const class QoreMethod *method;

      DLLLOCAL virtual ~RunTimeObjectScopedMethodReferenceNode();

   public:
      DLLLOCAL RunTimeObjectScopedMethodReferenceNode(class QoreObject *n_obj, const class QoreMethod *n_method);
      DLLLOCAL virtual AbstractQoreNode *exec(const class QoreListNode *args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual QoreProgram *getProgram() const;
      DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
      {
	 return RunTimeObjectScopedMethodReferenceNode::is_equal_hard(v, xsink);
      }
      DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;
};

#endif
