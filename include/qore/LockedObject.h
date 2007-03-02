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
   public:
      pthread_mutex_t ptm_lock;

      inline LockedObject()
      {
	 pthread_mutex_init(&ptm_lock, NULL);
      }

      inline ~LockedObject()
      {
	 pthread_mutex_destroy(&ptm_lock);
      }

      inline void lock();

      inline void unlock();

      inline int trylock()
      {
	 return pthread_mutex_trylock(&ptm_lock);
      }
};

inline void LockedObject::lock()
{
   pthread_mutex_lock(&ptm_lock);
}

inline void LockedObject::unlock()
{
   pthread_mutex_unlock(&ptm_lock);
}

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
   SafeLocker(LockedObject *l)
   {
      lck = l;
      lck->lock();
      locked = true;
   }
   ~SafeLocker()
   {
      if (locked)
	 lck->unlock();
   }
   void lock()
   {
      assert(!locked);
      lck->lock();
      locked = true;
   }
   void unlock()
   {
      assert(locked);
      locked = false;
      lck->unlock();
   }
};

#endif // _QORE_LOCKEDOBJECT_H
