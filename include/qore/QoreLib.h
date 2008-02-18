/*
  QoreLib.h

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

#ifndef _QORE_QORELIB_H

#define _QORE_QORELIB_H

#include <qore/common.h>
#include <qore/QoreThreadLock.h>
#include <qore/StringList.h>
#include <qore/qore_bitopts.h>
#include <qore/safe_dslist>

#include <time.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// function to try and make a class name out of a file path, returns a new string that must be free()ed
DLLEXPORT char *make_class_name(const char *fn);
// some string formatting functions that work with Qore data structures
DLLEXPORT class QoreStringNode *q_sprintf(const class QoreListNode *params, int field, int offset, class ExceptionSink *xsink);
DLLEXPORT class QoreStringNode *q_vsprintf(const class QoreListNode *params, int field, int offset, class ExceptionSink *xsink);
DLLEXPORT struct tm *q_localtime(const time_t *clock, struct tm *tms);
DLLEXPORT struct tm *q_gmtime(const time_t *clock, struct tm *tms);
// thread-safe basename function (resulting pointer must be free()ed)
DLLEXPORT char *q_basename(const char *path);
// returns a pointer within the same string
DLLEXPORT char *q_basenameptr(const char *path);
// thread-safe basename function (resulting pointer must be free()ed)
DLLEXPORT char *q_dirname(const char *path);
DLLEXPORT void qore_setup_argv(int pos, int argc, char *argv[]);

// implemented in Variable.h
DLLEXPORT AbstractQoreNode **get_var_value_ptr(const AbstractQoreNode *lvalue, class AutoVLock *vl, class ExceptionSink *xsink);
// implemented in Variable.h, only returns a value if the variable's value is of type string
DLLEXPORT class QoreStringNode **get_string_var_value_ptr(class AbstractQoreNode *lvalue, class AutoVLock *vl, class ExceptionSink *xsink);

// find one of any characters in a string
static inline char *strchrs(const char *str, const char *chars)
{
   while (*str)
   {
      if (strchr(chars, *str))
	 return (char *)str;
      str++;
   }
   return 0;
}

// find a character in a string up to len
static inline char *strnchr(const char *str, int len, char c)
{
   int i = 0;
   while (i++ != len)
   {
      if (*str == c)
	 return (char *)str;
      ++str;
   }
   return 0;
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

// this list must be thread-safe for reading, writing under a lock
class FeatureList : public safe_dslist<std::string>
{
   private:
      // not implemented
      DLLLOCAL FeatureList(const FeatureList&);
      DLLLOCAL FeatureList& operator=(const FeatureList&);

   public:
      DLLLOCAL FeatureList();
      DLLLOCAL ~FeatureList();
};

// list of qore features
DLLEXPORT extern FeatureList qoreFeatureList;

#endif // _QORE_QORELIB_H
