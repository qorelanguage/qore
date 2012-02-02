/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreOperatorNode.h

 Qore Programming Language

 Copyright 2003 - 2012 David Nichols

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

#ifndef _QORE_QOREOPERATORNODE_H
#define _QORE_QOREOPERATORNODE_H

class QoreOperatorNode : public ParseNode {
protected:
   bool ref_rv;

   DLLLOCAL virtual ~QoreOperatorNode() {}

   DLLLOCAL virtual void ignoreReturnValueImpl() {}

public:
   DLLLOCAL QoreOperatorNode(bool n_ref_rv = true) : ParseNode(NT_OPERATOR), ref_rv(n_ref_rv) {
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const = 0;

   DLLLOCAL void ignoreReturnValue() {
      ref_rv = false;
      ignoreReturnValueImpl();
   }
   
   DLLLOCAL virtual bool hasEffect() const = 0;

   DLLLOCAL virtual QoreOperatorNode *copyBackground(ExceptionSink *xsink) const {
      return const_cast<QoreOperatorNode *>(this);
   }
};

template <class T>
class QoreSingleExpressionOperatorNode : public T {
protected:
   AbstractQoreNode *exp;

   DLLLOCAL ~QoreSingleExpressionOperatorNode() {
      if (exp)
         exp->deref(0);
   }

public:
   DLLLOCAL QoreSingleExpressionOperatorNode(AbstractQoreNode *n_exp) : exp(n_exp) {
   }
   DLLLOCAL AbstractQoreNode *getExp() {
      return exp;
   }
   DLLLOCAL const AbstractQoreNode *getExp() const {
      return exp;
   }

   template <class O>
   DLLLOCAL QoreSingleExpressionOperatorNode *makeSpecialization() {
      // only generate the specialization if the lvalue is not a variable reference to a global var
      if (get_node_type(exp) != NT_VARREF || reinterpret_cast<VarRefNode *>(exp)->isGlobalVar())
         return this;

      AbstractQoreNode *e = exp;
      exp = 0;
      SimpleRefHolder<QoreSingleExpressionOperatorNode> del(this);
      O* rv = new O(e);
      if (!this->ref_rv)
         rv->ignoreReturnValue();
      return rv;
   }
};

template <class T = QoreOperatorNode>
class QoreBinaryOperatorNode : public T {
protected:
   AbstractQoreNode *left, *right;

public:
   DLLLOCAL QoreBinaryOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : left(n_left), right(n_right) {
   }

   DLLLOCAL ~QoreBinaryOperatorNode() {
      if (left)
         left->deref(0);
      if (right)
         right->deref(0);
   }

   template<typename U>
   DLLLOCAL QoreBinaryOperatorNode *makeSpecialization() {
      // only generate the specialization if the lvalue is not a variable reference to a global var
      if (get_node_type(left) != NT_VARREF || reinterpret_cast<VarRefNode *>(left)->isGlobalVar())
         return this;

      AbstractQoreNode *l = left, *r = right;
      left = right = 0;
      SimpleRefHolder<QoreBinaryOperatorNode> del(this);
      U* rv = new U(l, r);
      if (!this->ref_rv)
         rv->ignoreReturnValue();
      return rv;
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
};

class QoreBoolBinaryOperatorNode : public QoreBinaryOperatorNode<> {
public:
   DLLLOCAL QoreBoolBinaryOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreBinaryOperatorNode<>(n_left, n_right) {
   }

   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return boolTypeInfo;
   }

   DLLLOCAL virtual bool hasEffect() const {
      return false;
   }
};

#define OP_COMMON protected:\
   DLLLOCAL static QoreString op_str;\
public:\
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const {del = false;return &op_str;}\
   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {str.concat(&op_str);return 0;}\
   DLLLOCAL virtual const char *getTypeName() const {return op_str.getBuffer();}

class LValueOperatorNode : public QoreOperatorNode {
public:
   DLLLOCAL virtual bool hasEffect() const {
      return true;
   }

   DLLLOCAL void checkLValue(const AbstractQoreNode *exp) {
      if (exp && check_lvalue(exp))
         parse_error("expecing lvalue for %s, got '%s' instead", getTypeName(), exp->getTypeName());
   }
};

// include operator headers
#include <qore/intern/QoreDeleteOperatorNode.h>
#include <qore/intern/QoreRemoveOperatorNode.h>
#include <qore/intern/QoreSpliceOperatorNode.h>
#include <qore/intern/QoreExtractOperatorNode.h>
#include <qore/intern/QoreCastOperatorNode.h>
#include <qore/intern/QoreUnaryMinusOperatorNode.h>
#include <qore/intern/QoreDotEvalOperatorNode.h>
#include <qore/intern/QoreLogicalEqualsOperatorNode.h>
#include <qore/intern/QoreLogicalNotEqualsOperatorNode.h>
#include <qore/intern/QoreBinaryLValueOperatorNode.h>
#include <qore/intern/QoreAssignmentOperatorNode.h>
#include <qore/intern/QoreIntAssignmentOperatorNode.h>
#include <qore/intern/QorePlusEqualsOperatorNode.h>
#include <qore/intern/QoreIntPlusEqualsOperatorNode.h>
#include <qore/intern/QoreMinusEqualsOperatorNode.h>
#include <qore/intern/QoreIntMinusEqualsOperatorNode.h>
#include <qore/intern/QoreOrEqualsOperatorNode.h>
#include <qore/intern/QoreIntOrEqualsOperatorNode.h>
#include <qore/intern/QoreAndEqualsOperatorNode.h>
#include <qore/intern/QoreIntAndEqualsOperatorNode.h>
#include <qore/intern/QoreModulaEqualsOperatorNode.h>
#include <qore/intern/QoreIntModulaEqualsOperatorNode.h>
#include <qore/intern/QoreMultiplyEqualsOperatorNode.h>
#include <qore/intern/QoreIntMultiplyEqualsOperatorNode.h>
#include <qore/intern/QoreDivideEqualsOperatorNode.h>
#include <qore/intern/QoreIntDivideEqualsOperatorNode.h>
#include <qore/intern/QoreXorEqualsOperatorNode.h>
#include <qore/intern/QoreIntXorEqualsOperatorNode.h>
#include <qore/intern/QoreShiftLeftEqualsOperatorNode.h>
#include <qore/intern/QoreIntShiftLeftEqualsOperatorNode.h>
#include <qore/intern/QoreShiftRightEqualsOperatorNode.h>
#include <qore/intern/QoreIntShiftRightEqualsOperatorNode.h>
#include <qore/intern/QorePostIncrementOperatorNode.h>
#include <qore/intern/QoreIntPostIncrementOperatorNode.h>
#include <qore/intern/QorePostDecrementOperatorNode.h>
#include <qore/intern/QoreIntPostDecrementOperatorNode.h>
#include <qore/intern/QorePreIncrementOperatorNode.h>
#include <qore/intern/QoreIntPreIncrementOperatorNode.h>
#include <qore/intern/QorePreDecrementOperatorNode.h>
#include <qore/intern/QoreIntPreDecrementOperatorNode.h>
#include <qore/intern/QoreLogicalLessThanOperatorNode.h>
#include <qore/intern/QoreLogicalGreaterThanOrEqualsOperatorNode.h>
#include <qore/intern/QoreLogicalGreaterThanOperatorNode.h>
#include <qore/intern/QoreLogicalLessThanOrEqualsOperatorNode.h>

#endif
