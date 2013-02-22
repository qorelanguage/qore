/*
 QoreIntPreIncrementOperatorNode.cpp
 
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
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <qore/Qore.h>

QoreString QoreIntPreIncrementOperatorNode::op_str("++ (pre-increment) operator expression");

AbstractQoreNode *QoreIntPreIncrementOperatorNode::evalImpl(ExceptionSink *xsink) const {
   int64 rv = QoreIntPreIncrementOperatorNode::bigIntEvalImpl(xsink);
   if (!ref_rv || *xsink)
      return 0;

   return new QoreBigIntNode(rv);
}

AbstractQoreNode *QoreIntPreIncrementOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreIntPreIncrementOperatorNode::evalImpl(xsink);
}

int64 QoreIntPreIncrementOperatorNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   LValueHelper n(exp, xsink);
   if (!n)
      return 0;
   return n.preIncrementBigInt("<++ (pre) operator>");
}

int QoreIntPreIncrementOperatorNode::integerEvalImpl(ExceptionSink *xsink) const {
   return QoreIntPreIncrementOperatorNode::bigIntEvalImpl(xsink);
}

double QoreIntPreIncrementOperatorNode::floatEvalImpl(ExceptionSink *xsink) const {
   return QoreIntPreIncrementOperatorNode::bigIntEvalImpl(xsink);
}

bool QoreIntPreIncrementOperatorNode::boolEvalImpl(ExceptionSink *xsink) const {
   return QoreIntPreIncrementOperatorNode::bigIntEvalImpl(xsink);
}
