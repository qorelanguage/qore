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

#ifndef _QORE_FUNCTIONREFERENCENODE_H

#define _QORE_FUNCTIONREFERENCENODE_H

// cannot be a ParseNode or SimpleQoreNode because we require deref(xsink)
class AbstractFunctionReferenceNode : public AbstractQoreNode
{
   public:
      DLLLOCAL AbstractFunctionReferenceNode();
      DLLLOCAL virtual ~AbstractFunctionReferenceNode();

      // parse types should never be copied
      DLLLOCAL virtual class AbstractQoreNode *realCopy() const;
      DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;
      DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;
      DLLLOCAL virtual bool needs_eval() const;
      DLLLOCAL virtual bool is_value() const;
      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      // the ExceptionSink is only needed for QoreObject where a method may be executed
      // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
      // returns -1 for exception raised, 0 = OK
      DLLLOCAL virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;
      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;
      // returns the data type
      DLLLOCAL virtual const QoreType *getType() const;
      // returns the type name as a c string
      DLLLOCAL virtual const char *getTypeName() const;
      DLLLOCAL virtual int64 bigIntEval(ExceptionSink *xsink) const;
      DLLLOCAL virtual int integerEval(ExceptionSink *xsink) const;
      DLLLOCAL virtual bool boolEval(ExceptionSink *xsink) const;
      DLLLOCAL virtual double floatEval(ExceptionSink *xsink) const;
};

class ResolvedFunctionReferenceNode : public AbstractFunctionReferenceNode
{
   public:
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual QoreProgram *getProgram() const
      {
	 return 0;
      }
};

class UnresolvedFunctionReferenceNode : public AbstractFunctionReferenceNode
{
   public:
      char *str;
      
      DLLLOCAL UnresolvedFunctionReferenceNode(char *n_str);
      DLLLOCAL virtual ~UnresolvedFunctionReferenceNode();
      DLLLOCAL AbstractFunctionReferenceNode *resolve();
      DLLLOCAL void deref();
      DLLLOCAL void deref(ExceptionSink *xsink);
};

class UserFunctionReferenceNode : public ResolvedFunctionReferenceNode
{
      UserFunction *uf;
      QoreProgram *pgm;

   public:
      //DLLLOCAL UserFunctionReferenceNode(class UserFunction *n_uf);
      DLLLOCAL UserFunctionReferenceNode(class UserFunction *n_uf, class QoreProgram *n_pgm);
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL virtual QoreProgram *getProgram() const;
      DLLLOCAL virtual void deref(ExceptionSink *xsink);
};

class StaticUserFunctionReferenceNode : public ResolvedFunctionReferenceNode
{
      UserFunction *uf;
      QoreProgram *pgm;

   public:
      DLLLOCAL StaticUserFunctionReferenceNode(class UserFunction *n_uf, class QoreProgram *n_pgm);
      DLLLOCAL virtual AbstractQoreNode *eval(ExceptionSink *xsink) const;
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
};

class BuiltinFunctionReferenceNode : public ResolvedFunctionReferenceNode
{
      class BuiltinFunction *bf;

   public:
      DLLLOCAL BuiltinFunctionReferenceNode(class BuiltinFunction *n_bf);
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
};

class ImportedFunctionReferenceNode :  public ResolvedFunctionReferenceNode
{
      class ImportedFunctionCall *ifunc;

   public:
      DLLLOCAL ImportedFunctionReferenceNode(class ImportedFunctionCall *n_ifunc);
      DLLLOCAL virtual ~ImportedFunctionReferenceNode();
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL virtual QoreProgram *getProgram() const;
};

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
