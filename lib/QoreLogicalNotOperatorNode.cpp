/*
  QoreLogicalNotOperatorNode.cpp
 
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

QoreString QoreLogicalNotOperatorNode::LogicalNot_str("logical not operator expression");

// if del is true, then the returned QoreString* should be deleted, if false, then it must not be
QoreString* QoreLogicalNotOperatorNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
   del = false;
   return &LogicalNot_str;
}

int QoreLogicalNotOperatorNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
   str.concat(&LogicalNot_str);
   return 0;
}

AbstractQoreNode* QoreLogicalNotOperatorNode::evalImpl(ExceptionSink* xsink) const {
   bool b = exp->boolEval(xsink);
   if (*xsink)
      return 0;
   return b ? (AbstractQoreNode*)&False : (AbstractQoreNode*)&True;
}

AbstractQoreNode* QoreLogicalNotOperatorNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
   assert(exp);
   needs_deref = false;
   bool b = exp->boolEval(xsink);
   if (*xsink)
      return 0;
   return b ? (AbstractQoreNode*)&False : (AbstractQoreNode*)&True;
}

AbstractQoreNode* QoreLogicalNotOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   if (exp) {
      exp = exp->parseInit(oflag, pflag, lvids, typeInfo);

      // evaluate immediately if possible
      if (exp->is_value()) {
	 SimpleRefHolder<QoreLogicalNotOperatorNode> th(this);

	 return exp->getAsBool() ? (AbstractQoreNode*)&False : (AbstractQoreNode*)&True;
      }
   }
   typeInfo = boolTypeInfo;
   
   return this;
}
