/*
 QorePostIncrementOperatorNode.cpp
 
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

QoreString QorePostIncrementOperatorNode::op_str("++ (post-increment) operator expression");

AbstractQoreNode *QorePostIncrementOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   parseInitIntern(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);

   // version for local var
   return (typeInfo == bigIntTypeInfo || typeInfo == softBigIntTypeInfo) ? makeSpecialization<QoreIntPostIncrementOperatorNode>() : this;
}

AbstractQoreNode *QorePostIncrementOperatorNode::evalImpl(ExceptionSink *xsink) const {
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper n(exp, xsink);
   if (!n)
      return 0;
   if (n.getType() == NT_NUMBER)
      return n.postIncrementNumber(ref_rv, "<-- (post) operator>");

   if (n.getType() == NT_FLOAT) {
      double f = n.postIncrementFloat("<++ (post) operator>");
      assert(!*xsink);
      return ref_rv ? new QoreFloatNode(f) : 0;
   }

   int64 rc = n.postIncrementBigInt("<++ (post) operator>");
   return *xsink || !ref_rv ? 0 : new QoreBigIntNode(rc);
}

AbstractQoreNode *QorePostIncrementOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QorePostIncrementOperatorNode::evalImpl(xsink);
}
