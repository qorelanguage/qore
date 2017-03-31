/*
  ReturnStatement.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include <qore/intern/ReturnStatement.h>
#include <qore/intern/qore_program_private.h>

int ReturnStatement::execImpl(QoreValue& return_value, ExceptionSink* xsink) {
   if (exp) {
      return_value = exp->eval(xsink);
      return_value.sanitize();
   }

   if (!*xsink) {
      const QoreTypeInfo* returnTypeInfo = getReturnTypeInfo();
      QoreTypeInfo::acceptAssignment(returnTypeInfo, "<return statement>", return_value, xsink);
   }

   if (*xsink) {
      return_value.discard(xsink);
      return_value.clear();
   }

   return RC_RETURN;
}

int ReturnStatement::parseInitImpl(LocalVar* oflag, int pflag) {
   int lvids = 0;
   const QoreTypeInfo* argTypeInfo = 0;

   if (exp)
      exp = exp->parseInit(oflag, pflag & ~PF_TOP_LEVEL, lvids, argTypeInfo);
   else
      argTypeInfo = nothingTypeInfo;

   const QoreTypeInfo* returnTypeInfo = parse_get_return_type_info();

   //printd(5, "ReturnStatement::parseInitImpl() arg=%s rt=%s\n", argTypeInfo->getTypeName(), returnTypeInfo->getTypeName());

   // check return type and throw a parse exception or warning
   if (!returnTypeInfo->parseAccepts(argTypeInfo)) {
      // check if a warning should be generated, if require-types is not set and it is a class-special method
      const QoreClass *qc = getParseClass();
      const char* fname = get_parse_code();
      if (!parse_check_parse_option(PO_REQUIRE_TYPES) && qc &&
	  (!strcmp(fname, "constructor") || !strcmp(fname, "copy") || !strcmp(fname, "destructor"))) {
	 QoreStringNode* desc = new QoreStringNode;
	 desc->sprintf("the return statement for %s::%s() returns ", qc->getName(), fname);
	 QoreTypeInfo::getThisType(argTypeInfo, *desc);
	 desc->sprintf(", but %s methods may not return any value; this is only a warning when 'require-types' is not set on the Program object; to suppress this warning, remove the expression from the return statement or use '%%disable-warning invalid-operation' in your code", fname);
	 qore_program_private::makeParseWarning(getProgram(), loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
      }
      else {
	 QoreStringNode* desc = new QoreStringNode("return value for this block expects ");
	 QoreTypeInfo::getThisType(returnTypeInfo, *desc);
	 desc->concat(", but value given to the return statement is ");
	 QoreTypeInfo::getThisType(argTypeInfo, *desc);
	 qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", desc);
      }
   }
   else if (QoreTypeInfo::isType(returnTypeInfo, NT_NOTHING) && exp && (!argTypeInfo->hasType() || !QoreTypeInfo::isType(argTypeInfo, NT_NOTHING))) {
      const QoreClass* qc = getParseClass();
      const char* fname = get_parse_code();
      QoreStringNode* desc = new QoreStringNode;
      desc->sprintf("the return statement for %s%s%s() has an expression whose type cannot be resolved at parse time, however the block does not allow any value to be returned; if this expression resolves to a value a run-time error will result; to suppress this warning, move the expression in front of the return statement or use '%%disable-warning invalid-operation' in your code", qc ? qc->getName() : "", qc ? "::" : "", fname);
      qore_program_private::makeParseWarning(getProgram(), loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
   }
   return lvids;
}
