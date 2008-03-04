/* 
 RWLock.cc
 
 Read-Write Lock object (default: prefer readers to allow recursively grabbing the read lock)
 prefer writers not yet tested/completely implemented (ex: should throw an exception if a 
 thread holding the read lock tries to recursively grab the read lock)
 
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
#include <qore/intern/RWLock.h>

#include <assert.h>

RWLock::RWLock(bool p) : readRequests(0), prefer_writers(p), num_readers(0)
{
}

#ifdef DEBUG
RWLock::~RWLock()
{
   assert(!tmap.size());
}
#endif

int RWLock::numReaders()
{
   return num_readers;
}

int RWLock::grabImpl(int mtid, class VLock *nvl, ExceptionSink *xsink, int timeout_ms)
{
   // check for errors
   if (tid == mtid)
   {
      xsink->raiseException("LOCK-ERROR", "TID %d tried to grab the write lock twice", tid);
      return -1;
   }
   while (tid >= 0 || (tid == Lock_Unlocked && num_readers))
   {
      ++waiting;
      int rc;
      // if the write lock is grabbed, send vl (only one thread owns the lock)
      if (tid >= 0)
	 rc = nvl->waitOn((AbstractSmartLock *)this, vl, xsink, timeout_ms);
      else  // otherwise the read lock is grabbed, so send vmap (many threads own the lock) 
	 rc = nvl->waitOn((AbstractSmartLock *)this, vmap, xsink, timeout_ms);
      --waiting;
      if (rc)
	 return -1;
   }
   if (tid == Lock_Deleted)
   {
      xsink->raiseException("LOCK-ERROR", "The %s object has been deleted in another thread", getName());
      return -1;
   }
   return 0;
}

void RWLock::signalAllImpl()
{
   if (waiting)
      asl_cond.broadcast();
   if (readRequests)
      read.broadcast();
}

void RWLock::signalImpl()
{
   if (prefer_writers)
   {
      if (waiting)
	 asl_cond.signal();
      else if (readRequests)
	 read.broadcast();
   }
   else
   {
      if (readRequests)
	 read.broadcast();
      else if (waiting)
	 asl_cond.signal();
   }
}

void RWLock::destructorImpl(ExceptionSink *xsink)
{
   if (num_readers)
      asl_cond.broadcast();

   num_readers = 0;
   
   // release all read locks, but do not remove from the thread resource list
   for (vlock_map_t::iterator i = vmap.begin(), e = vmap.end(); i != e; ++i)
      i->second->pop((AbstractSmartLock *)this);

   vmap.clear();
   tmap.clear();
}

// return value 0 = last read lock for this thread released
int RWLock::cleanup_read_lock_intern(tid_map_t::iterator i)
{
   if (!--(i->second))
   {
      vlock_map_t::iterator vi = vmap.find(i->first);
      // pop the lock from this thread's lock list
      vi->second->pop((AbstractSmartLock *)this);
      // do not remove from the thread resource list here
      // remove map entries for this thread
      tmap.erase(i);
      vmap.erase(vi);
      assert((tmap.size() && num_readers) || (!tmap.size() && !num_readers));
      return 0;
   }
   return -1;
}

// internal use only - releases read and write locks
int RWLock::releaseImpl()
{
   if (num_readers)
   {
      // signal writers if any are waiting
      if (!--num_readers && waiting)
	 asl_cond.signal();

      int mtid = gettid();
      // remove reader for this thread
      tid_map_t::iterator ti = tmap.find(mtid);
      assert(ti != tmap.end());

      //if (ti != tmap.end())
	 cleanup_read_lock_intern(ti);
      return -1;
   }
   else if (tid >= 0)
   {
      return 0;
   }
   return -1;
}

// thread exited holding the lock: remove whatever lock was locked
void RWLock::cleanupImpl()
{
   // if it was a read lock
   if (num_readers)
   {
      int mtid = gettid();
      // remove reader for this thread
      vlock_map_t::iterator vi = vmap.find(mtid);

      // vmap entry must be present for this thread
      assert(vi != vmap.end());

      // delete entry from the thread lock list
      vi->second->pop(this);
      // erase vlock map entry
      vmap.erase(vi);
      tid_map_t::iterator ti = tmap.find(mtid);

      // tmap entry must be present if we found a vmap entry
      assert(ti != tmap.end());

      // subtract number of times this thread held the read lock from num_readers
      num_readers -= ti->second;
      // wake up threads waiting on write lock, if any
      if (!num_readers && waiting)
	 asl_cond.signal();
      
      // erase thread map entry
      tmap.erase(ti);
      assert((tmap.size() && num_readers) || (!tmap.size() && !num_readers));
   }
   else if (tid >= 0)  // if it was the write lock
   {
      // this thread must own the lock
      assert(tid == gettid());
      // mark lock as unlocked
      tid = -1;

      // delete entry from the thread lock list
      vl->pop(this);
      
      vl = 0;
      // wake up sleeping thread(s)
      signalImpl();
   }
}

int RWLock::releaseImpl(ExceptionSink *xsink)
{
   int mtid = gettid();
   if (tid == Lock_Deleted)
   {
      xsink->raiseException("LOCK-ERROR", "The %s object has been deleted in another thread", getName());
      return -1;
   }
   if (tid == Lock_Unlocked)
   {
      xsink->raiseException("LOCK-ERROR", "TID %d called %s::writeUnlock() while not holding the write lock", mtid, getName());
      return -1;
   }
   if (tid != mtid)
   {
      // use getName() here so it can be safely inherited
      xsink->raiseException("LOCK-ERROR", "%s::writeUnlock() called by TID %d while the write lock is held by TID %d", getName(), mtid, tid);
      return -1;
   }
   return 0;
}

int RWLock::tryGrabImpl(int mtid, class VLock *nvl)
{
   if (tid != Lock_Unlocked || num_readers)
      return -1;

   return 0;
}

void RWLock::mark_read_lock_intern(int mtid, class VLock *nvl)
{
   ++num_readers;
   
   // add read lock to thread and vlock maps
   // (do not set vl, set in vmap instead)
   tid_map_t::iterator i = tmap.find(mtid);
   if (i == tmap.end())
   {
      // only set these values the first time the lock is acquired
      tmap[mtid] = 1;
      vmap[mtid] = nvl;
      // now register that we have grabbed this lock with the thread list
      nvl->push((AbstractSmartLock *)this);
      // register the thread resource
      set_thread_resource((AbstractThreadResource *)this);
   }
   else // increment lock count otherwise
      ++(i->second);
}

int RWLock::readLock(ExceptionSink *xsink, int timeout_ms)
{
   int mtid = gettid();
   class VLock *nvl = getVLock();
   SafeLocker sl(&asl_lock);

   if (tid == mtid)
   {
      xsink->raiseException("LOCK-ERROR", "TID %d called %s::readLock() while holding the write lock", mtid, getName());
      return -1;
   }

   if (tid >= 0)
   {
      do
      {
	 ++readRequests;
	 int rc;
	 rc = nvl->waitOn((AbstractSmartLock *)this, &read, vl, xsink, timeout_ms);
	 --readRequests;
	 if (rc)
	    return -1;
      } while (tid >= 0);

      if (tid == Lock_Deleted)
      {
	 xsink->raiseException("LOCK-ERROR", "The %s object has been deleted in another thread", getName());
	 return -1;
      }
      ++num_readers;

      // we know that this thread was not in the read lock because the write lock was grabbed before
      tmap[mtid] = 1;
      vmap[mtid] = nvl;
      // register the thread resource
      set_thread_resource((AbstractThreadResource *)this);
      nvl->push((AbstractSmartLock *)this);
      return 0;
   }

   if (tid == Lock_Deleted)
   {
      xsink->raiseException("LOCK-ERROR", "The %s object has been deleted in another thread", getName());
      return -1;
   }

   // read lock must be open   
   mark_read_lock_intern(mtid, nvl);
   
   return 0;
}

int RWLock::readUnlock(ExceptionSink *xsink)
{
   int mtid = gettid();
   AutoLocker al(&asl_lock);
   if (tid == mtid)
   {
      xsink->raiseException("LOCK-ERROR", "TID %d called %s::readUnlock() while holding the write lock", mtid, getName());
      return -1;
   }

   if (tid == Lock_Deleted)
   {
      xsink->raiseException("LOCK-ERROR", "The %s object has been deleted in another thread", getName());
      return -1;
   }

   tid_map_t::iterator i = tmap.find(mtid);
   if (i == tmap.end())
   {
      xsink->raiseException("LOCK-ERROR", "TID %d called %s::readUnlock() while not holding the read lock", mtid, getName());
      return -1;
   }
   
   // signal writers if any are waiting
   if (!--num_readers && waiting)
      asl_cond.signal();

   // if this thread has released it's last read lock, then remove the thread resource
   if (!cleanup_read_lock_intern(i))
      remove_thread_resource((AbstractThreadResource *)this);
   return 0;
}

int RWLock::tryReadLock()
{
   AutoLocker al(&asl_lock);
   if (tid != Lock_Unlocked)
      return -1;

   mark_read_lock_intern(gettid(), getVLock());

   return 0;
}


// while holding the write lock changes it to a read lock
// and wakes up any blocked readers
// not sure if this function should be allowed if the object
// prefers writers
/*
void RWLock::writeToRead()
{
   AutoLocker al(&asl_lock);
   XXX
   if (readRequests)
      read.broadcast();
}
*/

