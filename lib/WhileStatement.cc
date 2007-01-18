/*
 WhileStatement.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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
#include <qore/WhileStatement.h>
#include <qore/Statement.h>
#include <qore/Variable.h>
#include <qore/QoreNode.h>

WhileStatement::WhileStatement(class QoreNode *c, class StatementBlock *cd)
{
   cond = c;
   code = cd;
   lvars = NULL;
}

WhileStatement::~WhileStatement()
{
   cond->deref(NULL);
   if (code)
      delete code;
   if (lvars)
      delete lvars;
}

// only executed by Statement::exec()
int WhileStatement::execWhile(class QoreNode **return_value, class ExceptionSink *xsink)
{
   int i, rc = 0;
   
   tracein("WhileStatement::execWhile()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);
   
   while (cond->boolEval(xsink) && !xsink->isEvent())
   {
      if (code && (((rc = code->exec(return_value, xsink)) == RC_BREAK) || xsink->isEvent()))
      {
	 rc = 0;
	 break;
      }
      if (rc == RC_RETURN)
	 break;
      else if (rc == RC_CONTINUE)
	 rc = 0;
   }
   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("WhileStatement::execWhile()");
   return rc;
}

// only executed by Statement::exec()
int WhileStatement::execDoWhile(class QoreNode **return_value, class ExceptionSink *xsink)
{
   int i, rc = 0;
   
   tracein("WhileStatement::execDoWhile()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);
   
   do
   {
      if (code && (((rc = code->exec(return_value, xsink)) == RC_BREAK) || xsink->isEvent()))
      {
	 rc = 0;
	 break;
      }
      if (rc == RC_RETURN)
	 break;
      else if (rc == RC_CONTINUE)
	 rc = 0;
   } while (cond->boolEval(xsink) && !xsink->isEvent());
   
   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("WhileStatement::execDoWhile()");
   return rc;
}

void WhileStatement::parseWhileInit(lvh_t oflag, int pflag)
{
   int i, lvids = 0;
   
   lvids += process_node(&cond, oflag, pflag);
   if (code)
      code->parseInit(oflag, pflag);
   
   // save local variables
   lvars = new LVList(lvids);
   for (i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();
}

/* do ... while statements can have variables local to the statement
 * however, it doesn't do much good :-) */
void WhileStatement::parseDoWhileInit(lvh_t oflag, int pflag)
{
   int i, lvids = 0;
   
   if (code)
      code->parseInit(oflag, pflag);
   lvids += process_node(&cond, oflag, pflag);
   
   // save local variables
   lvars = new LVList(lvids);
   for (i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();
}
