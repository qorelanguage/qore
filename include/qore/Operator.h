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
DLLEXPORT extern class Operator *OP_ASSIGNMENT, *OP_LOG_AND, *OP_LOG_OR, *OP_LOG_LT, 
   *OP_LOG_GT, *OP_LOG_EQ, *OP_LOG_NE, *OP_LOG_LE, *OP_LOG_GE, *OP_MODULA, 
   *OP_BIN_AND, *OP_BIN_OR, *OP_BIN_NOT, *OP_BIN_XOR, *OP_MINUS, *OP_PLUS, 
   *OP_MULT, *OP_DIV, *OP_UNARY_MINUS, *OP_NOT, *OP_SHIFT_LEFT, *OP_SHIFT_RIGHT, 
   *OP_POST_INCREMENT, *OP_POST_DECREMENT, *OP_PRE_INCREMENT, *OP_PRE_DECREMENT, 
   *OP_LOG_CMP, *OP_PLUS_EQUALS, *OP_MINUS_EQUALS, *OP_AND_EQUALS, *OP_OR_EQUALS, 
   *OP_LIST_REF, *OP_OBJECT_REF, *OP_ELEMENTS, *OP_KEYS, *OP_QUESTION_MARK, 
   *OP_ABSOLUTE_EQ, *OP_ABSOLUTE_NE, *OP_REGEX_MATCH, *OP_REGEX_NMATCH, 
   *OP_OBJECT_FUNC_REF, *OP_NEW, *OP_SHIFT, *OP_POP, *OP_PUSH, *OP_EXISTS, 
   *OP_SINGLE_ASSIGN, *OP_UNSHIFT, *OP_REGEX_SUBST, *OP_LIST_ASSIGNMENT, 
   *OP_SPLICE, *OP_MODULA_EQUALS, *OP_MULTIPLY_EQUALS, *OP_DIVIDE_EQUALS,
   *OP_XOR_EQUALS, *OP_SHIFT_LEFT_EQUALS, *OP_SHIFT_RIGHT_EQUALS, *OP_INSTANCEOF,
   *OP_REGEX_TRANS, *OP_REGEX_EXTRACT, *OP_CHOMP;

typedef safe_dslist<Operator *> oplist_t;

class OperatorList : public oplist_t
{
   public:
      DLLLOCAL OperatorList();
      DLLLOCAL ~OperatorList();
      DLLLOCAL void init();
      DLLLOCAL class Operator *add(class Operator *o);
};

DLLLOCAL extern class OperatorList oplist;

typedef class QoreNode *(* op_func_t)(class QoreNode *l, class QoreNode *r, class ExceptionSink *xsink);

class OperatorFunction {
   public:
      class QoreType *ltype;
      class QoreType *rtype;
      op_func_t op_func;
};

class Operator {
   private:
      int functionsAllocated;
      int (*opMatrix)[NUM_VALUE_TYPES];
      bool effect, lvalue;
      char *name, *description;
      int args;
      int evalArgs;
      int numFunctions;
      class OperatorFunction *functions;
      
      DLLLOCAL int findFunction(class QoreType *ltype, class QoreType *rtype) const; 
      DLLLOCAL static int match(class QoreType *ntype, class QoreType *rtype);

   public:
      DLLLOCAL Operator(int arg, char *n, char *desc, int ev, bool eff, bool lv = false);
      DLLLOCAL ~Operator();
      DLLLOCAL bool hasEffect() const;
      DLLLOCAL bool needsLValue() const;
      DLLLOCAL void addFunction(class QoreType *lt, class QoreType *rt, class QoreNode *(*f)(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink));
      DLLLOCAL class QoreNode *eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL void init();
      DLLLOCAL char *getName() const;
      DLLLOCAL char *getDescription() const;
};

#endif
