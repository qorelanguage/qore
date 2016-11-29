/*
  QoreBoolNode.cpp

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
#include "qore/intern/qore_date_private.h"

QoreBoolNode::QoreBoolNode(bool n_b) : UniqueValueQoreNode(NT_BOOLEAN), b(n_b) {
}

QoreBoolNode::~QoreBoolNode() {
}

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *QoreBoolNode::getStringRepresentation(bool &del) const {
   del = true;
   return new QoreString(b);
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void QoreBoolNode::getStringRepresentation(QoreString &str) const {
   str.concat(b ? '1' : '0');
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
DateTime *QoreBoolNode::getDateTimeRepresentation(bool &del) const {
   del = true;
   return DateTime::makeRelativeFromSeconds(b ? 1 : 0);
}

// assign date representation to a DateTime (no action for complex types = default implementation)
void QoreBoolNode::getDateTimeRepresentation(DateTime &dt) const {
   dt.setRelativeDateSeconds(b ? 1 : 0);
}

bool QoreBoolNode::getAsBoolImpl() const {
   return b;
}

int QoreBoolNode::getAsIntImpl() const {
   return b;
}

int64 QoreBoolNode::getAsBigIntImpl() const {
   return b;
}

double QoreBoolNode::getAsFloatImpl() const {
   return b;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int QoreBoolNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(b ? "True" : "False");
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreBoolNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return b ? &TrueString : &FalseString;
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL int compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreBoolNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   return v->getAsBool() == b;
}

bool QoreBoolNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   const QoreBoolNode *bn = dynamic_cast<const QoreBoolNode *>(v);
   if (!bn)
      return false;

   return b == bn->b;
}

// returns the type name as a c string
const char *QoreBoolNode::getTypeName() const {
   return getStaticTypeName();
}

#ifdef DEBUG
static bool qore_bool_true_init = false;
static bool qore_bool_false_init = false;
#endif

QoreBoolTrueNode::QoreBoolTrueNode() : QoreBoolNode(true) {
#ifdef DEBUG
   assert(!qore_bool_true_init);
   qore_bool_true_init = true;
#endif
}

QoreBoolFalseNode::QoreBoolFalseNode() : QoreBoolNode(false) {
#ifdef DEBUG
   assert(!qore_bool_false_init);
   qore_bool_false_init = true;
#endif
}

AbstractQoreNode *QoreBoolNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   //printd(0, "QoreBoolNode::parseInit() this=%p val=%s\n", this, b ? "true" : "false");
   typeInfo = boolTypeInfo;
   return this;
}
