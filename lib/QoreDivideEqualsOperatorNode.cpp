/*
 QoreDivideEqualsOperatorNode.cpp
 
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

QoreString QoreDivideEqualsOperatorNode::op_str("/= operator expression");

AbstractQoreNode *QoreDivideEqualsOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   parseInitIntern(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);

   if (ti == bigIntTypeInfo || ti == softBigIntTypeInfo)
      return makeSpecialization<QoreIntDivideEqualsOperatorNode>();

   return this;
}

AbstractQoreNode *QoreDivideEqualsOperatorNode::evalImpl(ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder res(right, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // is either side a float?
   if (res && res->getType() == NT_FLOAT) {
      const QoreFloatNode *rf = reinterpret_cast<const QoreFloatNode *>(*res);

      if (!rf->f) {
	 xsink->raiseException("DIVISION-BY-ZERO", "division by zero in floating-point expression");
	 return 0;
      }

      if (v.ensure_unique_float())
	 return 0;

      QoreFloatNode *vf = reinterpret_cast<QoreFloatNode *>(v.get_value());
      // divide current value with arg val
      vf->f /= rf->f;
   }
   else if (v.get_type() == NT_FLOAT) {
      float val = res ? res->getAsFloat() : 0.0;
      if (val == 0.0) {
	 xsink->raiseException("DIVISION-BY-ZERO", "division by zero in floating-point expression");
	 return 0;
      }
      v.ensure_unique();

      QoreFloatNode *vf = reinterpret_cast<QoreFloatNode *>(v.get_value());
      vf->f /= val;
   }
   else { // do integer divide equals
      int64 val = res ? res->getAsBigInt() : 0;
      if (!val) {
	 xsink->raiseException("DIVISION-BY-ZERO", "division by zero in integer expression");
	 return 0;
      }
      // get new value if necessary
      if (v.get_type() == NT_NOTHING) {
	 if (v.assign(new QoreBigIntNode))
	    return 0;
      }
      else {
	 if (v.ensure_unique_int())
	    return 0;

	 QoreBigIntNode *b = reinterpret_cast<QoreBigIntNode *>(v.get_value());
	 
	 // divide current value with arg val
	 b->val /= val;
      }
   }

   assert(v.get_value());
   // reference return value and return
   return ref_rv ? v.get_value()->refSelf() : 0;
}

AbstractQoreNode *QoreDivideEqualsOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreDivideEqualsOperatorNode::evalImpl(xsink);
}
