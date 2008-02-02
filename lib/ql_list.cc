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

static AbstractQoreNode *f_sort(const QoreListNode *params, ExceptionSink *xsink)
{
   // get list as first argument
   AbstractQoreNode *lst = get_param(params, 0);
   if (!lst)
      return 0;
   QoreListNode *l = dynamic_cast<QoreListNode *>(lst);
   if (!l)
      return lst->RefSelf();
   
   // check for a function name or call reference in second argument
   const AbstractQoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return l->sort();

   const QoreType *fntype = fn->getType();
   
   if (fntype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(fn);
      ReferenceHolder<ResolvedFunctionReferenceNode> fr(getFunctionReference(str, xsink), xsink);
      if (!fr)
	 return NULL;
      
      return l->sort(*fr, xsink);
   }

   if (fntype != NT_FUNCREF)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->getTypeName());
      return NULL;
   }
   return l->sort(reinterpret_cast<const ResolvedFunctionReferenceNode *>(fn), xsink);
}

static AbstractQoreNode *f_sortDescending(const QoreListNode *params, ExceptionSink *xsink)
{
   // get list as first argument
   AbstractQoreNode *lst = get_param(params, 0);
   if (!lst)
      return 0;
   QoreListNode *l = dynamic_cast<QoreListNode *>(lst);
   if (!l)
      return lst->RefSelf();
   
   // check for a function name or call reference in second argument
   const AbstractQoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return l->sortDescending();

   const QoreType *fntype = fn->getType();
   
   if (fntype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(fn);
      ReferenceHolder<ResolvedFunctionReferenceNode> fr(getFunctionReference(str, xsink), xsink);
      if (!fr)
	 return NULL;
      
      return l->sortDescending(*fr, xsink);
   }
   
   if (fntype != NT_FUNCREF)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->getTypeName());
      return NULL;
   }
   return l->sortDescending(reinterpret_cast<const ResolvedFunctionReferenceNode *>(fn), xsink);
}

static AbstractQoreNode *f_sortStable(const QoreListNode *params, ExceptionSink *xsink)
{
   // get list as first argument
   AbstractQoreNode *lst = get_param(params, 0);
   if (!lst)
      return 0;
   QoreListNode *l = dynamic_cast<QoreListNode *>(lst);
   if (!l)
      return lst->RefSelf();
   
   // check for a function name or call reference in second argument
   const AbstractQoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return l->sortStable();

   const QoreType *fntype = fn->getType();
   
   if (fntype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(fn);
      ReferenceHolder<ResolvedFunctionReferenceNode> fr(getFunctionReference(str, xsink), xsink);
      if (!fr)
	 return NULL;
      
      return l->sortStable(*fr, xsink);
   }
      
   if (fntype != NT_FUNCREF)
   {
      xsink->raiseException("SORTSTABLE-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->getTypeName());
      return NULL;
   }
   return l->sortStable(reinterpret_cast<const ResolvedFunctionReferenceNode *>(fn), xsink);
}

static AbstractQoreNode *f_sortDescendingStable(const QoreListNode *params, ExceptionSink *xsink)
{
   // get list as first argument
   AbstractQoreNode *lst = get_param(params, 0);
   if (!lst)
      return 0;
   QoreListNode *l = dynamic_cast<QoreListNode *>(lst);
   if (!l)
      return lst->RefSelf();
   
   // check for a function name or call reference in second argument
   const AbstractQoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return l->sortDescendingStable();

   const QoreType *fntype = fn->getType();
   
   if (fntype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(fn);
      ReferenceHolder<ResolvedFunctionReferenceNode> fr(getFunctionReference(str, xsink), xsink);
      if (!fr)
	 return NULL;
      
      return l->sortDescendingStable(*fr, xsink);
   }

   if (fntype != NT_FUNCREF)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->getTypeName());
      return NULL;
   }
   return l->sortDescendingStable(reinterpret_cast<const ResolvedFunctionReferenceNode *>(fn), xsink);
}

static AbstractQoreNode *f_min(const QoreListNode *params, ExceptionSink *xsink)
{   
   QoreListNode *lst = test_list_param(params, 0);
   if (lst)
   {
      const AbstractQoreNode *p = get_param(params, 1);
      if (is_nothing(p))
	 return lst->min();

      const QoreType *ptype = p->getType();
      
      if (ptype == NT_STRING) {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p);
	 ReferenceHolder<ResolvedFunctionReferenceNode> fr(getFunctionReference(str, xsink), xsink);
	 if (!fr)
	    return NULL;
	 
	 return lst->min(*fr, xsink);
      }

      if (ptype != NT_FUNCREF)
      {
	 xsink->raiseException("MIN-PARAM-ERROR", "second argument is present and is not a call reference or string (%s)", p->getTypeName());
	 return NULL;
      }
      return lst->min(reinterpret_cast<const ResolvedFunctionReferenceNode *>(p), xsink);
   }
   if (!num_params(params))
      return NULL;
   return params->min();
}

static AbstractQoreNode *f_max(const QoreListNode *params, ExceptionSink *xsink)
{   
   QoreListNode *lst = test_list_param(params, 0);
   if (lst)
   {
      const AbstractQoreNode *p = get_param(params, 1);
      if (is_nothing(p))
	 return lst->max();
      
      const QoreType *ptype = p->getType();
      
      if (ptype == NT_STRING) {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p);
	 ReferenceHolder<ResolvedFunctionReferenceNode> fr(getFunctionReference(str, xsink), xsink);
	 if (!fr)
	    return NULL;
	 
	 return lst->max(*fr, xsink);
      }

      if (ptype != NT_FUNCREF)
      {
	 xsink->raiseException("MAX-PARAM-ERROR", "second argument is present and is not a call reference or string (%s)", p->getTypeName());
	 return NULL;
      }
      return lst->max(reinterpret_cast<const ResolvedFunctionReferenceNode *>(p), xsink);
   }
   if (!num_params(params))
      return NULL;
   return params->max();
}

static AbstractQoreNode *f_reverse(const QoreListNode *params, ExceptionSink *xsink)
{ 
   const AbstractQoreNode *p = get_param(params, 0);

   const QoreType *ptype = p ? p->getType() : 0;
   
   if (ptype == NT_LIST)
      return reinterpret_cast<const QoreListNode *>(p)->reverse();

   if (ptype == NT_STRING)
      return reinterpret_cast<const QoreStringNode *>(p)->reverse();

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

