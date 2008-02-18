/*
 AutoVLock.h
  
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

#ifndef _QORE_AUTOVLOCK_H

#define _QORE_AUTOVLOCK_H

class AbstractSmartLock;

#ifndef QORE_AVL_INTERN
#define QORE_AVL_INTERN 5
#endif

//! AutoVLock is a container for safely managing AbstractSmartLock objects, required for functions accessing global variables and object data where locking is necessary
/** This object is used for nested lock management and automatically releasing all locks.
    For performance reasons a minimal list is included directly in this implementation.
 */
class AutoVLock
{
   private:
      //! the count of locks held directly in this structure
      int counter;
      //! the list of locks held directly in this structure
      AbstractSmartLock *fp[QORE_AVL_INTERN];
      //! private implementation of the generic lock container
      struct qore_avl_private *priv;
   
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL AutoVLock(const AutoVLock&);
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL AutoVLock& operator=(const AutoVLock&);
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void *operator new(size_t);
   
   public:
      //! creates an empty lock container
      DLLEXPORT AutoVLock();

      //! releases all locks held and destroys the container
      DLLEXPORT ~AutoVLock();

      //! manually releases all locks held
      DLLEXPORT void del();

      //! pushes a lock on the list
      DLLLOCAL void push(class AbstractSmartLock *asl);
};

#endif
