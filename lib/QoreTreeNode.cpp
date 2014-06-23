/*
  QoreTreeNode.cpp
  
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

QoreTreeNode::QoreTreeNode(AbstractQoreNode *l, Operator *o, AbstractQoreNode *r) : ParseNode(NT_TREE, true, o->hasEffect()), op(o), returnTypeInfo(0), left(l), right(r) {
   //printd(5, "QoreTreeNode::QoreTreeNode() this=%p left=%p (%s) right=%p (%s) op=%s has_effect=%d\n", this, left, get_type_name(left), right, get_type_name(right), op->getDescription(), has_effect());
}

QoreTreeNode::~QoreTreeNode() {
   //printd(5, "QoreTreeNode::~QoreTreeNode() this=%p left=%p right=%p\n", this, left, right);
   if (left)
      left->deref(0);
   if (right)
      right->deref(0);
}

void QoreTreeNode::ignoreReturnValue() {
   ignore_rv();
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
// FIXME: no deep effect - or is this ever needed?
int QoreTreeNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("tree (left=%s (%p) op=%s right=%s (%p))", get_type_name(left), left, op->getName(), get_type_name(right), right);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreTreeNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString;
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *QoreTreeNode::getTypeName() const {
   return "expression tree";
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *QoreTreeNode::evalImpl(ExceptionSink *xsink) const {
   return op->eval(left, right, need_rv(), xsink);
}

AbstractQoreNode *QoreTreeNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return op->eval(left, right, need_rv(), xsink);
}

int64 QoreTreeNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   return op->bigint_eval(left, right, xsink);
}

int QoreTreeNode::integerEvalImpl(ExceptionSink *xsink) const {
   return (int)op->bigint_eval(left, right, xsink);
}

bool QoreTreeNode::boolEvalImpl(ExceptionSink *xsink) const {
   return op->bool_eval(left, right, xsink);
}

double QoreTreeNode::floatEvalImpl(ExceptionSink *xsink) const {
   return op->float_eval(left, right, xsink);
}

AbstractQoreNode *QoreTreeNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   assert(!typeInfo);
   // set "parsing background" flag if the background operator is being parsed
   if (op == OP_BACKGROUND)
      pflag |= PF_BACKGROUND;

   // turn off "reference ok" and "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   // check argument types for operator   
   AbstractQoreNode *n = op->parseInit(this, oflag, pflag, lvids, typeInfo);
   if (n == this)
      returnTypeInfo = typeInfo;
   return n;
}
