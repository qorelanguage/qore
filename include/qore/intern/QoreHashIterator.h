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
   // if this flag is true, then getValue() returns a hash with 2 keys: "key" and "value"
   bool get_value_hash;

   DLLLOCAL virtual ~QoreHashIterator() {
   }

   DLLLOCAL int checkPtr(ExceptionSink* xsink) const {
      if (!ptr) {
         xsink->raiseException("ITERATOR-ERROR", "the %s is not pointing at a valid element; make sure %s::next() returns True before calling this method", getName(), getName());
         return -1;
      }
      return 0;
   }

   DLLLOCAL QoreHashIterator(QoreHashNode* h, bool gvh = false) : ConstHashIterator(h), get_value_hash(gvh) {
   }

public:
   DLLLOCAL QoreHashIterator(const QoreHashNode* h, bool gvh = false) : ConstHashIterator(h->hashRefSelf()), get_value_hash(gvh) {
   }

   DLLLOCAL QoreHashIterator() : ConstHashIterator(0), get_value_hash(false) {
   }

   DLLLOCAL QoreHashIterator(const QoreHashIterator& old) : ConstHashIterator(old.h ? old.h->hashRefSelf() : 0, old.ptr), get_value_hash(old.get_value_hash) {
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
      if (!get_value_hash)
         return ConstHashIterator::getReferencedValue();
      QoreHashNode* h = new QoreHashNode;
      h->setKeyValue("key", new QoreStringNode(ConstHashIterator::getKey()), 0);
      h->setKeyValue("value", ConstHashIterator::getReferencedValue(), 0);
      return h;
   }

   DLLLOCAL AbstractQoreNode* getReferencedKeyValue(ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;
      return ConstHashIterator::getReferencedValue();
   }

   DLLLOCAL QoreHashNode* getReferencedValuePair(ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;
      QoreHashNode* h = new QoreHashNode;
      h->setKeyValue("key", new QoreStringNode(ConstHashIterator::getKey()), 0);
      h->setKeyValue("value", ConstHashIterator::getReferencedValue(), 0);
      return h;
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
   DLLLOCAL QoreHashReverseIterator(const QoreHashNode* h, bool gvh = false) : QoreHashIterator(h, gvh) {
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
