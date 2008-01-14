/*
  Operator.cc

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

#include <qore/Qore.h>
#include <qore/intern/RegexSubst.h>
#include <qore/intern/RegexTrans.h>
#include <qore/intern/QoreRegex.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pcre.h>

DLLLOCAL class OperatorList oplist;

// the standard, system-default operator pointers
class Operator *OP_ASSIGNMENT, *OP_MODULA, 
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

// call to get a node with reference count 1 (copy on write)
static inline void ensure_unique(class QoreNode **v, class ExceptionSink *xsink)
{
   if (!(*v)->is_unique())
   {
      QoreNode *old = *v;
      (*v) = old->realCopy(xsink);
      old->deref(xsink);
   }
}

// call to get a unique douvle node
static inline void ensure_unique_float(class QoreNode **v, class ExceptionSink *xsink)
{
   if ((*v)->type != NT_FLOAT)
   {
      double f = (*v)->getAsFloat();
      (*v)->deref(xsink);
      (*v) = new QoreNode(f);
   }
   else
      ensure_unique(v, xsink);
}

// call to get a unique int64 node
static inline void ensure_unique_int(class QoreNode **v, class ExceptionSink *xsink)
{
   if ((*v)->type != NT_INT)
   {
      int64 i = (*v)->getAsBigInt();
      (*v)->deref(xsink);
      (*v) = new QoreNode(i);
   }
   else
      ensure_unique(v, xsink);
}

// operator functions for builtin types
static bool op_log_lt_bigint(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval < right->val.intval;
}

static bool op_log_gt_bigint(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   //printd(1, "op_log_gt_bigint() left=%lld right=%lld result=%d\n", left->val.intval, right->val.intval, (left->val.intval > right->val.intval));
   return left->val.intval > right->val.intval;
}

static bool op_log_eq_bigint(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   //printd(5, "op_log_eq_bigint() left=%lld right=%lld result=%d\n", left->val.intval, right->val.intval, (left->val.intval == right->val.intval));
   return left->val.intval == right->val.intval;
}

static bool op_log_eq_binary(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return !left->val.bin->compare(right->val.bin);
}

static bool op_log_ne_binary(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.bin->compare(right->val.bin);
}

static bool op_log_eq_boolean(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.boolval == right->val.boolval;
}

static bool op_log_ne_boolean(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.boolval != right->val.boolval;
}

static bool op_log_not_boolean(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return !left->val.boolval;
}

static bool op_log_ne_bigint(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval != right->val.intval;
}

static bool op_log_le_bigint(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval <= right->val.intval;
}

static bool op_log_ge_bigint(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval >= right->val.intval;
}

static bool op_log_eq_date(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.date_time->isEqual(right->val.date_time);
}

static bool op_log_gt_date(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return DateTime::compareDates(left->val.date_time, right->val.date_time) > 0;
}

static bool op_log_ge_date(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return DateTime::compareDates(left->val.date_time, right->val.date_time) >= 0;
}

static bool op_log_lt_date(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return DateTime::compareDates(left->val.date_time, right->val.date_time) < 0;
}

static bool op_log_le_date(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return DateTime::compareDates(left->val.date_time, right->val.date_time) <= 0;
}

static bool op_log_ne_date(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return !left->val.date_time->isEqual(right->val.date_time);
}

static bool op_log_lt_float(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.floatval < right->val.floatval;
}

static bool op_log_gt_float(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.floatval > right->val.floatval;
}

static bool op_log_eq_float(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.floatval == right->val.floatval;
}

static bool op_log_ne_float(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.floatval != right->val.floatval;
}

static bool op_log_le_float(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.floatval <= right->val.floatval;
}

static bool op_log_ge_float(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.floatval >= right->val.floatval;
}

static bool op_log_eq_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink)
{
   //   tracein("op_log_eq_string()");
   /*
    printd(5, "OLES() %08p %08p\n", left, right);
    printd(5, "OLES() %d %d %08p %08p\n", //\"%s\" == \"%s\"\n", 
	   left->type, right->type, left->val.c_str, right->val.c_str);
    */
   //   traceout("op_log_eq_string()");
   return !left->compareSoft(right, xsink);
}

static bool op_log_gt_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink)
{
   return left->compare(right) > 0;
}

static bool op_log_ge_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink)
{
   return right->compare(left) >= 0;
}

static bool op_log_lt_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink)
{
   return left->compare(right) < 0;
}

static bool op_log_le_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink)
{
   return left->compare(right) <= 0;
}

static bool op_log_ne_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink)
{
   return left->compare(right);
}

static bool op_absolute_log_eq(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder lnp(left, xsink);
   if (*xsink)
      return false;

   QoreNodeEvalOptionalRefHolder rnp(right, xsink);
   if (*xsink)
      return false;

   if (is_nothing(*lnp))
      if (is_nothing(*rnp))
	 return true;
      else 
	 return false;
   
   if (is_nothing(*rnp))
      return false;

   return lnp->is_equal_hard(*rnp, xsink);
}

static bool op_absolute_log_neq(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return !op_absolute_log_eq(left, right, xsink);
}

static bool op_regex_match(const QoreString *left, QoreRegex *right, ExceptionSink *xsink)
{
   return right->exec(left, xsink);
}

static bool op_regex_nmatch(const QoreString *left, QoreRegex *right, ExceptionSink *xsink)
{
   return !right->exec(left, xsink);
}

// takes all arguments unevaluated so logic short-circuiting can happen
static bool op_log_or(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   bool l = left->boolEval(xsink);   
   if (*xsink)
      return false;

   // if left side is true, then do not evaluate right side
   return l ? true : right->boolEval(xsink);
}

// "soft" comparison
static inline bool list_is_equal(const QoreList *l, const QoreList *r, ExceptionSink *xsink)
{
   if (l->size() != r->size())
      return false;
   for (int i = 0; i < l->size(); i++)
      if (compareSoft(l->retrieve_entry(i), r->retrieve_entry(i), xsink) || *xsink)
	 return false;
   return true;
}

static bool op_log_eq_list(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   if ((left->type != NT_LIST) || (right->type != NT_LIST))
      return false;
   
   return list_is_equal(left->val.list, right->val.list, xsink);
}

static bool op_log_eq_hash(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   if ((left->type != NT_HASH) || (right->type != NT_HASH))
      return false;

   return !left->val.hash->compareSoft(right->val.hash, xsink);
}

static bool op_log_eq_object(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   if ((left->type != NT_OBJECT) || (right->type != NT_OBJECT))
      return false;
   
   return !left->val.object->compareSoft(right->val.object, xsink);
}

static bool op_log_eq_nothing(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return true;
}

static bool op_log_eq_null(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   if (left && left->type == NT_NULL && right && right->type == NT_NULL)
      return true;
   return false;
}

static bool op_log_ne_list(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   if (left->type != NT_LIST || right->type != NT_LIST)
      return true;

   return !list_is_equal(left->val.list, right->val.list, xsink);
}

static bool op_log_ne_hash(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{   
   if (left->type != NT_HASH || right->type != NT_HASH)
      return true;

   return left->val.hash->compareSoft(right->val.hash, xsink);
}

static bool op_log_ne_object(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   bool b;
   
   if ((left->type != NT_OBJECT) || (right->type != NT_OBJECT))
      b = true;
   else
      b = left->val.object->compareSoft(right->val.object, xsink);
   return b;
}

static bool op_log_ne_nothing(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return false;
}

static bool op_log_ne_null(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   if (left && left->type == NT_NULL && right && right->type == NT_NULL)
      return false;
   return true;
}

static bool op_exists(QoreNode *left, class QoreNode *x, ExceptionSink *xsink)
{
   bool b;
   // if left == NOTHING
   if (is_nothing(left))
      b = false;
   else if (left->type->isValue())
      b = true;
   else
   {
      class QoreNode *tn = NULL;
      class AutoVLock vl;
      class QoreNode *n = getExistingVarValue(left, xsink, &vl, &tn);
      // return if an exception happened
      if (xsink->isEvent())
      {
	 vl.del();
	 if (tn) tn->deref(xsink);
	 //traceout("op_exists()");
	 return false;
      }
      
      // FIXME: this should return false for objects that have been deleted
      if (is_nothing(n))
	 b = false;
      else
	 b = true;
      if (tn) tn->deref(xsink);
   }
   return b;
}

static bool op_instanceof(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder nl(l, xsink);
   if (*xsink || !nl)
      return false;

   if (nl->type == NT_OBJECT && nl->val.object->validInstanceOf(r->val.classref->getID()))
      return true;

   return false;  
}

// takes all arguments unevaluated so logic short-circuiting can happen
static bool op_log_and(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
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

static int64 op_cmp_bigint(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval - right->val.intval;
}

static int64 op_minus_bigint(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval - right->val.intval;
}

static int64 op_plus_bigint(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval + right->val.intval;
}

static int64 op_multiply_bigint(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval * right->val.intval;
}

static int64 op_divide_bigint(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   if (!right->val.intval)
   {
      xsink->raiseException("DIVISION-BY-ZERO", "division by zero in integer expression");
      return 0;
   }
   return left->val.intval / right->val.intval;
}

static int64 op_unary_minus_bigint(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return -left->val.intval;
}

static class QoreNode *op_minus_date(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
    return new QoreNode(left->val.date_time->subtractBy(right->val.date_time));
}

static class QoreNode *op_plus_date(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
    return new QoreNode(left->val.date_time->add(right->val.date_time));
}

static int64 op_cmp_date(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return (int64)DateTime::compareDates(left->val.date_time, right->val.date_time);
}

static double op_minus_float(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.floatval - right->val.floatval;
}

static double op_plus_float(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.floatval + right->val.floatval;
}

static double op_multiply_float(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.floatval * right->val.floatval;
}

static double op_divide_float(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   if (!right->val.floatval)
   {
      xsink->raiseException("DIVISION-BY-ZERO", "division by zero in floating-point expression!");
      return 0.0;
   }
   return left->val.floatval / right->val.floatval;
}

static double op_unary_minus_float(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return -left->val.floatval;
}

static class QoreStringNode *op_plus_string(const QoreString *left, const QoreString *right, ExceptionSink *xsink)
{
   //printd(5, "op_plus_string() (%d) \"%s\" + (%d) \"%s\"\n", left->strlen(), left->getBuffer(), right->strlen(), right->getBuffer());
   TempQoreStringNode str(new QoreStringNode(*left));
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

static int64 op_elements(QoreNode *left, class QoreNode *null, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder np(left, xsink);
   if (*xsink || !np)
      return 0;

   if (np->type == NT_LIST)
      return np->val.list->size();

   if (np->type == NT_OBJECT)
      return np->val.object->size(xsink);

   if (np->type == NT_HASH)
      return np->val.hash->size();

   if (np->type == NT_BINARY)
      return np->val.bin->size();

   {
      QoreStringNode *str = dynamic_cast<QoreStringNode *>(*np);
      if (str)
	 return str->length();
   }

   return 0;
}

static class QoreNode *op_keys(QoreNode *left, class QoreNode *null, bool ref_rv, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder np(left, xsink);
   if (*xsink || !np || (np->type != NT_OBJECT && np->type != NT_HASH))
      return 0;

   class QoreList *l;
   if (np->type == NT_OBJECT)
      l = np->val.object->getMemberList(xsink);
   else
      l = np->val.hash->getKeys();

   return l ? new QoreNode(l) : NULL;
}

static class QoreNode *op_question_mark(QoreNode *left, class QoreNode *list, bool ref_rv, ExceptionSink *xsink)
{
   bool b = left->boolEval(xsink);
   if (xsink->isEvent())
      return NULL;

   if (b)
      return list->val.list->retrieve_entry(0)->eval(xsink);
   return list->val.list->retrieve_entry(1)->eval(xsink);
}

static class QoreNode *op_regex_subst(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   // get current value and save
   class AutoVLock vl;
   class QoreNode **v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // if it's not a string, then do nothing
   if (!(*v) || (*v)->type != NT_STRING)
      return NULL;

   QoreStringNode **vs = reinterpret_cast<QoreStringNode **>(v);
   QoreStringNode *nv = right->val.resub->exec((*vs), xsink);
   if (xsink->isEvent())
      return NULL;

   // assign new value to lvalue
   (*vs)->deref();
   (*v) = nv;

   // reference for return value if necessary
   return ref_rv ? (*v)->RefSelf() : 0;
}

static class QoreNode *op_regex_trans(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   // get current value and save
   class AutoVLock vl;
   class QoreNode **v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // if it's not a string, then do nothing
   if (!(*v) || (*v)->type != NT_STRING)
      return NULL;

   QoreStringNode **vs = reinterpret_cast<QoreStringNode **>(v);
   QoreStringNode *nv = right->val.retrans->exec((*vs), xsink);
   if (*xsink)
      return NULL;

   // assign new value to lvalue
   (*vs)->deref();
   (*v) = nv;
   // reference for return value
   (*v)->ref();
   return (*v);      
}

static class QoreNode *op_list_ref(QoreNode *left, QoreNode *index, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder lp(left, xsink);

   // return NULL if left side is not a list or string (or exception)
   if (!lp || *xsink || (lp->type != NT_LIST && lp->type != NT_STRING))
      return 0;

   class QoreNode *rv = 0;
   int ind = index->integerEval(xsink);
   if (!*xsink) {
      // get value
      if (lp->type == NT_LIST) {
	 rv = lp->val.list->retrieve_entry(ind);
	 // reference for return
	 if (rv)
	    rv->ref();
      }
      else if (ind >= 0) {
	 QoreStringNode *lpstr = reinterpret_cast<QoreStringNode *>(*lp);
	 rv = lpstr->substr(ind, 1);
      }
      //printd(5, "op_list_ref() index=%d, rv=%08p\n", ind, rv);
   }
   return rv;
}

// for the member name, a string is required.  non-string arguments will
// be converted.  The null string can also be used
static class QoreNode *op_object_ref(QoreNode *left, class QoreNode *member, bool ref_rv, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder op(left, xsink);

   // return NULL if left side is not an object (or exception)
   if (!op || *xsink || (op->type != NT_OBJECT && op->type != NT_HASH))
      return NULL;

   // evaluate member expression
   QoreNodeEvalOptionalRefHolder mem(member, xsink);
   if (*xsink)
      return 0;

   QoreStringNodeValueHelper key(*mem);
   class QoreNode *rv;

   if (op->type == NT_HASH)
      rv = op->val.hash->evalKey(key->getBuffer(), xsink);
   else
      rv = op->val.object->evalMember(*key, xsink);

   return rv;
}

static class QoreNode *op_object_method_call(QoreNode *left, class QoreNode *func, bool ref_rv, ExceptionSink *xsink)
{
   QoreNodeEvalOptionalRefHolder op(left, xsink);
   if (*xsink)
      return 0;

   // FIXME: this is an ugly hack!
   if (op && op->type == NT_HASH) {
      // see if the hash member is a call reference
      QoreNode *c = op->val.hash->getKeyValue(func->val.fcall->f.c_str);
      if (c && c->type == NT_FUNCREF)
	 return c->val.funcref->exec(func->val.fcall->args, xsink);
   }
   if (!op || op->type != NT_OBJECT)
   {
      //printd(5, "op=%08p (%s) func=%08p (%s)\n", op, op ? op->type->getName() : "n/a", func, func ? func->type->getName() : "n/a");
      xsink->raiseException("OBJECT-METHOD-EVAL-ON-NON-OBJECT", "member function \"%s\" called on type \"%s\"", 
			    func->val.fcall->f.c_str, op ? op->type->getName() : "NOTHING" );
      return 0;
   }

   return op->val.object->getClass()->evalMethod(op->val.object, func->val.fcall->f.c_str, func->val.fcall->args, xsink);
}

static class QoreNode *op_new_object(QoreNode *left, class QoreNode *x, bool ref_rv, ExceptionSink *xsink)
{
   tracein("op_new_object()");

   class QoreNode *rv = left->val.socall->oc->execConstructor(left->val.socall->args, xsink);
   printd(5, "op_new_object() returning node=%08p (type=%s)\n", rv, left->val.socall->oc->getName());
   // if there's an exception, the constructor will delete the object without the destructor
   traceout("op_new_object()");
   return rv;
}

static class QoreNode *op_assignment(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **v;

   // tracein("op_assignment()");

   /* assign new value, this value gets referenced with the
      eval(xsink) call, so there's no need to reference it again
      for the variable assignment - however it does need to be
      copied/referenced for the return value
   */
   ReferenceHolder<QoreNode> new_value(right->eval(xsink), xsink);
   if (*xsink)
      return 0;

   // get current value and save
   class AutoVLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (*xsink)
      return 0;

   // dereference old value if necessary
   discard(*v, xsink);
   if (*xsink)
   {
      *v = 0;
      return 0;
   }

   // assign new value 
   (*v) = new_value.release();
   vl.del();

#if 0
   printd(5, "op_assignment() *%08p=%08p (type=%s refs=%d)\n",
	  v, new_value, 
	  new_value ? new_value->type->getName() : "(null)",
	  new_value ? new_value->reference_count() : 0 );
#endif

   // traceout("op_assignment()");
   if (ref_rv && (*v))
      return (*v)->RefSelf();

   return NULL;
}

static class QoreNode *op_list_assignment(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **v;

   // tracein("op_assignment()");

   /* assign new value, this value gets referenced with the
      eval(xsink) call, so there's no need to reference it again
      for the variable assignment - however it does need to be
      copied/referenced for the return value
   */
   QoreNodeEvalOptionalRefHolder new_value(right, xsink);
   if (*xsink)
      return 0;

   // get values and save
   int i;
   for (i = 0; i < left->val.list->size(); i++)
   {
      class QoreNode *lv = left->val.list->retrieve_entry(i);

      class AutoVLock vl;
      v = get_var_value_ptr(lv, &vl, xsink);
      if (*xsink)
	 return 0;
      
      // dereference old value if necessary
      discard(*v, xsink);
      if (*xsink)
      {
	 *v = NULL;
	 return 0;
      }

      // if there's only one value, then save it
      if (!*new_value || new_value->type != NT_LIST)
      {
	 if (!i)
	    (*v) = new_value.getReferencedValue();
	 else
	    (*v) = NULL;
      }
      else // assign to list position
      {
	 (*v) = new_value->val.list->retrieve_entry(i);
	 if (*v)
	    (*v)->ref();
      }
      vl.del();
   }

   // traceout("op_list_assignment()");
   return ref_rv ? new_value.takeReferencedValue() : 0;
}

static class QoreNode *op_plus_equals(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **v;

   // tracein("op_plus_equals()");
   QoreNodeEvalOptionalRefHolder new_right(right, xsink);
   if (*xsink || !new_right)
      return 0;

   // get ptr to current value
   class AutoVLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return 0;
   
   // dereferences happen in each section so that the
   // already referenced value can be passed to list->push()
   // if necessary
   // do list plus-equals if left-hand side is a list
   if (*v && ((*v)->type == NT_LIST))
   {
      ensure_unique(v, xsink);
      if (new_right->type == NT_LIST)
	 (*v)->val.list->merge(new_right->val.list);
      else
	 (*v)->val.list->push(new_right.takeReferencedValue());
   }
   // do hash plus-equals if left side is a hash
   else if (*v && ((*v)->type == NT_HASH))
   {
      if (new_right->type == NT_HASH)
      {
	 ensure_unique(v, xsink);
	 (*v)->val.hash->merge(new_right->val.hash, xsink);
      }
      else if (new_right->type == NT_OBJECT)
      {
	 ensure_unique(v, xsink);
	 class QoreHash *h = new_right->val.object->evalData(xsink);
	 if (h)
	    (*v)->val.hash->assimilate(h, xsink);
      }
   }
   // do hash/object plus-equals if left side is an object
   else if (*v && ((*v)->type == NT_OBJECT))
   {
      // do not need ensure_unique() for objects
      if (new_right->type == NT_OBJECT)
      {
	 class QoreHash *h = new_right->val.object->evalData(xsink);
	 if (h)
	    (*v)->val.object->assimilate(h, xsink);
      }
      else if (new_right->type == NT_HASH)
	 (*v)->val.object->merge(new_right->val.hash, xsink);
   }
   // do string plus-equals if left-hand side is a string
   else if ((*v) && ((*v)->type == NT_STRING))
   {
      QoreStringValueHelper str(*new_right);

      ensure_unique(v, xsink);
      QoreStringNode **vs = reinterpret_cast<QoreStringNode **>(v);
      (*vs)->concat(*str, xsink);
   }
   else if ((*v) && ((*v)->type == NT_FLOAT))
   {
      ensure_unique(v, xsink);
      (*v)->val.floatval += new_right->getAsFloat();
   }
   else if ((*v) && ((*v)->type == NT_DATE))
   {
      DateTimeValueHelper date(*new_right);

      DateTime *nd = (*v)->val.date_time->add(*date);
      (*v)->deref(xsink);
      (*v) = new QoreNode(nd);
   }
   else if (is_nothing(*v))
   {
      if (*v)
	 (*v)->deref(xsink); // exception not possible here
      // assign rhs to lhs (take reference for assignment)
      *v = new_right.takeReferencedValue();
   }
   else // do integer plus-equals
   {
      // get new value if necessary
      if (!(*v))
	 (*v) = new QoreNode((int64)0);
      else
      {
	 if ((*v)->type != NT_INT)
	 {
	    class QoreNode *n = new QoreNode((*v)->getAsBigInt());
	    (*v)->deref(xsink);
	    (*v) = n;
	 }
	 ensure_unique(v, xsink);
      }

      // increment current value
      (*v)->val.intval += new_right->getAsBigInt();
   }

   // reference return value
   // traceout("op_plus_equals()");
   if (ref_rv)
      return (*v)->RefSelf();
   return NULL;
}

static class QoreNode *op_minus_equals(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   // tracein("op_minus_equals()");

   class QoreNode **v;

   QoreNodeEvalOptionalRefHolder new_right(right, xsink);
   if (*xsink || !new_right)
      return 0;

   // get ptr to current value

   class AutoVLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (*xsink)
      return 0;

   // do float minus-equals if left side is a float
   if ((*v) && ((*v)->type == NT_FLOAT))
   {
      ensure_unique(v, xsink);
      (*v)->val.floatval -= new_right->getAsFloat();
   }
   else if ((*v) && ((*v)->type == NT_DATE))
   {
      DateTimeValueHelper date(*new_right);

      ensure_unique(v, xsink);
      DateTime *nd = (*v)->val.date_time->subtractBy(*date);
      (*v)->deref(xsink);
      (*v) = new QoreNode(nd);
   }
   else if ((*v) && ((*v)->type == NT_HASH))
   {
      if (new_right->type == NT_HASH) {
	 // do nothing
      }
      else if (new_right->type == NT_LIST && new_right->val.list->size()) {
	 ensure_unique(v, xsink);

	 // treat each element in the list as a string giving a key to delete
	 ListIterator li(new_right->val.list);
	 while (li.next()) {
	    QoreStringValueHelper val(li.getValue());

	    (*v)->val.hash->deleteKey(*val, xsink);
	    if (*xsink)
	       return 0;
	 }
      }
      else {
	 QoreStringValueHelper str(*new_right);
	 ensure_unique(v, xsink);
	 (*v)->val.hash->deleteKey(*str, xsink);
      }
   }
   else // do integer minus-equals
   {
      if (new_right->type == NT_FLOAT)
      {
	 // we know the lhs type is not NT_FLOAT already
	 // get current float value and dereference node
	 double val;
	 if (*v) {
	    val = (*v)->getAsFloat();
	    (*v)->deref(xsink);
	 }
	 else
	    val = 0.0;

	 // assign negative argument
	 (*v) = new QoreNode(val - new_right->getAsFloat());
      }
      else
      {
	 // get new value if necessary
	 if (!(*v))
	    (*v) = new QoreNode((int64)0);
	 else
	 {
	    if ((*v)->type != NT_INT)
	    {
	       class QoreNode *n = new QoreNode((*v)->getAsBigInt());
	       (*v)->deref(xsink);
	       (*v) = n;
	    }
	    ensure_unique(v, xsink);
	 }

	 // increment current value
	 (*v)->val.intval -= new_right->getAsBigInt();	 
      }
   }

   // traceout("op_minus_equals()");
   // reference return value and return
   return ref_rv ? (*v)->RefSelf() : 0;
}

static class QoreNode *op_and_equals(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **v;

   //tracein("op_and_equals()");
   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return NULL;

   // get ptr to current value
   class AutoVLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // get new value if necessary
   if (!(*v))
      (*v) = new QoreNode((int64)0);
   else 
   {
      if ((*v)->type != NT_INT)
      {
	 class QoreNode *n = new QoreNode((*v)->getAsBigInt()); 
	 (*v)->deref(xsink);
	 (*v) = n;
      }

      ensure_unique(v, xsink);
   }

   // and current value with arg val
   (*v)->val.intval &= val;

   //traceout("op_and_equals()");

   // reference return value and return
   if (ref_rv)
      return (*v)->RefSelf();
   return NULL;
}

static class QoreNode *op_or_equals(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **v;

   //tracein("op_or_equals()");

   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return NULL;

   // get ptr to current value
   class AutoVLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // get new value if necessary
   if (!(*v))
      (*v) = new QoreNode((int64)0);
   else 
   {
      if ((*v)->type != NT_INT)
      {
	 class QoreNode *n = new QoreNode((*v)->getAsBigInt()); 
	 (*v)->deref(xsink);
	 (*v) = n;
      }

      ensure_unique(v, xsink);
   }

   // or current value with arg val
   (*v)->val.intval |= val;

   //traceout("op_or_equals()");
   // reference return value and return
   if (ref_rv)
      return (*v)->RefSelf();
   return NULL;
}

static class QoreNode *op_modula_equals(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **v;

   //tracein("op_modula_equals()");

   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return NULL;

   // get ptr to current value
   class AutoVLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // get new value if necessary
   if (!(*v))
      (*v) = new QoreNode((int64)0);
   else 
   {
      if ((*v)->type != NT_INT)
      {
	 class QoreNode *n = new QoreNode((*v)->getAsBigInt());
	 (*v)->deref(xsink);
	 (*v) = n;
      }

      ensure_unique(v, xsink);
   }

   // or current value with arg val
   (*v)->val.intval %= val;

   // reference return value and return
   if (ref_rv)
      return (*v)->RefSelf();

   //traceout("op_modula_equals()");
   return NULL;
}

static class QoreNode *op_multiply_equals(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **v;
   //tracein("op_multiply_equals()");

   QoreNodeEvalOptionalRefHolder res(right, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value
   class AutoVLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (*xsink)
      return 0;

   // is either side a float?
   if (res && res->type == NT_FLOAT)
   {
      if (!(*v))
	 (*v) = new QoreNode((double)0.0);
      else
      {
	 ensure_unique_float(v, xsink);
	 if (*xsink)
	    return 0;
	 
	 // multiply current value with arg val
	 (*v)->val.floatval *= res->val.floatval;
      }
   }
   else if ((*v) && (*v)->type == NT_FLOAT)
   {
      if (res)
      {
	 ensure_unique(v, xsink);
	 (*v)->val.floatval *= res->getAsFloat();
      }
      else // if factor is NOTHING, assign 0.0
      {
	 (*v)->deref(xsink);
	 (*v) = new QoreNode((double)0.0);
      }
   }
   else // do integer multiply equals
   {
      // get new value if necessary
      if (!(*v))
	 (*v) = new QoreNode((int64)0);
      else 
      {
	 if (res)
	 {
	    ensure_unique_int(v, xsink);
	    if (*xsink)
	       return 0;

	    // multiply current value with arg val
	    (*v)->val.intval *= res->getAsBigInt();
	 }
	 else // if factor is NOTHING, assign 0
	 {
	    (*v)->deref(xsink);
	    (*v) = new QoreNode((int64)0);
	 }
      }
   }

   //traceout("op_multiply_equals()");

   // reference return value and return
   return ref_rv ? (*v)->RefSelf() : 0;
}

static class QoreNode *op_divide_equals(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **v;
   //tracein("op_divide_equals()");

   QoreNodeEvalOptionalRefHolder res(right, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value
   class AutoVLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (*xsink)
      return 0;

   // is either side a float?
   if (res && res->type == NT_FLOAT)
   {
      if (!res->val.floatval)
	 xsink->raiseException("DIVISION-BY-ZERO", "division by zero in floating-point expression!");
      else
      {
	 if (!(*v))
	    (*v) = new QoreNode((double)0.0);
	 else
	 {
	    ensure_unique_float(v, xsink);

	    // divide current value with arg val
	    (*v)->val.floatval /= res->val.floatval;
	 }
      }
   }
   else if ((*v) && (*v)->type == NT_FLOAT)
   {
      if (res)
      {
	 float val = res->getAsFloat();
	 if (!val) {
	    xsink->raiseException("DIVISION-BY-ZERO", "division by zero in floating-point expression!");
	    return 0;
	 }
	 else {
	    ensure_unique(v, xsink);
	    (*v)->val.floatval /= val;
	 }
      }
      else // if factor is NOTHING, raise exception
	 xsink->raiseException("DIVISION-BY-ZERO", "division by zero in floating-point expression!");
   }
   else // do integer divide equals
   {
      int64 val = res ? res->getAsBigInt() : 0;
      if (!val) {
	 xsink->raiseException("DIVISION-BY-ZERO", "division by zero in integer expression!");
	 return 0;
      }
      // get new value if necessary
      if (!(*v))
	 (*v) = new QoreNode((int64)0);
      else 
      {
	 ensure_unique_int(v, xsink);
	 if (*xsink)
	    return 0;
	 
	 // divide current value with arg val
	 (*v)->val.intval /= val;
      }
   }

   // reference return value and return
   return ((*v) && ref_rv) ? (*v)->RefSelf() : 0;
}

static class QoreNode *op_xor_equals(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **v;

   //tracein("op_xor_equals()");

   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return NULL;

   // get ptr to current value
   class AutoVLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // get new value if necessary
   if (!(*v))
      (*v) = new QoreNode((int64)0);
   else 
   {
      if ((*v)->type != NT_INT)
      {
	 class QoreNode *n = new QoreNode((*v)->getAsBigInt());
	 (*v)->deref(xsink);
	 (*v) = n;
      }

      ensure_unique(v, xsink);
   }

   // xor current value with arg val
   (*v)->val.intval ^= val;

   // reference return value and return
   if (ref_rv)
      return (*v)->RefSelf();

   //traceout("op_xor_equals()");
   return NULL;
}

static class QoreNode *op_shift_left_equals(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **v;

   //tracein("op_shift_left_equals()");

   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return NULL;

   // get ptr to current value
   class AutoVLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // get new value if necessary
   if (!(*v))
      (*v) = new QoreNode((int64)0);
   else 
   {
      if ((*v)->type != NT_INT)
      {
	 class QoreNode *n = new QoreNode((*v)->getAsBigInt());
	 (*v)->deref(xsink);
	 (*v) = n;
      }

      ensure_unique(v, xsink);
   }

   // shift left current value by arg val
   (*v)->val.intval <<= val;

   // reference return value and return
   return ref_rv ? (*v)->RefSelf() : 0;
}

static class QoreNode *op_shift_right_equals(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **v;

   //tracein("op_shift_right_equals()");

   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return NULL;

   // get ptr to current value
   class AutoVLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // get new value if necessary
   if (!(*v))
      (*v) = new QoreNode((int64)0);
   else 
   {
      if ((*v)->type != NT_INT)
      {
	 class QoreNode *n = new QoreNode((*v)->getAsBigInt());
	 (*v)->deref(xsink);
	 (*v) = n;
      }

      ensure_unique(v, xsink);
   }

   // shift right current value by arg val
   (*v)->val.intval >>= val;

   // reference return value and return
   if (ref_rv)
      return (*v)->RefSelf();

   //traceout("op_shift_right_equals()");
   return NULL;
}

// this is the default (highest-priority) function for the + operator, so any type could be sent here on either side
static class QoreNode *op_plus_list(QoreNode *left, QoreNode *right)
{
   QoreList *rv;
   if (left->type == NT_LIST) {
      rv = left->val.list->copyList();
      if (right->type == NT_LIST)
	 rv->merge(right->val.list);
      else
	 rv->push(right->RefSelf());
   }
   else if (right->type != NT_LIST)
      return 0;
   else {
      QoreList *rv = new QoreList();
      rv->push(left->RefSelf());
      rv->merge(right->val.list);
   }

   return new QoreNode(rv);
}

static class QoreNode *op_plus_hash_hash(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   if (!left || left->type != NT_HASH)
      return 0;
   if (!right || right->type != NT_HASH)
      return 0;

   TempQoreHash rv(left->val.hash->copy(), xsink);
   rv->merge(right->val.hash, xsink);
   if (*xsink)
      return 0;
   return new QoreNode(rv.release());
}

static class QoreNode *op_plus_hash_object(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   assert(right && right->type == NT_OBJECT);
   TempQoreHash h(right->val.object->copyData(xsink), xsink);
   if (*xsink)
      return 0;

   assert(left && left->type == NT_HASH);
   TempQoreHash rv(left->val.hash->copy(), xsink);
   rv->assimilate(h.release(), xsink);
   if (*xsink)
      return 0;

   return new QoreNode(rv.release());
}

// note that this will return a hash
static class QoreNode *op_plus_object_hash(QoreNode *left, QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   assert(left && left->type == NT_OBJECT);
   TempQoreHash h(left->val.object->copyData(xsink), xsink);
   if (*xsink)
      return 0;

   assert(right && right->type == NT_HASH);
   h->merge(right->val.hash, xsink);
   if (*xsink)
      return 0;

   return new QoreNode(h.release());
}

static int64 op_cmp_double(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   int64 rc;
   if (left->val.floatval < right->val.floatval)
      rc = -1;
   else if (left->val.floatval == right->val.floatval)
      rc = 0;
   else
      rc = 1;
   return rc;
}

static int64 op_modula_int(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval % right->val.intval;
}

static int64 op_bin_and_int(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval & right->val.intval;
}

static int64 op_bin_or_int(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval | right->val.intval;
}

static int64 op_bin_not_int(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return ~left->val.intval;
}

static int64 op_bin_xor_int(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval ^ right->val.intval;
}

static int64 op_shift_left_int(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval << right->val.intval;
}

static int64 op_shift_right_int(QoreNode *left, QoreNode *right, ExceptionSink *xsink)
{
   return left->val.intval >> right->val.intval;
}

// variable assignment
static class QoreNode *op_post_inc(QoreNode *left, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **n, *rv;

   // tracein("op_post_inc()");
   class AutoVLock vl;
   n = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   rv = *n;

   // acquire new value if necessary
   if (!(*n))
      (*n) = new QoreNode((int64)0);
   else if ((*n)->type != NT_INT)
   {
      class QoreNode *nv = new QoreNode((*n)->getAsBigInt());

      // get unique value if necessary
      if (!nv->is_unique())
      {
	 (*n) = nv->realCopy(xsink);
	 nv->deref(xsink);
      }	 
      else
	 (*n) = nv;
   }
   else // copy value to get a node with reference count 1
      (*n) = (*n)->realCopy(xsink);

   // increment value
   (*n)->val.intval++;

   // traceout("op_post_inc");
   // return original value (may be null or non-integer)
   return rv;
}

// variable assignment
static class QoreNode *op_post_dec(QoreNode *left, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **n, *rv;
   // tracein("op_post_dec()");

   class AutoVLock vl;
   n = get_var_value_ptr(left, &vl, xsink);
   //printd(5, "op_post_dec() n=%08p, *n=%08p\n", n, *n);
   if (xsink->isEvent())
      return NULL;

   // acquire new value if necessary
   if (!(*n))
   {
      (*n) = new QoreNode((int64)0);
      rv = NULL;
   }
   else if ((*n)->type != NT_INT)
   {
      class QoreNode *nv = new QoreNode((*n)->getAsBigInt());
      rv = *n;

      // get unique value if necessary
      if (!nv->is_unique())
      {
	 (*n) = nv->realCopy(xsink);
	 nv->deref(xsink);
      }	 
      else
	 (*n) = nv;
   }
   else
   {
      // copy value and dereference original
      rv = (*n);
      (*n) = (*n)->realCopy(xsink);
   }

   // decrement value
   (*n)->val.intval--;

   //printd(5, "op_post_dec(): n=%08p, *n=%08p\n", n, *n);
   // traceout("op_post_dec()");
   // return original value (may be null or non-integer)
   return rv;
}

// variable assignment
static class QoreNode *op_pre_inc(QoreNode *left, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **n;
   // tracein("op_pre_inc()");

   class AutoVLock vl;
   n = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // acquire new value if necessary
   if (!(*n))
      (*n) = new QoreNode((int64)0);
   else
   {
      if ((*n)->type != NT_INT)
      {
	 class QoreNode *nv = new QoreNode((*n)->getAsBigInt());
	 (*n)->deref(xsink);
	 (*n) = nv;
      }

      ensure_unique(n, xsink);
   }

   // increment value
   (*n)->val.intval++;

   // traceout("op_pre_dec()");

   //printd(5, "op_pre_inc() ref_rv=%s\n", ref_rv ? "true" : "false");
   // reference for return value
   if (ref_rv)
      return (*n)->RefSelf();
   return NULL;
}

// variable assignment
static class QoreNode *op_pre_dec(QoreNode *left, bool ref_rv, ExceptionSink *xsink)
{
   class QoreNode **n;
   // tracein("op_pre_dec()");

   class AutoVLock vl;
   n = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // acquire new value if necessary
   if (!(*n))
      (*n) = new QoreNode((int64)0);
   else
   {
      if ((*n)->type != NT_INT)
      {
	 class QoreNode *nv = new QoreNode((*n)->getAsBigInt());
	 (*n)->deref(xsink);
	 (*n) = nv;
      }
      
      ensure_unique(n, xsink);
   }

   // decrement value
   (*n)->val.intval--;

   // traceout("op_pre_dec()");

   // reference return value
   if (ref_rv)
      return (*n)->RefSelf();
   return NULL;
}

// unshift lvalue, element
static QoreNode *op_unshift(QoreNode *left, class QoreNode *elem, bool ref_rv, ExceptionSink *xsink)
{
   //tracein("op_unshift()");
   printd(5, "op_unshift(%08p, %08p, isEvent=%d)\n", left, elem, xsink->isEvent());

   class AutoVLock vl;
   QoreNode **val = get_var_value_ptr(left, &vl, xsink);
   // value is not a list, so throw exception
   if (xsink->isEvent() || !(*val) || (*val)->type != NT_LIST)
   {
      xsink->raiseException("UNSHIFT-ERROR", "first argument to unshift is not a list");
      return NULL;
   }

   ensure_unique(val, xsink);

   printd(5, "op_unshift() *val=%08p (%s)\n", *val, *val ? (*val)->type->getName() : "(none)");
   printd(5, "op_unshift() about to call unshift() on list node %08p (%d) with element %08p\n", (*val), (*val)->val.list->size(), elem);

   if (elem)
   {
      elem = elem->eval(xsink);
      if (*xsink)
      {
	 if (elem) elem->deref(xsink);
	 return NULL;
      }
   }

   (*val)->val.list->insert(elem);

   // reference for return value
   if (ref_rv)
      return (*val)->RefSelf();

   //traceout("op_unshift()");
   return NULL;
}

static QoreNode *op_shift(QoreNode *left, class QoreNode *x, bool ref_rv, ExceptionSink *xsink)
{
   //tracein("op_shift()");
   printd(5, "op_shift(%08p, %08p, isEvent=%d)\n", left, x, xsink->isEvent());

   class AutoVLock vl;
   QoreNode **val = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent() || !(*val) || (*val)->type != NT_LIST)
      return NULL;

   ensure_unique(val, xsink);

   printd(5, "op_shift() *val=%08p (%s)\n", *val, *val ? (*val)->type->getName() : "(none)");
   printd(5, "op_shift() about to call QoreList::shift() on list node %08p (%d)\n", (*val), (*val)->val.list->size());

   QoreNode *rv = (*val)->val.list->shift();

   printd(5, "op_shift() got node %08p (%s)\n", rv, rv ? rv->type->getName() : "(none)");
   // the list reference will now be the reference for return value
   // therefore no need to reference again

   //traceout("op_shift()");
   return rv;
}

static QoreNode *op_pop(QoreNode *left, class QoreNode *x, bool ref_rv, ExceptionSink *xsink)
{
   //tracein("op_pop()");
   printd(5, "op_pop(%08p, %08p, isEvent=%d)\n", left, x, xsink->isEvent());

   class AutoVLock vl;
   QoreNode **val = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent() || !(*val) || (*val)->type != NT_LIST)
      return NULL;

   ensure_unique(val, xsink);

   printd(5, "op_pop() *val=%08p (%s)\n", *val, *val ? (*val)->type->getName() : "(none)");
   printd(5, "op_pop() about to call QoreList::pop() on list node %08p (%d)\n", (*val), (*val)->val.list->size());

   QoreNode *rv = (*val)->val.list->pop();

   printd(5, "op_pop() got node %08p (%s)\n", rv, rv ? rv->type->getName() : "(none)");
   // the list reference will now be the reference for return value
   // therefore no need to reference again

   //traceout("op_pop()");
   return rv;
}

static QoreNode *op_push(QoreNode *left, class QoreNode *elem, bool ref_rv, ExceptionSink *xsink)
{
   //tracein("op_push()");
   printd(5, "op_push(%08p, %08p, isEvent=%d)\n", left, elem, xsink->isEvent());

   class AutoVLock vl;
   QoreNode **val = get_var_value_ptr(left, &vl, xsink);
   // value is not a list, so throw exception
   if (xsink->isEvent() || !(*val) || (*val)->type != NT_LIST)
   {
      xsink->raiseException("PUSH-ERROR", "first argument to push is not a list");
      return NULL;
   }

   ensure_unique(val, xsink);

   printd(5, "op_push() about to call push() on list node %08p (%d) with element %08p\n", (*val), (*val)->val.list->size(), elem);

   if (elem)
   {
      elem = elem->eval(xsink);
      if (*xsink)
      {
	 if (elem) elem->deref(xsink);
	 return NULL;
      }
   }

   (*val)->val.list->push(elem);

   // reference for return value
   return ref_rv ? (*val)->RefSelf() : 0;
}

// lvalue, offset, [length, [list]]
static QoreNode *op_splice(QoreNode *left, class QoreNode *l, bool ref_rv, ExceptionSink *xsink)
{
   //tracein("op_splice()");
   printd(5, "op_splice(%08p, %08p, isEvent=%d)\n", left, l, xsink->isEvent());

   class AutoVLock vl;
   QoreNode **val = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // if value is not a list or string, throw exception
   if (!(*val) || ((*val)->type != NT_LIST && (*val)->type != NT_STRING))
   {
      xsink->raiseException("SPLICE-ERROR", "first argument to splice is not a list or a string");
      return NULL;
   }
   
   // evaluate list
   QoreNodeEvalOptionalRefHolder nl(l, xsink);
   if (*xsink)
      return 0;

   ensure_unique(val, xsink);

   // evaluating a list must give another list
   assert(nl->type == NT_LIST);
   int size = nl->val.list->size();
   int offset = nl->val.list->getEntryAsInt(0);

#ifdef DEBUG
   if ((*val)->type == NT_LIST)
      printd(5, "op_splice() val=%08p (size=%d) list=%08p (size=%d) offset=%d\n", (*val), (*val)->val.list->size(), *nl, size, offset);
   else {
      QoreStringNode **vs = reinterpret_cast<QoreStringNode **>(val);

      printd(5, "op_splice() val=%08p (strlen=%d) list=%08p (size=%d) offset=%d\n", (*val), (*vs)->strlen(), *nl, size, offset);
   }
#endif

   if ((*val)->type == NT_LIST)
   {
      if (size == 1)
	 (*val)->val.list->splice(offset, xsink);
      else
      {
	 int length = nl->val.list->getEntryAsInt(1);
	 if (size == 2)
	    (*val)->val.list->splice(offset, length, xsink);
	 else
	    (*val)->val.list->splice(offset, length, nl->val.list->retrieve_entry(2), xsink);
      }
   }
   else // must be a string
   {
      QoreStringNode **vs = reinterpret_cast<QoreStringNode **>(val);

      if (size == 1)
	 (*vs)->splice(offset, xsink);
      else
      {
	 int length = nl->val.list->getEntryAsInt(1);
	 if (size == 2)
	    (*vs)->splice(offset, length, xsink);
	 else
	    (*vs)->splice(offset, length, nl->val.list->retrieve_entry(2), xsink);
      }
   }

   // reference for return value
   return ref_rv ? (*val)->RefSelf() : 0;
}

static int64 op_chomp(class QoreNode *arg, class QoreNode *x, ExceptionSink *xsink)
{
   //tracein("op_chomp()");
   
   class AutoVLock vl;
   QoreNode **val = get_var_value_ptr(arg, &vl, xsink);
   if (xsink->isEvent())
      return 0;
   
   if (!(*val) || ((*val)->type != NT_STRING && (*val)->type != NT_LIST && (*val)->type != NT_HASH))
      return 0;
   int count = 0;
   
   // note that no exception can happen here
   ensure_unique(val, xsink);
   if ((*val)->type == NT_STRING) {
      QoreStringNode **vs = reinterpret_cast<QoreStringNode **>(val);
      count += (*vs)->chomp();
   }
   else if ((*val)->type == NT_LIST)
   {
      ListIterator li((*val)->val.list);
      while (li.next())
      {
	 class QoreNode **v = li.getValuePtr();
	 if (*v && (*v)->type == NT_STRING)
	 {
	    // note that no exception can happen here
	    ensure_unique(v, xsink);
	    QoreStringNode **vs = reinterpret_cast<QoreStringNode **>(v);
	    count += (*vs)->chomp();
	 }
      }      
   }
   else // is a hash
   {
      HashIterator hi((*val)->val.hash);
      while (hi.next())
      {
	 class QoreNode **v = hi.getValuePtr();
	 if (*v && (*v)->type == NT_STRING)
	 {
	    // note that no exception can happen here
	    ensure_unique(v, xsink);
	    QoreStringNode **vs = reinterpret_cast<QoreStringNode **>(v);
	    count += (*vs)->chomp();
	 }
      }
   }
   return (int64)count;
}

static QoreNode *op_trim(class QoreNode *arg, class QoreNode *x, bool ref_rv, ExceptionSink *xsink)
{
   //tracein("op_trim()");
   
   class AutoVLock vl;
   QoreNode **val = get_var_value_ptr(arg, &vl, xsink);
   if (xsink->isEvent())
      return 0;
   
   if (!(*val) || ((*val)->type != NT_STRING && (*val)->type != NT_LIST && (*val)->type != NT_HASH))
      return 0;
   
   // note that no exception can happen here
   ensure_unique(val, xsink);
   if ((*val)->type == NT_STRING) {
      QoreStringNode **vs = reinterpret_cast<QoreStringNode **>(val);
      (*vs)->trim();
   }
   else if ((*val)->type == NT_LIST)
   {
      ListIterator li((*val)->val.list);
      while (li.next())
      {
	 class QoreNode **v = li.getValuePtr();
	 if (*v && (*v)->type == NT_STRING)
	 {
	    // note that no exception can happen here
	    ensure_unique(v, xsink);
	    QoreStringNode **vs = reinterpret_cast<QoreStringNode **>(v);
	    (*vs)->trim();
	 }
      }      
   }
   else // is a hash
   {
      HashIterator hi((*val)->val.hash);
      while (hi.next())
      {
	 class QoreNode **v = hi.getValuePtr();
	 if (*v && (*v)->type == NT_STRING)
	 {
	    // note that no exception can happen here
	    ensure_unique(v, xsink);
	    QoreStringNode **vs = reinterpret_cast<QoreStringNode **>(v);
	    (*vs)->trim();
	 }
      }
   }

   // reference for return value
   if (ref_rv)
      return (*val)->RefSelf();
   
   return 0;
}

static QoreNode *op_minus_hash_string(const QoreHash *h, const QoreString *s, ExceptionSink *xsink)
{
   TempQoreHash nh(h->copy(), xsink);
   nh->deleteKey(s, xsink);
   if (*xsink)
      return 0;
   return new QoreNode(nh.release());
}

static QoreNode *op_minus_hash_list(class QoreNode *h, class QoreNode *l, ExceptionSink *xsink)
{
   ReferenceHolder<QoreNode> x(h->realCopy(xsink), xsink);
   if (*xsink)
      return 0;

   // treat each element in the list as a string giving a key to delete
   ListIterator li(l->val.list);
   while (li.next()) {
      QoreStringValueHelper val(li.getValue());
      
      x->val.hash->deleteKey(*val, xsink);
      if (*xsink)
	 return 0;
   }
   return x.release();
}

static class QoreNode *op_regex_extract(const QoreString *left, QoreRegex *right, ExceptionSink *xsink)
{
   class QoreList *l = right->extractSubstrings(left, xsink);
   if (!l)
      return NULL;
   return new QoreNode(l);
}

static QoreNode *get_node_type(QoreNode *n, const QoreType *t)
{
   assert(n);
   assert(n->type != t);

   if (t == NT_STRING) {
      QoreStringNode *str = new QoreStringNode();
      n->getStringRepresentation(*str);
      return str;
   }

   if (t == NT_INT)
      return new QoreNode(n->getAsBigInt());

   if (t == NT_FLOAT)
      return new QoreNode(n->getAsFloat());

   if (t == NT_BOOLEAN)
      return new QoreNode(n->getAsBool());

   if (t == NT_DATE) {
      DateTime *dt = new DateTime();
      n->getDateTimeRepresentation(*dt);
      return new QoreNode(dt);
   }
   
   if (t == NT_LIST) {
      QoreList *l = new QoreList();
      l->push(n ? n->RefSelf() : 0);
      return new QoreNode(l);
   }

   printd(0, "get_node_type() got type '%s'\n", t->getName());
   assert(false);
   return 0;
}

AbstractOperatorFunction::AbstractOperatorFunction(const QoreType *lt, const QoreType *rt) : ltype(lt), rtype(rt)
{
}

OperatorFunction::OperatorFunction(const QoreType *lt, const QoreType *rt, op_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
{
}

BoolOperatorFunction::BoolOperatorFunction(const QoreType *lt, const QoreType *rt, op_bool_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
{
}

BigIntOperatorFunction::BigIntOperatorFunction(const QoreType *lt, const QoreType *rt, op_bigint_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
{
}

FloatOperatorFunction::FloatOperatorFunction(const QoreType *lt, const QoreType *rt, op_float_func_t f) : AbstractOperatorFunction(lt, rt), op_func(f)
{
}

class QoreNode *OperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1)
      return op_func(left, 0, ref_rv, xsink);

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   return op_func(left, right, ref_rv, xsink);
}

bool OperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      ReferenceHolder<QoreNode> rv(op_func(left, 0, true, xsink), xsink);
      return *rv ? rv->getAsBool() : false;
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   ReferenceHolder<QoreNode> rv(op_func(left, right, true, xsink), xsink);
   return *rv ? rv->getAsBool() : false;
}

int64 OperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      ReferenceHolder<QoreNode> rv(op_func(left, 0, true, xsink), xsink);
      return *rv ? rv->getAsBigInt() : false;
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   ReferenceHolder<QoreNode> rv(op_func(left, right, true, xsink), xsink);
   return *rv ? rv->getAsBigInt() : 0;
}

double OperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      ReferenceHolder<QoreNode> rv(op_func(left, 0, true, xsink), xsink);
      return *rv ? rv->getAsFloat() : false;
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   ReferenceHolder<QoreNode> rv(op_func(left, right, true, xsink), xsink);
   return *rv ? rv->getAsFloat() : 0;
}

class QoreNode *NodeOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   if (!ref_rv)
      return 0;
   return op_func(left, right, xsink);
}

bool NodeOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> rv(op_func(left, right, xsink), xsink);
   return *rv ? rv->getAsBool() : false;
}

int64 NodeOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> rv(op_func(left, right, xsink), xsink);
   return *rv ? rv->getAsBigInt() : 0;
}

double NodeOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> rv(op_func(left, right, xsink), xsink);
   return *rv ? rv->getAsFloat() : 0;
}

class QoreNode *EffectNoEvalOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   return op_func(left, right, ref_rv, xsink);
}

bool EffectNoEvalOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> rv(op_func(left, right, true, xsink), xsink);
   return *rv ? rv->getAsBool() : false;
}

int64 EffectNoEvalOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> rv(op_func(left, right, true, xsink), xsink);
   return *rv ? rv->getAsBigInt() : 0;
}

double EffectNoEvalOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> rv(op_func(left, right, true, xsink), xsink);
   return *rv ? rv->getAsFloat() : 0;
}

class QoreNode *HashStringOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   assert(left && left->type == NT_HASH);
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return 0;

   QoreStringValueHelper r(right);
   return op_func(left->val.hash, *r, xsink);
}

bool HashStringOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(left && left->type == NT_HASH);
   // this operator can never return a value in a boolean context and cannot have a side-effect
   return false;
}

int64 HashStringOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(left && left->type == NT_HASH);
   // this operator can never return a value in an integer context and cannot have a side-effect
   return 0;
}

double HashStringOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(left && left->type == NT_HASH);
   // this operator can never return a value in a floating-point context and cannot have a side-effect
   return 0.0;
}

class QoreNode *HashListOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   assert(left && left->type == NT_HASH);
   assert(right && right->type == NT_LIST);
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return 0;

   return op_func(left->val.hash, right->val.list, xsink);
}

bool HashListOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(left && left->type == NT_HASH);
   // this operator can never return a value in a boolean context and cannot have a side-effect
   return false;
}

int64 HashListOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(left && left->type == NT_HASH);
   // this operator can never return a value in an integer context and cannot have a side-effect
   return 0;
}

double HashListOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(left && left->type == NT_HASH);
   // this operator can never return a value in a floating-point context and cannot have a side-effect
   return 0.0;
}

class QoreNode *NoConvertOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return 0;

   return op_func(left, right);
}

bool NoConvertOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   // this operator can never return a value in a boolean context and cannot have a side-effect
   return false;
}

int64 NoConvertOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   // this operator can never return a value in an integer context and cannot have a side-effect
   return 0;
}

double NoConvertOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   // this operator can never return a value in a floating-point context and cannot have a side-effect
   return 0.0;
}

class QoreNode *EffectBoolOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   bool b = op_func(left, right, xsink);
   return *xsink ? 0 : new QoreNode(b);
}

bool EffectBoolOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   return op_func(left, right, xsink);
}

int64 EffectBoolOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   return (int64)op_func(left, right, xsink);
}

double EffectBoolOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   return (double)op_func(left, right, xsink);
}

class QoreNode *VarRefOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   assert(left);
   return op_func(left, ref_rv, xsink);
}

bool VarRefOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(left);
   ReferenceHolder<QoreNode> rv(op_func(left, true, xsink), xsink);
   return rv->getAsBool();
}

int64 VarRefOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(left);
   ReferenceHolder<QoreNode> rv(op_func(left, true, xsink), xsink);
   return rv->getAsBigInt();
}

double VarRefOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(left);
   ReferenceHolder<QoreNode> rv(op_func(left, true, xsink), xsink);
   return rv->getAsFloat();
}

class QoreNode *StringStringStringOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return 0;

   QoreStringValueHelper l(left);
   QoreStringValueHelper r(right);
   return op_func(*l, *r, xsink);
}

bool StringStringStringOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   QoreStringValueHelper r(right);

   TempQoreStringNode rv(op_func(*l, *r, xsink));
   if (!*rv)
      return 0;
   return rv->getAsBool();
}

int64 StringStringStringOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   QoreStringValueHelper r(right);

   TempQoreStringNode rv(op_func(*l, *r, xsink));
   if (!*rv)
      return 0;
   return rv->getAsBigInt();
}

double StringStringStringOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   QoreStringValueHelper r(right);

   TempQoreStringNode rv(op_func(*l, *r, xsink));
   if (!*rv)
      return 0;
   return rv->getAsFloat();
}

class QoreNode *ListStringRegexOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   assert(right && right->type == NT_REGEX);
   // return immediately if the return value is ignored, this statement will have no effect and there can be no side-effects
   if (!ref_rv) return 0;

   QoreStringValueHelper l(left);
   return op_func(*l, right->val.regex, xsink);
}

bool ListStringRegexOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(right && right->type == NT_REGEX);
   // this operator can never return a value in this context and cannot have a side-effect
   return false;
}

int64 ListStringRegexOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(right && right->type == NT_REGEX);
   // this operator can never return a value in this context and cannot have a side-effect
   return 0;
}

double ListStringRegexOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   assert(right && right->type == NT_REGEX);
   // this operator can never return a value in this context and cannot have a side-effect
   return 0.0;
}

class QoreNode *BoolOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      bool rv = op_func(left, 0, xsink);
      if (!ref_rv || *xsink)
	 return 0;
      return new QoreNode(rv);
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   bool rv = op_func(left, right, xsink);
   if (!ref_rv || *xsink)
      return 0;
   return new QoreNode(rv);
}

bool BoolOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      return op_func(left, right, xsink);
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   return op_func(left, right, xsink);
}

int64 BoolOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      return (int64)op_func(left, right, xsink);
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   return (int64)op_func(left, right, xsink);
}

double BoolOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      return (double)op_func(left, right, xsink);
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   return (double)op_func(left, right, xsink);
}

class QoreNode *NoConvertBoolOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   if (args == 1) {
      bool rv = op_func(left, 0, xsink);
      if (!ref_rv || *xsink)
	 return 0;
      return new QoreNode(rv);
   }

   bool rv = op_func(left, right, xsink);
   if (!ref_rv || *xsink)
      return 0;
   return new QoreNode(rv);
}

bool NoConvertBoolOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   if (args == 1)
      return op_func(left, right, xsink);

   return op_func(left, right, xsink);
}

int64 NoConvertBoolOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   if (args == 1)
      return (int64)op_func(left, right, xsink);

   return (int64)op_func(left, right, xsink);
}

double NoConvertBoolOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   if (args == 1)
      return (double)op_func(left, right, xsink);

   return (double)op_func(left, right, xsink);
}

class QoreNode *BoolStrStrOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
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
   return new QoreNode(rv);
}

bool BoolStrStrOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   
   if (args == 1)
      return op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return op_func(*l, *r, xsink);
}

int64 BoolStrStrOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   
   if (args == 1)
      return (int64)op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return (int64)op_func(*l, *r, xsink);
}

double BoolStrStrOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);   
   if (args == 1)
      return (double)op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return (double)op_func(*l, *r, xsink);
}

class QoreNode *BoolStrRegexOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   assert(right && right->type == NT_REGEX);

   bool rv = op_func(*l, right->val.regex, xsink);
   if (!ref_rv || *xsink)
      return 0;
   return new QoreNode(rv);
}

bool BoolStrRegexOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   assert(right && right->type == NT_REGEX);

   return op_func(*l, right->val.regex, xsink);
}

int64 BoolStrRegexOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   assert(right && right->type == NT_REGEX);

   return (int64)op_func(*l, right->val.regex, xsink);
}

double BoolStrRegexOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   assert(right && right->type == NT_REGEX);

   return (double)op_func(*l, right->val.regex, xsink);
}

class QoreNode *BigIntStrStrOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
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
   return new QoreNode(rv);
}

bool BigIntStrStrOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   
   if (args == 1)
      return (bool)op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return (bool)op_func(*l, *r, xsink);
}

int64 BigIntStrStrOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);
   
   if (args == 1)
      return op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return op_func(*l, *r, xsink);
}

double BigIntStrStrOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   QoreStringValueHelper l(left);   
   if (args == 1)
      return (double)op_func(*l, 0, xsink);

   QoreStringValueHelper r(right);
   return (double)op_func(*l, *r, xsink);
}

class QoreNode *BigIntOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      int64 rv = op_func(left, right, xsink);
      if (!ref_rv || xsink->isException())
	 return NULL;
      return new QoreNode(rv);
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   int64 rv = op_func(left, right, xsink);
   if (!ref_rv || xsink->isException())
      return NULL;
   return new QoreNode(rv);
}

bool BigIntOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      return (bool)op_func(left, right, xsink);
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   return (bool)op_func(left, right, xsink);
}

int64 BigIntOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      return op_func(left, right, xsink);
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL)) {
      right = get_node_type(right, rtype);
      r = right;
   }

   return op_func(left, right, xsink);
}

double BigIntOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      return (double)op_func(left, right, xsink);
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL)) {
      right = get_node_type(right, rtype);
      r = right;
   }

   return (double)op_func(left, right, xsink);
}

class QoreNode *FloatOperatorFunction::eval(QoreNode *left, QoreNode *right, bool ref_rv, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      double rv = op_func(left, right, xsink);
      if (!ref_rv || xsink->isException())
	 return NULL;
      return new QoreNode(rv);
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL)) {
      right = get_node_type(right, rtype);
      r = right;
   }

   double rv = op_func(left, right, xsink);
   if (!ref_rv || xsink->isException())
      return NULL;
   return new QoreNode(rv);
}

bool FloatOperatorFunction::bool_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      return (bool)op_func(left, right, xsink);
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   return (bool)op_func(left, right, xsink);
}

int64 FloatOperatorFunction::bigint_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      return (int64)op_func(left, right, xsink);
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   return (int64)op_func(left, right, xsink);
}

double FloatOperatorFunction::float_eval(QoreNode *left, QoreNode *right, int args, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> l(xsink);

   // convert node type to required argument types for operator if necessary
   if ((left->type != ltype) && (ltype != NT_ALL))
   {
      left = get_node_type(left, ltype);
      l = left;
   }

   if (args == 1) {
      return op_func(left, right, xsink);
   }

   ReferenceHolder<QoreNode> r(xsink);

   // convert node type to required argument types for operator if necessary
   if ((right->type != rtype) && (rtype != NT_ALL))
   {
      right = get_node_type(right, rtype);
      r = right;
   }

   return op_func(left, right, xsink);
}

Operator::Operator(int arg, char *n, char *desc, int n_evalArgs, bool n_effect, bool n_lvalue)
{
   args = arg;
   name = n;
   description = desc;
   evalArgs = n_evalArgs;
   opMatrix = NULL;
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

char *Operator::getName() const
{
   return name;
}

char *Operator::getDescription() const
{
   return description;
}

void Operator::init()
{
   if (!evalArgs || (functions.size() == 1))
      return;
   opMatrix = new int[NUM_VALUE_TYPES][NUM_VALUE_TYPES];
   // create function lookup matrix
   for (int i = 0; i < NUM_VALUE_TYPES; i++)
      for (int j = 0; j < NUM_VALUE_TYPES; j++)
	 opMatrix[i][j] = findFunction(QTM.find(i), QTM.find(j));
}

// if there is no exact match, the first partial match counts as a match
// static method
int Operator::match(const QoreType *ntype, const QoreType *rtype)
{
   // if any type is OK, or an exact match
   if (rtype == NT_ALL || ntype == rtype || (rtype == NT_VARREF && ntype == NT_SELF_VARREF))
      return 1;
   else // otherwise fail
      return 0;
}

int Operator::get_function(QoreNodeEvalOptionalRefHolder &nleft, ExceptionSink *xsink) const
{
   int t;
   // find operator function
   if (functions.size() == 1)
      t = 0;
   else if (nleft->type->getID() < NUM_VALUE_TYPES)
      t = opMatrix[nleft->type->getID()][NT_NOTHING->getID()];
   else
      t = findFunction(nleft->type, NT_NOTHING);
   
   printd(5, "Operator::get_function() found function %d\n", t);
   return t;
}

int Operator::get_function(QoreNodeEvalOptionalRefHolder &nleft, QoreNodeEvalOptionalRefHolder &nright, ExceptionSink *xsink) const
{
   int t;
   // find operator function
   if (functions.size() == 1)
      t = 0;
   else if (nleft->type->getID() < NUM_VALUE_TYPES && nright->type->getID() < NUM_VALUE_TYPES)
      t = opMatrix[nleft->type->getID()][nright->type->getID()];
   else
      t = findFunction(nleft->type, nright->type);

   printd(5, "Operator::get_function() found function %d\n", t);
   return t;
}

// Operator::eval(): return value requires a deref(xsink) afterwards
// there are 3 main cases which have been split into 3 sections as a speed optimization
// 1: evalArgs 1 argument
// 2: evalArgs 2 arguments
// 3: pass-through all arguments
class QoreNode *Operator::eval(QoreNode *left_side, QoreNode *right_side, bool ref_rv, ExceptionSink *xsink) const
{
   printd(5, "evaluating operator %s (0x%08p 0x%08p)\n", description, left_side, right_side);
   if (evalArgs)
   {
      QoreNodeEvalOptionalRefHolder nleft(left_side, xsink);
      if (*xsink)
	 return 0;
      if (!nleft)
	 nleft.assign(false, Nothing);

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
	 nright.assign(false, Nothing);
      
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
bool Operator::bool_eval(QoreNode *left_side, QoreNode *right_side, ExceptionSink *xsink) const
{
   printd(5, "evaluating operator %s (0x%08p 0x%08p)\n", description, left_side, right_side);
   if (evalArgs)
   {
      QoreNodeEvalOptionalRefHolder nleft(left_side, xsink);
      if (*xsink)
	 return 0;
      if (!nleft)
	 nleft.assign(false, Nothing);
      
      int t;
      if (args == 1)
      {
	 if ((t = get_function(nleft, xsink)) == -1)
	    return 0;

	 return functions[t]->bool_eval(*nleft, NULL, 1, xsink);
      }

      QoreNodeEvalOptionalRefHolder nright(right_side, xsink);
      if (*xsink)
	 return 0;
      if (!nright)
	 nright.assign(false, Nothing);
      
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
int64 Operator::bigint_eval(QoreNode *left, QoreNode *right, ExceptionSink *xsink) const
{
   printd(5, "evaluating operator %s (0x%08p 0x%08p)\n", description, left, right);
   if (evalArgs)
   {
      QoreNodeEvalOptionalRefHolder nleft(left, xsink);
      if (*xsink)
	 return 0;
      if (!nleft)
	 nleft.assign(false, Nothing);

      int t;

      if (args == 1)
      {
	 if ((t = get_function(nleft, xsink)) == -1)
	    return 0;

	 return functions[t]->bigint_eval(*nleft, NULL, 1, xsink);
      }

      QoreNodeEvalOptionalRefHolder nright(right, xsink);
      if (*xsink)
	 return 0;
      if (!nright)
	 nright.assign(false, Nothing);
	 
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
double Operator::float_eval(QoreNode *left, QoreNode *right, ExceptionSink *xsink) const
{
   printd(5, "evaluating operator %s (0x%08p 0x%08p)\n", description, left, right);
   if (evalArgs)
   {
      QoreNodeEvalOptionalRefHolder nleft(left, xsink);
      if (*xsink)
	 return 0;
      if (!nleft)
	 nleft.assign(false, Nothing);

      int t;

      if (args == 1)
      {
	 if ((t = get_function(nleft, xsink)) == -1)
	    return 0;

	 return functions[t]->float_eval(*nleft, NULL, 1, xsink);
      }

      QoreNodeEvalOptionalRefHolder nright(right, xsink);
      if (*xsink)
	 return 0;
      if (!nright)
	 nright.assign(false, Nothing);
	 
      // find operator function
      if ((t = get_function(nleft, nright, xsink)) == -1)
	 return 0;

      return functions[t]->float_eval(*nleft, *nright, 2, xsink);
   }

   // in this case there will only be one entry (0)
   printd(5, "Operator::float_eval() evaluating function 0\n");
   return functions[0]->float_eval(left, right, args, xsink);
}

int Operator::findFunction(const QoreType *ltype, const QoreType *rtype) const
{
   int m = -1;
   
   //tracein("Operator::findFunction()");
   // loop through all operator functions
   
   for (int i = 0, size = functions.size(); i < size; i++)
   {
      // check for a match on the left side
      if (match(ltype, functions[i]->ltype))
      {
	 /* if there is only one operator or there is also
	 * a match on the right side, return */
	 if ((args == 1) || 
	     ((args == 2) && match(rtype, functions[i]->rtype)))
	    return i;
	 if (m == -1)
	    m = i;
	 continue;
      }
      if ((args == 2) && match(rtype, functions[i]->rtype) 
	  && (m == -1))
	 m = i;
   }
   /* if there is no match of any kind, take the highest priority function
      * (row 0), and try to convert the arguments, otherwise return the best 
      * partial match
      */
   //traceout("Operator::findFunction()");
   return m == -1 ? 0 : m;
}

void Operator::addFunction(const QoreType *lt, const QoreType *rt, op_func_t f)
{
   functions.push_back(new OperatorFunction(lt, rt, f));
}

void Operator::addFunction(const QoreType *lt, const QoreType *rt, op_bool_func_t f)
{
   functions.push_back(new BoolOperatorFunction(lt, rt, f));
}

void Operator::addFunction(const QoreType *lt, const QoreType *rt, op_bigint_func_t f)
{
   functions.push_back(new BigIntOperatorFunction(lt, rt, f));
}

void Operator::addFunction(const QoreType *lt, const QoreType *rt, op_float_func_t f)
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
void OperatorList::init()
{
   tracein("OperatorList::init()");
   
   OP_LOG_AND = add(new Operator(2, "&&", "logical-and", 0, false));
   OP_LOG_AND->addEffectFunction(op_log_and);

   OP_LOG_OR = add(new Operator(2, "||", "logical-or", 0, false));
   OP_LOG_OR->addEffectFunction(op_log_or);
   
   OP_LOG_LT = add(new Operator(2, "<", "less-than", 1, false));
   OP_LOG_LT->addFunction(NT_FLOAT,  NT_FLOAT,  op_log_lt_float);
   OP_LOG_LT->addFunction(NT_INT,    NT_INT,    op_log_lt_bigint);
   OP_LOG_LT->addFunction(op_log_lt_string);
   OP_LOG_LT->addFunction(NT_DATE,   NT_DATE,   op_log_lt_date);
   
   OP_LOG_GT = add(new Operator(2, ">", "greater-than", 1, false));
   OP_LOG_GT->addFunction(NT_FLOAT,  NT_FLOAT,  op_log_gt_float);
   OP_LOG_GT->addFunction(NT_INT,    NT_INT,    op_log_gt_bigint);
   OP_LOG_GT->addFunction(op_log_gt_string);
   OP_LOG_GT->addFunction(NT_DATE,   NT_DATE,   op_log_gt_date);

   OP_LOG_EQ = add(new Operator(2, "==", "logical-equals", 1, false));
   OP_LOG_EQ->addFunction(op_log_eq_string);
   OP_LOG_EQ->addFunction(NT_FLOAT,   NT_FLOAT,   op_log_eq_float);
   OP_LOG_EQ->addFunction(NT_INT,     NT_INT,     op_log_eq_bigint);
   OP_LOG_EQ->addFunction(NT_BOOLEAN, NT_BOOLEAN, op_log_eq_boolean);
   OP_LOG_EQ->addFunction(NT_DATE,    NT_DATE,    op_log_eq_date);
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

   OP_LOG_NE = add(new Operator(2, "!=", "not-equals", 1, false));
   OP_LOG_NE->addFunction(op_log_ne_string);
   OP_LOG_NE->addFunction(NT_FLOAT,   NT_FLOAT,   op_log_ne_float);
   OP_LOG_NE->addFunction(NT_INT,     NT_INT,     op_log_ne_bigint);
   OP_LOG_NE->addFunction(NT_BOOLEAN, NT_BOOLEAN, op_log_ne_boolean);
   OP_LOG_NE->addFunction(NT_DATE,    NT_DATE,    op_log_ne_date);
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
   
   OP_LOG_LE = add(new Operator(2, "<=", "less-than-or-equals", 1, false));
   OP_LOG_LE->addFunction(NT_FLOAT,  NT_FLOAT,  op_log_le_float);
   OP_LOG_LE->addFunction(NT_INT,    NT_INT,    op_log_le_bigint);
   OP_LOG_LE->addFunction(op_log_le_string);
   OP_LOG_LE->addFunction(NT_DATE,   NT_DATE,   op_log_le_date);

   OP_LOG_GE = add(new Operator(2, ">=", "greater-than-or-equals", 1, false));
   OP_LOG_GE->addFunction(NT_FLOAT,  NT_FLOAT,  op_log_ge_float);
   OP_LOG_GE->addFunction(NT_INT,    NT_INT,    op_log_ge_bigint);
   OP_LOG_GE->addFunction(op_log_ge_string);
   OP_LOG_GE->addFunction(NT_DATE,   NT_DATE,   op_log_ge_date);

   OP_ABSOLUTE_EQ = add(new Operator(2, "===", "absolute logical-equals", 0, false));
   OP_ABSOLUTE_EQ->addFunction(NT_ALL, NT_ALL, op_absolute_log_eq);
   
   OP_ABSOLUTE_NE = add(new Operator(2, "!==", "absolute logical-not-equals", 0, false));
   OP_ABSOLUTE_NE->addFunction(NT_ALL, NT_ALL, op_absolute_log_neq);
   
   OP_REGEX_MATCH = add(new Operator(2, "=~", "regular expression match", 1, false));
   OP_REGEX_MATCH->addFunction(op_regex_match);
   
   OP_REGEX_NMATCH = add(new Operator(2, "!~", "regular expression negative match", 1, false));
   OP_REGEX_NMATCH->addFunction(op_regex_nmatch);

   OP_EXISTS = add(new Operator(1, "exists", "exists", 0, false));
   OP_EXISTS->addFunction(NT_ALL, NT_NONE, op_exists);
   
   OP_INSTANCEOF = add(new Operator(2, "instanceof", "instanceof", 0, false));
   OP_INSTANCEOF->addFunction(NT_ALL, NT_CLASSREF, op_instanceof);
   
   OP_NOT = add(new Operator(1, "!", "logical-not", 1, false));
   OP_NOT->addFunction(NT_BOOLEAN, NT_NONE, op_log_not_boolean);
      
   // bigint operators
   OP_LOG_CMP = add(new Operator(2, "<=>", "logical-comparison", 1, false));
   OP_LOG_CMP->addFunction(op_cmp_string);
   OP_LOG_CMP->addFunction(NT_FLOAT,  NT_FLOAT,  op_cmp_double);
   OP_LOG_CMP->addFunction(NT_INT,    NT_INT,    op_cmp_bigint);
   OP_LOG_CMP->addFunction(NT_DATE,   NT_DATE,   op_cmp_date);

   OP_ELEMENTS = add(new Operator(1, "elements", "number of elements", 0, false));
   OP_ELEMENTS->addFunction(NT_ALL, NT_NONE, op_elements);

   OP_MODULA = add(new Operator(2, "%", "modula", 1, false));
   OP_MODULA->addFunction(NT_INT, NT_INT, op_modula_int);

   // non-boolean operators
   OP_ASSIGNMENT = add(new Operator(2, "=", "assignment", 0, true, true));
   OP_ASSIGNMENT->addFunction(NT_ALL, NT_ALL, op_assignment);
   
   OP_LIST_ASSIGNMENT = add(new Operator(2, "(list) =", "list assignment", 0, true, true));
   OP_LIST_ASSIGNMENT->addFunction(NT_ALL, NT_ALL, op_list_assignment);
   
   OP_BIN_AND = add(new Operator(2, "&", "binary-and", 1, false));
   OP_BIN_AND->addFunction(NT_INT, NT_INT, op_bin_and_int);

   OP_BIN_OR = add(new Operator(2, "|", "binary-or", 1, false));
   OP_BIN_OR->addFunction(NT_INT, NT_INT, op_bin_or_int);

   OP_BIN_NOT = add(new Operator(1, "~", "binary-not", 1, false));
   OP_BIN_NOT->addFunction(NT_INT, NT_NONE, op_bin_not_int);

   OP_BIN_XOR = add(new Operator(2, "^", "binary-xor", 1, false));
   OP_BIN_XOR->addFunction(NT_INT, NT_INT, op_bin_xor_int);

   OP_MINUS = add(new Operator(2, "-", "minus", 1, false));
   OP_MINUS->addFunction(NT_DATE,  NT_DATE,    op_minus_date);
   OP_MINUS->addFunction(NT_FLOAT, NT_FLOAT,   op_minus_float);
   OP_MINUS->addFunction(NT_INT,   NT_INT,     op_minus_bigint);
   //OP_MINUS->addFunction(NT_INT,   NT_NOTHING, op_minus_bigint);
   OP_MINUS->addFunction(op_minus_hash_string);
   OP_MINUS->addFunction(NT_ALL, NT_ALL, op_minus_hash_list);

   OP_PLUS = add(new Operator(2, "+", "plus", 1, false));
   OP_PLUS->addFunction(NT_LIST,    NT_LIST,   op_plus_list);
   OP_PLUS->addFunction(op_plus_string);
   OP_PLUS->addFunction(NT_DATE,    NT_DATE,   op_plus_date);
   OP_PLUS->addFunction(NT_FLOAT,   NT_FLOAT,  op_plus_float);
   OP_PLUS->addFunction(NT_INT,     NT_INT,    op_plus_bigint);
   OP_PLUS->addFunction(NT_HASH,    NT_HASH,   op_plus_hash_hash);
   OP_PLUS->addFunction(NT_HASH,    NT_OBJECT, op_plus_hash_object);
   OP_PLUS->addFunction(NT_OBJECT,  NT_HASH,   op_plus_object_hash);
   //OP_PLUS->addFunction(NT_NOTHING, NT_INT,    op_plus_bigint);

   OP_MULT = add(new Operator(2, "*", "multiply", 1, false));
   OP_MULT->addFunction(NT_FLOAT, NT_FLOAT, op_multiply_float);
   OP_MULT->addFunction(NT_INT,   NT_INT,   op_multiply_bigint);

   OP_DIV = add(new Operator(2, "/", "divide", 1, false));
   OP_DIV->addFunction(NT_FLOAT, NT_FLOAT, op_divide_float);
   OP_DIV->addFunction(NT_INT,   NT_INT,   op_divide_bigint);

   OP_UNARY_MINUS = add(new Operator(1, "-", "unary-minus", 1, false));
   OP_UNARY_MINUS->addFunction(NT_FLOAT, NT_NONE, op_unary_minus_float);
   OP_UNARY_MINUS->addFunction(NT_INT,   NT_NONE, op_unary_minus_bigint);

   OP_SHIFT_LEFT = add(new Operator(2, "<<", "shift-left", 1, false));
   OP_SHIFT_LEFT->addFunction(NT_INT, NT_INT, op_shift_left_int);

   OP_SHIFT_RIGHT = add(new Operator(2, ">>", "shift-right", 1, false));
   OP_SHIFT_RIGHT->addFunction(NT_INT, NT_INT, op_shift_right_int);

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
   OP_AND_EQUALS->addFunction(NT_ALL, NT_INT, op_and_equals);

   OP_OR_EQUALS = add(new Operator(2, "|=", "or-equals", 0, true, true));
   OP_OR_EQUALS->addFunction(NT_ALL, NT_INT, op_or_equals);

   OP_MODULA_EQUALS = add(new Operator(2, "%=", "modula-equals", 0, true, true));
   OP_MODULA_EQUALS->addFunction(NT_ALL, NT_INT, op_modula_equals);

   OP_MULTIPLY_EQUALS = add(new Operator(2, "*=", "multiply-equals", 0, true, true));
   OP_MULTIPLY_EQUALS->addFunction(op_multiply_equals);

   OP_DIVIDE_EQUALS = add(new Operator(2, "/=", "divide-equals", 0, true, true));
   OP_DIVIDE_EQUALS->addFunction(op_divide_equals);

   OP_XOR_EQUALS = add(new Operator(2, "^=", "xor-equals", 0, true, true));
   OP_XOR_EQUALS->addFunction(NT_ALL, NT_INT, op_xor_equals);

   OP_SHIFT_LEFT_EQUALS = add(new Operator(2, "<<=", "shift-left-equals", 0, true, true));
   OP_SHIFT_LEFT_EQUALS->addFunction(NT_ALL, NT_INT, op_shift_left_equals);

   OP_SHIFT_RIGHT_EQUALS = add(new Operator(2, ">>=", "shift-right-equals", 0, true, true));
   OP_SHIFT_RIGHT_EQUALS->addFunction(NT_ALL, NT_INT, op_shift_right_equals);

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

   OP_REGEX_EXTRACT = add(new Operator(2, "regular expression subpattern extraction", "regular expression subpattern extraction", 1, false));
   OP_REGEX_EXTRACT->addFunction(op_regex_extract);

   OP_CHOMP = add(new Operator(1, "chomp", "chomp EOL marker from lvalue", 0, true, true));
   OP_CHOMP->addFunction(NT_ALL, NT_NONE, op_chomp);

   OP_TRIM = add(new Operator(1, "trim", "trim characters from an lvalue", 0, true, true));
   OP_TRIM->addFunction(NT_ALL, NT_NONE, op_trim);

   // initialize all operators
   for (oplist_t::iterator i = begin(); i != end(); i++)
      (*i)->init();

   traceout("OperatorList::init()");
}
