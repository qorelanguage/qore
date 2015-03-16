/*
  QoreNothingNode.cpp

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

#ifdef DEBUG
static bool nothing_flag = 0;
#endif

QoreString YamlNullString("null");

QoreNothingNode::QoreNothingNode() : UniqueValueQoreNode(NT_NOTHING) {
#ifdef DEBUG
   assert(!nothing_flag);
   nothing_flag = true;
#endif
}

QoreNothingNode::~QoreNothingNode() {
   //printd(0, "QoreNothingNode::~QoreNothingNode() called this=%p\n", this);
}

AbstractQoreNode *QoreNothingNode::evalImpl(class ExceptionSink *xsink) const {
   assert(false);
   return 0;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int QoreNothingNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(foff == FMT_YAML_SHORT ? &YamlNullString : &NothingTypeString);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreNothingNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return foff == FMT_YAML_SHORT ? &YamlNullString : &NothingTypeString;
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL int QoreNothingNode::compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreNothingNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   return is_nothing(v);
}

bool QoreNothingNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   return is_nothing(v);
}

// returns the type name as a c string
const char *QoreNothingNode::getTypeName() const {
   return getStaticTypeName();
}

//! returns the type information
AbstractQoreNode *QoreNothingNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   typeInfo = nothingTypeInfo;
   return this;
}
