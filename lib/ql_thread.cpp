/*
  ql_thread.cpp

  POSIX thread library for Qore

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
#include <qore/intern/ql_thread.h>

#include <pthread.h>

static AbstractQoreNode *f_gettid(const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(gettid());
}

extern int num_threads;
static AbstractQoreNode *f_num_threads(const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(num_threads);
}

static AbstractQoreNode *f_thread_list(const QoreListNode *args, ExceptionSink *xsink) {
   return get_thread_list();
}

static AbstractQoreNode *f_save_thread_data_hash(const QoreListNode *args, ExceptionSink *xsink) {
   const QoreHashNode *h = HARD_QORE_HASH(args, 0);
   QoreHashNode *data = getProgram()->getThreadData();
   data->merge(h, xsink);
   
   return 0;
}

static AbstractQoreNode *f_save_thread_data_str_any(const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *str = HARD_QORE_STRING(args, 0);
   const AbstractQoreNode *p1 = get_param(args, 1);

   QoreHashNode *data = getProgram()->getThreadData();
   data->setKeyValue(str, p1 ? p1->refSelf() : 0, xsink);

   return 0;
}

static void delete_thread_data_intern(const QoreListNode *args, ExceptionSink *xsink) {
   // get thread data hash
   QoreHashNode *data = getProgram()->getThreadData();
      
   // iterate through arguments and delete each key
   for (unsigned i = 0; i < args->size(); i++) {
      const AbstractQoreNode *p = get_param(args, i);
      if (p) {
         QoreStringValueHelper t(p);
         data->deleteKey(*t, xsink);
         if (*xsink)
            break;
      }
   }
}

static AbstractQoreNode *f_delete_thread_data(const QoreListNode *args, ExceptionSink *xsink) {
   if (num_args(args))
      delete_thread_data_intern(args, xsink);
   return 0;
}

static AbstractQoreNode *f_delete_thread_data_list(const QoreListNode *args, ExceptionSink *xsink) {
   const QoreListNode *l = HARD_QORE_LIST(args, 0);
   delete_thread_data_intern(l, xsink);
   return 0;
}

static void remove_thread_data_intern(const QoreListNode *args, ExceptionSink *xsink) {
   // get thread data hash
   QoreHashNode *data = getProgram()->getThreadData();
      
   // iterate through arguments and remove each key
   for (unsigned i = 0; i < args->size(); i++) {
      const AbstractQoreNode *p = get_param(args, i);
      if (p) {
         QoreStringValueHelper t(p);
         data->removeKey(*t, xsink);
         if (*xsink)
            break;
      }
   }
}

static AbstractQoreNode *f_remove_thread_data(const QoreListNode *args, ExceptionSink *xsink) {
   if (num_args(args))
      remove_thread_data_intern(args, xsink);
   return 0;
}

static AbstractQoreNode *f_remove_thread_data_list(const QoreListNode *args, ExceptionSink *xsink) {
   const QoreListNode *l = HARD_QORE_LIST(args, 0);
   remove_thread_data_intern(l, xsink);
   return 0;
}

static AbstractQoreNode *f_delete_all_thread_data(const QoreListNode *args, ExceptionSink *xsink) {
   getProgram()->clearThreadData(xsink);
   return 0;
}

static AbstractQoreNode *f_get_thread_data(const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   QoreHashNode *data = getProgram()->getThreadData();
   AbstractQoreNode *v = data->getKeyValue(p0->getBuffer());
   return v ? v->refSelf() : 0;
}

static AbstractQoreNode *f_get_all_thread_data(const QoreListNode *args, ExceptionSink *xsink) {
   return getProgram()->getThreadData()->copy();
}

static AbstractQoreNode *f_getAllThreadCallStacks(const QoreListNode *args, ExceptionSink *xsink) {
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   return getAllCallStacks();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "this version of the Qore library was built without support for runtime thread stack tracing; check Qore::HAVE_RUNTIME_THREAD_STACK_TRACE before calling");
   return 0;
#endif
}

static AbstractQoreNode *f_throwThreadResourceExceptions(const QoreListNode *args, ExceptionSink *xsink) {
   purge_thread_resources(xsink);
   return 0;
}

void init_thread_functions() {
   builtinFunctions.add2("gettid", f_gettid, QC_CONSTANT, QDOM_THREAD_INFO, bigIntTypeInfo);
   builtinFunctions.add2("num_threads", f_num_threads, QC_CONSTANT, QDOM_THREAD_INFO, bigIntTypeInfo);
   builtinFunctions.add2("thread_list", f_thread_list, QC_CONSTANT, QDOM_THREAD_INFO, listTypeInfo);

   builtinFunctions.add2("save_thread_data", f_noop, QC_RUNTIME_NOOP, QDOM_THREAD_CONTROL, nothingTypeInfo);
   builtinFunctions.add2("save_thread_data", f_save_thread_data_hash, QC_NO_FLAGS, QDOM_THREAD_CONTROL, nothingTypeInfo, 1, hashTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("save_thread_data", f_save_thread_data_str_any, QC_NO_FLAGS, QDOM_THREAD_CONTROL, nothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, anyTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("delete_thread_data", f_delete_thread_data, QC_USES_EXTRA_ARGS, QDOM_THREAD_CONTROL, nothingTypeInfo);
   builtinFunctions.add2("delete_thread_data", f_delete_thread_data_list, QC_USES_EXTRA_ARGS, QDOM_THREAD_CONTROL, nothingTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("remove_thread_data", f_remove_thread_data, QC_USES_EXTRA_ARGS, QDOM_THREAD_CONTROL, nothingTypeInfo);
   builtinFunctions.add2("remove_thread_data", f_remove_thread_data_list, QC_USES_EXTRA_ARGS, QDOM_THREAD_CONTROL, nothingTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("delete_all_thread_data", f_delete_all_thread_data, QC_NO_FLAGS, QDOM_THREAD_CONTROL, nothingTypeInfo);

   builtinFunctions.add2("get_thread_data", f_noop, QC_RUNTIME_NOOP, QDOM_THREAD_CONTROL, nothingTypeInfo);
   builtinFunctions.add2("get_thread_data", f_get_thread_data, QC_CONSTANT, QDOM_THREAD_CONTROL | QDOM_THREAD_INFO, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("get_all_thread_data", f_get_all_thread_data, QC_CONSTANT, QDOM_THREAD_CONTROL | QDOM_THREAD_INFO, hashTypeInfo);

   builtinFunctions.add2("getAllThreadCallStacks", f_getAllThreadCallStacks, QC_NO_FLAGS, QDOM_THREAD_CONTROL | QDOM_THREAD_INFO, hashTypeInfo);

   builtinFunctions.add2("throwThreadResourceExceptions", f_throwThreadResourceExceptions, QC_NO_FLAGS, QDOM_THREAD_CONTROL, nothingTypeInfo);
}
