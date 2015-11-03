/*
  ql_bzip.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

class qore_bz_stream : public bz_stream
{
   public:
      DLLLOCAL qore_bz_stream()
      {
	 bzalloc = 0;
	 bzfree = 0;
	 opaque = 0;
      }
};

class qore_bz_compressor : public qore_bz_stream
{
      bool ok;

   public:
      DLLLOCAL qore_bz_compressor(int level, ExceptionSink *xsink)
      {
	 int rc = BZ2_bzCompressInit(this, level, QORE_BZ2_VERBOSITY, QORE_BZ2_WORK_FACTOR);
	 ok = (rc == BZ_OK);
	 if (!ok)
	    xsink->raiseException("BZIP2-COMPRESS-ERROR", "code %d returned from BZ2_bzCompressInit()", rc);
      }

      DLLLOCAL ~qore_bz_compressor()
      {
	 if (ok)
	    BZ2_bzCompressEnd(this);
      }
      DLLLOCAL operator bool() const { return ok; }

      DLLLOCAL BinaryNode *compress(const void *ptr, unsigned long len, ExceptionSink *xsink)
      {
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
	    avail_out = bs - done;

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

class qore_bz_decompressor : public qore_bz_stream
{
      bool ok;

   public:
      DLLLOCAL qore_bz_decompressor(ExceptionSink *xsink)
      {
	 int rc = BZ2_bzDecompressInit(this, QORE_BZ2_VERBOSITY, 0);
	 ok = (rc == BZ_OK);
	 if (!ok)
	    xsink->raiseException("BZIP2-DECOMPRESS-ERROR", "code %d returned from BZ2_bzDecompressInit()", rc);
      }

      DLLLOCAL ~qore_bz_decompressor()
      {
	 if (ok)
	    BZ2_bzDecompressEnd(this);
      }
      DLLLOCAL operator bool() const { return ok; }

      DLLLOCAL BinaryNode *decompress(const void *ptr, unsigned long len, ExceptionSink *xsink)
      {
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
	    avail_out = bs - done;

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

static AbstractQoreNode *f_bzip2(const QoreListNode *params, ExceptionSink *xsink)
{
  // need a string or binary argument
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (!p0)
      return 0;

   const AbstractQoreNode *p1 = get_param(params, 1);
   int level = p1 ? p1->getAsInt() : QORE_BZ2_DEFAULT_LEVEL;

   if (!level || level > 9)
   {
      xsink->raiseException("BZLIB2-LEVEL-ERROR", "level must be between 1 - 9 (value passed: %d)", level);
      return 0;
   }

   const void *ptr;
   unsigned long len;
   if (p0->getType() == NT_STRING)
   {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p0);
      ptr = str->getBuffer();
      len = str->strlen();
   }
   else if (p0->getType() == NT_BINARY)
   {
      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(p0);
      ptr = b->getPtr();
      len = b->size();
   }
   else
      return 0;

   return qore_bzip2(ptr, len, level, xsink);
}

static AbstractQoreNode *f_bunzip2_to_binary(const QoreListNode *params, ExceptionSink *xsink)
{
   // need a binary argument
   const BinaryNode *b = test_binary_param(params, 0);
   if (!b)
      return 0;
   
   return qore_bunzip2_to_binary(b, xsink);
}

static AbstractQoreNode *f_bunzip2_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   // need binary argument
   const BinaryNode *p = test_binary_param(params, 0);
   if (!p)
      return 0;

   const QoreEncoding *ccsid;
   const QoreStringNode *p1 = test_string_param(params, 1);
   ccsid = p1 ? QEM.findCreate(p1) : QCS_DEFAULT;

   return qore_bunzip2_to_string(p, ccsid, xsink);
}

void init_bzip_functions()
{
   // register builtin functions in this file
   builtinFunctions.add("bzip2",              f_bzip2);
   builtinFunctions.add("bunzip2_to_binary",  f_bunzip2_to_binary);
   builtinFunctions.add("bunzip2_to_string",  f_bunzip2_to_string);
}

