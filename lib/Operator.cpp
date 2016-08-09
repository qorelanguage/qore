/*
  Operator.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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

#include <cmath>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pcre.h>

DLLLOCAL OperatorList oplist;

DLLLOCAL extern const QoreTypeInfo* bigIntFloatOrNumberTypeInfo, * floatOrNumberTypeInfo;

// the standard, system-default operator pointers
Operator *OP_MINUS,
   *OP_PLUS,
   *OP_MULT,
   *OP_LOG_CMP,
   *OP_PUSH,
   *OP_LIST_ASSIGNMENT,
   *OP_LOG_AND,
   *OP_LOG_OR,
   *OP_LOG_LT,
   *OP_LOG_GT,
   *OP_LOG_EQ,
   *OP_LOG_NE,
   *OP_LOG_LE,
   *OP_LOG_GE,
   *OP_ABSOLUTE_EQ,
   *OP_ABSOLUTE_NE
   ;

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

static bool op_log_eq_string(const QoreString* left, const QoreString* right, ExceptionSink* xsink) {
   return !left->compareSoft(right, xsink);
}

static bool op_log_gt_string(const QoreString* left, const QoreString* right, ExceptionSink* xsink) {
   return left->compare(right) > 0;
}

static bool op_log_ge_string(const QoreString* left, const QoreString* right, ExceptionSink* xsink) {
   return right->compare(left) >= 0;
}

static bool op_log_lt_string(const QoreString* left, const QoreString* right, ExceptionSink* xsink) {
   return left->compare(right) < 0;
}

static bool op_log_le_string(const QoreString* left, const QoreString* right, ExceptionSink* xsink) {
   return left->compare(right) <= 0;
}

static bool op_log_ne_string(const QoreString* left, const QoreString* right, ExceptionSink* xsink) {
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

static bool op_log_lt_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return left->lessThan(*right);
}

static bool op_log_le_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return left->lessThanOrEqual(*right);
}

static bool op_log_gt_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return left->greaterThan(*right);
}

static bool op_log_ge_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return left->greaterThanOrEqual(*right);
}

static bool op_log_eq_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return left->equals(*right);
}

static bool op_log_ne_number(const QoreNumberNode* left, const QoreNumberNode* right) {
   return !left->equals(*right);
}

static int64 op_cmp_number(const QoreNumberNode* left, const QoreNumberNode* right, ExceptionSink* xsink) {
   if (left->nan() || right->nan()) {
      xsink->raiseException("NAN-COMPARE-ERROR", "NaN in logical comparison operator");
      return 1;
   }

   if (left->lessThan(*right))
       return -1;

   if (left->equals(*right))
      return 0;

   return 1;
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

static QoreStringNode* op_plus_string(const QoreString* left, const QoreString* right, ExceptionSink* xsink) {
   QoreStringNodeHolder str(new QoreStringNode(*left));
   //printd(5, "op_plus_string() (%d) %p \"%s\" + (%d) %p \"%s\"\n", left->strlen(), left->getBuffer(), left->getBuffer(), right->strlen(), right->getBuffer(), right->getBuffer());
   //printd(5, "op_plus_string() str= (%d) %p \"%s\"\n", str->strlen(), str->getBuffer(), str->getBuffer());
   str->concat(right, xsink);
   if (*xsink)
      return 0;

   printd(5, "op_plus_string() result=\"%s\"\n", str->getBuffer());
   return str.release();
}

static int64 op_cmp_string(const QoreString* left, const QoreString* right, ExceptionSink* xsink) {
   return (int64)left->compare(right);
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

static int64 op_cmp_double(double left, double right, ExceptionSink* xsink) {
   if (std::isnan(left) || std::isnan(right)) {
      xsink->raiseException("NAN-COMPARE-ERROR", "NaN in logical comparison operator");
      return 1;
   }

   if (left < right)
       return -1;

   if (left == right)
      return 0;

   return 1;
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

   // assign to a blank list if the lvalue has no value yet but is typed as a list or softlist
   if (val.getType() == NT_NOTHING) {
      if (val.getTypeInfo() == listTypeInfo && val.assign(listTypeInfo->getDefaultValue()))
         return 0;
      if (val.getTypeInfo() == softListTypeInfo && val.assign(softListTypeInfo->getDefaultValue()))
         return 0;
   }

   // value is not a list, so throw exception
   if (val.getType() != NT_LIST) {
      // only throw a runtime exception if %strict-args is in effect
      if (runtime_check_parse_option(PO_STRICT_ARGS))
         xsink->raiseException("PUSH-ERROR", "the lvalue argument to push is type \"%s\"; expecting \"list\"", val.getTypeName());
      return 0;
   }

   // no exception can occur here
   val.ensureUnique();

   QoreListNode* l = reinterpret_cast<QoreListNode*>(val.getValue());

   printd(5, "op_push() about to call push() on list node %p (%d) with element %p\n", l, l->size(), elem);

   l->push(value.getReferencedValue());
   // reference for return value
   return ref_rv ? l->refSelf() : 0;
}

static QoreHashNode* op_minus_hash_string(const QoreHashNode* h, const QoreString* s, ExceptionSink* xsink) {
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

/*
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
*/

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

QoreValue CompareFloatOperatorFunction::eval(const AbstractQoreNode* left, const AbstractQoreNode* right, bool ref_rv, int args, ExceptionSink* xsink) const {
   // these functions can have no side effects
   if (!ref_rv)
      return QoreValue();

   return op_func(left->getAsFloat(), right->getAsFloat(), xsink);
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

/*
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
*/

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

   return op_func(reinterpret_cast<const QoreNumberNode*>(left), reinterpret_cast<const QoreNumberNode*>(right), xsink);
}

Operator::~Operator() {
   // erase all functions
  for (unsigned i = 0, size = functions.size(); i < size; i++)
     delete functions[i];
}

void Operator::init() {
   if (!evalArgs || (functions.size() == 1))
      return;
   op_matrix.resize(NUM_VALUE_TYPES);
   // create function lookup matrix for value types
   for (qore_type_t i = 0; i < NUM_VALUE_TYPES; i++) {
      op_matrix[i].resize(NUM_VALUE_TYPES);
      for (qore_type_t j = 0; j < NUM_VALUE_TYPES; j++)
	 op_matrix[i][j] = findFunction(i, j);
   }
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
      t = op_matrix[nleft->getType()][NT_NOTHING];
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
      t = op_matrix[nleft->getType()][nright->getType()];
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
   for (oplist_t::iterator i = begin(), e = end(); i != e; ++i)
      delete (*i);
   clear();
}

Operator *OperatorList::add(Operator *o) {
   push_back(o);
   return o;
}

// checks for illegal "self" assignments in an object context
void check_self_assignment(AbstractQoreNode* n, LocalVar* selfid) {
   qore_type_t ntype = n->getType();

   // if it's a variable reference
   if (ntype == NT_VARREF) {
      VarRefNode* v = reinterpret_cast<VarRefNode*>(n);
      if (v->getType() == VT_LOCAL && v->ref.id == selfid)
         parse_error("illegal assignment to 'self' in an object context");
      return;
   }
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
	 parse_error("expecting lvalue in position %d of left-hand-side list in list assignment, got '%s' instead", li.index() + 1, v->getTypeName());

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
      if (leftTypeInfo->isType(NT_NUMBER)
            || rightTypeInfo->isType(NT_NUMBER))
         returnTypeInfo = numberTypeInfo;
      else if (leftTypeInfo->isType(NT_FLOAT)
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

// set the return value for op_multiply (*)
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
   OP_LOG_GT->addFunction(op_log_gt_number);
   OP_LOG_GT->addFunction(op_log_gt_float);
   OP_LOG_GT->addFunction(op_log_gt_bigint);
   OP_LOG_GT->addFunction(op_log_gt_string);
   OP_LOG_GT->addFunction(op_log_gt_date);

   OP_LOG_EQ = add(new Operator(2, "==", "logical-equals", 1, false, false, check_op_logical));
   OP_LOG_EQ->addFunction(op_log_eq_string);
   OP_LOG_EQ->addFunction(op_log_eq_number);
   OP_LOG_EQ->addFunction(op_log_eq_float);
   OP_LOG_EQ->addFunction(op_log_eq_bigint);
   OP_LOG_EQ->addFunction(op_log_eq_boolean);
   OP_LOG_EQ->addFunction(op_log_eq_date);
   OP_LOG_EQ->addNoConvertFunction(NT_ALL, NT_ALL, op_log_eq_all);

   OP_LOG_NE = add(new Operator(2, "!=", "not-equals", 1, false, false, check_op_logical));
   OP_LOG_NE->addFunction(op_log_ne_string);
   OP_LOG_NE->addFunction(op_log_ne_number);
   OP_LOG_NE->addFunction(op_log_ne_float);
   OP_LOG_NE->addFunction(op_log_ne_bigint);
   OP_LOG_NE->addFunction(op_log_ne_boolean);
   OP_LOG_NE->addFunction(op_log_ne_date);
   OP_LOG_NE->addNoConvertFunction(NT_ALL, NT_ALL, op_log_ne_all);

   OP_LOG_LE = add(new Operator(2, "<=", "less-than-or-equals", 1, false, false, check_op_logical));
   OP_LOG_LE->addFunction(op_log_le_number);
   OP_LOG_LE->addFunction(op_log_le_float);
   OP_LOG_LE->addFunction(op_log_le_bigint);
   OP_LOG_LE->addFunction(op_log_le_string);
   OP_LOG_LE->addFunction(op_log_le_date);

   OP_LOG_GE = add(new Operator(2, ">=", "greater-than-or-equals", 1, false, false, check_op_logical));
   OP_LOG_GE->addFunction(op_log_ge_number);
   OP_LOG_GE->addFunction(op_log_ge_float);
   OP_LOG_GE->addFunction(op_log_ge_bigint);
   OP_LOG_GE->addFunction(op_log_ge_string);
   OP_LOG_GE->addFunction(op_log_ge_date);

   OP_ABSOLUTE_EQ = add(new Operator(2, "===", "absolute logical-equals", 0, false, false, check_op_logical));
   OP_ABSOLUTE_EQ->addFunction(NT_ALL, NT_ALL, op_absolute_log_eq);

   OP_ABSOLUTE_NE = add(new Operator(2, "!==", "absolute logical-not-equals", 0, false, false, check_op_logical));
   OP_ABSOLUTE_NE->addFunction(NT_ALL, NT_ALL, op_absolute_log_neq);

   // bigint operators
   OP_LOG_CMP = add(new Operator(2, "<=>", "logical-comparison", 1, false, false, check_op_returns_integer));
   OP_LOG_CMP->addFunction(op_cmp_string);
   OP_LOG_CMP->addFunction(op_cmp_number);
   OP_LOG_CMP->addFunction(op_cmp_double);
   OP_LOG_CMP->addFunction(op_cmp_bigint);
   OP_LOG_CMP->addCompareDateFunction();

   // non-boolean operators
   OP_LIST_ASSIGNMENT = add(new Operator(2, "(list) =", "list assignment", 0, true, true, check_op_list_assignment));
   OP_LIST_ASSIGNMENT->addFunction(NT_ALL, NT_ALL, op_list_assignment);

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

   OP_PUSH = add(new Operator(2, "push", "push on list", 0, true, true, check_op_list_op));
   OP_PUSH->addFunction(op_push);

   // initialize all operators
   for (oplist_t::iterator i = begin(), e = end(); i != e; ++i)
      (*i)->init();
}
