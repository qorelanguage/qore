/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  StdoutOutputStream.h

  Qore Programming Language

  Copyright (C) 2016 - 2018 Qore Technologies, s.r.o.

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

#ifndef _QORE_STDOUTOUTPUTSTREAM_H
#define _QORE_STDOUTOUTPUTSTREAM_H

#include "qore/OutputStream.h"

/**
 * @brief Private data for the Qore::StdoutOutputStream class.
 */
class StdoutOutputStream : public OutputStream {

public:
   DLLLOCAL StdoutOutputStream() {
   }

   DLLLOCAL const char *getName() override {
      return "StdoutOutputStream";
   }

   DLLLOCAL bool isClosed() override {
      return false;
   }

   DLLLOCAL void close(ExceptionSink* xsink) override {
      return;
   }

   DLLLOCAL void write(const void *ptr, int64 count, ExceptionSink *xsink) override {
      assert(count >= 0);
      if (count < 0)
         return;
      const char* current = reinterpret_cast<const char*>(ptr);
      int64 blockCount = count / WRITE_BLOCK_SIZE;
      for (int64 i = 0; i < blockCount; i++) {
         fwrite(current, WRITE_BLOCK_SIZE, 1, stdout);
         current = (current + WRITE_BLOCK_SIZE);
         count -= WRITE_BLOCK_SIZE;
      }
      fwrite(ptr, count, 1, stdout);
   }
private:
   static const int WRITE_BLOCK_SIZE = 1024;
};

#endif // _QORE_STDOUTOUTPUTSTREAM_H
