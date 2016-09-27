/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  VRMutex.h

  recursive lock object

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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

#ifndef _QORE_VRMUTEX_H

#define _QORE_VRMUTEX_H

#include <qore/intern/AbstractSmartLock.h>
#include <qore/intern/VLock.h>

// reentrant thread lock using tiered locking and deadlock detection infrastructure
class VRMutex : public AbstractSmartLock {
private:
   int count;

   DLLLOCAL virtual int releaseImpl();
   DLLLOCAL virtual int releaseImpl(ExceptionSink *xsink);
   DLLLOCAL virtual int grabImpl(int mtid, VLock *nvl, ExceptionSink *xsink, int64 timeout_ms = 0);
   DLLLOCAL virtual int tryGrabImpl(int mtid, VLock *nvl);
   DLLLOCAL virtual void cleanupImpl();

public:
   DLLLOCAL VRMutex();

   // grabs the lock recursively, returns 0 for OK, non-zero for error
   DLLLOCAL int enter(ExceptionSink *xsink);

   // releases the lock, returns 0 if the lock is unlocked, -1 if there are still more calls to exit
   DLLLOCAL int exit();

   DLLLOCAL virtual const char* getName() const { return "VRMutex"; }

   DLLLOCAL int get_count() const {
      return count;
   }
};

class VRMutexOptionalLockHelper {
protected:
   VRMutex* m;

public:
   DLLLOCAL VRMutexOptionalLockHelper(VRMutex* vm, ExceptionSink* xsink) : m(vm && !vm->enter(xsink) ? vm : 0) {
      //printd(5, "VRMutexOptionalLockHelper::VRMutexOptionalLockHelper() vm: %p m: %p\n", vm, m);
   }

   DLLLOCAL ~VRMutexOptionalLockHelper() {
      if (m)
         m->exit();
   }
};

#endif
