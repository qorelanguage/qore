/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreMapOperatorNode.h

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

#ifndef _QORE_QOREMAPOPERATORNODE_H

#define _QORE_QOREMAPOPERATORNODE_H

#include <qore/intern/AbstractIteratorHelper.h>
#include <qore/intern/FunctionalOperator.h>
#include <qore/intern/FunctionalOperatorInterface.h>

class QoreMapOperatorNode : public QoreBinaryOperatorNode<>, public FunctionalOperator {
   friend class QoreFunctionalMapListOperator;
   friend class QoreFunctionalMapSingleValueOperator;
   friend class QoreFunctionalMapIteratorOperator;
   friend class QoreFunctionalMapOperator;

protected:
   const QoreTypeInfo* returnTypeInfo;
   FunctionalOperator* iterator_func;

   DLLLOCAL static QoreString map_str;

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL QoreValue evalValueFunc(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual ~QoreMapOperatorNode() {
   }

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return returnTypeInfo;
   }

   DLLLOCAL QoreValue mapIterator(AbstractIteratorHelper& h, ExceptionSink* xsink) const;

   DLLLOCAL virtual FunctionalOperatorInterface* getFunctionalIteratorImpl(FunctionalValueType& value_type, ExceptionSink* xsink) const;

public:
   DLLLOCAL QoreMapOperatorNode(AbstractQoreNode* l, AbstractQoreNode* r) : QoreBinaryOperatorNode<>(l, r), returnTypeInfo(0) {
   }

   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const;
   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const;

   // returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const {
      return map_str.getBuffer();
   }

   //DLLLOCAL AbstractQoreNode* map(ExceptionSink* xsink) const;
};

class QoreFunctionalMapListOperator : public FunctionalOperatorInterface, public ConstListIterator {
protected:
   const QoreMapOperatorNode* map;
   ExceptionSink* xsink;

public:
   DLLLOCAL QoreFunctionalMapListOperator(const QoreMapOperatorNode* m, QoreListNode* l, ExceptionSink* xs) : ConstListIterator(l), map(m), xsink(xs) {
   }

   DLLLOCAL virtual ~QoreFunctionalMapListOperator() {
      const_cast<QoreListNode*>(getList())->deref(xsink);
   }

   DLLLOCAL virtual bool getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink);
};

class QoreFunctionalMapSingleValueOperator : public FunctionalOperatorInterface {
protected:
   const QoreMapOperatorNode* map;
   QoreValue v;
   bool done;
   ExceptionSink* xsink;

public:
   DLLLOCAL QoreFunctionalMapSingleValueOperator(const QoreMapOperatorNode* m, QoreValue n, ExceptionSink* xs) : map(m), v(n), done(false), xsink(xs) {
   }

   DLLLOCAL virtual ~QoreFunctionalMapSingleValueOperator() {
      v.discard(xsink);
   }

   DLLLOCAL virtual bool getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink);
};

class QoreFunctionalMapIteratorOperator : public FunctionalOperatorInterface {
protected:
   const QoreMapOperatorNode* map;
   bool temp;
   AbstractIteratorHelper h;
   size_t index;
   ExceptionSink* xsink;

public:
   DLLLOCAL QoreFunctionalMapIteratorOperator(const QoreMapOperatorNode* m, bool t, AbstractIteratorHelper n_h, ExceptionSink* xs) : map(m), temp(t), h(n_h), index(0), xsink(xs) {
   }

   DLLLOCAL ~QoreFunctionalMapIteratorOperator() {
      if (temp)
         h.obj->deref(xsink);
   }

   DLLLOCAL virtual bool getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink);
};

class QoreFunctionalMapOperator : public FunctionalOperatorInterface {
protected:
   const QoreMapOperatorNode* map;
   FunctionalOperatorInterface* f;
   size_t index;

public:
   DLLLOCAL QoreFunctionalMapOperator(const QoreMapOperatorNode* m, FunctionalOperatorInterface* n_f) : map(m), f(n_f), index(0) {
   }

   DLLLOCAL ~QoreFunctionalMapOperator() {
      delete f;
   }

   DLLLOCAL virtual bool getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink);
};

#endif
