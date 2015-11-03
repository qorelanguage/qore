/*
  QoreString.cc

  QoreString Class Definition

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
#include <qore/intern/qore_string_private.h>
#include <qore/minitest.hpp>

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <iconv.h>
#include <ctype.h>

#ifdef DEBUG_TESTS
#  include "tests/QoreString_tests.cc"
#endif

// to be used for trim
static char default_whitespace[] = { ' ', '\t', '\n', '\r', '\v', '\0' };

struct code_table {
      char symbol;
      const char *code;
      unsigned len;
};

static const struct code_table html_codes[] = 
{ { '&', "&amp;", 5 },
  { '<', "&lt;", 4 },
  { '>', "&gt;", 4 },
  { '"', "&quot;", 6 } }; 

#define NUM_HTML_CODES (sizeof(html_codes) / sizeof (struct code_table))

QoreString::QoreString() : priv(new qore_string_private) {
   priv->len = 0;
   priv->allocated = STR_CLASS_BLOCK;
   priv->buf = (char *)malloc(priv->allocated * sizeof(char));
   priv->buf[0] = '\0';
   priv->charset = QCS_DEFAULT;
}

// FIXME: this is not very efficient with the array offsets...
QoreString::QoreString(const char *str) : priv(new qore_string_private)
{
   priv->len = 0;
   priv->allocated = STR_CLASS_BLOCK;
   priv->buf = (char *)malloc(priv->allocated * sizeof(char));
   if (str)
   {
      while (str[priv->len])
      {
	 priv->check_char(priv->len);
	 priv->buf[priv->len] = str[priv->len];
	 priv->len++;
      }
      priv->check_char(priv->len);
      priv->buf[priv->len] = '\0';
   }
   else
      priv->buf[0] = '\0';
   priv->charset = QCS_DEFAULT;
}

// FIXME: this is not very efficient with the array offsets...
QoreString::QoreString(const char *str, const QoreEncoding *new_qorecharset) : priv(new qore_string_private)
{
   priv->len = 0;
   priv->allocated = STR_CLASS_BLOCK;
   priv->buf = (char *)malloc(priv->allocated * sizeof(char));
   if (str)
   {
      while (str[priv->len])
      {
	 priv->check_char(priv->len);
	 priv->buf[priv->len] = str[priv->len];
	 priv->len++;
      }
      priv->check_char(priv->len);
      priv->buf[priv->len] = '\0';
   }
   else
      priv->buf[0] = '\0';
   priv->charset = new_qorecharset;
}

QoreString::QoreString(const std::string &str, const QoreEncoding *new_encoding) : priv(new qore_string_private)
{
   priv->allocated = str.size() + 1 + STR_CLASS_BLOCK;
   priv->buf = (char *)malloc(priv->allocated * sizeof(char));
   memcpy(priv->buf, str.c_str(), str.size() + 1);
   priv->len = str.size();
   priv->charset = new_encoding;
}

QoreString::QoreString(const QoreEncoding *new_qorecharset) : priv(new qore_string_private)
{
   priv->len = 0;
   priv->allocated = STR_CLASS_BLOCK;
   priv->buf = (char *)malloc(priv->allocated * sizeof(char));
   priv->buf[0] = '\0';
   priv->charset = new_qorecharset;
}

QoreString::QoreString(const char *str, qore_size_t size, const QoreEncoding *new_qorecharset) : priv(new qore_string_private)
{
   priv->len = size;
   priv->allocated = size + STR_CLASS_EXTRA;
   priv->buf = (char *)malloc(sizeof(char) * priv->allocated);
   memcpy(priv->buf, str, size);
   priv->buf[size] = '\0';
   priv->charset = new_qorecharset;
}

QoreString::QoreString(const QoreString *str) : priv(new qore_string_private(*(str->priv)))
{
}

QoreString::QoreString(const QoreString &str) : priv(new qore_string_private(*(str.priv)))
{
}

QoreString::QoreString(const QoreString *str, qore_size_t size) : priv(new qore_string_private)
{
   if (size >= str->priv->len)
      size = str->priv->len;
   priv->len = size;
   priv->allocated = size + STR_CLASS_EXTRA;
   priv->buf = (char *)malloc(sizeof(char) * priv->allocated);
   if (size)
      memcpy(priv->buf, str->priv->buf, size);
   priv->buf[size] = '\0';
   priv->charset = str->priv->charset;
}

QoreString::QoreString(char c) : priv(new qore_string_private)
{
   priv->len = 1;
   priv->allocated = STR_CLASS_BLOCK;
   priv->buf = (char *)malloc(sizeof(char) * priv->allocated);
   priv->buf[0] = c;
   priv->buf[1] = '\0';
   priv->charset = QCS_DEFAULT;
}

QoreString::QoreString(int64 i) : priv(new qore_string_private) {
   priv->allocated = MAX_BIGINT_STRING_LEN + 1;
   priv->buf = (char *)malloc(sizeof(char) * priv->allocated);
   priv->len = ::snprintf(priv->buf, MAX_BIGINT_STRING_LEN, "%lld", i);
   // terminate string just in case
   priv->buf[MAX_BIGINT_STRING_LEN] = '\0';
   priv->charset = QCS_DEFAULT;
}

QoreString::QoreString(bool b) : priv(new qore_string_private) {
   priv->allocated = 2;
   priv->buf = (char *)malloc(sizeof(char) * priv->allocated);
   priv->buf[0] = b ? '1' : '0';
   priv->buf[1] = 0;
   priv->len = 1;
   priv->charset = QCS_DEFAULT;
}

QoreString::QoreString(double f) : priv(new qore_string_private)
{
   priv->allocated = MAX_FLOAT_STRING_LEN + 1;
   priv->buf = (char *)malloc(sizeof(char) * priv->allocated);
   priv->len = ::snprintf(priv->buf, MAX_FLOAT_STRING_LEN, "%.9g", f);
   // terminate string just in case
   priv->buf[MAX_FLOAT_STRING_LEN] = '\0';
   priv->charset = QCS_DEFAULT;
}

QoreString::QoreString(const DateTime *d) : priv(new qore_string_private)
{
   priv->allocated = 15;
   priv->buf = (char *)malloc(sizeof(char) * priv->allocated);
   priv->len = ::sprintf(priv->buf, "%04d%02d%02d%02d%02d%02d", d->getYear(), d->getMonth(), d->getDay(),
		   d->getHour(), d->getMinute(), d->getSecond());
   priv->charset = QCS_DEFAULT;
}

QoreString::QoreString(const BinaryNode *b) : priv(new qore_string_private)
{
   priv->allocated = b->size() + (b->size() * 4) / 10 + 10; // estimate for base64 encoding
   priv->buf = (char *)malloc(sizeof(char) * priv->allocated);
   priv->len = 0;
   priv->charset = QCS_DEFAULT;
   concatBase64(b);
}

QoreString::QoreString(char *nbuf, qore_size_t nlen, qore_size_t nallocated, const QoreEncoding *enc) : priv(new qore_string_private) {
   assert(nallocated >= nlen);
   priv->buf = nbuf;
   priv->len = nlen;
   priv->allocated = nallocated;
   if (nallocated == nlen) {
      priv->check_char(nlen);
      priv->buf[nlen] = '\0';
   }
   priv->charset = enc;
}

// FIXME: remove this function
// private constructor
QoreString::QoreString(struct qore_string_private *p) : priv(p)
{
}

QoreString::~QoreString()
{
   delete priv;
}

// NULL values sorted at end
int QoreString::compare(const QoreString *str) const
{
   // empty strings are always equal even if the character encoding is different
   if (!priv->len) {
      if (!str->priv->len)
	 return 0;
      else
	 return 1;
   }

   if (str->priv->charset != priv->charset)
      return 1;

   return strcmp(priv->buf, str->priv->buf);
}

int QoreString::compare(const char *str) const
{
   if (!priv->buf) {
      if (!str)
	 return 0;
      else
	 return 1;
   }

   return strcmp(priv->buf, str);
}

void QoreString::terminate(qore_size_t size) {
   if (size > priv->len)
      priv->check_char(size);
   priv->len = size;
   priv->buf[size] = '\0';
}

void QoreString::take(char *str)
{
   if (priv->buf)
      free(priv->buf);
   priv->buf = str;
   if (str)
   {
      priv->len = ::strlen(str);
      priv->allocated = priv->len + 1;
   }
   else
   {
      priv->allocated = 0;
      priv->len = 0;
   }
}

void QoreString::take(char *str, const QoreEncoding *new_qorecharset)
{
   take(str);
   priv->charset = new_qorecharset;
}

void QoreString::take(char *str, qore_size_t size)
{
   if (priv->buf)
      free(priv->buf);
   priv->buf = str;
   priv->len = size;
   priv->allocated = size + 1;
}

void QoreString::take(char *str, qore_size_t size, const QoreEncoding *enc)
{
   if (priv->buf)
      free(priv->buf);
   priv->buf = str;
   priv->len = size;
   priv->allocated = size + 1;

   priv->charset = enc;
}

void QoreString::takeAndTerminate(char *str, qore_size_t size) {
   if (priv->buf)
      free(priv->buf);
   priv->buf = str;
   priv->len = size;
   priv->allocated = size + 1;
   priv->check_char(size);
   priv->buf[size] = '\0';
}

// NOTE: could be dangerous if we refer to the priv->buffer after this
// call and it's NULL (the only way the priv->buffer can become NULL)
char *QoreString::giveBuffer()
{
   char *rv = priv->buf;
   priv->buf = 0;
   priv->len = 0;
   priv->allocated = 0;
   // reset character set, just in case the string will be reused
   // (normally not after this call)
   priv->charset = QCS_DEFAULT;
   return rv;
}

void QoreString::clear()
{
   if (priv->allocated) {
     priv->len = 0;
     priv->buf[0] = '\0';
   }
}

void QoreString::set(const char *str, const QoreEncoding *new_qorecharset) {
   priv->len = 0;
   priv->charset = new_qorecharset;
   if (!str) {
      if (priv->buf)
	 priv->buf[0] = '\0';
   }
   else
      concat(str);
}

void QoreString::set(const QoreString *str) {
   priv->len = str->priv->len;
   priv->charset = str->priv->charset;
   allocate(str->priv->len + 1);
   // copy string and trailing null
   memcpy(priv->buf, str->priv->buf, str->priv->len + 1);
}

void QoreString::set(const QoreString &str) {
   set(&str);
}

void QoreString::replace(qore_size_t offset, qore_size_t dlen, const char *str) {
   if (str && str[0])
      splice_simple(offset, dlen, str, ::strlen(str));
   else
      splice_simple(offset, dlen);
}

void QoreString::replace(qore_size_t offset, qore_size_t dlen, const QoreString *str) {
   if (str->getEncoding() != priv->charset)
      return;

   if (str && str->strlen())
      splice_simple(offset, dlen, str->getBuffer(), str->strlen());
   else
      splice_simple(offset, dlen);
}

void QoreString::replace(qore_size_t offset, qore_size_t dlen, const QoreString *str, ExceptionSink *xsink) {
   if (str && str->strlen()) {
      TempEncodingHelper tmp(str, priv->charset, xsink);
      if (!tmp)
	 return;
      splice_simple(offset, dlen, tmp->getBuffer(), tmp->strlen());
      return;
   }

   splice_simple(offset, dlen);
}

void QoreString::splice(qore_offset_t offset, ExceptionSink *xsink) {
   if (!priv->charset->isMultiByte()) {
      qore_size_t n_offset = priv->check_offset(offset);
      if (n_offset == priv->len)
	 return;

      splice_simple(n_offset, priv->len - n_offset);
      return;
   }
   splice_complex(offset, xsink);
}

void QoreString::splice(qore_offset_t offset, qore_offset_t num, ExceptionSink *xsink) {
   if (!priv->charset->isMultiByte()) {
      qore_size_t n_offset, n_num;
      priv->check_offset(offset, num, n_offset, n_num);
      if (n_offset == priv->len || !n_num)
	 return;

      splice_simple(n_offset, n_num);
      return;
   }
   splice_complex(offset, num, xsink);
}

void QoreString::splice(qore_offset_t offset, qore_offset_t num, const AbstractQoreNode *strn, ExceptionSink *xsink) {
   if (!strn || strn->getType() != NT_STRING) {
      splice(offset, num, xsink);
      return;
   }

   const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(strn);
   TempEncodingHelper tmp(str, priv->charset, xsink);
   if (!tmp)
       return;

   if (!priv->charset->isMultiByte()) {
      qore_size_t n_offset, n_num;
      priv->check_offset(offset, num, n_offset, n_num);
      if (n_offset == priv->len) {
	 if (!tmp->priv->len)
	    return;
	 n_num = 0;
      }
      splice_simple(n_offset, n_num, tmp->getBuffer(), tmp->strlen());
      return;
   }
   splice_complex(offset, num, *tmp, xsink);
}

// removes a single trailing newline
qore_size_t QoreString::chomp()
{
   if (priv->len && priv->buf[priv->len - 1] == '\n')
   {
      terminate(priv->len - 1);
      if (priv->len && priv->buf[priv->len - 1] == '\r')
      {
	 terminate(priv->len - 1);
	 return 2;
      }
      return 1;
   }
   return 0;
}

// needed for platforms where the input buffer is defined as "const char"
template<typename T>
static inline size_t iconv_adapter (size_t (*iconv_f) (iconv_t, T, size_t *, char **, size_t *), iconv_t handle, char **inbuf, size_t *inavail, char **outbuf, size_t *outavail)
{
   return (*iconv_f) (handle, (T) inbuf, inavail, outbuf, outavail);
}

class IconvHelper {
   private:
      iconv_t c;

   public:
      DLLLOCAL IconvHelper(const QoreEncoding *to, const QoreEncoding *from, ExceptionSink *xsink)
      {
#ifdef NEED_ICONV_TRANSLIT
	 QoreString to_code((char *)to->getCode());
	 to_code.concat("//TRANSLIT");
	 c = iconv_open(to_code.getBuffer(), from->getCode());
#else
	 c = iconv_open(to->getCode(), from->getCode());
#endif
	 if (c == (iconv_t)-1)
	 {
	    if (errno == EINVAL)
	       xsink->raiseException("ENCODING-CONVERSION-ERROR", "cannot convert from \"%s\" to \"%s\"", from->getCode(), to->getCode());
	    else
	       xsink->raiseException("ENCODING-CONVERSION-ERROR", "unknown error converting from \"%s\" to \"%s\": %s", from->getCode(), to->getCode(), strerror(errno));
	 }
      }
      DLLLOCAL ~IconvHelper()
      {
	 if (c != (iconv_t)-1)
	    iconv_close(c);
      }
      DLLLOCAL iconv_t operator*()
      { 
	 return c; 
      }
};

// static function
int QoreString::convert_encoding_intern(const char *src, qore_size_t src_len, const QoreEncoding *from, QoreString &targ, const QoreEncoding *nccs, ExceptionSink *xsink)
{
   assert(targ.priv->charset == nccs);

   IconvHelper c(nccs, from, xsink);
   if (*xsink)
      return -1;
   
   // now convert value
   qore_size_t al = src_len + STR_CLASS_BLOCK;
   targ.allocate(al + 1);
   while (true)
   {
      size_t ilen = src_len;
      size_t olen = al;
      char *ib = (char *)src;
      char *ob = targ.priv->buf;
      size_t rc = iconv_adapter(iconv, *c, &ib, &ilen, &ob, &olen);
      if (rc == (size_t)-1)
	 switch (errno)
	 {
	    case EINVAL:
	    case EILSEQ:
	    {
	       xsink->raiseException("ENCODING-CONVERSION-ERROR", "illegal character sequence found in input type \"%s\" (while converting to \"%s\")",
				     from->getCode(), nccs->getCode());
	       targ.clear();
	       return -1;
	    }
	    case E2BIG:
	       al += STR_CLASS_BLOCK;
	       targ.allocate(al + 1);
	       break;
	    default:
	    {
	       xsink->raiseException("ENCODING-CONVERSION-ERROR", "error converting from \"%s\" to \"%s\": %s", 
				     from->getCode(), nccs->getCode(), strerror(errno));
	       targ.clear();
	       return -1;
	    }
	 }
      else
      {
	 // terminate string
	 targ.priv->buf[al - olen] = '\0';
	 targ.priv->len = al - olen;
	 break;
      }
   }
   return 0;
}

QoreString *QoreString::convertEncoding(const QoreEncoding *nccs, ExceptionSink *xsink) const
{
   printd(5, "QoreString::convertEncoding() from \"%s\" to \"%s\"\n", priv->charset->getCode(), nccs->getCode());

   if (nccs == priv->charset)
      return copy();
   if (!priv->len)
      return new QoreString(nccs);

   QoreString *targ = new QoreString(nccs);

   if (convert_encoding_intern(priv->buf, priv->len, priv->charset, *targ, nccs, xsink)) {
      delete targ;
      return 0;
   }
   return targ;
}

// endian-agnostic binary object -> base64 string function
// NOTE: not very high-performance - high-performance versions
//       would likely be endian-aware and operate directly on 32-bit words
void QoreString::concatBase64(const char *bbuf, qore_size_t size)
{
   //printf("bbuf=%08p, size=%d\n", bbuf, size);
   if (!size)
      return;

   unsigned char *p = (unsigned char *)bbuf;
   unsigned char *endbuf = p + size;
   while (p < endbuf)
   {
      // get 6 bits of data
      unsigned char c = p[0] >> 2;

      // byte 1: concat 1st 6-bit value
      concat(table64[c]);

      // byte 1: use remaining 2 bits in low order position
      c = (p[0] & 3) << 4;

      // check end
      if ((endbuf - p) == 1)
      {
	 concat(table64[c]);
	 concat("==");
	 break;
      }

      // byte 2: get 4 bits to make next 6-bit unit
      c |= p[1] >> 4;

      // concat 2nd 6-bit value
      concat(table64[c]);

      // byte 2: get 4 low bits
      c = (p[1] & 15) << 2;

      if ((endbuf - p) == 2)
      {
	 concat(table64[c]);
	 concat('=');
	 break;
      }

      // byte 3: get high 2 bits to make next 6-bit unit
      c |= p[2] >> 6;

      // concat 3rd 6-bit value
      concat(table64[c]);

      // byte 3: concat final 6 bits
      concat(table64[p[2] & 63]);
      p += 3;
   }
}

void QoreString::concatBase64(const BinaryNode *b)
{
   concatBase64((char *)b->getPtr(), b->size());
}

void QoreString::concatBase64(const QoreString *str)
{
   concatBase64(str->priv->buf, str->priv->len);
}


#define DO_HEX_CHAR(b) ((b) + (((b) > 9) ? 87 : 48))

void QoreString::concatHex(const char *binbuf, qore_size_t size)
{
   //printf("priv->buf=%08p, size=%d\n", binbuf, size);
   if (!size)
      return;

   unsigned char *p = (unsigned char *)binbuf;
   unsigned char *endbuf = p + size;
   while (p < endbuf)
   {
     char c = (*p & 0xf0) >> 4;
     concat(DO_HEX_CHAR(c));
     c = *p & 0x0f;
     concat(DO_HEX_CHAR(c));
     p++;
   }
}

// FIXME: this is slow, each concatenated character gets terminated as well
void QoreString::concatAndHTMLEncode(const QoreString *str, ExceptionSink *xsink)
{
   //printd(5, "QoreString::concatAndHTMLDecode() '%s'\n", str->getBuffer());

   // if it's not a null string
   if (str && str->priv->len)
   {
      TempEncodingHelper cstr(str, priv->charset, xsink);
      if (!cstr)
	 return;

      allocate(priv->len + cstr->priv->len + cstr->priv->len / 10 + 10); // avoid reallocations inside the loop, value guesstimated
      for (qore_size_t i = 0; i < cstr->priv->len; i++)
      {
	 // concatenate translated character
	 qore_size_t j;
	 for (j = 0; j < NUM_HTML_CODES; j++)
	    if (cstr->priv->buf[i] == html_codes[j].symbol)
	    {
	       concat(html_codes[j].code);
	       break;
	    }
	 // otherwise concatenate untranslated symbol
	 if (j == NUM_HTML_CODES)
	    concat(cstr->priv->buf[i]);
      }
   }
}

// FIXME: this is slow, each concatenated character gets terminated as well
void QoreString::concatAndHTMLEncode(const char *str)
{
   // if it's not a null string
   if (str)
   {
      qore_size_t i = 0;
      // iterate through new string
      while (str[i])
      {
	 // concatenate translated character
	 qore_size_t j;
	 for (j = 0; j < NUM_HTML_CODES; j++)
	    if (str[i] == html_codes[j].symbol)
	    {
	       concat(html_codes[j].code);
	       break;
	    }
	 // otherwise concatenate untranslated symbol
	 if (j == NUM_HTML_CODES)
	    concat(str[i]);
	 ++i;
      }
      /*
      // see if priv->buffer needs to be resized for '\0'
      priv->check_char(priv->len);
      // terminate string
      priv->buf[priv->len] = '\0';
      */
   }
}

// FIXME: this is slow, each concatenated character gets terminated as well
void QoreString::concatAndHTMLDecode(const QoreString *str)
{
   // if it's not a null string
   if (str && str->priv->len)
   {
      allocate(priv->len + str->priv->len); // avoid reallocations within the loop

      qore_size_t i = 0;
      while (str->priv->buf[i])
      {
         if (str->priv->buf[i] != '&') {
           concat(str->priv->buf[i++]);
           continue;
         }

	 // concatenate translated character
         const char* s = str->getBuffer() + i;
	 // check for unicode character references
	 if (*(s + 1) == '#')
	 {
	    s += 2;
	    // find end of character sequence
	    const char *e = strchr(s, ';');
	    // if not found or the number is too big, then don't try to decode it
	    if (e && (e - s) < 8)
	    {
	       unsigned code;
	       if (*s == 'x')
		  code = strtoul(s + 1, 0, 16);
	       else
		  code = strtoul(s, 0, 10);
	       
	       if (!concatUnicode(code))
	       {
		  i = e - str->priv->buf +1;
		  continue;
	       }
	       // error occurred, so back out
	       s -= 2;
	    }
	 }
         bool matched = false;
	 for (qore_size_t j = 0; j < NUM_HTML_CODES; j++) {
            bool found = true;
            for (qore_size_t k = 1; k < html_codes[j].len; ++k) {
               if (s[k] != html_codes[j].code[k]) {
                 found = false;
                 break;
               }
            }
            if (found) {
              concat(html_codes[j].symbol);
              i += html_codes[j].len;
              matched = true;
              break;
            }
         }
         if (!matched) {
	    //assert(false); // we should not abort with invalid HTML
	    concat(str->priv->buf[i++]);
         }
      }
   }
}

// return 0 for success
int QoreString::vsprintf(const char *fmt, va_list args)
{
   size_t fmtlen = ::strlen(fmt);
   // ensure minimum space is free
   if ((priv->allocated - priv->len - fmtlen) < MIN_SPRINTF_BUFSIZE)
   {
      priv->allocated += fmtlen + MIN_SPRINTF_BUFSIZE;
      // resize buffer
      priv->buf = (char *)realloc(priv->buf, priv->allocated * sizeof(char));
   }
   // set free buffer size
   qore_offset_t free = priv->allocated - priv->len;

   // copy formatted string to priv->buffer
   int i = ::vsnprintf(priv->buf + priv->len, free, fmt, args);

#ifdef HPUX
   // vsnprintf failed but didn't tell us how bug the priv->buffer should be
   if (i < 0)
   {
      //printf("DEBUG: vsnprintf() failed: i=%d priv->allocated=%d priv->len=%d priv->buf=%08p fmtlen=%d (new=i+%d = %d)\n", i, priv->allocated, priv->len, priv->buf, fmtlen, STR_CLASS_EXTRA, i + STR_CLASS_EXTRA);
      // resize priv->buffer
      priv->allocated += STR_CLASS_EXTRA;
      priv->buf = (char *)realloc(priv->buf, sizeof(char) * priv->allocated);
      *(priv->buf + priv->len) = '\0';
      return -1;
   }
#else
   if (i >= free)
   {
      //printd(5, "vsnprintf() failed: i=%d priv->allocated=%d priv->len=%d priv->buf=%08p fmtlen=%d (new=i+%d = %d)\n", i, priv->allocated, priv->len, priv->buf, fmtlen, STR_CLASS_EXTRA, i + STR_CLASS_EXTRA);
      // resize priv->buffer
      priv->allocated = priv->len + i + STR_CLASS_EXTRA;
      priv->buf = (char *)realloc(priv->buf, sizeof(char) * priv->allocated);
      *(priv->buf + priv->len) = '\0';
      return -1;
   }
#endif

   priv->len += i;
   return 0;
}

void QoreString::concat(const char *str)
{
   // if it's not a null string
   if (str)
   {
      qore_size_t i = 0;
      // iterate through new string
      while (str[i])
      {
	 // if priv->buffer needs to be resized
	 priv->check_char(priv->len);
	 // concatenate one character at a time
	 priv->buf[priv->len++] = str[i++];
      }
      // see if priv->buffer needs to be resized for '\0'
      priv->check_char(priv->len);
      // terminate string
      priv->buf[priv->len] = '\0';
   }
}

void QoreString::concat(const char *str, qore_size_t size)
{
   priv->check_char(priv->len + size);
   memcpy(priv->buf + priv->len, str, size);
   priv->len += size;
   priv->buf[priv->len] = '\0';
}

void QoreString::concat(const QoreString *str)
{
   // if it's not a null string
   if (str && str->priv->len) {
      // if priv->buffer needs to be resized
      priv->check_char(str->priv->len + priv->len + STR_CLASS_EXTRA);
      // concatenate new string
      memcpy(priv->buf + priv->len, str->priv->buf, str->priv->len);
      priv->len += str->priv->len;
      priv->buf[priv->len] = '\0';
   }
}

/*
void QoreString::concat(const QoreString *str, qore_size_t size)
{
   // if it's not a null string
   if (str && str->priv->len)
   {
      // if priv->buffer needs to be resized
      priv->check_char(str->priv->len + size);
      // concatenate new string
      memcpy(priv->buf + priv->len, str->priv->buf, size);
      priv->len += size;
      priv->buf[priv->len] = '\0';
   }
}
*/

void QoreString::concat(const QoreString *str, ExceptionSink *xsink)
{
   //printd(5, "concat() priv->buf='%s' str='%s'\n", priv->buf, str->priv->buf);
   // if it's not a null string
   if (str && str->priv->len)
   {
      TempEncodingHelper cstr(str, priv->charset, xsink);
      if (*xsink)
         return;

      // if priv->buffer needs to be resized
      priv->check_char(cstr->priv->len + priv->len + STR_CLASS_EXTRA);
      // concatenate new string
      memcpy(priv->buf + priv->len, cstr->priv->buf, cstr->priv->len);
      priv->len += cstr->priv->len;
      priv->buf[priv->len] = '\0';
   }
}

void QoreString::concat(const QoreString *str, qore_size_t size, ExceptionSink *xsink) {
   // if it's not a null string
   if (str && str->priv->len) {
      TempEncodingHelper cstr(str, priv->charset, xsink);
      if (*xsink)
         return;

      // adjust size for number of characters if this is a multi-byte character set
      if (priv->charset->isMultiByte()) {
	 size = priv->charset->getByteLen(cstr->priv->buf, cstr->priv->buf + cstr->priv->len, size, xsink);
	 if (*xsink)
	    return;
      }

      // if priv->buffer needs to be resized
      priv->check_char(cstr->priv->len + size + STR_CLASS_EXTRA);
      // concatenate new string
      memcpy(priv->buf + priv->len, cstr->priv->buf, size);
      priv->len += size;
      priv->buf[priv->len] = '\0';
   }
}

void QoreString::concat(char c) {
   if (priv->allocated) {
      priv->buf[priv->len] = c;
      priv->check_char(++priv->len);
      priv->buf[priv->len] = '\0';
      return;
   }
   // allocate new string priv->buffer
   priv->allocated = STR_CLASS_BLOCK;
   priv->len = 1;
   priv->buf = (char *)malloc(sizeof(char) * priv->allocated);
   priv->buf[0] = c;
   priv->buf[1] = '\0';
}

int QoreString::vsnprintf(size_t size, const char *fmt, va_list args) {
   // ensure minimum space is free
   if ((priv->allocated - priv->len) < (unsigned)size) {
      priv->allocated += (size + STR_CLASS_EXTRA);
      // resize priv->buffer
      priv->buf = (char *)realloc(priv->buf, priv->allocated * sizeof(char));
   }
   // copy formatted string to priv->buffer
   int i = ::vsnprintf(priv->buf + priv->len, size, fmt, args);
   priv->len += i;
   return i;
}

// returns 0 for success
int QoreString::sprintf(const char *fmt, ...) {
   va_list args;
   while (true) {
      va_start(args, fmt);
      int rc = vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }
   return 0;
}

int QoreString::snprintf(size_t size, const char *fmt, ...)
{
   va_list args;
   va_start(args, fmt);
   int i = vsnprintf(size, fmt, args);
   va_end(args);
   return i;
}

int QoreString::substr_simple(QoreString *ns, qore_offset_t offset, qore_offset_t length) const
{
   printd(5, "QoreString::substr_simple(offset=%d, length=%d) string=\"%s\" (this=%08p priv->len=%d)\n", 
	  offset, length, priv->buf, this, priv->len);

   qore_size_t n_offset;
   if (offset < 0)
      n_offset = priv->len + offset;
   else
      n_offset = offset;
   if (n_offset >= priv->len)  // if offset outside of string, return nothing
      return -1;

   qore_size_t n_length;
   if (length < 0)
   {
      length = priv->len - n_offset + length;
      if (length < 0)
	 n_length = 0;
      else
	 n_length = length;
   }
   else if ((qore_size_t)length > (priv->len - n_offset))
      n_length = priv->len - n_offset;
   else
      n_length = length;

   ns->concat(priv->buf + n_offset, n_length);
   return 0;
}

int QoreString::substr_simple(QoreString *ns, qore_offset_t offset) const
{
   printd(5, "QoreString::substr_simple(offset=%d) string=\"%s\" (this=%08p priv->len=%d)\n", 
	  offset, priv->buf, this, priv->len);

   qore_size_t n_offset;
   if (offset < 0)
      n_offset = priv->len + offset;
   else
      n_offset = offset;
   if (n_offset >= priv->len)  // if offset outside of string, return nothing
      return -1;

   // add length to ensure that the entire string is copied even if it has embedded nulls
   ns->concat(priv->buf + n_offset, priv->len - n_offset);
   return 0;
}

int QoreString::substr_complex(QoreString *ns, qore_offset_t offset, qore_offset_t length, ExceptionSink *xsink) const {
   QORE_TRACE("QoreString::substr_complex(offset, length)");
   printd(5, "QoreString::substr_complex(offset=%d, length=%d) string=\"%s\" (this=%08p priv->len=%d)\n", 
	  offset, length, priv->buf, this, priv->len);

   char *pend = priv->buf + priv->len;
   if (offset < 0) {
      int clength = priv->charset->getLength(priv->buf, pend, xsink);
      if (*xsink)
	 return -1;

      offset = clength + offset;

      if ((offset < 0) || (offset >= clength))  // if offset outside of string, return nothing
	 return -1;
   }

   qore_size_t start = priv->charset->getByteLen(priv->buf, pend, offset, xsink);
   if (*xsink)
      return -1;

   if (start == priv->len)
      return -1;

   if (length < 0) {
      length = priv->charset->getLength(priv->buf + start, pend, xsink) + length;
      if (*xsink)
	 return -1;

      if (length < 0)
	 length = 0;
   }
   qore_size_t end = priv->charset->getByteLen(priv->buf + start, pend, length, xsink);
   if (*xsink)
      return -1;

   ns->concat(priv->buf + start, end);
   return 0;
}

int QoreString::substr_complex(QoreString *ns, qore_offset_t offset, ExceptionSink *xsink) const {
   //printd(5, "QoreString::substr_complex(offset=%d) string=\"%s\" (this=%08p priv->len=%d)\n", offset, priv->buf, this, priv->len);
   char *pend = priv->buf + priv->len;
   if (offset < 0) {
      qore_size_t clength = priv->charset->getLength(priv->buf, pend, xsink);
      if (*xsink)
	 return -1;

      offset = clength + offset;

      if ((offset < 0) || ((qore_size_t)offset >= clength)) {  // if offset outside of string, return nothing
	 //printd(5, "this=%08p, priv->len=%d, offset=%d, clength=%d, priv->buf=%s\n", this, priv->len, offset, clength, priv->buf);
	 return -1;
      }
   }

   qore_size_t start = priv->charset->getByteLen(priv->buf, pend, offset, xsink);
   if (*xsink)
      return -1;

   //printd(5, "offset=%d, start=%d\n", offset, start);
   if (start == priv->len) {
      //printd(5, "this=%08p, priv->len=%d, offset=%d, priv->buf=%08p, start=d, %s\n", this, priv->len, offset, priv->buf, start, priv->buf);
      return -1;
   }

   // calculate byte offset
   ns->concat(priv->buf + start);
   return 0;
}

void QoreString::splice_simple(qore_size_t offset, qore_size_t num) {
   //printd(5, "splice_intern(offset=%d, num=%d, priv->len=%d)\n", offset, num, priv->len);
   qore_size_t end;
   if (num > (priv->len - offset)) {
      end = priv->len;
      num = priv->len - offset;
   }
   else
      end = offset + num;

   // move down entries if necessary
   if (end != priv->len)
      memmove(priv->buf + offset, priv->buf + end, sizeof(char) * (priv->len - end));

   // calculate new length
   priv->len -= num;
   // set last entry to NULL
   priv->buf[priv->len] = '\0';
}

void QoreString::splice_simple(qore_size_t offset, qore_size_t num, const char *str, qore_size_t str_len) {
   //printd(5, "splice_intern(offset=%d, num=%d, priv->len=%d)\n", offset, num, priv->len);

   qore_size_t end;
   if (num > (priv->len - offset)) {
      end = priv->len;
      num = priv->len - offset;
   }
   else
      end = offset + num;

   // get number of entries to insert
   if (str_len > num) { // make bigger
      qore_size_t ol = priv->len;
      priv->check_char(priv->len - num + str_len);
      // move trailing entries forward if necessary
      if (end != ol)
         memmove(priv->buf + (end - num + str_len), priv->buf + end, sizeof(char) * (ol - end));
   }
   else if (num > str_len) // make list smaller
      memmove(priv->buf + offset + str_len, priv->buf + offset + num, sizeof(char) * (priv->len - offset - str_len));

   memcpy(priv->buf + offset, str, str_len);

   // calculate new length
   priv->len = priv->len - num + str_len;
   // set last entry to NULL
   priv->buf[priv->len] = '\0';
}

void QoreString::splice_complex(qore_offset_t offset, ExceptionSink *xsink) {
   // get length in chars
   qore_size_t clen = priv->charset->getLength(priv->buf, priv->buf + priv->len, xsink);
   if (*xsink)
      return;

   //printd(0, "splice_complex(offset=%d) clen=%d\n", offset, clen);

   if (offset < 0) {
      offset = clen + offset;
      if (offset < 0)
	 offset = 0;
   }
   else if ((qore_size_t)offset >= clen)
      return;

   // calculate byte offset
   qore_size_t n_offset = offset ? priv->charset->getByteLen(priv->buf, priv->buf + priv->len, offset, xsink) : 0;
   if (*xsink)
      return;

   // truncate string at offset
   priv->len = n_offset;
   priv->buf[priv->len] = '\0';
}

void QoreString::splice_complex(qore_offset_t offset, qore_offset_t num, ExceptionSink *xsink) {
   //printd(5, "splice_complex(offset=%d, num=%d, priv->len=%d)\n", offset, num, priv->len);

   // get length in chars
   qore_size_t clen = priv->charset->getLength(priv->buf, priv->buf + priv->len, xsink);
   if (*xsink)
      return;

   if (offset < 0) {
      offset = clen + offset;
      if (offset < 0)
	 offset = 0;
   }
   else if ((qore_size_t)offset >= clen)
      return;

   if (num < 0) {
      num = clen + num - offset;
      if (num < 0)
	 num = 0;
   }

   qore_size_t end;
   if ((qore_size_t)num > (clen - offset)) {
      end = clen;
      num = clen - offset;
   }
   else
      end = offset + num;

   // get character positions
   offset = priv->charset->getByteLen(priv->buf, priv->buf + priv->len, offset, xsink);
   if (*xsink)
      return;

   end = priv->charset->getByteLen(priv->buf, priv->buf + priv->len, end, xsink);
   if (*xsink)
      return;

   num = priv->charset->getByteLen(priv->buf + offset, priv->buf + priv->len, num, xsink);
   if (*xsink)
      return;

   // move down entries if necessary
   if (end != priv->len)
      memmove(priv->buf + offset, priv->buf + end, sizeof(char) * (priv->len - end));

   // calculate new length
   priv->len -= num;

   // set last entry to NULL
   priv->buf[priv->len] = '\0';
}

void QoreString::splice_complex(qore_offset_t offset, qore_offset_t num, const QoreString *str, ExceptionSink *xsink) {
   // get length in chars
   qore_size_t clen = priv->charset->getLength(priv->buf, priv->buf + priv->len, xsink);
   if (*xsink)
      return;

   //printd(5, "splice_complex(offset=%d, num=%d, str='%s', priv->len=%d) clen=%d priv->buf='%s'\n", offset, num, str->getBuffer(), priv->len, clen, priv->buf);

   if (offset >= (qore_offset_t)clen)
      offset = clen;
   else if (offset < 0) {
      offset = clen + offset;
      if (offset < 0)
	 offset = 0;
   }

   if (num < 0) {
      num = clen + num - offset;
      if (num < 0)
	 num = 0;
   }

   qore_size_t end;
   if ((qore_size_t)num > (clen - offset)) {
      end = clen;
      num = clen - offset;
   }
   else
      end = offset + num;

   // get character positions
   char *endp = priv->buf + priv->len;
   offset = priv->charset->getByteLen(priv->buf, endp, offset, xsink);
   if (*xsink)
      return;

   end = priv->charset->getByteLen(priv->buf, endp, end, xsink);
   if (*xsink)
      return;

   num = priv->charset->getByteLen(priv->buf + offset, endp, num, xsink);
   if (*xsink)
      return;

   //printd(5, "offset=%d, end=%d, num=%d\n", offset, end, num);
   // get number of entries to insert
   if (str->priv->len > (qore_size_t)num) { // make bigger
      qore_size_t ol = priv->len;
      priv->check_char(priv->len - num + str->priv->len);
      // move trailing entries forward if necessary
      //printd(5, "priv->buf='%s'(%d), str='%s'(%d), end=%d, num=%d, newlen=%d\n", priv->buf, ol, str->priv->buf, str->priv->len, end, num, priv->len);
      if (end != ol)
         memmove(priv->buf + (end - num + str->priv->len), priv->buf + end, ol - end);
   }
   else if ((qore_size_t)num > str->priv->len) // make string smaller
      memmove(priv->buf + offset + str->priv->len, priv->buf + offset + num, sizeof(char) * (priv->len - offset - str->priv->len));

   memcpy(priv->buf + offset, str->priv->buf, str->priv->len);

   // calculate new length
   priv->len = priv->len - num + str->priv->len;

   // set last entry to NULL
   priv->buf[priv->len] = '\0';
}

// NULL values sorted at end
int QoreString::compareSoft(const QoreString *str, ExceptionSink *xsink) const
{
   if (!priv->buf) {
      if (!str->priv->buf)
	 return 0;
      else
	 return 1;
   }

   TempEncodingHelper t(str, priv->charset, xsink);
   if (*xsink)
      return 1;

   return strcmp(priv->buf, t->priv->buf);
}

void QoreString::concatEscape(const char *str, char c, char esc_char)
{
   // if it's not a null string
   if (str)
   {
      qore_size_t i = 0;
      // iterate through new string
      while (str[i])
      {
	 if (str[i] == c)
	 {
	    // check for space in priv->buffer
	    priv->check_char(priv->len + 1);
	    priv->buf[priv->len++] = esc_char;
	 }
	 else
	    priv->check_char(priv->len);
	 // concatenate one character at a time
	 priv->buf[priv->len++] = str[i++];
      }
      // see if priv->buffer needs to be resized for '\0'
      priv->check_char(priv->len);
      // terminate string
      priv->buf[priv->len] = '\0';
   }
}

void QoreString::concatEscape(const QoreString *str, char c, char esc_char, ExceptionSink *xsink)
{
   // if it's not a null string
   if (str && str->priv->len)
   {
      TempEncodingHelper cstr(str, priv->charset, xsink);
      if (*xsink)
	 return;

      // if priv->buffer needs to be resized
      priv->check_char(cstr->priv->len + priv->len);

      concatEscape(cstr->priv->buf, c, esc_char);
   }
}

QoreString *QoreString::substr(qore_offset_t offset, ExceptionSink *xsink) const {
   TempString str(new QoreString(priv->charset));

   int rc;
   if (!priv->charset->isMultiByte())
      rc = substr_simple(*str, offset);
   else
      rc = substr_complex(*str, offset, xsink);

   return !rc ? str.release() : 0;
}

QoreString *QoreString::substr(qore_offset_t offset, qore_offset_t length, ExceptionSink *xsink) const {
   TempString str(new QoreString(priv->charset));

   int rc;
   if (!priv->charset->isMultiByte())
      rc = substr_simple(*str, offset, length);
   else
      rc = substr_complex(*str, offset, length, xsink);

   return !rc ? str.release() : 0;
}

qore_size_t QoreString::length() const {
   if (priv->charset->isMultiByte() && priv->buf) {
      bool invalid;
      return priv->charset->getLength(priv->buf, priv->buf + priv->len, invalid);
   }
   return priv->len;
}

void QoreString::concat(const DateTime *d) {
   sprintf("%04d%02d%02d%02d%02d%02d", d->getYear(), d->getMonth(), d->getDay(), d->getHour(), d->getMinute(), d->getSecond());
}

void QoreString::concatISO8601DateTime(const DateTime *d) {
   sprintf("%04d%02d%02dT%02d:%02d:%02d", d->getYear(), d->getMonth(), d->getDay(), d->getHour(), d->getMinute(), d->getSecond());
}

void QoreString::concatHex(const BinaryNode *b) {
   concatHex((char *)b->getPtr(), b->size());
}

void QoreString::concatHex(const QoreString *str) {
   concatHex(str->priv->buf, str->priv->len);
}

// endian-agnostic base64 string -> binary object function
BinaryNode *QoreString::parseBase64(ExceptionSink *xsink) const {
   return ::parseBase64(priv->buf, priv->len, xsink);
}

// FIXME: implement possibility to specify character encoding
QoreString *QoreString::parseBase64ToString(ExceptionSink *xsink) const {
   SimpleRefHolder<BinaryNode> b(::parseBase64(priv->buf, priv->len, xsink));
   if (!b)
      return 0;

   qore_string_private *p = new qore_string_private;
   p->len = b->size() - 1;
   p->buf = (char *)b->giveBuffer();
   p->charset = QCS_DEFAULT;

   // free binary object
   b = 0;

   // check for null termination
   if (p->buf[p->len]) {
      ++p->len;
      p->buf = (char *)realloc(p->buf, p->len + 1);
      p->buf[p->len] = '\0';
   }

   p->allocated = p->len + 1;
   return new QoreString(p);
}

BinaryNode *QoreString::parseHex(ExceptionSink *xsink) const {
   return ::parseHex(priv->buf, priv->len, xsink);
}

void QoreString::allocate(unsigned requested_size) {
   if ((unsigned)priv->allocated >= requested_size) {
      return;  
   }
   requested_size = (requested_size / 16 + 1) * 16; // fill complete cache line
   char* aux = (char *)realloc(priv->buf, requested_size  * sizeof(char));
   if (!aux) {
     assert(false);
     // FIXME: std::bad_alloc() should be thrown here;
     return;
   }
   priv->buf = aux;
   priv->allocated = requested_size;
}

const QoreEncoding *QoreString::getEncoding() const 
{ 
   return priv->charset; 
}

QoreString *QoreString::copy() const
{
   return new QoreString(*this);
}

void QoreString::tolwr()
{
   char *c = priv->buf;
   while (*c)
   {
      *c = ::tolower(*c);
      c++;
   }
}

void QoreString::toupr()
{
   char *c = priv->buf;
   while (*c)
   {
      *c = ::toupper(*c);
      c++;
   }
}

// returns number of bytes
qore_size_t QoreString::strlen() const
{
   return priv->len;
}

const char *QoreString::getBuffer() const
{
   return priv->buf;
}

void QoreString::addch(char c, unsigned times)
{
   if (priv->allocated) {
      priv->check_char(priv->len + times + STR_CLASS_BLOCK); // more data will follow the padding
      memset(priv->buf + priv->len, c, times);
   } else {
      priv->allocated = times + STR_CLASS_BLOCK;
      priv->allocated = (priv->allocated / 16 + 1) * 16; // use complete cache line
      priv->buf = (char*)malloc(sizeof(char) * priv->allocated);
      memset(priv->buf, c, times);
   }
   priv->len += times;
   priv->buf[priv->len] = 0;
}

int QoreString::concatUnicode(unsigned code, ExceptionSink *xsink)
{
   if (priv->charset == QCS_UTF8)
   {
      concatUTF8FromUnicode(code);
      return 0;
   }

   QoreString tmp(QCS_UTF8);
   tmp.concatUTF8FromUnicode(code);
   TempString ns(tmp.convertEncoding(priv->charset, xsink));
   if (*xsink)
      return -1;
   concat(*ns);
   return 0;   
}

int QoreString::concatUnicode(unsigned code)
{
   if (priv->charset == QCS_UTF8)
   {
      concatUTF8FromUnicode(code);
      return 0;
   }
   QoreString tmp(QCS_UTF8);
   tmp.concatUTF8FromUnicode(code);

   ExceptionSink xsink;

   TempString ns(tmp.convertEncoding(priv->charset, &xsink));
   if (xsink) {
      // ignore exceptions
      xsink.clear();
      return -1;
   }

   concat(ns);
   return 0;   
}

void QoreString::concatUTF8FromUnicode(unsigned code)
{
   // 6-byte code
   if (code > 0x03ffffff)
   {
      concat(0xfc | (((1 << 30) & code) ? 1 : 0));
      concat(0x80 | ((code & (0x3f << 24)) >> 24));
      concat(0x80 | ((code & (0x3f << 18)) >> 18));
      concat(0x80 | ((code & (0x3f << 12)) >> 12));
      concat(0x80 | ((code & (0x3f << 6)) >> 6));
      concat(0x80 | (code & 0x3f));
   }
   else if (code > 0x001fffff) // 5-byte code
   {
      concat(0xf8 | ((code & (0x3 << 24)) >> 24));
      concat(0x80 | ((code & (0x3f << 18)) >> 18));
      concat(0x80 | ((code & (0x3f << 12)) >> 12));
      concat(0x80 | ((code & (0x3f << 6)) >> 6));
      concat(0x80 | (code & 0x3f));
   }
   else if (code > 0xffff) // 4-byte code
   {
      concat(0xf0 | ((code & (0x7 << 18)) >> 18));
      concat(0x80 | ((code & (0x3f << 12)) >> 12));
      concat(0x80 | ((code & (0x3f << 6)) >> 6));
      concat(0x80 | (code & 0x3f));
   }
   else if (code > 0x7ff) // 3-byte code
   {
      concat(0xe0 | ((code & (0xf << 12)) >> 12));
      concat(0x80 | ((code & (0x3f << 6)) >> 6));
      concat(0x80 | (code & 0x3f));
   }
   else if (code > 0x7f) // 2-byte code
   {
      concat(0xc0 | ((code & (0x2f << 6)) >> 6));
      concat(0x80 | (code & 0x3f));
   }
   else
      concat((char)code);
}

unsigned int QoreString::getUnicodePointFromUTF8(qore_offset_t offset) const {
   // get length in chars
   bool invalid;
   char *endp = priv->buf + priv->len;
   qore_size_t clen = priv->charset->getLength(priv->buf, endp, invalid);
   if (invalid)
      return 0;

   //printd(0, "splice_complex(offset=%d) clen=%d\n", offset, clen);
   if (offset < 0) {
      offset = clen + offset;
      if (offset < 0)
	 offset = 0;
   }
   else if ((qore_size_t)offset >= clen)
      return 0;

   // calculate byte offset
   if (offset) {
      offset = priv->charset->getByteLen(priv->buf, endp, offset, invalid);
      if (invalid)
	 return 0;
   }

   qore_size_t bl = priv->charset->getByteLen(priv->buf + offset, endp, 1, invalid);
   if (invalid)
      return 0;

   if (bl == 1)
      return priv->buf[offset];

   if (bl == 2)
      return ((priv->buf[offset] & 0x1f) << 6) | (priv->buf[offset + 1] & 0x3f);

   if (bl == 3)
      return ((priv->buf[offset] & 0x0f) << 12) | ((priv->buf[offset + 1] & 0x3f) << 6) | (priv->buf[offset + 2] & 0x3f);

   return (((unsigned)(priv->buf[offset] & 0x07)) << 18) 
      | (((unsigned)(priv->buf[offset + 1] & 0x3f)) << 12) 
      | ((((unsigned)priv->buf[offset + 2] & 0x3f)) << 6) 
      | (((unsigned)priv->buf[offset + 3] & 0x3f));
}

unsigned int QoreString::getUnicodePoint(qore_offset_t offset, ExceptionSink *xsink) const {
   TempEncodingHelper tmp(this, QCS_UTF8, xsink);
   if (*xsink)
      return 0;

   return tmp->getUnicodePointFromUTF8(offset);
}

QoreString *QoreString::reverse() const {
   QoreString *str = new QoreString(priv->charset);
   concat_reverse(str);
   return str;
}

// remove trailing char
void QoreString::trim_trailing(char c) {
   if (!priv->len)
      return;
   
   char *p = priv->buf + priv->len - 1;
   while (p >= priv->buf && (*p) == c)
      --p;
   
   terminate(p + 1 - priv->buf);
}

// remove leading char
void QoreString::trim_leading(char c) {
   if (!priv->len)
      return;
   
   qore_size_t i = 0;
   while (i < priv->len && priv->buf[i] == c)
      ++i;
   if (!i)
      return;
   
   memmove(priv->buf, priv->buf + i, priv->len + 1 - i);
   priv->len -= i;
}

// remove leading and trailing char
void QoreString::trim(char c) {
   trim_trailing(c);
   trim_leading(c);
}

// remove trailing chars
void QoreString::trim_trailing(const char *chars) {
   if (!priv->len)
      return;

   char *p = priv->buf + priv->len - 1;
   if (!chars) // use an alternate path here so we can check for embedded nulls as well
      while (p >= priv->buf && strnchr(default_whitespace, sizeof(default_whitespace), *p))
	 --p;
   else
      while (p >= priv->buf && strchr(chars, *p))
	 --p;

   terminate(p + 1 - priv->buf);
}

// remove leading char
void QoreString::trim_leading(const char *chars) {
   if (!priv->len)
      return;
   
   qore_size_t i = 0;
   if (!chars) 
      while (i < priv->len && strnchr(default_whitespace, sizeof(default_whitespace), priv->buf[i]))
	 ++i;
   else
      while (i < priv->len && strchr(chars, priv->buf[i]))
	 ++i;
   if (!i)
      return;
   
   memmove(priv->buf, priv->buf + i, priv->len + 1 - i);
   priv->len -= i;
}

// remove leading and trailing blanks
void QoreString::trim(const char *chars) {
   trim_trailing(chars);
   trim_leading(chars);
}

// writes a new QoreString with the characters reversed of the "this" QoreString
// assumes the encoding is the same and the length is 0
void QoreString::concat_reverse(QoreString *str) const {
   assert(str->priv->charset == priv->charset);
   assert(!str->priv->len);

   str->priv->check_char(priv->len);
   if (priv->charset->isMultiByte()) {
      char *p = priv->buf;
      char *end = str->priv->buf + priv->len;      
      while (*p) {
	 bool invalid;
	 int bl = priv->charset->getByteLen(p, end, 1, invalid);
	 if (invalid) // if we hit an invalid encoding, then we just copy bytes
	    bl = 1;
	 end -= bl;
	 // in case of corrupt data, make sure we don't go off the beginning of the string
	 if (end < str->priv->buf)
	    break;
	 strncpy(end, p, bl);
	 p += bl;
      }
   }
   else
      for (qore_size_t i = 0; i < priv->len; ++i)
	 str->priv->buf[i] = priv->buf[priv->len - i - 1];

   str->priv->buf[priv->len] = 0;
   str->priv->len = priv->len;
}
