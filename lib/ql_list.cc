/*
 ql_list.cc
 
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
#include <qore/ql_list.h>

static class QoreNode *f_sort(class QoreNode *params, ExceptionSink *xsink)
{
   // check for a function name in second argument and throw exception if not present
   // before checking for a list in the first argument
   QoreNode *lst = test_param(params, NT_LIST, 0);
   if (!lst)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "first argument is not a list");
      return NULL;
   }
   
   QoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return lst->val.list->sort();

   if (fn->type == NT_STRING)
   {
      class FunctionReference *fr = getFunctionReference(fn->val.String, xsink);
      if (!fr)
	 return NULL;
      
      class QoreNode *rv = lst->val.list->sort(fr, xsink);
      fr->del(xsink);
      return rv;
   }
   if (fn->type != NT_FUNCREF)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->type->getName());
      return NULL;
   }
   return lst->val.list->sort(fn->val.funcref, xsink);
}

static class QoreNode *f_sortDescending(class QoreNode *params, ExceptionSink *xsink)
{
   // check for a function name in second argument and throw exception if not present
   // before checking for a list in the first argument
   QoreNode *lst = test_param(params, NT_LIST, 0);
   if (!lst)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "first argument is not a list");
      return NULL;
   }
   
   QoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return lst->val.list->sortDescending();
   
   if (fn->type == NT_STRING)
   {
      class FunctionReference *fr = getFunctionReference(fn->val.String, xsink);
      if (!fr)
	 return NULL;
      
      class QoreNode *rv = lst->val.list->sortDescending(fr, xsink);
      fr->del(xsink);
      return rv;
   }
   if (fn->type != NT_FUNCREF)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->type->getName());
      return NULL;
   }
   return lst->val.list->sortDescending(fn->val.funcref, xsink);
}

static class QoreNode *f_sortStable(class QoreNode *params, ExceptionSink *xsink)
{
   // check for a function name in second argument and throw exception if not present
   // before checking for a list in the first argument
   QoreNode *lst = test_param(params, NT_LIST, 0);
   if (!lst)
   {
      xsink->raiseException("SORTSTABLE-PARAMETER-ERROR", "first argument is not a list");
      return NULL;
   }
   
   QoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return lst->val.list->sortStable();
   
   if (fn->type == NT_STRING)
   {
      class FunctionReference *fr = getFunctionReference(fn->val.String, xsink);
      if (!fr)
	 return NULL;
      
      class QoreNode *rv = lst->val.list->sortStable(fr, xsink);
      fr->del(xsink);
      return rv;
   }
   if (fn->type != NT_FUNCREF)
   {
      xsink->raiseException("SORTSTABLE-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->type->getName());
      return NULL;
   }
   return lst->val.list->sortStable(fn->val.funcref, xsink);
}

static class QoreNode *f_sortDescendingStable(class QoreNode *params, ExceptionSink *xsink)
{
   // check for a function name in second argument and throw exception if not present
   // before checking for a list in the first argument
   QoreNode *lst = test_param(params, NT_LIST, 0);
   if (!lst)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "first argument is not a list");
      return NULL;
   }
   
   QoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return lst->val.list->sortDescendingStable();
   
   if (fn->type == NT_STRING)
   {
      class FunctionReference *fr = getFunctionReference(fn->val.String, xsink);
      if (!fr)
	 return NULL;
      
      class QoreNode *rv = lst->val.list->sortDescendingStable(fr, xsink);
      fr->del(xsink);
      return rv;
   }
   if (fn->type != NT_FUNCREF)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->type->getName());
      return NULL;
   }
   return lst->val.list->sortDescendingStable(fn->val.funcref, xsink);
}

static class QoreNode *f_min(class QoreNode *params, ExceptionSink *xsink)
{   
   QoreNode *p = test_param(params, NT_LIST, 0);
   class List *lst;
   if (p)
   {
      lst = p->val.list;
      p = get_param(params, 1);
      if (is_nothing(p))
	 return lst->min();
      
      if (p->type == NT_STRING)
      {
	 class FunctionReference *fr = getFunctionReference(p->val.String, xsink);
	 if (!fr)
	    return NULL;
	 
	 class QoreNode *rv = lst->min(fr, xsink);
	 fr->del(xsink);
	 return rv;
      }
      if (p->type != NT_FUNCREF)
      {
	 xsink->raiseException("MIN-PARAM-ERROR", "second argument is present and is not a call reference or string (%s)", p->type->getName());
	 return NULL;
      }
      return lst->min(p->val.funcref, xsink);
   }
   if (!num_params(params))
      return NULL;
   return params->val.list->min();
}

static class QoreNode *f_max(class QoreNode *params, ExceptionSink *xsink)
{   
   QoreNode *p = test_param(params, NT_LIST, 0);
   class List *lst;
   if (p)
   {
      lst = p->val.list;
      p = get_param(params, 1);
      if (is_nothing(p))
	 return lst->max();
      
      if (p->type == NT_STRING)
      {
	 class FunctionReference *fr = getFunctionReference(p->val.String, xsink);
	 if (!fr)
	    return NULL;
	 
	 class QoreNode *rv = lst->max(fr, xsink);
	 fr->del(xsink);
	 return rv;
      }
      if (p->type != NT_FUNCREF)
      {
	 xsink->raiseException("MAX-PARAM-ERROR", "second argument is present and is not a call reference or string (%s)", p->type->getName());
	 return NULL;
      }
      return lst->max(p->val.funcref, xsink);
   }
   if (!num_params(params))
      return NULL;
   return params->val.list->max();
}

static class QoreNode *f_reverse(class QoreNode *params, ExceptionSink *xsink)
{ 
   class QoreNode *p = get_param(params, 0);
   if (p)
   {
      if (p->type == NT_LIST)
	 return new QoreNode(p->val.list->reverse());
      if (p->type == NT_STRING)
	 return new QoreNode(p->val.String->reverse());
   }
   return NULL;
}

void init_list_functions()
{
   builtinFunctions.add("sort", f_sort);
   builtinFunctions.add("sortStable", f_sortStable);
   builtinFunctions.add("sortDescending", f_sortDescending);
   builtinFunctions.add("sortDescendingStable", f_sortDescendingStable);   
   builtinFunctions.add("min", f_min);
   builtinFunctions.add("max", f_max);
   builtinFunctions.add("reverse", f_reverse);
}

