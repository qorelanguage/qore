/*
  ql_math.cpp

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
#include <qore/intern/ql_math.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static AbstractQoreNode *f_round(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_ROUND
   return new QoreFloatNode(round(HARD_QORE_FLOAT(params, 0)));
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "this system does not implement round(); for maximum portability use the constant Option::HAVE_ROUND to check if this function is implemented before calling");
   return 0;
#endif
}

static AbstractQoreNode *f_ceil(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(ceil(HARD_QORE_FLOAT(params, 0)));
}

static AbstractQoreNode *f_floor(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(floor(HARD_QORE_FLOAT(params, 0)));
}

static AbstractQoreNode *f_pow(const QoreListNode *params, ExceptionSink *xsink) {
   double y = HARD_QORE_FLOAT(params, 1);
   if (y < 0) {
      xsink->raiseException("DIVISION-BY-ZERO", "pow(x, y) y must be a non-negative value");
      return 0;
   }
   double x = HARD_QORE_FLOAT(params, 0);
   if (x < 0 && y != ceil(y)) {
      xsink->raiseException("INVALID-POW-ARGUMENTS", "pow(x, y) x cannot be negative when y is not an integer value");
      return 0;
   }

   return new QoreFloatNode(pow(x, y));
}

static AbstractQoreNode *f_abs_int(const QoreListNode *params, ExceptionSink *xsink) {
   int64 i = HARD_QORE_INT(params, 0);
   return new QoreBigIntNode(i < 0 ? -i : i);
}

static AbstractQoreNode *f_abs_float(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(fabs(HARD_QORE_FLOAT(params, 0)));
}

static AbstractQoreNode *f_hypot(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(hypot(get_float_param(params, 0), get_float_param(params, 1)));
}

static AbstractQoreNode *f_sqrt(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(sqrt(HARD_QORE_FLOAT(params, 0)));
}

static AbstractQoreNode *f_cbrt(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(cbrt(HARD_QORE_FLOAT(params, 0)));
}

static AbstractQoreNode *f_sin(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(sin(HARD_QORE_FLOAT(params, 0)));
}

static AbstractQoreNode *f_cos(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(cos(get_float_param(params, 0)));
}

static AbstractQoreNode *f_tan(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(tan(HARD_QORE_FLOAT(params, 0)));
}

static AbstractQoreNode *f_asin(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(asin(HARD_QORE_FLOAT(params, 0)));
}

static AbstractQoreNode *f_acos(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(acos(get_float_param(params, 0)));
}

static AbstractQoreNode *f_atan(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(atan(HARD_QORE_FLOAT(params, 0)));
}

static AbstractQoreNode *f_atan2(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(atan2(HARD_QORE_FLOAT(params, 0), get_float_param(params, 1)));
}

static AbstractQoreNode *f_sinh(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(sinh(HARD_QORE_FLOAT(params, 0)));
}

static AbstractQoreNode *f_cosh(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(cosh(get_float_param(params, 0)));
}

static AbstractQoreNode *f_tanh(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(tanh(HARD_QORE_FLOAT(params, 0)));
}

static AbstractQoreNode *f_nlog(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(log(get_float_param(params, 0)));
}

static AbstractQoreNode *f_log10(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(log10(get_float_param(params, 0)));
}

#if 0
static AbstractQoreNode *f_log2(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(log2(get_float_param(params, 0)));
}
#endif 

static AbstractQoreNode *f_log1p(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(log1p(HARD_QORE_FLOAT(params, 0)));
}

static AbstractQoreNode *f_logb(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(logb(get_float_param(params, 0)));
}

static AbstractQoreNode *f_exp(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(exp(get_float_param(params, 0)));
}

static AbstractQoreNode *f_exp2(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_EXP2
   return new QoreFloatNode(exp2(get_float_param(params, 0)));
#else
   return new QoreFloatNode(pow(2, get_float_param(params, 0)));
#endif
}

static AbstractQoreNode *f_expm1(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreFloatNode(expm1(HARD_QORE_FLOAT(params, 0)));
}

// syntax: format_number(".,3", <number>);
static AbstractQoreNode *f_format_number(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   int decimals = 0, neg = 1, len;
   int64 tr, bi, mi, th, val;
   char thousands_sep, decimal_sep = '.', chr[40], str[40], dec[20];

   len = p0->strlen();
   if ((len != 1) && (len != 3))
      return 0;

   thousands_sep = p0->getBuffer()[0];
   if (len == 3) {
      decimal_sep = p0->getBuffer()[1];
      decimals = atoi(p0->getBuffer() + 2);
   }

   double t = get_float_param(params, 1);
   if (t < 0) {
      neg = -1;
      t *= -1;
   }
   val = (int64)t;
   if (len == 3) {
      t -= (double)val;
      sprintf(chr, "%%.%df", decimals);
      sprintf(dec, chr, t);
   }
   tr = val / 1000000000000ll;
   val -= tr * 1000000000000ll;
   bi = val / 1000000000ll;
   val -= bi * 1000000000ll;
   mi = val / 1000000ll;
   val -= mi * 1000000ll;
   th = val / 1000ll;
   val -= th * 1000ll;
   //printd(0, "tr=%lld bi=%lld mi=%lld th=%lld val=%lld\n", tr, bi, mi, th, val);

   if (tr) {
      if (len == 3)
	 sprintf(str, "%lld%c%03lld%c%03lld%c%03lld%c%03lld%c%s", 
		 neg * tr, thousands_sep,
		 bi, thousands_sep, 
		 mi, thousands_sep,
		 th, thousands_sep,
		 val, decimal_sep,
		 dec + 2);
      else
	 sprintf(str, "%lld%c%03lld%c%03lld%c%03lld%c%03lld", 
		 neg * tr, thousands_sep,
		 bi, thousands_sep, 
		 mi, thousands_sep,
		 th, thousands_sep,
		 val);
   }
   else if (bi)
      if (len == 3)
	 sprintf(str, "%lld%c%03lld%c%03lld%c%03lld%c%s", 
		 neg * bi, thousands_sep,
		 mi, thousands_sep,
		 th, thousands_sep,
		 val, decimal_sep,
		 dec + 2);
      else
	 sprintf(str, "%lld%c%03lld%c%03lld%c%03lld", 
		 neg * bi, thousands_sep,
		 mi, thousands_sep,
		 th, thousands_sep,
		 val);
   else if (mi)
      if (len == 3)
	 sprintf(str, "%lld%c%03lld%c%03lld%c%s", neg * mi, thousands_sep,
		 th, thousands_sep,
		 val, decimal_sep,
		 dec + 2);
      else
	 sprintf(str, "%lld%c%03lld%c%03lld", neg * mi, thousands_sep,
		 th, thousands_sep,
		 val);
   else if (th)
      if (len == 3)
	 sprintf(str, "%lld%c%03lld%c%s", neg * th, thousands_sep,
		 val, decimal_sep,
		 dec + 2);
      else
	 sprintf(str, "%lld%c%03lld", neg * th, thousands_sep,
		 val);
   else
      if (len == 3)
	 sprintf(str, "%lld%c%s", neg * val, decimal_sep, &dec[2]);
      else
	 sprintf(str, "%lld", neg * val);

   return new QoreStringNode(str);
}

void init_math_functions() {
   builtinFunctions.add2("round",         f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("round",         f_round, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("ceil",          f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("ceil",          f_ceil, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("floor",         f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("floor",         f_floor, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("pow",           f_pow, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 2, softFloatTypeInfo, zero_float(), softFloatTypeInfo, zero_float());

   // overloaded abs()
   builtinFunctions.add2("abs",           f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("abs",           f_abs_float, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("abs",           f_abs_int, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo, 1, bigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("hypot",         f_hypot, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 2, anyTypeInfo, QORE_PARAM_NO_ARG, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("sqrt",          f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("sqrt",          f_sqrt, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("cbrt",          f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("cbrt",          f_cbrt, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);
 
   builtinFunctions.add2("sin",           f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("sin",           f_sin, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("cos",           f_cos, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("tan",           f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("tan",           f_tan, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("asin",          f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("asin",          f_asin, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("acos",          f_acos, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("atan",          f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("atan",          f_atan, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("atan2",         f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("atan2",         f_atan2, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 2, softFloatTypeInfo, QORE_PARAM_NO_ARG, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("sinh",          f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("sinh",          f_sinh, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("cosh",          f_cosh, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("tanh",          f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("tanh",          f_tanh, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("nlog",          f_nlog, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("log10",         f_log10, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   //builtinFunctions.add("log2",          f_log2);

   builtinFunctions.add2("log1p",         f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("log1p",         f_log1p, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("logb",          f_logb, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("exp",           f_exp, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("exp2",          f_exp2, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("expm1",         f_float_noop, QC_NOOP, QDOM_DEFAULT, floatTypeInfo);
   builtinFunctions.add2("expm1",         f_expm1, QC_CONSTANT, QDOM_DEFAULT, floatTypeInfo, 1, softFloatTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("format_number", f_noop, QC_NOOP);
   builtinFunctions.add2("format_number", f_format_number, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, anyTypeInfo, QORE_PARAM_NO_ARG);
}
