/*
 ql_object.cc
 
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
#include <qore/Exception.h>
#include <qore/params.h>
#include <qore/QoreClass.h>
#include <qore/QoreNode.h>
#include <qore/Object.h>
#include <qore/qore_thread.h>
#include <qore/ql_object.h>

// returns a list of method names for the object passed as a parameter
static QoreNode *f_getMethodList(QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   if (!p0)
      return NULL;
   
   return new QoreNode(p0->val.object->getClass()->getMethodList());
}

static QoreNode *f_callObjectMethod(QoreNode *params, ExceptionSink *xsink)
{
   // get object
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   if (!p0)
      return NULL;
   
   // get method name
   QoreNode *p1 = test_param(params, NT_STRING, 1);
   if (!p1)
      return NULL;
   
   QoreNode *args;
   
   // if there are arguments to pass
   if (get_param(params, 2))
   {
      // create argument list by copying current list
      List *l = params->val.list->copyListFrom(2);
      if (xsink->isEvent())
      {
         if (l)
	    l->derefAndDelete(xsink);
         return NULL;
      }
      args = new QoreNode(l);
   }
   else
      args = NULL;
   
   // make sure method call is internal (allows access to private methods) if this function was called internally
   substituteObjectIfEqual(p0->val.object);
   QoreNode *rv = p0->val.object->evalMethod(p1->val.String, args, xsink);
   if (args)
      args->deref(xsink);
   return rv;
}

static QoreNode *f_callObjectMethodArgs(QoreNode *params, ExceptionSink *xsink)
{
   // get object
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   if (!p0)
      return NULL;
   
   // get method name
   QoreNode *p1 = test_param(params, NT_STRING, 1);
   if (!p1)
      return NULL;
   
   QoreNode *args, *p2;
   
   // if there are arguments to pass
   if ((p2 = get_param(params, 2)))
   {
      if (p2->type == NT_LIST)
	 args = p2;
      else
      {
	 args = new QoreNode(new List());
	 args->val.list->push(p2);
      }
   }
   else
      args = NULL;
   
   // make sure method call is internal (allows access to private methods) if this function was called internally
   substituteObjectIfEqual(p0->val.object);
   QoreNode *rv = p0->val.object->evalMethod(p1->val.String, args, xsink);
   
   if (p2 != args)
   {
      args->val.list->shift();
      args->deref(xsink);
   }
   
   return rv;
}

void init_object_functions()
{
   tracein("init_object_functions()");
   
   builtinFunctions.add("getMethodList", f_getMethodList);
   builtinFunctions.add("callObjectMethod", f_callObjectMethod);
   builtinFunctions.add("callObjectMethodArgs", f_callObjectMethodArgs);
   traceout("init_object_functions()");
}
