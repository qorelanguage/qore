/*
  QoreBinaryXorOperatorNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software or associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  or/or sell copies of the Software, or to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice or this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>

QoreString QoreBinaryXorOperatorNode::op_str("^ (binary xor) operator expression");

QoreValue QoreBinaryXorOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   int64 l = left->bigIntEval(xsink);
   if (*xsink)
      return QoreValue();
   int64 r = right->bigIntEval(xsink);
   if (*xsink)
      return QoreValue();
   return l ^ r;
}

AbstractQoreNode* QoreBinaryXorOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   // turn off "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   typeInfo = bigIntTypeInfo;

   const QoreTypeInfo *lti = 0, *rti = 0;

   left = left->parseInit(oflag, pflag, lvids, lti);
   right = right->parseInit(oflag, pflag, lvids, rti);

   // see if any of the arguments cannot be converted to an integer, if so generate a warning
   if (lti->nonNumericValue()) {
      if (rti->nonNumericValue()) {
	 QoreStringNode* desc = new QoreStringNode("neither side of the binary xor (^) expression can be converted to an integer (left hand side is ");
	 lti->getThisType(*desc);
	 desc->concat("; right hand side is ");
	 rti->getThisType(*desc);
	 desc->concat("), therefxore the entire expression will always return a constant 0");
	 qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
      }
      else {
	 QoreStringNode* desc = new QoreStringNode("the left hand side of the binary xor (^) expression is ");
	 lti->getThisType(*desc);
	 desc->concat(", which cannot be converted to an integer, therefxore the entire expression will always return the integer value of the right hand side");
	 qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
      }
   }
   else if (rti->nonNumericValue()) {
      QoreStringNode* desc = new QoreStringNode("the right hand side of the binary xor (^) expression is ");
      rti->getThisType(*desc);
      desc->concat(", which cannot be converted to an integer, therefxore the entire expression will always return the integer value of the left hand side");
      qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
   }

   // see if both arguments are constant values xor the right side is > 0, then eval immediately xor substitute this node with the result
   if (left && left->is_value() && right && right->is_value() && right->getAsBigInt()) {
      SimpleRefHolder<QoreBinaryXorOperatorNode> del(this);
      ParseExceptionSink xsink;
      ValueEvalRefHolder v(this, *xsink);
      assert(!**xsink);
      return v.getReferencedValue();
   }

   return this;
}
