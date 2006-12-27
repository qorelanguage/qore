/*
  Function.h

  Qore programming language

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

#ifndef _QORE_FUNCTION_H

#define _QORE_FUNCTION_H

#include <qore/ReferenceObject.h>
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

// these data structures are all private to the library
// FIXME: messy implementation - clean up!

class QoreNode *doPartialEval(class QoreNode *n, bool *is_self_ref, class ExceptionSink *xsink);

class ImportedFunctionCall {
   public:
      class QoreProgram *pgm;
      class UserFunction *func;

      DLLLOCAL ImportedFunctionCall(class QoreProgram *p, class UserFunction *f) { pgm = p; func = f; }
      DLLLOCAL class QoreNode *eval(class QoreNode *args, class ExceptionSink *xsink);
};

class SelfFunctionCall {
  public:
      char *name;
      class NamedScope *ns;
      class Method *func;

      DLLLOCAL SelfFunctionCall(char *n);
      DLLLOCAL SelfFunctionCall(class NamedScope *n);
      DLLLOCAL SelfFunctionCall(class Method *f);
      DLLLOCAL ~SelfFunctionCall();
      DLLLOCAL class QoreNode *eval(class QoreNode *args, class ExceptionSink *xsink);
      DLLLOCAL void resolve();
};

class FunctionCall {
   private:

   public:
      union uFCall {
	    class UserFunction *ufunc;
	    class BuiltinFunction *bfunc;
	    class SelfFunctionCall *sfunc;
	    class ImportedFunctionCall *ifunc;
	    char *c_str;
      } f;
      class QoreNode *args;
      int type;

      DLLLOCAL FunctionCall(class UserFunction *u, class QoreNode *a);
      DLLLOCAL FunctionCall(class BuiltinFunction *b, class QoreNode *a);

      // "self" in-object function call constructors
      DLLLOCAL FunctionCall(class QoreNode *a, char *name);
      DLLLOCAL FunctionCall(class QoreNode *a, class NamedScope *n);
      DLLLOCAL FunctionCall(class Method *func, class QoreNode *a);

      // normal function call constructor
      DLLLOCAL FunctionCall(char *name, class QoreNode *a);

      DLLLOCAL FunctionCall(class QoreProgram *p, class UserFunction *u, class QoreNode *a);
      DLLLOCAL ~FunctionCall();

      // to transform an "unresolved" function to a "method" type
      DLLLOCAL void parseMakeMethod();
      DLLLOCAL class QoreNode *parseMakeNewObject();
      DLLLOCAL class QoreNode *eval(class ExceptionSink *);
      DLLLOCAL int existsUserParam(int i) const;
      DLLLOCAL int getType() const;
      DLLLOCAL char *getName() const;
};

// object definitions and interfaces
class BuiltinFunction
{
   private:
      int type;

   public:
      char *name;
      class BuiltinFunction *next;
      union {
	    q_func_t func;
	    q_method_t method;
	    q_constructor_t constructor;
	    q_destructor_t destructor;
	    q_copy_t copy;
      } code;

      DLLLOCAL BuiltinFunction(char *nme, q_func_t f, int typ);
      DLLLOCAL BuiltinFunction(char *nme, q_method_t m, int typ);
      DLLLOCAL BuiltinFunction(q_constructor_t m, int typ);
      DLLLOCAL BuiltinFunction(q_destructor_t m, int typ);
      DLLLOCAL BuiltinFunction(q_copy_t m, int typ);
      DLLLOCAL class QoreNode *evalMethod(class Object *self, void *private_data, class QoreNode *args, class ExceptionSink *xsink);
      DLLLOCAL void evalConstructor(class Object *self, class QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL void evalDestructor(class Object *self, void *private_data, class ExceptionSink *xsink);
      DLLLOCAL void evalCopy(class Object *self, class Object *old, void *private_data, class ExceptionSink *xsink);
      DLLLOCAL void evalSystemConstructor(class Object *self, class QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink);
      DLLLOCAL void evalSystemDestructor(class Object *self, void *private_data, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *evalWithArgs(class Object *self, class QoreNode *args, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *eval(class QoreNode *args, class ExceptionSink *xsink);
      DLLLOCAL int getType() const { return type; }
      DLLLOCAL char *getName() const { return name; }
};

class Paramlist {
   public:
      int num_params;
      char **names;
      lvh_t *ids;
      lvh_t argvid;
      lvh_t selfid;

      DLLLOCAL Paramlist(class QoreNode *params);
      DLLLOCAL ~Paramlist();
};

class UserFunction : public ReferenceObject
{
   private:
      bool synchronized;
      // for "synchronized" functions
      class VRMutex *gate;

   protected:
      DLLLOCAL ~UserFunction();

   public:
      char *name;
      class Paramlist *params;
      class StatementBlock *statements;

      DLLLOCAL UserFunction(char *nme, class Paramlist *parms, class StatementBlock *states, bool synced = false);
      DLLLOCAL class QoreNode *eval(class QoreNode *args, class Object *self, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *evalConstructor(class QoreNode *args, class Object *self, class BCList *bcl, class BCEAList *scbceal, class ExceptionSink *xsink);
      DLLLOCAL void evalCopy(class Object *old, class Object *self, class ExceptionSink *xsink);
      DLLLOCAL bool isSynchronized() const 
      { 
	 return synchronized; 
      }
      DLLLOCAL void deref();
      DLLLOCAL char *getName() const 
      {
	 return name;
      }
};

#endif // _QORE_FUNCTION_H
