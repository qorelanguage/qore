/*
  ql_math.cc

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
#include <qore/intern/ql_math.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static AbstractQoreNode *f_round(const QoreListNode *params, ExceptionSink *xsink)
{
#ifdef HAVE_ROUND
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(round(p0->getAsFloat()));
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "this system does not implement round(); for maximum portability use the constant Option::HAVE_ROUND to check if this function is implemented before calling");
   return 0;
#endif
}

static AbstractQoreNode *f_ceil(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(ceil(p0->getAsFloat()));
}

static AbstractQoreNode *f_floor(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(floor(p0->getAsFloat()));
}

static AbstractQoreNode *f_pow(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0, *p1;

   if (!(p0 = get_param(params, 0)))
      return 0;

   if (!(p1 = get_param(params, 1)))
      return 0;

   double y = p1->getAsFloat();
   if (y < 0)  
   {
      xsink->raiseException("DIVISION-BY-ZERO", "pow(x, y) y must be a non-negative value");
      return 0;
   }
   double x = p0->getAsFloat();
   if (x < 0 && y != ceil(y))
   {
      xsink->raiseException("INVALID-POW-ARGUMENTS", "pow(x, y) x cannot be negative when y is not an integer value");
      return 0;
   }

   return new QoreFloatNode(pow(x, y));
}

static AbstractQoreNode *f_abs(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   {
      const QoreBigIntNode *b = dynamic_cast<const QoreBigIntNode *>(p0);
      if (b)
	 return new QoreBigIntNode(b->val < 0 ? -(b->val) : b->val);
   }

   return new QoreFloatNode(fabs(p0->getAsFloat()));
}

static AbstractQoreNode *f_hypot(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0, *p1;

   if (!(p0 = get_param(params, 0)))
      return 0;

   if (!(p1 = get_param(params, 1)))
      return 0;

   return new QoreFloatNode(hypot(p0->getAsFloat(), p1->getAsFloat()));
}

static AbstractQoreNode *f_sqrt(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(sqrt(p0->getAsFloat()));
}

static AbstractQoreNode *f_cbrt(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(cbrt(p0->getAsFloat()));
}

static AbstractQoreNode *f_sin(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(sin(p0->getAsFloat()));
}

static AbstractQoreNode *f_cos(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(cos(p0->getAsFloat()));
}

static AbstractQoreNode *f_tan(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(tan(p0->getAsFloat()));
}

static AbstractQoreNode *f_asin(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(asin(p0->getAsFloat()));
}

static AbstractQoreNode *f_acos(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(acos(p0->getAsFloat()));
}

static AbstractQoreNode *f_atan(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(atan(p0->getAsFloat()));
}

static AbstractQoreNode *f_atan2(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   const AbstractQoreNode *p1 = get_param(params, 1);

   return new QoreFloatNode(atan2(p0->getAsFloat(), p1 ? p1->getAsFloat() : 0.0));
}

static AbstractQoreNode *f_sinh(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(sinh(p0->getAsFloat()));
}

static AbstractQoreNode *f_cosh(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(cosh(p0->getAsFloat()));
}

static AbstractQoreNode *f_tanh(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(tanh(p0->getAsFloat()));
}

static AbstractQoreNode *f_nlog(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(log(p0->getAsFloat()));
}

static AbstractQoreNode *f_log10(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(log10(p0->getAsFloat()));
}

#if 0
static AbstractQoreNode *f_log2(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(log2(p0->getAsFloat()));
}
#endif 

static AbstractQoreNode *f_log1p(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(log1p(p0->getAsFloat()));
}

static AbstractQoreNode *f_logb(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(logb(p0->getAsFloat()));
}


static AbstractQoreNode *f_exp(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(exp(p0->getAsFloat()));
}

static AbstractQoreNode *f_exp2(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

#ifdef HAVE_EXP2
   return new QoreFloatNode(exp2(p0->getAsFloat()));
#else
   return new QoreFloatNode(pow(2, p0->getAsFloat()));
#endif
}

static AbstractQoreNode *f_expm1(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return 0;

   return new QoreFloatNode(expm1(p0->getAsFloat()));
}

// syntax: format_number(".,3", <number>);
static AbstractQoreNode *f_format_number(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;
   const AbstractQoreNode *p1;
   int decimals = 0, neg = 1, len;
   int64 tr, bi, mi, th, val;
   char thousands_sep, decimal_sep = '.', chr[40], str[40], dec[20];

   if (!(p0 = test_string_param(params, 0)) || !(p1 = get_param(params, 1)))
      return 0;

   len = p0->strlen();
   if ((len != 1) && (len != 3))
      return 0;

   thousands_sep = p0->getBuffer()[0];
   if (len == 3)
   {
      decimal_sep = p0->getBuffer()[1];
      decimals = atoi(p0->getBuffer() + 2);
   }

   double t = p1->getAsFloat();
   if (t < 0)
   {
      neg = -1;
      t *= -1;
   }
   val = (int64)t;
   if (len == 3)
   {
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
//   printf("tr=%lld bi=%lld mi=%lld th=%lld val=%lld\n", tr, bi, mi, th, val);

   if (tr)
   {
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

void init_math_functions()
{
   builtinFunctions.add("round",         f_round);
   builtinFunctions.add("ceil",          f_ceil);
   builtinFunctions.add("floor",         f_floor);
   builtinFunctions.add("pow",           f_pow);
   builtinFunctions.add("abs",           f_abs);
   builtinFunctions.add("hypot",         f_hypot);
   builtinFunctions.add("sqrt",          f_sqrt);
   builtinFunctions.add("cbrt",          f_cbrt);
   builtinFunctions.add("sin",           f_sin);
   builtinFunctions.add("cos",           f_cos);
   builtinFunctions.add("tan",           f_tan);
   builtinFunctions.add("asin",          f_asin);
   builtinFunctions.add("acos",          f_acos);
   builtinFunctions.add("atan",          f_atan);
   builtinFunctions.add("atan2",         f_atan2);
   builtinFunctions.add("sinh",          f_sinh);
   builtinFunctions.add("cosh",          f_cosh);
   builtinFunctions.add("tanh",          f_tanh);

   builtinFunctions.add("nlog",          f_nlog);
   builtinFunctions.add("log10",         f_log10);
   //builtinFunctions.add("log2",          f_log2);
   builtinFunctions.add("log1p",         f_log1p);
   builtinFunctions.add("logb",          f_logb);
   builtinFunctions.add("exp",           f_exp);
   builtinFunctions.add("exp2",          f_exp2);
   builtinFunctions.add("expm1",         f_expm1);

   builtinFunctions.add("format_number", f_format_number);
}
