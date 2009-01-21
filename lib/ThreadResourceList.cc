/*
  ThreadResourceList.cc

  POSIX thread library for Qore

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

class ThreadResourceNode
{
   public:
      AbstractThreadResource *atr;
      class ThreadResourceNode *next, *prev;
      
      DLLLOCAL ThreadResourceNode(AbstractThreadResource *a);
      DLLLOCAL void call(ExceptionSink *xsink);
};

ThreadResourceNode::ThreadResourceNode(AbstractThreadResource *a) : atr(a), prev(0)
{
}

void ThreadResourceNode::call(ExceptionSink *xsink)
{
   atr->cleanup(xsink);
   atr->deref();
}

ThreadResourceList::ThreadResourceList()
{
   head = 0;
}

ThreadResourceList::~ThreadResourceList()
{
   assert(!head);
}

class ThreadResourceNode *ThreadResourceList::find(AbstractThreadResource *atr)
{
   class ThreadResourceNode *w = head;
   while (w)
   {
      if (w->atr == atr)
	 return w;
      w = w->next;
   }
   return 0;
}

/*
inline class ThreadResourceNode *ThreadResourceList::find(AbstractThreadResource *atr, int tid)
{
   class ThreadResourceNode *w = head;
   while (w)
   {
      if (w->atr == atr && w->tid == tid)
	 return w;
      w = w->next;
   }
   return 0;
}
*/

void ThreadResourceList::setIntern(class ThreadResourceNode *n)
{
   n->next = head;
   if (head)
      head->prev = n;
   head = n;
   printd(5, "TRL::setIntern(atr=%08p) head=%08p\n", n->atr, head);
}

void ThreadResourceList::set(AbstractThreadResource *atr)
{
   //printd(5, "TRL::set(atr=%08p, func=%08p, tid=%d)\n", atr, func, gettid());
   class ThreadResourceNode *n = new ThreadResourceNode(atr);

   assert(!find(atr));
   atr->ref();
   setIntern(n);
   //printd(5, "TRL::set(atr=%08p, func=%08p, tid=%d) n=%08p, head=%08p, head->next=%08p\n", atr, func, gettid(), n, head, head->next);
}

int ThreadResourceList::setOnce(AbstractThreadResource *atr)
{
   int rc = 0;

   if (find(atr))
      rc = -1;
   else
   {
      atr->ref();
      setIntern(new ThreadResourceNode(atr));
   }
   return rc;
}

void ThreadResourceList::removeIntern(class ThreadResourceNode *w)
{
   //printd(5, "removeIntern(%08p) starting (head=%08p)\n", w, head);
   if (w->prev)
      w->prev->next = w->next;
   else
      head = w->next;
   if (w->next)
      w->next->prev = w->prev;
   //printd(5, "removeIntern(%08p) done (head=%08p)\n", w, head);
}

void ThreadResourceList::purge(ExceptionSink *xsink)
{
   class ThreadResourceNode *w = head;
   while (w)
   {
      w->call(xsink);
      class ThreadResourceNode *n = w->next;
      delete w;
      w = n;
   }
   head = 0;
   //printd(5, "TRL::purge() done\n");
}

int ThreadResourceList::remove(AbstractThreadResource *atr)
{
   //printd(0, "TRL::remove(atr=%08p)\n", atr);

   class ThreadResourceNode *w = find(atr);
   if (!w)
      return -1;

   removeIntern(w);
   w->atr->deref();
   delete w;
   return 0;
}

/*
// there must be only one of these
void ThreadResourceList::remove(AbstractThreadResource *atr, int tid)
{
   AutoLocker al((QoreThreadLock *)this);

   //printd(0, "TRL::remove(atr=%08p, tid=%d) this=%08p, head=%08p\n", atr, tid, this, head);
   class ThreadResourceNode *w = head;
   while (w)
   {
      //printd(0, "TRL::remove(atr=%08p, tid=%d) this=%08p, w=%08p atr=%08p, tid=%d\n", atr, tid, this, w, w->atr, w->tid);
      if (w->atr == atr && w->tid == tid)
      {
	 removeIntern(w);
	 delete w;
	 return;
      }
      w = w->next;
   }
   //printd(0, "TRL::remove(atr=%08p, tid=%d) this=%08p ABORT\n", atr, tid, this);
   // if this function fails, there is a bug in the thread resource handling
   assert(false);
}
*/
