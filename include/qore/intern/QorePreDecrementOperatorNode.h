/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QorePreDecrementEqualsOperatorNode.h

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
 Foundation, Dec., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _QORE_QOREPREDECREMENTOPERATORNODE_H
#define _QORE_QOREPREDECREMENTOPERATORNODE_H

class QorePreDecrementOperatorNode : public QorePreIncrementOperatorNode {
   friend class QorePostDecrementOperatorNode;
OP_COMMON
protected:
   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

public:
   DLLLOCAL QorePreDecrementOperatorNode(AbstractQoreNode *n_exp) : QorePreIncrementOperatorNode(n_exp) {
   }
};

#endif

