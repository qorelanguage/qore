/*
  QoreAssignmentOperatorNode.cpp
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2014 David Nichols
 
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

QoreString QoreAssignmentOperatorNode::op_str("assignment operator expression");

AbstractQoreNode* QoreAssignmentOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   // turn off "reference ok" and "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);
  
   left = left->parseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, ti);
   //printd(5, "QoreAssignmentOperatorNode::parseInitImpl() this: %p left: %p '%s' nt: %d ti: %p '%s'\n", this, left, get_type_name(left), get_node_type(left), ti, ti->getName());
   checkLValue(left, pflag);

   // return type info is the same as the lvalue's typeinfo
   typeInfo = ti;

   const QoreTypeInfo* r = 0;
   right = right->parseInit(oflag, pflag, lvids, r);

   // check for illegal assignment to $self
   if (oflag)
      check_self_assignment(left, oflag);

   //printd(5, "QoreAssignmentOperatorNode::parseInitImpl() this: %p left: %s ti: %p '%s', right: %s ti: %s\n", this, get_type_name(left), ti, ti->getName(), get_type_name(right), r->getName());

   if (ti->hasType() && r->hasType() && !ti->parseAccepts(r)) {
      if (getProgram()->getParseExceptionSink()) {
	 QoreStringNode *edesc = new QoreStringNode("lvalue for assignment operator (=) expects ");
	 ti->getThisType(*edesc);
	 edesc->concat(", but right-hand side is ");
	 r->getThisType(*edesc);
	 qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", edesc);
      }
   }

   // replace this node with optimized operator implementations, if possible
   if (ti == bigIntTypeInfo || ti == softBigIntTypeInfo)
      return makeSpecialization<QoreIntAssignmentOperatorNode>();

   return this;
}

AbstractQoreNode* QoreAssignmentOperatorNode::evalImpl(ExceptionSink *xsink) const {
   /* assign new value, this value gets referenced with the
      eval(xsink) call, so there's no need to reference it again
      for the variable assignment - however it does need to be
      copied/referenced for the return value
   */
   ReferenceHolder<AbstractQoreNode> new_value(right->eval(xsink), xsink);
   if (*xsink)
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;

   // assign new value
   if (v.assign(new_value.release()))
      return 0;

#if 0
   printd(5, "QoreAssignmentOperatorNode::evalImpl() *%p=%p (type=%s refs=%d)\n",
	  v, new_value, 
	  new_value ? new_value->getTypeName() : "(null)",
	  new_value ? new_value->reference_count() : 0);
#endif

   // reference return value if necessary
   return ref_rv ? v.getReferencedValue() : 0;
}

AbstractQoreNode* QoreAssignmentOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreAssignmentOperatorNode::evalImpl(xsink);
}
