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

struct qore_avl_private;

#ifndef QORE_AVL_INTERN
#define QORE_AVL_INTERN 5
#endif

// AutoVLock is for grabbing a series of locks that will only be deleted when the AutoVLock structure is deleted
class AutoVLock
{
   private:
      // for performance reasons a minimal list is included directly
      // in this implementation
      int counter;
      AbstractSmartLock *fp[QORE_AVL_INTERN];
      struct qore_avl_private *priv;
   
      // not implemented
      AutoVLock(const AutoVLock&);
      AutoVLock& operator=(const AutoVLock&);
      void *operator new(size_t);
   
   public:
      DLLEXPORT AutoVLock();
      DLLEXPORT ~AutoVLock();
      DLLEXPORT void del();

      DLLLOCAL void push(class AbstractSmartLock *asl);
};

#endif
