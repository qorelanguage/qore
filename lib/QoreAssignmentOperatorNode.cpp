/*
  QoreAssignmentOperatorNode.cpp

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

QoreString QoreAssignmentOperatorNode::op_str("assignment operator expression");

AbstractQoreNode* QoreAssignmentOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   // turn off "reference ok" and "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   left = left->parseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, ti);
   //printd(5, "QoreAssignmentOperatorNode::parseInitImpl() this: %p left: %p '%s' nt: %d ti: %p '%s'\n", this, left, get_type_name(left), get_node_type(left), ti, QoreTypeInfo::getName(ti));
   checkLValue(left, pflag);

   // return type info is the same as the lvalue's typeinfo
   typeInfo = ti;

   // if "broken-int-assignments" is set, then set flag if applicable
   if ((ti == bigIntTypeInfo || ti == softBigIntTypeInfo)
       && (getProgram()->getParseOptions64() & PO_BROKEN_INT_ASSIGNMENTS))
      broken_int = true;

   const QoreTypeInfo* r = 0;
   right = right->parseInit(oflag, pflag, lvids, r);

   // check for illegal assignment to $self
   if (oflag)
      check_self_assignment(loc, left, oflag);

   //printd(5, "QoreAssignmentOperatorNode::parseInitImpl() this: %p left: %s ti: %p '%s', right: %s ti: %s\n", this, get_type_name(left), ti, QoreTypeInfo::getName(ti), get_type_name(right), QoreTypeInfo::getName(r));

   if (left->getType() == NT_VARREF && right->getType() == NT_VARREF
       && !strcmp(static_cast<VarRefNode*>(left)->getName(), static_cast<VarRefNode*>(right)->getName()))
      qore_program_private::makeParseException(getProgram(), loc, "PARSE-EXCEPTION", new QoreStringNodeMaker("illegal assignment of variable \"%s\" to itself", static_cast<VarRefNode*>(left)->getName()));

   if (QoreTypeInfo::hasType(ti) && QoreTypeInfo::hasType(r) && getProgram()->getParseExceptionSink()) {
      if (!QoreTypeInfo::parseAccepts(ti, r)) {
         QoreStringNode* edesc = new QoreStringNode("lvalue for assignment operator (=) expects ");
         QoreTypeInfo::getThisType(ti, *edesc);
         edesc->concat(", but right-hand side is ");
         QoreTypeInfo::getThisType(r, *edesc);
         qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", edesc);
      }
      /*
      else {
         // verify reference assignments at parse time: only catch the initial assignment
         if ((ti == referenceTypeInfo || (ti == referenceOrNothingTypeInfo && !QoreTypeInfo::parseAcceptsReturns(r, NT_NOTHING)))
             && get_node_type(left) == NT_VARREF
             && get_node_type(right) != NT_PARSEREFERENCE && get_node_type(right) != NT_REFERENCE
             && dynamic_cast<VarRefDeclNode*>(left)) {
            QoreStringNode* edesc = new QoreStringNode("lvalue for assignment operator (=) in the initial assignment expects ");
            QoreTypeInfo::getThisType(ti, *edesc);
            edesc->concat(", but right-hand side is ");
            QoreTypeInfo::getThisType(r, *edesc);
            qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", edesc);
         }
      }
      */
   }

   return this;
}

QoreValue QoreAssignmentOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   /* assign new value, this value gets referenced with the
      eval(xsink) call, so there's no need to reference it again
      for the variable assignment - however it does need to be
      copied/referenced for the return value
   */
   ValueEvalRefHolder new_value(right, xsink);
   if (*xsink)
      return QoreValue();

   if (broken_int) {
      // convert the value to an int unconditionally
      new_value.setValue(new_value->getAsBigInt());
      if (*xsink)
         return QoreValue();
   }
   else {
      // we have to ensure that the value is referenced before the assignment in case the lvalue
      // is the same value, so it can be copied in the LValueHelper constructor
      new_value.ensureReferencedValue();
   }

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return QoreValue();

   assert(!*xsink);

   // assign new value
   if (v.assign(new_value.takeReferencedValue()))
      return QoreValue();

   // reference return value if necessary
   return ref_rv ? v.getReferencedValue() : QoreValue();
}
