/*
 ForStatement.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006 David Nichols
 
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
#include <qore/ForStatement.h>
#include <qore/Statement.h>
#include <qore/Variable.h>
#include <qore/QoreNode.h>

ForStatement::ForStatement(class QoreNode *a, class QoreNode *c, class QoreNode *i, class StatementBlock *cd)
{
   assignment = a;
   cond = c;
   iterator = i;
   code = cd;
   lvars = NULL;
}

ForStatement::~ForStatement()
{
   if (assignment)
      assignment->deref(NULL);
   if (cond)
      cond->deref(NULL);
   if (iterator)
      iterator->deref(NULL);
   if (code)
      delete code;
   if (lvars)
      delete lvars;
}

// only executed by Statement::exec()
int ForStatement::exec(class QoreNode **return_value, class ExceptionSink *xsink)
{
   int i, rc = 0;
   
   tracein("ForStatement::exec()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);
   
   // evaluate assignment expression and discard results if any
   if (assignment)
      discard(assignment->eval(xsink), xsink);
   
   // execute "for" body
   while (!xsink->isEvent())
   {
      // check conditional expression, exit "for" loop if condition is
      // false
      if (cond && (!cond->boolEval(xsink) || xsink->isEvent()))
	 break;
      
      // otherwise, execute "for" body
      if (code && (((rc = code->exec(return_value, xsink)) == RC_BREAK) || xsink->isEvent()))
      {
	 rc = 0;
	 break;
      }
      if (rc == RC_RETURN)
	 break;
      else if (rc == RC_CONTINUE)
	 rc = 0;
      
      // evaluate iterator expression and discard results if any
      if (iterator)
	 discard(iterator->eval(xsink), xsink);
   }
   
   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("ForStatement::exec()");
   return rc;
}

void ForStatement::parseInit(lvh_t oflag, int pflag)
{
   int i, lvids = 0;
   
   if (assignment)
   {
      lvids += process_node(&assignment, oflag, pflag);
      // enable optimizations when return value is ignored for operator expressions
      if (assignment->type == NT_TREE)
	 assignment->val.tree->ignoreReturnValue();
   }
   if (cond)
      lvids += process_node(&cond, oflag, pflag);
   if (iterator)
   {
      lvids += process_node(&iterator, oflag, pflag);
      // enable optimizations when return value is ignored for operator expressions
      if (iterator->type == NT_TREE)
	 iterator->val.tree->ignoreReturnValue();
   }
   if (code)
      code->parseInit(oflag, pflag);
   
   // save local variables
   lvars = new LVList(lvids);
   for (i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();
}

