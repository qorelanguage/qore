/* 
 RWLock.cc
 
 Read-Write Lock object (default: prefer readers)
 
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
#include <qore/RWLock.h>

#include <assert.h>

RWLock::RWLock(bool p)
{
   prefer_writers = p;
   readRequests = 0;
}

#ifdef DEBUG
RWLock::~RWLock()
{
   assert(!tmap.size());
}
#endif

int RWLock::numReaders()
{
   AutoLocker al(&asl_lock);
   return tmap.size();
}

int RWLock::grabImpl(int mtid, class VLock *nvl, class ExceptionSink *xsink)
{
   //printd(5, "RWLock::grabImpl() mtid=%d nvl=%08p tid=%d\n", mtid, nvl, xsink); 
   // check for errors
   if (tid == mtid)
   {
      xsink->raiseException("LOCK-ERROR", "TID %d tried to grab the write lock twice", tid);
      return -1;
   }
   while (tid >= 0 || (tid == -1 && vmap.size()))
   {
      ++waiting;
      int rc;
      if (tid >= 0)
	 rc = nvl->waitOn((AbstractSmartLock *)this, vl, mtid, xsink);
      else 
	 rc = nvl->waitOn((AbstractSmartLock *)this, vmap, mtid, xsink);
      --waiting;
      if (rc)
	 return -1;
   }
   if (tid == -2)
   {
      xsink->raiseException("LOCK-ERROR", "The %s object has been deleted in another thread", getName());
      return -1;
   }
   return 0;
}

int RWLock::grabImpl(int mtid, int timeout_ms, class VLock *nvl, class ExceptionSink *xsink)
{
   // check for errors
   if (tid == mtid)
   {
      xsink->raiseException("LOCK-ERROR", "TID %d tried to grab the write lock twice", tid);
      return -1;
   }
   while (tid >= 0 || (tid == -1 && vmap.size()))
   {
      ++waiting;
      int rc;
      if (tid >= 0)
	 rc = nvl->waitOn((AbstractSmartLock *)this, vl, mtid, xsink);
      else 
	 rc = nvl->waitOn((AbstractSmartLock *)this, vmap, mtid, xsink);
      --waiting;
      if (rc)
	 return -1;
   }
   if (tid == -2)
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

void RWLock::destructorImpl(class ExceptionSink *xsink)
{
   if (vmap.size())
      asl_cond.broadcast();
   // release all read locks
   for (vlock_map_t::iterator i = vmap.begin(), e = vmap.end(); i != e; ++i)
   {
      i->second->pop((AbstractSmartLock *)this);
      trlist.remove((AbstractSmartLock *)this, i->first);
   }
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
      // do not remove from the trlist (thread resource list) here
      // remove map entries for this thread
      tmap.erase(i);
      vmap.erase(vi);
      // signal writers if any are waiting
      if (!tmap.size() && waiting)
	 asl_cond.signal();
      return 0;
   }
   return -1;
}

// internal use only - releases read and write locks
int RWLock::releaseImpl()
{
   if (tmap.size())
   {
      int mtid = gettid();
      // remove reader for this thread
      tid_map_t::iterator ti = tmap.find(mtid);
      if (ti != tmap.end())
	 cleanup_read_lock_intern(ti);
      return -1;
   }
   else if (tid >= 0)
   {
      return 0;
   }
   return -1;
}

// remove whatever lock was locked
void RWLock::cleanup()
{
   if (tmap.size())
   {
      int mtid = gettid();
      // remove reader for this thread
      vlock_map_t::iterator vi = vmap.find(mtid);
      if (vi == vmap.end())
	 return;
      vi->second->pop(this);
      vmap.erase(vi);
      tid_map_t::iterator ti = tmap.find(mtid);
      tmap.erase(ti);
      if (!tmap.size() && waiting)
	 asl_cond.signal();
   }
   else if (tid >= 0)
   {
      vl->pop(this);
      tid = -1;
      vl = NULL;
      signalImpl();
   }
}

int RWLock::releaseImpl(class ExceptionSink *xsink)
{
   int mtid = gettid();
   if (tid == -2)
   {
      xsink->raiseException("LOCK-ERROR", "The %s object has been deleted in another thread", getName());
      return -1;
   }
   if (tid == -1)
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
   if (tid != -1 || tmap.size())
      return -1;

   return 0;
}

int RWLock::readLock(class ExceptionSink *xsink)
{
   int mtid = gettid();
   class VLock *nvl = getVLock();
   AutoLocker al(&asl_lock);

   if (tid == mtid)
   {
      xsink->raiseException("LOCK-ERROR", "TID %d called %s::readLock() while holding the write lock", tid, getName());
      return -1;
   }

   while (tid >= 0)
   {
      ++readRequests;
      int rc = nvl->waitOn((AbstractSmartLock *)this, &read, vl, mtid, xsink);
      --readRequests;
      if (rc)
	 return -1;
   }

   if (tid == -2)
   {
      xsink->raiseException("LOCK-ERROR", "The %s object has been deleted in another thread", getName());
      return -1;
   }

   mark_read_lock_intern(mtid, nvl);

   return 0;
}

int RWLock::readLock(int timeout_ms, class ExceptionSink *xsink)
{
   int mtid = gettid();
   class VLock *nvl = getVLock();
   AutoLocker al(&asl_lock);

   if (tid == mtid)
   {
      xsink->raiseException("LOCK-ERROR", "TID %d called %s::readLock() while holding the write lock", mtid, getName());
      return -1;
   }

   while (tid >= 0)
   {
      ++readRequests;
      int rc = nvl->waitOn((AbstractSmartLock *)this, &read, vl, mtid, xsink);
      --readRequests;
      if (rc)
	 return -1;
   }

   if (tid == -2)
   {
      xsink->raiseException("LOCK-ERROR", "The %s object has been deleted in another thread", getName());
      return -1;
   }

   mark_read_lock_intern(mtid, nvl);
   
   return 0;
}

void RWLock::mark_read_lock_intern(int mtid, class VLock *nvl)
{
   // add read lock to thread and vlock maps
   // (do not set vl, set in vmap instead)
   tid_map_t::iterator i = tmap.find(mtid);
   if (i == tmap.end())
   {
      tmap[mtid] = 1;
      vmap[mtid] = nvl;
      // now register that we have grabbed this lock with the thread list
      nvl->push((AbstractSmartLock *)this);
      // register the thread resource
      trlist.set((AbstractSmartLock *)this, (qtrdest_t)abstract_smart_lock_cleanup);
   }
   else
      ++(i->second);
}

int RWLock::readUnlock(class ExceptionSink *xsink)
{
   int mtid = gettid();
   AutoLocker al(&asl_lock);
   if (tid == mtid)
   {
      xsink->raiseException("LOCK-ERROR", "TID %d called %s::readUnlock() while holding the write lock", mtid, getName());
      return -1;
   }

   if (tid == -2)
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

   // if this thread has released it's last read lock, then remove the thread resource
   if (!cleanup_read_lock_intern(i))
      trlist.remove((AbstractSmartLock *)this, mtid);
   return 0;
}

int RWLock::tryReadLock()
{
   AutoLocker al(&asl_lock);
   if (tid != -1)
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

