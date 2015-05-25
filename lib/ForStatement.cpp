/*
  ForStatement.cpp
 
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
#include <qore/intern/ForStatement.h>
#include <qore/intern/StatementBlock.h>

ForStatement::ForStatement(int start_line, int end_line, AbstractQoreNode *a, AbstractQoreNode *c, AbstractQoreNode *i, StatementBlock *cd) : AbstractStatement(start_line, end_line), assignment(a), cond(c), iterator(i), code(cd), lvars(0) {
}

ForStatement::~ForStatement() {
   if (assignment)
      assignment->deref(0);
   if (cond)
      cond->deref(0);
   if (iterator)
      iterator->deref(0);
   delete code;
   delete lvars;
}

int ForStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   int rc = 0;
   
   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);
   
   // evaluate assignment expression and discard results if any
   if (assignment)
      assignment->bigIntEval(xsink);
   
   // execute "for" body
   while (!xsink->isEvent()) {
      // check conditional expression, exit "for" loop if condition is
      // false
      if (cond && (!cond->boolEval(xsink) || xsink->isEvent()))
	 break;
      
      // otherwise, execute "for" body
      if (code && (((rc = code->execImpl(return_value, xsink)) == RC_BREAK) || xsink->isEvent())) {
	 rc = 0;
	 break;
      }
      if (rc == RC_RETURN)
	 break;
      else if (rc == RC_CONTINUE)
	 rc = 0;
      
      // evaluate iterator expression and discard results if any
      if (iterator)
	 iterator->bigIntEval(xsink);
   }
   
   return rc;
}

int ForStatement::parseInitImpl(LocalVar *oflag, int pflag) {
   int lvids = 0;

   // turn off top-level flag for statement vars
   pflag &= (~PF_TOP_LEVEL);

   const QoreTypeInfo *argTypeInfo = 0;
   if (assignment) {
      assignment = assignment->parseInit(oflag, pflag | PF_RETURN_VALUE_IGNORED, lvids, argTypeInfo);
       // enable optimizations when return value is ignored for operator expressions
      ignore_return_value(assignment);
   }
   if (cond) {
      argTypeInfo = 0;
      cond = cond->parseInit(oflag, pflag, lvids, argTypeInfo);
   }
   if (iterator) {
      argTypeInfo = 0;
      iterator = iterator->parseInit(oflag, pflag | PF_RETURN_VALUE_IGNORED, lvids, argTypeInfo);
      // enable optimizations when return value is ignored for operator expressions
      ignore_return_value(iterator);
   }
   if (code)
      code->parseInitImpl(oflag, pflag);
   
   // save local variables
   if (lvids)
      lvars = new LVList(lvids);

   return 0;
}
