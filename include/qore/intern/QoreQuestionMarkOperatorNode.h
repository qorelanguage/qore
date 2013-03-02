/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreQuestionMarkOperatorNode.h

 Qore Programming Language

 Copyright 2003 - 2013 David Nichols

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

#ifndef _QORE_QOREQUESTIONMARKOPERATORNODE_H
#define _QORE_QOREQUESTIONMARKOPERATORNODE_H

class QoreQuestionMarkOperatorNode : public QoreTrinaryOperatorNode<> {
protected:
   static QoreString question_mark_str;

   const QoreTypeInfo* typeInfo;

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo) {
      const QoreTypeInfo *leftTypeInfo = 0;
      e[0] = e[0]->parseInit(oflag, pflag, lvids, leftTypeInfo);

      if (leftTypeInfo->nonNumericValue() && checkParseOption(PO_STRICT_BOOLEAN_EVAL))
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
