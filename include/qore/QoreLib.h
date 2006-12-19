/*
  QoreLib.h

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

#ifndef _QORE_QORELIB_H

#define _QORE_QORELIB_H

#include <qore/config.h>
#include <qore/common.h>
#include <qore/LockedObject.h>
#include <qore/StringList.h>
#include <qore/qore_bitopts.h>

#include <time.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// function to try and make a class name out of a file path, returns a new string that must be free()ed
DLLEXPORT char *make_class_name(char *fn);
// some string formatting functions that work with Qore data structures
DLLEXPORT class QoreString *q_sprintf(class QoreNode *params, int field, int offset, class ExceptionSink *xsink);
DLLEXPORT class QoreString *q_vsprintf(class QoreNode *params, int field, int offset, class ExceptionSink *xsink);
DLLEXPORT struct tm *q_localtime(const time_t *clock, struct tm *tms);
DLLEXPORT struct tm *q_gmtime(const time_t *clock, struct tm *tms);
// thread-safe basename function (resulting pointer must be free()ed)
DLLEXPORT char *q_basename(char *path);
// returns a pointer within the same string
DLLEXPORT char *q_basenameptr(char *path);
// thread-safe basename function (resulting pointer must be free()ed)
DLLEXPORT char *q_dirname(char *path);
DLLEXPORT void qore_setup_argv(int pos, int argc, char *argv[]);

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

static inline void strtolower(char *str)
{
   while (*(str))
   {
      (*str) = tolower(*str);
      str++;
   }
}

static inline char *strtoupper(char *str)
{
   char *p = str;
   while (*(p))
   {
      *p = toupper(*p);
      p++;
   }
   return str;
}

class featureList : public charPtrList
{
   public:
      DLLLOCAL featureList();
      DLLLOCAL ~featureList();
};

// list of qore features
DLLEXPORT extern featureList qoreFeatureList;

#ifndef HAVE_LOCALTIME_R
DLLLOCAL extern class LockedObject lck_localtime;
#endif

#ifndef HAVE_GMTIME_R
DLLLOCAL extern class LockedObject lck_gmtime;
#endif

DLLLOCAL extern char table64[64];

DLLLOCAL class BinaryObject *parseBase64(char *buf, int len, class ExceptionSink *xsink);
DLLLOCAL class BinaryObject *parseHex(char *buf, int len, class ExceptionSink *xsink);
DLLLOCAL class BinaryObject *parseHex(char *buf, int len);
DLLLOCAL void print_node(FILE *fp, class QoreNode *node);
DLLLOCAL void delete_global_variables();
DLLLOCAL void initENV(char *env[]);

#ifdef DEBUG
DLLLOCAL class QoreString *dni(class QoreString *s, class QoreNode *n, int indent, class ExceptionSink *xsink);
#endif

#endif // _QORE_QORELIB_H
