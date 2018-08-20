/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AbstractSmartLock.h
 
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

#ifndef _QORE_ABSTRACTSMARTLOCK_H

#define _QORE_ABSTRACTSMARTLOCK_H

#include <qore/Qore.h>
#include <qore/QoreThreadLock.h>
#include <qore/QoreCondition.h>
#include <qore/AbstractThreadResource.h>

class VLock;

class QoreCondition;

class AbstractSmartLock : public AbstractThreadResource {
protected:
   enum lock_status_e { Lock_Deleted = -2, Lock_Unlocked = -1 };
   
   VLock *vl;
   int tid, waiting;
   cond_map_t cmap;       // map of condition variables to wait counts

   virtual int releaseImpl() = 0;
   virtual int releaseImpl(ExceptionSink *xsink) = 0;
   virtual int grabImpl(int mtid, VLock *nvl, ExceptionSink *xsink, int64 timeout_ms = 0) = 0;
   virtual int tryGrabImpl(int mtid, VLock *nvl) = 0;

   DLLLOCAL virtual int externWaitImpl(int mtid, QoreCondition *cond, ExceptionSink *xsink, int64 timeout_ms = 0);
   DLLLOCAL virtual void destructorImpl(ExceptionSink *xsink);
   DLLLOCAL virtual void signalAllImpl();
   DLLLOCAL virtual void signalImpl();
   DLLLOCAL virtual void cleanupImpl();

   DLLLOCAL void mark_and_push(int mtid, VLock *nvl);
   DLLLOCAL void release_and_signal();
   DLLLOCAL void grab_intern(int mtid, VLock *nvl);
   DLLLOCAL void release_intern();
   DLLLOCAL int verify_wait_unlocked(int mtid, ExceptionSink *xsink);

public:
   mutable QoreThreadLock asl_lock;
   QoreCondition asl_cond;

   DLLLOCAL AbstractSmartLock() : vl(NULL), tid(-1), waiting(0)  {}
   DLLLOCAL virtual ~AbstractSmartLock() {}
   DLLLOCAL void destructor(ExceptionSink *xsink);
   DLLLOCAL virtual void cleanup(ExceptionSink *xsink);

   DLLLOCAL int grab(ExceptionSink *xsink, int64 timeout_ms = 0);
   DLLLOCAL int tryGrab();
   DLLLOCAL int release();
   DLLLOCAL int release(ExceptionSink *xsink);

   DLLLOCAL int self_wait(int64 timeout_ms) { 
      return timeout_ms ? asl_cond.wait(&asl_lock, timeout_ms) : asl_cond.wait(&asl_lock); 
   }

   DLLLOCAL int self_wait(QoreCondition *cond, int64 timeout_ms = 0) { 
      return timeout_ms ? cond->wait(&asl_lock, timeout_ms) : cond->wait(&asl_lock); 
   }

   DLLLOCAL int extern_wait(QoreCondition *cond, ExceptionSink *xsink, int64 timeout_ms = 0);

   DLLLOCAL int get_tid() const { return tid; }
   DLLLOCAL int get_waiting() const { return waiting; }
   DLLLOCAL virtual const char *getName() const = 0;
   DLLLOCAL int cond_count(QoreCondition *cond) const {
      AutoLocker al(&asl_lock);
      cond_map_t::const_iterator i = cmap.find(cond);
      return i != cmap.end() ? i->second : 0;
   }
};

#endif
