/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreDotEvalOperatorNode.h
 
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

#ifndef _QORE_QOREDOTEVALOPERATORNODE_H

#define _QORE_QOREDOTEVALOPERATORNODE_H

class QoreDotEvalOperatorNode : public QoreOperatorNode {
protected:
   static QoreString name;

   AbstractQoreNode *left;
   MethodCallNode *m;

   const QoreTypeInfo *returnTypeInfo;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return returnTypeInfo;
   }

public:
   DLLLOCAL QoreDotEvalOperatorNode(AbstractQoreNode *n_left, MethodCallNode *n_m) : left(n_left), m(n_m) {
   }

   DLLLOCAL ~QoreDotEvalOperatorNode() {
      if (left)
	 left->deref(0);
      if (m)
	 m->deref(0);
   }

   DLLLOCAL const AbstractQoreNode *getExpression() const {
      return left;
   }

   DLLLOCAL void replaceExpression(AbstractQoreNode *n_left) {
      left->deref(0);
      left = n_left;
   }

   DLLLOCAL AbstractQoreNode *makeCallReference();

   // if del is true, then the returned QoreString * should be removed, if false, then it must not be
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const {
      del = false;
      return &name;
   }

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
      str.concat(&name);
      return 0;
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const {
      return name.getBuffer();
   }

   DLLLOCAL virtual bool hasEffect() const {
      return true;
   }

   DLLLOCAL virtual QoreOperatorNode *copyBackground(ExceptionSink *xsink) const;
};

#endif
