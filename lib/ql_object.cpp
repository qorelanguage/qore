/*
 ql_object.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2010 David Nichols
 
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
#include <qore/intern/ql_object.h>

// returns a list of method names for the object passed as a parameter
static AbstractQoreNode *f_getMethodList(const QoreListNode *args, ExceptionSink *xsink) {
   QoreObject *p0 = HARD_QORE_OBJECT(args, 0);
   return p0->getClass()->getMethodList();
}

static AbstractQoreNode *f_callObjectMethod(const QoreListNode *args, ExceptionSink *xsink) {
   QoreObject *p0 = HARD_QORE_OBJECT(args, 0);
   const QoreStringNode *p1 = HARD_QORE_STRING(args, 1);

   ReferenceHolder<QoreListNode> call_args(xsink);
   
   // if there are arguments to pass
   if (num_args(args) > 2) {
      // create argument list by copying current list
      call_args = args->copyListFrom(2);
   }

   // make sure method call is internal (allows access to private methods) if this function was called internally
   CodeContextHelper cch(0, p0, xsink);
   return p0->evalMethod(p1, *call_args, xsink);
}

static AbstractQoreNode *f_callObjectMethodArgs(const QoreListNode *args, ExceptionSink *xsink) {
   QoreObject *p0 = HARD_QORE_OBJECT(args, 0);
   const QoreStringNode *p1 = HARD_QORE_STRING(args, 1);

   CodeContextHelper cch(0, p0, xsink);
   return p0->evalMethod(p1, 0, xsink);
}

static AbstractQoreNode *f_callObjectMethodArgs_something(const QoreListNode *args, ExceptionSink *xsink) {
   QoreObject *p0 = HARD_QORE_OBJECT(args, 0);
   const QoreStringNode *p1 = HARD_QORE_STRING(args, 1);
   
   const AbstractQoreNode *p2 = get_param(args, 2);
   assert(p2);

   ReferenceHolder<QoreListNode> call_args(new QoreListNode, xsink);
   call_args->push(p2->refSelf());

   CodeContextHelper cch(0, p0, xsink);
   return p0->evalMethod(p1, *call_args, xsink);
}

static AbstractQoreNode *f_callObjectMethodArgs_list(const QoreListNode *args, ExceptionSink *xsink) {
   QoreObject *p0 = HARD_QORE_OBJECT(args, 0);
   const QoreStringNode *p1 = HARD_QORE_STRING(args, 1);
   const QoreListNode *call_args = HARD_QORE_LIST(args, 2);

   CodeContextHelper cch(0, p0, xsink);
   return p0->evalMethod(p1, call_args, xsink);
}

void init_object_functions() {
   QORE_TRACE("init_object_functions()");
   
   builtinFunctions.add2("getMethodList", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("getMethodList", f_getMethodList, QC_CONSTANT, QDOM_DEFAULT, listTypeInfo, 1, objectTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("callObjectMethod", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("callObjectMethod", f_callObjectMethod, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, 0, 2, objectTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("callObjectMethodArgs", f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("callObjectMethodArgs", f_callObjectMethodArgs, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, objectTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("callObjectMethodArgs", f_callObjectMethodArgs_something, QC_NO_FLAGS, QDOM_DEFAULT, 0, 3, objectTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, somethingTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("callObjectMethodArgs", f_callObjectMethodArgs_list, QC_NO_FLAGS, QDOM_DEFAULT, 0, 3, objectTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);
}
