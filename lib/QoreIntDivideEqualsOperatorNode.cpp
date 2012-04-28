/*
 QoreIntDivideEqualsOperatorNode.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2012 David Nichols
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <qore/Qore.h>

AbstractQoreNode *QoreIntDivideEqualsOperatorNode::evalImpl(ExceptionSink *xsink) const {
   int64 rv = QoreIntDivideEqualsOperatorNode::bigIntEvalImpl(xsink);
   if (!ref_rv || *xsink)
      return 0;

   return new QoreBigIntNode(rv);
}

AbstractQoreNode *QoreIntDivideEqualsOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreIntDivideEqualsOperatorNode::evalImpl(xsink);
}

int64 QoreIntDivideEqualsOperatorNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   int64 rv = right->bigIntEval(xsink);
   if (*xsink)
      return 0;

   if (!rv) {
      xsink->raiseException("DIVISION-BY-ZERO", "division by zero in integer expression");
      return 0;
   }
   LValueHelper v(left, xsink);
   if (!v)
      return 0;
   return v.divideEqualsBigInt(rv, "</= operator>");
}

int QoreIntDivideEqualsOperatorNode::integerEvalImpl(ExceptionSink *xsink) const {
   return QoreIntDivideEqualsOperatorNode::bigIntEvalImpl(xsink);
}

double QoreIntDivideEqualsOperatorNode::floatEvalImpl(ExceptionSink *xsink) const {
   return QoreIntDivideEqualsOperatorNode::bigIntEvalImpl(xsink);
}

bool QoreIntDivideEqualsOperatorNode::boolEvalImpl(ExceptionSink *xsink) const {
   return QoreIntDivideEqualsOperatorNode::bigIntEvalImpl(xsink);
}
