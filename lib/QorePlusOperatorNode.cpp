/*
  QorePlusOperatorNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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
#include <qore/intern/QoreObjectIntern.h>

QoreString QorePlusOperatorNode::plus_str("+ operator expression");

QoreValue QorePlusOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder lh(left, xsink);
   if (*xsink)
      return QoreValue();
   ValueEvalRefHolder rh(right, xsink);
   if (*xsink)
      return QoreValue();

   qore_type_t lt = lh->getType();
   qore_type_t rt = rh->getType();

   if (lt == NT_LIST) {
      const QoreListNode* l = lh->get<const QoreListNode>();
      QoreListNode* rv = l->copy();
      if (rt == NT_LIST)
	 rv->merge(rh->get<const QoreListNode>());
      else
	 rv->push(rh->getReferencedValue());
      //printd(5, "QorePlusOperatorNode::evalValueImpl() returning list=%p size=%d\n", rv, rv->size());
      return rv;
   }

   if (rt == NT_LIST) {
      const QoreListNode* r = rh->get<const QoreListNode>();

      QoreListNode* rv = new QoreListNode;
      rv->push(lh->getReferencedValue());
      rv->merge(r);
      return rv;
   }

   if (lt == NT_STRING) {
      QoreStringNodeHolder str(new QoreStringNode(*lh->get<const QoreStringNode>()));

      if (rt == NT_STRING)
	 str->concat(rh->get<const QoreStringNode>(), xsink);
      else {
	 QoreStringValueHelper r(*rh, str->getEncoding(), xsink);
	 if (*xsink)
	    return QoreValue();
	 str->concat(*r, xsink);
      }
      return str.release();
   }

   if (rt == NT_STRING) {
      const QoreStringNode* r = rh->get<const QoreStringNode>();
      QoreStringNodeValueHelper strval(*lh, r->getEncoding(), xsink);
      if (*xsink)
         return QoreValue();
      SimpleRefHolder<QoreStringNode> str(strval->is_unique() ? strval.getReferencedValue() : new QoreStringNode(*strval));
      assert(str->reference_count() == 1);

      QoreStringNode* rv = const_cast<QoreStringNode*>(*str);

      rv->concat(r, xsink);
      if (*xsink)
         return QoreValue();
      return str.release();
   }

   if (lt == NT_DATE || rt == NT_DATE) {
      DateTimeNodeValueHelper l(*lh);
      DateTimeValueHelper r(*rh);
      return l->add(*r);
   }

   if (lt == NT_NUMBER || rt == NT_NUMBER) {
      QoreNumberNodeHelper l(*lh);
      QoreNumberNodeHelper r(*rh);
      return l->doPlus(**r);
   }

   if (lt == NT_FLOAT || rt == NT_FLOAT) {
      return lh->getAsFloat() + rh->getAsFloat();
   }

   if (lt == NT_INT || rt == NT_INT) {
      return lh->getAsBigInt() + rh->getAsBigInt();
   }

   if (lt == NT_HASH) {
      const QoreHashNode* l = lh->get<const QoreHashNode>();
      if (rt == NT_HASH) {
	 const QoreHashNode* r = rh->get<const QoreHashNode>();
	 ReferenceHolder<QoreHashNode> rv(l->copy(), xsink);
	 rv->merge(r, xsink);
	 if (*xsink)
	    return 0;
	 return rv.release();
      }
      if (rt == NT_OBJECT) {
	 QoreObject* r = rh->get<QoreObject>();
	 ReferenceHolder<QoreHashNode> rv(l->copy(), xsink);
	 qore_object_private::get(*r)->mergeDataToHash(*rv, xsink);
	 if (*xsink)
	    return 0;

	 return rv.release();
      }
      return l->refSelf();
   }

   if (lt == NT_OBJECT) {
      QoreObject* l = lh->get<QoreObject>();
      if (rt != NT_HASH)
	 return l->refSelf();
      const QoreHashNode* r = rh->get<const QoreHashNode>();

      ReferenceHolder<QoreHashNode> h(qore_object_private::get(*l)->getRuntimeMemberHash(xsink), xsink);
      if (*xsink)
	 return 0;

      h->merge(r, xsink);
      if (*xsink)
	 return 0;

      return h.release();
   }

   if (rt == NT_HASH || rt == NT_OBJECT) {
      return rh->getReferencedValue();
   }

   if (lt == NT_BINARY) {
      if (rt != NT_BINARY)
	 return lh->getReferencedValue();

      BinaryNode* rv = lh->get<const BinaryNode>()->copy();
      rv->append(rh->get<const BinaryNode>());
      return rv;
   }

   if (rt == NT_BINARY) {
      return rh->getReferencedValue();
   }

   return QoreValue();
}

AbstractQoreNode* QorePlusOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& parseTypeInfo) {
   // turn off "reference ok" and "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   assert(!parseTypeInfo);

   const QoreTypeInfo* leftTypeInfo = 0, *rightTypeInfo = 0;

   left = left->parseInit(oflag, pflag, lvids, leftTypeInfo);
   right = right->parseInit(oflag, pflag, lvids, rightTypeInfo);

   // see if both arguments are constants, then eval immediately and substitute this node with the result
   if (right && right->is_value() && left && left->is_value()) {
      SimpleRefHolder<QorePlusOperatorNode> del(this);
      ParseExceptionSink xsink;
      AbstractQoreNode* rv = QorePlusOperatorNode::evalImpl(*xsink);
      return rv ? rv : &Nothing;
   }

   // if either side is a list, then the return type is list (highest priority)
   if (leftTypeInfo->isType(NT_LIST) || rightTypeInfo->isType(NT_LIST))
      returnTypeInfo = listTypeInfo;
   // otherwise only set return type if return types on both sides are known at parse time
   else if (leftTypeInfo->hasType() && rightTypeInfo->hasType()) {
      if (leftTypeInfo->isType(NT_STRING)
	  || rightTypeInfo->isType(NT_STRING))
	 returnTypeInfo = stringTypeInfo;

      else if (leftTypeInfo->isType(NT_DATE)
	       || rightTypeInfo->isType(NT_DATE))
	 returnTypeInfo = dateTypeInfo;

      else if (leftTypeInfo->isType(NT_NUMBER)
               || rightTypeInfo->isType(NT_NUMBER))
         returnTypeInfo = numberTypeInfo;

      else if (leftTypeInfo->isType(NT_FLOAT)
	       || rightTypeInfo->isType(NT_FLOAT))
	 returnTypeInfo = floatTypeInfo;

      else if (leftTypeInfo->isType(NT_INT)
	       || rightTypeInfo->isType(NT_INT))
	 returnTypeInfo = bigIntTypeInfo;

      else if (leftTypeInfo->isType(NT_HASH)
	       || leftTypeInfo->isType(NT_OBJECT))
	 returnTypeInfo = hashTypeInfo;

      else if (rightTypeInfo->isType(NT_OBJECT))
	 returnTypeInfo = objectTypeInfo;

      else if (leftTypeInfo->isType(NT_BINARY)
	       || rightTypeInfo->isType(NT_BINARY))
	 returnTypeInfo = binaryTypeInfo;

      else if (leftTypeInfo->returnsSingle() && rightTypeInfo->returnsSingle())
	 // only return type nothing if both types are available and return a single type
	 returnTypeInfo = nothingTypeInfo;
   }

   if (returnTypeInfo)
      parseTypeInfo = returnTypeInfo;

   return this;
}
