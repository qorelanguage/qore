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
#include <qore/intern/ForEachStatement.h>
#include <qore/intern/StatementBlock.h>

ForEachStatement::ForEachStatement(int start_line, int end_line, AbstractQoreNode *v, AbstractQoreNode *l, class StatementBlock *cd) : AbstractStatement(start_line, end_line)
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

int ForEachStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink)
{
   if (is_ref)
      return execRef(return_value, xsink);

   int rc = 0;

   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);

   // get list evaluation (although may be a single node)
   AbstractQoreNode *tlist = list->eval(xsink);
   if (tlist && is_nothing(tlist))
   {
      tlist->deref(xsink);
      tlist = NULL;
   }
   
   QoreListNode *l_tlist = tlist && tlist->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>(tlist) : 0;

   // execute "foreach" body
   if (!xsink->isEvent() && tlist && (!l_tlist || l_tlist->size()))
   {
      int i = 0;

      while (true)
      {
	 class AutoVLock vl;
	 AbstractQoreNode **n = get_var_value_ptr(var, &vl, xsink);
	 if (xsink->isEvent())
	 {
	    // unlock lock now
	    vl.del();
	    // dereference single value (because it won't be assigned
	    // to the variable and dereferenced later because an 
	    // exception has been thrown)
	    if (tlist->getType() != NT_LIST)
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
	       if (tlist->getType() != NT_LIST)
		  tlist->deref(xsink);
	       break;
	    }
	 }

	 // assign variable to current value in list
	 if (l_tlist)
	    *n = l_tlist->eval_entry(i, xsink);
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
	 // if the argument is not a list or list iteration is done, then break
	 if (!l_tlist || i == l_tlist->size())
	    break;
      }
   }
   // dereference list (but not single values; their reference belongs to the variable assignment
   if (l_tlist)
      tlist->deref(xsink);

   return rc;
}

int ForEachStatement::execRef(AbstractQoreNode **return_value, ExceptionSink *xsink)
{
   int rc = 0;

   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);

   // get list evaluation (although may be a single node)
   AbstractQoreNode *tlist, *vr;
   bool is_self_ref = false;

   ReferenceNode *r = reinterpret_cast<ReferenceNode *>(list);
   vr = doPartialEval(r->lvexp, &is_self_ref, xsink);
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

   QoreListNode *l_tlist = tlist && tlist->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>(tlist) : 0;

   AutoVLock vl;

   // execute "foreach" body
   if (!xsink->isEvent() && tlist && (!l_tlist || l_tlist->size()))
   {
      AbstractQoreNode *ln = NULL;
      int i = 0;

      if (l_tlist)
	 ln = new QoreListNode();

      while (true)
      {
	 AbstractQoreNode **n = get_var_value_ptr(var, &vl, xsink);
	 if (xsink->isEvent())
	 {
	    // unlock lock now
	    vl.del();
	    // dereference single value (because it won't be assigned
	    // to the variable and dereferenced later because an 
	    // exception has been thrown)
	    if (!l_tlist)
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
	       if (!l_tlist)
		  tlist->deref(xsink);
	       break;
	    }
	 }

	 // assign variable to current value in list
	 if (l_tlist)
	    *n = l_tlist->eval_entry(i, xsink);
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

	    AbstractQoreNode *nv;
	    if (*n)
	       nv = (*n)->eval(xsink);
	    else
	       nv = NULL;

	    // assign new value to referenced variable
	    if (l_tlist)
	       (reinterpret_cast<QoreListNode *>(ln))->set_entry(i, nv, NULL);
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
	 if (!l_tlist || i == l_tlist->size())
	    break;
      }

      if (!xsink->isEvent())
      {
	 // write the value back to the lvalue
	 AbstractQoreNode **val = get_var_value_ptr(vr, &vl, xsink);
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
   if (l_tlist)
      tlist->deref(xsink);

   // dereference partial evaluation for lvalue assignment
   if (vr)
      vr->deref(xsink);
   
   return rc;
}

int ForEachStatement::parseInitImpl(LocalVar *oflag, int pflag)
{
   int lvids = 0;
   
   lvids += process_node(&var, oflag, pflag);
   lvids += process_node(&list, oflag, pflag | PF_REFERENCE_OK);
   if (code)
      code->parseInitImpl(oflag, pflag);
   
   // save local variables 
   lvars = new LVList(lvids);

   is_ref = (list->getType() == NT_REFERENCE);

   return 0;
}
