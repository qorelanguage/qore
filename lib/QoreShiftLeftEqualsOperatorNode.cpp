/*
 QoreShiftLeftEqualsOperatorNode.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2011 David Nichols
 
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

QoreString QoreShiftLeftEqualsOperatorNode::op_str("<<= operator expression");

AbstractQoreNode *QoreShiftLeftEqualsOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   parseInitIntLValue(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);
   // version for local var
   return makeSpecialization<QoreIntShiftLeftEqualsOperatorNode>();
}

AbstractQoreNode *QoreShiftLeftEqualsOperatorNode::evalImpl(ExceptionSink *xsink) const {
   int64 val = right->bigIntEval(xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // get new value if necessary
   if (v.ensure_unique_int())
      return 0;

   QoreBigIntNode *b = reinterpret_cast<QoreBigIntNode *>(v.get_value());

   // ShiftLeft current value with arg val
   b->val <<= val;

   // reference return value and return
   return ref_rv ? v.get_value()->refSelf() : 0;
}

AbstractQoreNode *QoreShiftLeftEqualsOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreShiftLeftEqualsOperatorNode::evalImpl(xsink);
}
