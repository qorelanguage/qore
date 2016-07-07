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

class ZlibHelper {

public:
   static void mapError(int rc, ExceptionSink *xsink) {
      QoreStringNode* desc = new QoreStringNode();
      switch (rc) {
         case Z_ERRNO:
            q_strerror(*desc, errno);
            break;
         case Z_STREAM_ERROR:
            desc->concat("inconsistent stream state");
            break;
         case Z_DATA_ERROR:
            desc->set("unable to process input data; data corrupted");
            break;
         case Z_MEM_ERROR:
            desc->set("insufficient memory to complete operation");
            break;
         case Z_BUF_ERROR:
            desc->set("qore buffer-handling error (report as bug to qore developers)");
            break;
         case Z_VERSION_ERROR:
            desc->set("version mismatch on zlib shared library, check library requirements");
            break;
         default:
            desc->sprintf("error code %d encountered", rc);
            break;
      }
      xsink->raiseException("ZLIB-ERROR", desc);
   }
};

class ZlibDeflateTransform : public Transform {

public:
   ZlibDeflateTransform(int64 level, ExceptionSink *xsink, bool gzipFormat) : state(NOT_INIT) {
      strm.zalloc = Z_NULL;
      strm.zfree = Z_NULL;
      strm.opaque = Z_NULL;

      if (level == CompressionTransforms::LEVEL_DEFAULT) {
         level = Z_DEFAULT_COMPRESSION;
      } else if (!(level >= 0 && level <= 9)) {
         xsink->raiseException("ZLIB-LEVEL-ERROR", "level must be between 0 - 9 or -1 (value passed: %d)", level);
         return;
      }

      int rc = deflateInit2(&strm, level, Z_DEFLATED, 15 + (gzipFormat ? 16 : 0), 8, Z_DEFAULT_STRATEGY);
      if (rc != Z_OK) {
         ZlibHelper::mapError(rc, xsink);
         return;
      }
      state = OK;
   }

   ~ZlibDeflateTransform() {
      if (state != NOT_INIT) {
         deflateEnd(&strm);
      }
   }

   std::pair<int64, int64> apply(const void *src, int64 srcLen, void *dst, int64 dstLen, ExceptionSink *xsink) {
      if (state != OK) {
          xsink->raiseException("ZLIB-ERROR", "invalid zlib stream state");
          return std::make_pair(0, 0);
      }
      strm.next_in = static_cast<Bytef *>(const_cast<void *>(src));
      strm.avail_in = srcLen;
      strm.next_out = static_cast<Bytef *>(dst);
      strm.avail_out = dstLen;
      int rc = deflate(&strm, src ? Z_NO_FLUSH : Z_FINISH);
      if (rc != Z_OK && rc != Z_STREAM_END) {
          ZlibHelper::mapError(rc, xsink);
          state = ERROR;
          return std::make_pair(0, 0);
      }
      return std::make_pair(srcLen - strm.avail_in, dstLen - strm.avail_out);
   }

private:
   enum State {
      OK, ERROR, NOT_INIT
   };

private:
   z_stream strm;
   State state;
};

class ZlibInflateTransform : public Transform {

public:
   ZlibInflateTransform(ExceptionSink *xsink, bool gzipFormat) : state(NOT_INIT) {
      strm.zalloc = Z_NULL;
      strm.zfree = Z_NULL;
      strm.opaque = Z_NULL;

      int rc = inflateInit2(&strm, 15 + (gzipFormat ? 16 : 0));
      if (rc != Z_OK) {
          ZlibHelper::mapError(rc, xsink);
          return;
       }
       state = OK;
   }

   ~ZlibInflateTransform() {
      if (state != NOT_INIT) {
         inflateEnd(&strm);
      }
   }

   std::pair<int64, int64> apply(const void *src, int64 srcLen, void *dst, int64 dstLen, ExceptionSink *xsink) {
      if (state == END) {
         if (src) {
            xsink->raiseException("ZLIB-ERROR", "Unexpected extra bytes at the end of the compressed data stream");
            state = ERROR;
         }
         return std::make_pair(0, 0);
      }
      if (state != OK) {
          xsink->raiseException("ZLIB-ERROR", "invalid zlib stream state");
          return std::make_pair(0, 0);
      }
      strm.next_in = static_cast<Bytef *>(const_cast<void *>(src));
      strm.avail_in = srcLen;
      strm.next_out = static_cast<Bytef *>(dst);
      strm.avail_out = dstLen;
      int rc = inflate(&strm, Z_NO_FLUSH);
      if (rc == Z_STREAM_END) {
         if (strm.avail_in != 0) {
            xsink->raiseException("ZLIB-ERROR", "Unexpected extra bytes at the end of the compressed data stream");
            state = ERROR;
            return std::make_pair(0, 0);
         }
         state = END;
      } else if ((rc == Z_OK || rc == Z_BUF_ERROR) && (!src && dstLen - strm.avail_out == 0)) {
        xsink->raiseException("ZLIB-ERROR", "Unexpected end of compressed data stream");
        state = ERROR;
        return std::make_pair(0, 0);
      } else if (rc != Z_OK) {
          ZlibHelper::mapError(rc, xsink);
          state = ERROR;
          return std::make_pair(0, 0);
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
      return new ZlibDeflateTransform(level, xsink, false);
   } else if (*alg == ALG_GZIP) {
         return new ZlibDeflateTransform(level, xsink, true);
   }
   xsink->raiseException("COMPRESS-ERROR", "Unknown compression algorithm: %s", alg->getBuffer());
   return 0;
}

Transform *CompressionTransforms::getDecompressor(const QoreStringNode *alg, ExceptionSink *xsink) {
   if (*alg == ALG_ZLIB) {
      return new ZlibInflateTransform(xsink, false);
   } else if (*alg == ALG_GZIP) {
       return new ZlibInflateTransform(xsink, true);
   }
   xsink->raiseException("COMPRESS-ERROR", "Unknown compression algorithm: %s", alg->getBuffer());
   return 0;
}
