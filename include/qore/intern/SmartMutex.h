/* -*- mode: c++; indent-tabs-mode: nil -*- */
/* 
  SmartMutex.h

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

#ifndef _QORE_SMARTMUTEX

#define _QORE_SMARTMUTEX

#include <qore/Qore.h>
#include "qore/intern/AbstractSmartLock.h"
#include <qore/QoreCondition.h>

#include <map>

// track all external condition variables waiting on this
// so we can signal them in case the object is destroyed

class SmartMutex : public AbstractSmartLock {
private:
   DLLLOCAL virtual int releaseImpl();
   DLLLOCAL virtual int grabImpl(int mtid, VLock *nvl, ExceptionSink *xsink, int64 timeout_ms = 0);
   DLLLOCAL virtual int releaseImpl(ExceptionSink *xsink);
   DLLLOCAL virtual int tryGrabImpl(int mtid, VLock *nvl);
   DLLLOCAL virtual int externWaitImpl(int mtid, QoreCondition *cond, ExceptionSink *xsink, int64 timeout = 0);
   DLLLOCAL virtual void destructorImpl(ExceptionSink *xsink);
   
public:
   DLLLOCAL SmartMutex() {}
#ifdef DEBUG
   DLLLOCAL virtual ~SmartMutex();
#endif
   
   DLLLOCAL bool owns_lock();
   DLLLOCAL virtual const char *getName() const { return "Mutex"; }
};

#endif // _QORE_SMARTMUTEX
