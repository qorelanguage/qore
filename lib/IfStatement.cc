/*
 IfStatement.cc
 
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

#include <qore/Qore.h>
#include <qore/intern/IfStatement.h>
#include <qore/Variable.h>

IfStatement::IfStatement(int start_line, int end_line, class QoreNode *c, class StatementBlock *i, class StatementBlock *e) : AbstractStatement(start_line, end_line)
{
   cond = c;
   if_code = i;
   else_code = e;
   lvars = NULL;
}

IfStatement::~IfStatement()
{
   cond->deref(NULL);
   if (if_code)
      delete if_code;
   if (else_code)
      delete else_code;
   if (lvars)
      delete lvars;
}

// only executed by Statement::exec()
int IfStatement::execImpl(class QoreNode **return_value, class ExceptionSink *xsink)
{
   int i, rc = 0;
   
   tracein("IfStatement::exec()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);
   
   if (cond->boolEval(xsink))
   {
      if (!xsink->isEvent() && if_code)
	 rc = if_code->execImpl(return_value, xsink);
   }
   else if (else_code)
      rc = else_code->execImpl(return_value, xsink);
   
   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("IfStatement::exec()");
   return rc;
}

int IfStatement::parseInitImpl(lvh_t oflag, int pflag)
{
   int lvids = 0;
   
   lvids += process_node(&cond, oflag, pflag);
   if (if_code)
      if_code->parseInitImpl(oflag, pflag);
   if (else_code)
      else_code->parseInitImpl(oflag, pflag);

   // save local variables
   lvars = new LVList(lvids);

   return 0;
}
