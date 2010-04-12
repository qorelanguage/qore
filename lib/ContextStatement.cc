/*
 ContextStatement.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2010 David Nichols
 
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
#include <qore/intern/ContextStatement.h>
#include <qore/intern/StatementBlock.h>

ContextMod::ContextMod(int t, AbstractQoreNode *n) {
   type = t;
   c.exp = n;
}

ContextMod::~ContextMod() {
   if (c.exp)
      c.exp->deref(0);
}

ContextModList::ContextModList(ContextMod *cm) {
   push_back(cm);
}

ContextModList::~ContextModList() {
   cxtmod_list_t::iterator i;
   while ((i = begin()) != end()) {
      //printd(5, "CML::~CML() %d (%08p)\n", (*i)->getType(), (*i)->c.exp);
      if (*i)
	 delete *i;
      erase(i);
   }
}

void ContextModList::addContextMod(ContextMod *cm) {
   push_back(cm);
   //printd(5, "CML::CML() %d (%08p)\n", cm->getType(), cm->c.exp);
}

ContextStatement::ContextStatement(int start_line, int end_line, char *n, AbstractQoreNode *expr, class ContextModList *mods, class StatementBlock *cd) : AbstractStatement(start_line, end_line) {
   name = n;
   exp = expr;
   code = cd;
   lvars = 0;
   where_exp = sort_ascending = sort_descending = 0;
   if (mods)
   {
      for (cxtmod_list_t::iterator i = mods->begin(); i != mods->end(); i++)
      {
	 switch ((*i)->type)
	 {
	    case CM_WHERE_NODE:
	       if (!where_exp)
	       {
		  where_exp = (*i)->c.exp;
		  (*i)->c.exp = 0;
	       }
	       else
		  parseException("CONTEXT-PARSE-ERROR", "multiple where conditions found for context statement!");
	       break;
	    case CM_SORT_ASCENDING:
	       if (!sort_ascending && !sort_descending)
	       {
		  sort_ascending = (*i)->c.exp;
		  (*i)->c.exp = 0;
	       }
	       else
		  parseException("CONTEXT-PARSE-ERROR", "multiple sort conditions found for context statement!");
	       break;
	    case CM_SORT_DESCENDING:
	       if (!sort_descending && !sort_ascending)
	       {
		  sort_descending = (*i)->c.exp;
		  (*i)->c.exp = 0;
	       }
	       else
		  parseException("CONTEXT-PARSE-ERROR", "multiple sort conditions found for context statement!");
	       break;
	 }
      }
      delete mods;
   }
}

ContextStatement::~ContextStatement()
{
   if (name)
      free(name);
   if (exp)
      exp->deref(0);
   if (code)
      delete code;
   if (lvars)
      delete lvars;
   if (where_exp)
      where_exp->deref(0);
   if (sort_ascending)
      sort_ascending->deref(0);
   if (sort_descending)
      sort_descending->deref(0);
}

// FIXME: local vars should only be instantiated if there is a non-null context
int ContextStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink)
{
   int rc = 0;
   AbstractQoreNode *sort = sort_ascending ? sort_ascending : sort_descending;
   int sort_type = sort_ascending ? CM_SORT_ASCENDING : (sort_descending ? CM_SORT_DESCENDING : -1);
      
   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);
   
   // create the context
   ReferenceHolder<Context> context(new Context(name, xsink, exp, where_exp, sort_type, sort, 0), xsink);
   if (*xsink || !code)
      return rc;

   // execute the statements
   for (context->pos = 0; context->pos < context->max_pos && !xsink->isEvent(); context->pos++) {
      printd(4, "ContextStatement::exec() iteration %d/%d\n", context->pos, context->max_pos);
      if (((rc = code->execImpl(return_value, xsink)) == RC_BREAK) || *xsink) {
	 rc = 0;
	 break;
      }
      else if (rc == RC_RETURN)
	 break;
      else if (rc == RC_CONTINUE)
	 rc = 0;
   }

   return rc;   
}

int ContextStatement::parseInitImpl(LocalVar *oflag, int pflag) {
   QORE_TRACE("ContextStatement::parseInitImpl()");
   
   int lvids = 0;
   
   if (!exp && !getCVarStack())
      parse_error("subcontext statement out of context");
   
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
      
   // initialize statement block
   if (code)
      code->parseInitImpl(oflag, pflag);
   
   // save local variables
   lvars = new LVList(lvids);
   
   pop_cvar();

   return 0;
}
