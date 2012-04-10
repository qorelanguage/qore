/*
 ReturnStatement.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2012 David Nichols
 
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
#include <qore/intern/qore_program_private.h>

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

   if (!*xsink) {
      const QoreTypeInfo *returnTypeInfo = getReturnTypeInfo();
      *return_value = returnTypeInfo->acceptAssignment("<return statement>", *return_value, xsink);
   }

   if (*xsink) {
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
      argTypeInfo = nothingTypeInfo;

   const QoreTypeInfo *returnTypeInfo = getReturnTypeInfo();

   //printd(5, "ReturnStatement::parseInitImpl() arg=%s rt=%s\n", argTypeInfo->getTypeName(), returnTypeInfo->getTypeName());

   // check return type and throw a parse exception or warning
   if (!returnTypeInfo->parseAccepts(argTypeInfo)) {
      // check if a warning should be generated, if require-types is not set and it is a class-special method
      const QoreClass *qc = getParseClass();
      const char *fname = get_parse_code();
      if (!(getProgram()->getParseOptions64() & PO_REQUIRE_TYPES) && qc && 
	  (!strcmp(fname, "constructor") || !strcmp(fname, "copy") || !strcmp(fname, "destructor"))) {
	 QoreStringNode *desc = new QoreStringNode;
	 desc->sprintf("the return statement for %s::%s() returns ", qc->getName(), fname);
	 argTypeInfo->getThisType(*desc);
	 desc->sprintf(", but %s methods may not return any value; this is only a warning when 'require-types' is not set on the Program object; to suppress this warning, remove the expression from the return statement or use '%%disable-warning invalid-operation' in your code", fname);
	 qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
      }
      else {
	 QoreStringNode *desc = new QoreStringNode("return value for this block expects ");
	 returnTypeInfo->getThisType(*desc);
	 desc->concat(", but value given to the return statement is ");
	 argTypeInfo->getThisType(*desc);
	 qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", desc);
      }
   }
   else if (returnTypeInfo->isType(NT_NOTHING) && exp && (!argTypeInfo->hasType() || !argTypeInfo->isType(NT_NOTHING))) {
      const QoreClass *qc = getParseClass();
      const char *fname = get_parse_code();
      QoreStringNode *desc = new QoreStringNode;
      desc->sprintf("the return statement for %s%s%s() has an expression whose type cannot be resolved at parse time, however the block does not allow any value to be returned; if this expression resolves to a value a run-time error will result; to suppress this warning, move the expression in front of the return statement or use '%%disable-warning invalid-operation' in your code", qc ? qc->getName() : "", qc ? "::" : "", fname);
      qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
   }
   return lvids;
}
