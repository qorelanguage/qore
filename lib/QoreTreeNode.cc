/*
 Tree.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
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

QoreTreeNode::QoreTreeNode(AbstractQoreNode *l, class Operator *o, AbstractQoreNode *r) : ParseNode(NT_TREE)
{
   left = l;
   op = o;
   right = r;
   ref_rv = true;
}

QoreTreeNode::~QoreTreeNode()
{
   if (left)
      left->deref(0);
   if (right)
      right->deref(0);
}

void QoreTreeNode::ignoreReturnValue()
{
   // OPTIMIZATION: change post incremement to pre increment for top-level expressions to avoid extra SMP cache invalidations
   if (op == OP_POST_INCREMENT)
      op = OP_PRE_INCREMENT;
   else if (op == OP_POST_DECREMENT)
      op = OP_PRE_DECREMENT;
      
   ref_rv = false;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
// FIXME: no deep effect - or is this ever needed?
int QoreTreeNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("tree (left=%s (%p) op=%s right=%s (%p))", get_type_name(left), left, op->getName(), get_type_name(right), right);

   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreTreeNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *QoreTreeNode::getTypeName() const {
   return "expression tree";
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *QoreTreeNode::evalImpl(ExceptionSink *xsink) const
{
   return op->eval(left, right, ref_rv, xsink);
}

AbstractQoreNode *QoreTreeNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   needs_deref = true;
   return op->eval(left, right, ref_rv, xsink);
}

int64 QoreTreeNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   return op->bigint_eval(left, right, xsink);
}

int QoreTreeNode::integerEvalImpl(ExceptionSink *xsink) const
{
   return op->bigint_eval(left, right, xsink);
}

bool QoreTreeNode::boolEvalImpl(ExceptionSink *xsink) const
{
   return op->bool_eval(left, right, xsink);
}

double QoreTreeNode::floatEvalImpl(ExceptionSink *xsink) const
{
   return op->float_eval(left, right, xsink);
}
