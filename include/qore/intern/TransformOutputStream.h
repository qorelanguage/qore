/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  TransformOutputStream.h

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

#ifndef _QORE_TRANSFORMOUTPUTSTREAM_H
#define _QORE_TRANSFORMOUTPUTSTREAM_H

#include "qore/intern/OutputStreamBase.h"
#include "qore/Transform.h"
#include "qore/intern/core/Exception.h"

class TransformOutputStream : public OutputStreamBase {

public:
   TransformOutputStream(OutputStream *os, Transform *t) : os(os), t(t), closed(false) {
   }

   const char *getName() override {
      return "TransformOutputStream";
   }

   bool isClosed() override {
      return closed;
   }

   void close() override {
      char buf[BUFSIZE];
      closed = true;
      while (true) {
         std::pair<int64, int64> r = t->apply(NULL, 0, buf, sizeof(buf));
         if (!r.second) {
            break;
         }
         os->write(buf, r.second);
      }
   }

   void write(const void *ptr, int64 len) override {
      assert(len >= 0);
      const char *src = static_cast<const char *>(ptr);
      char buf[BUFSIZE];
      while (len > 0) {
         std::pair<int64, int64> r = t->apply(src, len, buf, sizeof(buf));
         if (r.second) {
            os->write(buf, r.second);
         }
         src += r.first;
         len -= r.first;
      }
   }

private:
   static constexpr size_t BUFSIZE = 4096;

private:
   SimpleRefHolder<OutputStream> os;
   SimpleRefHolder<Transform> t;
   bool closed;
};

#endif // _QORE_TRANSFORMOUTPUTSTREAM_H
