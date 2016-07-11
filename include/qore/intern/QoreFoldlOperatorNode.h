/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreFoldlOperatorNode.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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

#ifndef _QORE_QOREFOLDLOPERATORNODE_H

#define _QORE_QOREFOLDLOPERATORNODE_H

#include <qore/intern/AbstractIteratorHelper.h>
#include <qore/intern/FunctionalOperator.h>
#include <qore/intern/FunctionalOperatorInterface.h>

class QoreFoldlOperatorNode : public QoreBinaryOperatorNode<> {
protected:
   const QoreTypeInfo* returnTypeInfo;
   FunctionalOperator* iterator_func;

   DLLLOCAL static QoreString foldl_str;

   DLLLOCAL QoreValue doFold(bool fwd, bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual ~QoreFoldlOperatorNode() {
   }

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return returnTypeInfo;
   }

   DLLLOCAL virtual bool hasEffect() const {
      // FIXME: check iterated expression to see if it really has an effect
      return true;
   }

   DLLLOCAL QoreValue foldIterator(AbstractIteratorHelper& h, ExceptionSink* xsink) const;

   DLLLOCAL FunctionalOperatorInterface* getFunctionalIterator(FunctionalOperator::FunctionalValueType& value_type, bool fwd, ExceptionSink* xsink) const;

public:
   DLLLOCAL QoreFoldlOperatorNode(AbstractQoreNode* l, AbstractQoreNode* r) : QoreBinaryOperatorNode<>(l, r), returnTypeInfo(0), iterator_func(0) {
   }

   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const;
   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const;

   // returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const {
      return foldl_str.getBuffer();
   }
};

class QoreFunctionalListOperator : public FunctionalOperatorInterface, public ConstListIterator {
protected:
   bool temp;
   bool fwd;
   ExceptionSink* xsink;

public:
   DLLLOCAL QoreFunctionalListOperator(bool t, bool f, QoreListNode* l, ExceptionSink* xs) : ConstListIterator(l), temp(t), fwd(f), xsink(xs) {
   }

   DLLLOCAL virtual ~QoreFunctionalListOperator() {
      if (temp)
         const_cast<QoreListNode*>(getList())->deref(xsink);
   }

   DLLLOCAL virtual bool getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink);
};

class QoreFunctionalSingleValueOperator : public FunctionalOperatorInterface {
protected:
   QoreValue v;
   bool done;
   ExceptionSink* xsink;

public:
   DLLLOCAL QoreFunctionalSingleValueOperator(QoreValue n, ExceptionSink* xs) : v(n), done(false), xsink(xs) {
   }

   DLLLOCAL virtual ~QoreFunctionalSingleValueOperator() {
      v.discard(xsink);
   }

   DLLLOCAL virtual bool getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink);
};

class QoreFunctionalIteratorOperator : public FunctionalOperatorInterface {
protected:
   bool temp;
   AbstractIteratorHelper h;
   size_t index;
   ExceptionSink* xsink;

public:
   DLLLOCAL QoreFunctionalIteratorOperator(bool t, AbstractIteratorHelper n_h, ExceptionSink* xs) : temp(t), h(n_h), index(0), xsink(xs) {
   }

   DLLLOCAL ~QoreFunctionalIteratorOperator() {
      if (temp)
         h.obj->deref(xsink);
   }

   DLLLOCAL virtual bool getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink);
};

class QoreFoldrOperatorNode : public QoreFoldlOperatorNode {
protected:
   DLLLOCAL static QoreString foldr_str;

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

public:
   DLLLOCAL QoreFoldrOperatorNode(AbstractQoreNode* l, AbstractQoreNode* r) : QoreFoldlOperatorNode(l, r) {
   }

   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const;
   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const;

   // returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const {
      return foldr_str.getBuffer();
   }
};

#endif
