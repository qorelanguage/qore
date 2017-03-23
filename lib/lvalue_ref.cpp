/* -*- indent-tabs-mode: nil -*- */
/*
  lvalue_ref.cpp

  POSIX thread library for Qore

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

lvalue_ref::lvalue_ref(AbstractQoreNode* n_lvexp, QoreObject* n_self, const void* lvid) : vexp(n_lvexp), self(n_self), pgm(getProgram()), lvalue_id(lvid) {
   //printd(5, "lvalue_ref::lvalue_ref() this: %p vexp: %p self: %p pgm: %p\n", this, vexp, self, pgm);
   if (self)
      self->tRef();
}

lvalue_ref::lvalue_ref(const lvalue_ref& old) : vexp(old.vexp->refSelf()), self(old.self), pgm(old.pgm), lvalue_id(old.lvalue_id) {
   //printd(5, "lvalue_ref::lvalue_ref() this: %p vexp: %p self: %p pgm: %p\n", this, vexp, self, pgm);
   if (self)
      self->tRef();
}

bool lvalue_ref::scanReference(RSetHelper& rsh) {
   return scanNode(rsh, vexp);
}

bool lvalue_ref::scanNode(RSetHelper& rsh, AbstractQoreNode* vexp) {
   //printd(5, "lvalue_ref::scanNode() vexp: %p %s\n", vexp, get_type_name(vexp));
   qore_type_t ntype = vexp->getType();
   if (ntype == NT_VARREF) {
      return reinterpret_cast<VarRefNode*>(vexp)->scanMembers(rsh);
   }
   else if (ntype == NT_OPERATOR) {
      QoreSquareBracketsOperatorNode* op = dynamic_cast<QoreSquareBracketsOperatorNode*>(vexp);
      assert(op);
      assert(op->getRight()->getType() == NT_INT);
      // we scan the whole list here because we have a reference to the list
      return scanNode(rsh, op->getLeft());
   }
   else if (ntype == NT_TREE) {
      // it must be a tree
      QoreTreeNode* tree = reinterpret_cast<QoreTreeNode*>(vexp);
      assert(tree->getOp() == OP_OBJECT_REF);
      // we scan the whole hash here because we have a reference to the hash
      return scanNode(rsh, tree->left);
   }
   else if (ntype == NT_REFERENCE) {
      return scanNode(rsh, reinterpret_cast<ReferenceNode*>(vexp)->priv->vexp);
   }
   /* other possibilities:
      - NT_CLASS_VARREF: does not need a scan - values are deleted when the Program is deleted
      - NT_SELF_VARREF: no strong reference is made to the object
   */

   return false;
}

bool lvalue_ref::needsScan() {
   // we always perform the scan
   return true;
}
