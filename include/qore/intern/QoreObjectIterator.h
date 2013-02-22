/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreHashIterator.h

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

#ifndef _QORE_QOREOBJECTITERATOR_H

#define _QORE_QOREOBJECTITERATOR_H

#include <qore/intern/QoreHashIterator.h>

// the c++ object
class QoreObjectIterator : public QoreHashIterator {
public:
   DLLLOCAL QoreObjectIterator(const QoreObject* o) : QoreHashIterator(o->getRuntimeMemberHash(0)) {
   }

   DLLLOCAL QoreObjectIterator() {
   }

   DLLLOCAL QoreObjectIterator(const QoreObjectIterator& old) : QoreHashIterator(old) {
   }

   DLLLOCAL virtual const char* getName() const { return "ObjectIterator"; }
};

// internal reverse iterator class implementation only for the getName() function - the iterators remain
// forwards and are used in the reverse sense by the Qore language class implementation below
class QoreObjectReverseIterator : public QoreObjectIterator {
public:
   DLLLOCAL QoreObjectReverseIterator(const QoreObject* o) : QoreObjectIterator(o) {
   }

   DLLLOCAL QoreObjectReverseIterator() {
   }

   DLLLOCAL QoreObjectReverseIterator(const QoreObjectReverseIterator& old) : QoreObjectIterator(old) {
   }

   DLLLOCAL virtual const char* getName() const {
      return "ObjectReverseIterator";
   }
};

#endif // _QORE_QOREOBJECTITERATOR_H
