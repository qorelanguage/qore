/*
  ReferenceNode.h
  
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

#ifndef _QORE_REFERENCENODE_H

#define _QORE_REFERENCENODE_H 

#include <qore/AbstractQoreNode.h>

//! parse type: reference to a lvalue expression
/** This type could be passed to a builtin function.  To get and set the value of the reference, 
    use the ReferenceHelper class.  To create a reference argument to pass to a user or builtin
    function, use the ReferenceArgumentHelper class.
    @see ReferenceHelper
    @see ReferenceArgumentHelper
 */
class ReferenceNode : public SimpleValueQoreNode
{
   protected:
      //! lvalue expression for reference
      AbstractQoreNode *lvexp;

      //! frees all memory and destroys the object
      DLLEXPORT virtual ~ReferenceNode();

   public:
      //! creates the ReferenceNode object with the given lvalue expression
      /** @param exp must be a parse expression for an lvalue
       */
      DLLLOCAL ReferenceNode(AbstractQoreNode *exp);

      //! concatenate the verbose string representation of the value to an existing QoreString
      /** used for %n and %N printf formatting
	  @param str the string representation of the type will be concatenated to this QoreString reference
	  @param foff for multi-line formatting offset, -1 = no line breaks (ignored in this version of the function)
	  @param xsink ignored in this version of the function
	  @return this implementation of the function always returns 0 for no error raised
      */
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      //! returns a QoreString giving the verbose string representation of the value
      /** Used for %n and %N printf formatting.  Do not call this function directly; use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead
	  @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
	  @param foff for multi-line formatting offset, -1 = no line breaks (ignored in this version of the function)
	  @param xsink ignored in this version of the function
	  @see QoreNodeAsStringHelper
      */
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      //! returns a copy of the object
      DLLEXPORT virtual AbstractQoreNode *realCopy() const;

      //! always returns false
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! always returns false
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;

      //! returns the lvalue expression for this object
      /** @return the lvalue expression for this object
       */
      DLLLOCAL AbstractQoreNode *getExpression() const
      {
	 return lvexp;
      }

      //! returns a pointer to the lvalue expression
      DLLLOCAL AbstractQoreNode **getExpressionPtr();
};

#endif
