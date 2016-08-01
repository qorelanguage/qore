/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreMultiplyEqualsOperatorNode.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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

#ifndef _QORE_QOREMULTIPLYEQUALSOPERATORNODE_H
#define _QORE_QOREMULTIPLYEQUALSOPERATORNODE_H

class QoreMultiplyEqualsOperatorNode : public QoreBinaryLValueOperatorNode {
OP_COMMON
protected:
   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

public:
   DLLLOCAL QoreMultiplyEqualsOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreBinaryLValueOperatorNode(n_left, n_right) {
   }

   DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink *xsink) const {
      return copyBackgroundExplicit<QoreMultiplyEqualsOperatorNode>(xsink);
   }

   DLLLOCAL void parseInitIntern(const char *name, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      left = left->parseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, ti);
      checkLValue(left, pflag);

      const QoreTypeInfo *rightTypeInfo = 0;
      right = right->parseInit(oflag, pflag, lvids, rightTypeInfo);

      if (!ti->isType(NT_NUMBER)) {
         if (rightTypeInfo->isType(NT_NUMBER)) {
            check_lvalue_number(ti, name);
            ti = numberTypeInfo;
         }
         else if (!ti->isType(NT_FLOAT)) {
            if (rightTypeInfo->isType(NT_FLOAT)) {
               check_lvalue_float(ti, name);
               ti = floatTypeInfo;
            }
            else if (ti->returnsSingle()) {
               check_lvalue_int(ti, name);
               ti = bigIntTypeInfo;
            }
            else
               ti = 0;
         }
      }

      typeInfo = ti;
   }
};

#endif
