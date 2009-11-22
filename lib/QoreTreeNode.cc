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

QoreTreeNode::QoreTreeNode(AbstractQoreNode *l, Operator *o, AbstractQoreNode *r) : ParseNode(NT_TREE) {
   left = l;
   op = o;
   right = r;
   ref_rv = true;
}

QoreTreeNode::~QoreTreeNode() {
   if (left)
      left->deref(0);
   if (right)
      right->deref(0);
}

void QoreTreeNode::ignoreReturnValue() {
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
   str.sprintf("tree (left=%s (0x%08p) op=%s right=%s (%0x08p))", left ? left->getTypeName() : "NOTHING", 
	       op->getName(), right ? right->getTypeName() : "NOTHING");

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
AbstractQoreNode *QoreTreeNode::evalImpl(ExceptionSink *xsink) const {
   return op->eval(left, right, ref_rv, xsink);
}

AbstractQoreNode *QoreTreeNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return op->eval(left, right, ref_rv, xsink);
}

int64 QoreTreeNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   return op->bigint_eval(left, right, xsink);
}

int QoreTreeNode::integerEvalImpl(ExceptionSink *xsink) const {
   return op->bigint_eval(left, right, xsink);
}

bool QoreTreeNode::boolEvalImpl(ExceptionSink *xsink) const {
   return op->bool_eval(left, right, xsink);
}

double QoreTreeNode::floatEvalImpl(ExceptionSink *xsink) const {
   return op->float_eval(left, right, xsink);
}

// checks for illegal $self assignments in an object context
static inline void checkSelf(AbstractQoreNode *n, LocalVar *selfid) {
   // if it's a variable reference
   qore_type_t ntype = n->getType();
   if (ntype == NT_VARREF) {
      VarRefNode *v = reinterpret_cast<VarRefNode *>(n);
      if (v->getType() == VT_LOCAL && v->ref.id == selfid)
	 parse_error("illegal assignment to $self in an object context");
      return;
   }
   
   if (ntype != NT_TREE)
      return;

   QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(n);

   // otherwise it's a tree: go to root expression 
   while (tree->left->getType() == NT_TREE) {
      n = tree->left;
      tree = reinterpret_cast<QoreTreeNode *>(n);
   }

   if (tree->left->getType() != NT_VARREF)
      return;

   VarRefNode *v = reinterpret_cast<VarRefNode *>(tree->left);

   // left must be variable reference, check if the tree is
   // a list reference; if so, it's invalid
   if (v->getType() == VT_LOCAL && v->ref.id == selfid  && tree->op == OP_LIST_REF)
      parse_error("illegal conversion of $self to a list");
}

AbstractQoreNode *QoreTreeNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   // set "parsing background" flag if the background operator is being parsed
   if (op == OP_BACKGROUND)
      pflag |= PF_BACKGROUND;

   // turn off "reference ok" flag
   pflag &= ~PF_REFERENCE_OK;

   const QoreTypeInfo *leftTypeInfo = 0;
   // process left branch of tree
   if (left)
      left = left->parseInit(oflag, pflag, lvids, leftTypeInfo);

   const QoreTypeInfo *rightTypeInfo = 0;
   // process right branch if it exists
   if (right)
      right = right->parseInit(oflag, pflag, lvids, rightTypeInfo);

   // check for illegal changes to local variables in background expressions
   if (pflag & PF_BACKGROUND && op->needsLValue()) {
      if (left && left->getType() == NT_VARREF && reinterpret_cast<VarRefNode *>(left)->getType() == VT_LOCAL)
	 parse_error("illegal local variable modification in background expression");
   }

   // check argument types for operator   
   op->parseInit(leftTypeInfo, rightTypeInfo);

   // throw a parse exception if an assignment is attempted on $self
   if (op == OP_ASSIGNMENT && oflag)
      checkSelf(left, oflag);
   
   return this;
}
