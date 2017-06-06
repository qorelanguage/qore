/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreBinaryLValueOperatorNode.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

#ifndef _QORE_QOREBINARYLVALUEOPERATORNODE_H

#define _QORE_QOREBINARYLVALUEOPERATORNODE_H

class QoreBinaryLValueOperatorNode : public QoreBinaryOperatorNode<LValueOperatorNode> {
protected:
   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return ti;
   }

public:
   const QoreTypeInfo *ti; // typeinfo of lhs

   DLLLOCAL QoreBinaryLValueOperatorNode(const QoreProgramLocation& loc, AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreBinaryOperatorNode<LValueOperatorNode>(loc, n_left, n_right), ti(0) {
   }

   DLLLOCAL virtual bool hasEffect() const {
      return true;
   }
};

// for operators that try to change the lvalue to an int
class QoreBinaryIntLValueOperatorNode : public QoreBinaryOperatorNode<LValueOperatorNode> {
protected:
   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return bigIntTypeInfo;
   }

public:
   DLLLOCAL QoreBinaryIntLValueOperatorNode(int sline, int eline, AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreBinaryOperatorNode<LValueOperatorNode>(sline, eline, n_left, n_right) {
   }

   DLLLOCAL QoreBinaryIntLValueOperatorNode(const QoreProgramLocation& loc, AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreBinaryOperatorNode<LValueOperatorNode>(loc, n_left, n_right) {
   }

   DLLLOCAL void parseInitIntLValue(const char *name, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      // turn off "reference ok" and "return value ignored" flags
      pflag &= ~(PF_RETURN_VALUE_IGNORED);

      typeInfo = bigIntTypeInfo;

      const QoreTypeInfo *mti = 0;
      left = left->parseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, mti);
      checkLValue(left, pflag);

      // make sure left side can take an integer value
      check_lvalue_int(loc, mti, name);

      mti = 0;
      // FIXME: check for invalid operation - type cannot be converted to integer
      right = right->parseInit(oflag, pflag, lvids, mti);
   }

   DLLLOCAL virtual bool hasEffect() const {
      return true;
   }
};
#endif
