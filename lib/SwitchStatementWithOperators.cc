/*
  SwitchStatementWithOperators.cc

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QoreNode.h>
#include <qore/Statement.h>
#include <qore/SwitchStatementWithOperators.h>
#include <qore/minitest.hpp>
#include <qore/Operator.h>

#ifdef DEBUG
#  include "tests/SwitchStatementWithOperators_tests.cc"
#endif

CaseNode::CaseNode(class QoreNode *v, class StatementBlock *c)
{
   val = v;
   code = c;
   next = NULL;
}

CaseNode::~CaseNode()
{
   if (val)
      val->deref(NULL);
   if (code)
      delete code;
}

bool CaseNode::isCaseNodeImpl() const
{
   return true;
}

bool CaseNode::matches(QoreNode* lhs_value, class ExceptionSink *xsink) {
   return !compareHard(lhs_value, val); // the ! is because of compareHard() semantics
}

bool CaseNode::isCaseNode() const
{
   return isCaseNodeImpl();
}

SwitchStatement::SwitchStatement(class CaseNode *f)
{
   deflt = NULL;
   head = tail = f;
   sexp = NULL;
   lvars = NULL;
}

SwitchStatement::~SwitchStatement()
{
   while (head)
   {
      class CaseNode *w = head->next;
      delete head;
      head = w;
   }
   if (sexp)
      sexp->deref(NULL);
   if (lvars)
      delete lvars;
}

void SwitchStatement::setSwitch(class QoreNode *s)
{
   sexp = s;
}

void SwitchStatement::addCase(class CaseNode *c)
{
   if (tail)
      tail->next = c;
   else
      head = c;
   tail = c;
   if (!c->val)
   {
      if (deflt)
	 parse_error("multiple defaults in switch statement");
      deflt = c;
   }
}

// only executed by Statement::exec()
int SwitchStatement::exec(class QoreNode **return_value, class ExceptionSink *xsink)
{
   int i, rc = 0;
   
   tracein("SwitchStatement::exec()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);
   
   class QoreNode *se = sexp->eval(xsink);
   if (!xsink->isEvent())
   {
      // find match
      class CaseNode *w = head;
      while (w)
      {
	 if (w->matches(se, xsink))
	    break;
	 w = w->next;
      }
      if (!w && deflt)
	 w = deflt;
      
      while (w && !rc && !xsink->isEvent())
      {
	 if (w->code)
	    rc = w->code->exec(return_value, xsink);
	 
	 w = w->next;
      }
      if (rc == RC_BREAK || rc == RC_CONTINUE)
	 rc = 0;
   }
   
   if (se)
      se->deref(xsink);
   
   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   
   traceout("SwitchStatement::exec()");
   return rc;
}

void SwitchStatement::parseInit(lvh_t oflag, int pflag)
{
   int i, lvids = 0;
   
   lvids += process_node(&sexp, oflag, pflag);
   
   class CaseNode *w = head;
   while (w)
   {
      if (w->val)
      {
	 getRootNS()->parseInitConstantValue(&w->val, 0);
	 
	 // check for duplicate values
	 class CaseNode *cw = head;
	 while (cw != w)
	 {
            // Check only the simple case blocks (case 1: ...),
            // not those with relational operators. Could be changed later to provide more checking.
            if (w->isCaseNode() && cw->isCaseNode() && !compareHard(w->val, cw->val))
	       parse_error("duplicate case values in switch");
	    cw = cw->next;
	 }
      }
      
      if (w->code)
	 w->code->parseInit(oflag, pflag);
      w = w->next;
   }
   
   // save local variables
   lvars = new LVList(lvids);
   for (i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();
}

bool CaseNodeWithOperator::isCaseNodeImpl() const
{
  return false;
}

//-----------------------------------------------------------------------------
bool CaseNodeWithOperator::matches(QoreNode* lhs_value, class ExceptionSink *xsink)
{
   return m_operator->bool_eval(lhs_value, val, xsink);
}

// EOF

