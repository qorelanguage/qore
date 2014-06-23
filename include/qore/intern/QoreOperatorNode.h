/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreOperatorNode.h

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

#ifndef _QORE_QOREOPERATORNODE_H
#define _QORE_QOREOPERATORNODE_H

class QoreOperatorNode : public ParseNode {
protected:
   bool ref_rv;

   DLLLOCAL virtual ~QoreOperatorNode() {}

   DLLLOCAL virtual void ignoreReturnValueImpl() {}

public:
   // populated automatically on creation
   QoreProgramLocation loc;

   DLLLOCAL QoreOperatorNode(bool n_ref_rv = true) : ParseNode(NT_OPERATOR), ref_rv(n_ref_rv), loc(ParseLocation) {
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
   AbstractQoreNode* left, * right;

public:
   DLLLOCAL QoreBinaryOperatorNode(AbstractQoreNode* n_left, AbstractQoreNode* n_right) : left(n_left), right(n_right) {
   }

   DLLLOCAL ~QoreBinaryOperatorNode() {
      if (left)
         left->deref(0);
      if (right)
         right->deref(0);
   }

   template<typename U>
   DLLLOCAL QoreBinaryOperatorNode* makeSpecialization() {
      AbstractQoreNode* l = left,* r = right;
      left = right = 0;
      SimpleRefHolder<QoreBinaryOperatorNode> del(this);
      U* rv = new U(l, r);
      if (!this->ref_rv)
         rv->ignoreReturnValue();
      return rv;
   }

   DLLLOCAL AbstractQoreNode* swapRight(AbstractQoreNode *n_right) {
      AbstractQoreNode *old_r = right;
      right = n_right;
      return old_r;
   }

   DLLLOCAL AbstractQoreNode* getLeft() {
      return left;
   }

   DLLLOCAL AbstractQoreNode* getRight() {
      return right;
   }
};

class QoreBoolBinaryOperatorNode : public QoreBinaryOperatorNode<> {
public:
   DLLLOCAL QoreBoolBinaryOperatorNode(AbstractQoreNode* n_left, AbstractQoreNode* n_right) : QoreBinaryOperatorNode<>(n_left, n_right) {
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
   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const {del = false;return &op_str;}\
   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {str.concat(&op_str);return 0;}\
   DLLLOCAL virtual const char* getTypeName() const {return op_str.getBuffer();}

class LValueOperatorNode : public QoreOperatorNode {
public:
   DLLLOCAL virtual bool hasEffect() const {
      return true;
   }

   DLLLOCAL void checkLValue(AbstractQoreNode* exp, int pflag, bool assignment = true) {
      if (exp) {
         if (check_lvalue(exp, assignment))
            parse_error("expecing lvalue for %s, got '%s' instead", getTypeName(), exp->getTypeName());
         else if ((pflag & PF_BACKGROUND) && exp && exp->getType() == NT_VARREF && reinterpret_cast<const VarRefNode*>(exp)->getType() == VT_LOCAL)
            parse_error("illegal local variable modification with the background operator in %s", getTypeName());
      }
   }
};

template <class T = QoreOperatorNode>
class QoreTrinaryOperatorNode : public T {
protected:
   AbstractQoreNode* e[3];

public:
   DLLLOCAL QoreTrinaryOperatorNode(AbstractQoreNode* e0, AbstractQoreNode* e1, AbstractQoreNode* e2) {
      e[0] = e0;
      e[1] = e1;
      e[2] = e2;
   }

   DLLLOCAL ~QoreTrinaryOperatorNode() {
      for (unsigned i = 0; i < 3; ++i)
         if (e[i]) e[i]->deref(0);
   }

   DLLLOCAL AbstractQoreNode* get(unsigned i) {
      assert(i < 3);
      return e[i];
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
#include <qore/intern/QoreAndEqualsOperatorNode.h>
#include <qore/intern/QoreModulaEqualsOperatorNode.h>
#include <qore/intern/QoreMultiplyEqualsOperatorNode.h>
#include <qore/intern/QoreDivideEqualsOperatorNode.h>
#include <qore/intern/QoreXorEqualsOperatorNode.h>
#include <qore/intern/QoreShiftLeftEqualsOperatorNode.h>
#include <qore/intern/QoreShiftRightEqualsOperatorNode.h>
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
#include <qore/intern/QoreQuestionMarkOperatorNode.h>

#endif
