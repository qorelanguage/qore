/*
  ql_math.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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
#include <qore/support.h>
#include <qore/QoreNode.h>
#include <qore/QoreString.h>
#include <qore/params.h>
#include <qore/BuiltinFunctionList.h>

#include <qore/ql_math.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static class QoreNode *f_ceil(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(ceil(p0->getAsFloat()));
}

static class QoreNode *f_floor(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(floor(p0->getAsFloat()));
}

static class QoreNode *f_pow(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0, *p1;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   if (!(p1 = get_param(params, 1)))
      return NULL;

   double y = p1->getAsFloat();
   if (y < 0)  
   {
      xsink->raiseException("DIVISION-BY-ZERO", "pow(x, y) y must be a non-negative value");
      return NULL;
   }
   double x = p0->getAsFloat();
   if (x < 0 && y != ceil(y))
   {
      xsink->raiseException("INVALID-POW-ARGUMENTS", "pow(x, y) x cannot be negative when y is not an integer value");
      return NULL;
   }

   return new QoreNode(pow(x, y));
}

static class QoreNode *f_abs(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   if (p0->type == NT_INT)
      return new QoreNode(p0->val.intval < 0 ? -(p0->val.intval) : p0->val.intval);

   return new QoreNode(fabs(p0->getAsFloat()));
}

static class QoreNode *f_hypot(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0, *p1;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   if (!(p1 = get_param(params, 1)))
      return NULL;

   return new QoreNode(hypot(p0->getAsFloat(), p1->getAsFloat()));
}

static class QoreNode *f_sqrt(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(sqrt(p0->getAsFloat()));
}

static class QoreNode *f_cbrt(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(cbrt(p0->getAsFloat()));
}

static class QoreNode *f_sin(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(sin(p0->getAsFloat()));
}

static class QoreNode *f_cos(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(cos(p0->getAsFloat()));
}

static class QoreNode *f_tan(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(tan(p0->getAsFloat()));
}

static class QoreNode *f_asin(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(asin(p0->getAsFloat()));
}

static class QoreNode *f_acos(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(acos(p0->getAsFloat()));
}

static class QoreNode *f_atan(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(atan(p0->getAsFloat()));
}

static class QoreNode *f_sinh(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(sinh(p0->getAsFloat()));
}

static class QoreNode *f_cosh(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(cosh(p0->getAsFloat()));
}

static class QoreNode *f_tanh(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(tanh(p0->getAsFloat()));
}

static class QoreNode *f_nlog(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(log(p0->getAsFloat()));
}

static class QoreNode *f_log10(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(log10(p0->getAsFloat()));
}

#if 0
static class QoreNode *f_log2(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(log2(p0->getAsFloat()));
}
#endif 

static class QoreNode *f_log1p(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(log1p(p0->getAsFloat()));
}

static class QoreNode *f_logb(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(logb(p0->getAsFloat()));
}


static class QoreNode *f_exp(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(exp(p0->getAsFloat()));
}

static class QoreNode *f_exp2(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

#ifdef HAVE_EXP2
   return new QoreNode(exp2(p0->getAsFloat()));
#else
   return new QoreNode(pow(2, p0->getAsFloat()));
#endif
}

static class QoreNode *f_expm1(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   return new QoreNode(expm1(p0->getAsFloat()));
}

// syntax: format_number(".,3", <number>);
static class QoreNode *f_format_number(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0, *p1;
   int decimals = 0, neg = 1, len;
   int64 tr, bi, mi, th, val;
   char thousands_sep, decimal_sep = '.', chr[40], str[40], dec[20];

   if (!(p0 = test_param(params, NT_STRING, 0))
       || !(p1 = get_param(params, 1)))
      return NULL;

   len = p0->val.String->strlen();
   if ((len != 1) && (len != 3))
      return NULL;

   thousands_sep = p0->val.String->getBuffer()[0];
   if (len == 3)
   {
      decimal_sep = p0->val.String->getBuffer()[1];
      decimals = atoi(p0->val.String->getBuffer() + 2);
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

   return new QoreNode(str);
}

void init_math_functions()
{
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
