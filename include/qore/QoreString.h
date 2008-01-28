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

#include <stdarg.h>

#include <string>

struct code_table {
      char symbol;
      char *code;
      int len;
};

#define NUM_HTML_CODES 4
extern const class QoreEncoding *QCS_DEFAULT;
extern class code_table html_codes[];

class QoreString {
   protected:
      struct qore_string_private *priv;

      // the following function is not implemented
      DLLLOCAL QoreString & operator=(const QoreString &);

      DLLLOCAL inline void check_char(unsigned i);
      DLLLOCAL inline void check_offset(int &offset);
      DLLLOCAL inline void check_offset(int &offset, int &num);
      DLLLOCAL void splice_simple(int offset, int length, class ExceptionSink *xsink);
      DLLLOCAL void splice_simple(int offset, int length, const class QoreString *str, class ExceptionSink *xsink);
      DLLLOCAL void splice_complex(int offset, class ExceptionSink *xsink);
      DLLLOCAL void splice_complex(int offset, int length, class ExceptionSink *xsink);
      DLLLOCAL void splice_complex(int offset, int length, const class QoreString *str, class ExceptionSink *xsink);
      DLLLOCAL int substr_simple(QoreString *str, int offset) const;
      DLLLOCAL int substr_simple(QoreString *str, int offset, int length) const;
      DLLLOCAL int substr_complex(QoreString *str, int offset) const;
      DLLLOCAL int substr_complex(QoreString *str, int offset, int length) const;

      // writes a new QoreString with the characters reversed of the "this" QoreString
      // assumes the encoding is the same and the length is 0
      DLLLOCAL void concat_reverse(QoreString *targ) const;

      DLLLOCAL int snprintf(int size, const char *fmt, ...);
      DLLLOCAL int vsnprintf(int size, const char *fmt, va_list args);
      DLLLOCAL static int convert_encoding_intern(const char *src, int src_len, const class QoreEncoding *from, QoreString &targ, const class QoreEncoding *to, class ExceptionSink *xsink);

   public:
      // common HTML encodings table
      DLLEXPORT static const struct code_table html_codes[];

      DLLEXPORT QoreString();
      DLLEXPORT QoreString(bool b);
      DLLEXPORT QoreString(const char *);
      DLLEXPORT QoreString(const char *str, const class QoreEncoding *new_qorecharset);
      DLLEXPORT QoreString(const class QoreEncoding *new_qorecharset);
      DLLEXPORT QoreString(const char *str, int len, const class QoreEncoding *new_qorecharset = QCS_DEFAULT);
      DLLEXPORT QoreString(const std::string &str, const class QoreEncoding *new_encoding = QCS_DEFAULT);
      DLLEXPORT QoreString(char);
      DLLEXPORT QoreString(const QoreString &str);
      DLLEXPORT QoreString(const QoreString *str);
      DLLEXPORT QoreString(const QoreString *, int);
      DLLEXPORT QoreString(int64);
      DLLEXPORT QoreString(double);
      DLLEXPORT QoreString(const class DateTime *);
      DLLEXPORT QoreString(const class BinaryNode *);
      // takes ownership of the char * passed
      DLLEXPORT QoreString(char *nbuf, int nlen, int nallocated, const class QoreEncoding *enc);
      DLLEXPORT ~QoreString();

      // returns the number of characters
      DLLEXPORT int length() const;
      DLLEXPORT void set(const char *str, const class QoreEncoding *new_qorecharset = QCS_DEFAULT);
      DLLEXPORT void set(const QoreString *str);
      DLLEXPORT void set(const QoreString &str);
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
      DLLEXPORT void concatBase64(const class BinaryNode *bin);
      DLLEXPORT void concatBase64(const class QoreString *str);
      DLLEXPORT class BinaryNode *parseBase64(class ExceptionSink *xsink) const;
      DLLEXPORT class QoreString *parseBase64ToString(class ExceptionSink *xsink) const;
      DLLEXPORT void concatHex(const char *buf, int size);
      DLLEXPORT void concatHex(const class BinaryNode *bin);
      DLLEXPORT void concatHex(const class QoreString *str);
      DLLEXPORT class BinaryNode *parseHex(class ExceptionSink *xsink) const;
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
      // this will concatentate the new formatted string to the existing string
      DLLEXPORT int sprintf(const char *fmt, ...);
      // this will concatentate the new formatted string to the existing string
      DLLEXPORT int vsprintf(const char *fmt, va_list args);
      DLLEXPORT void take(char *);
      DLLEXPORT void take(char *, const class QoreEncoding *enc);
      DLLEXPORT void take(char *, int size);
      DLLEXPORT void take(char *, int size, const class QoreEncoding *enc);
      DLLEXPORT void takeAndTerminate(char *, int size);
      DLLEXPORT class QoreString *convertEncoding(const class QoreEncoding *nccs, class ExceptionSink *xsink) const;
      DLLEXPORT char *giveBuffer();
      // reset string to zero length. Memory is NOT deallocated.
      DLLEXPORT void clear();
      DLLEXPORT void replace(int offset, int len, const char *str);
      DLLEXPORT void replace(int offset, int len, const class QoreString *str);
      DLLEXPORT void splice(int offset, class ExceptionSink *xsink);
      DLLEXPORT void splice(int offset, int length, class ExceptionSink *xsink);
      DLLEXPORT void splice(int offset, int length, const class QoreNode *strn, class ExceptionSink *xsink);
      DLLEXPORT class QoreString *substr(int offset) const;
      DLLEXPORT class QoreString *substr(int offset, int length) const;
      DLLEXPORT int chomp();
      // returns the encoding for the string
      DLLEXPORT const class QoreEncoding *getEncoding() const;
      DLLEXPORT class QoreString *copy() const;
      DLLEXPORT void tolwr();
      DLLEXPORT void toupr();
      // returns number of bytes
      DLLEXPORT int strlen() const;
      // the string's buffer should normally not be changed
      DLLEXPORT const char *getBuffer() const;
      // Make sure the internal buffer has at least expected size in bytes. 
      // Useful to eliminate repeated reallocate() when data are appended in a loop.
      DLLEXPORT void ensureBufferSize(unsigned requested_size);
      // append a character to the string a number of times
      DLLEXPORT void addch(char c, unsigned times);
      // append a UTF-8 character sequence from a unicode code point
      DLLEXPORT void concatUTF8FromUnicode(unsigned code);
      // append a character sequence from a unicode code point (returns 0 for OK, -1 for exception)
      DLLEXPORT int concatUnicode(unsigned code, class ExceptionSink *xsink);
      // append a character sequence from a unicode code point (returns 0 for OK, -1 for error)
      DLLEXPORT int concatUnicode(unsigned code);
      // return a Qorestring with the characters reversed
      DLLEXPORT class QoreString *reverse() const;
      // remove trailing whitespace or other characters
      DLLEXPORT void trim_trailing(const char *chars = 0);
      // remove leading whitespace or other characters
      DLLEXPORT void trim_leading(const char *chars = 0);
      // remove leading and trailing whitespace or other characters
      DLLEXPORT void trim(const char *chars = 0);
      // remove trailing character
      DLLEXPORT void trim_trailing(char c);
      // remove leading characters
      DLLEXPORT void trim_leading(char c);
      // remove leading and trailing single char
      DLLEXPORT void trim(char c);
      // return Unicode code point for character offset, string must be UTF-8
      DLLEXPORT unsigned int getUnicodePointFromUTF8(int offset = 0) const;
      // return Unicode code point for character offset
      DLLEXPORT unsigned int getUnicodePoint(int offset, class ExceptionSink *xsink) const;

      // concatenates a qorestring without converting encodings - internal only
      DLLLOCAL void concat(const QoreString *);
      // private constructor
      DLLLOCAL QoreString(struct qore_string_private *p);
};

DLLEXPORT class QoreString *checkEncoding(const class QoreString *str, const class QoreEncoding *enc, class ExceptionSink *xsink);

class TempString {
   private:
      class QoreString *str;

      // not implemented
      TempString(const TempString &);
      TempString & operator=(const TempString &);
      void *operator new(size_t);

   public:
      DLLEXPORT TempString(class QoreString *s)
      {
	 str = s;
      }
      DLLEXPORT ~TempString()
      {
	 delete str;
      }
      DLLEXPORT QoreString *operator->(){ return str; };
      DLLEXPORT QoreString *operator*() { return str; };
      DLLEXPORT operator bool() const { return str != 0; }
      DLLEXPORT QoreString *release() { QoreString *rv = str; str = 0; return rv; }
};

// class for using strings possibly temporarily converted to another encoding
class TempEncodingHelper {
   private:
      class QoreString *str;
      bool temp;

      // not implemented
      TempEncodingHelper(const TempEncodingHelper &);
      TempEncodingHelper& operator=(const TempEncodingHelper &);
      void *operator new(size_t);

   public:
      DLLEXPORT TempEncodingHelper(class QoreString *s, const class QoreEncoding *qe, class ExceptionSink *xsink)
      {
	 if (s->getEncoding() != qe)
	 {
	    str = s->convertEncoding(qe, xsink);
	    temp = true;
	 }
	 else
	 {
	    str = s;
	    temp = false;
	 }
      }
      DLLEXPORT ~TempEncodingHelper()
      {
	 if (temp && str)
	    delete str;
      }
      DLLEXPORT bool is_temp() const
      {
	 return temp;
      }
      DLLEXPORT QoreString *operator->(){ return str; };
      DLLEXPORT QoreString *operator*() { return str; };
      // to check for an exception in the constructor
      DLLEXPORT operator bool() const { return str != 0; }
};

// class for using strings possibly temporarily converted to another encoding
class ConstTempEncodingHelper {
   private:
      const class QoreString *str;
      bool temp;

      // not implemented
      ConstTempEncodingHelper(const ConstTempEncodingHelper &);
      ConstTempEncodingHelper& operator=(const ConstTempEncodingHelper &);
      void *operator new(size_t);

   public:
      DLLEXPORT ConstTempEncodingHelper(const class QoreString *s, const class QoreEncoding *qe, class ExceptionSink *xsink)
      {
	 if (s->getEncoding() != qe)
	 {
	    str = s->convertEncoding(qe, xsink);
	    temp = true;
	 }
	 else
	 {
	    str = s;
	    temp = false;
	 }
      }
      DLLEXPORT ~ConstTempEncodingHelper()
      {
	 if (temp && str)
	    delete const_cast<QoreString *>(str);
      }
      DLLEXPORT bool is_temp() const
      {
	 return temp;
      }
      DLLEXPORT const QoreString *operator->(){ return str; };
      DLLEXPORT const QoreString *operator*() { return str; };
      // to check for an exception in the constructor
      DLLEXPORT operator bool() const { return str != 0; }
};

#endif

