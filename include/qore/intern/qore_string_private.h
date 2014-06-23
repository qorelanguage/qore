/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_string_private.h

  QoreString private implementation

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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

struct qore_string_private {
private:

public:
   qore_size_t len;
   qore_size_t allocated;
   char *buf;
   const QoreEncoding *charset;

   DLLLOCAL qore_string_private() {
   }

   DLLLOCAL qore_string_private(const qore_string_private &p) {
      allocated = p.len + STR_CLASS_EXTRA;
      buf = (char*)malloc(sizeof(char) * allocated);
      len = p.len;
      if (len)
         memcpy(buf, p.buf, len);
      buf[len] = '\0';
      charset = p.charset;
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

   DLLLOCAL static qore_offset_t index_simple(const char *haystack, const char *needle, qore_offset_t pos = 0) {
      const char *p;
      if (!(p = strstr(haystack + pos, needle)))
         return -1;
      return (qore_offset_t)(p - haystack);
   }

   DLLLOCAL qore_offset_t index(const QoreString &orig_needle, qore_offset_t pos, ExceptionSink *xsink) const {
      TempEncodingHelper needle(orig_needle, charset, xsink);
      if (!needle)
         return -1;

      // do simple index
      if (!charset->isMultiByte()) {
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
         ind = charset->getCharPos(buf, buf + pos + ind, xsink);
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
      assert(charset->isMultiByte());
      if (!pos)
         return 0;
      // get positive character offset if negative
      if (pos < 0) {
         // get the length of the string in characters
         qore_size_t clen = charset->getLength(buf + start, buf + len, xsink);
         if (*xsink)
            return -1;
         pos = clen + pos;
      }
      // now get the byte position from this character offset
      pos = charset->getByteLen(buf + start, buf + len, pos, xsink);
      return *xsink ? -1 : 0;
   }

   DLLLOCAL qore_offset_t rindex(const QoreString &orig_needle, qore_offset_t pos, ExceptionSink *xsink) const {
      TempEncodingHelper needle(orig_needle, charset, xsink);
      if (!needle)
         return -1;

      if (!charset->isMultiByte()) {
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
         ind = charset->getCharPos(buf, buf + ind, xsink);
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
      assert(str.charset == charset);

      if (!charset->isMultiByte()) {
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
      assert(str.charset == charset);
      assert(plen);

      if (!charset->isMultiByte()) {
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
      qore_size_t rc;
      if (i) {
         rc = charset->getByteLen(buf, buf + len, i, xsink);
         if (*xsink)
            return -1;
      }
      else
         rc = 0;
      return rc > len ? -1 : (qore_offset_t)rc;
   }
};

#endif
