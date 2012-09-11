/*
 QorePostDecrementOperatorNode.cpp
 
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
 Foundation, Dec., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <qore/Qore.h>

QoreString QorePostDecrementOperatorNode::op_str("-- (post-decrement) operator expression");

AbstractQoreNode *QorePostDecrementOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   parseInitIntern(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);

   // version for local var
   return (typeInfo == bigIntTypeInfo || typeInfo == softBigIntTypeInfo) ? makeSpecialization<QoreIntPostDecrementOperatorNode>() : this;
}

AbstractQoreNode *QorePostDecrementOperatorNode::evalImpl(ExceptionSink *xsink) const {
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper n(exp, xsink);
   if (!n)
      return 0;
   if (n.getType() == NT_NUMBER)
      return n.postDecrementNumber(ref_rv, "<-- (post) operator>");

   if (n.getType() == NT_FLOAT) {
      double f = n.postDecrementFloat("<-- (post) operator>");
      assert(!*xsink);
      return ref_rv ? new QoreFloatNode(f) : 0;
   }

   int64 rc = n.postDecrementBigInt("<-- (post) operator>");
   return *xsink || !ref_rv ? 0 : new QoreBigIntNode(rc);
}

AbstractQoreNode *QorePostDecrementOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QorePostDecrementOperatorNode::evalImpl(xsink);
}
