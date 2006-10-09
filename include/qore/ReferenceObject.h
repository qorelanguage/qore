/*
  ReferenceObject.h

  Qore Programming Language

  Copyright (C) 2003, 2004

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

#ifndef _QORE_REFERENCE_OBJECT_H

#define _QORE_REFERENCE_OBJECT_H

#include <qore/config.h>
#include <qore/macros.h>

#if !defined(HAVE_ATOMIC_MACROS)
#include <qore/LockedObject.h>
#endif

class ReferenceObject 
{
   protected:
      int references;
#if !defined(HAVE_ATOMIC_MACROS)
   // for atomic reference updates
      class LockedObject m;
#endif
   public:
      inline ReferenceObject();
      inline int reference_count() { return references; }
      inline bool is_unique() { return references == 1; }
      inline void ROreference();
      inline bool ROdereference();
#ifdef DEBUG
      //virtual void test() {}
#endif
};

inline ReferenceObject::ReferenceObject()
{
   references = 1;
}

inline void ReferenceObject::ROreference()
{
#ifdef HAVE_ATOMIC_MACROS
   atomic_inc(&references);
#else
   m.lock();
   ++references; 
   m.unlock();
#endif
}

// returns true when references reach zero
inline bool ReferenceObject::ROdereference()
{
   // do not do a cache sync (or at worst a mutex lock and unlock) if references == 1
   if (references == 1)
      return true;
#ifdef HAVE_ATOMIC_MACROS
   return atomic_dec(&references);
#else
   m.lock();
   int rc = --references;
   m.unlock();
   return !rc;
#endif
}

#endif // _QORE_REFERENCE_OBJECT_H
