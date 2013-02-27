/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ThreadResourceList.h

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

#ifndef _QORE_THREADRESOURCELIST_H

#define _QORE_THREADRESOURCELIST_H

#include <qore/intern/Sequence.h>

#include <set>

typedef std::set<AbstractThreadResource*> trset_t;

class ThreadResourceList {
private:
   static Sequence seq;
   trset_t trset;

public:
   ThreadResourceList* prev;

   DLLLOCAL ThreadResourceList(ThreadResourceList* p = 0) : prev(p) {
   }

   DLLLOCAL ~ThreadResourceList() {
      assert(trset.empty());
   }

   DLLLOCAL void set(AbstractThreadResource* atr);

   DLLLOCAL bool check(AbstractThreadResource* atr) const;

   // returns 0 if removed, -1 if not found
   DLLLOCAL int remove(AbstractThreadResource* atr);

   DLLLOCAL void purge(ExceptionSink* xsink);

   DLLLOCAL bool empty() const {
      return trset.empty();
   }
};

#endif
