/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  StreamPipe.h

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

#ifndef _QORE_STREAMPIPE_H
#define _QORE_STREAMPIPE_H

#include <stdint.h>
#include "qore/intern/InputStreamBase.h"
#include "qore/intern/OutputStreamBase.h"

/**
 * @brief Private data for the Qore::StreamPipe class.
 *
 * Contains the state (buffer, pointers and synchronization objects) shared by the PipeInputStream and PipeOutputStream.
 */
class StreamPipe : public AbstractPrivateData {

public:
   DLLLOCAL StreamPipe(int64 bufferSize);
};

/**
 * @brief Private data for the Qore::PipeInputStream class.
 */
class PipeInputStream : public InputStreamBase {

public:
   DLLLOCAL PipeInputStream(StreamPipe *pipe) : pipe(pipe) {
   }

   DLLLOCAL const char *getName() /*override*/ {
      return "PipeInputStream";
   }

   DLLLOCAL bool isClosed() /*override*/;
   DLLLOCAL void close(ExceptionSink* xsink) /*override*/;
   DLLLOCAL int64 read(int64 timeout, ExceptionSink* xsink) /*override*/;
   DLLLOCAL int64 bulkRead(void *ptr, int64 limit, int64 timeout, ExceptionSink *xsink) /*override*/;

private:
   SimpleRefHolder<StreamPipe> pipe;
};

/**
 * @brief Private data for the Qore::PipeOutputStream class.
 */
class PipeOutputStream : public OutputStreamBase {

public:
   DLLLOCAL PipeOutputStream(StreamPipe *pipe) : pipe(pipe) {
   }

   DLLLOCAL const char *getName() /*override*/ {
      return "PipeOutputStream";
   }

   DLLLOCAL bool isClosed() /*override*/;
   DLLLOCAL void close(ExceptionSink* xsink) /*override*/;
   DLLLOCAL void write(int64 value, int64 timeout, ExceptionSink* xsink) /*override*/;
   DLLLOCAL void bulkWrite(const void *ptr, int64 count, int64 timeout, ExceptionSink *xsink) /*override*/;

private:
   SimpleRefHolder<StreamPipe> pipe;
};

#endif // _QORE_STREAMPIPE_H
