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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/ReferenceObject.h>

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
   // do not do a cache sync (or at worst a mutex lock and unlock) if references == 1
   if (references == 1)
      return true;
#endif
#ifdef HAVE_ATOMIC_MACROS
   return atomic_dec(&references);
#else
   mRO.lock();
   int rc = --references;
   mRO.unlock();
   return !rc;
#endif
}
