/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreLogicalLessThanOperatorNode.h
 
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

#ifndef _QORE_QORELOGICALLESSTHANOPERATORNODE_H

#define _QORE_QORELOGICALLESSTHANOPERATORNODE_H

class QoreLogicalLessThanOperatorNode : public QoreBoolBinaryOperatorNode {
OP_COMMON
protected:
   // type of pointer to optimized versions depending on arguments found at parse-time
   typedef bool(QoreLogicalLessThanOperatorNode::*eval_t)(ExceptionSink *xsink) const;
   // pointer to optimized versions depending on arguments found at parse-time   
   eval_t pfunc;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const {
      bool rc = QoreLogicalLessThanOperatorNode::boolEvalImpl(xsink);
      return *xsink ? 0 : get_bool_node(rc);
   }

   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
      needs_deref = false;
      return QoreLogicalLessThanOperatorNode::evalImpl(xsink);
   }

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const {
      return QoreLogicalLessThanOperatorNode::boolEvalImpl(xsink);
   }
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const {
      return QoreLogicalLessThanOperatorNode::boolEvalImpl(xsink);
   }
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const {
      return QoreLogicalLessThanOperatorNode::boolEvalImpl(xsink);
   }

   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL AbstractQoreNode *parseInitIntern(const char *name, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL bool floatLessThan(ExceptionSink *xsink) const;
   DLLLOCAL bool bigIntLessThan(ExceptionSink *xsink) const;

public:
   DLLLOCAL QoreLogicalLessThanOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreBoolBinaryOperatorNode(n_left, n_right), pfunc(0) {
   }
};

#endif
