/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AbstractInputStream.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_ABSTRACTINPUTSTREAM_H
#define _QORE_ABSTRACTINPUTSTREAM_H

#include "qore/AbstractPrivateData.h"

DLLEXPORT extern QoreClass* QC_ABSTRACTINPUTSTREAM;

/**
 * @brief Base class for private data of input stream implementations.
 *
 * Qore's AbstractInputStream class can be subclassed either in Qore or in C++. Its methods can also be called
 * either from Qore or C++, which gives us for possible situations.
 *   1. a subclass of Qore::AbstractInputStream is implemented in the C++ Language
 *      - the private data of the resulting object is an instance of a class derived from this class that overrides
 *        bulkRead() and read() with the actual implementation of the read functions
 *      a) C++ code reads bytes by retrieving the private data of the object and calling bulkRead()
 *         - normal C++ virtual method dispatching is used here
 *      b) Qore code reads bytes by calling bulkRead() on the object which is implemented by bulkReadToBinary() (defined
 *         in this class) which in turn calls bulkRead() on this class, again using normal C++ virtual method dispatch,
 *         and wraps the result in a `binary` value
 *      For this to work, the C++ subclasses of Qore::AbstractInputStream must be final.
 *   2. a subclass of Qore::AbstractInputStream is implemented in the Qore Language
 *      - the private data of the resulting object is an instance of this class
 *      a) Qore code reads bytes by simply calling X::bulkRead() which is implemented in Qore, nothing special
 *      b) C++ code reads bytes by retrieving the private data of the object and calling bulkRead() which in this case
 *        is not overridden and thus the default implementation (in this class) is used. This method invokes the
 *        bulkRead() method implemented in Qore and unwraps the data from the resulting `binary` by copying them
 *        to the buffer provided by the caller
 *
 * @todo add timeout support
 * @todo add thread checks
 */
class AbstractInputStream : public AbstractPrivateData {

public:
   /**
    * @brief Constructor.
    * @param self the QoreObject associated to this private data
    */
   AbstractInputStream(QoreObject *self) : self(self) {
   }

   /**
    * @brief Reads a single byte from the input stream.
    *
    * This method is called from C++ code to read a single byte from the input stream.
    * Default implementation invokes the Qore version of this method on the actual QoreObject. This is intended to
    * happen only for classes implementing the AbstractInputStream in the Qore language. C++ implementations should
    * override this method and read the byte directly from the source.
    * @param xsink the exception sink
    * @return the byte (0-255) read or -1 if the end of the stream has been reached
    */
   DLLLOCAL virtual int64 read(ExceptionSink* xsink) {
      return self->evalMethodValue("read", 0, xsink).getAsBigInt();
   }

   /**
    * @brief Reads up to `limit` bytes from the input stream.
    *
    * This method is called from C++ code to read bytes from the input stream.
    * Default implementation invokes the Qore version of this method on the actual QoreObject. This is intended to
    * happen only for classes implementing the AbstractInputStream in the Qore language. C++ implementations should
    * override this method and read the byte directly from the source.
    * @param ptr the destination buffer to read data into
    * @param limit the maximum number of bytes to read, must be &gt; 0
    * @param xsink the exception sink
    * @return the number of bytes read, 0 indicates the end of the stream
    */
   DLLLOCAL virtual int64 bulkRead(void *ptr, int64 limit, ExceptionSink *xsink) {
      assert(limit > 0);
      ReferenceHolder<QoreListNode> args(new QoreListNode(), xsink);
      args->push(new QoreBigIntNode(limit));
      ValueHolder bufHolder(self->evalMethodValue("bulkRead", *args, xsink), xsink);
      if (!bufHolder) {
         return 0;
      }
      BinaryNode *buf = bufHolder->get<BinaryNode>();
      qore_size_t count = buf->size();
      if (count == 0) {
         xsink->raiseException("INPUT-STREAM-ERROR",
               "%s::bulkRead() returned an empty binary; NOTHING should be used to indicate the end of the stream",
               self->getClassName());
         return 0;
      }
      if (count > static_cast<qore_size_t>(limit)) {
         xsink->raiseException("INPUT-STREAM-ERROR",
               "%s::bulkRead() returned %lu bytes which is more than the specified limit of %lu",
               self->getClassName(), count, static_cast<qore_size_t>(limit));
         return 0;
      }
      memcpy(ptr, buf->getPtr(), count);
      return count;
   }

   /**
    * @brief Helper method that calls bulkRead() and wraps the read data to Qore's `binary` value.
    *
    * This method is used by subclasses of Qore::AbstractInputStream implemented in C++ to provide their bulkRead()
    * function.
    * @param limit the maximum number of bytes to read
    * @param xsink the exception sink
    * @return the `binary` wrapping the read data or `NOTHING` if the end of the stream has been reached
    */
   DLLLOCAL BinaryNode *bulkReadToBinary(int64 limit, ExceptionSink *xsink) {
      if (limit <= 0) {
         xsink->raiseException("INPUT-STREAM-ERROR", "%s::bulkRead() called with non-positive limit %lld",
               self->getClassName(), limit);
         return 0;
      }
      SimpleRefHolder<BinaryNode> result(new BinaryNode());
      result->preallocate(limit);
      int64 count = bulkRead(const_cast<void *>(result->getPtr()), limit, xsink);
      result->setSize(count);
      return count ? result.release() : 0;
   }

protected:
   QoreObject *self;                    //!< The QoreObject associated to this private data
};

#endif // _QORE_ABSTRACTINPUTSTREAM_H
