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
#include <bzlib.h>
#include "qore/Qore.h"
#include "qore/intern/CompressionTransforms.h"

class CompressionErrorHelper {

public:
   static void mapZlibError(int rc, ExceptionSink *xsink) {
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

   static void mapBzip2Error(int rc, ExceptionSink *xsink) {
      QoreStringNode* desc = new QoreStringNode();
      switch (rc) {
         case BZ_DATA_ERROR:
            desc->set("unable to process input data; data corrupted");
            break;
         case BZ_DATA_ERROR_MAGIC:
            desc->set("unable to process input data; data header corrupted");
            break;
         case BZ_MEM_ERROR:
            desc->set("insufficient memory to complete operation");
            break;
         case BZ_SEQUENCE_ERROR:
             desc->set("sequence error (report as bug to qore developers)");
             break;
         case BZ_PARAM_ERROR:
            desc->set("parameter error (report as bug to qore developers)");
            break;
         case BZ_CONFIG_ERROR:
            desc->set("version mismatch on zlib shared library, check library requirements");
            break;
         default:
            desc->sprintf("error code %d encountered", rc);
            break;
      }
      xsink->raiseException("BZIP2-ERROR", desc);
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
         CompressionErrorHelper::mapZlibError(rc, xsink);
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
         CompressionErrorHelper::mapZlibError(rc, xsink);
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
         CompressionErrorHelper::mapZlibError(rc, xsink);
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
         CompressionErrorHelper::mapZlibError(rc, xsink);
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

class Bzip2CompressTransform : public Transform {

public:
    Bzip2CompressTransform(int64 level, ExceptionSink *xsink) : state(NOT_INIT) {
      strm.bzalloc = Z_NULL;
      strm.bzfree = Z_NULL;
      strm.opaque = Z_NULL;

      if (level == CompressionTransforms::LEVEL_DEFAULT) {
         level = 3;
      } else if (!(level >= 0 && level <= 9)) {
         xsink->raiseException("BZIP2-LEVEL-ERROR", "level must be between 0 - 9 or -1 (value passed: %d)", level);
         return;
      }

      int rc = BZ2_bzCompressInit(&strm, level, 0, 30);
      if (rc != BZ_OK) {
         CompressionErrorHelper::mapBzip2Error(rc, xsink);
         return;
      }
      state = OK;
   }

   ~Bzip2CompressTransform() {
      if (state != NOT_INIT) {
         BZ2_bzCompressEnd(&strm);
      }
   }

   std::pair<int64, int64> apply(const void *src, int64 srcLen, void *dst, int64 dstLen, ExceptionSink *xsink) {
      if (state == END) {
         return std::make_pair(0, 0);
      }
      if (state != OK) {
         xsink->raiseException("BZIP2-ERROR", "invalid zlib stream state");
         return std::make_pair(0, 0);
      }
      strm.next_in = static_cast<char *>(const_cast<void *>(src));
      strm.avail_in = srcLen;
      strm.next_out = static_cast<char *>(dst);
      strm.avail_out = dstLen;
      int rc = BZ2_bzCompress(&strm, src ? BZ_RUN : BZ_FINISH);
      if (rc != BZ_RUN_OK && rc != BZ_FINISH_OK && rc != BZ_STREAM_END) {
         CompressionErrorHelper::mapBzip2Error(rc, xsink);
         state = ERROR;
         return std::make_pair(0, 0);
      }
      if (rc == BZ_STREAM_END) {
         state = END;
      }
      return std::make_pair(srcLen - strm.avail_in, dstLen - strm.avail_out);
   }

private:
   enum State {
      OK, ERROR, END, NOT_INIT
   };

private:
   bz_stream strm;
   State state;
};

class Bzip2DecompressTransform : public Transform {

public:
    Bzip2DecompressTransform(ExceptionSink *xsink) : state(NOT_INIT) {
      strm.bzalloc = Z_NULL;
      strm.bzfree = Z_NULL;
      strm.opaque = Z_NULL;

      int rc = BZ2_bzDecompressInit(&strm, 0, 0);
      if (rc != Z_OK) {
         CompressionErrorHelper::mapBzip2Error(rc, xsink);
         return;
      }
      state = OK;
   }

   ~Bzip2DecompressTransform() {
      if (state != NOT_INIT) {
         BZ2_bzDecompressEnd(&strm);
      }
   }

   std::pair<int64, int64> apply(const void *src, int64 srcLen, void *dst, int64 dstLen, ExceptionSink *xsink) {
      if (state == END) {
         if (src) {
            xsink->raiseException("BZIP2-ERROR", "Unexpected extra bytes at the end of the compressed data stream");
            state = ERROR;
         }
         return std::make_pair(0, 0);
      }
      if (state != OK) {
         xsink->raiseException("BZIP2-ERROR", "invalid bzip2 stream state");
         return std::make_pair(0, 0);
      }
      strm.next_in = static_cast<char *>(const_cast<void *>(src));
      strm.avail_in = srcLen;
      strm.next_out = static_cast<char *>(dst);
      strm.avail_out = dstLen;
      int rc = BZ2_bzDecompress(&strm);
      if (rc == BZ_STREAM_END) {
         if (strm.avail_in != 0) {
            xsink->raiseException("BZIP2-ERROR", "Unexpected extra bytes at the end of the compressed data stream");
            state = ERROR;
            return std::make_pair(0, 0);
         }
         state = END;
      } else if (rc == BZ_OK && (!src && dstLen - strm.avail_out == 0)) {
         xsink->raiseException("BZIP2-ERROR", "Unexpected end of compressed data stream");
         state = ERROR;
         return std::make_pair(0, 0);
      } else if (rc != BZ_OK) {
         CompressionErrorHelper::mapBzip2Error(rc, xsink);
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
   bz_stream strm;
   State state;
};

Transform *CompressionTransforms::getCompressor(const QoreStringNode *alg, int64 level, ExceptionSink *xsink) {
   if (*alg == ALG_ZLIB) {
      return new ZlibDeflateTransform(level, xsink, false);
   } else if (*alg == ALG_GZIP) {
      return new ZlibDeflateTransform(level, xsink, true);
   } else if (*alg == ALG_BZIP2) {
      return new Bzip2CompressTransform(level, xsink);
   }
   xsink->raiseException("COMPRESS-ERROR", "Unknown compression algorithm: %s", alg->getBuffer());
   return 0;
}

Transform *CompressionTransforms::getDecompressor(const QoreStringNode *alg, ExceptionSink *xsink) {
   if (*alg == ALG_ZLIB) {
      return new ZlibInflateTransform(xsink, false);
   } else if (*alg == ALG_GZIP) {
      return new ZlibInflateTransform(xsink, true);
   } else if (*alg == ALG_BZIP2) {
      return new Bzip2DecompressTransform(xsink);
   }
   xsink->raiseException("COMPRESS-ERROR", "Unknown compression algorithm: %s", alg->getBuffer());
   return 0;
}
