/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreListHashIterator.h

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

#ifndef _QORE_QORELISTHASHITERATOR_H

#define _QORE_QORELISTHASHITERATOR_H

#include <qore/intern/QoreListIterator.h>

#include <assert.h>

// the c++ object
class QoreListHashIterator : public QoreListIterator {
protected:
   DLLLOCAL virtual ~QoreListHashIterator() {
   }

   DLLLOCAL const QoreHashNode* checkHash(ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;
      const AbstractQoreNode* n = getValue();
      if (get_node_type(n) != NT_HASH) {
         xsink->raiseException("ITERATOR-ERROR", "The %s object is not a list of hashes, element "QLLD" (starting with 0) is type '%s' instead (expected 'hash')", getName(), index(), get_type_name(n));
         return 0;
      }
      return static_cast<const QoreHashNode*>(n);
   }

   DLLLOCAL AbstractQoreNode* getReferencedKeyValueIntern(const QoreHashNode* h, const char* key, ExceptionSink* xsink) const {
      bool exists = false;
      const AbstractQoreNode* n = h->getKeyValueExistence(key, exists);
      if (!exists) {
         xsink->raiseException("LISTHASHITERATOR-ERROR", "key '%s' does not exist in the hash at element "QLLD" (starting with 0)", key, index());
         return 0;
      }
      return n ? n->refSelf() : 0;
   }

public:
   DLLLOCAL QoreListHashIterator(const QoreListNode* n_l) : QoreListIterator(n_l) {
   }

   DLLLOCAL QoreListHashIterator(const QoreListHashIterator& old) : QoreListIterator(old) {
   }

   DLLLOCAL AbstractQoreNode* getReferencedKeyValue(const char* key, ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;

      const QoreHashNode* h = checkHash(xsink);
      if (!h)
         return 0;
      return getReferencedKeyValueIntern(h, key, xsink);
   }

   DLLLOCAL QoreHashNode* getRow(ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;

      const QoreHashNode* h = checkHash(xsink);
      return h ? h->hashRefSelf() : 0;
   }

   DLLLOCAL QoreHashNode* getSlice(const QoreListNode* args, ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;

      const QoreHashNode* h = checkHash(xsink);
      if (!h)
         return 0;

      ReferenceHolder<QoreHashNode> rv(new QoreHashNode, xsink);

      ConstListIterator li(args);
      while (li.next()) {
         QoreStringValueHelper str(li.getValue(), QCS_UTF8, xsink);
         if (*xsink)
            return 0;
         const char* key = str->getBuffer();
         AbstractQoreNode* n = getReferencedKeyValueIntern(h, key, xsink);
         if (*xsink)
            return 0;
         rv->setKeyValue(key, n, xsink);
         // cannot have an exception here
         assert(!*xsink);
      }

      return rv.release();
   }

   DLLLOCAL virtual const char* getName() const {
      return "ListHashIterator";
   }
};

#endif // _QORE_QORELISTHASHITERATOR_H
