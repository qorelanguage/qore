/*
  ContextRowNode.cpp
 
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

ContextRowNode::ContextRowNode() : ParseNode(NT_CONTEXT_ROW) {
}

ContextRowNode::~ContextRowNode() {
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int ContextRowNode::getAsString(QoreString &qstr, int foff, ExceptionSink *xsink) const {
   qstr.sprintf("context row reference '%%' (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *ContextRowNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *ContextRowNode::getTypeName() const {
   return "context row reference";
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *ContextRowNode::evalImpl(ExceptionSink *xsink) const {
   return evalContextRow(xsink);
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *ContextRowNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return ContextRowNode::evalImpl(xsink);
}

int64 ContextRowNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(ContextRowNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int ContextRowNode::integerEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(ContextRowNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool ContextRowNode::boolEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(ContextRowNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double ContextRowNode::floatEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(ContextRowNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}

AbstractQoreNode *ContextRowNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   typeInfo = hashTypeInfo;
   if (!getCVarStack())
      parse_error("context row reference \"%%\" encountered out of context");
   return this;
}
