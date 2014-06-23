/*
  charset.cpp

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

#include <qore/Qore.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <iconv.h>

#include <map>

const QoreEncoding *QCS_DEFAULT, *QCS_USASCII, *QCS_UTF8, *QCS_ISO_8859_1,
   *QCS_ISO_8859_2, *QCS_ISO_8859_3, *QCS_ISO_8859_4, *QCS_ISO_8859_5,
   *QCS_ISO_8859_6, *QCS_ISO_8859_7, *QCS_ISO_8859_8, *QCS_ISO_8859_9,
   *QCS_ISO_8859_10, *QCS_ISO_8859_11, *QCS_ISO_8859_13, *QCS_ISO_8859_14,
   *QCS_ISO_8859_15, *QCS_ISO_8859_16, *QCS_KOI8_R, *QCS_KOI8_U, *QCS_KOI7;

static qore_size_t UTF8_getLength(const char *p, const char *end, bool &invalid);
static qore_size_t UTF8_getByteLen(const char *p, const char *end, qore_size_t l, bool &invalid);
static qore_size_t UTF8_getCharPos(const char *p, const char *e, bool &invalid);

encoding_map_t QoreEncodingManager::emap;
const_encoding_map_t QoreEncodingManager::amap;
class QoreThreadLock QoreEncodingManager::mutex;
class QoreEncodingManager QEM;

const QoreEncoding *QoreEncodingManager::addUnlocked(const char *code, const char *desc, unsigned char maxwidth, mbcs_length_t l, mbcs_end_t e, mbcs_pos_t p, mbcs_charlen_t c) {
   QoreEncoding *qcs = new QoreEncoding(code, desc, maxwidth, l, e, p, c);
   emap[qcs->getCode()] = qcs;
   return qcs;
}

const QoreEncoding *QoreEncodingManager::add(const char *code, const char *desc, unsigned char maxwidth, mbcs_length_t l, mbcs_end_t e, mbcs_pos_t p, mbcs_charlen_t c) {
   QoreEncoding *qcs = new QoreEncoding(code, desc, maxwidth, l, e, p, c);
   mutex.lock();
   emap[qcs->getCode()] = qcs;
   mutex.unlock();
   return qcs;
}

// have to handle HP-UX's non-standard names for character sets separately
#ifdef HPUX
#define ISO88591_STR "ISO8859-1"
#define ISO88592_STR "ISO8859-2"
#define ISO88593_STR "ISO8859-3"
#define ISO88594_STR "ISO8859-4"
#define ISO88595_STR "ISO8859-5"
#define ISO88596_STR "ISO8859-6"
#define ISO88597_STR "ISO8859-7"
#define ISO88598_STR "ISO8859-8"
#define ISO88599_STR "ISO8859-9"
#define ISO885910_STR "ISO8859-10"
#define ISO885911_STR "ISO8859-11"
#define ISO885913_STR "ISO8859-13"
#define ISO885914_STR "ISO8859-14"
#define ISO885915_STR "ISO8859-15"
#define ISO885916_STR "ISO8859-16"
#else
#define ISO88591_STR "ISO-8859-1"
#define ISO88592_STR "ISO-8859-2"
#define ISO88593_STR "ISO-8859-3"
#define ISO88594_STR "ISO-8859-4"
#define ISO88595_STR "ISO-8859-5"
#define ISO88596_STR "ISO-8859-6"
#define ISO88597_STR "ISO-8859-7"
#define ISO88598_STR "ISO-8859-8"
#define ISO88599_STR "ISO-8859-9"
#define ISO885910_STR "ISO-8859-10"
#define ISO885911_STR "ISO-8859-11"
#define ISO885913_STR "ISO-8859-13"
#define ISO885914_STR "ISO-8859-14"
#define ISO885915_STR "ISO-8859-15"
#define ISO885916_STR "ISO-8859-16"
#endif

QoreEncodingManager::QoreEncodingManager() {
   // add character sets and setup aliases
   
   QCS_USASCII     = addUnlocked("US-ASCII",    "7-bit ASCII character set");
   addAlias(QCS_USASCII, "ASCII");
   addAlias(QCS_USASCII, "USASCII");
   addAlias(QCS_USASCII, "US-ASCII");

   QCS_UTF8        = addUnlocked("UTF-8",       "variable-width universal character set", 4, UTF8_getLength, UTF8_getByteLen, UTF8_getCharPos, q_UTF8_get_char_len);
   addAlias(QCS_UTF8, "UTF8");

   QCS_ISO_8859_1  = addUnlocked(ISO88591_STR,  "latin-1, Western European character set");
   addAlias(QCS_ISO_8859_1, "ISO88591");
   addAlias(QCS_ISO_8859_1, "ISO-8859-1");
   addAlias(QCS_ISO_8859_1, "ISO8859-1");
   addAlias(QCS_ISO_8859_1, "ISO-88591");
   addAlias(QCS_ISO_8859_1, "ISO8859P1");
   addAlias(QCS_ISO_8859_1, "ISO81");
   addAlias(QCS_ISO_8859_1, "LATIN1");
   addAlias(QCS_ISO_8859_1, "LATIN-1");

   QCS_ISO_8859_2  = addUnlocked(ISO88592_STR,  "latin-2, Central European character set");
   addAlias(QCS_ISO_8859_2, "ISO88592");
   addAlias(QCS_ISO_8859_2, "ISO-8859-2");
   addAlias(QCS_ISO_8859_2, "ISO8859-2");
   addAlias(QCS_ISO_8859_2, "ISO-88592");
   addAlias(QCS_ISO_8859_2, "ISO8859P2");
   addAlias(QCS_ISO_8859_2, "ISO82");
   addAlias(QCS_ISO_8859_2, "LATIN2");
   addAlias(QCS_ISO_8859_2, "LATIN-2");

   QCS_ISO_8859_3  = addUnlocked(ISO88593_STR,  "latin-3, Southern European character set");
   addAlias(QCS_ISO_8859_3, "ISO88593");
   addAlias(QCS_ISO_8859_3, "ISO-8859-3");
   addAlias(QCS_ISO_8859_3, "ISO8859-3");
   addAlias(QCS_ISO_8859_3, "ISO-88593");
   addAlias(QCS_ISO_8859_3, "ISO8859P3");
   addAlias(QCS_ISO_8859_3, "ISO83");
   addAlias(QCS_ISO_8859_3, "LATIN3");
   addAlias(QCS_ISO_8859_3, "LATIN-3");

   QCS_ISO_8859_4  = addUnlocked(ISO88594_STR,  "latin-4, Northern European character set");
   addAlias(QCS_ISO_8859_4, "ISO88594");
   addAlias(QCS_ISO_8859_4, "ISO-8859-4");
   addAlias(QCS_ISO_8859_4, "ISO8859-4");
   addAlias(QCS_ISO_8859_4, "ISO-88594");
   addAlias(QCS_ISO_8859_4, "ISO8859P4");
   addAlias(QCS_ISO_8859_4, "ISO84");
   addAlias(QCS_ISO_8859_4, "LATIN4");
   addAlias(QCS_ISO_8859_4, "LATIN-4");

   QCS_ISO_8859_5  = addUnlocked(ISO88595_STR,  "Cyrillic character set");
   addAlias(QCS_ISO_8859_5, "ISO88595");
   addAlias(QCS_ISO_8859_5, "ISO-8859-5");
   addAlias(QCS_ISO_8859_5, "ISO8859-5");
   addAlias(QCS_ISO_8859_5, "ISO-88595");
   addAlias(QCS_ISO_8859_5, "ISO8859P5");
   addAlias(QCS_ISO_8859_5, "ISO85");

   QCS_ISO_8859_6  = addUnlocked(ISO88596_STR,  "Arabic character set");
   addAlias(QCS_ISO_8859_6, "ISO88596");
   addAlias(QCS_ISO_8859_6, "ISO-8859-6");
   addAlias(QCS_ISO_8859_6, "ISO8859-6");
   addAlias(QCS_ISO_8859_6, "ISO-88596");
   addAlias(QCS_ISO_8859_6, "ISO8859P6");
   addAlias(QCS_ISO_8859_6, "ISO86");

   QCS_ISO_8859_7  = addUnlocked(ISO88597_STR,  "Greek character set");
   addAlias(QCS_ISO_8859_7, "ISO88597");
   addAlias(QCS_ISO_8859_7, "ISO-8859-7");
   addAlias(QCS_ISO_8859_7, "ISO8859-7");
   addAlias(QCS_ISO_8859_7, "ISO-88597");
   addAlias(QCS_ISO_8859_7, "ISO8859P7");
   addAlias(QCS_ISO_8859_7, "ISO87");

   QCS_ISO_8859_8  = addUnlocked(ISO88598_STR,  "Hebrew character set");
   addAlias(QCS_ISO_8859_8, "ISO88598");
   addAlias(QCS_ISO_8859_8, "ISO-8859-8");
   addAlias(QCS_ISO_8859_8, "ISO8859-8");
   addAlias(QCS_ISO_8859_8, "ISO-88598");
   addAlias(QCS_ISO_8859_8, "ISO8859P8");
   addAlias(QCS_ISO_8859_8, "ISO88");

   QCS_ISO_8859_9  = addUnlocked(ISO88599_STR,  "latin-5, Turkish character set");
   addAlias(QCS_ISO_8859_9, "ISO88599");
   addAlias(QCS_ISO_8859_9, "ISO-8859-9");
   addAlias(QCS_ISO_8859_9, "ISO8859-9");
   addAlias(QCS_ISO_8859_9, "ISO-88599");
   addAlias(QCS_ISO_8859_9, "ISO8859P9");
   addAlias(QCS_ISO_8859_9, "ISO89");
   addAlias(QCS_ISO_8859_9, "LATIN5");
   addAlias(QCS_ISO_8859_9, "LATIN-5");

   QCS_ISO_8859_10 = addUnlocked(ISO885910_STR, "latin-6, Nordic character set");
   addAlias(QCS_ISO_8859_10, "ISO885910");
   addAlias(QCS_ISO_8859_10, "ISO-8859-10");
   addAlias(QCS_ISO_8859_10, "ISO8859-10");
   addAlias(QCS_ISO_8859_10, "ISO-885910");
   addAlias(QCS_ISO_8859_10, "ISO8859P10");
   addAlias(QCS_ISO_8859_10, "ISO810");
   addAlias(QCS_ISO_8859_10, "LATIN6");
   addAlias(QCS_ISO_8859_10, "LATIN-6");

   QCS_ISO_8859_11 = addUnlocked(ISO885911_STR, "Thai character set");
   addAlias(QCS_ISO_8859_11, "ISO885911");
   addAlias(QCS_ISO_8859_11, "ISO-8859-11");
   addAlias(QCS_ISO_8859_11, "ISO8859-11");
   addAlias(QCS_ISO_8859_11, "ISO-885911");
   addAlias(QCS_ISO_8859_11, "ISO8859P11");
   addAlias(QCS_ISO_8859_11, "ISO811");

   // there is no ISO-8859-12
   QCS_ISO_8859_13 = addUnlocked(ISO885913_STR, "latin-7, Baltic rim character set");
   addAlias(QCS_ISO_8859_13, "ISO885913");
   addAlias(QCS_ISO_8859_13, "ISO-8859-13");
   addAlias(QCS_ISO_8859_13, "ISO8859-13");
   addAlias(QCS_ISO_8859_13, "ISO-885913");
   addAlias(QCS_ISO_8859_13, "ISO8859P13");
   addAlias(QCS_ISO_8859_13, "ISO813");
   addAlias(QCS_ISO_8859_13, "LATIN7");
   addAlias(QCS_ISO_8859_13, "LATIN-7");

   QCS_ISO_8859_14 = addUnlocked(ISO885914_STR, "latin-8, Celtic character set");
   addAlias(QCS_ISO_8859_14, "ISO885914");
   addAlias(QCS_ISO_8859_14, "ISO-8859-14");
   addAlias(QCS_ISO_8859_14, "ISO8859-14");
   addAlias(QCS_ISO_8859_14, "ISO-885914");
   addAlias(QCS_ISO_8859_14, "ISO8859P14");
   addAlias(QCS_ISO_8859_14, "ISO814");
   addAlias(QCS_ISO_8859_14, "LATIN8");
   addAlias(QCS_ISO_8859_14, "LATIN-8");

   QCS_ISO_8859_15 = addUnlocked(ISO885915_STR, "latin-9, Western European with euro symbol");
   addAlias(QCS_ISO_8859_15, "ISO885915");
   addAlias(QCS_ISO_8859_15, "ISO-8859-15");
   addAlias(QCS_ISO_8859_15, "ISO8859-15");
   addAlias(QCS_ISO_8859_15, "ISO-885915");
   addAlias(QCS_ISO_8859_15, "ISO8859P15");
   addAlias(QCS_ISO_8859_15, "ISO815");
   addAlias(QCS_ISO_8859_15, "LATIN9");
   addAlias(QCS_ISO_8859_15, "LATIN-9");

   QCS_ISO_8859_16 = addUnlocked(ISO885916_STR, "latin-10, Southeast European character set");
   addAlias(QCS_ISO_8859_16, "ISO885916");
   addAlias(QCS_ISO_8859_16, "ISO-8859-16");
   addAlias(QCS_ISO_8859_16, "ISO8859-16");
   addAlias(QCS_ISO_8859_16, "ISO-885916");
   addAlias(QCS_ISO_8859_16, "ISO8859P16");
   addAlias(QCS_ISO_8859_16, "ISO816");
   addAlias(QCS_ISO_8859_16, "LATIN10");
   addAlias(QCS_ISO_8859_16, "LATIN-10");

   QCS_KOI8_R      = addUnlocked("KOI8-R",      "Russian: Kod Obmena Informatsiey, 8 bit");
   addAlias(QCS_KOI8_R, "KOI8R");

   QCS_KOI8_U      = addUnlocked("KOI8-U",      "Ukrainian: Kod Obmena Informatsiey, 8 bit");
   addAlias(QCS_KOI8_U, "KOI8U");

   QCS_KOI7        = addUnlocked("KOI7",        "Russian: Kod Obmena Informatsiey, 7 bit characters");
   
   QCS_DEFAULT = QCS_UTF8;
};

QoreEncodingManager::~QoreEncodingManager() {
   encoding_map_t::iterator i;
   while ((i = emap.begin()) != emap.end()) {
      class QoreEncoding *qe = i->second;
      emap.erase(i);
      delete qe;
   }
}

void QoreEncodingManager::showEncodings() {
   for (encoding_map_t::const_iterator i = emap.begin(); i != emap.end(); i++)
      printf("%s: %s\n", i->first, i->second->getDesc());
}

void QoreEncodingManager::showAliases() {
   for (const_encoding_map_t::const_iterator i = amap.begin(); i != amap.end(); i++)
      if (strcmp(i->first, i->second->getCode()))
	  printf("%s = %s: %s\n", i->first, i->second->getCode(), i->second->getDesc());
}

void QoreEncodingManager::init(const char *def) {
   // now set default character set
   if (def)
      QCS_DEFAULT = findCreate(def);
   else {
      // first see if QORE_CHARSET exists
      char *estr = getenv("QORE_CHARSET");
      if (estr)
	 QCS_DEFAULT = findCreate(estr);
      else { // try to get character set name from LANG variable
	 estr = getenv("LANG");
	 char *p;
	 if (estr && ((p = strrchr(estr, '.')))) {
	    char *o = strchr(p + 1, '@');
	    if (!o)
	       QCS_DEFAULT = findCreate(p + 1);
	    else {
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

void QoreEncodingManager::addAlias(const QoreEncoding *qcs, const char *alias) {
   mutex.lock();
   amap[alias] = qcs;
   mutex.unlock();
}

const QoreEncoding *QoreEncodingManager::findUnlocked(const char *name) {
   {
      encoding_map_t::const_iterator i = emap.find(name);
      if (i != emap.end())
	 return i->second;
   }

   const_encoding_map_t::const_iterator i = amap.find(name);
   if (i != amap.end())
      return i->second;

   return 0;
}

const QoreEncoding *QoreEncodingManager::findCreate(const char *name) {
   const QoreEncoding *rv;
   mutex.lock();
   rv = findUnlocked(name);
   if (!rv)
      rv = addUnlocked(name, 0);
   mutex.unlock();
   return rv;
}

const QoreEncoding *QoreEncodingManager::findCreate(const QoreString *str) {
   return findCreate(str->getBuffer());
}

qore_size_t q_UTF8_get_char_len(const char* p, qore_size_t len) {
   // see if a multi-byte char is starting
   if ((*p & 0xc0) == 0xc0) {
      //printd(5, "MULTIBYTE *p = %hhx\n", *p);
      // check for a 3-byte sequence
      if ((*p) & 0x20) { 
	 // check for a 4-byte sequence
	 if ((*p) & 0x10) {
	    if (len >= 4) {
	       if ((*(p+1)) & 0x80 && (*(p+2)) & 0x80 && (*(p+3)) & 0x80)
		  return 4;
	       // encoding error - invalid UTF-8 character
	       return 0;
	    }
	    return -4;
	 }
	 else { // should be 3-byte sequence
	    if (len >= 3) {
	       if ((*(p+1)) & 0x80 && (*(p+2)) & 0x80)
		  return 3;
	       // encoding error - invalid UTF-8 character
	       return 0;
	    }
	    return -3;
	 }
      }
      else { // should be a 2-byte sequence - check next char for high bit
	 if (len >= 2) {
	    if ((*(p+1)) & 0x80)
	       return 2;
	    // encoding error - invalid UTF-8 character
	    return 0;
	 }
	 return -2;
      }
   }
   return 1;
}

static qore_size_t UTF8_getLength(const char *p, const char *end, bool &invalid) {
   qore_size_t i = 0;
   while (*p) {
      qore_size_t l = q_UTF8_get_char_len(p, end - p);
      if (l <= 0) {
	 invalid = true;
	 return i;
      }
      p += l;
      i++;
   }

   invalid = false;
   return i;
}

static qore_size_t UTF8_getByteLen(const char *p, const char *end, qore_size_t l, bool &invalid) {
   qore_size_t b = 0;
   while (*p && l) {
      qore_size_t bl = q_UTF8_get_char_len(p, end - p);
      if (bl <= 0) {
	 invalid = true;
	 return b;
      }
      b += bl;
      p += bl;
      l--;
   }
   invalid = false;
   return b;
}

static qore_size_t UTF8_getCharPos(const char *p, const char *end, bool &invalid) {
   qore_size_t i = 0;
   while (p < end) {
      qore_size_t l = q_UTF8_get_char_len(p, end - p);
      if (l <= 0) {
	 invalid = true;
	 return i;
      }
      p += l;
      i++;
   }

   invalid = false;
   return i;
}

qore_size_t QoreEncoding::getLength(const char *p, const char *end, ExceptionSink *xsink) const {
   if (!flength)
      return strlen(p);

   bool invalid;
   qore_size_t rc = flength(p, end, invalid);
   if (invalid) {
      xsink->raiseException("INVALID-ENCODING", "invalid %s encoding encountered in string", code.c_str());
      return 0;
   }
   return rc;
}

qore_size_t QoreEncoding::getByteLen(const char *p, const char *end, qore_size_t c, ExceptionSink *xsink) const {
   if (!fend) {
      qore_size_t len = (end - p);
      if (c > len)
	 c = len;
      return c;
   }

   bool invalid;
   qore_size_t rc = fend(p, end, c, invalid);
   if (invalid) {
      xsink->raiseException("INVALID-ENCODING", "invalid %s encoding encountered in string", code.c_str());
      return 0;
   }
   return rc;
}

qore_size_t QoreEncoding::getCharPos(const char *p, const char *end, ExceptionSink *xsink) const {
   if (!fpos)
      return end - p;

   bool invalid;
   qore_size_t rc = fpos(p, end, invalid);
   if (invalid) {
      xsink->raiseException("INVALID-ENCODING", "invalid %s encoding encountered in string", code.c_str());
      return 0;
   }
   return rc;
}

qore_size_t q_get_byte_len(const QoreEncoding* enc, const char *p, const char *end, qore_size_t c, ExceptionSink *xsink) {
   return enc->getByteLen(p, end, c, xsink);
}

qore_size_t q_get_char_len(const QoreEncoding* enc, const char *p, qore_size_t valid_len, ExceptionSink* xsink) {
   qore_size_t rc = enc->getCharLen(p, valid_len);
   if (rc <= 0) {
      xsink->raiseException("INVALID-ENCODING", "invalid %s encoding encountered in string", enc->getCode());
      return -1;
   }
   return rc;
}

