/*
 QoreIntMultiplyEqualsOperatorNode.cpp
 
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

AbstractQoreNode *QoreIntMultiplyEqualsOperatorNode::evalImpl(ExceptionSink *xsink) const {
   int64 rv = QoreIntMultiplyEqualsOperatorNode::bigIntEvalImpl(xsink);
   if (!ref_rv || *xsink)
      return 0;

   return new QoreBigIntNode(rv);
}

AbstractQoreNode *QoreIntMultiplyEqualsOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreIntMultiplyEqualsOperatorNode::evalImpl(xsink);
}

int64 QoreIntMultiplyEqualsOperatorNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   int64 rv = right->bigIntEval(xsink);
   if (*xsink)
      return 0;
   LValueHelper v(left, xsink);
   if (!v)
      return 0;
   return v.multiplyEqualsBigInt(rv, "<*= operator>");
}

int QoreIntMultiplyEqualsOperatorNode::integerEvalImpl(ExceptionSink *xsink) const {
   return QoreIntMultiplyEqualsOperatorNode::bigIntEvalImpl(xsink);
}

double QoreIntMultiplyEqualsOperatorNode::floatEvalImpl(ExceptionSink *xsink) const {
   return QoreIntMultiplyEqualsOperatorNode::bigIntEvalImpl(xsink);
}

bool QoreIntMultiplyEqualsOperatorNode::boolEvalImpl(ExceptionSink *xsink) const {
   return QoreIntMultiplyEqualsOperatorNode::bigIntEvalImpl(xsink);
}
