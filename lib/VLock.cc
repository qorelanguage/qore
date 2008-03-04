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
#include <qore/AutoVLock.h>

#include <assert.h>

struct qore_avl_private {
      abstract_lock_list_t l;
};

AutoVLock::AutoVLock() : counter(0), priv(0)
{
   //printd(5, "AutoVLock::AutoVLock() this=%08p\n", this);
}

AutoVLock::~AutoVLock()
{
   //printd(5, "AutoVLock::~AutoVLock() this=%08p size=%d\n", this, size());
   del();
   delete priv;
}

void AutoVLock::del()
{
   //printd(5, "AutoVLock::del() this=%08p size=%d\n", this, size());

   if (priv) {
      abstract_lock_list_t::iterator i;
      while ((i = priv->l.begin()) != priv->l.end()) {
	 //printd(5, "AutoVLock::del() this=%08p releasing=%08p\n", this, *i);
	 (*i)->release();
	 priv->l.erase(i);
      }
   }
   while (counter)
      fp[--counter]->release();
}

void AutoVLock::push(class AbstractSmartLock *p)
{
   //printd(5, "AutoVLock::push(%08p) this=%08p\n", p, this);
   if (counter == QORE_AVL_INTERN) {
      if (!priv)
	 priv = new qore_avl_private;
      priv->l.push_back(p);
      return;
   }
   fp[counter++] = p;
}

int VLock::waitOn(AbstractSmartLock *asl, VLock *vl, ExceptionSink *xsink, int timeout_ms)
{
   waiting_on = asl;
   
   int rc = 0;
   AbstractSmartLock *vl_wait = vl->waiting_on;
   //printd(5, "VLock::waitOn(asl=%08p) vl_wait=%08p other_tid=%d\n", asl, vl_wait, vl->tid);
   if (vl_wait && find(vl_wait))
   {
      // NOTE: we throw an exception here anyway as a deadlock is a programming mistake and therefore should be visible to the programmer
      // (even if it really wouldn't technically deadlock at this point due to the timeout)
      if (timeout_ms)
	 xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d would deadlock on the same resources; this represents a programming error so even though a %s method was called with a timeout and therefore would not technically deadlock at this point, this exception is thrown anyway.", vl->tid, tid, asl->getName());
      else
	 xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d have deadlocked trying to acquire the same resources", vl->tid, tid);
      rc = -1;
   }

   if (!rc)
   {
      //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p about to block on VRMutex owned by TID %d\n", this, asl, vl ? vl->tid : -1);
      rc = asl->self_wait(timeout_ms);
      //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p regrabbed lock\n", this, asl);
   }
   
   waiting_on = 0;
   
   return rc;
}

int VLock::waitOn(AbstractSmartLock *asl, QoreCondition *cond, VLock *vl, ExceptionSink *xsink, int timeout_ms)
{
   waiting_on = asl;

   int rc = 0;
   AbstractSmartLock *vl_wait = vl->waiting_on;
   //printd(5, "VLock::waitOn(asl=%08p) vl_wait=%08p other_tid=%d\n", asl, vl_wait, vl->tid);
   if (vl_wait && find(vl_wait))
   {
      // NOTE: we throw an exception here anyway as a deadlock is a programming mistake and therefore should be visible to the programmer
      // (even if it really wouldn't technically deadlock at this point due to the timeout)
      if (timeout_ms)
	 xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d would deadlock on the same resources; this represents a programming error so even though a %s method was called with a timeout and therefore would not technically deadlock at this point, this exception is thrown anyway.", vl->tid, tid, asl->getName());
      else
	 xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d have deadlocked trying to acquire the same resources", vl->tid, tid);
      rc = -1;
   }
   
   if (!rc)
   {
      //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p about to block on VRMutex owned by TID %d\n", this, asl, vl ? vl->tid : -1);
      rc = asl->self_wait(cond, timeout_ms);
      //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p regrabbed lock\n", this, asl);
   }

   waiting_on = 0;
   
   return rc;
}

int VLock::waitOn(AbstractSmartLock *asl, vlock_map_t &vmap, ExceptionSink *xsink, int timeout_ms)
{
   waiting_on = asl;

   int rc = 0;
   for (vlock_map_t::iterator i = vmap.begin(), e = vmap.end(); i != e; ++i)
   {
      AbstractSmartLock *vl_wait = i->second->waiting_on;
      //printd(5, "VLock::waitOn(asl=%08p, vmap size=%d) vl_wait=%08p other_tid=%d\n", asl, vmap.size(), vl_wait, i->second->tid);
      if (vl_wait && find(vl_wait))
      {
	 // NOTE: we throw an exception here anyway as a deadlock is a programming mistake and therefore should be visible to the programmer
	 // (even if it really wouldn't technically deadlock at this point due to the timeout)
	 if (timeout_ms)
	    xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d would deadlock on the same resources; this represents a programming error so even though a %s method was called with a timeout and therefore would not technically deadlock at this point, this exception is thrown anyway.", i->second->tid, tid, asl->getName());
	 else
	    xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d have deadlocked trying to acquire the same resources", i->second->tid, tid);
	 rc = -1;
	 break;
      }
   }
   
   if (!rc)
   {
      //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p about to block on VRMutex owned by TID %d\n", this, asl, vl ? vl->tid : -1);
      rc = asl->self_wait(timeout_ms);
      //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p regrabbed lock\n", this, asl);
   }

   waiting_on = 0;

   return rc;
}

#ifdef DEBUG
void VLock::show(class VLock *vl) const
{
   //printd(0, "VLock::show() this=%08p, vl=%08p vl->waiting_on=%08p (in this=%08p)\n", this, vl, vl ? vl->waiting_on : 0, vl ? find(vl->waiting_on) : 0);
}
#endif

VLock::VLock(int n_tid) : waiting_on(0), tid(n_tid)
{
}

VLock::~VLock()
{
   //printd(5, "VLock::~VLock() this=%08p\n", this);
   assert(begin() == end());
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

class AbstractSmartLock *VLock::find(class AbstractSmartLock *g) const
{
   for (abstract_lock_list_t::const_iterator i = begin(), e = end(); i != e; ++i)
      if (*i == g)
	 return g;
   return 0;
}
