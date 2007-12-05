/*
 ForEachStatement.cc
 
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
#include <qore/ForEachStatement.h>

ForEachStatement::ForEachStatement(int start_line, int end_line, class QoreNode *v, class QoreNode *l, class StatementBlock *cd) : AbstractStatement(start_line, end_line)
{
   var = v;
   list = l;
   code = cd;
   lvars = NULL;
}

ForEachStatement::~ForEachStatement()
{
   var->deref(NULL);
   list->deref(NULL);
   if (code)
      delete code;
   if (lvars)
      delete lvars;
}

int ForEachStatement::execImpl(class QoreNode **return_value, class ExceptionSink *xsink)
{
   if (list->type == NT_REFERENCE)
      return execRef(return_value, xsink);

   int i, rc = 0;

   tracein("ForEachStatement::exec()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   // get list evaluation (although may be a single node)
   class QoreNode *tlist = list->eval(xsink);
   if (tlist && is_nothing(tlist))
   {
      tlist->deref(xsink);
      tlist = NULL;
   }

   // execute "foreach" body
   if (!xsink->isEvent() && tlist && ((tlist->type != NT_LIST) || tlist->val.list->size()))
   {
      int i = 0;

      while (true)
      {
	 class AutoVLock vl;
	 class QoreNode **n = get_var_value_ptr(var, &vl, xsink);
	 if (xsink->isEvent())
	 {
	    // unlock lock now
	    vl.del();
	    // dereference single value (because it won't be assigned
	    // to the variable and dereferenced later because an 
	    // exception has been thrown)
	    if (tlist->type != NT_LIST)
	       tlist->deref(xsink);
	    break;
	 }

	 // dereference old value of variable
	 if (*n)
	 {
	    (*n)->deref(xsink);
	    if (xsink->isEvent())
	    {
	       (*n) = NULL;
	       // unlock lock now
	       vl.del();
	       // dereference single value (because it won't be assigned
	       // to the variable and dereferenced later because an 
	       // exception has been thrown)
	       if (tlist->type != NT_LIST)
		  tlist->deref(xsink);
	       break;
	    }
	 }

	 // assign variable to current value in list
	 if (tlist->type == NT_LIST)
	    *n = tlist->val.list->eval_entry(i, xsink);
	 else
	    *n = tlist;

	 // unlock variable
	 vl.del();

	 // execute "for" body
	 if (code && (((rc = code->execImpl(return_value, xsink)) == RC_BREAK) || xsink->isEvent()))
	 {
	    rc = 0;
	    break;
	 }

	 if (rc == RC_RETURN)
	    break;
	 else if (rc == RC_CONTINUE)
	    rc = 0;
	 i++;
	 if (tlist->type != NT_LIST)
	    break;
	 if (i == tlist->val.list->size())
	    break;
      }
   }
   // dereference list (but not single values; their reference belongs to the
   // variable assignment
   if (tlist && tlist->type == NT_LIST)
      tlist->deref(xsink);

   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("ForEachStatement::exec()");
   return rc;
}

int ForEachStatement::execRef(class QoreNode **return_value, class ExceptionSink *xsink)
{
   int i, rc = 0;

   tracein("ForEachStatement::execRef()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   // get list evaluation (although may be a single node)
   class QoreNode *tlist, *vr;
   bool is_self_ref = false;
   vr = doPartialEval(list->val.lvexp, &is_self_ref, xsink);
   if (!xsink->isEvent())
   {
      tlist = vr->eval(xsink);
      if (tlist && is_nothing(tlist))
      {
	 tlist->deref(xsink);
	 tlist = NULL;
      }
   }
   else
      tlist = NULL;

   AutoVLock vl;

   // execute "foreach" body
   if (!xsink->isEvent() && tlist && ((tlist->type != NT_LIST) || tlist->val.list->size()))
   {
      class QoreNode *ln = NULL;
      int i = 0;

      if (tlist->type == NT_LIST)
	 ln = new QoreNode(new QoreList());

      while (true)
      {
	 class QoreNode **n = get_var_value_ptr(var, &vl, xsink);
	 if (xsink->isEvent())
	 {
	    // unlock lock now
	    vl.del();
	    // dereference single value (because it won't be assigned
	    // to the variable and dereferenced later because an 
	    // exception has been thrown)
	    if (tlist->type != NT_LIST)
	       tlist->deref(xsink);
	    break;
	 }

	 // dereference old value of variable
	 if (*n)
	 {
	    (*n)->deref(xsink);
	    if (xsink->isEvent())
	    {
	       (*n) = NULL;
	       // unlock lock now
	       vl.del();
	       // dereference single value (because it won't be assigned
	       // to the variable and dereferenced later because an 
	       // exception has been thrown)
	       if (tlist->type != NT_LIST)
		  tlist->deref(xsink);
	       break;
	    }
	 }

	 // assign variable to current value in list
	 if (tlist->type == NT_LIST)
	    *n = tlist->val.list->eval_entry(i, xsink);
	 else
	    *n = tlist;
	 
	 // unlock variable
	 vl.del();
	 
	 // execute "for" body
	 if (code)
	 {
	    rc = code->execImpl(return_value, xsink);

	    // assign value of variable to referenced variable
	    n = get_var_value_ptr(var, &vl, xsink);
	    if (xsink->isEvent())
	    {
	       // unlock lock now
	       vl.del();
	       break;
	    }

	    QoreNode *nv;
	    if (*n)
	       nv = (*n)->eval(xsink);
	    else
	       nv = NULL;

	    // assign new value to referenced variable
	    if (tlist->type == NT_LIST)
	       ln->val.list->set_entry(i, nv, NULL);
	    else
	       ln = nv;

	    vl.del();
	 }

	 if (!code || xsink->isEvent() || rc == RC_BREAK)
	 {
	    rc = 0;
	    break;
	 }

	 if (rc == RC_RETURN)
	    break;
	 else if (rc == RC_CONTINUE)
	    rc = 0;
	 i++;

	 // break out of loop if appropriate
	 if (tlist->type != NT_LIST || i == tlist->val.list->size())
	    break;
      }

      if (!xsink->isEvent())
      {
	 // write the value back to the lvalue
	 QoreNode **val = get_var_value_ptr(vr, &vl, xsink);
	 if (!xsink->isEvent())
	 {
	    discard(*val, xsink);
	    *val = ln;
	    vl.del();
	 }
	 else
	 {
	    vl.del();
	    discard(ln, xsink);
	 }
      }
   }

    // dereference list (but not single values; their reference belongs to the
   // variable assignment
   if (tlist && tlist->type == NT_LIST)
      tlist->deref(xsink);

   // dereference partial evaluation for lvalue assignment
   if (vr)
      vr->deref(xsink);
   
   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);

   traceout("ForEachStatement::execRef()");
   return rc;
}

int ForEachStatement::parseInitImpl(lvh_t oflag, int pflag)
{
   int lvids = 0;
   
   lvids += process_node(&var, oflag, pflag);
   lvids += process_node(&list, oflag, pflag | PF_REFERENCE_OK);
   if (code)
      code->parseInitImpl(oflag, pflag);
   
   // save local variables 
   lvars = new LVList(lvids);

   return 0;
}
