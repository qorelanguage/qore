/*
  ContextrefNode.cpp
 
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

ContextrefNode::ContextrefNode(char *c_str) : ParseNode(NT_CONTEXTREF), str(c_str) {
}

ContextrefNode::~ContextrefNode() {
   if (str)
      free(str);
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int ContextrefNode::getAsString(QoreString &qstr, int foff, ExceptionSink *xsink) const {
   qstr.sprintf("context reference '%s' (0x%08p)", str ? str : "<null>", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *ContextrefNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *ContextrefNode::getTypeName() const {
   return "context reference";
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *ContextrefNode::evalImpl(ExceptionSink *xsink) const {
   return evalContextRef(str, xsink);
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *ContextrefNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return ContextrefNode::evalImpl(xsink);
}

int64 ContextrefNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(ContextrefNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int ContextrefNode::integerEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(ContextrefNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool ContextrefNode::boolEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(ContextrefNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double ContextrefNode::floatEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(ContextrefNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}

AbstractQoreNode *ContextrefNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   typeInfo = 0;

   if (!getCVarStack())
      parse_error("context reference \"%s\" out of context", str);
   return this;
}
