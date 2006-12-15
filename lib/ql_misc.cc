/*
  ql_misc.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/QoreNode.h>
#include <qore/support.h>
#include <qore/Exception.h>
#include <qore/Function.h>
#include <qore/params.h>
#include <qore/charset.h>
#include <qore/QoreProgram.h>
#include <qore/QoreNet.h>
#include <qore/ModuleManager.h>
#include <qore/Qore.h>
#include <qore/ql_misc.h>
#include <qore/QT_backquote.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/ssl_constants.h>
#include <qore/BinaryObject.h>

#include <string.h>
#include <zlib.h>

static class QoreNode *f_sort(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *lst = test_param(params, NT_LIST, 0);
   if (!lst)
      return NULL;

   return lst->val.list->sort();
}

static class QoreNode *f_sortDescending(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *lst = test_param(params, NT_LIST, 0);
   if (!lst)
      return NULL;

   return lst->val.list->sortDescending();
}

// FIXME: don't copy the list - the arguments have already been evaluated
// just build a new list and then zero it out before deleting it
static class QoreNode *f_call_function(class QoreNode *params, ExceptionSink *xsink)
{
   char *fname;
   QoreNode *args = NULL, *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("CALL-FUNCTION-PARAMETER-ERROR",
			 "invalid arguments passed to call_function(), must be either function name or function name plus argument list");
      return NULL;
   }
   fname = p0->val.String->getBuffer();

   // if there are arguments to pass
   if (get_param(params, 1))
   {
      // create argument list by copying current list
      args = params->val.list->evalFrom(1, xsink);
      if (xsink->isEvent())
      {
	 if (args)
	    args->deref(xsink);
	 return NULL;
      }
   }

   class QoreNode *rv = getProgram()->callFunction(fname, args, xsink);

   if (args)
      args->deref(xsink);

   return rv;
}

static class QoreNode *f_call_function_args(class QoreNode *params, ExceptionSink *xsink)
{
   char *fname;
   class QoreNode *p0, *p1, *args;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("CALL-FUNCTION-ARGS-PARAMETER-ERROR",
			    "invalid arguments passed to call_function_args(), must be either function name or function name plus argument list");
      return NULL;
   }
   fname = p0->val.String->getBuffer();

   p1 = get_param(params, 1);
   if (p1 && p1->type == NT_LIST)
      args = p1;
   else if (p1)
   {
      args = new QoreNode(new List());
      args->val.list->push(p1);
   }
   else
      args = NULL;

   class QoreNode *rv = getProgram()->callFunction(fname, args, xsink);

   if (p1 != args)
   {
      args->val.list->shift();
      args->deref(xsink);
   }

   return rv;
}

static class QoreNode *f_existsFunction(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   if (getProgram()->existsFunction(p0->val.String->getBuffer()))
      return boolean_true();
   if (builtinFunctions.find(p0->val.String->getBuffer()))
      return boolean_true();
   return boolean_false();
}

// FIXME: should probably return constants
static class QoreNode *f_functionType(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   if (getProgram()->existsFunction(p0->val.String->getBuffer()))
      return new QoreNode("user");
   if (builtinFunctions.find(p0->val.String->getBuffer()))
      return new QoreNode("builtin");
   return NULL;
}

static class QoreNode *f_html_encode(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   class QoreString *ns = new QoreString(p0->val.String->getEncoding());
   char *str = p0->val.String->getBuffer();
   for (int i = 0; i < p0->val.String->strlen(); i++)
   {
      int j;
      for (j = 0; j < (int)NUM_HTML_CODES; j++)
	 if (str[i] == QoreString::html_codes[j].symbol)
	 {
	    ns->concat(QoreString::html_codes[j].code);
	    break;
	 }
      if (j == NUM_HTML_CODES)
	 ns->concat(str[i]);
   }
   return new QoreNode(ns);
}

static class QoreNode *f_html_decode(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   QoreString *ns = new QoreString(p0->val.String->getEncoding());
   char *str = p0->val.String->getBuffer();
   int i = 0;
   int len = p0->val.String->strlen();
   while (str[i])
   {
      int j;
      for (j = 0; j < (int)NUM_HTML_CODES; j++)
	 if ((len - i) >= QoreString::html_codes[j].len && 
	     !strncmp(QoreString::html_codes[j].code, &str[i], QoreString::html_codes[j].len))
	 {
	    ns->concat(QoreString::html_codes[j].symbol);
	    i += QoreString::html_codes[j].len;
	    break;
	 }
      if (j == NUM_HTML_CODES)
      {
	 ns->concat(str[i]);
	 i++;
      }
   }
   return new QoreNode(ns);
}

static class QoreNode *f_get_default_encoding(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(QCS_DEFAULT->code);
}

class QoreNode *f_parse(class QoreNode *params, ExceptionSink *xsink)
{
   tracein("f_parse()");

   QoreNode *p0, *p1;

   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)))
   {
      traceout("f_parse()");
      return NULL;
   }
   QoreProgram *pgm = getProgram();

   pgm->parse(p0->val.String, p1->val.String, xsink);

   traceout("f_parse()");
   return NULL;
}

static class QoreNode *f_getClassName(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   if (!p0)
      return NULL;

   return new QoreNode(p0->val.object->getClass()->getName());
}

static class QoreNode *f_parseURL(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
      return NULL;

   QoreURL url(p0->val.String);
   if (url.isValid())
      return new QoreNode(url.getHash());

   return NULL;
}

static class QoreNode *f_backquote(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
      return NULL;

   return backquoteEval(p0->val.String->getBuffer(), xsink);
}

static class QoreNode *f_makeBase64String(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0)
      return NULL;

   if (p0->type == NT_BINARY)
      return new QoreNode(new QoreString(p0->val.bin));

   if (p0->type != NT_STRING)
      return NULL;

   class QoreString *str = new QoreString();
   str->concatBase64(p0->val.String);
   return new QoreNode(str);
}

static class QoreNode *f_parseBase64String(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
      return NULL;

   class BinaryObject *b = p0->val.String->parseBase64(xsink);
   if (xsink->isEvent())
      return NULL;
   return new QoreNode(b);
}

static class QoreNode *f_getModuleList(class QoreNode *params, ExceptionSink *xsink)
{
   List *l = MM.getModuleList();
   if (l)
      return new QoreNode(l);
   return NULL;
}

static class QoreNode *f_getFeatureList(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(getProgram()->getFeatureList());
}

static class QoreNode *f_hash_values(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_HASH, 0);
   if (!p0)
      return NULL;

   class List *l = new List();
   class HashIterator hi(p0->val.hash);
   while (hi.next() && !xsink->isEvent())
      l->push(hi.eval(xsink));

   if (xsink->isEvent())
   {
      l->derefAndDelete(xsink);
      return NULL;
   }
   return new QoreNode(l);
}

static class QoreNode *f_compress(class QoreNode *params, ExceptionSink *xsink)
{
   // need a string or binary argument
   QoreNode *p0 = get_param(params, 0);
   if (!p0)
      return NULL;

   void *ptr;
   unsigned long len;
   if (p0->type == NT_STRING)
   {
      ptr = p0->val.String->getBuffer();
      len = p0->val.String->strlen() + 1;
   }
   else if (p0->type == NT_BINARY)
   {
      ptr = p0->val.bin->getPtr();
      len = p0->val.bin->size();
   }
   else
      return NULL;

   if (!ptr || !len)
      return NULL;

   // allocate new buffer
   unsigned long blen = len + (len / 10) + 12;
   void *buf = malloc(blen);
   int rc = compress((Bytef *)buf, &blen, (Bytef *)ptr, len);

   if (rc != Z_OK)
   {
      xsink->raiseException("ZLIB-COMPRESS-ERROR", "compress() returned error code %d", rc);
      free(buf);
      return NULL;
   }

   return new QoreNode(new BinaryObject(buf, blen));
}

static class QoreNode *f_compress2(class QoreNode *params, ExceptionSink *xsink)
{
   // need a string or binary argument
   QoreNode *p0 = get_param(params, 0);
   if (!p0)
      return NULL;

   QoreNode *p1 = get_param(params, 1);
   int level = p1 ? p1->getAsInt() : 0;

   if (level > 9)
   {
      xsink->raiseException("ZLIB-LEVEL-ERROR", "level must be between 0 - 9 (value passed: %d)", level);
      return NULL;
   }

   void *ptr;
   unsigned long len;
   if (p0->type == NT_STRING)
   {
      ptr = p0->val.String->getBuffer();
      len = p0->val.String->strlen() + 1;
   }
   else if (p0->type == NT_BINARY)
   {
      ptr = p0->val.bin->getPtr();
      len = p0->val.bin->size();
   }
   else
      return NULL;

   if (!ptr || !len)
      return NULL;

   // allocate new buffer
   unsigned long blen = len + (len / 10) + 12;
   void *buf = malloc(blen);
   int rc = compress2((Bytef *)buf, &blen, (Bytef *)ptr, len, level);

   if (rc != Z_OK)
   {
      xsink->raiseException("ZLIB-COMPRESS2-ERROR", "compress2() returned error code %d", rc);
      free(buf);
      return NULL;
   }

   return new QoreNode(new BinaryObject(buf, blen));
}

// syntax: binary object, new string length
// returns -1 for buffer too small
static class QoreNode *f_uncompress_to_string(class QoreNode *params, ExceptionSink *xsink)
{
   // need binary argument
   QoreNode *p0 = test_param(params, NT_BINARY, 0);
   if (!p0)
      return NULL;

   QoreNode *p1 = get_param(params, 1);
   unsigned long blen = p1 ? p1->getAsInt() : 0;
   if (!blen)
      return new QoreNode((int64)-1);

   // allocate new buffer
   void *buf = malloc(blen);
   int rc = uncompress((Bytef *)buf, &blen, (Bytef *)p0->val.bin->getPtr(), p0->val.bin->size());

   if (rc == Z_BUF_ERROR)
   {
      free(buf);
      return new QoreNode((int64)-1);
   }

   if (rc != Z_OK)
   {
      xsink->raiseException("ZLIB-UNCOMPRESS-ERROR", "uncompress() returned error code %d", rc);
      free(buf);
      return NULL;
   }

   class QoreString *str = new QoreString();
   str->take((char *)buf);
   return new QoreNode(str);
}

// syntax: binary object, new binary buffer size
// returns -1 for buffer too small
static class QoreNode *f_uncompress_to_binary(class QoreNode *params, ExceptionSink *xsink)
{
   // need binary argument
   QoreNode *p0 = test_param(params, NT_BINARY, 0);
   if (!p0)
      return NULL;

   QoreNode *p1 = get_param(params, 1);
   unsigned long blen = p1 ? p1->getAsInt() : 0;
   if (!blen)
      return new QoreNode((int64)-1);

   // allocate new buffer
   void *buf = malloc(blen);
   int rc = uncompress((Bytef *)buf, &blen, (Bytef *)p0->val.bin->getPtr(), p0->val.bin->size());

   if (rc == Z_BUF_ERROR)
   {
      free(buf);
      return new QoreNode((int64)-1);
   }

   if (rc != Z_OK)
   {
      xsink->raiseException("ZLIB-UNCOMPRESS-ERROR", "uncompress() returned error code %d", rc);
      free(buf);
      return NULL;
   }

   return new QoreNode(new BinaryObject(buf, blen));
}

static class QoreNode *f_getByte(class QoreNode *params, ExceptionSink *xsink)
{
   // need binary argument
   QoreNode *p0 = get_param(params, 0);
   if (!p0)
      return NULL;
   unsigned char *ptr;
   int size;
   if (p0->type == NT_BINARY)
   {
      ptr = (unsigned char *)p0->val.bin->getPtr();
      size = p0->val.bin->size();
   }
   else if (p0->type == NT_STRING)
   {
      ptr = (unsigned char *)p0->val.String->getBuffer();
      size = p0->val.String->strlen();
   }
   else
      return NULL;
   QoreNode *p1 = get_param(params, 1);
   int offset = p1 ? p1->getAsInt() : 0;
   if (!ptr || offset >= size || offset < 0)
      return NULL;

   return new QoreNode((int64)ptr[offset]);  
}

// same as splice operator, but operates on a copy of the list
static class QoreNode *f_splice(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0)
      return NULL;
   QoreNode *p1, *p2, *p3, *rv;
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);
   p3 = get_param(params, 3);
   int start = p1->getAsInt();
   if (p0->type == NT_LIST)
   {
      List *l = p0->val.list->copyList();

      if (is_nothing(p3))
	 if (is_nothing(p2))
	    l->splice(start, xsink);
	 else
	    l->splice(start, p2->getAsInt(), xsink);
      else
	 l->splice(start, p2->getAsInt(), p3, xsink);
      rv = new QoreNode(l);
   }
   else if (p0->type == NT_STRING)
   {
      QoreString *str = p0->val.String->copy();

      if (!p3 || p3->type != NT_STRING)
	 if (is_nothing(p2))
	    str->splice(start, xsink);
	 else
	    str->splice(start, p2->getAsInt(), xsink);
      else
	 str->splice(start, p2->getAsInt(), p3, xsink);
      rv = new QoreNode(str);
   }
   else
      return NULL;

   if (xsink->isEvent())
   {
      rv->deref(xsink);
      return NULL;
   }
   return rv;
}

static class QoreNode *f_makeHexString(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->type != NT_BINARY && p0->type != NT_STRING))
      return NULL;

   class QoreString *str = new QoreString();
   if (p0->type == NT_STRING)
     str->concatHex(p0->val.String);
   else
     str->concatHex(p0->val.bin);
   return new QoreNode(str);
}

static class QoreNode *f_parseHexString(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
      return NULL;

   class BinaryObject *b = p0->val.String->parseHex(xsink);
   if (!b)
      return NULL;
   return new QoreNode(b);
}


void init_misc_functions()
{
   // register builtin functions in this file
   builtinFunctions.add("parse", f_parse);
   builtinFunctions.add("call_function", f_call_function);
   builtinFunctions.add("call_function_args", f_call_function_args);
   builtinFunctions.add("existsFunction", f_existsFunction);
   builtinFunctions.add("functionType", f_functionType);
   builtinFunctions.add("html_encode", f_html_encode);
   builtinFunctions.add("html_decode", f_html_decode);
   builtinFunctions.add("get_default_encoding", f_get_default_encoding);
   builtinFunctions.add("parseURL", f_parseURL);
   builtinFunctions.add("getClassName", f_getClassName);
   builtinFunctions.add("backquote", f_backquote);
   builtinFunctions.add("sort", f_sort);
   builtinFunctions.add("sortDescending", f_sortDescending);
   builtinFunctions.add("parseBase64String", f_parseBase64String);
   builtinFunctions.add("makeBase64String", f_makeBase64String);
   builtinFunctions.add("parseHexString", f_parseHexString);
   builtinFunctions.add("makeHexString", f_makeHexString);
   builtinFunctions.add("getModuleList", f_getModuleList);
   builtinFunctions.add("getFeatureList", f_getFeatureList);
   builtinFunctions.add("hash_values", f_hash_values);
   builtinFunctions.add("compress", f_compress);
   builtinFunctions.add("compress2", f_compress2);
   builtinFunctions.add("uncompress_to_string", f_uncompress_to_string);
   builtinFunctions.add("uncompress_to_binary", f_uncompress_to_binary);
   builtinFunctions.add("getByte", f_getByte);
   builtinFunctions.add("splice", f_splice);
}
