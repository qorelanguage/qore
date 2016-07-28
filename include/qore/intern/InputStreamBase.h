/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  InputStreamBase.h

  Qore Programming Language

  Copyright (C) 2016 Qore Technologies, sro

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

#ifndef _QORE_INPUTSTREAMBASE_H
#define _QORE_INPUTSTREAMBASE_H

#include "qore/InputStream.h"
#include "qore/intern/core/Exception.h"
#include "qore/intern/core/StringBuilder.h"

/**
 * @brief Base class for private data of input stream implementations in C++.
 */
class InputStreamBase : public InputStream {

public:
   /**
    * @brief Helper method that checks that the current thread is the same as when the instance was created,
    * calls read() and wraps the read data to Qore's `binary` value.
    * @param limit the maximum number of bytes to read
    * @param xsink the exception sink
    * @return the `binary` wrapping the read data or `NOTHING` if the end of the stream has been reached
    */
   DLLLOCAL BinaryNode *readHelper(int64 limit, ExceptionSink *xsink) {
      try {
         checkThread();
         if (limit <= 0) {
            throw qore::Exception("INPUT-STREAM-ERROR", qore::StringBuilder() << getName()
                  << "::read() called with non-positive limit " << limit);
         }
         SimpleRefHolder<BinaryNode> result(new BinaryNode());
         result->preallocate(limit);
         int64 count = read(const_cast<void *>(result->getPtr()), limit);
         result->setSize(count);
         return count ? result.release() : 0;
      } CATCH(xsink, return 0)
   }

   /**
    * @brief Checks that the current thread is the same as when the instance was created and that the stream has
    * not yet been closed.
    * @return true if the checks passed, false if an exception has been raised
    * @throws INPUT-STREAM-THREAD-ERROR if the current thread is not the same as when the instance was created
    * @throws INPUT-STREAM-CLOSED-ERROR if the stream has been closed
    */
   void checkThread() {
      if (tid != gettid()) {
         throw qore::Exception("INPUT-STREAM-THREAD-ERROR", qore::StringBuilder() << "this " << getName()
               << " object was created in TID " << tid << "; it is an error to access it from any other thread "
               "(accessed from TID " << gettid() << ")");
      }
   }

protected:
   /**
    * @brief Constructor.
    */
   InputStreamBase() : tid(gettid()) {
   }

   /**
    * @brief Returns the name of the class.
    * @return the name of the class
    */
   DLLLOCAL virtual const char *getName() = 0;

protected:
   int tid;                             //!< The id of the thread that created the instance
};

#endif // _QORE_INPUTSTREAMBASE_H
