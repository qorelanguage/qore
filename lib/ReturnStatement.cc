/*
 ReturnStatement.cc
 
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
#include <qore/intern/ReturnStatement.h>

ReturnStatement::ReturnStatement(int start_line, int end_line, AbstractQoreNode *v) : AbstractStatement(start_line, end_line) {
   exp = v;
}

ReturnStatement::~ReturnStatement() {
   // this should never be 0, but in case the implementation changes...
   if (exp)
      exp->deref(0);
}

int ReturnStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   if (exp)
      (*return_value) = exp->eval(xsink);

   const QoreTypeInfo *returnTypeInfo = getReturnTypeInfo();
   if (returnTypeInfo->checkType(*return_value, xsink)) {
      discard(*return_value, xsink);
      (*return_value) = 0;
   }

   return RC_RETURN;
}

int ReturnStatement::parseInitImpl(LocalVar *oflag, int pflag) {
   int lvids = 0;
   const QoreTypeInfo *argTypeInfo = 0;

   if (exp)
      exp = exp->parseInit(oflag, pflag, lvids, argTypeInfo);
   else
      argTypeInfo = &nothingTypeInfo;

   const QoreTypeInfo *returnTypeInfo = getReturnTypeInfo();

   //printd(5, "ReturnStatement::parseInitImpl() arg=%s rt=%s\n", argTypeInfo->getTypeName(), returnTypeInfo->getTypeName());

   // check return type and throw a parse exception only if parse exceptions are enabled
   if (!returnTypeInfo->parseEqual(argTypeInfo) && getProgram()->getParseExceptionSink()) {
      QoreStringNode *desc = new QoreStringNode("return value for this block expects ");
      returnTypeInfo->getThisType(*desc);
      desc->concat(", but value given to the return statement is ");
      argTypeInfo->getThisType(*desc);
      getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
   }
   return lvids;
}
