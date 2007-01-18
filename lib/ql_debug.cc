/*
  ql_debug.cc

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
#include <qore/qore_thread.h>

static inline void strindent(QoreString *s, int indent)
{
   for (int i = 0; i < indent; i++)
      s->concat(' ');
}

static class QoreString *dni(class QoreString *s, class QoreNode *n, int indent, class ExceptionSink *xsink)
{
   tracein("dni()");
   if (!n)
   {
      s->concat("node=NULL\n");
      traceout("dni()");
      return s;
   }
   
   s->sprintf("node=%08p refs=%d type=%s ", n, n->reference_count(), n->type->getName());
   
   if (n->type == NT_BOOLEAN)
      s->sprintf("val=%s\n", n->val.boolval ? "True" : "False");
   
   else if (n->type == NT_INT)
      s->sprintf("val=%lld\n", n->val.intval);
   
   else if (n->type == NT_NOTHING)
      s->sprintf("val=NOTHING\n");
   
   else if (n->type == NT_NULL)
      s->sprintf("val=SQL NULL\n");
   
   else if (n->type == NT_FLOAT)
      s->sprintf("val=%f\n", n->val.floatval);
   
   else if (n->type == NT_STRING)
      s->sprintf("val=(enc=%s, %d:%d) \"%s\"\n", 
                 n->val.String->getEncoding()->code,
                 n->val.String->length(),
                 n->val.String->strlen(),
                 n->val.String->getBuffer());
   
   else if (n->type ==  NT_LIST)
   {
      s->sprintf("elements=%d\n", n->val.list->size());
      for (int i = 0; i < n->val.list->size(); i++)
      {
         strindent(s, indent);
         s->sprintf("list element %d/%d: ", i, n->val.list->size());
         dni(s, n->val.list->retrieve_entry(i), indent + 3, xsink);
      }
   }
   
   else if (n->type == NT_OBJECT)
   {
      s->sprintf("elements=%d (type=%s, valid=%s)\n", n->val.object->size(),
                 n->val.object->getClass() ? n->val.object->getClass()->getName() : "<none>",
		 n->val.object->isValid() ? "yes" : "no");
      {
         List *l = n->val.object->getMemberList(xsink);
         if (l)
         {
            for (int i = 0; i < l->size(); i++)
            {
               strindent(s, indent);
               s->sprintf("key %d/%d \"%s\" = ", i, l->size(), l->retrieve_entry(i)->val.String->getBuffer());
               QoreNode *nn;
               dni(s, nn = n->val.object->evalMemberNoMethod(l->retrieve_entry(i)->val.String->getBuffer(), xsink), indent + 3, xsink);
               discard(nn, xsink);
            }
            l->derefAndDelete(xsink);
         }
      }
   }
   
   else if (n->type == NT_HASH)
   {
      s->sprintf("elements=%d\n", n->val.hash->size());
      {
         int i = 0;
         HashIterator hi(n->val.hash);
         while (hi.next())
         {
            strindent(s, indent);
            s->sprintf("key %d/%d \"%s\" = ", i++, n->val.hash->size(), hi.getKey());
            dni(s, hi.getValue(), indent + 3, xsink);
         }
      }
   }
   
   else if (n->type == NT_DATE)
      s->sprintf("%04d-%02d-%02d %02d:%02d:%02d.%03d (rel=%s)\n", 
                 n->val.date_time->getYear(),
                 n->val.date_time->getMonth(),
                 n->val.date_time->getDay(),
                 n->val.date_time->getHour(),
                 n->val.date_time->getMinute(),
                 n->val.date_time->getSecond(),
                 n->val.date_time->getMillisecond(),
                 n->val.date_time->isRelative() ? "True" : "False");
   else if (n->type == NT_BINARY)
      s->sprintf("ptr=%08p len=%d\n", n->val.bin->getPtr(), n->val.bin->size());
   else
      s->sprintf("don't know how to print value :-(\n");
   
   //printd(5, "D\n");
   traceout("dni()");
   return s;
}

static class QoreNode *f_dbg_get_object_ptr(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)(unsigned long)getCallStack()->getPrevStackObject());
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

