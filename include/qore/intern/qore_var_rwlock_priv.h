/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_var_rwlock_priv.h

  internal read-write lock for variables

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_VAR_RWLOCK_PRIV_H
#define _QORE_VAR_RWLOCK_PRIV_H

class qore_var_rwlock_priv : public QoreRWLock {
protected:
   DLLLOCAL virtual void notifyIntern() {
   }

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL qore_var_rwlock_priv(const qore_var_rwlock_priv&);
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL qore_var_rwlock_priv& operator=(const qore_var_rwlock_priv&);

public:
   int write_tid;
   bool has_notify;

   //! creates and initializes the lock
   DLLLOCAL qore_var_rwlock_priv() : write_tid(-1), has_notify(false) {
   }

   //! destroys the lock
   DLLLOCAL virtual ~qore_var_rwlock_priv() {
   }

   //! grabs the write lock
   DLLLOCAL int wrlock() {
      int rc = pthread_rwlock_wrlock(&m);
      if (!rc)
         write_tid = gettid();
      return rc;
   }

   //! tries to grab the write lock; does not block if unsuccessful; returns 0 if successful
   DLLLOCAL int trywrlock() {
      int rc = pthread_rwlock_trywrlock(&m);
      if (!rc) {
	 assert(!write_tid);
         write_tid = gettid();
      }
      return rc;
   }

   //! unlocks the lock (assumes the lock is locked)
   DLLLOCAL int unlock() {
      if (write_tid == gettid()) {
         write_tid = -1;
	 if (has_notify)
	    notifyIntern();
      }
      return pthread_rwlock_unlock(&m);
   }

   //! grabs the read lock
   DLLLOCAL int rdlock() {
      return pthread_rwlock_rdlock(&m);
   }

   //! tries to grab the read lock; does not block if unsuccessful; returns 0 if successful
   DLLLOCAL int tryrdlock() {
      return pthread_rwlock_tryrdlock(&m);
   }

   DLLLOCAL void setNotify() {
      assert(!has_notify);
      has_notify = true;
   }
};

class QoreAutoVarRWReadLocker {
private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreAutoVarRWReadLocker(const QoreAutoVarRWReadLocker&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreAutoVarRWReadLocker& operator=(const QoreAutoVarRWReadLocker&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void *operator new(size_t);

protected:
   //! the pointer to the lock that will be managed
   QoreVarRWLock *l;

public:
   //! creates the object and grabs the read lock
   DLLLOCAL QoreAutoVarRWReadLocker(QoreVarRWLock &n_l) : l(&n_l) {
      l->rdlock();
   }

   //! creates the object and grabs the read lock
   DLLLOCAL QoreAutoVarRWReadLocker(QoreVarRWLock *n_l) : l(n_l) {
      l->rdlock();
   }

   //! destroys the object and releases the lock
   DLLLOCAL ~QoreAutoVarRWReadLocker() {
      l->unlock();
   }
};

class QoreAutoVarRWWriteLocker {
private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreAutoVarRWWriteLocker(const QoreAutoVarRWWriteLocker&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreAutoVarRWWriteLocker& operator=(const QoreAutoVarRWWriteLocker&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void *operator new(size_t);

protected:
   //! the pointer to the lock that will be managed
   QoreVarRWLock *l;

public:
   //! creates the object and grabs the write lock
   DLLLOCAL QoreAutoVarRWWriteLocker(QoreVarRWLock &n_l) : l(&n_l) {
      l->wrlock();
   }

   //! creates the object and grabs the write lock
   DLLLOCAL QoreAutoVarRWWriteLocker(QoreVarRWLock *n_l) : l(n_l) {
      l->wrlock();
   }

   //! destroys the object and releases the lock
   DLLLOCAL ~QoreAutoVarRWWriteLocker() {
      l->unlock();
   }
};

class QoreSafeVarRWReadLocker {
private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreSafeVarRWReadLocker(const QoreSafeVarRWReadLocker&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreSafeVarRWReadLocker& operator=(const QoreSafeVarRWReadLocker&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void *operator new(size_t);

protected:
   //! the pointer to the lock that will be managed
   QoreVarRWLock *l;

   //! lock flag
   bool locked;

public:
   //! creates the object and grabs the read lock
   DLLLOCAL QoreSafeVarRWReadLocker(QoreVarRWLock &n_l) : l(&n_l) {
      l->rdlock();
      locked = true;
   }

   //! creates the object and grabs the read lock
   DLLLOCAL QoreSafeVarRWReadLocker(QoreVarRWLock *n_l) : l(n_l) {
      l->rdlock();
      locked = true;
   }

   //! destroys the object and releases the lock
   DLLLOCAL ~QoreSafeVarRWReadLocker() {
      if (locked)
         l->unlock();
   }

   //! locks the object and updates the locked flag, assumes that the lock is not already held
   DLLLOCAL void lock() {
      assert(!locked);
      l->rdlock();
      locked = true;
   }

   //! unlocks the object and updates the locked flag, assumes that the lock is held
   DLLLOCAL void unlock() {
      assert(locked);
      locked = false;
      l->unlock();
   }

   //! will not unlock the lock when the destructor is run; do not use any other functions of this class after calling this function
   DLLLOCAL void stay_locked() {
      assert(locked);
      locked = false;
   }
};

class QoreSafeVarRWWriteLocker {
private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreSafeVarRWWriteLocker(const QoreSafeVarRWWriteLocker&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreSafeVarRWWriteLocker& operator=(const QoreSafeVarRWWriteLocker&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void *operator new(size_t);

protected:
   //! the pointer to the lock that will be managed
   QoreVarRWLock *l;

   //! lock flag
   bool locked;

public:
   //! creates the object and grabs the write lock
   DLLLOCAL QoreSafeVarRWWriteLocker(QoreVarRWLock &n_l) : l(&n_l) {
      l->wrlock();
      locked = true;
   }

   //! creates the object and grabs the write lock
   DLLLOCAL QoreSafeVarRWWriteLocker(QoreVarRWLock *n_l) : l(n_l) {
      l->wrlock();
      locked = true;
   }

   //! destroys the object and releases the lock
   DLLLOCAL ~QoreSafeVarRWWriteLocker() {
      if (locked)
         l->unlock();
   }

   //! locks the object and updates the locked flag, assumes that the lock is not already held
   DLLLOCAL void lock() {
      assert(!locked);
      l->wrlock();
      locked = true;
   }

   //! unlocks the object and updates the locked flag, assumes that the lock is held
   DLLLOCAL void unlock() {
      assert(locked);
      locked = false;
      l->unlock();
   }

   //! will not unlock the lock when the destructor is run; do not use any other functions of this class after calling this function
   DLLLOCAL void stay_locked() {
      assert(locked);
      locked = false;
   }
};

class QoreOptionalVarRWWriteLocker {
protected:
   QoreVarRWLock* l;

public:
   DLLLOCAL QoreOptionalVarRWWriteLocker(QoreVarRWLock* n_l) : l(n_l->trywrlock() ? 0 : n_l) {
   }

   DLLLOCAL QoreOptionalVarRWWriteLocker(QoreVarRWLock& n_l) : l(n_l.trywrlock() ? 0 : &n_l) {
   }

   DLLLOCAL ~QoreOptionalVarRWWriteLocker() {
      if (l)
         l->unlock();
   }

   DLLLOCAL operator bool() const {
      return (bool)l;
   }
};

class QoreOptionalVarRWReadLocker {
protected:
   QoreVarRWLock* l;

public:
   DLLLOCAL QoreOptionalVarRWReadLocker(QoreVarRWLock* n_l) : l(n_l->tryrdlock() ? 0 : n_l) {
   }

   DLLLOCAL QoreOptionalVarRWReadLocker(QoreVarRWLock& n_l) : l(n_l.tryrdlock() ? 0 : &n_l) {
   }

   DLLLOCAL ~QoreOptionalVarRWReadLocker() {
      if (l)
         l->unlock();
   }

   DLLLOCAL operator bool() const {
      return (bool)l;
   }
};

#endif
