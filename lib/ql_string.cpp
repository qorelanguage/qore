/*
  ql_string.cpp

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
#include <qore/intern/ql_string.h>

#include <stdlib.h>
#include <string.h>

// returns number of characters in a string
static AbstractQoreNode *f_length_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, args, 0);
   return new QoreBigIntNode(str->length());
}

// returns number of bytes in a binary object
static AbstractQoreNode *f_length_bin(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(bin, const BinaryNode, args, 0);
   return new QoreBigIntNode(bin->size());
}

// returns number of bytes in a string
static AbstractQoreNode *f_strlen_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, args, 0);
   return new QoreBigIntNode(str->strlen());
}

static AbstractQoreNode *f_tolower(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   QoreStringNode *rv = p0->copy();
   char *p = (char *)rv->getBuffer();
   while (*p) {
      *p = tolower(*p);
      p++;
   }
   return rv;
}

static AbstractQoreNode *f_toupper(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   QoreStringNode *rv = p0->copy();
   char *p = (char *)rv->getBuffer();
   while (*p) {
      *p = toupper(*p);
      p++;
   }
   return rv;
}

// syntax substr(string, start[, length]) - note 1st character is 0, not 1
static AbstractQoreNode *f_substr_str_int(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   int start = (int)HARD_QORE_INT(args, 1);
   return p0->substr(start, xsink);
}

static AbstractQoreNode *f_substr_str_int_int(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   int start = (int)HARD_QORE_INT(args, 1);
   int len = (int)HARD_QORE_INT(args, 2);
   return p0->substr(start, len, xsink);
}

/* index(string, substring[, position])
 * returns the index position in characters (starting with 0) of the first occurrence
 * of substring within string, optionally starting at position if available
 * returns -1 if not found
 */
static AbstractQoreNode *f_index_str_str_int(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(hs, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(t1, const QoreStringNode, args, 1);
   qore_offset_t pos = (qore_offset_t)HARD_QORE_INT(args, 2);

   qore_offset_t rc = hs->index(*t1, pos, xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

/* bindex(string, substring [, position])
 * returns the index position in bytes (starting with 0) of the first occurrence
 * of substring within string, optionally starting at position if available
 * returns -1 if not found
 * see index() for the character position
 */
static AbstractQoreNode *f_bindex_str_str_int(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(hs, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(t1, const QoreStringNode, args, 1);
   qore_offset_t pos = (qore_offset_t)HARD_QORE_INT(args, 2);

   return new QoreBigIntNode(hs->bindex(*t1, pos));
}

// finds the last occurrence of needle in haystack at or before position pos
// pos must be a non-negative valid byte offset in haystack
static inline int rindex_simple_intern(const char *haystack, int hlen, const char *needle, int nlen, int pos) {
   // if the offset does not allow for the needle string to be present, then adjust
   if ((pos + nlen) > hlen) {
      pos = hlen - nlen;
      if (pos < 0)
	 return -1;
   }

   while (pos != -1) {
      if (!strncmp(haystack + pos, needle, nlen))
	 return pos;
      pos--;
   }
   return -1;
}

// syntax: rindex(string, substring, [pos])
static AbstractQoreNode *f_rindex_str_str_int(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(hs, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(t1, const QoreStringNode, args, 1);
   qore_offset_t pos = (qore_offset_t)HARD_QORE_INT(args, 2);

   qore_offset_t rc = hs->rindex(*t1, pos, xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

/* brindex(string, substring [, position])
 * returns the index position in bytes (starting with 0) of the first occurrence
 * of substring within string, searching from the end of the string
 */
static AbstractQoreNode *f_brindex_str_str_int(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(hs, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(t1, const QoreStringNode, args, 1);
   qore_offset_t pos = (qore_offset_t)HARD_QORE_INT(args, 2);

   return new QoreBigIntNode(hs->brindex(*t1, pos));
}

// syntax: ord(string, [offset = 0])
// note that ord() only works on byte offsets and gives byte values
static AbstractQoreNode *f_ord_str_int(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(offset, const QoreBigIntNode, args, 1);

   if (offset->val < 0 || offset->val >= (qore_offset_t)str->strlen())
      return new QoreBigIntNode(-1);

   return new QoreBigIntNode((str->getBuffer()[offset->val]));
}

static AbstractQoreNode *f_ord_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, args, 0);

   return new QoreBigIntNode(!str->strlen() ? -1 : str->getBuffer()[0]);
}

static AbstractQoreNode *f_chr_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreStringNode('\0');
}

static AbstractQoreNode *f_chr_int(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreBigIntNode, args, 0);
   return new QoreStringNode((char)p0->val);
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

static void split_add_element(QoreListNode *l, const char *str, unsigned len, const QoreEncoding *enc) {
   if (enc)
      l->push(new QoreStringNode(str, len, enc));
   else {
      BinaryNode *b = new BinaryNode;
      b->append(str, len);
      l->push(b);
   }
}

static QoreListNode *split_intern(const char *pattern, qore_size_t pl, const char *str, qore_size_t sl, const QoreEncoding *enc) {
   QoreListNode *l = new QoreListNode();
   const char *ostr = str;
   while (const char *p = memstr(str, pattern, pl, sl - (str - ostr))) {
      split_add_element(l, str, p - str, enc);
      str = p + pl;
   }
   // add last field if there is data remaining
   if (sl - (str - ostr))
      split_add_element(l, str, sl - (str - ostr), enc);

   return l;
}

// syntax: split(pattern, string);
static AbstractQoreNode *f_split_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(s0, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(s1, const QoreStringNode, args, 1);

   // convert pattern encoding to string if necessary
   TempEncodingHelper temp(s0, s1->getEncoding(), xsink);
   if (*xsink)
      return 0;
   
   return split_intern(temp->getBuffer(), temp->strlen(), s1->getBuffer(), s1->strlen(), s1->getEncoding());
}

// syntax: split(pattern, string, quote);
static AbstractQoreNode *f_split_str_str_str(const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *s0 = HARD_QORE_STRING(args, 0);
   const QoreStringNode *s1 = HARD_QORE_STRING(args, 1);
   const QoreStringNode *s2 = HARD_QORE_STRING(args, 2);

   // convert pattern encoding to string if necessary
   TempEncodingHelper pat(s0, s1->getEncoding(), xsink);
   if (*xsink)
      return 0;
   
   // convert quote to string if necessary
   TempEncodingHelper quote(s2, s1->getEncoding(), xsink);
   if (*xsink)
      return 0;

   if (!quote->strlen() || quote->strlen() > s1->strlen())
      return split_intern(pat->getBuffer(), pat->strlen(), s1->getBuffer(), s1->strlen(), s1->getEncoding());
   
   ReferenceHolder<QoreListNode> l(new QoreListNode, xsink);
   const char *ostr = s1->getBuffer();
   qore_size_t sl = s1->strlen();
   const char *pattern = pat->getBuffer();
   qore_size_t pl = pat->strlen();

   const char *str = ostr;

   // remaining byte length
   qore_size_t len = sl;

   while (len > 0) {
      // see if the field begins with the quote string
      // and if the remaining string length is at least big enough for two quote strings
      if ((quote->strlen() * 2) <= len
	  && !memcmp(quote->getBuffer(), str, quote->strlen())) {
	 // advance pointer past quote
	 str += quote->strlen();
	 // find next quote character, ignore escaped quotes
	 const char *tstr = str;
	 const char *p;
	 while (true) {
	    p = memstr(tstr, quote->getBuffer(), quote->strlen(), len);
	    if (!p) {
	       xsink->raiseException("SPLIT-ERROR", "cannot find closing quote '%s' in field %d", quote->getBuffer(), l->size() + 1);
	       return 0;
	    }
	    if (p == tstr)
	       break;
	    if (*(p - 1) != '\\')
	       break;
	    tstr = p + 1;
	 }
	 // optimistically add the field to the list
	 l->push(new QoreStringNode(str, p - str, s1->getEncoding()));

	 str = p + quote->strlen();
	 // see if we are either at the end of the string
	 len = sl + (ostr - str);

	 if (!len)
	    break;

	 // or a separator string comes next
	 if (len < pl || memcmp(pattern, str, pl)) {
	    xsink->raiseException("SPLIT-ERROR", "separator pattern '%s' does not follow end quote in field %d", pattern, l->size());
	    return 0;
	 }
	 str += pl;
	 len -= pl;
	 continue;
      }
      
      const char *p = memstr(str, pattern, pl, sl - (str - ostr));
      if (!p) {
	 if (str != ostr)
	    l->push(new QoreStringNode(str, sl - (str - ostr), s1->getEncoding()));
	 break;
      }

      l->push(new QoreStringNode(str, p - str, s1->getEncoding()));
      len -= (p - str);
      str = p + pl;      
   }
   
   return l.release();
}

static AbstractQoreNode *f_split_bin(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(b0, const BinaryNode, args, 0);
   HARD_QORE_PARAM(b1, const BinaryNode, args, 1);

   return split_intern((const char*)b0->getPtr(), b0->size(), (const char*)b1->getPtr(), b1->size(), 0);
}

static AbstractQoreNode *f_get_encoding_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   return new QoreStringNode(p0->getEncoding()->getCode());
}

// usage: convert_encoding(string, new_encoding);
static AbstractQoreNode *f_convert_encoding(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(p1, const QoreStringNode, args, 1);
   return p0->convertEncoding(QEM.findCreate(p1), xsink);
}

static AbstractQoreNode *f_force_encoding(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(p1, const QoreStringNode, args, 1);
   return new QoreStringNode(p0->getBuffer(), p0->strlen(), QEM.findCreate(p1));
}

static AbstractQoreNode *f_regex(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(p1, const QoreStringNode, args, 1);
   int options = (int)HARD_QORE_INT(args, 2);
   
   QoreRegexNode qr(p1, options, xsink);
   if (*xsink)
      return 0;

   return get_bool_node(qr.exec(p0, xsink));
}

// syntax: regex_subst(string, pattern, substitution_pattern, options)
static AbstractQoreNode *f_regex_subst(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(p1, const QoreStringNode, args, 1);
   HARD_QORE_PARAM(p2, const QoreStringNode, args, 2);
   // QRE_GLOBAL is just outside the 32-bit space, so we have to get an in64 value to check it
   int64 options = HARD_QORE_INT(args, 3);
   
   bool global;
   if (options & QRE_GLOBAL) {
      global = true;
      options &= 0xffffffff;
   }
   else
      global = false;

   RegexSubstNode qrs(p1, (int)options, xsink);
   if (*xsink)
      return 0;

   if (global)
      qrs.setGlobal();
   return qrs.exec(p0, p2, xsink);
}

static AbstractQoreNode *f_regex_extract(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(p1, const QoreStringNode, args, 1);
   int options = (int)HARD_QORE_INT(args, 2);
   
   QoreRegexNode qr(p1, options, xsink);
   if (*xsink)
      return 0;
   
   return qr.extractSubstrings(p0, xsink);
}

// usage: replace(string, substring, new substring)
static AbstractQoreNode *f_replace(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(p1, const QoreStringNode, args, 1);
   HARD_QORE_PARAM(p2, const QoreStringNode, args, 2);

   const QoreEncoding *ccs = p0->getEncoding();

   TempEncodingHelper t1(p1, ccs, xsink);
   if (*xsink)
      return 0;

   TempEncodingHelper t2(p2, ccs, xsink);
   if (*xsink)
      return 0;

   if (!t1->strlen())
      return p0->refSelf();

   QoreStringNode *nstr = new QoreStringNode(ccs);   

   const char *str, *pattern;
   str = p0->getBuffer();
   pattern = t1->getBuffer();
   int plen = strlen(pattern);

   while (const char *p = strstr(str, pattern)) {
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

static QoreStringNode *join_intern(const QoreStringNode *p0, const QoreListNode *l, int offset = 0) {
   QoreStringNode *str = new QoreStringNode();

   for (unsigned i = offset; i < l->size(); i++) {
      const AbstractQoreNode *p = l->retrieve_entry(i);
      if (p) {
	 QoreStringValueHelper t(p);	 
	 str->concat(*t);
      }
      if (i < (l->size() - 1))
	 str->concat(p0);
   }
   return str;
}

// perl-style join function
static AbstractQoreNode *f_join_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);

   return join_intern(p0, args, 1);
}

// perl-style join function
static AbstractQoreNode *f_join_str_list(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(l, const QoreListNode, args, 1);

   return join_intern(p0, l);
}

static AbstractQoreNode *f_chomp_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(pstr, const QoreStringNode, args, 0);
   QoreStringNode *str = pstr->copy();
   str->chomp();
   return str;
}

static AbstractQoreNode *f_chomp_ref(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(r, const ReferenceNode, args, 0);

   AutoVLock vl(xsink);
   QoreTypeSafeReferenceHelper ref(r, vl, xsink);
   if (!ref || ref.getType() != NT_STRING)
      return 0;

   QoreStringNode *str = reinterpret_cast<QoreStringNode *>(ref.getUnique(xsink));
   if (*xsink)
      return 0;

   str->chomp();
   return str->refSelf();
}

static AbstractQoreNode *f_trim_str_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(pstr, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(p1, const QoreStringNode, args, 1);

   const char *chars = p1->strlen() ? p1->getBuffer() : 0;
   QoreStringNode *str = pstr->copy();
   str->trim(chars);
   return str;
}

static AbstractQoreNode *f_trim_ref_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(r, const ReferenceNode, args, 0);
   HARD_QORE_PARAM(p1, const QoreStringNode, args, 1);

   const char *chars = p1->strlen() ? p1->getBuffer() : 0;
   AutoVLock vl(xsink);
   QoreTypeSafeReferenceHelper ref(r, vl, xsink);
   if (!ref || ref.getType() != NT_STRING)
      return 0;

   QoreStringNode *str = reinterpret_cast<QoreStringNode *>(ref.getUnique(xsink));
   if (*xsink)
      return 0;

   str->trim(chars);
   return str->refSelf();
}

void init_string_functions() {
   builtinFunctions.add2("length", f_length_str, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, softStringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("length", f_length_bin, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("length", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("length", f_int_noop, QC_NOOP, QDOM_DEFAULT, bigIntTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("strlen", f_strlen_str, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, softStringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("strlen", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("strlen", f_int_noop, QC_NOOP, QDOM_DEFAULT, bigIntTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   // tolower() called without a string argument returns 0
   builtinFunctions.add2("tolower", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("tolower", f_tolower, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // toupper() called without a string argument returns 0
   builtinFunctions.add2("toupper", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("toupper", f_toupper, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("substr", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("substr", f_substr_str_int, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 2, softStringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("substr", f_substr_str_int_int, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 3, softStringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("index", f_int_minus_one_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, bigIntTypeInfo);
   builtinFunctions.add2("index", f_index_str_str_int, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 3, softStringTypeInfo, QORE_PARAM_NO_ARG, softStringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());

   builtinFunctions.add2("bindex", f_int_minus_one_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, bigIntTypeInfo);
   builtinFunctions.add2("bindex", f_bindex_str_str_int, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 3, softStringTypeInfo, QORE_PARAM_NO_ARG, softStringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());

   builtinFunctions.add2("rindex", f_int_minus_one_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, bigIntTypeInfo);
   builtinFunctions.add2("rindex", f_rindex_str_str_int, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 3, softStringTypeInfo, QORE_PARAM_NO_ARG, softStringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(-1));

   builtinFunctions.add2("brindex", f_int_minus_one_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, bigIntTypeInfo);
   builtinFunctions.add2("brindex", f_brindex_str_str_int, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 3, softStringTypeInfo, QORE_PARAM_NO_ARG, softStringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(-1));

   builtinFunctions.add2("ord", f_int_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, bigIntTypeInfo);
   builtinFunctions.add2("ord", f_ord_str, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, softStringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("ord", f_ord_str_int, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 2, softStringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("chr", f_chr_int, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("chr", f_chr_noop, QC_NOOP, QDOM_DEFAULT, stringTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("chr", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);

   // an empty list was returned by split() if the types were not correct
   builtinFunctions.add2("split", f_list_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, listTypeInfo);
   builtinFunctions.add2("split", f_split_str, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   // split(string $pattern, string $str, string $quote) returns list
   builtinFunctions.add2("split", f_split_str_str_str, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo, 3, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("split", f_split_bin, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, binaryTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_encoding", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("get_encoding", f_get_encoding_str, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("convert_encoding", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("convert_encoding", f_convert_encoding, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("force_encoding", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("force_encoding", f_force_encoding, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("regex", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("regex", f_regex, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 3, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, bigIntTypeInfo, zero());

   builtinFunctions.add2("regex_subst", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("regex_subst", f_regex_subst, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 4, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, bigIntTypeInfo, zero());

   builtinFunctions.add2("regex_extract", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   // regex_extract() returns *list
   builtinFunctions.add2("regex_extract", f_regex_extract, QC_NO_FLAGS, QDOM_DEFAULT, listOrNothingTypeInfo, 3, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, bigIntTypeInfo, zero());

   builtinFunctions.add2("replace", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("replace", f_replace, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 3, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("join", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("join", f_join_str, QC_CONSTANT | QC_USES_EXTRA_ARGS, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("join", f_join_str_list, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("chomp", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("chomp", f_chomp_str, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("chomp", f_chomp_ref, QC_NO_FLAGS, QDOM_DEFAULT, 0, 1, referenceTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("trim", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("trim", f_trim_str_str, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, null_string());
   builtinFunctions.add2("trim", f_trim_ref_str, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, referenceTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, null_string());
}
