/*
  ql_env.cpp
  
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
#include <qore/intern/ql_env.h>

#include <stdio.h>
#include <stdlib.h>

static AbstractQoreNode *f_getenv(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, params, 0);
   return SysEnv.getAsStringNode(str->getBuffer());
}

static AbstractQoreNode *f_setenv(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   HARD_QORE_PARAM(t, const QoreStringNode, params, 1);

   return new QoreBigIntNode(SysEnv.set(p0->getBuffer(), t->getBuffer()));
}

static AbstractQoreNode *f_unsetenv(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   return new QoreBigIntNode(SysEnv.unset(p0->getBuffer()));
}

void init_env_functions() {
   builtinFunctions.add2("getenv", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   // getenv() returns *string
   builtinFunctions.add2("getenv", f_getenv, QC_CONSTANT, QDOM_DEFAULT, 0, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("setenv", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("setenv", f_setenv, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softStringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("unsetenv", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("unsetenv", f_unsetenv, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
}
