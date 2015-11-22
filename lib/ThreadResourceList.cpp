/*
  ThreadResourceList.cpp

  POSIX thread library for Qore

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

#include <qore/Qore.h>
#include <qore/intern/ThreadResourceList.h>
#include <qore/AbstractThreadResource.h>

Sequence ThreadResourceList::seq;

void ThreadResourceList::set(AbstractThreadResource* atr) {
   //printd(5, "TRL::set() this: %p atr: %p\n", this, atr);
   assert(trset.find(atr) == trset.end());

   atr->ref();
   trset.insert(atr);
}

bool ThreadResourceList::check(AbstractThreadResource* atr) const {
   //printd(5, "TRL::check(atr: %p)\n", atr);
   return trset.find(atr) != trset.end();
}

void ThreadResourceList::purge(ExceptionSink* xsink) {
   while (true) {
      trset_t::iterator i = trset.begin();
      if (i == trset.end())
	  break;

      AbstractThreadResource* atr = *i;
      //printd(5, "TRL::purge() this: %p cleaning up atr: %p\n", this, atr);
      // we have to remove the thread resource from the list before running cleanup in case of user thread resources
      // where the cleanup routine will cause the object to be deleted and therefore for remove_thread_resource() to
      // be called which can result in a crash
      trset.erase(i);

      atr->cleanup(xsink);
      atr->deref();
   }

   //printd(5, "TRL::purge() this: %p done\n", this);
}

void breakit() {}
int ThreadResourceList::remove(AbstractThreadResource* atr) {
   //printd(5, "TRL::remove() this: %p atr: %p\n", this, atr);
   breakit();

   trset_t::iterator i = trset.find(atr);
   if (i == trset.end())
      return -1;

   (*i)->deref();
   trset.erase(i);
   return 0;
}
