/*
  QoreReferenceCounter.cpp
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2015 David Nichols
 
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

#include <qore/Qore.h>

QoreReferenceCounter::QoreReferenceCounter() : references(1) {
}

QoreReferenceCounter::~QoreReferenceCounter() {
}

void QoreReferenceCounter::ROreference() const {
#ifdef DEBUG
   if (references < 0 || references > 10000000) {
      printd(0, "QoreReferenceCounter::ROreference() this=%p references=%d\n", this, references);
      assert(false);
   }
#endif
#ifdef HAVE_ATOMIC_MACROS
   atomic_inc(&references);
#else
   mRO.lock();
   ++references; 
   mRO.unlock();
#endif
}

// returns true when references reach zero
bool QoreReferenceCounter::ROdereference() const {
#ifdef DEBUG
   if (references <= 0 || references > 10000000) {
      printd(0, "QoreReferenceCounter::ROdereference() this=%p references=%d\n", this, references);
      assert(false);
   }
#endif
#ifdef HAVE_ATOMIC_MACROS
   // do not do a cache sync if references == 1
   // this optimization leads to a race condition on platforms without atomic reference counts
   // (i.e. using a mutex lock), as one thread could decrement from 2 -> 1, and then before
   // the lock is released, the caches are synced with another CPU that sees reference count = 1
   // and deletes the object, then the first thread tries to unlock the mutex, but it's already
   // been deleted...  therefore this optimization cannot be used where atomic reference counting
   // is enforced with a mutex, but only here when the operation is atomic without a mutex
   if (references == 1) {
      // we have to set this to 0 because the QoreObject class "comes back from the dead" to execute the destructor
      // and increments the reference count from 0 again
      // however when the reference count is 1 it means that only 1 thread is manipulating the object
      // so it's safe to set the value directly like this
      references = 0;
      return true;
   }
   return atomic_dec(&references);
#else
   mRO.lock();
   int rc = --references;
   mRO.unlock();
   return !rc;
#endif
}
