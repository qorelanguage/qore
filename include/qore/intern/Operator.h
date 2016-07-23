/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  Operator.h

  Qore flexible operator support

  Copyright (C) 2003 - 2016 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_OPERATOR_H

#define _QORE_OPERATOR_H

#include <qore/safe_dslist>
#include <qore/node_types.h>
#include <vector>

class Operator;

// system default operators
DLLLOCAL extern Operator *OP_MINUS, *OP_PLUS,
   *OP_MULT,
   *OP_LOG_CMP,
   *OP_OBJECT_REF, *OP_QUESTION_MARK,
   *OP_SHIFT, *OP_POP, *OP_PUSH,
   *OP_UNSHIFT, *OP_REGEX_SUBST, *OP_LIST_ASSIGNMENT,
   *OP_REGEX_TRANS, *OP_REGEX_EXTRACT,
   *OP_LOG_AND, *OP_LOG_OR, *OP_LOG_LT,
   *OP_LOG_GT, *OP_LOG_EQ, *OP_LOG_NE, *OP_LOG_LE, *OP_LOG_GE,
   *OP_ABSOLUTE_EQ, *OP_ABSOLUTE_NE, *OP_REGEX_MATCH, *OP_REGEX_NMATCH,
   *OP_INSTANCEOF;

typedef safe_dslist<Operator*> oplist_t;

class OperatorList : public oplist_t {
public:
   DLLLOCAL OperatorList();
   DLLLOCAL ~OperatorList();
   DLLLOCAL void init();
   DLLLOCAL Operator *add(Operator *o);
};

DLLLOCAL extern OperatorList oplist;

class QoreRegexNode;

typedef bool (*op_bool_str_str_func_t)(const QoreString *l, const QoreString *r, ExceptionSink* xsink);
typedef bool (*op_bool_str_regex_func_t)(const QoreString *l, const QoreRegexNode* r, ExceptionSink* xsink);
typedef int64 (*op_bigint_str_str_func_t)(const QoreString *l, const QoreString *r, ExceptionSink* xsink);
typedef QoreHashNode* (*op_hash_string_func_t)(const QoreHashNode* l, const QoreString *r, ExceptionSink* xsink);
typedef QoreStringNode* (*op_str_str_str_func_t)(const QoreString *l, const QoreString *r, ExceptionSink* xsink);
// should be QoreListNode (return value)
typedef AbstractQoreNode* (*op_list_str_regex_func_t)(const QoreString *l, const QoreRegexNode* r, ExceptionSink* xsink);
typedef AbstractQoreNode* (*op_varref_func_t)(const AbstractQoreNode* vref, bool ref_rv, ExceptionSink* xsink);
typedef QoreHashNode* (*op_hash_list_func_t)(const QoreHashNode* l, const QoreListNode* r, ExceptionSink* xsink);
typedef AbstractQoreNode* (*op_noconvert_func_t)(const AbstractQoreNode* l, const AbstractQoreNode* r);
typedef AbstractQoreNode* (*op_node_int_func_t)(const AbstractQoreNode* l, int r, ExceptionSink* xsink);
typedef AbstractQoreNode* (*op_node_func_t)(const AbstractQoreNode* l, const AbstractQoreNode* r, ExceptionSink* xsink);
typedef bool (*op_bool_int_func_t)(int64 l, int64 r);
typedef int64 (*op_int_int_func_t)(int64 l, int64 r);
typedef int64 (*op_divide_int_func_t)(int64 l, int64 r, ExceptionSink* xsink);
typedef bool (*op_logic_func_t)(bool l, bool r);

typedef bool (*op_bool_float_func_t)(double l, double r);
typedef double (*op_float_float_func_t)(double l, double r);
typedef double (*op_divide_float_func_t)(double l, double r, ExceptionSink* xsink);
typedef int64 (*op_compare_float_func_t)(double l, double r, ExceptionSink* xsink);

typedef bool (*op_bool_date_func_t)(const DateTimeNode* l, const DateTimeNode* r);
typedef DateTimeNode* (*op_date_func_t)(const DateTimeNode* l, const DateTimeNode* r);

typedef bool (*op_bool_bin_func_t)(const BinaryNode* l, const BinaryNode* r);
typedef bool (*op_simple_bool_func_t)(const AbstractQoreNode* l, const AbstractQoreNode* r);

typedef AbstractQoreNode*(*op_func_t)(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, ExceptionSink* xsink);
typedef bool (*op_bool_func_t)(const AbstractQoreNode* l, const AbstractQoreNode* r, ExceptionSink* xsink);

typedef int64 (*op_bigint_func_t)(const AbstractQoreNode* l, const AbstractQoreNode* r, ExceptionSink* xsink);
typedef double (*op_float_func_t)(const AbstractQoreNode* l, const AbstractQoreNode* r, ExceptionSink* xsink);

typedef QoreNumberNode* (*op_number_func_t)(const QoreNumberNode* l, const QoreNumberNode* r, ExceptionSink* xsink);
typedef bool (*op_bool_number_func_t)(const QoreNumberNode* l, const QoreNumberNode* r);
typedef int64 (*op_int_number_func_t)(const QoreNumberNode* l, const QoreNumberNode* r, ExceptionSink* xsink);

class AbstractOperatorFunction {
public:
   qore_type_t ltype, rtype;
   bool exact;

   DLLLOCAL AbstractOperatorFunction(bool n_exact, qore_type_t lt, qore_type_t rt) : ltype(lt), rtype(rt), exact(n_exact) {
   }
   DLLLOCAL AbstractOperatorFunction(qore_type_t lt, qore_type_t rt) : ltype(lt), rtype(rt), exact(false) {
   }
   DLLLOCAL virtual ~AbstractOperatorFunction() {}

   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const = 0;

   DLLLOCAL bool needsExactMatch() { return exact; }
};

class OperatorFunction : public AbstractOperatorFunction {
private:
   op_func_t op_func;

public:
   DLLLOCAL OperatorFunction(qore_type_t lt, qore_type_t rt, op_func_t f);
   DLLLOCAL virtual ~OperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class NodeOperatorFunction : public AbstractOperatorFunction {
private:
   op_node_func_t op_func;

public:
   DLLLOCAL NodeOperatorFunction(qore_type_t lt, qore_type_t rt, op_node_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f) {
   }
   DLLLOCAL virtual ~NodeOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class EffectNoEvalOperatorFunction : public AbstractOperatorFunction {
private:
   op_func_t op_func;

public:
   DLLLOCAL EffectNoEvalOperatorFunction(op_func_t f) : AbstractOperatorFunction(NT_ALL, NT_ALL), op_func(f) {
   }
   DLLLOCAL virtual ~EffectNoEvalOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class VarRefOperatorFunction : public AbstractOperatorFunction {
private:
   op_varref_func_t op_func;

public:
   DLLLOCAL VarRefOperatorFunction(op_varref_func_t f) : AbstractOperatorFunction(NT_VARREF, NT_NONE), op_func(f) {
   }
   DLLLOCAL virtual ~VarRefOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class HashStringOperatorFunction : public AbstractOperatorFunction {
private:
   op_hash_string_func_t op_func;

public:
   DLLLOCAL HashStringOperatorFunction(op_hash_string_func_t f) : AbstractOperatorFunction(true, NT_HASH, NT_STRING), op_func(f) {
   }
   DLLLOCAL virtual ~HashStringOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class HashListOperatorFunction : public AbstractOperatorFunction {
private:
   op_hash_list_func_t op_func;

public:
   DLLLOCAL HashListOperatorFunction(op_hash_list_func_t f) : AbstractOperatorFunction(true, NT_HASH, NT_LIST), op_func(f) {
   }
   DLLLOCAL virtual ~HashListOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class NodeIntOperatorFunction : public AbstractOperatorFunction {
private:
   op_node_int_func_t op_func;

public:
   DLLLOCAL NodeIntOperatorFunction(op_node_int_func_t f) : AbstractOperatorFunction(NT_LIST, NT_INT), op_func(f) {
   }
   DLLLOCAL virtual ~NodeIntOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class NoConvertOperatorFunction : public AbstractOperatorFunction {
private:
   op_noconvert_func_t op_func;

public:
   DLLLOCAL NoConvertOperatorFunction(qore_type_t lt, qore_type_t rt, op_noconvert_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f) {
   }
   DLLLOCAL virtual ~NoConvertOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class ListStringRegexOperatorFunction : public AbstractOperatorFunction {
private:
   op_list_str_regex_func_t op_func;

public:
   DLLLOCAL ListStringRegexOperatorFunction(op_list_str_regex_func_t f) : AbstractOperatorFunction(NT_STRING, NT_REGEX), op_func(f) {
   }
   DLLLOCAL virtual ~ListStringRegexOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class StringStringStringOperatorFunction : public AbstractOperatorFunction {
private:
   op_str_str_str_func_t op_func;

public:
   DLLLOCAL StringStringStringOperatorFunction(op_str_str_str_func_t f) : AbstractOperatorFunction(NT_STRING, NT_STRING), op_func(f) {
   }
   DLLLOCAL virtual ~StringStringStringOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class BoolStrStrOperatorFunction : public AbstractOperatorFunction {
private:
   op_bool_str_str_func_t op_func;

public:
   DLLLOCAL BoolStrStrOperatorFunction(op_bool_str_str_func_t f) : AbstractOperatorFunction(NT_STRING, NT_STRING), op_func(f) {
   }
   DLLLOCAL virtual ~BoolStrStrOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class BoolDateOperatorFunction : public AbstractOperatorFunction {
private:
   op_bool_date_func_t op_func;

public:
   DLLLOCAL BoolDateOperatorFunction(op_bool_date_func_t f) : AbstractOperatorFunction(NT_DATE, NT_DATE), op_func(f) {
   }
   DLLLOCAL virtual ~BoolDateOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class DateOperatorFunction : public AbstractOperatorFunction {
private:
   op_date_func_t op_func;

public:
   DLLLOCAL DateOperatorFunction(op_date_func_t f) : AbstractOperatorFunction(NT_DATE, NT_DATE), op_func(f) {
   }
   DLLLOCAL virtual ~DateOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class BoolBinOperatorFunction : public AbstractOperatorFunction {
private:
   op_bool_bin_func_t op_func;

public:
   DLLLOCAL BoolBinOperatorFunction(op_bool_bin_func_t f) : AbstractOperatorFunction(NT_BINARY, NT_BINARY), op_func(f) {
   }
   DLLLOCAL virtual ~BoolBinOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class NoConvertBoolOperatorFunction : public AbstractOperatorFunction {
private:
   op_bool_func_t op_func;

public:
   DLLLOCAL NoConvertBoolOperatorFunction(qore_type_t lt, qore_type_t rt, op_bool_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f) {
   }
   DLLLOCAL virtual ~NoConvertBoolOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class EffectBoolOperatorFunction : public AbstractOperatorFunction {
private:
   op_bool_func_t op_func;

public:
   DLLLOCAL EffectBoolOperatorFunction(op_bool_func_t f) : AbstractOperatorFunction(NT_ALL, NT_ALL), op_func(f) {
   }
   DLLLOCAL virtual ~EffectBoolOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class BoolStrRegexOperatorFunction : public AbstractOperatorFunction {
private:
   op_bool_str_regex_func_t op_func;

public:
   DLLLOCAL BoolStrRegexOperatorFunction(op_bool_str_regex_func_t f) : AbstractOperatorFunction(NT_STRING, NT_REGEX), op_func(f) {
   }
   DLLLOCAL virtual ~BoolStrRegexOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class BoolIntOperatorFunction : public AbstractOperatorFunction {
private:
   op_bool_int_func_t op_func;

public:
   DLLLOCAL BoolIntOperatorFunction(op_bool_int_func_t f) : AbstractOperatorFunction(NT_INT, NT_INT), op_func(f) {
   }
   DLLLOCAL virtual ~BoolIntOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class IntIntOperatorFunction : public AbstractOperatorFunction {
private:
   op_int_int_func_t op_func;

public:
   DLLLOCAL IntIntOperatorFunction(op_int_int_func_t f) : AbstractOperatorFunction(NT_INT, NT_INT), op_func(f) {
   }
   DLLLOCAL virtual ~IntIntOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class BoolFloatOperatorFunction : public AbstractOperatorFunction {
private:
   op_bool_float_func_t op_func;

public:
   DLLLOCAL BoolFloatOperatorFunction(op_bool_float_func_t f) : AbstractOperatorFunction(NT_FLOAT, NT_FLOAT), op_func(f) {
   }
   DLLLOCAL virtual ~BoolFloatOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class FloatFloatOperatorFunction : public AbstractOperatorFunction {
private:
   op_float_float_func_t op_func;

public:
   DLLLOCAL FloatFloatOperatorFunction(op_float_float_func_t f) : AbstractOperatorFunction(NT_FLOAT, NT_FLOAT), op_func(f) {
   }
   DLLLOCAL virtual ~FloatFloatOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

// this whole class just for one operator (floating-point comparison <=>)...
class CompareFloatOperatorFunction : public AbstractOperatorFunction {
private:
   op_compare_float_func_t op_func;

public:
   DLLLOCAL CompareFloatOperatorFunction(op_compare_float_func_t f) : AbstractOperatorFunction(NT_FLOAT, NT_FLOAT), op_func(f) {
   }
   DLLLOCAL virtual ~CompareFloatOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

// this whole class just for one operator (date comparison <=>)...
class CompareDateOperatorFunction : public AbstractOperatorFunction {
public:
   DLLLOCAL CompareDateOperatorFunction() : AbstractOperatorFunction(NT_DATE, NT_DATE) {
   }
   DLLLOCAL virtual ~CompareDateOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class LogicOperatorFunction : public AbstractOperatorFunction {
private:
   op_logic_func_t op_func;

public:
   DLLLOCAL LogicOperatorFunction(op_logic_func_t f) : AbstractOperatorFunction(NT_BOOLEAN, NT_BOOLEAN), op_func(f) {
   }
   DLLLOCAL virtual ~LogicOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class BigIntStrStrOperatorFunction : public AbstractOperatorFunction {
private:
   op_bigint_str_str_func_t op_func;

public:
   DLLLOCAL BigIntStrStrOperatorFunction(op_bigint_str_str_func_t f) : AbstractOperatorFunction(NT_STRING, NT_STRING), op_func(f) {
   }
   DLLLOCAL virtual ~BigIntStrStrOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

// this class if only for operators that have no side effects
class SimpleBoolOperatorFunction : public AbstractOperatorFunction {
private:
   op_simple_bool_func_t op_func;

public:
   DLLLOCAL SimpleBoolOperatorFunction(qore_type_t lt, qore_type_t rt, op_simple_bool_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f) {
   }
   DLLLOCAL virtual ~SimpleBoolOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class BoolOperatorFunction : public AbstractOperatorFunction {
private:
   op_bool_func_t op_func;

public:
   DLLLOCAL BoolOperatorFunction(qore_type_t lt, qore_type_t rt, op_bool_func_t f);
   DLLLOCAL virtual ~BoolOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class BigIntOperatorFunction : public AbstractOperatorFunction {
private:
   op_bigint_func_t op_func;

public:
   DLLLOCAL BigIntOperatorFunction(qore_type_t lt, qore_type_t rt, op_bigint_func_t f);
   DLLLOCAL virtual ~BigIntOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class FloatOperatorFunction : public AbstractOperatorFunction {
private:
   op_float_func_t op_func;

public:
   DLLLOCAL FloatOperatorFunction(qore_type_t lt, qore_type_t rt, op_float_func_t f);
   DLLLOCAL virtual ~FloatOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class NumberOperatorFunction : public AbstractOperatorFunction {
private:
   op_number_func_t op_func;

public:
   DLLLOCAL NumberOperatorFunction(op_number_func_t f) : AbstractOperatorFunction(NT_NUMBER, NT_NUMBER), op_func(f) {
   }
   DLLLOCAL virtual ~NumberOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class BoolNumberOperatorFunction : public AbstractOperatorFunction {
private:
   op_bool_number_func_t op_func;

public:
   DLLLOCAL BoolNumberOperatorFunction(op_bool_number_func_t f) : AbstractOperatorFunction(NT_NUMBER, NT_NUMBER), op_func(f) {
   }
   DLLLOCAL virtual ~BoolNumberOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class IntNumberOperatorFunction : public AbstractOperatorFunction {
private:
   op_int_number_func_t op_func;

public:
   DLLLOCAL IntNumberOperatorFunction(op_int_number_func_t f) : AbstractOperatorFunction(NT_NUMBER, NT_NUMBER), op_func(f) {}
   DLLLOCAL virtual ~IntNumberOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const;
};

class DefaultNothingOperatorFunction : public AbstractOperatorFunction {
public:
   DLLLOCAL DefaultNothingOperatorFunction() : AbstractOperatorFunction(NT_ALL, NT_ALL) {
   }
   DLLLOCAL virtual ~DefaultNothingOperatorFunction() {}
   DLLLOCAL virtual QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, int args, ExceptionSink* xsink) const {
      return QoreValue();
   }
};

typedef std::vector<AbstractOperatorFunction *> opfunc_list_t;

typedef bool (*op_bool_str_str_func_t)(const QoreString *l, const QoreString *r, ExceptionSink* xsink);

class QoreTreeNode;
class LocalVar;

typedef AbstractQoreNode* (*op_check_args_t)(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& resultTypeInfo, const char* name, const char* desc);

class Operator {
private:
   typedef std::vector<std::vector<int> > opmatrix_t;

   opfunc_list_t functions;
   opmatrix_t op_matrix;
   bool effect, lvalue;
   const char* name, *description;
   int args;
   int evalArgs;
   op_check_args_t check_args;

   DLLLOCAL static int match(qore_type_t ntype, qore_type_t rtype);
   DLLLOCAL int findFunction(qore_type_t ltype, qore_type_t rtype) const;
   DLLLOCAL int get_function(const QoreNodeEvalOptionalRefHolder &nleft, ExceptionSink* xsink) const;
   DLLLOCAL int get_function(const QoreNodeEvalOptionalRefHolder &nleft, const QoreNodeEvalOptionalRefHolder &nright, ExceptionSink* xsink) const;

public:
   // create a new Operator
   /** @param arg number of arguments (1 or 2)
       @param n the name of the operator
       @param desc the description of the operator
       @param n_evalArgs if the arguments should be evaluated before passing to the operator function or not
       @param n_effect if the operator modifies anything
       @param n_lvalue if the operator requires an lvalue on the left side (for modification, ex: $a =~ s/x/p/ )
   */
   DLLLOCAL Operator(int arg, const char* n, const char* desc, int n_evalArgs, bool n_effect, bool n_lvalue = false, op_check_args_t n_check_args = 0)
      : effect(n_effect), lvalue(n_lvalue),
        name(n), description(desc), args(arg),
        evalArgs(n_evalArgs), check_args(n_check_args) {
   }
   DLLLOCAL ~Operator();

   // returns 0 = OK, -1 = parse exception raised
   DLLLOCAL AbstractQoreNode* parseInit(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& resultTypeInfo);
   DLLLOCAL void init();
   DLLLOCAL bool hasEffect() const {
      return effect;
   }
   DLLLOCAL bool needsLValue() const {
      return lvalue;
   }
   DLLLOCAL int numArgs() const {
      return args;
   }
   DLLLOCAL void addFunction(op_bool_str_str_func_t f) {
      functions.push_back(new BoolStrStrOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_bool_str_regex_func_t f) {
      functions.push_back(new BoolStrRegexOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_bigint_str_str_func_t f) {
      functions.push_back(new BigIntStrStrOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_hash_string_func_t f) {
      functions.push_back(new HashStringOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_hash_list_func_t f) {
      functions.push_back(new HashListOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_str_str_str_func_t f) {
      functions.push_back(new StringStringStringOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_list_str_regex_func_t f) {
      functions.push_back(new ListStringRegexOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_varref_func_t f) {
      functions.push_back(new VarRefOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_func_t f) {
      functions.push_back(new EffectNoEvalOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_node_int_func_t f) {
      functions.push_back(new NodeIntOperatorFunction(f));
   }
   DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_node_func_t f) {
      functions.push_back(new NodeOperatorFunction(lt, rt, f));
   }
   DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_simple_bool_func_t f) {
      functions.push_back(new SimpleBoolOperatorFunction(lt, rt, f));
   }
   DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_noconvert_func_t f) {
      functions.push_back(new NoConvertOperatorFunction(lt, rt, f));
   }
   DLLLOCAL void addNoConvertFunction(qore_type_t lt, qore_type_t rt, op_bool_func_t f) {
      functions.push_back(new NoConvertBoolOperatorFunction(lt, rt, f));
   }
   DLLLOCAL void addEffectFunction(op_bool_func_t f) {
      functions.push_back(new EffectBoolOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_bool_int_func_t f) {
      functions.push_back(new BoolIntOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_int_int_func_t f) {
      functions.push_back(new IntIntOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_bool_float_func_t f) {
      functions.push_back(new BoolFloatOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_float_float_func_t f) {
      functions.push_back(new FloatFloatOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_compare_float_func_t f) {
      functions.push_back(new CompareFloatOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_logic_func_t f) {
      functions.push_back(new LogicOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_bool_date_func_t f) {
      functions.push_back(new BoolDateOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_bool_bin_func_t f) {
      functions.push_back(new BoolBinOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_date_func_t f) {
      functions.push_back(new DateOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_number_func_t f) {
      functions.push_back(new NumberOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_bool_number_func_t f) {
      functions.push_back(new BoolNumberOperatorFunction(f));
   }
   DLLLOCAL void addFunction(op_int_number_func_t f) {
      functions.push_back(new IntNumberOperatorFunction(f));
   }
   DLLLOCAL void addCompareDateFunction() {
      functions.push_back(new CompareDateOperatorFunction());
   }
   DLLLOCAL void addDefaultNothing() {
      functions.push_back(new DefaultNothingOperatorFunction());
   }

   DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_func_t f);
   DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_bool_func_t f);
   DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_bigint_func_t f);
   DLLLOCAL void addFunction(qore_type_t lt, qore_type_t rt, op_float_func_t f);

   DLLLOCAL QoreValue eval(const QoreValue l, const QoreValue r, bool ref_rv, ExceptionSink* xsink) const {
      ReferenceHolder<> lr(xsink);
      ReferenceHolder<> rr(xsink);
      if (l.type != QV_Node)
         lr = l.getReferencedValue();
      if (r.type != QV_Node)
         rr = r.getReferencedValue();
      return eval(lr ? *lr : l.getInternalNode(), rr ? *rr : r.getInternalNode(), ref_rv, xsink);
   }

   DLLLOCAL QoreValue eval(const AbstractQoreNode* l, const AbstractQoreNode* r, bool ref_rv, ExceptionSink* xsink) const;

   DLLLOCAL const char* getName() const {
      return name;
   }

   DLLLOCAL const char* getDescription() const {
      return description;
   }
};

#endif
