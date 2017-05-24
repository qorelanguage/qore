/*
  BarewordNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2016 David Nichols

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
#include "qore/intern/QoreNamespaceIntern.h"

// object takes over ownership of str
BarewordNode::BarewordNode(char *c_str, int sline, int eline) : ParseNoEvalNode(NT_BAREWORD), loc(sline, eline), finalized(false), str(c_str) {
}

BarewordNode::~BarewordNode() {
   if (str)
      free(str);
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int BarewordNode::getAsString(QoreString &qstr, int foff, ExceptionSink *xsink) const {
   qstr.sprintf("%s '%s' (%p)", getTypeName(), str ? str : "<null>", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *BarewordNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the data type
qore_type_t BarewordNode::getType() const {
   return NT_BAREWORD;
}

// returns the type name as a c string
const char *BarewordNode::getTypeName() const {
   return "bareword";
}

QoreStringNode *BarewordNode::makeQoreStringNode() {
   assert(str);
   int len = strlen(str);
   QoreStringNode *qstr = new QoreStringNode(str, len, len + 1, QCS_DEFAULT);
   str = 0;
   return qstr;
}

char *BarewordNode::takeString() {
   assert(str);
   char *p = str;
   str = 0;
   return p;
}

AbstractQoreNode *BarewordNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   //printd(5, "BarewordNode::parseInitImpl() this: %p str: %s\n", this, str);
   AbstractQoreNode *n = qore_root_ns_private::parseResolveBareword(loc, str, typeInfo);
   if (!n)
      return this;

   deref(0);
   typeInfo = 0;
   return n->parseInit(oflag, pflag, lvids, typeInfo);
}
