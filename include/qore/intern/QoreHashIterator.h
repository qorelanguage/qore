/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreHashIterator.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

#ifndef _QORE_QOREHASHITERATOR_H

#define _QORE_QOREHASHITERATOR_H

extern QoreClass* QC_HASHITERATOR;
extern QoreClass* QC_HASHKEYITERATOR;
extern QoreClass* QC_HASHPAIRITERATOR;
extern QoreClass* QC_HASHREVERSEITERATOR;

// the c++ object
class QoreHashIterator : public QoreIteratorBase, public ConstHashIterator {
protected:
   // reusable hash for pair iterator performance enhancement; provides an approx 70% speed improvement
   mutable QoreHashNode* pairHash;

   DLLLOCAL virtual ~QoreHashIterator() {
      assert(!pairHash);
      assert(!h);
   }

   DLLLOCAL int checkPtr(ExceptionSink* xsink) const {
      if (!valid()) {
         xsink->raiseException("ITERATOR-ERROR", "the %s is not pointing at a valid element; make sure %s::next() returns True before calling this method", getName(), getName());
         return -1;
      }
      return 0;
   }

   DLLLOCAL QoreHashIterator(QoreHashNode* h) : ConstHashIterator(h), pairHash(0) {
   }

public:
   DLLLOCAL QoreHashIterator(const QoreHashNode* h) : ConstHashIterator(h->hashRefSelf()), pairHash(0) {
   }

   DLLLOCAL QoreHashIterator() : ConstHashIterator(0), pairHash(0) {
   }

   DLLLOCAL QoreHashIterator(const QoreHashIterator& old) : ConstHashIterator(*this), pairHash(0) {
   }

   using AbstractPrivateData::deref;
   DLLLOCAL virtual void deref(ExceptionSink* xsink) {
      if (ROdereference()) {
         if (h) {
            const_cast<QoreHashNode*>(h)->deref(xsink);
#ifdef DEBUG
            h = 0;
#endif
         }
         if (pairHash) {
            pairHash->deref(xsink);
#ifdef DEBUG
            pairHash = 0;
#endif
         }
         delete this;
      }
   }

   DLLLOCAL AbstractQoreNode* getReferencedValue(ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;
      return ConstHashIterator::getReferencedValue();
   }

   DLLLOCAL AbstractQoreNode* getReferencedKeyValue(ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;
      return ConstHashIterator::getReferencedValue();
   }

   DLLLOCAL QoreHashNode* getReferencedValuePair(ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;
      // create or re-use the pair hash if possible
      if (!pairHash)
         pairHash = new QoreHashNode;
      else if (!pairHash->is_unique()) {
         pairHash->deref(xsink);
         pairHash = new QoreHashNode;
      }
      pairHash->setKeyValue("key", new QoreStringNode(ConstHashIterator::getKey()), xsink);
      pairHash->setKeyValue("value", ConstHashIterator::getReferencedValue(), xsink);
      return pairHash->hashRefSelf();
   }

   DLLLOCAL QoreStringNode* getKey(ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;
      return new QoreStringNode(ConstHashIterator::getKey());
   }

   DLLLOCAL bool empty() const {
      return !h || h->empty();
   }

   DLLLOCAL bool next() {
      if (!h)
         return false;
      return ConstHashIterator::next();
   }

   DLLLOCAL bool prev() {
      if (!h)
         return false;
      return ConstHashIterator::prev();
   }

   DLLLOCAL virtual const char* getName() const { return "HashIterator"; }
};

// internal reverse iterator class implementation only for the getName() function - the iterators remain
// forwards and are used in the reverse sense by the Qore language class implementation below
class QoreHashReverseIterator : public QoreHashIterator {
public:
   DLLLOCAL QoreHashReverseIterator(const QoreHashNode* h) : QoreHashIterator(h) {
   }

   DLLLOCAL QoreHashReverseIterator() {
   }

   DLLLOCAL QoreHashReverseIterator(const QoreHashReverseIterator& old) : QoreHashIterator(old) {
   }

   DLLLOCAL virtual const char* getName() const {
      return "HashReverseIterator";
   }
};

#endif // _QORE_QOREHASHITERATOR_H
