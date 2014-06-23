/*
  ContextStatement.cpp
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2014 David Nichols
 
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
#include <qore/intern/SummarizeStatement.h>
#include <qore/intern/StatementBlock.h>

int SummarizeStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   int rc = 0;
   Context *context;
   AbstractQoreNode *sort = sort_ascending ? sort_ascending : sort_descending;
   int sort_type = sort_ascending ? CM_SORT_ASCENDING : (sort_descending ? CM_SORT_DESCENDING : -1);

   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);
      
   // create the context
   context = new Context(name, xsink, exp, where_exp, sort_type, sort, summarize);
   
   // execute the statements
   if (code) {
      if (context->max_group_pos && !xsink->isEvent())
	 do {
	    if (((rc = code->execImpl(return_value, xsink)) == RC_BREAK) || xsink->isEvent()) {
	       rc = 0;
	       break;
	    }
	    else if (rc == RC_RETURN)
	       break;
	    else if (rc == RC_CONTINUE)
	       rc = 0;
	 }
	    while (!xsink->isEvent() && context->next_summary());
   }
   
   // destroy the context
   context->deref(xsink);
   
   return rc;
}

int SummarizeStatement::parseInitImpl(LocalVar *oflag, int pflag) {
   QORE_TRACE("SummarizeStatement::parseInit()");
   
   int lvids = 0;
   
   // turn off top-level flag for statement vars
   pflag &= (~PF_TOP_LEVEL);

   const QoreTypeInfo *argTypeInfo = 0;

   // initialize context expression
   if (exp)
      exp = exp->parseInit(oflag, pflag, lvids, argTypeInfo);
   
   // need to push something on the stack even if the context is not named
   push_cvar(name);

   if (where_exp) {
      argTypeInfo = 0;
      where_exp = where_exp->parseInit(oflag, pflag, lvids, argTypeInfo);
   }
   if (sort_ascending) {
      argTypeInfo = 0;
      sort_ascending = sort_ascending->parseInit(oflag, pflag, lvids, argTypeInfo);
   }
   if (sort_descending) {
      argTypeInfo = 0;
      sort_descending = sort_descending->parseInit(oflag, pflag, lvids, argTypeInfo);
   }
   if (summarize) {
      argTypeInfo = 0;
      summarize = summarize->parseInit(oflag, pflag, lvids, argTypeInfo);
   }
      
   // initialize statement block
   if (code)
      code->parseInitImpl(oflag, pflag);
   
   // save local variables
   if (lvars)
      lvars = new LVList(lvids);
   
   pop_cvar();

   return 0;
}
