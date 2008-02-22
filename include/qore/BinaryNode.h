/*
  BinaryNode.h

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

#ifndef _QORE_BINARYNODE_H

#define _QORE_BINARYNODE_H

#include <qore/AbstractQoreNode.h>

//! holds arbitrary binary data
/** this class is implemented simply as a pointer and a length indicator
 */
class BinaryNode : public SimpleQoreNode
{
   private:
      void *ptr;
      unsigned long len;

      // not yet implemented
      DLLLOCAL BinaryNode(const BinaryNode&);
      DLLLOCAL BinaryNode& operator=(const BinaryNode&);

   protected:
      DLLEXPORT virtual ~BinaryNode();

   public:
      DLLEXPORT BinaryNode(void *p = NULL, unsigned long size = 0);

      //! concatenate the verbose string representation of the value to an existing QoreString
      /** used for %n and %N printf formatting
	  @param str the string representation of the type will be concatenated to this QoreString reference
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return -1 for exception raised, 0 = OK
      */
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      //! returns a QoreString giving the verbose string representation of the List (including all contained values for container types)
      /** used for %n and %N printf formatting
	  @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
	  @see QoreNodeAsStringHelper
      */
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

      //! tests for equality
      /** this function does not throw a Qore-language exception with the BinaryNode class
	  @param v the value to compare
	  @param xsink is not used in this implementation of the function
       */
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! tests for equality
      /** this function does not throw a Qore-language exception with the BinaryNode class
	  @param v the value to compare
	  @param xsink is not used in this implementation of the function
       */
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! returns the data type
      DLLEXPORT virtual const QoreType *getType() const;

      //! returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;

      DLLLOCAL static const char *getStaticTypeName()
      {
	 return "binary";
      }

      //! returns 0 = equal, 1 = not equal
      DLLEXPORT int compare(const BinaryNode *obj) const;

      //! returns the number of bytes in the object
      DLLEXPORT unsigned long size() const;

      //! returns a copy of the object
      /**
	 @return a copy of the current object
       */
      DLLEXPORT class BinaryNode *copy() const;
      
      //! returns the pointer to the data
      DLLEXPORT const void *getPtr() const;
      
      //! resizes the object and appends a copy of the data passed to the object
      DLLEXPORT void append(const void *nptr, unsigned long size);

      //! resizes the object and appends a copy of the data passed to the object
      DLLEXPORT void append(const BinaryNode *b);

      //! returns the data being managed and leaves this object empty
      /**
	 @return the data being managed (leaves this object empty)
       */
      DLLEXPORT void *giveBuffer();
};

#endif // _QORE_BINARYOBJECT_H
