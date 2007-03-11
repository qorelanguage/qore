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
      // whenever a lock is about to block, deadlock detection must be synchronized with the global lock
      static LockedObject global_lock;
      AbstractSmartLock *waiting_on;
      int tid;

      // not implemented
      VLock(const VLock&);
      VLock& operator=(const VLock&);
      
   public:
      DLLLOCAL VLock();
      DLLLOCAL ~VLock();
      DLLLOCAL void push(AbstractSmartLock *g);
      DLLLOCAL int pop(AbstractSmartLock *asl);
      DLLLOCAL void del();
      DLLLOCAL AbstractSmartLock *find(AbstractSmartLock *g) const; 

      // for blocking smart locks with deadlock detection
      DLLLOCAL int waitOn(AbstractSmartLock *asl, class VLock *vl, int tid, class ExceptionSink *xsink, int timeout_ms = 0);
      // for smart locks using an alternate condition variable
      DLLLOCAL int waitOn(AbstractSmartLock *asl, class QoreCondition *cond, class VLock *vl, int tid, class ExceptionSink *xsink, int timeout_ms = 0);
      // for smart locks that can be held by more than one thread and are using an alternate condition variable
      DLLLOCAL int waitOn(AbstractSmartLock *asl, vlock_map_t &vmap, int tid, class ExceptionSink *xsink, int timeout_ms = 0);
      DLLLOCAL int getTID() { return tid; }
         
#ifdef DEBUG
      DLLLOCAL void show(class VLock *nvl) const; 
#endif
};

// AutoVLock is for grabbing a series of locks that will only be deleted when the AutoVLock structure is deleted
class AutoVLock : protected abstract_lock_list_t
{
   private:
      // not implemented
      AutoVLock(const AutoVLock&);
      AutoVLock& operator=(const AutoVLock&);
      void *operator new(size_t);
   
   public:
      DLLLOCAL AutoVLock();
      DLLLOCAL ~AutoVLock();
      DLLLOCAL void del();
      DLLLOCAL void push(AbstractSmartLock *asl);
};

#endif
