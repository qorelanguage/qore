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

#include <qore/common.h>

DLLEXPORT int printe(const char *fmt, ...);
DLLEXPORT char *remove_trailing_newlines(char *str);
DLLEXPORT char *remove_trailing_blanks(char *str);

// we supply debugging function also for non-debugging builds as library entry points
// in case a debugging-enabled binary is linked against a non-debugging-enabled lib

//! do not call this function directly, use the macro QORE_TRACE()
DLLEXPORT void trace_function(int code, const char *funcname);
//! do not call this function directly, use the macro printd()
DLLEXPORT int print_debug(int level, const char *fmt, ...);

DLLEXPORT extern int qore_trace;
DLLEXPORT extern int debug;

#define TRACE_IN   1
#define TRACE_OUT  2

#ifdef DEBUG
//! a macro for printing debugging statements; when "DEBUG" is not defined, the function call to print_debug() not be included in the compiled output
#define printd print_debug

//! a macro for tracing, when "DEBUG" is not defined, the function call to trace_function() will not be included in the compiled output
#define QORE_TRACE(a) { trace_function(TRACE_IN, a); ON_BLOCK_EXIT(trace_function, TRACE_OUT, a); }

#else
#ifdef __GNUC__
//! a macro for printing debugging statements; when "DEBUG" is not defined, the function call not be included in the compiled output
#define printd(args...)
//! a macro for tracing, when "DEBUG" is not defined, the function call to trace_function() will not be included in the compiled output
#define QORE_TRACE(args...)
#else
//! a macro for printing debugging statements; when "DEBUG" is not defined, the function call not be included in the compiled output
#define printd(args, ...)
//! a macro for tracing, when "DEBUG" is not defined, the function call to trace_function() will not be included in the compiled output
#define QORE_TRACE(x)
#endif
#endif

#if !defined(HAVE_ISBLANK) && !defined(isblank)
#define isblank(a) ((a) == ' ' || (a) == '\t')
#endif

#endif
