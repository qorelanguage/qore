/*
  Function.h

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

#ifndef _QORE_FUNCTION_H

#define _QORE_FUNCTION_H

#include <qore/QoreReferenceCounter.h>
#include <qore/Restrictions.h>
#include <qore/common.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constructors and destructors can never be explicitly called so we don't need FunctionCall constants for them
#define FC_UNRESOLVED     1
#define FC_USER           2
#define FC_BUILTIN        3
#define FC_SELF           4
#define FC_IMPORTED       5
#define FC_METHOD         6
#define FC_STATICUSERREF  7 // only used by FunctionReference

// these data structures are all private to the library
// FIXME: messy implementation - clean up!

AbstractQoreNode *doPartialEval(class AbstractQoreNode *n, bool *is_self_ref, ExceptionSink *xsink);

// object definitions and interfaces
class BuiltinFunction
{
   private:
      int type;
      const char *name;

   public:
      class BuiltinFunction *next;
      union {
	    q_func_t func;
	    q_method_t method;
	    q_constructor_t constructor;
	    q_system_constructor_t system_constructor;
	    q_destructor_t destructor;
	    q_copy_t copy;
      } code;

      DLLLOCAL BuiltinFunction(const char *nme, q_func_t f, int typ);
      DLLLOCAL BuiltinFunction(const char *nme, q_method_t m, int typ);
      DLLLOCAL BuiltinFunction(q_constructor_t m, int typ);
      DLLLOCAL BuiltinFunction(q_system_constructor_t m, int typ);
      DLLLOCAL BuiltinFunction(q_destructor_t m, int typ);
      DLLLOCAL BuiltinFunction(q_copy_t m, int typ);
      DLLLOCAL class AbstractQoreNode *evalMethod(class QoreObject *self, void *private_data, const class QoreListNode *args, class ExceptionSink *xsink) const;
      DLLLOCAL void evalConstructor(class QoreObject *self, const class QoreListNode *args, class BCList *bcl, class BCEAList *bceal, const char *class_name, class ExceptionSink *xsink) const;
      DLLLOCAL void evalDestructor(class QoreObject *self, void *private_data, const char *class_name, class ExceptionSink *xsink) const;
      DLLLOCAL void evalCopy(class QoreObject *self, class QoreObject *old, void *private_data, const char *class_name, class ExceptionSink *xsink) const;
      DLLLOCAL void evalSystemConstructor(class QoreObject *self, int code, va_list args) const;
      DLLLOCAL void evalSystemDestructor(class QoreObject *self, void *private_data, class ExceptionSink *xsink) const;
      DLLLOCAL class AbstractQoreNode *evalWithArgs(class QoreObject *self, const class QoreListNode *args, class ExceptionSink *xsink) const;
      DLLLOCAL class AbstractQoreNode *eval(const class QoreListNode *args, class ExceptionSink *xsink) const;
      DLLLOCAL int getType() const { return type; }
      DLLLOCAL const char *getName() const { return name; }
};

class Paramlist {
   public:
      int num_params;
      char **names;
      lvh_t *ids;
      lvh_t argvid;
      lvh_t selfid;

      DLLLOCAL Paramlist(class AbstractQoreNode *params);
      DLLLOCAL ~Paramlist();
};

class UserFunction : public QoreReferenceCounter
{
   private:
      bool synchronized;
      // for "synchronized" functions
      class VRMutex *gate;
      char *name;

   protected:
      DLLLOCAL ~UserFunction();

   public:
      class Paramlist *params;
      class StatementBlock *statements;

      // the object owns the memory for "nme"
      DLLLOCAL UserFunction(char *nme, class Paramlist *parms, class StatementBlock *states, bool synced = false);
      DLLLOCAL AbstractQoreNode *eval(const class QoreListNode *args, class QoreObject *self, class ExceptionSink *xsink, const char *class_name = 0) const;
      DLLLOCAL AbstractQoreNode *evalConstructor(const class QoreListNode *args, class QoreObject *self, class BCList *bcl, class BCEAList *scbceal, const char *class_name, class ExceptionSink *xsink) const;
      DLLLOCAL void evalCopy(class QoreObject *old, class QoreObject *self, const char *class_name, class ExceptionSink *xsink) const;
      DLLLOCAL bool isSynchronized() const 
      { 
	 return synchronized; 
      }
      DLLLOCAL void deref();
      DLLLOCAL const char *getName() const 
      {
	 return name;
      }
};

#endif // _QORE_FUNCTION_H
