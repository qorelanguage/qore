/*
 VLock.cc
 
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
#include <qore/AutoVLock.h>

#include <assert.h>

#include <vector>
#include <string>

struct qore_obj_notification_s {
      QoreObject *obj;
      std::string member;

      DLLLOCAL qore_obj_notification_s(QoreObject *o, const char *m) : obj(o), member(m) {}
};

typedef std::vector<qore_obj_notification_s> qore_notify_list_t;

struct qore_avl_private {
      qore_notify_list_t *notify_list;

      DLLLOCAL qore_avl_private() : notify_list(0)
      {
      }
      DLLLOCAL ~qore_avl_private()
      {
	 if (notify_list)
	    delete notify_list;
      }
      DLLLOCAL void add(QoreObject *obj, const char *mem)
      {
	 //printd(5, "qore_avl_private::add(%08x, '%s')\n", obj, mem);
	 if (!notify_list)
	    notify_list = new qore_notify_list_t;
	 else {
	    for (qore_notify_list_t::iterator i = notify_list->begin(), e = notify_list->end(); i != e; ++i)
	       if ((*notify_list)[0].obj == obj && (*notify_list)[0].member != mem)
		  return;
	 }

	 notify_list->push_back(qore_obj_notification_s(obj, mem));
      }
};

AutoVLock::AutoVLock(ExceptionSink *n_xsink) : m(0), o(0), xsink(n_xsink), priv(0)
{
   //printd(5, "AutoVLock::AutoVLock() this=%08p\n", this);
}

AutoVLock::~AutoVLock()
{
   //printd(5, "AutoVLock::~AutoVLock() this=%08p size=%d\n", this, size());
   del();
   if (priv && priv->notify_list) {
      ExceptionSink xsink2;
      
      for (qore_notify_list_t::iterator i = priv->notify_list->begin(), e = priv->notify_list->end(); i != e; ++i) {
	 // run member notifications regardless of exception status
	 //printd(5, "posting notification to object %08p, member %s\n", i->obj, i->member.c_str());
	 i->obj->execMemberNotification(i->member.c_str(), &xsink2);
      }
      xsink->assimilate(&xsink2);
   }

   delete priv;
}

void AutoVLock::del()
{
   if (m) {
      m->unlock();
      m = 0;
      if (o) {
	 o->tDeref();
	 o = 0;
      }
   }
   assert(!o);
}

void AutoVLock::set(QoreThreadLock *n_m)
{
   assert(!m);
   m = n_m;
}

void AutoVLock::set(QoreObject *n_o, QoreThreadLock *n_m) {
   assert(!m);
   o = n_o;
   m = n_m;
}

QoreThreadLock *AutoVLock::get() {
   return m;
}

void AutoVLock::addMemberNotification(QoreObject *obj, const char *member) {
   // ignore member notifications for updates made within the class
   if (obj == getStackObject() || !obj->hasMemberNotification())
      return;

   if (!priv)
      priv = new qore_avl_private;

   priv->add(obj, member);
}

int VLock::waitOn(AbstractSmartLock *asl, VLock *vl, ExceptionSink *xsink, int timeout_ms) {
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
