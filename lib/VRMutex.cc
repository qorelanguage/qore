/*
  VRMutex.cc

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
#include <qore/intern/VRMutex.h>

#include <assert.h>

VRMutex::VRMutex() : count(0)
{
}

int VRMutex::enter(ExceptionSink *xsink)
{
   int mtid = gettid();
   VLock *nvl = getVLock();
   AutoLocker al(&asl_lock);
   int rc = VRMutex::grabImpl(mtid, nvl, xsink);
   if (!rc)
      mark_and_push(mtid, nvl);
   return rc;
}

int VRMutex::exit()
{
   AutoLocker al(&asl_lock);
   int rc = VRMutex::releaseImpl();
   if (!rc)
      release_and_signal();
   return rc;
}

void VRMutex::cleanupImpl()
{
   if (tid == gettid()) {
      release_and_signal();
      count = 0;
   }
}

int VRMutex::grabImpl(int mtid, class VLock *nvl, ExceptionSink *xsink, int timeout_ms)
{
   if (tid != mtid) {
      while (tid != Lock_Unlocked) {
	 if (tid == Lock_Deleted) {
	    xsink->raiseException("LOCK-ERROR", "TID %d cannot execute %s::enter() because the object has been deleted in another thread", mtid, getName());
	    return -1;
	 }
	 
	 ++waiting;
	 int rc = nvl->waitOn((AbstractSmartLock *)this, vl, xsink, timeout_ms);
	 --waiting;
	 // if rc is non-zero there was a timeout or deadlock
	 if (rc)
	    return -1;
      }
      // the thread lock list must always be the same if the lock was grabbed
      assert((mtid == tid  && vl == nvl) || (tid == Lock_Unlocked && !vl));
   }
   printd(5, "VRMutex::enter() this=%p count: %d->%d\n", this, count, count + 1);

   return count++;
}

int VRMutex::tryGrabImpl(int mtid, class VLock *nvl)
{
   if (tid != mtid) {
      if (tid != Lock_Unlocked)
	 return -1;

      // the thread lock list must always be the same if the lock was grabbed
      assert((mtid == tid  && vl == nvl) || (tid == Lock_Unlocked && !vl));
   }

   printd(5, "VRMutex::enter() this=%p count: %d->%d\n", this, count, count + 1);

   return count++;
}

// internal use only
int VRMutex::releaseImpl()
{
   assert(tid == gettid());
   printd(5, "VRMutex::exit() this=%p count: %d->%d\n", this, count, count - 1);

   --count;
   // if this is the last thread from the group to exit the lock, then return 0
   return count ? -1 : 0;
}

int VRMutex::releaseImpl(ExceptionSink *xsink)
{
   int mtid = gettid();
   if (tid == Lock_Unlocked) {
      // use getName() here so it can be safely inherited
      xsink->raiseException("LOCK-ERROR", "TID %d called %s::exit() without acquiring the lock", mtid, getName());
      return -1;
   }
   if (tid == Lock_Deleted) {
      xsink->raiseException("LOCK-ERROR", "TID %d cannot execute %s::exit() because the object has been deleted in another thread", mtid, getName());
      return -1;
   }
   if (tid != mtid) {
      // use getName() here so it can be safely inherited
      xsink->raiseException("LOCK-ERROR", "TID %d called %s::exit() while the lock is held by TID %d", mtid, getName(), tid);
      return -1;      
   }
   // count must be > 0 because tid > 0
   assert(count);

   printd(5, "VRMutex::exit() this=%p count: %d->%d\n", this, count, count - 1);

   --count;
   // if this is the last thread from the group to exit the lock, then return 0
   return count ? -1 : 0;
}
