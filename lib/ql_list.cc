/*
 ql_list.cc
 
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
#include <qore/intern/ql_list.h>

static AbstractQoreNode *f_sort(const QoreListNode *params, ExceptionSink *xsink)
{
   // get list as first argument
   const AbstractQoreNode *lst = get_param(params, 0);
   if (!lst)
      return 0;

   const QoreListNode *l = dynamic_cast<const QoreListNode *>(lst);
   if (!l)
      return lst->refSelf();
   
   // check for a function name or call reference in second argument
   const AbstractQoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return l->sort();

   qore_type_t fntype = fn->getType();
   
   if (fntype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(fn);
      ReferenceHolder<ResolvedCallReferenceNode> fr(getCallReference(str, xsink), xsink);
      if (!fr)
	 return 0;
      
      return l->sort(*fr, xsink);
   }

   if (fntype != NT_FUNCREF && fntype != NT_RUNTIME_CLOSURE)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->getTypeName());
      return 0;
   }
   return l->sort(reinterpret_cast<const ResolvedCallReferenceNode *>(fn), xsink);
}

static AbstractQoreNode *f_sortDescending(const QoreListNode *params, ExceptionSink *xsink)
{
   // get list as first argument
   const AbstractQoreNode *lst = get_param(params, 0);
   if (!lst)
      return 0;
   const QoreListNode *l = dynamic_cast<const QoreListNode *>(lst);
   if (!l)
      return lst->refSelf();
   
   // check for a function name or call reference in second argument
   const AbstractQoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return l->sortDescending();

   qore_type_t fntype = fn->getType();
   
   if (fntype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(fn);
      ReferenceHolder<ResolvedCallReferenceNode> fr(getCallReference(str, xsink), xsink);
      if (!fr)
	 return 0;
      
      return l->sortDescending(*fr, xsink);
   }
   
   if (fntype != NT_FUNCREF && fntype != NT_RUNTIME_CLOSURE)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->getTypeName());
      return 0;
   }
   return l->sortDescending(reinterpret_cast<const ResolvedCallReferenceNode *>(fn), xsink);
}

static AbstractQoreNode *f_sortStable(const QoreListNode *params, ExceptionSink *xsink)
{
   // get list as first argument
   const AbstractQoreNode *lst = get_param(params, 0);
   if (!lst)
      return 0;
   const QoreListNode *l = dynamic_cast<const QoreListNode *>(lst);
   if (!l)
      return lst->refSelf();
   
   // check for a function name or call reference in second argument
   const AbstractQoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return l->sortStable();

   qore_type_t fntype = fn->getType();
   
   if (fntype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(fn);
      ReferenceHolder<ResolvedCallReferenceNode> fr(getCallReference(str, xsink), xsink);
      if (!fr)
	 return 0;
      
      return l->sortStable(*fr, xsink);
   }
      
   if (fntype != NT_FUNCREF && fntype != NT_RUNTIME_CLOSURE)
   {
      xsink->raiseException("SORTSTABLE-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->getTypeName());
      return 0;
   }
   return l->sortStable(reinterpret_cast<const ResolvedCallReferenceNode *>(fn), xsink);
}

static AbstractQoreNode *f_sortDescendingStable(const QoreListNode *params, ExceptionSink *xsink)
{
   // get list as first argument
   const AbstractQoreNode *lst = get_param(params, 0);
   if (!lst)
      return 0;
   const QoreListNode *l = dynamic_cast<const QoreListNode *>(lst);
   if (!l)
      return lst->refSelf();
   
   // check for a function name or call reference in second argument
   const AbstractQoreNode *fn = get_param(params, 1);
   if (is_nothing(fn))
      return l->sortDescendingStable();

   qore_type_t fntype = fn->getType();
   
   if (fntype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(fn);
      ReferenceHolder<ResolvedCallReferenceNode> fr(getCallReference(str, xsink), xsink);
      if (!fr)
	 return 0;
      
      return l->sortDescendingStable(*fr, xsink);
   }

   if (fntype != NT_FUNCREF && fntype != NT_RUNTIME_CLOSURE)
   {
      xsink->raiseException("SORT-PARAMETER-ERROR", "second argument is present and is not a call reference or string (%s)", fn->getTypeName());
      return 0;
   }
   return l->sortDescendingStable(reinterpret_cast<const ResolvedCallReferenceNode *>(fn), xsink);
}

static AbstractQoreNode *f_min(const QoreListNode *params, ExceptionSink *xsink)
{   
   const QoreListNode *lst = test_list_param(params, 0);
   if (lst)
   {
      const AbstractQoreNode *p = get_param(params, 1);
      if (is_nothing(p))
	 return lst->min();

      qore_type_t ptype = p->getType();
      
      if (ptype == NT_STRING) {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p);
	 ReferenceHolder<ResolvedCallReferenceNode> fr(getCallReference(str, xsink), xsink);
	 if (!fr)
	    return 0;
	 
	 return lst->min(*fr, xsink);
      }

      if (ptype != NT_FUNCREF && ptype != NT_RUNTIME_CLOSURE)
      {
	 xsink->raiseException("MIN-PARAM-ERROR", "second argument is present and is not a call reference or string (%s)", p->getTypeName());
	 return 0;
      }
      return lst->min(reinterpret_cast<const ResolvedCallReferenceNode *>(p), xsink);
   }
   if (!num_params(params))
      return 0;
   return params->min();
}

static AbstractQoreNode *f_max(const QoreListNode *params, ExceptionSink *xsink)
{   
   const QoreListNode *lst = test_list_param(params, 0);
   if (lst)
   {
      const AbstractQoreNode *p = get_param(params, 1);
      if (is_nothing(p))
	 return lst->max();
      
      qore_type_t ptype = p->getType();
      
      if (ptype == NT_STRING) {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p);
	 ReferenceHolder<ResolvedCallReferenceNode> fr(getCallReference(str, xsink), xsink);
	 if (!fr)
	    return 0;
	 
	 return lst->max(*fr, xsink);
      }

      if (ptype != NT_FUNCREF && ptype != NT_RUNTIME_CLOSURE)
      {
	 xsink->raiseException("MAX-PARAM-ERROR", "second argument is present and is not a call reference or string (%s)", p->getTypeName());
	 return 0;
      }
      return lst->max(reinterpret_cast<const ResolvedCallReferenceNode *>(p), xsink);
   }
   if (!num_params(params))
      return 0;
   return params->max();
}

static AbstractQoreNode *f_reverse(const QoreListNode *params, ExceptionSink *xsink)
{ 
   const AbstractQoreNode *p = get_param(params, 0);

   qore_type_t ptype = p ? p->getType() : 0;
   
   if (ptype == NT_LIST)
      return reinterpret_cast<const QoreListNode *>(p)->reverse();

   if (ptype == NT_STRING)
      return reinterpret_cast<const QoreStringNode *>(p)->reverse();

   return 0;
}

static AbstractQoreNode *f_inlist(const QoreListNode *params, ExceptionSink *xsink)
{ 
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);

   // always return False if 2nd argument is NOTHING
   if (is_nothing(p1))
      return &False;

   if (p1->getType() != NT_LIST)
      return get_bool_node(OP_LOG_EQ->bool_eval(p0, p1, xsink));

   ConstListIterator li(reinterpret_cast<const QoreListNode *>(p1));
   while (li.next()) {
      bool b = OP_LOG_EQ->bool_eval(p0, li.getValue(), xsink);
      if (*xsink)
	 return 0;
      if (b)
	 return &True;
   }
   return &False;
}

static AbstractQoreNode *f_inlist_hard(const QoreListNode *params, ExceptionSink *xsink)
{ 
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);

   // always return False if 2nd argument is NOTHING
   if (is_nothing(p1))
      return &False;

   if (p1->getType() != NT_LIST)
      return get_bool_node(OP_ABSOLUTE_EQ->bool_eval(p0, p1, xsink));

   ConstListIterator li(reinterpret_cast<const QoreListNode *>(p1));
   while (li.next()) {
      bool b = OP_ABSOLUTE_EQ->bool_eval(p0, li.getValue(), xsink);
      if (*xsink)
	 return 0;
      if (b)
	 return &True;
   }
   return &False;
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
   builtinFunctions.add("inlist", f_inlist);
   builtinFunctions.add("inlist_hard", f_inlist_hard);
}

