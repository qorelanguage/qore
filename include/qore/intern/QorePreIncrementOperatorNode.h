/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QorePreIncrementEqualsOperatorNode.h

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

#ifndef _QORE_QOREPREINCREMENTOPERATORNODE_H
#define _QORE_QOREPREINCREMENTOPERATORNODE_H

class QorePreIncrementOperatorNode : public QoreSingleExpressionOperatorNode<LValueOperatorNode> {
   friend class QorePostIncrementOperatorNode;

   OP_COMMON
protected:
   const QoreTypeInfo* typeInfo;

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&outTypeInfo);
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;
   DLLLOCAL void parseInitIntern(const char *name, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo*& outTypeInfo) {
      assert(!outTypeInfo);
      // turn off "reference ok" and "return value ignored" flags
      pflag &= ~(PF_RETURN_VALUE_IGNORED);
      
      exp = exp->parseInit(oflag, pflag, lvids, outTypeInfo);
      checkLValue(exp, pflag);

      // make sure left side can take an integer or floating-point value
      check_lvalue_int_float_number(outTypeInfo, name);

      // save return type
      typeInfo = outTypeInfo;
   }

   // always returns an int
   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return typeInfo;
   }

public:
   DLLLOCAL QorePreIncrementOperatorNode(AbstractQoreNode *n_exp) : QoreSingleExpressionOperatorNode<LValueOperatorNode>(n_exp) {
   }
};

#endif

