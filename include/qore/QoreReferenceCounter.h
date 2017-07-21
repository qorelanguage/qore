/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreReferenceCounter.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_QOREREFERENCECOUNTER_H

#define _QORE_QOREREFERENCECOUNTER_H

#include <qore/common.h>
#include <qore/macros.h>

class QoreThreadLock;

//! provides atomic reference counting to Qore objects
class QoreReferenceCounter {
protected:
   mutable int references;
#ifndef HAVE_ATOMIC_MACROS
   //! pthread lock to ensure atomicity of updates for architectures where we don't have an atomic increment and decrement implementation
   mutable QoreThreadLock mRO;
#endif

public:
   //! creates the reference counter object
   DLLEXPORT QoreReferenceCounter();

   //! creates a new object with a reference count of 1
   /** @since %Qore 0.8.12.9
    */
   DLLEXPORT QoreReferenceCounter(const QoreReferenceCounter& old);

   //! destroys the reference counter object
   DLLEXPORT ~QoreReferenceCounter();

   //! gets the reference count
   /**
      @return returns the current reference count
   */
   DLLLOCAL int reference_count() const {
      return references;
   }

   //! returns true if the reference count is 1
   /**
      @return returns true if the reference count is 1
   */
   DLLLOCAL bool is_unique() const {
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
