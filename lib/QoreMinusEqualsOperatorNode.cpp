/*
 QoreMinusEqualsOperatorNode.cpp
 
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

QoreString QoreMinusEqualsOperatorNode::op_str("-= operator expression");

AbstractQoreNode *QoreMinusEqualsOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) { 
   // turn off "reference ok" and "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   left = left->parseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, ti);
   checkLValue(left);

   const QoreTypeInfo *rightTypeInfo = 0;
   right = right->parseInit(oflag, pflag, lvids, rightTypeInfo);

   if (!ti->isType(NT_HASH)
       && !ti->isType(NT_OBJECT)
       && !ti->isType(NT_FLOAT)
       && !ti->isType(NT_DATE)) {
      // if the lhs type is not one of the above types, 
      // there are 2 possibilities: the lvalue has no value, in which
      // case it takes the value of the right side, or if it's anything else it's
      // converted to an integer, so we just check if it can be assigned an
      // integer value below, this is enough
      if (ti->returnsSingle()) {
	 check_lvalue_int(ti, "-=");
	 ti = bigIntTypeInfo;
	 return makeSpecialization<QoreIntMinusEqualsOperatorNode>();
      }
      else
	 ti = 0;
   }
   typeInfo = ti;

   return this;
}

AbstractQoreNode *QoreMinusEqualsOperatorNode::evalImpl(ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder new_right(right, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   if (is_nothing(*new_right)) {
      return ref_rv ? v.getReferencedValue() : 0;
   }

   // do float minus-equals if left side is a float
   qore_type_t vtype = v.getType();

   if (vtype == NT_NOTHING) {
      // see if the lvalue has a default type
      const QoreTypeInfo *typeInfo = v.getTypeInfo();
      if (typeInfo->hasDefaultValue()) {
	 if (v.assign(typeInfo->getDefaultValue()))
	    return 0;
	 vtype = v.getType();
      }
      else if (new_right) {
	 if (new_right->getType() == NT_FLOAT) {
	    const QoreFloatNode *f = reinterpret_cast<const QoreFloatNode *>(*new_right);
	    v.assign(new QoreFloatNode(-f->f));
	 }
	 else {
	    // optimization to eliminate a virtual function call in the most common case
	    int64 i = new_right->getAsBigInt();
	    v.assign(new QoreBigIntNode(-i));
	 }

	 if (*xsink)
	    return 0;

	 // v has been assigned to a value by this point
	 // reference return value
	 return ref_rv ? v.getReferencedValue() : 0;
      }
   }

   if (vtype == NT_FLOAT) {
      v.minusEqualsFloat(new_right->getAsFloat());
   }
   else if (vtype == NT_DATE) {
      DateTimeValueHelper date(*new_right);
      v.assign(reinterpret_cast<DateTimeNode *>(v.getValue())->subtractBy(*date));
   }
   else if (vtype == NT_HASH) {
      if (new_right->getType() != NT_HASH && new_right->getType() != NT_OBJECT) {
	 v.ensureUnique();
	 QoreHashNode *vh = reinterpret_cast<QoreHashNode *>(v.getValue());

	 const QoreListNode *nrl = (new_right->getType() == NT_LIST) ? reinterpret_cast<const QoreListNode *>(*new_right) : 0;
	 if (nrl && nrl->size()) {
	    // treat each element in the list as a string giving a key to delete
	    ConstListIterator li(nrl);
	    while (li.next()) {
	       QoreStringValueHelper val(li.getValue());
	       
	       vh->removeKey(*val, xsink);
	       if (*xsink)
		  return 0;
	    }
	 }
	 else {
	    QoreStringValueHelper str(*new_right);
	    vh->removeKey(*str, xsink);
	 }
      }
   }
   else if (vtype == NT_OBJECT) {
      if (new_right->getType() != NT_HASH && new_right->getType() != NT_OBJECT) {
	 QoreObject *o = reinterpret_cast<QoreObject *>(v.getValue());

	 const QoreListNode *nrl = (new_right->getType() == NT_LIST) ? reinterpret_cast<const QoreListNode *>(*new_right) : 0;
	 if (nrl && nrl->size()) {
	    // treat each element in the list as a string giving a key to delete
	    ConstListIterator li(nrl);
	    while (li.next()) {
	       QoreStringValueHelper val(li.getValue());
	       
	       o->removeMember(*val, xsink);
	       if (*xsink)
		  return 0;
	    }
	 }
	 else {
	    QoreStringValueHelper str(*new_right);
	    o->removeMember(*str, xsink);
	 }
      }
   }
   else { // do integer minus-equals
      v.minusEqualsBigInt(new_right->getAsBigInt());
   }

   if (*xsink)
      return 0;

   // here we know that v has a value
   // reference return value and return
   return ref_rv ? v.getReferencedValue() : 0;
}

AbstractQoreNode *QoreMinusEqualsOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreMinusEqualsOperatorNode::evalImpl(xsink);
}
