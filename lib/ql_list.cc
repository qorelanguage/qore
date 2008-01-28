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
#include <qore/intern/ql_list.h>

static class QoreNode *f_sort(const QoreListNode *params, ExceptionSink *xsink)
{
   // get list as first argument
   QoreNode *lst = get_param(params, 0);
   if (!lst)
      return 0;
   QoreListNode *l = dynamic_cast<QoreListNode *>(lst);
   if (!l)
      return lst->RefSelf();
   
   // check for a function name or call reference in second argument
   QoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return l->sort();

   {
      QoreStringNode *str = dynamic_cast<QoreStringNode *>(fn);
      if (str) {
	 class FunctionReference *fr = getFunctionReference(str, xsink);
	 if (!fr)
	    return NULL;
      
	 class QoreNode *rv = l->sort(fr, xsink);
	 fr->del(xsink);
	 return rv;
      }
   }
   if (fn->type != NT_FUNCREF)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->getTypeName());
      return NULL;
   }
   return l->sort(fn->val.funcref, xsink);
}

static class QoreNode *f_sortDescending(const QoreListNode *params, ExceptionSink *xsink)
{
   // get list as first argument
   QoreNode *lst = get_param(params, 0);
   if (!lst)
      return 0;
   QoreListNode *l = dynamic_cast<QoreListNode *>(lst);
   if (!l)
      return lst->RefSelf();
   
   // check for a function name or call reference in second argument
   QoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return l->sortDescending();

   {
      QoreStringNode *str = dynamic_cast<QoreStringNode *>(fn);
      if (str) {
	 class FunctionReference *fr = getFunctionReference(str, xsink);
	 if (!fr)
	    return NULL;
      
	 class QoreNode *rv = l->sortDescending(fr, xsink);
	 fr->del(xsink);
	 return rv;
      }
   }
   
   if (fn->type != NT_FUNCREF)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->getTypeName());
      return NULL;
   }
   return l->sortDescending(fn->val.funcref, xsink);
}

static class QoreNode *f_sortStable(const QoreListNode *params, ExceptionSink *xsink)
{
   // get list as first argument
   QoreNode *lst = get_param(params, 0);
   if (!lst)
      return 0;
   QoreListNode *l = dynamic_cast<QoreListNode *>(lst);
   if (!l)
      return lst->RefSelf();
   
   // check for a function name or call reference in second argument
   QoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return l->sortStable();

   {
      QoreStringNode *str = dynamic_cast<QoreStringNode *>(fn);
      if (str) {
	 class FunctionReference *fr = getFunctionReference(str, xsink);
	 if (!fr)
	    return NULL;
      
	 class QoreNode *rv = l->sortStable(fr, xsink);
	 fr->del(xsink);
	 return rv;
      }
   }
      
   if (fn->type != NT_FUNCREF)
   {
      xsink->raiseException("SORTSTABLE-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->getTypeName());
      return NULL;
   }
   return l->sortStable(fn->val.funcref, xsink);
}

static class QoreNode *f_sortDescendingStable(const QoreListNode *params, ExceptionSink *xsink)
{
   // get list as first argument
   QoreNode *lst = get_param(params, 0);
   if (!lst)
      return 0;
   QoreListNode *l = dynamic_cast<QoreListNode *>(lst);
   if (!l)
      return lst->RefSelf();
   
   // check for a function name or call reference in second argument
   QoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return l->sortDescendingStable();
   
   {
      QoreStringNode *str = dynamic_cast<QoreStringNode *>(fn);
      if (str) {
	 class FunctionReference *fr = getFunctionReference(str, xsink);
	 if (!fr)
	    return NULL;
      
	 class QoreNode *rv = l->sortDescendingStable(fr, xsink);
	 fr->del(xsink);
	 return rv;
      }
   }

   if (fn->type != NT_FUNCREF)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->getTypeName());
      return NULL;
   }
   return l->sortDescendingStable(fn->val.funcref, xsink);
}

static class QoreNode *f_min(const QoreListNode *params, ExceptionSink *xsink)
{   
   QoreListNode *lst = test_list_param(params, 0);
   if (lst)
   {
      QoreNode *p = get_param(params, 1);
      if (is_nothing(p))
	 return lst->min();
      
      {
	 QoreStringNode *str = dynamic_cast<QoreStringNode *>(p);
	 if (str) {
	    class FunctionReference *fr = getFunctionReference(str, xsink);
	    if (!fr)
	       return NULL;
	    
	    class QoreNode *rv = lst->min(fr, xsink);
	    fr->del(xsink);
	    return rv;
	 }
      }

      if (p->type != NT_FUNCREF)
      {
	 xsink->raiseException("MIN-PARAM-ERROR", "second argument is present and is not a call reference or string (%s)", p->getTypeName());
	 return NULL;
      }
      return lst->min(p->val.funcref, xsink);
   }
   if (!num_params(params))
      return NULL;
   return params->min();
}

static class QoreNode *f_max(const QoreListNode *params, ExceptionSink *xsink)
{   
   QoreListNode *lst = test_list_param(params, 0);
   if (lst)
   {
      QoreNode *p = get_param(params, 1);
      if (is_nothing(p))
	 return lst->max();
      
      {
	 QoreStringNode *str = dynamic_cast<QoreStringNode *>(p);
	 if (str) {
	    class FunctionReference *fr = getFunctionReference(str, xsink);
	    if (!fr)
	       return NULL;
	    
	    class QoreNode *rv = lst->max(fr, xsink);
	    fr->del(xsink);
	    return rv;
	 }
      }

      if (p->type != NT_FUNCREF)
      {
	 xsink->raiseException("MAX-PARAM-ERROR", "second argument is present and is not a call reference or string (%s)", p->getTypeName());
	 return NULL;
      }
      return lst->max(p->val.funcref, xsink);
   }
   if (!num_params(params))
      return NULL;
   return params->max();
}

static class QoreNode *f_reverse(const QoreListNode *params, ExceptionSink *xsink)
{ 
   QoreNode *p = get_param(params, 0);
   {
      QoreListNode *l = dynamic_cast<QoreListNode *>(p);
      if (l)
	 return l->reverse();
   }

   QoreStringNode *str = dynamic_cast<QoreStringNode *>(p);
   if (str)
      return str->reverse();

   return 0;
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

