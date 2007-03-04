/*
  VRMutex.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
#include <qore/VRMutex.h>

#include <assert.h>

VRMutex::VRMutex()
{
   count = waiting = 0;
   tid = -1;
   vl = NULL;
}

/*
void VRMutex::enter()
{
   //printd(5, "VRMutex::enter() %08p\n", this);

   int mtid = gettid();
   AutoLocker al(&asl_lock);
   while (tid != -1 && tid != mtid)
   {
      waiting++;
      printd(5, "VRMutex::enter() this=%08p about to block on VRMutex owned by TID %d\n", this, tid);
#ifdef DEBUG
      vl->show(NULL);
#endif
      wait((LockedObject *)this);
      printd(5, "VRMutex::enter() this=%08p grabbed VRMutex\n", this);
      waiting--;
   }
   tid = mtid;
#ifdef DEBUG
   //printd(5, "VRMutex::enter() this=%p count: %d->%d\n", this, count, count + 1);
#endif
   count++;
#ifdef DEBUG
   //if (count == 1)
   //printd(5, "VRMutex::enter() this=%08p grabbed lock (vl=%08p)\n", this, vl);
#endif
}
*/

int VRMutex::enter(class ExceptionSink *xsink)
{
   class VLock *nvl = getVLock();
   int mtid = gettid();

   // grabs the lock and releases it when the function is exited
   AutoLocker al(&asl_lock);

   while (tid != -1 && tid != mtid)
   {
      assert(nvl);
      waiting++;
      int rc = nvl->waitOn((AbstractSmartLock *)this, vl, mtid, xsink);
      waiting--;
      // if rc is non-zero there was a deadlock
      if (rc)
	 return -1;
   }
   // The list must always be the same!
   assert((mtid == tid  && vl == nvl) || (tid == -1 && !vl));

   if (tid == -1)
   {
      tid = mtid;
      vl = nvl;
#ifdef DEBUG
      if (count == 1)
	 printd(5, "VRMutex::enter() this=%08p grabbed lock (vl=%08p nvl=%08p)\n", this, vl, nvl);
#endif
      nvl->push(this);
   }
   
   printd(5, "VRMutex::enter() this=%p count: %d->%d\n", this, count, count + 1);

   count++;

   return 0;
}

int VRMutex::exit()
{
   assert(tid == gettid());

   //fprintf(stderr, "Gate::exit() %08p\n", this);
   // grabs the lock and releases it when the function is exited
   AutoLocker al(&asl_lock);
   // if the lock is not locked, then return an error
   if (!count)
      return -1;
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
      AbstractSmartLock *g = vl->pop();
      assert(g == this);
#else
      vl->pop();
#endif
      
      tid = -1;
      vl = NULL;

      // wake up a sleeping thread if there are threads waiting 
      // on the lock
      if (waiting)
	 asl_cond.signal();
   }

   return 0;
}

int VRMutex::release()
{
   assert(tid == gettid());
   
   //fprintf(stderr, "Gate::exit() %08p\n", this);
   // grabs the lock and releases it when the function is exited
   AutoLocker al(&asl_lock);
   // if the lock is not locked, then return an error
   if (!count)
      return -1;
#ifdef DEBUG
   printd(5, "VRMutex::release() this=%p count: %d->%d\n", this, count, count - 1);
#endif
   
   count--;
   // if this is the last thread from the group to exit the lock
   // then unlock the lock
   if (!count)
   {
      printd(5, "VRMutex::exit() this=%p releasing lock (vl=%08p)\n", this, vl);
      
      tid = -1;
      vl = NULL;
      
      // wake up a sleeping thread if there are threads waiting 
      // on the lock
      if (waiting)
	 asl_cond.signal();
   }
   
   return 0;
}

