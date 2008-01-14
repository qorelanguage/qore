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
DLLLOCAL extern class Operator *OP_ASSIGNMENT, *OP_MODULA, 
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

typedef bool (*op_bool_str_str_func_t)(const QoreString *l, const QoreString *r, ExceptionSink *xsink);
// FIXME: change to const QoreRegex
typedef bool (*op_bool_str_regex_func_t)(const QoreString *l, QoreRegex *r, ExceptionSink *xsink);
typedef int64 (*op_bigint_str_str_func_t)(const QoreString *l, const QoreString *r, ExceptionSink *xsink);
// should be QoreHash (return value)
typedef QoreNode *(*op_hash_string_func_t)(const QoreHash *l, const QoreString *r, ExceptionSink *xsink);
typedef QoreStringNode *(*op_str_str_str_func_t)(const QoreString *l, const QoreString *r, ExceptionSink *xsink);
// FIXME: change to const QoreRegex
// should be QoreList (return value)
typedef QoreNode *(*op_list_str_regex_func_t)(const QoreString *l, QoreRegex *r, ExceptionSink *xsink);
typedef QoreNode *(*op_varref_func_t)(QoreNode *vref, bool ref_rv, ExceptionSink *xsink);
typedef bool (*op_bool_hash_hash_func_t)(const QoreHash *l, const QoreHash *r, ExceptionSink *xsink);
typedef QoreNode *(*op_hash_list_func_t)(const QoreHash *l, const QoreList *r, ExceptionSink *xsink);
typedef QoreNode *(*op_noconvert_func_t)(QoreNode *l, QoreNode *r);
typedef QoreNode *(*op_node_int_func_t)(QoreNode *l, int r, ExceptionSink *xsink);
typedef QoreNode *(*op_node_func_t)(QoreNode *l, QoreNode *r, ExceptionSink *xsink);

typedef QoreNode *(* op_func_t)(QoreNode *l, QoreNode *r, bool ref_rv, ExceptionSink *xsink);
typedef bool (*op_bool_func_t)(QoreNode *l, QoreNode *r, ExceptionSink *xsink);

typedef int64 (*op_bigint_func_t)(QoreNode *l, QoreNode *r, ExceptionSink *xsink);
typedef double (*op_float_func_t)(QoreNode *l, QoreNode *r, ExceptionSink *xsink);

class AbstractOperatorFunction {
   public:
      const QoreType *ltype, *rtype;

      DLLLOCAL AbstractOperatorFunction(const QoreType *lt, const QoreType *rt);
      DLLLOCAL virtual ~AbstractOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const = 0;
};

class OperatorFunction : public AbstractOperatorFunction
{
   private:
      op_func_t op_func;

   public:
      DLLLOCAL OperatorFunction(const QoreType *lt, const QoreType *rt, op_func_t f);
      DLLLOCAL virtual ~OperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
};

class NodeOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_node_func_t op_func;

   public:
      DLLLOCAL NodeOperatorFunction(const QoreType *lt, const QoreType *rt, op_node_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
      {
      }
      DLLLOCAL virtual ~NodeOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
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
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
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
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
};

class HashStringOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_hash_string_func_t op_func;

   public:
      DLLLOCAL HashStringOperatorFunction(op_hash_string_func_t f) : AbstractOperatorFunction(NT_HASH, NT_STRING), op_func(f)
      {
      }
      DLLLOCAL virtual ~HashStringOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
};

class HashListOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_hash_list_func_t op_func;

   public:
      DLLLOCAL HashListOperatorFunction(op_hash_list_func_t f) : AbstractOperatorFunction(NT_HASH, NT_LIST), op_func(f)
      {
      }
      DLLLOCAL virtual ~HashListOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
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
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
};

class NoConvertOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_noconvert_func_t op_func;

   public:
      DLLLOCAL NoConvertOperatorFunction(const QoreType *lt, const QoreType *rt, op_noconvert_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
      {
      }
      DLLLOCAL virtual ~NoConvertOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
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
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
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
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
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
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
};

class NoConvertBoolOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bool_func_t op_func;

   public:
      DLLLOCAL NoConvertBoolOperatorFunction(const QoreType *lt, const QoreType *rt, op_bool_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
      {
      }
      DLLLOCAL virtual ~NoConvertBoolOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
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
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
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
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
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
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
};

class BoolOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bool_func_t op_func;

   public:
      DLLLOCAL BoolOperatorFunction(const QoreType *lt, const QoreType *rt, op_bool_func_t f);
      DLLLOCAL virtual ~BoolOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
};

class BigIntOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_bigint_func_t op_func;

   public:
      DLLLOCAL BigIntOperatorFunction(const QoreType *lt, const QoreType *rt, op_bigint_func_t f);
      DLLLOCAL virtual ~BigIntOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
};

class FloatOperatorFunction : public AbstractOperatorFunction
{
   private:
      op_float_func_t op_func;

   public:
      DLLLOCAL FloatOperatorFunction(const QoreType *lt, const QoreType *rt, op_float_func_t f);
      DLLLOCAL virtual ~FloatOperatorFunction() {}
      DLLLOCAL virtual class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, int args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual bool bool_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigint_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
      DLLLOCAL virtual double float_eval(QoreNode *l, QoreNode *r, int args, ExceptionSink *xsink) const;
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

      DLLLOCAL static int match(const QoreType *ntype, const QoreType *rtype);
      DLLLOCAL int findFunction(const QoreType *ltype, const QoreType *rtype) const; 
      DLLLOCAL int get_function(QoreNodeEvalOptionalRefHolder &nleft, ExceptionSink *xsink) const;
      DLLLOCAL int get_function(QoreNodeEvalOptionalRefHolder &nleft, QoreNodeEvalOptionalRefHolder &nright, ExceptionSink *xsink) const;

   public:
      DLLLOCAL Operator(int arg, char *n, char *desc, int n_evalArgs, bool n_effect, bool n_lvalue = false);
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
      DLLLOCAL void addFunction(const QoreType *lt, const QoreType *rt, op_node_func_t f)
      {
	 functions.push_back(new NodeOperatorFunction(lt, rt, f));
      }
      DLLLOCAL void addFunction(const QoreType *lt, const QoreType *rt, op_noconvert_func_t f)
      {
	 functions.push_back(new NoConvertOperatorFunction(lt, rt, f));
      }
      DLLLOCAL void addNoConvertFunction(const QoreType *lt, const QoreType *rt, op_bool_func_t f)
      {
	 functions.push_back(new NoConvertBoolOperatorFunction(lt, rt, f));
      }
      DLLLOCAL void addEffectFunction(op_bool_func_t f)
      {
	 functions.push_back(new EffectBoolOperatorFunction(f));
      }

      DLLLOCAL void addFunction(const QoreType *lt, const QoreType *rt, op_func_t f); 
      DLLLOCAL void addFunction(const QoreType *lt, const QoreType *rt, op_bool_func_t f); 
      DLLLOCAL void addFunction(const QoreType *lt, const QoreType *rt, op_bigint_func_t f); 
      DLLLOCAL void addFunction(const QoreType *lt, const QoreType *rt, op_float_func_t f); 
      DLLLOCAL class QoreNode *eval(QoreNode *l, QoreNode *r, bool ref_rv, ExceptionSink *xsink) const;
      DLLLOCAL bool bool_eval(QoreNode *l, QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL int64 bigint_eval(QoreNode *l, QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL double float_eval(QoreNode *l, QoreNode *r, ExceptionSink *xsink) const;
      DLLLOCAL char *getName() const;
      DLLLOCAL char *getDescription() const;
};

#endif
