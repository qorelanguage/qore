/*
 QoreLogicalGreaterThanOrEqualsOperatorNode.cpp

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

QoreString QoreLogicalGreaterThanOrEqualsOperatorNode::op_str(">= operator expression");

bool QoreLogicalGreaterThanOrEqualsOperatorNode::boolEvalImpl(ExceptionSink *xsink) const {
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

AbstractQoreNode *QoreLogicalGreaterThanOrEqualsOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   typeInfo = boolTypeInfo;

   const QoreTypeInfo *lti = 0, *rti = 0;

   left = left->parseInit(oflag, pflag, lvids, lti);
   right = right->parseInit(oflag, pflag, lvids, rti);

   // see if both arguments are constants, then eval immediately and substitute this node with the result
   if (left && left->is_value() && right && right->is_value()) {
      SimpleRefHolder<QoreLogicalGreaterThanOrEqualsOperatorNode> del(this);
      ParseExceptionSink xsink;
      AbstractQoreNode *rv = get_bool_node(QoreLogicalGreaterThanOrEqualsOperatorNode::boolEvalImpl(*xsink));
      return rv;
   }

   // check for optimizations based on type; but only if types are known on both sides, although the highest priority (float)
   // can be assigned if either side is a float
   if (lti->isType(NT_FLOAT) || rti->isType(NT_FLOAT))
      pfunc = &QoreLogicalGreaterThanOrEqualsOperatorNode::floatGreaterThanOrEquals;
   else if (lti->hasType() && rti->hasType()) {
      if (lti->isType(NT_INT)) {
	 if (rti->isType(NT_INT))
	    pfunc = &QoreLogicalGreaterThanOrEqualsOperatorNode::bigIntGreaterThanOrEquals;
      }
   }

   return this;
}

bool QoreLogicalGreaterThanOrEqualsOperatorNode::floatGreaterThanOrEquals(ExceptionSink *xsink) const {
   double l = left->floatEval(xsink);
   if (*xsink) return false;
   double r = right->floatEval(xsink);
   if (*xsink) return false;

   return l < r;
}

bool QoreLogicalGreaterThanOrEqualsOperatorNode::bigIntGreaterThanOrEquals(ExceptionSink *xsink) const {
   int64 l = left->bigIntEval(xsink);
   if (*xsink) return false;
   int64 r = right->bigIntEval(xsink);
   if (*xsink) return false;

   return l < r;
}
