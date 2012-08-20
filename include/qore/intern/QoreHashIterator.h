/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreHashIterator.h

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

#ifndef _QORE_QOREHASHITERATOR_H

#define _QORE_QOREHASHITERATOR_H

// the c++ object
class QoreHashIterator : public QoreIteratorBase, public ConstHashIterator {
protected:
   DLLLOCAL virtual ~QoreHashIterator() {
   }

   DLLLOCAL int checkPtr(ExceptionSink* xsink) const {
      if (!ptr) {
         xsink->raiseException("ITERATOR-ERROR", "the %s is not pointing at a valid element; make sure %s::next() returns True before calling this method", getName(), getName());
         return -1;
      }
      return 0;
   }

public:
   DLLLOCAL QoreHashIterator(const QoreHashNode* h) : ConstHashIterator(h->hashRefSelf()) {
   }

   DLLLOCAL QoreHashIterator() : ConstHashIterator(0) {
   }

   using QoreIteratorBase::deref;
   DLLLOCAL virtual void deref(ExceptionSink* xsink) {
      if (ROdereference()) {
         if (h)
            const_cast<QoreHashNode*>(h)->deref(xsink);
         delete this;
      }
   }

   DLLLOCAL AbstractQoreNode* getReferencedValue(ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;
      return ConstHashIterator::getReferencedValue();
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

#endif // _QORE_QOREHASHITERATOR_H
