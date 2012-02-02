/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreLogicalEqualsOperatorNode.h
 
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

#ifndef _QORE_QORELOGICALEQUALSOPERATORNODE_H

#define _QORE_QORELOGICALEQUALSOPERATORNODE_H

class QoreLogicalEqualsOperatorNode : public QoreOperatorNode {
protected:
   AbstractQoreNode *left, *right; // parts of the expression

   // type of pointer to optimized versions depending on arguments found at parse-time
   typedef bool(QoreLogicalEqualsOperatorNode::*eval_t)(ExceptionSink *xsink) const;
   // pointer to optimized versions depending on arguments found at parse-time   
   eval_t pfunc;

   DLLLOCAL static QoreString logical_equals_str;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const {
      bool rc = QoreLogicalEqualsOperatorNode::boolEvalImpl(xsink);
      return *xsink ? 0 : get_bool_node(rc);
   }

   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
      needs_deref = false;
      return QoreLogicalEqualsOperatorNode::evalImpl(xsink);
   }

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const {
      return QoreLogicalEqualsOperatorNode::boolEvalImpl(xsink);
   }
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const {
      return QoreLogicalEqualsOperatorNode::boolEvalImpl(xsink);
   }
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const {
      return QoreLogicalEqualsOperatorNode::boolEvalImpl(xsink);
   }

   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return boolTypeInfo;
   }

   DLLLOCAL bool floatSoftEqual(ExceptionSink *xsink) const;
   DLLLOCAL bool bigIntSoftEqual(ExceptionSink *xsink) const;
   DLLLOCAL bool boolSoftEqual(ExceptionSink *xsink) const;

public:
   DLLLOCAL QoreLogicalEqualsOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : left(n_left), right(n_right), pfunc(0) {
   }

   DLLLOCAL ~QoreLogicalEqualsOperatorNode() {
      if (left)
         left->deref(0);
      if (right)
         right->deref(0);
   }

   // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const {
      del = false;
      return &logical_equals_str;
   }

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
      str.concat(&logical_equals_str);
      return 0;
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const {
      return logical_equals_str.getBuffer();
   }

   DLLLOCAL virtual bool hasEffect() const {
      return false;
   }

   DLLLOCAL static bool softEqual(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink);
};

#endif
