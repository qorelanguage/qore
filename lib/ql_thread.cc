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

class QoreNode *f_gettid(const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)gettid());
}

extern int num_threads;
class QoreNode *f_num_threads(const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)num_threads);
}

class QoreNode *f_thread_list(const QoreList *params, ExceptionSink *xsink)
{
   return get_thread_list();
}

class QoreNode *f_save_thread_data(const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->type != NT_HASH && p0->type != NT_STRING))
      return NULL;

   QoreHash *data = getProgram()->getThreadData();
   if (p0->type == NT_HASH)
      data->merge(reinterpret_cast<QoreHashNode *>(p0), xsink);
   else
   {
      QoreNode *p1 = get_param(params, 1);
      if (p1)
	 p1->RefSelf();

      data->setKeyValue(((QoreStringNode *)p0), p1, xsink);
   }

   return NULL;
}

class QoreNode *f_delete_thread_data(const QoreList *params, ExceptionSink *xsink)
{
   if (num_params(params))
   {
      // get thread data hash
      QoreHash *data = getProgram()->getThreadData();
      
      // iterate through arguments and delete each key
      for (int i = 0; i < params->size(); i++)
      {
	 QoreNode *p = get_param(params, i);
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

class QoreNode *f_delete_all_thread_data(const QoreList *params, ExceptionSink *xsink)
{
   // get thread data hash
   QoreHash *data = getProgram()->getThreadData();

   data->dereference(xsink);
   return NULL;
}

class QoreNode *f_get_thread_data(const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
      return NULL;
   QoreHash *data = getProgram()->getThreadData();
   QoreNode *v = data->getKeyValue(p0->getBuffer());
   return v ? v->RefSelf() : 0;
}

class QoreNode *f_get_all_thread_data(const QoreList *params, ExceptionSink *xsink)
{
   return getProgram()->getThreadData()->copyNode();
}

class QoreNode *f_getAllThreadCallStacks(const QoreList *params, ExceptionSink *xsink)
{
#ifdef DEBUG
   return getAllCallStacks();
#else
   return new QoreStringNode("getAllThreadCallStacks() not available without debugging");
#endif
}

class QoreNode *f_throwThreadResourceExceptions(const QoreList *params, ExceptionSink *xsink)
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
