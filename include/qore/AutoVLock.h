/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 AutoVLock.h
  
 Qore Programming Language
 
 Copyright 2003 - 2013 David Nichols
 
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

#ifndef _QORE_AUTOVLOCK_H

#define _QORE_AUTOVLOCK_H

class AbstractSmartLock;

struct QLckPtr {
private:
   size_t ptr;

protected:
   DLLLOCAL void unlockIntern() {
      assert(ptr);
      if (ptr & 1)
         ((QoreRWLock*)(ptr ^ 1))->unlock();
      else
         ((QoreThreadLock*)ptr)->unlock();
   }

public:
   DLLLOCAL QLckPtr() : ptr(0) {
   }

   DLLLOCAL void set(QoreThreadLock* m) {
      ptr = (size_t)m;
   }

   DLLLOCAL void set(QoreRWLock* rwl) {
      ptr = (size_t)rwl | 1;
   }

   DLLLOCAL QoreThreadLock* getMutex() const {
      assert(!(ptr & 1));
      return (QoreThreadLock*)ptr;
   }

   DLLLOCAL QoreRWLock* getRWL() const {
      assert(ptr & 1);
      return (QoreRWLock*)(ptr ^ 1);
   }

   DLLLOCAL bool isSet() const {
      return ptr;
   }

   /*
   DLLLOCAL void unlock() {
      if (ptr)
         unlockIntern();
   }
   */

   DLLLOCAL void unlockAndClear() {
      if (ptr) {
         unlockIntern();
         ptr = 0;
      }
   }

   DLLLOCAL void clear() {
      assert(ptr);
      ptr = 0;
   }
};

//! AutoVLock is a container for safely managing global variable and object lock handovers, required for functions accessing global variables and object data where locking is necessary
/** This object is used for lock handover management and automatically releasing the last lock.
 */
class AutoVLock {
private:
   // pointer to lock currently held
   QLckPtr lock;
   //QoreThreadLock* m;

   // pointer to object to dereference
   QoreObject* o;

public:
   // pointer to ExceptionSink object for use with object notifications
   ExceptionSink* xsink;

private:
   //! private implementation of the object notification container
   struct qore_avl_private* priv;

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL AutoVLock(const AutoVLock&);
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL AutoVLock& operator=(const AutoVLock&);
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void* operator new(size_t);
   
public:
   //! creates an empty lock container
   /** @param n_xsink pointer to ExceptionSink object for use with object notifications
    */
   DLLEXPORT AutoVLock(ExceptionSink* n_xsink);

   //! releases all locks held and destroys the container
   DLLEXPORT ~AutoVLock();

   //! returns true if managing a lock, false if not (unlocked)
   DLLEXPORT operator bool() const;

   //! manually releases the lock currently held
   DLLEXPORT void del();

   //! sets the current lock
   DLLLOCAL void set(QoreThreadLock* n_m);

   //! sets the current object (for dereference) and lock
   DLLLOCAL void set(QoreObject* n_o, QoreRWLock* n_rwl);

   //! gets the current read-write lock
   DLLLOCAL QoreRWLock* getRWL() const;

   //! gets the current object
   DLLLOCAL QoreObject* getObject() const;

   //! leaves the lock locked and the object referenced and clears the object and lock pointers
   DLLLOCAL void clear();

   //! adds an object member notification entry, internal-only
   /** @param o the object to add
       @param member the member that was changed (must be in QCS_DEFAULT encoding)
   */
   DLLLOCAL void addMemberNotification(QoreObject* o, const char* member);
};

#endif
