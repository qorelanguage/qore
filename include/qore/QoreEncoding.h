/*
  QoreEncoding.h

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

#ifndef _QORE_CHARSET_H

#define _QORE_CHARSET_H

/** @file QoreEncoding.h
    provides definitions related to character encoding support in Qore including the QoreEncoding class
    and QCS_DEFAULT, the default encoding for the Qore library
 */

#include <qore/common.h>
#include <qore/QoreThreadLock.h>

#include <strings.h>
#include <string.h>

#include <map>

#include <string>

//! for multi-byte character set encodings: gives the length of the string in characters
typedef qore_size_t (*mbcs_length_t)(const char *str, const char *end, bool &invalid);

//! for multi-byte character set encodings: gives the number of bytes for the number of chars
typedef qore_size_t (*mbcs_end_t)(const char *str, const char *end, qore_size_t num_chars, bool &invalid);

//! for multi-byte character set encodings: gives the character position of the ptr
typedef qore_size_t (*mbcs_pos_t)(const char *str, const char *ptr, bool &invalid);

//! for multi-byte encodings: gives the number of total bytes for the character given one or more characters
/** @param str a pointer to the character data to check
    @param len the number of valid bytes at the start of the character pointer
    @return 0=invalid, positive = number of characters needed, negative numbers = number of additional bytes needed to perform the check
 */
typedef qore_size_t (*mbcs_charlen_t)(const char *str, qore_size_t valid_len);

class ExceptionSink;

//! defines string encoding functions in Qore
/** for performance reasons this is not a class hierarchy with virtual methods;
    this ugly implementation with function pointers is much faster.
    Only encodings where a single character can be more than 1 byte needs to
    have functions implemented.
    @note only encodings that are backwards compatible with ASCII are supported
    by Qore; currently the only multi-byte encoding supported by qore is UTF-8
    @note the default encoding is represented by QCS_DEFAULT; unless another 
    encoding is explicitly given, all strings will be tagged with QCS_DEFAULT
    @see QCS_DEFAULT
*/
class QoreEncoding {
private:
      std::string code;
      std::string desc;
      mbcs_length_t flength;
      mbcs_end_t fend;
      mbcs_pos_t fpos;
      mbcs_charlen_t fcharlen;
      unsigned char maxwidth;

public:
      DLLLOCAL QoreEncoding(const char *n_code, const char *n_desc = 0, unsigned char n_maxwidth = 1, mbcs_length_t l = 0, mbcs_end_t e = 0, mbcs_pos_t p = 0, mbcs_charlen_t c = 0) : code(n_code), desc(n_desc ? n_desc : ""), flength(l), fend(e), fpos(p), fcharlen(c), maxwidth(n_maxwidth) {
      }

      DLLLOCAL ~QoreEncoding() {
      }

      //! gives the length of the string in characters
      /** @param p a pointer to the character data
	  @param end a pointer to the next byte after the end of the character data
	  @param invalid if true after executing the function, invalid input was given and the return value should be ignored 
	  @return the number of characters in the string
       */
      DLLLOCAL qore_size_t getLength(const char *p, const char *end, bool &invalid) const {
	 return flength ? flength(p, end, invalid) : strlen(p);
      }

      //! gives the length of the string in characters
      /** @param p a pointer to the character data
	  @param end a pointer to the next byte after the end of the character data
	  @param xsink Qore-language exceptions will be raised using this argument
	  @return the number of characters in the string
       */
      DLLLOCAL qore_size_t getLength(const char *p, const char *end, ExceptionSink *xsink) const;

      //! gives the number of bytes for the number of chars in the string or up to the end of the string
      /** @param p a pointer to the character data
	  @param end a pointer to the next byte after the end of the character data
	  @param c the number of characters to check
	  @param invalid if true after executing the function, invalid input was given and the return value should be ignored 
	  @return the number of bytes for the given number of characters in the string or up to the end of the string
       */
      DLLLOCAL qore_size_t getByteLen(const char *p, const char *end, qore_size_t c, bool &invalid) const {
	 return fend ? fend(p, end, c, invalid) : c;
      }

      //! gives the number of bytes for the number of chars in the string or up to the end of the string
      /** @param p a pointer to the character data
	  @param end a pointer to the next byte after the end of the character data
	  @param c the number of characters to check
	  @param xsink Qore-language exceptions will be raised using this argument
	  @return the number of bytes for the given number of characters in the string or up to the end of the string
       */
      DLLLOCAL qore_size_t getByteLen(const char *p, const char *end, qore_size_t c, ExceptionSink *xsink) const;

      //! gives the character position (number of characters) starting from the first pointer to the second
      /** @param p a pointer to the character data
	  @param end a pointer to the next byte after the end of the character data
	  @param invalid if true after executing the function, invalid input was given and the return value should be ignored 
	  @return the number of bytes for the given number of characters in the string
       */
      DLLLOCAL qore_size_t getCharPos(const char *p, const char *end, bool &invalid) const {
	 return fpos ? fpos(p, end, invalid) : end - p;
      }

      //! gives the character position (number of characters) starting from the first pointer to the second
      /** @param p a pointer to the character data
	  @param end a pointer to the next byte after the end of the character data
	  @param xsink Qore-language exceptions will be raised using this argument
	  @return the number of bytes for the given number of characters in the string
       */
      DLLLOCAL qore_size_t getCharPos(const char *p, const char *end, ExceptionSink *xsink) const;

      //! gives the number of total bytes for the character given one or more characters
      /** always returns 1 for single-byte encodings
	  @param p a pointer to the character data to check
	  @param valid_len the number of valid bytes at the start of the character pointer
	  @return 0=invalid, positive = number of characters needed, negative numbers = number of additional bytes needed to perform the check
      */
      DLLLOCAL qore_size_t getCharLen(const char *p, qore_size_t valid_len) const {
	 return fcharlen ? fcharlen(p, valid_len) : 1;
      }
      
      //! returns true if the encoding is a multi-byte encoding
      DLLEXPORT bool isMultiByte() const {
	 return (bool)flength;
      }

      //! returns the string code (ex: "UTF-8") for the encoding
      DLLEXPORT const char *getCode() const {
	 return code.c_str();
      }

      //! returns the description for the encoding
      DLLEXPORT const char *getDesc() const {
	 return desc.empty() ? "<no description available>" : desc.c_str();
      }

      //! returns the maximum character width in bytes for the encoding
      DLLEXPORT int getMaxCharWidth() const {
	  return maxwidth;
      }
};

// case-insensitive maps for encodings
typedef std::map<const char *, QoreEncoding *, class ltcstrcase> encoding_map_t;
typedef std::map<const char *, const QoreEncoding *, class ltcstrcase> const_encoding_map_t;

class QoreString;

//! manages encodings in Qore
/** there will always only be one of these, therefore all members and methods are static
 */
class QoreEncodingManager
{
   private:
      DLLLOCAL static encoding_map_t emap;
      DLLLOCAL static const_encoding_map_t amap;
      DLLLOCAL static class QoreThreadLock mutex;
   
      DLLLOCAL static const QoreEncoding *addUnlocked(const char *code, const char *desc, unsigned char maxwidth = 1, mbcs_length_t l = 0, mbcs_end_t e = 0, mbcs_pos_t p = 0, mbcs_charlen_t = 0);
      DLLLOCAL static const QoreEncoding *findUnlocked(const char *name);

   public:
      //! adds an alias for an encoding
      DLLEXPORT static void addAlias(const QoreEncoding *qcs, const char *alias);

      //! finds an encoding if it exists (also looks up against alias names) and creates a new one if it doesn't
      DLLEXPORT static const QoreEncoding *findCreate(const char *name);

      //! finds an encoding if it exists (also looks up against alias names) and creates a new one if it doesn't
      DLLEXPORT static const QoreEncoding *findCreate(const QoreString *str);

      //! prints out all valid encodings to stdout
      DLLEXPORT static void showEncodings();

      //! prints out all aliases to stdout
      DLLEXPORT static void showAliases();

      //! adds a new encoding to the list
      DLLEXPORT static const QoreEncoding *add(const char *code, const char *desc = 0, unsigned char maxwidth = 1, mbcs_length_t l = 0, mbcs_end_t e = 0, mbcs_pos_t p = 0, mbcs_charlen_t = 0);

      DLLLOCAL static void init(const char *def);
      DLLLOCAL QoreEncodingManager();
      DLLLOCAL ~QoreEncodingManager();
};

//! the QoreEncodingManager object
DLLEXPORT extern QoreEncodingManager QEM;

// builtin character encodings
DLLEXPORT extern const QoreEncoding *QCS_DEFAULT, //!< the default encoding for the Qore library 
   *QCS_USASCII,                                  //!< ascii encoding
   *QCS_UTF8,                                     //!< UTF-8 multi-byte encoding (the only multi-byte encoding, all others are single-byte encodings)
   *QCS_ISO_8859_1,                               //!< latin-1, Western European encoding
   *QCS_ISO_8859_2,                               //!< latin-2, Central European encoding
   *QCS_ISO_8859_3,                               //!< latin-3, Southern European character set
   *QCS_ISO_8859_4,                               //!< latin-4, Northern European character set
   *QCS_ISO_8859_5,                               //!< Cyrillic character set
   *QCS_ISO_8859_6,                               //!< Arabic character set
   *QCS_ISO_8859_7,                               //!< Greek character set
   *QCS_ISO_8859_8,                               //!< Hebrew character set
   *QCS_ISO_8859_9,                               //!< latin-5, Turkish character set
   *QCS_ISO_8859_10,                              //!< latin-6, Nordic character set
   *QCS_ISO_8859_11,                              //!< Thai character set
   *QCS_ISO_8859_13,                              //!< latin-7, Baltic rim character set
   *QCS_ISO_8859_14,                              //!< latin-8, Celtic character set
   *QCS_ISO_8859_15,                              //!< latin-9, Western European with euro symbol
   *QCS_ISO_8859_16,                              //!< latin-10, Southeast European character set
   *QCS_KOI8_R,                                   //!< Russian: Kod Obmena Informatsiey, 8 bit
   *QCS_KOI8_U,                                   //!< Ukrainian: Kod Obmena Informatsiey, 8 bit
   *QCS_KOI7;                                     //!< Russian: Kod Obmena Informatsiey, 7 bit characters

#endif // _QORE_CHARSET_H
