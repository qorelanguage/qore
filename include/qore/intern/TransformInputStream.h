/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  TransformInputStream.h

  Qore Programming Language

  Copyright (C) 2016 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_TRANSFORMINPUTSTREAM_H
#define _QORE_TRANSFORMINPUTSTREAM_H

#include <cassert>
#include "qore/intern/InputStreamBase.h"
#include "qore/Transform.h"

class TransformInputStream : public InputStreamBase {
public:
   DLLLOCAL TransformInputStream(InputStream *is, Transform *t) :
      is(is),
      t(t),
      inBufSize(t->inputBufferSize()),
      outBufSize(t->outputBufferSize()),
      buf(new char[inBufSize]),
      outBuf(new char[outBufSize]),
      bufCount(0),
      outBufCount(0),
      eof(false) {
   }

   DLLLOCAL ~TransformInputStream() {
      delete [] buf;
      delete [] outBuf;
   }

   DLLLOCAL const char *getName() override {
      return "TransformInputStream";
   }

   DLLLOCAL int64 read(void *ptr, int64 limit, ExceptionSink *xsink) override {
      if (outBufCount > 0) {
         int64 toCopy = QORE_MIN(static_cast<int64>(outBufCount), limit);
         memcpy(ptr, outBuf, toCopy);
         memmove(outBuf, outBuf + toCopy, outBufCount - toCopy); // Shift out buffer.
         outBufCount -= toCopy;
         return toCopy;
      }
      while (true) {
         if (!eof && inBufSize - bufCount > 0) {
            int64 r = is->read(buf + bufCount, inBufSize - bufCount, xsink);
            if (*xsink) {
               return 0;
            }
            if (!r) {
               eof = true;
            } else {
               bufCount += r;
            }
         }

         assert(eof || bufCount > 0);
         std::pair<int64, int64> r = t->apply(bufCount ? buf : nullptr, bufCount, ptr, limit, xsink);
         if (*xsink) {
            return 0;
         }
         if (r.first) {
            bufCount -= r.first;
            memmove(buf, buf + r.first, bufCount);
         }
         if (r.second) {
            return r.second;
         }
         if (!r.first) {
            //did not produce anything and did not read anything
            assert(eof);
            assert(!bufCount);
            return 0;
         }
      }
   }

   DLLLOCAL int64 peek(ExceptionSink* xsink) override {
      if (outBufCount > 0)
         return outBuf[0];
      int64 rc = read(outBuf, outBufSize, xsink);
      if (*xsink)
         return -2;
      if (rc == 0) {
         eof = true;
         return -1;
      }
      outBufCount += rc;
      return outBuf[0];
   }

private:
   SimpleRefHolder<InputStream> is;
   SimpleRefHolder<Transform> t;
   size_t inBufSize; //!< Input buffer size.
   size_t outBufSize; //!< Output buffer size.
   char* buf; //!< Buffer with source data from input stream.
   char* outBuf; //!< Output buffer used for peeking.
   size_t bufCount; //!< Actual count of bytes in the buffer.
   size_t outBufCount; //!< Actual count of bytes in the out buffer.
   bool eof;
};

#endif // _QORE_TRANSFORMINPUTSTREAM_H
