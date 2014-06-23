/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreThreadLocalStorage.h

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
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
      return (T *)pthread_getspecific(key);
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
