/*
  type.cpp

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
   return get_bool_node(HARD_QORE_BOOL(params, 0));
}

static AbstractQoreNode *f_int(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(HARD_QORE_INT(params, 0));
}

static AbstractQoreNode *f_float(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(HARD_QORE_FLOAT(params, 0));
}

static AbstractQoreNode *f_string(const QoreListNode *params, ExceptionSink *xsink) {
   return HARD_QORE_STRING(params, 0)->refSelf();
}

static AbstractQoreNode *f_binary_str(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *str = HARD_QORE_STRING(params, 0);
   BinaryNode *b = new BinaryNode;
   b->append(str->getBuffer(), str->strlen());
   return b;
}

static AbstractQoreNode *f_binary_bin(const QoreListNode *params, ExceptionSink *xsink) {
   return HARD_QORE_BINARY(params, 0)->refSelf();
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
   // do not flag with QC_NOOP, as it is used as an initializer
   builtinFunctions.add2("boolean", f_bool_noop, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo);
   builtinFunctions.add2("boolean", f_boolean, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo, 1, softBoolTypeInfo, QORE_PARAM_NO_ARG);
   
   // do not flag with QC_NOOP, as it is used as an initializer
   builtinFunctions.add2("int", f_int_noop, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);
   builtinFunctions.add2("int", f_int, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // do not flag with QC_NOOP, as it is used as an initializer
   builtinFunctions.add2("float", f_float_noop, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("float", f_float, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   // do not flag with QC_NOOP, as it is used as an initializer
   builtinFunctions.add2("string", f_string_noop, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo);
   builtinFunctions.add2("string", f_string, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, softStringTypeInfo, QORE_PARAM_NO_ARG);

   // do not flag with QC_NOOP, as it is used as an initializer
   builtinFunctions.add2("binary", f_binary_noop, QC_CONSTANT, QDOM_DEFAULT, binaryTypeInfo);
   builtinFunctions.add2("binary", f_binary_str, QC_CONSTANT, QDOM_DEFAULT, binaryTypeInfo, 1, softStringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("binary", f_binary_bin, QC_CONSTANT, QDOM_DEFAULT, binaryTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);

   // do not flag with QC_NOOP, as it is used as an initializer
   builtinFunctions.add2("date", f_date_noop, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("date", f_date, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("date", f_date, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add("list", f_list);

   builtinFunctions.add("hash", f_hash);

   builtinFunctions.add("type", f_type);
   builtinFunctions.add("typename", f_type);
   builtinFunctions.add("binary_to_string", f_binary_to_string);
}
