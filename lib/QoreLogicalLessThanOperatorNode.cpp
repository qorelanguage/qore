/*
  QoreLogicalLessThanOperatorNode.cpp

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

QoreString QoreLogicalLessThanOperatorNode::op_str("< operator expression");
QoreString QoreLogicalGreaterThanOrEqualsOperatorNode::op_str(">= operator expression");

bool QoreLogicalLessThanOperatorNode::boolEvalImpl(ExceptionSink *xsink) const {
   if (pfunc)
      return (this->*pfunc)(xsink);

   QoreNodeEvalOptionalRefHolder lh(left, xsink);
   if (*xsink)
      return false;
   QoreNodeEvalOptionalRefHolder rh(right, xsink);
   if (*xsink)
      return false;

   const AbstractQoreNode *l = *lh, *r = *rh;

   qore_type_t lt = get_node_type(l);
   qore_type_t rt = get_node_type(r);

   if (!l)
      l = &Nothing;
   if (!r)
      r = &Nothing;

   if (lt == NT_NUMBER) {
      switch (rt) {
	 case NT_NUMBER:
	    return reinterpret_cast<const QoreNumberNode*>(l)->compare(*reinterpret_cast<const QoreNumberNode*>(r)) < 0;
	 case NT_FLOAT:
	    return reinterpret_cast<const QoreNumberNode*>(l)->compare(reinterpret_cast<const QoreFloatNode*>(r)->f) < 0;
	 case NT_INT:
	    return reinterpret_cast<const QoreNumberNode*>(l)->compare(reinterpret_cast<const QoreBigIntNode*>(r)->val) < 0;
	 default: {
	    ReferenceHolder<QoreNumberNode> rn(new QoreNumberNode(r), xsink);
	    return reinterpret_cast<const QoreNumberNode*>(l)->compare(**rn) < 0;
	 }
      }
   }

   if (rt == NT_NUMBER) {
      assert(lt != NT_NUMBER);
      switch (lt) {
	 case NT_FLOAT:
	    return reinterpret_cast<const QoreNumberNode*>(r)->compare(reinterpret_cast<const QoreFloatNode*>(l)->f) >= 0;
	 case NT_INT:
	    return reinterpret_cast<const QoreNumberNode*>(r)->compare(reinterpret_cast<const QoreBigIntNode*>(l)->val) >= 0;
	 default: {
	    ReferenceHolder<QoreNumberNode> ln(new QoreNumberNode(l), xsink);
	    return reinterpret_cast<const QoreNumberNode*>(r)->compare(**ln) >= 0;
	 }
      }
   }

   if (lt == NT_FLOAT || rt == NT_FLOAT)
      return l->getAsFloat() < r->getAsFloat();

   if (lt == NT_INT || rt == NT_INT)
      return l->getAsBigInt() < r->getAsBigInt();

   if (lt == NT_STRING || rt == NT_STRING) {
      QoreStringValueHelper ls(l);
      QoreStringValueHelper rs(r, ls->getEncoding(), xsink);
      if (*xsink)
	 return false;
      return ls->compare(*rs) < 0;
   }
 
   if (lt == NT_DATE || rt == NT_DATE) {
      DateTimeNodeValueHelper ld(l);
      DateTimeNodeValueHelper rd(r);
      return DateTime::compareDates(*ld, *rd) < 0;
   }

   return l->getAsFloat() < r->getAsFloat();
}

AbstractQoreNode *QoreLogicalLessThanOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   return parseInitIntern(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);
}

AbstractQoreNode *QoreLogicalLessThanOperatorNode::parseInitIntern(const char *name, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   // turn off "reference ok" and "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   typeInfo = boolTypeInfo;

   const QoreTypeInfo *lti = 0, *rti = 0;

   left = left->parseInit(oflag, pflag, lvids, lti);
   right = right->parseInit(oflag, pflag, lvids, rti);

   // see if both arguments are constants, then eval immediately and substitute this node with the result
   if (left && left->is_value() && right && right->is_value()) {
      SimpleRefHolder<QoreLogicalLessThanOperatorNode> del(this);
      ParseExceptionSink xsink;
      AbstractQoreNode *rv = get_bool_node(QoreLogicalLessThanOperatorNode::boolEvalImpl(*xsink));
      return rv;
   }

   // check for optimizations based on type; but only if types are known on both sides, although the highest priority (float)
   // can be assigned if either side is a float
   if (lti->isType(NT_FLOAT) || rti->isType(NT_FLOAT))
      pfunc = &QoreLogicalLessThanOperatorNode::floatLessThan;
   else if (lti->hasType() && rti->hasType()) {
      if (lti->isType(NT_INT)) {
	 if (rti->isType(NT_INT))
	    pfunc = &QoreLogicalLessThanOperatorNode::bigIntLessThan;
      }
      // FIXME: check for invalid operation here      
   }

   return this;
}

bool QoreLogicalLessThanOperatorNode::floatLessThan(ExceptionSink *xsink) const {
   double l = left->floatEval(xsink);
   if (*xsink) return false;
   double r = right->floatEval(xsink);
   if (*xsink) return false;

   return l < r;
}

bool QoreLogicalLessThanOperatorNode::bigIntLessThan(ExceptionSink *xsink) const {
   int64 l = left->bigIntEval(xsink);
   if (*xsink) return false;
   int64 r = right->bigIntEval(xsink);
   if (*xsink) return false;

   return l < r;
}
