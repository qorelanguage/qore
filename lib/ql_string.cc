/*
  ql_string.cc

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
#include <qore/intern/ql_string.h>

#include <stdlib.h>
#include <string.h>

// retuns number of characters in a string
static AbstractQoreNode *f_length(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   int l;

   if (p0->getType() == NT_BINARY)
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
      return 0;

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
static AbstractQoreNode *f_substr(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0, *p1;

   if (is_nothing(p0 = get_param(params, 0)) || is_nothing(p1 = get_param(params, 1)))
      return 0;

   QoreStringNodeValueHelper temp(p0);
   const AbstractQoreNode *p2;
   if ((p2 = get_param(params, 2)))
      return temp->substr(p1->getAsInt(), p2->getAsInt(), xsink);

   return temp->substr(p1->getAsInt(), xsink);
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
static AbstractQoreNode *f_index(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0, *p1, *p2;

   if (!(p0 = get_param(params, 0)) ||
       !(p1 = get_param(params, 1)))
      return new QoreBigIntNode(-1);

   QoreStringValueHelper hs(p0);
   QoreStringValueHelper t1(p1);

   p2 = get_param(params, 2);
   int pos = p2 ? p2->getAsInt() : 0;

   int ind;
   if (!hs->getEncoding()->isMultiByte()) {
      if (pos < 0) {
	 pos = hs->strlen() + pos;
	 if (pos < 0)
	    pos = 0;
	 ind = index_intern(hs->getBuffer(), t1->getBuffer(), pos);
      }
      else {
	 if ((qore_size_t)pos >= hs->strlen())
	    ind = -1;
	 else
	    ind = index_intern(hs->getBuffer(), t1->getBuffer(), pos);
      }
   }
   else { // do multibyte index()
      qore_size_t start;
      if (pos < 0) {
	 pos = hs->length() + pos;
	 if (pos < 0)
	    pos = 0;
      }
      if (pos) {
	 start = hs->getEncoding()->getByteLen(hs->getBuffer(), hs->getBuffer() + hs->strlen(), pos, xsink);
	 if (*xsink)
	    return 0;
	 if (start == hs->strlen())
	    ind = -1;
	 else
	    ind = 0;
      }
      else {
	 ind = 0;
	 start = 0;
      }
      if (ind != -1) {
	 ind = index_intern(hs->getBuffer() + start, t1->getBuffer());
	 if (ind != -1) {
	    ind = hs->getEncoding()->getCharPos(hs->getBuffer(), hs->getBuffer() + start + ind, xsink);
	    if (*xsink)
	       return 0;
	 }
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
   if (pos < 0) {
      pos = hs->strlen() + pos;
      if (pos < 0)
	 pos = 0;
      ind = index_intern(hs->getBuffer(), t1->getBuffer(), pos);
   }
   else {
      if ((qore_size_t)pos >= hs->strlen())
	 ind = -1;
      else
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
   else { // do multi-byte rindex
      int l = hs->length();
      if (pos == -1)
	 pos = l - 1;
      else if (pos < 0)
	 pos = l + pos;
      if (pos < 0)
	 ind = -1;
      else {
	 // calculate byte position from character position
	 if (pos) {
	    pos = hs->getEncoding()->getByteLen(hs->getBuffer(), hs->getBuffer() + hs->strlen(), pos, xsink);
	    if (*xsink)
	       return 0;
	 }
	 // get byte rindex position
	 ind = rindex_intern(hs->getBuffer(), hs->strlen(), t1->getBuffer(), t1->strlen(), pos);
	 // calculate character position from byte position
	 if (ind && ind != -1) {
	    ind = hs->getEncoding()->getCharPos(hs->getBuffer(), hs->getBuffer() + ind, xsink);
	    if (*xsink)
	       return 0;
	 }
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
      return 0;
   
   QoreStringValueHelper str(p0);

   if (!str->strlen())
      return 0;

   const AbstractQoreNode *p1 = get_param(params, 1);
   qore_size_t offset = p1 ? p1->getAsBigInt() : 0;

   if (offset >= str->strlen())
       return 0;

   return new QoreBigIntNode((str->getBuffer()[offset]));
}

static AbstractQoreNode *f_chr(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;
   
   return new QoreStringNode((char)p0->getAsInt());
}

static inline const char *memstr(const char *str, const char *pattern, qore_size_t pl, qore_size_t len) {
   while (true) {
      const char *p = (const char *)memchr(str, pattern[0], len);
      if (!p)
	 return 0;

      //printd(5, "memstr() pattern=%s str=%p p=%p len=%d pl=%d remaining=%d (%c %c %c)\n", pattern, str, p, len, pl, len-(p-str), p[1], p[2], p[3]);

      // if there is not enough string left for the pattern, then return
      if ((len - (p - str)) < pl)
	 return 0;

      bool found = true;
      for (qore_size_t i = 1; i < pl; ++i) {
	 if (pattern[i] != p[i]) {
	    len -= (p - str + 1);
	    str = p + 1;
	    found = false;
	    break;
	 }
      }
      if (!found)
	 continue;
     
      // found
      //printd(5, "memstr() got it! p=%p\n", p);
      return p;
   }
   return 0;
}

// syntax: split(pattern, string);
static AbstractQoreNode *f_split(const QoreListNode *params, ExceptionSink *xsink) {
   const char *str, *pattern;
   const AbstractQoreNode *p0 = get_param(params, 0);
   
   // pattern length, string length
   qore_size_t pl, sl;

   // to be used if necessary to convert string arguments
   TempEncodingHelper temp;

   // to be sued when saving a string
   const QoreEncoding *enc = 0;

   if (p0 && p0->getType() == NT_STRING) {
      const QoreStringNode *s0 = reinterpret_cast<const QoreStringNode *>(p0);
      const QoreStringNode *s1 = test_string_param(params, 1);

      if (!s1)
	 return new QoreListNode();

      // convert pattern encoding to string if necessary
      temp.set(s0, s1->getEncoding(), xsink);
      if (*xsink)
	 return 0;

      pattern = temp->getBuffer();
      pl = temp->strlen();

      str = s1->getBuffer();
      sl = s1->strlen();
      enc = s1->getEncoding();
   }
   else {
      if (!p0 || p0->getType() != NT_BINARY)
	 return new QoreListNode();

      const BinaryNode *b0 = reinterpret_cast<const BinaryNode *>(p0);
      const BinaryNode *b1 = test_binary_param(params, 1);

      if (!b1)
	 return new QoreListNode();

      pattern = (const char*)b0->getPtr();
      pl = b0->size();

      str = (const char*)b1->getPtr();
      sl = b1->size();
   }

   QoreListNode *l = new QoreListNode();
   const char *ostr = str;
   while (const char *p = memstr(str, pattern, pl, sl - (str - ostr))) {
      //printd(5, "str=%08p p=%08p \"%s\" \"%s\"\n", str, p, str, pattern);
      if (enc)
	 l->push(new QoreStringNode(str, p - str, enc));
      else {
	 BinaryNode *b = new BinaryNode();
	 b->append(str, p - str);
	 l->push(b);
      }
      str = p + pl;
   }
   //printd(5, "f_split() str=%p %d remaining=%d\n", str, *str, sl - (str - ostr));
   // add last field if there is data remaining
   if (sl - (str - ostr)) {
      if (enc) 
	 l->push(new QoreStringNode(str, sl - (str - ostr), enc));
      else {
	 BinaryNode *b = new BinaryNode();
	 b->append(str, sl - (str - ostr));
	 l->push(b);
      }
   }
   return l;
}

static AbstractQoreNode *f_get_encoding(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return 0;
   return new QoreStringNode(p0->getEncoding()->getCode());
}

// usage: convert_encoding(string, new_encoding);
static AbstractQoreNode *f_convert_encoding(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1;
   if (!(p0 = test_string_param(params, 0)) ||
       !(p1 = test_string_param(params, 1)))
      return 0;

   return p0->convertEncoding(QEM.findCreate(p1), xsink);
}

static AbstractQoreNode *f_force_encoding(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1;
   if (!(p0 = test_string_param(params, 0)) ||
       !(p1 = test_string_param(params, 1)))
      return 0;

   return new QoreStringNode(p0->getBuffer(), p0->strlen(), QEM.findCreate(p1));
}

static AbstractQoreNode *f_regex(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0, *p1;
   if (!(p0 = test_string_param(params, 0)) ||
       !(p1 = test_string_param(params, 1)))
       return 0;
   
   const AbstractQoreNode *p2 = get_param(params, 2);
   int options = p2 ? p2->getAsInt() : 0;

   QoreRegexNode qr(p1, options, xsink);
   if (*xsink)
      return 0;

   return get_bool_node(qr.exec(p0, xsink));
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
      return 0;
   
   unsigned n = num_params(params);
   if (n < 2)
      return 0;

   const QoreListNode *l = test_list_param(params, 1);
   if (n == 2 && l)
      n = 0;
   else
   {
      n = 1;
      l = params;
   }

   QoreStringNode *str = new QoreStringNode();

   for (unsigned i = n; i < l->size(); i++) {
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

   qore_type_t ptype = p->getType();
   if (ptype == NT_STRING) {
      const QoreStringNode *pstr = reinterpret_cast<const QoreStringNode *>(p);
      QoreStringNode *str = pstr->copy();
      str->chomp();
      return str;
   } 

   if (ptype != NT_REFERENCE)
      return 0;

   const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(p);

   AutoVLock vl(xsink);
   ReferenceHelper ref(r, vl, xsink);
   if (!ref || ref.getType() != NT_STRING)
      return 0;

   QoreStringNode *str = reinterpret_cast<QoreStringNode *>(ref.getUnique(xsink));
   if (*xsink)
      return 0;

   str->chomp();
   return str->refSelf();
}

static AbstractQoreNode *f_trim(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);   
   if (!p0)
      return 0;
   const QoreStringNode *p1 = test_string_param(params, 1);
   const char *chars = p1 ? p1->getBuffer() : 0;

   qore_type_t p0_type = p0->getType();

   if (p0_type == NT_STRING) {
      const QoreStringNode *pstr = reinterpret_cast<const QoreStringNode *>(p0);
      QoreStringNode *str = pstr->copy();
      str->trim(chars);
      return str;
   }

   if (p0_type != NT_REFERENCE)
      return 0;

   const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(p0);

   AutoVLock vl(xsink);
   ReferenceHelper ref(r, vl, xsink);
   if (!ref || ref.getType() != NT_STRING)
      return 0;

   QoreStringNode *str = reinterpret_cast<QoreStringNode *>(ref.getUnique(xsink));
   if (*xsink)
      return 0;

   str->trim(chars);
   return str->refSelf();
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
   builtinFunctions.add("force_encoding", f_force_encoding);
   builtinFunctions.add("regex", f_regex);
   builtinFunctions.add("regex_subst", f_regex_subst);
   builtinFunctions.add("regex_extract", f_regex_extract);
   builtinFunctions.add("replace", f_replace);
   builtinFunctions.add("join", f_join);
   builtinFunctions.add("chomp", f_chomp);
   builtinFunctions.add("trim", f_trim);
}
