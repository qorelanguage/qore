/*
  charset.h

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

#ifndef _QORE_CHARSET_H

#define _QORE_CHARSET_H

#include <qore/LockedObject.h>

#include <strings.h>
#include <string.h>

// for multi-byte character set encodings: gives the length of the string in characters
typedef int (*mbcs_length_t)(char *);
// for multi-byte character set encodings: gives the number of bytes for the number of chars
typedef int (*mbcs_end_t)(char *, int);
// for multi-byte character set encodings: gives the character position of the ptr
typedef int (*mbcs_pos_t)(char *, char *);

struct QoreEncoding {
      const char *code;
      mbcs_length_t flength;
      mbcs_end_t fend;
      mbcs_pos_t fpos;
      const char *desc;
      struct QoreEncoding *next;

      inline QoreEncoding(const char *c, mbcs_length_t l, mbcs_end_t e, mbcs_pos_t p, const char *d)
      {
	 code = c;
	 flength = l;
	 fend = e;
	 fpos = p;
	 desc = d;
	 next = NULL;
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

struct QoreEncodingAlias {
      struct QoreEncoding *qcs;
      const char *alias;
      struct QoreEncodingAlias *next;

      inline QoreEncodingAlias(struct QoreEncoding *q, const char *a)
      {
	 qcs = q;
	 alias = a;
	 next = NULL;
      }
};

class QoreEncodingManager : public LockedObject 
{
   private:
      class QoreEncoding *head, *tail;
      class QoreEncodingAlias *ahead, *atail;

      inline void addUnlocked(struct QoreEncoding *qcs)
      {
	 if (tail)
	    tail->next = qcs;
	 else
	    head = qcs;
	 tail = qcs;
      }
      inline struct QoreEncoding *addUnlocked(const char *code, mbcs_length_t l, mbcs_end_t e, mbcs_pos_t p, const char *desc)
      {
	 struct QoreEncoding *qcs = new QoreEncoding(code, l, e, p, desc);
	 addUnlocked(qcs);
	 return qcs;
      }

   public:
      QoreEncodingManager();
      inline ~QoreEncodingManager()
      {
	 while (head)
	 {
	    tail = head->next;
	    delete head;
	    head = tail;
	 }
	 while (ahead)
	 {
	    atail = ahead->next;
	    delete ahead;
	    ahead = atail;
	 }
      }
      inline struct QoreEncoding *add(const char *code, mbcs_length_t l, mbcs_end_t e, mbcs_pos_t p, const char *desc)
      {
	 struct QoreEncoding *qcs = new QoreEncoding(code, l, e, p, desc);
	 lock();
	 addUnlocked(qcs);
	 unlock();
	 return qcs;
      }
      inline void addAlias(struct QoreEncoding *qcs, const char *alias)
      {
	 struct QoreEncodingAlias *qca = new QoreEncodingAlias(qcs, alias);
	 lock();
	 if (atail)
	    atail->next = qca;
	 else
	    ahead = qca;
	 atail = qca;
	 unlock();
      }
      inline struct QoreEncoding *find(char *name)
      {
	 struct QoreEncoding *w = head;
	 while (w)
	 {
	    if (!strcasecmp(name, w->code))
	       return w;
	    w = w->next;
	 }

	 struct QoreEncodingAlias *aw = ahead;
	 while (aw)
	 {
	    if (!strcasecmp(name, aw->alias))
	       return aw->qcs;
	    aw = aw->next;
	 }
	 return NULL;
      }
      inline struct QoreEncoding *findCreate(char *name)
      {
	 struct QoreEncoding *rv;
	 lock();
	 rv = find(name);
	 if (!rv)
	    rv = add(name, NULL, NULL, NULL, NULL);
	 unlock();
	 return rv;
      }
      inline struct QoreEncoding *findCreate(class QoreString *str);
      void showEncodings();
      void showAliases();
      void init(char *def);
};

extern QoreEncodingManager QEM;

// builtin character encodings
extern QoreEncoding *QCS_DEFAULT, *QCS_USASCII, *QCS_UTF8, *QCS_ISO_8859_1,
   *QCS_ISO_8859_2, *QCS_ISO_8859_3, *QCS_ISO_8859_4, *QCS_ISO_8859_5,
   *QCS_ISO_8859_6, *QCS_ISO_8859_7, *QCS_ISO_8859_8, *QCS_ISO_8859_9,
   *QCS_ISO_8859_10, *QCS_ISO_8859_11, *QCS_ISO_8859_13, *QCS_ISO_8859_14,
   *QCS_ISO_8859_15, *QCS_ISO_8859_16, *QCS_KOI8_R, *QCS_KOI8_U, *QCS_KOI7;

#include <qore/QoreString.h>

inline struct QoreEncoding *QoreEncodingManager::findCreate(class QoreString *str)
{
   return findCreate(str->getBuffer());
}

#endif // _QORE_CHARSET_H
