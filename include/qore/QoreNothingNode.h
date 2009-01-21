/*
  QoreNothingNode.h
  
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

#ifndef _QORE_QORENOTHINGNODE_H

#define _QORE_QORENOTHINGNODE_H

#include <qore/AbstractQoreNode.h>

/** @file QoreNothingNode.h
    defines the QoreNothingNode class
*/

//! Qore's SQL "NOTHING" parse tree/value type, not-referenced counted, not dynamically allocated
/** This class cannot be instantiated; there will only be one single QoreNothingNode object instantiated and used
    everywhere in the Qore library.  Use the nothing() function or simply &Nothing to acquire a pointer to an 
    object of this class.
    This value can be represented in Qore code as the keyword "NOTHING"
    @note Qore's "NULL" is not equal to "NOTHING"
    @note in C++ code, use the is_nothing() function to test an AbstractQoreNode* to see if it's "NOTHING" (because also a null pointer is equivalent to NOTHING)
    @see QoreNullNode
 */
class QoreNothingNode : public UniqueValueQoreNode
{
   protected:
      //! this function is never called for this type
      /** @see AbstractQoreNode::evalImpl()
       */
      DLLLOCAL AbstractQoreNode *evalImpl(class ExceptionSink *xsink) const;

   public:
      DLLEXPORT QoreNothingNode();

      DLLEXPORT virtual ~QoreNothingNode();

      //! concatenate "<NOTHING>" to an existing QoreString
      /** used for %n and %N printf formatting
	  @param str the string representation of the type will be concatenated to this QoreString reference
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return -1 for exception raised, 0 = OK
      */
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      //! returns a QoreString with the text: "<NOTHING>"
      /** used for %n and %N printf formatting
	  @param del is always set to true for this implementation of the function, meaning that the returned QoreString pointer should be deleted
	  @param foff for multi-line formatting offset, -1 = no line breaks (ignored by this version of the function)
	  @param xsink ignored by this version of the function
	  NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
	  @see QoreNodeAsStringHelper
      */
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      //! tests for equality with possible type conversion (soft compare)
      /** since no type can be implicitly converted to NOTHING, this comparison is the same as is_equal_hard() for QoreNothingNode
	  @param v the value to compare
	  @param xsink ignored for this version of the function
       */
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! tests for equality without type conversions (hard compare)
      /**
	 @param v the value to compare
	  @param xsink ignored for this version of the function
       */
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;

      DLLLOCAL static const char *getStaticTypeName()
      {
	 return "nothing";
      }
};

//! the global and unique NOTHING object in Qore
DLLEXPORT extern QoreNothingNode Nothing;

//! returns a pointer to Nothing
static inline QoreNothingNode *nothing()
{
   return &Nothing;
}

#endif
