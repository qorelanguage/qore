/*
  QoreEncoding.h

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

#ifndef _QORE_CHARSET_H

#define _QORE_CHARSET_H

#include <qore/common.h>
#include <qore/QoreThreadLock.h>

#include <strings.h>
#include <string.h>

#include <map>

#include <string>

// for performance reasons this is not a class hierarchy with virtual methods
// this ugly implementation with function pointers is much faster

// for multi-byte character set encodings: gives the length of the string in characters
typedef int (*mbcs_length_t)(const char *);
// for multi-byte character set encodings: gives the number of bytes for the number of chars
typedef int (*mbcs_end_t)(const char *, int);
// for multi-byte character set encodings: gives the character position of the ptr
typedef int (*mbcs_pos_t)(const char *, const char *);

class QoreEncoding {
private:
      std::string code;
      mbcs_length_t flength;
      mbcs_end_t fend;
      mbcs_pos_t fpos;
      std::string desc;

public:
      DLLLOCAL QoreEncoding(const char *c, mbcs_length_t l, mbcs_end_t e, mbcs_pos_t p, const char *d)
      {
	 code = c;
	 flength = l;
	 fend = e;
	 fpos = p;
	 desc = d ? d : "";
      }
      DLLLOCAL ~QoreEncoding()
      {
      }

      DLLLOCAL int getLength(const char *p) const
      {
	 return flength ? flength(p) : strlen(p);
      }
      DLLLOCAL int getByteLen(const char *p, int c) const
      {
	 return fend ? fend(p, c) : c;
      }
      DLLLOCAL int getCharPos(const char *p, const char *e) const
      {
	 return fpos ? fpos(p, e) : e - p;
      }
      DLLEXPORT bool isMultiByte() const
      {
	 return (bool)flength;
      }
      DLLEXPORT const char *getCode() const
      {
	 return code.c_str();
      }
      DLLEXPORT const char *getDesc() const
      {
	 return desc.empty() ? "<no description available>" : desc.c_str();
      }
      
};

// case-insensitive maps for encodings
typedef std::map<const char *, QoreEncoding *, class ltcstrcase> encoding_map_t;
typedef std::map<const char *, const QoreEncoding *, class ltcstrcase> const_encoding_map_t;

// there will always only be one of these, therefore all members and methods are static
class QoreEncodingManager
{
   private:
      DLLLOCAL static encoding_map_t emap;
      DLLLOCAL static const_encoding_map_t amap;
      DLLLOCAL static class QoreThreadLock mutex;
   
      DLLLOCAL static const QoreEncoding *addUnlocked(const char *code, mbcs_length_t l, mbcs_end_t e, mbcs_pos_t p, const char *desc);
      DLLLOCAL static const QoreEncoding *findUnlocked(const char *name);

   public:
      DLLEXPORT static void addAlias(const QoreEncoding *qcs, const char *alias);
      DLLEXPORT static const QoreEncoding *findCreate(const char *name);
      DLLEXPORT static const QoreEncoding *findCreate(const QoreString *str);
      DLLEXPORT static void showEncodings();
      DLLEXPORT static void showAliases();
      DLLEXPORT static const QoreEncoding *add(const char *code, mbcs_length_t l, mbcs_end_t e, mbcs_pos_t p, const char *desc);

      DLLLOCAL static void init(const char *def);
      DLLLOCAL QoreEncodingManager();
      DLLLOCAL ~QoreEncodingManager();
};

DLLEXPORT extern QoreEncodingManager QEM;

// builtin character encodings
DLLEXPORT extern const QoreEncoding *QCS_DEFAULT, *QCS_USASCII, *QCS_UTF8, 
   *QCS_ISO_8859_1, *QCS_ISO_8859_2, *QCS_ISO_8859_3, *QCS_ISO_8859_4, 
   *QCS_ISO_8859_5, *QCS_ISO_8859_6, *QCS_ISO_8859_7, *QCS_ISO_8859_8, 
   *QCS_ISO_8859_9, *QCS_ISO_8859_10, *QCS_ISO_8859_11, *QCS_ISO_8859_13, 
   *QCS_ISO_8859_14, *QCS_ISO_8859_15, *QCS_ISO_8859_16, *QCS_KOI8_R, *QCS_KOI8_U, 
   *QCS_KOI7;

#endif // _QORE_CHARSET_H
