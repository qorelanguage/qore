/*
  QoreSquareBracketsOperatorNode.cpp

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
#include "qore/intern/qore_program_private.h"

QoreString QoreSquareBracketsOperatorNode::op_str("[] operator expression");

AbstractQoreNode* QoreSquareBracketsOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& returnTypeInfo) {
   // turn off "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   assert(!typeInfo);
   assert(!returnTypeInfo);

   const QoreTypeInfo *lti = 0, *rti = 0;

   left = left->parseInit(oflag, pflag, lvids, lti);
   right = right->parseInit(oflag, pflag & ~(PF_FOR_ASSIGNMENT), lvids, rti);

   if (QoreTypeInfo::hasType(lti)) {
      // if we are trying to convert to a list
      if (pflag & PF_FOR_ASSIGNMENT) {
         // only throw a parse exception if parse exceptions are enabled
         if (!QoreTypeInfo::parseAcceptsReturns(lti, NT_LIST) && getProgram()->getParseExceptionSink()) {
            QoreStringNode* edesc = new QoreStringNode("cannot convert lvalue defined as ");
            QoreTypeInfo::getThisType(lti, *edesc);
            edesc->sprintf(" to a list using the '[]' operator in an assignment expression");
            qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", edesc);
         }
      }
      else {
         if (QoreTypeInfo::isType(lti, NT_STRING)) {
            returnTypeInfo = stringOrNothingTypeInfo;
         }
         else if (QoreTypeInfo::isType(lti, NT_BINARY)) {
             if (QoreTypeInfo::hasType(rti) && QoreTypeInfo::isType(rti, NT_LIST))
                 returnTypeInfo = binaryOrNothingTypeInfo;
             else
                 returnTypeInfo = bigIntOrNothingTypeInfo;
         }
         else if (!QoreTypeInfo::parseAccepts(listTypeInfo, lti)
            && !QoreTypeInfo::parseAccepts(stringTypeInfo, lti)
            && !QoreTypeInfo::parseAccepts(binaryTypeInfo, lti)) {
            QoreStringNode* edesc = new QoreStringNode("left-hand side of the expression with the '[]' operator is ");
            QoreTypeInfo::getThisType(lti, *edesc);
            edesc->concat(" and so this expression will always return NOTHING; the '[]' operator only returns a value within the legal bounds of lists, strings, and binary objects");
            qore_program_private::makeParseWarning(getProgram(), loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
            returnTypeInfo = nothingTypeInfo;
         }
      }
      if (!returnTypeInfo) {
         const QoreTypeInfo* ti = QoreTypeInfo::getUniqueReturnComplexList(lti);
         if (ti) {
            // issue #2115 when dereferencing a hash, we could get also NOTHING when the requested key value is not present
            returnTypeInfo = get_or_nothing_type_check(ti);
         }
      }
   }

   // see if the rhs is a type that can be converted to an integer, if not raise an invalid operation warning
/*
   if (QoreTypeInfo::hasType(rti)
       && !QoreTypeInfo::parseAccepts(bigIntTypeInfo, rti)
       && !QoreTypeInfo::parseAccepts(floatTypeInfo, rti)
       && !QoreTypeInfo::parseAccepts(numberTypeInfo, rti)
       && !QoreTypeInfo::parseAccepts(boolTypeInfo, rti)
       && !QoreTypeInfo::parseAccepts(stringTypeInfo, rti)
       && !QoreTypeInfo::parseAccepts(dateTypeInfo, rti)
       && !QoreTypeInfo::parseAccepts(listTypeInfo, rti)) {
         */
   if (!QoreTypeInfo::canConvertToScalar(rti)
       && !QoreTypeInfo::parseAccepts(listTypeInfo, rti))
   {
	    QoreStringNode* edesc = new QoreStringNode("the offset operand expression with the '[]' operator is ");
	    QoreTypeInfo::getThisType(rti, *edesc);
	    edesc->concat(" and so will always evaluate to zero");
	    qore_program_private::makeParseWarning(getProgram(), loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
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
    qore_type_t left_type = l.getType();
    qore_type_t right_type = r.getType();

    if (right_type == NT_LIST) {
        ConstListIterator it(r.get<const QoreListNode>());
        switch (left_type) {
            case NT_LIST: {
                QoreListNode* ret = new QoreListNode();
                while (it.next()) {
                    QoreValue entry = doSquareBrackets(l, it.getValue(), xsink);
                    if (!entry.isNothing())
                        ret->push(entry.getReferencedValue());
                }
                return ret;
            }
            case NT_STRING: {
                QoreStringNode* ret = new QoreStringNode();
                while (it.next()) {
                    QoreValue entry = doSquareBrackets(l, it.getValue(), xsink);
                    if (!entry.isNothing())
                        ret->concat(entry.get<QoreStringNode>());
                }
                return ret;
            }
            case NT_BINARY: {
                qore_size_t size = r.get<const QoreListNode>()->size();
                qore_size_t i = 0;
                void* ptr = malloc(size);
                while (it.next()) {
                    QoreValue entry = doSquareBrackets(l, it.getValue(), xsink);
                    if (!entry.isNothing()) {
                        int64 value = entry.getAsBigInt();
                        memcpy((char*)ptr + i++, &value, 1);
                    }
                }
                return new BinaryNode(ptr, i);
            }
            default:
                return QoreValue();
        }
    }
    else {
        int64 offset = r.getAsBigInt();
        switch (left_type) {
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
    }

    return QoreValue();
}
