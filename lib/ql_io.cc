/*
  ql_io.cc

  Qore programming language

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

#include <qore/config.h>
#include <qore/ql_io.h>
#include <qore/Function.h>
#include <qore/Variable.h>
#include <qore/QoreNode.h>
#include <qore/support.h>
#include <qore/QoreString.h>
#include <qore/params.h>
#include <qore/Exception.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/QoreLib.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

static class QoreNode *f_print(class QoreNode *params, ExceptionSink *xsink)
{
   int i;

   if (!params)
      return NULL;

   for (i = 0; i < params->val.list->size(); i++)
      print_node(stdout, get_param(params, i));
   return NULL;
}

/* f_f_sprintf (f_sprintf) (field sprintf)
 * works like sprintf except when a field width is given, the field width is
 * a hard limit that can't be broken: that is, the arguments output will be
 * truncated if they are larger than the width 
 */
static class QoreNode *f_f_sprintf(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(q_sprintf(params, 1, 0, xsink));
}

static class QoreNode *f_sprintf(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(q_sprintf(params, 0, 0, xsink));
}

static class QoreNode *f_vsprintf(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(q_vsprintf(params, 0, 0, xsink));
}

static class QoreNode *f_f_printf(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *node;

   print_node(stdout, node = f_f_sprintf(params, xsink));
   return node;
}

static class QoreNode *f_printf(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *node;

   tracein("f_printf()");
   print_node(stdout, node = f_sprintf(params, xsink)); 
   traceout("f_printf()");
   return node;
}

static class QoreNode *f_vprintf(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *node;

   tracein("f_vprintf()");
   print_node(stdout, node = f_vsprintf(params, xsink)); 
   traceout("f_vprintf()");
   return node;
}

static class QoreNode *f_flush(class QoreNode *params, ExceptionSink *xsink)
{
   fflush(stdout);
   return NULL;
}

void init_io_functions()
{
   builtinFunctions.add("print", f_print);
   builtinFunctions.add("sprintf", f_sprintf);
   builtinFunctions.add("printf", f_printf);
   builtinFunctions.add("f_sprintf", f_f_sprintf);
   builtinFunctions.add("f_printf", f_f_printf);
   builtinFunctions.add("vprintf", f_vprintf);
   builtinFunctions.add("vsprintf", f_vsprintf);
   builtinFunctions.add("flush", f_flush);
}
