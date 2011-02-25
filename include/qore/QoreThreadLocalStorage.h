/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreThreadLocalStorage.h

  Qore Programming Language

  Copyright (C) 2003 - 2010 David Nichols, all rights reserved

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

#ifndef _QORE_QORETHREADLOCALSTORAGE_H

#define _QORE_QORETHREADLOCALSTORAGE_H

#include <pthread.h>
#include <assert.h>

//! provides access to thread-local storage
/** This class is just a simple wrapper for pthread_key_t.  It does not provide
    any special logic for checking for correct usage, etc.
 */
template<typename T>
class QoreThreadLocalStorage {
protected:
   //! the actual thread local storage key wrapped in this class
   pthread_key_t key;
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreThreadLocalStorage(const QoreThreadLocalStorage&);
   
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreThreadLocalStorage& operator=(const QoreThreadLocalStorage&);

public:
   //! creates the key
   DLLLOCAL QoreThreadLocalStorage() {
      create();
   }

   //! destroys the key
   DLLLOCAL ~QoreThreadLocalStorage() {
      destroy();
   }

   //! creates the key
   DLLLOCAL void create() {
      pthread_key_create(&key, 0);
   }

   //! destroys the key
   DLLLOCAL void destroy() {
      pthread_key_delete(key);
   }

   //! retrieves the key's value
   DLLLOCAL T *get() {
#ifndef DEBUG
      return (T *)pthread_getspecific(key);
#else
      T *rv = (T *)pthread_getspecific(key);
      assert(rv);
      return rv;
#endif
   }

   //! sets the key's value
   DLLLOCAL void set(T *ptr) {
#ifndef DEBUG
      pthread_setspecific(key, (void *)ptr);
#else
      int rc = pthread_setspecific(key, (void *)ptr);
      assert(!rc);
#endif
   }
};

#endif // _QORE_QORETHREADLOCALSTORAGE_H
