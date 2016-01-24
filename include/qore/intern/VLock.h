/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 VLock.h
  
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

#ifndef _QORE_VLOCK_H

#define _QORE_VLOCK_H

#include <qore/Qore.h>
#include <qore/intern/AbstractSmartLock.h>

#include <vector>
#include <map>

typedef std::map<int, class VLock *> vlock_map_t;

// testing shows that a vector is slightly faster than a deque for this usage
// and must faster than a list
typedef std::vector<AbstractSmartLock *> abstract_lock_list_t;

// for tracking locks per thread and detecting deadlocks
class VLock : protected abstract_lock_list_t
{
   private:
      AbstractSmartLock *waiting_on;   // the lock this object is waiting on
      int tid;

      // not implemented
      VLock(const VLock&);
      VLock& operator=(const VLock&);
      
   public:
      DLLLOCAL VLock(int n_tid);
      DLLLOCAL ~VLock();
      DLLLOCAL void push(AbstractSmartLock *g);
      DLLLOCAL int pop(AbstractSmartLock *asl);
      DLLLOCAL void del();
      DLLLOCAL AbstractSmartLock *find(AbstractSmartLock *g) const; 

      // for blocking smart locks with deadlock detection
      DLLLOCAL int waitOn(AbstractSmartLock *asl, class VLock *vl, class ExceptionSink *xsink, int timeout_ms = 0);
      // for smart locks using an alternate condition variable
      DLLLOCAL int waitOn(AbstractSmartLock *asl, class QoreCondition *cond, class VLock *vl, class ExceptionSink *xsink, int timeout_ms = 0);
      // for smart locks that can be held by more than one thread
      DLLLOCAL int waitOn(AbstractSmartLock *asl, vlock_map_t &vmap, class ExceptionSink *xsink, int timeout_ms = 0);
      DLLLOCAL int getTID() const { return tid; }
         
#ifdef DEBUG
      DLLLOCAL void show(class VLock *nvl) const; 
#endif
};

#endif
