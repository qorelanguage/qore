/*
  ql_io.cc

  Qore programming language

  Copyright 2003 - 2009 David Nichols

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

#include <qore/Qore.h>
#include <qore/intern/ql_io.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

static AbstractQoreNode *f_print(const QoreListNode *params, ExceptionSink *xsink)
{
   if (!params)
      return 0;

   for (unsigned i = 0; i < params->size(); i++)
      print_node(stdout, get_param(params, i));
   return 0;
}

/* f_f_sprintf (f_sprintf) (field sprintf)
 * works like sprintf except when a field width is given, the field width is
 * a hard limit that can't be broken: that is, the arguments output will be
 * truncated if they are larger than the width 
 */
static AbstractQoreNode *f_f_sprintf(const QoreListNode *params, ExceptionSink *xsink)
{
   return q_sprintf(params, 1, 0, xsink);
}

static AbstractQoreNode *f_sprintf(const QoreListNode *params, ExceptionSink *xsink)
{
   return q_sprintf(params, 0, 0, xsink);
}

static AbstractQoreNode *f_vsprintf(const QoreListNode *params, ExceptionSink *xsink)
{
   return q_vsprintf(params, 0, 0, xsink);
}

static AbstractQoreNode *f_f_printf(const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *node;

   print_node(stdout, node = f_f_sprintf(params, xsink));
   return node;
}

static AbstractQoreNode *f_printf(const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *node;

   QORE_TRACE("f_printf()");
   print_node(stdout, node = f_sprintf(params, xsink)); 

   return node;
}

static AbstractQoreNode *f_vprintf(const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *node;

   QORE_TRACE("f_vprintf()");
   print_node(stdout, node = f_vsprintf(params, xsink)); 

   return node;
}

static AbstractQoreNode *f_flush(const QoreListNode *params, ExceptionSink *xsink)
{
   fflush(stdout);
   return 0;
}

void init_io_functions()
{
   builtinFunctions.add("print", f_print, QDOM_TERMINAL_IO);
   builtinFunctions.add("sprintf", f_sprintf);
   builtinFunctions.add("printf", f_printf, QDOM_TERMINAL_IO);
   builtinFunctions.add("f_sprintf", f_f_sprintf);
   builtinFunctions.add("f_printf", f_f_printf, QDOM_TERMINAL_IO);
   builtinFunctions.add("vprintf", f_vprintf, QDOM_TERMINAL_IO);
   builtinFunctions.add("vsprintf", f_vsprintf);
   builtinFunctions.add("flush", f_flush, QDOM_TERMINAL_IO);
}
