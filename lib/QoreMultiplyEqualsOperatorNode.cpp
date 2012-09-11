/*
 QoreMultiplyEqualsOperatorNode.cpp
 
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

QoreString QoreMultiplyEqualsOperatorNode::op_str("*= operator expression");

AbstractQoreNode *QoreMultiplyEqualsOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   parseInitIntern(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);

   return this;
}

AbstractQoreNode *QoreMultiplyEqualsOperatorNode::evalImpl(ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder res(right, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // is either side a number?
   if ((res && res->getType() == NT_NUMBER) || v.getType() == NT_NUMBER) {
      v.multiplyEqualsNumber(*res, "<*= operator>");
   }
   // is either side a float?
   else if (v.getType() == NT_FLOAT)
      v.multiplyEqualsFloat(res ? res->getAsFloat() : 0.0, "<*= operator>");
   else {
      if (res && res->getType() == NT_FLOAT) {
	 v.multiplyEqualsFloat((reinterpret_cast<const QoreFloatNode*>(*res))->f, "<*= operator>");
      }
      else { // do integer multiply equals
        if (v.getType() == NT_NOTHING || !res) {
           if (v.assignBigInt(0))
              return 0;
        }
        else
	    v.multiplyEqualsBigInt(res->getAsBigInt(), "<*= operator>");
      }
   }

   // reference return value and return
   return ref_rv ? v.getReferencedValue() : 0;
}

AbstractQoreNode *QoreMultiplyEqualsOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreMultiplyEqualsOperatorNode::evalImpl(xsink);
}
