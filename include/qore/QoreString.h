/*
  QoreString.h

  QoreString Class Definition

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

#ifndef _QORE_QORESTRING_H

#define _QORE_QORESTRING_H

#include <qore/common.h>

#include <stdarg.h>

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
      unsigned allocated;
      char *buf;
      class QoreEncoding *charset;

      DLLLOCAL inline void check_char(unsigned i);
      DLLLOCAL inline void check_offset(int &offset);
      DLLLOCAL inline void check_offset(int &offset, int &num);
      DLLLOCAL void splice_simple(int offset, int length, class ExceptionSink *xsink);
      DLLLOCAL void splice_simple(int offset, int length, const class QoreString *str, class ExceptionSink *xsink);
      DLLLOCAL void splice_complex(int offset, class ExceptionSink *xsink);
      DLLLOCAL void splice_complex(int offset, int length, class ExceptionSink *xsink);
      DLLLOCAL void splice_complex(int offset, int length, const class QoreString *str, class ExceptionSink *xsink);
      DLLLOCAL class QoreString *substr_simple(int offset);
      DLLLOCAL class QoreString *substr_simple(int offset, int length);
      DLLLOCAL class QoreString *substr_complex(int offset);
      DLLLOCAL class QoreString *substr_complex(int offset, int length);

      DLLLOCAL int snprintf(int size, const char *fmt, ...);
      DLLLOCAL int vsnprintf(int size, const char *fmt, va_list args);

   public:
      // common HTML encodings table
      DLLEXPORT static const struct code_table html_codes[];

      DLLEXPORT QoreString();
      DLLEXPORT QoreString(bool b);
      DLLEXPORT QoreString(const char *);
      DLLEXPORT QoreString(const char *str, class QoreEncoding *new_qorecharset);
      DLLEXPORT QoreString(class QoreEncoding *new_qorecharset);
      DLLEXPORT QoreString(const char *str, int len, class QoreEncoding *new_qorecharset = QCS_DEFAULT);
      DLLEXPORT QoreString(char);
      DLLEXPORT QoreString(const QoreString *str);
      DLLEXPORT QoreString(const QoreString *, int);
      DLLEXPORT QoreString(int64);
      DLLEXPORT QoreString(double);
      DLLEXPORT QoreString(const class DateTime *);
      DLLEXPORT QoreString(const class BinaryObject *);
      DLLEXPORT ~QoreString();

      // returns the number of characters
      DLLEXPORT int length() const;
      DLLEXPORT void set(const char *str, class QoreEncoding *new_qorecharset = QCS_DEFAULT);
      DLLEXPORT void concatAndHTMLEncode(const char *);
      DLLEXPORT void concatAndHTMLDecode(const QoreString *str);
      // concatenates a string and escapes character c with esc_char (converts encodings if necessary)
      DLLEXPORT void concatEscape(const QoreString *, char c, char esc_char, class ExceptionSink *xsink);
      // concatenates a string and escapes character c with esc_char
      DLLEXPORT void concatEscape(const char *, char c, char esc_char = '\\');
      // concatenation with character set conversion
      DLLEXPORT void concatAndHTMLEncode(const QoreString *, class ExceptionSink *xsink);
      // concatenates a string and converts encodings if necessary
      DLLEXPORT void concat(const QoreString *, class ExceptionSink *xsink);
      // in the following method, size refers to the number of characters, not bytes
      DLLEXPORT void concat(const QoreString *, int size, class ExceptionSink *xsink);
      DLLEXPORT void concatBase64(const char *buf, int size);
      DLLEXPORT void concatBase64(const class BinaryObject *bin);
      DLLEXPORT void concatBase64(const class QoreString *str);
      DLLEXPORT class BinaryObject *parseBase64(class ExceptionSink *xsink) const;
      DLLEXPORT void concatHex(const char *buf, int size);
      DLLEXPORT void concatHex(const class BinaryObject *bin);
      DLLEXPORT void concatHex(const class QoreString *str);
      DLLEXPORT class BinaryObject *parseHex(class ExceptionSink *xsink) const;
      DLLEXPORT void concat(const class DateTime *d);
      DLLEXPORT void concatISO8601DateTime(const class DateTime *d);
      DLLEXPORT void concat(const char *);
      DLLEXPORT void concat(const char *, int size);
      DLLEXPORT void concat(const char);
      DLLEXPORT int compareSoft(const QoreString *, class ExceptionSink *) const;
      DLLEXPORT int compare(const QoreString *) const;
      DLLEXPORT int compare(const char *) const;
      DLLEXPORT void allocate(int);
      DLLEXPORT void terminate(int);
      DLLEXPORT int sprintf(const char *fmt, ...);
      DLLEXPORT int vsprintf(const char *fmt, va_list args);
      DLLEXPORT void take(char *);
      DLLEXPORT void take(char *, const class QoreEncoding *new_charset);
      DLLEXPORT void take(char *, int size);
      DLLEXPORT class QoreString *convertEncoding(const class QoreEncoding *nccs, class ExceptionSink *xsink) const;
      DLLEXPORT char *giveBuffer();
      DLLEXPORT void clear();
      DLLEXPORT void replace(int offset, int len, const char *str);
      DLLEXPORT void replace(int offset, int len, const class QoreString *str);
      DLLEXPORT void splice(int offset, class ExceptionSink *xsink);
      DLLEXPORT void splice(int offset, int length, class ExceptionSink *xsink);
      DLLEXPORT void splice(int offset, int length, const class QoreNode *strn, class ExceptionSink *xsink);
      DLLEXPORT class QoreString *substr(int offset);
      DLLEXPORT class QoreString *substr(int offset, int length);
      DLLEXPORT int chomp();
      // returns the encoding for the string
      DLLEXPORT class QoreEncoding *getEncoding() const;
      DLLEXPORT class QoreString *copy() const;
      DLLEXPORT void tolwr();
      DLLEXPORT void toupr();
      // returns number of bytes
      DLLEXPORT int strlen() const;
      DLLEXPORT char *getBuffer() const;
      // Make sure the internal buffer has at least expected size in bytes. 
      // Useful to eliminate repeated reallocate() when data are appended in a loop.
      DLLEXPORT void ensureBufferSize(unsigned requested_size);
      // append a character to the string a number of times
      DLLEXPORT void addch(char c, unsigned times);

      // concatenates a qorestring without converting encodings - internal only
      DLLLOCAL void concat(const QoreString *);
};

DLLEXPORT class QoreString *checkEncoding(const class QoreString *str, const class QoreEncoding *enc, class ExceptionSink *xsink);

#endif

