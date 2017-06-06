/*
  QoreMinusEqualsOperatorNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

QoreString QoreMinusEqualsOperatorNode::op_str("-= operator expression");

AbstractQoreNode *QoreMinusEqualsOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   // turn off "reference ok" and "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   left = left->parseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, ti);
   checkLValue(left, pflag);

   const QoreTypeInfo *rightTypeInfo = 0;
   right = right->parseInit(oflag, pflag, lvids, rightTypeInfo);

   if (!QoreTypeInfo::isType(ti, NT_HASH)
       && !QoreTypeInfo::isType(ti, NT_OBJECT)
       && !QoreTypeInfo::isType(ti, NT_FLOAT)
       && !QoreTypeInfo::isType(ti, NT_NUMBER)
       && !QoreTypeInfo::isType(ti, NT_DATE)) {
      // if the lhs type is not one of the above types,
      // there are 2 possibilities: the lvalue has no value, in which
      // case it takes the value of the right side, or if it's anything else it's
      // converted to an integer, so we just check if it can be assigned an
      // integer value below, this is enough
      if (QoreTypeInfo::returnsSingle(ti)) {
	 check_lvalue_int(loc, ti, "-=");
	 ti = bigIntTypeInfo;
	 return makeSpecialization<QoreIntMinusEqualsOperatorNode>();
      }
      else
	 ti = 0;
   }
   typeInfo = ti;

   return this;
}

QoreValue QoreMinusEqualsOperatorNode::evalValueImpl(bool &needs_deref, ExceptionSink *xsink) const {
   ValueEvalRefHolder new_right(right, xsink);
   if (*xsink)
      return QoreValue();

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return QoreValue();

   if (new_right->isNothing())
      return needs_deref ? v.getReferencedValue() : QoreValue();

   // do float minus-equals if left side is a float
   qore_type_t vtype = v.getType();

   //printd(5, "QoreMinusEqualsOperatorNode::evalValueImpl() vtype: %d rtype: %d\n", vtype, new_right->getType());

   if (vtype == NT_NOTHING) {
      // see if the lvalue has a default type
      const QoreTypeInfo* typeInfo = v.getTypeInfo();
      if (QoreTypeInfo::hasDefaultValue(typeInfo)) {
	 if (v.assign(QoreTypeInfo::getDefaultValue(typeInfo)))
	    return QoreValue();
	 vtype = v.getType();
      }
      else {
	 if (new_right->getType() == NT_FLOAT) {
	    v.assign(-new_right->getAsFloat());
	 }
	 else if (new_right->getType() == NT_NUMBER) {
            const QoreNumberNode* num = reinterpret_cast<const QoreNumberNode*>(new_right->getInternalNode());
            v.assign(num->negate());
         }
	 else
	    v.assign(-new_right->getAsBigInt());

	 if (*xsink)
	    return QoreValue();

	 // v has been assigned to a value by this point
	 return ref_rv ? v.getReferencedValue() : QoreValue();
      }
   }

   if (vtype == NT_FLOAT)
      return v.minusEqualsFloat(new_right->getAsFloat());
   else if (vtype == NT_NUMBER) {
      // FIXME: could be more efficient
      ReferenceHolder<> ra(new_right.getReferencedValue(), xsink);
      v.minusEqualsNumber(*ra, "<-= operator>");
   }
   else if (vtype == NT_DATE) {
      // get a relative date-time value
      DateTime date(*new_right);
      //DateTimeValueHelper date(*new_right);
      v.assign(reinterpret_cast<DateTimeNode*>(v.getValue())->subtractBy(date));
   }
   else if (vtype == NT_HASH) {
      if (new_right->getType() != NT_HASH && new_right->getType() != NT_OBJECT) {
	 v.ensureUnique();
	 QoreHashNode* vh = reinterpret_cast<QoreHashNode*>(v.getValue());

	 const QoreListNode* nrl = (new_right->getType() == NT_LIST) ? reinterpret_cast<const QoreListNode*>(new_right->getInternalNode()) : 0;
	 if (nrl && nrl->size()) {
	    // treat each element in the list as a string giving a key to delete
	    ConstListIterator li(nrl);
	    while (li.next()) {
	       QoreStringValueHelper val(li.getValue());

	       vh->removeKey(*val, xsink);
	       if (*xsink)
		  return QoreValue();
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
	 QoreObject *o = reinterpret_cast<QoreObject*>(v.getValue());

	 const QoreListNode *nrl = (new_right->getType() == NT_LIST) ? reinterpret_cast<const QoreListNode *>(new_right->getInternalNode()) : 0;
	 if (nrl && nrl->size()) {
	    // treat each element in the list as a string giving a key to delete
	    ConstListIterator li(nrl);
	    while (li.next()) {
	       QoreStringValueHelper val(li.getValue());

	       o->removeMember(*val, xsink);
	       if (*xsink)
		  return QoreValue();
	    }
	 }
	 else {
	    QoreStringValueHelper str(*new_right);
	    o->removeMember(*str, xsink);
	 }
      }
   }
   else // do integer minus-equals
      return v.minusEqualsBigInt(new_right->getAsBigInt());

   if (*xsink)
      return QoreValue();

   // here we know that v has a value
   // reference return value and return
   return ref_rv ? v.getReferencedValue() : QoreValue();
}
