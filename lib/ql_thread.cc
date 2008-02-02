/*
  ql_thread.cc

  POSIX thread library for Qore

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
#include <qore/intern/ql_thread.h>

#include <pthread.h>

AbstractQoreNode *f_gettid(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(gettid());
}

extern int num_threads;
AbstractQoreNode *f_num_threads(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(num_threads);
}

AbstractQoreNode *f_thread_list(const QoreListNode *params, ExceptionSink *xsink)
{
   return get_thread_list();
}

AbstractQoreNode *f_save_thread_data(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->type != NT_HASH && p0->type != NT_STRING))
      return NULL;

   QoreHash *data = getProgram()->getThreadData();
   if (p0->type == NT_HASH)
      data->merge(reinterpret_cast<const QoreHashNode *>(p0), xsink);
   else
   {
      const AbstractQoreNode *p1 = get_param(params, 1);

      data->setKeyValue(reinterpret_cast<const QoreStringNode *>(p0), p1 ? p1->RefSelf() : 0, xsink);
   }

   return NULL;
}

AbstractQoreNode *f_delete_thread_data(const QoreListNode *params, ExceptionSink *xsink)
{
   if (num_params(params))
   {
      // get thread data hash
      QoreHash *data = getProgram()->getThreadData();
      
      // iterate through arguments and delete each key
      for (int i = 0; i < params->size(); i++)
      {
	 const AbstractQoreNode *p = get_param(params, i);
	 if (p)
	 {
	    QoreStringValueHelper t(p);
	    data->deleteKey(*t, xsink);
	    if (*xsink)
	       break;
	 }
      }
   }
   return NULL;
}

AbstractQoreNode *f_delete_all_thread_data(const QoreListNode *params, ExceptionSink *xsink)
{
   // get thread data hash
   QoreHash *data = getProgram()->getThreadData();

   data->dereference(xsink);
   return NULL;
}

AbstractQoreNode *f_get_thread_data(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
      return NULL;
   QoreHash *data = getProgram()->getThreadData();
   AbstractQoreNode *v = data->getKeyValue(p0->getBuffer());
   return v ? v->RefSelf() : 0;
}

AbstractQoreNode *f_get_all_thread_data(const QoreListNode *params, ExceptionSink *xsink)
{
   return getProgram()->getThreadData()->copyNode();
}

AbstractQoreNode *f_getAllThreadCallStacks(const QoreListNode *params, ExceptionSink *xsink)
{
#ifdef DEBUG
   return getAllCallStacks();
#else
   return new QoreStringNode("getAllThreadCallStacks() not available without debugging");
#endif
}

AbstractQoreNode *f_throwThreadResourceExceptions(const QoreListNode *params, ExceptionSink *xsink)
{
   purge_thread_resources(xsink);
   return NULL;
}

void init_thread_functions()
{
   builtinFunctions.add("gettid", f_gettid);
   builtinFunctions.add("num_threads", f_num_threads);
   builtinFunctions.add("thread_list", f_thread_list);
   builtinFunctions.add("get_thread_data", f_get_thread_data, QDOM_THREAD_CONTROL);
   builtinFunctions.add("get_all_thread_data", f_get_all_thread_data, QDOM_THREAD_CONTROL);
   builtinFunctions.add("save_thread_data", f_save_thread_data, QDOM_THREAD_CONTROL);
   builtinFunctions.add("delete_thread_data", f_delete_thread_data, QDOM_THREAD_CONTROL);
   builtinFunctions.add("delete_all_thread_data", f_delete_all_thread_data, QDOM_THREAD_CONTROL);
   builtinFunctions.add("getAllThreadCallStacks", f_getAllThreadCallStacks, QDOM_THREAD_CONTROL);
   builtinFunctions.add("throwThreadResourceExceptions", f_throwThreadResourceExceptions, QDOM_THREAD_CONTROL);
}
