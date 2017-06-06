/*
  QoreUnshiftOperatorNode.cpp

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
#include "qore/intern/qore_program_private.h"

QoreString QoreUnshiftOperatorNode::unshift_str("unshift operator expression");

AbstractQoreNode* QoreUnshiftOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   pflag &= ~PF_RETURN_VALUE_IGNORED;

   const QoreTypeInfo* leftTypeInfo = 0;
   left = left->parseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, leftTypeInfo);

   const QoreTypeInfo* rightTypeInfo = 0;
   right = right->parseInit(oflag, pflag, lvids, rightTypeInfo);

   if (left) {
      checkLValue(left, pflag);

      if (!QoreTypeInfo::parseAcceptsReturns(leftTypeInfo, NT_LIST)) {
         // only raise a parse exception if parse exceptions are enabled
         if (getProgram()->getParseExceptionSink()) {
            QoreStringNode* edesc = new QoreStringNode("the lvalue expression with the ");
            edesc->sprintf("'%s' operator is ", getTypeName());
            QoreTypeInfo::getThisType(leftTypeInfo, *edesc);
            edesc->sprintf(" therefore this operation is invalid and would throw an exception at run-time; the '%s' operator can only operate on lists", getTypeName());
            qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", edesc);
         }
      }
      else
         returnTypeInfo = listTypeInfo;
   }

   return this;
}

QoreValue QoreUnshiftOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder res(right, xsink);
   if (*xsink)
      return QoreValue();

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(left, xsink);
   if (!val)
      return QoreValue();

   // assign to a blank list if the lvalue has no value yet but is typed as a list or a softlist
   if (val.getType() == NT_NOTHING) {
      if (val.getTypeInfo() == listTypeInfo && val.assign(QoreTypeInfo::getDefaultValue(listTypeInfo)))
         return QoreValue();
      if (val.getTypeInfo() == softListTypeInfo && val.assign(QoreTypeInfo::getDefaultValue(softListTypeInfo)))
         return QoreValue();
   }

   // value is not a list, so throw exception
   if (val.getType() != NT_LIST) {
      // no need to check for PO_STRICT_ARGS; this exception was always thrown
      xsink->raiseException("UNSHIFT-ERROR", "the lvalue argument to unshift is type \"%s\"; expecting \"list\"", val.getTypeName());
      return QoreValue();
   }

   // no exception can occur here
   val.ensureUnique();

   QoreListNode* l = reinterpret_cast<QoreListNode*>(val.getValue());

   l->insert(res.getReferencedValue());

   // reference for return value
   return ref_rv ? l->refSelf() : QoreValue();
}
