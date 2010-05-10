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
   QoreListNode *l;
   if (num_params(params) > 1)
      l = params->copy();
   else {
      l = new QoreListNode;
      const AbstractQoreNode *p0 = get_param(params, 0);
      if (p0)
	 l->push(p0->refSelf());
   }
   return l;
}

static AbstractQoreNode *f_hash_obj(const QoreListNode *params, ExceptionSink *xsink) {
   return HARD_QORE_OBJECT(params, 0)->getRuntimeMemberHash(xsink);
}

static AbstractQoreNode *f_hash_list(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreListNode *l = HARD_QORE_LIST(params, 0);

   ReferenceHolder<QoreHashNode> h(new QoreHashNode, xsink);

   ConstListIterator li(l);
   while (li.next()) {
      QoreStringValueHelper str(li.getValue());
      h->setKeyValue(str->getBuffer(), li.next() ? li.getReferencedValue() : 0, xsink);
      if (*xsink)
	 return 0;
   }
   return h.release();
}

static AbstractQoreNode *f_hash_hash(const QoreListNode *params, ExceptionSink *xsink) {
   return HARD_QORE_HASH(params, 0)->refSelf();
}

static AbstractQoreNode *f_hash(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreHashNode;
}

static AbstractQoreNode *f_type(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   return new QoreStringNode(p ? p->getTypeName() : "nothing");
}

static AbstractQoreNode *f_binary_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   const BinaryNode *b = HARD_QORE_BINARY(params, 0);
   if (!b->size())
      return new QoreStringNode;
   return new QoreStringNode((const char *)b->getPtr(), b->size());
}

static AbstractQoreNode *f_binary_to_string_bin_str(const QoreListNode *params, ExceptionSink *xsink) {
   const BinaryNode *b = HARD_QORE_BINARY(params, 0);
   const QoreEncoding *qcs = get_hard_qore_encoding_param(params, 1);

   if (!b->size())
      return new QoreStringNode(qcs);

   return new QoreStringNode((const char *)b->getPtr(), b->size(), qcs);
}

static AbstractQoreNode *f_is_date_relative(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *d = HARD_QORE_DATE(params, 0);
   return get_bool_node(d->isRelative());
}

static AbstractQoreNode *f_is_date_absolute(const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *d = HARD_QORE_DATE(params, 0);
   return get_bool_node(d->isAbsolute());
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

   builtinFunctions.add2("list", f_list, QC_CONSTANT | QC_USES_EXTRA_ARGS, QDOM_DEFAULT, listTypeInfo);

   builtinFunctions.add2("hash", f_hash, QC_CONSTANT, QDOM_DEFAULT, hashTypeInfo);
   builtinFunctions.add2("hash", f_hash_list, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("hash", f_hash_hash, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, hashTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("hash", f_hash_obj, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, objectTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("type", f_type, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("typename", f_type, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("binary_to_string", f_binary_to_string, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("binary_to_string", f_binary_to_string_bin_str, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("is_date_relative", f_bool_noop, QC_NOOP, QDOM_DEFAULT, boolTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_date_relative", f_is_date_relative, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("is_date_absolute", f_bool_noop, QC_NOOP, QDOM_DEFAULT, boolTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_date_absolute", f_is_date_absolute, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);
}
