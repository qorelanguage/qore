/*
  QoreQuestionMarkOperatorNode.cpp

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

QoreString QoreQuestionMarkOperatorNode::question_mark_str("question mark (?:) operator expression");

AbstractQoreNode* QoreQuestionMarkOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& returnTypeInfo) {
   const QoreTypeInfo* leftTypeInfo = 0;
   e[0] = e[0]->parseInit(oflag, pflag, lvids, leftTypeInfo);

   if (!QoreTypeInfo::canConvertToScalar(leftTypeInfo) && parse_check_parse_option(PO_STRICT_BOOLEAN_EVAL))
      leftTypeInfo->doNonBooleanWarning(loc, "the initial expression with the '?:' operator is ");

   leftTypeInfo = 0;
   e[1] = e[1]->parseInit(oflag, pflag, lvids, leftTypeInfo);

   const QoreTypeInfo* rightTypeInfo = 0;
   e[2] = e[2]->parseInit(oflag, pflag, lvids, rightTypeInfo);

   // see if all arguments are constant values, then eval immediately and substitute this node with the result
   if (e[0] && e[0]->is_value() && e[1] && e[1]->is_value() && e[2] && e[2]->is_value()) {
      SimpleRefHolder<QoreQuestionMarkOperatorNode> del(this);
      ParseExceptionSink xsink;
      ValueEvalRefHolder v(this, *xsink);
      assert(!**xsink);
      return v.getReferencedValue();
   }

   typeInfo = returnTypeInfo = QoreTypeInfo::isOutputIdentical(leftTypeInfo, rightTypeInfo) ? leftTypeInfo : 0;

   return this;
}

QoreValue QoreQuestionMarkOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder b(e[0], xsink);
   if (*xsink)
      return QoreValue();

   AbstractQoreNode* exp = b->getAsBool() ? e[1] : e[2];

   ValueEvalRefHolder rv(exp, xsink);
   if (*xsink)
      return QoreValue();

   return rv.takeValue(needs_deref);
}
