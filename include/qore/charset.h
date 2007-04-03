/*
  charset.h

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
#include <qore/LockedObject.h>

#include <strings.h>
#include <string.h>

#include <map>

// for multi-byte character set encodings: gives the length of the string in characters
typedef int (*mbcs_length_t)(char *);
// for multi-byte character set encodings: gives the number of bytes for the number of chars
typedef int (*mbcs_end_t)(char *, int);
// for multi-byte character set encodings: gives the character position of the ptr
typedef int (*mbcs_pos_t)(char *, char *);

struct QoreEncoding {
      char *code;
      mbcs_length_t flength;
      mbcs_end_t fend;
      mbcs_pos_t fpos;
      char *desc;

      inline QoreEncoding(char *c, mbcs_length_t l, mbcs_end_t e, mbcs_pos_t p, char *d)
      {
	 code = c ? strdup(c) : NULL;
	 flength = l;
	 fend = e;
	 fpos = p;
	 desc = d ? strdup(d) : NULL;
      }
      inline ~QoreEncoding()
      {
	 free(code);
	 if (desc)
	    free(desc);
      }
      inline int getLength(char *p)
      {
	 return flength ? flength(p) : strlen(p);
      }
      inline int getByteLen(char *p, int c)
      {
	 return fend ? fend(p, c) : c;
      }
      inline int getCharPos(char *p, char *e)
      {
	 return fpos ? fpos(p, e) : e - p;
      }
      inline bool isMultiByte()
      {
	 return (bool)flength;
      }
};

typedef std::map<const char *, struct QoreEncoding *, class ltcstrcase> encoding_map_t;

// there will always only be one of these, therefore all members and methods are static
class QoreEncodingManager
{
   private:
      DLLLOCAL static encoding_map_t emap, amap;
      DLLLOCAL static class LockedObject mutex;
   
      DLLLOCAL static struct QoreEncoding *addUnlocked(char *code, mbcs_length_t l, mbcs_end_t e, mbcs_pos_t p, char *desc);
      DLLLOCAL static struct QoreEncoding *findUnlocked(char *name);

   public:
      DLLEXPORT static void addAlias(struct QoreEncoding *qcs, const char *alias);
      DLLEXPORT static struct QoreEncoding *findCreate(char *name);
      DLLEXPORT static struct QoreEncoding *findCreate(class QoreString *str);
      DLLEXPORT static void showEncodings();
      DLLEXPORT static void showAliases();
      DLLEXPORT static void init(char *def);

      DLLLOCAL QoreEncodingManager();
      DLLLOCAL ~QoreEncodingManager();
      DLLLOCAL static struct QoreEncoding *add(char *code, mbcs_length_t l, mbcs_end_t e, mbcs_pos_t p, char *desc);
};

DLLEXPORT extern QoreEncodingManager QEM;

// builtin character encodings
DLLEXPORT extern QoreEncoding *QCS_DEFAULT, *QCS_USASCII, *QCS_UTF8, *QCS_ISO_8859_1,
   *QCS_ISO_8859_2, *QCS_ISO_8859_3, *QCS_ISO_8859_4, *QCS_ISO_8859_5,
   *QCS_ISO_8859_6, *QCS_ISO_8859_7, *QCS_ISO_8859_8, *QCS_ISO_8859_9,
   *QCS_ISO_8859_10, *QCS_ISO_8859_11, *QCS_ISO_8859_13, *QCS_ISO_8859_14,
   *QCS_ISO_8859_15, *QCS_ISO_8859_16, *QCS_KOI8_R, *QCS_KOI8_U, *QCS_KOI7;

#endif // _QORE_CHARSET_H
