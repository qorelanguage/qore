/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreRegexMatchOperatorNode.h

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

#ifndef _QORE_QOREREGEXMATCHOPERATORNODE_H

#define _QORE_QOREREGEXMATCHOPERATORNODE_H

class QoreRegexMatchOperatorNode : public QoreSingleExpressionOperatorNode<> {
OP_COMMON
protected:
   SimpleRefHolder<QoreRegex> regex;

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
      return parseInitIntern(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);
   }

   DLLLOCAL AbstractQoreNode* parseInitIntern(const char* name, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

public:
   DLLLOCAL QoreRegexMatchOperatorNode(AbstractQoreNode* n_exp, QoreRegex* r) : QoreSingleExpressionOperatorNode<>(n_exp), regex(r) {
   }

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return boolTypeInfo;
   }

   DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink* xsink) const {
      ReferenceHolder<> n_exp(copy_and_resolve_lvar_refs(exp, xsink), xsink);
      if (*xsink)
         return 0;
      return new QoreRegexMatchOperatorNode(n_exp.release(), regex->refSelf());
   }
};

#endif
