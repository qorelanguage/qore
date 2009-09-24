/*
  BinaryNode.h

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

#ifndef _QORE_BINARYNODE_H

#define _QORE_BINARYNODE_H

#include <qore/AbstractQoreNode.h>

//! holds arbitrary binary data
/** this class is implemented simply as a pointer and a length indicator
 */
class BinaryNode : public SimpleValueQoreNode
{
   private:
      //! pointer to memory owned by the object
      void *ptr;
      //! size of the memory block owned by the object
      qore_size_t len;

      // not yet implemented
      DLLLOCAL BinaryNode(const BinaryNode&);
      DLLLOCAL BinaryNode& operator=(const BinaryNode&);

   protected:
      //! frees and memory owned by the object
      DLLEXPORT virtual ~BinaryNode();

   public:
      //! creates the object
      /** @param p a pointer to the memory, the BinaryNode object takes over ownership of this pointer
	  @param size the byte length of the memory
       */
      DLLEXPORT BinaryNode(void *p = 0, qore_size_t size = 0);

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

      //! returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;

      DLLLOCAL static const char *getStaticTypeName()
      {
	 return "binary";
      }

      //! returns 0 = equal, 1 = not equal
      DLLEXPORT int compare(const BinaryNode *obj) const;

      //! returns the number of bytes in the object
      DLLEXPORT qore_size_t size() const;

      //! returns a copy of the object
      /**
	 @return a copy of the current object
       */
      DLLEXPORT class BinaryNode *copy() const;
      
      //! returns the pointer to the data
      DLLEXPORT const void *getPtr() const;
      
      //! resizes the object and appends a copy of the data passed to the object
      DLLEXPORT void append(const void *nptr, qore_size_t size);

      //! resizes the object and appends a copy of the data passed to the object
      DLLEXPORT void append(const BinaryNode *b);

      //! resizes the object and appends a copy of the data passed to the object
      DLLEXPORT void append(const BinaryNode &b);

      //! resizes the object and prepends a copy of the data passed to the beginning of the object
      DLLEXPORT void prepend(const void *nptr, qore_size_t size);

      //! returns the data being managed and leaves this object empty
      /**
	 @return the data being managed (leaves this object empty)
	 @note it would be a grevious error to call this function on an object
	 with a reference_count > 1 (i.e. is_unique() is false)
       */
      DLLEXPORT void *giveBuffer();

      //! pre-allocates a buffer of a certain size
      /** This function can be used to write data directly to a new BinaryNode object.
	  This call can be made more than once, subsequent calls will cause realloc() 
	  to be called on the buffer which can be used to extend the buffer size.
	  @param size the number of bytes to pre-allocate
	  @return 0 for OK, -1 for error (memory could not be allocated)
       */
      DLLEXPORT int preallocate(qore_size_t size);

      //! sets the buffer size after preallocation
      /** This function is designed to be used with BinaryNode::preallocate().  The
	  size to be set must be less than the currently allocated size.
	  @param size the size of the BinaryNode to set
	  @return 0 for OK, -1 for error (size > current size)
       */
      DLLEXPORT int setSize(qore_size_t size);
};

#endif // _QORE_BINARYOBJECT_H
