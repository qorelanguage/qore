/*
  AbstractSmartLock.cpp
 
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
#include <qore/intern/AbstractSmartLock.h>

void AbstractSmartLock::cleanupImpl() {
   if (tid == gettid())
      release_and_signal();
}

void AbstractSmartLock::cleanup(ExceptionSink *xsink) {
   xsink->raiseException("LOCK-ERROR", "TID %d terminated while holding a %s lock; the lock will be automatically released", gettid(), getName());
   AutoLocker al(&asl_lock);	    
   cleanupImpl();
}

void AbstractSmartLock::mark_and_push(int mtid, VLock *nvl) {
   nvl->push(this);
   
   tid = mtid;
   vl = nvl;
}

void AbstractSmartLock::signalAllImpl() {
   if (waiting)
      asl_cond.broadcast();
}

void AbstractSmartLock::signalImpl() {
   if (waiting)
      asl_cond.signal();
}

void AbstractSmartLock::release_and_signal() {
   vl->pop(this);
   
   if (tid >= 0)
      tid = Lock_Unlocked;
   vl = 0;
   signalImpl();
}

void AbstractSmartLock::grab_intern(int mtid, VLock *nvl) {
   printd(5, "AbstractSmartLock::grab_intern() (%s) this: %p grabbed lock (nvl: %p)\n", getName(), this, nvl);
   mark_and_push(mtid, nvl);
   set_thread_resource(this);
}
      
void AbstractSmartLock::release_intern() {
   remove_thread_resource(this);
   release_and_signal();
}

void AbstractSmartLock::destructorImpl(ExceptionSink *xsink) {
}

void AbstractSmartLock::destructor(ExceptionSink *xsink) {
   AutoLocker al(&asl_lock);
   destructorImpl(xsink);
   if (tid >= 0) {
      vl->pop(this);
      
      int mtid = gettid();
      if (mtid == tid) {
	 xsink->raiseException("LOCK-ERROR", "TID %d deleted %s object while holding the lock", mtid, getName());
	 remove_thread_resource(this);
      }
      else
	 xsink->raiseException("LOCK-ERROR", "TID %d deleted %s object while TID %d was holding the lock", mtid, getName(), tid);

      signalAllImpl();
   }
   tid = Lock_Deleted;
}

// grab return values: 
//    0   = grabbed the lock
//    > 0 = acquired the lock recursively (was already acquired by this thread)
//    < 0 = error occured (deadlock or timeout)
/*
int AbstractSmartLock::grabIntern(ExceptionSink *xsink) {
int mtid = gettid();
AutoLocker al(&asl_lock);
return grabInternImpl(mtid);
}
*/

int AbstractSmartLock::grab(ExceptionSink *xsink, int64 timeout_ms) {
   int mtid = gettid();
   
   VLock *nvl = getVLock();
   AutoLocker al(&asl_lock);
   int rc = grabImpl(mtid, nvl, xsink, timeout_ms);
   if (!rc)
      grab_intern(mtid, nvl);
   return rc;
}

int AbstractSmartLock::tryGrab() {
   int mtid = gettid();
   VLock *nvl = getVLock();
   AutoLocker al(&asl_lock);
   int rc = tryGrabImpl(mtid, nvl);
   if (!rc)
      grab_intern(mtid, nvl);
   return rc;
}

int AbstractSmartLock::release() {
   AutoLocker al(&asl_lock);
   int rc = releaseImpl();
   if (!rc)
      release_and_signal();
   return rc;
}

int AbstractSmartLock::release(ExceptionSink *xsink) {
   AutoLocker al(&asl_lock);
   int rc = releaseImpl(xsink);
   if (!rc)
      release_intern();
   return rc;
}

int AbstractSmartLock::externWaitImpl(int mtid, QoreCondition *cond, ExceptionSink *xsink, int64 timeout_ms) {
   xsink->raiseException("WAIT-ERROR", "cannot wait on %s objects", getName());
   return -1;
}

int AbstractSmartLock::extern_wait(QoreCondition *cond, ExceptionSink *xsink, int64 timeout_ms) {
   AutoLocker al(&asl_lock);
   return externWaitImpl(gettid(), cond, xsink, timeout_ms);
}

int AbstractSmartLock::verify_wait_unlocked(int mtid, ExceptionSink *xsink) {
   if (tid == mtid)
      return 0;
   if (tid < 0)
      xsink->raiseException("WAIT-ERROR", "wait() called with unlocked %s argument", getName());
   else
      xsink->raiseException("WAIT-ERROR", "TID %d called wait() with %s lock argument held by TID %d", mtid, getName(), tid);
   return -1;
}
