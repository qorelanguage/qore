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

#ifndef _QORE_FUNCTIONREFERENCENODE_H

#define _QORE_FUNCTIONREFERENCENODE_H

//! base class for call references, reference-counted, dynamically allocated only
/** cannot be a ParseNode or SimpleQoreNode because we require deref(xsink)
 */
class AbstractCallReferenceNode : public AbstractQoreNode
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
      //! this function should never be called for function references; this function should never be called directly
      /** in debug mode this function calls assert(false)
       */
      DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

      //! this function should never be called for function references; this function should never be called directly
      /** in debug mode this function calls assert(false)
       */
      DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

      //! this function should never be called for function references; this function should never be called directly
      /** in debug mode this function calls assert(false)
       */
      DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;

      //! this function should never be called for function references; this function should never be called directly
      /** in debug mode this function calls assert(false)
       */
      DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;

      //! this function should never be called for function references; this function should never be called directly
      /** in debug mode this function calls assert(false)
       */
      DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;

      //! this function should never be called for function references; this function should never be called directly
      /** in debug mode this function calls assert(false)
       */
      DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

      //! protected constructor for subclasses that are not reference-counted
      DLLLOCAL AbstractCallReferenceNode(bool n_needs_eval, bool n_there_can_be_only_one, qore_type_t n_type = NT_FUNCREF);

   public:
      DLLLOCAL AbstractCallReferenceNode(bool n_needs_eval = false, qore_type_t n_type = NT_FUNCREF);

      DLLLOCAL virtual ~AbstractCallReferenceNode();

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

      //! returns the type name as a c string
      DLLLOCAL virtual const char *getTypeName() const;

      DLLLOCAL static const char *getStaticTypeName()
      {
	 return "call reference";
      }
};

//! base class for resolved call references
class ResolvedCallReferenceNode : public AbstractCallReferenceNode
{
   public:
      //! constructor is not exported outside the library
      DLLLOCAL ResolvedCallReferenceNode(bool n_needs_eval = false, qore_type_t n_type = NT_FUNCREF);

      //! pure virtual function for executing the function reference
      /** executes the function reference and returns the value returned
	 @param args the arguments to the function
	 @param xsink any Qore-language exception thrown (and not handled) will be added here
	 @return a pointer to an AbstractQoreNode, the caller owns the reference count returned (can also be 0)
       */
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const = 0;

      //! returns a pointer to the QoreProgram object associated with this reference (can be 0)
      /** this function is not exported in the library's public interface
	  @return a pointer to the QoreProgram object associated with this reference (can be 0)
       */
      DLLLOCAL virtual QoreProgram *getProgram() const;

      DLLLOCAL ResolvedCallReferenceNode *refRefSelf() const
      {
	 ref();
	 return const_cast<ResolvedCallReferenceNode *>(this);
      }
};


#endif
