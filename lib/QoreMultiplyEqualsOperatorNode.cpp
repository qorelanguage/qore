/*
 QoreMultiplyEqualsOperatorNode.cpp
 
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

QoreString QoreMultiplyEqualsOperatorNode::op_str("*= operator expression");

AbstractQoreNode *QoreMultiplyEqualsOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   parseInitIntern(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);

   if (ti == bigIntTypeInfo || ti == softBigIntTypeInfo)
      return makeSpecialization<QoreIntMultiplyEqualsOperatorNode>();

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

   // is either side a float?
   if (v.get_type() == NT_FLOAT) {
      double f = res ? res->getAsFloat() : 0;

      if (f) {
	 v.ensure_unique();
	 QoreFloatNode *vf = reinterpret_cast<QoreFloatNode *>(v.get_value());
	 vf->f *= f;
      }
      else { // if factor is NOTHING, assign 0.0
	 if (v.assign(new QoreFloatNode))
	    return 0;
      }
   }
   else {
      if (res && res->getType() == NT_FLOAT) {
	 if (v.ensure_unique_float())
	    return 0;

	 // multiply current value with arg val
	 QoreFloatNode *vf = reinterpret_cast<QoreFloatNode *>(v.get_value());
	 vf->f *= (reinterpret_cast<const QoreFloatNode *>(*res))->f;
      }
      else { // do integer multiply equals
	 // get new value if necessary
	 if (v.get_type() == NT_NOTHING) {
	    if (v.assign(new QoreBigIntNode))
	       return 0;
	 }
	 else {
	    if (res) {
	       if (v.ensure_unique_int())
		  return 0;

	       QoreBigIntNode *b = reinterpret_cast<QoreBigIntNode *>(v.get_value());
	       
	       // multiply current value with arg val
	       b->val *= res->getAsBigInt();
	    }
	    else { // if factor is NOTHING, assign 0
	       if (v.assign(new QoreBigIntNode))
		  return 0;
	    }
	 }
      }
   }

   // reference return value and return
   return ref_rv ? v.get_value()->refSelf() : 0;
}

AbstractQoreNode *QoreMultiplyEqualsOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreMultiplyEqualsOperatorNode::evalImpl(xsink);
}
