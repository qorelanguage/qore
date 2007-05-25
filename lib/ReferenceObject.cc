/*
 ReferenceObject.cc
 
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

#include <qore/Qore.h>

ReferenceObject::ReferenceObject()
{
   references = 1;
}

ReferenceObject::~ReferenceObject()
{
}

void ReferenceObject::ROreference()
{
#ifdef HAVE_ATOMIC_MACROS
   atomic_inc(&references);
#else
   mRO.lock();
   ++references; 
   mRO.unlock();
#endif
}

// returns true when references reach zero
bool ReferenceObject::ROdereference()
{
#ifdef HAVE_ATOMIC_MACROS
   // do not do a cache sync if references == 1
   // this optimization leads to a race condition on platforms without atomic reference counts
   // (i.e. using a mutex lock), as one thread could decrement from 2 -> 1, and then before
   // the lock is released, the caches are synced with another CPU that sees reference count = 1
   // and deletes the object, then the first thread tries to unlock the mutex, but it's already
   // been deleted...  therefore this optimization cannot be used where atomic reference counting
   // is enforced with a mutex
   if (references == 1)
      return true;
   return atomic_dec(&references);
#else
   mRO.lock();
   int rc = --references;
   mRO.unlock();
   return !rc;
#endif
}
