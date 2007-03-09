/*
 VLock.h
  
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

#ifndef _QORE_VLOCK_H

#define _QORE_VLOCK_H

#include <qore/Qore.h>
#include <qore/AbstractSmartLock.h>

#include <list>
#include <map>

typedef std::list<AbstractSmartLock *> abstract_lock_list_t;

typedef std::map<int, class VLock *> vlock_map_t;

// for tracking locks per thread and detecting deadlocks
class VLock : protected abstract_lock_list_t
{
   private:
      AbstractSmartLock *waiting_on;
      int tid;
      
   public:
      DLLLOCAL VLock();
      DLLLOCAL ~VLock();
      DLLLOCAL void push(AbstractSmartLock *g);
      DLLLOCAL int pop(AbstractSmartLock *asl);
      DLLLOCAL void del();
      DLLLOCAL AbstractSmartLock *find(AbstractSmartLock *g) const; 

      // for blocking smart locks with deadlock detection
      DLLLOCAL int waitOn(AbstractSmartLock *asl, class VLock *vl, int tid, class ExceptionSink *xsink);
      DLLLOCAL int waitOn(AbstractSmartLock *asl, class VLock *vl, int tid, int timeout_ms, class ExceptionSink *xsink);
      // for smart locks using an alternate condition variable
      DLLLOCAL int waitOn(AbstractSmartLock *asl, class QoreCondition *cond, class VLock *vl, int tid, class ExceptionSink *xsink);
      DLLLOCAL int waitOn(AbstractSmartLock *asl, class QoreCondition *cond, class VLock *vl, int tid, int timeout_ms, class ExceptionSink *xsink);
      // for smart locks that can be held by more than one thread and are using an alternate condition variable
      DLLLOCAL int waitOn(AbstractSmartLock *asl, vlock_map_t &vmap, int tid, class ExceptionSink *xsink);
      DLLLOCAL int waitOn(AbstractSmartLock *asl, vlock_map_t &vmap, int tid, int timeout_ms, class ExceptionSink *xsink);
      DLLLOCAL int getTID() { return tid; }
         
#ifdef DEBUG
      DLLLOCAL void show(class VLock *nvl) const; 
#endif
};

class AutoVLock : protected abstract_lock_list_t
{
   public:
      DLLLOCAL AutoVLock();
      DLLLOCAL ~AutoVLock();
      DLLLOCAL void del();
      DLLLOCAL void push(AbstractSmartLock *asl);
};


#endif
