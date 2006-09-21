/*
  charset.cc

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/charset.h>
#include <qore/QoreString.h>
#include <qore/support.h>

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <iconv.h>

struct QoreEncoding *QCS_DEFAULT, *QCS_USASCII, *QCS_UTF8, *QCS_ISO_8859_1,
   *QCS_ISO_8859_2, *QCS_ISO_8859_3, *QCS_ISO_8859_4, *QCS_ISO_8859_5,
   *QCS_ISO_8859_6, *QCS_ISO_8859_7, *QCS_ISO_8859_8, *QCS_ISO_8859_9,
   *QCS_ISO_8859_10, *QCS_ISO_8859_11, *QCS_ISO_8859_13, *QCS_ISO_8859_14,
   *QCS_ISO_8859_15, *QCS_ISO_8859_16, *QCS_KOI8_R, *QCS_KOI8_U, *QCS_KOI7;

class QoreEncodingManager QEM;

static int utf8cl(char *p);
static int utf8end(char *p, int l);
static int utf8cpos(char *p, char *e);

QoreEncodingManager::QoreEncodingManager()
{
   head = tail = NULL;
   ahead = atail = NULL;
   
   QCS_USASCII     = addUnlocked("US-ASCII",    NULL,   NULL,    NULL,     "7-bit ASCII character set");
   QCS_UTF8        = addUnlocked("UTF-8",       utf8cl, utf8end, utf8cpos, "variable-width universal character set");
   QCS_ISO_8859_1  = addUnlocked("ISO-8859-1",  NULL,   NULL,    NULL,     "latin-1, Western European character set");
   QCS_ISO_8859_2  = addUnlocked("ISO-8859-2",  NULL,   NULL,    NULL,     "latin-2, Central European character set");
   QCS_ISO_8859_3  = addUnlocked("ISO-8859-3",  NULL,   NULL,    NULL,     "latin-3, Southern European character set");
   QCS_ISO_8859_4  = addUnlocked("ISO-8859-4",  NULL,   NULL,    NULL,     "latin-4, Northern European character set");
   QCS_ISO_8859_5  = addUnlocked("ISO-8859-5",  NULL,   NULL,    NULL,     "Cyrillic character set");
   QCS_ISO_8859_6  = addUnlocked("ISO-8859-6",  NULL,   NULL,    NULL,     "Arabic character set");
   QCS_ISO_8859_7  = addUnlocked("ISO-8859-7",  NULL,   NULL,    NULL,     "Greek character set");
   QCS_ISO_8859_8  = addUnlocked("ISO-8859-8",  NULL,   NULL,    NULL,     "Hebrew character set");
   QCS_ISO_8859_9  = addUnlocked("ISO-8859-9",  NULL,   NULL,    NULL,     "latin-5, Turkish character set");
   QCS_ISO_8859_10 = addUnlocked("ISO-8859-10", NULL,   NULL,    NULL,     "latin-6, Nordic character set");
   QCS_ISO_8859_11 = addUnlocked("ISO-8859-11", NULL,   NULL,    NULL,     "Thai character set");
   // there is no ISO-8859-12
   QCS_ISO_8859_13 = addUnlocked("ISO-8859-13", NULL,   NULL,    NULL,     "Latin-7, Baltic rim character set");
   QCS_ISO_8859_14 = addUnlocked("ISO-8859-14", NULL,   NULL,    NULL,     "Latin-8, Celtic character set");
   QCS_ISO_8859_15 = addUnlocked("ISO-8859-15", NULL,   NULL,    NULL,     "Latin-9, Western European with euro symbol");
   QCS_ISO_8859_16 = addUnlocked("ISO-8859-16", NULL,   NULL,    NULL,     "Latin-10, Southeast European character set");
   QCS_KOI8_R      = addUnlocked("KOI8-R",      NULL,   NULL,    NULL,     "Russian: Kod Obmena Informatsiey, 8 bit");
   QCS_KOI8_U      = addUnlocked("KOI8-U",      NULL,   NULL,    NULL,     "Ukrainian: Kod Obmena Informatsiey, 8 bit");
   QCS_KOI7        = addUnlocked("KOI7",        NULL,   NULL,    NULL,     "Russian: Kod Obmena Informatsiey, 7 bit characters");
   
   QCS_DEFAULT = QCS_UTF8;

   // setup aliases
   addAlias(QCS_USASCII, "ASCII");
   addAlias(QCS_USASCII, "USASCII");

   addAlias(QCS_UTF8, "UTF8");

   addAlias(QCS_ISO_8859_1, "ISO88591");
   addAlias(QCS_ISO_8859_1, "ISO8859-1");
   addAlias(QCS_ISO_8859_1, "ISO-88591");
   addAlias(QCS_ISO_8859_1, "ISO8859P1");

   addAlias(QCS_ISO_8859_2, "ISO88592");
   addAlias(QCS_ISO_8859_2, "ISO8859-2");
   addAlias(QCS_ISO_8859_2, "ISO-88592");
   addAlias(QCS_ISO_8859_2, "ISO8859P2");

   addAlias(QCS_ISO_8859_3, "ISO88593");
   addAlias(QCS_ISO_8859_3, "ISO8859-3");
   addAlias(QCS_ISO_8859_3, "ISO-88593");
   addAlias(QCS_ISO_8859_3, "ISO8859P3");

   addAlias(QCS_ISO_8859_4, "ISO88594");
   addAlias(QCS_ISO_8859_4, "ISO8859-4");
   addAlias(QCS_ISO_8859_4, "ISO-88594");
   addAlias(QCS_ISO_8859_4, "ISO8859P4");

   addAlias(QCS_ISO_8859_5, "ISO88595");
   addAlias(QCS_ISO_8859_5, "ISO8859-5");
   addAlias(QCS_ISO_8859_5, "ISO-88595");
   addAlias(QCS_ISO_8859_5, "ISO8859P5");

   addAlias(QCS_ISO_8859_6, "ISO88596");
   addAlias(QCS_ISO_8859_6, "ISO8859-6");
   addAlias(QCS_ISO_8859_6, "ISO-88596");
   addAlias(QCS_ISO_8859_6, "ISO8859P6");

   addAlias(QCS_ISO_8859_7, "ISO88597");
   addAlias(QCS_ISO_8859_7, "ISO8859-7");
   addAlias(QCS_ISO_8859_7, "ISO-88597");
   addAlias(QCS_ISO_8859_7, "ISO8859P7");

   addAlias(QCS_ISO_8859_8, "ISO88598");
   addAlias(QCS_ISO_8859_8, "ISO8859-8");
   addAlias(QCS_ISO_8859_8, "ISO-88598");
   addAlias(QCS_ISO_8859_8, "ISO8859P8");

   addAlias(QCS_ISO_8859_9, "ISO88599");
   addAlias(QCS_ISO_8859_9, "ISO8859-9");
   addAlias(QCS_ISO_8859_9, "ISO-88599");
   addAlias(QCS_ISO_8859_9, "ISO8859P9");

   addAlias(QCS_ISO_8859_10, "ISO885910");
   addAlias(QCS_ISO_8859_10, "ISO8859-10");
   addAlias(QCS_ISO_8859_10, "ISO-885910");
   addAlias(QCS_ISO_8859_10, "ISO8859P10");

   addAlias(QCS_ISO_8859_11, "ISO885911");
   addAlias(QCS_ISO_8859_11, "ISO8859-11");
   addAlias(QCS_ISO_8859_11, "ISO-885911");
   addAlias(QCS_ISO_8859_11, "ISO8859P11");

   // there is no ISO-8859-12

   addAlias(QCS_ISO_8859_13, "ISO885913");
   addAlias(QCS_ISO_8859_13, "ISO8859-13");
   addAlias(QCS_ISO_8859_13, "ISO-885913");
   addAlias(QCS_ISO_8859_13, "ISO8859P13");

   addAlias(QCS_ISO_8859_14, "ISO885914");
   addAlias(QCS_ISO_8859_14, "ISO8859-14");
   addAlias(QCS_ISO_8859_14, "ISO-885914");
   addAlias(QCS_ISO_8859_14, "ISO8859P14");

   addAlias(QCS_ISO_8859_15, "ISO885915");
   addAlias(QCS_ISO_8859_15, "ISO8859-15");
   addAlias(QCS_ISO_8859_15, "ISO-885915");
   addAlias(QCS_ISO_8859_15, "ISO8859P15");

   addAlias(QCS_ISO_8859_16, "ISO885916");
   addAlias(QCS_ISO_8859_16, "ISO8859-16");
   addAlias(QCS_ISO_8859_16, "ISO-885916");
   addAlias(QCS_ISO_8859_16, "ISO8859P16");

   addAlias(QCS_KOI8_R, "KOI8R");
   addAlias(QCS_KOI8_U, "KOI8U");
};

void QoreEncodingManager::showEncodings()
{
   struct QoreEncoding *w = head;
   while (w)
   {
      printf("%s: %s\n", w->code, w->desc ? w->desc : "(no description available)");
      w = w->next;
   }
}

void QoreEncodingManager::showAliases()
{
   struct QoreEncoding *last = NULL;
   struct QoreEncodingAlias *aw = ahead;
   while (aw)
   {
      if (last != aw->qcs)
      {
	 last = aw->qcs;
	 printf("encoding: %s\n", last->code);
      }
      printf(" + alias: %s\n", aw->alias);
      aw = aw->next;
   }
}


void QoreEncodingManager::init(char *def)
{
   // now set default character set
   if (def)
      QCS_DEFAULT = findCreate(def);
   else
   {
      // first see if QORE_CHARSET exists
      char *estr = getenv("QORE_CHARSET");
      if (estr)
	 QCS_DEFAULT = findCreate(estr);
      else // try to get character set name from LANG variable
      {
	 estr = getenv("LANG");
	 char *p;
	 if (estr && ((p = strrchr(estr, '.'))))
	 {
	    char *o = strchr(p + 1, '@');
	    if (!o)
	       QCS_DEFAULT = findCreate(p + 1);
	    else
	    {
	       *o = '\0';
	       QCS_DEFAULT = findCreate(p + 1);
	       *o = '@';
	    }
	 }
	 else // otherwise set QCS_DEFAULT to UTF-8
	    QCS_DEFAULT = QCS_UTF8;
      }
   }
}

static inline int utf8clen(char *p)
{
   // see if a multi-byte char is starting
   if ((*p & 0xc0) == 0xc0)
   {
      //printd(5, "MULTIBYTE *p = %hhx\n", *p);
      // check for a 3-byte sequence
      if ((*p) & 0x20)
      {
	 // check for a 4-byte sequence
	 if ((*p) & 0x10)
	 {
	    if ((*(p+1)) & 0x80 && (*(p+2)) & 0x80 && (*(p+3)) & 0x80)
	       return 4;
	 }
	 else // should be 3-byte sequence
	    if ((*(p+1)) & 0x80 && (*(p+2)) & 0x80)
	       return 3;
      }
      else // should be a 2-byte sequence - check next char for high bit
	 if ((*(p+1)) & 0x80)
	    return 2;
   }
   // if there was an error above, the error will be counted as one character
   return 1;
}

static int utf8cl(char *p)
{
   int i = 0;
   while (*p)
   {
      p += utf8clen(p);
      i++;
   }

   return i;
}

static int utf8end(char *p, int l)
{
   int b = 0;
   while (*p && l)
   {
      int bl = utf8clen(p);
      b += bl;
      p += bl;
      l--;
   }
   return b;
}

static int utf8cpos(char *p, char *end)
{
   int i = 0;
   while (p < end)
   {
      p += utf8clen(p);
      i++;
   }

   return i;
}
