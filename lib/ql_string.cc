/*
  ql_string.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
#include <qore/intern/ql_string.h>

#include <stdlib.h>
#include <string.h>

// retuns number of characters in a string
static AbstractQoreNode *f_length(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   int l;

   if (p0->type == NT_BINARY)
      l = (reinterpret_cast<const BinaryNode *>(p0))->size();
   else
   {
      QoreStringValueHelper temp(p0);
      l = temp->length();
   }
   return new QoreBigIntNode(l);
}

// retuns number of bytes in a string
static AbstractQoreNode *f_strlen(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   QoreStringValueHelper temp(p0);
   return new QoreBigIntNode(temp->strlen());
}

static AbstractQoreNode *f_tolower(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
      return 0;

   QoreStringNode *rv = p0->copy();
   char *p = (char *)rv->getBuffer();
   while (*p)
   {
      *p = tolower(*p);
      p++;
   }
   return rv;
}

static AbstractQoreNode *f_toupper(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
      return 0;

   QoreStringNode *rv = p0->copy();
   char *p = (char *)rv->getBuffer();
   while (*p)
   {
      *p = toupper(*p);
      p++;
   }
   return rv;
}

// syntax substr(string, start[, length]) - note 1st character is 0, not 1
static AbstractQoreNode *f_substr(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0, *p1;

   if (is_nothing(p0 = get_param(params, 0)) || is_nothing(p1 = get_param(params, 1)))
      return 0;

   QoreStringNodeValueHelper temp(p0);
   const AbstractQoreNode *p2;
   if ((p2 = get_param(params, 2)))
      return temp->substr(p1->getAsInt(), p2->getAsInt());

   return temp->substr(p1->getAsInt());
}

static inline int index_intern(const char *haystack, const char *needle, int pos = 0)
{
   const char *p;
   if (!(p = strstr(haystack + pos, needle)))
      return -1;
   return (int)(p - haystack);
}

/* index(string, substring [, position])
 * returns the index position in characters (starting with 0) of the first occurrence
 * of substring within string, optionally starting at position if available
 * returns -1 if not found
 */
static AbstractQoreNode *f_index(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0, *p1, *p2;

   if (!(p0 = get_param(params, 0)) ||
       !(p1 = get_param(params, 1)))
      return new QoreBigIntNode(-1);

   QoreStringValueHelper hs(p0);
   QoreStringValueHelper t1(p1);

   p2 = get_param(params, 2);
   int pos = p2 ? p2->getAsInt() : 0;

   int ind;
   if (!hs->getEncoding()->isMultiByte())
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
	 ind = index_intern(hs->getBuffer(), t1->getBuffer(), pos);
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
	 ind = index_intern(hs->getBuffer() + start, t1->getBuffer());
	 if (ind != -1)
	    ind = hs->getEncoding()->getCharPos(hs->getBuffer(), hs->getBuffer() + start + ind);
      }
   }
   
   return new QoreBigIntNode(ind);
}

/* bindex(string, substring [, position])
 * returns the index position in bytes (starting with 0) of the first occurrence
 * of substring within string, optionally starting at position if available
 * returns -1 if not found
 * see index() for the character position
 */
static AbstractQoreNode *f_bindex(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0, *p1, *p2;

   if (!(p0 = get_param(params, 0)) ||
       !(p1 = get_param(params, 1)))
      return new QoreBigIntNode(-1);

   QoreStringValueHelper hs(p0);
   QoreStringValueHelper t1(p1);

   p2 = get_param(params, 2);
   int pos = p2 ? p2->getAsInt() : 0;

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

      ind = index_intern(hs->getBuffer(), t1->getBuffer(), pos);
   }
   
   return new QoreBigIntNode(ind);
}

// finds the last occurrence of needle in haystack at or before position pos
// pos must be a non-negative valid byte offset in haystack
static inline int rindex_intern(const char *haystack, int hlen, const char *needle, int nlen, int pos)
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
static AbstractQoreNode *f_brindex(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0, *p1, *p2;
 
   if (!(p0 = get_param(params, 0)) || !(p1 = get_param(params, 1)))
      return new QoreBigIntNode(-1);

   QoreStringValueHelper hs(p0);
   QoreStringValueHelper t1(p1);

   p2 = get_param(params, 2);
   int pos = p2 ? p2->getAsInt() : hs->strlen() - 1;

   int ind;
   if (pos < 0)
      pos = hs->strlen() + pos;
   if (pos < 0)
      ind = -1;
   else
      ind = rindex_intern(hs->getBuffer(), hs->strlen(), t1->getBuffer(), t1->strlen(), pos);      
   
   return new QoreBigIntNode(ind);
}

// syntax: rindex(string, substring, [pos])
static AbstractQoreNode *f_rindex(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0, *p1, *p2;

   if (!(p0 = get_param(params, 0)) || !(p1 = get_param(params, 1)))
      return new QoreBigIntNode(-1);

   QoreStringValueHelper hs(p0);
   QoreStringValueHelper t1(p1);

   p2 = get_param(params, 2);
   int pos = p2 ? p2->getAsInt() : -1;

   int ind;
   if (!hs->getEncoding()->isMultiByte())
   {
      if (pos == -1)
	 pos = hs->strlen() - 1;
      else if (pos < 0)
	 pos = hs->strlen() + pos;
      if (pos < 0)
	 ind = -1;
      else
	 ind = rindex_intern(hs->getBuffer(), hs->strlen(), t1->getBuffer(), t1->strlen(), pos);      
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
	 ind = rindex_intern(hs->getBuffer(), hs->strlen(), t1->getBuffer(), t1->strlen(), pos);
	 // calculate character position from byte position
	 if (ind && ind != -1)
	    ind = hs->getEncoding()->getCharPos(hs->getBuffer(), hs->getBuffer() + ind);
      }
   }

   return new QoreBigIntNode(ind);
}

// syntax: ord(string, [offset = 0])
// note that ord() only works on byte offsets and gives byte values
static AbstractQoreNode *f_ord(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;
   
   if (!(p0 = get_param(params, 0)))
      return NULL;
   
   QoreStringValueHelper str(p0);

   if (!str->strlen())
      return 0;

   const AbstractQoreNode *p1 = get_param(params, 1);
   int offset = p1 ? p1->getAsInt() : 0;

   if (offset >= str->strlen())
       return 0;

   return new QoreBigIntNode((str->getBuffer()[offset]));
}

static AbstractQoreNode *f_chr(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;
   
   return new QoreStringNode((char)p0->getAsInt());
}

// syntax: split(pattern, string);
static AbstractQoreNode *f_split(const QoreListNode *params, ExceptionSink *xsink)
{
   const char *str, *pattern;
   const QoreStringNode *p0, *p1;

   if (!(p0 = test_string_param(params, 0)) ||
       !(p1 = test_string_param(params, 1)))
      return new QoreListNode();

   // convert pattern encoding to string if necessary
   TempEncodingHelper temp(p0, p1->getEncoding(), xsink);
   if (*xsink)
      return 0;

   pattern = temp->getBuffer();
   str = p1->getBuffer();

   printd(5, "in f_split(\"%s\", \"%s\")\n", pattern, str);
   QoreListNode *l = new QoreListNode();
   while (const char *p = strstr(str, pattern))
   {
      //printd(5, "str=%08p p=%08p \"%s\" \"%s\"\n", str, p, str, pattern);
      l->push(new QoreStringNode(str, p - str, p1->getEncoding()));
      str = p + strlen(pattern);
   }
   // add last field
   if (*str)
      l->push(new QoreStringNode(str, p1->getEncoding()));
   return l;
}

static AbstractQoreNode *f_get_encoding(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return NULL;
   return new QoreStringNode(p0->getEncoding()->getCode());
}

// usage: convert_encoding(string, new_encoding);
static AbstractQoreNode *f_convert_encoding(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1;
   if (!(p0 = test_string_param(params, 0)) ||
       !(p1 = test_string_param(params, 1)))
      return NULL;

   return p0->convertEncoding(QEM.findCreate(p1), xsink);
}

static AbstractQoreNode *f_regex(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1;
   if (!(p0 = test_string_param(params, 0)) ||
       !(p1 = test_string_param(params, 1)))
       return NULL;
   
   const AbstractQoreNode *p2 = get_param(params, 2);
   int options = p2 ? p2->getAsInt() : 0;

   QoreRegexNode qr(p1, options, xsink);
   if (*xsink)
      return NULL;

   return new QoreBoolNode(qr.exec(p0, xsink));
}

// syntax: regex_subst(string, pattern, substitution_pattern, options)
static AbstractQoreNode *f_regex_subst(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1, *p2;
   if (!(p0 = test_string_param(params, 0)) ||
       !(p1 = test_string_param(params, 1)) ||
       !(p2 = test_string_param(params, 2)))
       return 0;

   const AbstractQoreNode *p3 = get_param(params, 3);
   int64 options = p3 ? p3->getAsBigInt() : 0;

   bool global;
   if (options & QRE_GLOBAL)
   {
      global = true;
      options &= 0xffffffff;
   }
   else
      global = false;

   RegexSubstNode qrs(p1, options, xsink);
   if (*xsink)
      return 0;

   if (global)
      qrs.setGlobal();
   return qrs.exec(p0, p2, xsink);
}

static AbstractQoreNode *f_regex_extract(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1;
   if (!(p0 = test_string_param(params, 0)) ||
       !(p1 = test_string_param(params, 1)))
      return 0;
   
   const AbstractQoreNode *p2 = get_param(params, 2);
   int options = p2 ? p2->getAsInt() : 0;
   
   QoreRegexNode qr(p1, options, xsink);
   if (*xsink)
      return 0;
   
   return qr.extractSubstrings(p0, xsink);
}

// usage: replace(string, substring, new substring)
static AbstractQoreNode *f_replace(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1, *p2;
   if (!(p0 = test_string_param(params, 0)) ||
       !(p1 = test_string_param(params, 1)) ||
       !(p2 = test_string_param(params, 2)))
      return 0;

   const QoreEncoding *ccs = p0->getEncoding();

   TempEncodingHelper t1(p1, ccs, xsink);
   if (*xsink)
      return 0;

   TempEncodingHelper t2(p2, ccs, xsink);
   if (*xsink)
      return 0;

   QoreStringNode *nstr = new QoreStringNode(ccs);   

   const char *str, *pattern;
   str = p0->getBuffer();
   pattern = t1->getBuffer();
   int plen = strlen(pattern);

   while (const char *p = strstr(str, pattern))
   {
      //printd(5, "str=%08p p=%08p '%s' '%s'->'%s'\n", str, p, str, pattern, t1->getBuffer());
      if (p != str)
	 nstr->concat(str, p - str);
      nstr->concat(*t2);

      str = p + plen;
   }
   // add last field
   if (*str)
      nstr->concat(str);
   
   return nstr;
}

// perl-style join function
static AbstractQoreNode *f_join(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return NULL;
   
   int n = num_params(params);
   if (n < 2)
      return NULL;

   const QoreListNode *l = test_list_param(params, 1);
   if (n == 2 && l)
      n = 0;
   else
   {
      n = 1;
      l = params;
   }

   QoreStringNode *str = new QoreStringNode();

   for (int i = n; i < l->size(); i++)
   {
      const AbstractQoreNode *p = l->retrieve_entry(i);
      if (p)
      {
	 QoreStringValueHelper t(p);	 
	 str->concat(*t);
      }
      if (i < (l->size() - 1))
	 str->concat(p0);
   }
   return str;
}

static AbstractQoreNode *f_chomp(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);   
   if (!p)
      return 0;

   const QoreType *ptype = p->getType();
   if (ptype == NT_STRING) {
      const QoreStringNode *pstr = reinterpret_cast<const QoreStringNode *>(p);
      QoreStringNode *str = pstr->copy();
      str->chomp();
      return str;
   } 
   if (ptype != NT_REFERENCE)
      return 0;

   const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(p);
   class AutoVLock vl;
   QoreStringNode **vp = get_string_var_value_ptr(r->lvexp, &vl, xsink);
   if (*xsink || !(*vp))
      return 0;
   if (!(*vp)->is_unique())
   {
      QoreStringNode *old = *vp;
      (*vp) = old->copy();
      old->deref();
   }
   (*vp)->chomp();
   (*vp)->ref();
   return *vp;
}

static AbstractQoreNode *f_trim(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);   
   if (!p0)
      return 0;
   const QoreStringNode *p1 = test_string_param(params, 1);
   const char *chars = p1 ? p1->getBuffer() : 0;

   const QoreType *p0_type = p0->getType();

   if (p0_type == NT_STRING) {
      const QoreStringNode *pstr = reinterpret_cast<const QoreStringNode *>(p0);
      QoreStringNode *str = pstr->copy();
      str->trim(chars);
      return str;
   }

   if (p0_type != NT_REFERENCE)
      return 0;

   const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(p0);

   class AutoVLock vl;
   QoreStringNode **vp = get_string_var_value_ptr(r->lvexp, &vl, xsink);
   if (*xsink || !(*vp))
      return 0;
   if (!(*vp)->is_unique())
   {
      QoreStringNode *old = *vp;
      (*vp) = old->copy();
      old->deref();
   }
   (*vp)->trim(chars);
   (*vp)->ref();
   return *vp;
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
   builtinFunctions.add("regex_extract", f_regex_extract);
   builtinFunctions.add("replace", f_replace);
   builtinFunctions.add("join", f_join);
   builtinFunctions.add("chomp", f_chomp);
   builtinFunctions.add("trim", f_trim);
}
