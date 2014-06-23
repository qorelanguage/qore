/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreLogicalEqualsOperatorNode.h
 
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
