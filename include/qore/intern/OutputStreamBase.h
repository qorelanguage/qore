/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  OutputStreamBase.h

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

#ifndef _QORE_OUTPUTSTREAMBASE_H
#define _QORE_OUTPUTSTREAMBASE_H

#include "qore/OutputStream.h"
#include "qore/intern/core/Exception.h"
#include "qore/intern/core/StringBuilder.h"

/**
 * @brief Base class for private data of output stream implementations in C++.
 */
class OutputStreamBase : public OutputStream {

public:
   /**
    * @brief Helper method that checks that the current thread is the same as when the instance was created,
    * that the stream has not yet been closed and calls close().
    * @param xsink the exception sink
    */
   DLLLOCAL void closeHelper(ExceptionSink *xsink) {
      try {
         checkThreadAndState();
         close();
      } CATCH(xsink, return)
   }

   /**
    * @brief Helper method that checks that the current thread is the same as when the instance was created,
    * that the stream has not yet been closed and calls write().
    * @param data the data to write
    * @param xsink the exception sink
    */
   DLLLOCAL void writeHelper(const BinaryNode *data, ExceptionSink *xsink) {
      try {
         checkThreadAndState();
         write(data->getPtr(), data->size());
      } CATCH(xsink, return)
   }

   /**
    * @brief Checks that the current thread is the same as when the instance was created and that the stream has
    * not yet been closed.
    * @throws OUTPUT-STREAM-THREAD-ERROR if the current thread is not the same as when the instance was created
    * @throws OUTPUT-STREAM-CLOSED-ERROR if the stream has been closed
    */
   void checkThreadAndState() {
      if (tid != gettid()) {
         throw qore::Exception("OUTPUT-STREAM-THREAD-ERROR", qore::StringBuilder() << "this " << getName()
               << " object was created in TID " << tid << "; it is an error to access it from any other thread "
               "(accessed from TID " << gettid() << ")");
      }
      if (isClosed()) {
         throw qore::Exception("OUTPUT-STREAM-CLOSED-ERROR", qore::StringBuilder() << "this " << getName()
               << " object has been already closed");
      }
   }

protected:
   /**
    * @brief Constructor.
    */
   OutputStreamBase() : tid(gettid()) {
   }

   /**
    * @brief Returns true is the stream has been closed.
    * @return true is the stream has been closed
    */
   DLLLOCAL virtual bool isClosed() = 0;

   /**
    * @brief Returns the name of the class.
    * @return the name of the class
    */
   DLLLOCAL virtual const char *getName() = 0;

protected:
   int tid;                             //!< The id of the thread that created the instance
};

#endif // _QORE_OUTPUTSTREAMBASE_H
