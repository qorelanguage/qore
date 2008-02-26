/*
 FunctionReferenceNode.h
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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

//! an unresolved call reference, only present temporarily in the parse tree
class UnresolvedFunctionReferenceNode : public AbstractFunctionReferenceNode
{
   protected:

   public:
      char *str;
      
      DLLLOCAL UnresolvedFunctionReferenceNode(char *n_str);
      DLLLOCAL virtual ~UnresolvedFunctionReferenceNode();
      DLLLOCAL AbstractFunctionReferenceNode *resolve();
      DLLLOCAL void deref();
};

//! a call reference to a user function
class UserFunctionReferenceNode : public ResolvedFunctionReferenceNode
{
      UserFunction *uf;
      QoreProgram *pgm;

   protected:
      DLLLOCAL virtual bool derefImpl(ExceptionSink *xsink);

   public:
      DLLLOCAL UserFunctionReferenceNode(class UserFunction *n_uf, class QoreProgram *n_pgm);
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL virtual QoreProgram *getProgram() const;
};

//! a call reference to a user function from within the same QoreProgram object
class StaticUserFunctionReferenceNode : public ResolvedFunctionReferenceNode
{
      UserFunction *uf;
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
      DLLLOCAL StaticUserFunctionReferenceNode(class UserFunction *n_uf, class QoreProgram *n_pgm);
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;

      //! returns true
      //DLLLOCAL virtual bool needs_eval() const;
};

//! a call reference to a builtin function
class BuiltinFunctionReferenceNode : public ResolvedFunctionReferenceNode
{
      const class BuiltinFunction *bf;

   public:
      DLLLOCAL BuiltinFunctionReferenceNode(const class BuiltinFunction *n_bf);
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
};

//! a call reference to an imported function
class ImportedFunctionReferenceNode :  public ResolvedFunctionReferenceNode
{
      class ImportedFunctionCall *ifunc;

   public:
      DLLLOCAL ImportedFunctionReferenceNode(class ImportedFunctionCall *n_ifunc);
      DLLLOCAL virtual ~ImportedFunctionReferenceNode();
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL virtual QoreProgram *getProgram() const;
};

//! a run-time call reference to a method of a particular object
class RunTimeObjectMethodReferenceNode : public ResolvedFunctionReferenceNode
{
   private:
      class QoreObject *obj;
      char *method;

      DLLLOCAL virtual ~RunTimeObjectMethodReferenceNode();
   
   public:
      DLLLOCAL RunTimeObjectMethodReferenceNode(class QoreObject *n_obj, char *n_method);
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual QoreProgram *getProgram() const;
};

//! a run-time call reference to a method of a particular object where the method's class is explicitly specified
class RunTimeObjectScopedMethodReferenceNode : public ResolvedFunctionReferenceNode
{
   private:
      class QoreObject *obj;
      const class QoreMethod *method;

      DLLLOCAL virtual ~RunTimeObjectScopedMethodReferenceNode();

   public:
      DLLLOCAL RunTimeObjectScopedMethodReferenceNode(class QoreObject *n_obj, const class QoreMethod *n_method);
      DLLLOCAL virtual AbstractQoreNode *exec(const class QoreListNode *args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual QoreProgram *getProgram() const;
};

#endif
