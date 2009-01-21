/*
 ContextStatement.cc
 
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
#include <qore/intern/SummarizeStatement.h>
#include <qore/intern/StatementBlock.h>

int SummarizeStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink)
{
   int rc = 0;
   class Context *context;
   AbstractQoreNode *sort = sort_ascending ? sort_ascending : sort_descending;
   int sort_type = sort_ascending ? CM_SORT_ASCENDING : (sort_descending ? CM_SORT_DESCENDING : -1);

   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);
      
   // create the context
   context = new Context(name, xsink, exp, where_exp, sort_type, sort, summarize);
   
   // execute the statements
   if (code)
   {
      if (context->max_group_pos && !xsink->isEvent())
	 do
	 {
	    if (((rc = code->execImpl(return_value, xsink)) == RC_BREAK) || xsink->isEvent())
	    {
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

int SummarizeStatement::parseInitImpl(LocalVar *oflag, int pflag)
{
   QORE_TRACE("SummarizeStatement::parseInit()");
   
   int lvids = 0;
   
   // initialize context expression
   if (exp)
      lvids += process_node(&exp, oflag, pflag);
   
   // need to push something on the stack even if the context is not named
   push_cvar(name);

   if (where_exp)
      process_node(&where_exp, oflag, pflag);
   if (sort_ascending)
      process_node(&sort_ascending, oflag, pflag);
   if (sort_descending)
      process_node(&sort_descending, oflag, pflag);
   if (summarize)
      process_node(&summarize, oflag, pflag);
      
   // initialize statement block
   if (code)
      code->parseInitImpl(oflag, pflag);
   
   // save local variables
   lvars = new LVList(lvids);
   
   pop_cvar();

   return 0;
}
