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
#include <pthread.h>
#endif

class ReferenceObject {
#if !defined(HAVE_ATOMIC_MACROS)
      // for atomic reference updates
      pthread_mutex_t ref_mutex;
#endif
   protected:
      int references;
   public:
      inline ReferenceObject();
#if !defined(HAVE_ATOMIC_MACROS)
      inline ~ReferenceObject()
      {
	 pthread_mutex_destroy(&ref_mutex);
      }
#endif
      inline int reference_count() { return references; }
      inline bool is_unique() { return references == 1; }
      inline void ROreference();
      inline int ROdereference();
};

inline ReferenceObject::ReferenceObject()
{
   references = 1;
#if !defined(HAVE_ATOMIC_MACROS)
   pthread_mutex_init(&ref_mutex, NULL);
#endif
}

inline void ReferenceObject::ROreference()
{
#ifdef HAVE_ATOMIC_MACROS
   atomic_inc(&references);
#else
   pthread_mutex_lock(&ref_mutex);
   ++references; 
   pthread_mutex_unlock(&ref_mutex);
#endif
}

// returns 1 when references reach zero
inline int ReferenceObject::ROdereference()
{
#ifdef HAVE_ATOMIC_MACROS
   return atomic_dec(&references);
#else
   pthread_mutex_lock(&ref_mutex);
   int rc = --references;
   pthread_mutex_unlock(&ref_mutex);
   return !rc;
#endif
}

#endif // _QORE_REFERENCE_OBJECT_H
