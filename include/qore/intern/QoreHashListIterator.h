/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreHashListIterator.h

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

#ifndef _QORE_QOREHASHLISTITERATOR_H

#define _QORE_QOREHASHLISTITERATOR_H

// the c++ object
class QoreHashListIterator : public QoreIteratorBase {
protected:
   QoreHashNode* h;
   qore_offset_t i, limit;

   DLLLOCAL virtual ~QoreHashListIterator() {
   }

   DLLLOCAL int checkPtr(ExceptionSink* xsink) const {
      if (i < 0) {
         xsink->raiseException("ITERATOR-ERROR", "the %s is not pointing at a valid element; make sure %s::next() returns True before calling this method", getName(), getName());
         return -1;
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode* getReferencedValueIntern(const AbstractQoreNode* n, const char* key, ExceptionSink* xsink) const {
      qore_type_t t = get_node_type(n);
      if (t == NT_NOTHING)
         return 0;

      if (t != NT_LIST) {
         xsink->raiseException("HASHLISTITERATOR-ERROR", "hash key '%s' is assigned to type '%s'; expecting 'list'", key, get_type_name(n));
         return 0;
      }
      return reinterpret_cast<const QoreListNode*>(n)->get_referenced_entry(i);
   }

   DLLLOCAL AbstractQoreNode* getReferencedKeyValueIntern(const char* key, ExceptionSink* xsink) const {
      return getReferencedValueIntern(h->getKeyValue(key), key, xsink);
   }

public:
   DLLLOCAL QoreHashListIterator(const QoreHashNode* n_h) : h(n_h->hashRefSelf()), i(-1), limit(0) {
      if (h->empty())
         return;
      // use an iterator for quick access to the first key in the hash
      ConstHashIterator hi(h);
      // must succeed because the hash is not empty
      hi.next();
      const AbstractQoreNode* n = hi.getValue();
      if (get_node_type(n) != NT_LIST)
         return;
      limit = (qore_offset_t)reinterpret_cast<const QoreListNode*>(n)->size();
   }

   DLLLOCAL QoreHashListIterator() : h(0), i(-1), limit(0) {
   }

   DLLLOCAL QoreHashListIterator(const QoreHashListIterator& old) : h(old.h ? old.h->hashRefSelf() : 0), i(old.i), limit(old.limit) {
   }

   DLLLOCAL void reset() {
      if (i != -1)
         i = -1;
   }

   using QoreIteratorBase::deref;
   DLLLOCAL virtual void deref(ExceptionSink* xsink) {
      if (ROdereference()) {
         if (h)
            h->deref(xsink);
         delete this;
      }
   }

   DLLLOCAL bool next() {
      if (++i == limit) {
         i = -1;
         return false; // finished
      }
      return true;
   }

   DLLLOCAL bool prev() {
      if (!limit)
         return false; // empty
      if (i == -1) {
         i = limit - 1;
         return true;
      }
      --i;
      return i >= 0;
   }

   DLLLOCAL bool empty() const {
      return !limit;
   }

   // returns true if the iterator is pointing to a valid element
   DLLLOCAL bool valid() const {
      return i != -1;
   }

   // returns the current iterator position in the list or -1 if not pointing at a valid element
   DLLLOCAL qore_size_t index() const {
      return i;
   }

   // returns the number of elements in the list
   DLLLOCAL qore_size_t max() const {
      return limit;
   }

   // returns true when the iterator is pointing to the first element in the list
   DLLLOCAL bool first() const {
      return i == 0;
   }

    // returns true when the iterator is pointing to the last element in the list
   DLLLOCAL bool last() const {
      return i == (limit - 1);
   }

   DLLLOCAL bool set(qore_offset_t pos) {
      if (pos < 0 || pos >= limit) {
         i = -1;
         return false;
      }
      i = pos;
      return true;
   }

   DLLLOCAL AbstractQoreNode* getReferencedKeyValue(const char* key, ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;

      return getReferencedKeyValueIntern(key, xsink);
   }

   DLLLOCAL QoreHashNode* getRow(ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;

      ReferenceHolder<QoreHashNode> rv(new QoreHashNode, xsink);

      ConstHashIterator hi(h);
      while (hi.next()) {
         AbstractQoreNode* n = const_cast<AbstractQoreNode*>(hi.getValue());
         n = getReferencedValueIntern(n, hi.getKey(), xsink);
         if (*xsink)
            return 0;
         rv->setKeyValue(hi.getKey(), n, xsink);
         // cannot have an exception here
         assert(!*xsink);
      }

      return rv.release();
   }

   DLLLOCAL QoreHashNode* getSlice(const QoreListNode* args, ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;

      ReferenceHolder<QoreHashNode> rv(new QoreHashNode, xsink);

      ConstListIterator li(args);
      while (li.next()) {
         QoreStringValueHelper str(li.getValue(), QCS_UTF8, xsink);
         if (*xsink)
            return 0;
         const char* key = str->getBuffer();
         AbstractQoreNode* n = getReferencedKeyValueIntern(key, xsink);
         if (*xsink)
            return 0;
         rv->setKeyValue(key, n, xsink);
         // cannot have an exception here
         assert(!*xsink);
      }

      return rv.release();
   }

   DLLLOCAL virtual const char* getName() const {
      return "HashListIterator";
   }
};

#endif // _QORE_QOREHASHLISTITERATOR_H
