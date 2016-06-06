/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  BinaryInputStream.h

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

#ifndef _QORE_BINARYINPUTSTREAM_H
#define _QORE_BINARYINPUTSTREAM_H

#include <stdint.h>
#include "qore/intern/InputStreamBase.h"

/**
 * @brief Private data for the Qore::BinaryInputStream class.
 */
class BinaryInputStream : public InputStreamBase {

public:
   DLLLOCAL BinaryInputStream(BinaryNode *src) : src(src), offset(0) {
   }

   DLLLOCAL const char *getName() /*override*/ {
      return "BinaryInputStream";
   }

   DLLLOCAL bool isClosed() /*override*/ {
      return !src;
   }

   DLLLOCAL void close(ExceptionSink* xsink) /*override*/ {
      assert(!isClosed());
      src = 0;
   }

   DLLLOCAL int64 read(ExceptionSink* xsink) /*override*/ {
      assert(!isClosed());
      return offset >= src->size() ? -1 : static_cast<const uint8_t *>(src->getPtr())[offset++];
   }

   DLLLOCAL int64 bulkRead(void *ptr, int64 limit, ExceptionSink *xsink) /*override*/ {
      assert(!isClosed());
      assert(limit > 0);
      qore_size_t count = src->size() - offset;
      if (count == 0) {
         return 0;
      }
      if (count > static_cast<qore_size_t>(limit)) {
         count = limit;
      }
      memcpy(ptr, static_cast<const uint8_t *>(src->getPtr()) + offset, count);
      offset += count;
      return count;
   }

private:
   SimpleRefHolder<BinaryNode> src;
   qore_size_t offset;                          //!< @invariant offset >= 0 && offset <= src->size()
};

#endif // _QORE_BINARYINPUTSTREAM_H
