/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_string_private.h

  QoreString private implementation

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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
      buf = (char *)malloc(sizeof(char) * allocated);
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
         buf = (char *)realloc(buf, allocated * sizeof(char));
      }
   }

   DLLLOCAL qore_size_t check_offset(qore_offset_t offset) {
      if (offset < 0) {
         offset = len + offset;
         return offset < 0 ? 0 : offset;
      }

      if ((qore_size_t)offset > len)
         return len;

      return offset;
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

   DLLLOCAL static qore_offset_t index_simple(const char *haystack, const char *needle, qore_offset_t pos = 0) {
      const char *p;
      if (!(p = strstr(haystack + pos, needle)))
         return -1;
      return (int)(p - haystack);
   }

   DLLLOCAL qore_offset_t index(const QoreString &needle, qore_offset_t pos, ExceptionSink *xsink) const {
      // do simple index
      if (!charset->isMultiByte()) {
         if (pos < 0) {
            pos = len + pos;
            if (pos < 0)
               pos = 0;
            return index_simple(buf, needle.getBuffer(), pos);
         }

         if (pos >= (qore_offset_t)len)
            return -1;

         return index_simple(buf, needle.getBuffer(), pos);
      }

      // do multibyte index()
      qore_size_t start;
      if (pos < 0) {
         pos = len + pos;
         if (pos < 0)
            pos = 0;
      }
      if (pos) {
         start = charset->getByteLen(buf, buf + len, pos, xsink);
         if (*xsink)
            return -1;

         if (start == len)
            return -1;
      }
      else
         start = 0;

      qore_offset_t ind = index_simple(buf + start, needle.getBuffer());
      if (ind != -1) {         
         ind = charset->getCharPos(buf, buf + start + ind, xsink);
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
         return index_simple(buf, needle, pos);
      }

      if (pos >= (qore_offset_t)len)
         return -1;

      return index_simple(buf, needle, pos);
   }

   // finds the last occurrence of needle in haystack at or before position pos
   // pos must be a non-negative valid byte offset in haystack
   static DLLLOCAL qore_offset_t rindex_simple(const char *haystack, qore_size_t hlen, const char *needle, qore_size_t nlen, qore_offset_t pos) {
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

   DLLLOCAL qore_offset_t rindex(const QoreString &needle, qore_offset_t pos, ExceptionSink *xsink) const {
      qore_offset_t ind;
      if (!charset->isMultiByte()) {
         if (pos == -1)
            pos = len - 1;
         else if (pos < 0)
            pos = len + pos;

         if (pos < 0)
            return -1;

         return rindex_simple(buf, len, needle.getBuffer(), needle.strlen(), pos);      
      }

      // do multi-byte rindex
      int l = len;
      if (pos == -1)
         pos = l - 1;
      else if (pos < 0)
         pos = l + pos;

      if (pos < 0)
         return -1;
         
      // calculate byte position from character position
      if (pos) {
         pos = charset->getByteLen(buf, buf + len, pos, xsink);
         if (*xsink)
            return 0;
      }
      // get byte rindex position
      ind = rindex_simple(buf, len, needle.getBuffer(), needle.strlen(), pos);

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
};

#endif
