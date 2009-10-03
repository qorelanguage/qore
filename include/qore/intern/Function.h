/*
  Function.h

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
#define FC_STATICUSERREF  7 // only used by CallReference
#define FC_STATIC_METHOD2 8

// these data structures are all private to the library
// FIXME: messy implementation - clean up!

AbstractQoreNode *doPartialEval(class AbstractQoreNode *n, bool *is_self_ref, ExceptionSink *xsink);

// object definitions and interfaces
class BuiltinFunction {
   private:
      int type;
      const char *name;
      
   public:
      BuiltinFunction *next;
      union {
	    q_func_t func;
	    q_static_method2_t static_method;
	    q_method_t method;
	    q_method2_t method2;
	    q_constructor_t constructor;
	    q_constructor2_t constructor2;
	    q_system_constructor_t system_constructor;
	    q_system_constructor2_t system_constructor2;
	    q_destructor_t destructor;
	    q_destructor2_t destructor2;
	    q_copy_t copy;
	    q_copy2_t copy2;
	    q_delete_blocker_t delete_blocker;
      } code;
      
      DLLLOCAL BuiltinFunction(const char *nme, q_func_t f, int typ);
      DLLLOCAL BuiltinFunction(const char *nme, q_static_method2_t f, int typ);
      DLLLOCAL BuiltinFunction(const char *nme, q_method_t m, int typ);
      DLLLOCAL BuiltinFunction(const char *nme, q_method2_t m, int typ);
      DLLLOCAL BuiltinFunction(q_constructor_t m, int typ);
      DLLLOCAL BuiltinFunction(q_constructor2_t m, int typ);
      DLLLOCAL BuiltinFunction(q_system_constructor_t m, int typ);
      DLLLOCAL BuiltinFunction(q_system_constructor2_t m, int typ);
      DLLLOCAL BuiltinFunction(q_destructor_t m, int typ);
      DLLLOCAL BuiltinFunction(q_destructor2_t m, int typ);
      DLLLOCAL BuiltinFunction(q_copy_t m, int typ);
      DLLLOCAL BuiltinFunction(q_copy2_t m, int typ);
      DLLLOCAL BuiltinFunction(q_delete_blocker_t m);
      DLLLOCAL AbstractQoreNode *evalMethod(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL AbstractQoreNode *evalMethod(const QoreMethod &method, QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL void evalConstructor(QoreObject *self, const QoreListNode *args, BCList *bcl, BCEAList *bceal, const char *class_name, ExceptionSink *xsink) const;
      DLLLOCAL void evalConstructor2(const QoreClass &thisclass, QoreObject *self, const QoreListNode *args, BCList *bcl, BCEAList *bceal, const char *class_name, ExceptionSink *xsink) const;
      DLLLOCAL void evalDestructor(const QoreClass &thisclass, QoreObject *self, AbstractPrivateData *private_data, const char *class_name, bool new_calling_convention, ExceptionSink *xsink) const;
      DLLLOCAL void evalCopy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, bool new_calling_convention, ExceptionSink *xsink) const;
      DLLLOCAL bool evalDeleteBlocker(QoreObject *self, AbstractPrivateData *private_data) const;
      DLLLOCAL void evalSystemConstructor(const QoreClass &thisclass, bool new_calling_convention, QoreObject *self, int code, va_list args) const;
      DLLLOCAL void evalSystemDestructor(const QoreClass &thisclass, bool new_calling_convention, QoreObject *self, AbstractPrivateData *private_data, ExceptionSink *xsink) const;
      DLLLOCAL AbstractQoreNode *evalWithArgs(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL AbstractQoreNode *eval(const QoreListNode *args, ExceptionSink *xsink, const char *class_name = 0) const;
      DLLLOCAL AbstractQoreNode *evalStatic2(const QoreMethod &method, const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL int getType() const { return type; }
      DLLLOCAL const char *getName() const { return name; }
};

class LocalVar;

class Paramlist {
   public:
      int num_params;
      char **names;
      LocalVar **lv;
      LocalVar *argvid;
      LocalVar *selfid;

      DLLLOCAL Paramlist(AbstractQoreNode *params);
      DLLLOCAL ~Paramlist();
};

class VRMutex;

class UserFunction : public QoreReferenceCounter
{
   private:
      bool synchronized;
      // for "synchronized" functions
      VRMutex *gate;
      char *name;

   protected:
      DLLLOCAL ~UserFunction();

   public:
      Paramlist *params;
      StatementBlock *statements;

      // the object owns the memory for "n_name", 0 for anonymous closures
      DLLLOCAL UserFunction(char *n_name, Paramlist *parms, StatementBlock *states, bool synced = false);
      DLLLOCAL AbstractQoreNode *eval(const QoreListNode *args, QoreObject *self, ExceptionSink *xsink, const char *class_name = 0) const;
      DLLLOCAL AbstractQoreNode *evalConstructor(const QoreListNode *args, QoreObject *self, class BCList *bcl, class BCEAList *scbceal, const char *class_name, ExceptionSink *xsink) const;
      DLLLOCAL void evalCopy(QoreObject *old, QoreObject *self, const char *class_name, ExceptionSink *xsink) const;
      DLLLOCAL bool isSynchronized() const 
      { 
	 return synchronized; 
      }
      DLLLOCAL void deref();
      DLLLOCAL const char *getName() const 
      {
	 return name ? name : "<anonymous closure>";
      }
};

#endif // _QORE_FUNCTION_H
