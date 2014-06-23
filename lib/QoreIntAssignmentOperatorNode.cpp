/*
  QoreIntAssignmentOperatorNode.cpp
 
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

AbstractQoreNode *QoreIntAssignmentOperatorNode::evalImpl(ExceptionSink *xsink) const {
   int64 rv = QoreIntAssignmentOperatorNode::bigIntEvalImpl(xsink);
   if (!ref_rv || *xsink)
      return 0;

   return new QoreBigIntNode(rv);
}

AbstractQoreNode *QoreIntAssignmentOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreIntAssignmentOperatorNode::evalImpl(xsink);
}

int64 QoreIntAssignmentOperatorNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   int64 rv = right->bigIntEval(xsink);
   if (*xsink)
      return 0;
   LValueHelper v(left, xsink);
   if (*xsink)
      return 0;
   v.assignBigInt(rv, "<+= operator>");
   return rv;
}

int QoreIntAssignmentOperatorNode::integerEvalImpl(ExceptionSink *xsink) const {
   return QoreIntAssignmentOperatorNode::bigIntEvalImpl(xsink);
}

double QoreIntAssignmentOperatorNode::floatEvalImpl(ExceptionSink *xsink) const {
   return QoreIntAssignmentOperatorNode::bigIntEvalImpl(xsink);
}

bool QoreIntAssignmentOperatorNode::boolEvalImpl(ExceptionSink *xsink) const {
   return QoreIntAssignmentOperatorNode::bigIntEvalImpl(xsink);
}
