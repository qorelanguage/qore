/*
  Operator.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

#include <qore/Qore.h>
#include <qore/intern/QoreObjectIntern.h>
#include <qore/intern/qore_program_private.h>
#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/AbstractIteratorHelper.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pcre.h>

QoreString QoreQuestionMarkOperatorNode::question_mark_str("question mark operator expression");

DLLLOCAL OperatorList oplist;

DLLLOCAL extern const QoreTypeInfo* bigIntFloatOrNumberTypeInfo, * floatOrNumberTypeInfo;

// the standard, system-default operator pointers
Operator *OP_BIN_AND, *OP_BIN_OR, *OP_BIN_NOT, *OP_BIN_XOR, *OP_MINUS, *OP_PLUS, 
   *OP_MULT, *OP_DIV, *OP_SHIFT_LEFT, *OP_SHIFT_RIGHT, 
   *OP_LOG_CMP, 
   *OP_LIST_REF, *OP_OBJECT_REF, *OP_ELEMENTS, *OP_KEYS,
   *OP_SHIFT, *OP_POP, *OP_PUSH,
   *OP_UNSHIFT, *OP_REGEX_SUBST, *OP_LIST_ASSIGNMENT, 
   *OP_REGEX_TRANS, *OP_REGEX_EXTRACT, 
   *OP_CHOMP, *OP_TRIM, *OP_LOG_AND, *OP_LOG_OR, *OP_LOG_LT, 
   *OP_LOG_GT, *OP_LOG_EQ, *OP_LOG_NE, *OP_LOG_LE, *OP_LOG_GE, 
   *OP_ABSOLUTE_EQ, *OP_ABSOLUTE_NE, *OP_REGEX_MATCH, *OP_REGEX_NMATCH,
   *OP_EXISTS, *OP_INSTANCEOF, *OP_FOLDR, *OP_FOLDL,
   *OP_SELECT;

// call to get a node with reference count 1 (copy on write)
void ensure_unique(AbstractQoreNode* *v, ExceptionSink* xsink) {
   assert(*v);
   if (!(*v)->is_unique()) {
      AbstractQoreNode* old = *v;
      (*v) = old->realCopy();
      old->deref(xsink);
      assert(!*xsink);
   }
}

// operator functions for builtin types
static bool op_log_lt_bigint(int64 left, int64 right) {
   return left < right;
}

static bool op_log_gt_bigint(int64 left, int64 right) {
   return left > right;
}

static bool op_log_eq_bigint(int64 left, int64 right) {
   return left == right;
}

static bool op_log_eq_boolean(bool left, bool right) {
   return left == right;
}

static bool op_log_ne_boolean(bool left, bool right) {
   return left != right;
}

static bool op_log_ne_bigint(int64 left, int64 right) {
   return left != right;
}

static bool op_log_le_bigint(int64 left, int64 right) {
   return left <= right;
}

static bool op_log_ge_bigint(int64 left, int64 right) {
   return left >= right;
}

static bool op_log_eq_date(const DateTimeNode* left, const DateTimeNode* right) {
   return left->isEqual(right);
}

static bool op_log_gt_date(const DateTimeNode* left, const DateTimeNode* right) {
   return DateTime::compareDates(left, right) > 0;
}

static bool op_log_ge_date(const DateTimeNode* left, const DateTimeNode* right) {
   return DateTime::compareDates(left, right) >= 0;
}

static bool op_log_lt_date(const DateTimeNode* left, const DateTimeNode* right) {
   return DateTime::compareDates(left, right) < 0;
}

static bool op_log_le_date(const DateTimeNode* left, const DateTimeNode* right) {
   return DateTime::compareDates(left, right) <= 0;
}

static bool op_log_ne_date(const DateTimeNode* left, const DateTimeNode* right) {
   return !left->isEqual(right);
}

static bool op_log_lt_float(double left, double right) {
   return left < right;
}

static bool op_log_gt_float(double left, double right) {
   return left > right;
}

static bool op_log_eq_float(double left, double right) {
   return left == right;
}

static bool op_log_ne_float(double left, double right) {
   return left != right;
}

static bool op_log_le_float(double left, double right) {
   return left <= right;
}

static bool op_log_ge_float(double left, double right) {
   return left >= right;
}

static bool op_log_eq_string(const QoreString *left, const QoreString *right, ExceptionSink* xsink) {
   return !left->compareSoft(right, xsink);
}

static bool op_log_gt_string(const QoreString *left, const QoreString *right, ExceptionSink* xsink) {
   return left->compare(right) > 0;
}

static bool op_log_ge_string(const QoreString *left, const QoreString *right, ExceptionSink* xsink) {
   return right->compare(left) >= 0;
}

static bool op_log_lt_string(const QoreString *left, const QoreString *right, ExceptionSink* xsink) {
   return left->compare(right) < 0;
}

static bool op_log_le_string(const QoreString *left, const QoreString *right, ExceptionSink* xsink) {
   return left->compare(right) <= 0;
}

static bool op_log_ne_string(const QoreString *left, const QoreString *right, ExceptionSink* xsink) {
   return left->compareSoft(right, xsink);
}

static bool op_absolute_log_eq(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   QoreNodeEvalOptionalRefHolder lnp(left, xsink);
   if (*xsink)
      return false;

   QoreNodeEvalOptionalRefHolder rnp(right, xsink);
   if (*xsink)
      return false;

   if (is_nothing(*lnp)) {
      if (is_nothing(*rnp))
	 return true;
      else 
	 return false;
   }
   
   if (is_nothing(*rnp))
      return false;

   return lnp->is_equal_hard(*rnp, xsink);
}

static bool op_absolute_log_neq(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   return !op_absolute_log_eq(left, right, xsink);
}

static bool op_regex_match(const QoreString *left, const QoreRegexNode* right, ExceptionSink* xsink) {
   return right->exec(left, xsink);
}

static bool op_regex_nmatch(const QoreString *left, const QoreRegexNode* right, ExceptionSink* xsink) {
   return !right->exec(left, xsink);
}

// takes all arguments unevaluated so logic short-circuiting can happen
static bool op_log_or(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   bool l = left->boolEval(xsink);   
   if (*xsink)
      return false;

   // if left side is true, then do not evaluate right side
   return l ? true : right->boolEval(xsink);
}

static bool op_log_eq_all(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   qore_type_t lt = left ? left->getType() : -1;
   qore_type_t rt = right ? right->getType() : -1;
   return (lt != -1 && rt != -1) ? left->is_equal_soft(right, xsink) : false;
}

/*
static bool op_log_eq_list(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   if (left->getType() != NT_LIST)
      return false;
   if (right->getType() != NT_LIST)
      return false;

   const QoreListNode* l = reinterpret_cast<const QoreListNode*>(left);
   const QoreListNode* r = reinterpret_cast<const QoreListNode*>(right);
   return l->is_equal_soft(r, xsink);
}

static bool op_log_eq_hash(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   const QoreHashNode* lh = left->getType() == NT_HASH ? reinterpret_cast<const QoreHashNode*>(left) : 0;
   if (!lh)
      return false;

   const QoreHashNode* rh = right->getType() == NT_HASH ? reinterpret_cast<const QoreHashNode*>(right) : 0;
   if (!rh)
      return false;

   return !lh->compareSoft(rh, xsink);
}

static bool op_log_eq_object(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   if (left->getType() != NT_OBJECT)
      return false;
   if (right->getType() != NT_OBJECT)
      return false;

   const QoreObject *l = reinterpret_cast<const QoreObject *>(left);
   const QoreObject *r = reinterpret_cast<const QoreObject *>(right);
   return !l->compareSoft(r, xsink);
}

static bool op_log_eq_nothing(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   assert(left->getType() == NT_NOTHING && right->getType() == NT_NOTHING);
   return true;
}

// this function is the catch-all for all types
static bool op_log_eq_null(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   qore_type_t lt = left ? left->getType() : -1;
   qore_type_t rt = right ? right->getType() : -1;
   return (lt != -1 && rt != -1) ? left->is_equal_soft(right, xsink) : false;
}

static bool op_log_eq_binary(const AbstractQoreNode* left, const AbstractQoreNode* right) {
   const BinaryNode* l = left->getType() == NT_BINARY ? reinterpret_cast<const BinaryNode*>(left) : 0;
   const BinaryNode* r = right->getType() == NT_BINARY ? reinterpret_cast<const BinaryNode*>(right) : 0;
   assert(l || r);

   if (!l || !r)
      return false;
   return !l->compare(r);
}
*/

static bool op_log_ne_all(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   qore_type_t lt = left ? left->getType() : -1;
   qore_type_t rt = right ? right->getType() : -1;
   return (lt != -1 && rt != -1) ? !left->is_equal_soft(right, xsink) : true;
}

/*
static bool op_log_ne_list(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   if (left->getType() != NT_LIST)
      return true;
   if (right->getType() != NT_LIST)
      return true;

   const QoreListNode* l = reinterpret_cast<const QoreListNode*>(left);
   const QoreListNode* r = reinterpret_cast<const QoreListNode*>(right);
   return !l->is_equal_soft(r, xsink);
}

static bool op_log_ne_hash(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   if (left->getType() != NT_HASH)
      return true;
   if (right->getType() != NT_HASH)
      return true;

   const QoreHashNode* lh = reinterpret_cast<const QoreHashNode*>(left);
   const QoreHashNode* rh = reinterpret_cast<const QoreHashNode*>(right);
   return lh->compareSoft(rh, xsink);
}

static bool op_log_ne_object(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   if (left->getType() != NT_OBJECT)
      return true;
   if (right->getType() != NT_OBJECT)
      return true;

   const QoreObject *l = reinterpret_cast<const QoreObject *>(left);
   const QoreObject *r = reinterpret_cast<const QoreObject *>(right);
   return l->compareSoft(r, xsink);
}

static bool op_log_ne_nothing(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   assert(left->getType() == NT_NOTHING && right->getType() == NT_NOTHING);
   return false;
}

static bool op_log_ne_null(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   if (left && left->getType() == NT_NULL && right && right->getType() == NT_NULL)
      return false;
   return true;
}

static bool op_log_ne_binary(const AbstractQoreNode* left, const AbstractQoreNode* right) {
   const BinaryNode* l = left->getType() == NT_BINARY ? reinterpret_cast<const BinaryNode*>(left) : 0;
   const BinaryNode* r = right->getType() == NT_BINARY ? reinterpret_cast<const BinaryNode*>(right) : 0;
   assert(l || r);

   if (!l || !r)
      return true;
   return l->compare(r);
}
*/

static bool op_exists(const AbstractQoreNode* left, const AbstractQoreNode* x, ExceptionSink* xsink) {
   assert(!left->needs_eval());
   return is_nothing(left) ? false : true;
}

static bool op_instanceof(const AbstractQoreNode* l, const AbstractQoreNode* r, ExceptionSink* xsink) {
   assert(r && r->getType() == NT_CLASSREF);

   QoreNodeEvalOptionalRefHolder nl(l, xsink);
   if (*xsink || !nl || nl->getType() != NT_OBJECT)
      return false;

   const QoreObject *o = reinterpret_cast<const QoreObject*>(*nl);
   return o->validInstanceOf(*reinterpret_cast<const ClassRefNode*>(r)->getClass());
}

// takes all arguments unevaluated so logic short-circuiting can happen
static bool op_log_and(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   // if left side is 0, then do not evaluate right side
   bool l = left->boolEval(xsink);
   if (!l || xsink->isEvent())
      return false;
   return right->boolEval(xsink);
}

static int64 op_cmp_bigint(int64 left, int64 right) {
   return left - right;
}

static int64 op_minus_bigint(int64 left, int64 right) {
   return left - right;
}

static int64 op_plus_bigint(int64 left, int64 right) {
   return left + right;
}

static int64 op_multiply_bigint(int64 left, int64 right) {
   return left * right;
}

static int64 op_divide_bigint(int64 left, int64 right, ExceptionSink* xsink) {
   if (!right) {
      xsink->raiseException("DIVISION-BY-ZERO", "division by zero in integer expression");
      return 0;
   }
   return left / right;
}

static DateTimeNode* op_minus_date(const DateTimeNode* left, const DateTimeNode* right) {
    return left->subtractBy(right);
}

static DateTimeNode* op_plus_date(const DateTimeNode* left, const DateTimeNode* right) {
    return left->add(right);
}

static double op_minus_float(double left, double right) {
   return left - right;
}

static double op_plus_float(double left, double right) {
   return left + right;
}

static double op_multiply_float(double left, double right) {
   return left * right;
}

static double op_divide_float(double left, double right, ExceptionSink* xsink) {
   if (!right) {
      xsink->raiseException("DIVISION-BY-ZERO", "division by zero in floating-point expression");
      return 0.0;
   }
   return left / right;
}

static bool op_log_lt_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return left->compare(*right) < 0;
}

static bool op_log_le_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return left->compare(*right) <= 0;
}

static bool op_log_gt_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return left->compare(*right) > 0;
}

static bool op_log_ge_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return left->compare(*right) >= 0;
}

static bool op_log_eq_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return !left->compare(*right);
}

static bool op_log_ne_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return (bool)left->compare(*right);
}

static int64 op_cmp_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return (int64)left->compare(*right);
}

static QoreNumberNode* op_plus_number(const QoreNumberNode* left, const QoreNumberNode* right, ExceptionSink* xsink) {
   return left->doPlus(*right);
}

static QoreNumberNode* op_minus_number(const QoreNumberNode* left, const QoreNumberNode* right, ExceptionSink* xsink) {
   return left->doMinus(*right);
}

static QoreNumberNode* op_multiply_number(const QoreNumberNode* left, const QoreNumberNode* right, ExceptionSink* xsink) {
   return left->doMultiply(*right);
}

static QoreNumberNode* op_divide_number(const QoreNumberNode* left, const QoreNumberNode* right, ExceptionSink* xsink) {
   return left->doDivideBy(*right, xsink);
}

static QoreStringNode* op_plus_string(const QoreString *left, const QoreString *right, ExceptionSink* xsink) {
   QoreStringNodeHolder str(new QoreStringNode(*left));
   //printd(5, "op_plus_string() (%d) %p \"%s\" + (%d) %p \"%s\"\n", left->strlen(), left->getBuffer(), left->getBuffer(), right->strlen(), right->getBuffer(), right->getBuffer());
   //printd(5, "op_plus_string() str= (%d) %p \"%s\"\n", str->strlen(), str->getBuffer(), str->getBuffer());
   str->concat(right, xsink);
   if (*xsink)
      return 0;
   
   printd(5, "op_plus_string() result=\"%s\"\n", str->getBuffer());
   return str.release();
}

static int64 op_cmp_string(const QoreString *left, const QoreString *right, ExceptionSink* xsink) {
   return (int64)left->compare(right);
}

static int64 op_elements(const AbstractQoreNode* left, const AbstractQoreNode* null, ExceptionSink* xsink) {
   QoreNodeEvalOptionalRefHolder np(left, xsink);
   if (*xsink || !np)
      return 0;

   qore_type_t ltype = np->getType();

   if (ltype == NT_LIST)
      return reinterpret_cast<const QoreListNode*>(*np)->size();

   if (ltype == NT_STRING)
      return reinterpret_cast<const QoreStringNode*>(*np)->length();

   if (ltype == NT_HASH)
      return reinterpret_cast<const QoreHashNode*>(*np)->size();	 

   if (ltype == NT_OBJECT)
      return reinterpret_cast<const QoreObject *>(*np)->size(xsink);

   if (ltype == NT_BINARY)
      return reinterpret_cast<const BinaryNode*>(*np)->size();

   return 0;
}

static QoreListNode* get_keys(const AbstractQoreNode* p, ExceptionSink* xsink) {   
   if (!p)
      return 0;

   if (p->getType() == NT_HASH)
      return reinterpret_cast<const QoreHashNode*>(p)->getKeys();

   if (p->getType() == NT_OBJECT)
      return reinterpret_cast<const QoreObject *>(p)->getMemberList(xsink);

   return 0;
}

// FIXME: do not need ref_rv here - also do not need second argument
static AbstractQoreNode* op_keys(const AbstractQoreNode* left, const AbstractQoreNode* null, bool ref_rv, ExceptionSink* xsink) {
   QoreNodeEvalOptionalRefHolder np(left, xsink);
   if (*xsink)
      return 0;

   return get_keys(*np, xsink);
}

static AbstractQoreNode* op_regex_subst(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, ExceptionSink* xsink) {
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // if it's not a string, then do nothing
   if (!v.checkType(NT_STRING))
      return 0;

   const QoreStringNode* str = reinterpret_cast<const QoreStringNode*>(v.getValue());

   assert(right && right->getType() == NT_REGEX_SUBST);
   const RegexSubstNode* rs = reinterpret_cast<const RegexSubstNode*>(right);

   // get new value
   QoreStringNode* nv = rs->exec(str, xsink);

   // if there is an exception above, nv = 0
   if (xsink->isEvent())
      return 0;

   // assign new value to lvalue (no exception possible here)
   v.assign(nv);
   assert(!*xsink);

   // reference for return value if necessary
   return ref_rv ? nv->refSelf() : 0;
}

static AbstractQoreNode* op_transliterate(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, ExceptionSink* xsink) {
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // if it's not a string, then do nothing
   if (!v.checkType(NT_STRING))
      return 0;

   const QoreStringNode* str = reinterpret_cast<const QoreStringNode*>(v.getValue());

   // get new value
   assert(right && right->getType() == NT_REGEX_TRANS);
   QoreStringNode* nv = reinterpret_cast<const RegexTransNode*>(right)->exec(str, xsink);

   // if there is an exception above, nv = 0
   if (*xsink)
      return 0;

   // assign new value to lvalue (no exception possible here)
   v.assign(nv);
   assert(!*xsink);

   // reference for return value
   return ref_rv ? nv->refSelf() : 0;
}

static AbstractQoreNode* op_list_ref(const AbstractQoreNode* left, const AbstractQoreNode* index, ExceptionSink* xsink) {
   QoreNodeEvalOptionalRefHolder lp(left, xsink);

   // return 0 if left side is not a list or string (or exception)
   if (!lp || *xsink)
      return 0;

   qore_type_t t = lp->getType();
   if (t != NT_LIST && t != NT_STRING && t != NT_BINARY)
      return 0;

   AbstractQoreNode* rv = 0;
   int ind = index->integerEval(xsink);
   if (!*xsink) {
      // get value
      if (t == NT_LIST) {
	 const QoreListNode* l = reinterpret_cast<const QoreListNode*>(*lp);
	 rv = l->get_referenced_entry(ind);
      }
      else if (t == NT_BINARY) {
	 const BinaryNode* b = reinterpret_cast<const BinaryNode*>(*lp);
	 if (ind < 0 || (unsigned)ind >= b->size())
	    return 0;
	 return new QoreBigIntNode(((unsigned char* )b->getPtr())[ind]);
      }
      else if (ind >= 0) {
	 const QoreStringNode* lpstr = reinterpret_cast<const QoreStringNode*>(*lp);
	 rv = lpstr->substr(ind, 1, xsink);
      }
      //printd(5, "op_list_ref() index=%d, rv=%p\n", ind, rv);
   }
   return rv;
}

// for the member name, a string is required.  non-string arguments will
// be converted.  The null string can also be used
static AbstractQoreNode* op_object_ref(const AbstractQoreNode* left, const AbstractQoreNode* member, bool ref_rv, ExceptionSink* xsink) {
   QoreNodeEvalOptionalRefHolder op(left, xsink);
   if (*xsink || !op)
      return 0;

   if (op->getType() == NT_HASH) {
      const QoreHashNode* h = reinterpret_cast<const QoreHashNode*>(*op);

      // evaluate member expression
      QoreNodeEvalOptionalRefHolder mem(member, xsink);
      if (*xsink)
	 return 0;

      if (mem && mem->getType() == NT_LIST) {
	 return h->getSlice(reinterpret_cast<const QoreListNode*>(*mem), xsink);
      }

      QoreStringNodeValueHelper key(*mem);
      return h->evalKeyValue(*key, xsink);      
   }
   if (op->getType() != NT_OBJECT)
      return 0;

   QoreObject *o = const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(*op));

   // evaluate member expression
   QoreNodeEvalOptionalRefHolder mem(member, xsink);
   if (*xsink)
      return 0;

   if (mem && mem->getType() == NT_LIST) {
      return o->getSlice(reinterpret_cast<const QoreListNode*>(*mem), xsink);
   }

   QoreStringNodeValueHelper key(*mem);
   return o->evalMember(*key, xsink);
}

static AbstractQoreNode* op_list_assignment(const AbstractQoreNode* n_left, const AbstractQoreNode* right, bool ref_rv, ExceptionSink* xsink) {
   assert(n_left && n_left->getType() == NT_LIST);
   const QoreListNode* left = reinterpret_cast<const QoreListNode*>(n_left);
   
   /* assign new value, this value gets referenced with the
      eval(xsink) call, so there's no need to reference it again
      for the variable assignment - however it does need to be
      copied/referenced for the return value
   */
   QoreNodeEvalOptionalRefHolder new_value(right, xsink);
   if (*xsink)
      return 0;

   // get values and save
   for (unsigned i = 0; i < left->size(); i++) {
      const AbstractQoreNode* lv = left->retrieve_entry(i);

      // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
      LValueHelper v(lv, xsink);
      if (!v)
	 return 0;

      // if there's only one value, then save it
      if (new_value && new_value->getType() == NT_LIST) { // assign to list position
	 const QoreListNode* nv = reinterpret_cast<const QoreListNode*>(*new_value);
	 v.assign(nv->get_referenced_entry(i));
      }
      else {
	 if (!i)
	    v.assign(new_value.getReferencedValue());
	 else
	    v.assign(QoreValue());
      }
      if (*xsink)
	 return 0;
   }


   return ref_rv ? new_value.getReferencedValue() : 0;
}

// this is the default (highest-priority) function for the + operator, so any type could be sent here on either side
static AbstractQoreNode* op_plus_list(const AbstractQoreNode* left, const AbstractQoreNode* right) {
   if (left->getType() == NT_LIST) {
      const QoreListNode* l = reinterpret_cast<const QoreListNode*>(left);
      QoreListNode* rv = l->copy();
      if (right->getType() == NT_LIST)
	 rv->merge(reinterpret_cast<const QoreListNode*>(right));
      else
	 rv->push(right->refSelf());
      //printd(5, "op_plus_list() returning list=%p size=%d\n", rv, rv->size());
      return rv;
   }

   if (right->getType() != NT_LIST)
      return 0;
   const QoreListNode* r = reinterpret_cast<const QoreListNode*>(right);

   QoreListNode* rv = new QoreListNode();
   rv->push(left->refSelf());
   rv->merge(r);
   return rv;
}

static AbstractQoreNode* op_plus_hash_hash(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   if (left->getType() == NT_HASH) {
      const QoreHashNode* lh = reinterpret_cast<const QoreHashNode*>(left);

      if (right->getType() != NT_HASH)
	 return left->refSelf();
      const QoreHashNode* rh = reinterpret_cast<const QoreHashNode*>(right);

      ReferenceHolder<QoreHashNode> rv(lh->copy(), xsink);
      rv->merge(rh, xsink);
      if (*xsink)
	 return 0;
      return rv.release();
   }

   return right->getType() == NT_HASH ? right->refSelf() : 0;
}

static AbstractQoreNode* op_plus_hash_object(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   if (left->getType() == NT_HASH) {
      const QoreHashNode* lh = reinterpret_cast<const QoreHashNode*>(left);
      if (right->getType() != NT_OBJECT)
	 return left->refSelf();

      QoreObject *r = const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(right));
      ReferenceHolder<QoreHashNode> rv(lh->copy(), xsink);
      r->mergeDataToHash(*rv, xsink);
      if (*xsink)
	 return 0;
      
      return rv.release();
   }

   return right->getType() == NT_OBJECT ? right->refSelf() : 0;
}

// note that this will return a hash
static AbstractQoreNode* op_plus_object_hash(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   if (left->getType() == NT_OBJECT) {
      if (right->getType() != NT_HASH)
	 return left->refSelf();

      QoreObject *l = const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(left));
      const QoreHashNode* rh = reinterpret_cast<const QoreHashNode*>(right);

      ReferenceHolder<QoreHashNode> h(l->copyData(xsink), xsink);
      if (*xsink)
	 return 0;
      
      h->merge(rh, xsink);
      if (*xsink)
	 return 0;
      
      return h.release();
   }

   return right->getType() == NT_HASH ? right->refSelf() : 0;
}

static AbstractQoreNode* op_plus_binary_binary(const AbstractQoreNode* left, const AbstractQoreNode* right, ExceptionSink* xsink) {
   if (right->getType() != NT_BINARY)
      return left ? left->refSelf() : 0;

   if (left->getType() != NT_BINARY)
      return right->refSelf();

   const BinaryNode* l = reinterpret_cast<const BinaryNode*>(left);
   const BinaryNode* r = reinterpret_cast<const BinaryNode*>(right);

   BinaryNode* rv = l->copy();
   rv->append(r);
   return rv;
}

static int64 op_cmp_double(double left, double right) {
   if (left < right)
       return -1;
       
   if (left == right)
      return 0;
       
   return 1;
}

static int64 op_bin_and_int(int64 left, int64 right) {
   return left & right;
}

static int64 op_bin_or_int(int64 left, int64 right) {
   return left | right;
}

static int64 op_bin_xor_int(int64 left, int64 right) {
   return left ^ right;
}

static int64 op_shift_left_int(int64 left, int64 right) {
   return left << right;
}

static int64 op_shift_right_int(int64 left, int64 right) {
   return left >> right;
}

// unshift lvalue, element
static AbstractQoreNode* op_unshift(const AbstractQoreNode* left, const AbstractQoreNode* elem, bool ref_rv, ExceptionSink* xsink) {
   printd(5, "op_unshift(%p, %p, isEvent=%d)\n", left, elem, xsink->isEvent());

   QoreNodeEvalOptionalRefHolder value(elem, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(left, xsink);
   if (!val)
      return 0;

   // assign to a blank list if the lvalue has no value yet but is typed as a list
   if (val.getType() == NT_NOTHING && val.getTypeInfo() == listTypeInfo && val.assign(listTypeInfo->getDefaultValue()))
      return 0;

   // value is not a list, so throw exception
   if (val.getType() != NT_LIST) {
      xsink->raiseException("UNSHIFT-ERROR", "first argument to unshift is not a list");
      return 0;
   }

   // no exception can occur here
   val.ensureUnique();

   QoreListNode* l = reinterpret_cast<QoreListNode*>(val.getValue());

   printd(5, "op_unshift() about to call unshift() on list node %p (%d) with element %p\n", l, l->size(), elem);
   l->insert(value.getReferencedValue());

   // reference for return value
   return ref_rv ? l->refSelf() : 0;
}

static AbstractQoreNode* op_shift(const AbstractQoreNode* left, const AbstractQoreNode* x, bool ref_rv, ExceptionSink* xsink) {
   //QORE_TRACE("op_shift()");
   printd(5, "op_shift(%p, %p, isEvent=%d)\n", left, x, xsink->isEvent());

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(left, xsink);
   if (!val)
      return 0;

   if (val.getType() != NT_LIST)
      return 0;

   // no exception can occurr here
   val.ensureUnique();

   QoreListNode* l = reinterpret_cast<QoreListNode*>(val.getValue());

   printd(5, "op_shift() about to call QoreListNode::shift() on list node %p (%d)\n", l, l->size());
   // the list reference will now be the reference for return value
   // therefore no need to reference again
   return l->shift();
}

static AbstractQoreNode* op_pop(const AbstractQoreNode* left, const AbstractQoreNode* x, bool ref_rv, ExceptionSink* xsink) {
   printd(5, "op_pop(%p, %p, isEvent=%d)\n", left, x, xsink->isEvent());

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(left, xsink);
   if (!val)
      return 0;

   // assign to a blank list if the lvalue has no vaule yet but is typed as a list
   if (val.getType() == NT_NOTHING && val.getTypeInfo() == listTypeInfo && val.assign(listTypeInfo->getDefaultValue()))
      return 0;

   if (val.getType() != NT_LIST)
      return 0;

   // no exception can occurr here
   val.ensureUnique();

   QoreListNode* l = reinterpret_cast<QoreListNode*>(val.getValue());

   printd(5, "op_pop() about to call QoreListNode::pop() on list node %p (%d)\n", l, l->size());

   // the list reference will now be the reference for return value
   // therefore no need to reference again
   return l->pop();
}

static AbstractQoreNode* op_push(const AbstractQoreNode* left, const AbstractQoreNode* elem, bool ref_rv, ExceptionSink* xsink) {
   //QORE_TRACE("op_push()");
   printd(5, "op_push(%p, %p, isEvent=%d)\n", left, elem, xsink->isEvent());

   QoreNodeEvalOptionalRefHolder value(elem, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(left, xsink);
   if (!val)
      return 0;

   // assign to a blank list if the lvalue has no vaule yet but is typed as a list
   if (val.getType() == NT_NOTHING && val.getTypeInfo() == listTypeInfo && val.assign(listTypeInfo->getDefaultValue()))
      return 0;

   if (val.getType() != NT_LIST)
      return 0;

   // no exception can occurr here
   val.ensureUnique();

   QoreListNode* l = reinterpret_cast<QoreListNode*>(val.getValue());

   printd(5, "op_push() about to call push() on list node %p (%d) with element %p\n", l, l->size(), elem);

   l->push(value.getReferencedValue());
   // reference for return value
   return ref_rv ? l->refSelf() : 0;
}

static int64 op_chomp(const AbstractQoreNode* arg, const AbstractQoreNode* x, ExceptionSink* xsink) {
   //QORE_TRACE("op_chomp()");

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(arg, xsink);
   if (!val)
      return 0;

   qore_type_t vtype = val.getType();
   if (vtype != NT_LIST && vtype != NT_STRING && vtype != NT_HASH)
      return 0;

   // note that no exception can happen here
   val.ensureUnique();
   assert(!*xsink);

   if (vtype == NT_STRING)
      return reinterpret_cast<QoreStringNode*>(val.getValue())->chomp();

   int64 count = 0;   

   if (vtype == NT_LIST) {
      QoreListNode* l = reinterpret_cast<QoreListNode*>(val.getValue());
      ListIterator li(l);
      while (li.next()) {
	 AbstractQoreNode** v = li.getValuePtr();
	 if (*v && (*v)->getType() == NT_STRING) {
	    // note that no exception can happen here
	    ensure_unique(v, xsink);
	    assert(!*xsink);
	    QoreStringNode* vs = reinterpret_cast<QoreStringNode*>(*v);
	    count += vs->chomp();
	 }
      }      
      return count;
   }

   // must be a hash
   QoreHashNode* vh = reinterpret_cast<QoreHashNode*>(val.getValue());
   HashIterator hi(vh);
   while (hi.next()) {
      AbstractQoreNode** v = hi.getValuePtr();
      if (*v && (*v)->getType() == NT_STRING) {
	 // note that no exception can happen here
	 ensure_unique(v, xsink);
	 assert(!*xsink);
	 QoreStringNode* vs = reinterpret_cast<QoreStringNode*>(*v);
	 count += vs->chomp();
      }
   }
   return count;
}

static AbstractQoreNode* op_trim(const AbstractQoreNode* arg, const AbstractQoreNode* x, bool ref_rv, ExceptionSink* xsink) {
   //QORE_TRACE("op_trim()");
   
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(arg, xsink);
   if (!val)
      return 0;

   qore_type_t vtype = val.getType();
   if (vtype != NT_LIST && vtype != NT_STRING && vtype != NT_HASH)
      return 0;
   
   // note that no exception can happen here
   val.ensureUnique();
   assert(!*xsink);

   if (vtype == NT_STRING) {
      QoreStringNode* vs = reinterpret_cast<QoreStringNode*>(val.getValue());
      vs->trim();
   }
   else if (vtype == NT_LIST) {
      QoreListNode* l = reinterpret_cast<QoreListNode*>(val.getValue());
      ListIterator li(l);
      while (li.next()) {
	 AbstractQoreNode** v = li.getValuePtr();
	 if (*v && (*v)->getType() == NT_STRING) {
	    // note that no exception can happen here
	    ensure_unique(v, xsink);
	    assert(!*xsink);
	    QoreStringNode* vs = reinterpret_cast<QoreStringNode*>(*v);
	    vs->trim();
	 }
      }      
   }
   else { // is a hash
      QoreHashNode* vh = reinterpret_cast<QoreHashNode*>(val.getValue());
      HashIterator hi(vh);
      while (hi.next()) {
	 AbstractQoreNode** v = hi.getValuePtr();
	 if (*v && (*v)->getType() == NT_STRING) {
	    // note that no exception can happen here
	    assert(!*xsink);
	    ensure_unique(v, xsink);
	    QoreStringNode* vs = reinterpret_cast<QoreStringNode*>(*v);
	    vs->trim();
	 }
      }
   }

   // reference for return value
   if (!ref_rv)
      return 0;
   return val.getReferencedNodeValue();
}

static AbstractQoreNode* op_fold_iterator(const AbstractQoreNode* left, AbstractIteratorHelper& h, bool ref_rv, ExceptionSink* xsink) {
   // set offset in thread-local data for "$#"
   ImplicitElementHelper eh(-1);

   // first try to get first argument
   bool b = h.next(xsink);
   // if there is no first argument or an exception occurred, then return 0
   if (!b || *xsink)
      return 0;

   // get first argument value
   ReferenceHolder<AbstractQoreNode> result(h.getValue(xsink), xsink);
   if (*xsink)
      return 0;

   while (true) {
      bool b = h.next(xsink);
      if (*xsink)
         return 0;
      if (!b)
         break;

      // get next argument value
      ReferenceHolder<AbstractQoreNode> arg(h.getValue(xsink), xsink);
      if (*xsink)
         return 0;

      // create argument list for fold expression
      QoreListNode* args = new QoreListNode;
      args->push(result.release());
      args->push(arg.release());
      ArgvContextHelper argv_helper(args, xsink);
      result = left->eval(xsink);
      if (*xsink)
         return 0;
   }

   return result.release();
}

static AbstractQoreNode* op_foldl(const AbstractQoreNode* left, const AbstractQoreNode* arg_exp, bool ref_rv, ExceptionSink* xsink) {
   // conditionally evaluate argument
   QoreNodeEvalOptionalRefHolder arg(arg_exp, xsink);
   if (!arg || *xsink)
      return 0;

   // return the argument if there is no list
   qore_type_t t = arg->getType();
   if (t != NT_LIST) {
      if (t == NT_OBJECT) {
         AbstractIteratorHelper h(xsink, "foldl operator", const_cast<QoreObject*>(reinterpret_cast<const QoreObject*>(*arg)));
         if (*xsink)
            return 0;
         if (h)
            return op_fold_iterator(left, h, ref_rv, xsink);
      }
      return arg.getReferencedValue();
   }

   const QoreListNode* l = reinterpret_cast<const QoreListNode*>(*arg);

   // returns NOTHING if the list is empty
   if (!l->size())
      return 0;

   ReferenceHolder<AbstractQoreNode> result(l->get_referenced_entry(0), xsink);

   // return the first element if the list only has one element
   if (l->size() == 1)
      return result.release();

   // skip the first element
   ConstListIterator li(l, 0);
   while (li.next()) {
      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(li.index());
      // create argument list
      QoreListNode* args = new QoreListNode();
      args->push(result.release());
      args->push(li.getReferencedValue());

      ArgvContextHelper argv_helper(args, xsink);

      result = left->eval(xsink);
      if (*xsink)
	 return 0;
   }
   return result.release();
}

static AbstractQoreNode* op_foldr(const AbstractQoreNode* left, const AbstractQoreNode* arg_exp, bool ref_rv, ExceptionSink* xsink) {
   // conditionally evaluate argument
   QoreNodeEvalOptionalRefHolder arg(arg_exp, xsink);
   if (!arg || *xsink)
      return 0;

   // return the argument if there is no list
   qore_type_t t = arg->getType();
   if (t != NT_LIST) {
      if (t == NT_OBJECT) {
         AbstractIteratorHelper h(xsink, "foldr operator", const_cast<QoreObject*>(reinterpret_cast<const QoreObject*>(*arg)), false);
         if (*xsink)
            return 0;
         if (h)
            return op_fold_iterator(left, h, ref_rv, xsink);
      }
      return arg.getReferencedValue();
   }

   const QoreListNode* l = reinterpret_cast<const QoreListNode*>(*arg);

   // returns NOTHING if the list is empty
   if (!l->size())
      return 0;

   ReferenceHolder<AbstractQoreNode> result(l->get_referenced_entry(l->size() - 1), xsink);

   // return the first element if the list only has one element
   if (l->size() == 1)
      return result.release();

   ConstListIterator li(l);
   // skip the first element
   li.prev();
   while (li.prev()) {
      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(li.index());
      // create argument list
      QoreListNode* args = new QoreListNode();
      args->push(result.release());
      args->push(li.getReferencedValue());

      ArgvContextHelper argv_helper(args, xsink);

      result = left->eval(xsink);
      if (*xsink)
	 return 0;
   }
   return result.release();
}

static AbstractQoreNode* op_select_iterator(const AbstractQoreNode* select, AbstractIteratorHelper& h, bool ref_rv, ExceptionSink* xsink) {
   qore_size_t i = 0;
   ReferenceHolder<QoreListNode> rv(new QoreListNode, xsink);
   while (true) {
      bool b = h.next(xsink);
      if (*xsink)
         return 0;
      if (!b)
         break;

      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(i++);

      ReferenceHolder<> iv(h.getValue(xsink), xsink);
      if (*xsink)
         return 0;
      SingleArgvContextHelper argv_helper(*iv, xsink);
      if (*xsink)
         return 0;
      b = select->boolEval(xsink);
      if (*xsink)
         return 0;
      if (b) {
         // get next argument value
         ReferenceHolder<AbstractQoreNode> arg(h.getValue(xsink), xsink);
         if (*xsink)
            return 0;
         rv->push(arg.release());
      }
   }
   return rv.release();
}

static AbstractQoreNode* op_select(const AbstractQoreNode* arg_exp, const AbstractQoreNode* select, bool ref_rv, ExceptionSink* xsink) {
   // conditionally evaluate argument
   QoreNodeEvalOptionalRefHolder arg(arg_exp, xsink);
   if (!arg || *xsink)
      return 0;

   // return the argument if there is no list
   qore_type_t t = arg->getType();
   if (t != NT_LIST) {
      if (t == NT_OBJECT) {
         AbstractIteratorHelper h(xsink, "select operator", const_cast<QoreObject*>(reinterpret_cast<const QoreObject*>(*arg)));
         if (*xsink)
            return 0;
         if (h)
            return op_select_iterator(select, h, ref_rv, xsink);
      }
      SingleArgvContextHelper argv_helper(*arg, xsink);
      bool b = select->boolEval(xsink);
      if (*xsink)
         return 0;

      return b ? arg.getReferencedValue() : 0;
   }

   ReferenceHolder<QoreListNode> rv(new QoreListNode(), xsink);
   ConstListIterator li(reinterpret_cast<const QoreListNode*>(*arg));
   while (li.next()) {
      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(li.index());
      SingleArgvContextHelper argv_helper(li.getValue(), xsink);
      bool b = select->boolEval(xsink);
      if (*xsink)
	 return 0;
      if (b)
	 rv->push(li.getReferencedValue());
   }
   return rv.release();
}

static QoreHashNode* op_minus_hash_string(const QoreHashNode* h, const QoreString *s, ExceptionSink* xsink) {
   ReferenceHolder<QoreHashNode> nh(h->copy(), xsink);
   nh->removeKey(s, xsink);
   if (*xsink)
      return 0;
   return nh.release();
}

static QoreHashNode* op_minus_hash_list(const QoreHashNode* h, const QoreListNode* l, ExceptionSink* xsink) {
   ReferenceHolder<QoreHashNode> x(h->copy(), xsink);

   // treat each element in the list as a string giving a key to delete
   ConstListIterator li(l);
   while (li.next()) {
      QoreStringValueHelper val(li.getValue());
      
      x->removeKey(*val, xsink);
      if (*xsink)
	 return 0;
   }
   return x.release();
}

static AbstractQoreNode* op_regex_extract(const QoreString *left, const QoreRegexNode* right, ExceptionSink* xsink) {
   return right->extractSubstrings(left, xsink);
}

static AbstractQoreNode* get_node_type(const AbstractQoreNode* n, qore_type_t t) {
   assert(n);
   assert(n->getType() != t);

   if (t == NT_STRING) {
      QoreStringNode* str = new QoreStringNode();
      n->getStringRepresentation(*str);
      return str;
   }

   if (t == NT_INT)
      return new QoreBigIntNode(n->getAsBigInt());

   if (t == NT_FLOAT)
      return new QoreFloatNode(n->getAsFloat());

   if (t == NT_NUMBER)
      return new QoreNumberNode(n);

   if (t == NT_BOOLEAN)
      return get_bool_node(n->getAsBool());

   if (t == NT_DATE) {
      DateTimeNode* dt = new DateTimeNode();
      n->getDateTimeRepresentation(*dt);
      return dt;
   }
   
   if (t == NT_LIST) {
      QoreListNode* l = new QoreListNode();
      l->push(n ? n->refSelf() : 0);
      return l;
   }

   printd(0, "DEBUG: get_node_type() got type '%s', aborting\n", n->getTypeName());
   assert(false);
   return 0;
}

OperatorFunction::OperatorFunction(qore_type_t lt, qore_type_t rt, op_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f) {
}

BoolOperatorFunction::BoolOperatorFunction(qore_type_t lt, qore_type_t rt, op_bool_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f) {
}

BigIntOperatorFunction::BigIntOperatorFunction(qore_type_t lt, qore_type_t rt, op_bigint_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f) {
}

FloatOperatorFunction::FloatOperatorFunction(qore_type_t lt, qore_type_t rt, op_float_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f) {
}

QoreValue OperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL)) {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1)
      return op_func(left, 0, ref_rv, xsink);

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL)) {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return op_func(left, right, ref_rv, xsink);
}

QoreValue NodeOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   if (!ref_rv)
      return QoreValue();
   return op_func(left, right, xsink);
}

QoreValue EffectNoEvalOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   return op_func(left, right, ref_rv, xsink);
}

QoreValue HashStringOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   assert(left && left->getType() == NT_HASH);
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return QoreValue();

   QoreStringValueHelper r(right);
   return op_func(reinterpret_cast<const QoreHashNode*>(left), *r, xsink);
}

QoreValue HashListOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   assert(left && left->getType() == NT_HASH);
   assert(right && right->getType() == NT_LIST);
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return QoreValue();

   return op_func(reinterpret_cast<const QoreHashNode*>(left), reinterpret_cast<const QoreListNode*>(right), xsink);
}

QoreValue NoConvertOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return QoreValue();

   return op_func(left, right);
}

QoreValue EffectBoolOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   return op_func(left, right, xsink);
}

QoreValue SimpleBoolOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   if (!ref_rv)
      return QoreValue();

   return op_func(left, right);
}

QoreValue VarRefOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   assert(left);
   return op_func(left, ref_rv, xsink);
}

QoreValue StringStringStringOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return QoreValue();

   QoreStringValueHelper l(left);
   QoreStringValueHelper r(right);
   return op_func(*l, *r, xsink);
}

QoreValue ListStringRegexOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   assert(right && right->getType() == NT_REGEX);

   // conditionally evaluate left-hand node only
   QoreNodeEvalOptionalRefHolder le(left, xsink);
   // return immediately if the return value is ignored, this statement will have no effect and there can be no (other) side-effects
   if (*xsink || !ref_rv)
      return QoreValue();

   QoreStringValueHelper l(*le);
   return op_func(*l, reinterpret_cast<const QoreRegexNode*>(right), xsink);
}

QoreValue BoolOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL)) {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1)
      return op_func(left, 0, xsink);

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL)) {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return op_func(left, right, xsink);
}

QoreValue NoConvertBoolOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   return args == 1 ? op_func(left, 0, xsink) : op_func(left, right, xsink);
}

QoreValue BoolStrStrOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   QoreStringValueHelper l(left);
   if (args == 1)
      return op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return op_func(*l, *r, xsink);
}

QoreValue BoolDateOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // these functions can have no side-effects
   if (!ref_rv)
      return QoreValue();

   DateTimeNodeValueHelper l(left);
   DateTimeNodeValueHelper r(right);
   return op_func(*l, *r);
}

QoreValue DateOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // these functions (date addition and subtraction) can have no side-effects
   if (!ref_rv)
      return QoreValue();

   DateTimeNodeValueHelper l(left);
   DateTimeNodeValueHelper r(right);

   return op_func(*l, *r);
}

QoreValue BoolIntOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // these functions can have no side effects
   if (!ref_rv)
      return QoreValue();

   return op_func(left->getAsBigInt(), right->getAsBigInt());
}

QoreValue IntIntOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // these functions can have no side effects
   if (!ref_rv)
      return QoreValue();

   return op_func(left->getAsBigInt(), right->getAsBigInt());
}

QoreValue DivideIntOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // these functions can have no side effects
   if (!ref_rv)
      return QoreValue();

   return op_func(left->getAsBigInt(), right->getAsBigInt(), xsink);
}

QoreValue BoolFloatOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // these functions can have no side effects
   if (!ref_rv)
      return QoreValue();

   return op_func(left->getAsFloat(), right->getAsFloat());
}

QoreValue FloatFloatOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // these functions can have no side effects
   if (!ref_rv)
      return QoreValue();

   return op_func(left->getAsFloat(), right->getAsFloat());
}

QoreValue DivideFloatOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // these functions can have no side effects
   if (!ref_rv)
      return QoreValue();

   return op_func(left->getAsFloat(), right->getAsFloat(), xsink);
}

QoreValue CompareFloatOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // these functions can have no side effects
   if (!ref_rv)
      return QoreValue();

   return op_func(left->getAsFloat(), right->getAsFloat());
}

QoreValue IntegerNotOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // these functions can have no side effects
   return ~left->getAsBigInt();
}

QoreValue CompareDateOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // this operator can have no side effects
   if (!ref_rv)
      return QoreValue();

   DateTimeValueHelper l(left);   
   DateTimeValueHelper r(right);   

   return (int64)DateTime::compareDates(*l, *r);
}

QoreValue LogicOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // these functions can have no side effects
   if (!ref_rv)
      return QoreValue();

   return op_func(left->getAsBool(), right->getAsBool());
}

QoreValue BoolStrRegexOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   assert(right && right->getType() == NT_REGEX);

   // conditionally evaluate left-hand node only
   QoreNodeEvalOptionalRefHolder le(left, xsink);
   if (*xsink) return QoreValue();

   // return immediately if the return value is ignored, this statement will have no effect and there can be no (other) side-effects
   if (!ref_rv) return QoreValue();

   QoreStringValueHelper l(*le);
   return op_func(*l, reinterpret_cast<const QoreRegexNode*>(right), xsink);
}

QoreValue BigIntStrStrOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   QoreStringValueHelper l(left);
   
   if (args == 1)
      return op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return op_func(*l, *r, xsink);
}

QoreValue BigIntOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL)) {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1)
      return op_func(left, right, xsink);

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL)) {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return op_func(left, right, xsink);
}

QoreValue FloatOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL)) {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1)
      return op_func(left, right, xsink);

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL)) {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return op_func(left, right, xsink);
}

QoreValue NumberOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL)) {
      l = get_node_type(left, ltype);
      left = *l;
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL)) {
      r = get_node_type(right, rtype);
      right = *r;
   }

   SimpleRefHolder<QoreNumberNode> rv(op_func(reinterpret_cast<const QoreNumberNode*>(left), reinterpret_cast<const QoreNumberNode*>(right), xsink));
   assert(!(*xsink && rv));
   if (!ref_rv || *xsink)
      return QoreValue();
   return rv.release();
}

QoreValue BoolNumberOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL)) {
      l = get_node_type(left, ltype);
      left = *l;
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL)) {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return op_func(reinterpret_cast<const QoreNumberNode*>(left), reinterpret_cast<const QoreNumberNode*>(right));
}

QoreValue IntNumberOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL)) {
      l = get_node_type(left, ltype);
      left = *l;
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL)) {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return op_func(reinterpret_cast<const QoreNumberNode*>(left), reinterpret_cast<const QoreNumberNode*>(right));
}

Operator::~Operator() {
   // erase all functions
   for (unsigned i = 0, size = functions.size(); i < size; i++)
      delete functions[i];
   if (opMatrix)
      delete [] opMatrix;
}

void Operator::init() {
   if (!evalArgs || (functions.size() == 1))
      return;
   opMatrix = new int[NUM_VALUE_TYPES][NUM_VALUE_TYPES];
   // create function lookup matrix for value types
   for (qore_type_t i = 0; i < NUM_VALUE_TYPES; i++)
      for (qore_type_t j = 0; j < NUM_VALUE_TYPES; j++)
	 opMatrix[i][j] = findFunction(i, j);
}

AbstractQoreNode* Operator::parseInit(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& resultTypeInfo) {
   // check for illegal changes to local variables in background expressions
   if (pflag & PF_BACKGROUND && lvalue) {
      if (tree->left && tree->left->getType() == NT_VARREF && reinterpret_cast<VarRefNode*>(tree->left)->getType() == VT_LOCAL)
	 parse_error("illegal local variable modification in background expression");
   }

   if (!check_args)
      return tree->defaultParseInit(oflag, pflag, lvids, resultTypeInfo);

   return check_args(tree, oflag, pflag, lvids, resultTypeInfo, name, description);
}

// if there is no exact match, the first partial match counts as a match
// static method
int Operator::match(qore_type_t ntype, qore_type_t rtype) {
   // if any type is OK, or an exact match
   if (rtype == NT_ALL || ntype == rtype || (rtype == NT_VARREF && ntype == NT_SELF_VARREF))
      return 1;
   else // otherwise fail
      return 0;
}

int Operator::get_function(const QoreNodeEvalOptionalRefHolder &nleft, ExceptionSink* xsink) const {
   int t;
   // find operator function
   if (functions.size() == 1)
      t = 0;
   else if (nleft->getType() < NUM_VALUE_TYPES)
      t = opMatrix[nleft->getType()][NT_NOTHING];
   else
      t = findFunction(nleft->getType(), NT_NOTHING);

   printd(5, "Operator::get_function() found function %d\n", t);
   return t;
}

int Operator::get_function(const QoreNodeEvalOptionalRefHolder &nleft, const QoreNodeEvalOptionalRefHolder &nright, ExceptionSink* xsink) const {
   int t;
   // find operator function
   if (functions.size() == 1)
      t = 0;
   else if (nleft->getType() < NUM_VALUE_TYPES && nright->getType() < NUM_VALUE_TYPES)
      t = opMatrix[nleft->getType()][nright->getType()];
   else
      t = findFunction(nleft->getType(), nright->getType());

   printd(5, "Operator::get_function() found function %d\n", t);
   return t;
}

// Operator::eval(): return value requires a deref(xsink) afterwards
// there are 3 main cases which have been split into 3 sections as a speed optimization
// 1: evalArgs 1 argument
// 2: evalArgs 2 arguments
// 3: pass-through all arguments
QoreValue Operator::eval(const AbstractQoreNode* left_side, const AbstractQoreNode* right_side, bool ref_rv, ExceptionSink* xsink) const {
   printd(5, "evaluating operator %s (%p %p)\n", description, left_side, right_side);
   if (evalArgs) {
      QoreNodeEvalOptionalRefHolder nleft(left_side, xsink);
      if (*xsink)
	 return QoreValue();
      if (!nleft)
	 nleft.assign(false, &Nothing);

      int t;

      if (args == 1) {
	 if ((t = get_function(nleft, xsink)) == -1)
	    return QoreValue();

	 return functions[t]->eval(*nleft, 0, ref_rv, 1, xsink);
      }

      QoreNodeEvalOptionalRefHolder nright(right_side, xsink);
      if (*xsink)
	 return QoreValue();
      if (!nright)
	 nright.assign(false, &Nothing);

      // find operator function
      if ((t = get_function(nleft, nright, xsink)) == -1)
	 return QoreValue();

      return functions[t]->eval(*nleft, *nright, ref_rv, 2, xsink);
   }

   // in this case there will only be one entry (0)
   printd(5, "Operator::eval() evaluating function 0\n");
   return functions[0]->eval(left_side, right_side, ref_rv, args, xsink);
}

int Operator::findFunction(qore_type_t ltype, qore_type_t rtype) const {
   int m = -1;

   //QORE_TRACE("Operator::findFunction()");
   // loop through all operator functions

   for (int i = 0, size = functions.size(); i < size; i++) {
      AbstractOperatorFunction *f = functions[i];

      // only check for default if it's not the first function
      // and is the last (meaning there is more than 1)
      if (i && i == (size - 1) && f->ltype == NT_ALL && f->rtype == NT_ALL) {
	 // only return this entry if no partial match has already been made
	 return m == -1 ? i : m;
      }

      // check for a match on the left side
      if (match(ltype, f->ltype)) {
	 /* if there is only one operator or there is also
	 * a match on the right side, return */
	 if ((args == 1) ||
	     ((args == 2) && match(rtype, f->rtype))) {
	    return i;
	 }
	 if (!f->needsExactMatch() && m == -1)
	    m = i;
	 continue;
      }
      if ((args == 2) && !f->needsExactMatch() && match(rtype, f->rtype)
	  && (m == -1))
	 m = i;
   }
   /* if there is no match of any kind, take the highest priority function
      * (row 0), and try to convert the arguments, otherwise return the best
      * partial match
      */

   return m == -1 ? 0 : m;
}

void Operator::addFunction(qore_type_t lt, qore_type_t rt, op_func_t f) {
   functions.push_back(new OperatorFunction(lt, rt, f));
}

void Operator::addFunction(qore_type_t lt, qore_type_t rt, op_bool_func_t f) {
   functions.push_back(new BoolOperatorFunction(lt, rt, f));
}

void Operator::addFunction(qore_type_t lt, qore_type_t rt, op_bigint_func_t f) {
   functions.push_back(new BigIntOperatorFunction(lt, rt, f));
}

void Operator::addFunction(qore_type_t lt, qore_type_t rt, op_float_func_t f) {
   functions.push_back(new FloatOperatorFunction(lt, rt, f));
}

OperatorList::OperatorList() {
}

OperatorList::~OperatorList() {
   oplist_t::iterator i;
   while ((i = begin()) != end()) {
      delete (*i);
      erase(i);
   }
}

Operator *OperatorList::add(Operator *o) {
   push_back(o);
   return o;
}

// checks for illegal $self assignments in an object context
void check_self_assignment(AbstractQoreNode* n, LocalVar* selfid) {
   // if it's a variable reference
   qore_type_t ntype = n->getType();
   if (ntype == NT_VARREF) {
      VarRefNode* v = reinterpret_cast<VarRefNode*>(n);
      if (v->getType() == VT_LOCAL && v->ref.id == selfid)
         parse_error("illegal assignment to 'self' in an object context");
      return;
   }

   if (ntype != NT_TREE)
      return;

   QoreTreeNode* tree = reinterpret_cast<QoreTreeNode*>(n);

   // otherwise it's a tree: go to root expression
   while (tree->left->getType() == NT_TREE) {
      n = tree->left;
      tree = reinterpret_cast<QoreTreeNode*>(n);
   }

   if (tree->left->getType() != NT_VARREF)
      return;

   VarRefNode* v = reinterpret_cast<VarRefNode*>(tree->left);

   // left must be variable reference, check if the tree is
   // a list reference; if so, it's invalid
   if (v->getType() == VT_LOCAL && v->ref.id == selfid  && tree->getOp() == OP_LIST_REF)
      parse_error("illegal conversion of 'self' to a list");
}

static AbstractQoreNode* check_op_list_assignment(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& resultTypeInfo, const char* name, const char* desc) {
   assert(tree->left && tree->left->getType() == NT_LIST);
   QoreListNode* l = reinterpret_cast<QoreListNode*>(tree->left);

   QoreListNodeParseInitHelper li(l, oflag, pflag | PF_FOR_ASSIGNMENT, lvids);
   QorePossibleListNodeParseInitHelper ri(&tree->right, oflag, pflag, lvids);

   const QoreTypeInfo *argInfo = 0;
   while (li.next()) {
      ri.next();

      const QoreTypeInfo *prototypeInfo = 0;
      AbstractQoreNode* v = li.parseInit(prototypeInfo);

      if (v && check_lvalue(v))
	 parse_error("expecing lvalue in position %d of left-hand-side list in list assignment, got '%s' instead", li.index() + 1, v->getTypeName());

      // check for illegal assignment to $self
      if (oflag)
	 check_self_assignment(v, oflag);

      ri.parseInit(argInfo);

      if (prototypeInfo->hasType()) {
	 if (!prototypeInfo->parseAccepts(argInfo)) {
	    // raise an exception only if parse exceptions are not disabled
	    if (getProgram()->getParseExceptionSink()) {
	       QoreStringNode* edesc = new QoreStringNode("lvalue for assignment operator in position ");
	       edesc->sprintf("%d of list assignment expects ", li.index() + 1);
	       prototypeInfo->getThisType(*edesc);
	       edesc->concat(", but right-hand side is ");
	       argInfo->getThisType(*edesc);
	       qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", edesc);
	    }
	 }
      }
   }

   while (ri.next())
      ri.parseInit(argInfo);

   return tree;
}

// for logical operators that always return a boolean
static AbstractQoreNode* check_op_logical(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* desc) {
   returnTypeInfo = boolTypeInfo;
   return tree->defaultParseInit(oflag, pflag, lvids, returnTypeInfo);
}

// for operators that always return an integer
static AbstractQoreNode* check_op_returns_integer(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* desc) {
   returnTypeInfo = bigIntTypeInfo;
   return tree->defaultParseInit(oflag, pflag, lvids, returnTypeInfo);
}

int check_lvalue_int(const QoreTypeInfo*& typeInfo, const char* name) {
   // make sure the lvalue can be assigned an integer value
   // raise a parse exception only if parse exceptions are not suppressed
   if (!typeInfo->parseAcceptsReturns(NT_INT)) {
      if (getProgram()->getParseExceptionSink()) {
	 QoreStringNode* desc = new QoreStringNode("lvalue has type ");
	 typeInfo->getThisType(*desc);
	 desc->sprintf(", but the %s will assign it an integer value", name);
	 qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", desc);
      }
      return -1;
   }
   return 0;
}

int check_lvalue_float(const QoreTypeInfo*& typeInfo, const char* name) {
   // make sure the lvalue can be assigned a floating-point value
   // raise a parse exception only if parse exceptions are not suppressed
   if (!typeInfo->parseAcceptsReturns(NT_FLOAT) && getProgram()->getParseExceptionSink()) {
      QoreStringNode* desc = new QoreStringNode("lvalue has type ");
      typeInfo->getThisType(*desc);
      desc->sprintf(", but the %s will assign it a float value", name);
      qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", desc);
      return -1;
   }
   return 0;
}

int check_lvalue_int_float_number(const QoreTypeInfo*& typeInfo, const char* name) {
   // make sure the lvalue can be assigned an integer value
   // raise a parse exception only if parse exceptions are not suppressed
   if (!typeInfo->parseAcceptsReturns(NT_INT)
         && !typeInfo->parseAcceptsReturns(NT_FLOAT)
         && !typeInfo->parseAcceptsReturns(NT_NUMBER)) {
      if (getProgram()->getParseExceptionSink()) {
         QoreStringNode* desc = new QoreStringNode("lvalue has type ");
         typeInfo->getThisType(*desc);
         desc->sprintf(", but the %s only works with integer, floating-point, or numeric lvalues", name);
         qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", desc);
      }
      return -1;
   }
   if (typeInfo->parseReturnsType(NT_INT)) {
      if (typeInfo->parseReturnsType(NT_FLOAT)) {
         if (typeInfo->parseReturnsType(NT_NUMBER))
            typeInfo = bigIntFloatOrNumberTypeInfo;
         else
            typeInfo = bigIntOrFloatTypeInfo;
      }
      else
         typeInfo = bigIntTypeInfo;
   }
   else {
      if (typeInfo->parseReturnsType(NT_FLOAT))
         if (typeInfo->parseReturnsType(NT_NUMBER))
            typeInfo = floatOrNumberTypeInfo;
         else
            typeInfo = floatTypeInfo;
      else
         typeInfo = numberTypeInfo;
   }

   return 0;
}

int check_lvalue_number(const QoreTypeInfo*& typeInfo, const char* name) {
   // make sure the lvalue can be assigned a floating-point value
   // raise a parse exception only if parse exceptions are not suppressed
   if (!typeInfo->parseAcceptsReturns(NT_NUMBER) && getProgram()->getParseExceptionSink()) {
      QoreStringNode* desc = new QoreStringNode("lvalue has type ");
      typeInfo->getThisType(*desc);
      desc->sprintf(", but the %s will assign it a number value", name);
      qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", desc);
      return -1;
   }
   return 0;
}

// set the return value for op_minus (-)
static AbstractQoreNode* check_op_minus(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* desc) {
   const QoreTypeInfo *leftTypeInfo = 0;
   tree->leftParseInit(oflag, pflag, lvids, leftTypeInfo);

   const QoreTypeInfo *rightTypeInfo = 0;
   tree->rightParseInit(oflag, pflag, lvids, rightTypeInfo);

   if (tree->constArgs())
      return tree->evalSubst(returnTypeInfo);

   // if either side is a date, then the return type is date (highest priority)
   if (leftTypeInfo->isType(NT_DATE)
       || rightTypeInfo->isType(NT_DATE))
      returnTypeInfo = dateTypeInfo;
   // otherwise we have to make sure types are known on both sides of the expression
   else if (leftTypeInfo->hasType() && rightTypeInfo->hasType()) {
      if (leftTypeInfo->isType(NT_FLOAT)
	  || rightTypeInfo->isType(NT_FLOAT))
	 returnTypeInfo = floatTypeInfo;
      else if (leftTypeInfo->isType(NT_INT)
	       || rightTypeInfo->isType(NT_INT))
	 returnTypeInfo = bigIntTypeInfo;
      else if ((leftTypeInfo->isType(NT_HASH)
		|| leftTypeInfo->isType(NT_OBJECT))
	       && (rightTypeInfo->isType(NT_STRING)
		   || rightTypeInfo->isType(NT_LIST)))
	 returnTypeInfo = hashTypeInfo;
      else if (leftTypeInfo->returnsSingle() && rightTypeInfo->returnsSingle())
	 // only return type nothing if both types are available and return a single type
	 returnTypeInfo = nothingTypeInfo;
   }
   else
      returnTypeInfo = 0;

   return tree;
}

// set the return value for op_plus (+)
static AbstractQoreNode* check_op_plus(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* desc) {
   const QoreTypeInfo *leftTypeInfo = 0;
   tree->leftParseInit(oflag, pflag, lvids, leftTypeInfo);

   const QoreTypeInfo *rightTypeInfo = 0;
   tree->rightParseInit(oflag, pflag, lvids, rightTypeInfo);

   if (tree->constArgs())
      return tree->evalSubst(returnTypeInfo);

   // if either side is a list, then the return type is list (highest priority)
   if (leftTypeInfo->isType(NT_LIST)
       || rightTypeInfo->isType(NT_LIST))
	 returnTypeInfo = listTypeInfo;

   // otherwise only set return type if return types on both sides are known at parse time
   else if (leftTypeInfo->hasType() && rightTypeInfo->hasType()) {

      if (leftTypeInfo->isType(NT_STRING)
	       || rightTypeInfo->isType(NT_STRING))
	 returnTypeInfo = stringTypeInfo;

      else if (leftTypeInfo->isType(NT_DATE)
	       || rightTypeInfo->isType(NT_DATE))
	 returnTypeInfo = dateTypeInfo;

      else if (leftTypeInfo->isType(NT_NUMBER)
               || rightTypeInfo->isType(NT_NUMBER))
         returnTypeInfo = numberTypeInfo;

      else if (leftTypeInfo->isType(NT_FLOAT)
	       || rightTypeInfo->isType(NT_FLOAT))
	 returnTypeInfo = floatTypeInfo;

      else if (leftTypeInfo->isType(NT_INT)
	       || rightTypeInfo->isType(NT_INT))
	 returnTypeInfo = bigIntTypeInfo;

      else if (leftTypeInfo->isType(NT_HASH)
	       || leftTypeInfo->isType(NT_OBJECT))
	 returnTypeInfo = hashTypeInfo;

      else if (rightTypeInfo->isType(NT_OBJECT))
	 returnTypeInfo = objectTypeInfo;

      else if (leftTypeInfo->isType(NT_BINARY)
	       || rightTypeInfo->isType(NT_BINARY))
	 returnTypeInfo = binaryTypeInfo;

      else if (leftTypeInfo->returnsSingle() && rightTypeInfo->returnsSingle())
	 // only return type nothing if both types are available and return a single type
	 returnTypeInfo = nothingTypeInfo;
   }
   else
      returnTypeInfo = 0;

   return tree;
}

// set the return value for op_multiply (*) - also used for op_divide
static AbstractQoreNode* check_op_multiply(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* desc) {
   const QoreTypeInfo *leftTypeInfo = 0;
   tree->leftParseInit(oflag, pflag, lvids, leftTypeInfo);

   const QoreTypeInfo *rightTypeInfo = 0;
   tree->rightParseInit(oflag, pflag, lvids, rightTypeInfo);

   if (tree->constArgs())
      return tree->evalSubst(returnTypeInfo);

   // if either side is a float, then the return type is float (highest priority)
   if (leftTypeInfo->isType(NT_FLOAT) || rightTypeInfo->isType(NT_FLOAT))
      returnTypeInfo = floatTypeInfo;

   // otherwise only set return type if return types on both sides are known at parse time
   else if (leftTypeInfo->hasType() && rightTypeInfo->hasType()) {
      if (leftTypeInfo->isType(NT_INT) && rightTypeInfo->isType(NT_INT))
	 returnTypeInfo = bigIntTypeInfo;
   }
   else
      returnTypeInfo = 0;

   //printd(5, "check_op_multiply() %s %s = %s\n", leftTypeInfo->getName(), rightTypeInfo->getName(), returnTypeInfo->getName());

   return tree;
}

static AbstractQoreNode* check_op_list_ref(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* desc) {
   const QoreTypeInfo *leftTypeInfo = 0;
   tree->leftParseInit(oflag, pflag, lvids, leftTypeInfo);

   const QoreTypeInfo *rightTypeInfo = 0;
   tree->rightParseInit(oflag, pflag, lvids, rightTypeInfo);

   if (tree->constArgs())
      return tree->evalSubst(returnTypeInfo);

   if (leftTypeInfo->hasType()) {
      // if we are trying to convert to a list
      if (pflag & PF_FOR_ASSIGNMENT) {
	 // only throw a parse exception if parse exceptions are enabled
	 if (!leftTypeInfo->parseAcceptsReturns(NT_LIST) && getProgram()->getParseExceptionSink()) {
	    QoreStringNode* edesc = new QoreStringNode("cannot convert lvalue defined as ");
	    leftTypeInfo->getThisType(*edesc);
	    edesc->sprintf(" to a list using the '[]' operator in an assignment expression");
	    qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", edesc);
	 }
      }
      else if (!listTypeInfo->parseAccepts(leftTypeInfo)
	  && !stringTypeInfo->parseAccepts(leftTypeInfo)
	  && !binaryTypeInfo->parseAccepts(leftTypeInfo)) {
	 QoreStringNode* edesc = new QoreStringNode("left-hand side of the expression with the '[]' operator is ");
	 leftTypeInfo->getThisType(*edesc);
	 edesc->concat(" and so this expression will always return NOTHING; the '[]' operator only returns a value within the legal bounds of lists, strings, and binary objects");
	 qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
	 returnTypeInfo = nothingTypeInfo;
      }
   }

   return tree;
}

static AbstractQoreNode* check_op_object_ref(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* desc) {
   QoreProgramLocation loc = get_parse_location();

   const QoreTypeInfo *leftTypeInfo = 0;
   tree->leftParseInit(oflag, pflag, lvids, leftTypeInfo);

   const QoreTypeInfo *rightTypeInfo = 0;
   tree->rightParseInit(oflag, pflag, lvids, rightTypeInfo);

   printd(5, "check_op_object_object_ref() l=%p %s (%s) r=%p %s\n", leftTypeInfo, leftTypeInfo->getName(), leftTypeInfo->getUniqueReturnClass() ? leftTypeInfo->getUniqueReturnClass()->getName() : "n/a", rightTypeInfo, rightTypeInfo->getName());

   if (leftTypeInfo->hasType()) {
      bool can_be_obj = objectTypeInfo->parseAccepts(leftTypeInfo);
      bool can_be_hash = hashTypeInfo->parseAccepts(leftTypeInfo);

      bool is_obj = can_be_obj ? leftTypeInfo->isType(NT_OBJECT) : false;
      bool is_hash = can_be_hash ? leftTypeInfo->isType(NT_HASH) : false;

      const QoreClass *qc = leftTypeInfo->getUniqueReturnClass();
      // see if we can check for legal access
      if (qc && tree->right) {
	 qore_type_t rt = tree->right->getType();
	 if (rt == NT_STRING) {
	    const char* member = reinterpret_cast<const QoreStringNode*>(tree->right)->getBuffer();
	    qore_class_private::parseCheckMemberAccess(*qc, loc, member, returnTypeInfo, pflag);
	 }
	 else if (rt == NT_LIST) { // check object slices as well if strings are available
	    ConstListIterator li(reinterpret_cast<const QoreListNode*>(tree->right));
	    while (li.next()) {
	       if (li.getValue() && li.getValue()->getType() == NT_STRING) {
		  const char* member = reinterpret_cast<const QoreStringNode*>(li.getValue())->getBuffer();
		  qore_class_private::parseCheckMemberAccess(*qc, loc, member, returnTypeInfo, pflag);
	       }
	    }
	 }
      }

      // if we are taking a slice of an object or a hash, then the return type is a hash
      if (rightTypeInfo->hasType() && rightTypeInfo->isType(NT_LIST) && (is_obj || is_hash))
	 returnTypeInfo = hashTypeInfo;

      // if we are trying to convert to a hash
      if (pflag & PF_FOR_ASSIGNMENT) {
	 // only throw a parse exception if parse exceptions are enabled
	 if (!can_be_hash
	     && !can_be_obj
	     && getProgram()->getParseExceptionSink()) {
	    QoreStringNode* edesc = new QoreStringNode("cannot convert lvalue defined as ");
	    leftTypeInfo->getThisType(*edesc);
	    edesc->sprintf(" to a hash using the '.' or '{}' operator in an assignment expression");
	    qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", edesc);
	 }
      }
      else if (!can_be_hash && !can_be_obj) {
	 QoreStringNode* edesc = new QoreStringNode("left-hand side of the expression with the '.' or '{}' operator is ");
	 leftTypeInfo->getThisType(*edesc);
	 edesc->concat(" and so this expression will always return NOTHING; the '.' or '{}' operator only returns a value with hashes and objects");
	 qore_program_private::makeParseWarning(getProgram(), loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
	 returnTypeInfo = nothingTypeInfo;
      }
   }

   //printd(5, "check_op_object_ref() rightTypeInfo=%s rightTypeInfo->nonStringValue(): %d !listTypeInfo->parseAccepts(rightTypeInfo): %d\n", rightTypeInfo->getName(), rightTypeInfo->nonStringValue(), !listTypeInfo->parseAccepts(rightTypeInfo));

   // issue a warning if the right side of the expression cannot be converted to a string
   // and can not be a list (for a slice)
   if (rightTypeInfo->nonStringValue() && !listTypeInfo->parseAccepts(rightTypeInfo))
      rightTypeInfo->doNonStringWarning(loc, "the right side of the expression with the '.' or '{}' operator is ");

   return tree;
}

// for operators that always return an integer
static AbstractQoreNode* check_op_elements(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* desc) {
   returnTypeInfo = bigIntTypeInfo;

   const QoreTypeInfo *leftTypeInfo = 0;
   tree->leftParseInit(oflag, pflag, lvids, leftTypeInfo);

   assert(!tree->right);

   if (leftTypeInfo->hasType()
         && !listTypeInfo->parseAccepts(leftTypeInfo)
         && !hashTypeInfo->parseAccepts(leftTypeInfo)
         && !stringTypeInfo->parseAccepts(leftTypeInfo)
         && !binaryTypeInfo->parseAccepts(leftTypeInfo)
         && !objectTypeInfo->parseAccepts(leftTypeInfo)) {
         QoreStringNode* edesc = new QoreStringNode("the argument given to the 'elements' operator is ");
         leftTypeInfo->getThisType(*edesc);
         edesc->concat(", so this expression will always return 0; the 'elements' operator can only return a value with lists, hashes, strings, binary objects, and objects");
         qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
   }

   if (tree->constArgs())
      return tree->evalSubst(returnTypeInfo);

   return tree;
}

static AbstractQoreNode* check_op_keys(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* desc) {
   const QoreTypeInfo *leftTypeInfo = 0;
   tree->leftParseInit(oflag, pflag, lvids, leftTypeInfo);

   assert(!tree->right);

   if (leftTypeInfo->hasType()) {
      if (leftTypeInfo->isType(NT_HASH) || leftTypeInfo->isType(NT_OBJECT))
	 returnTypeInfo = listTypeInfo;
      else if (!hashTypeInfo->parseAccepts(leftTypeInfo)
	       && !objectTypeInfo->parseAccepts(leftTypeInfo)) {
	 QoreStringNode* edesc = new QoreStringNode("the expression with the 'keys' operator is ");
	 leftTypeInfo->getThisType(*edesc);
	 edesc->concat(" and so this expression will always return NOTHING; the 'keys' operator can only return a value with hashes and objects");
	 qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
	 returnTypeInfo = nothingTypeInfo;
      }
      else
	 returnTypeInfo = listOrNothingTypeInfo;
   }

   if (tree->constArgs())
      return tree->evalSubst(returnTypeInfo);

   return tree;
}

// issues a warning
static AbstractQoreNode* check_op_list_op(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* desc) {
   const QoreTypeInfo *leftTypeInfo = 0;
   tree->leftParseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, leftTypeInfo);

   const QoreTypeInfo *rightTypeInfo = 0;
   tree->rightParseInit(oflag, pflag, lvids, rightTypeInfo);

   if (!leftTypeInfo->parseAcceptsReturns(NT_LIST)) {
      QoreStringNode* edesc = new QoreStringNode("the lvalue expression with the ");
      edesc->sprintf("'%s' operator is ", name);
      leftTypeInfo->getThisType(*edesc);
      edesc->sprintf(" therefore this operation will have no effect on the lvalue and will always return NOTHING; the '%s' operator can only operate on lists", name);
      qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
      returnTypeInfo = nothingTypeInfo;
   }

   return tree;
}

// throws a parse exception
static AbstractQoreNode* check_op_unshift(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* desc) {
   const QoreTypeInfo *leftTypeInfo = 0;
   tree->leftParseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, leftTypeInfo);

   const QoreTypeInfo *rightTypeInfo = 0;
   tree->rightParseInit(oflag, pflag, lvids, rightTypeInfo);

   if (!leftTypeInfo->parseAcceptsReturns(NT_LIST)) {
      // only raise a parse exception if parse exceptions are enabled
      if (getProgram()->getParseExceptionSink()) {
	 QoreStringNode* edesc = new QoreStringNode("the lvalue expression with the ");
	 edesc->sprintf("'%s' operator is ", name);
	 leftTypeInfo->getThisType(*edesc);
	 edesc->sprintf(" therefore this operation is invalid and would throw an exception at run-time; the '%s' operator can only operate on lists", name);
	 qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", edesc);
      }
   }
   else
      returnTypeInfo = listTypeInfo;

   return tree;
}

static AbstractQoreNode* check_op_lvalue_string(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* descr) {
   const QoreTypeInfo *leftTypeInfo = 0;
   tree->leftParseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, leftTypeInfo);

   const QoreTypeInfo *rightTypeInfo = 0;
   tree->rightParseInit(oflag, pflag, lvids, rightTypeInfo);

   if (!leftTypeInfo->parseAcceptsReturns(NT_STRING)) {
      QoreStringNode* desc = new QoreStringNode("the lvalue expression with the ");
      desc->sprintf("%s operator is ", descr);
      leftTypeInfo->getThisType(*desc);
      desc->sprintf(", therefore this operation will have no effect on the lvalue and will always return NOTHING; this operator only works on strings");
      qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
      returnTypeInfo = nothingTypeInfo;
   }
   else
      returnTypeInfo = stringTypeInfo;

   return tree;
}

static AbstractQoreNode* check_op_chomp_trim(QoreTreeNode* tree, LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& returnTypeInfo, const char* name, const char* descr) {
   const QoreTypeInfo *leftTypeInfo = 0;
   tree->leftParseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, leftTypeInfo);

   assert(!tree->right);

   if (leftTypeInfo->hasType()
       && !leftTypeInfo->parseAcceptsReturns(NT_STRING)
       && !leftTypeInfo->parseAcceptsReturns(NT_LIST)
       && !leftTypeInfo->parseAcceptsReturns(NT_HASH)) {
      QoreStringNode* desc = new QoreStringNode("the lvalue expression with the ");
      desc->sprintf("%s operator is ", name);
      leftTypeInfo->getThisType(*desc);
      desc->sprintf(", therefore this operation will have no effect on the lvalue and will always return NOTHING; this operator only works on strings, lists, and hashes");
      qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
      returnTypeInfo = nothingTypeInfo;
   }
   else
      returnTypeInfo = bigIntTypeInfo;

   return tree;
}

// registers the system operators and system operator functions
void OperatorList::init() {
   QORE_TRACE("OperatorList::init()");

   OP_LOG_AND = add(new Operator(2, "&&", "logical-and", 0, false, false, check_op_logical));
   OP_LOG_AND->addEffectFunction(op_log_and);

   OP_LOG_OR = add(new Operator(2, "||", "logical-or", 0, false, false, check_op_logical));
   OP_LOG_OR->addEffectFunction(op_log_or);

   OP_LOG_LT = add(new Operator(2, "<", "less-than", 1, false, false, check_op_logical));
   OP_LOG_LT->addFunction(op_log_lt_number);
   OP_LOG_LT->addFunction(op_log_lt_float);
   OP_LOG_LT->addFunction(op_log_lt_bigint);
   OP_LOG_LT->addFunction(op_log_lt_string);
   OP_LOG_LT->addFunction(op_log_lt_date);

   OP_LOG_GT = add(new Operator(2, ">", "greater-than", 1, false, false, check_op_logical));
   OP_LOG_LT->addFunction(op_log_gt_number);
   OP_LOG_GT->addFunction(op_log_gt_float);
   OP_LOG_GT->addFunction(op_log_gt_bigint);
   OP_LOG_GT->addFunction(op_log_gt_string);
   OP_LOG_GT->addFunction(op_log_gt_date);

   OP_LOG_EQ = add(new Operator(2, "==", "logical-equals", 1, false, false, check_op_logical));
   OP_LOG_EQ->addFunction(op_log_eq_string);
   OP_LOG_LT->addFunction(op_log_eq_number);
   OP_LOG_EQ->addFunction(op_log_eq_float);
   OP_LOG_EQ->addFunction(op_log_eq_bigint);
   OP_LOG_EQ->addFunction(op_log_eq_boolean);
   OP_LOG_EQ->addFunction(op_log_eq_date);
   OP_LOG_EQ->addNoConvertFunction(NT_ALL, NT_ALL, op_log_eq_all);

   OP_LOG_NE = add(new Operator(2, "!=", "not-equals", 1, false, false, check_op_logical));
   OP_LOG_NE->addFunction(op_log_ne_string);
   OP_LOG_LT->addFunction(op_log_ne_number);
   OP_LOG_NE->addFunction(op_log_ne_float);
   OP_LOG_NE->addFunction(op_log_ne_bigint);
   OP_LOG_NE->addFunction(op_log_ne_boolean);
   OP_LOG_NE->addFunction(op_log_ne_date);
   OP_LOG_NE->addNoConvertFunction(NT_ALL, NT_ALL, op_log_ne_all);

   OP_LOG_LE = add(new Operator(2, "<=", "less-than-or-equals", 1, false, false, check_op_logical));
   OP_LOG_LT->addFunction(op_log_le_number);
   OP_LOG_LE->addFunction(op_log_le_float);
   OP_LOG_LE->addFunction(op_log_le_bigint);
   OP_LOG_LE->addFunction(op_log_le_string);
   OP_LOG_LE->addFunction(op_log_le_date);

   OP_LOG_GE = add(new Operator(2, ">=", "greater-than-or-equals", 1, false, false, check_op_logical));
   OP_LOG_LT->addFunction(op_log_ge_number);
   OP_LOG_GE->addFunction(op_log_ge_float);
   OP_LOG_GE->addFunction(op_log_ge_bigint);
   OP_LOG_GE->addFunction(op_log_ge_string);
   OP_LOG_GE->addFunction(op_log_ge_date);

   OP_ABSOLUTE_EQ = add(new Operator(2, "===", "absolute logical-equals", 0, false, false, check_op_logical));
   OP_ABSOLUTE_EQ->addFunction(NT_ALL, NT_ALL, op_absolute_log_eq);

   OP_ABSOLUTE_NE = add(new Operator(2, "!==", "absolute logical-not-equals", 0, false, false, check_op_logical));
   OP_ABSOLUTE_NE->addFunction(NT_ALL, NT_ALL, op_absolute_log_neq);

   OP_REGEX_MATCH = add(new Operator(2, "=~", "regular expression match", 0, false, false, check_op_logical));
   OP_REGEX_MATCH->addFunction(op_regex_match);

   OP_REGEX_NMATCH = add(new Operator(2, "!~", "regular expression negative match", 0, false, false, check_op_logical));
   OP_REGEX_NMATCH->addFunction(op_regex_nmatch);

   OP_EXISTS = add(new Operator(1, "exists", "exists", 1, false, false, check_op_logical));
   OP_EXISTS->addFunction(NT_ALL, NT_NONE, op_exists);

   OP_INSTANCEOF = add(new Operator(2, "instanceof", "instanceof", 0, false, false, check_op_logical));
   OP_INSTANCEOF->addFunction(NT_ALL, NT_CLASSREF, op_instanceof);

   // bigint operators
   OP_LOG_CMP = add(new Operator(2, "<=>", "logical-comparison", 1, false, false, check_op_returns_integer));
   OP_LOG_CMP->addFunction(op_cmp_string);
   OP_LOG_CMP->addFunction(op_cmp_number);
   OP_LOG_CMP->addFunction(op_cmp_double);
   OP_LOG_CMP->addFunction(op_cmp_bigint);
   OP_LOG_CMP->addCompareDateFunction();

   OP_ELEMENTS = add(new Operator(1, "elements", "number of elements", 0, false, false, check_op_elements));
   OP_ELEMENTS->addFunction(NT_ALL, NT_NONE, op_elements);

   // non-boolean operators
   OP_LIST_ASSIGNMENT = add(new Operator(2, "(list) =", "list assignment", 0, true, true, check_op_list_assignment));
   OP_LIST_ASSIGNMENT->addFunction(NT_ALL, NT_ALL, op_list_assignment);

   OP_BIN_AND = add(new Operator(2, "&", "binary-and", 1, false, false, check_op_returns_integer));
   OP_BIN_AND->addFunction(op_bin_and_int);

   OP_BIN_OR = add(new Operator(2, "|", "binary-or", 1, false, false, check_op_returns_integer));
   OP_BIN_OR->addFunction(op_bin_or_int);

   OP_BIN_NOT = add(new Operator(1, "~", "binary-not", 1, false, false, check_op_returns_integer));
   OP_BIN_NOT->addIntegerNotFunction();

   OP_BIN_XOR = add(new Operator(2, "^", "binary-xor", 1, false, false, check_op_returns_integer));
   OP_BIN_XOR->addFunction(op_bin_xor_int);

   OP_MINUS = add(new Operator(2, "-", "minus", 1, false, false, check_op_minus));
   OP_MINUS->addFunction(op_minus_date);
   OP_MINUS->addFunction(op_minus_number);
   OP_MINUS->addFunction(op_minus_float);
   OP_MINUS->addFunction(op_minus_bigint);
   OP_MINUS->addFunction(op_minus_hash_string);
   OP_MINUS->addFunction(op_minus_hash_list);
   OP_MINUS->addDefaultNothing();

   OP_PLUS = add(new Operator(2, "+", "plus", 1, false, false, check_op_plus));
   OP_PLUS->addFunction(NT_LIST,    NT_LIST,   op_plus_list);
   OP_PLUS->addFunction(op_plus_string);
   OP_PLUS->addFunction(op_plus_date);
   OP_PLUS->addFunction(op_plus_number);
   OP_PLUS->addFunction(op_plus_float);
   OP_PLUS->addFunction(op_plus_bigint);
   OP_PLUS->addFunction(NT_HASH,    NT_HASH,   op_plus_hash_hash);
   OP_PLUS->addFunction(NT_HASH,    NT_OBJECT, op_plus_hash_object);
   OP_PLUS->addFunction(NT_OBJECT,  NT_HASH,   op_plus_object_hash);
   OP_PLUS->addFunction(NT_BINARY,  NT_BINARY, op_plus_binary_binary);
   OP_PLUS->addDefaultNothing();

   OP_MULT = add(new Operator(2, "*", "multiply", 1, false, false, check_op_multiply));
   OP_MULT->addFunction(op_multiply_number);
   OP_MULT->addFunction(op_multiply_float);
   OP_MULT->addFunction(op_multiply_bigint);

   // return value is the same as with *
   OP_DIV = add(new Operator(2, "/", "divide", 1, false, false, check_op_multiply));
   OP_DIV->addFunction(op_divide_number);
   OP_DIV->addFunction(op_divide_float);
   OP_DIV->addFunction(op_divide_bigint);

   OP_SHIFT_LEFT = add(new Operator(2, "<<", "shift-left", 1, false, false, check_op_returns_integer));
   OP_SHIFT_LEFT->addFunction(op_shift_left_int);

   OP_SHIFT_RIGHT = add(new Operator(2, ">>", "shift-right", 1, false, false, check_op_returns_integer));
   OP_SHIFT_RIGHT->addFunction(op_shift_right_int);

   // cannot validate return type here yet
   OP_LIST_REF = add(new Operator(2, "[]", "list, string, or binary dereference", 0, false, false, check_op_list_ref));
   OP_LIST_REF->addFunction(NT_ALL, NT_ALL, op_list_ref);

   // cannot validate return type here yet
   OP_OBJECT_REF = add(new Operator(2, ".", "hash/object-reference", 0, false, false, check_op_object_ref));
   OP_OBJECT_REF->addFunction(NT_ALL, NT_ALL, op_object_ref);

   // can return a list or NOTHING
   OP_KEYS = add(new Operator(1, "keys", "list of keys", 0, false, false, check_op_keys));
   OP_KEYS->addFunction(NT_ALL, NT_NONE, op_keys);

   OP_SHIFT = add(new Operator(1, "shift", "shift from list", 0, true, true, check_op_list_op));
   OP_SHIFT->addFunction(op_shift);

   OP_POP = add(new Operator(1, "pop", "pop from list", 0, true, true, check_op_list_op));
   OP_POP->addFunction(op_pop);

   OP_PUSH = add(new Operator(2, "push", "push on list", 0, true, true, check_op_list_op));
   OP_PUSH->addFunction(op_push);

   OP_UNSHIFT = add(new Operator(2, "unshift", "unshift/insert to begnning of list", 0, true, true, check_op_unshift));
   OP_UNSHIFT->addFunction(op_unshift);

   // can return a string or NOTHING
   OP_REGEX_SUBST = add(new Operator(2, "regex subst", "regular expression substitution", 0, true, true, check_op_lvalue_string));
   OP_REGEX_SUBST->addFunction(NT_ALL, NT_REGEX_SUBST, op_regex_subst);

   // can return a string or NOTHING
   OP_REGEX_TRANS = add(new Operator(2, "transliteration", "transliteration", 0, true, true, check_op_lvalue_string));
   OP_REGEX_TRANS->addFunction(NT_ALL, NT_REGEX_TRANS, op_transliterate);

   // can return a list or NOTHING
   OP_REGEX_EXTRACT = add(new Operator(2, "regular expression subpattern extraction", "regular expression subpattern extraction", 0, false));
   OP_REGEX_EXTRACT->addFunction(op_regex_extract);

   OP_CHOMP = add(new Operator(1, "chomp", "chomp EOL marker from lvalue", 0, true, true, check_op_chomp_trim));
   OP_CHOMP->addFunction(NT_ALL, NT_NONE, op_chomp);

   OP_TRIM = add(new Operator(1, "trim", "trim characters from an lvalue", 0, true, true, check_op_chomp_trim));
   OP_TRIM->addFunction(NT_ALL, NT_NONE, op_trim);

   OP_FOLDL = add(new Operator(2, "foldl", "left fold expression", 0, true, false));
   OP_FOLDL->addFunction(NT_ALL, NT_ALL, op_foldl);

   OP_FOLDR = add(new Operator(2, "foldr", "right fold expression", 0, true, false));
   OP_FOLDR->addFunction(NT_ALL, NT_ALL, op_foldr);

   // can return a list or NOTHING
   OP_SELECT = add(new Operator(2, "select", "select elements from a list", 0, true, false));
   OP_SELECT->addFunction(NT_ALL, NT_ALL, op_select);

   // initialize all operators
   for (oplist_t::iterator i = begin(), e = end(); i != e; ++i)
      (*i)->init();
}
