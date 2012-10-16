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

#ifndef _QORE_QOREOBJECTITERATOR_H

#define _QORE_QOREOBJECTITERATOR_H

#include <qore/intern/QoreHashIterator.h>

// the c++ object
class QoreObjectIterator : public QoreHashIterator {
public:
   DLLLOCAL QoreObjectIterator(const QoreObject* o, bool gvh = false) : QoreHashIterator(o->getRuntimeMemberHash(0), gvh) {
   }

   DLLLOCAL QoreObjectIterator() {
   }

   DLLLOCAL QoreObjectIterator(const QoreObjectIterator& old) : QoreHashIterator(old) {
   }

   DLLLOCAL virtual const char* getName() const { return "ObjectIterator"; }
};

#endif // _QORE_QOREOBJECTITERATOR_H
