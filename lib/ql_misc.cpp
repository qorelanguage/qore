/*
  ql_misc.cpp

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/ql_misc.h>
#include <qore/intern/ssl_constants.h>
#include <qore/intern/QoreSignal.h>

#include <string.h>
#include <zlib.h>
#include <time.h>
#include <errno.h>

#ifdef HAVE_GZ_HEADER
class qore_gz_header : public gz_header {
   DLLLOCAL qore_gz_header(bool n_text, char *n_name, char *n_comment) {
      text = n_text;
      time = ::time(0);
      xflags = 0;
      os = 3; // always set to UNIX
      extra = Z_NULL;
      extra_len = 0;
      extra_max = 0;
      name = (Bytef *)n_name;
      name_max = 0;
      comment = (Bytef *)n_comment;
      comm_max = 0;
      hcrc = 0;
      done = 0;
   }
};
#endif

static AbstractQoreNode *f_call_function_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);

   ReferenceHolder<QoreListNode> args(xsink);
   // if there are arguments to pass, create argument list by copying current list
   if (num_params(params) > 1)
      args = params->copyListFrom(1);

   return getProgram()->callFunction(str->getBuffer(), *args, xsink);
}

static AbstractQoreNode *f_call_function_code(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(f, const ResolvedCallReferenceNode, params, 0);

   ReferenceHolder<QoreListNode> args(xsink);
   // if there are arguments to pass, create argument list by copying current list
   if (num_params(params) > 1)
      args = params->copyListFrom(1);

   return f->exec(*args, xsink);
}

static AbstractQoreNode *f_call_function_args_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);

   return getProgram()->callFunction(str->getBuffer(), 0, xsink);
}

static AbstractQoreNode *f_call_function_args_str_something(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   assert(p1);

   ReferenceHolder<QoreListNode> args(new QoreListNode, xsink);
   args->push(p1->refSelf());

   return getProgram()->callFunction(str->getBuffer(), *args, xsink);
}

static AbstractQoreNode *f_call_function_args_str_list(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);
   HARD_QORE_PARAM(args, const QoreListNode, params, 1);

   return getProgram()->callFunction(str->getBuffer(), args, xsink);
}

static AbstractQoreNode *f_call_function_args_code(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(f, const ResolvedCallReferenceNode, params, 0);
   return f->exec(0, xsink);
}

static AbstractQoreNode *f_call_function_args_code_something(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(f, const ResolvedCallReferenceNode, params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   assert(p1);

   ReferenceHolder<QoreListNode> args(new QoreListNode, xsink);
   args->push(p1->refSelf());

   return f->exec(*args, xsink);
}

static AbstractQoreNode *f_call_function_args_code_list(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(f, const ResolvedCallReferenceNode, params, 0);
   HARD_QORE_PARAM(args, const QoreListNode, params, 1);

   return f->exec(args, xsink);
}


static AbstractQoreNode *f_call_builtin_function(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   const BuiltinFunction *f = BuiltinFunctionList::find(p0->getBuffer());
   if (!f) {
      xsink->raiseException("NO-FUNCTION", "cannot find any builtin function '%s()'", p0->getBuffer());
      return 0;
   }

   ReferenceHolder<QoreListNode> args(xsink);
   // if there are arguments to pass, create argument list by copying current list
   if (num_params(params) > 1)
      args = params->copyListFrom(1);

   return f->evalDynamic(*args, xsink);
}

static const BuiltinFunction *get_builtin_func(const QoreStringNode *str, ExceptionSink *xsink) {
   const BuiltinFunction *f = BuiltinFunctionList::find(str->getBuffer());
   if (!f)
      xsink->raiseException("NO-FUNCTION", "cannot find any builtin function '%s()'", str->getBuffer());
   return f;
}

static AbstractQoreNode *f_call_builtin_function_args(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   const BuiltinFunction *f = get_builtin_func(p0, xsink);
   if (!f)
      return 0;

   return f->evalDynamic(0, xsink);
}

static AbstractQoreNode *f_call_builtin_function_args_something(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   const BuiltinFunction *f = get_builtin_func(p0, xsink);
   if (!f)
      return 0;

   const AbstractQoreNode *p1 = get_param(params, 1);
   assert(p1);

   ReferenceHolder<QoreListNode> args(new QoreListNode, xsink);
   args->push(p1->refSelf());

   return f->evalDynamic(*args, xsink);
}

static AbstractQoreNode *f_call_builtin_function_args_list(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   const BuiltinFunction *f = get_builtin_func(p0, xsink);
   if (!f)
      return 0;

   HARD_QORE_PARAM(args, const QoreListNode, params, 1);

   return f->evalDynamic(args, xsink);
}

static AbstractQoreNode *f_exists(const QoreListNode *params, ExceptionSink *xsink) {
   // to emulate the exists operator, we must return True if more than one argument is passed
   // as this will appear to be a list to the exists operator, which is different from NOTHING
   int np = num_params(params);
   if (np <= 1)
      return get_bool_node(!is_nothing(get_param(params, 0)));
   return &True;
}

static AbstractQoreNode *f_existsFunction_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   const char *str = p0->getBuffer();
   
   if (getProgram()->existsFunction(str))
      return boolean_true();

   return builtinFunctions.find(str) ? boolean_true() : boolean_false();
}

// FIXME: should probably return constants
static AbstractQoreNode *f_functionType(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   const char *str = p0->getBuffer();

   if (getProgram()->existsFunction(str))
      return new QoreStringNode("user");

   return builtinFunctions.find(str) ? new QoreStringNode("builtin") : 0;
}

static AbstractQoreNode *f_html_encode(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   QoreStringNode *ns = new QoreStringNode(p0->getEncoding());
   ns->concatAndHTMLEncode(p0->getBuffer());
   return ns;
}

static AbstractQoreNode *f_html_decode(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   QoreStringNode *ns = new QoreStringNode(p0->getEncoding());
   ns->concatAndHTMLDecode(p0);
   return ns;
}

static AbstractQoreNode *f_get_default_encoding(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreStringNode(QCS_DEFAULT->getCode());
}

AbstractQoreNode *f_parse(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   HARD_QORE_PARAM(p1, const QoreStringNode, params, 1);
   getProgram()->parse(p0, p1, xsink);
   return 0;
}

static AbstractQoreNode *f_getClassName(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreObject, params, 0);
   return new QoreStringNode(p0->getClass()->getName());
}

// parse_url() - with hard typing
static AbstractQoreNode *f_parse_url(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   QoreURL url(p0);
   if (!url.isValid()) {
      xsink->raiseException("PARSE-URL-ERROR", "URL '%s' cannot be parsed", p0->getBuffer());
      return 0;
   }

   return url.getHash();
}

// parseURL(string $url) returns *hash
static AbstractQoreNode *f_parseURL(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   QoreURL url(p0);
   return url.isValid() ? url.getHash() : 0;
}

static AbstractQoreNode *f_backquote(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   return backquoteEval(p0->getBuffer(), xsink);
}

static AbstractQoreNode *f_makeBase64String_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(pstr, const QoreStringNode, params, 0);
   QoreStringNode *str = new QoreStringNode();
   str->concatBase64(pstr);
   return str;
}

static AbstractQoreNode *f_makeBase64String_bin(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   return new QoreStringNode(b);
}

static AbstractQoreNode *f_parseBase64String(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   return p0->parseBase64(xsink);
}

static AbstractQoreNode *f_parseBase64StringToString(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   return p0->parseBase64ToString(xsink);
}

static AbstractQoreNode *f_getModuleList(const QoreListNode *params, ExceptionSink *xsink) {
   return MM.getModuleList();
}

static AbstractQoreNode *f_getModuleHash(const QoreListNode *params, ExceptionSink *xsink) {
   return MM.getModuleHash();
}

static AbstractQoreNode *f_getFeatureList(const QoreListNode *params, ExceptionSink *xsink) {
   return getProgram()->getFeatureList();
}

static AbstractQoreNode *f_hash_values(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreHashNode *p0 = HARD_QORE_HASH(params, 0);

   QoreListNode *l = new QoreListNode;

   ConstHashIterator hi(p0);
   while (hi.next())
      l->push(hi.getReferencedValue());

   return l;
}

void do_zlib_exception(int rc, const char *func, ExceptionSink *xsink) {
   QoreStringNode *desc = new QoreStringNode();
   desc->sprintf("%s(): ", func);
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

void do_deflate_end(z_stream *c_stream, ExceptionSink *xsink) {
   int rc = deflateEnd(c_stream);
   if (rc != Z_OK)
      do_zlib_exception(rc, "deflateEnd", xsink);
}

void do_inflate_end(z_stream *d_stream, ExceptionSink *xsink) {
   int rc = inflateEnd(d_stream);
   if (rc != Z_OK)
      do_zlib_exception(rc, "inflateEnd", xsink);
}

BinaryNode *qore_deflate(const void *ptr, unsigned long len, int level, ExceptionSink *xsink) {
   z_stream c_stream; // compression stream
   c_stream.zalloc = Z_NULL;
   c_stream.zfree = Z_NULL;
   c_stream.opaque = Z_NULL;
   
   int rc = deflateInit(&c_stream, level);   
   if (rc != Z_OK) {
      do_zlib_exception(rc, "deflateInit", xsink);
      return 0;
   }

   ON_BLOCK_EXIT(do_deflate_end, &c_stream, xsink);
   
   // allocate new buffer
   unsigned long bsize = len / 5 + 100;
   void *buf = malloc(bsize);
   
   c_stream.next_in = (Bytef *)ptr;
   c_stream.next_out = (Bytef *)buf;
   c_stream.avail_in = len;
   c_stream.avail_out = bsize;

   while (c_stream.avail_in) {
      rc = deflate(&c_stream, Z_NO_FLUSH);
      if (rc != Z_OK && rc != Z_BUF_ERROR) {
         free(buf);
         do_zlib_exception(rc, "deflate", xsink);
         return 0;
      }
      
      if (!c_stream.avail_out) {
         int new_space = ((len / 3) + 100);
         //printd(5, "deflate() Z_BUF_ERROR:1 bsize=%d->%d, new_space=%d avail_out=%d -> %d next_out=%p\n", bsize, bsize + new_space, new_space, c_stream.avail_out, c_stream.avail_out + new_space, c_stream.next_out);
         bsize += new_space;
         c_stream.avail_out += new_space;
         buf = realloc(buf, bsize);
         c_stream.next_out = ((Bytef *)buf) + c_stream.total_out;
      }
      //printd(5, "deflate() Z_BUF_ERROR:1 bsize=%d, avail_out=%d, next_out=%p\n", bsize, c_stream.avail_out, c_stream.next_out);
   }

   while (true) {
      rc = deflate(&c_stream, Z_FINISH);
      if (rc == Z_STREAM_END)
         break;
      if (rc != Z_OK && rc != Z_BUF_ERROR) {
         free(buf);
         do_zlib_exception(rc, "deflate", xsink);
         return 0;
      }
      // resize buffer
      int new_space = 2; //((len / 3) + 100);
      //printd(5, "deflate() Z_BUF_ERROR:2 bsize=%d->%d, new_space=%d avail_out=%d -> %d, next_out=%p\n", bsize, bsize + new_space, new_space, c_stream.avail_out, c_stream.avail_out + new_space, c_stream.next_out);
      
      bsize += new_space;
      c_stream.avail_out += new_space;
      buf = realloc(buf, bsize);
      c_stream.next_out = ((Bytef *)buf) + c_stream.total_out;
   }

   //printd(5, "deflate() buf=%p, bsize=%d, avail_out=%d, size=%d, next_out=%p\n", buf, bsize, c_stream.avail_out, bsize - c_stream.avail_out, c_stream.next_out);
   return new BinaryNode(buf, bsize - c_stream.avail_out);
}

QoreStringNode *qore_inflate_to_string(const BinaryNode *b, const QoreEncoding *enc, ExceptionSink *xsink) {
   z_stream d_stream; // decompression stream
   d_stream.zalloc = Z_NULL;
   d_stream.zfree = Z_NULL;
   d_stream.opaque = Z_NULL;

   int rc = inflateInit(&d_stream);
   if (rc != Z_OK) {
      do_zlib_exception(rc, "inflateInit", xsink);
      return 0;
   }

   ON_BLOCK_EXIT(do_inflate_end, &d_stream, xsink);
   
   int len = b->size();

   // allocate new buffer
   unsigned long bsize = len * 2 + 100;
   void *buf = malloc(bsize);
   
   d_stream.next_in = (Bytef *)b->getPtr();
   d_stream.next_out = (Bytef *)buf;
   d_stream.avail_in = b->size();
   d_stream.avail_out = bsize;

   while (true) {
      rc = inflate(&d_stream, Z_NO_FLUSH);
      if (rc == Z_STREAM_END)
         break;
      if (rc == Z_BUF_ERROR) {
         int new_space = ((len * 3) + 100);
         bsize += new_space;
         d_stream.avail_out += new_space;
         buf = realloc(buf, bsize);
         d_stream.next_out = ((Bytef *)buf) + d_stream.total_out;
      }
      else if (rc != Z_OK) {
         free(buf);
         do_zlib_exception(rc, "inflate", xsink);
         return 0;
      }
   }

   // how much data was decompressed
   len = bsize - d_stream.avail_out;

   // create and return the string
   return new QoreStringNode((char *)buf, len, len, enc);
}

BinaryNode *qore_inflate_to_binary(const BinaryNode *b, ExceptionSink *xsink) {
   z_stream d_stream; // decompression stream
   d_stream.zalloc = Z_NULL;
   d_stream.zfree = Z_NULL;
   d_stream.opaque = Z_NULL;

   int rc = inflateInit(&d_stream);
   if (rc != Z_OK) {
      do_zlib_exception(rc, "inflateInit", xsink);
      return 0;
   }

   ON_BLOCK_EXIT(do_inflate_end, &d_stream, xsink);
   
   int len = b->size();

   // allocate new buffer
   unsigned long bsize = len * 2 + 100;
   void *buf = malloc(bsize);
   
   d_stream.next_in = (Bytef *)b->getPtr();
   d_stream.next_out = (Bytef *)buf;
   d_stream.avail_in = b->size();
   d_stream.avail_out = bsize;

   while (true){
      rc = inflate(&d_stream, Z_NO_FLUSH);
      if (rc == Z_STREAM_END)
         break;
      if (rc == Z_BUF_ERROR) {
         int new_space = ((len * 3) + 100);
         bsize += new_space;
         d_stream.avail_out += new_space;
         buf = realloc(buf, bsize);
         d_stream.next_out = ((Bytef *)buf) + d_stream.total_out;
      }
      else if (rc != Z_OK) {
         free(buf);
         do_zlib_exception(rc, "inflate", xsink);
         return 0;
      }
   }
   
   return new BinaryNode(buf, bsize - d_stream.avail_out);
}

BinaryNode *qore_gzip(const void *ptr, unsigned long len, int level, ExceptionSink *xsink) {
   z_stream c_stream; // compression stream
   c_stream.zalloc = Z_NULL;
   c_stream.zfree = Z_NULL;
   c_stream.opaque = Z_NULL;
   c_stream.next_in = (Bytef *)ptr;
   c_stream.avail_in = len;

   int rc = deflateInit2(&c_stream, level, Z_DEFLATED, 31, 8,  Z_DEFAULT_STRATEGY);   
   if (rc != Z_OK) {
      do_zlib_exception(rc, "deflateInit2", xsink);
      return 0;
   }

   ON_BLOCK_EXIT(do_deflate_end, &c_stream, xsink);
   
   // allocate new buffer
   unsigned long bsize = len / 5 + 100;
   void *buf = malloc(bsize);
   
   c_stream.next_out = (Bytef *)buf;
   c_stream.avail_out = bsize;

   while (c_stream.avail_in) {
      rc = deflate(&c_stream, Z_NO_FLUSH);
      if (rc != Z_OK && rc != Z_BUF_ERROR) {
         free(buf);
         do_zlib_exception(rc, "deflate", xsink);
         return 0;
      }

      if (!c_stream.avail_out) {
         int new_space = ((len / 3) + 100);
         //printd(5, "deflate() Z_BUF_ERROR:1 bsize=%d->%d, new_space=%d avail_out=%d -> %d next_out=%p\n", bsize, bsize + new_space, new_space, c_stream.avail_out, c_stream.avail_out + new_space, c_stream.next_out);

         bsize += new_space;
         c_stream.avail_out += new_space;
         buf = realloc(buf, bsize);
         c_stream.next_out = ((Bytef *)buf) + c_stream.total_out;
      }
      //printd(5, "deflate() Z_BUF_ERROR:1 bsize=%d, avail_out=%d, next_out=%p\n", bsize, c_stream.avail_out, c_stream.next_out);
   }

   while (true) {
      rc = deflate(&c_stream, Z_FINISH);
      if (rc == Z_STREAM_END)
         break;
      if (rc != Z_OK && rc != Z_BUF_ERROR) {
         free(buf);
         do_zlib_exception(rc, "deflate", xsink);
         return 0;
      }
      // resize buffer
      int new_space = 2; //((len / 3) + 100);
      //printd(5, "deflate() Z_BUF_ERROR:2 bsize=%d->%d, new_space=%d avail_out=%d -> %d, next_out=%p\n", bsize, bsize + new_space, new_space, c_stream.avail_out, c_stream.avail_out + new_space, c_stream.next_out);

      bsize += new_space;
      c_stream.avail_out += new_space;
      buf = realloc(buf, bsize);
      c_stream.next_out = ((Bytef *)buf) + c_stream.total_out;
   }

   //printd(5, "deflate() buf=%p, bsize=%d, avail_out=%d, size=%d, next_out=%p\n", buf, bsize, c_stream.avail_out, bsize - c_stream.avail_out, c_stream.next_out);
   return new BinaryNode(buf, bsize - c_stream.avail_out);
}

QoreStringNode *qore_gunzip_to_string(const BinaryNode *bin, const QoreEncoding *enc, ExceptionSink *xsink) {
   z_stream d_stream; // decompression stream
   d_stream.zalloc = Z_NULL;
   d_stream.zfree = Z_NULL;
   d_stream.opaque = Z_NULL;

   d_stream.next_in = (Bytef *)bin->getPtr();
   d_stream.avail_in = bin->size();

   int rc = inflateInit2(&d_stream, 47);
   if (rc != Z_OK) {
      do_zlib_exception(rc, "inflateInit2", xsink);
      return 0;
   }

   ON_BLOCK_EXIT(do_inflate_end, &d_stream, xsink);
   
   int len = bin->size();

   // allocate new buffer
   unsigned long bsize = len * 2 + 100;
   void *buf = malloc(bsize);
   
   d_stream.next_out = (Bytef *)buf;
   d_stream.avail_out = bsize;

   while (true) {
      rc = inflate(&d_stream, Z_NO_FLUSH);
      if (rc == Z_STREAM_END)
         break;
      if (rc == Z_BUF_ERROR) {
         int new_space = ((len * 3) + 100);
         bsize += new_space;
         d_stream.avail_out += new_space;
         buf = realloc(buf, bsize);
         d_stream.next_out = ((Bytef *)buf) + d_stream.total_out;
      }
      else if (rc != Z_OK) {
         free(buf);
         do_zlib_exception(rc, "inflate", xsink);
         return 0;
      }
   }

   // how much data was decompressed
   len = bsize - d_stream.avail_out;

   // create and return the string
   return new QoreStringNode((char *)buf, len, len, enc);
}

BinaryNode *qore_gunzip_to_binary(const BinaryNode *bin, ExceptionSink *xsink) {
   z_stream d_stream; // decompression stream
   d_stream.zalloc = Z_NULL;
   d_stream.zfree = Z_NULL;
   d_stream.opaque = Z_NULL;

   d_stream.next_in = (Bytef *)bin->getPtr();
   d_stream.avail_in = bin->size();

   int rc = inflateInit2(&d_stream, 47);
   if (rc != Z_OK) {
      do_zlib_exception(rc, "inflateInit2", xsink);
      return 0;
   }

   ON_BLOCK_EXIT(do_inflate_end, &d_stream, xsink);
   
   int len = bin->size();

   // allocate new buffer
   unsigned long bsize = len * 2 + 100;
   void *buf = malloc(bsize);
   
   d_stream.next_out = (Bytef *)buf;
   d_stream.avail_out = bsize;

   while (true) {
      rc = inflate(&d_stream, Z_NO_FLUSH);
      if (rc == Z_STREAM_END)
         break;
      if (rc == Z_BUF_ERROR) {
         int new_space = ((len * 3) + 100);
         bsize += new_space;
         d_stream.avail_out += new_space;
         buf = realloc(buf, bsize);
         d_stream.next_out = ((Bytef *)buf) + d_stream.total_out;
      }
      else if (rc != Z_OK) {
         free(buf);
         do_zlib_exception(rc, "inflate", xsink);
         return 0;
      }
   }
   
   return new BinaryNode(buf, bsize - d_stream.avail_out);
}

static AbstractQoreNode *f_compress_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);
   int level = (int)HARD_QORE_INT(params, 1);

   if ((level < 1 && level != -1) || level > 9) {
      xsink->raiseException("ZLIB-LEVEL-ERROR", "level must be between 1 - 9 or -1 (value passed: %d)", level);
      return 0;
   }
   
   if (!str->strlen())
      return new BinaryNode;
   
   return qore_deflate(str->getBuffer(), str->strlen(), level, xsink);
}

static AbstractQoreNode *f_compress_bin(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   int level = (int)HARD_QORE_INT(params, 1);

   if ((level < 1 && level != -1) || level > 9) {
      xsink->raiseException("ZLIB-LEVEL-ERROR", "level must be between 1 - 9 or -1 (value passed: %d)", level);
      return 0;
   }
   
   if (!b->size())
      return new BinaryNode;
   
   return qore_deflate(b->getPtr(), b->size(), level, xsink);
}

// syntax: uncompress_to_string(binary object, [encoding of new string])
static AbstractQoreNode *f_uncompress_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   const QoreEncoding *ccsid = get_encoding_param(params, 1);
   return qore_inflate_to_string(b, ccsid, xsink);
}

// syntax: uncompress_to_binary(binary object)
static AbstractQoreNode *f_uncompress_to_binary(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   return qore_inflate_to_binary(b, xsink);
}

static AbstractQoreNode *f_gzip_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);
   int level = (int)HARD_QORE_INT(params, 1);

   if (!level || level > 9) {
      xsink->raiseException("ZLIB-LEVEL-ERROR", "level must be between 0 - 9 (value passed: %d)", level);
      return 0;
   }
   
   if (!str->strlen())
      return new BinaryNode;
   
   return qore_gzip(str->getBuffer(), str->strlen(), level, xsink);
}

static AbstractQoreNode *f_gzip_bin(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   int level = (int)HARD_QORE_INT(params, 1);

   if (!level || level > 9) {
      xsink->raiseException("ZLIB-LEVEL-ERROR", "level must be between 0 - 9 (value passed: %d)", level);
      return 0;
   }
   
   if (!b->size())
      return new BinaryNode;
   
   return qore_gzip(b->getPtr(), b->size(), level, xsink);
}

// syntax: gunzip_to_string(binary object, [encoding of new string])
static AbstractQoreNode *f_gunzip_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   const QoreEncoding *ccsid = get_encoding_param(params, 1);
   return qore_gunzip_to_string(b, ccsid, xsink);
}

// syntax: gunzip_to_binary(binary object)
static AbstractQoreNode *f_gunzip_to_binary(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   return qore_gunzip_to_binary(b, xsink);
}

static AbstractQoreNode *f_get_byte_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);
   unsigned char *ptr = (unsigned char *)str->getBuffer();
   int size = str->strlen();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt();

   if (offset >= size || offset < 0)
      return 0;

   return new QoreBigIntNode(ptr[offset]);  
}

static AbstractQoreNode *f_get_byte_bin(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   unsigned char *ptr = (unsigned char *)b->getPtr();
   int size = b->size();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt();

   if (offset >= size || offset < 0)
      return 0;

   return new QoreBigIntNode(ptr[offset]);  
}

static AbstractQoreNode *f_get_word_16_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);
   unsigned char *ptr = (unsigned char *)str->getBuffer();
   int size = str->strlen();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt() * 2;

   if (offset >= (size - 1) || offset < 0)
      return 0;

   short val = ntohs(*((unsigned short *)&ptr[offset]));
   return new QoreBigIntNode(val);
}

static AbstractQoreNode *f_get_word_16_bin(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   unsigned char *ptr = (unsigned char *)b->getPtr();
   int size = b->size();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt() * 2;

   if (offset >= (size - 1) || offset < 0)
      return 0;

   short val = ntohs(*((unsigned short *)&ptr[offset]));

   return new QoreBigIntNode(val);
}

static AbstractQoreNode *f_get_word_32_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);
   unsigned char *ptr = (unsigned char *)str->getBuffer();
   int size = str->strlen();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt() * 4;

   if (offset >= (size - 3) || offset < 0)
      return 0;

   int64 val = ntohl(*((unsigned int *)&ptr[offset]));
   return new QoreBigIntNode(val);
}

static AbstractQoreNode *f_get_word_32_bin(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   unsigned char *ptr = (unsigned char *)b->getPtr();
   int size = b->size();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt() * 4;

   if (offset >= (size - 3) || offset < 0)
      return 0;

   int64 val = ntohl(*((unsigned int *)&ptr[offset]));

   return new QoreBigIntNode(val);
}

static AbstractQoreNode *f_get_word_64_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);
   unsigned char *ptr = (unsigned char *)str->getBuffer();
   int size = str->strlen();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt() * 8;

   if (offset >= (size - 7) || offset < 0)
      return 0;

   int64 val = MSBi8(*((int64 *)&ptr[offset]));
   return new QoreBigIntNode(val);
}

static AbstractQoreNode *f_get_word_64_bin(const QoreListNode *params, ExceptionSink *xsink) {   
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   unsigned char *ptr = (unsigned char *)b->getPtr();
   int size = b->size();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt() * 8;

   if (offset >= (size - 7) || offset < 0)
      return 0;

   int64 val = MSBi8(*((int64 *)&ptr[offset]));

   return new QoreBigIntNode(val);
}

static AbstractQoreNode *f_get_word_16_lsb_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);
   unsigned char *ptr = (unsigned char *)str->getBuffer();
   int size = str->strlen();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt() * 2;

   if (offset >= (size - 1) || offset < 0)
      return 0;

   short val = LSBi2(*((unsigned short *)&ptr[offset]));
   return new QoreBigIntNode(val);
}

static AbstractQoreNode *f_get_word_16_lsb_bin(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   unsigned char *ptr = (unsigned char *)b->getPtr();
   int size = b->size();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt() * 2;

   if (offset >= (size - 1) || offset < 0)
      return 0;

   short val = LSBi2(*((unsigned short *)&ptr[offset]));

   return new QoreBigIntNode(val);
}

static AbstractQoreNode *f_get_word_32_lsb_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);
   unsigned char *ptr = (unsigned char *)str->getBuffer();
   int size = str->strlen();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt() * 4;

   if (offset >= (size - 3) || offset < 0)
      return 0;

   int64 val = LSBi4(*((unsigned int *)&ptr[offset]));
   return new QoreBigIntNode(val);
}

static AbstractQoreNode *f_get_word_32_lsb_bin(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   unsigned char *ptr = (unsigned char *)b->getPtr();
   int size = b->size();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt() * 4;

   if (offset >= (size - 3) || offset < 0)
      return 0;

   int64 val = LSBi4(*((unsigned int *)&ptr[offset]));

   return new QoreBigIntNode(val);
}

static AbstractQoreNode *f_get_word_64_lsb_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);
   unsigned char *ptr = (unsigned char *)str->getBuffer();
   int size = str->strlen();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt() * 8;

   if (offset >= (size - 7) || offset < 0)
      return 0;

   int64 val = LSBi8(*((int64 *)&ptr[offset]));
   return new QoreBigIntNode(val);
}

static AbstractQoreNode *f_get_word_64_lsb_bin(const QoreListNode *params, ExceptionSink *xsink) {   
   HARD_QORE_PARAM(b, const BinaryNode, params, 0);
   unsigned char *ptr = (unsigned char *)b->getPtr();
   int size = b->size();

   HARD_QORE_PARAM(p1, const QoreBigIntNode, params, 1);
   int offset = p1->getAsInt() * 8;

   if (offset >= (size - 7) || offset < 0)
      return 0;

   int64 val = LSBi8(*((int64 *)&ptr[offset]));

   return new QoreBigIntNode(val);
}

static AbstractQoreNode *f_splice_str_int(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(pstr, const QoreStringNode, params, 0);
   int start = (int)HARD_QORE_INT(params, 1);
   QoreStringNodeHolder str(pstr->copy());
   str->splice(start, xsink);
   return *xsink ? 0 : str.release();
}

static AbstractQoreNode *f_splice_str_int_int(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(pstr, const QoreStringNode, params, 0);
   int start = (int)HARD_QORE_INT(params, 1);
   int len = (int)HARD_QORE_INT(params, 2);
   const QoreStringNode *p3 = test_string_param(params, 3);
   QoreStringNodeHolder str(pstr->copy());
   str->splice(start, len, p3, xsink);
   return *xsink ? 0 : str.release();
}

static AbstractQoreNode *f_splice_list_int(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(lst, const QoreListNode, params, 0);
   int start = (int)HARD_QORE_INT(params, 1);
   ReferenceHolder<QoreListNode> l(lst->copy(), xsink);
   l->splice(start, xsink);
   return *xsink ? 0 : l.release();
}

static AbstractQoreNode *f_splice_list_int_int(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(lst, const QoreListNode, params, 0);
   int start = (int)HARD_QORE_INT(params, 1);
   ReferenceHolder<QoreListNode> l(lst->copy(), xsink);
   int len = (int)HARD_QORE_INT(params, 2);
   const QoreListNode *p3 = test_list_param(params, 3);
   l->splice(start, len, p3, xsink);
   return *xsink ? 0 : l.release();
}

static AbstractQoreNode *f_makeHexString_str(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   QoreStringNode *str = new QoreStringNode;
   str->concatHex(p0);
   return str;
}

static AbstractQoreNode *f_makeHexString_bin(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const BinaryNode, params, 0);
   QoreStringNode *str = new QoreStringNode;
   str->concatHex(p0);
   return str;
}

static AbstractQoreNode *f_parseHexString(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   return p0->parseHex(xsink);
}

// takes a hex string like "6d4f84e0" (with or without leading x or 0x) and returns the corresponding base-10 integer
static AbstractQoreNode *f_hextoint(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   if (!p0->strlen())
      return zero();

   int64 rc = 0;
   int64 pow = 0;
   const char *buf = p0->getBuffer();
   if (*buf == '0' && *(buf + 1) == 'x')
      buf += 2;
   else if (*buf == 'x')
      buf++;
   for (const char *p = p0->strlen() + buf - 1; p >= buf; p--) {
      int n = get_nibble(*p, xsink);
      if (xsink->isException())
	 return 0;
      if (!pow) {
	 rc = n;
	 pow = 16;
      }
      else {
	 rc += n * pow;
	 pow *= 16;
      }
   }
   return new QoreBigIntNode(rc);
}

// parses a string representing a number in a configurable base and returns the integer
static AbstractQoreNode *f_strtoint(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   int base = (int)HARD_QORE_INT(params, 1);
   return new QoreBigIntNode(strtoll(p0->getBuffer(), 0, base));
}

static AbstractQoreNode *f_load_module(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   MM.runTimeLoadModule(p0->getBuffer(), xsink);
   return 0;
}

static AbstractQoreNode *f_set_signal_handler(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_SIGNAL_HANDLING
   int signal = (int)HARD_QORE_INT(params, 0);
   if (!signal || signal > QORE_SIGNAL_MAX) {
      xsink->raiseException("SET-SIGNAL-HANDLER-ERROR", "%d is not a valid signal", signal);
      return 0;
   }

   HARD_QORE_PARAM(f, const ResolvedCallReferenceNode, params, 1);
   QoreSignalManager::setHandler(signal, f, xsink);
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "this platform does not support signal handling, therefore the set_signal_handler() and remove_signal_handler() functions are not available in Qore; for maximum portability, use the constant Option::HAVE_SIGNAL_HANDLING to check if this function is implemented before calling");
#endif
   return 0;
}

static AbstractQoreNode *f_remove_signal_handler(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_SIGNAL_HANDLING
   int signal = (int)HARD_QORE_INT(params, 0);
   if (!signal || signal > QORE_SIGNAL_MAX) {
      xsink->raiseException("REMOVE-SIGNAL-HANDLER-ERROR", "%d is not a valid signal", signal);
      return 0;
   }
   QoreSignalManager::removeHandler(signal, xsink);
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "this platform does not support signal handling, therefore the set_signal_handler() and remove_signal_handler() functions are not available in Qore; for maximum portability, use the constant Option::HAVE_SIGNAL_HANDLING to check if this function is implemented before calling");
#endif
   return 0;
}

// returns a string with percent-encodings substituted for characters
static AbstractQoreNode *f_decode_url(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p, const QoreStringNode, params, 0);
   QoreStringNode *str = new QoreStringNode(p->getEncoding());
   
   if (!p->strlen())
      return str;

   const char *c = p->getBuffer();

   while (*c) {
      if (*c == '%' && isxdigit(*(c + 1)) && isxdigit(*(c + 2))) {
         char x[3] = { *(c + 1), *(c + 2), '\0' };
         char code = strtol(x, 0, 16);
         str->concat(code);
         c += 3;
         continue;
      }
      str->concat(*c);
      c++;
   }
   return str;
}

static AbstractQoreNode *f_get_script_path(const QoreListNode *params, ExceptionSink *xsink) {
   return getProgram()->getScriptPath();
}

static AbstractQoreNode *f_get_script_dir(const QoreListNode *params, ExceptionSink *xsink) {
   return getProgram()->getScriptDir();
}

static AbstractQoreNode *f_get_script_name(const QoreListNode *params, ExceptionSink *xsink) {
   return getProgram()->getScriptName();
}

static AbstractQoreNode *f_get_qore_option_list(const QoreListNode *params, ExceptionSink *xsink) {
   static const char *tlist[] = { "OPTION", "ALGORITHM", "FUNCTION" };
   QoreListNode *l = new QoreListNode;

   for (unsigned j = 0; j < qore_option_list_size; ++j) {
      QoreHashNode *h = new QoreHashNode;
      h->setKeyValue("option", new QoreStringNode(qore_option_list[j].option), xsink);
      h->setKeyValue("constant", new QoreStringNode(qore_option_list[j].constant), xsink);
      h->setKeyValue("type", new QoreStringNode(tlist[qore_option_list[j].type]), xsink);
      h->setKeyValue("value", get_bool_node(qore_option_list[j].value), xsink);
      l->push(h);
   }
   return l;
}

static AbstractQoreNode *f_get_qore_library_info(const QoreListNode *params, ExceptionSink *xsink) {
   QoreHashNode *h = new QoreHashNode;

   h->setKeyValue("PlatformOS", new QoreStringNode(qore_target_os), xsink);
   h->setKeyValue("PlatformCPU", new QoreStringNode(qore_target_arch), xsink);
   h->setKeyValue("VersionString", new QoreStringNode(qore_version_string), xsink);   
   h->setKeyValue("VersionMajor", new QoreBigIntNode(qore_version_major), xsink);
   h->setKeyValue("VersionMinor", new QoreBigIntNode(qore_version_minor), xsink);
   h->setKeyValue("VersionSub", new QoreBigIntNode(qore_version_sub), xsink);
   h->setKeyValue("Build", new QoreStringNode(qore_build_number), xsink);
   h->setKeyValue("ModuleDir", new QoreStringNode(qore_module_dir), xsink);

   h->setKeyValue("BuildHost", new QoreStringNode(qore_build_host), xsink);
   h->setKeyValue("Compiler", new QoreStringNode(qore_cplusplus_compiler), xsink);
   h->setKeyValue("CFLAGS", new QoreStringNode(qore_cflags), xsink);
   h->setKeyValue("LDFLAGS", new QoreStringNode(qore_ldflags), xsink);

   return h;
}

void init_misc_functions() {
   // register builtin functions in this file
   builtinFunctions.add2("parse", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("parse", f_parse, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("call_function", f_call_function_str, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, 0, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("call_function", f_call_function_code, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, 0, 1, codeTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("call_function_args", f_call_function_args_str_list, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("call_function_args", f_call_function_args_str, QC_NO_FLAGS, QDOM_DEFAULT, 0, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("call_function_args", f_call_function_args_str_something, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, stringTypeInfo, QORE_PARAM_NO_ARG, somethingTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("call_function_args", f_call_function_args_code_list, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, codeTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("call_function_args", f_call_function_args_code, QC_NO_FLAGS, QDOM_DEFAULT, 0, 1, codeTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("call_function_args", f_call_function_args_code_something, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, codeTypeInfo, QORE_PARAM_NO_ARG, somethingTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("call_builtin_function", f_call_builtin_function, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, 0, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("call_builtin_function_args", f_call_builtin_function_args_list, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("call_builtin_function_args", f_call_builtin_function_args, QC_NO_FLAGS, QDOM_DEFAULT, 0, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("call_builtin_function_args", f_call_builtin_function_args_something, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, stringTypeInfo, QORE_PARAM_NO_ARG, somethingTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("exists", f_exists, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("existsFunction", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("existsFunction", f_bool_true_noop, QC_NOOP, QDOM_DEFAULT, boolTypeInfo, 1, codeTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("existsFunction", f_existsFunction_str, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("functionType", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   // functionType(string $name) returns *string
   builtinFunctions.add2("functionType", f_functionType, QC_CONSTANT, QDOM_DEFAULT, stringOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("html_encode", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("html_encode", f_html_encode, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   
   builtinFunctions.add2("html_decode", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("html_decode", f_html_decode, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_default_encoding", f_get_default_encoding, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo);

   builtinFunctions.add2("parse_url", f_parse_url, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("parseURL", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   // parseURL(string $url) returns *hash
   builtinFunctions.add2("parseURL", f_parseURL, QC_NO_FLAGS, QDOM_DEFAULT, hashOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("getClassName", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("getClassName", f_getClassName, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, objectTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("backquote", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("backquote", f_backquote, QC_NO_FLAGS, QDOM_EXTERNAL_PROCESS, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   
   builtinFunctions.add2("makeBase64String", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("makeBase64String", f_makeBase64String_str, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("makeBase64String", f_makeBase64String_bin, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("parseBase64String", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("parseBase64String", f_parseBase64String, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   
   builtinFunctions.add2("parseBase64StringToString", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("parseBase64StringToString", f_parseBase64StringToString, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   
   builtinFunctions.add2("getModuleList", f_getModuleList, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo);

   builtinFunctions.add2("getModuleHash", f_getModuleHash, QC_CONSTANT, QDOM_DEFAULT, hashTypeInfo);

   builtinFunctions.add2("getFeatureList", f_getFeatureList, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo);

   builtinFunctions.add2("hash_values", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("hash_values", f_hash_values, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo, 1, hashTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("compress", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("compress", f_compress_str, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(Z_DEFAULT_COMPRESSION));
   builtinFunctions.add2("compress", f_compress_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(Z_DEFAULT_COMPRESSION));

   // for backwards-compatibility, add compress() as compress2() as well
   builtinFunctions.add2("compress2", f_noop, QC_RUNTIME_NOOP | QC_DEPRECATED, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("compress2", f_compress_str, QC_DEPRECATED, QDOM_DEFAULT, binaryTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(Z_DEFAULT_COMPRESSION));
   builtinFunctions.add2("compress2", f_compress_bin, QC_DEPRECATED, QDOM_DEFAULT, binaryTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(Z_DEFAULT_COMPRESSION));

   builtinFunctions.add2("uncompress_to_string", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("uncompress_to_string", f_uncompress_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("uncompress_to_string", f_uncompress_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("uncompress_to_binary", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("uncompress_to_binary", f_uncompress_to_binary, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("gzip", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("gzip", f_gzip_str, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(Z_DEFAULT_COMPRESSION));
   builtinFunctions.add2("gzip", f_gzip_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(Z_DEFAULT_COMPRESSION));

   builtinFunctions.add2("gunzip_to_string", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("gunzip_to_string", f_gunzip_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("gunzip_to_string", f_gunzip_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("gunzip_to_binary", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("gunzip_to_binary", f_gunzip_to_binary, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("getByte", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   // getByte(string $str, softint $offset = 0) returns *int
   builtinFunctions.add2("getByte", f_get_byte_str, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   // getByte(binary $bin, softint $offset = 0) returns *int
   builtinFunctions.add2("getByte", f_get_byte_bin, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());

   builtinFunctions.add2("getWord32", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   // getWord32(string $str, softint $offset = 0) returns *int
   builtinFunctions.add2("getWord32", f_get_word_32_str, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   // getWord32(binary $bin, softint $offset = 0) returns *int
   builtinFunctions.add2("getWord32", f_get_word_32_bin, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());

   // with only hard typing
   builtinFunctions.add2("get_byte", f_get_byte_str, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   builtinFunctions.add2("get_byte", f_get_byte_bin, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());

   builtinFunctions.add2("get_word_16", f_get_word_16_str, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   builtinFunctions.add2("get_word_16", f_get_word_16_bin, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());

   builtinFunctions.add2("get_word_32", f_get_word_32_str, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   builtinFunctions.add2("get_word_32", f_get_word_32_bin, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());

   builtinFunctions.add2("get_word_64", f_get_word_64_str, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   builtinFunctions.add2("get_word_64", f_get_word_64_bin, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   
   builtinFunctions.add2("get_word_16_lsb", f_get_word_16_lsb_str, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   builtinFunctions.add2("get_word_16_lsb", f_get_word_16_lsb_bin, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());

   builtinFunctions.add2("get_word_32_lsb", f_get_word_32_lsb_str, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   builtinFunctions.add2("get_word_32_lsb", f_get_word_32_lsb_bin, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());

   builtinFunctions.add2("get_word_64_lsb", f_get_word_64_lsb_str, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   builtinFunctions.add2("get_word_64_lsb", f_get_word_64_lsb_bin, QC_CONSTANT, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());
   
   builtinFunctions.add2("splice", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("splice", f_string_noop, QC_NOOP, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("splice", f_splice_str_int, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("splice", f_splice_str_int_int, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 3, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("splice", f_splice_str_int_int, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 4, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("splice", f_list_noop, QC_NOOP, QDOM_DEFAULT, listTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("splice", f_splice_list_int, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 2, listTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("splice", f_splice_list_int_int, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 3, listTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("splice", f_splice_list_int_int, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 4, listTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("makeHexString", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("makeHexString", f_makeHexString_str, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("makeHexString", f_makeHexString_bin, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("parseHexString", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("parseHexString", f_parseHexString, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   
   builtinFunctions.add2("hextoint", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("hextoint", f_hextoint, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("strtoint", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("strtoint", f_strtoint, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(10));

   builtinFunctions.add2("load_module", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("load_module", f_load_module, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("set_signal_handler", f_set_signal_handler, QC_NO_FLAGS, QDOM_PROCESS, nothingTypeInfo, 2, softBigIntTypeInfo, QORE_PARAM_NO_ARG, codeTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("remove_signal_handler", f_remove_signal_handler, QC_NO_FLAGS, QDOM_PROCESS, nothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("decode_url", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("decode_url", f_decode_url, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_script_name", f_get_script_name, QC_NO_FLAGS, QDOM_DEFAULT);
   builtinFunctions.add2("get_script_dir", f_get_script_dir, QC_NO_FLAGS, QDOM_DEFAULT);
   builtinFunctions.add2("get_script_path", f_get_script_path, QC_NO_FLAGS, QDOM_DEFAULT);

   builtinFunctions.add2("get_qore_option_list", f_get_qore_option_list, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo);
   builtinFunctions.add2("get_qore_library_info", f_get_qore_library_info, QC_CONSTANT, QDOM_DEFAULT, hashTypeInfo);
   
   // deprecated with stupid capitalization
   builtinFunctions.add2("hexToInt", f_noop, QC_RUNTIME_NOOP | QC_DEPRECATED, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("hexToInt", f_hextoint, QC_DEPRECATED, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
}
