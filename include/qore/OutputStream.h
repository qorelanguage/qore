/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  OutputStream.h

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

#ifndef _QORE_OUTPUTSTREAM_H
#define _QORE_OUTPUTSTREAM_H

#include "qore/AbstractPrivateData.h"

DLLEXPORT extern QoreClass* QC_OUTPUTSTREAM;

/**
 * @brief Interface for private data of output streams.
 *
 * Methods in this interface serve as low-level API for using output streams from C++ code.
 */
class OutputStream : public AbstractPrivateData {

public:
   /**
    * @brief Closes the output stream.
    * @param xsink the exception sink
    * @throws IO-ERROR if an I/O error occurs
    */
   DLLLOCAL virtual void close(ExceptionSink* xsink) = 0;

   /**
    * @brief Writes a single byte to the output stream.
    * @param value the byte to write
    * @param timeout timeout in milliseconds, &lt; 0 blocks indefinitely, == 0 does not block
    * @param xsink the exception sink
    * @throws IO-ERROR if an I/O error occurs
    * @throws TIMEOUT-ERROR if the byte could not be written in the specified timeout
    */
   DLLLOCAL virtual void write(int64 value, int64 timeout, ExceptionSink* xsink) = 0;

   /**
    * @brief Writes bytes to the output stream.
    * @param ptr the source buffer to write to the stream
    * @param count the number of bytes to write, must be &gt;= 0
    * @param timeout timeout in milliseconds, &lt; 0 blocks indefinitely, == 0 does not block
    * @param xsink the exception sink
    * @throws IO-ERROR if an I/O error occurs
    * @throws TIMEOUT-ERROR if the bytes could not be written in the specified timeout
    */
   DLLLOCAL virtual void bulkWrite(void *ptr, int64 count, int64 timeout, ExceptionSink *xsink) = 0;

protected:
   /**
    * @brief Constructor.
    */
   OutputStream() {
   }
};

#endif // _QORE_OUTPUTSTREAM_H
