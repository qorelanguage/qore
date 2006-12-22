/*
  support.h

  Qore Programming Language

  Copyright (C) David Nichols 2005

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

#ifndef QORE_SUPPORT_H

#define QORE_SUPPORT_H

#include <qore/config.h>
#include <qore/common.h>

DLLEXPORT int printe(const char *fmt, ...);
DLLEXPORT char *remove_trailing_newlines(char *str);
DLLEXPORT char *remove_trailing_blanks(char *str);

// we supply debugging function also for non-debugging builds as library entry points
// in case a debugging-enabled binary is linked against a non-debugging-enabled lib
DLLEXPORT void trace_function(int code, char *funcname);
DLLEXPORT int print_debug(int level, const char *fmt, ...);

DLLEXPORT extern int qore_trace;
DLLEXPORT extern int debug;

#define TRACE_IN   1
#define TRACE_OUT  2

#ifdef DEBUG
#define printd print_debug

#define tracein(a) trace_function(TRACE_IN, a)
#define traceout(a) trace_function(TRACE_OUT, a)

#else
#ifdef __GNUC__
#define printd(args...)
#define tracein(args...)
#define traceout(args...)
#else
#define printd(args, ...)
#define tracein(args, ...)
#define traceout(args, ...)
#endif
#endif

// the following functions are only referenced from C++ source
DLLLOCAL void parse_error(const char *fmt, ...);
DLLLOCAL void parseException(char *err, const char *fmt, ...);
DLLLOCAL class QoreString *findFileInEnvPath(char *file, char *varname);

#ifndef HAVE_ISBLANK
#define isblank(a) ((a) == ' ' || (a) == '\t')
#endif

#endif
