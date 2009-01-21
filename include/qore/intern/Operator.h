/*
  Operator.h

  Qore flexible operator support

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

#ifndef _QORE_OPERATOR_H

#define _QORE_OPERATOR_H

#include <qore/safe_dslist>
#include <qore/node_types.h>
#include <vector>

class Operator;

// system default operators
DLLLOCAL extern Operator *OP_ASSIGNMENT, *OP_MODULA, 
   *OP_BIN_AND, *OP_BIN_OR, *OP_BIN_NOT, *OP_BIN_XOR, *OP_MINUS, *OP_PLUS, 
   *OP_MULT, *OP_DIV, *OP_UNARY_MINUS, *OP_SHIFT_LEFT, *OP_SHIFT_RIGHT, 
   *OP_POST_INCREMENT, *OP_POST_DECREMENT, *OP_PRE_INCREMENT, *OP_PRE_DECREMENT, 
   *OP_LOG_CMP, *OP_PLUS_EQUALS, *OP_MINUS_EQUALS, *OP_AND_EQUALS, *OP_OR_EQUALS, 
   *OP_LIST_REF, *OP_OBJECT_REF, *OP_ELEMENTS, *OP_KEYS, *OP_QUESTION_MARK, 
   *OP_OBJECT_FUNC_REF, *OP_NEW, *OP_SHIFT, *OP_POP, *OP_PUSH,
   *OP_UNSHIFT, *OP_REGEX_SUBST, *OP_LIST_ASSIGNMENT, *OP_SPLICE, *OP_MODULA_EQUALS, 
   *OP_MULTIPLY_EQUALS, *OP_DIVIDE_EQUALS, *OP_XOR_EQUALS, *OP_SHIFT_LEFT_EQUALS, 
   *OP_SHIFT_RIGHT_EQUALS, *OP_REGEX_TRANS, *OP_REGEX_EXTRACT, 
   *OP_CHOMP, *OP_TRIM, *OP_LOG_AND, *OP_LOG_OR, *OP_LOG_LT, 
   *OP_LOG_GT, *OP_LOG_EQ, *OP_LOG_NE, *OP_LOG_LE, *OP_LOG_GE, *OP_NOT, 
   *OP_ABSOLUTE_EQ, *OP_ABSOLUTE_NE, *OP_REGEX_MATCH, *OP_REGEX_NMATCH,
   *OP_EXISTS, *OP_INSTANCEOF, *OP_MAP, *OP_MAP_SELECT, *OP_FOLDR, *OP_FOLDL,
   *OP_SELECT;

typedef safe_dslist<Operator *> oplist_t;

class OperatorList : public oplist_t {
   public:
      DLLLOCAL OperatorList();
      DLLLOCAL ~OperatorList();
      DLLLOCAL void init();
      DLLLOCAL Operator *add(Operator *o);
};

DLLLOCAL extern class OperatorList oplist;

class QoreRegexNode;

typedef bool (*op_bool_str_str_func_t)(const QoreString *l, const QoreString *r, ExceptionSink *xsink);
typedef bool (*op_bool_str_regex_func_t)(const QoreString *l, const QoreRegexNode *r, ExceptionSink *xsink);
typedef int64 (*op_bigint_str_str_func_t)(const QoreString *l, const QoreString *r, ExceptionSink *xsink);
typedef QoreHashNode *(*op_hash_string_func_t)(const QoreHashNode *l, const QoreString *r, ExceptionSink *xsink);
typedef QoreStringNode *(*op_str_str_str_func_t)(const QoreString *l, const QoreString *r, ExceptionSink *xsink);
// should be QoreListNode (return value)
typedef AbstractQoreNode *(*op_list_str_regex_func_t)(const QoreString *l, const QoreRegexNode *r, ExceptionSink *xsink);
typedef AbstractQoreNode *(*op_varref_func_t)(const AbstractQoreNode *vref, bool ref_rv, ExceptionSink *xsink);
typedef QoreHashNode *(*op_hash_list_func_t)(const QoreHashNode *l, const QoreListNode *r, ExceptionSink *xsink);
typedef AbstractQoreNode *(*op_noconvert_func_t)(const AbstractQoreNode *l, const AbstractQoreNode *r);
typedef AbstractQoreNode *(*op_node_int_func_t)(const AbstractQoreNode *l, int r, ExceptionSink *xsink);
typedef AbstractQoreNode *(*op_node_func_t)(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink);
typedef bool (*op_bool_int_func_t)(int64 l, int64 r);
typedef int64 (*op_int_int_func_t)(int64 l, int64 r);
typedef int64 (*op_divide_int_func_t)(int64 l, int64 r, ExceptionSink *xsink);
typedef bool (*op_logic_func_t)(bool l, bool r);

typedef bool (*op_bool_float_func_t)(double l, double r);
typedef double (*op_float_float_func_t)(double l, double r);
typedef double (*op_divide_float_func_t)(double l, double r, ExceptionSink *xsink);
typedef int64 (*op_compare_float_func_t)(double l, double r);

typedef bool (*op_bool_date_func_t)(const DateTimeNode *l, const DateTimeNode *r);
typedef DateTimeNode *(*op_date_func_t)(const DateTimeNode *l, const DateTimeNode *r);

typedef bool (*op_bool_bin_func_t)(const BinaryNode *l, const BinaryNode *r);
typedef bool (*op_simple_bool_func_t)(const AbstractQoreNode *l, const AbstractQoreNode *r);

typedef AbstractQoreNode *(* op_func_t)(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, ExceptionSink *xsink);
typedef bool (*op_bool_func_t)(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink);

typedef int64 (*op_bigint_func_t)(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink);
typedef double (*op_float_func_t)(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink);

class AbstractOperatorFunction {
   public:
      qore_type_t ltype, rtype;
      bool exact;

      DLLLOCAL AbstractOperatorFunction(bool n_exact, qore_type_t lt, qore_type_t rt) : ltype(lt), rtype(rt), exact(n_exact) {
      }
      DLLLOCAL AbstractOperatorFunction(qore_type_t lt, qore_type_t rt) : ltype(lt), rtype(rt), exact(false) {
      }
      DLLLOCAL virtual ~AbstractOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const = 0;

      DLLLOCAL bool needsExactMatch() { return exact; }
};

class OperatorFunction : public AbstractOperatorFunction
{
   private:
      op_func_t op_func;

   public:
      DLLLOCAL OperatorFunction(qore_type_t lt, qore_type_t rt, op_func_t f);
      DLLLOCAL virtual ~OperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class NodeOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_node_func_t op_func;

   public:
      DLLLOCAL NodeOperatorFunction(qore_type_t lt, qore_type_t rt, op_node_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
      {
      }
      DLLLOCAL virtual ~NodeOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class EffectNoEvalOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_func_t op_func;

   public:
      DLLLOCAL EffectNoEvalOperatorFunction(op_func_t f) : AbstractOperatorFunction(NT_ALL, NT_ALL), op_func(f)
      {
      }
      DLLLOCAL virtual ~EffectNoEvalOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class VarRefOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_varref_func_t op_func;

   public:
      DLLLOCAL VarRefOperatorFunction(op_varref_func_t f) : AbstractOperatorFunction(NT_VARREF, NT_NONE), op_func(f)
      {
      }
      DLLLOCAL virtual ~VarRefOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class HashStringOperatorFunction : public AbstractOperatorFunction {
   private:
      op_hash_string_func_t op_func;

   public:
      DLLLOCAL HashStringOperatorFunction(op_hash_string_func_t f) : AbstractOperatorFunction(true, NT_HASH, NT_STRING), op_func(f) {
      }
      DLLLOCAL virtual ~HashStringOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class HashListOperatorFunction : public AbstractOperatorFunction {
   private:
      op_hash_list_func_t op_func;

   public:
      DLLLOCAL HashListOperatorFunction(op_hash_list_func_t f) : AbstractOperatorFunction(true, NT_HASH, NT_LIST), op_func(f) {
      }
      DLLLOCAL virtual ~HashListOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class NodeIntOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_node_int_func_t op_func;

   public:
      DLLLOCAL NodeIntOperatorFunction(op_node_int_func_t f) : AbstractOperatorFunction(NT_LIST, NT_INT), op_func(f)
      {
      }
      DLLLOCAL virtual ~NodeIntOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class NoConvertOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_noconvert_func_t op_func;

   public:
      DLLLOCAL NoConvertOperatorFunction(qore_type_t lt, qore_type_t rt, op_noconvert_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
      {
      }
      DLLLOCAL virtual ~NoConvertOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class ListStringRegexOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_list_str_regex_func_t op_func;

   public:
      DLLLOCAL ListStringRegexOperatorFunction(op_list_str_regex_func_t f) : AbstractOperatorFunction(NT_STRING, NT_REGEX), op_func(f)
      {
      }
      DLLLOCAL virtual ~ListStringRegexOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class StringStringStringOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_str_str_str_func_t op_func;

   public:
      DLLLOCAL StringStringStringOperatorFunction(op_str_str_str_func_t f) : AbstractOperatorFunction(NT_STRING, NT_STRING), op_func(f)
      {
      }
      DLLLOCAL virtual ~StringStringStringOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class BoolStrStrOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bool_str_str_func_t op_func;

   public:
      DLLLOCAL BoolStrStrOperatorFunction(op_bool_str_str_func_t f) : AbstractOperatorFunction(NT_STRING, NT_STRING), op_func(f)
      {
      }      
      DLLLOCAL virtual ~BoolStrStrOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class BoolDateOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bool_date_func_t op_func;

   public:
      DLLLOCAL BoolDateOperatorFunction(op_bool_date_func_t f) : AbstractOperatorFunction(NT_DATE, NT_DATE), op_func(f)
      {
      }      
      DLLLOCAL virtual ~BoolDateOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class DateOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_date_func_t op_func;

   public:
      DLLLOCAL DateOperatorFunction(op_date_func_t f) : AbstractOperatorFunction(NT_DATE, NT_DATE), op_func(f)
      {
      }      
      DLLLOCAL virtual ~DateOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class BoolBinOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bool_bin_func_t op_func;

   public:
      DLLLOCAL BoolBinOperatorFunction(op_bool_bin_func_t f) : AbstractOperatorFunction(NT_BINARY, NT_BINARY), op_func(f)
      {
      }      
      DLLLOCAL virtual ~BoolBinOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class NoConvertBoolOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bool_func_t op_func;

   public:
      DLLLOCAL NoConvertBoolOperatorFunction(qore_type_t lt, qore_type_t rt, op_bool_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
      {
      }
      DLLLOCAL virtual ~NoConvertBoolOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class EffectBoolOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bool_func_t op_func;

   public:
      DLLLOCAL EffectBoolOperatorFunction(op_bool_func_t f) : AbstractOperatorFunction(NT_ALL, NT_ALL), op_func(f)
      {
      }
      DLLLOCAL virtual ~EffectBoolOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};


class BoolStrRegexOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bool_str_regex_func_t op_func;

   public:
      DLLLOCAL BoolStrRegexOperatorFunction(op_bool_str_regex_func_t f) : AbstractOperatorFunction(NT_STRING, NT_REGEX), op_func(f)
      {
      }
      DLLLOCAL virtual ~BoolStrRegexOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class BoolIntOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bool_int_func_t op_func;

   public:
      DLLLOCAL BoolIntOperatorFunction(op_bool_int_func_t f) : AbstractOperatorFunction(NT_INT, NT_INT), op_func(f)
      {
      }
      DLLLOCAL virtual ~BoolIntOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class IntIntOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_int_int_func_t op_func;

   public:
      DLLLOCAL IntIntOperatorFunction(op_int_int_func_t f) : AbstractOperatorFunction(NT_INT, NT_INT), op_func(f)
      {
      }
      DLLLOCAL virtual ~IntIntOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

// this whole class just for one operator (integer division)...
class DivideIntOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_divide_int_func_t op_func;

   public:
      DLLLOCAL DivideIntOperatorFunction(op_divide_int_func_t f) : AbstractOperatorFunction(NT_INT, NT_INT), op_func(f)
      {
      }
      DLLLOCAL virtual ~DivideIntOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

// this whole class just for one operator (integer unary minus)...
class UnaryMinusIntOperatorFunction : public AbstractOperatorFunction
{
   public:
      DLLLOCAL UnaryMinusIntOperatorFunction() : AbstractOperatorFunction(NT_INT, NT_INT)
      {
      }
      DLLLOCAL virtual ~UnaryMinusIntOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

// this whole class just for one operator (binary integer not)...
class IntegerNotOperatorFunction : public AbstractOperatorFunction
{
   public:
      DLLLOCAL IntegerNotOperatorFunction() : AbstractOperatorFunction(NT_INT, NT_NONE)
      {
      }
      DLLLOCAL virtual ~IntegerNotOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class BoolFloatOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bool_float_func_t op_func;

   public:
      DLLLOCAL BoolFloatOperatorFunction(op_bool_float_func_t f) : AbstractOperatorFunction(NT_FLOAT, NT_FLOAT), op_func(f)
      {
      }
      DLLLOCAL virtual ~BoolFloatOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class FloatFloatOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_float_float_func_t op_func;

   public:
      DLLLOCAL FloatFloatOperatorFunction(op_float_float_func_t f) : AbstractOperatorFunction(NT_FLOAT, NT_FLOAT), op_func(f)
      {
      }
      DLLLOCAL virtual ~FloatFloatOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

// this whole class just for one operator (floating-point division)...
class DivideFloatOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_divide_float_func_t op_func;

   public:
      DLLLOCAL DivideFloatOperatorFunction(op_divide_float_func_t f) : AbstractOperatorFunction(NT_FLOAT, NT_FLOAT), op_func(f)
      {
      }
      DLLLOCAL virtual ~DivideFloatOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

// this whole class just for one operator (floating-point unary minus)...
class UnaryMinusFloatOperatorFunction : public AbstractOperatorFunction
{
   public:
      DLLLOCAL UnaryMinusFloatOperatorFunction() : AbstractOperatorFunction(NT_FLOAT, NT_FLOAT)
      {
      }
      DLLLOCAL virtual ~UnaryMinusFloatOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

// this whole class just for one operator (floating-point comparison <=>)...
class CompareFloatOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_compare_float_func_t op_func;

   public:
      DLLLOCAL CompareFloatOperatorFunction(op_compare_float_func_t f) : AbstractOperatorFunction(NT_FLOAT, NT_FLOAT), op_func(f)
      {
      }
      DLLLOCAL virtual ~CompareFloatOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

// this whole class just for one operator (boolean/logical not)...
class BoolNotOperatorFunction : public AbstractOperatorFunction
{
   public:
      DLLLOCAL BoolNotOperatorFunction() : AbstractOperatorFunction(NT_BOOLEAN, NT_NONE)
      {
      }
      DLLLOCAL virtual ~BoolNotOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

// this whole class just for one operator (date comparison <=>)...
class CompareDateOperatorFunction : public AbstractOperatorFunction
{
   public:
      DLLLOCAL CompareDateOperatorFunction() : AbstractOperatorFunction(NT_DATE, NT_DATE)
      {
      }
      DLLLOCAL virtual ~CompareDateOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class LogicOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_logic_func_t op_func;

   public:
      DLLLOCAL LogicOperatorFunction(op_logic_func_t f) : AbstractOperatorFunction(NT_BOOLEAN, NT_BOOLEAN), op_func(f)
      {
      }
      DLLLOCAL virtual ~LogicOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class BigIntStrStrOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bigint_str_str_func_t op_func;

   public:
      DLLLOCAL BigIntStrStrOperatorFunction(op_bigint_str_str_func_t f) : AbstractOperatorFunction(NT_STRING, NT_STRING), op_func(f)
      {
      }
      DLLLOCAL virtual ~BigIntStrStrOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

// this class if only for operators that have no side effects
class SimpleBoolOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_simple_bool_func_t op_func;

   public:
      DLLLOCAL SimpleBoolOperatorFunction(qore_type_t lt, qore_type_t rt, op_simple_bool_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
      {
      }
      DLLLOCAL virtual ~SimpleBoolOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class BoolOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bool_func_t op_func;

   public:
      DLLLOCAL BoolOperatorFunction(qore_type_t lt, qore_type_t rt, op_bool_func_t f);
      DLLLOCAL virtual ~BoolOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class BigIntOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bigint_func_t op_func;

   public:
      DLLLOCAL BigIntOperatorFunction(qore_type_t lt, qore_type_t rt, op_bigint_func_t f);
      DLLLOCAL virtual ~BigIntOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class FloatOperatorFunction : public AbstractOperatorFunction {
   private:
      op_float_func_t op_func;

   public:
      DLLLOCAL FloatOperatorFunction(qore_type_t lt, qore_type_t rt, op_float_func_t f);
      DLLLOCAL virtual ~FloatOperatorFunction() {}
      DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const;
};

class DefaultNothingOperatorFunction : public AbstractOperatorFunction {
public:
    DLLLOCAL DefaultNothingOperatorFunction() : AbstractOperatorFunction(NT_ALL, NT_ALL) {
    }
    DLLLOCAL virtual ~DefaultNothingOperatorFunction() {}
    DLLLOCAL virtual AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const {
	return 0;
    }
    DLLLOCAL virtual bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const {
	return false;
    }
    DLLLOCAL virtual int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const {
	return 0;
    }
    DLLLOCAL virtual double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, int args, ExceptionSink *xsink) const {
	return 0.0;
    }
};

typedef std::vector<AbstractOperatorFunction *> opfunc_list_t;

class Operator {
   private:
      opfunc_list_t functions;
      int (*opMatrix)[NUM_VALUE_TYPES];
      bool effect, lvalue;
      const char *name, *description;
      int args;
      int evalArgs;

      DLLLOCAL static int match(qore_type_t ntype, qore_type_t rtype);
      DLLLOCAL int findFunction(qore_type_t ltype, qore_type_t rtype) const; 
      DLLLOCAL int get_function(const QoreNodeEvalOptionalRefHolder &nleft, ExceptionSink *xsink) const;
      DLLLOCAL int get_function(const QoreNodeEvalOptionalRefHolder &nleft, const QoreNodeEvalOptionalRefHolder &nright, ExceptionSink *xsink) const;

   public:
      // create a new Operator
      /** @param arg number of arguments (1 or 2)
	  @param n the name of the operator
	  @param desc the description of the operator
	  @param n_evalArgs if the arguments should be evaluated before passing to the operator function or not
	  @param n_effect if the operator modifies anything
	  @param n_lvalue if the operator requires an lvalue on the left side (for modification, ex: $a =~ s/x/p/ )
       */
      DLLLOCAL Operator(int arg, const char *n, const char *desc, int n_evalArgs, bool n_effect, bool n_lvalue = false);
      DLLLOCAL ~Operator();
      DLLLOCAL void init();
      DLLLOCAL bool hasEffect() const;
      DLLLOCAL bool needsLValue() const;

      DLLLOCAL void addFunction(op_bool_str_str_func_t f)
      {
	 functions.push_back(new BoolStrStrOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_bool_str_regex_func_t f)
      {
	 functions.push_back(new BoolStrRegexOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_bigint_str_str_func_t f)
      {
	 functions.push_back(new BigIntStrStrOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_hash_string_func_t f)
      {
	 functions.push_back(new HashStringOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_hash_list_func_t f)
      {
	 functions.push_back(new HashListOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_str_str_str_func_t f)
      {
	 functions.push_back(new StringStringStringOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_list_str_regex_func_t f)
      {
	 functions.push_back(new ListStringRegexOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_varref_func_t f)
      {
	 functions.push_back(new VarRefOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_func_t f)
      {
	 functions.push_back(new EffectNoEvalOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_node_int_func_t f)
      {
	 functions.push_back(new NodeIntOperatorFunction(f));
      }
      DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_node_func_t f)
      {
	 functions.push_back(new NodeOperatorFunction(lt, rt, f));
      }
      DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_simple_bool_func_t f)
      {
	 functions.push_back(new SimpleBoolOperatorFunction(lt, rt, f));
      }
      DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_noconvert_func_t f)
      {
	 functions.push_back(new NoConvertOperatorFunction(lt, rt, f));
      }
      DLLLOCAL void addNoConvertFunction(qore_type_t lt, qore_type_t rt, op_bool_func_t f)
      {
	 functions.push_back(new NoConvertBoolOperatorFunction(lt, rt, f));
      }
      DLLLOCAL void addEffectFunction(op_bool_func_t f)
      {
	 functions.push_back(new EffectBoolOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_bool_int_func_t f)
      {
	 functions.push_back(new BoolIntOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_int_int_func_t f)
      {
	 functions.push_back(new IntIntOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_divide_int_func_t f)
      {
	 functions.push_back(new DivideIntOperatorFunction(f));
      }
      DLLLOCAL void addUnaryMinusIntFunction()
      {
	 functions.push_back(new UnaryMinusIntOperatorFunction());
      }
      DLLLOCAL void addFunction(op_bool_float_func_t f)
      {
	 functions.push_back(new BoolFloatOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_float_float_func_t f)
      {
	 functions.push_back(new FloatFloatOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_divide_float_func_t f)
      {
	 functions.push_back(new DivideFloatOperatorFunction(f));
      }
      DLLLOCAL void addUnaryMinusFloatFunction()
      {
	 functions.push_back(new UnaryMinusFloatOperatorFunction());
      }
      DLLLOCAL void addFunction(op_compare_float_func_t f)
      {
	 functions.push_back(new CompareFloatOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_logic_func_t f)
      {
	 functions.push_back(new LogicOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_bool_date_func_t f)
      {
	 functions.push_back(new BoolDateOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_bool_bin_func_t f)
      {
	 functions.push_back(new BoolBinOperatorFunction(f));
      }
      DLLLOCAL void addFunction(op_date_func_t f)
      {
	 functions.push_back(new DateOperatorFunction(f));
      }
      DLLLOCAL void addCompareDateFunction()
      {
	 functions.push_back(new CompareDateOperatorFunction());
      }
      DLLLOCAL void addBoolNotFunction()
      {
	 functions.push_back(new BoolNotOperatorFunction());
      }
      DLLLOCAL void addIntegerNotFunction()
      {
	 functions.push_back(new IntegerNotOperatorFunction());
      }

      DLLLOCAL void addDefaultNothing() {
	  functions.push_back(new DefaultNothingOperatorFunction());
      }

      DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_func_t f); 
      DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_bool_func_t f); 
      DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_bigint_func_t f); 
      DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_float_func_t f); 
      DLLLOCAL AbstractQoreNode *eval(const AbstractQoreNode *l, const AbstractQoreNode *r, bool ref_rv, ExceptionSink *xsink) const;
      DLLLOCAL bool bool_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL int64 bigint_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL double float_eval(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL const char *getName() const;
      DLLLOCAL const char *getDescription() const;
};

#endif
