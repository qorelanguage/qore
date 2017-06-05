/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreCastOperatorNode.h

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

#ifndef _QORE_QORECASTOPERATORNODE_H

#define _QORE_QORECASTOPERATORNODE_H

class QoreCastOperatorNode : public QoreSingleExpressionOperatorNode<QoreOperatorNode> {
protected:
   DLLLOCAL static QoreString cast_str;
   NamedScope *path;
   QoreClass *qc;

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return qc ? qc->getTypeInfo() : objectTypeInfo;
   }

   DLLLOCAL QoreCastOperatorNode(const QoreProgramLocation& loc, QoreClass* q, AbstractQoreNode *n_exp) : QoreSingleExpressionOperatorNode<QoreOperatorNode>(loc, n_exp), path(0), qc(q) {
   }

public:
   DLLLOCAL QoreCastOperatorNode(int sline, int eline, char *str, AbstractQoreNode *n_exp) : QoreSingleExpressionOperatorNode<QoreOperatorNode>(sline, eline, n_exp), path(new NamedScope(str)), qc(0) {
   }

   DLLLOCAL QoreCastOperatorNode(const QoreProgramLocation& loc, char *str, AbstractQoreNode *n_exp) : QoreSingleExpressionOperatorNode<QoreOperatorNode>(loc, n_exp), path(new NamedScope(str)), qc(0) {
   }

   DLLLOCAL virtual ~QoreCastOperatorNode() {
      delete path;
   }

   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const {
      return cast_str.getBuffer();
   }

   DLLLOCAL virtual bool hasEffect() const {
      return false;
   }

   DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink *xsink) const {
      ReferenceHolder<> n_exp(copy_and_resolve_lvar_refs(exp, xsink), xsink);
      if (*xsink)
         return 0;
      assert(!path);
      assert(qc);
      return new QoreCastOperatorNode(loc, qc, n_exp.release());
   }
};

#endif
