/*
 QoreAssignmentOperatorNode.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2011 David Nichols
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <qore/Qore.h>

#include <qore/intern/qore_program_private.h>

QoreString QoreAssignmentOperatorNode::op_str("assignment operator expression");

AbstractQoreNode *QoreAssignmentOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) { 
   // turn off "reference ok" and "return value ignored" flags
   pflag &= ~(PF_REFERENCE_OK | PF_RETURN_VALUE_IGNORED);
  
   left = left->parseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, ti);
   checkLValue(left);

   // return type info is the same as the lvalue's typeinfo
   typeInfo = ti;

   const QoreTypeInfo *r = 0;
   right = right->parseInit(oflag, pflag, lvids, r);

   // check for illegal assignment to $self
   if (oflag)
      check_self_assignment(left, oflag);

   printd(5, "QoreAssignmentOperatorNode::parseInitImpl() left=%s ti=%s\n", get_type_name(left), ti->getName());

   if (ti->hasType() && r->hasType() && !ti->parseAccepts(r)) {
      if (getProgram()->getParseExceptionSink()) {
	 QoreStringNode *edesc = new QoreStringNode("lvalue for assignment operator (=) expects ");
	 ti->getThisType(*edesc);
	 edesc->concat(", but right-hand side is ");
	 r->getThisType(*edesc);
	 qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", edesc);
      }
   }

   // replace this node with optimized operator implementations, if possible
   if (ti == bigIntTypeInfo || ti == softBigIntTypeInfo)
      return makeSpecialization<QoreIntAssignmentOperatorNode>();

   return this;
}

AbstractQoreNode *QoreAssignmentOperatorNode::evalImpl(ExceptionSink *xsink) const {
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

AbstractQoreNode *QoreAssignmentOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QoreAssignmentOperatorNode::evalImpl(xsink);
}
