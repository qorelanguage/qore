/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  InputStream.h

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

#ifndef _QORE_INPUTSTREAM_H
#define _QORE_INPUTSTREAM_H

#include "qore/AbstractPrivateData.h"

DLLEXPORT extern QoreClass* QC_INPUTSTREAM;

/**
 * @brief Interface for private data of input streams.
 *
 * Methods in this interface serve as low-level API for using input streams from C++ code.
 */
class InputStream : public AbstractPrivateData {

public:
   /**
    * @brief Closes the input stream.
    * @param xsink the exception sink
    * @throws IO-ERROR if an I/O error occurs
    */
   DLLLOCAL virtual void close(ExceptionSink* xsink) = 0;

   /**
    * @brief Reads a single byte from the input stream.
    * @param timeout timeout in milliseconds, &lt; 0 blocks indefinitely, == 0 does not block
    * @param xsink the exception sink
    * @return the byte (0-255) read or -1 if the end of the stream has been reached
    * @throws IO-ERROR if an I/O error occurs
    * @throws TIMEOUT-ERROR if no bytes were read in the specified timeout
    */
   DLLLOCAL virtual int64 read(int64 timeout, ExceptionSink* xsink) = 0;

   /**
    * @brief Reads up to `limit` bytes from the input stream.
    * @param ptr the destination buffer to read data into
    * @param limit the maximum number of bytes to read, must be &gt; 0
    * @param timeout timeout in milliseconds, &lt; 0 blocks indefinitely, == 0 does not block
    * @param xsink the exception sink
    * @return the number of bytes read, 0 indicates the end of the stream
    * @throws IO-ERROR if an I/O error occurs
    * @throws TIMEOUT-ERROR if no bytes were read in the specified timeout
    */
   DLLLOCAL virtual int64 bulkRead(void *ptr, int64 limit, int64 timeout, ExceptionSink *xsink) = 0;

protected:
   /**
    * @brief Constructor.
    */
   InputStream() {
   }
};

#endif // _QORE_INPUTSTREAM_H
