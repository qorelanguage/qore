/*
 ForStatement.cc
 
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
#include <qore/intern/ForStatement.h>
#include <qore/intern/StatementBlock.h>

ForStatement::ForStatement(int start_line, int end_line, AbstractQoreNode *a, AbstractQoreNode *c, AbstractQoreNode *i, class StatementBlock *cd) : AbstractStatement(start_line, end_line)
{
   assignment = a;
   cond = c;
   iterator = i;
   code = cd;
   lvars = 0;
}

ForStatement::~ForStatement()
{
   if (assignment)
      assignment->deref(0);
   if (cond)
      cond->deref(0);
   if (iterator)
      iterator->deref(0);
   if (code)
      delete code;
   if (lvars)
      delete lvars;
}

int ForStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink)
{
   int rc = 0;
   
   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);
   
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
      if (code && (((rc = code->execImpl(return_value, xsink)) == RC_BREAK) || xsink->isEvent()))
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
   
   return rc;
}

int ForStatement::parseInitImpl(LocalVar *oflag, int pflag)
{
   int lvids = 0;
   
   if (assignment)
   {
      lvids += process_node(&assignment, oflag, pflag);
      // enable optimizations when return value is ignored for operator expressions
      QoreTreeNode *tree = dynamic_cast<QoreTreeNode *>(assignment);
      if (tree)
	 tree->ignoreReturnValue();
   }
   if (cond)
      lvids += process_node(&cond, oflag, pflag);
   if (iterator)
   {
      lvids += process_node(&iterator, oflag, pflag);
      // enable optimizations when return value is ignored for operator expressions

      QoreTreeNode *tree = dynamic_cast<QoreTreeNode *>(iterator);
      if (tree)
	 tree->ignoreReturnValue();
   }
   if (code)
      code->parseInitImpl(oflag, pflag);
   
   // save local variables
   lvars = new LVList(lvids);

   return 0;
}

