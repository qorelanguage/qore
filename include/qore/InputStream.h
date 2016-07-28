/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  InputStream.h

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
    * @brief Reads up to `limit` bytes from the input stream.
    * @param ptr the destination buffer to read data into
    * @param limit the maximum number of bytes to read, must be &gt; 0
    * @return the number of bytes read, 0 indicates the end of the stream
    */
   virtual int64 read(void *ptr, int64 limit) = 0;

protected:
   /**
    * @brief Constructor.
    */
   InputStream() = default;
};

#endif // _QORE_INPUTSTREAM_H
