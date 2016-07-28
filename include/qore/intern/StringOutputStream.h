/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  StringOutputStream.h

  Qore Programming Language

  Copyright (C) 2016 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation strings (the "Software"),
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

#ifndef _QORE_STRINGOUTPUTSTREAM_H
#define _QORE_STRINGOUTPUTSTREAM_H

#include <stdint.h>
#include "qore/intern/OutputStreamBase.h"

/**
 * @brief Private data for the Qore::StringOutputStream class.
 */
class StringOutputStream : public OutputStreamBase {

public:
   DLLLOCAL StringOutputStream() : StringOutputStream(QCS_DEFAULT) {
   }

   DLLLOCAL StringOutputStream(const QoreEncoding* e) : enc(e), buf(new QoreStringNode) {
   }

   DLLLOCAL const char *getName() override {
      return "StringOutputStream";
   }

   DLLLOCAL bool isClosed() override {
      return !buf;
   }

   DLLLOCAL void close() override {
      assert(!isClosed());
      buf = 0;
   }

   DLLLOCAL void write(const void *ptr, int64 count) override {
      assert(!isClosed());
      assert(count >= 0);
      buf->concat((const char*)ptr, count);
   }

   DLLLOCAL const QoreEncoding* getEncoding() const {
      return enc;
   }

   DLLLOCAL QoreStringNode *getData() {
      checkThreadAndState();
      assert(!isClosed());
      QoreStringNode *ret = buf.release();
      buf = new QoreStringNode(enc);
      return ret;
   }

private:
   const QoreEncoding* enc;
   SimpleRefHolder<QoreStringNode> buf;
};

#endif // _QORE_STRINGOUTPUTSTREAM_H
