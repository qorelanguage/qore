/*
  QoreLib.h

  Qore Programming Language

  Copyright (C) 2005 David Nichols

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

#ifndef _QORE_QORELIB_H

#define _QORE_QORELIB_H

#include <qore/config.h>
#include <qore/common.h>

static inline int64 i8LSB(int64 i);
static inline int   i4LSB(int i);
static inline short i2LSB(short i);

static inline int64 LSBi8(int64 i);
static inline int   LSBi4(int i);
static inline short LSBi2(short i);

static inline int64 i8MSB(int64 i);
static inline int64 MSBi8(int64 i);

#include <qore/LockedObject.h>
#include <qore/StringList.h>

#include <time.h>
#include <string.h>
#include <strings.h>

#ifndef HAVE_LOCALTIME_R
extern class LockedObject lck_localtime;
#endif

#ifndef HAVE_GMTIME_R
extern class LockedObject lck_gmtime;
#endif

extern char table64[64];

class BinaryObject *parseBase64(char *buf, int len, class ExceptionSink *xsink);

#ifdef DEBUG
class QoreString *dni(class QoreString *s, class QoreNode *n, int indent, class ExceptionSink *xsink);
#endif

inline struct tm *q_localtime(const time_t *clock, struct tm *tms)
{
#ifdef HAVE_LOCALTIME_R
   localtime_r(clock, tms);
#else
   lck_localtime.lock();
   struct tm *t = localtime(clock);
   memcpy(tms, t, sizeof(struct tm));
   lck_localtime.unlock();
#endif
   return tms;
}

inline struct tm *q_gmtime(const time_t *clock, struct tm *tms)
{
#ifdef HAVE_GMTIME_R
   gmtime_r(clock, tms);
#else
   lck_gmtime.lock();
   struct tm *t = gmtime(clock);
   memcpy(tms, t, sizeof(struct tm));
   lck_gmtime.unlock();
#endif
   return tms;
}

// thread-safe basename function                                                                                                         
static inline char *q_basename(char *path)
{
   char *p = rindex(path, '/');
   if (!p)
      return strdup(path);
   return strdup(p + 1);
}

static inline char *strchrs(char *str, char *chars)
{
   while (*str)
   {
      if (strchr(chars, *str))
	 return str;
      str++;
   }
   return NULL;
}

// some string formatting functions that work with Qore data structures
class QoreString *q_sprintf(class QoreNode *params, int field, int offset, class ExceptionSink *xsink);
class QoreString *q_vsprintf(class QoreNode *params, int field, int offset, class ExceptionSink *xsink);

static inline int64 swapi8(int64 i)
{ 
   char obuf[8];
   char *ibuf = (char *)&i;
   obuf[7] = ibuf[0];
   obuf[6] = ibuf[1];
   obuf[5] = ibuf[2];
   obuf[4] = ibuf[3];
   obuf[3] = ibuf[4];
   obuf[2] = ibuf[5];
   obuf[1] = ibuf[6];
   obuf[0] = ibuf[7];

   return *((int64 *)obuf);
}

static inline int swapi4(int i)
{ 
   char obuf[4];
   char *ibuf = (char *)&i;
   obuf[3] = ibuf[0];
   obuf[2] = ibuf[1];
   obuf[1] = ibuf[2];
   obuf[0] = ibuf[3];

   return *((int *)obuf);
}

static inline short swapi2(short i)
{ 
   char obuf[2];
   char *ibuf = (char *)&i;
   obuf[1] = ibuf[0];
   obuf[0] = ibuf[1];

   return *((short *)obuf);
}

#ifdef WORDS_BIGENDIAN
static inline int64 i8LSB(int64 i)
{
   return swapi8(i);
/*
   int64 j;
   char *buf = (char *)j;
   buf[0] = i & 0xff;
   buf[1] = (i >> 8) & 0xff;
   buf[2] = (i >> 16) & 0xff;
   buf[3] = (i >> 24) & 0xff;
   buf[4] = (i >> 32) & 0xff;
   buf[5] = (i >> 40) & 0xff;
   buf[6] = (i >> 48) & 0xff;
   buf[7] = (i >> 56) & 0xff;
   return j;
*/
}

static inline int i4LSB(int i)
{
   return swapi4(i);
/*
   int j;
   char *buf = (char *)j;
   buf[0] = i & 0xff;
   buf[1] = (i >> 8) & 0xff;
   buf[2] = (i >> 16) & 0xff;
   buf[3] = (i >> 24) & 0xff;
   return j;
*/
}

static inline short i2LSB(short i)
{
   return swapi2(i);
/* 
   short j;
   char *buf = (char *)j;
   buf[0] = i & 0xff;
   buf[1] = (i >> 8) & 0xff;
   return j;
*/
}

static inline int64 LSBi8(int64 i)
{ 
   return swapi8(i);
/*
   int64 j;
   char *obuf = (char *)j;
   buf[7] = i & 0xff;
   buf[6] = (i >> 8) & 0xff;
   buf[5] = (i >> 16) & 0xff;
   buf[4] = (i >> 24) & 0xff;
   buf[3] = (i >> 32) & 0xff;
   buf[2] = (i >> 40) & 0xff;
   buf[1] = (i >> 48) & 0xff;
   buf[0] = (i >> 56) & 0xff;
   return j;
*/
}

static inline int LSBi4(int i)
{
   return swapi4(i);
/*
   int j;
   char *buf = (char *)j;
   buf[0] = (i >> 24) & 0xff;
   buf[1] = (i >> 16) & 0xff;
   buf[2] = (i >> 8) & 0xff;
   buf[3] = i & 0xff;
   return j;
*/
}

static inline short LSBi2(short i)
{ 
   return swapi2(i);
/*
   short j;
   char *buf = (char *)j;
   buf[0] = (i >> 8) & 0xff;
   buf[1] = i & 0xff;
   return j;
*/
}

static inline int64 i8MSB(int64 i) { return i; }
static inline int64 MSBi8(int64 i) { return i; }
#else
static inline int64 i8LSB(int64 i) { return i; }
static inline int   i4LSB(int i)   { return i; }
static inline short i2LSB(short i) { return i; }

static inline int64 LSBi8(int64 i) { return i; }
static inline int   LSBi4(int i)   { return i; }
static inline short LSBi2(short i) { return i; }

static inline int64 i8MSB(int64 i)
{ 
   return swapi8(i);
/*
   int64 j;
   char *buf = (char *)j;
   buf[7] = i & 0xff;
   buf[6] = (i >> 8) & 0xff;
   buf[5] = (i >> 16) & 0xff;
   buf[4] = (i >> 24) & 0xff;
   buf[3] = (i >> 32) & 0xff;
   buf[2] = (i >> 40) & 0xff;
   buf[1] = (i >> 48) & 0xff;
   buf[0] = (i >> 56) & 0xff;
   return j;
*/
}

static inline int64 MSBi8(int64 i) 
{ 
   return swapi8(i);
}

#endif

class featureList : public charPtrList
{
   public:
      featureList()
      {
	 // register default features
	 append("sql");
	 append("threads");
	 append("xml");
#ifdef QORE_DEBUG
	 append("debug");
#endif
#ifdef QORE_MONOLITHIC
# ifdef NCURSES
	 append("ncurses");
# endif
# ifdef ORACLE
	 append("oracle");
# endif
# ifdef QORE_MYSQL
	 append("mysql");
# endif
# ifdef TIBRV
	 append("tibrv");
# endif
# ifdef TIBAE
	 append("tibae");
# endif
#endif
      }

      inline void populate(class charPtrList *l)
      {
	 class charPtrNode *w = getHead();
	 while (w)
	 {
	    l->append(w->str);
	    w = w->next;
	 }
      }
};

// list of qore features
extern featureList qoreFeatureList;

#endif // _QORE_QORELIB_H
