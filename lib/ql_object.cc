/*
 ql_object.cc
 
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
#include <qore/intern/ql_object.h>

// returns a list of method names for the object passed as a parameter
static AbstractQoreNode *f_getMethodList(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p0 = test_object_param(params, 0);
   if (!p0)
      return 0;
   
   return p0->getClass()->getMethodList();
}

static AbstractQoreNode *f_callObjectMethod(const QoreListNode *params, ExceptionSink *xsink)
{
   // get object
   QoreObject *p0 = test_object_param(params, 0);
   if (!p0)
      return 0;
   
   // get method name
   const QoreStringNode *p1 = test_string_param(params, 1);
   if (!p1)
      return 0;
   
   ReferenceHolder<QoreListNode> args(xsink);
   
   // if there are arguments to pass
   if (get_param(params, 2)) {
      // create argument list by copying current list
      ReferenceHolder<QoreListNode> l(params->copyListFrom(2), xsink);
      if (*xsink)
	 return 0;
      args = l.release();
   }

   // make sure method call is internal (allows access to private methods) if this function was called internally
   CodeContextHelper cch(0, p0, xsink);
   return p0->evalMethod(p1, *args, xsink);
}

static AbstractQoreNode *f_callObjectMethodArgs(const QoreListNode *params, ExceptionSink *xsink) {
   // get object
   QoreObject *p0 = test_object_param(params, 0);
   if (!p0)
      return 0;
   
   // get method name
   const QoreStringNode *p1 = test_string_param(params, 1);
   if (!p1)
      return 0;

   ReferenceHolder<QoreListNode> args(xsink);
   const AbstractQoreNode *p2;

   // we do some dangerous playing with reference counts here to avoid potentially 
   // costly atomic operations
   bool new_args = false;
   bool targs = false;
   // if there are arguments to pass
   if ((p2 = get_param(params, 2))) {
      args = p2 && p2->getType() == NT_LIST ? const_cast<QoreListNode*>(reinterpret_cast<const QoreListNode *>(p2)) : 0;
      // if we are using the list as the argument list, then set targs = true
      // to ensure that it won't be dereferenced at the end; instead we will
      // reuse the reference count for call to the method
      if (args)
	 targs = true;
      else {
	 args = new QoreListNode();
	 args->push(const_cast<AbstractQoreNode *>(p2));
	 // set new_args = true because we are reusing the reference count of p2
	 // for the argument list; true means do not dereference
	 new_args = true;
      }
   }
   
   // make sure method call is internal (allows access to private methods) if this function was called internally
   AbstractQoreNode *rv;
   {
      CodeContextHelper cch(0, p0, xsink);
      rv = p0->evalMethod(p1, *args, xsink);
   }

   // remove value (and borrowed reference) from list if necessary
   if (new_args)
      args->shift();
   else if (targs)
      args.release();
   
   return rv;
}

void init_object_functions() {
   QORE_TRACE("init_object_functions()");
   
   builtinFunctions.add("getMethodList", f_getMethodList);
   builtinFunctions.add("callObjectMethod", f_callObjectMethod);
   builtinFunctions.add("callObjectMethodArgs", f_callObjectMethodArgs);
}
