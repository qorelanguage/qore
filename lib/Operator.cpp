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
