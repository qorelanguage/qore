/*
  QoreMultiplyEqualsOperatorNode.cpp
 
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
