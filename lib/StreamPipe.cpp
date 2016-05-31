/* indent-tabs-mode: nil -*- */
/*
  StreamPipe.cpp

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
#include <qore/Qore.h>
#include <qore/intern/StreamPipe.h>

StreamPipe::StreamPipe(int64 bufferSize) {
}

bool PipeInputStream::isClosed() /*override*/ {
   return true;
}

void PipeInputStream::close(ExceptionSink* xsink) /*override*/ {

}

int64 PipeInputStream::read(int64 timeout, ExceptionSink* xsink) /*override*/ {
   return -1;
}

int64 PipeInputStream::bulkRead(void *ptr, int64 limit, int64 timeout, ExceptionSink *xsink) /*override*/ {
   return 0;
}

bool PipeOutputStream::isClosed() /*override*/ {
   return true;
}

void PipeOutputStream::close(ExceptionSink* xsink) /*override*/ {

}

void PipeOutputStream::write(int64 value, int64 timeout, ExceptionSink* xsink) /*override*/ {

}

void PipeOutputStream::bulkWrite(const void *ptr, int64 count, int64 timeout, ExceptionSink *xsink) /*override*/ {

}
