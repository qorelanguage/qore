/*
  QoreUnaryMinusOperatorNode.cpp
 
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

QoreValue QoreUnaryMinusOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink *xsink) const {
   ValueEvalRefHolder v(exp, xsink);
   if (*xsink)
      return QoreValue();

   switch (v->getType()) {
      case NT_NUMBER: {
	 return v.takeReferencedNode<QoreNumberNode>()->negate();
      }

      case NT_FLOAT: {
	 return -(v->getAsFloat());
      }
	 
      case NT_DATE: {
	 return v->get<const DateTimeNode>()->unaryMinus();
      }

      case NT_INT: {
	 return -(v->getAsBigInt());
      }
   }

   return QoreValue(0ll);
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
