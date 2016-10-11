/*
  WhileStatement.cpp

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
#include <qore/intern/WhileStatement.h>
#include <qore/intern/StatementBlock.h>

WhileStatement::WhileStatement(int start_line, int end_line, AbstractQoreNode *c, class StatementBlock *cd) : AbstractStatement(start_line, end_line) {
   cond = c;
   code = cd;
   lvars = 0;
}

WhileStatement::~WhileStatement() {
   cond->deref(0);
   delete code;
   delete lvars;
}

int WhileStatement::execImpl(QoreValue& return_value, ExceptionSink *xsink) {
   int rc = 0;

   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);

   while (cond->boolEval(xsink) && !xsink->isEvent()) {
      if (code && (((rc = code->execImpl(return_value, xsink)) == RC_BREAK) || xsink->isEvent())) {
	 rc = 0;
	 break;
      }
      if (rc == RC_RETURN)
	 break;
      else if (rc == RC_CONTINUE)
	 rc = 0;
   }

   return rc;
}

int WhileStatement::parseInitImpl(LocalVar *oflag, int pflag) {
   int lvids = 0;

   // turn off top-level flag for statement vars
   pflag &= (~PF_TOP_LEVEL);

   if (cond) {
      const QoreTypeInfo *argTypeInfo = 0;
      cond = cond->parseInit(oflag, pflag, lvids, argTypeInfo);
   }
   if (code)
      code->parseInitImpl(oflag, pflag | PF_BREAK_OK | PF_CONTINUE_OK);

   // save local variables
   if (lvids)
      lvars = new LVList(lvids);

   return 0;
}
