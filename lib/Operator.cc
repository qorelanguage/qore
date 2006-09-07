/*
  Operator.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/Operator.h>
#include <qore/QoreNode.h>
#include <qore/Variable.h>
#include <qore/support.h>
#include <qore/List.h>
#include <qore/Object.h>
#include <qore/Exception.h>
#include <qore/QoreString.h>
#include <qore/Function.h>
#include <qore/Context.h>
#include <qore/Namespace.h>
#include <qore/BinaryObject.h>
#include <qore/RegexSubst.h>
#include <qore/RegexTrans.h>
#include <qore/QoreRegex.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pcre.h>

class OperatorList oplist;

// here are the standard, system-default operator pointers
class Operator *OP_ASSIGNMENT, *OP_LOG_AND, *OP_LOG_OR, *OP_LOG_LT, 
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
   *OP_REGEX_TRANS, *OP_REGEX_EXTRACT;

// Operator::eval(): return value requires a deref(xsink) afterwards
// there are 3 main cases which have been split into 3 sections as a speed optimization
// 1: evalArgs 1 argument
// 2: evalArgs 2 arguments
// 3: pass-through all arguments
class QoreNode *Operator::eval(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode *result;

   tracein("Operator::eval()");
   printd(5, "evaluating operator %s (0x%08p 0x%08p)\n", description, left, right);
   if (evalArgs)
   {
      if (args == 1)
      {
	 if (left)
	 {
	    left = left->eval(xsink);
	    if (xsink->isEvent())
	    {
	       if (left) left->deref(xsink);
	       return NULL;
	    }
	 }
	 if (!left)
	    left = nothing();

	 int t;
	 // find operator function
	 if (numFunctions == 1)
	    t = 0;
	 else if (left->type->getID() < NUM_VALUE_TYPES)
	    t = opMatrix[left->type->getID()][NT_NOTHING->getID()];
	 else
	    t = findFunction(left->type, NT_NOTHING);
	 
	 printd(5, "Operator::eval() found function %d\n", t);	 
	 // convert node type to required argument types for operator if necessary
	 if ((left->type != functions[t].ltype) && (functions[t].ltype != NT_ALL))
	 {
	    QoreNode *nl = functions[t].ltype->convertTo(left, xsink);
	    left->deref(xsink);
	    if (xsink->isEvent())
	    {
	       if (nl) nl->deref(xsink);
	       return NULL;
	    }
	    left = nl;
	 }
	 result = functions[t].op_func(left, NULL, xsink);
	 // dereference converted argument
	 left->deref(xsink);
      }
      else // 2 arguments
      {
	 if (left)
	 {
	    left = left->eval(xsink);
	    if (xsink->isEvent())
	    {
	       if (left) left->deref(xsink);
	       return NULL;
	    }
	 }
	 if (!left)
	    left = nothing();

	 if (right) 
	 {
	    right = right->eval(xsink);
	    if (xsink->isEvent())
	    {
	       left->deref(xsink);
	       if (right) right->deref(xsink);
	       return NULL;
	    }
	 }
	 if (!right)
	    right = nothing();

	 int t;
	 // find operator function
	 if (numFunctions == 1)
	    t = 0;
	 else if (left->type->getID() < NUM_VALUE_TYPES && right->type->getID() < NUM_VALUE_TYPES)
	    t = opMatrix[left->type->getID()][right->type->getID()];
	 else
	    t = findFunction(left->type, right->type);
	 
	 printd(5, "Operator::eval() found function %d\n", t);
	 
	 // convert node type to required argument types for operator if necessary
	 // convert left node
	 if ((left->type != functions[t].ltype) && 
	     (functions[t].ltype != NT_ALL))
	 {
	    QoreNode *nl = functions[t].ltype->convertTo(left, xsink);
	    left->deref(xsink);
	    if (xsink->isEvent())
	    {
	       if (nl) nl->deref(xsink);
	       return NULL;
	    }
	    left = nl;
	 }
	 // convert right node if necessary
	 if ((right->type != functions[t].rtype) && (functions[t].rtype != NT_ALL))
	 {
	    QoreNode *nr = functions[t].rtype->convertTo(right, xsink);
	    right->deref(xsink);
	    if (xsink->isEvent())
	    {
	       if (left) left->deref(xsink);
	       if (nr) nr->deref(xsink);
	       return NULL;
	    }
	    right = nr;
	 }
	 result = functions[t].op_func(left, right, xsink);
	 // dereference converted arguments
	 left->deref(xsink);
	 right->deref(xsink);
      }
   }
   else
   {
      // in this case there will only be one entry (0)
      printd(5, "Operator::eval() evaluating function 0\n");
      result = functions[0].op_func(left, right, xsink);
   }
   traceout("Operator::eval()");
   return result;
}

// operator functions for builtin types
static class QoreNode *op_log_lt_bigint(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.intval < right->val.intval));
}

static class QoreNode *op_log_gt_bigint(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   //printd(1, "op_log_gt_bigint() left=%lld right=%lld result=%d\n", left->val.intval, right->val.intval, (left->val.intval > right->val.intval));
   return new QoreNode((bool)(left->val.intval > right->val.intval));
}

static class QoreNode *op_log_eq_bigint(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.intval == right->val.intval));
}

static class QoreNode *op_log_ne_bigint(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.intval != right->val.intval));
}

static class QoreNode *op_log_le_bigint(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.intval <= right->val.intval));
}

static class QoreNode *op_log_ge_bigint(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.intval >= right->val.intval));
}

static class QoreNode *op_cmp_bigint(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, left->val.intval - right->val.intval);
}

static class QoreNode *op_minus_bigint(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, left->val.intval - right->val.intval);
}

static class QoreNode *op_plus_bigint(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, left->val.intval + right->val.intval);
}

static class QoreNode *op_multiply_bigint(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, left->val.intval * right->val.intval);
}

static class QoreNode *op_divide_bigint(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   if (!right->val.intval)
   {
      xsink->raiseException("DIVISION-BY-ZERO", "division by zero in integer expression");
      return NULL;
   }
   return new QoreNode(NT_INT, left->val.intval / right->val.intval);
}

static class QoreNode *op_unary_minus_bigint(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, -left->val.intval);
}

static class QoreNode *op_log_eq_binary(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(!left->val.bin->compare(right->val.bin)));
}

static class QoreNode *op_log_ne_binary(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.bin->compare(right->val.bin)));
}

static class QoreNode *op_log_eq_boolean(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.boolval == right->val.boolval));
}

static class QoreNode *op_log_ne_boolean(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.boolval != right->val.boolval));
}

static class QoreNode *op_log_not_boolean(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)!left->val.boolval);
}

static class QoreNode *op_minus_date(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
    return new QoreNode(left->val.date_time->subtractBy(right->val.date_time));
}

static class QoreNode *op_plus_date(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
    return new QoreNode(left->val.date_time->add(right->val.date_time));
}

static class QoreNode *op_log_eq_date(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(left->val.date_time->isEqual(right->val.date_time));
}

static class QoreNode *op_log_gt_date(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(DateTime::compareDates(left->val.date_time, right->val.date_time) > 0));
}

static class QoreNode *op_log_ge_date(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(DateTime::compareDates(left->val.date_time, right->val.date_time) >= 0));
}

static class QoreNode *op_log_lt_date(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(DateTime::compareDates(left->val.date_time, right->val.date_time) < 0));
}

static class QoreNode *op_log_le_date(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(DateTime::compareDates(left->val.date_time, right->val.date_time) <= 0));
}

static class QoreNode *op_cmp_date(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, DateTime::compareDates(left->val.date_time, right->val.date_time));
}

static class QoreNode *op_log_ne_date(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)!left->val.date_time->isEqual(right->val.date_time));
}

static class QoreNode *op_log_lt_float(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.floatval < right->val.floatval));
}

static class QoreNode *op_log_gt_float(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.floatval > right->val.floatval));
}

static class QoreNode *op_log_eq_float(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.floatval == right->val.floatval));
}

static class QoreNode *op_log_ne_float(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.floatval != right->val.floatval));
}

static class QoreNode *op_log_le_float(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.floatval <= right->val.floatval));
}

static class QoreNode *op_log_ge_float(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.floatval >= right->val.floatval));
}

static class QoreNode *op_minus_float(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(left->val.floatval - right->val.floatval);
}

static class QoreNode *op_plus_float(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(left->val.floatval + right->val.floatval);
}

static class QoreNode *op_multiply_float(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(left->val.floatval * right->val.floatval);
}

static class QoreNode *op_divide_float(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{

   if (!right->val.floatval)
   {
      xsink->raiseException("DIVISION-BY-ZERO", "division by zero in floating-point expression!");
      return NULL;
   }
   return new QoreNode(left->val.floatval / right->val.floatval);
}

static class QoreNode *op_unary_minus_float(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(-left->val.floatval);
}

static class QoreNode *op_log_eq_string(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
//   tracein("op_log_eq_string()");
/*
   printd(5, "OLES() %08p %08p\n", left, right);
   printd(5, "OLES() %d %d %08p %08p\n", //\"%s\" == \"%s\"\n", 
	  left->type, right->type, left->val.c_str, right->val.c_str);
*/
//   traceout("op_log_eq_string()");
   return new QoreNode((bool)!left->val.String->compareSoft(right->val.String, xsink));
}

static class QoreNode *op_plus_string(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode *node;

   printd(5, "op_plus_string() (%d) \"%s\" + (%d) \"%s\"\n", 
	  left->val.String->strlen(),
	  left->val.String->getBuffer(),
	  right->val.String->strlen(),
	  right->val.String->getBuffer());

   node = new QoreNode(NT_STRING);
   node->val.String = new QoreString(left->val.String);

   node->val.String->concat(right->val.String, xsink);
   printd(5, "op_plus_string() result=\"%s\"\n", node->val.String->getBuffer());
   return node;
}

static class QoreNode *op_log_gt_string(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.String->compare(right->val.String) > 0));
}

static class QoreNode *op_log_ge_string(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(right->val.String->compare(left->val.String) >= 0));
}

static class QoreNode *op_log_lt_string(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.String->compare(right->val.String) < 0));
}

static class QoreNode *op_log_le_string(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)(left->val.String->compare(right->val.String) <= 0));
}

static class QoreNode *op_cmp_string(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, left->val.String->compare(right->val.String));
}

static class QoreNode *op_log_ne_string(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode((bool)left->val.String->compare(right->val.String));
}

static class QoreNode *op_elements(class QoreNode *left, class QoreNode *null, ExceptionSink *xsink)
{
   tracein("op_elements()");
   if (!left)
      return NULL;

   QoreNode *np = left->eval(xsink);

   if (!np)
      return NULL;

   if (xsink->isEvent())
   {
      np->deref(xsink);
      return NULL;
   }

   int size;

   if (np->type == NT_LIST)
      size = np->val.list->size();
   else if (np->type == NT_OBJECT)
      size = np->val.object->size();
   else if (np->type == NT_HASH)
      size = np->val.hash->size();
   else if (np->type == NT_BINARY)
      size = np->val.bin->size();
   else if (np->type == NT_STRING)
      size = np->val.String->length();
   else
   {
      np->deref(xsink);
      return NULL;
   }

   np->deref(xsink);
   traceout("op_elements()");
   return new QoreNode((int64)size);
}

static class QoreNode *op_keys(class QoreNode *left, class QoreNode *null, ExceptionSink *xsink)
{
   if (!left)
      return NULL;

   QoreNode *np = left->eval(xsink);

   if (xsink->isEvent() || !np || (np->type != NT_OBJECT && np->type != NT_HASH))
   {
      if (np) np->deref(xsink);
      return NULL;
   }
   
   class List *l;
   if (np->type == NT_OBJECT)
      l = np->val.object->getMemberList(xsink);
   else
      l = np->val.hash->getKeys();
   if (np) np->deref(xsink);
   if (l)
      return new QoreNode(l);
   return NULL;
}

static class QoreNode *op_question_mark(class QoreNode *left, class QoreNode *list, ExceptionSink *xsink)
{
   bool b = left->boolEval(xsink);
   if (xsink->isEvent())
      return NULL;
   if (b)
      return list->val.list->retrieve_entry(0)->eval(xsink);
   else
      return list->val.list->retrieve_entry(1)->eval(xsink);
}

static class QoreNode *op_absolute_log_eq(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   QoreNode *lnp = left->eval(xsink);
   if (xsink->isEvent())
   {
      if (lnp) lnp->deref(xsink);
      return NULL;
   }

   class QoreNode *rv;
   QoreNode *rnp = right->eval(xsink);

   if (!xsink->isEvent())
      rv = new QoreNode((bool)!compareHard(lnp, rnp));
   else
      rv = NULL;

   if (lnp) lnp->deref(xsink);      
   if (rnp) rnp->deref(xsink);
   return rv;
}

static class QoreNode *op_absolute_log_neq(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   QoreNode *lnp = left->eval(xsink);
   if (xsink->isEvent())
   {
      if (lnp) lnp->deref(xsink);
      return NULL;
   }

   class QoreNode *rv;
   QoreNode *rnp = right->eval(xsink);

   if (!xsink->isEvent())
      rv = new QoreNode((bool)compareHard(lnp, rnp));
   else
      rv = NULL;

   if (lnp) lnp->deref(xsink);      
   if (rnp) rnp->deref(xsink);
   return rv;
}

static class QoreNode *op_regex_match(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(right->val.regex->exec(left->val.String, xsink));
}

static class QoreNode *op_regex_nmatch(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(!right->val.regex->exec(left->val.String, xsink));
}

static class QoreNode *op_regex_subst(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   // get current value and save
   class VLock vl;
   class QoreNode **v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // if it's not a string, then do nothing
   if (!(*v) || (*v)->type != NT_STRING)
      return NULL;

   QoreString *nv = right->val.resub->exec((*v)->val.String, xsink);
   if (xsink->isEvent())
      return NULL;

   // assign new value to lvalue
   (*v)->deref(xsink);
   (*v) = new QoreNode(nv);
   // reference for return value
   (*v)->ref();
   return (*v);      
}

static class QoreNode *op_regex_trans(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   // get current value and save
   class VLock vl;
   class QoreNode **v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // if it's not a string, then do nothing
   if (!(*v) || (*v)->type != NT_STRING)
      return NULL;

   QoreString *nv = right->val.retrans->exec((*v)->val.String, xsink);
   if (xsink->isEvent())
      return NULL;

   // assign new value to lvalue
   (*v)->deref(xsink);
   (*v) = new QoreNode(nv);
   // reference for return value
   (*v)->ref();
   return (*v);      
}

static class QoreNode *op_list_ref(class QoreNode *left, class QoreNode *index, ExceptionSink *xsink)
{
   QoreNode *lp = left->eval(xsink);

   // return NULL if left side is not a list (or exception)
   if (xsink->isEvent() || !lp || lp->type != NT_LIST)
   {
      if (lp) lp->deref(xsink);
      return NULL;
   }

   // retrieve value and reference for return if there
   class QoreNode *rv;
   if ((rv = lp->val.list->retrieve_entry(index->integerEval(xsink))))
      rv->ref();
   //printd(5, "op_list_ref() index=%d, rv=%08p\n", ind, rv);
   if (lp) lp->deref(xsink);
   return rv;
}

// for the member name, a string is required.  non-string arguments will
// be converted.  The null string can also be used
static class QoreNode *op_object_ref(class QoreNode *left, class QoreNode *member, ExceptionSink *xsink)
{

   TempNode *tn = NULL;
   class VLock vl;
   QoreNode *op = getExistingVarValueWithMethod(left, xsink, &vl, &tn);

   // return NULL if left side is not an object (or exception)
   if (xsink->isEvent() || !op || (op->type != NT_OBJECT && op->type != NT_HASH))
   {
      vl.del();
      if (tn) tn->del(xsink);
      return NULL;
   }

   // evaluate member expression
   if (!member || (!(member = member->eval(xsink))))
   {
      if (xsink->isEvent())
      {
	 vl.del();
	 if (tn) tn->del(xsink);
	 return NULL;
      }
      member = null_string();
   }
   else if (member->type != NT_STRING)
   {
      class QoreNode *nm = member->convert(NT_STRING);
      member->deref(xsink);
      member = nm;
   }

   class QoreNode *rv;

   if (op->type == NT_HASH)
   {
      rv = op->val.hash->evalKey(member->val.String->getBuffer(), xsink);
      vl.del();
   }
   else
   {
      Object *obj = op->val.object;
      obj->ref();
      vl.del();
      //printd(5, "op_object_ref() %08p (%s)\n", *op, member->val.String->getBuffer());
      //printd(5, "size=%d mem=\"%s\"\n", (*op)->val.object->size(), member->val.String->getBuffer());
      // retrieve value and reference for return if there
      //class QoreNode *rv = op->val.object->retrieve_value(member->val.String->getBuffer());
      rv = obj->evalMember(member, xsink);
      obj->dereference(xsink);
   }
   if (tn) tn->del(xsink);
   member->deref(xsink);
   return rv;
}

static class QoreNode *op_object_method_call(class QoreNode *left, class QoreNode *func, ExceptionSink *xsink)
{
   tracein("op_object_method_call()");
   TempNode *tn = NULL;

   class VLock vl;
   QoreNode *op = getExistingVarValueWithMethod(left, xsink, &vl, &tn);

   // return if an exception happened
   if (xsink->isEvent())
   {
      vl.del();
      if (tn) tn->del(xsink);
      traceout("op_object_method_call()");
      return NULL;
   }

   if (!op || op->type != NT_OBJECT)
   {
      xsink->raiseException("OBJECT-METHOD-EVAL-ON-NON-OBJECT", "member function \"%s\" called on type \"%s\"", 
		     func->val.fcall->f.c_str, op ? op->type->name : "NOTHING" );
      vl.del();
      if (tn) tn->del(xsink);
      traceout("op_object_method_call()");
      return NULL;
   }
   Object *obj = op->val.object;
   obj->ref();
   printd(5, "op_object_method_call() %s:%s() op=%08p, op->val.object=%08p obj=%08p class=%08p (stack=%08p)\n",
	  obj->getClass()->name, func->val.fcall->f.c_str, op, op->val.object, obj, obj->getClass(), getStackClass());
   vl.del();
   QoreNode *rv = obj->getClass()->evalMethod(obj, func->val.fcall->f.c_str, func->val.fcall->args, xsink);
   obj->dereference(xsink);

   if (tn) tn->del(xsink);
   traceout("op_object_method_call()");
   return rv;
}

static class QoreNode *op_new_object(class QoreNode *left, class QoreNode *x, ExceptionSink *xsink)
{
   tracein("op_new_object()");

   class QoreNode *rv = left->val.socall->oc->execConstructor(left->val.socall->args, xsink);
   printd(5, "op_new_object() returning node=%08p (type=%s)\n", rv, left->val.socall->oc->name);
   // if there's an exception, the constructor will delete the object without the destructor
   traceout("op_new_object()");
   return rv;
}

static class QoreNode *op_assignment(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v, *new_value;

   // tracein("op_assignment()");

   /* assign new value, this value gets referenced with the
      eval(xsink) call, so there's no need to reference it again
      for the variable assignment - however it does need to be
      copied/referenced for the return value
   */
   new_value = right->eval(xsink);
   if (xsink->isEvent())
   {
      discard(new_value, xsink);
      return NULL;
   }

   // get current value and save
   class VLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
   {
      vl.del();
      discard(new_value, xsink);
      return NULL;
   }

   // dereference old value if necessary
   discard(*v, xsink);
   if (xsink->isEvent())
   {
      *v = NULL;
      vl.del();
      discard(new_value, xsink);
      return NULL;
   }

   // assign new value 
   (*v) = new_value;
   vl.del();

#if 0
   printd(5, "op_assignment() *%08p=%08p (type=%s refs=%d)\n",
	  v, new_value, 
	  new_value ? new_value->type->name : "(null)",
	  new_value ? new_value->reference_count() : 0 );
#endif

   // copy/reference return value if exists
   if (new_value)
      new_value->ref();

   // traceout("op_assignment()");
   return new_value;
}

static class QoreNode *op_list_assignment(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v, *new_value;

   // tracein("op_assignment()");

   /* assign new value, this value gets referenced with the
      eval(xsink) call, so there's no need to reference it again
      for the variable assignment - however it does need to be
      copied/referenced for the return value
   */
   new_value = right->eval(xsink);
   if (xsink->isEvent())
   {
      discard(new_value, xsink);
      return NULL;
   }

   // get values and save
   int i;
   for (i = 0; i < left->val.list->size(); i++)
   {
      class QoreNode *lv = left->val.list->retrieve_entry(i);

      class VLock vl;
      v = get_var_value_ptr(lv, &vl, xsink);
      if (xsink->isEvent())
      {
	 vl.del();
	 discard(new_value, xsink);
	 return NULL;
      }
      
      // dereference old value if necessary
      discard(*v, xsink);
      if (xsink->isEvent())
      {
	 *v = NULL;
	 vl.del();
	 discard(new_value, xsink);
	 return NULL;
      }

      // if there's only one value, then save it
      if (!new_value || new_value->type != NT_LIST)
      {
	 if (!i)
	 {
	    (*v) = new_value;

	    // ref for return value if exists
	    if (new_value)
	       new_value->ref();
	 }
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

   // traceout("op_assignment()");
   return new_value;
}

static class QoreNode *op_single_assignment(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v, *new_value;

   // tracein("op_single_assignment()");

   /* assign new value, this value gets referenced with the
      eval(xsink) call, so there's no need to reference it again
      for the variable assignment
   */
   new_value = right->eval(xsink);
   if (xsink->isEvent())
   {
      discard(new_value, xsink);
      return NULL;
   }

   // get current value and save
   class VLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
   {
      vl.del();
      discard(new_value, xsink);
      return NULL;
   }

   // dereference old value if necessary
   discard(*v, xsink);
   if (xsink->isEvent())
   {
      *v = NULL;
      vl.del();
      discard(new_value, xsink);
      return NULL;
   }

   // assign new value 
   (*v) = new_value;

   // traceout("op_single_assignment()");
   return NULL;
}

static class QoreNode *op_plus_equals(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v, *new_right;

   // tracein("op_plus_equals()");
   new_right = right->eval(xsink);
   if (xsink->isEvent())
   {
      if (new_right)
	 new_right->deref(xsink);
      return NULL;
   }
   if (!new_right)
      return NULL;

   // get ptr to current value
   class VLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
   {
      vl.del();
      new_right->deref(xsink);
      return NULL;
   }

   // dereferences happen in each section so that the
   // already referenced value can be passed to list->push()
   // if necessary
   // do list plus-equals if left-hand side is a list
   if (*v && ((*v)->type == NT_LIST))
   {
      ensure_unique(v, xsink);
      if (new_right->type == NT_LIST)
      {
	 (*v)->val.list->merge(new_right->val.list);
	 new_right->deref(xsink);
      }
      else
	 (*v)->val.list->push(new_right);
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
	 class Hash *h = new_right->val.object->evalData(xsink);
	 if (h)
	    (*v)->val.hash->assimilate(h, xsink);
      }
      new_right->deref(xsink);
   }
   // do hash/object plus-equals if left side is an object
   else if (*v && ((*v)->type == NT_OBJECT))
   {
      if (new_right->type == NT_OBJECT)
      {
	 class Hash *h = new_right->val.object->evalData(xsink);
	 if (h)
	    (*v)->val.object->assimilate(h, xsink);
      }
      else if (new_right->type == NT_HASH)
	 (*v)->val.object->merge(new_right->val.hash, xsink);
      new_right->deref(xsink);
   }
   // do string plus-equals if left-hand side is a string
   else if ((*v) && ((*v)->type == NT_STRING))
   {
      if (new_right->type != NT_STRING)
      {
	 class QoreNode *n = new_right->convert(NT_STRING);
	 new_right->deref(xsink);
	 new_right = n;
      }
      ensure_unique(v, xsink);
      (*v)->val.String->concat(new_right->val.String, xsink);
      new_right->deref(xsink);
   }
   else if ((*v) && ((*v)->type == NT_FLOAT))
   {
      if (new_right->type != NT_FLOAT)
      {
	 class QoreNode *n = new_right->convert(NT_FLOAT);
	 new_right->deref(xsink);
	 new_right = n;
      }
      ensure_unique(v, xsink);
      (*v)->val.floatval += new_right->val.floatval;
      new_right->deref(xsink);
   }
   else if ((*v) && ((*v)->type == NT_DATE))
   {
      if (new_right->type != NT_DATE)
      {
	 class QoreNode *n = new_right->convert(NT_DATE);
	 new_right->deref(xsink);
	 new_right = n;
      }
      // get new date value
      class DateTime *nd = (*v)->val.date_time->add(new_right->val.date_time);
      // dereference old value
      (*v)->deref(NULL);
      // assign new value
      (*v) = new QoreNode(nd);
      // dereference evaluated expression
      new_right->deref(xsink);
   }
   else // do integer plus-equals
   {
      // get new value if necessary
      if (!(*v))
      {
	 (*v) = new QoreNode(NT_INT);
	 (*v)->val.intval = 0;
      }
      else
      {
	 if ((*v)->type != NT_INT)
	 {
	    class QoreNode *n = (*v)->convert(NT_INT);
	    (*v)->deref(xsink);
	    (*v) = n;
	 }
	 ensure_unique(v, xsink);
      }

      // increment current value
      (*v)->val.intval += new_right->getAsBigInt();
      new_right->deref(xsink);
   }

   // reference return value
   // traceout("op_plus_equals()");
   return (*v)->RefSelf();
}

static class QoreNode *op_minus_equals(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v, *new_right;

   new_right = right->eval(xsink);
   if (xsink->isEvent())
   {
      if (new_right)
	 new_right->deref(xsink);
      return NULL;
   }
   if (!new_right)
      return NULL;

   // tracein("op_minus_equals()");
   // get ptr to current value

   class VLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
   {
      vl.del();
      new_right->deref(xsink);
      return NULL;
   }

   // do float minus-equals if left side is a float
   if ((*v) && ((*v)->type == NT_FLOAT))
   {
      if (new_right->type != NT_FLOAT)
      {
	 class QoreNode *n = new_right->convert(NT_FLOAT);
	 new_right->deref(xsink);
	 new_right = n;
      }
      ensure_unique(v, xsink);
      (*v)->val.floatval -= new_right->val.floatval;
   }
   else if ((*v) && ((*v)->type == NT_DATE))
   {
      if (new_right->type != NT_DATE)
      {
	 class QoreNode *n = new_right->convert(NT_DATE);
	 new_right->deref(xsink);
	 new_right = n;
      }
      // get new date value
      class DateTime *nd = (*v)->val.date_time->subtractBy(new_right->val.date_time);
      // dereference old value
      (*v)->deref(NULL);
      // assign new value
      (*v) = new QoreNode(nd);
   }
   else if ((*v) && ((*v)->type == NT_HASH))
   {
      if (new_right->type != NT_STRING)
      {
	 class QoreNode *n = new_right->convert(NT_STRING);
	 new_right->deref(xsink);
	 new_right = n;
      }
      ensure_unique(v, xsink);
      (*v)->val.hash->deleteKey(new_right->val.String, xsink);
   }
   else // do integer minus-equals
   {
      // get new value if necessary
      if (!(*v))
      {
	 (*v) = new QoreNode(NT_INT);
	 (*v)->val.intval = 0;
      }
      else 
      {
	 if ((*v)->type != NT_INT)
	 {
	    class QoreNode *n = (*v)->convert(NT_INT);
	    (*v)->deref(xsink);
	    (*v) = n;
	 }
	 ensure_unique(v, xsink);
      }

      // increment current value
      (*v)->val.intval -= new_right->getAsBigInt();
   }
   new_right->deref(xsink);

   // traceout("op_minus_equals()");
   // reference return value and return
   return (*v)->RefSelf();
}

static class QoreNode *op_and_equals(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v;

   //tracein("op_and_equals()");
   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return NULL;

   // get ptr to current value
   class VLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // get new value if necessary
   if (!(*v))
   {
      (*v) = new QoreNode(NT_INT);
      (*v)->val.intval = 0;
   }
   else 
   {
      if ((*v)->type != NT_INT)
      {
	 class QoreNode *n;
	 
	 n = (*v)->convert(NT_INT);
	 (*v)->deref(xsink);
	 (*v) = n;
      }

      ensure_unique(v, xsink);
   }

   // and current value with arg val
   (*v)->val.intval &= val;

   // reference return value and return
   (*v)->ref();
   //traceout("op_and_equals()");
   return *v;
}

static class QoreNode *op_or_equals(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v;

   //tracein("op_or_equals()");

   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return NULL;

   // get ptr to current value
   class VLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // get new value if necessary
   if (!(*v))
   {
      (*v) = new QoreNode(NT_INT);
      (*v)->val.intval = 0;
   }
   else 
   {
      if ((*v)->type != NT_INT)
      {
	 class QoreNode *n;
	 
	 n = (*v)->convert(NT_INT);
	 (*v)->deref(xsink);
	 (*v) = n;
      }

      ensure_unique(v, xsink);
   }

   // or current value with arg val
   (*v)->val.intval |= val;

   // reference return value and return
   if (*v) (*v)->ref();

   //traceout("op_or_equals()");
   return *v;
}

static class QoreNode *op_modula_equals(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v;

   //tracein("op_modula_equals()");

   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return NULL;

   // get ptr to current value
   class VLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // get new value if necessary
   if (!(*v))
   {
      (*v) = new QoreNode(NT_INT);
      (*v)->val.intval = 0;
   }
   else 
   {
      if ((*v)->type != NT_INT)
      {
	 class QoreNode *n;
	 
	 n = (*v)->convert(NT_INT);
	 (*v)->deref(xsink);
	 (*v) = n;
      }

      ensure_unique(v, xsink);
   }

   // or current value with arg val
   (*v)->val.intval %= val;

   // reference return value and return
   if (*v) (*v)->ref();

   //traceout("op_modula_equals()");
   return *v;
}

static class QoreNode *op_multiply_equals(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v;
   //tracein("op_multiply_equals()");

   class QoreNode *res = right->eval(xsink);
   if (xsink->isEvent())
   {
      if (res)
	 res->deref(xsink);
      return NULL;
   }

   // get ptr to current value
   class VLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
   {
      if (res)
	 res->deref(xsink);
      return NULL;
   }

   // is either side a float?
   if (res && res->type == NT_FLOAT)
   {
      if (!(*v))
	 (*v) = new QoreNode((double)0.0);
      else
      {
	 ensure_type(v, NT_FLOAT, xsink);
	 ensure_unique(v, xsink);

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
	    ensure_type(v, NT_INT, xsink);
	    ensure_unique(v, xsink);

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

   // dereference factor expression result
   if (res)
      res->deref(xsink);

   // reference return value and return
   if (*v) (*v)->ref();

   //traceout("op_multiply_equals()");
   return *v;
}

static class QoreNode *op_divide_equals(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v;
   //tracein("op_divide_equals()");
   class QoreNode *res = right->eval(xsink);
   if (xsink->isEvent())
   {
      if (res)
	 res->deref(xsink);
      return NULL;
   }

   // get ptr to current value
   class VLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
   {
      if (res)
	 res->deref(xsink);
      return NULL;
   }

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
	    ensure_type(v, NT_FLOAT, xsink);
	    ensure_unique(v, xsink);

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
	 if (!val)
	    xsink->raiseException("DIVISION-BY-ZERO", "division by zero in floating-point expression!");
	 else
	 {
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
      if (!val)
	 xsink->raiseException("DIVISION-BY-ZERO", "division by zero in integer expression!");
      {
	 // get new value if necessary
	 if (!(*v))
	    (*v) = new QoreNode((int64)0);
	 else 
	 {
	    ensure_type(v, NT_INT, xsink);
	    ensure_unique(v, xsink);
	       
	    // divide current value with arg val
	    (*v)->val.intval /= val;
	 }
      }
   }

   // dereference factor expression result
   if (res)
      res->deref(xsink);

   // reference return value and return
   if (*v) (*v)->ref();

   //traceout("op_divide_equals()");
   return *v;
}

static class QoreNode *op_xor_equals(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v;

   //tracein("op_xor_equals()");

   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return NULL;

   // get ptr to current value
   class VLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // get new value if necessary
   if (!(*v))
   {
      (*v) = new QoreNode(NT_INT);
      (*v)->val.intval = 0;
   }
   else 
   {
      if ((*v)->type != NT_INT)
      {
	 class QoreNode *n;
	 
	 n = (*v)->convert(NT_INT);
	 (*v)->deref(xsink);
	 (*v) = n;
      }

      ensure_unique(v, xsink);
   }

   // xor current value with arg val
   (*v)->val.intval ^= val;

   // reference return value and return
   if (*v) (*v)->ref();

   //traceout("op_xor_equals()");
   return *v;
}

static class QoreNode *op_shift_left_equals(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v;

   //tracein("op_shift_left_equals()");

   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return NULL;

   // get ptr to current value
   class VLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // get new value if necessary
   if (!(*v))
   {
      (*v) = new QoreNode(NT_INT);
      (*v)->val.intval = 0;
   }
   else 
   {
      if ((*v)->type != NT_INT)
      {
	 class QoreNode *n;
	 
	 n = (*v)->convert(NT_INT);
	 (*v)->deref(xsink);
	 (*v) = n;
      }

      ensure_unique(v, xsink);
   }

   // shift left current value by arg val
   (*v)->val.intval <<= val;

   // reference return value and return
   if (*v) (*v)->ref();

   //traceout("op_shift_left_equals()");
   return *v;
}

static class QoreNode *op_shift_right_equals(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **v;

   //tracein("op_shift_right_equals()");

   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return NULL;

   // get ptr to current value
   class VLock vl;
   v = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // get new value if necessary
   if (!(*v))
   {
      (*v) = new QoreNode(NT_INT);
      (*v)->val.intval = 0;
   }
   else 
   {
      if ((*v)->type != NT_INT)
      {
	 class QoreNode *n;
	 
	 n = (*v)->convert(NT_INT);
	 (*v)->deref(xsink);
	 (*v) = n;
      }

      ensure_unique(v, xsink);
   }

   // shift right current value by arg val
   (*v)->val.intval >>= val;

   // reference return value and return
   if (*v) (*v)->ref();

   //traceout("op_shift_right_equals()");
   return *v;
}

// takes all arguments unevaluated so logic short-circuiting can happen
static class QoreNode *op_log_and(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   // if left side is 0, then do not evaluate right side
   bool l = left->boolEval(xsink);
   if (xsink->isEvent())
      return NULL;
   bool b;
   if (!l)
      b = false;
   else
      b = right->boolEval(xsink);

   return new QoreNode(b);
}

// takes all arguments unevaluated so logic short-circuiting can happen
static class QoreNode *op_log_or(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   bool l = left->boolEval(xsink);

   if (xsink->isEvent())
      return NULL;

   bool b;
   // if left side is 1, then do not evaluate right side
   if (l)
      b = true;
   else
      b = right->boolEval(xsink);

   return new QoreNode(b);
}

// "soft" comparison
// 0 = equal, 1 = not equal
static inline bool compare_lists(class List *l, class List *r, ExceptionSink *xsink)
{
   if (l->size() != r->size())
      return 1;
   for (int i = 0; i < l->size(); i++)
   {
      if (compareSoft(l->retrieve_entry(i), r->retrieve_entry(i), xsink))
	 return 1;
      if (xsink->isEvent())
	 return 0;
   }
   return 0;
}

static class QoreNode *op_log_eq_list(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   if ((left->type != NT_LIST) || (right->type != NT_LIST))
      return boolean_false();

   return new QoreNode((bool)!compare_lists(left->val.list, right->val.list, xsink));
}

static class QoreNode *op_log_eq_hash(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   if ((left->type != NT_HASH) || (right->type != NT_HASH))
      return boolean_false();

   bool b = !left->val.hash->compareSoft(right->val.hash, xsink);
   
   return new QoreNode(b);
}

static class QoreNode *op_log_eq_object(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   if ((left->type != NT_OBJECT) || (right->type != NT_OBJECT))
      return boolean_false();

   return new QoreNode((bool)!left->val.object->compareSoft(right->val.object, xsink));
}

static class QoreNode *op_log_eq_nothing(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return boolean_true();
}

static class QoreNode *op_log_eq_null(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   if (left && left->type == NT_NULL && right && right->type == NT_NULL)
      return boolean_true();
   return boolean_false();
}

static class QoreNode *op_log_ne_list(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   bool b;

   if ((left->type != NT_LIST) || (right->type != NT_LIST))
      b = true;
   else
      b = compare_lists(left->val.list, right->val.list, xsink);
   return new QoreNode(b);
}

static class QoreNode *op_log_ne_hash(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   bool b;

   if ((left->type != NT_HASH) || (right->type != NT_HASH))
      b = true;
   else
      b = left->val.hash->compareSoft(right->val.hash, xsink);
   return new QoreNode(b);
}

static class QoreNode *op_log_ne_object(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   bool b;

   if ((left->type != NT_OBJECT) || (right->type != NT_OBJECT))
      b = true;
   else
      b = left->val.object->compareSoft(right->val.object, xsink);
   return new QoreNode(b);
}

static class QoreNode *op_plus_list(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode *node = left->realCopy(xsink);
   node->val.list->merge(right->val.list);
   return node;
}

static class QoreNode *op_plus_hash_hash(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode *node = left->realCopy(xsink);
   node->val.hash->merge(right->val.hash, xsink);
   return node;
}

static class QoreNode *op_plus_hash_object(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class Hash *h = right->val.object->evalData(xsink);
   if (!h)
      return NULL;

   class QoreNode *node = left->realCopy(xsink);
   node->val.hash->assimilate(h, xsink);
   return node;
}

// note that this will return a hash
static class QoreNode *op_plus_object_hash(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class Hash *h = left->val.object->evalData(xsink);
   if (!h)
      return NULL;
   h->merge(right->val.hash, xsink);
   return new QoreNode(h);
}

static class QoreNode *op_cmp_double(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   int rc;
   if (left->val.floatval < right->val.floatval)
      rc = -1;
   else if (left->val.floatval == right->val.floatval)
      rc = 0;
   else
      rc = 1;
   return new QoreNode(NT_INT, rc);
}

static class QoreNode *op_log_ne_nothing(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return boolean_false();
}

static class QoreNode *op_log_ne_null(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   if (left && left->type == NT_NULL && right && right->type == NT_NULL)
      return boolean_false();
   return boolean_true();
}

static class QoreNode *op_modula_int(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, left->val.intval % right->val.intval);
}

static class QoreNode *op_bin_and_int(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, left->val.intval & right->val.intval);
}

static class QoreNode *op_bin_or_int(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, left->val.intval | right->val.intval);
}

static class QoreNode *op_bin_not_int(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, ~left->val.intval);
}

static class QoreNode *op_bin_xor_int(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, (left->val.intval ^ right->val.intval));
}

static class QoreNode *op_shift_left_int(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, left->val.intval << right->val.intval);
}

static class QoreNode *op_shift_right_int(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   return new QoreNode(NT_INT, left->val.intval >> right->val.intval);
}

// variable assignment
static class QoreNode *op_post_inc(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **n, *rv;

   // tracein("op_post_inc()");

   class VLock vl;
   n = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   rv = *n;

   // acquire new value if necessary
   if (!(*n))
      (*n) = new QoreNode(NT_INT, 0);
   else if ((*n)->type != NT_INT)
   {
      class QoreNode *nv = (*n)->convert(NT_INT);

      // get unique value if necessary
      if (nv->reference_count() > 1)
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
static class QoreNode *op_post_dec(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **n, *rv;
   // tracein("op_post_dec()");

   class VLock vl;
   n = get_var_value_ptr(left, &vl, xsink);
   //printd(5, "op_post_dec() n=%08p, *n=%08p\n", n, *n);
   if (xsink->isEvent())
      return NULL;

   // acquire new value if necessary
   if (!(*n))
   {
      (*n) = new QoreNode(NT_INT);
      (*n)->val.intval = 0;
      rv = NULL;
   }
   else if ((*n)->type != NT_INT)
   {
      class QoreNode *nv = (*n)->convert(NT_INT);
      rv = *n;

      // get unique value if necessary
      if (nv->reference_count() > 1)
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
static class QoreNode *op_pre_inc(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **n;
   // tracein("op_pre_inc()");

   class VLock vl;
   n = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // acquire new value if necessary
   if (!(*n))
   {
      (*n) = new QoreNode(NT_INT);
      (*n)->val.intval = 0;
   }
   else
   {
      if ((*n)->type != NT_INT)
      {
	 class QoreNode *nv = (*n)->convert(NT_INT);
	 (*n)->deref(xsink);
	 (*n) = nv;
      }

      ensure_unique(n, xsink);
   }

   // increment value
   (*n)->val.intval++;

   // reference for return value
   // don't need eval for ints
   (*n)->ref();

   // traceout("op_pre_inc");
   return *n;
}

// variable assignment
static class QoreNode *op_pre_dec(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class QoreNode **n;
   // tracein("op_pre_dec()");

   class VLock vl;
   n = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent())
      return NULL;

   // acquire new value if necessary
   if (!(*n))
   {
      (*n) = new QoreNode(NT_INT);
      (*n)->val.intval = 0;
   }
   else
   {
      if ((*n)->type != NT_INT)
      {
	 class QoreNode *nv = (*n)->convert(NT_INT);
	 (*n)->deref(xsink);
	 (*n) = nv;
      }
      
      ensure_unique(n, xsink);
   }

   // decrement value
   (*n)->val.intval--;

   // copy/reference return value
   // don't need eval for ints
   if (*n) (*n)->ref();

   // traceout("op_ppre_dec()");
   return *n;
}

// unshift lvalue, element
static QoreNode *op_unshift(class QoreNode *left, class QoreNode *elem, ExceptionSink *xsink)
{
   //tracein("op_unshift()");
   printd(5, "op_unshift(%08p, %08p, isEvent=%d)\n", left, elem, xsink->isEvent());

   class VLock vl;
   QoreNode **val = get_var_value_ptr(left, &vl, xsink);
   // value is not a list, so throw exception
   if (xsink->isEvent() || !(*val) || (*val)->type != NT_LIST)
   {
      xsink->raiseException("UNSHIFT-ERROR", "first argument to unshift is not a list");
      return NULL;
   }

   ensure_unique(val, xsink);

   printd(5, "op_unshift() *val=%08p (%s)\n", *val, *val ? (*val)->type->name : "(none)");
   printd(5, "op_unshift() about to call unshift() on list node %08p (%d) with element %08p\n", (*val), (*val)->val.list->size(), elem);

   if (elem)
   {
      elem = elem->eval(xsink);
      if (xsink->isEvent())
      {
	 if (elem) elem->deref(xsink);
	 return NULL;
      }
   }

   (*val)->val.list->insert(elem);

   // reference for return value
   (*val)->ref();

   //traceout("op_unshift()");
   return (*val);
}

static QoreNode *op_shift(class QoreNode *left, class QoreNode *x, ExceptionSink *xsink)
{
   //tracein("op_shift()");
   printd(5, "op_shift(%08p, %08p, isEvent=%d)\n", left, x, xsink->isEvent());

   class VLock vl;
   QoreNode **val = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent() || !(*val) || (*val)->type != NT_LIST)
      return NULL;

   ensure_unique(val, xsink);

   printd(5, "op_shift() *val=%08p (%s)\n", *val, *val ? (*val)->type->name : "(none)");
   printd(5, "op_shift() about to call List::shift() on list node %08p (%d)\n", (*val), (*val)->val.list->size());

   QoreNode *rv = (*val)->val.list->shift();

   printd(5, "op_shift() got node %08p (%s)\n", rv, rv ? rv->type->name : "(none)");
   // the list reference will now be the reference for return value
   // therefore no need to reference again

   //traceout("op_shift()");
   return rv;
}

static QoreNode *op_pop(class QoreNode *left, class QoreNode *x, ExceptionSink *xsink)
{
   //tracein("op_pop()");
   printd(5, "op_pop(%08p, %08p, isEvent=%d)\n", left, x, xsink->isEvent());

   class VLock vl;
   QoreNode **val = get_var_value_ptr(left, &vl, xsink);
   if (xsink->isEvent() || !(*val) || (*val)->type != NT_LIST)
      return NULL;

   ensure_unique(val, xsink);

   printd(5, "op_pop() *val=%08p (%s)\n", *val, *val ? (*val)->type->name : "(none)");
   printd(5, "op_pop() about to call List::pop() on list node %08p (%d)\n", (*val), (*val)->val.list->size());

   QoreNode *rv = (*val)->val.list->pop();

   printd(5, "op_pop() got node %08p (%s)\n", rv, rv ? rv->type->name : "(none)");
   // the list reference will now be the reference for return value
   // therefore no need to reference again

   //traceout("op_pop()");
   return rv;
}

static QoreNode *op_push(class QoreNode *left, class QoreNode *elem, ExceptionSink *xsink)
{
   //tracein("op_push()");
   printd(5, "op_push(%08p, %08p, isEvent=%d)\n", left, elem, xsink->isEvent());

   class VLock vl;
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
      if (xsink->isEvent())
      {
	 if (elem) elem->deref(xsink);
	 return NULL;
      }
   }

   (*val)->val.list->push(elem);

   // reference for return value
   (*val)->ref();

   //traceout("op_push()");
   return (*val);
}

// lvalue, offset, [length, [list]]
static QoreNode *op_splice(class QoreNode *left, class QoreNode *l, ExceptionSink *xsink)
{
   //tracein("op_splice()");
   printd(5, "op_splice(%08p, %08p, isEvent=%d)\n", left, l, xsink->isEvent());

   class VLock vl;
   QoreNode **val = get_var_value_ptr(left, &vl, xsink);
   // value is not a list, so throw exception
   if (xsink->isEvent())
      return NULL;

   if (!(*val) || ((*val)->type != NT_LIST && (*val)->type != NT_STRING))
   {
      xsink->raiseException("SPLICE-ERROR", "first argument to splice is not a list or a string");
      return NULL;
   }
   // evaluate list
   l = l->eval(xsink);
   if (xsink->isEvent())
   {
      if (l)
	 l->deref(xsink);
      return NULL;
   }

   ensure_unique(val, xsink);

   // evaluating a list must give another list
   int size = l->val.list->size();
   int offset = l->val.list->getEntryAsInt(0);

   printd(5, "op_splice() val=%08p (size=%d) list=%08p (size=%d) offset=%d\n", (*val), (*val)->val.list->size(), l, size, offset);

   if ((*val)->type == NT_LIST)
   {
      if (size == 1)
	 (*val)->val.list->splice(offset, xsink);
      else
      {
	 int length = l->val.list->getEntryAsInt(1);
	 if (size == 2)
	    (*val)->val.list->splice(offset, length, xsink);
	 else
	    (*val)->val.list->splice(offset, length, l->val.list->retrieve_entry(2), xsink);
      }
   }
   else // must be a string
   {
      if (size == 1)
	 (*val)->val.String->splice(offset, xsink);
      else
      {
	 int length = l->val.list->getEntryAsInt(1);
	 if (size == 2)
	    (*val)->val.String->splice(offset, length, xsink);
	 else
	    (*val)->val.String->splice(offset, length, l->val.list->retrieve_entry(2), xsink);
      }
   }
   l->deref(xsink);

   // reference for return value
   (*val)->ref();

   //traceout("op_splice()");
   return (*val);
}

static QoreNode *op_exists(class QoreNode *left, class QoreNode *x, ExceptionSink *xsink)
{
   bool b;
   // if left == NOTHING
   if (is_nothing(left))
      b = false;
   else if (left->type->isValue())
      b = true;
   else
   {
      class TempNode *tn = NULL;
      class VLock vl;
      class QoreNode *n = getExistingVarValue(left, xsink, &vl, &tn);
      // return if an exception happened
      if (xsink->isEvent())
      {
	 vl.del();
	 if (tn) tn->del(xsink);
	 //traceout("op_exists()");
	 return NULL;
      }

      // FIXME: this should return false for objects that have been deleted
      if (is_nothing(n))
	 b = false;
      else
	 b = true;
      if (tn) tn->del(xsink);
   }
   return new QoreNode(b);
}

static QoreNode *op_instanceof(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink)
{
   l = l->eval(xsink);
   if (xsink->isEvent())
   {
      if (l)
	 l->deref(xsink);
      return NULL;
   }
   if (!l)
      return boolean_false();

   class QoreNode *rv;
   if (l->type != NT_OBJECT || !l->val.object->validInstanceOf(r->val.classref->getID()))
      rv = boolean_false();
   else
      rv = boolean_true();

   l->deref(xsink);
   return rv;
}

static QoreNode *op_minus_hash_string(class QoreNode *h, class QoreNode *s, ExceptionSink *xsink)
{
   class QoreNode *x = h->realCopy(xsink);
   if (xsink->isEvent())
   {
      if (x)
	 x->deref(xsink);
      return NULL;
   }
   x->val.hash->deleteKey(s->val.String, xsink);
   return x;
}

static class QoreNode *op_regex_extract(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   class List *l = right->val.regex->extractSubstrings(left->val.String, xsink);
   if (!l)
      return NULL;
   return new QoreNode(l);
}

void Operator::init()
{
   if (!evalArgs || numFunctions == 1)
      return;
   opMatrix = new int[NUM_VALUE_TYPES][NUM_VALUE_TYPES];
   // create function lookup matrix
   for (int i = 0; i < NUM_VALUE_TYPES; i++)
      for (int j = 0; j < NUM_VALUE_TYPES; j++)
	 opMatrix[i][j] = findFunction(QTM.typelist[i], QTM.typelist[j]);
}

// registers the system operators and system operator functions
// WARNING: declaration order is hardcoded in operators.h
void operatorsInit()
{
   tracein("operatorsInit()");
   
   OP_ASSIGNMENT = oplist.add(new Operator(2, "=", "assignment", 0, true, true));
   OP_ASSIGNMENT->addFunction(NT_ALL, NT_ALL, op_assignment);
   
   OP_LIST_ASSIGNMENT = oplist.add(new Operator(2, "(list) =", "list assignment", 0, true, true));
   OP_LIST_ASSIGNMENT->addFunction(NT_ALL, NT_ALL, op_list_assignment);
   
   OP_LOG_AND = oplist.add(new Operator(2, "&&", "logical-and", 0, false));
   OP_LOG_AND->addFunction(NT_INT, NT_INT, op_log_and);

   OP_LOG_OR = oplist.add(new Operator(2, "||", "logical-or", 0, false));
   OP_LOG_OR->addFunction(NT_INT, NT_INT, op_log_or);
   
   OP_LOG_LT = oplist.add(new Operator(2, "<", "less-than", 1, false));
   OP_LOG_LT->addFunction(NT_FLOAT,  NT_FLOAT,  op_log_lt_float);
   OP_LOG_LT->addFunction(NT_INT,    NT_INT,    op_log_lt_bigint);
   OP_LOG_LT->addFunction(NT_STRING, NT_STRING, op_log_lt_string);
   OP_LOG_LT->addFunction(NT_DATE,   NT_DATE,   op_log_lt_date);
   
   OP_LOG_GT = oplist.add(new Operator(2, ">", "greater-than", 1, false));
   OP_LOG_GT->addFunction(NT_FLOAT,  NT_FLOAT,  op_log_gt_float);
   OP_LOG_GT->addFunction(NT_INT,    NT_INT,    op_log_gt_bigint);
   OP_LOG_GT->addFunction(NT_STRING, NT_STRING, op_log_gt_string);
   OP_LOG_GT->addFunction(NT_DATE,   NT_DATE,   op_log_gt_date);

   OP_LOG_EQ = oplist.add(new Operator(2, "==", "logical-equals", 1, false));
   OP_LOG_EQ->addFunction(NT_STRING,  NT_STRING,  op_log_eq_string);
   OP_LOG_EQ->addFunction(NT_FLOAT,   NT_FLOAT,   op_log_eq_float);
   OP_LOG_EQ->addFunction(NT_INT,     NT_INT,     op_log_eq_bigint);
   OP_LOG_EQ->addFunction(NT_BOOLEAN, NT_BOOLEAN, op_log_eq_boolean);
   OP_LOG_EQ->addFunction(NT_DATE,    NT_DATE,    op_log_eq_date);
   OP_LOG_EQ->addFunction(NT_LIST,    NT_ALL,     op_log_eq_list);
   OP_LOG_EQ->addFunction(NT_ALL,     NT_LIST,    op_log_eq_list);
   OP_LOG_EQ->addFunction(NT_HASH,    NT_ALL,     op_log_eq_hash);
   OP_LOG_EQ->addFunction(NT_ALL,     NT_HASH,    op_log_eq_hash);
   OP_LOG_EQ->addFunction(NT_OBJECT,  NT_ALL,     op_log_eq_object);
   OP_LOG_EQ->addFunction(NT_ALL,     NT_OBJECT,  op_log_eq_object);
   OP_LOG_EQ->addFunction(NT_NULL,    NT_ALL,     op_log_eq_null);
   OP_LOG_EQ->addFunction(NT_ALL,     NT_NULL,    op_log_eq_null);
   OP_LOG_EQ->addFunction(NT_NOTHING, NT_NOTHING, op_log_eq_nothing);
   OP_LOG_EQ->addFunction(NT_BINARY,  NT_BINARY,  op_log_eq_binary);

   OP_LOG_NE = oplist.add(new Operator(2, "!=", "not-equals", 1, false));
   OP_LOG_NE->addFunction(NT_STRING,  NT_STRING,  op_log_ne_string);
   OP_LOG_NE->addFunction(NT_FLOAT,   NT_FLOAT,   op_log_ne_float);
   OP_LOG_NE->addFunction(NT_INT,     NT_INT,     op_log_ne_bigint);
   OP_LOG_NE->addFunction(NT_BOOLEAN, NT_BOOLEAN, op_log_ne_boolean);
   OP_LOG_NE->addFunction(NT_DATE,    NT_DATE,    op_log_ne_date);
   OP_LOG_NE->addFunction(NT_LIST,    NT_ALL,     op_log_ne_list);
   OP_LOG_NE->addFunction(NT_ALL,     NT_LIST,    op_log_ne_list);
   OP_LOG_NE->addFunction(NT_HASH,    NT_ALL,     op_log_ne_hash);
   OP_LOG_NE->addFunction(NT_ALL,     NT_HASH,    op_log_ne_hash);
   OP_LOG_NE->addFunction(NT_OBJECT,  NT_ALL,     op_log_ne_object);
   OP_LOG_NE->addFunction(NT_ALL,     NT_OBJECT,  op_log_ne_object);
   OP_LOG_NE->addFunction(NT_NULL,    NT_ALL,     op_log_ne_null);
   OP_LOG_NE->addFunction(NT_ALL,     NT_NULL,    op_log_ne_null);
   OP_LOG_NE->addFunction(NT_NOTHING, NT_NOTHING, op_log_ne_nothing);
   OP_LOG_NE->addFunction(NT_BINARY,  NT_BINARY,  op_log_ne_binary);
   
   OP_LOG_LE = oplist.add(new Operator(2, "<=", "less-than-or-equals", 1, false));
   OP_LOG_LE->addFunction(NT_FLOAT,  NT_FLOAT,  op_log_le_float);
   OP_LOG_LE->addFunction(NT_INT,    NT_INT,    op_log_le_bigint);
   OP_LOG_LE->addFunction(NT_STRING, NT_STRING, op_log_le_string);
   OP_LOG_LE->addFunction(NT_DATE,   NT_DATE,   op_log_le_date);

   OP_LOG_GE = oplist.add(new Operator(2, ">=", "greater-than-or-equals", 1, false));
   OP_LOG_GE->addFunction(NT_FLOAT,  NT_FLOAT,  op_log_ge_float);
   OP_LOG_GE->addFunction(NT_INT,    NT_INT,    op_log_ge_bigint);
   OP_LOG_GE->addFunction(NT_STRING, NT_STRING, op_log_ge_string);
   OP_LOG_GE->addFunction(NT_DATE,   NT_DATE,   op_log_ge_date);

   OP_MODULA = oplist.add(new Operator(2, "%", "modula", 1, false));
   OP_MODULA->addFunction(NT_INT, NT_INT, op_modula_int);

   OP_BIN_AND = oplist.add(new Operator(2, "&", "binary-and", 1, false));
   OP_BIN_AND->addFunction(NT_INT, NT_INT, op_bin_and_int);

   OP_BIN_OR = oplist.add(new Operator(2, "|", "binary-or", 1, false));
   OP_BIN_OR->addFunction(NT_INT, NT_INT, op_bin_or_int);

   OP_BIN_NOT = oplist.add(new Operator(1, "~", "binary-not", 1, false));
   OP_BIN_NOT->addFunction(NT_INT, NT_NONE, op_bin_not_int);

   OP_BIN_XOR = oplist.add(new Operator(2, "^", "binary-xor", 1, false));
   OP_BIN_XOR->addFunction(NT_INT, NT_INT, op_bin_xor_int);

   OP_MINUS = oplist.add(new Operator(2, "-", "minus", 1, false));
   OP_MINUS->addFunction(NT_DATE,  NT_DATE,    op_minus_date);
   OP_MINUS->addFunction(NT_FLOAT, NT_FLOAT,   op_minus_float);
   OP_MINUS->addFunction(NT_INT,   NT_INT,     op_minus_bigint);
   OP_MINUS->addFunction(NT_INT,   NT_NOTHING, op_minus_bigint);
   OP_MINUS->addFunction(NT_HASH,  NT_STRING,  op_minus_hash_string);

   OP_PLUS = oplist.add(new Operator(2, "+", "plus", 1, false));
   OP_PLUS->addFunction(NT_LIST,    NT_LIST,   op_plus_list);
   OP_PLUS->addFunction(NT_STRING,  NT_STRING, op_plus_string);
   OP_PLUS->addFunction(NT_DATE,    NT_DATE,   op_plus_date);
   OP_PLUS->addFunction(NT_FLOAT,   NT_FLOAT,  op_plus_float);
   OP_PLUS->addFunction(NT_INT,     NT_INT,    op_plus_bigint);
   OP_PLUS->addFunction(NT_HASH,    NT_HASH,   op_plus_hash_hash);
   OP_PLUS->addFunction(NT_HASH,    NT_OBJECT, op_plus_hash_object);
   //OP_PLUS->addFunction(NT_OBJECT,  NT_OBJECT, op_plus_object_object);
   OP_PLUS->addFunction(NT_OBJECT,  NT_HASH,   op_plus_object_hash);
   OP_PLUS->addFunction(NT_NOTHING, NT_INT,    op_plus_bigint);

   OP_MULT = oplist.add(new Operator(2, "*", "multiply", 1, false));
   OP_MULT->addFunction(NT_FLOAT, NT_FLOAT, op_multiply_float);
   OP_MULT->addFunction(NT_INT,   NT_INT,   op_multiply_bigint);

   OP_DIV = oplist.add(new Operator(2, "/", "divide", 1, false));
   OP_DIV->addFunction(NT_FLOAT, NT_FLOAT, op_divide_float);
   OP_DIV->addFunction(NT_INT,   NT_INT,   op_divide_bigint);

   OP_UNARY_MINUS = oplist.add(new Operator(1, "-", "unary-minus", 1, false));
   OP_UNARY_MINUS->addFunction(NT_FLOAT, NT_NONE, op_unary_minus_float);
   OP_UNARY_MINUS->addFunction(NT_INT,   NT_NONE, op_unary_minus_bigint);

   OP_NOT = oplist.add(new Operator(1, "!", "logical-not", 1, false));
   OP_NOT->addFunction(NT_BOOLEAN, NT_NONE, op_log_not_boolean);

   OP_SHIFT_LEFT = oplist.add(new Operator(2, "<<", "shift-left", 1, false));
   OP_SHIFT_LEFT->addFunction(NT_INT, NT_INT, op_shift_left_int);

   OP_SHIFT_RIGHT = oplist.add(new Operator(2, ">>", "shift-right", 1, false));
   OP_SHIFT_RIGHT->addFunction(NT_INT, NT_INT, op_shift_right_int);

   OP_POST_INCREMENT = oplist.add(new Operator(1, "++", "post-increment", 0, true, true));
   OP_POST_INCREMENT->addFunction(NT_VARREF, NT_NONE, op_post_inc);

   OP_POST_DECREMENT = oplist.add(new Operator(1, "--", "post-decrement", 0, true, true));
   OP_POST_DECREMENT->addFunction(NT_VARREF, NT_NONE, op_post_dec);

   OP_PRE_INCREMENT = oplist.add(new Operator(1, "++", "pre-increment", 0, true, true));
   OP_PRE_INCREMENT->addFunction(NT_VARREF, NT_NONE, op_pre_inc);

   OP_PRE_DECREMENT = oplist.add(new Operator(1, "--", "pre-decrement", 0, true, true));
   OP_PRE_DECREMENT->addFunction(NT_VARREF, NT_NONE, op_pre_dec);

   OP_LOG_CMP = oplist.add(new Operator(2, "<=>", "logical-comparison", 1, false));
   OP_LOG_CMP->addFunction(NT_STRING, NT_STRING, op_cmp_string);
   OP_LOG_CMP->addFunction(NT_FLOAT,  NT_FLOAT,  op_cmp_double);
   OP_LOG_CMP->addFunction(NT_INT,    NT_INT,    op_cmp_bigint);
   OP_LOG_CMP->addFunction(NT_DATE,   NT_DATE,   op_cmp_date);

   OP_PLUS_EQUALS = oplist.add(new Operator(2, "+=", "plus-equals", 0, true, true));
   OP_PLUS_EQUALS->addFunction(NT_ALL, NT_INT, op_plus_equals);

   OP_MINUS_EQUALS = oplist.add(new Operator(2, "-=", "minus-equals", 0, true, true));
   OP_MINUS_EQUALS->addFunction(NT_ALL, NT_INT, op_minus_equals);

   OP_AND_EQUALS = oplist.add(new Operator(2, "&=", "and-equals", 0, true, true));
   OP_AND_EQUALS->addFunction(NT_ALL, NT_INT, op_and_equals);

   OP_OR_EQUALS = oplist.add(new Operator(2, "|=", "or-equals", 0, true, true));
   OP_OR_EQUALS->addFunction(NT_ALL, NT_INT, op_or_equals);

   OP_MODULA_EQUALS = oplist.add(new Operator(2, "%=", "modula-equals", 0, true, true));
   OP_MODULA_EQUALS->addFunction(NT_ALL, NT_INT, op_modula_equals);

   OP_MULTIPLY_EQUALS = oplist.add(new Operator(2, "*=", "multiply-equals", 0, true, true));
   OP_MULTIPLY_EQUALS->addFunction(NT_ALL, NT_INT, op_multiply_equals);

   OP_DIVIDE_EQUALS = oplist.add(new Operator(2, "/=", "divide-equals", 0, true, true));
   OP_DIVIDE_EQUALS->addFunction(NT_ALL, NT_INT, op_divide_equals);

   OP_XOR_EQUALS = oplist.add(new Operator(2, "^=", "xor-equals", 0, true, true));
   OP_XOR_EQUALS->addFunction(NT_ALL, NT_INT, op_xor_equals);

   OP_SHIFT_LEFT_EQUALS = oplist.add(new Operator(2, "<<=", "shift-left-equals", 0, true, true));
   OP_SHIFT_LEFT_EQUALS->addFunction(NT_ALL, NT_INT, op_shift_left_equals);

   OP_SHIFT_RIGHT_EQUALS = oplist.add(new Operator(2, ">>=", "shift-right-equals", 0, true, true));
   OP_SHIFT_RIGHT_EQUALS->addFunction(NT_ALL, NT_INT, op_shift_right_equals);

   OP_LIST_REF = oplist.add(new Operator(2, "[]", "list-reference", 0, false));
   OP_LIST_REF->addFunction(NT_LIST, NT_INT, op_list_ref);

   OP_OBJECT_REF = oplist.add(new Operator(2, ".", "hash/object-reference", 0, false));
   OP_OBJECT_REF->addFunction(NT_ALL, NT_ALL, op_object_ref); 

   OP_ELEMENTS = oplist.add(new Operator(1, "elements", "number of elements", 0, false));
   OP_ELEMENTS->addFunction(NT_ALL, NT_NONE, op_elements);

   OP_KEYS = oplist.add(new Operator(1, "keys", "list of keys", 0, false));
   OP_KEYS->addFunction(NT_ALL, NT_NONE, op_keys);

   OP_QUESTION_MARK = oplist.add(new Operator(2, "question", "question-mark colon", 0, false));
   OP_QUESTION_MARK->addFunction(NT_ALL, NT_ALL, op_question_mark);

   OP_ABSOLUTE_EQ = oplist.add(new Operator(2, "===", "absolute logical-equals", 0, false));
   OP_ABSOLUTE_EQ->addFunction(NT_ALL, NT_ALL, op_absolute_log_eq);
   
   OP_ABSOLUTE_NE = oplist.add(new Operator(2, "!==", "absolute logical-not-equals", 0, false));
   OP_ABSOLUTE_NE->addFunction(NT_ALL, NT_ALL, op_absolute_log_neq);

   OP_REGEX_MATCH = oplist.add(new Operator(2, "=~", "regular expression match", 1, false));
   OP_REGEX_MATCH->addFunction(NT_STRING, NT_REGEX, op_regex_match);

   OP_REGEX_NMATCH = oplist.add(new Operator(2, "!~", "regular expression negative match", 1, false));
   OP_REGEX_NMATCH->addFunction(NT_STRING, NT_REGEX, op_regex_nmatch);

   OP_OBJECT_FUNC_REF = oplist.add(new Operator(2, ".", "object method call", 0, true, false));
   OP_OBJECT_FUNC_REF->addFunction(NT_ALL, NT_ALL, op_object_method_call);

   OP_NEW = oplist.add(new Operator(1, "new", "new object", 0, true, false));
   OP_NEW->addFunction(NT_ALL, NT_NONE, op_new_object);

   OP_SHIFT = oplist.add(new Operator(1, "shift", "shift from list", 0, true, true));
   OP_SHIFT->addFunction(NT_ALL, NT_NONE, op_shift);

   OP_POP = oplist.add(new Operator(1, "pop", "pop from list", 0, true, true));
   OP_POP->addFunction(NT_ALL, NT_NONE, op_pop);

   OP_PUSH = oplist.add(new Operator(1, "push", "push on list", 0, true, true));
   OP_PUSH->addFunction(NT_ALL, NT_NONE, op_push);

   OP_SPLICE = oplist.add(new Operator(2, "splice", "splice in list", 0, true, true));
   OP_SPLICE->addFunction(NT_ALL, NT_ALL, op_splice);

   OP_EXISTS = oplist.add(new Operator(1, "exists", "exists", 0, false));
   OP_EXISTS->addFunction(NT_ALL, NT_NONE, op_exists);

   OP_SINGLE_ASSIGN = oplist.add(new Operator(2, "single =", "single assignment", 0, true, true));
   OP_SINGLE_ASSIGN->addFunction(NT_ALL, NT_ALL, op_single_assignment);

   OP_UNSHIFT = oplist.add(new Operator(1, "unshift", "unshift/insert to begnning of list", 0, true, true));
   OP_UNSHIFT->addFunction(NT_ALL, NT_NONE, op_unshift);

   OP_REGEX_SUBST = oplist.add(new Operator(1, "regex subst", "regular expression substitution", 0, true, true));
   OP_REGEX_SUBST->addFunction(NT_ALL, NT_REGEX_SUBST, op_regex_subst);

   OP_INSTANCEOF = oplist.add(new Operator(2, "instanceof", "instanceof", 0, false));
   OP_INSTANCEOF->addFunction(NT_ALL, NT_CLASSREF, op_instanceof);

   OP_REGEX_TRANS = oplist.add(new Operator(1, "transliteration", "transliteration", 0, true, true));
   OP_REGEX_TRANS->addFunction(NT_ALL, NT_REGEX_TRANS, op_regex_trans);

   OP_REGEX_EXTRACT = oplist.add(new Operator(2, "regular expression subpattern extraction", "regular expression subpattern extraction", 1, false));
   OP_REGEX_EXTRACT->addFunction(NT_STRING, NT_REGEX, op_regex_extract);

   // initialize all operators
   class Operator *w = oplist.getHead();
   while (w)
   {
      w->init();
      w = w->next;
   }

   traceout("operatorsInit()");
}
