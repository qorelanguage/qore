/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_string_private.h

  QoreString private implementation

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#ifndef QORE_QORE_STRING_PRIVATE_H
#define QORE_QORE_STRING_PRIVATE_H

#define MAX_INT_STRING_LEN     48
#define MAX_BIGINT_STRING_LEN  48
#define MAX_FLOAT_STRING_LEN   48
#define STR_CLASS_BLOCK        80
#define STR_CLASS_EXTRA        40

#define MIN_SPRINTF_BUFSIZE   120

#define QUS_PATH     0
#define QUS_QUERY    1
#define QUS_FRAGMENT 2

struct qore_string_private {
private:

public:
   qore_size_t len;
   qore_size_t allocated;
   char* buf;
   const QoreEncoding* charset;

   DLLLOCAL qore_string_private() {
   }

   DLLLOCAL qore_string_private(const qore_string_private &p) {
      allocated = p.len + STR_CLASS_EXTRA;
      buf = (char*)malloc(sizeof(char) * allocated);
      len = p.len;
      if (len)
         memcpy(buf, p.buf, len);
      buf[len] = '\0';
      charset = p.getEncoding();
   }

   DLLLOCAL ~qore_string_private() {
      if (buf)
         free(buf);
   }

   DLLLOCAL void check_char(qore_size_t i) {
      if (i >= allocated) {
         qore_size_t d = i >> 2;
         allocated = i + (d < STR_CLASS_BLOCK ? STR_CLASS_BLOCK : d);
         //allocated = i + STR_CLASS_BLOCK;
         allocated = (allocated / 16 + 1) * 16; // use complete cache line
         buf = (char*)realloc(buf, allocated * sizeof(char));
      }
   }

   DLLLOCAL qore_size_t check_offset(qore_offset_t offset) {
      if (offset < 0) {
         offset = len + offset;
         return offset < 0 ? 0 : offset;
      }

      return ((qore_size_t)offset > len) ? len : offset;
   }

   DLLLOCAL void check_offset(qore_offset_t offset, qore_offset_t num, qore_size_t &n_offset, qore_size_t &n_num) {
      n_offset = check_offset(offset);

      if (num < 0) {
         num = len + num - n_offset;
         if (num < 0)
            n_num = 0;
         else
            n_num = num;
         return;
      }
      n_num = num;
   }

   // NOTE: this is purely byte oriented - no character semantics here
   DLLLOCAL qore_offset_t find(char c, qore_offset_t pos = 0) {
      if (pos < 0) {
         pos = len + pos;
         if (pos < 0)
            pos = 0;
      }
      else if (pos > 0 && pos > (qore_offset_t)len)
         return -1;
      const char* p;
      if (!(p = strchr(buf + pos, c)))
         return -1;
      return (qore_offset_t)(p - buf);
   }

   // NOTE: this is purely byte oriented - no character semantics here
   DLLLOCAL qore_offset_t rfind(char c, qore_offset_t pos = -1) {
      if (pos < 0) {
         pos = len + pos;
         if (pos < 0)
            return -1;
      }
      else if (pos > 0 && pos > (qore_offset_t)len)
         pos = len - 1;

      const char* p = buf + pos;
      while (p >= buf) {
         if (*p == c)
            return (qore_offset_t)(p - buf);
         --p;
      }
      return -1;
   }

      // NOTE: this is purely byte oriented - no character semantics here
   DLLLOCAL qore_offset_t findAny(const char* str, qore_offset_t pos = 0) {
      if (pos < 0) {
         pos = len + pos;
         if (pos < 0)
            pos = 0;
      }
      else if (pos > 0 && pos > (qore_offset_t)len)
         return -1;
      const char* p;
      if (!(p = strstr(buf + pos, str)))
         return -1;
      return (qore_offset_t)(p - buf);
   }

   // NOTE: this is purely byte oriented - no character semantics here
   DLLLOCAL qore_offset_t rfindAny(const char* str, qore_offset_t pos = -1) {
      if (pos < 0) {
         pos = len + pos;
         if (pos < 0)
            return -1;
      }
      else if (pos > 0 && pos > (qore_offset_t)len)
         pos = len - 1;

      const char* p = buf + pos;
      while (p >= buf) {
         for (const char* t = str; *t; ++t) {
            if (*p == *t)
               return (qore_offset_t)(p - buf);
         }
         --p;
      }
      return -1;
   }

   DLLLOCAL static qore_offset_t index_simple(const char *haystack, const char *needle, qore_offset_t pos = 0) {
      const char *p;
      if (!(p = strstr(haystack + pos, needle)))
         return -1;
      return (qore_offset_t)(p - haystack);
   }

   DLLLOCAL qore_offset_t index(const QoreString &orig_needle, qore_offset_t pos, ExceptionSink *xsink) const {
      assert(xsink);
      TempEncodingHelper needle(orig_needle, getEncoding(), xsink);
      if (!needle)
         return -1;

      // do simple index
      if (!getEncoding()->isMultiByte()) {
         if (pos < 0) {
            pos = len + pos;
            if (pos < 0)
               pos = 0;
         }
         else if (pos >= (qore_offset_t)len)
            return -1;

         return index_simple(buf, needle->getBuffer(), pos);
      }

      // do multibyte index()
      if (findByteOffset(pos, xsink))
         return -1;
      if (pos < 0)
         pos = 0;
      else if (pos >= (qore_offset_t)len)
         return -1;

      qore_offset_t ind = index_simple(buf + pos, needle->getBuffer());
      if (ind != -1) {
         ind = getEncoding()->getCharPos(buf, buf + pos + ind, xsink);
         if (*xsink)
            return -1;
      }

      return ind;
   }

   DLLLOCAL qore_offset_t bindex(const QoreString &needle, qore_offset_t pos) const {
      if (needle.strlen() + pos > len)
         return -1;

      return bindex(needle.getBuffer(), pos);
   }

   DLLLOCAL qore_offset_t bindex(const std::string &needle, qore_offset_t pos) const {
      if (needle.size() + pos > len)
         return -1;

      return bindex(needle.c_str(), pos);
   }

   DLLLOCAL qore_offset_t bindex(const char *needle, qore_offset_t pos) const {
      if (pos < 0) {
         pos = len + pos;
         if (pos < 0)
            pos = 0;
      }
      else if (pos >= (qore_offset_t)len)
         return -1;

      return index_simple(buf, needle, pos);
   }

   // finds the last occurrence of needle in haystack at or before position pos
   // pos must be a non-negative valid byte offset in haystack
   DLLLOCAL static qore_offset_t rindex_simple(const char *haystack, qore_size_t hlen, const char *needle, qore_size_t nlen, qore_offset_t pos) {
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

   // start is a byte offset that has to point to the start of a valid character
   DLLLOCAL int findByteOffset(qore_offset_t& pos, ExceptionSink* xsink, qore_size_t start = 0) const {
      assert(xsink);
      assert(getEncoding()->isMultiByte());
      if (!pos)
         return 0;
      // get positive character offset if negative
      if (pos < 0) {
         // get the length of the string in characters
         qore_size_t clen = getEncoding()->getLength(buf + start, buf + len, xsink);
         if (*xsink)
            return -1;
         pos = clen + pos;
      }
      // now get the byte position from this character offset
      pos = getEncoding()->getByteLen(buf + start, buf + len, pos, xsink);
      return *xsink ? -1 : 0;
   }

   DLLLOCAL qore_offset_t rindex(const QoreString &orig_needle, qore_offset_t pos, ExceptionSink *xsink) const {
      assert(xsink);
      TempEncodingHelper needle(orig_needle, getEncoding(), xsink);
      if (!needle)
         return -1;

      if (!getEncoding()->isMultiByte()) {
         if (pos < 0) {
            pos = len + pos;
            if (pos < 0)
               return -1;
         }

         return rindex_simple(buf, len, needle->getBuffer(), needle->strlen(), pos);
      }

      // do multi-byte rindex
      if (findByteOffset(pos, xsink))
         return -1;
      if (pos < 0)
         return -1;

      // get byte rindex position
      qore_offset_t ind = rindex_simple(buf, len, needle->getBuffer(), needle->strlen(), pos);

      // calculate character position from byte position
      if (ind && ind != -1) {
         ind = getEncoding()->getCharPos(buf, buf + ind, xsink);
         if (*xsink)
            return 0;
      }

      return ind;
   }

   DLLLOCAL qore_offset_t brindex(const QoreString &needle, qore_offset_t pos) const {
      return brindex(needle.getBuffer(), needle.strlen(), pos);
   }

   DLLLOCAL qore_offset_t brindex(const std::string &needle, qore_offset_t pos) const {
      return brindex(needle.c_str(), needle.size(), pos);
   }

   DLLLOCAL qore_offset_t brindex(const char *needle, qore_size_t needle_len, qore_offset_t pos) const {
      if (needle_len + pos > len)
         return -1;

      if (pos < 0)
         pos = len + pos;

      if (pos < 0)
         return -1;

      return rindex_simple(buf, len, needle, needle_len, pos);
   }

   DLLLOCAL bool isDataPrintableAscii() const {
      for (qore_size_t i = 0; i < len; ++i) {
         if (buf[i] < 32 || buf[i] > 126)
            return false;
      }
      return true;
   }

   DLLLOCAL bool isDataAscii() const {
      for (qore_size_t i = 0; i < len; ++i) {
         if ((unsigned char)(buf[i]) > 127)
            return false;
      }
      return true;
   }

   DLLLOCAL void concat_intern(const char* p, qore_size_t plen) {
      assert(p);
      assert(plen);
      check_char(len + plen);
      memcpy(buf + len, p, plen);
      len += plen;
      buf[len] = '\0';
   }

   DLLLOCAL void concat_simple(const qore_string_private& str, qore_offset_t pos) {
      if (pos < 0) {
         pos = str.len + pos;
         if (pos < 0)
            pos = 0;
      }
      else if (pos >= (qore_offset_t)str.len)
         return;

      concat_intern(str.buf + pos, str.len - pos);
   }

   DLLLOCAL int concat(const qore_string_private& str, qore_offset_t pos, ExceptionSink* xsink) {
      assert(str.getEncoding() == getEncoding());

      if (!getEncoding()->isMultiByte()) {
         concat_simple(str, pos);
         return 0;
      }

      // find byte positions from character positions
      if (pos) {
         if (str.findByteOffset(pos, xsink))
            return -1;
         if (pos < 0)
            pos = 0;
         else if (pos > (qore_offset_t)str.len)
            return 0;
      }

      concat_intern(str.buf + pos, str.len - pos);
      return 0;
   }

   DLLLOCAL void concat_simple(const qore_string_private& str, qore_offset_t pos, qore_offset_t plen) {
      if (pos < 0) {
         pos = str.len + pos;
         if (pos < 0)
            pos = 0;
      }
      else if (pos >= (qore_offset_t)str.len)
         return;

      if (plen < 0) {
         plen = str.len + plen;
         if (plen <= 0)
            return;
      }
      else if (plen > (qore_offset_t)str.len)
         plen = str.len;

      concat_intern(str.buf + pos, plen);
   }

   DLLLOCAL int concat(const qore_string_private& str, qore_offset_t pos, qore_offset_t plen, ExceptionSink* xsink) {
      assert(str.getEncoding() == getEncoding());
      assert(plen);

      if (!getEncoding()->isMultiByte()) {
         concat_simple(str, pos);
         return 0;
      }

      // find byte positions from character positions
      if (pos) {
         if (str.findByteOffset(pos, xsink))
            return -1;
         if (pos < 0)
            pos = 0;
         else if (pos > (qore_offset_t)str.len)
            return 0;
      }

      // find the byte position from the starting byte
      if (str.findByteOffset(plen, xsink, pos))
         return -1;
      if (plen <= 0)
         return 0;
      if (plen > (qore_offset_t)str.len)
         plen = str.len;

      concat_intern(str.buf + pos, plen);
      return 0;
   }

   DLLLOCAL qore_offset_t getByteOffset(qore_size_t i, ExceptionSink* xsink) const {
      assert(xsink);
      qore_size_t rc;
      if (i) {
         rc = getEncoding()->getByteLen(buf, buf + len, i, xsink);
         if (*xsink)
            return -1;
      }
      else
         rc = 0;
      return rc > len ? -1 : (qore_offset_t)rc;
   }

   DLLLOCAL void concat(char c) {
      if (allocated) {
         buf[len] = c;
         check_char(++len);
         buf[len] = '\0';
         return;
      }
      // allocate new string buffer
      allocated = STR_CLASS_BLOCK;
      len = 1;
      buf = (char *)malloc(sizeof(char) * allocated);
      buf[0] = c;
      buf[1] = '\0';
   }

   DLLLOCAL void concat(const qore_string_private* str) {
      assert(!str || (str->charset == charset) || !str->charset);

      // if it's not a null string
      if (str && str->len) {
         // if priv->buffer needs to be resized
         check_char(str->len + len + STR_CLASS_EXTRA);
         // concatenate new string
         memcpy(buf + len, str->buf, str->len);
         len += str->len;
         buf[len] = '\0';
      }
   }

   DLLLOCAL void concat(const char *str) {
      // if it's not a null string
      if (str) {
         qore_size_t i = 0;
         // iterate through new string
         while (str[i]) {
            // if priv->buffer needs to be resized
            check_char(len);
            // concatenate one character at a time
            buf[len++] = str[i++];
         }
         // see if priv->buffer needs to be resized for '\0'
         check_char(len);
         // terminate string
         buf[len] = '\0';
      }
   }

   DLLLOCAL int concat(const QoreString* str, ExceptionSink* xsink);

   // return 0 for success
   DLLLOCAL int vsprintf(const char *fmt, va_list args) {
      size_t fmtlen = ::strlen(fmt);
      // ensure minimum space is free
      if ((allocated - len - fmtlen) < MIN_SPRINTF_BUFSIZE) {
         allocated += fmtlen + MIN_SPRINTF_BUFSIZE;
         // resize buffer
         buf = (char *)realloc(buf, allocated * sizeof(char));
      }
      // set free buffer size
      qore_offset_t free = allocated - len;

      // copy formatted string to buffer
      int i = ::vsnprintf(buf + len, free, fmt, args);

#ifdef HPUX
      // vsnprintf failed but didn't tell us how big the buffer should be
      if (i < 0) {
         //printf("DEBUG: vsnprintf() failed: i=%d allocated="QSD" len="QSD" buf=%p fmtlen="QSD" (new=i+%d = %d)\n", i, allocated, len, buf, fmtlen, STR_CLASS_EXTRA, i + STR_CLASS_EXTRA);
         // resize buffer
         allocated += STR_CLASS_EXTRA;
         buf = (char *)realloc(buf, sizeof(char) * allocated);
         *(buf + len) = '\0';
         return -1;
      }
#else
      if (i >= free) {
         //printf("DEBUG: vsnprintf() failed: i=%d allocated="QSD" len="QSD" buf=%p fmtlen="QSD" (new=i+%d = %d)\n", i, allocated, len, buf, fmtlen, STR_CLASS_EXTRA, i + STR_CLASS_EXTRA);
         // resize buffer
         allocated = len + i + STR_CLASS_EXTRA;
         buf = (char *)realloc(buf, sizeof(char) * allocated);
         *(buf + len) = '\0';
         return -1;
      }
#endif

      len += i;
      return 0;
   }

   DLLLOCAL int sprintf(const char *fmt, ...) {
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

   DLLLOCAL void concatUTF8FromUnicode(unsigned code);

   DLLLOCAL int concatUnicode(unsigned code, ExceptionSink *xsink) {
      assert(xsink);
      if (getEncoding() == QCS_UTF8) {
         concatUTF8FromUnicode(code);
         return 0;
      }

      QoreString tmp(QCS_UTF8);
      tmp.concatUTF8FromUnicode(code);
      TempString ns(tmp.convertEncoding(getEncoding(), xsink));
      if (*xsink)
         return -1;
      concat(ns->priv);
      return 0;
   }

   DLLLOCAL int concatUnicode(unsigned code);

   DLLLOCAL int concatDecodeUriIntern(ExceptionSink* xsink, const qore_string_private& str, bool detect_query = false);

   DLLLOCAL int concatEncodeUriRequest(ExceptionSink* xsink, const qore_string_private& str);

   DLLLOCAL unsigned int getUnicodePointFromBytePos(qore_size_t offset, unsigned& len, ExceptionSink* xsink) const;

   DLLLOCAL int concatEncode(ExceptionSink* xsink, const QoreString& str, unsigned code = CE_XHTML);
   DLLLOCAL int concatDecode(ExceptionSink* xsink, const QoreString& str, unsigned code = CD_ALL);

   DLLLOCAL int allocate(unsigned requested_size) {
      if ((unsigned)allocated >= requested_size)
         return 0;
      requested_size = (requested_size / 16 + 1) * 16; // fill complete cache line
      char* aux = (char*)realloc(buf, requested_size * sizeof(char));
      if (!aux) {
         assert(false);
         // FIXME: std::bad_alloc() should be thrown here;
         return -1;
      }
      buf = aux;
      allocated = requested_size;
      return 0;
   }

   DLLLOCAL const QoreEncoding* getEncoding() const {
      return charset ? charset : QCS_USASCII;
   }

   DLLLOCAL static int getHex(const char*& p) {
      if (*p == '%' && isxdigit(*(p + 1)) && isxdigit(*(p + 2))) {
         char x[3] = { *(p + 1), *(p + 2), '\0' };
         p += 3;
         return strtol(x, 0, 16);
      }
      return -1;
   }

   DLLLOCAL static int convert_encoding_intern(const char* src, qore_size_t src_len, const QoreEncoding* from, QoreString& targ, const QoreEncoding* nccs, ExceptionSink* xsink);
};

#endif
