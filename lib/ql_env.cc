/*
  ql_env.cc
  
  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

static class QoreNode *f_getenv(const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
      return NULL;
   
   return Env.getAsStringNode(p0->getBuffer());
}

static class QoreNode *f_setenv(const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   QoreNode *p1;
   
   if (!(p0 = test_string_param(params, 0))
       || is_nothing((p1 = get_param(params, 1))))
      return NULL;

   QoreStringValueHelper t(p1);
   return new QoreNode((int64)Env.set(p0->getBuffer(), t->getBuffer()));
}

static class QoreNode *f_unsetenv(const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;
   return new QoreNode((int64)Env.unset(p0->getBuffer()));
}

void init_env_functions()
{
   builtinFunctions.add("getenv", f_getenv);
   builtinFunctions.add("setenv", f_setenv);
   builtinFunctions.add("unsetenv", f_unsetenv);
}
