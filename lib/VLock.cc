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

AutoVLock::AutoVLock()
{
   vl = getVLock();
   mark = vl->getMark();
   //printd(5, "AutoVLock::AutoVLock() this=%08p, vl=%08p mark=%08p\n", this, vl, mark);
}

AutoVLock::~AutoVLock()
{
   //printd(5, "AutoVLock::~AutoVLock() this=%08p, vl=%08p, mark=%08p\n", this, vl, mark);
   vl->delToMark(mark);
}

void AutoVLock::del()
{
   //printd(5, "AutoVLock::~AutoVLock() this=%08p, vl=%08p, mark=%08p\n", this, vl, mark);
   vl->delToMark(mark);
}

AbstractSmartLock *VLock::getMark() const
{
   abstract_lock_list_t::const_reverse_iterator i = rbegin();
   AbstractSmartLock *p = (i != rend() ? *i : NULL);
   //printd(5, "VLock::getMark() returning %08p size=%d\n", p, size());
   return p;
}

int VLock::waitOn(AbstractSmartLock *asl, VLock *vl, int current_tid, class ExceptionSink *xsink)
{
   assert(vl);
   AbstractSmartLock *vl_wait = vl->waiting_on;
   if (vl_wait && find(vl_wait))
   {
      xsink->raiseException("THREAD-DEADLOCK", "TID %d and %d have deadlocked trying to acquire the same resource", vl->tid, current_tid);
      return -1;      
   }
   waiting_on = asl;
   tid = current_tid;

#ifdef DEBUG
   show(vl);
#endif
   
   //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p about to block on VRMutex owned by TID %d\n", this, asl, vl ? vl->tid : -1);
   asl->self_wait();
   //printd(0, "AbstractSmartLock::block() this=%08p asl=%08p regrabbed lock\n", this, asl);
   waiting_on = NULL;
   
   return 0;
}

#ifdef DEBUG
void VLock::show(class VLock *vl) const
{
   //printd(0, "VLock::show() this=%08p, vl=%08p vl->waiting_on=%08p (in this=%08p)\n", this, vl, vl ? vl->waiting_on : NULL, vl ? find(vl->waiting_on) : NULL);
}
#endif

VLock::VLock()
{
   waiting_on = NULL;
   tid = 0;
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

AbstractSmartLock *VLock::pop()
{
   assert(begin() != end());
   AbstractSmartLock *asl = back();
   //printd(5, "VLock::pop() this=%08p asl=%08p size=%d\n", this, asl, size());
   pop_back();
   return asl;
}

void VLock::delToMark(AbstractSmartLock *mark)
{
   //printd(5, "VLock::delToMark() mark=%08p size=%d\n", mark, size());
   abstract_lock_list_t::reverse_iterator i;
   while (rbegin() != rend() && *(i = rbegin()) != mark)
   {
      //printd(5, "VLock::delToMark() releasing %08p (%d)\n", *i, rbegin() == rend());
      (*i)->release();
      pop_back();
   }
   //printd(5, "VLock::delToMark() rbegin=%08p\n", rbegin() != rend() ? *(rbegin()) : NULL);
}

class AbstractSmartLock *VLock::find(class AbstractSmartLock *g) const
{
   for (abstract_lock_list_t::const_iterator i = begin(), e = end(); i != e; ++i)
      if (*i == g)
	 return g;
   return NULL;
}
