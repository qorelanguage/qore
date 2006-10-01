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
class BinaryObject *parseHex(char *buf, int len, class ExceptionSink *xsink);
class BinaryObject *parseHex(char *buf, int len);

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

// thread-safe basename function (resulting pointer must be free()ed)
static inline char *q_basename(char *path)
{
   char *p = strrchr(path, '/');
   if (!p)
      return strdup(path);
   return strdup(p + 1);
}

// returns a pointer within the same string
static inline char *q_basenameptr(char *path)
{
   char *p = strrchr(path, '/');
   if (!p)
      return path;
   return p + 1;   
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

// function to try and make a class name out of a file path, returns a new string that must be free()ed
char *make_class_name(char *fn);

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
}

static inline int i4LSB(int i)
{
   return swapi4(i);
}

static inline short i2LSB(short i)
{
   return swapi2(i);
}

static inline int64 LSBi8(int64 i)
{ 
   return swapi8(i);
}

static inline int LSBi4(int i)
{
   return swapi4(i);
}

static inline short LSBi2(short i)
{ 
   return swapi2(i);
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
	 push_back("sql");
	 push_back("threads");
	 push_back("xml");
#ifdef QORE_DEBUG
	 push_back("debug");
#endif
#ifdef QORE_MONOLITHIC
# ifdef NCURSES
	 push_back("ncurses");
# endif
# ifdef ORACLE
	 push_back("oracle");
# endif
# ifdef QORE_MYSQL
	 push_back("mysql");
# endif
# ifdef TIBRV
	 push_back("tibrv");
# endif
# ifdef TIBAE
	 push_back("tibae");
# endif
#endif
      }
};

// for grabbing a lock and releasing it when the object goes out of scope
class TransientLock {
   private:
      LockedObject *lock;
   public:
      TransientLock(LockedObject *l) { lock = l; l->lock(); }
      ~TransientLock() { lock->unlock(); }
};

// list of qore features
extern featureList qoreFeatureList;

#endif // _QORE_QORELIB_H
