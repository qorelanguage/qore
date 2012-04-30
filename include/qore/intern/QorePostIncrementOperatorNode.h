/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QorePostIncrementEqualsOperatorNode.h

 Qore Programming Language

 Copyright 2003 - 2012 David Nichols

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

#ifndef _QORE_QOREPOSTINCREMENTOPERATORNODE_H
#define _QORE_QOREPOSTINCREMENTOPERATORNODE_H

class QorePostIncrementOperatorNode : public QoreSingleExpressionOperatorNode<LValueOperatorNode> {
OP_COMMON
protected:
   const QoreTypeInfo *ti;

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;
   DLLLOCAL void parseInitIntern(const char *name, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      // turn off "reference ok" and "return value ignored" flags
      pflag &= ~(PF_REFERENCE_OK | PF_RETURN_VALUE_IGNORED);
      
      exp = exp->parseInit(oflag, pflag, lvids, ti);
      checkLValue(exp);
      // returns the left side
      typeInfo = ti;

      // make sure left side can take an integer or floating-point value
      check_lvalue_int_float(ti, name);
   }

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return ti;
   }

public:
   DLLLOCAL QorePostIncrementOperatorNode(AbstractQoreNode *n_exp) : QoreSingleExpressionOperatorNode<LValueOperatorNode>(n_exp), ti(0) {
   }
};

#endif

