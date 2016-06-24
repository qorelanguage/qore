/* indent-tabs-mode: nil -*- */
/*
  CompressionTransforms.cpp

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
#include <zlib.h>
#include "qore/Qore.h"
#include "qore/intern/CompressionTransforms.h"

class ZlibDeflateTransform : public Transform {

public:
   ZlibDeflateTransform(int64 level, ExceptionSink *xsink) {
      strm.zalloc = Z_NULL;
      strm.zfree = Z_NULL;
      strm.opaque = Z_NULL;
      //TODO use level
      deflateInit(&strm, Z_DEFAULT_COMPRESSION);
      //TODO check return value
   }

   ~ZlibDeflateTransform() {
      deflateEnd(&strm);
   }

   std::pair<int64, int64> apply(const void *src, int64 srcLen, void *dst, int64 dstLen, ExceptionSink *xsink) {
      strm.next_in = static_cast<Bytef *>(const_cast<void *>(src));
      strm.avail_in = srcLen;
      strm.next_out = static_cast<Bytef *>(dst);
      strm.avail_out = dstLen;
      deflate(&strm, src ? Z_NO_FLUSH : Z_FINISH);
      //TODO check return value
      return std::make_pair(srcLen - strm.avail_in, dstLen - strm.avail_out);
   }

private:
   z_stream strm;
};

class ZlibInflateTransform : public Transform {

public:
   ZlibInflateTransform(ExceptionSink *xsink) : state(OK) {
      strm.zalloc = Z_NULL;
      strm.zfree = Z_NULL;
      strm.opaque = Z_NULL;
      if (inflateInit(&strm) != Z_OK) {
         state = NOT_INIT;
         //TODO xsink->raiseException()
      }
   }

   ~ZlibInflateTransform() {
      if (state != NOT_INIT) {
         inflateEnd(&strm);
      }
   }

   std::pair<int64, int64> apply(const void *src, int64 srcLen, void *dst, int64 dstLen, ExceptionSink *xsink) {
      if (state == END) {
         if (src) {
            xsink->raiseException("ZLIB", "Unexpected extra bytes at the end of the compressed data stream");
            state = ERROR;
         }
         return std::make_pair(0, 0);
      }
      assert(state == OK);
      strm.next_in = static_cast<Bytef *>(const_cast<void *>(src));
      strm.avail_in = srcLen;
      strm.next_out = static_cast<Bytef *>(dst);
      strm.avail_out = dstLen;
      int rc = inflate(&strm, Z_NO_FLUSH);
      if (rc == Z_STREAM_END) {
         state = END;
         if (strm.avail_in != 0) {
            xsink->raiseException("ZLIB", "Unexpected extra bytes at the end of the compressed data stream");
            state = ERROR;
            return std::make_pair(0, 0);
         }
      } else {
         if (!src && dstLen - strm.avail_out == 0) {
            xsink->raiseException("ZLIB", "Unexpected end of compressed data stream");
            state = ERROR;
            return std::make_pair(0, 0);
         }
      }
      return std::make_pair(srcLen - strm.avail_in, dstLen - strm.avail_out);
   }

private:
   enum State {
      OK, ERROR, END, NOT_INIT
   };

private:
   z_stream strm;
   State state;
};

Transform *CompressionTransforms::getCompressor(const QoreStringNode *alg, int64 level, ExceptionSink *xsink) {
   if (*alg == ALG_ZLIB) {
      return new ZlibDeflateTransform(level, xsink);
   }
   xsink->raiseException("COMPRESS-ERROR", "Unknown compression algorithm: %s", alg->getBuffer());
   return 0;
}

Transform *CompressionTransforms::getDecompressor(const QoreStringNode *alg, ExceptionSink *xsink) {
   if (*alg == ALG_ZLIB) {
      return new ZlibInflateTransform(xsink);
   }
   xsink->raiseException("COMPRESS-ERROR", "Unknown compression algorithm: %s", alg->getBuffer());
   return 0;
}
