/*
  ql_debug.cc

  Qore Programming Language

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
#include <qore/intern/CallStack.h>
#include <qore/intern/ql_debug.h>
#include <qore/intern/ql_type.h>

static inline void strindent(QoreString *s, int indent) {
   for (int i = 0; i < indent; i++)
      s->concat(' ');
}

static void dni(QoreStringNode *s, const AbstractQoreNode *n, int indent, ExceptionSink *xsink) {
   if (!n)
   {
      s->concat("node=NULL");
      return;
   }
   
   s->sprintf("node=%08p refs=%d type=%s ", n, n->reference_count(), n->getTypeName());

   qore_type_t ntype = n->getType();

   if (ntype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(n);
      s->sprintf("val=(enc=%s, %d:%d) \"%s\"", str->getEncoding()->getCode(), str->length(), str->strlen(), str->getBuffer());
      return;
   }
   
   if (ntype == NT_BOOLEAN) {
      s->sprintf("val=%s", reinterpret_cast<const QoreBoolNode *>(n)->getValue() ? "True" : "False");
      return;
   }

   if (ntype == NT_INT) {
      s->sprintf("val=%lld", (reinterpret_cast<const QoreBigIntNode *>(n))->val);
      return;
   }
  
   if (ntype == NT_NOTHING) {
      s->sprintf("val=NOTHING");
      return;
   }

   if (ntype == NT_NULL) {
      s->sprintf("val=SQL NULL");
      return;
   }

   if (ntype == NT_FLOAT) {
      s->sprintf("val=%f", reinterpret_cast<const QoreFloatNode *>(n)->f);
      return;
   }
   
   if (ntype == NT_LIST) {
      const QoreListNode *l = reinterpret_cast<const QoreListNode *>(n);
      s->sprintf("elements=%d", l->size());
      ConstListIterator li(l);
      while (li.next()) {
	 s->concat('\n');
	 strindent(s, indent);
	 s->sprintf("list element %d/%d: ", li.index(), l->size());
	 dni(s, li.getValue(), indent + 3, xsink);
      }
      return;
   }
   
   if (ntype == NT_OBJECT) {
      const QoreObject *o = reinterpret_cast<const QoreObject *>(n);
      s->sprintf("elements=%d (type=%s, valid=%s)", o->size(xsink),
		 o->getClass() ? o->getClass()->getName() : "<none>",
		 o->isValid() ? "yes" : "no");
      {
	 // FIXME: this is inefficient, use copyData and a hashiterator instead
	 ReferenceHolder<QoreListNode> l(o->getMemberList(xsink), xsink);
	 if (l)
	 {
	    for (unsigned i = 0; i < l->size(); i++) {
	       s->concat('\n');
	       strindent(s, indent);
	       QoreStringNode *entry = reinterpret_cast<QoreStringNode *>(l->retrieve_entry(i));
	       s->sprintf("key %d/%d \"%s\" = ", i, l->size(), entry->getBuffer());
	       AbstractQoreNode *nn;
	       dni(s, nn = o->getReferencedMemberNoMethod(entry->getBuffer(), xsink), indent + 3, xsink);
	       discard(nn, xsink);
	    }
	 }
      }
      return;
   }

   if (ntype == NT_HASH) {
      const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(n);
      s->sprintf("elements=%d", h->size());
      {
	 int i = 0;
	 ConstHashIterator hi(h);
	 while (hi.next())
	 {
	    s->concat('\n');
	    strindent(s, indent);
	    s->sprintf("key %d/%d \"%s\" = ", i++, h->size(), hi.getKey());
	    dni(s, hi.getValue(), indent + 3, xsink);
	 }
      }
      return;
   }

   if (ntype == NT_DATE) {
      const DateTimeNode *date = reinterpret_cast<const DateTimeNode *>(n);
      if (date) {
	 s->sprintf("%04d-%02d-%02d %02d:%02d:%02d.%03d (rel=%s)", 
		    date->getYear(), date->getMonth(), date->getDay(), date->getHour(),
		    date->getMinute(), date->getSecond(), date->getMillisecond(), date->isRelative() ? "True" : "False");
	 return;
      }
   }

   if (ntype == NT_BINARY) {
      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(n);
      s->sprintf("ptr=%08p len=%d", b->getPtr(), b->size());
      return;
   }

   s->sprintf("don't know how to print type '%s' :-(", n->getTypeName());
}

//static 
AbstractQoreNode *f_dbg_node_info(const QoreListNode *params, ExceptionSink *xsink) {
   QoreStringNodeHolder s(new QoreStringNode());
   dni(*s, get_param(params, 0), 0, xsink);
   if (*xsink)
      return 0;
   return s.release();
}

AbstractQoreNode *f_dbg_node_addr(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   return new QoreBigIntNode((int64)p);
}

// returns a hash of all namespace information
static AbstractQoreNode *f_dbg_get_ns_info(const QoreListNode *params, ExceptionSink *xsink) {
   return getRootNS()->getInfo();
}

static AbstractQoreNode *f_dbg_global_vars(const QoreListNode *params, ExceptionSink *xsink) {
   return getProgram()->getVarList();
}

void init_debug_functions() {
   builtinFunctions.add("dbg_node_info", f_dbg_node_info);
   builtinFunctions.add("dbg_node_addr", f_dbg_node_addr);
   builtinFunctions.add("dbg_global_vars", f_dbg_global_vars);
   builtinFunctions.add("dbg_get_ns_info", f_dbg_get_ns_info);
}

