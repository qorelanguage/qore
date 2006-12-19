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
      class LockedObject mRO;
#endif

   public:
      DLLEXPORT ReferenceObject();
      DLLEXPORT ~ReferenceObject();
      DLLEXPORT int reference_count() const 
      { 
	 return references; 
      }
      DLLEXPORT bool is_unique() const 
      { 
	 return references == 1; 
      }
      DLLEXPORT void ROreference();
      DLLEXPORT bool ROdereference();
};

#endif // _QORE_REFERENCE_OBJECT_H
