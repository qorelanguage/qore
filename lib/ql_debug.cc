/*
  ql_debug.cc

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/ql_debug.h>
#include <qore/QoreNode.h>
#include <qore/Variable.h>
#include <qore/ql_type.h>
#include <qore/Object.h>
#include <qore/params.h>
#include <qore/charset.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/QoreLib.h>

class Object *CallStack::getPrevStackObject()
{
   class CallNode *w = tail;
   while (w)
   {
      if (w->obj)
	 return w->obj;
      w = w->prev;
   }
   return NULL;
}

static class QoreNode *f_dbg_get_object_ptr(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)(unsigned)thread_list[gettid()].callStack->getPrevStackObject());
}

//static 
class QoreNode *f_dbg_node_info(class QoreNode *params, ExceptionSink *xsink)
{
   QoreString *s = new QoreString();

   return new QoreNode(dni(s, get_param(params, 0), 0, xsink));
}

// returns a hash of all namespace information
static class QoreNode *f_dbg_get_ns_info(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(getRootNS()->getInfo());
}

static class QoreNode *f_dbg_global_vars(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(getProgram()->getVarList());   
}

void init_debug_functions()
{
   builtinFunctions.add("dbg_node_info", f_dbg_node_info);
   builtinFunctions.add("dbg_global_vars", f_dbg_global_vars);
   builtinFunctions.add("dbg_get_ns_info", f_dbg_get_ns_info);
   builtinFunctions.add("dbg_get_object_ptr", f_dbg_get_object_ptr);
}

