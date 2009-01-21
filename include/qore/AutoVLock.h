/*
 AutoVLock.h
  
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

#ifndef _QORE_AUTOVLOCK_H

#define _QORE_AUTOVLOCK_H

class AbstractSmartLock;

//! AutoVLock is a container for safely managing global variable and object lock handovers, required for functions accessing global variables and object data where locking is necessary
/** This object is used for lock handover management and automatically releasing the last lock.
 */
class AutoVLock
{
   private:
      // pointer to lock currently held
      QoreThreadLock *m;

      // pointer to object to dereference
      QoreObject *o;

      // pointer to ExceptionSink object for use with object notifications
      ExceptionSink *xsink;

      //! private implementation of the object notification container
      struct qore_avl_private *priv;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL AutoVLock(const AutoVLock&);
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL AutoVLock& operator=(const AutoVLock&);
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void *operator new(size_t);
   
   public:
      //! creates an empty lock container
      /** @param n_xsink pointer to ExceptionSink object for use with object notifications
       */
      DLLEXPORT AutoVLock(ExceptionSink *n_xsink);

      //! releases all locks held and destroys the container
      DLLEXPORT ~AutoVLock();

      //! manually releases the lock currently held
      DLLEXPORT void del();

      //! sets the current lock
      DLLLOCAL void set(QoreThreadLock *n_m);

      //! sets the current object (for dereference) and lock
      DLLLOCAL void set(QoreObject *n_o, QoreThreadLock *n_m);

      //! gets the current lock
      DLLLOCAL QoreThreadLock *get();

      //! adds an object member notification entry, internal-only
      /** @param o the object to add
	  @param member the member that was changed (must be in QCS_DEFAULT encoding)
       */
      DLLLOCAL void addMemberNotification(QoreObject *o, const char *member);
};

#endif
