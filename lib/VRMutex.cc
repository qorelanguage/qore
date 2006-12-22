/*
  VRMutex.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/VRMutex.h>
#include <qore/Exception.h>
#include <qore/support.h>

#include <assert.h>

#ifdef DEBUG
inline void show_vl(class VLock *vl, class VLock *nvl)
{
   printd(5, "VRMutex::enter() vl=%08p, nvl=%08p vl->waiting_on=%08p (in nvl=%08p)\n", vl, nvl, vl ? vl->waiting_on : NULL, (nvl && vl) ? nvl->find(vl->waiting_on) : NULL);
}
#endif

VLNode::VLNode(class VRMutex *gate) 
{
   g = gate;
   next = NULL;
}

VLock::VLock()
{
   waiting_on = NULL;
   tid = 0;
   head = NULL;
   tail = NULL;
}

VLock::~VLock()
{
   del();
}

void VLock::add(class VRMutex *g)
{
   class VLNode *n = new VLNode(g);
   if (tail)
      tail->next = n;
   else
      head = n;
   tail = n;
}

void VLock::del()
{
   while (head)
   {
      tail = head->next;
      head->g->exit();
      delete head;
      head = tail;
   }
}

class VRMutex *VLock::find(class VRMutex *g)
{
   class VLNode *w = head;
   while (w)
   {
      if (w->g == g)
	 return g;
      w = w->next;
   }
   return NULL;
}

VRMutex::VRMutex()
{
   count = waiting = 0;
   tid = -1;
   vl = NULL;
}

void VRMutex::enter()
{
   //printd(5, "VRMutex::enter() %08p\n", this);

   int mtid = gettid();
   lock();
   while (tid != -1 && tid != mtid)
   {
      waiting++;
      printd(5, "VRMutex::enter() this=%08p about to block on VRMutex owned by TID %d\n", this, tid);
#ifdef DEBUG
      show_vl(vl, NULL);
#endif
      wait((LockedObject *)this);
      printd(5, "VRMutex::enter() this=%08p grabbed VRMutex\n", this);
      waiting--;
   }
   tid = mtid;
#ifdef DEBUG
   printd(5, "VRMutex::enter() this=%p count: %d->%d\n", this, count, count + 1);
#endif
   count++;
#ifdef DEBUG
   if (count == 1)
      printd(5, "VRMutex::enter() this=%08p grabbed lock (vl=%08p)\n", this, vl);
#endif
   unlock();
}

int VRMutex::enter(class VLock *nvl, class ExceptionSink *xsink)
{
   int mtid = gettid();
   lock();
   while (tid != -1 && tid != mtid)
   {
      // check for a deadlock - 
      // if the thread that's holding the lock is waiting on a lock
      // that we are holding, then there is a deadlock
      if (vl && vl->waiting_on && nvl->find(vl->waiting_on))
      {
	 xsink->raiseException("THREAD-DEADLOCK", "TIDs %d and %d have deadlocked trying to acquire the same resources", tid, mtid);
	 unlock();
	 return -1;
      }

      waiting++;
      nvl->waiting_on = this;
      nvl->tid = mtid;
      printd(5, "VRMutex::enter() this=%08p about to block on VRMutex owned by TID %d\n", this, tid);
#ifdef DEBUG
      show_vl(vl, nvl);
#endif
      wait((LockedObject *)this);
      printd(5, "VRMutex::enter() this=%08p grabbed VRMutex\n", this);
      nvl->waiting_on = NULL;
      waiting--;
   }
   tid = mtid;

#ifdef DEBUG
   printd(5, "VRMutex::enter() this=%p count: %d->%d\n", this, count, count + 1);
#endif

   count++;

   if (!vl)
      vl = nvl;

#ifdef DEBUG
   if (count == 1)
      printd(5, "VRMutex::enter() this=%08p grabbed lock (vl=%08p nvl=%08p)\n", this, vl, nvl);
#endif

   unlock();
   return 0;
}

int VRMutex::exit()
{
   assert(tid == gettid());

   //fprintf(stderr, "Gate::exit() %08p\n", this);
   lock();
   // if the lock is not locked, then return an error
   if (!count)
   {
      unlock();
      return -1;
   }
#ifdef DEBUG
   printd(5, "VRMutex::exit() this=%p count: %d->%d\n", this, count, count - 1);
#endif

   count--;
   // if this is the last thread from the group to exit the lock
   // then unlock the lock
   if (!count)
   {
#ifdef DEBUG
      printd(5, "VRMutex::exit() this=%p releasing lock (vl=%08p)\n", this, vl);
#endif
      tid = -1;
      vl = NULL;

      // wake up a sleeping thread if there are threads waiting 
      // on the lock
      if (waiting)
	 signal();
   }

   unlock();

   return 0;
}

