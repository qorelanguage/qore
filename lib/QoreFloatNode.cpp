/*
  QoreFloatNode.cpp
  
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

QoreFloatNode::QoreFloatNode(double n_f) : SimpleValueQoreNode(NT_FLOAT), f(n_f) {
}

QoreFloatNode::QoreFloatNode() : SimpleValueQoreNode(NT_FLOAT), f(0.0) {
}

QoreFloatNode::~QoreFloatNode() {
}

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *QoreFloatNode::getStringRepresentation(bool &del) const {
   del = true;
   return new QoreString(f);
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void QoreFloatNode::getStringRepresentation(QoreString &str) const {
   str.sprintf("%.9g", f);
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
DateTime *QoreFloatNode::getDateTimeRepresentation(bool &del) const {
   del = true;
   return DateTime::makeAbsoluteLocal(currentTZ(), (int64)f, (int)((f - (float)((int)f)) * 1000000));
}

// assign date representation to a DateTime (no action for complex types = default implementation)
void QoreFloatNode::getDateTimeRepresentation(DateTime &dt) const {
   dt.setLocalDate(currentTZ(), (int64)f, (int)((f - (float)((int)f)) * 1000000));
}

bool QoreFloatNode::getAsBoolImpl() const {
   return (bool)f;
}

int QoreFloatNode::getAsIntImpl() const {
   return (int)f;
}

int64 QoreFloatNode::getAsBigIntImpl() const {
   return (int64)f;
}

double QoreFloatNode::getAsFloatImpl() const {
   return f;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int QoreFloatNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   getStringRepresentation(str);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreFloatNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   return getStringRepresentation(del);
}

AbstractQoreNode *QoreFloatNode::realCopy() const {
   return new QoreFloatNode(f);
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreFloatNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   return v->getAsFloat() == f;
}

bool QoreFloatNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   const QoreFloatNode *fn = dynamic_cast<const QoreFloatNode *>(v);
   if (!fn)
      return false;

   return fn->f == f;
}

// returns the type name as a c string
const char *QoreFloatNode::getTypeName() const {
   return getStaticTypeName();
}

AbstractQoreNode *QoreFloatNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   typeInfo = floatTypeInfo;
   return this;
}
