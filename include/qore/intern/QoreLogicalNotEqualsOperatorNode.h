/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreLogicalNotEqualsOperatorNode.h
 
 Qore Programming Language
 
 Copyright 2003 - 2011 David Nichols
 
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

#ifndef _QORE_QORELOGICALNOTEQUALSOPERATORNODE_H

#define _QORE_QORELOGICALNOTEQUALSOPERATORNODE_H

class QoreLogicalNotEqualsOperatorNode : public QoreLogicalEqualsOperatorNode {
protected:
   DLLLOCAL static QoreString logical_not_equals_str;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const {
      bool rc = QoreLogicalNotEqualsOperatorNode::boolEvalImpl(xsink);
      return *xsink ? 0 : get_bool_node(rc);
   }

   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
      needs_deref = false;
      return QoreLogicalNotEqualsOperatorNode::evalImpl(xsink);
   }

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const {
      return QoreLogicalNotEqualsOperatorNode::boolEvalImpl(xsink);
   }
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const {
      return QoreLogicalNotEqualsOperatorNode::boolEvalImpl(xsink);
   }
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const {
      return QoreLogicalNotEqualsOperatorNode::boolEvalImpl(xsink);
   }

   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const {
      return !QoreLogicalEqualsOperatorNode::boolEvalImpl(xsink);
   }

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      AbstractQoreNode *rv = QoreLogicalEqualsOperatorNode::parseInitImpl(oflag, pflag, lvids, typeInfo);
      // make sure to reverse sense of comparison if this expression was resolved to a constant boolean value
      if (rv != this)
         return rv->getAsBool() ? (AbstractQoreNode *)&False : (AbstractQoreNode *)&True;
      return rv;
   }

public:
   DLLLOCAL QoreLogicalNotEqualsOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreLogicalEqualsOperatorNode(n_left, n_right) {
   }

   // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const {
      del = false;
      return &logical_not_equals_str;
   }

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
      str.concat(&logical_not_equals_str);
      return 0;
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const {
      return logical_not_equals_str.getBuffer();
   }

   DLLLOCAL virtual bool hasEffect() const {
      return false;
   }

   DLLLOCAL static bool softEqual(const AbstractQoreNode *left, const AbstractQoreNode *right, ExceptionSink *xsink);
};

#endif
