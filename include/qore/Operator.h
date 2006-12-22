/*
  Operator.h

  Qore flexible operator support

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  FIXME: dynamic allocation of operators after initialization is not thread-safe

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

#ifndef _QORE_OPERATOR_H

#define _QORE_OPERATOR_H

#include <qore/safe_dslist>

// system default operators
DLLEXPORT extern class Operator *OP_ASSIGNMENT, *OP_MODULA, 
   *OP_BIN_AND, *OP_BIN_OR, *OP_BIN_NOT, *OP_BIN_XOR, *OP_MINUS, *OP_PLUS, 
   *OP_MULT, *OP_DIV, *OP_UNARY_MINUS, *OP_SHIFT_LEFT, *OP_SHIFT_RIGHT, 
   *OP_POST_INCREMENT, *OP_POST_DECREMENT, *OP_PRE_INCREMENT, *OP_PRE_DECREMENT, 
   *OP_LOG_CMP, *OP_PLUS_EQUALS, *OP_MINUS_EQUALS, *OP_AND_EQUALS, *OP_OR_EQUALS, 
   *OP_LIST_REF, *OP_OBJECT_REF, *OP_ELEMENTS, *OP_KEYS, *OP_QUESTION_MARK, 
   *OP_OBJECT_FUNC_REF, *OP_NEW, *OP_SHIFT, *OP_POP, *OP_PUSH,
   *OP_UNSHIFT, *OP_REGEX_SUBST, *OP_LIST_ASSIGNMENT, *OP_SPLICE, *OP_MODULA_EQUALS, 
   *OP_MULTIPLY_EQUALS, *OP_DIVIDE_EQUALS, *OP_XOR_EQUALS, *OP_SHIFT_LEFT_EQUALS, 
   *OP_SHIFT_RIGHT_EQUALS, *OP_REGEX_TRANS, *OP_REGEX_EXTRACT, 
   *OP_CHOMP;

DLLEXPORT extern class BoolOperator *OP_LOG_AND, *OP_LOG_OR, *OP_LOG_LT, 
   *OP_LOG_GT, *OP_LOG_EQ, *OP_LOG_NE, *OP_LOG_LE, *OP_LOG_GE, *OP_NOT, 
   *OP_ABSOLUTE_EQ, *OP_ABSOLUTE_NE, *OP_REGEX_MATCH, *OP_REGEX_NMATCH,
   *OP_EXISTS, *OP_INSTANCEOF;

typedef safe_dslist<AbstractOperator *> oplist_t;

class OperatorList : public oplist_t
{
   public:
      DLLLOCAL OperatorList();
      DLLLOCAL ~OperatorList();
      DLLLOCAL void init();
      DLLLOCAL class Operator *add(class Operator *o);
      DLLLOCAL class BoolOperator *add(class BoolOperator *o);
};

DLLLOCAL extern class OperatorList oplist;

typedef class QoreNode *(* op_func_t)(class QoreNode *l, class QoreNode *r, bool ref_rv, class ExceptionSink *xsink);
typedef bool (*op_bool_func_t)(class QoreNode *l, class QoreNode *r, bool ref_rv, class ExceptionSink *xsink);

class OperatorFunction {
   public:
      class QoreType *ltype;
      class QoreType *rtype;
      op_func_t op_func;
};

class BoolOperatorFunction {
public:
   class QoreType *ltype;
   class QoreType *rtype;
   op_bool_func_t op_func;
};

class AbstractOperator {
   protected:
      int functionsAllocated;
      int (*opMatrix)[NUM_VALUE_TYPES];
      bool effect, lvalue, ref_rv;
      char *name, *description;
      int args;
      int evalArgs;
      int numFunctions;
      
      DLLLOCAL static int match(class QoreType *ntype, class QoreType *rtype);
      DLLLOCAL virtual int findFunction(class QoreType *ltype, class QoreType *rtype) const = 0; 

   public:
      DLLLOCAL AbstractOperator(int arg, char *n, char *desc, int ev, bool eff, bool lv = false);
      DLLLOCAL virtual ~AbstractOperator();
      DLLLOCAL void init();
      DLLLOCAL bool hasEffect() const;
      DLLLOCAL bool needsLValue() const;
      DLLLOCAL virtual class QoreNode *eval(class QoreNode *l, class QoreNode *r, bool ref_rv, ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual bool bool_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const = 0;
      DLLLOCAL char *getName() const;
      DLLLOCAL char *getDescription() const;
};

class Operator : public AbstractOperator
{
private:
   class OperatorFunction *functions;

   DLLLOCAL virtual int findFunction(class QoreType *ltype, class QoreType *rtype) const; 

public:
   DLLLOCAL Operator(int arg, char *n, char *desc, int ev, bool eff, bool lv = false);
   DLLLOCAL virtual ~Operator();
   DLLLOCAL virtual void addFunction(class QoreType *lt, class QoreType *rt, op_func_t f); 
   DLLLOCAL virtual class QoreNode *eval(class QoreNode *l, class QoreNode *r, bool ref_rv, ExceptionSink *xsink) const;
   DLLLOCAL virtual bool bool_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
};

class BoolOperator : public AbstractOperator
{
private:
   class BoolOperatorFunction *functions;

   DLLLOCAL virtual int findFunction(class QoreType *ltype, class QoreType *rtype) const; 

public:
   DLLLOCAL BoolOperator(int arg, char *n, char *desc, int ev, bool eff, bool lv = false);
   DLLLOCAL virtual ~BoolOperator();
   DLLLOCAL virtual void addFunction(class QoreType *lt, class QoreType *rt, op_bool_func_t f); 
   DLLLOCAL virtual class QoreNode *eval(class QoreNode *l, class QoreNode *r, bool ref_rv, ExceptionSink *xsink) const;
   DLLLOCAL virtual bool bool_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
};

#endif
