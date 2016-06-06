/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ConversionInputStreamBase.h

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

#ifndef _QORE_CONVERSIONINPUTSTREAMBASE_H
#define _QORE_CONVERSIONINPUTSTREAMBASE_H

#include "qore/intern/InputStreamBase.h"

/**
 * @brief Base class for input streams that perform conversion.
 *
 * @tparam SIZE the size of the internal input and output buffers
 * @tparam T the type of the element of the buffer
 * @todo this API is work in progress
 */
template<size_t SIZE = 4096, typename T = char>
class ConversionInputStreamBase : public InputStreamBase {

public:
   DLLLOCAL bool isClosed() /*override*/ {
      return !is;
   }

   DLLLOCAL void close(ExceptionSink* xsink) /*override*/ {
      assert(!isClosed());
      is->close(xsink);
      is = 0;
   }

   DLLLOCAL int64 read(ExceptionSink* xsink) /*override*/ {
      assert(!isClosed());
      if (!fillOutput(1, xsink)) {
         //exception raised
         return -1;
      }
      if (outBuf.count == 0) {
         //end of stream
         return -1;
      }
      --outBuf.count;
      return *outBuf.ptr++ & 0xFF;
   }

   DLLLOCAL int64 bulkRead(void *ptr, int64 limit, ExceptionSink *xsink) /*override*/ {
      assert(!isClosed());
      assert(limit > 0);
      if (!fillOutput(limit, xsink)) {
         //exception raised
         return -1;
      }
      int64 count = outBuf.count;
      if (count > limit) {
         count = limit;
      }
      memcpy(ptr, outBuf.ptr, count);
      outBuf.count -= count;
      outBuf.ptr += count;
      return count;
   }

protected:
   /**
    * @brief Constructor.
    * @param is the private data of `is`, takes over its reference count
    * @param xsink the exception sink (used in the destructor when dereferencing `isObj`)
    */
   ConversionInputStreamBase(InputStream *is, ExceptionSink *xsink) : is(is, xsink), eofReached(false) {
   }

   /**
    * @brief Called to perform the conversion.
    *
    * When this method is called, there is inBuf.count input bytes available in the memory location pointed to by
    * inBuf.ptr and outAvail bytes available in the output buffer pointed to by outBuf.getWriteBuf(). Both sizes are
    * greater than zero. The implementation must:
    *  - increase inBuf.ptr by the number of bytes read
    *  - decrease inBuf.count by the number of bytes read
    *  - increase outBuf.count by the number of bytes written
    * The SIZE template parameter of this class must by chosen so that it is guaranteed that this method is always able
    * to convert at least some bytes.
    * @param outAvail the number of bytes available
    * @param xsink the exception sink
    * @return true if successful, false if an exception has been raised
    */
   DLLLOCAL virtual bool performConversion(size_t outAvail, ExceptionSink *xsink) = 0;

private:
   bool fillInput(ExceptionSink *xsink) {
      size_t inAvail;
      if (eofReached || (inAvail = inBuf.compact()) == 0) {
         return true;
      }
      int64 readCount = is->bulkRead(inBuf.getWritePtr(), inAvail, xsink);
      if (*xsink) {
         return false;
      }
      if (readCount == 0) {
         eofReached = true;
      } else {
         inBuf.count += readCount;
      }
      return true;
   }

   DLLLOCAL bool fillOutput(size_t cnt, ExceptionSink *xsink) {
      if (outBuf.count >= cnt) {
         return true;
      }
      do {
         if (!fillInput(xsink)) {
            return false;
         }
         size_t outAvail = outBuf.compact();
         if (inBuf.count == 0 || outAvail == 0) {
            break;
         }
         if (!performConversion(outAvail, xsink)) {
            return false;
         }
      } while (!outBuf.count);
      return true;
   }

protected:
   struct Buffer {
      T buf[SIZE];
      T *ptr;
      size_t count;

      Buffer() : ptr(buf), count(0) {
      }

      size_t compact() {
         if (ptr != buf) {
            memmove(buf, ptr, count);
            ptr = buf;
         }
         return SIZE - count;
      }

      T *getReadPtr() const {
         return ptr;
      }

      T *getWritePtr() const {
         return ptr + count;
      }
   };

protected:
   ReferenceHolder<InputStream> is;
   Buffer inBuf;
   Buffer outBuf;
   bool eofReached;
};

#endif // _QORE_CONVERSIONINPUTSTREAMBASE_H
