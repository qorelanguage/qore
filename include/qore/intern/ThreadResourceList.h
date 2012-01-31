/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ThreadResourceList.h

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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
#include <map>

typedef std::set<AbstractThreadResource *> trset_t;
typedef std::map<q_trid_t, AbstractThreadResource *> trmap_t;

class ThreadResourceList {
private:
   static Sequence seq;
   trset_t trset;
   trmap_t trmap;

public:
   ThreadResourceList* prev;

   DLLLOCAL ThreadResourceList(ThreadResourceList* p = 0) : prev(p) {
   }

   DLLLOCAL ~ThreadResourceList() {
      assert(trset.empty());
   }

   DLLLOCAL void set(q_trid_t trid, AbstractThreadResource *atr) {
      assert(trmap.find(trid) == trmap.end());

      atr->ref();
      trmap[trid] = atr;
   }

   DLLLOCAL void set(AbstractThreadResource *atr);

   // returns true if already set, false if not
   DLLLOCAL bool check(q_trid_t trid) {
      return trmap.find(trid) != trmap.end();
   }

   //returns 0 if not already set, -1 if already set
   DLLLOCAL int setOnce(AbstractThreadResource *atr);

   // returns 0 if removed, -1 if not found
   DLLLOCAL int remove(AbstractThreadResource *atr);

   // returns 0 if removed, -1 if not found
   DLLLOCAL int remove_id(q_trid_t trid) {
      trmap_t::iterator i = trmap.find(trid);
      if (i == trmap.end())
         return -1;

      //printd(0, "TRL::remove(trid=%d, atr=%p)\n", trid, i->second);

      i->second->deref();
      trmap.erase(i);
      return 0;
   }

   DLLLOCAL void purge(ExceptionSink *xsink);

   DLLLOCAL static q_trid_t get_resource_id() {
      return (q_trid_t)seq.next();
   }
};



#endif
