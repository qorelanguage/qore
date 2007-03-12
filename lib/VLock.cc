/*
 VLock.cc
 
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
#include <qore/VLock.h>

#include <assert.h>

LockedObject VLock::global_lock;

AutoVLock::AutoVLock()
{
   //printd(5, "AutoVLock::AutoVLock() this=%08p\n", this);
}

AutoVLock::~AutoVLock()
{
   //printd(5, "AutoVLock::~AutoVLock() this=%08p size=%d\n", this, size());
   del();
}

void AutoVLock::del()
{
   //printd(5, "AutoVLock::del() this=%08p size=%d\n", this, size());

   abstract_lock_list_t::iterator i;
   while ((i = begin()) != end())
   {
      //printd(5, "AutoVLock::del() this=%08p releasing=%08p\n", this, *i);
      (*i)->release();
      erase(i);
   }
}

void AutoVLock::push(class AbstractSmartLock *p)
{
   //printd(5, "AutoVLock::push(%08p) this=%08p\n", p, this);
   push_back(p);
}

int VLock::waitOn(AbstractSmartLock *asl, VLock *vl, class ExceptionSink *xsink, int timeout_ms)
{
   SafeLocker al(&global_lock);
   AbstractSmartLock *vl_wait = vl->waiting_on;
   if (vl_wait && find(vl_wait))
   {
      al.unlock();
      // NOTE: we throw an exception here anyway as a deadlock is a programming mistake and therefore should be visible to the programmer
      // (even if it really wouldn't technically deadlock at this point due to the timeout)
      if (timeout_ms)
	 xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d would deadlock on the same resources; this represents a programming error so even though a %s method was called with a timeout and therefore would not technically deadlock at this point, this exception is thrown anyway.", vl->tid, tid, asl->getName());
      else
	 xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d have deadlocked trying to acquire the same resources", vl->tid, tid);
      return -1;
   }
   waiting_on = asl;
   al.unlock();
   
#ifdef DEBUG
   //show(vl);
#endif
   
   //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p about to block on VRMutex owned by TID %d\n", this, asl, vl ? vl->tid : -1);
   int rc = asl->self_wait(timeout_ms);
   //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p regrabbed lock\n", this, asl);
   waiting_on = NULL;
   
   return rc;
}

int VLock::waitOn(AbstractSmartLock *asl, QoreCondition *cond, VLock *vl, class ExceptionSink *xsink, int timeout_ms)
{
   SafeLocker al(&global_lock);
   AbstractSmartLock *vl_wait = vl->waiting_on;
   //printd(5, "VLock::waitOn(asl=%08p) vl_wait=%08p other_tid=%d\n", asl, vl_wait, vl->tid);
   if (vl_wait && find(vl_wait))
   {
      al.unlock();
      // NOTE: we throw an exception here anyway as a deadlock is a programming mistake and therefore should be visible to the programmer
      // (even if it really wouldn't technically deadlock at this point due to the timeout)
      if (timeout_ms)
	 xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d would deadlock on the same resources; this represents a programming error so even though a %s method was called with a timeout and therefore would not technically deadlock at this point, this exception is thrown anyway.", vl->tid, tid, asl->getName());
      else
	 xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d have deadlocked trying to acquire the same resources", vl->tid, tid);
      return -1;
   }
   waiting_on = asl;
   al.unlock();

   //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p about to block on VRMutex owned by TID %d\n", this, asl, vl ? vl->tid : -1);
   int rc = asl->self_wait(cond, timeout_ms);
   //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p regrabbed lock\n", this, asl);
   waiting_on = NULL;
   
   return rc;
}

int VLock::waitOn(AbstractSmartLock *asl, vlock_map_t &vmap, class ExceptionSink *xsink, int timeout_ms)
{
   SafeLocker al(&global_lock);
   for (vlock_map_t::iterator i = vmap.begin(), e = vmap.end(); i != e; ++i)
   {
      AbstractSmartLock *vl_wait = i->second->waiting_on;
      //printd(5, "VLock::waitOn(asl=%08p, vmap size=%d) vl_wait=%08p other_tid=%d\n", asl, vmap.size(), vl_wait, i->second->tid);
      if (vl_wait && find(vl_wait))
      {
	 al.unlock();
	 // NOTE: we throw an exception here anyway as a deadlock is a programming mistake and therefore should be visible to the programmer
	 // (even if it really wouldn't technically deadlock at this point due to the timeout)
	 if (timeout_ms)
	    xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d would deadlock on the same resources; this represents a programming error so even though a %s method was called with a timeout and therefore would not technically deadlock at this point, this exception is thrown anyway.", i->second->tid, tid, asl->getName());
	 else
	    xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d have deadlocked trying to acquire the same resources", i->second->tid, tid);
	 return -1;
      }
   }
   waiting_on = asl;
   al.unlock();

   //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p about to block on VRMutex owned by TID %d\n", this, asl, vl ? vl->tid : -1);
   int rc = asl->self_wait(timeout_ms);
   //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p regrabbed lock\n", this, asl);
   waiting_on = NULL;
   
   return rc;
}

#ifdef DEBUG
void VLock::show(class VLock *vl) const
{
   //printd(0, "VLock::show() this=%08p, vl=%08p vl->waiting_on=%08p (in this=%08p)\n", this, vl, vl ? vl->waiting_on : NULL, vl ? find(vl->waiting_on) : NULL);
}
#endif

VLock::VLock(int n_tid) : waiting_on(0), tid(n_tid)
{
}

VLock::~VLock()
{
   //printd(5, "VLock::~VLock() this=%08p\n", this);
   assert(begin() == end());
/*
   abstract_lock_list_t::iterator i;
   while ((i = begin()) != end())
   {
      (*i)->release();
      erase(i);
   }
*/
}

void VLock::push(AbstractSmartLock *g)
{
   //printd(5, "VLock::push() this=%08p asl=%08p size=%d\n", this, g, size());
   push_back(g);
}

int VLock::pop(AbstractSmartLock *g)
{
   assert(begin() != end());

   if (g == back())
   {
      pop_back();
      return 0;
   }

   abstract_lock_list_t::iterator i = end();
   --i;
   --i;
   while (*i != g) 
      --i;

   erase(i);
   return -1;
}

/*
void VLock::delToMark(AbstractSmartLock *mark)
{
   //printd(5, "VLock::delToMark() mark=%08p size=%d\n", mark, size());
   abstract_lock_list_t::iterator i = end();
   if (i == begin())
      return;

   do {
      --i;
      if (*i == mark)
	 break;
      if ((*i)->auto_delete)
      {
	 abstract_lock_list_t::iterator j = i + 1;
	 //printd(5, "VLock::delToMark() releasing %08p (%d)\n", *i, begin() == end());
	 (*i)->release();
	 erase(i);
	 i = j;
      }
   } while (end() != begin());
   //printd(5, "VLock::delToMark() begin=%08p\n", begin() != end() ? *(begin()) : NULL);

   //abstract_lock_list_t::reverse_iterator i;
   //while (rbegin() != rend() && *(i = rbegin()) != mark)
   //{
   //   (*i)->release();
   //   pop_back();
   //}
   //printd(5, "VLock::delToMark() rbegin=%08p\n", rbegin() != rend() ? *(rbegin()) : NULL);
}
*/

class AbstractSmartLock *VLock::find(class AbstractSmartLock *g) const
{
   for (abstract_lock_list_t::const_iterator i = begin(), e = end(); i != e; ++i)
      if (*i == g)
	 return g;
   return NULL;
}
