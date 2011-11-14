/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreBinaryLValueOperatorNode.h
 
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

#ifndef _QORE_QOREBINARYLVALUEOPERATORNODE_H

#define _QORE_QOREBINARYLVALUEOPERATORNODE_H

class QoreBinaryLValueOperatorNode : public LValueOperatorNode {
protected:
   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return ti;
   }

public:
   AbstractQoreNode *left, *right; // parts of the expression
   const QoreTypeInfo *ti;         // typeinfo of lhs

   DLLLOCAL QoreBinaryLValueOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : left(n_left), right(n_right), ti(0) {
   }

   DLLLOCAL virtual ~QoreBinaryLValueOperatorNode() {
      if (left)
         left->deref(0);
      if (right)
         right->deref(0);
   }

   template<typename T>
   DLLLOCAL QoreBinaryLValueOperatorNode *makeSpecialization() {
      // only generate the specialization if the lvalue is not a variable reference to a global var
      if (get_node_type(left) != NT_VARREF || reinterpret_cast<VarRefNode *>(left)->isGlobalVar())
         return this;

      AbstractQoreNode *l = left, *r = right;
      left = right = 0;
      SimpleRefHolder<QoreBinaryLValueOperatorNode> del(this);
      return new T(l, r);
   }

   DLLLOCAL AbstractQoreNode *swapRight(AbstractQoreNode *n_right) {
      AbstractQoreNode *old_r = right;
      right = n_right;
      return old_r;
   }

   DLLLOCAL AbstractQoreNode *getLeft() {
      return left;
   }

   DLLLOCAL AbstractQoreNode *getRight() {
      return right;
   }

   DLLLOCAL void parseInitIntLValue(const char *name, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      // turn off "reference ok" and "return value ignored" flags
      pflag &= ~(PF_REFERENCE_OK | PF_RETURN_VALUE_IGNORED);

      typeInfo = bigIntTypeInfo;

      const QoreTypeInfo *mti = 0;
      left = left->parseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, mti);
      
      // make sure left side can take an integer value
      check_lvalue_int(mti, name);

      mti = 0;
      // FIXME: check for invalid operation - type cannot be converted to integer
      right = right->parseInit(oflag, pflag, lvids, mti);
   }
};

#endif
