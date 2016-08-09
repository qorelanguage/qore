/*
  QoreLogicalComparisonOperatorNode.cpp

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

#include <cmath>

QoreString QoreLogicalComparisonOperatorNode::logical_comparison_str("logical comparison (<=>) operator expression");

QoreValue QoreLogicalComparisonOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder l(left, xsink);
   if (*xsink)
      return QoreValue();

   ValueEvalRefHolder r(right, xsink);
   if (*xsink)
      return QoreValue();

   return doComparison(*l, *r, xsink);
}

AbstractQoreNode *QoreLogicalComparisonOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   pflag &= ~PF_RETURN_VALUE_IGNORED;
   typeInfo = bigIntTypeInfo;

   const QoreTypeInfo *lti = 0, *rti = 0;

   left = left->parseInit(oflag, pflag, lvids, lti);
   right = right->parseInit(oflag, pflag, lvids, rti);

   // see if both arguments are constant values, then eval immediately and substitute this node with the result
   if (left && left->is_value() && right && right->is_value()) {
      SimpleRefHolder<QoreLogicalComparisonOperatorNode> del(this);
      ParseExceptionSink xsink;
      ValueEvalRefHolder v(this, *xsink);
      return v.getReferencedValue();
   }

   return this;
}

int QoreLogicalComparisonOperatorNode::doComparison(const QoreValue left, const QoreValue right, ExceptionSink* xsink) {
   qore_type_t lt = left.getType();
   qore_type_t rt = right.getType();

   if (lt == NT_STRING) {
      const QoreStringNode* l = left.get<const QoreStringNode>();
      if (rt == NT_STRING) {
	 const QoreStringNode* r = right.get<const QoreStringNode>();
	 if (l->getEncoding() != r->getEncoding()) {
	    QoreStringValueHelper rstr(r, l->getEncoding(), xsink);
	    if (*xsink)
	       return 0;
	    return l->compare(*rstr);
	 }
         return l->compare(r);
      }
      QoreStringValueHelper r(right, l->getEncoding(), xsink);
      if (*xsink)
         return 0;
      return l->compare(*r);
   }
   else if (rt == NT_STRING) {
      const QoreStringNode* r = right.get<const QoreStringNode>();
      QoreStringValueHelper l(left, r->getEncoding(), xsink);
      if (*xsink)
         return 0;
      return l->compare(r);
   }

   if (lt == NT_NUMBER) {
      const QoreNumberNode* l = left.get<const QoreNumberNode>();
      if (l->nan()) {
	 xsink->raiseException("NAN-COMPARE-ERROR", "NaN in arbitrary-precision value on left hand side of logical comparison operator");
	 return 0;
      }
      switch (rt) {
         case NT_NUMBER: {
	    const QoreNumberNode* r = right.get<const QoreNumberNode>();
	    if (r->nan()) {
	       xsink->raiseException("NAN-COMPARE-ERROR", "NaN in arbitrary-precision value on right hand side of logical comparison operator");
	       return 0;
	    }
	    if (l->lessThan(*r))
	       return -1;

	    return l->equals(*r) ? 0 : 1;
	 }
         case NT_FLOAT: {
	    float f = right.getAsFloat();
	    if (std::isnan(f)) {
	       xsink->raiseException("NAN-COMPARE-ERROR", "NaN in floating-point value on right hand side of logical comparison operator");
	       return 0;
	    }

	    if (l->lessThan(f))
	       return -1;

	    return l->equals(f) ? 0 : 1;
	 }
         case NT_INT:
         case NT_BOOLEAN: {
	    int64 r = right.getAsBigInt();

	    if (l->lessThan(r))
	       return -1;

	    return l->equals(r) ? 0 : 1;
	 }
         default: {
            ReferenceHolder<QoreNumberNode> rn(new QoreNumberNode(right.getInternalNode()), xsink);

	    if (l->lessThan(**rn))
	       return -1;

	    return l->equals(**rn) ? 0 : 1;
         }
      }
   }

   if (rt == NT_NUMBER) {
      assert(lt != NT_NUMBER);

      const QoreNumberNode* r = right.get<const QoreNumberNode>();
      if (r->nan()) {
	 xsink->raiseException("NAN-COMPARE-ERROR", "NaN in arbitrary-precision value on right hand side of logical comparison operator");
	 return 0;
      }

      switch (lt) {
         case NT_FLOAT: {
	    float l = left.getAsFloat();
	    if (std::isnan(l)) {
	       xsink->raiseException("NAN-COMPARE-ERROR", "NaN in floating-point value on left hand side of logical comparison operator");
	       return 0;
	    }

	    if (r->greaterThan(l))
	       return -1;

	    return r->equals(l) ? 0 : 1;
	 }
         case NT_INT:
         case NT_BOOLEAN: {
	    int64 l = left.getAsBigInt();

	    if (r->greaterThan(l))
	       return -1;

	    return r->equals(l) ? 0 : 1;
	 }
         default: {
            ReferenceHolder<QoreNumberNode> ln(new QoreNumberNode(left.getInternalNode()), xsink);

	    if (ln->lessThan(*r))
	       return -1;

	    return ln->equals(*r) ? 0 : 1;
         }
      }
   }

   if (lt == NT_FLOAT || rt == NT_FLOAT) {
      float l = left.getAsFloat();
      if (std::isnan(l)) {
	 xsink->raiseException("NAN-COMPARE-ERROR", "NaN in floating-point value on left hand side of logical comparison operator");
	 return 0;
      }

      float r = right.getAsFloat();
      if (std::isnan(r)) {
	 xsink->raiseException("NAN-COMPARE-ERROR", "NaN in floating-point value on right hand side of logical comparison operator");
	 return 0;
      }

      if (l < r)
	 return -1;
      return l == r ? 0 : 1;
   }

   if (lt == NT_INT || rt == NT_INT) {
      int64 l = left.getAsBigInt();
      int64 r = right.getAsBigInt();

      if (l < r)
	 return -1;
      return l == r ? 0 : 1;
   }

   DateTimeValueHelper l(left);
   DateTimeValueHelper r(right);

   return (int)DateTime::compareDates(*l, *r);
}
