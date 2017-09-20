/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreTransliterationOperatorNode.h

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

#ifndef _QORE_QORETRANSLITERATIONOPERATORNODE_H

#define _QORE_QORETRANSLITERATIONOPERATORNODE_H

class QoreTransliterationOperatorNode : public QoreSingleExpressionOperatorNode<LValueOperatorNode> {
OP_COMMON
protected:
   const QoreTypeInfo* typeInfo;
   SimpleRefHolder<QoreTransliteration> regex;

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

public:
   DLLLOCAL QoreTransliterationOperatorNode(const QoreProgramLocation& loc, AbstractQoreNode* n_exp, QoreTransliteration* r) : QoreSingleExpressionOperatorNode<LValueOperatorNode>(loc, n_exp), typeInfo(0), regex(r) {
   }

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return typeInfo;
   }

   DLLLOCAL virtual bool hasEffect() const {
      return true;
   }

   DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink* xsink) const {
      ReferenceHolder<> n_exp(copy_and_resolve_lvar_refs(exp, xsink), xsink);
      if (*xsink)
         return nullptr;
      return new QoreTransliterationOperatorNode(loc, n_exp.release(), regex->refSelf());
   }
};

#endif
