/*
  LockedObject.h

  Qore Programming Language

  Copyright (C) 2003, 2004

  Abstract class for objects that need to be able to lock themselves

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

#ifndef _QORE_LOCKEDOBJECT_H

#define _QORE_LOCKEDOBJECT_H

#include <pthread.h>
#include <assert.h>

class LockedObject {
      friend class QoreCondition;

      LockedObject(const LockedObject&); // not implemented
      LockedObject& operator=(const LockedObject&); // not implemented
      pthread_mutex_t ptm_lock;

   public:
      DLLEXPORT LockedObject()
      {
	 pthread_mutex_init(&ptm_lock, NULL);
      }

      DLLEXPORT ~LockedObject()
      {
	 pthread_mutex_destroy(&ptm_lock);
      }

      DLLEXPORT void lock()
      {
	 pthread_mutex_lock(&ptm_lock);
      }
      
      DLLEXPORT void unlock()
      {
	 pthread_mutex_unlock(&ptm_lock);
      }
      
      DLLEXPORT int trylock()
      {
	 return pthread_mutex_trylock(&ptm_lock);
      }
};

// to be used as a stack object (not on the heap) as an exception-safe way to ensure that locks are released
// this object does not allow the lock to be released earlier than the object's scope
class AutoLocker {
   // not implemented
   AutoLocker(const AutoLocker&);
   AutoLocker& operator=(const AutoLocker&);
   void *operator new(size_t);

private:
   LockedObject *lck;

public:
   DLLEXPORT AutoLocker(LockedObject *l)
   {
      lck = l;
      lck->lock();
   }
   DLLEXPORT ~AutoLocker()
   {
      lck->unlock();
   }
};

// to be used as a stack object (not on the heap) as an exception-safe way to ensure that locks are released
// this object allows the lock to be released earlier than the SafeLocker's scope
class SafeLocker
{
   // not implemented
   SafeLocker(const SafeLocker&);
   SafeLocker& operator=(const SafeLocker&);
   void *operator new(size_t);

private:
   LockedObject *lck;
   bool locked;

public:
   DLLEXPORT SafeLocker(LockedObject *l)
   {
      lck = l;
      lck->lock();
      locked = true;
   }
   DLLEXPORT ~SafeLocker()
   {
      if (locked)
	 lck->unlock();
   }
   DLLEXPORT void lock()
   {
      assert(!locked);
      lck->lock();
      locked = true;
   }
   DLLEXPORT void unlock()
   {
      assert(locked);
      locked = false;
      lck->unlock();
   }
};

#endif // _QORE_LOCKEDOBJECT_H
