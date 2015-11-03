/*
  ql_thread.cc

  POSIX thread library for Qore

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
#include <qore/intern/ql_thread.h>

#include <pthread.h>

AbstractQoreNode *f_gettid(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(gettid());
}

extern int num_threads;
AbstractQoreNode *f_num_threads(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(num_threads);
}

AbstractQoreNode *f_thread_list(const QoreListNode *params, ExceptionSink *xsink) {
   return get_thread_list();
}

AbstractQoreNode *f_save_thread_data(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->getType() != NT_HASH && p0->getType() != NT_STRING))
      return 0;

   QoreHashNode *data = getProgram()->getThreadData();
   if (p0->getType() == NT_HASH)
      data->merge(reinterpret_cast<const QoreHashNode *>(p0), xsink);
   else {
      const AbstractQoreNode *p1 = get_param(params, 1);

      data->setKeyValue(reinterpret_cast<const QoreStringNode *>(p0), p1 ? p1->refSelf() : 0, xsink);
   }

   return 0;
}

AbstractQoreNode *f_delete_thread_data(const QoreListNode *params, ExceptionSink *xsink) {
   if (num_params(params)) {
      // get thread data hash
      QoreHashNode *data = getProgram()->getThreadData();
      
      // iterate through arguments and delete each key
      for (unsigned i = 0; i < params->size(); i++) {
	 const AbstractQoreNode *p = get_param(params, i);
	 if (p) {
	    QoreStringValueHelper t(p);
	    data->deleteKey(*t, xsink);
	    if (*xsink)
	       break;
	 }
      }
   }
   return 0;
}

AbstractQoreNode *f_remove_thread_data(const QoreListNode *params, ExceptionSink *xsink) {
   if (num_params(params)) {
      // get thread data hash
      QoreHashNode *data = getProgram()->getThreadData();
      
      // iterate through arguments and delete each key
      for (unsigned i = 0; i < params->size(); i++) {
	 const AbstractQoreNode *p = get_param(params, i);
	 if (p) {
	    QoreStringValueHelper t(p);
	    data->removeKey(*t, xsink);
	    if (*xsink)
	       break;
	 }
      }
   }
   return 0;
}

AbstractQoreNode *f_delete_all_thread_data(const QoreListNode *params, ExceptionSink *xsink) {
   getProgram()->clearThreadData(xsink);
   return 0;
}

AbstractQoreNode *f_get_thread_data(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
      return 0;
   QoreHashNode *data = getProgram()->getThreadData();
   AbstractQoreNode *v = data->getKeyValue(p0->getBuffer());
   return v ? v->refSelf() : 0;
}

AbstractQoreNode *f_get_all_thread_data(const QoreListNode *params, ExceptionSink *xsink) {
   return getProgram()->getThreadData()->copy();
}

AbstractQoreNode *f_getAllThreadCallStacks(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   return getAllCallStacks();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "this version of the Qore library was built without support for runtime thread stack tracing; check Qore::HAVE_RUNTIME_THREAD_STACK_TRACE before calling");
   return 0;
#endif
}

AbstractQoreNode *f_throwThreadResourceExceptions(const QoreListNode *params, ExceptionSink *xsink) {
   purge_thread_resources(xsink);
   return 0;
}

void init_thread_functions() {
   builtinFunctions.add("gettid", f_gettid);
   builtinFunctions.add("num_threads", f_num_threads);
   builtinFunctions.add("thread_list", f_thread_list);
   builtinFunctions.add("get_thread_data", f_get_thread_data, QDOM_THREAD_CONTROL);
   builtinFunctions.add("get_all_thread_data", f_get_all_thread_data, QDOM_THREAD_CONTROL);
   builtinFunctions.add("save_thread_data", f_save_thread_data, QDOM_THREAD_CONTROL);
   builtinFunctions.add("delete_thread_data", f_delete_thread_data, QDOM_THREAD_CONTROL);
   builtinFunctions.add("remove_thread_data", f_remove_thread_data, QDOM_THREAD_CONTROL);
   builtinFunctions.add("delete_all_thread_data", f_delete_all_thread_data, QDOM_THREAD_CONTROL);
   builtinFunctions.add("getAllThreadCallStacks", f_getAllThreadCallStacks, QDOM_THREAD_CONTROL);
   builtinFunctions.add("throwThreadResourceExceptions", f_throwThreadResourceExceptions, QDOM_THREAD_CONTROL);
}
