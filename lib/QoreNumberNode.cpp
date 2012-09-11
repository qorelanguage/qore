/*
  QoreNumberNode.cpp
  
  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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
#include <qore/intern/qore_number_private.h>

QoreNumberNode::QoreNumberNode(struct qore_number_private* p) : SimpleValueQoreNode(NT_NUMBER), priv(p) {
}

QoreNumberNode::QoreNumberNode(const AbstractQoreNode* n) : SimpleValueQoreNode(NT_NUMBER), priv(0) {
   qore_type_t t = get_node_type(n);
   if (t == NT_NUMBER) {
      priv = new qore_number_private(*reinterpret_cast<const QoreNumberNode*>(n)->priv);
      return;
   }

   if (t == NT_FLOAT) {
      priv = new qore_number_private(reinterpret_cast<const QoreFloatNode*>(n)->f);
      return;
   }

   if (t == NT_STRING) {
      priv = new qore_number_private(reinterpret_cast<const QoreStringNode*>(n)->getBuffer());
      return;
   }

   if (t == NT_INT || (t > QORE_NUM_TYPES && dynamic_cast<const QoreBigIntNode*>(n))) {
      priv = new qore_number_private(reinterpret_cast<const QoreBigIntNode*>(n)->val);
      return;
   }

   if (t != NT_BOOLEAN
       && t != NT_DATE
       && t != NT_NULL) {
      priv = new qore_number_private(0ll);
      return;
   }

   priv = new qore_number_private(n->getAsFloat());
}

QoreNumberNode::QoreNumberNode(double f) : SimpleValueQoreNode(NT_NUMBER), priv(new qore_number_private(f)) {
}

QoreNumberNode::QoreNumberNode(int64 i) : SimpleValueQoreNode(NT_NUMBER), priv(new qore_number_private(i)) {
}

QoreNumberNode::QoreNumberNode(const char* str) : SimpleValueQoreNode(NT_NUMBER), priv(new qore_number_private(str)) {
}

QoreNumberNode::QoreNumberNode() : SimpleValueQoreNode(NT_NUMBER), priv(new qore_number_private(0ll)) {
}

QoreNumberNode::QoreNumberNode(const QoreNumberNode& old) : SimpleValueQoreNode(old), priv(new qore_number_private(*old.priv)) {
}

QoreNumberNode::~QoreNumberNode() {
   delete priv;
}

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString* QoreNumberNode::getStringRepresentation(bool& del) const {
   del = true;
   QoreString* str = new QoreString;
   priv->getAsString(*str);
   return str;
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void QoreNumberNode::getStringRepresentation(QoreString& str) const {
   priv->getAsString(str);
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
DateTime *QoreNumberNode::getDateTimeRepresentation(bool& del) const {
   del = true;
   double f = priv->getAsFloat();
   return DateTime::makeAbsoluteLocal(currentTZ(), (int64)f, (int)((f - (float)((int)f)) * 1000000));
}

// assign date representation to a DateTime (no action for complex types = default implementation)
void QoreNumberNode::getDateTimeRepresentation(DateTime& dt) const {
   double f = priv->getAsFloat();
   dt.setLocalDate(currentTZ(), (int64)f, (int)((f - (float)((int)f)) * 1000000));
}

bool QoreNumberNode::getAsBoolImpl() const {
   return priv->getAsBool();
}

int QoreNumberNode::getAsIntImpl() const {
   return priv->getAsBigInt();
}

int64 QoreNumberNode::getAsBigIntImpl() const {
   return priv->getAsBigInt();
}

double QoreNumberNode::getAsFloatImpl() const {
   return priv->getAsFloat();
}

int QoreNumberNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
   getStringRepresentation(str);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreNumberNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
   return getStringRepresentation(del);
}

AbstractQoreNode* QoreNumberNode::realCopy() const {
   return new QoreNumberNode(*this);
}

// the type passed must always be equal to the current type
bool QoreNumberNode::is_equal_soft(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   if (v->getType() == NT_NUMBER)
      return !priv->compare(*reinterpret_cast<const QoreNumberNode*>(v)->priv);
   if (v->getType() == NT_INT || dynamic_cast<const QoreBigIntNode*>(v))
      return !priv->compare(reinterpret_cast<const QoreBigIntNode*>(v)->val);

   return !priv->compare(v->getAsFloat());
}

bool QoreNumberNode::is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   if (v->getType() != NT_NUMBER)
      return false;
   const QoreNumberNode* n = reinterpret_cast<const QoreNumberNode*>(v);
   return !priv->compare(*n->priv);
}

// returns the type name as a c string
const char* QoreNumberNode::getTypeName() const {
   return getStaticTypeName();
}

AbstractQoreNode* QoreNumberNode::parseInit(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   typeInfo = numberTypeInfo;
   return this;
}

bool QoreNumberNode::zero() const {
   return priv->zero();
}

QoreNumberNode* QoreNumberNode::doPlus(const QoreNumberNode& right) const {
   return new QoreNumberNode(priv->doPlus(*right.priv));
}

//! add the argument to this value and return the result
QoreNumberNode* QoreNumberNode::doMinus(const QoreNumberNode& n) const {
   return new QoreNumberNode(priv->doMinus(*n.priv));
}

//! add the argument to this value and return the result
QoreNumberNode* QoreNumberNode::doMultiply(const QoreNumberNode& n) const {
   return new QoreNumberNode(priv->doMultiply(*n.priv));
}

//! add the argument to this value and return the result
QoreNumberNode* QoreNumberNode::doDivideBy(const QoreNumberNode& n, ExceptionSink* xsink) const {
   qore_number_private* p = priv->doDivideBy(*n.priv, xsink);
   return p ? new QoreNumberNode(p) : 0;
}

QoreNumberNode* QoreNumberNode::negate() const {
   return new QoreNumberNode(priv->negate());
}

int QoreNumberNode::compare(const QoreNumberNode& n) const {
   return priv->compare(*n.priv);
}

QoreNumberNode* QoreNumberNode::numberRefSelf() const {
   ref();
   return const_cast<QoreNumberNode*>(this);
}
