/*
 QoreUnaryMinusOperatorNode.cpp
 
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
#include <qore/intern/qore_number_private.h>

QoreString QoreUnaryMinusOperatorNode::unaryminus_str("unary minus operator expression");

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreUnaryMinusOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return &unaryminus_str;
}

int QoreUnaryMinusOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(&unaryminus_str);
   return 0;
}

AbstractQoreNode *QoreUnaryMinusOperatorNode::evalImpl(ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder v(exp, xsink);
   if (*xsink)
      return 0;

   if (v) {
      if (v->getType() == NT_NUMBER)
         return reinterpret_cast<const QoreNumberNode*>(*v)->negate();
      if (v->getType() == NT_FLOAT)
	 return new QoreFloatNode(-reinterpret_cast<const QoreFloatNode*>(*v)->f);
      if (v->getType() == NT_DATE)
	 return reinterpret_cast<const DateTimeNode*>(*v)->unaryMinus();
      if (v->getType() == NT_INT)
	 return new QoreBigIntNode(-reinterpret_cast<const QoreBigIntNode*>(*v)->val);
   }
   // return zero
   return Zero->refSelf();
}

AbstractQoreNode *QoreUnaryMinusOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder v(exp, xsink);
   if (*xsink)
      return 0;

   if (v) {
      if (v->getType() == NT_NUMBER) {
	 needs_deref = true;
         return reinterpret_cast<const QoreNumberNode*>(*v)->negate();
      }
      if (v->getType() == NT_FLOAT) {
	 needs_deref = true;
	 return new QoreFloatNode(-reinterpret_cast<const QoreFloatNode*>(*v)->f);
      }
      if (v->getType() == NT_DATE) {
	 needs_deref = true;
	 return reinterpret_cast<const DateTimeNode*>(*v)->unaryMinus();
      }
      if (v->getType() == NT_INT) {
	 needs_deref = true;
	 return new QoreBigIntNode(-reinterpret_cast<const QoreBigIntNode*>(*v)->val);
      }
   }
   // return zero
   needs_deref = false;
   return Zero;
}

AbstractQoreNode *QoreUnaryMinusOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   if (exp) {
      exp = exp->parseInit(oflag, pflag, lvids, typeInfo);

      // evaluate immediately if possible
      if (exp->is_value()) {
	 SimpleRefHolder<QoreUnaryMinusOperatorNode> th(this);

	 qore_type_t t = exp->getType();
	 if (t == NT_INT) {
	    typeInfo = bigIntTypeInfo;
	    return new QoreBigIntNode(-reinterpret_cast<const QoreBigIntNode*>(exp)->val);
	 }
         if (t == NT_NUMBER) {
            typeInfo = numberTypeInfo;
            return reinterpret_cast<const QoreNumberNode*>(exp)->negate();
         }
	 if (t == NT_FLOAT) {
	    typeInfo = floatTypeInfo;
	    return new QoreFloatNode(-reinterpret_cast<const QoreFloatNode*>(exp)->f);
	 }
	 if (t == NT_DATE) {
	    typeInfo = dateTypeInfo;
	    return reinterpret_cast<const DateTimeNode*>(exp)->unaryMinus();
	 }

	 th.release();	 
      }

      if (typeInfo->hasType()) {
	 if (typeInfo->isType(NT_FLOAT))
	    typeInfo = floatTypeInfo;
	 else if (typeInfo->isType(NT_NUMBER))
            typeInfo = numberTypeInfo;
	 else if (typeInfo->isType(NT_DATE))
	    typeInfo = dateTypeInfo;
	 else if (typeInfo->isType(NT_INT))
	    typeInfo = bigIntTypeInfo;
	 else {
	    if (typeInfo->returnsSingle())
	       typeInfo = bigIntTypeInfo;
	    else
	       typeInfo = 0;
	 }
	 // FIXME: add invalid operation warning for types that can't convert to int
      }
   }
   else
      typeInfo = 0;

   returnTypeInfo = typeInfo;
   return this;
}

// static function
AbstractQoreNode* QoreUnaryMinusOperatorNode::makeNode(AbstractQoreNode *v) {
   if (v) {
      assert(v->is_unique());
      if (v->getType() == NT_NUMBER) {
         qore_number_private::negateInPlace(*reinterpret_cast<QoreNumberNode*>(v));
	 return v;
      }

      if (v->getType() == NT_FLOAT) {
	 QoreFloatNode* f = reinterpret_cast<QoreFloatNode*>(v);
	 f->f = -f->f;
	 return v;
      }

      if (v->getType() == NT_DATE) {
	 reinterpret_cast<DateTimeNode*>(v)->unaryMinusInPlace();
	 return v;
      }

      if (v->getType() == NT_INT) {
	 QoreBigIntNode* i = reinterpret_cast<QoreBigIntNode*>(v);
	 i->val = -i->val;
	 return v;
      }
   }
   return new QoreUnaryMinusOperatorNode(v);
}
