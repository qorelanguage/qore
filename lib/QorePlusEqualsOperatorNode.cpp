/*
 QorePlusEqualsOperatorNode.cpp
 
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

QoreString QorePlusEqualsOperatorNode::op_str("+= operator expression");

AbstractQoreNode *QorePlusEqualsOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) { 
   // turn off "reference ok" and "return value ignored" flags
   pflag &= ~(PF_REFERENCE_OK | PF_RETURN_VALUE_IGNORED);

   left = left->parseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, ti);

   const QoreTypeInfo *rightTypeInfo = 0;
   right = right->parseInit(oflag, pflag, lvids, rightTypeInfo);

   if (!ti->isType(NT_LIST)
       && !ti->isType(NT_HASH)
       && !ti->isType(NT_OBJECT)
       && !ti->isType(NT_STRING)
       && !ti->isType(NT_FLOAT)
       && !ti->isType(NT_DATE)
       && !ti->isType(NT_BINARY)) {
      // if the lhs type is not one of the above types, 
      // there are 2 possibilities: the lvalue has no value, in which
      // case it takes the value of the right side, or if it's anything else it's
      // converted to an integer, so we just check if it can be assigned an
      // integer value below, this is enough
      if (ti->returnsSingle()) {
	 ti = bigIntTypeInfo;
	 check_lvalue_int(ti, "+=");
	 return makeSpecialization<QoreIntPlusEqualsOperatorNode>();
      }
      else
	 ti = 0;
   }
   typeInfo = ti;

   return this;
}

AbstractQoreNode *QorePlusEqualsOperatorNode::evalImpl(ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder new_right(right, xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // dereferences happen in each section so that the
   // already referenced value can be passed to list->push()
   // if necessary
   // do list plus-equals if left-hand side is a list
   qore_type_t vtype = v.get_type();

   if (vtype == NT_NOTHING) {
      // see if the lvalue has a default type
      const QoreTypeInfo *typeInfo = v.get_type_info();
      if (typeInfo->hasDefaultValue()) {
	 if (v.assign(typeInfo->getDefaultValue()))
	    return 0;
	 vtype = v.get_type();
      }
      else if (new_right) {
	 // assign rhs to lhs (take reference for plusequals)
	 if (v.assign(new_right.getReferencedValue()))
	    return 0;

	 // v has been assigned to a value by this point
	 // reference return value
	 return ref_rv ? v.get_value()->refSelf() : 0;
      }
   }

   if (vtype == NT_LIST) {
      v.ensure_unique(); // no exception possible here
      QoreListNode *l = reinterpret_cast<QoreListNode *>(v.get_value());
      if (new_right && new_right->getType() == NT_LIST)
	 l->merge(reinterpret_cast<const QoreListNode *>(*new_right));
      else
	 l->push(new_right.getReferencedValue());
   } // do hash plus-equals if left side is a hash
   else if (vtype == NT_HASH) {
      if (new_right) {
	 if (new_right->getType() == NT_HASH) {
	    v.ensure_unique();
	    reinterpret_cast<QoreHashNode *>(v.get_value())->merge(reinterpret_cast<const QoreHashNode *>(*new_right), xsink);
	 }
	 else if (new_right->getType() == NT_OBJECT) {
	    v.ensure_unique();
	    const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(*new_right))->mergeDataToHash(reinterpret_cast<QoreHashNode *>(v.get_value()), xsink);
	 }
      }
   }
   // do hash/object plus-equals if left side is an object
   else if (vtype == NT_OBJECT) {
      QoreObject *o = reinterpret_cast<QoreObject *>(v.get_value());
      qore_object_private::plusEquals(o, *new_right, v.getObjMap(), v.getAutoVLock(), xsink);
      // duplicates are checked in the call above
      v.alreadyChecked();
   }
   // do string plus-equals if left-hand side is a string
   else if (vtype == NT_STRING) {
      if (new_right) {
	 QoreStringValueHelper str(*new_right);

	 v.ensure_unique();
	 QoreStringNode *vs = reinterpret_cast<QoreStringNode *>(v.get_value());
	 vs->concat(*str, xsink);
      }
   }
   else if (vtype == NT_FLOAT) {
      double f = new_right ? new_right->getAsFloat() : 0.0;
      if (f != 0.0) {
	 v.ensure_unique();
	 QoreFloatNode *vf = reinterpret_cast<QoreFloatNode *>(v.get_value());
	 vf->f += f;
      }
   }
   else if (vtype == NT_DATE) {
      if (new_right) {
	 DateTimeValueHelper date(*new_right);
	 v.assign(reinterpret_cast<DateTimeNode *>(v.get_value())->add(*date));
      }
   }
   else if (vtype == NT_BINARY) {
      if (new_right) {
	 v.ensure_unique();
	 BinaryNode *b = reinterpret_cast<BinaryNode *>(v.get_value());
	 if (new_right->getType() == NT_BINARY) {
	    const BinaryNode *arg = reinterpret_cast<const BinaryNode *>(*new_right);
	    b->append(arg);
	 }
	 else {
	    QoreStringNodeValueHelper str(*new_right);
	    if (str->strlen())
	       b->append(str->getBuffer(), str->strlen());
	 }
      }
   }
   else { // do integer plus-equals
      v.plusEqualsBigInt(new_right ? new_right->getAsBigInt() : 0);
   }
   if (*xsink)
      return 0;

   // v has been assigned to a value by this point
   // reference return value
   return ref_rv ? v.getReferencedValue() : 0;
}

AbstractQoreNode *QorePlusEqualsOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QorePlusEqualsOperatorNode::evalImpl(xsink);
}
