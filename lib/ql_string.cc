/*
  ql_string.cc

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
#include <qore/ql_string.h>
#include <qore/support.h>
#include <qore/QoreNode.h>
#include <qore/QoreString.h>
#include <qore/params.h>
#include <qore/charset.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/BinaryObject.h>
#include <qore/QoreRegex.h>
#include <qore/RegexSubst.h>

#include <stdlib.h>
#include <string.h>

// retuns number of characters in a string
static class QoreNode *f_length(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *temp, *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   int l;

   if (p0->type == NT_BINARY)
      l = p0->val.bin->size();
   else
   {
      if (p0->type == NT_STRING)
	 temp = p0;
      else
	 temp = p0->convert(NT_STRING);
      l = temp->val.String->length();
      if (temp != p0)
	 temp->deref(xsink);
   }
   return new QoreNode((int64)l);
}

// retuns number of bytes in a string
static class QoreNode *f_strlen(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *rv, *temp, *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   rv = new QoreNode(NT_INT);

   if (p0->type == NT_STRING)
      temp = p0;
   else
      temp = p0->convert(NT_STRING);
   rv->val.intval = temp->val.String->strlen();
   if (temp != p0)
      temp->deref(xsink);
   return rv;
}

static class QoreNode *f_tolower(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   char *p;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   QoreNode *rv = new QoreNode(new QoreString(p0->val.String));
   p = rv->val.String->getBuffer();
   while (*p)
   {
      *p = tolower(*p);
      p++;
   }
   return rv;
}

static class QoreNode *f_toupper(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   char *p;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   QoreNode *rv = new QoreNode(new QoreString(p0->val.String));
   printd(5, "f_toupper() p0->val.String=%08x (buf=%08x) rv->val.String=%08x (buf=%08x)\n",
	  p0->val.String, p0->val.String->getBuffer(),
	  rv->val.String, rv->val.String->getBuffer());
   p = rv->val.String->getBuffer();
   while (*p)
   {
      *p = toupper(*p);
      p++;
   }
   return rv;
}

// syntax substr(string, start[, length]) - note 1st character is 0, not 1
static class QoreNode *f_substr(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *temp, *p0, *p1, *p2, *rv;
   class QoreString *ns;

   tracein("f_substr()");
   if (!(p0 = get_param(params, 0)) ||
       !(p1 = get_param(params, 1)))
   {
      traceout("f_substr()");
      return NULL;
   }
   if (p0->type == NT_STRING)
      temp = p0;
   else
   {
      //printd(5, "f_substr() node->type=%s\n", p0 ? p0->type->name : "(null)");
      temp = p0->convert(NT_STRING);
   }
   if ((p2 = get_param(params, 2)))
      ns = temp->val.String->substr(p1->getAsInt(), p2->getAsInt());
   else
      ns = temp->val.String->substr(p1->getAsInt());
   if (temp != p0)
      temp->deref(xsink);
   if (ns)
      rv = new QoreNode(ns);
   else
      rv = NULL;
   traceout("f_substr()");
   return rv;
}

static inline int index_intern(char *haystack, char *needle, int pos = 0)
{
   char *p;
   if (!(p = strstr(haystack + pos, needle)))
      return -1;
   return (int)(p - haystack);
}

/* index(string, substring [, position])
 * returns the index position in characters (starting with 0) of the first occurrence
 * of substring within string, optionally starting at position if available
 * returns -1 if not found
 */
static class QoreNode *f_index(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *t0, *t1, *p0, *p1, *p2;

   if (!(p0 = get_param(params, 0)) ||
       !(p1 = get_param(params, 1)))
      return new QoreNode((int64)-1);

   t0 = (p0->type == NT_STRING) ? p0 : p0->convert(NT_STRING);
   t1 = (p1->type == NT_STRING) ? p1 : p1->convert(NT_STRING);

   p2 = get_param(params, 2);
   int pos = p2 ? p2->getAsInt() : 0;

   class QoreString *hs = t0->val.String;

   int ind;
   if (p0 != t0 || !hs->getEncoding() || !hs->getEncoding()->isMultiByte())
   {
      if (pos >= hs->strlen())
	 ind = -1;
      else
      {
	 if (pos < 0)
	 {
	    pos = hs->strlen() + pos;
	    if (pos < 0)
	       pos = 0;
	 }
	 ind = index_intern(hs->getBuffer(), t1->val.String->getBuffer(), pos);
      }
   }
   else // do multibyte index()
   {
      int start;
      if (pos < 0)
      {
	 pos = hs->length() + pos;
	 if (pos < 0)
	    pos = 0;
      }
      if (pos)
      {
	 start = hs->getEncoding()->getByteLen(hs->getBuffer(), pos);
	 if (start == hs->strlen())
	    ind = -1;
	 else
	    ind = 0;
      }
      else
      {
	 ind = 0;
	 start = 0;
      }
      if (ind != -1)
      {
	 ind = index_intern(hs->getBuffer() + start, t1->val.String->getBuffer());
	 if (ind != -1)
	    ind = hs->getEncoding()->getCharPos(hs->getBuffer(), hs->getBuffer() + start + ind);
      }
   }
   
   if (t0 != p0)
      t0->deref(xsink);
   if (t1 != p1)
      t1->deref(xsink);
   return new QoreNode((int64)ind);
}

/* bindex(string, substring [, position])
 * returns the index position in bytes (starting with 0) of the first occurrence
 * of substring within string, optionally starting at position if available
 * returns -1 if not found
 * see index() for the character position
 */
static class QoreNode *f_bindex(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *t0, *t1, *p0, *p1, *p2;

   if (!(p0 = get_param(params, 0)) ||
       !(p1 = get_param(params, 1)))
      return new QoreNode((int64)-1);

   t0 = (p0->type == NT_STRING) ? p0 : p0->convert(NT_STRING);
   t1 = (p1->type == NT_STRING) ? p1 : p1->convert(NT_STRING);

   p2 = get_param(params, 2);
   int pos = p2 ? p2->getAsInt() : 0;

   class QoreString *hs = t0->val.String;
   int ind;
   if (pos >= hs->strlen())
      ind = -1;
   else
   {
      if (pos < 0)
      {
	 pos = hs->strlen() + pos;
	 if (pos < 0)
	    pos = 0;
      }

      ind = index_intern(hs->getBuffer(), t1->val.String->getBuffer(), pos);
   }
   
   if (t0 != p0)
      t0->deref(xsink);
   if (t1 != p1)
      t1->deref(xsink);
   return new QoreNode((int64)ind);
}

// finds the last occurrence of needle in haystack at or before position pos
// pos must be a non-negative valid byte offset in haystack
static inline int rindex_intern(char *haystack, int hlen, char *needle, int nlen, int pos)
{
   // if the offset does not allow for the needle string to be present, then adjust
   if ((pos + nlen) > hlen)
   {
      pos = hlen - nlen;
      if (pos < 0)
	 return -1;
   }

   while (pos != -1)
   {
      if (!strncmp(haystack + pos, needle, nlen))
	 return pos;
      pos--;
   }
   return -1;
}

/* brindex(string, substring [, position])
 * returns the index position in bytes (starting with 0) of the first occurrence
 * of substring within string, searching from the end of the string
 */
static class QoreNode *f_brindex(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *t0, *t1, *p0, *p1, *p2;
 
   if (!(p0 = get_param(params, 0)) || !(p1 = get_param(params, 1)))
      return new QoreNode((int64)-1);

   t0 = (p0->type == NT_STRING) ? p0 : p0->convert(NT_STRING);
   t1 = (p1->type == NT_STRING) ? p1 : p1->convert(NT_STRING);

   class QoreString *hs = t0->val.String;
   p2 = get_param(params, 2);
   int pos = p2 ? p2->getAsInt() : hs->strlen() - 1;

   int ind;
   if (pos < 0)
      pos = hs->strlen() + pos;
   if (pos < 0)
      ind = -1;
   else
      ind = rindex_intern(hs->getBuffer(), hs->strlen(), t1->val.String->getBuffer(), t1->val.String->strlen(), pos);      
   
   if (t0 != p0)
      t0->deref(xsink);
   if (t1 != p1)
      t1->deref(xsink);
   return new QoreNode((int64)ind);
}

// syntax: rindex(string, substring, [pos])
static class QoreNode *f_rindex(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *t0, *t1, *p0, *p1, *p2;

   if (!(p0 = get_param(params, 0)) || !(p1 = get_param(params, 1)))
      return new QoreNode((int64)-1);

   t0 = (p0->type == NT_STRING) ? p0 : p0->convert(NT_STRING);
   t1 = (p1->type == NT_STRING) ? p1 : p1->convert(NT_STRING);
   p2 = get_param(params, 2);

   int pos = p2 ? p2->getAsInt() : -1;

   class QoreString *hs = t0->val.String;

   int ind;
   if (p0 != t0 || !hs->getEncoding() || !hs->getEncoding()->isMultiByte())
   {
      if (pos == -1)
	 pos = hs->strlen() - 1;
      else if (pos < 0)
	 pos = hs->strlen() + pos;
      if (pos < 0)
	 ind = -1;
      else
	 ind = rindex_intern(hs->getBuffer(), hs->strlen(), t1->val.String->getBuffer(), t1->val.String->strlen(), pos);      
   }
   else // do multi-byte rindex
   {
      int l = hs->length();
      if (pos == -1)
	 pos = l - 1;
      else if (pos < 0)
	 pos = l + pos;
      if (pos < 0)
	 ind = -1;
      else
      {
	 // calculate byte position from character position
	 if (pos)
	    pos = hs->getEncoding()->getByteLen(hs->getBuffer(), pos);
	 // get byte rindex position
	 ind = rindex_intern(hs->getBuffer(), hs->strlen(), t1->val.String->getBuffer(), t1->val.String->strlen(), pos);
	 // calculate character position from byte position
	 if (ind && ind != -1)
	    ind = hs->getEncoding()->getCharPos(hs->getBuffer(), hs->getBuffer() + ind);
      }
   }

   if (t0 != p0)
      t0->deref(xsink);
   if (t1 != p1)
      t1->deref(xsink);
   return new QoreNode((int64)ind);
}

// syntax: ord(string, [offset = 0])
// note that ord() only works on byte offsets and gives byte values
static class QoreNode *f_ord(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *temp, *p0;
   
   if (!(p0 = get_param(params, 0)))
      return NULL;

   temp = (p0->type == NT_STRING) ? p0 : p0->convert(NT_STRING);

   if (!temp->val.String->strlen())
   {
      if (temp != p0)
	 temp->deref(xsink);
      return NULL;
   }

   class QoreString *str = temp->val.String;
   
   class QoreNode *p1 = get_param(params, 1);
   int offset = p1 ? p1->getAsInt() : 0;

   if (offset >= str->strlen())
   {
      if (temp != p0)
	 temp->deref(xsink);
       return NULL;
   }

   int c = (unsigned char)str->getBuffer()[offset];
   if (temp != p0)
      temp->deref(xsink);
   return new QoreNode((int64)c);
}

static class QoreNode *f_chr(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(new QoreString((char)p0->getAsInt()));
}

// syntax: split(pattern, string);
static class QoreNode *f_split(class QoreNode *params, ExceptionSink *xsink)
{
   char *str, *pattern;
   QoreNode *p0, *p1;

   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)))
      return new QoreNode(new List());
   pattern = p0->val.String->getBuffer();
   str = p1->val.String->getBuffer();
   printd(5, "in f_split(\"%s\", \"%s\")\n", pattern, str);
   QoreNode *rv = new QoreNode(NT_LIST);
   rv->val.list = new List();
   while (char *p = strstr(str, pattern))
   {
      //printd(5, "str=%08x p=%08x \"%s\" \"%s\"\n", str, p, str, pattern);
      rv->val.list->push(new QoreNode(new QoreString(str, p - str, p1->val.String->getEncoding())));
      str = p + strlen(pattern);
   }
   // add last field
   if (*str)
      rv->val.list->push(new QoreNode(new QoreString(str, p1->val.String->getEncoding())));
   return rv;
}

static class QoreNode *f_get_encoding(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
      return NULL;
   return new QoreNode(new QoreString(p0->val.String->getEncoding() ? p0->val.String->getEncoding()->code : "(unknown)"));
}

// usage: convert_encoding(string, new_encoding);
static class QoreNode *f_convert_encoding(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;
   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)))
      return NULL;
   QoreString *ns = p0->val.String->convertEncoding(QEM.findCreate(p1->val.String), xsink);
   if (ns)
      return new QoreNode(ns);
   return NULL;
}

static class QoreNode *f_regex(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1, *p2;
   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)))
      return NULL;

   p2 = get_param(params, 2);
   int options = p2 ? p2->getAsInt() : 0;

   QoreRegex qr(p1->val.String, options, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreNode(qr.exec(p0->val.String, xsink));
}

// syntax: regex_subst(string, pattern, substitution_pattern, options)
static class QoreNode *f_regex_subst(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1, *p2, *p3;
   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)) ||
       !(p2 = test_param(params, NT_STRING, 2)))
      return NULL;

   p3 = get_param(params, 3);
   int64 options = p3 ? p3->getAsBigInt() : 0;
   bool global;
   if (options & QRE_GLOBAL)
   {
      global = true;
      options &= 0xffffffff;
   }
   else
      global = false;

   class RegexSubst qrs(p1->val.String, options, xsink);
   if (xsink->isEvent())
      return NULL;

   if (global)
      qrs.setGlobal();
   return new QoreNode(qrs.exec(p0->val.String, p2->val.String, xsink));
}

// usage: replace(string, substring, new substring)
static class QoreNode *f_replace(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1, *p2;
   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)) ||
       !(p2 = test_param(params, NT_STRING, 2)))
      return NULL;
   class QoreEncoding *ccs = p0->val.String->getEncoding();

   QoreString *nstr = new QoreString(ccs);
   
   QoreString *t1, *t2;

   if (ccs)
   {
      if (p1->val.String->getEncoding() != ccs
	  && p1->val.String->getEncoding())
      {
	 t1 = p1->val.String->convertEncoding(ccs, xsink);
	 if (xsink->isEvent())
	    return NULL;
      }
      else
	 t1 = p1->val.String;

      if (p2->val.String->getEncoding() != ccs
	  && p2->val.String->getEncoding())
      {
	 t2 = p2->val.String->convertEncoding(ccs, xsink);
	 if (xsink->isEvent())
	 {
	    if (t1 != p1->val.String)
	       delete t1;
	    return NULL;
	 }
      }
      else
	 t2 = p2->val.String;
   }
   else
   {
      t1 = p1->val.String;
      t2 = p2->val.String;
   }

   char *str, *pattern;
   str = p0->val.String->getBuffer();
   pattern = t1->getBuffer();
   int plen = strlen(pattern);

   while (char *p = strstr(str, pattern))
   {
      //printd(5, "str=%08x p=%08x '%s' '%s'->'%s'\n", str, p, str, pattern, t1->getBuffer());
      if (p != str)
	 nstr->concat(str, p - str);
      nstr->concat(t2);

      str = p + plen;
   }
   // add last field
   if (*str)
      nstr->concat(str);

   if (t1 != p1->val.String)
      delete t1;
   if (t2 != p2->val.String)
      delete t2;
   
   return new QoreNode(nstr);
}

// perl-style join function
static class QoreNode *f_join(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
      return NULL;
   
   int n = num_params(params);
   if (n < 2)
      return NULL;

   class QoreNode *p1 = test_param(params, NT_LIST, 1);
   class List *l;
   if (n == 2 && p1)
   {
      n = 0;
      l = p1->val.list;
   }
   else
   {
      n = 1;
      l = params->val.list;
   }
   class QoreString *str = new QoreString();
   for (int i = n; i < l->size(); i++)
   {
      class QoreNode *p = l->retrieve_entry(i);
      if (p)
      {
	 class QoreNode *t;
	 if (p->type != NT_STRING)
	    t = p->convert(NT_STRING);
	 else
	    t = p;
	 
	 str->concat(t->val.String);
	 if (t != p)
	    t->deref(xsink);
      }
      if (i < (l->size() - 1))
	 str->concat(p0->val.String);
   }
   return new QoreNode(str);
}

void init_string_functions()
{
   builtinFunctions.add("length", f_length);
   builtinFunctions.add("strlen", f_strlen);
   builtinFunctions.add("tolower", f_tolower);
   builtinFunctions.add("toupper", f_toupper);
   builtinFunctions.add("substr", f_substr);
   builtinFunctions.add("index", f_index);
   builtinFunctions.add("bindex", f_bindex);
   builtinFunctions.add("rindex", f_rindex);
   builtinFunctions.add("brindex", f_brindex);
   builtinFunctions.add("ord", f_ord);
   builtinFunctions.add("chr", f_chr);
   builtinFunctions.add("split", f_split);
   builtinFunctions.add("get_encoding", f_get_encoding);
   builtinFunctions.add("convert_encoding", f_convert_encoding);
   builtinFunctions.add("regex", f_regex);
   builtinFunctions.add("regex_subst", f_regex_subst);
   builtinFunctions.add("replace", f_replace);
   builtinFunctions.add("join", f_join);
}
