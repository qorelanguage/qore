/*
  QoreBigIntNode.cpp
  
  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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

QoreBigIntNode::QoreBigIntNode() : SimpleValueQoreNode(NT_INT), val(0) {
}

QoreBigIntNode::QoreBigIntNode(int64 v) : SimpleValueQoreNode(NT_INT), val(v) {
}

// protected constructor for subclasses only
QoreBigIntNode::QoreBigIntNode(qore_type_t t, int64 v) : SimpleValueQoreNode(t), val(v) {
}

QoreBigIntNode::~QoreBigIntNode() {
}

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *QoreBigIntNode::getStringRepresentation(bool &del) const {
   del = true;
   return new QoreString(val);
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void QoreBigIntNode::getStringRepresentation(QoreString &str) const {
   str.sprintf("%lld", val);
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
DateTime *QoreBigIntNode::getDateTimeRepresentation(bool &del) const {
   del = true;
   return new DateTime(val);
}

// assign date representation to a DateTime (no action for complex types = default implementation)
void QoreBigIntNode::getDateTimeRepresentation(DateTime &dt) const {
   dt.setDate(val);
}

bool QoreBigIntNode::getAsBoolImpl() const {
   return (bool)val;
}

int QoreBigIntNode::getAsIntImpl() const {
   return (int)val;
}

int64 QoreBigIntNode::getAsBigIntImpl() const {
   return val;
}

double QoreBigIntNode::getAsFloatImpl() const {
   return (double)val;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *QoreBigIntNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   return getStringRepresentation(del);
}

int QoreBigIntNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   getStringRepresentation(str);
   return 0;
}

AbstractQoreNode *QoreBigIntNode::realCopy() const {
   return new QoreBigIntNode(val);
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreBigIntNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   return v->getAsBigInt() == val;
}

bool QoreBigIntNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   const QoreBigIntNode *i = dynamic_cast<const QoreBigIntNode *>(v);
   if (!i)
      return false;

   return i->val == val;
}

// returns the type name as a c string
const char *QoreBigIntNode::getTypeName() const {
   return getStaticTypeName();
}

AbstractQoreNode *QoreBigIntNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   typeInfo = bigIntTypeInfo;
   return this;
}
