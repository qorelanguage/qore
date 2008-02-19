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

extern const class QoreEncoding *QCS_DEFAULT;

//! Qore's string type supported by the QoreEncoding class
/** A QoreString is implemented by a char pointer, a length, and a QoreEncoding pointer.
    For the equivalent Qore parse tree/value type, see QoreStringNode
    @see QoreStringNode
 */
class QoreString {
   protected:
      //! the private implementation of QoreString
      struct qore_string_private *priv;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
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
      //! creates an empty string and assigns the default encoding QCS_DEFAULT
      DLLEXPORT QoreString();

      //! creates a single-character string (either '0' or '1') and assigns the default encoding QCS_DEFAULT
      DLLEXPORT QoreString(bool b);

      //! copies the c-string passed and assigns the default encoding QCS_DEFAULT
      DLLEXPORT QoreString(const char *str);

      //! copies the c-string passed and assigns the encoding passed
      DLLEXPORT QoreString(const char *str, const class QoreEncoding *new_qorecharset);

      //! creates an empty string and assigns the encoding passed
      DLLEXPORT QoreString(const class QoreEncoding *new_qorecharset);

      //! copies the c-string passed and assigns the length and encoding passed
      DLLEXPORT QoreString(const char *str, int len, const class QoreEncoding *new_qorecharset = QCS_DEFAULT);

      //! copies the std::string passed and assigns the encoding passed
      DLLEXPORT QoreString(const std::string &str, const class QoreEncoding *new_encoding = QCS_DEFAULT);

      //! creates a single-character string from the argument and assigns the default encoding QCS_DEFAULT
      DLLEXPORT QoreString(char c);

      //! creates a copy of the QoreString argument passed
      DLLEXPORT QoreString(const QoreString &str);

      //! creates a copy of the QoreString argument passed
      DLLEXPORT QoreString(const QoreString *str);

      //! creates a copy of the QoreString argument passed up to byte "len" and assigns "len" as the byte length of the new string
      DLLEXPORT QoreString(const QoreString *str, int len);

      //! creates a new string with the string representation of the integer passed and assigns the default encoding QCS_DEFAULT
      DLLEXPORT QoreString(int64 i);

      //! creates a new string with the string representation of the floating-point value passed and assigns the default encoding QCS_DEFAULT
      DLLEXPORT QoreString(double f);

      //! creates a new string from the DateTime value passed in the format YYYYMMDDHHmmSS
      DLLEXPORT QoreString(const class DateTime *date);

      //! creates a new string as the base64-encoded value of the binary object passed
      DLLEXPORT QoreString(const class BinaryNode *bin);

      //! takes ownership of the char * passed
      DLLEXPORT QoreString(char *nbuf, int nlen, int nallocated, const class QoreEncoding *enc);

      //! frees any memory allocated by the string
      DLLEXPORT ~QoreString();

      //! returns the number of characters (not bytes) in the string
      DLLEXPORT int length() const;

      //! copies the c-string passed and sets the value of the string and its encoding
      DLLEXPORT void set(const char *str, const class QoreEncoding *new_qorecharset = QCS_DEFAULT);

      //! sets the value to the copy of the QoreString passed
      DLLEXPORT void set(const QoreString *str);

      //! sets the value to the copy of the QoreString passed
      DLLEXPORT void set(const QoreString &str);

      //! concatenates HTML-encoded version of the c-string passed
      DLLEXPORT void concatAndHTMLEncode(const char *str);

      //! concatenates HTML-decoded version of the c-string passed
      DLLEXPORT void concatAndHTMLDecode(const QoreString *str);

      //! concatenates a string and escapes character c with esc_char (converts encodings if necessary)
      DLLEXPORT void concatEscape(const QoreString *, char c, char esc_char, class ExceptionSink *xsink);

      //! concatenates a string and escapes character c with esc_char
      DLLEXPORT void concatEscape(const char *, char c, char esc_char = '\\');

      //! concatenation with character set conversion
      DLLEXPORT void concatAndHTMLEncode(const QoreString *, class ExceptionSink *xsink);

      //! concatenates a string and converts encodings if necessary
      DLLEXPORT void concat(const QoreString *, class ExceptionSink *xsink);

      //! concatenates a QoreString up to character "len"
      /** An exception could be thrown if the string to concatenate requires character set encoding conversion and the conversion fails
	  @param str the QoreString to concatenate
	  @param len the number of characters to copy (not bytes)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void concat(const QoreString *str, int size, class ExceptionSink *xsink);

      //! concatenates the base64-encoded version of the binary data passed
      DLLEXPORT void concatBase64(const char *buf, int size);

      //! concatenates the base64-encoded version of the binary data passed
      DLLEXPORT void concatBase64(const class BinaryNode *bin);

      //! concatenates the base64-encoded version of the binary data passed
      DLLEXPORT void concatBase64(const class QoreString *str);

      //! parses the current string data as base64-encoded data and returns it as a BinaryNode pointer (caller owns the reference count), throws a qore-language exception if any errors are encountered
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return a pointer to a BinaryNode object of the decoded data, the caller owns the reference count of the object returned (0 if an exception occurs)
       */
      DLLEXPORT class BinaryNode *parseBase64(class ExceptionSink *xsink) const;

      //! parses the current string data as base64-encoded data and returns it as a QoreString pointer owned by the caller
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return a QoreString of the decoded data (0 if an exception occurs), the QoreString pointer is owned by the caller
       */
      DLLEXPORT QoreString *parseBase64ToString(class ExceptionSink *xsink) const;

      //! concatenates hexidecimal digits corresponding to the binary data passed up to byte "len"
      DLLEXPORT void concatHex(const char *buf, int size);

      //! concatenates hexidecimal digits corresponding to the binary data passed
      DLLEXPORT void concatHex(const class BinaryNode *bin);

      //! concatenates hexidecimal digits corresponding to the QoreString data passed interpreted as binary data
      DLLEXPORT void concatHex(const class QoreString *str);

      //! parses the current string data as hexadecimal-encoded data and returns it as a BinaryNode pointer (caller owns the reference count), throws a qore-language exception if any errors are encountered
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return a pointer to a BinaryNode object of the decoded data, the caller owns the reference count of the object returned (0 if an exception occurs)
       */
      DLLEXPORT class BinaryNode *parseHex(class ExceptionSink *xsink) const;

      //! concatenates a DateTime value to a string in the format YYYYMMDDHHmmSS
      DLLEXPORT void concat(const class DateTime *d);

      //! concatenates a DateTime value to a string in the format YYYYMMDDTHH:mm:SS <- where the "T" is a literal "T"
      DLLEXPORT void concatISO8601DateTime(const class DateTime *d);

      //! concatenates a c-string to the existing string
      DLLEXPORT void concat(const char *str);

      //! concatenates a c-string to the existing string, up to byte "size"
      DLLEXPORT void concat(const char *str, int size);

      //! concatenates a single character to the string
      DLLEXPORT void concat(const char c);

      //! compares the string with another string, performing character set encoding conversion if necessary
      /**
	 @param str the string to compare
	 @param xsink if an error occurs, the Qore-language exception information will be added here	 
	 @return -1, 0, or 1 if "this" is less than, equal to, or greater than "str" respectively
       */
      DLLEXPORT int compareSoft(const QoreString *str, class ExceptionSink *xsink) const;

      //! compares two strings without converting encodings (if the encodings do not match then "this" is deemed automatically less than the argument)
      /**
	 @param str the string to compare
	 @return -1, 0, or 1 if "this" is less than, equal to, or greater than "str" respectively
       */
      DLLEXPORT int compare(const QoreString *str) const;

      //! compares the string with a c-string, which is assumed to be in the same encoding as the string
      /**
	 @param str the string to compare
	 @return -1, 0, or 1 if "this" is less than, equal to, or greater than "str" respectively
       */
      DLLEXPORT int compare(const char *str) const;

      //! terminates the string at byte position "size", the string is reallocated if necessary
      DLLEXPORT void terminate(int size);

      //! this will concatentate a formatted string to the existing string according to the format string and the arguments
      DLLEXPORT int sprintf(const char *fmt, ...);

      //! this will concatentate a formatted string to the existing string according to the format string and the arguments
      DLLEXPORT int vsprintf(const char *fmt, va_list args);

      //! takes ownership of the character pointer passed and assigns it to the string (frees memory previously allocated)
      /** the encoding marker is not changed by this call, it remains the same as the previous encoding
       */
      DLLEXPORT void take(char *str);

      //! takes ownership of the character pointer passed and assigns it to the string (frees memory previously allocated), sets the encoding to the encoding passed
      DLLEXPORT void take(char *str, const class QoreEncoding *enc);
      //! takes ownership of the character pointer passed and assigns it to the string (frees memory previously allocated) and sets the string byte lentgh to "size"

      DLLEXPORT void take(char *str, int size);

      //! takes ownership of the character pointer passed and assigns it to the string (frees memory previously allocated), sets the string byte lentgh to "size", and sets the encoding to the encoding passed
      DLLEXPORT void take(char *str, int size, const class QoreEncoding *enc);

      //! takes ownership of the character pointer passed and assigns it to the string (frees memory previously allocated), sets the string byte length to "size", and sets the amount of member allocated to size + 1
      DLLEXPORT void takeAndTerminate(char *str, int size);

      //! converts the encoding of the string to the specified encoding, returns 0 if an error occurs, the caller owns the pointer returned
      /** if the encoding is the same as the current encoding, a copy of the string is returned
	  @param nccs the encoding for the new string
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the new string with the desired encoding or 0 if an error occured
       */
      DLLEXPORT QoreString *convertEncoding(const class QoreEncoding *nccs, class ExceptionSink *xsink) const;

      //! returns the character buffer and leaves the QoreString empty, the caller owns the memory returned (must be manually freed)
      /**
	 @return the character buffer for the string, the caller owns the memory returned (must be manually freed)
       */
      DLLEXPORT char *giveBuffer();

      //! reset string to zero length. Memory is not deallocated.
      DLLEXPORT void clear();

      DLLEXPORT void replace(int offset, int len, const char *str);
      DLLEXPORT void replace(int offset, int len, const class QoreString *str);
      DLLEXPORT void splice(int offset, class ExceptionSink *xsink);
      DLLEXPORT void splice(int offset, int length, class ExceptionSink *xsink);
      DLLEXPORT void splice(int offset, int length, const class AbstractQoreNode *strn, class ExceptionSink *xsink);
      DLLEXPORT class QoreString *substr(int offset) const;
      DLLEXPORT class QoreString *substr(int offset, int length) const;
      DLLEXPORT int chomp();

      //! returns the encoding for the string
      DLLEXPORT const class QoreEncoding *getEncoding() const;
      DLLEXPORT QoreString *copy() const;

      //! converts the string to lower-case in place
      /** WARNING: only works with ASCII characters!
       */
      DLLEXPORT void tolwr();

      //! converts the string to upper-case in place
      /** WARNING: only works with ASCII characters!
       */
      DLLEXPORT void toupr();

      //! returns number of bytes in the string
      DLLEXPORT int strlen() const;

      //! returns the string's buffer; this data should not be changed
      DLLEXPORT const char *getBuffer() const;

      // Make sure the internal buffer has at least expected size in bytes. 
      // Useful to eliminate repeated reallocate() when data are appended in a loop.
      DLLEXPORT void allocate(unsigned requested_size);

      //! append a character to the string a number of times
      DLLEXPORT void addch(char c, unsigned times);

      //! append a UTF-8 character sequence from a unicode code point, assumes the string is tagged with QCS_UTF8 encoding
      /** WARNING! Does not check the encoding before appending data to the string; if the string
	  was not created as a UTF-8 string, then this function will append invalid data to the
	  string.
	  @param code the Unicode code point to append to the string as UTF-8 data
	  @see QoreString::concatUnicode()
       */
      DLLEXPORT void concatUTF8FromUnicode(unsigned code);

      //! append a character sequence from a unicode code point (returns 0 for OK, -1 for exception)
      /** tries to convert the unicode data to the string's character encoding, if an error occurs an exception will be thrown
	  @param code the Unicode code point to append to the string
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT int concatUnicode(unsigned code, class ExceptionSink *xsink);

      //! append a character sequence from a unicode code point (returns 0 for OK, -1 for error)
      /**
	  @param code the Unicode code point to append to the string
	  @return returns 0 for OK, -1 for error (current encoding does not support this unicode character)
       */
      DLLEXPORT int concatUnicode(unsigned code);

      //! return a Qorestring with the characters reversed
      /**
	  @return a Qorestring with the characters reversed
       */
      DLLEXPORT class QoreString *reverse() const;

      //! remove trailing whitespace or other characters
      DLLEXPORT void trim_trailing(const char *chars = 0);

      //! remove leading whitespace or other characters
      DLLEXPORT void trim_leading(const char *chars = 0);

      //! remove leading and trailing whitespace or other characters
      DLLEXPORT void trim(const char *chars = 0);

      //! remove trailing character
      DLLEXPORT void trim_trailing(char c);

      //! remove leading characters
      DLLEXPORT void trim_leading(char c);

      //! remove leading and trailing single char
      DLLEXPORT void trim(char c);

      //! return Unicode code point for character offset, string must be UTF-8
      /** if the string is not in UTF-8 encoding (tagged with QCS_UTF8), an unpredictable value will be returned
	  @param offset the offset in characters (not bytes) in the string
	  @return the unicode code for the character
       */
      DLLEXPORT unsigned int getUnicodePointFromUTF8(int offset = 0) const;

      //! return Unicode code point for character offset
      /** if the string is not in UTF-8 encoding (tagged with QCS_UTF8), a temporary string will be created in UTF-8 encoding
	  @param offset the offset in characters (not bytes) in the string
	  @return the unicode code for the character
       */
      DLLEXPORT unsigned int getUnicodePoint(int offset, class ExceptionSink *xsink) const;

      // concatenates a qorestring without converting encodings - internal only
      DLLLOCAL void concat(const QoreString *);
      // private constructor
      DLLLOCAL QoreString(struct qore_string_private *p);
};

DLLEXPORT class QoreString *checkEncoding(const class QoreString *str, const class QoreEncoding *enc, class ExceptionSink *xsink);

//! class used to hold a possibly temporary QoreString pointer, stack only, cannot be dynamically allocated
class TempString {
   private:
      QoreString *str;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      TempString(const TempString &);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      TempString & operator=(const TempString &);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      void *operator new(size_t);

   public:
      //! populates the object with the QoreString pointer to be managed
      DLLLOCAL TempString(class QoreString *s)
      {
	 str = s;
      }

      //! deletes the QoreString pointer being managed
      DLLLOCAL ~TempString()
      {
	 delete str;
      }

      //! returns the QoreString pointer being managed
      DLLLOCAL QoreString *operator->(){ return str; };

      //! returns the QoreString pointer being managed
      DLLLOCAL QoreString *operator*() { return str; };

      //! returns true if a QoreString pointer is being managed
      DLLLOCAL operator bool() const { return str != 0; }

      //! releases the QoreString pointer being managed and sets the internal pointer to 0
      DLLLOCAL QoreString *release() { QoreString *rv = str; str = 0; return rv; }
};

//! use this class to manage strings where the character encoding must be specified and may be different than the actual encoding in the string
/** this class calls QoreString::convertEncoding() if necessary and manages any temporary string created by this call.
    the destructor will delete any temporary string if necessary.  Note that the constructor may add Qore-language exception information
    to the "xsink" parameter in case character set encoding conversion was necessary and failed
    @see QoreString
 */
class TempEncodingHelper {
   private:
      QoreString *str;
      bool temp;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      TempEncodingHelper(const TempEncodingHelper &);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      TempEncodingHelper& operator=(const TempEncodingHelper &);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      void *operator new(size_t);

   public:
      //! converts the given string to the required encoding if necessary
      /**
	 @param s a pointer to the QoreString input value
	 @param qe the QoreEncoding required
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL TempEncodingHelper(const QoreString *s, const QoreEncoding *qe, ExceptionSink *xsink)
      {
	 if (s->getEncoding() != qe)
	 {
	    str = s->convertEncoding(qe, xsink);
	    temp = true;
	 }
	 else
	 {
	    str = const_cast<QoreString *>(s);
	    temp = false;
	 }
      }

      //! deletes any temporary string being managed by the object
      DLLLOCAL ~TempEncodingHelper()
      {
	 if (temp && str)
	    delete str;
      }

      //! returns true if a temporary string is being managed
      DLLLOCAL bool is_temp() const
      {
	 return temp;
      }

      //! returns the string being managed
      DLLLOCAL const QoreString *operator->(){ return str; };

      //! returns the string being managed
      DLLLOCAL const QoreString *operator*() { return str; };

      //! returns false if the object is empty (for example, if a Qore-language exception was thrown in the constructor), true if not
      /**
	 @return false if the object is empty, true if not
       */
      DLLLOCAL operator bool() const { return str != 0; }

      //! returns a char pointer of the string, the caller owns the pointer returned (it must be manually freed)
      /**
	 @return a char pointer of the string, the caller owns the pointer returned
       */
      DLLLOCAL char *giveBuffer() 
      {
	 if (!str)
	    return 0;
	 if (temp)
	    return str->giveBuffer();
	 return strdup(str->getBuffer());
      }
};

#endif

