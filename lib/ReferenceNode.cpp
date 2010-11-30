/*
  ReferenceNode.h
  
  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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

ReferenceNode::ReferenceNode(AbstractQoreNode *exp) : SimpleValueQoreNode(NT_REFERENCE), lvexp(exp) {
}

ReferenceNode::~ReferenceNode() {
   if (lvexp)
      lvexp->deref(0);
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int ReferenceNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("reference expression (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *ReferenceNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

AbstractQoreNode *ReferenceNode::realCopy() const {
   assert(false);
   return 0;
}

// the type passed must always be equal to the current type
bool ReferenceNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   assert(false);
   return false;
}

bool ReferenceNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   assert(false);
   return false;
}

// returns the type name as a c string
const char *ReferenceNode::getTypeName() const {
   return "reference to lvalue";
}

static inline qore_var_t getBaseLVType(AbstractQoreNode *n) {
   while (true) {
      qore_type_t ntype = n->getType();
      if (ntype == NT_SELF_VARREF)
	 return VT_OBJECT;
      if (ntype == NT_VARREF)
	 return reinterpret_cast<VarRefNode *>(n)->getType();
      assert(ntype == NT_TREE);
      // must be a tree
      n = reinterpret_cast<QoreTreeNode *>(n)->left;
   }
}

AbstractQoreNode *ReferenceNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   // otherwise throw a parse exception if an illegal reference is used
   if (!(pflag & PF_REFERENCE_OK)) {	 
      parse_error("the reference operator can only be used in argument lists and in foreach statements");
      return this;
   }
   if (lvexp) {
      const QoreTypeInfo *argTypeInfo;
      lvexp = lvexp->parseInit(oflag, pflag & ~PF_REFERENCE_OK, lvids, argTypeInfo);

      if (lvexp && check_lvalue(lvexp))
	 parse_error("the reference operator was expecting an lvalue, got '%s' instead", lvexp->getTypeName());
   }

   // if a background expression is being parsed, then check that no references to local variables
   // or object members are being used
   if (pflag & PF_BACKGROUND) {
      qore_var_t vtype = getBaseLVType(lvexp);
      
      if (vtype == VT_LOCAL)
	 parse_error("the reference operator cannot be used with local variables in a background expression");
   }
   return this;
}
