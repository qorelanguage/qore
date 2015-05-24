/*
  SwitchStatement.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2015 Qore Technologies

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
#include <qore/intern/SwitchStatement.h>
#include <qore/intern/StatementBlock.h>
#include <qore/intern/CaseNodeWithOperator.h>
#include <qore/intern/CaseNodeRegex.h>
#include <qore/intern/qore_program_private.h>
#include <qore/minitest.hpp>

#ifdef DEBUG_TESTS
#  include "tests/SwitchStatementWithOperators_tests.cpp"
#endif

CaseNode::CaseNode(AbstractQoreNode* v, StatementBlock* c) {
   val = v;
   code = c;
   next = 0;
}

CaseNode::~CaseNode() {
   if (val)
      val->deref(0);
   if (code)
      delete code;
}

bool CaseNode::isCaseNodeImpl() const {
   return true;
}

bool CaseNode::matches(AbstractQoreNode *lhs_value, ExceptionSink *xsink) {
   return !compareHard(lhs_value, val, xsink); // the ! is because of compareHard() semantics
}

bool CaseNode::isCaseNode() const {
   return isCaseNodeImpl();
}

// start and end line are set later
SwitchStatement::SwitchStatement(CaseNode *f) : AbstractStatement(-1, -1), head(f), tail(f), sexp(0), deflt(f->isDefault() ? f : 0), lvars(0) {
}

SwitchStatement::~SwitchStatement() {
   while (head) {
      CaseNode *w = head->next;
      delete head;
      head = w;
   }
   if (sexp)
      sexp->deref(0);
   delete lvars;
}

void SwitchStatement::setSwitch(AbstractQoreNode *s) {
   sexp = s;
}

void SwitchStatement::addCase(CaseNode *c) {
   if (tail)
      tail->next = c;
   else
      head = c;
   tail = c;
   if (c->isDefault()) {
      if (deflt)
	 parse_error("multiple defaults in switch statement");
      deflt = c;
   }
}

int SwitchStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   int rc = 0;
   
   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);
   
   AbstractQoreNode *se = sexp->eval(xsink);
   if (!xsink->isEvent()) {
      // find match
      CaseNode *w = head;
      while (w) {
	 if (w->matches(se, xsink))
	    break;
	 w = w->next;
      }
      if (!w && deflt)
	 w = deflt;
      
      while (w && !rc && !xsink->isEvent()) {
	 if (w->code)
	    rc = w->code->execImpl(return_value, xsink);
	 
	 w = w->next;
      }
      if (rc == RC_BREAK || rc == RC_CONTINUE)
	 rc = 0;
   }
   
   if (se)
      se->deref(xsink);
   
   return rc;
}

int SwitchStatement::parseInitImpl(LocalVar *oflag, int pflag) {
   int lvids = 0;
   
   // turn off top-level flag for statement vars
   pflag &= (~PF_TOP_LEVEL);

   const QoreTypeInfo *argTypeInfo = 0;

   if (sexp)
      sexp = sexp->parseInit(oflag, pflag, lvids, argTypeInfo);
   
   CaseNode *w = head;
   ExceptionSink xsink;
   QoreProgram *pgm = getProgram();
   while (w) {
      if (w->val) {
         argTypeInfo = 0;
         w->val = w->val->parseInit(oflag, pflag | PF_CONST_EXPRESSION, lvids, argTypeInfo);
	 if (lvids) {
	    parse_error("illegal local variable declaration in assignment expression for case block");
	    while (lvids--)
	       pop_local_var();

	    w = w->next;
	    continue;
	 }

	 // evaluate case expression if necessary and no parse expressions have been raised
	 if (w->val && !w->val->is_value()) {
	    if (pgm->parseExceptionRaised()) {
	       w = w->next;
	       continue;
	    }

	    ReferenceHolder<AbstractQoreNode> v(w->val->eval(&xsink), &xsink);
	    if (!xsink) {
	       w->val->deref(&xsink);
	       w->val = v.release();
	       if (!w->val)
		  w->val = nothing();
	    }
	    else
	       qore_program_private::addParseException(pgm, xsink);
	 }
	 //printd(5, "SwitchStatement::parseInit() this=%p case exp: %p %s\n", this, w->val, get_type_name(w->val));

	 // check for duplicate values
	 CaseNode *cw = head;
	 while (cw != w) {    
            // Check only the simple case blocks (case 1: ...),
            // not those with relational operators. Could be changed later to provide more checking.
	    // note that no exception can be raised here as the case node values are parse values
            if (w->isCaseNode() && cw->isCaseNode() && !compareHard(w->val, cw->val, &xsink))
	       parse_error("duplicate case values in switch");
	    assert(!xsink);
	    cw = cw->next;
	 }
      }
      
      if (w->code)
	 w->code->parseInitImpl(oflag, pflag);
      w = w->next;
   }
   
   // save local variables
   if (lvids)
      lvars = new LVList(lvids);

   return 0;
}

CaseNodeWithOperator::CaseNodeWithOperator(AbstractQoreNode* v, StatementBlock* c, Operator* op) : CaseNode(v, c), m_operator(op) {
}

bool CaseNodeWithOperator::isCaseNodeImpl() const {
   return false;
}

bool CaseNodeWithOperator::matches(AbstractQoreNode* lhs_value, ExceptionSink *xsink) {
   ValueHolder rv(m_operator->eval(lhs_value, val, true, xsink), xsink);
   return rv->getAsBool();
}

CaseNodeRegex::CaseNodeRegex(QoreRegexNode *m_re, StatementBlock *blk) : CaseNode(NULL, blk), re(m_re) {
}

bool CaseNodeRegex::matches(AbstractQoreNode *lhs_value, ExceptionSink *xsink) {
   QoreStringValueHelper str(lhs_value);
   
   return re->exec(*str, xsink);
}

bool CaseNodeNegRegex::matches(AbstractQoreNode *lhs_value, ExceptionSink *xsink) {
   QoreStringValueHelper str(lhs_value);
   
   return !re->exec(*str, xsink);
}

