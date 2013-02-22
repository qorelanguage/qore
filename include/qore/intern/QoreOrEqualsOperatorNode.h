/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreOrEqualsOperatorNode.h

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

#ifndef _QORE_QOREOREQUALSOPERATORNODE_H
#define _QORE_QOREOREQUALSOPERATORNODE_H

class QoreOrEqualsOperatorNode : public QoreBinaryIntLValueOperatorNode {
OP_COMMON
protected:
   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);
   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const {
      int64 rv = QoreOrEqualsOperatorNode::bigIntEvalImpl(xsink);
      return *xsink || !ref_rv ? 0 : new QoreBigIntNode(rv);
   }

   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
      needs_deref = ref_rv;
      int64 rv = QoreOrEqualsOperatorNode::bigIntEvalImpl(xsink);
      return *xsink || ! ref_rv ? 0 : new QoreBigIntNode(rv);
   }

   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const {
      return (int)QoreOrEqualsOperatorNode::bigIntEvalImpl(xsink);
   }

   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const {
      return (double)QoreOrEqualsOperatorNode::bigIntEvalImpl(xsink);
   }

   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const {
      return (bool)QoreOrEqualsOperatorNode::bigIntEvalImpl(xsink);
   }

public:
   DLLLOCAL QoreOrEqualsOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreBinaryIntLValueOperatorNode(n_left, n_right) {
   }
};

#endif

