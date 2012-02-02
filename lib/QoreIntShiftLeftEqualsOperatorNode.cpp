/*
 QoreIntShiftLeftEqualsOperatorNode.cpp
 
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

AbstractQoreNode *QoreIntShiftLeftEqualsOperatorNode::evalImpl(ExceptionSink *xsink) const {
   int64 rv = QoreIntShiftLeftEqualsOperatorNode::bigIntEvalImpl(xsink);
   if (!ref_rv || *xsink)
      return 0;

   return new QoreBigIntNode(rv);
}

AbstractQoreNode *QoreIntShiftLeftEqualsOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreIntShiftLeftEqualsOperatorNode::evalImpl(xsink);
}

int64 QoreIntShiftLeftEqualsOperatorNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   int64 rv = right->bigIntEval(xsink);
   if (*xsink)
      return 0;

   VarRefNode *v = reinterpret_cast<VarRefNode *>(left);
   return v->shiftLeftEqualsBigInt(rv, xsink);
}

int QoreIntShiftLeftEqualsOperatorNode::integerEvalImpl(ExceptionSink *xsink) const {
   return QoreIntShiftLeftEqualsOperatorNode::bigIntEvalImpl(xsink);
}

double QoreIntShiftLeftEqualsOperatorNode::floatEvalImpl(ExceptionSink *xsink) const {
   return QoreIntShiftLeftEqualsOperatorNode::bigIntEvalImpl(xsink);
}

bool QoreIntShiftLeftEqualsOperatorNode::boolEvalImpl(ExceptionSink *xsink) const {
   return QoreIntShiftLeftEqualsOperatorNode::bigIntEvalImpl(xsink);
}
