/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreQuestionMarkOperatorNode.h

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

#ifndef _QORE_QOREQUESTIONMARKOPERATORNODE_H
#define _QORE_QOREQUESTIONMARKOPERATORNODE_H

class QoreQuestionMarkOperatorNode : public QoreTrinaryOperatorNode<> {
protected:
   static QoreString question_mark_str;

   const QoreTypeInfo* typeInfo;

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo) {
      const QoreTypeInfo *leftTypeInfo = 0;
      e[0] = e[0]->parseInit(oflag, pflag, lvids, leftTypeInfo);

      if (leftTypeInfo->nonNumericValue() && parse_check_parse_option(PO_STRICT_BOOLEAN_EVAL))
         leftTypeInfo->doNonBooleanWarning("the initial expression with the '?:' operator is ");

      leftTypeInfo = 0;
      e[1] = e[1]->parseInit(oflag, pflag, lvids, leftTypeInfo);

      const QoreTypeInfo* rightTypeInfo = 0;
      e[2] = e[2]->parseInit(oflag, pflag, lvids, rightTypeInfo);

      typeInfo = returnTypeInfo = leftTypeInfo->isOutputIdentical(rightTypeInfo) ? leftTypeInfo : 0;

      return this;
   }

public:
   DLLLOCAL QoreQuestionMarkOperatorNode(AbstractQoreNode* e0, AbstractQoreNode* e1, AbstractQoreNode* e2) : QoreTrinaryOperatorNode<>(e0, e1, e2), typeInfo(0) {
   }

   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const {
      del = false;
      return &question_mark_str;
   }

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
      str.concat(&question_mark_str);
      return 0;
   }

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const {
      bool b = e[0]->boolEval(xsink);
      if (xsink->isEvent())
         return 0;

      return b ? e[1]->eval(xsink) : e[2]->eval(xsink);
   }

   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
      needs_deref = true;
      return QoreQuestionMarkOperatorNode::evalImpl(xsink);
   }

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const {
      bool b = e[0]->boolEval(xsink);
      if (xsink->isEvent())
         return 0;

      return b ? e[1]->bigIntEval(xsink) : e[2]->bigIntEval(xsink);
   }

   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const {
      bool b = e[0]->boolEval(xsink);
      if (xsink->isEvent())
         return 0;

      return b ? e[1]->integerEval(xsink) : e[2]->integerEval(xsink);
   }

   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const {
      bool b = e[0]->boolEval(xsink);
      if (xsink->isEvent())
         return 0;

      return b ? e[1]->floatEval(xsink) : e[2]->floatEval(xsink);
   }

   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const {
      bool b = e[0]->boolEval(xsink);
      if (xsink->isEvent())
         return 0;

      return b ? e[1]->boolEval(xsink) : e[2]->boolEval(xsink);
   }

   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return typeInfo;
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const {
      return question_mark_str.getBuffer();
   }

   DLLLOCAL virtual bool hasEffect() const {
      return ::node_has_effect(e[1]) || ::node_has_effect(e[2]);
   }
};

#endif
