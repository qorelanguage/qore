/*
  type.cc

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
#include <qore/intern/ql_type.h>

static AbstractQoreNode *f_boolean(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   return get_bool_node(p ? p->getAsBool() : false);
}

static AbstractQoreNode *f_int(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(p0 ? p0->getAsBigInt() : 0);
}

static AbstractQoreNode *f_float(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreFloatNode(p0 ? p0->getAsFloat() : 0.0);
}

static AbstractQoreNode *f_string(const QoreListNode *params, ExceptionSink *xsink) {
   QoreStringNodeValueHelper str(get_param(params, 0));
   return str.getReferencedValue();
}

static AbstractQoreNode *f_binary(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (!p0)
      return new BinaryNode();

   qore_type_t p0_type = p0->getType();
   if (p0_type == NT_BINARY)
      return p0->refSelf();

   if (p0_type == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p0);
      BinaryNode *b = new BinaryNode();
      b->append(str->getBuffer(), str->strlen());
      return b;
   }

   // convert to string and make binary object
   QoreStringValueHelper t(p0);
   TempString str(t.giveString());
   int len = str->strlen();
   return new BinaryNode(str->giveBuffer(), len);
}

static AbstractQoreNode *f_date(const QoreListNode *params, ExceptionSink *xsink) {
   DateTimeNodeValueHelper date(get_param(params, 0));
   return date.getReferencedValue();
}

static AbstractQoreNode *f_list(const QoreListNode *params, ExceptionSink *xsink) {
   QoreListNode *l = new QoreListNode();
   if (num_params(params) > 1)
      l->push(params->eval(xsink));
   else {
      const AbstractQoreNode *p0 = get_param(params, 0);
      if (!is_nothing(p0))
	 l->push(p0->eval(xsink));
   }
   return l;
}

static AbstractQoreNode *f_hash(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   qore_type_t t = p ? p->getType() : NT_NOTHING;
   if (t == NT_HASH)
      return p->refSelf();

   if (t == NT_OBJECT) {
      const QoreObject *o = reinterpret_cast<const QoreObject *>(p);
      return o->getRuntimeMemberHash(xsink);
   }

   ReferenceHolder<QoreHashNode> h(new QoreHashNode, xsink);

   if (t == NT_LIST) {
      ConstListIterator li(reinterpret_cast<const QoreListNode *>(p));
      while (li.next()) {
	 QoreStringValueHelper str(li.getValue());
	 h->setKeyValue(str->getBuffer(), li.next() ? li.getReferencedValue() : 0, xsink);
	 if (*xsink)
	    return 0;
      }
   }
   return h.release();
}

static AbstractQoreNode *f_type(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p)
      return new QoreStringNode("nothing");
   return new QoreStringNode(p->getTypeName());
}

static AbstractQoreNode *f_binary_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   const BinaryNode *b = test_binary_param(params, 0);
   if (!b) {
      xsink->raiseException("BIANRY-TO-STRING-ERROR", "missing required binary value as first argument to binary_to_string()");
      return 0;
   }

   const QoreEncoding *qcs;
   {
      const QoreStringNode *pcs;
      qcs = (pcs = test_string_param(params, 1)) ? QEM.findCreate(pcs) : QCS_DEFAULT;
   }

   if (!b->size())
      return new QoreStringNode(qcs);

   return new QoreStringNode((const char *)b->getPtr(), b->size(), qcs);
}

void init_type_functions() {
   builtinFunctions.add("boolean", f_boolean);
   builtinFunctions.add("int", f_int);
   builtinFunctions.add("float", f_float);
   builtinFunctions.add("string", f_string);
   builtinFunctions.add("date", f_date);
   builtinFunctions.add("binary", f_binary);
   builtinFunctions.add("list", f_list);
   builtinFunctions.add("hash", f_hash);
   builtinFunctions.add("type", f_type);
   builtinFunctions.add("typename", f_type);
   builtinFunctions.add("binary_to_string", f_binary_to_string);
}
