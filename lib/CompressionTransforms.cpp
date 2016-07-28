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
#include "qore/intern/core/Exception.h"
#include "qore/intern/core/StringBuilder.h"
#include "qore/intern/CompressionTransforms.h"

class ZlibError : public virtual qore::Exception {

public:
   ZlibError(int rc) : Exception("ZLIB-ERROR") {
      switch (rc) {
         case Z_STREAM_ERROR:
            setDescription("inconsistent stream state");
            break;
         case Z_DATA_ERROR:
            setDescription("unable to process input data; data corrupted");
            break;
         case Z_MEM_ERROR:
            setDescription("insufficient memory to complete operation");
            break;
         case Z_BUF_ERROR:
            setDescription("qore buffer-handling error (report as bug to qore developers)");
            break;
         case Z_VERSION_ERROR:
            setDescription("version mismatch on zlib shared library, check library requirements");
            break;
         default:
            setDescription(qore::StringBuilder() << "error code " << rc << " encountered");
            break;
      }
   }

   ZlibError(std::string desc) : Exception("ZLIB-ERROR", std::move(desc)) {
   }

   ZlibError(ZlibError &&) noexcept = default;
   ZlibError &operator=(ZlibError &&) noexcept = default;

private:
   ZlibError(const ZlibError &) = delete;
   ZlibError &operator=(const ZlibError &) = delete;
};

class Bzip2Error : public virtual qore::Exception {

public:
   Bzip2Error(int rc) : Exception("BZIP2-ERROR") {
      switch (rc) {
         case BZ_DATA_ERROR:
            setDescription("unable to process input data; data corrupted");
            break;
         case BZ_DATA_ERROR_MAGIC:
            setDescription("unable to process input data; data header corrupted");
            break;
         case BZ_MEM_ERROR:
            setDescription("insufficient memory to complete operation");
            break;
         case BZ_SEQUENCE_ERROR:
            setDescription("sequence error (report as bug to qore developers)");
            break;
         case BZ_PARAM_ERROR:
            setDescription("parameter error (report as bug to qore developers)");
            break;
         case BZ_CONFIG_ERROR:
            setDescription("version mismatch on zlib shared library, check library requirements");
            break;
         default:
            setDescription(qore::StringBuilder() << "error code " << rc << " encountered");
            break;
      }
   }

   Bzip2Error(std::string desc) : Exception("BZIP2-ERROR", std::move(desc)) {
   }

   Bzip2Error(Bzip2Error &&) noexcept = default;
   Bzip2Error &operator=(Bzip2Error &&) noexcept = default;

private:
   Bzip2Error(const Bzip2Error &) = delete;
   Bzip2Error &operator=(const Bzip2Error &) = delete;
};

class ZlibDeflateTransform : public Transform {

public:
   ZlibDeflateTransform(int64 level, bool gzipFormat) {
      strm.zalloc = Z_NULL;
      strm.zfree = Z_NULL;
      strm.opaque = Z_NULL;

      if (level == CompressionTransforms::LEVEL_DEFAULT) {
         level = Z_DEFAULT_COMPRESSION;
      } else if (!(level >= 0 && level <= 9)) {
         throw qore::Exception("ZLIB-LEVEL-ERROR", qore::StringBuilder()
               << "level must be between 0 - 9 or -1 (value passed: " << level << ")");
      }

      int rc = deflateInit2(&strm, level, Z_DEFLATED, 15 + (gzipFormat ? 16 : 0), 8, Z_DEFAULT_STRATEGY);
      if (rc != Z_OK) {
         throw ZlibError(rc);
      }
   }

   ~ZlibDeflateTransform() {
      deflateEnd(&strm);
   }

   std::pair<int64, int64> apply(const void *src, int64 srcLen, void *dst, int64 dstLen) override {
      strm.next_in = static_cast<Bytef *>(const_cast<void *>(src));
      strm.avail_in = srcLen;
      strm.next_out = static_cast<Bytef *>(dst);
      strm.avail_out = dstLen;
      int rc = deflate(&strm, src ? Z_NO_FLUSH : Z_FINISH);
      if (rc != Z_OK && rc != Z_STREAM_END) {
         throw ZlibError(rc);
      }
      return std::make_pair(srcLen - strm.avail_in, dstLen - strm.avail_out);
   }

private:
   z_stream strm;
};

class ZlibInflateTransform : public Transform {

public:
   ZlibInflateTransform(bool gzipFormat) : state(OK) {
      strm.zalloc = Z_NULL;
      strm.zfree = Z_NULL;
      strm.opaque = Z_NULL;

      int rc = inflateInit2(&strm, 15 + (gzipFormat ? 16 : 0));
      if (rc != Z_OK) {
         throw ZlibError(rc);
      }
   }

   ~ZlibInflateTransform() {
      inflateEnd(&strm);
   }

   std::pair<int64, int64> apply(const void *src, int64 srcLen, void *dst, int64 dstLen) override {
      if (state == END) {
         if (src) {
            throw ZlibError("Unexpected extra bytes at the end of the compressed data stream");
         }
         return std::make_pair(0, 0);
      }
      if (state != OK) {
         throw ZlibError("invalid zlib stream state");
      }
      strm.next_in = static_cast<Bytef *>(const_cast<void *>(src));
      strm.avail_in = srcLen;
      strm.next_out = static_cast<Bytef *>(dst);
      strm.avail_out = dstLen;
      int rc = inflate(&strm, Z_NO_FLUSH);
      if (rc == Z_STREAM_END) {
         if (strm.avail_in != 0) {
            throw ZlibError("Unexpected extra bytes at the end of the compressed data stream");
         }
         state = END;
      } else if ((rc == Z_OK || rc == Z_BUF_ERROR) && (!src && dstLen - strm.avail_out == 0)) {
         throw ZlibError("Unexpected end of compressed data stream");
      } else if (rc != Z_OK) {
         throw ZlibError(rc);
      }
      return std::make_pair(srcLen - strm.avail_in, dstLen - strm.avail_out);
   }

private:
   enum State {
      OK, END
   };

private:
   z_stream strm;
   State state;
};

class Bzip2CompressTransform : public Transform {

public:
    Bzip2CompressTransform(int64 level) : state(OK) {
      strm.bzalloc = Z_NULL;
      strm.bzfree = Z_NULL;
      strm.opaque = Z_NULL;

      if (level == CompressionTransforms::LEVEL_DEFAULT) {
         level = 3;
      } else if (!(level >= 0 && level <= 9)) {
         throw qore::Exception("BZIP2-LEVEL-ERROR", qore::StringBuilder()
               << "level must be between 0 - 9 or -1 (value passed: " << level << ")");
      }

      int rc = BZ2_bzCompressInit(&strm, level, 0, 30);
      if (rc != BZ_OK) {
         throw Bzip2Error(rc);
      }
   }

   ~Bzip2CompressTransform() {
      BZ2_bzCompressEnd(&strm);
   }

   std::pair<int64, int64> apply(const void *src, int64 srcLen, void *dst, int64 dstLen) override {
      if (state == END) {
         return std::make_pair(0, 0);
      }
      strm.next_in = static_cast<char *>(const_cast<void *>(src));
      strm.avail_in = srcLen;
      strm.next_out = static_cast<char *>(dst);
      strm.avail_out = dstLen;
      int rc = BZ2_bzCompress(&strm, src ? BZ_RUN : BZ_FINISH);
      if (rc != BZ_RUN_OK && rc != BZ_FINISH_OK && rc != BZ_STREAM_END) {
         throw Bzip2Error(rc);
      }
      if (rc == BZ_STREAM_END) {
         state = END;
      }
      return std::make_pair(srcLen - strm.avail_in, dstLen - strm.avail_out);
   }

private:
   enum State {
      OK, END
   };

private:
   bz_stream strm;
   State state;
};

class Bzip2DecompressTransform : public Transform {

public:
    Bzip2DecompressTransform() : state(OK) {
      strm.bzalloc = Z_NULL;
      strm.bzfree = Z_NULL;
      strm.opaque = Z_NULL;

      int rc = BZ2_bzDecompressInit(&strm, 0, 0);
      if (rc != Z_OK) {
         throw Bzip2Error(rc);
      }
   }

   ~Bzip2DecompressTransform() {
      BZ2_bzDecompressEnd(&strm);
   }

   std::pair<int64, int64> apply(const void *src, int64 srcLen, void *dst, int64 dstLen) override {
      if (state == END) {
         if (src) {
            throw Bzip2Error("Unexpected extra bytes at the end of the compressed data stream");
         }
         return std::make_pair(0, 0);
      }
      strm.next_in = static_cast<char *>(const_cast<void *>(src));
      strm.avail_in = srcLen;
      strm.next_out = static_cast<char *>(dst);
      strm.avail_out = dstLen;
      int rc = BZ2_bzDecompress(&strm);
      if (rc == BZ_STREAM_END) {
         if (strm.avail_in != 0) {
            throw Bzip2Error("Unexpected extra bytes at the end of the compressed data stream");
         }
         state = END;
      } else if (rc == BZ_OK && (!src && dstLen - strm.avail_out == 0)) {
         throw Bzip2Error("Unexpected end of compressed data stream");
      } else if (rc != BZ_OK) {
         throw Bzip2Error(rc);
      }
      return std::make_pair(srcLen - strm.avail_in, dstLen - strm.avail_out);
   }

private:
   enum State {
      OK, END
   };

private:
   bz_stream strm;
   State state;
};

Transform *CompressionTransforms::getCompressor(const std::string &alg, int64 level) {
   if (alg == ALG_ZLIB) {
      return new ZlibDeflateTransform(level, false);
   } else if (alg == ALG_GZIP) {
      return new ZlibDeflateTransform(level, true);
   } else if (alg == ALG_BZIP2) {
      return new Bzip2CompressTransform(level);
   }
   throw qore::Exception("COMPRESS-ERROR", qore::StringBuilder() << "Unknown compression algorithm: " << alg);
}

Transform *CompressionTransforms::getDecompressor(const std::string &alg) {
   if (alg == ALG_ZLIB) {
      return new ZlibInflateTransform(false);
   } else if (alg == ALG_GZIP) {
      return new ZlibInflateTransform(true);
   } else if (alg == ALG_BZIP2) {
      return new Bzip2DecompressTransform();
   }
   throw qore::Exception("COMPRESS-ERROR", qore::StringBuilder() << "Unknown compression algorithm: " << alg);
}
