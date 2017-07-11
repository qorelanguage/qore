/*
  QoreRegexExtractOperatorNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

QoreString QoreRegexExtractOperatorNode::op_str("regex extract (=~ x//) operator expression");

QoreValue QoreRegexExtractOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder lh(exp, xsink);
   if (*xsink)
      return QoreValue();

   QoreStringNodeValueHelper str(*lh);
   return regex->extractSubstrings(*str, xsink);
}

AbstractQoreNode* QoreRegexExtractOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   // turn off "reference ok" and "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   typeInfo = listOrNothingTypeInfo;

   const QoreTypeInfo *lti = 0;
   exp = exp->parseInit(oflag, pflag, lvids, lti);

   if (!QoreTypeInfo::canConvertToScalar(lti)) {
      QoreStringMaker desc("the left side of the %s is ", op_str.c_str());
      lti->doNonStringWarning(loc, desc.c_str());
   }

   // see if the left-hand arguments is a constant, then eval immediately and substitute this node with the result
   if (exp && exp->is_value()) {
      SimpleRefHolder<QoreRegexMatchOperatorNode> del(this);
      ParseExceptionSink xsink;
      ValueEvalRefHolder v(this, *xsink);
      assert(!**xsink);
      return v.getReferencedValue();
   }

   return this;
}
