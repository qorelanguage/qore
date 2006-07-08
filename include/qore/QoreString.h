/*
  QoreString.h

  QoreString Class Definition

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

#ifndef _QORE_QORESTRING_H

#define _QORE_QORESTRING_H

#include <qore/common.h>

#include <stdarg.h>
#include <ctype.h>

#define MAX_INT_STRING_LEN    48
#define MAX_BIGINT_STRING_LEN 48
#define MAX_FLOAT_STRING_LEN  48
#define STR_CLASS_BLOCK 80
#define STR_CLASS_EXTRA 40

struct code_table {
      char symbol;
      char *code;
      int len;
};

#define NUM_HTML_CODES 4
extern class QoreEncoding *QCS_DEFAULT;
extern class code_table html_codes[];

class QoreString {
      int len;
      int allocated;
      char *buf;
      class QoreEncoding *charset;

      inline void check_char(int i);
      inline void check_offset(int &offset);
      inline void check_offset(int &offset, int &num);
      void splice_simple(int offset, int length, class ExceptionSink *xsink);
      void splice_simple(int offset, int length, class QoreString *str, class ExceptionSink *xsink);
      void splice_complex(int offset, class ExceptionSink *xsink);
      void splice_complex(int offset, int length, class ExceptionSink *xsink);
      void splice_complex(int offset, int length, class QoreString *str, class ExceptionSink *xsink);
      class QoreString *substr_simple(int offset);
      class QoreString *substr_simple(int offset, int length);
      class QoreString *substr_complex(int offset);
      class QoreString *substr_complex(int offset, int length);

   public:
      inline QoreString();
      inline QoreString(bool b);
      inline QoreString(char *);
      inline QoreString(char *str, class QoreEncoding *new_qorecharset);
      inline QoreString(class QoreEncoding *new_qorecharset);
      inline QoreString(char *str, int len, class QoreEncoding *new_qorecharset = QCS_DEFAULT);
      inline QoreString(char);
      inline QoreString(QoreString *str);
      inline QoreString(QoreString *, int);
      inline QoreString(int64);
      inline QoreString(double);
      inline QoreString(class DateTime *);
      inline QoreString(class BinaryObject *);
      inline ~QoreString();
      inline int strlen();
      inline int length();
      inline char *getBuffer();
      inline void set(char *str, class QoreEncoding *new_qorecharset = QCS_DEFAULT);
      void concatAndHTMLEncode(char *);
      void concatAndHTMLDecode(QoreString *str);
      void concat(QoreString *);

      // concatenation with character set conversion
      void concatAndHTMLEncode(QoreString *, class ExceptionSink *xsink);
      void concat(QoreString *, class ExceptionSink *xsink);
      // in the following method, size refers to the number of characters, not bytes
      void concat(QoreString *, int size, class ExceptionSink *xsink);

      void concatBase64(char *buf, int size);
      void concatBase64(class BinaryObject *bin);
      inline void concatBase64(class QoreString *str);
      inline void concatISO8601DateTime(class DateTime *d);
      inline void concat(class DateTime *d);
      void concat(char *);
      void concat(char *, int size);
      void concat(char);
      int compareSoft(QoreString *, class ExceptionSink *);
      inline int compare(QoreString *);
      inline int compare(char *);
      inline void terminate(int);
      int sprintf(const char *fmt, ...);
      int snprintf(int size, const char *fmt, ...);
      int vsprintf(const char *fmt, va_list args);
      int vsnprintf(int size, const char *fmt, va_list args);
      inline void take(char *);
      inline void take(char *, class QoreEncoding *new_charset);
      inline void take(char *, int size);
      inline class QoreEncoding *getEncoding() { return charset; }
      class QoreString *convertEncoding(class QoreEncoding *nccs, class ExceptionSink *xsink);
      inline class QoreString *copy();
      inline char *giveBuffer();
      inline void clear();
      inline class BinaryObject *parseBase64(class ExceptionSink *xsink);
      inline void replace(int offset, int len, char *str);
      inline void replace(int offset, int len, class QoreString *str);
      inline void splice(int offset, class ExceptionSink *xsink);
      inline void splice(int offset, int length, class ExceptionSink *xsink);
      inline void splice(int offset, int length, class QoreNode *strn, class ExceptionSink *xsink);
      inline class QoreString *substr(int offset);
      inline class QoreString *substr(int offset, int length);
      inline void tolwr()
      {
	 char *c = buf;
	 while (*c)
	 {
	    *c = ::tolower(*c);
	    c++;
	 }
      }
      inline void toupr()
      {
	 char *c = buf;
	 while (*c)
	 {
	    *c = ::toupper(*c);
	    c++;
	 }
      }
};

#include <qore/support.h>
#include <qore/DateTime.h>
#include <qore/Exception.h>
#include <qore/charset.h>
#include <qore/BinaryObject.h>
#include <qore/QoreLib.h>
#include <qore/QoreType.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>

inline void QoreString::check_char(int i)
{
   if (i >= allocated)
   {
      allocated = i + STR_CLASS_BLOCK;
      buf = (char *)realloc(buf, allocated * sizeof(char));
   }
}

inline QoreString::QoreString()
{
   len = 0;
   allocated = STR_CLASS_BLOCK;
   buf = (char *)malloc(allocated * sizeof(char));
   buf[0] = '\0';
   charset = QCS_DEFAULT;
}

// FIXME: this is not very efficient with the array offsets...
inline QoreString::QoreString(char *str)
{
   len = 0;
   allocated = STR_CLASS_BLOCK;
   buf = (char *)malloc(allocated * sizeof(char));
   if (str)
   {
      while (str[len])
      {
	 check_char(len);
	 buf[len] = str[len];
	 len++;
      }
      check_char(len);
      buf[len] = '\0';
   }
   else
      buf[0] = '\0';
   charset = QCS_DEFAULT;
}

// FIXME: this is not very efficient with the array offsets...
inline QoreString::QoreString(char *str, class QoreEncoding *new_qorecharset)
{
   len = 0;
   allocated = STR_CLASS_BLOCK;
   buf = (char *)malloc(allocated * sizeof(char));
   if (str)
   {
      while (str[len])
      {
	 check_char(len);
	 buf[len] = str[len];
	 len++;
      }
      check_char(len);
      buf[len] = '\0';
   }
   else
      buf[0] = '\0';
   charset = new_qorecharset;
}

inline QoreString::QoreString(class QoreEncoding *new_qorecharset)
{
   len = 0;
   allocated = STR_CLASS_BLOCK;
   buf = (char *)malloc(allocated * sizeof(char));
   buf[0] = '\0';
   charset = new_qorecharset;
}

inline QoreString::QoreString(char *str, int size, class QoreEncoding *new_qorecharset)
{
   len = size;
   allocated = size + STR_CLASS_EXTRA;
   buf = (char *)malloc(sizeof(char) * allocated);
   memcpy(buf, str, size);
   buf[size] = '\0';
   charset = new_qorecharset;
}

inline QoreString::QoreString(QoreString *str)
{
   allocated = str->len + STR_CLASS_EXTRA;
   buf = (char *)malloc(sizeof(char) * allocated);
   len = str->len;
   if (str->len)
      memcpy(buf, str->buf, str->len);
   buf[len] = '\0';
   charset = str->charset;
}

inline QoreString::QoreString(QoreString *str, int size)
{
   if (size >= str->len)
      size = str->len;
   len = size;
   allocated = size + STR_CLASS_EXTRA;
   buf = (char *)malloc(sizeof(char) * allocated);
   if (size)
      memcpy(buf, str->buf, size);
   buf[size] = '\0';
   charset = str->charset;
}

inline QoreString::QoreString(char c)
{
   len = 1;
   allocated = STR_CLASS_BLOCK;
   buf = (char *)malloc(sizeof(char) * allocated);
   buf[0] = c;
   buf[1] = '\0';
   charset = QCS_DEFAULT;
}

inline QoreString::QoreString(int64 i)
{
   allocated = MAX_BIGINT_STRING_LEN + 1;
   buf = (char *)malloc(sizeof(char) * allocated);
   len = ::snprintf(buf, MAX_BIGINT_STRING_LEN, "%lld", i);
   // terminate string just in case
   buf[MAX_BIGINT_STRING_LEN] = '\0';
   charset = QCS_DEFAULT;
}

inline QoreString::QoreString(bool b)
{
   allocated = 2;
   buf = (char *)malloc(sizeof(char) * allocated);
   len = ::sprintf(buf, "%d", b);
   charset = QCS_DEFAULT;
}

inline QoreString::QoreString(double f)
{
   allocated = MAX_FLOAT_STRING_LEN + 1;
   buf = (char *)malloc(sizeof(char) * allocated);
   len = ::snprintf(buf, MAX_FLOAT_STRING_LEN, "%.9g", f);
   // terminate string just in case
   buf[MAX_FLOAT_STRING_LEN] = '\0';
   charset = QCS_DEFAULT;
}

inline QoreString::QoreString(class DateTime *d)
{
   allocated = 15;
   buf = (char *)malloc(sizeof(char) * allocated);
   len = ::sprintf(buf, "%04d%02d%02d%02d%02d%02d",
		   d->year, d->month, d->day,
		   d->hour, d->minute, d->second);
   charset = QCS_DEFAULT;
}

inline QoreString::QoreString(class BinaryObject *b)
{
   allocated = STR_CLASS_BLOCK;
   buf = (char *)malloc(sizeof(char) * allocated);
   len = 0;
   charset = QCS_DEFAULT;
   concatBase64(b);
}

inline QoreString::~QoreString()
{
   if (buf)
      free(buf);
}

// FIXME: (should) return the number of characters for all supported character sets
inline int QoreString::length()
{
   if (charset && charset->isMultiByte() && buf)
      return charset->getLength(buf);
   return len;
}

// returns number of bytes
inline int QoreString::strlen()
{
   return len;
}

inline char *QoreString::getBuffer()
{
   return buf;
}

inline void QoreString::concat(class DateTime *d)
{
   sprintf("%04d%02d%02d%02d%02d%02d", d->year, d->month, d->day, d->hour, d->minute, d->second);
}

inline void QoreString::concatISO8601DateTime(class DateTime *d)
{
   sprintf("%04d%02d%02dT%02d:%02d:%02d", d->year, d->month, d->day, d->hour, d->minute, d->second);
}

// NULL values sorted at end
inline int QoreString::compare(QoreString *str)
{
   // empty strings are always equal even if the character encoding is different
   if (!len)
      if (!str->len)
	 return 0;
      else
	 return 1;

   if (str->charset != charset)
      return 1;

   return strcmp(buf, str->buf);
}

inline int QoreString::compare(char *str)
{
   if (!buf)
      if (!str)
	 return 0;
      else
	 return 1;
   return strcmp(buf, str);
}

inline void QoreString::terminate(int size)
{
   if (size < len)
   {
      len = size;
      buf[size] = '\0';
   }
}

#define MIN_SPRINTF_BUFSIZE 120
inline void QoreString::take(char *str)
{
   if (buf)
      free(buf);
   buf = str;
   if (str)
   {
      len = ::strlen(str);
      allocated = len + 1;
   }
   else
   {
      allocated = 0;
      len = 0;
   }
}

inline void QoreString::take(char *str, class QoreEncoding *new_qorecharset)
{
   take(str);
   charset = new_qorecharset;
}

inline void QoreString::take(char *str, int size)
{
   if (buf)
      free(buf);
   buf = str;
   len = size;
   allocated = size + 1;
}

inline class QoreString *QoreString::copy()
{
   return new QoreString(this);
}

// NOTE: could be dangerous if we refer to the buffer after this
// call and it's NULL (the only way the buffer can become NULL)
inline char *QoreString::giveBuffer()
{
   char *rv = buf;
   buf = NULL;
   len = 0;
   allocated = 0;
   // reset character set, just in case the string will be reused
   // (normally not after this call)
   charset = QCS_DEFAULT;
   return rv;
}

inline void QoreString::clear()
{
   if (buf)
   {
      free(buf);
      buf = NULL;
   }
   allocated = 0;
   len = 0;
}

inline void QoreString::set(char *str, class QoreEncoding *new_qorecharset)
{
   len = 0;
   charset = new_qorecharset;
   if (!str)
   {
      if (buf)
	 buf[0] = '\0';
   }
   else
      concat(str);
}

inline void QoreString::concatBase64(class BinaryObject *b)
{
   concatBase64((char *)b->getPtr(), b->size());
}

inline void QoreString::concatBase64(class QoreString *str)
{
   concatBase64(str->buf, str->len);
}

inline void QoreString::replace(int offset, int dlen, char *str)
{
   int nl = str ? ::strlen(str) : 0;
   // ensure that enough memory is allocated if extending the string
   if (nl > dlen)
      check_char(len - dlen + nl + 1);

   if (nl != dlen)
      memmove(buf + offset + nl, buf + offset + dlen, len - offset - dlen + 1);

   if (str)
      strncpy(buf + offset, str, nl);
   len = len - dlen + nl;
}

inline void QoreString::replace(int offset, int dlen, class QoreString *str)
{
   // ensure that enough memory is allocated if extending the string
   if (str->len > dlen)
      check_char(len - dlen + str->len + 1);

   if (str->len != dlen)
      memmove(buf + offset + str->len, buf + offset + dlen, len - offset - dlen + 1);

   if (str->len)
      strncpy(buf + offset, str->buf, str->len);
   len = len - dlen + str->len;
}

// endian-agnostic base64 string -> binary object function
inline class BinaryObject *QoreString::parseBase64(class ExceptionSink *xsink)
{
   return ::parseBase64(buf, len, xsink);
}

inline void QoreString::check_offset(int &offset)
{
   if (offset < 0)
   {
      offset = len + offset;
      if (offset < 0)
         offset = 0;
   }
   else if (offset > len)
      offset = len;
}

inline void QoreString::check_offset(int &offset, int &num)
{
   check_offset(offset);
   if (num < 0)
   {
      num = len + num - offset;
      if (num < 0)
	 num = 0;
   }
}

void QoreString::splice(int offset, class ExceptionSink *xsink)
{
   if (!charset || !charset->isMultiByte())
   {
      check_offset(offset);
      if (offset == len)
	 return;

      splice_simple(offset, len - offset, xsink);
      return;
   }
   splice_complex(offset, xsink);
}

void QoreString::splice(int offset, int num, class ExceptionSink *xsink)
{
   if (!charset || !charset->isMultiByte())
   {
      check_offset(offset, num);
      if (offset == len || !num)
	 return;

      splice_simple(offset, num, xsink);
      return;
   }
   splice_complex(offset, num, xsink);
}

void QoreString::splice(int offset, int num, class QoreNode *strn, class ExceptionSink *xsink)
{
   if (!strn || strn->type != NT_STRING)
   {
      splice(offset, num, xsink);
      return;
   }

   if (!charset || !charset->isMultiByte())
   {
      check_offset(offset, num);
      if (offset == len)
      {
	 if (!strn->val.String->len)
	    return;
	 num = 0;
      }
      splice_simple(offset, num, strn->val.String, xsink);
      return;
   }
   splice_complex(offset, num, strn->val.String, xsink);
}

inline class QoreString *QoreString::substr(int offset)
{
   if (!charset || !charset->isMultiByte())
      return substr_simple(offset);

   return substr_complex(offset);
}

inline class QoreString *QoreString::substr(int offset, int length)
{
   if (!charset || !charset->isMultiByte())
      return substr_simple(offset, length);

   return substr_complex(offset, length);
}

#endif
