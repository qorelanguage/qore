/*
 IfStatement.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
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
#include <qore/intern/StatementBlock.h>

IfStatement::IfStatement(int start_line, int end_line, AbstractQoreNode *c, class StatementBlock *i, class StatementBlock *e) : AbstractStatement(start_line, end_line)
{
   cond = c;
   if_code = i;
   else_code = e;
   lvars = 0;
}

IfStatement::~IfStatement()
{
   cond->deref(0);
   if (if_code)
      delete if_code;
   if (else_code)
      delete else_code;
   if (lvars)
      delete lvars;
}

// only executed by Statement::exec()
int IfStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink)
{
   int rc = 0;
   
   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);
   
   if (cond->boolEval(xsink))
   {
      if (!xsink->isEvent() && if_code)
	 rc = if_code->execImpl(return_value, xsink);
   }
   else if (else_code)
      rc = else_code->execImpl(return_value, xsink);
   
   return rc;
}

int IfStatement::parseInitImpl(LocalVar *oflag, int pflag)
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
