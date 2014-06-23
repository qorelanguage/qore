/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreShiftRightEqualsOperatorNode.h

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

#ifndef _QORE_QORESHIFTRIGHTEQUALSOPERATORNODE_H
#define _QORE_QORESHIFTRIGHTEQUALSOPERATORNODE_H

class QoreShiftRightEqualsOperatorNode : public QoreBinaryIntLValueOperatorNode {
OP_COMMON
protected:
   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);
   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const {
      int64 rv = QoreShiftRightEqualsOperatorNode::bigIntEvalImpl(xsink);
      return *xsink || !ref_rv ? 0 : new QoreBigIntNode(rv);
   }

   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
      needs_deref = ref_rv;
      int64 rv = QoreShiftRightEqualsOperatorNode::bigIntEvalImpl(xsink);
      return *xsink || ! ref_rv ? 0 : new QoreBigIntNode(rv);
   }

   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const {
      return (int)QoreShiftRightEqualsOperatorNode::bigIntEvalImpl(xsink);
   }

   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const {
      return (double)QoreShiftRightEqualsOperatorNode::bigIntEvalImpl(xsink);
   }

   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const {
      return (bool)QoreShiftRightEqualsOperatorNode::bigIntEvalImpl(xsink);
   }

public:
   DLLLOCAL QoreShiftRightEqualsOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreBinaryIntLValueOperatorNode(n_left, n_right) {
   }
};

#endif

