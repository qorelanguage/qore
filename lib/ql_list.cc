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
   const char *name = NULL;
   // check for a function name in second argument and throw exception if not present
   // before checking for a list in the first argument
   QoreNode *fn = get_param(params, 1);
   if (!is_nothing(fn))
   {
      if (fn->type != NT_STRING)
      {
	 xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a string (%s)", fn->type->getName());
	 return NULL;
      }
      name = fn->val.String->getBuffer();
   }
   
   QoreNode *lst = test_param(params, NT_LIST, 0);
   if (!lst)
      return NULL;
   
   if (name)
      return lst->val.list->sort(name, xsink);
   return lst->val.list->sort();
}

static class QoreNode *f_sortDescending(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *lst = test_param(params, NT_LIST, 0);
   if (!lst)
      return NULL;
   
   return lst->val.list->sortDescending();
}

static class QoreNode *f_sortStable(class QoreNode *params, ExceptionSink *xsink)
{
   const char *name = NULL;
   // check for a function name in second argument and throw exception if not present
   // before checking for a list in the first argument
   QoreNode *fn = get_param(params, 1);
   if (!is_nothing(fn))
   {
      if (fn->type != NT_STRING)
      {
	 xsink->raiseException("SORTSTABLE-PARAMETER-ERROR", "second argument is present and is not a string (%s)", fn->type->getName());
	 return NULL;
      }
      name = fn->val.String->getBuffer();
   }
   
   QoreNode *lst = test_param(params, NT_LIST, 0);
   if (!lst)
      return NULL;
   
   if (name)
      return lst->val.list->sortStable(name, xsink);
   return lst->val.list->sortStable();
}

static class QoreNode *f_sortDescendingStable(class QoreNode *params, ExceptionSink *xsink)
{   
   QoreNode *lst = test_param(params, NT_LIST, 0);
   if (!lst)
      return NULL;
   
   return lst->val.list->sortDescendingStable();
}

static class QoreNode *f_min(class QoreNode *params, ExceptionSink *xsink)
{   
   QoreNode *p = test_param(params, NT_LIST, 0);
   class List *lst;
   const char *callback = NULL;
   if (p)
   {
      lst = p->val.list;
      p = test_param(params, NT_STRING, 1);
      if (p)
	 callback = p->val.String->getBuffer();
   }
   else
   {
      if (!num_params(params))
	 return NULL;
      lst = params->val.list;
   }

   class QoreNode *rv;
   if (callback)
      rv = lst->min(callback, xsink);
   else
      rv = lst->min();

   return rv;
}  

static class QoreNode *f_max(class QoreNode *params, ExceptionSink *xsink)
{   
   QoreNode *p = test_param(params, NT_LIST, 0);
   class List *lst;
   const char *callback = NULL;
   if (p)
   {
      lst = p->val.list;
      p = test_param(params, NT_STRING, 1);
      if (p)
	 callback = p->val.String->getBuffer();
   }
   else
   {
      if (!num_params(params))
	 return NULL;
      lst = params->val.list;
   }

   class QoreNode *rv;
   if (callback)
      rv = lst->max(callback, xsink);
   else
      rv = lst->max();

   return rv;
}   

void init_list_functions()
{
   builtinFunctions.add("sort", f_sort);
   builtinFunctions.add("sortDescending", f_sortDescending);
   builtinFunctions.add("sortStable", f_sortStable);
   builtinFunctions.add("sortDescendingStable", f_sortDescendingStable);   
   builtinFunctions.add("min", f_min);
   builtinFunctions.add("max", f_max);
}

