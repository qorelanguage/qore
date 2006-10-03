/*
  ql_thread.cc

  POSIX thread library for Qore

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/ql_thread.h>
#include <qore/common.h>
#include <qore/Exception.h>
#include <qore/QoreType.h>
#include <qore/support.h>
#include <qore/qore_thread.h>
#include <qore/Object.h>
#include <qore/params.h>
#include <qore/QoreProgram.h>
#include <qore/BuiltinFunctionList.h>

#include <pthread.h>

class QoreNode *f_gettid(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, gettid());
}

extern int num_threads;
class QoreNode *f_num_threads(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, num_threads);
}

class QoreNode *f_thread_list(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(get_thread_list());
}

class QoreNode *f_save_thread_data(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->type != NT_HASH && p0->type != NT_STRING))
      return NULL;

   Hash *data = getProgram()->getThreadData();
   if (p0->type == NT_HASH)
      data->merge(p0->val.hash, xsink);
   else
   {
      QoreNode *p1 = get_param(params, 1);
      if (p1)
	 p1->RefSelf();

      data->setKeyValue(p0->val.String, p1, xsink);
   }

   return NULL;
}

class QoreNode *f_delete_thread_data(class QoreNode *params, ExceptionSink *xsink)
{
   if (num_params(params))
   {
      // get thread data hash
      Hash *data = getProgram()->getThreadData();
      
      // iterate through arguments and delete each key
      for (int i = 0; i < params->val.list->size(); i++)
      {
	 QoreNode *p = get_param(params, i);
	 if (p)
	 {
	    if (p->type != NT_STRING)
	    {
	       QoreNode *t = p->convert(NT_STRING);
	       data->deleteKey(t->val.String, xsink);
	       t->deref(xsink);
	    }
	    else
	       data->deleteKey(p->val.String, xsink);
	    
	    if (xsink->isEvent())
	       break;
	 }
      }
   }
   return NULL;
}

class QoreNode *f_delete_all_thread_data(class QoreNode *params, ExceptionSink *xsink)
{
   // get thread data hash
   Hash *data = getProgram()->getThreadData();

   data->dereference(xsink);
   return NULL;
}

class QoreNode *f_get_thread_data(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;
   Hash *data = getProgram()->getThreadData();
   QoreNode *v = data->getKeyValue(p0->val.String->getBuffer());
   if (v)
   {
      v = v->eval(xsink);
      if (xsink->isEvent())
      {
	 if (v)
	    v->deref(xsink);
	 return NULL;
      }
   }
   return v;
}

class QoreNode *f_get_all_thread_data(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(getProgram()->getThreadData()->copy());
}

class QoreNode *f_getAllThreadCallStacks(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(getAllCallStacks());
}

class QoreNode *f_throwThreadResourceExceptions(class QoreNode *params, ExceptionSink *xsink)
{
   trlist.purgeTID(gettid(), xsink);
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
