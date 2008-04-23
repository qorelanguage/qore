/*
  Variable.h

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

#ifndef _QORE_VARIABLE_H

#define _QORE_VARIABLE_H

#define VT_UNRESOLVED 1
#define VT_LOCAL      2
#define VT_GLOBAL     3
#define VT_OBJECT     4  // used for references only

#define GV_VALUE  1
#define GV_IMPORT 2

#include <qore/intern/VRMutex.h>

#include <string.h>
#include <stdlib.h>

#include <string>

#ifndef QORE_THREAD_STACK_BLOCK
#define QORE_THREAD_STACK_BLOCK 128
#endif

class Var;

union VarValue {
      // for value
      struct {
	    AbstractQoreNode *value;
	    char *name;
      } val;
      // for imported variables
      struct {
	    Var *refptr;
	    bool readonly;
      } ivar;
};

// structure for global variables
class Var : public QoreReferenceCounter
{
   private:
      unsigned char type;
      // holds the value of the variable or a pointer to the imported variable
      union VarValue v;
      mutable VRMutex gate;

      DLLLOCAL void del(ExceptionSink *xsink);

   protected:
      DLLLOCAL ~Var() {}

   public:
      DLLLOCAL Var(const char *nme, AbstractQoreNode *val = NULL);
      DLLLOCAL Var(Var *ref, bool ro = false);
      DLLLOCAL const char *getName() const;
      DLLLOCAL void setValue(AbstractQoreNode *val, ExceptionSink *xsink);
      DLLLOCAL void makeReference(Var *v, ExceptionSink *xsink, bool ro = false);
      DLLLOCAL bool isImported() const;
      DLLLOCAL void deref(ExceptionSink *xsink);
      DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink);
      DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, ExceptionSink *xsink) const;
      DLLLOCAL AbstractQoreNode *getValue(AutoVLock *vl, ExceptionSink *xsink);
};

DLLLOCAL AbstractQoreNode *getNoEvalVarValue(AbstractQoreNode *n, AutoVLock *vl, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *getExistingVarValue(const AbstractQoreNode *n, ExceptionSink *xsink, AutoVLock *vl, ReferenceHolder<AbstractQoreNode> &pt);
DLLLOCAL void delete_var_node(AbstractQoreNode *node, ExceptionSink *xsink);
DLLLOCAL void delete_global_variables();

// for retrieving a pointer to a pointer to an lvalue expression
DLLLOCAL AbstractQoreNode **get_var_value_ptr(const AbstractQoreNode *lvalue, AutoVLock *vl, ExceptionSink *xsink);

DLLLOCAL extern QoreHashNode *ENV;

class LValueHelper {
   private:
      AbstractQoreNode **v;
      ExceptionSink *xsink;
      AutoVLock vl;

   public:
      DLLLOCAL LValueHelper(const AbstractQoreNode *exp, ExceptionSink *n_xsink) : xsink(n_xsink), vl(n_xsink)
      {
	 v = get_var_value_ptr(exp, &vl, xsink);
      }
      DLLLOCAL operator bool() const { return v != 0; }
      DLLLOCAL const qore_type_t get_type() const { return *v ? (*v)->getType() : NT_NOTHING; }
      DLLLOCAL bool check_type(const qore_type_t t) const
      {
	 if (!(*v))
	    if (t == NT_NOTHING)
	       return true;
	    else
	       return false;
	 return t == (*v)->getType();
      }
      DLLLOCAL bool is_nothing() const { return ::is_nothing(*v); }
      DLLLOCAL AbstractQoreNode *get_value() { return *v; }
      DLLLOCAL AbstractQoreNode *take_value() { AbstractQoreNode *rv = *v; *v = 0; return rv; }
      DLLLOCAL int assign(AbstractQoreNode *val)
      {
	 if (*v) {
	    (*v)->deref(xsink);
	    if (*xsink) {
	       (*v) = 0;
	       discard(val, xsink);
	       return -1;
	    }
	 }
	 (*v) = val;
	 return 0;
      }
      DLLLOCAL int ensure_unique()
      {
	 assert((*v) && (*v)->getType() != NT_OBJECT);

	 if (!(*v)->is_unique())
	 {
	    AbstractQoreNode *old = *v;
	    (*v) = old->realCopy();
	    old->deref(xsink);
	    return *xsink; 
	 }
	 return 0;
      }

      DLLLOCAL int ensure_unique_int()
      {
	 if (!(*v)) {
	    (*v) = new QoreBigIntNode();
	    return 0;
	 }

	 if ((*v)->getType() != NT_INT) {
	    int64 i = (*v)->getAsBigInt();
	    (*v)->deref(xsink);
	    if (*xsink) {
	       (*v) = 0;
	       return -1;
	    }
	    (*v) = new QoreBigIntNode(i);
	    return 0;
	 }
	 
	 if ((*v)->is_unique())
	    return 0;

	 QoreBigIntNode *old = reinterpret_cast<QoreBigIntNode *>(*v);
	 (*v) = old->realCopy();
	 old->deref();
	 return 0;
      }

      DLLLOCAL int ensure_unique_float()
      {
	 if (!(*v)) {
	    (*v) = new QoreFloatNode();
	    return 0;
	 }

	 if ((*v)->getType() != NT_FLOAT) {
	    double f = (*v)->getAsFloat();
	    (*v)->deref(xsink);
	    if (*xsink) {
	       (*v) = 0;
	       return -1;
	    }
	    (*v) = new QoreFloatNode(f);
	    return 0;
	 }
	 
	 if ((*v)->is_unique())
	    return 0;

	 QoreFloatNode *old = reinterpret_cast<QoreFloatNode *>(*v);
	 (*v) = old->realCopy();
	 old->deref();
	 return 0;
      }
};

#endif // _QORE_VARIABLE_H
