/*
  Operator.cc

  Qore Programming Language

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

#include <qore/Qore.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pcre.h>

DLLLOCAL OperatorList oplist;

// the standard, system-default operator pointers
Operator *OP_ASSIGNMENT, *OP_MODULA, 
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

// call to get a node with reference count 1 (copy on write)
static inline void ensure_unique(AbstractQoreNode **v, ExceptionSink *xsink) {
   if (!(*v)->is_unique()) {
      AbstractQoreNode *old = *v;
      (*v) = old->realCopy();
      old->deref(xsink);
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

static bool op_log_eq_date(const DateTimeNode *left, const DateTimeNode *right) {
   return left->isEqual(right);
}

static bool op_log_gt_date(const DateTimeNode *left, const DateTimeNode *right) {
   return DateTime::compareDates(left, right) > 0;
}

static bool op_log_ge_date(const DateTimeNode *left, const DateTimeNode *right) {
   return DateTime::compareDates(left, right) >= 0;
}

static bool op_log_lt_date(const DateTimeNode *left, const DateTimeNode *right) {
   return DateTime::compareDates(left, right) < 0;
}

static bool op_log_le_date(const DateTimeNode *left, const DateTimeNode *right) {
   return DateTime::compareDates(left, right) <= 0;
}

static bool op_log_ne_date(const DateTimeNode *left, const DateTimeNode *right) {
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

static bool op_log_eq_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink) {
   return !left->compareSoft(right, xsink);
}

static bool op_log_gt_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink) {
   return left->compare(right) > 0;
}

static bool op_log_ge_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink) {
   return right->compare(left) >= 0;
}

static bool op_log_lt_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink) {
   return left->compare(right) < 0;
}

static bool op_log_le_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink) {
   return left->compare(right) <= 0;
}

static bool op_log_ne_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink) {
   return left->compareSoft(right, xsink);
}

static bool op_absolute_log_eq(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
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

static bool op_absolute_log_neq(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   return !op_absolute_log_eq(left, right, xsink);
}

static bool op_regex_match(const QoreString *left, const QoreRegexNode *right, ExceptionSink *xsink) {
   return right->exec(left, xsink);
}

static bool op_regex_nmatch(const QoreString *left, const QoreRegexNode *right, ExceptionSink *xsink) {
   return !right->exec(left, xsink);
}

// takes all arguments unevaluated so logic short-circuiting can happen
static bool op_log_or(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   bool l = left->boolEval(xsink);   
   if (*xsink)
      return false;

   // if left side is true, then do not evaluate right side
   return l ? true : right->boolEval(xsink);
}

static bool op_log_eq_all(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   qore_type_t lt = left ? left->getType() : -1;
   qore_type_t rt = right ? right->getType() : -1;
   return (lt != -1 && rt != -1) ? left->is_equal_soft(right, xsink) : false;
}

/*
static bool op_log_eq_list(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   if (left->getType() != NT_LIST)
      return false;
   if (right->getType() != NT_LIST)
      return false;

   const QoreListNode *l = reinterpret_cast<const QoreListNode *>(left);
   const QoreListNode *r = reinterpret_cast<const QoreListNode *>(right);
   return l->is_equal_soft(r, xsink);
}

static bool op_log_eq_hash(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   const QoreHashNode *lh = left->getType() == NT_HASH ? reinterpret_cast<const QoreHashNode *>(left) : 0;
   if (!lh)
      return false;

   const QoreHashNode *rh = right->getType() == NT_HASH ? reinterpret_cast<const QoreHashNode *>(right) : 0;
   if (!rh)
      return false;

   return !lh->compareSoft(rh, xsink);
}

static bool op_log_eq_object(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   if (left->getType() != NT_OBJECT)
      return false;
   if (right->getType() != NT_OBJECT)
      return false;

   const QoreObject *l = reinterpret_cast<const QoreObject *>(left);
   const QoreObject *r = reinterpret_cast<const QoreObject *>(right);
   return !l->compareSoft(r, xsink);
}

static bool op_log_eq_nothing(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   assert(left->getType() == NT_NOTHING && right->getType() == NT_NOTHING);
   return true;
}

// this function is the catch-all for all types
static bool op_log_eq_null(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   qore_type_t lt = left ? left->getType() : -1;
   qore_type_t rt = right ? right->getType() : -1;
   return (lt != -1 && rt != -1) ? left->is_equal_soft(right, xsink) : false;
}

static bool op_log_eq_binary(const AbstractQoreNode *left, const AbstractQoreNode *right) {
   const BinaryNode *l = left->getType() == NT_BINARY ? reinterpret_cast<const BinaryNode *>(left) : 0;
   const BinaryNode *r = right->getType() == NT_BINARY ? reinterpret_cast<const BinaryNode *>(right) : 0;
   assert(l || r);

   if (!l || !r)
      return false;
   return !l->compare(r);
}
*/

static bool op_log_ne_all(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   qore_type_t lt = left ? left->getType() : -1;
   qore_type_t rt = right ? right->getType() : -1;
   return (lt != -1 && rt != -1) ? !left->is_equal_soft(right, xsink) : true;
}

/*
static bool op_log_ne_list(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   if (left->getType() != NT_LIST)
      return true;
   if (right->getType() != NT_LIST)
      return true;

   const QoreListNode *l = reinterpret_cast<const QoreListNode *>(left);
   const QoreListNode *r = reinterpret_cast<const QoreListNode *>(right);
   return !l->is_equal_soft(r, xsink);
}

static bool op_log_ne_hash(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink)
{
   if (left->getType() != NT_HASH)
      return true;
   if (right->getType() != NT_HASH)
      return true;

   const QoreHashNode *lh = reinterpret_cast<const QoreHashNode *>(left);
   const QoreHashNode *rh = reinterpret_cast<const QoreHashNode *>(right);
   return lh->compareSoft(rh, xsink);
}

static bool op_log_ne_object(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink)
{
   if (left->getType() != NT_OBJECT)
      return true;
   if (right->getType() != NT_OBJECT)
      return true;

   const QoreObject *l = reinterpret_cast<const QoreObject *>(left);
   const QoreObject *r = reinterpret_cast<const QoreObject *>(right);
   return l->compareSoft(r, xsink);
}

static bool op_log_ne_nothing(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink)
{
   assert(left->getType() == NT_NOTHING && right->getType() == NT_NOTHING);
   return false;
}

static bool op_log_ne_null(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink)
{
   if (left && left->getType() == NT_NULL && right && right->getType() == NT_NULL)
      return false;
   return true;
}

static bool op_log_ne_binary(const AbstractQoreNode *left, const AbstractQoreNode *right) {
   const BinaryNode *l = left->getType() == NT_BINARY ? reinterpret_cast<const BinaryNode *>(left) : 0;
   const BinaryNode *r = right->getType() == NT_BINARY ? reinterpret_cast<const BinaryNode *>(right) : 0;
   assert(l || r);

   if (!l || !r)
      return true;
   return l->compare(r);
}
*/

static bool op_exists(const AbstractQoreNode *left, const AbstractQoreNode *x, ExceptionSink *xsink)
{
   if (is_nothing(left))
      return false;
   if (!left->needs_eval())
      return true;

   ReferenceHolder<AbstractQoreNode> tn(xsink);
   AutoVLock vl(xsink);
   AbstractQoreNode *n = getExistingVarValue(left, xsink, &vl, tn);

   // return if an exception happened
   if (*xsink)
      return false;

   return is_nothing(n) ? false : true;
}

static bool op_instanceof(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink)
{
   assert(r && r->getType() == NT_CLASSREF);

   QoreNodeEvalOptionalRefHolder nl(l, xsink);
   if (*xsink || !nl || nl->getType() != NT_OBJECT)
      return false;

   const QoreObject *o = reinterpret_cast<const QoreObject *>(*nl);
   return o->validInstanceOf(reinterpret_cast<const ClassRefNode *>(r)->getID());
}

// takes all arguments unevaluated so logic short-circuiting can happen
static bool op_log_and(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink)
{
   // if left side is 0, then do not evaluate right side
   bool l = left->boolEval(xsink);
   if (xsink->isEvent())
      return false;
   bool b;
   if (!l)
      b = false;
   else
      b = right->boolEval(xsink);
   
   return b;
}

static int64 op_cmp_bigint(int64 left, int64 right)
{
   return left - right;
}

static int64 op_minus_bigint(int64 left, int64 right)
{
   return left - right;
}

static int64 op_plus_bigint(int64 left, int64 right)
{
   return left + right;
}

static int64 op_multiply_bigint(int64 left, int64 right)
{
   return left * right;
}

static int64 op_divide_bigint(int64 left, int64 right, ExceptionSink *xsink)
{
   if (!right)
   {
      xsink->raiseException("DIVISION-BY-ZERO", "division by zero in integer expression");
      return 0;
   }
   return left / right;
}

static DateTimeNode *op_minus_date(const DateTimeNode *left, const DateTimeNode *right)
{
    return left->subtractBy(right);
}

static DateTimeNode *op_plus_date(const DateTimeNode *left, const DateTimeNode *right)
{
    return left->add(right);
}

static double op_minus_float(double left, double right)
{
   return left - right;
}

static double op_plus_float(double left, double right)
{
   return left + right;
}

static double op_multiply_float(double left, double right)
{
   return left * right;
}

static double op_divide_float(double left, double right, ExceptionSink *xsink)
{
   if (!right)
   {
      xsink->raiseException("DIVISION-BY-ZERO", "division by zero in floating-point expression!");
      return 0.0;
   }
   return left / right;
}

static QoreStringNode *op_plus_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink)
{
   QoreStringNodeHolder str(new QoreStringNode(*left));
   //printd(5, "op_plus_string() (%d) %08p \"%s\" + (%d) %08p \"%s\"\n", left->strlen(), left->getBuffer(), left->getBuffer(), right->strlen(), right->getBuffer(), right->getBuffer());
   //printd(5, "op_plus_string() str= (%d) %08p \"%s\"\n", str->strlen(), str->getBuffer(), str->getBuffer());
   str->concat(right, xsink);
   if (*xsink)
      return 0;
   
   printd(5, "op_plus_string() result=\"%s\"\n", str->getBuffer());
   return str.release();
}

static int64 op_cmp_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink)
{
   return (int64)left->compare(right);
}

static int64 op_elements(const AbstractQoreNode *left, const AbstractQoreNode *null, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder np(left, xsink);
   if (*xsink || !np)
      return 0;

   qore_type_t ltype = np->getType();

   if (ltype == NT_LIST)
      return reinterpret_cast<const QoreListNode *>(*np)->size();

   if (ltype == NT_STRING)
      return reinterpret_cast<const QoreStringNode *>(*np)->length();

   if (ltype == NT_HASH)
      return reinterpret_cast<const QoreHashNode *>(*np)->size();	 

   if (ltype == NT_OBJECT)
      return reinterpret_cast<const QoreObject *>(*np)->size(xsink);

   if (ltype == NT_BINARY)
      return reinterpret_cast<const BinaryNode *>(*np)->size();

   return 0;
}

static QoreListNode *get_keys(const AbstractQoreNode *p, ExceptionSink *xsink)
{   
   if (!p)
      return 0;

   if (p->getType() == NT_HASH)
      return reinterpret_cast<const QoreHashNode *>(p)->getKeys();

   if (p->getType() == NT_OBJECT)
      return reinterpret_cast<const QoreObject *>(p)->getMemberList(xsink);

   return 0;
}

// FIXME: do not need ref_rv here - also do not need second argument
static AbstractQoreNode *op_keys(const AbstractQoreNode *left, const AbstractQoreNode *null, bool ref_rv, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder np(left, xsink);
   if (*xsink)
      return 0;

   return get_keys(*np, xsink);
}

// FIXME: do not need ref_rv here
static AbstractQoreNode *op_question_mark(const AbstractQoreNode *left, const AbstractQoreNode *list, bool ref_rv, ExceptionSink *xsink)
{
   assert(list && list->getType() == NT_LIST);
   bool b = left->boolEval(xsink);
   if (xsink->isEvent())
      return 0;

   const QoreListNode *l = reinterpret_cast<const QoreListNode *>(list);

   if (b)
      return l->retrieve_entry(0)->eval(xsink);
   return l->retrieve_entry(1)->eval(xsink);
}

static AbstractQoreNode *op_regex_subst(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // if it's not a string, then do nothing
   if (!v.check_type(NT_STRING))
      return 0;

   const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(v.get_value());

   assert(right && right->getType() == NT_REGEX_SUBST);
   const RegexSubstNode *rs = reinterpret_cast<const RegexSubstNode *>(right);

   // get new value
   QoreStringNode *nv = rs->exec(str, xsink);

   // if there is an exception above, nv = 0
   if (xsink->isEvent())
      return 0;

   // assign new value to lvalue (no exception possible here)
   v.assign(nv);

   // reference for return value if necessary
   return ref_rv ? nv->refSelf() : 0;
}

static AbstractQoreNode *op_regex_trans(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // if it's not a string, then do nothing
   if (!v.check_type(NT_STRING))
      return 0;

   const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(v.get_value());

   // get new value
   assert(right && right->getType() == NT_REGEX_TRANS);
   QoreStringNode *nv = reinterpret_cast<const RegexTransNode *>(right)->exec(str, xsink);

   // if there is an exception above, nv = 0
   if (*xsink)
      return 0;

   // assign new value to lvalue (no exception possible here)
   v.assign(nv);

   // reference for return value
   return ref_rv ? nv->refSelf() : 0;
}

static AbstractQoreNode *op_list_ref(const AbstractQoreNode *left, const AbstractQoreNode *index, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder lp(left, xsink);

   // return 0 if left side is not a list or string (or exception)
   if (!lp || *xsink)
      return 0;

   qore_type_t t = lp->getType();
   if (t != NT_LIST && t != NT_STRING && t != NT_BINARY)
      return 0;

   AbstractQoreNode *rv = 0;
   int ind = index->integerEval(xsink);
   if (!*xsink) {
      // get value
      if (t == NT_LIST) {
	 const QoreListNode *l = reinterpret_cast<const QoreListNode *>(*lp);
	 rv = l->get_referenced_entry(ind);
      }
      else if (t == NT_BINARY) {
	 const BinaryNode *b = reinterpret_cast<const BinaryNode *>(*lp);
	 if (ind < 0 || (unsigned)ind >= b->size())
	    return 0;
	 return new QoreBigIntNode(((unsigned char *)b->getPtr())[ind]);
      }
      else if (ind >= 0) {
	 const QoreStringNode *lpstr = reinterpret_cast<const QoreStringNode *>(*lp);
	 rv = lpstr->substr(ind, 1, xsink);
      }
      //printd(5, "op_list_ref() index=%d, rv=%08p\n", ind, rv);
   }
   return rv;
}

// for the member name, a string is required.  non-string arguments will
// be converted.  The null string can also be used
static AbstractQoreNode *op_object_ref(const AbstractQoreNode *left, const AbstractQoreNode *member, bool ref_rv, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder op(left, xsink);
   if (*xsink || !op)
      return 0;

   if (op->getType() == NT_HASH) {
      const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(*op);

      // evaluate member expression
      QoreNodeEvalOptionalRefHolder mem(member, xsink);
      if (*xsink)
	 return 0;

      if (mem && mem->getType() == NT_LIST) {
	 return h->getSlice(reinterpret_cast<const QoreListNode *>(*mem), xsink);
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
      return o->getSlice(reinterpret_cast<const QoreListNode *>(*mem), xsink);
   }

   QoreStringNodeValueHelper key(*mem);
   return o->evalMember(*key, xsink);
}

static AbstractQoreNode *op_object_method_call(const AbstractQoreNode *left, const AbstractQoreNode *func, bool ref_rv, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder op(left, xsink);
   if (*xsink)
      return 0;

   assert(func && func->getType() == NT_METHOD_CALL);
   const FunctionCallNode *f = reinterpret_cast<const FunctionCallNode *>(func);

   if (*op && (*op)->getType() == NT_HASH) {
      // FIXME: this is an ugly hack!
      const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(*op);
      // see if the hash member is a call reference
      const AbstractQoreNode *ref = h->getKeyValue(f->f.c_str);
      if (ref && (ref->getType() == NT_FUNCREF || ref->getType() == NT_RUNTIME_CLOSURE))
	 return reinterpret_cast<const ResolvedCallReferenceNode *>(ref)->exec(f->getArgs(), xsink);
   }

   if (!(*op) || (*op)->getType() != NT_OBJECT) {
      //printd(5, "op=%08p (%s) func=%08p (%s)\n", op, op ? op->getTypeName() : "n/a", func, func ? func->getTypeName() : "n/a");
      xsink->raiseException("OBJECT-METHOD-EVAL-ON-NON-OBJECT", "member function \"%s\" called on type \"%s\"", 
			    f->f.c_str, op ? op->getTypeName() : "NOTHING" );
      return 0;
   }

   QoreObject *o = const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(*op));
   return o->evalMethod(f->f.c_str, f->getArgs(), xsink);
}

static AbstractQoreNode *op_new_object(const AbstractQoreNode *left, const AbstractQoreNode *x, bool ref_rv, ExceptionSink *xsink)
{
   QORE_TRACE("op_new_object()");
   
   assert(left->getType() == NT_SCOPE_REF);
   const ScopedObjectCallNode *c = reinterpret_cast<const ScopedObjectCallNode *>(left);
   AbstractQoreNode *rv = c->oc->execConstructor(c->args, xsink);
   printd(5, "op_new_object() returning node=%08p (type=%s)\n", rv, c->oc->getName());
   // if there's an exception, the constructor will delete the object without the destructor

   return rv;
}

static AbstractQoreNode *op_assignment(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink) {
   /* assign new value, this value gets referenced with the
      eval(xsink) call, so there's no need to reference it again
      for the variable assignment - however it does need to be
      copied/referenced for the return value
   */
   ReferenceHolder<AbstractQoreNode> new_value(right->eval(xsink), xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // assign new value
   if (v.assign(new_value.release()))
      return 0;

#if 0
   printd(5, "op_assignment() *%08p=%08p (type=%s refs=%d)\n",
	  v, new_value, 
	  new_value ? new_value->getTypeName() : "(null)",
	  new_value ? new_value->reference_count() : 0 );
#endif

   // reference return value if necessary
   const AbstractQoreNode *rv = v.get_value();
   return ref_rv && rv ? rv->refSelf() : 0;
}

static AbstractQoreNode *op_list_assignment(const AbstractQoreNode *n_left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   assert(n_left && n_left->getType() == NT_LIST);
   const QoreListNode *left = reinterpret_cast<const QoreListNode *>(n_left);
   
   /* assign new value, this value gets referenced with the
      eval(xsink) call, so there's no need to reference it again
      for the variable assignment - however it does need to be
      copied/referenced for the return value
   */
   QoreNodeEvalOptionalRefHolder new_value(right, xsink);
   if (*xsink)
      return 0;

   // get values and save
   for (unsigned i = 0; i < left->size(); i++)
   {
      const AbstractQoreNode *lv = left->retrieve_entry(i);

      // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
      LValueHelper v(lv, xsink);
      if (!v)
	 return 0;

      // if there's only one value, then save it
      if (new_value && new_value->getType() == NT_LIST) { // assign to list position
	 const QoreListNode *nv = reinterpret_cast<const QoreListNode *>(*new_value);
	 v.assign(nv->get_referenced_entry(i));
      }
      else {
	 if (!i)
	    v.assign(new_value.getReferencedValue());
	 else
	    v.assign(0);
      }
      if (*xsink)
	 return 0;
   }


   return ref_rv ? new_value.getReferencedValue() : 0;
}

static AbstractQoreNode *op_plus_equals(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink) {
   QoreNodeEvalOptionalRefHolder new_right(right, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // dereferences happen in each section so that the
   // already referenced value can be passed to list->push()
   // if necessary
   // do list plus-equals if left-hand side is a list
   qore_type_t vtype = v.get_type();
   if (vtype == NT_LIST) {
      v.ensure_unique(); // no exception possible here
      QoreListNode *l = reinterpret_cast<QoreListNode *>(v.get_value());
      if (new_right && new_right->getType() == NT_LIST)
	 l->merge(reinterpret_cast<const QoreListNode *>(*new_right));
      else
	 l->push(new_right.getReferencedValue());
   } // do hash plus-equals if left side is a hash
   else if (vtype == NT_HASH) {
      if (new_right) {
	 if (new_right->getType() == NT_HASH) {
	    v.ensure_unique();
	    reinterpret_cast<QoreHashNode *>(v.get_value())->merge(reinterpret_cast<const QoreHashNode *>(*new_right), xsink);
	 }
	 else if (new_right->getType() == NT_OBJECT) {
	    v.ensure_unique();
	    const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(*new_right))->mergeDataToHash(reinterpret_cast<QoreHashNode *>(v.get_value()), xsink);
	 }
      }
   }
   // do hash/object plus-equals if left side is an object
   else if (vtype == NT_OBJECT) {
      if (new_right) {
	 QoreObject *o = reinterpret_cast<QoreObject *>(v.get_value());
	 // do not need ensure_unique() for objects
	 if (new_right->getType() == NT_OBJECT) {
	    ReferenceHolder<QoreHashNode> h(const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(*new_right))->copyData(xsink), xsink);
	    if (h)
	       o->merge(*h, xsink);
	 }
	 else if (new_right->getType() == NT_HASH)
	    o->merge(reinterpret_cast<const QoreHashNode *>(*new_right), xsink);
      }
   }
   // do string plus-equals if left-hand side is a string
   else if (vtype == NT_STRING) {
      if (new_right) {
	 QoreStringValueHelper str(*new_right);

	 v.ensure_unique();
	 QoreStringNode *vs = reinterpret_cast<QoreStringNode *>(v.get_value());
	 vs->concat(*str, xsink);
      }
   }
   else if (vtype == NT_FLOAT) {
      double f = new_right ? new_right->getAsFloat() : 0.0;
      if (f != 0.0) {
	 v.ensure_unique();
	 QoreFloatNode *vf = reinterpret_cast<QoreFloatNode *>(v.get_value());
	 vf->f += f;
      }
   }
   else if (vtype == NT_DATE) {
      if (new_right) {
	 DateTimeValueHelper date(*new_right);
	 if (v.assign(reinterpret_cast<DateTimeNode *>(v.get_value())->add(*date)))
	    return 0;
      }
   }
   else if (vtype == NT_NOTHING) {
      if (new_right) {
	 // assign rhs to lhs (take reference for assignment)
	 v.assign(new_right.getReferencedValue());
      }
   }
   else if (vtype == NT_BINARY) {
      if (new_right) {
	 v.ensure_unique();
	 BinaryNode *b = reinterpret_cast<BinaryNode *>(v.get_value());
	 if (new_right->getType() == NT_BINARY) {
	    const BinaryNode *arg = reinterpret_cast<const BinaryNode *>(*new_right);
	    b->append(arg);
	 }
	 else {
	    QoreStringNodeValueHelper str(*new_right);
	    if (str->strlen())
	       b->append(str->getBuffer(), str->strlen());
	 }
      }
   }
   else { // do integer plus-equals
      int64 iv = new_right ? new_right->getAsBigInt() : 0;

      // get new value if necessary
      if (v.ensure_unique_int())
	 return 0;
      QoreBigIntNode *i = reinterpret_cast<QoreBigIntNode *>(v.get_value());
      
      // increment current value
      i->val += iv;
   }

   // v has been assigned to a value by this point
   // reference return value
   return ref_rv ? v.get_value()->refSelf() : 0;
}

static AbstractQoreNode *op_minus_equals(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink) {
   QoreNodeEvalOptionalRefHolder new_right(right, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   if (is_nothing(*new_right)) {
      if (!ref_rv)
	 return 0;

      AbstractQoreNode *val = v.get_value();
      return val ? val->refSelf() : 0;
   }

   // do float minus-equals if left side is a float
   qore_type_t vtype = v.get_type();
   if (vtype == NT_FLOAT) {
      double f = new_right->getAsFloat();

      if (f) {
	 v.ensure_unique();
	 QoreFloatNode *vf = reinterpret_cast<QoreFloatNode *>(v.get_value());
	 vf->f -= f;
      }
   }
   else if (vtype == NT_DATE) {
      DateTimeValueHelper date(*new_right);
      v.assign(reinterpret_cast<DateTimeNode *>(v.get_value())->subtractBy(*date));
   }
   else if (vtype == NT_HASH) {
      if (new_right->getType() != NT_HASH && new_right->getType() != NT_OBJECT) {
	 v.ensure_unique();
	 QoreHashNode *vh = reinterpret_cast<QoreHashNode *>(v.get_value());

	 const QoreListNode *nrl = (new_right->getType() == NT_LIST) ? reinterpret_cast<const QoreListNode *>(*new_right) : 0;
	 if (nrl && nrl->size()) {
	    // treat each element in the list as a string giving a key to delete
	    ConstListIterator li(nrl);
	    while (li.next()) {
	       QoreStringValueHelper val(li.getValue());
	       
	       vh->removeKey(*val, xsink);
	       if (*xsink)
		  return 0;
	    }
	 }
	 else {
	    QoreStringValueHelper str(*new_right);
	    vh->removeKey(*str, xsink);
	 }
      }
   }
   else if (vtype == NT_OBJECT) {
      if (new_right->getType() != NT_HASH && new_right->getType() != NT_OBJECT) {
	 QoreObject *o = reinterpret_cast<QoreObject *>(v.get_value());

	 const QoreListNode *nrl = (new_right->getType() == NT_LIST) ? reinterpret_cast<const QoreListNode *>(*new_right) : 0;
	 if (nrl && nrl->size()) {
	    // treat each element in the list as a string giving a key to delete
	    ConstListIterator li(nrl);
	    while (li.next()) {
	       QoreStringValueHelper val(li.getValue());
	       
	       o->removeMember(*val, xsink);
	       if (*xsink)
		  return 0;
	    }
	 }
	 else {
	    QoreStringValueHelper str(*new_right);
	    o->removeMember(*str, xsink);
	 }
      }
   }
   else if (vtype == NT_NOTHING) {
      if (new_right->getType() == NT_FLOAT) {
	 const QoreFloatNode *f = reinterpret_cast<const QoreFloatNode *>(*new_right);
	 v.assign(new QoreFloatNode(-f->f));
      }
      else {
	 // optimization to eliminate a virtual function call in the most common case
	 int64 i = new_right->getAsBigInt();
	 v.assign(new QoreBigIntNode(-i));
      }
   }
   else { // do integer minus-equals
      int64 iv = new_right->getAsBigInt();
      
      // get new value if necessary
      if (v.ensure_unique_int())
	 return 0;
      QoreBigIntNode *i = reinterpret_cast<QoreBigIntNode *>(v.get_value());
      
      // decrement current value
      i->val -= iv;
   }

   // here we know that v has a value


   // reference return value and return
   return ref_rv ? v.get_value()->refSelf() : 0;
}

static AbstractQoreNode *op_and_equals(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   //QORE_TRACE("op_and_equals()");
   int64 val = right->bigIntEval(xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;
   
   QoreBigIntNode *b;
   // get new value if necessary
   if (v.get_type() == NT_NOTHING) {
      b = new QoreBigIntNode();
      if (v.assign(b))
	 return 0;
   }
   else {
      if (v.ensure_unique_int())
	 return 0;
      b = reinterpret_cast<QoreBigIntNode *>(v.get_value());
   }

   // and current value with arg val
   b->val &= val;

   // reference return value and return
   return ref_rv ? b->refSelf() : 0;
}

static AbstractQoreNode *op_or_equals(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink) {
   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   QoreBigIntNode *b;
   // get new value if necessary
   if (v.get_type() == NT_NOTHING) {
      b = new QoreBigIntNode(0);
      if (v.assign(b))
	 return 0;
   }
   else {
      if (v.ensure_unique_int())
	 return 0;
      b = reinterpret_cast<QoreBigIntNode *>(v.get_value());
   }

   // or current value with arg val
   b->val |= val;

   // reference return value and return
   return ref_rv ? b->refSelf() : 0;
}

static AbstractQoreNode *op_modula_equals(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink) {
   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return 0;
   
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   QoreBigIntNode *b;
   // get new value if necessary
   if (v.get_type() == NT_NOTHING) {
      b = new QoreBigIntNode(0);
      if (v.assign(b))
	 return 0;
   }
   else {
      if (v.ensure_unique_int())
	 return 0;
      b = reinterpret_cast<QoreBigIntNode *>(v.get_value());
   }

   if (val)
      b->val %= val;
   else
      b->val = 0;

   // reference return value and return
   return ref_rv ? b->refSelf() : 0;
}

static AbstractQoreNode *op_multiply_equals(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink) {
   QoreNodeEvalOptionalRefHolder res(right, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // is either side a float?
   if (v.get_type() == NT_FLOAT) {
      double f = res ? res->getAsFloat() : 0;

      if (f) {
	 v.ensure_unique();
	 QoreFloatNode *vf = reinterpret_cast<QoreFloatNode *>(v.get_value());
	 vf->f *= f;
      }
      else { // if factor is NOTHING, assign 0.0
	 if (v.assign(new QoreFloatNode))
	    return 0;
      }
   }
   else {
      if (res && res->getType() == NT_FLOAT) {
	 if (v.get_type() == NT_NOTHING)
	    v.assign(new QoreFloatNode(0.0));
	 else {
	    if (v.ensure_unique_float())
	       return 0;
	    
	    // multiply current value with arg val
	    QoreFloatNode *vf = reinterpret_cast<QoreFloatNode *>(v.get_value());
	    vf->f *= (reinterpret_cast<const QoreFloatNode *>(*res))->f;
	 }
      }
      else { // do integer multiply equals
	 // get new value if necessary
	 if (v.get_type() == NT_NOTHING) {
	    v.assign(new QoreBigIntNode);
	 }
	 else {
	    if (res) {
	       if (v.ensure_unique_int())
		  return 0;

	       QoreBigIntNode *b = reinterpret_cast<QoreBigIntNode *>(v.get_value());
	       
	       // multiply current value with arg val
	       b->val *= res->getAsBigInt();
	    }
	    else { // if factor is NOTHING, assign 0
	       if (v.assign(new QoreBigIntNode))
		  return 0;
	    }
	 }
      }
   }

   // reference return value and return
   return ref_rv ? v.get_value()->refSelf() : 0;
}

static AbstractQoreNode *op_divide_equals(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink) {
   QoreNodeEvalOptionalRefHolder res(right, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // is either side a float?
   if (res && res->getType() == NT_FLOAT) {
      const QoreFloatNode *rf = reinterpret_cast<const QoreFloatNode *>(*res);

      if (!rf->f) {
	 xsink->raiseException("DIVISION-BY-ZERO", "division by zero in floating-point expression!");
	 return 0;
      }

      if (v.get_type() == NT_NOTHING)
	 v.assign(new QoreFloatNode());
      else {
	 if (v.ensure_unique_float())
	    return 0;

	 QoreFloatNode *vf = reinterpret_cast<QoreFloatNode *>(v.get_value());
	 // divide current value with arg val
	 vf->f /= rf->f;
      }
   }
   else if (v.get_type() == NT_FLOAT) {
      float val = res ? res->getAsFloat() : 0.0;
      if (val == 0.0) {
	 xsink->raiseException("DIVISION-BY-ZERO", "division by zero in floating-point expression!");
	 return 0;
      }
      v.ensure_unique();

      QoreFloatNode *vf = reinterpret_cast<QoreFloatNode *>(v.get_value());
      vf->f /= val;
   }
   else { // do integer divide equals
      int64 val = res ? res->getAsBigInt() : 0;
      if (!val) {
	 xsink->raiseException("DIVISION-BY-ZERO", "division by zero in integer expression!");
	 return 0;
      }
      // get new value if necessary
      if (v.get_type() == NT_NOTHING)
	 v.assign(new QoreBigIntNode());
      else {
	 if (v.ensure_unique_int())
	    return 0;

	 QoreBigIntNode *b = reinterpret_cast<QoreBigIntNode *>(v.get_value());
	 
	 // divide current value with arg val
	 b->val /= val;
      }
   }

   assert(v.get_value());
   // reference return value and return
   return ref_rv ? v.get_value()->refSelf() : 0;
}

static AbstractQoreNode *op_xor_equals(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink) {
   int64 val = right->bigIntEval(xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   QoreBigIntNode *b;

   // get new value if necessary
   if (v.get_type() == NT_NOTHING) {
      b = new QoreBigIntNode;
      v.assign(b);
   }
   else {
      if (v.ensure_unique_int())
	 return 0;

      b = reinterpret_cast<QoreBigIntNode *>(v.get_value());
   }

   // xor current value with arg val
   b->val ^= val;

   // reference return value and return
   return ref_rv ? v.get_value()->refSelf() : 0;
}

static AbstractQoreNode *op_shift_left_equals(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink) {
   int64 val = right->bigIntEval(xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   QoreBigIntNode *b;
   // get new value if necessary
   if (v.get_type() == NT_NOTHING) {
      b = new QoreBigIntNode(0);
      v.assign(b);
   }
   else {
      if (v.ensure_unique_int())
	 return 0;

      b = reinterpret_cast<QoreBigIntNode *>(v.get_value());
   }

   // shift left current value by arg val
   b->val <<= val;

   // reference return value and return
   return ref_rv ? v.get_value()->refSelf() : 0;
}

static AbstractQoreNode *op_shift_right_equals(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   //QORE_TRACE("op_shift_right_equals()");

   int64 val = right->bigIntEval(xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   QoreBigIntNode *b;
   // get new value if necessary
   if (v.get_type() == NT_NOTHING) {
      b = new QoreBigIntNode();
      v.assign(b);
   }
   else {
      if (v.ensure_unique_int())
	 return 0;

      b = reinterpret_cast<QoreBigIntNode *>(v.get_value());
   }

   // shift right current value by arg val
   b->val >>= val;

   // reference return value and return
   return ref_rv ? v.get_value()->refSelf() : 0;
}

// this is the default (highest-priority) function for the + operator, so any type could be sent here on either side
static AbstractQoreNode *op_plus_list(const AbstractQoreNode *left, const AbstractQoreNode *right) {
   if (left->getType() == NT_LIST) {
      const QoreListNode *l = reinterpret_cast<const QoreListNode *>(left);
      QoreListNode *rv = l->copy();
      if (right->getType() == NT_LIST)
	 rv->merge(reinterpret_cast<const QoreListNode *>(right));
      else
	 rv->push(right->refSelf());
      //printd(5, "op_plus_list() returning list=%08p size=%d\n", rv, rv->size());
      return rv;
   }

   if (right->getType() != NT_LIST)
      return 0;
   const QoreListNode *r = reinterpret_cast<const QoreListNode *>(right);

   QoreListNode *rv = new QoreListNode();
   rv->push(left->refSelf());
   rv->merge(r);
   return rv;
}

static AbstractQoreNode *op_plus_hash_hash(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   if (left->getType() == NT_HASH) {
      const QoreHashNode *lh = reinterpret_cast<const QoreHashNode *>(left);

      if (right->getType() != NT_HASH)
	 return left->refSelf();
      const QoreHashNode *rh = reinterpret_cast<const QoreHashNode *>(right);

      ReferenceHolder<QoreHashNode> rv(lh->copy(), xsink);
      rv->merge(rh, xsink);
      if (*xsink)
	 return 0;
      return rv.release();
   }

   return right->getType() == NT_HASH ? right->refSelf() : 0;
}

static AbstractQoreNode *op_plus_hash_object(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   if (left->getType() == NT_HASH) {
      const QoreHashNode *lh = reinterpret_cast<const QoreHashNode *>(left);
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
static AbstractQoreNode *op_plus_object_hash(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   if (left->getType() == NT_OBJECT) {
      if (right->getType() != NT_HASH)
	 return left->refSelf();

      QoreObject *l = const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(left));
      const QoreHashNode *rh = reinterpret_cast<const QoreHashNode *>(right);

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

static AbstractQoreNode *op_plus_binary_binary(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) {
   if (right->getType() != NT_BINARY)
      return left ? left->refSelf() : 0;

   if (left->getType() != NT_BINARY)
      return right->refSelf();

   const BinaryNode *l = reinterpret_cast<const BinaryNode *>(left);
   const BinaryNode *r = reinterpret_cast<const BinaryNode *>(right);

   BinaryNode *rv = l->copy();
   rv->append(r);
   return rv;
}

static int64 op_cmp_double(double left, double right)
{
   if (left < right)
       return -1;
       
   if (left == right)
      return 0;
       
   return 1;
}

static int64 op_modula_int(int64 left, int64 right)
{
    return right ? left % right : 0;
}

static int64 op_bin_and_int(int64 left, int64 right)
{
   return left & right;
}

static int64 op_bin_or_int(int64 left, int64 right)
{
   return left | right;
}

static int64 op_bin_xor_int(int64 left, int64 right)
{
   return left ^ right;
}

static int64 op_shift_left_int(int64 left, int64 right)
{
   return left << right;
}

static int64 op_shift_right_int(int64 left, int64 right)
{
   return left >> right;
}

// variable assignment
static AbstractQoreNode *op_post_inc(const AbstractQoreNode *left, bool ref_rv, ExceptionSink *xsink) {
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper n(left, xsink);
   if (!n)
      return 0;

   // reference for return value is reference for variable assignment (if not null)
   AbstractQoreNode * rv = n.take_value();

   // acquire new value
   QoreBigIntNode *b = new QoreBigIntNode(!is_nothing(rv) ? rv->getAsBigInt() : 0);
   // no exception possible due to the "take_value()" call above
   n.assign(b);
   assert(!*xsink);

   // increment value
   b->val++;

   // return original value (may be null or non-integer)
   return rv;
}

// variable assignment
static AbstractQoreNode *op_post_dec(const AbstractQoreNode *left, bool ref_rv, ExceptionSink *xsink) {
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper n(left, xsink);
   if (!n)
      return 0;

   // reference for return value is reference for variable assignment (if not null)
   AbstractQoreNode *rv = n.take_value();

   // acquire new value
   QoreBigIntNode *b = new QoreBigIntNode(!is_nothing(rv) ? rv->getAsBigInt() : 0);
   // no exception possible due to the "take_value()" call above
   n.assign(b);
   assert(!*xsink);
   
   // decrement value
   b->val--;

   // return original value (may be null or non-integer)
   return rv;
}

// variable assignment
static AbstractQoreNode *op_pre_inc(const AbstractQoreNode *left, bool ref_rv, ExceptionSink *xsink) {
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper n(left, xsink);
   if (!n)
      return 0;

   QoreBigIntNode *b;
   // acquire new value if necessary
   if (n.get_type() == NT_NOTHING) {
      b = new QoreBigIntNode;
      n.assign(b);
   }
   else {
      if (n.ensure_unique_int())
	 return 0;
      b = reinterpret_cast<QoreBigIntNode *>(n.get_value());
   }

   // increment value
   ++b->val;

   //printd(5, "op_pre_inc() ref_rv=%s\n", ref_rv ? "true" : "false");
   // reference for return value
   return ref_rv ? b->refSelf() : 0;
}

// variable assignment
static AbstractQoreNode *op_pre_dec(const AbstractQoreNode *left, bool ref_rv, ExceptionSink *xsink) {
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper n(left, xsink);
   if (!n)
      return 0;

   QoreBigIntNode *b;
   // acquire new value if necessary
   if (n.get_type() == NT_NOTHING) {
      b = new QoreBigIntNode(0);
      n.assign(b);
   }
   else {
      if (n.ensure_unique_int())
	 return 0;
      b = reinterpret_cast<QoreBigIntNode *>(n.get_value());
   }

   // decrement value
   b->val--;

   // reference return value
   return ref_rv ? b->refSelf() : 0;
}

// unshift lvalue, element
static AbstractQoreNode *op_unshift(const AbstractQoreNode *left, const AbstractQoreNode *elem, bool ref_rv, ExceptionSink *xsink) {
   printd(5, "op_unshift(%08p, %08p, isEvent=%d)\n", left, elem, xsink->isEvent());

   QoreNodeEvalOptionalRefHolder value(elem, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(left, xsink);
   if (!val)
      return 0;

   // value is not a list, so throw exception
   if (val.get_type() != NT_LIST) {
      xsink->raiseException("UNSHIFT-ERROR", "first argument to unshift is not a list");
      return 0;
   }

   // no exception can occur here
   val.ensure_unique();

   QoreListNode *l = reinterpret_cast<QoreListNode *>(val.get_value());

   printd(5, "op_unshift() about to call unshift() on list node %08p (%d) with element %08p\n", l, l->size(), elem);

   l->insert(value.getReferencedValue());

   // reference for return value
   return ref_rv ? l->refSelf() : 0;
}

static AbstractQoreNode *op_shift(const AbstractQoreNode *left, const AbstractQoreNode *x, bool ref_rv, ExceptionSink *xsink) {
   //QORE_TRACE("op_shift()");
   printd(5, "op_shift(%08p, %08p, isEvent=%d)\n", left, x, xsink->isEvent());

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(left, xsink);
   if (!val)
      return 0;

   if (val.get_type() != NT_LIST)
      return 0;

   // no exception can occurr here
   val.ensure_unique();

   QoreListNode *l = reinterpret_cast<QoreListNode *>(val.get_value());

   printd(5, "op_shift() about to call QoreListNode::shift() on list node %08p (%d)\n", l, l->size());

   // the list reference will now be the reference for return value
   // therefore no need to reference again
   return l->shift();
}

static AbstractQoreNode *op_pop(const AbstractQoreNode *left, const AbstractQoreNode *x, bool ref_rv, ExceptionSink *xsink) {
   printd(5, "op_pop(%08p, %08p, isEvent=%d)\n", left, x, xsink->isEvent());

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(left, xsink);
   if (!val)
      return 0;

   if (val.get_type() != NT_LIST)
      return 0;

   // no exception can occurr here
   val.ensure_unique();

   QoreListNode *l = reinterpret_cast<QoreListNode *>(val.get_value());

   printd(5, "op_pop() about to call QoreListNode::pop() on list node %08p (%d)\n", l, l->size());

   // the list reference will now be the reference for return value
   // therefore no need to reference again
   return l->pop();
}

static AbstractQoreNode *op_push(const AbstractQoreNode *left, const AbstractQoreNode *elem, bool ref_rv, ExceptionSink *xsink) {
   //QORE_TRACE("op_push()");
   printd(5, "op_push(%08p, %08p, isEvent=%d)\n", left, elem, xsink->isEvent());

   QoreNodeEvalOptionalRefHolder value(elem, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(left, xsink);
   if (!val)
      return 0;

   if (val.get_type() != NT_LIST)
      return 0;

   // no exception can occurr here
   val.ensure_unique();

   QoreListNode *l = reinterpret_cast<QoreListNode *>(val.get_value());

   printd(5, "op_push() about to call push() on list node %08p (%d) with element %08p\n", l, l->size(), elem);

   l->push(value.getReferencedValue());

   // reference for return value
   return ref_rv ? l->refSelf() : 0;
}

// lvalue, offset, [length, [list]]
static AbstractQoreNode *op_splice(const AbstractQoreNode *left, const AbstractQoreNode *n_l, bool ref_rv, ExceptionSink *xsink)
{
   printd(5, "op_splice(%08p, %08p, isEvent=%d)\n", left, n_l, xsink->isEvent());

   assert(n_l->getType() == NT_LIST);
   const QoreListNode *l = reinterpret_cast<const QoreListNode *>(n_l);

   // evaluate list
   QoreListNodeEvalOptionalRefHolder nl(l, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(left, xsink);
   if (!val)
      return 0;

   // if value is not a list or string, throw exception
   const qore_type_t vt = val.get_type();
   if (vt != NT_LIST && vt != NT_STRING) {
      xsink->raiseException("SPLICE-ERROR", "first argument to splice is not a list or a string");
      return 0;
   }
   
   // no exception can occur here
   val.ensure_unique();

   // evaluating a list must give another list
   int size = nl->size();
   int offset = nl->getEntryAsInt(0);

#ifdef DEBUG
   if (vt == NT_LIST) {
      QoreListNode *vl = reinterpret_cast<QoreListNode *>(val.get_value());
      printd(5, "op_splice() val=%08p (size=%d) list=%08p (size=%d) offset=%d\n", val.get_value(), vl->size(), *nl, size, offset);
   }
   else {
      QoreStringNode *vs = reinterpret_cast<QoreStringNode *>(val.get_value());
      printd(5, "op_splice() val=%08p (strlen=%d) list=%08p (size=%d) offset=%d\n", val.get_value(), vs->strlen(), *nl, size, offset);
   }
#endif

   if (vt == NT_LIST) {
      QoreListNode *vl = reinterpret_cast<QoreListNode *>(val.get_value());
      if (size == 1)
	 vl->splice(offset, xsink);
      else {
	 int length = nl->getEntryAsInt(1);
	 if (size == 2)
	    vl->splice(offset, length, xsink);
	 else
	    vl->splice(offset, length, nl->retrieve_entry(2), xsink);
      }
   }
   else { // must be a string
      QoreStringNode *vs = reinterpret_cast<QoreStringNode *>(val.get_value());
      if (size == 1)
	 vs->splice(offset, xsink);
      else {
	 int length = nl->getEntryAsInt(1);
	 if (size == 2)
	    vs->splice(offset, length, xsink);
	 else
	    vs->splice(offset, length, nl->retrieve_entry(2), xsink);
      }
   }

   // reference for return value
   return ref_rv ? val.get_value()->refSelf() : 0;
}

static int64 op_chomp(const AbstractQoreNode *arg, const AbstractQoreNode *x, ExceptionSink *xsink)
{
   //QORE_TRACE("op_chomp()");

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(arg, xsink);
   if (!val)
      return 0;

   const qore_type_t vtype = val.get_type();
   if (vtype != NT_LIST && vtype != NT_STRING && vtype != NT_HASH)
      return 0;

   // note that no exception can happen here
   val.ensure_unique();
   assert(!*xsink);

   if (vtype == NT_STRING)
      return reinterpret_cast<QoreStringNode *>(val.get_value())->chomp();

   int64 count = 0;   

   if (vtype == NT_LIST) {
      QoreListNode *l = reinterpret_cast<QoreListNode *>(val.get_value());
      ListIterator li(l);
      while (li.next()) {
	 AbstractQoreNode **v = li.getValuePtr();
	 if (*v && (*v)->getType() == NT_STRING) {
	    // note that no exception can happen here
	    ensure_unique(v, xsink);
	    assert(!*xsink);
	    QoreStringNode *vs = reinterpret_cast<QoreStringNode *>(*v);
	    count += vs->chomp();
	 }
      }      
      return count;
   }

   // must be a hash
   QoreHashNode *vh = reinterpret_cast<QoreHashNode *>(val.get_value());
   HashIterator hi(vh);
   while (hi.next()) {
      AbstractQoreNode **v = hi.getValuePtr();
      if (*v && (*v)->getType() == NT_STRING) {
	 // note that no exception can happen here
	 ensure_unique(v, xsink);
	 assert(!*xsink);
	 QoreStringNode *vs = reinterpret_cast<QoreStringNode *>(*v);
	 count += vs->chomp();
      }
   }
   return count;
}

static AbstractQoreNode *op_trim(const AbstractQoreNode *arg, const AbstractQoreNode *x, bool ref_rv, ExceptionSink *xsink) {
   //QORE_TRACE("op_trim()");
   
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(arg, xsink);
   if (!val)
      return 0;

   const qore_type_t vtype = val.get_type();
   if (vtype != NT_LIST && vtype != NT_STRING && vtype != NT_HASH)
      return 0;
   
   // note that no exception can happen here
   val.ensure_unique();
   assert(!*xsink);

   if (vtype == NT_STRING) {
      QoreStringNode *vs = reinterpret_cast<QoreStringNode *>(val.get_value());
      vs->trim();
   }
   else if (vtype == NT_LIST) {
      QoreListNode *l = reinterpret_cast<QoreListNode *>(val.get_value());
      ListIterator li(l);
      while (li.next()) {
	 AbstractQoreNode **v = li.getValuePtr();
	 if (*v && (*v)->getType() == NT_STRING) {
	    // note that no exception can happen here
	    ensure_unique(v, xsink);
	    assert(!*xsink);
	    QoreStringNode *vs = reinterpret_cast<QoreStringNode *>(*v);
	    vs->trim();
	 }
      }      
   }
   else { // is a hash
      QoreHashNode *vh = reinterpret_cast<QoreHashNode *>(val.get_value());
      HashIterator hi(vh);
      while (hi.next()) {
	 AbstractQoreNode **v = hi.getValuePtr();
	 if (*v && (*v)->getType() == NT_STRING) {
	    // note that no exception can happen here
	    assert(!*xsink);
	    ensure_unique(v, xsink);
	    QoreStringNode *vs = reinterpret_cast<QoreStringNode *>(*v);
	    vs->trim();
	 }
      }
   }

   // reference for return value
   return ref_rv ? val.get_value()->refSelf() : 0;
}

static AbstractQoreNode *op_map(const AbstractQoreNode *left, const AbstractQoreNode *arg_exp, bool ref_rv, ExceptionSink *xsink) {
   // conditionally evaluate argument
   QoreNodeEvalOptionalRefHolder arg(arg_exp, xsink);
   if (*xsink || is_nothing(*arg))
      return 0;

   if (arg->getType() != NT_LIST) {
      SingleArgvContextHelper argv_helper(*arg, xsink);
      return left->eval(xsink);
   }

   ReferenceHolder<QoreListNode> rv(ref_rv ? new QoreListNode() : 0, xsink);
   ConstListIterator li(reinterpret_cast<const QoreListNode *>(*arg));
   while (li.next()) {
      SingleArgvContextHelper argv_helper(li.getValue(), xsink);
      ReferenceHolder<AbstractQoreNode> val(left->eval(xsink), xsink);
      if (*xsink)
	 return 0;
      if (ref_rv)
	  rv->push(val.release());
   }
   return rv.release();
}

static AbstractQoreNode *op_map_select(const AbstractQoreNode *left, const AbstractQoreNode *arg_exp, bool ref_rv, ExceptionSink *xsink) {
   assert(arg_exp->getType() == NT_LIST);

   const QoreListNode *arg_list = reinterpret_cast<const QoreListNode *>(arg_exp);

   // conditionally evaluate argument expression
   QoreNodeEvalOptionalRefHolder marg(arg_list->retrieve_entry(0), xsink);
   if (*xsink)
      return 0;

   const AbstractQoreNode *select = arg_list->retrieve_entry(1);

   if (!marg || marg->getType() != NT_LIST) {
      // check if value can be mapped
      {
	 SingleArgvContextHelper argv_helper(*marg, xsink);
	 bool b = select->boolEval(xsink);
	 if (*xsink || !b)
	    return 0;
      }

      SingleArgvContextHelper argv_helper(*marg, xsink);
      ReferenceHolder<AbstractQoreNode> val(left->eval(xsink), xsink);
      return *xsink ? 0 : val.release();
   }

   ReferenceHolder<QoreListNode> rv(ref_rv ? new QoreListNode() : 0, xsink);
   ConstListIterator li(reinterpret_cast<const QoreListNode *>(*marg));
   while (li.next()) {
      const AbstractQoreNode *elem = li.getValue();
      // check if value can be mapped
      {
	 SingleArgvContextHelper argv_helper(elem, xsink);
	 bool b = select->boolEval(xsink);
	 if (*xsink)
	    return 0;
	 if (!b)
	    continue;
      }

      SingleArgvContextHelper argv_helper(elem, xsink);
      ReferenceHolder<AbstractQoreNode> val(left->eval(xsink), xsink);
      if (*xsink)
	 return 0;
      if (ref_rv)
	  rv->push(val.release());
   }
   return rv.release();
}

static AbstractQoreNode *op_foldl(const AbstractQoreNode *left, const AbstractQoreNode *arg_exp, bool ref_rv, ExceptionSink *xsink) {
   // conditionally evaluate argument
   QoreNodeEvalOptionalRefHolder arg(arg_exp, xsink);
   if (!arg || *xsink)
      return 0;

   // return the argument if there is no list
   if (arg->getType() != NT_LIST)
      return arg.getReferencedValue();

   const QoreListNode *l = reinterpret_cast<const QoreListNode *>(*arg);

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
      // create argument list
      QoreListNode *args = new QoreListNode();
      args->push(result.release());
      args->push(li.getReferencedValue());

      ArgvContextHelper argv_helper(args, xsink);

      result = left->eval(xsink);
      if (*xsink)
	 return 0;
   }
   return result.release();
}

static AbstractQoreNode *op_foldr(const AbstractQoreNode *left, const AbstractQoreNode *arg_exp, bool ref_rv, ExceptionSink *xsink) {
   // conditionally evaluate argument
   QoreNodeEvalOptionalRefHolder arg(arg_exp, xsink);
   if (!arg || *xsink)
      return 0;

   // return the argument if there is no list
   if (arg->getType() != NT_LIST)
      return arg->refSelf();

   const QoreListNode *l = reinterpret_cast<const QoreListNode *>(*arg);

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
      // create argument list
      QoreListNode *args = new QoreListNode();
      args->push(result.release());
      args->push(li.getReferencedValue());

      ArgvContextHelper argv_helper(args, xsink);

      result = left->eval(xsink);
      if (*xsink)
	 return 0;
   }
   return result.release();
}

static AbstractQoreNode *op_select(const AbstractQoreNode *arg_exp, const AbstractQoreNode *select, bool ref_rv, ExceptionSink *xsink) {
   // conditionally evaluate argument
   QoreNodeEvalOptionalRefHolder arg(arg_exp, xsink);
   if (!arg || *xsink)
      return 0;

   if (arg->getType() != NT_LIST) {
      SingleArgvContextHelper argv_helper(*arg, xsink);
      bool b = select->boolEval(xsink);
      if (*xsink)
	 return 0;

      return b ? arg.getReferencedValue() : 0;
   }

   ReferenceHolder<QoreListNode> rv(new QoreListNode(), xsink);
   ConstListIterator li(reinterpret_cast<const QoreListNode *>(*arg));
   while (li.next()) {
      SingleArgvContextHelper argv_helper(li.getValue(), xsink);
      bool b = select->boolEval(xsink);
      if (*xsink)
	 return 0;
      if (b)
	 rv->push(li.getReferencedValue());
   }
   return rv.release();
}

static QoreHashNode *op_minus_hash_string(const QoreHashNode *h, const QoreString *s, ExceptionSink *xsink) {
   ReferenceHolder<QoreHashNode> nh(h->copy(), xsink);
   nh->removeKey(s, xsink);
   if (*xsink)
      return 0;
   return nh.release();
}

static QoreHashNode *op_minus_hash_list(const QoreHashNode *h, const QoreListNode *l, ExceptionSink *xsink) {
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

static AbstractQoreNode *op_regex_extract(const QoreString *left, const QoreRegexNode *right, ExceptionSink *xsink) {
   return right->extractSubstrings(left, xsink);
}

static AbstractQoreNode *get_node_type(const AbstractQoreNode *n, qore_type_t t) {
   assert(n);
   assert(n->getType() != t);

   if (t == NT_STRING) {
      QoreStringNode *str = new QoreStringNode();
      n->getStringRepresentation(*str);
      return str;
   }

   if (t == NT_INT)
      return new QoreBigIntNode(n->getAsBigInt());

   if (t == NT_FLOAT)
      return new QoreFloatNode(n->getAsFloat());

   if (t == NT_BOOLEAN)
      return get_bool_node(n->getAsBool());

   if (t == NT_DATE) {
      DateTimeNode *dt = new DateTimeNode();
      n->getDateTimeRepresentation(*dt);
      return dt;
   }
   
   if (t == NT_LIST) {
      QoreListNode *l = new QoreListNode();
      l->push(n ? n->refSelf() : 0);
      return l;
   }

   printd(0, "DEBUG: get_node_type() got type '%s', aborting\n", n->getTypeName());
   assert(false);
   return 0;
}

OperatorFunction::OperatorFunction(qore_type_t lt, qore_type_t rt, op_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
{
}

BoolOperatorFunction::BoolOperatorFunction(qore_type_t lt, qore_type_t rt, op_bool_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
{
}

BigIntOperatorFunction::BigIntOperatorFunction(qore_type_t lt, qore_type_t rt, op_bigint_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
{
}

FloatOperatorFunction::FloatOperatorFunction(qore_type_t lt, qore_type_t rt, op_float_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
{
}

AbstractQoreNode *OperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
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

bool OperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(OperatorFunction::eval(left, right, true, args, xsink), xsink);
   return *rv ? rv->getAsBool() : false;
}

int64 OperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(OperatorFunction::eval(left, right, true, args, xsink), xsink);
   return *rv ? rv->getAsBigInt() : 0;
}

double OperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(OperatorFunction::eval(left, right, true, args, xsink), xsink);
   return *rv ? rv->getAsFloat() : 0;
}

AbstractQoreNode *NodeOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   if (!ref_rv)
      return 0;
   return op_func(left, right, xsink);
}

bool NodeOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(op_func(left, right, xsink), xsink);
   return *rv ? rv->getAsBool() : false;
}

int64 NodeOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(op_func(left, right, xsink), xsink);
   return *rv ? rv->getAsBigInt() : 0;
}

double NodeOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(op_func(left, right, xsink), xsink);
   return *rv ? rv->getAsFloat() : 0;
}

AbstractQoreNode *EffectNoEvalOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   return op_func(left, right, ref_rv, xsink);
}

bool EffectNoEvalOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(op_func(left, right, true, xsink), xsink);
   return *rv ? rv->getAsBool() : false;
}

int64 EffectNoEvalOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(op_func(left, right, true, xsink), xsink);
   return *rv ? rv->getAsBigInt() : 0;
}

double EffectNoEvalOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(op_func(left, right, true, xsink), xsink);
   return *rv ? rv->getAsFloat() : 0;
}

AbstractQoreNode *HashStringOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const {
   assert(left && left->getType() == NT_HASH);
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return 0;

   QoreStringValueHelper r(right);
   return op_func(reinterpret_cast<const QoreHashNode *>(left), *r, xsink);
}

bool HashStringOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   assert(left && left->getType() == NT_HASH);
   // this operator can never return a value in a boolean context and cannot have a side-effect
   return false;
}

int64 HashStringOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   assert(left && left->getType() == NT_HASH);
   // this operator can never return a value in an integer context and cannot have a side-effect
   return 0;
}

double HashStringOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   assert(left && left->getType() == NT_HASH);
   // this operator can never return a value in a floating-point context and cannot have a side-effect
   return 0.0;
}

AbstractQoreNode *HashListOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const {
   assert(left && left->getType() == NT_HASH);
   assert(right && right->getType() == NT_LIST);
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return 0;

   return op_func(reinterpret_cast<const QoreHashNode *>(left), reinterpret_cast<const QoreListNode *>(right), xsink);
}

bool HashListOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   assert(left && left->getType() == NT_HASH);
   assert(right && right->getType() == NT_LIST);

   // this operator can never return a value in a boolean context and cannot have a side-effect
   return false;
}

int64 HashListOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   assert(left && left->getType() == NT_HASH);
   assert(right && right->getType() == NT_LIST);

   // this operator can never return a value in an integer context and cannot have a side-effect
   return 0;
}

double HashListOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   assert(left && left->getType() == NT_HASH);
   assert(right && right->getType() == NT_LIST);

   // this operator can never return a value in a floating-point context and cannot have a side-effect
   return 0.0;
}

AbstractQoreNode *NoConvertOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const {
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return 0;

   return op_func(left, right);
}

bool NoConvertOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   // this operator can never return a value in a boolean context and cannot have a side-effect
   return false;
}

int64 NoConvertOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   // this operator can never return a value in an integer context and cannot have a side-effect
   return 0;
}

double NoConvertOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   // this operator can never return a value in a floating-point context and cannot have a side-effect
   return 0.0;
}

AbstractQoreNode *EffectBoolOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const {
   bool b = op_func(left, right, xsink);
   return *xsink ? 0 : get_bool_node(b);
}

bool EffectBoolOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   return op_func(left, right, xsink);
}

int64 EffectBoolOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   return (int64)op_func(left, right, xsink);
}

double EffectBoolOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   return (double)op_func(left, right, xsink);
}

AbstractQoreNode *SimpleBoolOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const {
   if (!ref_rv)
      return 0;

   bool b = op_func(left, right);
   return get_bool_node(b);
}

bool SimpleBoolOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return op_func(left, right);
}

int64 SimpleBoolOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   return (int64)op_func(left, right);
}

double SimpleBoolOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   return (double)op_func(left, right);
}

AbstractQoreNode *VarRefOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const {
   assert(left);
   return op_func(left, ref_rv, xsink);
}

bool VarRefOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   assert(left);
   ReferenceHolder<AbstractQoreNode> rv(op_func(left, true, xsink), xsink);
   return *rv ? rv->getAsBool() : false;
}

int64 VarRefOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const {
   assert(left);
   ReferenceHolder<AbstractQoreNode> rv(op_func(left, true, xsink), xsink);
   return *rv ? rv->getAsBigInt() : 0;
}

double VarRefOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(left);
   ReferenceHolder<AbstractQoreNode> rv(op_func(left, true, xsink), xsink);
   return *rv ? rv->getAsFloat() : 0.0;
}

AbstractQoreNode *StringStringStringOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return 0;

   QoreStringValueHelper l(left);
   QoreStringValueHelper r(right);
   return op_func(*l, *r, xsink);
}

bool StringStringStringOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   QoreStringValueHelper r(right);

   QoreStringNodeHolder rv(op_func(*l, *r, xsink));
   return *rv ? rv->getAsBool() : false;
}

int64 StringStringStringOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   QoreStringValueHelper r(right);

   QoreStringNodeHolder rv(op_func(*l, *r, xsink));
   return *rv ? rv->getAsBigInt() : 0;
}

double StringStringStringOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   QoreStringValueHelper r(right);

   QoreStringNodeHolder rv(op_func(*l, *r, xsink));
   return *rv ? rv->getAsFloat() : 0.0;
}

AbstractQoreNode *ListStringRegexOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   assert(right && right->getType() == NT_REGEX);

   // conditionally evaluate left-hand node only
   QoreNodeEvalOptionalRefHolder le(left, xsink);
   if (*xsink) return 0;
   
   // return immediately if the return value is ignored, this statement will have no effect and there can be no (other) side-effects
   if (!ref_rv) return 0;

   QoreStringValueHelper l(*le);
   return op_func(*l, reinterpret_cast<const QoreRegexNode *>(right), xsink);
}

bool ListStringRegexOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(right && right->getType() == NT_REGEX);
   // this operator can never return a value in this context and cannot have a side-effect
   return false;
}

int64 ListStringRegexOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(right && right->getType() == NT_REGEX);
   // this operator can never return a value in this context and cannot have a side-effect
   return 0;
}

double ListStringRegexOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(right && right->getType() == NT_REGEX);
   // this operator can never return a value in this context and cannot have a side-effect
   return 0.0;
}

AbstractQoreNode *BoolOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL))
   {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1) {
      bool rv = op_func(left, 0, xsink);
      if (!ref_rv || *xsink)
	 return 0;
      return get_bool_node(rv);
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL))
   {
      r = get_node_type(right, rtype);
      right = *r;
   }

   bool rv = op_func(left, right, xsink);
   if (!ref_rv || *xsink)
      return 0;
   return get_bool_node(rv);
}

bool BoolOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL))
   {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1) {
      return op_func(left, right, xsink);
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL))
   {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return op_func(left, right, xsink);
}

int64 BoolOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL))
   {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1) {
      return (int64)op_func(left, right, xsink);
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL))
   {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return (int64)op_func(left, right, xsink);
}

double BoolOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL))
   {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1) {
      return (double)op_func(left, right, xsink);
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL))
   {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return (double)op_func(left, right, xsink);
}

AbstractQoreNode *NoConvertBoolOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   if (args == 1) {
      bool rv = op_func(left, 0, xsink);
      if (!ref_rv || *xsink)
	 return 0;
      return get_bool_node(rv);
   }

   bool rv = op_func(left, right, xsink);
   if (!ref_rv || *xsink)
      return 0;
   return get_bool_node(rv);
}

bool NoConvertBoolOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   if (args == 1)
      return op_func(left, right, xsink);

   return op_func(left, right, xsink);
}

int64 NoConvertBoolOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   if (args == 1)
      return (int64)op_func(left, right, xsink);

   return (int64)op_func(left, right, xsink);
}

double NoConvertBoolOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   if (args == 1)
      return (double)op_func(left, right, xsink);

   return (double)op_func(left, right, xsink);
}

AbstractQoreNode *BoolStrStrOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   
   bool rv;
   if (args == 1) {
      rv = op_func(*l, 0, xsink);
   }
   else {
      QoreStringValueHelper r(right);

      rv = op_func(*l, *r, xsink);
   }
   if (!ref_rv || *xsink)
      return 0;
   return get_bool_node(rv);
}

bool BoolStrStrOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   
   if (args == 1)
      return op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return op_func(*l, *r, xsink);
}

int64 BoolStrStrOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   
   if (args == 1)
      return (int64)op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return (int64)op_func(*l, *r, xsink);
}

double BoolStrStrOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);   
   if (args == 1)
      return (double)op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return (double)op_func(*l, *r, xsink);
}

AbstractQoreNode *BoolDateOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side-effects
   if (!ref_rv)
      return 0;

   DateTimeNodeValueHelper l(left);
   DateTimeNodeValueHelper r(right);
   bool b = op_func(*l, *r);
   return get_bool_node(b);
}

bool BoolDateOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   DateTimeNodeValueHelper l(left);
   DateTimeNodeValueHelper r(right);
   return op_func(*l, *r);
}

int64 BoolDateOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   DateTimeNodeValueHelper l(left);
   DateTimeNodeValueHelper r(right);
   return (int64)op_func(*l, *r);
}

double BoolDateOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   DateTimeNodeValueHelper l(left);   
   DateTimeNodeValueHelper r(right);
   return (double)op_func(*l, *r);
}

AbstractQoreNode *DateOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions (date addition and subtraction) can have no side-effects
   if (!ref_rv)
      return 0;

   DateTimeNodeValueHelper l(left);
   DateTimeNodeValueHelper r(right);

   return op_func(*l, *r);
}

bool DateOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   DateTimeNodeValueHelper l(left);
   DateTimeNodeValueHelper r(right);

   SimpleRefHolder<DateTimeNode> date(op_func(*l, *r));
   return date->getEpochSeconds() ? true : false;
}

int64 DateOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   DateTimeNodeValueHelper l(left);
   DateTimeNodeValueHelper r(right);

   SimpleRefHolder<DateTimeNode> date(op_func(*l, *r));
   return date->getEpochSeconds();
}

double DateOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   DateTimeNodeValueHelper l(left);
   DateTimeNodeValueHelper r(right);

   SimpleRefHolder<DateTimeNode> date(op_func(*l, *r));
   return (double)date->getEpochSeconds();
}

AbstractQoreNode *BoolIntOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side effects
   if (!ref_rv)
      return 0;

   bool b = op_func(left->getAsBigInt(), right->getAsBigInt());
   return get_bool_node(b);
}

bool BoolIntOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return op_func(left->getAsBigInt(), right->getAsBigInt());
}

int64 BoolIntOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (int64)op_func(left->getAsBigInt(), right->getAsBigInt());
}

double BoolIntOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (double)op_func(left->getAsBigInt(), right->getAsBigInt());
}

AbstractQoreNode *IntIntOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side effects
   if (!ref_rv)
      return 0;

   int64 i = op_func(left->getAsBigInt(), right->getAsBigInt());
   return new QoreBigIntNode(i);
}

bool IntIntOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (bool)op_func(left->getAsBigInt(), right->getAsBigInt());
}

int64 IntIntOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return op_func(left->getAsBigInt(), right->getAsBigInt());
}

double IntIntOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (double)op_func(left->getAsBigInt(), right->getAsBigInt());
}

AbstractQoreNode *DivideIntOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side effects
   if (!ref_rv)
      return 0;

   int64 i = op_func(left->getAsBigInt(), right->getAsBigInt(), xsink);
   return *xsink ? 0 : new QoreBigIntNode(i);
}

bool DivideIntOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (bool)op_func(left->getAsBigInt(), right->getAsBigInt(), xsink);
}

int64 DivideIntOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return op_func(left->getAsBigInt(), right->getAsBigInt(), xsink);
}

double DivideIntOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (double)op_func(left->getAsBigInt(), right->getAsBigInt(), xsink);
}

AbstractQoreNode *UnaryMinusIntOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side effects
   if (!ref_rv)
      return 0;

   int64 i = -left->getAsBigInt();
   return new QoreBigIntNode(i);
}

bool UnaryMinusIntOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (bool)-left->getAsBigInt();
}

int64 UnaryMinusIntOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return -left->getAsBigInt();
}

double UnaryMinusIntOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (double)-left->getAsBigInt();
}

AbstractQoreNode *BoolFloatOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side effects
   if (!ref_rv)
      return 0;

   bool b = op_func(left->getAsFloat(), right->getAsFloat());
   return get_bool_node(b);
}

bool BoolFloatOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return op_func(left->getAsFloat(), right->getAsFloat());
}

int64 BoolFloatOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (int64)op_func(left->getAsFloat(), right->getAsFloat());
}

double BoolFloatOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (double)op_func(left->getAsFloat(), right->getAsFloat());
}

AbstractQoreNode *FloatFloatOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side effects
   if (!ref_rv)
      return 0;

   double f = op_func(left->getAsFloat(), right->getAsFloat());
   return new QoreFloatNode(f);
}

bool FloatFloatOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (bool)op_func(left->getAsFloat(), right->getAsFloat());
}

int64 FloatFloatOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (int64)op_func(left->getAsFloat(), right->getAsFloat());
}

double FloatFloatOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return op_func(left->getAsFloat(), right->getAsFloat());
}

AbstractQoreNode *DivideFloatOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side effects
   if (!ref_rv)
      return 0;

   double f = op_func(left->getAsFloat(), right->getAsFloat(), xsink);
   return *xsink ? 0 : new QoreFloatNode(f);
}

bool DivideFloatOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (bool)op_func(left->getAsFloat(), right->getAsFloat(), xsink);
}

int64 DivideFloatOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (int64)op_func(left->getAsFloat(), right->getAsFloat(), xsink);
}

double DivideFloatOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return op_func(left->getAsFloat(), right->getAsFloat(), xsink);
}

AbstractQoreNode *UnaryMinusFloatOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side effects
   if (!ref_rv)
      return 0;

   double f = -left->getAsFloat();
   return new QoreFloatNode(f);
}

bool UnaryMinusFloatOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (bool)-left->getAsFloat();
}

int64 UnaryMinusFloatOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (int64)-left->getAsFloat();
}

double UnaryMinusFloatOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return -left->getAsFloat();
}

AbstractQoreNode *CompareFloatOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side effects
   if (!ref_rv)
      return 0;

   int64 i = op_func(left->getAsFloat(), right->getAsFloat());
   return new QoreBigIntNode(i);
}

bool CompareFloatOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (bool)op_func(left->getAsFloat(), right->getAsFloat());
}

int64 CompareFloatOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return op_func(left->getAsFloat(), right->getAsFloat());
}

double CompareFloatOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (double)op_func(left->getAsFloat(), right->getAsFloat());
}

AbstractQoreNode *BoolNotOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side effects
   if (!ref_rv)
      return 0;

   return get_bool_node(!left->getAsBool());
}

bool BoolNotOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return !left->getAsBool();
}

int64 BoolNotOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (int64)(!left->getAsBool());
}

double BoolNotOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (double)(!left->getAsBool());
}

AbstractQoreNode *IntegerNotOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side effects
   if (!ref_rv)
      return 0;

   return new QoreBigIntNode(~left->getAsBigInt());
}

bool IntegerNotOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (bool)~left->getAsBigInt();
}

int64 IntegerNotOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return ~left->getAsBigInt();
}

double IntegerNotOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (double)(~left->getAsBigInt());
}

AbstractQoreNode *CompareDateOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // this operator can have no side effects
   if (!ref_rv)
      return 0;

   DateTimeValueHelper l(left);   
   DateTimeValueHelper r(right);   

   int64 i = DateTime::compareDates(*l, *r);
   return new QoreBigIntNode(i);
}

bool CompareDateOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   DateTimeValueHelper l(left);   
   DateTimeValueHelper r(right);   

   return (bool)DateTime::compareDates(*l, *r);
}

int64 CompareDateOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   DateTimeValueHelper l(left);   
   DateTimeValueHelper r(right);   

   return DateTime::compareDates(*l, *r);
}

double CompareDateOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   DateTimeValueHelper l(left);   
   DateTimeValueHelper r(right);   

   return (double)DateTime::compareDates(*l, *r);
}

AbstractQoreNode *LogicOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // these functions can have no side effects
   if (!ref_rv)
      return 0;

   bool b = op_func(left->getAsBool(), right->getAsBool());
   return get_bool_node(b);
}

bool LogicOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return op_func(left->getAsBool(), right->getAsBool());
}

int64 LogicOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (int64)op_func(left->getAsBool(), right->getAsBool());
}

double LogicOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   return (double)op_func(left->getAsBool(), right->getAsBool());
}

AbstractQoreNode *BoolStrRegexOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   assert(right && right->getType() == NT_REGEX);

   // conditionally evaluate left-hand node only
   QoreNodeEvalOptionalRefHolder le(left, xsink);
   if (*xsink) return 0;

   // return immediately if the return value is ignored, this statement will have no effect and there can be no (other) side-effects
   if (!ref_rv) return 0;

   QoreStringValueHelper l(*le);
   bool rv = op_func(*l, reinterpret_cast<const QoreRegexNode *>(right), xsink);
   return *xsink ? 0 : get_bool_node(rv);
}

bool BoolStrRegexOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(right && right->getType() == NT_REGEX);

   // conditionally evaluate left-hand node only
   QoreNodeEvalOptionalRefHolder le(left, xsink);
   if (*xsink) return 0;

   QoreStringValueHelper l(*le);

   return op_func(*l, reinterpret_cast<const QoreRegexNode *>(right), xsink);
}

int64 BoolStrRegexOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(right && right->getType() == NT_REGEX);

   // conditionally evaluate left-hand node only
   QoreNodeEvalOptionalRefHolder le(left, xsink);
   if (*xsink) return 0;

   QoreStringValueHelper l(*le);

   return (int64)op_func(*l, reinterpret_cast<const QoreRegexNode *>(right), xsink);
}

double BoolStrRegexOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(right && right->getType() == NT_REGEX);

   // conditionally evaluate left-hand node only
   QoreNodeEvalOptionalRefHolder le(left, xsink);
   if (*xsink) return 0;

   QoreStringValueHelper l(*le);

   return (double)op_func(*l, reinterpret_cast<const QoreRegexNode *>(right), xsink);
}

AbstractQoreNode *BigIntStrStrOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   
   int64 rv;
   if (args == 1) {
      rv = op_func(*l, 0, xsink);
   }
   else {
      QoreStringValueHelper r(right);

      rv = op_func(*l, *r, xsink);
   }
   if (!ref_rv || *xsink)
      return 0;
   return new QoreBigIntNode(rv);
}

bool BigIntStrStrOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   
   if (args == 1)
      return (bool)op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return (bool)op_func(*l, *r, xsink);
}

int64 BigIntStrStrOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   
   if (args == 1)
      return op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return op_func(*l, *r, xsink);
}

double BigIntStrStrOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);   
   if (args == 1)
      return (double)op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return (double)op_func(*l, *r, xsink);
}

AbstractQoreNode *BigIntOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL))
   {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1) {
      int64 rv = op_func(left, right, xsink);
      if (!ref_rv || xsink->isException())
	 return 0;
      return new QoreBigIntNode(rv);
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL))
   {
      r = get_node_type(right, rtype);
      right = *r;
   }

   int64 rv = op_func(left, right, xsink);
   if (!ref_rv || xsink->isException())
      return 0;
   return new QoreBigIntNode(rv);
}

bool BigIntOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL))
   {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1) {
      return (bool)op_func(left, right, xsink);
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL))
   {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return (bool)op_func(left, right, xsink);
}

int64 BigIntOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL))
   {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1) {
      return op_func(left, right, xsink);
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL)) {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return op_func(left, right, xsink);
}

double BigIntOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL))
   {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1) {
      return (double)op_func(left, right, xsink);
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL)) {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return (double)op_func(left, right, xsink);
}

AbstractQoreNode *FloatOperatorFunction::eval(const AbstractQoreNode *left, const AbstractQoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL))
   {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1) {
      double rv = op_func(left, right, xsink);
      if (!ref_rv || xsink->isException())
	 return 0;
      return new QoreFloatNode(rv);
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL)) {
      r = get_node_type(right, rtype);
      right = *r;
   }

   double rv = op_func(left, right, xsink);
   if (!ref_rv || xsink->isException())
      return 0;
   return new QoreFloatNode(rv);
}

bool FloatOperatorFunction::bool_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL))
   {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1) {
      return (bool)op_func(left, right, xsink);
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL))
   {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return (bool)op_func(left, right, xsink);
}

int64 FloatOperatorFunction::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL))
   {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1) {
      return (int64)op_func(left, right, xsink);
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL))
   {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return (int64)op_func(left, right, xsink);
}

double FloatOperatorFunction::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->getType() != ltype) && (ltype != NT_ALL))
   {
      l = get_node_type(left, ltype);
      left = *l;
   }

   if (args == 1) {
      return op_func(left, right, xsink);
   }

   ReferenceHolder<AbstractQoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->getType() != rtype) && (rtype != NT_ALL))
   {
      r = get_node_type(right, rtype);
      right = *r;
   }

   return op_func(left, right, xsink);
}

Operator::Operator(int arg, const char *n, const char *desc, int n_evalArgs, bool n_effect, bool n_lvalue)
{
   args = arg;
   name = n;
   description = desc;
   evalArgs = n_evalArgs;
   opMatrix = 0;
   effect = n_effect;
   lvalue = n_lvalue;
}

Operator::~Operator()
{
   // erase all functions
   for (unsigned i = 0, size = functions.size(); i < size; i++)
      delete functions[i];
   if (opMatrix)
      delete [] opMatrix;
}

bool Operator::hasEffect() const
{ 
   return effect; 
}

bool Operator::needsLValue() const
{ 
   return lvalue;
}

const char *Operator::getName() const
{
   return name;
}

const char *Operator::getDescription() const
{
   return description;
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

// if there is no exact match, the first partial match counts as a match
// static method
int Operator::match(qore_type_t ntype, qore_type_t rtype)
{
   // if any type is OK, or an exact match
   if (rtype == NT_ALL || ntype == rtype || (rtype == NT_VARREF && ntype == NT_SELF_VARREF))
      return 1;
   else // otherwise fail
      return 0;
}

int Operator::get_function(const QoreNodeEvalOptionalRefHolder &nleft, ExceptionSink *xsink) const {
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

int Operator::get_function(const QoreNodeEvalOptionalRefHolder &nleft, const QoreNodeEvalOptionalRefHolder &nright, ExceptionSink *xsink) const {
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
AbstractQoreNode *Operator::eval(const AbstractQoreNode *left_side, const AbstractQoreNode *right_side, bool ref_rv, ExceptionSink *xsink) const {
   printd(5, "evaluating operator %s (0x%08p 0x%08p)\n", description, left_side, right_side);
   if (evalArgs) {
      QoreNodeEvalOptionalRefHolder nleft(left_side, xsink);
      if (*xsink)
	 return 0;
      if (!nleft)
	 nleft.assign(false, &Nothing);

      int t;

      if (args == 1)
      {
	 if ((t = get_function(nleft, xsink)) == -1)
	    return 0;

	 return functions[t]->eval(*nleft, 0, ref_rv, 1, xsink);
      }

      QoreNodeEvalOptionalRefHolder nright(right_side, xsink);
      if (*xsink)
	 return 0;
      if (!nright)
	 nright.assign(false, &Nothing);
      
      // find operator function
      if ((t = get_function(nleft, nright, xsink)) == -1)
	 return 0;
      
      return functions[t]->eval(*nleft, *nright, ref_rv, 2, xsink);
   }

   // in this case there will only be one entry (0)
   printd(5, "Operator::eval() evaluating function 0\n");
   return functions[0]->eval(left_side, right_side, ref_rv, args, xsink);
}

// Operator::bool_eval(): return value
// there are 3 main cases which have been split into 3 sections as a speed optimization
// 1: evalArgs 1 argument
// 2: evalArgs 2 arguments
// 3: pass-through all arguments
bool Operator::bool_eval(const AbstractQoreNode *left_side, const AbstractQoreNode *right_side, ExceptionSink *xsink) const
{
   printd(5, "evaluating operator %s (0x%08p 0x%08p)\n", description, left_side, right_side);
   if (evalArgs)
   {
      QoreNodeEvalOptionalRefHolder nleft(left_side, xsink);
      if (*xsink)
	 return 0;
      if (!nleft)
	 nleft.assign(false, &Nothing);
      
      int t;
      if (args == 1)
      {
	 if ((t = get_function(nleft, xsink)) == -1)
	    return 0;

	 return functions[t]->bool_eval(*nleft, 0, 1, xsink);
      }

      QoreNodeEvalOptionalRefHolder nright(right_side, xsink);
      if (*xsink)
	 return 0;
      if (!nright)
	 nright.assign(false, &Nothing);
      
      // find operator function
      if ((t = get_function(nleft, nright, xsink)) == -1)
	 return 0;
      
      return functions[t]->bool_eval(*nleft, *nright, 2, xsink);
   }

   // in this case there will only be one entry (0)
   printd(5, "Operator::bool_eval() evaluating function 0\n");
   return functions[0]->bool_eval(left_side, right_side, args, xsink);
}

// Operator::bigint_eval(): return value
// there are 3 main cases which have been split into 3 sections as a speed optimization
// 1: evalArgs 1 argument
// 2: evalArgs 2 arguments
// 3: pass-through all arguments
int64 Operator::bigint_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) const
{
   printd(5, "evaluating operator %s (0x%08p 0x%08p)\n", description, left, right);
   if (evalArgs)
   {
      QoreNodeEvalOptionalRefHolder nleft(left, xsink);
      if (*xsink)
	 return 0;
      if (!nleft)
	 nleft.assign(false, &Nothing);

      int t;

      if (args == 1)
      {
	 if ((t = get_function(nleft, xsink)) == -1)
	    return 0;

	 return functions[t]->bigint_eval(*nleft, 0, 1, xsink);
      }

      QoreNodeEvalOptionalRefHolder nright(right, xsink);
      if (*xsink)
	 return 0;
      if (!nright)
	 nright.assign(false, &Nothing);
	 
      // find operator function
      if ((t = get_function(nleft, nright, xsink)) == -1)
	 return 0;
      
      return functions[t]->bigint_eval(*nleft, *nright, 2, xsink);
   }

   // in this case there will only be one entry (0)
   printd(5, "Operator::bigint_eval() evaluating function 0\n");
   return functions[0]->bigint_eval(left, right, args, xsink);
}

// Operator::float_eval(): return value
// there are 3 main cases which have been split into 3 sections as a speed optimization
// 1: evalArgs 1 argument
// 2: evalArgs 2 arguments
// 3: pass-through all arguments
double Operator::float_eval(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink) const {
   printd(5, "evaluating operator %s (0x%08p 0x%08p)\n", description, left, right);
   if (evalArgs)
   {
      QoreNodeEvalOptionalRefHolder nleft(left, xsink);
      if (*xsink)
	 return 0;
      if (!nleft)
	 nleft.assign(false, &Nothing);

      int t;

      if (args == 1)
      {
	 if ((t = get_function(nleft, xsink)) == -1)
	    return 0;

	 return functions[t]->float_eval(*nleft, 0, 1, xsink);
      }

      QoreNodeEvalOptionalRefHolder nright(right, xsink);
      if (*xsink)
	 return 0;
      if (!nright)
	 nright.assign(false, &Nothing);
	 
      // find operator function
      if ((t = get_function(nleft, nright, xsink)) == -1)
	 return 0;

      return functions[t]->float_eval(*nleft, *nright, 2, xsink);
   }

   // in this case there will only be one entry (0)
   printd(5, "Operator::float_eval() evaluating function 0\n");
   return functions[0]->float_eval(left, right, args, xsink);
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

void Operator::addFunction(qore_type_t lt, qore_type_t rt, op_func_t f)
{
   functions.push_back(new OperatorFunction(lt, rt, f));
}

void Operator::addFunction(qore_type_t lt, qore_type_t rt, op_bool_func_t f)
{
   functions.push_back(new BoolOperatorFunction(lt, rt, f));
}

void Operator::addFunction(qore_type_t lt, qore_type_t rt, op_bigint_func_t f)
{
   functions.push_back(new BigIntOperatorFunction(lt, rt, f));
}

void Operator::addFunction(qore_type_t lt, qore_type_t rt, op_float_func_t f)
{
   functions.push_back(new FloatOperatorFunction(lt, rt, f));
}

OperatorList::OperatorList()
{
}

OperatorList::~OperatorList()
{
   oplist_t::iterator i;
   while ((i = begin()) != end())
   {
      delete (*i);
      erase(i);
   }
}

class Operator *OperatorList::add(class Operator *o)
{
   push_back(o);
   return o;
}

// registers the system operators and system operator functions
void OperatorList::init() {
   QORE_TRACE("OperatorList::init()");
   
   OP_LOG_AND = add(new Operator(2, "&&", "logical-and", 0, false));
   OP_LOG_AND->addEffectFunction(op_log_and);

   OP_LOG_OR = add(new Operator(2, "||", "logical-or", 0, false));
   OP_LOG_OR->addEffectFunction(op_log_or);
   
   OP_LOG_LT = add(new Operator(2, "<", "less-than", 1, false));
   OP_LOG_LT->addFunction(op_log_lt_float);
   OP_LOG_LT->addFunction(op_log_lt_bigint);
   OP_LOG_LT->addFunction(op_log_lt_string);
   OP_LOG_LT->addFunction(op_log_lt_date);
   
   OP_LOG_GT = add(new Operator(2, ">", "greater-than", 1, false));
   OP_LOG_GT->addFunction(op_log_gt_float);
   OP_LOG_GT->addFunction(op_log_gt_bigint);
   OP_LOG_GT->addFunction(op_log_gt_string);
   OP_LOG_GT->addFunction(op_log_gt_date);

   OP_LOG_EQ = add(new Operator(2, "==", "logical-equals", 1, false));
   OP_LOG_EQ->addFunction(op_log_eq_string);
   OP_LOG_EQ->addFunction(op_log_eq_float);
   OP_LOG_EQ->addFunction(op_log_eq_bigint);
   OP_LOG_EQ->addFunction(op_log_eq_boolean);
   OP_LOG_EQ->addFunction(op_log_eq_date);
   OP_LOG_EQ->addNoConvertFunction(NT_ALL, NT_ALL, op_log_eq_all);
   /*
   OP_LOG_EQ->addNoConvertFunction(NT_LIST,    NT_ALL,     op_log_eq_list);
   OP_LOG_EQ->addNoConvertFunction(NT_ALL,     NT_LIST,    op_log_eq_list);
   OP_LOG_EQ->addNoConvertFunction(NT_HASH,    NT_ALL,     op_log_eq_hash);
   OP_LOG_EQ->addNoConvertFunction(NT_ALL,     NT_HASH,    op_log_eq_hash);
   OP_LOG_EQ->addNoConvertFunction(NT_OBJECT,  NT_ALL,     op_log_eq_object);
   OP_LOG_EQ->addNoConvertFunction(NT_ALL,     NT_OBJECT,  op_log_eq_object);
   OP_LOG_EQ->addFunction(NT_NULL,    NT_ALL,     op_log_eq_null);
   OP_LOG_EQ->addFunction(NT_ALL,     NT_NULL,    op_log_eq_null);
   OP_LOG_EQ->addFunction(NT_NOTHING, NT_NOTHING, op_log_eq_nothing);
   OP_LOG_EQ->addFunction(NT_BINARY,  NT_BINARY,  op_log_eq_binary);
   */

   OP_LOG_NE = add(new Operator(2, "!=", "not-equals", 1, false));
   OP_LOG_NE->addFunction(op_log_ne_string);
   OP_LOG_NE->addFunction(op_log_ne_float);
   OP_LOG_NE->addFunction(op_log_ne_bigint);
   OP_LOG_NE->addFunction(op_log_ne_boolean);
   OP_LOG_NE->addFunction(op_log_ne_date);
   OP_LOG_NE->addNoConvertFunction(NT_ALL, NT_ALL, op_log_ne_all);
   /*
   OP_LOG_NE->addNoConvertFunction(NT_LIST,    NT_ALL,     op_log_ne_list);
   OP_LOG_NE->addNoConvertFunction(NT_ALL,     NT_LIST,    op_log_ne_list);
   OP_LOG_NE->addNoConvertFunction(NT_HASH,    NT_ALL,     op_log_ne_hash);
   OP_LOG_NE->addNoConvertFunction(NT_ALL,     NT_HASH,    op_log_ne_hash);
   OP_LOG_NE->addNoConvertFunction(NT_OBJECT,  NT_ALL,     op_log_ne_object);
   OP_LOG_NE->addNoConvertFunction(NT_ALL,     NT_OBJECT,  op_log_ne_object);
   OP_LOG_NE->addFunction(NT_NULL,    NT_ALL,     op_log_ne_null);
   OP_LOG_NE->addFunction(NT_ALL,     NT_NULL,    op_log_ne_null);
   OP_LOG_NE->addFunction(NT_NOTHING, NT_NOTHING, op_log_ne_nothing);
   OP_LOG_NE->addFunction(NT_BINARY,  NT_BINARY,  op_log_ne_binary);
   */
 
   OP_LOG_LE = add(new Operator(2, "<=", "less-than-or-equals", 1, false));
   OP_LOG_LE->addFunction(op_log_le_float);
   OP_LOG_LE->addFunction(op_log_le_bigint);
   OP_LOG_LE->addFunction(op_log_le_string);
   OP_LOG_LE->addFunction(op_log_le_date);

   OP_LOG_GE = add(new Operator(2, ">=", "greater-than-or-equals", 1, false));
   OP_LOG_GE->addFunction(op_log_ge_float);
   OP_LOG_GE->addFunction(op_log_ge_bigint);
   OP_LOG_GE->addFunction(op_log_ge_string);
   OP_LOG_GE->addFunction(op_log_ge_date);

   OP_ABSOLUTE_EQ = add(new Operator(2, "===", "absolute logical-equals", 0, false));
   OP_ABSOLUTE_EQ->addFunction(NT_ALL, NT_ALL, op_absolute_log_eq);
   
   OP_ABSOLUTE_NE = add(new Operator(2, "!==", "absolute logical-not-equals", 0, false));
   OP_ABSOLUTE_NE->addFunction(NT_ALL, NT_ALL, op_absolute_log_neq);
   
   OP_REGEX_MATCH = add(new Operator(2, "=~", "regular expression match", 0, false));
   OP_REGEX_MATCH->addFunction(op_regex_match);
   
   OP_REGEX_NMATCH = add(new Operator(2, "!~", "regular expression negative match", 0, false));
   OP_REGEX_NMATCH->addFunction(op_regex_nmatch);

   OP_EXISTS = add(new Operator(1, "exists", "exists", 0, false));
   OP_EXISTS->addFunction(NT_ALL, NT_NONE, op_exists);
   
   OP_INSTANCEOF = add(new Operator(2, "instanceof", "instanceof", 0, false));
   OP_INSTANCEOF->addFunction(NT_ALL, NT_CLASSREF, op_instanceof);
   
   OP_NOT = add(new Operator(1, "!", "logical-not", 1, false));
   OP_NOT->addBoolNotFunction();
      
   // bigint operators
   OP_LOG_CMP = add(new Operator(2, "<=>", "logical-comparison", 1, false));
   OP_LOG_CMP->addFunction(op_cmp_string);
   OP_LOG_CMP->addFunction(op_cmp_double);
   OP_LOG_CMP->addFunction(op_cmp_bigint);
   OP_LOG_CMP->addCompareDateFunction();

   OP_ELEMENTS = add(new Operator(1, "elements", "number of elements", 0, false));
   OP_ELEMENTS->addFunction(NT_ALL, NT_NONE, op_elements);

   OP_MODULA = add(new Operator(2, "%", "modula", 1, false));
   OP_MODULA->addFunction(op_modula_int);

   // non-boolean operators
   OP_ASSIGNMENT = add(new Operator(2, "=", "assignment", 0, true, true));
   OP_ASSIGNMENT->addFunction(NT_ALL, NT_ALL, op_assignment);
   
   OP_LIST_ASSIGNMENT = add(new Operator(2, "(list) =", "list assignment", 0, true, true));
   OP_LIST_ASSIGNMENT->addFunction(NT_ALL, NT_ALL, op_list_assignment);
   
   OP_BIN_AND = add(new Operator(2, "&", "binary-and", 1, false));
   OP_BIN_AND->addFunction(op_bin_and_int);

   OP_BIN_OR = add(new Operator(2, "|", "binary-or", 1, false));
   OP_BIN_OR->addFunction(op_bin_or_int);

   OP_BIN_NOT = add(new Operator(1, "~", "binary-not", 1, false));
   OP_BIN_NOT->addIntegerNotFunction();

   OP_BIN_XOR = add(new Operator(2, "^", "binary-xor", 1, false));
   OP_BIN_XOR->addFunction(op_bin_xor_int);

   OP_MINUS = add(new Operator(2, "-", "minus", 1, false));
   OP_MINUS->addFunction(op_minus_date);
   OP_MINUS->addFunction(op_minus_float);
   OP_MINUS->addFunction(op_minus_bigint);
   OP_MINUS->addFunction(op_minus_hash_string);
   OP_MINUS->addFunction(op_minus_hash_list);
   OP_MINUS->addDefaultNothing();

   OP_PLUS = add(new Operator(2, "+", "plus", 1, false));
   OP_PLUS->addFunction(NT_LIST,    NT_LIST,   op_plus_list);
   OP_PLUS->addFunction(op_plus_string);
   OP_PLUS->addFunction(op_plus_date);
   OP_PLUS->addFunction(op_plus_float);
   OP_PLUS->addFunction(op_plus_bigint);
   OP_PLUS->addFunction(NT_HASH,    NT_HASH,   op_plus_hash_hash);
   OP_PLUS->addFunction(NT_HASH,    NT_OBJECT, op_plus_hash_object);
   OP_PLUS->addFunction(NT_OBJECT,  NT_HASH,   op_plus_object_hash);
   OP_PLUS->addFunction(NT_BINARY,  NT_BINARY, op_plus_binary_binary);
   OP_PLUS->addDefaultNothing();

   OP_MULT = add(new Operator(2, "*", "multiply", 1, false));
   OP_MULT->addFunction(op_multiply_float);
   OP_MULT->addFunction(op_multiply_bigint);

   OP_DIV = add(new Operator(2, "/", "divide", 1, false));
   OP_DIV->addFunction(op_divide_float);
   OP_DIV->addFunction(op_divide_bigint);

   OP_UNARY_MINUS = add(new Operator(1, "-", "unary-minus", 1, false));
   OP_UNARY_MINUS->addUnaryMinusFloatFunction();
   OP_UNARY_MINUS->addUnaryMinusIntFunction();

   OP_SHIFT_LEFT = add(new Operator(2, "<<", "shift-left", 1, false));
   OP_SHIFT_LEFT->addFunction(op_shift_left_int);

   OP_SHIFT_RIGHT = add(new Operator(2, ">>", "shift-right", 1, false));
   OP_SHIFT_RIGHT->addFunction(op_shift_right_int);

   OP_POST_INCREMENT = add(new Operator(1, "++", "post-increment", 0, true, true));
   OP_POST_INCREMENT->addFunction(op_post_inc);

   OP_POST_DECREMENT = add(new Operator(1, "--", "post-decrement", 0, true, true));
   OP_POST_DECREMENT->addFunction(op_post_dec);

   OP_PRE_INCREMENT = add(new Operator(1, "++", "pre-increment", 0, true, true));
   OP_PRE_INCREMENT->addFunction(op_pre_inc);

   OP_PRE_DECREMENT = add(new Operator(1, "--", "pre-decrement", 0, true, true));
   OP_PRE_DECREMENT->addFunction(op_pre_dec);

   OP_PLUS_EQUALS = add(new Operator(2, "+=", "plus-equals", 0, true, true));
   OP_PLUS_EQUALS->addFunction(op_plus_equals);

   OP_MINUS_EQUALS = add(new Operator(2, "-=", "minus-equals", 0, true, true));
   OP_MINUS_EQUALS->addFunction(op_minus_equals);

   OP_AND_EQUALS = add(new Operator(2, "&=", "and-equals", 0, true, true));
   OP_AND_EQUALS->addFunction(op_and_equals);

   OP_OR_EQUALS = add(new Operator(2, "|=", "or-equals", 0, true, true));
   OP_OR_EQUALS->addFunction(op_or_equals);

   OP_MODULA_EQUALS = add(new Operator(2, "%=", "modula-equals", 0, true, true));
   OP_MODULA_EQUALS->addFunction(op_modula_equals);

   OP_MULTIPLY_EQUALS = add(new Operator(2, "*=", "multiply-equals", 0, true, true));
   OP_MULTIPLY_EQUALS->addFunction(op_multiply_equals);

   OP_DIVIDE_EQUALS = add(new Operator(2, "/=", "divide-equals", 0, true, true));
   OP_DIVIDE_EQUALS->addFunction(op_divide_equals);

   OP_XOR_EQUALS = add(new Operator(2, "^=", "xor-equals", 0, true, true));
   OP_XOR_EQUALS->addFunction(op_xor_equals);

   OP_SHIFT_LEFT_EQUALS = add(new Operator(2, "<<=", "shift-left-equals", 0, true, true));
   OP_SHIFT_LEFT_EQUALS->addFunction(op_shift_left_equals);

   OP_SHIFT_RIGHT_EQUALS = add(new Operator(2, ">>=", "shift-right-equals", 0, true, true));
   OP_SHIFT_RIGHT_EQUALS->addFunction(op_shift_right_equals);

   OP_LIST_REF = add(new Operator(2, "[]", "list-reference", 0, false));
   OP_LIST_REF->addFunction(NT_ALL, NT_ALL, op_list_ref);

   OP_OBJECT_REF = add(new Operator(2, ".", "hash/object-reference", 0, false));
   OP_OBJECT_REF->addFunction(NT_ALL, NT_ALL, op_object_ref); 

   OP_KEYS = add(new Operator(1, "keys", "list of keys", 0, false));
   OP_KEYS->addFunction(NT_ALL, NT_NONE, op_keys);

   OP_QUESTION_MARK = add(new Operator(2, "question", "question-mark colon", 0, false));
   OP_QUESTION_MARK->addFunction(NT_ALL, NT_ALL, op_question_mark);

   OP_OBJECT_FUNC_REF = add(new Operator(2, ".", "object method call", 0, true, false));
   OP_OBJECT_FUNC_REF->addFunction(NT_ALL, NT_ALL, op_object_method_call);

   OP_NEW = add(new Operator(1, "new", "new object", 0, true, false));
   OP_NEW->addFunction(NT_ALL, NT_NONE, op_new_object);

   OP_SHIFT = add(new Operator(1, "shift", "shift from list", 0, true, true));
   OP_SHIFT->addFunction(op_shift);

   OP_POP = add(new Operator(1, "pop", "pop from list", 0, true, true));
   OP_POP->addFunction(op_pop);

   OP_PUSH = add(new Operator(2, "push", "push on list", 0, true, true));
   OP_PUSH->addFunction(op_push);

   OP_SPLICE = add(new Operator(2, "splice", "splice in list", 0, true, true));
   OP_SPLICE->addFunction(op_splice);

   OP_UNSHIFT = add(new Operator(2, "unshift", "unshift/insert to begnning of list", 0, true, true));
   OP_UNSHIFT->addFunction(op_unshift);

   OP_REGEX_SUBST = add(new Operator(2, "regex subst", "regular expression substitution", 0, true, true));
   OP_REGEX_SUBST->addFunction(NT_ALL, NT_REGEX_SUBST, op_regex_subst);

   OP_REGEX_TRANS = add(new Operator(2, "transliteration", "transliteration", 0, true, true));
   OP_REGEX_TRANS->addFunction(NT_ALL, NT_REGEX_TRANS, op_regex_trans);

   OP_REGEX_EXTRACT = add(new Operator(2, "regular expression subpattern extraction", "regular expression subpattern extraction", 0, false));
   OP_REGEX_EXTRACT->addFunction(op_regex_extract);

   OP_CHOMP = add(new Operator(1, "chomp", "chomp EOL marker from lvalue", 0, true, true));
   OP_CHOMP->addFunction(NT_ALL, NT_NONE, op_chomp);

   OP_TRIM = add(new Operator(1, "trim", "trim characters from an lvalue", 0, true, true));
   OP_TRIM->addFunction(NT_ALL, NT_NONE, op_trim);

   OP_MAP = add(new Operator(2, "map", "map call reference or closure to a list", 0, true, false));
   OP_MAP->addFunction(NT_ALL, NT_ALL, op_map);

   OP_MAP_SELECT = add(new Operator(2, "map with select", "map call reference or closure to a list with select code expression", 0, true, false));
   OP_MAP_SELECT->addFunction(NT_ALL, NT_ALL, op_map_select);

   OP_FOLDL = add(new Operator(2, "foldl", "left fold call reference or closure on a list", 0, true, false));
   OP_FOLDL->addFunction(NT_ALL, NT_ALL, op_foldl);

   OP_FOLDR = add(new Operator(2, "foldr", "right fold call reference or closure on a list", 0, true, false));
   OP_FOLDR->addFunction(NT_ALL, NT_ALL, op_foldr);

   OP_SELECT = add(new Operator(2, "select", "select elements from a list", 0, true, false));
   OP_SELECT->addFunction(NT_ALL, NT_ALL, op_select);

   // initialize all operators
   for (oplist_t::iterator i = begin(), e = end(); i != e; ++i)
      (*i)->init();


}
