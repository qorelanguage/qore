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
#include <qore/intern/qore_date_private.h>

#include <memory>

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

static AbstractQoreNode *f_date_date(const QoreListNode *params, ExceptionSink *xsink) {
  return HARD_QORE_DATE(params, 0)->refSelf();
}

static AbstractQoreNode *f_date(const QoreListNode *params, ExceptionSink *xsink) {
   DateTimeNodeValueHelper date(get_param(params, 0));
   return date.getReferencedValue();
}

// returns 0 if invalid data is encountered
static inline int parse_int_2(const char *p) {
   if (*p < '0' || *p > '9')
      return 0;
   int rv = (*p - '0') * 10;
   ++p;
   if (*p < '0' || *p > '9')
      return 0;
   rv += (*p - '0');
   return rv;
}

// returns 0 if invalid data is encountered
static inline int parse_int_3(const char *p) {
   if (*p < '0' || *p > '9')
      return 0;
   int rv = (*p - '0') * 100;
   ++p;
   if (*p < '0' || *p > '9')
      return 0;
   rv += (*p - '0') * 10;
   ++p;
   if (*p < '0' || *p > '9')
      return 0;
   rv += (*p - '0');
   return rv;
}

// returns 0 if invalid data is encountered
static inline int parse_int_4(const char *p) {
   if (*p < '0' || *p > '9')
      return 0;
   int rv = (*p - '0') * 1000;
   ++p;
   if (*p < '0' || *p > '9')
      return 0;
   rv += (*p - '0') * 100;
   ++p;
   if (*p < '0' || *p > '9')
      return 0;
   rv += (*p - '0') * 10;
   ++p;
   if (*p < '0' || *p > '9')
      return 0;
   rv += (*p - '0');
   return rv;
}

static AbstractQoreNode *f_date_mask(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode * dtstr = HARD_QORE_STRING(params, 0);
   const QoreStringNode * mask = HARD_QORE_STRING(params, 1);

   // obtain the century
   int century;
   {
      DateTime tmpdt(q_epoch());
      century = tmpdt.getYear() / 100 * 100;
   }

   struct tm dt;
   zero_tm(dt);
   // milliseconds
   int ms = 0;

   const char *d = dtstr->getBuffer();
   const char *s = mask->getBuffer();

   while (*s) {
      switch (*s) {
         case 'Y':
            if (s[1] != 'Y') {
               xsink->raiseException("DATE-CONVERT-ERROR", "'Y' has to be used as 'YY' or 'YYYY'");
               return 0;
               break;
            }
            ++s;
            if ((s[1] == 'Y') && (s[2] == 'Y')) {
               dt.tm_year = parse_int_4(d) - 1900;
               s += 2;
               d += 3;
            }
            else {
               dt.tm_year = parse_int_2(d) + century;
               ++d;
            }
            break;
         case 'M':
            if (s[1] == 'M') {
               dt.tm_mon = parse_int_2(d) - 1;
               s++;
               d++;
               break;
            }
            // 'M' is not supported because there is no clear way how to get eg. 1 or 11
            if ((s[1] == 'o') && (s[2] == 'n')) {
	       QoreString str(d, 3);
               dt.tm_mon = str.strlen() == 3 ? qore_date_info::getMonthIxFromAbbr(str.getBuffer(), false) : 0;
               if (dt.tm_mon < 0 || dt.tm_mon > 11) {
		  if (!*xsink)
		     xsink->raiseException("DATE-CONVERT-ERROR", "Wrong 'Mon' string: '%s'", !str.empty() ? str.getBuffer() : "<none>");
		  return 0;
               }
               s += 2;
               d += 2;
               break;
            }
            if ((s[1] == 'O') && (s[2] == 'N')) {
	       QoreString str(d, 3);
               dt.tm_mon = str.strlen() == 3 ? qore_date_info::getMonthIxFromAbbr(str.getBuffer(), true) : 0;
               if (dt.tm_mon < 0 || dt.tm_mon > 11) {
		  if (!*xsink)
		     xsink->raiseException("DATE-CONVERT-ERROR", "Wrong 'MON' string: '%s'", !str.empty() ? str.getBuffer() : "<none>");
		  return 0;
               }
               s += 2;
               d += 2;
               break;
            }
            break;
         case 'D':
            if (s[1] == 'D') {
               dt.tm_mday = parse_int_2(d);
               s++;
               d++;
            }
           break;
         case 'H':
            if (s[1] == 'H') {
               dt.tm_hour = parse_int_2(d);
               s++;
               d++;
            }
            break;
         case 'm':
            if (s[1] == 'm') {
               dt.tm_min = parse_int_2(d);
               s++;
               d++;
            }
            break;
         case 's':
            if (s[1] == 's' && s[2] == 's') {
               ms = parse_int_3(d);
               s += 2;
               d += 2;
            }
            break;
         case 'S':
            if (s[1] == 'S') {
               dt.tm_sec = parse_int_2(d);
               s++;
               d++;
            }
            break;
#if 0
            TODO/FIXME: timezones?
         case 'z':
	    str.sprintf("%s", i.zname);
            break;
	    // add iso8601 UTC offset
	 case 'Z':
	    concatOffset(i.utcoffset, str);
	    break;
         default:
	    str.concat(*s);
            break;
#endif
      }
      s++;
      d++;
   }

   if (*xsink)
      return 0;

   DateTimeNode *n = new DateTimeNode;
   n->setDate(&dt, ms);
   return n;
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

static AbstractQoreNode *f_hash_list_list(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreListNode *keys = HARD_QORE_LIST(params, 0);
   const QoreListNode *values = HARD_QORE_LIST(params, 1);

   ReferenceHolder<QoreHashNode> h(new QoreHashNode, xsink);

   ConstListIterator ki(keys);
   ConstListIterator vi(values);

   bool valid = true;
   while (ki.next()) {
      if (valid)
	 valid = vi.next();

      QoreStringValueHelper str(ki.getValue());
      h->setKeyValue(str->getBuffer(), valid ? vi.getReferencedValue() : 0, xsink);      
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
   builtinFunctions.add2("boolean", f_bool_noop, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo, 1, nullTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("boolean", f_boolean, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo, 1, softBoolTypeInfo, QORE_PARAM_NO_ARG);

   // do not flag with QC_NOOP, as it is used as an initializer
   builtinFunctions.add2("int", f_int_noop, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);
   builtinFunctions.add2("int", f_int_noop, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, nullTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("int", f_int, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // do not flag with QC_NOOP, as it is used as an initializer
   builtinFunctions.add2("float", f_float_noop, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("float", f_float_noop, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, nullTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("float", f_float, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   // do not flag with QC_NOOP, as it is used as an initializer
   builtinFunctions.add2("string", f_string_noop, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo);
   builtinFunctions.add2("string", f_string_noop, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, nullTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("string", f_string, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, softStringTypeInfo, QORE_PARAM_NO_ARG);

   // do not flag with QC_NOOP, as it is used as an initializer
   builtinFunctions.add2("binary", f_binary_noop, QC_CONSTANT, QDOM_DEFAULT, binaryTypeInfo);
   builtinFunctions.add2("binary", f_binary_noop, QC_CONSTANT, QDOM_DEFAULT, binaryTypeInfo, 1, nullTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("binary", f_binary_str, QC_CONSTANT, QDOM_DEFAULT, binaryTypeInfo, 1, softStringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("binary", f_binary_bin, QC_CONSTANT, QDOM_DEFAULT, binaryTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);

   // do not flag with QC_NOOP, as it is used as an initializer
   builtinFunctions.add2("date", f_date_noop, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo);
   builtinFunctions.add2("date", f_date_noop, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, nullTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("date", f_date, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("date", f_date_mask, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("date", f_date, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, floatTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("date", f_date, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("date", f_date_date, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("list", f_list, QC_CONSTANT | QC_USES_EXTRA_ARGS, QDOM_DEFAULT, listTypeInfo);

   builtinFunctions.add2("hash", f_hash, QC_CONSTANT, QDOM_DEFAULT, hashTypeInfo);
   builtinFunctions.add2("hash", f_hash_list, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("hash", f_hash_list_list, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 2, listTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("hash", f_hash_hash, QC_CONSTANT, QDOM_DEFAULT, hashTypeInfo, 1, hashTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("hash", f_hash_obj, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, objectTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("type", f_type, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("typename", f_type, QC_CONSTANT | QC_DEPRECATED, QDOM_DEFAULT, stringTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("binary_to_string", f_binary_to_string, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("binary_to_string", f_binary_to_string_bin_str, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("is_date_relative", f_bool_noop, QC_NOOP, QDOM_DEFAULT, boolTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_date_relative", f_is_date_relative, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("is_date_absolute", f_bool_noop, QC_NOOP, QDOM_DEFAULT, boolTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_date_absolute", f_is_date_absolute, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);
}
