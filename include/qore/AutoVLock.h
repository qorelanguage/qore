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

struct qore_avl_private;

// AutoVLock is for grabbing a series of locks that will only be deleted when the AutoVLock structure is deleted
class AutoVLock
{
   private:
      struct qore_avl_private *priv; // private implementation

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
