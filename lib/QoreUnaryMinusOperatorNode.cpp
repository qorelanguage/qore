/*
  QoreUnaryMinusOperatorNode.cpp

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
#include "qore/intern/qore_number_private.h"
#include "qore/intern/qore_program_private.h"

QoreString QoreUnaryMinusOperatorNode::unaryminus_str("unary minus (-) operator expression");

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
         needs_deref = true;
         return static_cast<QoreNumberNode *>(v->v.n)->negate();
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
   assert(!typeInfo);

   if (exp) {
      const QoreTypeInfo* eti = 0;
      exp = exp->parseInit(oflag, pflag, lvids, eti);

      if (eti->hasType()) {
	 int tcnt = 0;
	 if (bigIntTypeInfo->parseAccepts(eti)) {
	    typeInfo = bigIntTypeInfo;
	    ++tcnt;
	 }

	 if (floatTypeInfo->parseAccepts(eti)) {
	    typeInfo = floatTypeInfo;
	    ++tcnt;
	 }

	 if (numberTypeInfo->parseAccepts(eti)) {
	    typeInfo = numberTypeInfo;
	    ++tcnt;
	 }

	 if (dateTypeInfo->parseAccepts(eti)) {
	    typeInfo = dateTypeInfo;
	    ++tcnt;
	 }

	 // if multiple types match, then set to no type (FIXME: can't currently handle multiple possible types)
	 if (tcnt > 0)
	    typeInfo = 0;
	 else if (!tcnt) {
	    typeInfo = bigIntTypeInfo;
	    QoreStringNode* edesc = new QoreStringNode("the expression with the unary minus '-' operator is ");
            eti->getThisType(*edesc);
            edesc->concat(" and so this expression will always return 0; the unary minus '-' operator only returns a value with integers, floats, numbers, and relative date/time values");
            qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
	 }
      }
   }

   // see if the argument is a constant value, then eval immediately and substitute this node with the result
   if (exp && exp->is_value()) {
      SimpleRefHolder<QoreUnaryMinusOperatorNode> del(this);
      ParseExceptionSink xsink;
      ValueEvalRefHolder v(this, *xsink);
      assert(!**xsink);
      return v.getReferencedValue();
   }

   returnTypeInfo = typeInfo;
   return this;
}
