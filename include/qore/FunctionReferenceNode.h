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

//! base class for call references, reference-counted, dynamically allocated only
/** cannot be a ParseNode or SimpleQoreNode because we require deref(xsink)
 */
class AbstractFunctionReferenceNode : public AbstractQoreNode
{
   private:
      //! this function will never be executed for parse types; this function should never be called directly
      /** in debug mode this function calls assert(false)
       */
      DLLLOCAL virtual class AbstractQoreNode *realCopy() const;

      //! this function will never be executed for parse types; this function should never be called directly
      /** in debug mode this function calls assert(false)
       */
      DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! this function will never be executed for parse types; this function should never be called directly
      /** in debug mode this function calls assert(false)
       */
      DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

   protected:
      DLLLOCAL AbstractFunctionReferenceNode(bool n_there_can_be_only_one);

   public:
      DLLLOCAL AbstractFunctionReferenceNode();
      DLLLOCAL virtual ~AbstractFunctionReferenceNode();

      //! returns true
      DLLLOCAL virtual bool needs_eval() const;

      //! returns false
      DLLLOCAL virtual bool is_value() const;

      //! concatenate the verbose string representation of the value to an existing QoreString
      /** used for %n and %N printf formatting
	  @param str the string representation of the type will be concatenated to this QoreString reference
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink not used by this implementation of the function
	  @return -1 for exception raised, 0 = OK
      */
      DLLLOCAL virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      //! returns a QoreString giving the verbose string representation of the value
      /** used for %n and %N printf formatting
	  @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink not used by this implementation of the function
	  NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
	  @see QoreNodeAsStringHelper
      */
      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      //! returns the data type
      DLLLOCAL virtual const QoreType *getType() const;

      //! returns the type name as a c string
      DLLLOCAL virtual const char *getTypeName() const;

      DLLLOCAL virtual int64 bigIntEval(ExceptionSink *xsink) const;
      DLLLOCAL virtual int integerEval(ExceptionSink *xsink) const;
      DLLLOCAL virtual bool boolEval(ExceptionSink *xsink) const;
      DLLLOCAL virtual double floatEval(ExceptionSink *xsink) const;

      DLLLOCAL static const char *getStaticTypeName()
      {
	 return "function reference";
      }
};

//! base class for resolved call references
class ResolvedFunctionReferenceNode : public AbstractFunctionReferenceNode
{
   public:
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual QoreProgram *getProgram() const
      {
	 return 0;
      }
};

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

   public:
      DLLLOCAL StaticUserFunctionReferenceNode(class UserFunction *n_uf, class QoreProgram *n_pgm);
      DLLLOCAL virtual AbstractQoreNode *eval(ExceptionSink *xsink) const;
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;
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
