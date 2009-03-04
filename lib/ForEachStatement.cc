/*
 ForEachStatement.cc
 
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
#include <qore/intern/ForEachStatement.h>
#include <qore/intern/StatementBlock.h>

ForEachStatement::ForEachStatement(int start_line, int end_line, AbstractQoreNode *v, AbstractQoreNode *l, class StatementBlock *cd) : AbstractStatement(start_line, end_line), var(v), list(l), code(cd), lvars(0)
{
}

ForEachStatement::~ForEachStatement()
{
   if (var)
      var->deref(0);
   if (list)
      list->deref(0);
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
   ReferenceHolder<AbstractQoreNode> tlist(list->eval(xsink), xsink);
   if (!code || *xsink || is_nothing(*tlist))
      return 0;

   QoreListNode *l_tlist = tlist->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>(*tlist) : 0;
   if (l_tlist && l_tlist->empty())
      return 0;

   // execute "foreach" body
   unsigned i = 0;

   while (true) {
      {
	 LValueHelper n(var, xsink);
	 if (!n)
	    break;
	 
	 // assign variable to current value in list
	 if (n.assign(l_tlist ? l_tlist->get_referenced_entry(i) : tlist.release()))
	    break;
      }
      
      // execute "foreach" body
      if (((rc = code->execImpl(return_value, xsink)) == RC_BREAK) || *xsink) {
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

   return rc;
}

int ForEachStatement::execRef(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   int rc = 0;

   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);

   // get list evaluation (although may be a single node)
   bool is_self_ref = false;

   ReferenceNode *r = reinterpret_cast<ReferenceNode *>(list);

   // here we do a "doPartialEval()" to evaluate all parts of the expression not related to the lvalue so
   // that these parts will only be executed once (and not again when this lvalue is actually assigned)
   ReferenceHolder<AbstractQoreNode> vr(doPartialEval(r->getExpression(), &is_self_ref, xsink), xsink);
   if (*xsink)
      return 0;

   // get the current value of the lvalue expression
   ReferenceHolder<AbstractQoreNode> tlist(vr->eval(xsink), xsink);
   if (!code || *xsink || is_nothing(*tlist))
      return 0;

   QoreListNode *l_tlist = tlist->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>(*tlist) : 0;
   if (l_tlist && l_tlist->empty())
      return 0;

   // execute "foreach" body
   ReferenceHolder<AbstractQoreNode> ln(0, xsink);
   unsigned i = 0;

   if (l_tlist)
      ln = new QoreListNode();

   while (true) {
      {
	 LValueHelper n(var, xsink);
	 if (!n)
	    return 0;

	 // assign variable to current value in list
	 if (n.assign(l_tlist ? l_tlist->get_referenced_entry(i) : tlist.release()))
	    return 0;
      }
      
      // execute "for" body
      rc = code->execImpl(return_value, xsink);
      if (*xsink)
	 return 0;

      // get value of foreach variable
      AbstractQoreNode *nv = var->eval(xsink);
      if (*xsink)
	 return 0;

      // assign new value to temporary variable for later assignment to referenced lvalue
      if (l_tlist)
	 reinterpret_cast<QoreListNode *>(*ln)->push(nv);
      else
	 ln = nv;
      
      if (rc == RC_BREAK) {
	 // assign remaining values to list unchanged
	 if (l_tlist)
	    while (++i < l_tlist->size())
	       reinterpret_cast<QoreListNode *>(*ln)->push(l_tlist->get_referenced_entry(i));

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

   // write the value back to the lvalue
   LValueHelper val(*vr, xsink);
   if (!val)
      return 0;
   
   if (val.assign(ln.release()))
      return 0;

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
