/* indent-tabs-mode: nil -*- */
/*
  QoreValue.cpp

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
#include <qore/intern/ParseNode.h>

QoreValue::QoreValue() : type(QV_Node) {
   v.n = 0;
}

QoreValue::QoreValue(bool b) : type(QV_Bool) {
   v.b = b;
}

QoreValue::QoreValue(int64 i) : type(QV_Int) {
   v.i = i;
}

QoreValue::QoreValue(double f) : type(QV_Float) {
   v.f = f;
}

QoreValue::QoreValue(AbstractQoreNode* n) : type(QV_Node) {
   v.n = n;
}

QoreValue::QoreValue(const AbstractQoreNode* n) {
   switch (get_node_type(n)) {
      case NT_NOTHING:
	 type = QV_Node;
	 v.n = 0;
	 return;
      case NT_INT:
	 type = QV_Int;
	 v.i = reinterpret_cast<const QoreBigIntNode*>(n)->val;
	 return;
      case NT_FLOAT:
	 type = QV_Float;
	 v.f = reinterpret_cast<const QoreFloatNode*>(v.n)->f;
	 return;
      case NT_BOOLEAN:
	 type = QV_Bool;
	 v.b = reinterpret_cast<const QoreBoolNode*>(v.n)->getValue();
	 return;
   }
   type = QV_Node;
   // n cannot be 0 here because we covered the NT_NOTHING case above
   v.n = n->refSelf();
}

QoreValue::QoreValue(const QoreValue& old): type(old.type) {
   switch (type) {
      case QV_Bool: v.b = old.v.b; break;
      case QV_Int: v.i = old.v.i; break;
      case QV_Float: v.f = old.v.f; break;
      case QV_Node: v.n = old.v.n; break;
      default:
         assert(false);
         // no break
   }
}

void QoreValue::swap(QoreValue& val) {
   QoreValue v1(*this);
   *this = val;
   val = v1;
}

QoreValue& QoreValue::operator=(const QoreValue& n) {
   type = n.type;
   switch (type) {
      case QV_Bool: v.b = n.v.b; break;
      case QV_Int: v.i = n.v.i; break;
      case QV_Float: v.f = n.v.f; break;
      case QV_Node: v.n = n.v.n; break;
      default: assert(false);
         // no break
   }
   return *this;
}

bool QoreValue::getAsBool() const {
   switch (type) {
      case QV_Bool: return v.b;
      case QV_Int: return (bool)v.i;
      case QV_Float: return (bool)v.f;
      case QV_Node: return v.n ? v.n->getAsBool() : false;
      default: assert(false);
         // no break
   }
   return false;
}

int64 QoreValue::getAsBigInt() const {
   switch (type) {
      case QV_Bool: return (int64)v.b;
      case QV_Int: return v.i;
      case QV_Float: return (int64)v.f;
      case QV_Node: return v.n ? v.n->getAsBigInt() : 0;
      default: assert(false);
         // no break
   }
   return 0;
}

double QoreValue::getAsFloat() const {
   switch (type) {
      case QV_Bool: return (double)v.b;
      case QV_Int: return (double)v.i;
      case QV_Float: return v.f;
      case QV_Node: return v.n ? v.n->getAsFloat() : 0.0;
      default: assert(false);
         // no break
   }
   return 0.0;
}

AbstractQoreNode* QoreValue::getInternalNode() {
   return type == QV_Node ? v.n : 0;
}

QoreValue QoreValue::refSelf() const {
   if (type == QV_Node && v.n)
      v.n->ref();
   return const_cast<QoreValue&>(*this);
}

const AbstractQoreNode* QoreValue::getInternalNode() const {
   return type == QV_Node ? v.n : 0;
}

AbstractQoreNode* QoreValue::assign(AbstractQoreNode* n) {
   AbstractQoreNode* rv = takeIfNode();
   type = QV_Node;
   v.n = n;
   return rv;
}

AbstractQoreNode* QoreValue::assignAndSanitize(const AbstractQoreNode* n) {
   AbstractQoreNode* rv = takeIfNode();
   switch (get_node_type(n)) {
      case NT_NOTHING:
	 type = QV_Node;
	 v.n = 0;
	 break;
      case NT_INT:
         type = QV_Int;
         v.i = reinterpret_cast<const QoreBigIntNode*>(n)->val;
         break;
      case NT_FLOAT:
         type = QV_Float;
         v.f = reinterpret_cast<const QoreFloatNode*>(n)->f;
         break;
      case NT_BOOLEAN:
         type = QV_Bool;
         v.b = reinterpret_cast<const QoreBoolNode*>(n)->getValue();
         break;
      default:
         type = QV_Node;
         v.n = n->refSelf();
         break;
   }
   return rv;
}

AbstractQoreNode* QoreValue::assign(int64 n) {
   AbstractQoreNode* rv = takeIfNode();
   type = QV_Int;
   v.i = n;
   return rv;
}

AbstractQoreNode* QoreValue::assign(double n) {
   AbstractQoreNode* rv = takeIfNode();
   type = QV_Float;
   v.f = n;
   return rv;
}

AbstractQoreNode* QoreValue::assign(bool n) {
   AbstractQoreNode* rv = takeIfNode();
   type = QV_Bool;
   v.b = n;
   return rv;
}

AbstractQoreNode* QoreValue::assignNothing() {
   AbstractQoreNode* rv = takeIfNode();
   type = QV_Node;
   v.n = 0;
   return rv;
}

bool QoreValue::isEqualSoft(const QoreValue v, ExceptionSink* xsink) const {
   return QoreLogicalEqualsOperatorNode::softEqual(*this, v, xsink);
}

bool QoreValue::isEqualHard(const QoreValue n) const {
   qore_type_t t = getType();
   if (t != n.getType())
      return false;
   switch (t) {
      case NT_INT: return getAsBigInt() == n.getAsBigInt();
      case NT_BOOLEAN: return getAsBool() == n.getAsBool();
      case NT_FLOAT: return getAsFloat() == n.getAsFloat();
      case NT_NOTHING:
      case NT_NULL:
	 return true;
   }
   return !compareHard(v.n, n.v.n, 0);
}

void QoreValue::sanitize() {
   if (type != QV_Node || !v.n)
      return;
   switch (v.n->getType()) {
      case NT_NOTHING: v.n = 0; break;
      case NT_INT: {
	 int64 i = reinterpret_cast<QoreBigIntNode*>(v.n)->val;
	 type = QV_Int;
	 v.n->deref(0);
	 v.i = i;
	 break;
      }
      case NT_FLOAT: {
	 double f = reinterpret_cast<QoreFloatNode*>(v.n)->f;
	 type = QV_Float;
	 v.n->deref(0);
	 v.f = f;
	 break;
      }
      case NT_BOOLEAN: {
	 bool b = reinterpret_cast<QoreBoolNode*>(v.n)->getValue();
	 type = QV_Bool;
	 v.b = b;
	 break;
      }
   }
}

void QoreValue::discard(ExceptionSink* xsink) {
   if (type == QV_Node && v.n) {
      v.n->deref(xsink);
      v.n = 0;
   }
}

int QoreValue::getAsString(QoreString& str, int format_offset, ExceptionSink *xsink) const {
   if (isNothing()) {
      str.concat(format_offset == FMT_YAML_SHORT ? &YamlNullString : &NothingTypeString);
      return 0;
   }
   switch (type) {
      case QV_Int: str.sprintf(QLLD, v.i); break;
      case QV_Bool: str.concat(v.b ? &TrueString : &FalseString); break;
      case QV_Float: str.sprintf("%.9g", v.f); break;
      case QV_Node: return v.n->getAsString(str, format_offset, xsink);
      default:
	 assert(false);
	 // no break;
   }
   return 0;
}

QoreString* QoreValue::getAsString(bool& del, int format_offset, ExceptionSink* xsink) const {
   if (isNothing()) {
      del = false;
      return format_offset == FMT_YAML_SHORT ? &YamlNullString : &NothingTypeString;
   }
   switch (type) {
      case QV_Int: del = true; return new QoreStringMaker(QLLD, v.i);
      case QV_Bool: del = false; return v.b ? &TrueString : &FalseString;
      case QV_Float: del = true; return new QoreStringMaker("%.9g", v.f);
      case QV_Node: return v.n->getAsString(del, format_offset, xsink);
      default:
	 assert(false);
	 // no break;
   }
   return 0;
}

AbstractQoreNode* QoreValue::getReferencedValue() const {
   switch (type) {
      case QV_Bool: return get_bool_node(v.b);
      case QV_Int: return new QoreBigIntNode(v.i);
      case QV_Float: return new QoreFloatNode(v.f);
      case QV_Node: return v.n ? v.n->refSelf() : 0;
      default: assert(false);
         // no break
   }
   return 0;
}

AbstractQoreNode* QoreValue::takeNode() {
   switch (type) {
      case QV_Bool: return get_bool_node(v.b);
      case QV_Int: return new QoreBigIntNode(v.i);
      case QV_Float: return new QoreFloatNode(v.f);
      case QV_Node: return takeNodeIntern();
      default: assert(false);
         // no break
   }
   return 0;
}

AbstractQoreNode* QoreValue::takeIfNode() {
   return type == QV_Node ? takeNodeIntern() : 0;
}

qore_type_t QoreValue::getType() const {
   switch (type) {
      case QV_Bool: return NT_BOOLEAN;
      case QV_Int: return NT_INT;
      case QV_Float: return NT_FLOAT;
      case QV_Node: return v.n ? v.n->getType() : 0;
      default: assert(false);
         // no break
   }
   // to avoid a warning
   return NT_NOTHING;
}

const char* QoreValue::getTypeName() const {
   switch (type) {
      case QV_Bool: return QoreBoolNode::getStaticTypeName();
      case QV_Int: return QoreBigIntNode::getStaticTypeName();
      case QV_Float: return QoreFloatNode::getStaticTypeName();
      case QV_Node: return get_type_name(v.n);
      default: assert(false);
         // no break
   }
   return 0;
}

AbstractQoreNode* QoreValue::takeNodeIntern() {
   assert(type == QV_Node);
   AbstractQoreNode* rv = v.n;
   v.n = 0;
   return rv;
}

bool QoreValue::hasNode() const {
   return type == QV_Node && v.n;
}

bool QoreValue::isNothing() const {
   return type == QV_Node && is_nothing(v.n);
}

bool QoreValue::isNull() const {
   return type == QV_Node && is_null(v.n);
}

bool QoreValue::isNullOrNothing() const {
   return type == QV_Node && (is_nothing(v.n) || is_null(v.n));
}

ValueHolder::~ValueHolder() {
   discard(v.getInternalNode(), xsink);
}

AbstractQoreNode* ValueHolder::getReferencedValue() {
   return v.takeNode();
}

ValueOptionalRefHolder::~ValueOptionalRefHolder() {
   if (needs_deref)
      discard(v.getInternalNode(), xsink);
}

ValueEvalRefHolder::ValueEvalRefHolder(const AbstractQoreNode* exp, ExceptionSink* xs) : ValueOptionalRefHolder(xs) {
   if (!exp)
      return;

   if (exp->hasValueApi()) {
      const ParseNode* pn = reinterpret_cast<const ParseNode*>(exp);
      v = pn->evalValue(needs_deref, xsink);
      return;
   }

   v = exp->eval(needs_deref, xsink);
}

AbstractQoreNode* ValueEvalRefHolder::getReferencedValue() {
   if (v.type == QV_Node) {
      if (!needs_deref && v.v.n)
	 v.v.n->ref();
      return v.takeNodeIntern();
   }
   return v.takeNode();
}

QoreValue ValueEvalRefHolder::takeReferencedValue() {
   if (v.type == QV_Node) {
      if (needs_deref) {
	 needs_deref = false;
	 return v.takeNodeIntern();
      }
      if (v.v.n)
	 v.v.n->ref();
      return v.takeNodeIntern();
   }
   return v;
}
