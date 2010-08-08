/*
  ThreadResourceList.cpp

  POSIX thread library for Qore

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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

#include <qore/Qore.h>
#include <qore/intern/ThreadResourceList.h>
#include <qore/AbstractThreadResource.h>

Sequence ThreadResourceList::seq;

void ThreadResourceList::set(AbstractThreadResource *atr) {
   //printd(5, "TRL::set(atr=%p, func=%p, tid=%d)\n", atr, func, gettid());
   assert(trset.find(atr) == trset.end());

   atr->ref();
   trset.insert(atr);
}

int ThreadResourceList::setOnce(AbstractThreadResource *atr) {
   if (trset.find(atr) != trset.end())
      return -1;

   atr->ref();
   trset.insert(atr);
   return 0;
}

void ThreadResourceList::purge(ExceptionSink *xsink) {
   for (trset_t::iterator i = trset.begin(), e = trset.end(); i != e; ++i) {
      (*i)->cleanup(xsink);
      (*i)->deref();
   }
   trset.clear();

   for (trmap_t::iterator i = trmap.begin(), e = trmap.end(); i != e; ++i) {
      i->second->cleanup(xsink);
      i->second->deref();
   }
   trmap.clear();

   //printd(5, "TRL::purge() done\n");
}

int ThreadResourceList::remove(AbstractThreadResource *atr) {
   //printd(0, "TRL::remove(atr=%p)\n", atr);

   trset_t::iterator i = trset.find(atr);
   if (i == trset.end())
      return -1;

   (*i)->deref();
   trset.erase(i);
   return 0;
}
