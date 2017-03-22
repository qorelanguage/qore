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

lvalue_ref::lvalue_ref(ReferenceNode* ref, AbstractQoreNode* n_lvexp, QoreObject* n_self, const void* lvid) : RObject(ref->references), ref(ref), vexp(n_lvexp), self(n_self), pgm(getProgram()), lvalue_id(lvid) {
   //printd(5, "lvalue_ref::lvalue_ref() this: %p vexp: %p self: %p pgm: %p\n", this, vexp, self, pgm);
   if (self)
      self->tRef();
}

lvalue_ref::lvalue_ref(const lvalue_ref& old, ReferenceNode* ref) : RObject(ref->references), ref(ref), vexp(old.vexp->refSelf()), self(old.self), pgm(old.pgm), lvalue_id(old.lvalue_id) {
   //printd(5, "lvalue_ref::lvalue_ref() this: %p vexp: %p self: %p pgm: %p\n", this, vexp, self, pgm);
   if (self)
      self->tRef();
}

bool lvalue_ref::scanMembers(RSetHelper& rsh) {
   assert(rml.checkRSectionExclusive());

   return scanNode(rsh, vexp);
}

bool lvalue_ref::scanNode(RSetHelper& rsh, AbstractQoreNode* vexp) {
   printd(0, "lvalue_ref::scanNode() vexp: %p %s\n", vexp, get_type_name(vexp));
   qore_type_t ntype = vexp->getType();
   if (ntype == NT_VARREF) {
      return reinterpret_cast<VarRefNode*>(vexp)->scanMembers(rsh);
   }
   /*
   else if (ntype == NT_SELF_VARREF) {
      //printd(5, "lvalue_ref::scanMembers() self ref %p\n", vexp);
      // here we scan the whole object instead of just the key since effectively we have a link to the object
      // note that getStackObject() is guaranteed to return a value here (self varref is only valid in a method)
      QoreObject* obj = runtime_get_stack_object();
      return rsh.checkNode(obj);
   }
   */
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
      return rsh.checkNode(vexp);
   }
   /* other possibilities:
      - NT_CLASS_VARREF: does not need a scan - values are deleted when the Program is deleted
      - NT_SELF_VARREF: no strong reference is made to the object
   */

   return false;
}

bool lvalue_ref::needsScan(bool scan_now) {
   // we always perform the scan
   return true;
}

void lvalue_ref::deleteObject() {
   delete ref;
}

const char* lvalue_ref::getName() const {
   return "lvalue reference";
}

void lvalue_ref::doRef() const {
   // the mutex ensures atomicity
   AutoLocker al(rlck);
   ++references;
}

void lvalue_ref::doDeref(ExceptionSink* xsink) {
   int ref_copy;
   bool do_del = false;
   {
      robject_dereference_helper qodh(this);
      ref_copy = qodh.getRefs();
      printd(0, "lvalue_ref::doDeref() refs: %d -> %d\n", ref_copy + 1, ref_copy);

      if (!ref_copy) {
         do_del = true;
      }
      else {
         while (true) {
            {
               QoreRSectionLocker al(rml);

               if (!rset) {
                  if (ref_copy == rcount) {
                     do_del = true;
                  }
                  break;
               }
               if (!qodh.deferredScan()) {
                  int rc = rset->canDelete(ref_copy, rcount);
                  if (rc == 1) {
                     printd(0, "lvalue_ref::doDeref() this: %p found recursive reference; deleting value\n", this);
                     do_del = true;
                     break;
                  }
                  if (!rc)
                     break;
                  assert(rc == -1);
               }
            }
            if (!qodh.doScan()) {
               return;
            }
            // need to recalculate references
            RSetHelper rsh(*this);
         }
         if (do_del)
            qodh.willDelete();
      }
   }

   if (do_del) {
      // first invalidate any rset
      removeInvalidateRSet();
      // now dereference the expression which should cause the entire chain to be destroyed
      vexp->deref(xsink);
      vexp = 0;
   }

   if (!ref_copy) {
      printd(QORE_DEBUG_OBJ_REFS, "lvalue_ref::doDeref() this: %p deleting\n", this);
      delete ref;
      return;
   }
}
