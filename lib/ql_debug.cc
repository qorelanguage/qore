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

#include <qore/Qore.h>
#include <qore/intern/CallStack.h>
#include <qore/intern/ql_debug.h>
#include <qore/intern/ql_type.h>

static inline void strindent(QoreString *s, int indent)
{
   for (int i = 0; i < indent; i++)
      s->concat(' ');
}

static void dni(QoreStringNode *s, class QoreNode *n, int indent, class ExceptionSink *xsink)
{
   if (!n)
   {
      s->concat("node=NULL\n");
      return;
   }
   
   s->sprintf("node=%08p refs=%d type=%s ", n, n->reference_count(), n->type->getName());

   {
      QoreStringNode *str = dynamic_cast<QoreStringNode *>(n);
      if (str) {
	 s->sprintf("val=(enc=%s, %d:%d) \"%s\"", str->getEncoding()->getCode(), str->length(), str->strlen(), str->getBuffer());
	 return;
      }
   }
   
   if (n->type == NT_BOOLEAN) {
      s->sprintf("val=%s", n->val.boolval ? "True" : "False");
      return;
   }

   if (n->type == NT_INT) {
      s->sprintf("val=%lld", n->val.intval);
      return;
   }
  
   if (n->type == NT_NOTHING) {
      s->sprintf("val=NOTHING");
      return;
   }

   if (n->type == NT_NULL) {
      s->sprintf("val=SQL NULL");
      return;
   }

   if (n->type == NT_FLOAT) {
      s->sprintf("val=%f", n->val.floatval);
      return;
   }
   
   if (n->type == NT_LIST)
   {
      s->sprintf("elements=%d\n", n->val.list->size());
      ListIterator li(n->val.list);
      int i = 0;
      while (li.next()) {
         strindent(s, indent);
         s->sprintf("list element %d/%d: ", i++, n->val.list->size());
         dni(s, li.getValue(), indent + 3, xsink);
	 if (!li.last())
	    s->concat('\n');
      }
      return;
   }
   
   if (n->type == NT_OBJECT)
   {
      s->sprintf("elements=%d (type=%s, valid=%s)\n", n->val.object->size(xsink),
                 n->val.object->getClass() ? n->val.object->getClass()->getName() : "<none>",
		 n->val.object->isValid() ? "yes" : "no");
      {
         QoreList *l = n->val.object->getMemberList(xsink);
         if (l)
         {
            for (int i = 0; i < l->size(); i++)
            {
               strindent(s, indent);
	       QoreStringNode *entry = reinterpret_cast<QoreStringNode *>(l->retrieve_entry(i));
               s->sprintf("key %d/%d \"%s\" = ", i, l->size(), entry->getBuffer());
               QoreNode *nn;
               dni(s, nn = n->val.object->evalMemberNoMethod(entry->getBuffer(), xsink), indent + 3, xsink);
               discard(nn, xsink);
	       if (i != (l->size() - 1))
		  s->concat('\n');
            }
            l->derefAndDelete(xsink);
         }
      }
      return;
   }
   
   if (n->type == NT_HASH)
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
	    if (!hi.last())
	       s->concat('\n');
         }
      }
      return;
   }
   
   {
      DateTimeNode *date = dynamic_cast<DateTimeNode *>(n);
      if (date) {
	 s->sprintf("%04d-%02d-%02d %02d:%02d:%02d.%03d (rel=%s)", 
		    date->getYear(), date->getMonth(), date->getDay(), date->getHour(),
		    date->getMinute(), date->getSecond(), date->getMillisecond(), date->isRelative() ? "True" : "False");
	 return;
      }
   }

   if (n->type == NT_BINARY) {
      s->sprintf("ptr=%08p len=%d", n->val.bin->getPtr(), n->val.bin->size());
      return;
   }

   s->sprintf("don't know how to print type '%s' :-(", n->type->getName());
}

//static 
class QoreNode *f_dbg_node_info(const QoreNode *params, ExceptionSink *xsink)
{
   TempQoreStringNode s(new QoreStringNode());
   dni(*s, get_param(params, 0), 0, xsink);
   if (*xsink)
      return 0;
   return s.release();
}

// returns a hash of all namespace information
static class QoreNode *f_dbg_get_ns_info(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(getRootNS()->getInfo());
}

static class QoreNode *f_dbg_global_vars(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(getProgram()->getVarList());   
}

void init_debug_functions()
{
   builtinFunctions.add("dbg_node_info", f_dbg_node_info);
   builtinFunctions.add("dbg_global_vars", f_dbg_global_vars);
   builtinFunctions.add("dbg_get_ns_info", f_dbg_get_ns_info);
}

