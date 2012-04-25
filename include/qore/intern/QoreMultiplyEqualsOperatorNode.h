/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreMultiplyEqualsOperatorNode.h

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

#ifndef _QORE_QOREMULTIPLYEQUALSOPERATORNODE_H
#define _QORE_QOREMULTIPLYEQUALSOPERATORNODE_H

class QoreMultiplyEqualsOperatorNode : public QoreBinaryLValueOperatorNode {
OP_COMMON
protected:
   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

public:
   DLLLOCAL QoreMultiplyEqualsOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreBinaryLValueOperatorNode(n_left, n_right) {
   }

   DLLLOCAL void parseInitIntern(const char *name, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      left = left->parseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, ti);
      checkLValue(left);

      const QoreTypeInfo *rightTypeInfo = 0;
      right = right->parseInit(oflag, pflag, lvids, rightTypeInfo);

      if (!ti->isType(NT_FLOAT)) {
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

      typeInfo = ti;
   }
};

#endif

