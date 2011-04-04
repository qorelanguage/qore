/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreThreadLock.h

  Qore Programming Language

  Copyright (C) 2003 - 2011 David Nichols, all rights reserved

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

#ifndef _QORE_QORETHREADLOCK_H

#define _QORE_QORETHREADLOCK_H

#include <pthread.h>

#include <assert.h>

#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

//! provides a mutually-exclusive thread lock
/** This class is just a simple wrapper for pthread_mutex_t.  It does not provide any special
    logic for checking for correct usage, etc.
*/
class QoreThreadLock {
   friend class QoreCondition;

private:
   //! the actual locking primitive wrapped in this class
   pthread_mutex_t ptm_lock;

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreThreadLock& operator=(const QoreThreadLock&);

   //! internal initialization
   DLLLOCAL void init(const pthread_mutexattr_t *pma = 0) {
#ifndef NDEBUG
      int rc =
#endif
	 pthread_mutex_init(&ptm_lock, pma);
      assert(!rc);
   }

public:

   //! creates the lock
   DLLLOCAL QoreThreadLock() {
      init();
   }

   //! creates the lock with the given attributes
   DLLLOCAL QoreThreadLock(const pthread_mutexattr_t *ma) {
      init(ma);
   }

   //! destroys the lock
   DLLLOCAL ~QoreThreadLock() {
      pthread_mutex_destroy(&ptm_lock);
   }

   //! creates a new object (not based on the original lock status)
   DLLLOCAL QoreThreadLock(const QoreThreadLock&) {
      init();
   }

   //! grabs the lock (assumes that the lock is unlocked)
   /** no error checking happens here; if you grab the lock twice it will deadlock
    */
   DLLLOCAL void lock() {
#ifndef NDEBUG
      int rc =
#endif
	 pthread_mutex_lock(&ptm_lock);
      assert(!rc);
   }

   //! releases the lock (assumes that the lock is locked)
   /** no error checking is implemented here
    */
   DLLLOCAL void unlock() {
#ifndef NDEBUG
      int rc =
#endif
	 pthread_mutex_unlock(&ptm_lock);
      assert(!rc);
   }

   //! attempts to acquire the mutex and returns the status immediately; does not block
   /**
      @return 0 if the lock was acquired, a non-zero error number if the lock was not acquired
   */
   DLLLOCAL int trylock() {
      return pthread_mutex_trylock(&ptm_lock);
   }
};

//! provides a safe and exception-safe way to hold locks in Qore, only to be used on the stack, cannot be dynamically allocated
/** Ensures that locks are released by locking the lock when the
    object is created and releasing it when the object is destroyed.
    For a similar object that allows for unlocking the lock earlier 
    than the object's destruction, see SafeLocker.
    @see SafeLocker
    @see OptLocker
*/
class AutoLocker {
private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL AutoLocker(const AutoLocker&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL AutoLocker& operator=(const AutoLocker&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void *operator new(size_t);

protected:
   //! the pointer to the lock that will be managed
   QoreThreadLock *lck;

public:
   //! creates the object and grabs the lock
   DLLLOCAL AutoLocker(QoreThreadLock *l) : lck(l) {
      lck->lock();
   }

   //! creates the object and grabs the lock
   DLLLOCAL AutoLocker(QoreThreadLock &l) : lck(&l) {
      lck->lock();
   }

   //! destroys the object and grabs the lock
   DLLLOCAL ~AutoLocker() {
      lck->unlock();
   }
};

//! provides an exception-safe way to manage locks in Qore, only to be used on the stack, cannot be dynamically allocated
/** Ensures that locks are released by locking the lock when the
    object is created and releasing it when the object is destroyed.
    Also allows the lock to be released before the object's destruction
    at the expense of one extra byte on the stack compared to the AutoLocker class.
    @see AutoLocker
    @see OptLocker
*/
class SafeLocker {
private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL SafeLocker(const SafeLocker&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL SafeLocker& operator=(const SafeLocker&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void *operator new(size_t);

protected:
   //! the pointer to the lock that will be managed      
   QoreThreadLock *lck;

   //! flag indicating if the lock is held or not
   bool locked;

public:
   //! creates the object and grabs the lock
   DLLEXPORT SafeLocker(QoreThreadLock *l) : lck(l) {
      lck->lock();
      locked = true;
   }

   //! creates the object and grabs the lock
   DLLEXPORT SafeLocker(QoreThreadLock &l) : lck(&l) {
      lck->lock();
      locked = true;
   }

   //! destroys the object and unlocks the lock if it's held
   DLLEXPORT ~SafeLocker() {
      if (locked)
         lck->unlock();
   }

   //! locks the object and updates the locked flag, assumes that the lock is not already held
   DLLEXPORT void lock() {
      assert(!locked);
      lck->lock();
      locked = true;
   }

   //! unlocks the object and updates the locked flag, assumes that the lock is held
   DLLEXPORT void unlock() {
      assert(locked);
      locked = false;
      lck->unlock();
   }

   //! will not unlock the lock when the destructor is run; do not use any other functions of this class after calling this function
   DLLEXPORT void stay_locked() {
      assert(locked);
      locked = false;
   }
};

//! provides a safe and exception-safe way to hold optional locks in Qore, only to be used on the stack, cannot be dynamically allocated
/** Ensures that locks are released by locking the lock when the object is created and releasing it when the object is destroyed.
    @see SafeLocker
    @see AutoLocker
*/
class OptLocker {
private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL OptLocker(const OptLocker&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL OptLocker& operator=(const OptLocker&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void *operator new(size_t);

protected:
   //! the pointer to the lock that will be managed
   QoreThreadLock *lck;

public:
   //! creates the object and grabs the lock if the argument is not NULL
   DLLLOCAL OptLocker(QoreThreadLock *l) : lck(l) {
      if (lck)
         lck->lock();
   }

   //! releases the lock if there is a lock pointer being managed and destroys the object
   DLLLOCAL ~OptLocker() {
      if (lck)
         lck->unlock();
   }
};

#endif // _QORE_QORETHREADLOCK_H
