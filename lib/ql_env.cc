/*
  ql_env.cc
  
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
#include <qore/intern/ql_env.h>

#include <stdio.h>
#include <stdlib.h>

static AbstractQoreNode *f_getenv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
      return 0;
   
   return SysEnv.getAsStringNode(p0->getBuffer());
}

static AbstractQoreNode *f_setenv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;
   const AbstractQoreNode *p1;
   
   if (!(p0 = test_string_param(params, 0))
       || is_nothing((p1 = get_param(params, 1))))
      return 0;

   QoreStringValueHelper t(p1);
   return new QoreBigIntNode(SysEnv.set(p0->getBuffer(), t->getBuffer()));
}

static AbstractQoreNode *f_unsetenv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return 0;
   return new QoreBigIntNode(SysEnv.unset(p0->getBuffer()));
}

void init_env_functions()
{
   builtinFunctions.add("getenv", f_getenv);
   builtinFunctions.add("setenv", f_setenv);
   builtinFunctions.add("unsetenv", f_unsetenv);
}
