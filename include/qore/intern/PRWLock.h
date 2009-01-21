/*
  PRWLock.cc

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

#ifndef _QORE_PRWLOCK_H
#define _QORE_PRWLOCK_H

#include <pthread.h>

class PRWLock {
protected:
   pthread_rwlock_t m;

public:
   DLLLOCAL PRWLock() {
      pthread_rwlock_init(&m, 0);
   }

   DLLLOCAL ~PRWLock() {
      pthread_rwlock_destroy(&m);
   }

   DLLLOCAL int rdlock() {
      return pthread_rwlock_rdlock(&m);
   }

   DLLLOCAL int wrlock() {
      return pthread_rwlock_wrlock(&m);
   }

   DLLLOCAL int tryrdlock() {
      return pthread_rwlock_tryrdlock(&m);
   }

   DLLLOCAL int trywrlock() {
      return pthread_rwlock_trywrlock(&m);
   }

   DLLLOCAL int unlock() {
      return pthread_rwlock_unlock(&m);
   }
};

class AutoPRWReadLocker {
protected:
   PRWLock *l;

public:
   DLLLOCAL AutoPRWReadLocker(PRWLock &n_l) : l(&n_l) {
      l->rdlock();
   }

   DLLLOCAL AutoPRWReadLocker(PRWLock *n_l) : l(n_l) {
      l->rdlock();
   }

   DLLLOCAL ~AutoPRWReadLocker() {
      l->unlock();
   }
};

class AutoPRWWriteLocker {
protected:
   PRWLock *l;

public:
   DLLLOCAL AutoPRWWriteLocker(PRWLock &n_l) : l(&n_l) {
      l->wrlock();
   }

   DLLLOCAL AutoPRWWriteLocker(PRWLock *n_l) : l(n_l) {
      l->wrlock();
   }

   DLLLOCAL ~AutoPRWWriteLocker() {
      l->unlock();
   }
};

#endif // #ifndef _QORE_PRWLOCK_H
