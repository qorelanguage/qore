/*
  AbstractQoreNode.cpp

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
#include <qore/intern/qore_list_private.h>
#include <qore/intern/QoreHashNodeIntern.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define TRACK_REFS 0

#if TRACK_REFS
#define REF_LVL (type!=NT_HASH)
#endif

AbstractQoreNode::AbstractQoreNode(qore_type_t t, bool n_value, bool n_needs_eval, bool n_there_can_be_only_one, bool n_custom_reference_handlers) : type(t), value(n_value), needs_eval_flag(n_needs_eval), there_can_be_only_one(n_there_can_be_only_one), custom_reference_handlers(n_custom_reference_handlers), has_value_api(false) {
#if TRACK_REFS
   printd(REF_LVL, "AbstractQoreNode::ref() %p type: %d (0->1)\n", this, type);
#endif
}

AbstractQoreNode::AbstractQoreNode(const AbstractQoreNode& v) : type(v.type), value(v.value), needs_eval_flag(v.needs_eval_flag), there_can_be_only_one(v.there_can_be_only_one), custom_reference_handlers(v.custom_reference_handlers), has_value_api(v.has_value_api) {
#if TRACK_REFS
   printd(REF_LVL, "AbstractQoreNode::ref() %p type: %d (0->1)\n", this, type);
#endif
}

AbstractQoreNode::~AbstractQoreNode() {
#if 0
   printd(5, "AbstractQoreNode::~AbstractQoreNode() type: %d (%s)\n", type, getTypeName());
#endif
}

/*
bool test(const AbstractQoreNode* n) {
   if (n->getType() == NT_OBJECT) {
      const QoreObject* obj = reinterpret_cast<const QoreObject*>(n);
      //return !strcmp(obj->getClassName(), "T");
      return !strcmp(obj->getClassName(), "SharedLogFile") && qore_object_private::hackId(*obj);
   }
   return false;
}

static void break_ref() {}
*/
void AbstractQoreNode::ref() const {
#ifdef DEBUG
   /*
   if (test(this)) {
      printd(0, "AbstractQoreNode::ref() %p type: %d %s (%d->%d)\n", this, type, getTypeName(), references, references + 1);
      break_ref();
   }
   */
#if TRACK_REFS
   if (type == NT_OBJECT) {
      const QoreObject *o = reinterpret_cast<const QoreObject*>(this);
      printd(REF_LVL, "AbstractQoreNode::ref() %p type: %d object (%d->%d) object: %p, class: %s\n", this, type, references, references + 1, o, o->getClass()->getName());
   }
   else
      printd(REF_LVL, "AbstractQoreNode::ref() %p type: %d %s (%d->%d)\n", this, type, getTypeName(), references, references + 1);
#endif
#endif
   if (!there_can_be_only_one) {
      if (custom_reference_handlers)
	 customRef();
      else
	 ROreference();
   }
}

AbstractQoreNode* AbstractQoreNode::refSelf() const {
   ref();
   return const_cast<AbstractQoreNode*>(this);
}

bool AbstractQoreNode::derefImpl(ExceptionSink* xsink) {
   return true;
}

void AbstractQoreNode::customRef() const {
   assert(false);
}

void AbstractQoreNode::customDeref(ExceptionSink* xsink) {
   assert(false);
}

//static void break_deref() {}
void AbstractQoreNode::deref(ExceptionSink* xsink) {
   //QORE_TRACE("AbstractQoreNode::deref()");
#ifdef DEBUG
   /*
   if (test(this)) {
      printd(0, "AbstractQoreNode::deref() %p type: %d %s (%d->%d)\n", this, type, getTypeName(), references, references - 1);
      break_deref();
   }
   */
#if TRACK_REFS
   if (type == NT_OBJECT)
      printd(REF_LVL, "QoreObject::deref() %p class: %s (%d->%d) %d\n", this, ((QoreObject*)this)->getClassName(), references, references - 1, custom_reference_handlers);
   else
      printd(REF_LVL, "AbstractQoreNode::deref() %p type: %d %s (%d->%d)\n", this, type, getTypeName(), references, references - 1);

#endif
   if (references > 10000000 || references <= 0){
      if (type == NT_STRING)
	 printd(0, "AbstractQoreNode::deref() WARNING, node %p references: %d (type: %s) (val=\"%s\")\n",
		this, references, getTypeName(), ((QoreStringNode*)this)->getBuffer());
      else
	 printd(0, "AbstractQoreNode::deref() WARNING, node %p references: %d (type: %s)\n", this, references, getTypeName());
      assert(false);
   }
#endif
   assert(references > 0);

   if (there_can_be_only_one) {
      assert(is_unique());
      return;
   }

   if (custom_reference_handlers) {
      customDeref(xsink);
   }
   else if (ROdereference()) {
      if (type < NUM_SIMPLE_TYPES || derefImpl(xsink))
	 delete this;
   }
}

QoreValue AbstractQoreNode::evalValue(ExceptionSink* xsink) const {
   if (!needs_eval_flag)
      return refSelf();

   if (has_value_api) {
      const ParseNode* pn = reinterpret_cast<const ParseNode*>(this);
      ValueEvalRefHolder v(pn, xsink);
      return v.takeReferencedValue();
   }

   return evalImpl(xsink);
}

QoreValue AbstractQoreNode::evalValue(bool& needs_deref, ExceptionSink* xsink) const {
   if (!needs_eval_flag) {
      needs_deref = false;
      return const_cast<AbstractQoreNode*>(this);
   }

   if (has_value_api) {
      const ParseNode* pn = reinterpret_cast<const ParseNode*>(this);
      ValueEvalRefHolder v(pn, xsink);
      return v.takeValue(needs_deref);
   }

   return evalImpl(needs_deref, xsink);
}

// AbstractQoreNode::eval(): return value requires a dereference
AbstractQoreNode* AbstractQoreNode::eval(ExceptionSink* xsink) const {
   if (!needs_eval_flag)
      return refSelf();
   return evalImpl(xsink);
}

// AbstractQoreNode::eval(): return value requires a dereference if needs_deref is true
AbstractQoreNode* AbstractQoreNode::eval(bool &needs_deref, ExceptionSink* xsink) const {
   if (!needs_eval_flag) {
      needs_deref = false;
      return const_cast<AbstractQoreNode*>(this);
   }

   return evalImpl(needs_deref, xsink);
}

int64 AbstractQoreNode::bigIntEvalImpl(ExceptionSink* xsink) const {
   if (has_value_api) {
      const ParseNode* pn = reinterpret_cast<const ParseNode*>(this);
      ValueEvalRefHolder v(pn, xsink);
      return v->getAsBigInt();
   }
   ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int AbstractQoreNode::integerEvalImpl(ExceptionSink* xsink) const {
   if (has_value_api) {
      const ParseNode* pn = reinterpret_cast<const ParseNode*>(this);
      ValueEvalRefHolder v(pn, xsink);
      return v->getAsBigInt();
   }
   ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool AbstractQoreNode::boolEvalImpl(ExceptionSink* xsink) const {
   if (has_value_api) {
      const ParseNode* pn = reinterpret_cast<const ParseNode*>(this);
      ValueEvalRefHolder v(pn, xsink);
      return v->getAsBool();
   }
   ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
   return rv ? rv->getAsBool() : false;
}

double AbstractQoreNode::floatEvalImpl(ExceptionSink* xsink) const {
   if (has_value_api) {
      const ParseNode* pn = reinterpret_cast<const ParseNode*>(this);
      ValueEvalRefHolder v(pn, xsink);
      return v->getAsFloat();
   }
   ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
   return rv ? rv->getAsFloat() : 0.0;
}

int64 AbstractQoreNode::bigIntEval(ExceptionSink* xsink) const {
   if (needs_eval_flag)
      return bigIntEvalImpl(xsink);
   return getAsBigInt();
}

int AbstractQoreNode::integerEval(ExceptionSink* xsink) const {
   if (needs_eval_flag)
      return integerEvalImpl(xsink);
   return getAsInt();
}

bool AbstractQoreNode::boolEval(ExceptionSink* xsink) const {
   if (needs_eval_flag)
      return boolEvalImpl(xsink);
   return getAsBool();
}

double AbstractQoreNode::floatEval(ExceptionSink* xsink) const {
   if (needs_eval_flag)
      return floatEvalImpl(xsink);
   return getAsFloat();
}

bool AbstractQoreNode::getAsBool() const {
   if (type == NT_BOOLEAN)
      return reinterpret_cast<const QoreBoolNode*>(this)->getValue();
   return getAsBoolImpl();
}

int AbstractQoreNode::getAsInt() const {
   if (type == NT_INT)
      return (int)(reinterpret_cast<const QoreBigIntNode*>(this)->val);
   return getAsIntImpl();
}

int64 AbstractQoreNode::getAsBigInt() const {
   if (type == NT_INT)
      return reinterpret_cast<const QoreBigIntNode*>(this)->val;
   return getAsBigIntImpl();
}

double AbstractQoreNode::getAsFloat() const {
   if (type == NT_FLOAT)
      return reinterpret_cast<const QoreFloatNode*>(this)->f;
   return getAsFloatImpl();
}

// get the value of the type in a string context, empty string for complex types (default implementation)
QoreString *AbstractQoreNode::getStringRepresentation(bool& del) const {
   del = false;
   return NullString;
}

// empty default implementation
void AbstractQoreNode::getStringRepresentation(QoreString& str) const {
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
DateTime *AbstractQoreNode::getDateTimeRepresentation(bool& del) const {
   del = false;
   return ZeroDate;
}

// assign date representation to a DateTime (no action for complex types = default implementation)
void AbstractQoreNode::getDateTimeRepresentation(DateTime& dt) const {
   dt.setDate(0LL);
}

AbstractQoreNode* AbstractQoreNode::parseInit(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   return this;
}

bool AbstractQoreNode::hasValueApi() const {
   return has_value_api;
}

// for getting relative time values or integer values
int getSecZeroInt(const AbstractQoreNode* a) {
   if (is_nothing(a))
      return 0;

   if (a->getType() == NT_DATE)
      return (int)(reinterpret_cast<const DateTimeNode*>(a)->getRelativeSeconds());

   return a->getAsInt();
}

int64 getSecZeroBigInt(const AbstractQoreNode* a) {
   if (is_nothing(a))
      return 0;

   if (a->getType() == NT_DATE)
      return reinterpret_cast<const DateTimeNode*>(a)->getRelativeSeconds();

   return a->getAsBigInt();
}

// for getting relative time values or integer values
int getSecMinusOneInt(const AbstractQoreNode* a) {
   if (is_nothing(a))
      return -1;

   if (a->getType() == NT_DATE)
      return (int)(reinterpret_cast<const DateTimeNode*>(a)->getRelativeSeconds());

   return a->getAsInt();
}

int64 getSecMinusOneBigInt(const AbstractQoreNode* a) {
   if (is_nothing(a))
      return -1;

   if (a->getType() == NT_DATE)
      return reinterpret_cast<const DateTimeNode*>(a)->getRelativeSeconds();

   return a->getAsBigInt();
}

int getMsZeroInt(const AbstractQoreNode* a) {
   if (is_nothing(a))
      return 0;

   if (a->getType() == NT_DATE)
      return (int)(reinterpret_cast<const DateTimeNode*>(a)->getRelativeMilliseconds());

   return a->getAsInt();
}

int64 getMsZeroBigInt(const AbstractQoreNode* a) {
   if (is_nothing(a))
      return 0;

   if (a->getType() == NT_DATE)
      return reinterpret_cast<const DateTimeNode*>(a)->getRelativeMilliseconds();

   return a->getAsBigInt();
}

// for getting relative time values or integer values
int getMsMinusOneInt(const AbstractQoreNode* a) {
   if (is_nothing(a))
      return -1;

   if (a->getType() == NT_DATE)
      return (int)(reinterpret_cast<const DateTimeNode*>(a)->getRelativeMilliseconds());

   return a->getAsInt();
}

int64 getMsMinusOneBigInt(const AbstractQoreNode* a) {
   if (is_nothing(a))
      return -1;

   if (a->getType() == NT_DATE)
      return reinterpret_cast<const DateTimeNode*>(a)->getRelativeMilliseconds();

   return a->getAsBigInt();
}

int getMicroSecZeroInt(const AbstractQoreNode* a) {
   if (is_nothing(a))
      return 0;

   if (a->getType() == NT_DATE)
      return (int)(reinterpret_cast<const DateTimeNode*>(a)->getRelativeMicroseconds());

   return a->getAsInt();
}

int64 getMicroSecZeroInt64(const AbstractQoreNode* a) {
   if (is_nothing(a))
      return 0;

   if (a->getType() == NT_DATE)
      return (reinterpret_cast<const DateTimeNode*>(a)->getRelativeMicroseconds());

   return a->getAsBigInt();
}

static inline QoreListNode* crlr_list_copy(const QoreListNode* n, ExceptionSink* xsink) {
   ReferenceHolder<QoreListNode> l(new QoreListNode(true), xsink);
   for (unsigned i = 0; i < n->size(); i++) {
      l->push(copy_and_resolve_lvar_refs(n->retrieve_entry(i), xsink));
      if (*xsink)
	 return 0;
   }
   return l.release();
}

static inline QoreValueList* crlr_list_copy(const QoreValueList* n, ExceptionSink* xsink) {
   ReferenceHolder<QoreValueList> l(new QoreValueList, xsink);
   for (unsigned i = 0; i < n->size(); i++) {
      l->push(copy_value_and_resolve_lvar_refs(n->retrieveEntry(i), xsink));
      if (*xsink)
	 return 0;
   }
   return l.release();
}

static inline AbstractQoreNode* crlr_hash_copy(const QoreHashNode* n, ExceptionSink* xsink) {
   ReferenceHolder<QoreHashNode> h(new QoreHashNode(true), xsink);
   ConstHashIterator hi(n);
   while (hi.next()) {
      h->setKeyValue(hi.getKey(), copy_and_resolve_lvar_refs(hi.getValue(), xsink), xsink);
      if (*xsink)
	 return 0;
   }
   return h.release();
}

static inline AbstractQoreNode* crlr_tree_copy(const QoreTreeNode* n, ExceptionSink* xsink) {
   return new QoreTreeNode(copy_and_resolve_lvar_refs(n->left, xsink), n->getOp(),
			   n->right ? copy_and_resolve_lvar_refs(n->right, xsink) : 0);
}

static inline AbstractQoreNode* crlr_selfcall_copy(const SelfFunctionCallNode* n, ExceptionSink* xsink) {
   QoreListNode* na = const_cast<QoreListNode*>(n->getArgs());
   if (na)
      na = crlr_list_copy(na, xsink);

   return new SelfFunctionCallNode(*n, na);
}

static inline AbstractQoreNode* crlr_fcall_copy(const FunctionCallNode* n, ExceptionSink* xsink) {
   QoreListNode* na = const_cast<QoreListNode*>(n->getArgs());
   if (na)
      na = crlr_list_copy(na, xsink);

   return new FunctionCallNode(n->getFunction(), na, n->getProgram());
}

static inline AbstractQoreNode* crlr_mcall_copy(const MethodCallNode* m, ExceptionSink* xsink) {
   QoreListNode* args = const_cast<QoreListNode*>(m->getArgs());
   //printd(5, "crlr_mcall_copy() m: %p (%s) args: %p (len: %d)\n", m, m->getName(), args, args ? args->size() : 0);
   if (args) {
      ReferenceHolder<QoreListNode> args_holder(crlr_list_copy(args, xsink), xsink);
      if (*xsink)
	 return 0;

      args = args_holder.release();
   }

   return new MethodCallNode(*m, args);
}

static inline AbstractQoreNode* crlr_smcall_copy(const StaticMethodCallNode* m, ExceptionSink* xsink) {
   QoreListNode* args = const_cast<QoreListNode*>(m->getArgs());
   //printd(5, "crlr_mcall_copy() m: %p (%s) args: %p (len: %d)\n", m, m->getName(), args, args ? args->size() : 0);
   if (args) {
      ReferenceHolder<QoreListNode> args_holder(crlr_list_copy(args, xsink), xsink);
      if (*xsink)
	 return 0;

      args = args_holder.release();
   }

   return new StaticMethodCallNode(m->getMethod(), args);
}

static AbstractQoreNode* call_ref_call_copy(const CallReferenceCallNode* n, ExceptionSink* xsink) {
   ReferenceHolder<AbstractQoreNode> exp(copy_and_resolve_lvar_refs(n->getExp(), xsink), xsink);
   if (*xsink)
      return 0;

   QoreListNode* args = const_cast<QoreListNode*>(n->getArgs());
   if (args) {
      ReferenceHolder<QoreListNode> args_holder(crlr_list_copy(args, xsink), xsink);
      if (*xsink)
	 return 0;

      args = args_holder.release();
   }

   return new CallReferenceCallNode(exp.release(), args);
}

static AbstractQoreNode* eval_notnull(const AbstractQoreNode* n, ExceptionSink* xsink) {
   ReferenceHolder<AbstractQoreNode> exp(n->eval(xsink), xsink);
   if (*xsink)
      return 0;

   return exp ? exp.release() : nothing();
}

QoreValue copy_value_and_resolve_lvar_refs(QoreValue& n, ExceptionSink* xsink) {
   if (!n.hasNode())
      return n;
   return copy_and_resolve_lvar_refs(n.getInternalNode(), xsink);
}

AbstractQoreNode* copy_and_resolve_lvar_refs(const AbstractQoreNode* n, ExceptionSink* xsink) {
   if (!n) return 0;

   qore_type_t ntype = n->getType();

   if (ntype == NT_LIST)
      return crlr_list_copy(reinterpret_cast<const QoreListNode*>(n), xsink);

   if (ntype == NT_HASH)
      return crlr_hash_copy(reinterpret_cast<const QoreHashNode*>(n), xsink);

   if (ntype == NT_TREE)
      return crlr_tree_copy(reinterpret_cast<const QoreTreeNode*>(n), xsink);

   if (ntype == NT_OPERATOR)
      return reinterpret_cast<const QoreOperatorNode*>(n)->copyBackground(xsink);

   if (ntype == NT_SELF_CALL)
      return crlr_selfcall_copy(reinterpret_cast<const SelfFunctionCallNode*>(n), xsink);

   if (ntype == NT_FUNCTION_CALL || ntype == NT_PROGRAM_FUNC_CALL)
      return crlr_fcall_copy(reinterpret_cast<const FunctionCallNode*>(n), xsink);

   // must make sure to return a value here or it could cause a segfault - parse expressions expect non-NULL values for the operands
   if (ntype == NT_FIND)
      return eval_notnull(n, xsink);

   if (ntype == NT_VARREF && reinterpret_cast<const VarRefNode*>(n)->getType() != VT_GLOBAL)
      return eval_notnull(n, xsink);

   if (ntype == NT_FUNCREFCALL)
      return call_ref_call_copy(reinterpret_cast<const CallReferenceCallNode*>(n), xsink);

   if (ntype == NT_METHOD_CALL)
      return crlr_mcall_copy(reinterpret_cast<const MethodCallNode*>(n), xsink);

   if (ntype == NT_STATIC_METHOD_CALL)
      return crlr_smcall_copy(reinterpret_cast<const StaticMethodCallNode*>(n), xsink);

   if (ntype == NT_PARSEREFERENCE)
      return reinterpret_cast<const ParseReferenceNode*>(n)->evalToIntermediate(xsink);

   // ensure closures are evaluated in the parent thread so closure-bound local vars can be found and bound before
   // launching the background thread (fixes https://github.com/qorelanguage/qore/issues/12)
   if (ntype == NT_CLOSURE)
      return reinterpret_cast<const QoreClosureParseNode*>(n)->evalBackground(xsink);

   assert(ntype != NT_VALUE_LIST);

   return n->refSelf();
}

void SimpleQoreNode::deref() {
   if (there_can_be_only_one) {
      assert(is_unique());
      return;
   }

   if (ROdereference())
      delete this;
}

AbstractQoreNode* SimpleValueQoreNode::evalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

AbstractQoreNode* SimpleValueQoreNode::evalImpl(bool &needs_deref, ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

int64 SimpleValueQoreNode::bigIntEvalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

int SimpleValueQoreNode::integerEvalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

bool SimpleValueQoreNode::boolEvalImpl(ExceptionSink* xsink) const {
   assert(false);
   return false;
}

double SimpleValueQoreNode::floatEvalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0.0;
}

AbstractQoreNode* UniqueValueQoreNode::realCopy() const {
   return const_cast<UniqueValueQoreNode*>(this);
}

bool is_container(const AbstractQoreNode* n) {
   switch (get_node_type(n)) {
      case NT_OBJECT:
      case NT_LIST:
      case NT_HASH:
	 return true;
      case NT_VALUE_LIST:
	 assert(false);
   }
   return false;
}

bool get_container_obj(const AbstractQoreNode* n) {
   if (!n)
      return false;

   switch (n->getType()) {
      case NT_LIST: return qore_list_private::getObjectCount(*static_cast<const QoreListNode*>(n)) ? true : false;
      case NT_HASH: return qore_hash_private::getObjectCount(*static_cast<const QoreHashNode*>(n)) ? true : false;
      case NT_OBJECT: return true;
      case NT_VALUE_LIST: assert(false); return qore_value_list_private::getObjectCount(*static_cast<const QoreValueList*>(n)) ? true : false;
   }

   return false;
}

void inc_container_obj(const AbstractQoreNode* n, int dt) {
   assert(n);
   switch (n->getType()) {
      case NT_LIST: qore_list_private::incObjectCount(*static_cast<const QoreListNode*>(n), dt); break;
      case NT_HASH: qore_hash_private::incObjectCount(*static_cast<const QoreHashNode*>(n), dt); break;
      case NT_OBJECT: qore_object_private::incObjectCount(*static_cast<const QoreObject*>(n), dt); break;
      case NT_VALUE_LIST: assert(false); qore_value_list_private::incObjectCount(*static_cast<const QoreValueList*>(n), dt); break;
      default: assert(false);
   }
}
