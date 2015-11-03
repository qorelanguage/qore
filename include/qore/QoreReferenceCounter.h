/*
  QoreReferenceCounter.h

  Qore Programming Language

  Copyright (C) 2003 - 2009 David Nichols, all rights reserved

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

#ifndef _QORE_QOREREFERENCECOUNTER_H

#define _QORE_QOREREFERENCECOUNTER_H

#include <qore/common.h>
#include <qore/macros.h>

class QoreThreadLock;

//! provides atomic reference counting to Qore objects
class QoreReferenceCounter 
{
   protected:
      mutable int references;
#ifndef HAVE_ATOMIC_MACROS
      //! pthread lock to ensure atomicity of updates for architectures where we don't have an atomic increment and decrement implementation
      mutable QoreThreadLock mRO;
#endif

   public:
      //! creates the reference counter object
      DLLEXPORT QoreReferenceCounter();

      //! destroys the reference counter object
      DLLEXPORT ~QoreReferenceCounter();

      //! gets the reference count
      /**
	 @return returns the current reference count
       */
      DLLEXPORT int reference_count() const 
      { 
	 return references; 
      }

      //! returns true if the reference count is 1
      /**
	 @return returns true if the reference count is 1
       */
      DLLEXPORT bool is_unique() const 
      { 
	 return references == 1; 
      }

      //! atomically increments the reference count
      DLLEXPORT void ROreference() const;

      //! atomically decrements the reference count
      /**
	 returns true if the reference count is now zero
	 @return true if the reference count is now zero
       */
      DLLEXPORT bool ROdereference() const;
};

#endif // _QORE_QOREREFERENCECOUNTER_H
