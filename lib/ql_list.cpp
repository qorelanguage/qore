/*
 ql_list.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2011 David Nichols
 
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

static AbstractQoreNode *f_sort_noop(const QoreListNode *args, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(args, 0);
   return p ? p->refSelf() : 0;
}

static AbstractQoreNode *f_sort(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   return l->sort();
}

static AbstractQoreNode *f_sort_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   HARD_QORE_PARAM(str, const QoreStringNode, args, 1);
   ReferenceHolder<ResolvedCallReferenceNode> fr(getCallReference(str, xsink), xsink);
   return !fr ? 0 : l->sort(*fr, xsink);
}

static AbstractQoreNode *f_sort_code(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   HARD_QORE_PARAM(f, const ResolvedCallReferenceNode, args, 1);
   return l->sort(f, xsink);
}

static AbstractQoreNode *f_sortDescending(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   return l->sortDescending();
}

static AbstractQoreNode *f_sortDescending_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   HARD_QORE_PARAM(str, const QoreStringNode, args, 1);
   ReferenceHolder<ResolvedCallReferenceNode> fr(getCallReference(str, xsink), xsink);
   return !fr ? 0 : l->sortDescending(*fr, xsink);
}

static AbstractQoreNode *f_sortDescending_code(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   HARD_QORE_PARAM(f, const ResolvedCallReferenceNode, args, 1);
   return l->sortDescending(f, xsink);
}

static AbstractQoreNode *f_sortStable(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   return l->sortStable();
}

static AbstractQoreNode *f_sortStable_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   HARD_QORE_PARAM(str, const QoreStringNode, args, 1);
   ReferenceHolder<ResolvedCallReferenceNode> fr(getCallReference(str, xsink), xsink);
   return !fr ? 0 : l->sortStable(*fr, xsink);
}

static AbstractQoreNode *f_sortStable_code(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   HARD_QORE_PARAM(f, const ResolvedCallReferenceNode, args, 1);
   return l->sortStable(f, xsink);
}

static AbstractQoreNode *f_sortDescendingStable(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   return l->sortDescendingStable();
}

static AbstractQoreNode *f_sortDescendingStable_str(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   HARD_QORE_PARAM(str, const QoreStringNode, args, 1);
   ReferenceHolder<ResolvedCallReferenceNode> fr(getCallReference(str, xsink), xsink);
   return !fr ? 0 : l->sortDescendingStable(*fr, xsink);
}

static AbstractQoreNode *f_sortDescendingStable_code(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   HARD_QORE_PARAM(f, const ResolvedCallReferenceNode, args, 1);
   return l->sortDescendingStable(f, xsink);
}

static AbstractQoreNode *f_min_list(const QoreListNode *args, ExceptionSink *xsink) {   
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   return l->min();
}

static AbstractQoreNode *f_min_list_str(const QoreListNode *args, ExceptionSink *xsink) {   
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   HARD_QORE_PARAM(str, const QoreStringNode, args, 1);
   ReferenceHolder<ResolvedCallReferenceNode> fr(getCallReference(str, xsink), xsink);
   return !fr ? 0 : l->min(*fr, xsink);
}

static AbstractQoreNode *f_min_list_code(const QoreListNode *args, ExceptionSink *xsink) {   
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   HARD_QORE_PARAM(f, const ResolvedCallReferenceNode, args, 1);
   return l->min(f, xsink);
}

static AbstractQoreNode *f_min(const QoreListNode *args, ExceptionSink *xsink) {
   return args ? args->min() : 0;
}

static AbstractQoreNode *f_max_list(const QoreListNode *args, ExceptionSink *xsink) {   
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   return l->max();
}

static AbstractQoreNode *f_max_list_str(const QoreListNode *args, ExceptionSink *xsink) {   
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   HARD_QORE_PARAM(str, const QoreStringNode, args, 1);
   ReferenceHolder<ResolvedCallReferenceNode> fr(getCallReference(str, xsink), xsink);
   return !fr ? 0 : l->max(*fr, xsink);
}

static AbstractQoreNode *f_max_list_code(const QoreListNode *args, ExceptionSink *xsink) {   
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   HARD_QORE_PARAM(f, const ResolvedCallReferenceNode, args, 1);
   return l->max(f, xsink);
}

static AbstractQoreNode *f_max(const QoreListNode *args, ExceptionSink *xsink) {
   return args ? args->max() : 0;
}

static AbstractQoreNode *f_reverse_str(const QoreListNode *args, ExceptionSink *xsink) { 
   HARD_QORE_PARAM(p, const QoreStringNode, args, 0);
   return p->reverse();
}

static AbstractQoreNode *f_reverse_list(const QoreListNode *args, ExceptionSink *xsink) { 
   HARD_QORE_PARAM(p, const QoreListNode, args, 0);
   return p->reverse();
}

static AbstractQoreNode *f_inlist_any_list(const QoreListNode *args, ExceptionSink *xsink) { 
   const AbstractQoreNode *p0 = get_param(args, 0);
   HARD_QORE_PARAM(p1, const QoreListNode, args, 1);

   ConstListIterator li(p1);
   while (li.next()) {
      bool b = QoreLogicalEqualsOperatorNode::softEqual(p0, li.getValue(), xsink);
      if (*xsink)
	 return 0;
      if (b)
	 return &True;
   }
   return &False;
}

static AbstractQoreNode *f_inlist_any_any(const QoreListNode *args, ExceptionSink *xsink) { 
   const AbstractQoreNode *p0 = get_param(args, 0);
   const AbstractQoreNode *p1 = get_param(args, 1);

   return get_bool_node(QoreLogicalEqualsOperatorNode::softEqual(p0, p1, xsink));
}

static AbstractQoreNode *f_inlist_hard_any_something(const QoreListNode *args, ExceptionSink *xsink) { 
   const AbstractQoreNode *p0 = get_param(args, 0);
   const AbstractQoreNode *p1 = get_param(args, 1);
   assert(p1);

   return get_bool_node(OP_ABSOLUTE_EQ->bool_eval(p0, p1, xsink));
}

static AbstractQoreNode *f_inlist_hard_any_list(const QoreListNode *args, ExceptionSink *xsink) { 
   const AbstractQoreNode *p0 = get_param(args, 0);
   bool p0_is_nothing = is_nothing(p0);

   HARD_QORE_PARAM(l, const QoreListNode, args, 1);

   ConstListIterator li(l);
   while (li.next()) {
      const AbstractQoreNode *lp = li.getValue();

      bool b;

      // do hard comparison inline
      if (is_nothing(lp))
	 b = p0_is_nothing;
      else if (p0_is_nothing)
	 b = false;
      else {
	 b = p0->is_equal_hard(lp, xsink);
	 if (*xsink)
	    return 0;
      }

      if (b)
	 return &True;
   }
   return &False;
}

void init_list_functions() {
   // sort() will return the first argument passed if it's not a list
   builtinFunctions.add2("sort", f_sort_noop, QC_NOOP, QDOM_DEFAULT);
   builtinFunctions.add2("sort", f_sort, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("sort", f_sort_str, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 2, listTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("sort", f_sort_code, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 2, listTypeInfo, QORE_PARAM_NO_ARG, codeTypeInfo, QORE_PARAM_NO_ARG);

   // sortStable() will return the first argument passed if it's not a list
   builtinFunctions.add2("sortStable", f_sort_noop, QC_NOOP, QDOM_DEFAULT);

   builtinFunctions.add2("sortStable", f_sortStable, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("sortStable", f_sortStable_str, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 2, listTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("sortStable", f_sortStable_code, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 2, listTypeInfo, QORE_PARAM_NO_ARG, codeTypeInfo, QORE_PARAM_NO_ARG);

   // sortDescending() will return the first argument passed if it's not a list
   builtinFunctions.add2("sortDescending", f_sort_noop, QC_NOOP, QDOM_DEFAULT);

   builtinFunctions.add2("sortDescending", f_sortDescending, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("sortDescending", f_sortDescending_str, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 2, listTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("sortDescending", f_sortDescending_code, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 2, listTypeInfo, QORE_PARAM_NO_ARG, codeTypeInfo, QORE_PARAM_NO_ARG);

   // sortDescendingStable() will return the first argument passed if it's not a list
   builtinFunctions.add2("sortDescendingStable", f_sort_noop, QC_NOOP, QDOM_DEFAULT);

   builtinFunctions.add2("sortDescendingStable", f_sortDescendingStable, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("sortDescendingStable", f_sortDescendingStable_str, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 2, listTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("sortDescendingStable", f_sortDescendingStable_code, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 2, listTypeInfo, QORE_PARAM_NO_ARG, codeTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("min", f_min, QC_CONSTANT | QC_USES_EXTRA_ARGS, QDOM_DEFAULT);
   builtinFunctions.add2("min", f_min_list, QC_CONSTANT, QDOM_DEFAULT, 0, 1, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("min", f_min_list_str, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, listTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("min", f_min_list_code, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, listTypeInfo, QORE_PARAM_NO_ARG, codeTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("max", f_max, QC_CONSTANT | QC_USES_EXTRA_ARGS, QDOM_DEFAULT);
   builtinFunctions.add2("max", f_max_list, QC_CONSTANT, QDOM_DEFAULT, 0, 1, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("max", f_max_list_str, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, listTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("max", f_max_list_code, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, listTypeInfo, QORE_PARAM_NO_ARG, codeTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("reverse", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("reverse", f_reverse_str, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("reverse", f_reverse_list, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("inlist", f_bool_noop, QC_NOOP, QDOM_DEFAULT, boolTypeInfo);
   builtinFunctions.add2("inlist", f_bool_noop, QC_NOOP, QDOM_DEFAULT, boolTypeInfo, 2, anyTypeInfo, QORE_PARAM_NO_ARG, nothingTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("inlist", f_inlist_any_list, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 2, anyTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("inlist", f_inlist_any_any, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 2, anyTypeInfo, QORE_PARAM_NO_ARG, somethingTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("inlist_hard", f_bool_noop, QC_NOOP, QDOM_DEFAULT, boolTypeInfo);
   builtinFunctions.add2("inlist_hard", f_bool_noop, QC_NOOP, QDOM_DEFAULT, boolTypeInfo, 2, anyTypeInfo, QORE_PARAM_NO_ARG, nothingTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("inlist_hard", f_inlist_hard_any_something, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 2, anyTypeInfo, QORE_PARAM_NO_ARG, somethingTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("inlist_hard", f_inlist_hard_any_list, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 2, anyTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);
}

