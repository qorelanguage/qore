/*
  ContextStatement.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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
      //printd(5, "CML::~CML() %d (%p)\n", (*i)->getType(), (*i)->c.exp);
      if (*i)
	 delete *i;
      erase(i);
   }
}

void ContextModList::addContextMod(ContextMod *cm) {
   push_back(cm);
   //printd(5, "CML::CML() %d (%p)\n", cm->getType(), cm->c.exp);
}

ContextStatement::ContextStatement(int start_line, int end_line, char *n, AbstractQoreNode *expr, class ContextModList *mods, class StatementBlock *cd) : AbstractStatement(start_line, end_line) {
   name = n;
   exp = expr;
   code = cd;
   lvars = 0;
   where_exp = sort_ascending = sort_descending = 0;
   if (mods) {
      for (cxtmod_list_t::iterator i = mods->begin(); i != mods->end(); i++) {
	 switch ((*i)->type) {
	    case CM_WHERE_NODE:
	       if (!where_exp) {
		  where_exp = (*i)->c.exp;
		  (*i)->c.exp = 0;
	       }
	       else
		  parseException("CONTEXT-PARSE-ERROR", "multiple where conditions found for context statement!");
	       break;
	    case CM_SORT_ASCENDING:
	       if (!sort_ascending && !sort_descending) {
		  sort_ascending = (*i)->c.exp;
		  (*i)->c.exp = 0;
	       }
	       else
		  parseException("CONTEXT-PARSE-ERROR", "multiple sort conditions found for context statement!");
	       break;
	    case CM_SORT_DESCENDING:
	       if (!sort_descending && !sort_ascending) {
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

ContextStatement::~ContextStatement() {
   if (name)
      free(name);
   if (exp)
      exp->deref(0);
   delete code;
   delete lvars;
   if (where_exp)
      where_exp->deref(0);
   if (sort_ascending)
      sort_ascending->deref(0);
   if (sort_descending)
      sort_descending->deref(0);
}

// FIXME: local vars should only be instantiated if there is a non-null context
int ContextStatement::execImpl(QoreValue& return_value, ExceptionSink *xsink) {
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

   // turn off top-level flag for statement vars
   pflag &= (~PF_TOP_LEVEL);

   if (!exp && !getCVarStack())
      parse_error("subcontext statement out of context");

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

   // initialize statement block
   if (code)
      code->parseInitImpl(oflag, pflag);

   // save local variables
   if (lvids)
      lvars = new LVList(lvids);

   pop_cvar();

   return 0;
}
