/*
 QoreIntPreDecrementOperatorNode.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2013 David Nichols
 
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
 Foundation, Dec., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <qore/Qore.h>

QoreString QoreIntPreDecrementOperatorNode::op_str("-- (pre-decrement) operator expression");

AbstractQoreNode *QoreIntPreDecrementOperatorNode::evalImpl(ExceptionSink *xsink) const {
   int64 rv = QoreIntPreDecrementOperatorNode::bigIntEvalImpl(xsink);
   if (!ref_rv || *xsink)
      return 0;

   return new QoreBigIntNode(rv);
}

AbstractQoreNode *QoreIntPreDecrementOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreIntPreDecrementOperatorNode::evalImpl(xsink);
}

int64 QoreIntPreDecrementOperatorNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   LValueHelper n(exp, xsink);
   if (!n)
      return 0;
   return n.preDecrementBigInt("<-- (pre) operator>");
}

int QoreIntPreDecrementOperatorNode::integerEvalImpl(ExceptionSink *xsink) const {
   return QoreIntPreDecrementOperatorNode::bigIntEvalImpl(xsink);
}

double QoreIntPreDecrementOperatorNode::floatEvalImpl(ExceptionSink *xsink) const {
   return QoreIntPreDecrementOperatorNode::bigIntEvalImpl(xsink);
}

bool QoreIntPreDecrementOperatorNode::boolEvalImpl(ExceptionSink *xsink) const {
   return QoreIntPreDecrementOperatorNode::bigIntEvalImpl(xsink);
}
