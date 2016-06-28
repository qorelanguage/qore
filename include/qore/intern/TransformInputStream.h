/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  TransformInputStream.h

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

#ifndef _QORE_TRANSFORMINPUTSTREAM_H
#define _QORE_TRANSFORMINPUTSTREAM_H

#include <cassert>
#include "qore/intern/InputStreamBase.h"
#include "qore/Transform.h"

//FIXME this is still work in progress
class TransformInputStream : public InputStreamBase {

public:
   TransformInputStream(InputStream *is, Transform *t) : is(is), t(t), count(0), eof(false) {
   }

   const char *getName() override {
      return "TransformInputStream";
   }

   int64 read(void *ptr, int64 limit, ExceptionSink *xsink) override {
      while (true) {
         if (!eof && BUFSIZE - count > 0) {
            int64 r = is->read(buf + count, BUFSIZE - count, xsink);
            if (*xsink) {
               return 0;
            }
            if (!r) {
               eof = true;
            } else {
               count += r;
            }
         }

         assert(eof || count > 0);
         std::pair<int64, int64> r = t->apply(count ? buf : nullptr, count, ptr, limit, xsink);
         if (*xsink) {
            return 0;
         }
         if (r.first) {
            count -= r.first;
            memmove(buf, buf + r.first, count);
         }
         if (r.second) {
            return r.second;
         }
         if (!r.first) {
            //did not produce anything and did not read anything
            assert(eof);
            assert(!count);
            return 0;
         }
      }
   }

private:
   static constexpr size_t BUFSIZE = 4096;

private:
   SimpleRefHolder<InputStream> is;
   SimpleRefHolder<Transform> t;
   char buf[BUFSIZE];
   size_t count;
   bool eof;
};

#endif // _QORE_TRANSFORMINPUTSTREAM_H
