/*
  BuiltinFunction.cc

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
#include <qore/intern/QoreClassIntern.h>

AbstractQoreNode *BuiltinFunction::eval(const QoreListNode *args, ExceptionSink *xsink) const {
   AbstractQoreNode *rv;
   ExceptionSink newsink;

   QORE_TRACE("BuiltinFunction::eval(Node)");
   printd(3, "BuiltinFunction::eval(Node) calling builtin function \"%s\"\n", name);
   
   //printd(5, "BuiltinFunction::eval(Node) args=%08p %s\n", args, args ? args->getTypeName() : "(null)");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   QoreListNodeEvalOptionalRefHolder tmp(args, xsink);
   if (*xsink)
      return 0;

   //printd(5, "BuiltinFunction::eval(Node) after eval tmp args=%08p %s\n", *tmp, *tmp ? *tmp->getTypeName() : "(null)");

   {
      CodeContextHelper cch(name, 0, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh(name, CT_BUILTIN, 0, xsink);
#endif

      // execute the function if no new exception has happened
      // necessary only in the case of a builtin object destructor
      if (!newsink.isEvent())
	 rv = func(*tmp, xsink);
      else
	 rv = 0;

      xsink->assimilate(&newsink);
   }

   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, 0, name, o_fn, o_ln, o_eln);

   return rv;
}

void BuiltinConstructor::eval(QoreObject *self, const QoreListNode *args, BCList *bcl, BCEAList *bceal, const char *class_name, ExceptionSink *xsink) const {
   QORE_TRACE("BuiltinConstructor::eval()");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   CodeContextHelper cch(name, self, xsink);

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   // push call on call stack
   CallStackHelper csh(name, CT_BUILTIN, self, xsink);
#endif

   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);
   
   if (!xsink->isEvent()) {
      constructor(self, args, xsink);
      if (xsink->isException())
	 xsink->addStackInfo(CT_BUILTIN, class_name, name, o_fn, o_ln, o_eln);
   }
}

void BuiltinConstructor2::eval(const QoreClass &thisclass, QoreObject *self, const QoreListNode *args, class BCList *bcl, class BCEAList *bceal, const char *class_name, ExceptionSink *xsink) const {
   QORE_TRACE("BuiltinConstructor2::eval()");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   CodeContextHelper cch(name, self, xsink);

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   // push call on call stack
   CallStackHelper csh(name, CT_BUILTIN, self, xsink);
#endif

   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);
   
   if (!xsink->isEvent()) {
      constructor(thisclass, self, args, xsink);
      if (xsink->isException())
	 xsink->addStackInfo(CT_BUILTIN, class_name, name, o_fn, o_ln, o_eln);
   }
}

void BuiltinDestructor::eval(QoreObject *self, AbstractPrivateData *private_data, const char *class_name, ExceptionSink *xsink) const {
   QORE_TRACE("BuiltinDestructor::eval()");
   
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   {
      CodeContextHelper cch(name, self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh(name, CT_BUILTIN, self, xsink);
#endif

      destructor(self, private_data, xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, class_name, name, o_fn, o_ln, o_eln);
}

void BuiltinDestructor2::eval(const QoreClass &thisclass, QoreObject *self, AbstractPrivateData *private_data, const char *class_name, ExceptionSink *xsink) const {
   QORE_TRACE("BuiltinDestructor2::eval()");
   
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   {
      CodeContextHelper cch(name, self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh(name, CT_BUILTIN, self, xsink);
#endif

      destructor(thisclass, self, private_data, xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, class_name, name, o_fn, o_ln, o_eln);
}

void BuiltinCopy::eval(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink) const {
   QORE_TRACE("BuiltinCopy::eval()");
   
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
   
   {
      CodeContextHelper cch("copy", self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh("copy", CT_BUILTIN, self, xsink);
#endif

      copy(self, old, private_data, xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, thisclass.getName(), "copy", o_fn, o_ln, o_eln);
}

void BuiltinCopy2::eval(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink) const {
   QORE_TRACE("BuiltinCopy2::eval()");
   
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
   
   {
      CodeContextHelper cch(name, self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh(name, CT_BUILTIN, self, xsink);
#endif

      copy(thisclass, self, old, private_data, xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, thisclass.getName(), name, o_fn, o_ln, o_eln);
}

AbstractQoreNode *BuiltinNormalMethod::eval(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const {
   printd(2, "BuiltinNormalMethod::eval() calling builtin func '%s' old calling convention obj=%08p data=%08p\n", name, self, private_data);

   CodeContextHelper cch(name, self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   // push call on call stack in debugging mode
   CallStackHelper csh(name, CT_BUILTIN, self, xsink);
#endif

   // exception information added at the level above
   // (program location must be saved before arguments are evaluated)
   return method(self, private_data, args, xsink);
}

AbstractQoreNode *BuiltinNormalMethod2::eval(const QoreMethod &qmethod, QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const {
   printd(2, "BuiltinNormalMethod2::evalMethod() calling builtin func '%s' new calling convention obj=%08p data=%08p\n", name, self, private_data);

   CodeContextHelper cch(name, self, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   // push call on call stack in debugging mode
   CallStackHelper csh(name, CT_BUILTIN, self, xsink);
#endif

   // exception information added at the level above
   // (program location must be saved before arguments are evaluated)
   return method(qmethod, self, private_data, args, xsink);
}

AbstractQoreNode *BuiltinStaticMethod::eval(const QoreMethod &method, const QoreListNode *args, ExceptionSink *xsink) const {
   AbstractQoreNode *rv;
   ExceptionSink newsink;

   QORE_TRACE("BuiltinStaticMethod::eval(Node)");
   printd(3, "BuiltinStaticMethod::eval(Node) calling builtin function \"%s\"\n", name);
   
   //printd(5, "BuiltinFunction::eval(Node) args=%08p %s\n", args, args ? args->getTypeName() : "(null)");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   QoreListNodeEvalOptionalRefHolder tmp(args, xsink);
   if (*xsink)
      return 0;

   //printd(5, "BuiltinFunction::eval(Node) after eval tmp args=%08p %s\n", *tmp, *tmp ? *tmp->getTypeName() : "(null)");

   {
      CodeContextHelper cch(name, 0, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh(name, CT_BUILTIN, 0, xsink);
#endif

      // execute the function if no new exception has happened
      // necessary only in the case of a builtin object destructor
      rv = !newsink.isEvent() ? static_method(*tmp, xsink) : 0;

      xsink->assimilate(&newsink);
   }

   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, method.getClass()->getName(), name, o_fn, o_ln, o_eln);

   return rv;
}

AbstractQoreNode *BuiltinStaticMethod2::eval(const QoreMethod &method, const QoreListNode *args, ExceptionSink *xsink) const {
   AbstractQoreNode *rv;
   ExceptionSink newsink;

   QORE_TRACE("BuiltinStaticMethod2:eval()");
   printd(3, "BuiltinStaticMethod2::eval() calling builtin function \"%s\"\n", name);
   
   //printd(5, "BuiltinFunction::eval(Node) args=%08p %s\n", args, args ? args->getTypeName() : "(null)");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   QoreListNodeEvalOptionalRefHolder tmp(args, xsink);
   if (*xsink)
      return 0;

   //printd(5, "BuiltinFunction::eval(Node) after eval tmp args=%08p %s\n", *tmp, *tmp ? *tmp->getTypeName() : "(null)");

   {
      CodeContextHelper cch(name, 0, xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // push call on call stack
      CallStackHelper csh(name, CT_BUILTIN, 0, xsink);
#endif

      // execute the function if no new exception has happened
      // necessary only in the case of a builtin object destructor
      rv = !newsink.isEvent() ? static_method(method, *tmp, xsink) :  0;

      xsink->assimilate(&newsink);
   }

   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, method.getClass()->getName(), name, o_fn, o_ln, o_eln);

   return rv;
}
