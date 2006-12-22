/*
  Variable.h

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

#ifndef _QORE_VARIABLE_H

#define _QORE_VARIABLE_H

#define VT_UNRESOLVED 1
#define VT_LOCAL      2
#define VT_GLOBAL     3
#define VT_OBJECT     4  // used for references only

#define GV_VALUE  1
#define GV_IMPORT 2

#include <qore/common.h>
#include <qore/ReferenceObject.h>
#include <qore/VRMutex.h>

#include <string.h>
#include <stdlib.h>

// structure for local variables
class LVar {
   private:
      class QoreNode *value;
      class QoreNode *vexp;  // partially evaluated lvalue expression for references
      class Object *obj;     // for references to object members

      DLLLOCAL class QoreNode *evalReference(class ExceptionSink *xsink);
      
   protected:
      DLLLOCAL ~LVar() {}

   public:
      lvh_t id;
      class LVar *next;

      DLLLOCAL LVar(lvh_t nid, class QoreNode *nvalue);
      DLLLOCAL LVar(lvh_t nid, class QoreNode *ve, class Object *o);
      DLLLOCAL class QoreNode **getValuePtr(class VLock *vl, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *getValue(class VLock *vl, class ExceptionSink *xsink);
      DLLLOCAL void setValue(class QoreNode *val, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *eval(class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *eval(bool &needs_deref, class ExceptionSink *xsink);
      DLLLOCAL bool checkRecursiveReference(lvh_t nid);
      DLLLOCAL void deref(class ExceptionSink *xsink);
};

union VarValue {
      // for value
      struct {
	    class QoreNode *value;
	    char *name;
      } val;
      // for imported variables
      struct {
	    class Var *refptr;
	    bool readonly;
      } ivar;
};

// structure for global variables
class Var : public ReferenceObject
{
   private:
      int type;
      // holds the value of the variable or a pointer to the imported variable
      union VarValue v;
      class VRMutex gate;

      DLLLOCAL void del(class ExceptionSink *xsink);

   protected:
      DLLLOCAL ~Var() {}

   public:
      DLLLOCAL Var(char *nme, class QoreNode *val = NULL);
      DLLLOCAL Var(class Var *ref, bool ro = false);
      DLLLOCAL char *getName() const;
      DLLLOCAL void setValue(class QoreNode *val, class ExceptionSink *xsink);
      DLLLOCAL void makeReference(class Var *v, class ExceptionSink *xsink, bool ro = false);
      DLLLOCAL bool isImported() const;
      DLLLOCAL void deref(class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *eval();
      DLLLOCAL class QoreNode **getValuePtr(class VLock *vl, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *getValue(class VLock *vl, class ExceptionSink *xsink);
};

class VarRef {
   public:
      char *name;
      int type;
      union var_u {
	    lvh_t id;          // for local variables
	    class Var *var;    // for global variables
      } ref;

      DLLLOCAL VarRef(char *n, int t);
      DLLLOCAL VarRef() {}
      DLLLOCAL ~VarRef();
      DLLLOCAL void resolve();
      // returns -1 if the variable did not already exist
      DLLLOCAL int resolveExisting();
      DLLLOCAL class VarRef *copy();
      DLLLOCAL class QoreNode *eval(class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *eval(bool &needs_deref, class ExceptionSink *xsink);
      DLLLOCAL void setValue(class QoreNode *val, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode **getValuePtr(class VLock *vl, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *getValue(class VLock *vl, class ExceptionSink *xsink);
};

DLLLOCAL class QoreNode **get_var_value_ptr(class QoreNode *lvalue, class VLock *vl, class ExceptionSink *xsink);
DLLLOCAL class QoreNode *getNoEvalVarValue(class QoreNode *n, class VLock *vl, class ExceptionSink *xsink);
DLLLOCAL class QoreNode *getExistingVarValue(class QoreNode *n, class ExceptionSink *xsink, class VLock *vl, class QoreNode **pt);
DLLLOCAL void delete_var_node(class QoreNode *node, class ExceptionSink *xsink);
DLLLOCAL void delete_global_variables();
DLLLOCAL class LVar *instantiateLVar(lvh_t id, class QoreNode *value);
DLLLOCAL class LVar *instantiateLVar(lvh_t id, class QoreNode *ve, class Object *o);
DLLLOCAL void uninstantiateLVar(class ExceptionSink *xsink);
DLLLOCAL class LVar *find_lvar(lvh_t id);

#endif // _QORE_VARIABLE_H
