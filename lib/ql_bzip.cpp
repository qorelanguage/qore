/*
  ql_bzip.cpp

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <qore/Qore.h>
#include <qore/intern/ql_bzip.h>

#include <bzlib.h>

#ifndef QORE_BZ2_WORK_FACTOR
#define QORE_BZ2_WORK_FACTOR 30
#endif

#ifndef QORE_BZ2_VERBOSITY
#define QORE_BZ2_VERBOSITY 0
#endif

#ifndef QORE_BZ2_DEFAULT_LEVEL
#define QORE_BZ2_DEFAULT_LEVEL 9
#endif

class qore_bz_stream : public bz_stream {
public:
   DLLLOCAL qore_bz_stream() {
      bzalloc = 0;
      bzfree = 0;
      opaque = 0;
   }
};

class qore_bz_compressor : public qore_bz_stream {
   bool ok;

public:
   DLLLOCAL qore_bz_compressor(int level, ExceptionSink *xsink) {
      int rc = BZ2_bzCompressInit(this, level, QORE_BZ2_VERBOSITY, QORE_BZ2_WORK_FACTOR);
      ok = (rc == BZ_OK);
      if (!ok)
	 xsink->raiseException("BZIP2-COMPRESS-ERROR", "code %d returned from BZ2_bzCompressInit()", rc);
   }
   
   DLLLOCAL ~qore_bz_compressor() {
      if (ok)
	 BZ2_bzCompressEnd(this);
   }
   DLLLOCAL operator bool() const { return ok; }

   DLLLOCAL BinaryNode *compress(const void *ptr, unsigned long len, ExceptionSink *xsink) {
      char *p = (char *)ptr;
      
      int bs = len >> 3;
      if (!bs)
	 bs = 1;

      next_in = p;
      avail_in = len;

      SimpleRefHolder<BinaryNode> b(new BinaryNode());

      if (b->preallocate(bs)) {
	 xsink->outOfMemory();
	 return 0;      
      }

      while (true) {
	 int64 done = (int64)total_out_lo32 + (((int64)total_out_hi32) << 32);
	 next_out = ((char *)b->getPtr()) + done;
	 avail_out = (unsigned)(bs - done);

	 int rc = BZ2_bzCompress(this, BZ_FINISH);
	 //printd(5, "bs=%d, done=%lld, avail_out=%d, rc=%d, total_out_lo32=%d, total_out_hi32 = %d\n", bs, done, avail_out, rc, total_out_lo32, total_out_hi32);
	 if (rc == BZ_STREAM_END)
	    break;
	 if (rc != BZ_FINISH_OK) {
	    xsink->raiseException("BZIP2-COMPRESS-ERROR", "error code %d returned from BZ2_bzCompress()", rc);
	    return 0;
	 }
	 bs *= 2;
	 if (b->preallocate(bs)) {
	    xsink->outOfMemory();
	    return 0;      
	 }
      }
      b->setSize((int64)total_out_lo32 + (((int64)total_out_hi32) << 32));

      return b.release();
   }
};

class qore_bz_decompressor : public qore_bz_stream {
   bool ok;

public:
   DLLLOCAL qore_bz_decompressor(ExceptionSink *xsink) {
      int rc = BZ2_bzDecompressInit(this, QORE_BZ2_VERBOSITY, 0);
      ok = (rc == BZ_OK);
      if (!ok)
	 xsink->raiseException("BZIP2-DECOMPRESS-ERROR", "code %d returned from BZ2_bzDecompressInit()", rc);
   }

   DLLLOCAL ~qore_bz_decompressor() {
      if (ok)
	 BZ2_bzDecompressEnd(this);
   }
   DLLLOCAL operator bool() const { return ok; }

   DLLLOCAL BinaryNode *decompress(const void *ptr, unsigned long len, ExceptionSink *xsink) {
      char *p = (char *)ptr;

      int bs = len << 1;

      next_in = p;
      avail_in = len;

      SimpleRefHolder<BinaryNode> b(new BinaryNode());

      if (b->preallocate(bs)) {
	 xsink->outOfMemory();
	 return 0;      
      }

      while (true) {
	 int64 done = (int64)total_out_lo32 + (((int64)total_out_hi32) << 32);
	 next_out = ((char *)b->getPtr()) + done;
	 avail_out = (unsigned)(bs - done);

	 int rc = BZ2_bzDecompress(this);
	 //printd(5, "bs=%d, done=%lld, avail_out=%d, rc=%d, total_out_lo32=%d, total_out_hi32 = %d\n", bs, done, avail_out, rc, total_out_lo32, total_out_hi32);
	 if (rc == BZ_STREAM_END)
	    break;
	 if (rc != BZ_OK) {
	    xsink->raiseException("BZIP2-DECOMPRESS-ERROR", "error code %d returned from BZ2_bzDecompress()", rc);
	    return 0;
	 }
	 bs *= 2;
	 if (b->preallocate(bs)) {
	    xsink->outOfMemory();
	    return 0;      
	 }
      }
      b->setSize((int64)total_out_lo32 + (((int64)total_out_hi32) << 32));
      
      return b.release();
   }

   DLLLOCAL QoreStringNode *decompress_to_string(const void *ptr, unsigned long len, const QoreEncoding *enc, ExceptionSink *xsink) {
      static char np[] = {'\0'};

      SimpleRefHolder<BinaryNode> b(decompress(ptr, len, xsink));
      if (!b)
	 return 0;

      len = b->size();

      // terminate the string
      b->append(np, 1);

      return new QoreStringNode((char *)b->giveBuffer(), len, len + 1, enc);
   }
};

BinaryNode *qore_bzip2(const void *ptr, unsigned long len, int level, ExceptionSink *xsink) {
   qore_bz_compressor c(level, xsink);
   if (!c)
      return 0;

   return c.compress(ptr, len, xsink);
}

BinaryNode *qore_bunzip2_to_binary(const BinaryNode *b, ExceptionSink *xsink) {
   qore_bz_decompressor c(xsink);
   if (!c)
      return 0;

   return c.decompress(b->getPtr(), b->size(), xsink);
}

QoreStringNode *qore_bunzip2_to_string(const BinaryNode *b, const QoreEncoding *enc, ExceptionSink *xsink) {
   qore_bz_decompressor c(xsink);
   if (!c)
      return 0;

   return c.decompress_to_string(b->getPtr(), b->size(), enc, xsink);
}

static AbstractQoreNode *f_bzip2_bin(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, args, 0);
   int level = get_int_param_with_default(args, 1, QORE_BZ2_DEFAULT_LEVEL);

   if (!level || level > 9) {
      xsink->raiseException("BZLIB2-LEVEL-ERROR", "level must be between 1 - 9 (value passed: %d)", level);
      return 0;
   }

   return qore_bzip2(b->getPtr(), b->size(), level, xsink);
}

static AbstractQoreNode *f_bzip2_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, args, 0);
   int level = get_int_param_with_default(args, 1, QORE_BZ2_DEFAULT_LEVEL);

   if (!level || level > 9) {
      xsink->raiseException("BZLIB2-LEVEL-ERROR", "level must be between 1 - 9 (value passed: %d)", level);
      return 0;
   }

   return qore_bzip2(str->getBuffer(), str->strlen(), level, xsink);
}

static AbstractQoreNode *f_bunzip2_to_binary(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, args, 0);
   return qore_bunzip2_to_binary(b, xsink);
}

static AbstractQoreNode *f_bunzip2_to_string(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, args, 0);
   const QoreEncoding *ccsid = get_hard_qore_encoding_param(args, 1);
   return qore_bunzip2_to_string(b, ccsid, xsink);
}

void init_bzip_functions() {
   builtinFunctions.add2("bzip2",              f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   // 2 string variants so the variant without the level can be used in constant expressions
   builtinFunctions.add2("bzip2",              f_bzip2_str, QC_CONSTANT, QDOM_DEFAULT, binaryTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("bzip2",              f_bzip2_str, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   // 2 binary variants so the variant without the level can be used in constant expressions
   builtinFunctions.add2("bzip2",              f_bzip2_bin, QC_CONSTANT, QDOM_DEFAULT, binaryTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("bzip2",              f_bzip2_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("bunzip2_to_binary",  f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("bunzip2_to_binary",  f_bunzip2_to_binary, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("bunzip2_to_string",  f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("bunzip2_to_string",  f_bunzip2_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, new QoreStringNode(QCS_DEFAULT->getCode()));
}

