/*
  Operator.h

  Qore flexible operator support

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

#ifndef _QORE_OPERATOR_H

#define _QORE_OPERATOR_H

#include <qore/safe_dslist>
#include <qore/node_types.h>
#include <vector>

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
   *OP_CHOMP, *OP_LOG_AND, *OP_LOG_OR, *OP_LOG_LT, 
   *OP_LOG_GT, *OP_LOG_EQ, *OP_LOG_NE, *OP_LOG_LE, *OP_LOG_GE, *OP_NOT, 
   *OP_ABSOLUTE_EQ, *OP_ABSOLUTE_NE, *OP_REGEX_MATCH, *OP_REGEX_NMATCH,
   *OP_EXISTS, *OP_INSTANCEOF;

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

typedef class QoreNode *(* op_func_t)(class QoreNode *l, class QoreNode *r, bool ref_rv, class ExceptionSink *xsink);
typedef bool (*op_bool_func_t)(class QoreNode *l, class QoreNode *r, class ExceptionSink *xsink);
typedef int64 (*op_bigint_func_t)(class QoreNode *l, class QoreNode *r, class ExceptionSink *xsink);
typedef double (*op_float_func_t)(class QoreNode *l, class QoreNode *r, class ExceptionSink *xsink);

class AbstractOperatorFunction {
   public:
      class QoreType *ltype, *rtype;

      DLLLOCAL AbstractOperatorFunction(class QoreType *lt, class QoreType *rt);
      DLLLOCAL virtual ~AbstractOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(class QoreNode *l, class QoreNode *r, bool ref_rv, class ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual bool bool_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual int64 bigint_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual double float_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const = 0;
};

class OperatorFunction : public AbstractOperatorFunction
{
   private:
      op_func_t op_func;

   public:
      DLLLOCAL OperatorFunction(class QoreType *lt, class QoreType *rt, op_func_t f);
      DLLLOCAL virtual ~OperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(class QoreNode *l, class QoreNode *r, bool ref_rv, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
};

class BoolOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bool_func_t op_func;

   public:
      DLLLOCAL BoolOperatorFunction(class QoreType *lt, class QoreType *rt, op_bool_func_t f);
      DLLLOCAL virtual ~BoolOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(class QoreNode *l, class QoreNode *r, bool ref_rv, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
};

class BigIntOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bigint_func_t op_func;

   public:
      DLLLOCAL BigIntOperatorFunction(class QoreType *lt, class QoreType *rt, op_bigint_func_t f);
      DLLLOCAL virtual ~BigIntOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(class QoreNode *l, class QoreNode *r, bool ref_rv, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
};

class FloatOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_float_func_t op_func;

   public:
      DLLLOCAL FloatOperatorFunction(class QoreType *lt, class QoreType *rt, op_float_func_t f);
      DLLLOCAL virtual ~FloatOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(class QoreNode *l, class QoreNode *r, bool ref_rv, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
};

typedef std::vector<class AbstractOperatorFunction *> opfunc_list_t;

class Operator {
   private:
      opfunc_list_t functions;
      int (*opMatrix)[NUM_VALUE_TYPES];
      bool effect, lvalue;
      char *name, *description;
      int args;
      int evalArgs;
      
      DLLLOCAL static int match(class QoreType *ntype, class QoreType *rtype);
      DLLLOCAL int findFunction(class QoreType *ltype, class QoreType *rtype) const; 

   public:
      DLLLOCAL Operator(int arg, char *n, char *desc, int ev, bool eff, bool lv = false);
      DLLLOCAL ~Operator();
      DLLLOCAL void init();
      DLLLOCAL bool hasEffect() const;
      DLLLOCAL bool needsLValue() const;
      DLLLOCAL void addFunction(class QoreType *lt, class QoreType *rt, op_func_t f); 
      DLLLOCAL void addFunction(class QoreType *lt, class QoreType *rt, op_bool_func_t f); 
      DLLLOCAL void addFunction(class QoreType *lt, class QoreType *rt, op_bigint_func_t f); 
      DLLLOCAL void addFunction(class QoreType *lt, class QoreType *rt, op_float_func_t f); 
      DLLLOCAL class QoreNode *eval(class QoreNode *l, class QoreNode *r, bool ref_rv, ExceptionSink *xsink) const;
      DLLLOCAL bool bool_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL int64 bigint_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL double float_eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL char *getName() const;
      DLLLOCAL char *getDescription() const;
};

#endif
