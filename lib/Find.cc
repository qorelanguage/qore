/*
 Find.cc
 
 Qore Programming Language
 
 Copyright (C) David Nichols 2003, 2004, 2005, 2006
 
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
#include <qore/Find.h>
#include <qore/Exception.h>
#include <qore/QoreNode.h>
#include <qore/Variable.h>
#include <qore/support.h>
#include <qore/Context.h>
#include <qore/List.h>

Find::Find(class QoreNode *expr, class QoreNode *find_expr, class QoreNode *w)
{
   exp = expr;
   find_exp = find_expr;
   where = w;
}

Find::~Find()
{
   if (find_exp)
      find_exp->deref(NULL);
   if (exp)
      exp->deref(NULL);
   if (where)
      where->deref(NULL);
}

class QoreNode *Find::eval(ExceptionSink *xsink)
{
   class Context *context;
   class QoreNode *rv = NULL;
   
   context = new Context(NULL, xsink, find_exp);
   if (xsink->isEvent())
   {
      if (context)
	 context->deref(xsink);
      return NULL;
   }
   
   int multi = 0;
   for (context->pos = 0; context->pos < context->max_pos && !xsink->isEvent(); context->pos++)
   {
      printd(4, "Find::eval() checking %d/%d\n", context->pos, context->max_pos);
      if (context->check_condition(where, xsink) && !xsink->isEvent())
      {
	 printd(4, "Find::eval() GOT IT: %d\n", context->pos);
	 QoreNode *result = exp->eval(xsink);
	 if (rv)
	 {
	    if (!multi)
	    {
	       List *l = new List();
	       l->push(rv);
	       l->push(result);
	       rv = new QoreNode(l);
	       multi = 1;
	    }
	    else
	       rv->val.list->push(result);
	 }
	 else
	    rv = result;
      }
   }
   if (xsink->isEvent())
   {
      if (rv)
      {
	 rv->deref(xsink);
	 rv = NULL;
      }
   }
   context->deref(xsink);
   return rv;
}
