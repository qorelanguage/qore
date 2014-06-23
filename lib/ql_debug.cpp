/*
  ql_debug.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include <qore/intern/ql_debug.h>
#include <qore/intern/ql_type.h>

static inline void strindent(QoreString *s, int indent) {
   for (int i = 0; i < indent; i++)
      s->concat(' ');
}

static void dni(QoreStringNode *s, const AbstractQoreNode *n, int indent, ExceptionSink *xsink) {
   if (!n) {
      s->concat("node=NULL");
      return;
   }
   
   s->sprintf("node=%p refs=%d type=%s ", n, n->reference_count(), n->getTypeName());

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
      s->sprintf("elements=%d (cls=%p, type=%s, valid=%s)", o->size(xsink),
		 o->getClass(),
		 o->getClass() ? o->getClass()->getName() : "<none>",
		 o->isValid() ? "yes" : "no");
      {
	 // FIXME: this is inefficient, use copyData and a hashiterator instead
	 ReferenceHolder<QoreListNode> l(o->getMemberList(xsink), xsink);
	 if (l) {
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
	 while (hi.next()) {
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
      qore_tm info;
      date->getInfo(info);
      s->sprintf("%04d-%02d-%02d %02d:%02d:%02d.%06d",
		 info.year, info.month, info.day, info.hour,
		 info.minute, info.second, info.us);
      if (date->isRelative())
	 s->concat(" (relative)");
      else
	 s->sprintf(" %s (%s)", info.zone_name, info.regionName());
      return;
   }

   if (ntype == NT_BINARY) {
      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(n);
      s->sprintf("ptr=%p len=%d", b->getPtr(), b->size());
      return;
   }

   s->sprintf("don't know how to print type %d: '%s' :-(", ntype, n->getTypeName());
}

//static 
AbstractQoreNode *f_dbg_node_info(const QoreListNode *params, ExceptionSink *xsink) {
   QoreStringNodeHolder s(new QoreStringNode());
   dni(*s, get_param(params, 0), 0, xsink);
   if (*xsink)
      return 0;
   return s.release();
}

int64 f_dbg_node_addr(const QoreListNode *params, ExceptionSink *xsink) {
   return (int64)get_param(params, 0);
}

// returns a hash of all namespace information
static AbstractQoreNode *f_dbg_get_ns_info(const QoreListNode *params, ExceptionSink *xsink) {
   return getRootNS()->getInfo();
}

static AbstractQoreNode *f_dbg_global_vars(const QoreListNode *params, ExceptionSink *xsink) {
   return getProgram()->getVarList();
}

void init_debug_functions(QoreNamespace& qns) {
   qns.addBuiltinVariant("dbg_node_info", f_dbg_node_info, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG, "node");
   qns.addBuiltinVariant("dbg_node_addr", f_dbg_node_addr, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG, "node");
   qns.addBuiltinVariant("dbg_global_vars", f_dbg_global_vars, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo);
   qns.addBuiltinVariant("dbg_get_ns_info", f_dbg_get_ns_info, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo);
}

