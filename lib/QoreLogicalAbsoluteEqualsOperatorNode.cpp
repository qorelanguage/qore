/*
  QoreLogicalAbsoluteEqualsOperatorNode.cpp

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

QoreString QoreLogicalAbsoluteEqualsOperatorNode::logical_absolute_equals_str("logical absolute equals (===) operator expression");
QoreString QoreLogicalAbsoluteNotEqualsOperatorNode::logical_absolute_not_equals_str("logical absolute not equals (!==) operator expression");

QoreValue QoreLogicalAbsoluteEqualsOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder l(left, xsink);
   if (*xsink)
      return QoreValue();

   ValueEvalRefHolder r(right, xsink);
   if (*xsink)
      return QoreValue();

   return hardEqual(*l, *r, xsink);
}

AbstractQoreNode *QoreLogicalAbsoluteEqualsOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   typeInfo = boolTypeInfo;

   const QoreTypeInfo *lti = 0, *rti = 0;

   left = left->parseInit(oflag, pflag, lvids, lti);
   right = right->parseInit(oflag, pflag, lvids, rti);

   // see if both arguments are constants, then eval immediately and substitute this node with the result
   if (left && left->is_value() && right && right->is_value()) {
      SimpleRefHolder<QoreLogicalAbsoluteEqualsOperatorNode> del(this);
      ParseExceptionSink xsink;
      AbstractQoreNode *rv = get_bool_node(hardEqual(left, right, *xsink));
      return rv;
   }

   return this;
}

bool QoreLogicalAbsoluteEqualsOperatorNode::hardEqual(const QoreValue left, const QoreValue right, ExceptionSink *xsink) {
   qore_type_t lt = left.getType();
   qore_type_t rt = right.getType();

   if (lt != rt)
      return false;

   return left.isEqualHard(right);
}
