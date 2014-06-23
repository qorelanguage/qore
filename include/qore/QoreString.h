/* -*- mode: c++; indent-tabs-mode: nil -*- */  
/*
  QoreString.h

  QoreString Class Definition

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

#ifndef _QORE_QORESTRING_H

#define _QORE_QORESTRING_H

#include <stdarg.h>

#include <string>

class DateTime;
class BinaryNode;

//! Qore's string type supported by the QoreEncoding class
/** A QoreString is implemented by a char pointer, a byte length, and a QoreEncoding pointer.
    For the equivalent Qore parse tree/value type, see QoreStringNode
    @see QoreStringNode
 */
class QoreString {
protected:
   //! the private implementation of QoreString
   struct qore_string_private* priv;

   DLLLOCAL void splice_simple(qore_size_t offset, qore_size_t length, QoreString* extract = 0);
   DLLLOCAL void splice_simple(qore_size_t offset, qore_size_t length, const char* str, qore_size_t str_len, QoreString* extract = 0);
   DLLLOCAL void splice_complex(qore_offset_t offset, ExceptionSink* xsink, QoreString* extract = 0);
   DLLLOCAL void splice_complex(qore_offset_t offset, qore_offset_t length, ExceptionSink* xsink, QoreString* extract = 0);
   DLLLOCAL void splice_complex(qore_offset_t offset, qore_offset_t length, const QoreString* str, ExceptionSink* xsink, QoreString* extract = 0);
   DLLLOCAL int substr_simple(QoreString* str, qore_offset_t offset) const;
   DLLLOCAL int substr_simple(QoreString* str, qore_offset_t offset, qore_offset_t length) const;
   DLLLOCAL int substr_complex(QoreString* str, qore_offset_t offset, ExceptionSink* xsink) const;
   DLLLOCAL int substr_complex(QoreString* str, qore_offset_t offset, qore_offset_t length, ExceptionSink* xsink) const;

   // writes a new QoreString with the characters reversed of the "this" QoreString
   // assumes the encoding is the same and the length is 0
   DLLLOCAL void concat_reverse(QoreString* targ) const;

   DLLLOCAL int snprintf(size_t size, const char* fmt, ...);
   DLLLOCAL int vsnprintf(size_t size, const char* fmt, va_list args);
   DLLLOCAL static int convert_encoding_intern(const char* src, qore_size_t src_len, const QoreEncoding* from, QoreString& targ, const QoreEncoding* to, ExceptionSink* xsink);

public:
   //! creates an empty string and assigns the default encoding QCS_DEFAULT
   DLLEXPORT QoreString();

   //! creates a single-character string (either '0' or '1') and assigns the default encoding QCS_DEFAULT
   DLLEXPORT QoreString(bool b);

   //! copies the c-string passed and assigns the default encoding QCS_DEFAULT
   DLLEXPORT QoreString(const char* str);

   //! copies the c-string passed and assigns the encoding passed
   DLLEXPORT QoreString(const char* str, const QoreEncoding* new_qorecharset);

   //! creates an empty string and assigns the encoding passed
   DLLEXPORT QoreString(const QoreEncoding* new_qorecharset);

   //! copies the c-string passed and assigns the length and encoding passed
   DLLEXPORT QoreString(const char* str, qore_size_t len, const QoreEncoding* new_qorecharset = QCS_DEFAULT);

   //! copies the std::string passed and assigns the encoding passed
   DLLEXPORT QoreString(const std::string& str, const QoreEncoding* new_encoding = QCS_DEFAULT);

   //! creates a single-character string from the argument and assigns the default encoding QCS_DEFAULT
   DLLEXPORT QoreString(char c);

   //! creates a copy of the QoreString argument passed
   DLLEXPORT QoreString(const QoreString& str);

   //! creates a copy of the QoreString argument passed
   DLLEXPORT QoreString(const QoreString* str);

   //! creates a copy of the QoreString argument passed up to byte "len" and assigns "len" as the byte length of the new string
   DLLEXPORT QoreString(const QoreString* str, qore_size_t len);

   //! creates a new string with the string representation of the integer passed and assigns the default encoding QCS_DEFAULT
   DLLEXPORT QoreString(int64 i);

   //! creates a new string with the string representation of the floating-point value passed and assigns the default encoding QCS_DEFAULT
   DLLEXPORT QoreString(double f);

   //! creates a new string from the DateTime value passed in the format YYYYMMDDHHmmSS
   DLLEXPORT QoreString(const DateTime* date);

   //! creates a new string as the base64-encoded value of the binary object passed
   DLLEXPORT QoreString(const BinaryNode* bin);

   //! creates a new string as the base64-encoded value of the binary object passed and ensures the maximum line length for the base64-encoded output
   DLLEXPORT QoreString(const BinaryNode* bin, qore_size_t maxlinelen);

   //! takes ownership of the char* passed
   DLLEXPORT QoreString(char* nbuf, qore_size_t nlen, qore_size_t nallocated, const QoreEncoding* enc);

   //! frees any memory allocated by the string
   DLLEXPORT ~QoreString();

   //! returns the number of characters (not bytes) in the string
   /** an invalid character length may be returned if invalid character encodings are found in a multi-byte character encoding 
    */
   DLLEXPORT qore_size_t length() const;

   //! copies the c-string passed and sets the value of the string and its encoding
   DLLEXPORT void set(const char* str, const QoreEncoding* new_qorecharset = QCS_DEFAULT);

   //! copies the string passed and sets the value of the string and its encoding
   DLLEXPORT void set(const std::string& str, const QoreEncoding* new_qorecharset = QCS_DEFAULT);

   //! sets the value to the copy of the QoreString passed
   DLLEXPORT void set(const QoreString* str);

   //! sets the value to the copy of the QoreString passed
   DLLEXPORT void set(const QoreString& str);

   //! changes the tagged encoding to the given encoding; does not affect the actual string buffer, only changes the tagged encoding value
   DLLEXPORT void setEncoding(const QoreEncoding* new_encoding);

   //! concatenates HTML-encoded version of the c-string passed
   DLLEXPORT void concatAndHTMLEncode(const char* str);

   //! concatenates HTML-decoded version of the c-string passed
   DLLEXPORT void concatAndHTMLDecode(const QoreString* str);

   //! concatenates HTML-decoded version of the c-string passed with the given length
   DLLEXPORT void concatAndHTMLDecode(const char* str, size_t slen);

   //! concatenates HTML-decoded version of the c-string passed
   DLLEXPORT void concatAndHTMLDecode(const char* str);

   //! concatenates a URL-decoded version of the c-string passed
   /** @deprecated does not support RFC 3986, use concatEncodeUrl(ExceptionSink*, const QoreString&, bool) instead
    */
   DLLEXPORT void concatDecodeUrl(const char* url);

   //! concatenates a URL-decoded version of the c-string passed (RFC 3986 compliant: http://tools.ietf.org/html/rfc3986)
   DLLEXPORT int concatDecodeUrl(const QoreString& url, ExceptionSink* xsink);

   //! concatenates a URL-encoded version of the c-string passed
   /** @param xsink Qore-language exceptions (in this case character encoding conversion errors) are raised here
       @param url the url to encode and concatentate to the current string
       @param encode_all if true then all reserved characters are percent encoded (!*'();:@&=+$,/?#[]), otherwise only non-ascii characters, '%' and ' ' are percent-encoded
    */
   DLLEXPORT int concatEncodeUrl(ExceptionSink* xsink, const QoreString& url, bool encode_all = false);

   //! concatenates a string and escapes character c with esc_char (converts encodings if necessary)
   DLLEXPORT void concatEscape(const QoreString* str, char c, char esc_char, ExceptionSink* xsink);

   //! concatenates a string and escapes character c with esc_char
   DLLEXPORT void concatEscape(const char* str, char c, char esc_char = '\\');

   //! concatenation with character set conversion
   DLLEXPORT void concatAndHTMLEncode(const QoreString* , ExceptionSink* xsink);

   //! concatenates a string and converts encodings if necessary
   DLLEXPORT void concat(const QoreString* str, ExceptionSink* xsink);

   //! concatenates a string and converts encodings if necessary
   /** @param str the string to concatenate to the current string (this)
       @param pos the starting character position (not byte) for concatenation (negative values are from the end)
       @param xsink if an error occurs converting character encodings, it will be raised here

       @return 0 for OK, -1 for exception raised
    */
   DLLEXPORT int concat(const QoreString& str, qore_offset_t pos, ExceptionSink* xsink);

   //! concatenates a string and converts encodings if necessary
   /** @param str the string to concatenate to the current string (this)
       @param pos the starting character (not byte) position for concatenation (negative values are from the end)
       @param len the number of characters (not bytes) to concatenate from the starting position (negative values indicate all except that many characters from the end)
       @param xsink if an error occurs converting character encodings, it will be raised here

       @return 0 for OK, -1 for exception raised
    */
   DLLEXPORT int concat(const QoreString& str, qore_offset_t pos, qore_offset_t len, ExceptionSink* xsink);

   //! concatenates a QoreString up to character "len"
   /** An exception could be thrown if the string to concatenate requires character set encoding conversion and the conversion fails
       @param str the QoreString to concatenate
       @param size the number of characters to copy (not bytes)
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT void concat(const QoreString* str, qore_size_t size, ExceptionSink* xsink);

   //! concatenates the base64-encoded version of the binary data passed
   DLLEXPORT void concatBase64(const char* buf, qore_size_t size);

   //! concatenates the base64-encoded version of the binary data passed
   DLLEXPORT void concatBase64(const BinaryNode* bin);

   //! concatenates the base64-encoded version of the binary data passed (does not make any character encoding conversions)
   DLLEXPORT void concatBase64(const QoreString* str);

   //! concatenates the base64-encoded version of the binary data passed and ensures the maximum line length for the base64-encoded output
   DLLEXPORT void concatBase64(const char* buf, qore_size_t size, qore_size_t maxlinelen);

   //! concatenates the base64-encoded version of the binary data passed and ensures the maximum line length for the base64-encoded output
   DLLEXPORT void concatBase64(const BinaryNode* bin, qore_size_t maxlinelen);

   //! concatenates the base64-encoded version of the binary data passed (does not make any character encoding conversions) and ensures the maximum line length for the base64-encoded output
   DLLEXPORT void concatBase64(const QoreString* str, qore_size_t maxlinelen);

   //! parses the current string data as base64-encoded data and returns it as a BinaryNode pointer (caller owns the reference count), throws a qore-language exception if any errors are encountered
   /**
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @return a pointer to a BinaryNode object of the decoded data, the caller owns the reference count of the object returned (0 if an exception occurs)
   */
   DLLEXPORT BinaryNode* parseBase64(ExceptionSink* xsink) const;

   //! parses the current string data as base64-encoded data and returns it as a QoreString pointer owned by the caller
   /**
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @return a QoreString of the decoded data tagged with the default encoding (0 if an exception occurs), the QoreString pointer is owned by the caller
   */
   DLLEXPORT QoreString* parseBase64ToString(ExceptionSink* xsink) const;

   //! parses the current string data as base64-encoded data and returns it as a QoreString pointer owned by the caller
   /** 
       @param enc the encoding to tag the decoded string with
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return a QoreString of the decoded data (0 if an exception occurs), the QoreString pointer is owned by the caller
   */
   DLLEXPORT QoreString* parseBase64ToString(const QoreEncoding* enc, ExceptionSink* xsink) const;

   //! concatenates hexidecimal digits corresponding to the binary data passed up to byte "len"
   DLLEXPORT void concatHex(const char* buf, qore_size_t size);

   //! concatenates hexidecimal digits corresponding to the binary data passed
   DLLEXPORT void concatHex(const BinaryNode* bin);

   //! concatenates hexidecimal digits corresponding to the QoreString data passed interpreted as binary data
   DLLEXPORT void concatHex(const QoreString* str);

   //! parses the current string data as hexadecimal-encoded data and returns it as a BinaryNode pointer (caller owns the reference count), throws a qore-language exception if any errors are encountered
   /**
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @return a pointer to a BinaryNode object of the decoded data, the caller owns the reference count of the object returned (0 if an exception occurs)
   */
   DLLEXPORT BinaryNode* parseHex(ExceptionSink* xsink) const;

   //! concatenates a DateTime value to a string in the format YYYYMMDDHHmmSS
   DLLEXPORT void concat(const DateTime* d);

   //! concatenates a DateTime value to a string in the format YYYYMMDDTHH:mm:SS <- where the "T" is a literal "T"
   DLLEXPORT void concatISO8601DateTime(const DateTime* d);

   //! concatenates a c-string to the existing string
   DLLEXPORT void concat(const char* str);

   //! concatenates an stl string to the existing string
   DLLEXPORT void concat(const std::string& str);

   //! concatenates a c-string to the existing string, up to byte "size"
   DLLEXPORT void concat(const char* str, qore_size_t size);

   //! concatenates a single character to the string
   DLLEXPORT void concat(const char c);

   //! compares the string with another string, performing character set encoding conversion if necessary
   /**
      @param str the string to compare
      @param xsink if an error occurs, the Qore-language exception information will be added here	 
      @return -1, 0, or 1 if "this" is less than, equal to, or greater than "str" respectively
   */
   DLLEXPORT int compareSoft(const QoreString* str, ExceptionSink* xsink) const;

   //! compares two strings without converting encodings (if the encodings do not match then "this" is deemed automatically less than the argument)
   /**
      @param str the string to compare
      @return -1, 0, or 1 if "this" is less than, equal to, or greater than "str" respectively
   */
   DLLEXPORT int compare(const QoreString* str) const;

   //! compares the string with a c-string, which is assumed to be in the same encoding as the string
   /**
      @param str the string to compare
      @return -1, 0, or 1 if "this" is less than, equal to, or greater than "str" respectively
   */
   DLLEXPORT int compare(const char* str) const;

   //! returns true if the strings are equal, false if not, if the character encodings are different, then the strings are not equal
   /** @since Qore 0.8.8
    */
   DLLEXPORT bool equal(const QoreString& str) const;

   //! returns true if the strings are equal, false if not (encodings are assumed to be equal)
   /** @since Qore 0.8.8
    */
   DLLEXPORT bool equal(const char* str) const;

   //! returns true if the strings are equal, false if not, if the character encodings are different, then the encoding of the argument string is temporarily converted to the encoding of the current string to do the comparison
   /** @param str the string to compare
       @param xsink if an error occurs, the Qore-language exception information will be added here	 
       @return true if the strings are equal, false if not

       @since Qore 0.8.8
    */
   DLLEXPORT bool equalSoft(const QoreString& str, ExceptionSink* xsink) const;

   //! returns true if the beginning of the current string matches the argument string, false if not, if the character encodings are different, then the strings are not equal
   /** @since Qore 0.8.8
    */
   DLLEXPORT bool equalPartial(const QoreString& str) const;

   //! returns true if the beginning of the current string matches the argument string, false if not (encodings are assumed to be equal)
   /** @since Qore 0.8.8
    */
   DLLEXPORT bool equalPartial(const char* str) const;

   //! returns true if the beginning of the current string matches the argument string, false if not, if the character encodings are different, then the encoding of the argument string is temporarily converted to the encoding of the current string to do the comparison
   /** @param str the string to compare
       @param xsink if an error occurs, the Qore-language exception information will be added here	 
       @return true if the beginning of the current string matches the argument string, false if not

       @since Qore 0.8.8
    */
   DLLEXPORT bool equalPartialSoft(const QoreString& str, ExceptionSink* xsink) const;

   //! returns true if the begining of the current string matches the argument string where either both strings are the same size or the current string has a '/' or '?' character after the point where the argument string stops, false if not, if the character encodings are different, then the encoding of the argument string is temporarily converted to the encoding of the current string to do the comparison
   /** @param str the string to compare
       @param xsink if an error occurs, the Qore-language exception information will be added here	 
       @return true if the beginning of the current string matches the argument string where either both strings are the same size or the current string has a '/' or '?' character after the point where the argument string stops, false if not

       @since Qore 0.8.8
    */
   DLLEXPORT bool equalPartialPath(const QoreString& str, ExceptionSink* xsink) const;

   //! terminates the string at byte position "size", the string is reallocated if necessary
   DLLEXPORT void terminate(qore_size_t size);

   //! ensures that at least the given size is available in the string; the string's contents are not affected
   /** actually reserves size + 1 bytes to accommodate the terminating '\0' character
   */
   DLLEXPORT void reserve(qore_size_t size);

   //! this will concatentate a formatted string to the existing string according to the format string and the arguments
   /** NOTE that the formatted string is concatenated to the end of the current string!
    */
   DLLEXPORT int sprintf(const char* fmt, ...);

   //! this will concatentate a formatted string to the existing string according to the format string and the arguments
   /** NOTE that the formatted string is concatenated to the end of the current string!
    */
   DLLEXPORT int vsprintf(const char* fmt, va_list args);

   //! takes ownership of the character pointer passed and assigns it to the string (frees memory previously allocated)
   /** the encoding marker is not changed by this call, it remains the same as the previous encoding
    */
   DLLEXPORT void take(char* str);

   //! takes ownership of the character pointer passed and assigns it to the string (frees memory previously allocated), sets the encoding to the encoding passed
   DLLEXPORT void take(char* str, const QoreEncoding* enc);
   //! takes ownership of the character pointer passed and assigns it to the string (frees memory previously allocated) and sets the string byte lentgh to "size"

   DLLEXPORT void take(char* str, qore_size_t size);

   //! takes ownership of the character pointer passed and assigns it to the string (frees memory previously allocated), sets the string byte lentgh to "size", and sets the encoding to the encoding passed
   DLLEXPORT void take(char* str, qore_size_t size, const QoreEncoding* enc);

   //! takes ownership of the character pointer passed and assigns it to the string (frees memory previously allocated), sets the string byte length to "size", and sets the amount of member allocated to size + 1
   DLLEXPORT void takeAndTerminate(char* str, qore_size_t size);

   //! takes ownership of the character pointer passed and assigns it to the string (frees memory previously allocated), sets the string byte length to "size", and sets the amount of member allocated to size + 1, and sets the new encoding
   DLLEXPORT void takeAndTerminate(char* str, qore_size_t size, const QoreEncoding* enc);

   //! converts the encoding of the string to the specified encoding, returns 0 if an error occurs, the caller owns the pointer returned
   /** if the encoding is the same as the current encoding, a copy of the string is returned
       @param nccs the encoding for the new string
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return the new string with the desired encoding or 0 if an error occured
   */
   DLLEXPORT QoreString* convertEncoding(const QoreEncoding* nccs, ExceptionSink* xsink) const;

   //! returns the character buffer and leaves the QoreString empty, the caller owns the memory returned (must be manually freed)
   /** note that after this call the string buffer memory is 0 (i.e. QoreString::getBuffer() will return NULL)
      @return the character buffer for the string, the caller owns the memory returned (must be manually freed)
   */
   DLLEXPORT char* giveBuffer();

   //! reset string to zero length; memory is not deallocated; string encoding does not change
   DLLEXPORT void clear();

   //! reset string to zero length; memory is deallocated; string encoding is reset to QCS_DEFAULT
   /** a new string buffer is allocated with a '\0' in the first location
    */
   DLLEXPORT void reset();

   //! replaces all occurences of the first string with the second string
   /** both of the new strings must be in the same encoding as the QoreString
    */
   DLLEXPORT void replaceAll(const char* old_str, const char* new_str);

   //! replaces bytes with the string passed
   /** offsets and size are in bytes, not characters
    */
   DLLEXPORT void replace(qore_size_t offset, qore_size_t len, const char* str);

   //! replaces bytes with the string passed
   /** offsets and size are in bytes, not characters
       does nothing if the encodings are different
       DEPRECATED: do not use
       FIXME: this function should take an ExceptionSink argument and convert encodings
   */
   DLLEXPORT void replace(qore_size_t offset, qore_size_t len, const QoreString* str);

   //! replaces bytes with the string passed
   /** offsets and size are in bytes, not characters
    */
   DLLEXPORT void replace(qore_size_t offset, qore_size_t len, const QoreString* str, ExceptionSink* xsink);

   //! replaces a byte with the byte passed
   /** offsets and size are in bytes, not characters
    */
   DLLEXPORT void replaceChar(qore_size_t offset, char c);

   //! removes characters from the string starting at position "offset"
   /** values are for characters, not bytes
       @param offset character position to start (rest of the string is removed) (offset starts with 0, negative offset means that many positions from the end of the string)
       @param xsink is ignored
   */
   DLLEXPORT void splice(qore_offset_t offset, ExceptionSink* xsink);

   //! removes "length" characters from the string starting at position "offset"
   /** values are for characters, not bytes
       @param offset character position to start (rest of the string is removed) (offset starts with 0, negative offset means that many positions from the end of the string)
       @param length the number of characters (not bytes) to remove (negative length means all but that many characters from the end of the string)
       @param xsink invalid multi-byte encodings can cause an exception to be thrown
   */
   DLLEXPORT void splice(qore_offset_t offset, qore_offset_t length, ExceptionSink* xsink);

   //! removes "length" characters from the string starting at position "offset" and replaces them with the string passed
   /** values are for characters, not bytes
       @param offset character position to start (rest of the string is removed) (offset starts with 0, negative offset means that many positions from the end of the string)
       @param length the number of characters (not bytes) to remove (negative length means all but that many characters from the end of the string)
       @param strn the string to insert at character position "offset" after "length" characters are removed
       @param xsink invalid multi-byte encodings can cause an exception to be thrown
   */
   DLLEXPORT void splice(qore_offset_t offset, qore_offset_t length, const AbstractQoreNode* strn, ExceptionSink* xsink);

   //! removes "length" characters from the string starting at position "offset" and replaces them with the string passed
   /** values are for characters, not bytes
       @param offset character position to start (rest of the string is removed) (offset starts with 0, negative offset means that many positions from the end of the string)
       @param length the number of characters (not bytes) to remove (negative length means all but that many characters from the end of the string)
       @param str the string to insert at character position "offset" after "length" characters are removed
       @param xsink invalid multi-byte encodings can cause an exception to be thrown
   */
   DLLEXPORT void splice(qore_offset_t offset, qore_offset_t length, const QoreString& str, ExceptionSink* xsink);

   //! removes characters from the string starting at position "offset" and returns a string of the characters removed
   /** values are for characters, not bytes.  If no characters a removed, an empty string is returned
       @param offset character position to start (rest of the string is removed) (offset starts with 0, negative offset means that many positions from the end of the string)
       @param xsink is ignored
       @return a string of the characters removed; if no characters a removed, an empty string is returned
   */
   DLLEXPORT QoreString* extract(qore_offset_t offset, ExceptionSink* xsink);

   //! removes "length" characters from the string starting at position "offset" and returns a string of the characters removed
   /** values are for characters, not bytes.  If no characters a removed, an empty string is returned
       @param offset character position to start (rest of the string is removed) (offset starts with 0, negative offset means that many positions from the end of the string)
       @param length the number of characters (not bytes) to remove (negative length means all but that many characters from the end of the string)
       @param xsink invalid multi-byte encodings can cause an exception to be thrown
       @return a string of the characters removed; if no characters a removed, an empty string is returned, however if an exception is raised converting encodings, then 0 is returned
   */
   DLLEXPORT QoreString* extract(qore_offset_t offset, qore_offset_t length, ExceptionSink* xsink);

   //! removes "length" characters from the string starting at position "offset" and replaces them with the string passed, then returns a string of the characters removed
   /** values are for characters, not bytes.  If no characters a removed, an empty string is returned
       @param offset character position to start (rest of the string is removed) (offset starts with 0, negative offset means that many positions from the end of the string)
       @param length the number of characters (not bytes) to remove (negative length means all but that many characters from the end of the string)
       @param strn the string to insert at character position "offset" after "length" characters are removed
       @param xsink invalid multi-byte encodings can cause an exception to be thrown
       @return a string of the characters removed; if no characters a removed, an empty string is returned, however if an exception is raised converting encodings, then 0 is returned
   */
   DLLEXPORT QoreString* extract(qore_offset_t offset, qore_offset_t length, const AbstractQoreNode* strn, ExceptionSink* xsink);

   //! returns a new string consisting of all the characters from the current string starting with character position "offset"
   /** offset is a character offset (not a byte offset)
       @param offset the offset in characters from the beginning of the string (starting with 0)
       @param xsink invalid multi-byte encodings can cause an exception to be thrown
       @return the new string; an empty string is returned if the arguments cannot be satisfied; 0 is returned only if a Qore-language exception is thrown due to character encoding conversion errors
   */
   DLLEXPORT QoreString* substr(qore_offset_t offset, ExceptionSink* xsink) const;

   //! returns a new string consisting of "length" characters from the current string starting with character position "offset"
   /** offset and length spoecify characters, not bytes
       @param offset the offset in characters from the beginning of the string (starting with 0)
       @param length the number of characters for the substring
       @param xsink invalid multi-byte encodings can cause an exception to be thrown
       @return the new string; an empty string is returned if the arguments cannot be satisfied; 0 is returned only if a Qore-language exception is thrown due to character encoding conversion errors
   */
   DLLEXPORT QoreString* substr(qore_offset_t offset, qore_offset_t length, ExceptionSink* xsink) const;

   //! removes a single \\n\\r or \\n from the end of the string and returns the number of characters removed
   DLLEXPORT qore_size_t chomp();

   //! returns the encoding for the string
   DLLEXPORT const QoreEncoding* getEncoding() const;

   //! returns an exact copy of the string
   DLLEXPORT QoreString* copy() const;

   //! converts the string to lower-case in place
   /** WARNING: only works with ASCII characters!
    */
   DLLEXPORT void tolwr();

   //! converts the string to upper-case in place
   /** WARNING: only works with ASCII characters!
    */
   DLLEXPORT void toupr();

   //! returns number of bytes in the string (not including the null pointer)
   DLLEXPORT qore_size_t strlen() const;

   //! returns number of bytes in the string (not including the null pointer)
   DLLEXPORT qore_size_t size() const;

   //! returns number of bytes allocated for the string's buffer, capacity is always >= size
   DLLEXPORT qore_size_t capacity() const;

   //! returns the string's buffer; this data should not be changed
   DLLEXPORT const char* getBuffer() const;

   //! Ensure the internal buffer has at least expected size in bytes
   /** Useful to eliminate repeated buffer reallocations when data are appended in a loop
    */
   DLLEXPORT void allocate(unsigned requested_size);

   //! insert a character at a certain position in the string a number of times
   /** @param c the character to insert
       @param pos the position to insert the character(s) (first position is 0)
       @param times the number of times the character should be inserted

       @return 0 = OK, -1 = error (pos is greater than the string's length)
    */
   DLLEXPORT int insertch(char c, qore_size_t pos, unsigned times);

   //! inserts a character string at a certain position in the string
   /** @param str the string to insert
       @param pos the position to insert the string (first position is 0)

       @return 0 = OK, -1 = error (pos is greater than the string's length)
    */
   DLLEXPORT int insert(const char* str, qore_size_t pos);

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
   DLLEXPORT int concatUnicode(unsigned code, ExceptionSink* xsink);

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
   DLLEXPORT QoreString* reverse() const;

   //! remove trailing whitespace or other characters
   DLLEXPORT void trim_trailing(const char* chars = 0);

   //! remove leading whitespace or other characters
   DLLEXPORT void trim_leading(const char* chars = 0);

   //! remove leading and trailing whitespace or other characters
   DLLEXPORT void trim(const char* chars = 0);

   //! remove trailing characters if present
   DLLEXPORT void trim_trailing(char c);

   //! remove a single trailing character if present
   DLLEXPORT void trim_single_trailing(char c);

   //! remove leading characters if present
   DLLEXPORT void trim_leading(char c);

   //! remove a single leading character if present
   DLLEXPORT void trim_single_leading(char c);

   //! remove leading and trailing characters if present
   DLLEXPORT void trim(char c);

   //! return Unicode code point for character offset, string must be UTF-8
   /** if the string is not in UTF-8 encoding (tagged with QCS_UTF8), an unpredictable value will be returned
       @param offset the offset in characters (not bytes) in the string (negative offset means that many characters from the end of the string)
       @return the unicode code for the character
   */
   DLLEXPORT unsigned int getUnicodePointFromUTF8(qore_offset_t offset = 0) const;

   //! return Unicode code point for the single character at the given character (not byte) offset in the string
   /** if the string is not in UTF-8 encoding, only a single character is converted to make the unicode code point calculation

       @param offset the offset in characters (not bytes) in the string; may be negative giving an offset from the end of the string
       @param xsink if an error occurs, the Qore-language exception information will be added here

       @return the unicode code for the character
   */
   DLLEXPORT unsigned int getUnicodePoint(qore_offset_t offset, ExceptionSink* xsink) const;

   //! return Unicode code point for the given byte offset
   /** if the string is not in UTF-8 encoding, only a single character is converted to make the unicode code point calculation
       @param offset the offset in bytes in the string
       @param len the length of the character in bytes in the source string in the original encoding
       @return the unicode code for the character at the given byte offset

       @since Qore 0.8.8
   */
   DLLEXPORT unsigned int getUnicodePointFromBytePos(qore_size_t offset, unsigned& len, ExceptionSink* xsink) const;

   //! prepends the string given to the string, assumes character encoding is the same as the string's
   DLLEXPORT void prepend(const char* str);

   //! prepends the string given to the string, assumes character encoding is the same as the string's
   DLLEXPORT void prepend(const char* str, qore_size_t size);

   //! assigns the value of one string to another
   DLLEXPORT QoreString& operator=(const QoreString& other);

   //! assigns the value of one string to another; note that in this case the string is assigned the default encoding (QCS_DEFAULT)
   DLLEXPORT QoreString& operator=(const char* other);

   //! assigns the value of one string to another; note that in this case the string is assigned the default encoding (QCS_DEFAULT)
   DLLEXPORT QoreString& operator=(const std::string& other);

   //! returns true if the other string is equal to this string (encodings also must be equal)
   DLLEXPORT bool operator==(const QoreString& other) const;

   //! returns true if the other string is equal to this string (encodings also must be equal)
   DLLEXPORT bool operator==(const std::string& other) const;

   //! returns true if the other string is equal to this string (encodings also must be equal)
   DLLEXPORT bool operator==(const char* other) const;

   //! returns the byte (not character) at the given location; if the location is invalid, returns 0
   /** @param pos offset in string, negative offsets are form the end of the string
       @return the byte (not character) at the given location; if the location is invalid, returns 0

       FIXME: return value should be int and should return -1 if location is invalid
    */
   DLLEXPORT char operator[](qore_offset_t pos) const;

   //! concatenates the characters to the string; assumes the string to be concatenated is already in the character encoding of the "this" string
   DLLEXPORT QoreString& operator+=(const char* str);

   //! concatenates the characters to the string; assumes the string to be concatenated is already in the character encoding of the "this" string
   DLLEXPORT QoreString& operator+=(const std::string& str);

   //! returns true if the string is empty, false if not
   DLLEXPORT bool empty() const;

   //! returns the character position of a substring within the string or -1 if not found
   DLLEXPORT qore_offset_t index(const QoreString& needle, qore_offset_t pos, ExceptionSink* xsink) const;

   //! returns the byte position of a substring within the string or -1 if not found
   DLLEXPORT qore_offset_t bindex(const QoreString& needle, qore_offset_t pos) const;

   //! returns the byte position of a substring within the string or -1 if not found
   DLLEXPORT qore_offset_t bindex(const char* needle, qore_offset_t pos) const;

   //! returns the byte position of a substring within the string or -1 if not found
   DLLEXPORT qore_offset_t bindex(const std::string& needle, qore_offset_t pos) const;

   //! returns the character position of a substring searching in reverse from a given position or -1 if not found
   /** @param needle the string to find
       @param pos the character position to start the search; if pos is < 0 then it gives an offset from the end of the string (-1 = last character)
       @param xsink if an error occurs, the Qore-language exception information will be added here

       @return -1 for not found otherwise the character position of the last occurrence of the search string
    */
   DLLEXPORT qore_offset_t rindex(const QoreString& needle, qore_offset_t pos, ExceptionSink* xsink) const;

   //! returns the byte position of a substring within the string searching in reverse from a given position or -1 if not found
   DLLEXPORT qore_offset_t brindex(const QoreString& needle, qore_offset_t pos) const;

   //! returns the byte position of a substring within the string searching in reverse from a given position or -1 if not found
   DLLEXPORT qore_offset_t brindex(const char* needle, qore_offset_t pos) const;

   //! returns the byte position of a substring within the string searching in reverse from a given position or -1 if not found
   DLLEXPORT qore_offset_t brindex(const std::string& needle, qore_offset_t pos) const;

   //! returns the byte position of a character (byte) within the string or -1 if not found
   DLLEXPORT qore_offset_t find(char c, qore_offset_t pos = 0) const;

   //! returns the last byte position of a character (byte) within the string or -1 if not found
   DLLEXPORT qore_offset_t rfind(char c, qore_offset_t pos = -1) const;

   //! returns true if the string is empty or only contains printable non-control ASCII characters (ie all characters > 31 && < 127)
   /** @note the string's encoding is ignored and the data itself is scanned for the return value
   */
   DLLEXPORT bool isDataPrintableAscii() const;

   //! returns true if the string is empty or has no characters with the high bit set (ie all characters < 128)
   /** @note the string's encoding is ignored and the data itself is scanned for the return value
   */
   DLLEXPORT bool isDataAscii() const;

   //! returns the value of the string as an int64
   DLLEXPORT int64 toBigInt() const;

   //! returns the byte position of the given character position in the string or -1 if the string does not have that many characters (or if an invalid encoding exception is raised); may be different than the byte number for multi-byte character encodings such as UTF-8
   /** @param i the character offset to find the byte offset for
       @param xsink if invalid encoded data is found the error is raised here

       @return the byte position of the given character position in the string or -1 if the string does not have that many characters (or if an invalid encoding exception is raised); may be different than the byte number for multi-byte character encodings such as UTF-8
    */
   DLLEXPORT qore_offset_t getByteOffset(qore_size_t i, ExceptionSink* xsink) const;

   // concatenates a qorestring without converting encodings - internal only
   DLLLOCAL void concat(const QoreString* str);

   // private constructor
   DLLLOCAL QoreString(struct qore_string_private* p);
};

DLLEXPORT QoreString* checkEncoding(const QoreString* str, const QoreEncoding* enc, ExceptionSink* xsink);

class QoreStringMaker : public QoreString {
private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreStringMaker(const QoreStringMaker& str);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreStringMaker& operator=(const QoreStringMaker&);

public:
   //! creates a string with a sprintf-style constructor
   /** @since %Qore 0.8.7
    */
   DLLEXPORT QoreStringMaker(const char* fmt, ...);

   //! creates a string with an explicit encoding and a sprintf-style constructor
   /** @since %Qore 0.8.7
    */
   DLLEXPORT QoreStringMaker(const QoreEncoding* enc, const char* fmt, ...);
};

//! class used to hold a possibly temporary QoreString pointer, stack only, cannot be dynamically allocated
/**
   @code
   TempString rv(new QoreString());
   ...
   if (error)
   return 0; // here the memory is automatically released
   return rv.release();
   @endcode
*/
class TempString {
private:
   QoreString* str;

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   TempString(const TempString& );

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   TempString& operator=(const TempString& );

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   void* operator new(size_t);

public:
   //! populates the object with a new QoreString that this object will manage
   DLLLOCAL TempString() : str(new QoreString) {
   }

   //! populates the object with a new QoreString in a specific encoding that this object will manage
   DLLLOCAL TempString(const QoreEncoding* enc) : str(new QoreString(enc)) {
   }

   //! populates the object with the QoreString pointer to be managed
   DLLLOCAL TempString(QoreString* s) {
      str = s;
   }

   //! deletes the QoreString pointer being managed
   DLLLOCAL ~TempString() {
      delete str;
   }

   //! returns the QoreString pointer being managed
   DLLLOCAL QoreString* operator->(){ return str; };

   //! returns the QoreString pointer being managed
   DLLLOCAL QoreString* operator*() { return str; };

   //! returns true if a QoreString pointer is being managed
   DLLLOCAL operator bool() const { return str != 0; }

   //! releases the QoreString pointer being managed and sets the internal pointer to 0
   DLLLOCAL QoreString* release() { QoreString* rv = str; str = 0; return rv; }
};

//! use this class to manage strings where the character encoding must be specified and may be different than the actual encoding in the string
/** this class calls QoreString::convertEncoding() if necessary and manages any temporary string created by this call.
    the destructor will delete any temporary string if necessary.  Note that the constructor may add Qore-language exception information
    to the "xsink" parameter in case character set encoding conversion was necessary and failed
    @see QoreString
    @code
    // ensure a string is in UTF-8 encoding
    TempEncodingHelper utf8_str(str, QCS_UTF8, xsink);
    if (!str) // !str is only true if an exception has been thrown in the conversion
    return 0;
    printf("%s\n", utf8_str->getBuffer());
    @endcode
*/
class TempEncodingHelper {
private:
   QoreString* str;
   bool temp;

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL TempEncodingHelper(const TempEncodingHelper& );

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL TempEncodingHelper& operator=(const TempEncodingHelper& );

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void* operator new(size_t);

   //! discards the internal state
   DLLLOCAL void discard_intern() {
      if (temp && str)
         delete str;
   }

   //! sets the internal state
   /**
      @param s a pointer to the QoreString input value
      @param qe the QoreEncoding required
      @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLLOCAL void set_intern(const QoreString* s, const QoreEncoding* qe, ExceptionSink* xsink) {
      if (s->getEncoding() != qe) {
         str = s->convertEncoding(qe, xsink);
         temp = true;
      }
      else {
         str = const_cast<QoreString* >(s);
         temp = false;
      }
   }

public:
   //! converts the given string to the required encoding if necessary
   /**
      @param s a reference to the QoreString input value
      @param qe the QoreEncoding required
      @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLLOCAL TempEncodingHelper(const QoreString& s, const QoreEncoding* qe, ExceptionSink* xsink) {
      set_intern(&s, qe, xsink);
   }

   //! converts the given string to the required encoding if necessary
   /**
      @param s a pointer to the QoreString input value
      @param qe the QoreEncoding required
      @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLLOCAL TempEncodingHelper(const QoreString* s, const QoreEncoding* qe, ExceptionSink* xsink) {
      set_intern(s, qe, xsink);
   }

   //! creates an empty TempEncodingHelperObject that may be initialized with TempEncodingHelper::set() later
   DLLLOCAL TempEncodingHelper() : str(0), temp(false) {
   }

   //! deletes any temporary string being managed by the object
   DLLLOCAL ~TempEncodingHelper() {
      discard_intern();
   }

   //! discards any current state and sets and converts (if necessary) a new string to the desired encoding
   /** note that the return value is the opposite of most qore functions because 
       it was implemented incorrectly; the documentation has been changed to reflect
       the incorrect implementation; the implementation was not fixed in order to preserve
       source compatibility
       @param s a pointer to the QoreString input value
       @param qe the QoreEncoding required
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return 1=OK, 0=an error occurred and a Qore-language exception was raised
   */
   DLLLOCAL int set(const QoreString* s, const QoreEncoding* qe, ExceptionSink* xsink) {
      discard_intern();

      set_intern(s, qe, xsink);
      return str != 0;
   }

   //! returns true if a temporary string is being managed
   DLLLOCAL bool is_temp() const {
      return temp;
   }

   //! ensures that the object is holding a temporary value
   DLLLOCAL void makeTemp() {
      if (!temp && str) {
         str = new QoreString(*str);
         temp = true;
      }
   }

   //! returns the string being managed
   DLLLOCAL const QoreString* operator->(){ return str; };

   //! returns the string being managed
   DLLLOCAL const QoreString* operator*() { return str; };

   //! returns false if the object is empty (for example, if a Qore-language exception was thrown in the constructor), true if not
   /**
      @return false if the object is empty, true if not
   */
   DLLLOCAL operator bool() const { return str != 0; }

   //! returns a char pointer of the string, the caller owns the pointer returned (it must be manually freed)
   /**
      @return a char pointer of the string, the caller owns the pointer returned
   */
   DLLLOCAL char* giveBuffer() {
      if (!str)
         return 0;
      if (temp)
         return str->giveBuffer();
      return strdup(str->getBuffer());
   }
};

#endif

