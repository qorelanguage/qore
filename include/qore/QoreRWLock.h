/*
  QoreRWLock.cc

  simple pthreads-based read-write lock

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

#ifndef _QORE_QORERWLOCK_H
#define _QORE_QORERWLOCK_H

#include <pthread.h>

//! provides a simple POSIX-threads-based read-write lock
/** This utility class is just a simple wrapper for pthread_rwlock_t.  It does 
    not provide any special logic for checking for correct usage, etc.
 */
class QoreRWLock {
protected:
   //! the actual locking primitive wrapped in this class
   pthread_rwlock_t m;

    //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreRWLock& operator=(const QoreRWLock&);

public:
   //! creates and initializes the lock
   DLLLOCAL QoreRWLock() {
#ifndef NDEBUG
      int rc =
#endif
      pthread_rwlock_init(&m, 0);
      assert(!rc);
   }

   //! destroys the lock
   DLLLOCAL ~QoreRWLock() {
#ifndef NDEBUG
      int rc =
#endif
      pthread_rwlock_destroy(&m);
      assert(!rc);
   }

   //! grabs the read lock
   DLLLOCAL int rdlock() {
      return pthread_rwlock_rdlock(&m);
   }

   //! grabs the write lock
   DLLLOCAL int wrlock() {
      return pthread_rwlock_wrlock(&m);
   }

   //! tries to grab the read lock; does not block if unsuccessful
   DLLLOCAL int tryrdlock() {
      return pthread_rwlock_tryrdlock(&m);
   }

   //! tries to grab the write lock; does not block if unsuccessful
   DLLLOCAL int trywrlock() {
      return pthread_rwlock_trywrlock(&m);
   }

   //! unlocks the lock (assumes the lock is locked)
   DLLLOCAL int unlock() {
      return pthread_rwlock_unlock(&m);
   }
};

//! provides a safe and exception-safe way to hold read locks in Qore, only to be used on the stack, cannot be dynamically allocated
/** Ensures that read locks are released by locking the read lock when the
    object is created and releasing it when the object is destroyed.
    @see QoreAutoRWWriteLocker
*/

class QoreAutoRWReadLocker {
private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreAutoRWReadLocker(const QoreAutoRWReadLocker&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreAutoRWReadLocker& operator=(const QoreAutoRWReadLocker&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void *operator new(size_t);

protected:
   //! the pointer to the lock that will be managed
   QoreRWLock *l;

public:
   //! creates the object and grabs the read lock
   DLLLOCAL QoreAutoRWReadLocker(QoreRWLock &n_l) : l(&n_l) {
      l->rdlock();
   }

   //! creates the object and grabs the read lock
   DLLLOCAL QoreAutoRWReadLocker(QoreRWLock *n_l) : l(n_l) {
      l->rdlock();
   }

   //! destroys the object and releases the lock
   DLLLOCAL ~QoreAutoRWReadLocker() {
      l->unlock();
   }
};

class QoreAutoRWWriteLocker {
private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreAutoRWWriteLocker(const QoreAutoRWWriteLocker&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreAutoRWWriteLocker& operator=(const QoreAutoRWWriteLocker&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void *operator new(size_t);

protected:
   //! the pointer to the lock that will be managed
   QoreRWLock *l;

public:
   //! creates the object and grabs the write lock
   DLLLOCAL QoreAutoRWWriteLocker(QoreRWLock &n_l) : l(&n_l) {
      l->wrlock();
   }

   //! creates the object and grabs the write lock
   DLLLOCAL QoreAutoRWWriteLocker(QoreRWLock *n_l) : l(n_l) {
      l->wrlock();
   }

   //! destroys the object and releases the lock
   DLLLOCAL ~QoreAutoRWWriteLocker() {
      l->unlock();
   }
};

#endif // #ifndef _QORE_QORERWLOCK_H
