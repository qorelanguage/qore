/*
  QoreSquareBracketsOperatorNode.cpp

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
#include <qore/intern/qore_program_private.h>

QoreString QoreSquareBracketsOperatorNode::op_str("[] operator expression");

AbstractQoreNode* QoreSquareBracketsOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& returnTypeInfo) {
   // turn off "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   assert(!typeInfo);
   assert(!returnTypeInfo);

   const QoreTypeInfo* lti = 0, *rti = 0;

   left = left->parseInit(oflag, pflag, lvids, lti);
   right = right->parseInit(oflag, pflag & ~(PF_FOR_ASSIGNMENT), lvids, rti);

   if (lti->hasType()) {
      // if we are trying to convert to a list
      if (pflag & PF_FOR_ASSIGNMENT) {
	 // only throw a parse exception if parse exceptions are enabled
	 if (!lti->parseAcceptsReturns(NT_LIST) && getProgram()->getParseExceptionSink()) {
	    QoreStringNode* edesc = new QoreStringNode("cannot convert lvalue defined as ");
	    lti->getThisType(*edesc);
	    edesc->sprintf(" to a list using the '[]' operator in an assignment expression");
	    qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", edesc);
	 }
      }
      else {
	 if (lti->isType(NT_STRING)) {
	    returnTypeInfo = stringOrNothingTypeInfo;
	 }
	 else if (lti->isType(NT_BINARY)) {
	    returnTypeInfo = binaryOrNothingTypeInfo;
	 }
	 else if (!listTypeInfo->parseAccepts(lti)
	     && !stringTypeInfo->parseAccepts(lti)
	     && !binaryTypeInfo->parseAccepts(lti)) {
	    QoreStringNode* edesc = new QoreStringNode("left-hand side of the expression with the '[]' operator is ");
	    lti->getThisType(*edesc);
	    edesc->concat(" and so this expression will always return NOTHING; the '[]' operator only returns a value within the legal bounds of lists, strings, and binary objects");
	    qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
	    returnTypeInfo = nothingTypeInfo;
	 }
      }
   }

   // see if the rhs is a type that can be converted to an integer, if not raise an invalid operation warning
   if (rti->hasType()
       && !bigIntTypeInfo->parseAccepts(rti)
       && !floatTypeInfo->parseAccepts(rti)
       && !numberTypeInfo->parseAccepts(rti)
       && !boolTypeInfo->parseAccepts(rti)
       && !stringTypeInfo->parseAccepts(rti)
       && !dateTypeInfo->parseAccepts(rti)) {
	    QoreStringNode* edesc = new QoreStringNode("the offset operand expression with the '[]' operator is ");
	    rti->getThisType(*edesc);
	    edesc->concat(" and so will always evaluate to zero");
	    qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
   }

   // see if both arguments are constants, then eval immediately and substitute this node with the result
   if (right && right->is_value()) {
      if (left && left->is_value()) {
         SimpleRefHolder<QoreSquareBracketsOperatorNode> del(this);
         ParseExceptionSink xsink;
         AbstractQoreNode* rv = QoreSquareBracketsOperatorNode::evalImpl(*xsink);
         return rv ? rv : &Nothing;
      }
   }

   typeInfo = returnTypeInfo;
   return this;
}

QoreValue QoreSquareBracketsOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder lh(left, xsink);
   if (*xsink)
      return QoreValue();
   ValueEvalRefHolder rh(right, xsink);
   if (*xsink)
      return QoreValue();

   return doSquareBrackets(*lh, *rh, xsink);
}

QoreValue QoreSquareBracketsOperatorNode::doSquareBrackets(QoreValue l, QoreValue r, ExceptionSink* xsink) {
   qore_type_t t = l.getType();

   int64 offset = r.getAsBigInt();
   switch (t) {
      case NT_LIST:
	 return l.get<const QoreListNode>()->get_referenced_entry(offset);
      case NT_STRING:
	 return l.get<const QoreStringNode>()->substr(offset, 1, xsink);
      case NT_BINARY: {
	 const BinaryNode* b = l.get<const BinaryNode>();
	 if (offset < 0 || (size_t)offset >= b->size())
	    return QoreValue();
	 return (int64)(((unsigned char*)b->getPtr())[offset]);
      }
   }

   return QoreValue();
}
