/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  EncodingConvertor.h

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

#ifndef INCLUDE_QORE_INTERN_ENCODINGCONVERTOR_H
#define INCLUDE_QORE_INTERN_ENCODINGCONVERTOR_H

#include "qore/Transform.h"
#include "qore/intern/IconvHelper.h"

class EncodingConvertor : public Transform {

public:
   EncodingConvertor(const QoreEncoding *srcEncoding, const QoreEncoding *dstEncoding)
         : conv(dstEncoding, srcEncoding), inCount(0), outCount(0) {
   }

   DLLLOCAL std::pair<int64, int64> apply(const void *src, int64 srcLen, void *dst, int64 dstLen) override {
      const char *srcPtr = static_cast<const char *>(src);
      char *dstPtr = static_cast<char *>(dst);
      while (true) {
         size_t r = QORE_MIN(static_cast<size_t>(srcLen), BUFSIZE - inCount);
         if (r) {
            memcpy(inBuf + inCount, srcPtr, r);
            srcLen -= r;
            srcPtr += r;
            inCount += r;
         }

         char *inbuf = inBuf;
         size_t inavail = inCount;
         char *outbuf = outBuf + outCount;
         size_t outavail = BUFSIZE - outCount;
         conv.iconv(&inbuf, &inavail, &outbuf, &outavail, src == nullptr);
         size_t rc = inCount - inavail;
         if (rc) {
            inCount -= rc;
            memmove(inBuf, inBuf + rc, inCount);
         }

         size_t wc = BUFSIZE - outCount - outavail;
         outCount += wc;

         size_t w = QORE_MIN(static_cast<size_t>(dstLen), outCount);
         if (w) {
            memcpy(dstPtr, outBuf, w);
            dstPtr += w;
            dstLen -= w;
            outCount -= w;
            memmove(outBuf, outBuf + w, outCount);
         }

         if (r == 0 && w == 0) {
            return std::make_pair(srcPtr - static_cast<const char *>(src), dstPtr - static_cast<char *>(dst));
         }
      }
   }

private:
   static constexpr size_t BUFSIZE = 4096;
   IconvHelper conv;
   char inBuf[BUFSIZE];
   size_t inCount;
   char outBuf[BUFSIZE];
   size_t outCount;
};

#endif // INCLUDE_QORE_INTERN_ENCODINGCONVERTOR_H
