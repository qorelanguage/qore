/*
 AbstractSmartLock.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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

#include <qore/Qore.h>
#include <qore/AbstractSmartLock.h>
#include <qore/VLock.h>

void abstract_smart_lock_cleanup(AbstractSmartLock *asl, class ExceptionSink *xsink)
{
   xsink->raiseException("THREAD-ERROR", "TID %d terminated while holding a %s lock; the lock will be automatically released", gettid(), asl->getName());
   asl->cleanup();
}

void AbstractSmartLock::cleanup()
{
   AutoLocker al(&asl_lock);	    
   release_and_signal();
}

void AbstractSmartLock::mark_and_push(int mtid, class VLock *nvl)
{
   tid = mtid;
   vl = nvl;
   nvl->push(this);
}

void AbstractSmartLock::signalAllImpl()
{
   if (waiting)
      asl_cond.broadcast();
}

void AbstractSmartLock::signalImpl()
{
   if (waiting)
      asl_cond.signal();
}

void AbstractSmartLock::release_and_signal()
{
   vl->pop(this);
   if (tid >= 0)
      tid = -1;
   vl = NULL;
   signalImpl();
}

void AbstractSmartLock::grab_intern(int mtid, class VLock *nvl)
{
   printd(5, "AbstractSmartLock::grab() (%s) this=%08p grabbed lock (nvl=%08p)\n", getName(), this, nvl);
   mark_and_push(mtid, nvl);
   trlist.set(this, (qtrdest_t)abstract_smart_lock_cleanup);
}
      
void AbstractSmartLock::release_intern()
{
   trlist.remove(this, tid);
   release_and_signal();
}

void AbstractSmartLock::destructorImpl(class ExceptionSink *xsink)
{
}

void AbstractSmartLock::destructor(class ExceptionSink *xsink)
{
   AutoLocker al(&asl_lock);
   destructorImpl(xsink);
   if (tid >= 0)
   {
      vl->pop(this);
      xsink->raiseException("LOCK-ERROR", "%s object destroyed while locked by TID %d", getName(), gettid());
      trlist.remove(this);
      signalAllImpl();
   }
   tid = -2;
}

// grab return values: 
//    0   = grabbed the lock
//    > 0 = acquired the lock recursively (was already acquired by this thread)
//    < 0 = error occured (deadlock or timeout)
/*
int AbstractSmartLock::grabIntern(class ExceptionSink *xsink)
{
int mtid = gettid();
AutoLocker al(&asl_lock);
return grabInternImpl(mtid);
}
*/

int AbstractSmartLock::grab(class ExceptionSink *xsink)
{
   int mtid = gettid();
   class VLock *nvl = getVLock();
   AutoLocker al(&asl_lock);
   int rc = grabImpl(mtid, nvl, xsink);
   if (!rc)
      grab_intern(mtid, nvl);
   return rc;
}

int AbstractSmartLock::grab(int timeout_ms, class ExceptionSink *xsink)
{
   int mtid = gettid();
   class VLock *nvl = getVLock();
   AutoLocker al(&asl_lock);
   int rc = grabImpl(mtid, timeout_ms, nvl, xsink);
   if (!rc)
      grab_intern(mtid, nvl);
   return rc;
}

int AbstractSmartLock::tryGrab()
{
   int mtid = gettid();
   class VLock *nvl = getVLock();
   AutoLocker al(&asl_lock);
   int rc = tryGrabImpl(mtid, nvl);
   if (!rc)
      grab_intern(mtid, nvl);
   return rc;
}

int AbstractSmartLock::release()
{
   AutoLocker al(&asl_lock);
   int rc = releaseImpl();
   if (!rc)
      release_and_signal();
   return rc;
}

int AbstractSmartLock::release(class ExceptionSink *xsink)
{
   AutoLocker al(&asl_lock);
   int rc = releaseImpl(xsink);
   if (!rc)
      release_intern();
   return rc;
}

int AbstractSmartLock::externWaitImpl(int mtid, class QoreCondition *cond, int timeout, class ExceptionSink *xsink)
{
   xsink->raiseException("WAIT-ERROR", "cannot wait on %s objects", getName());
   return -1;
}

int AbstractSmartLock::externWaitImpl(int mtid, class QoreCondition *cond, class ExceptionSink *xsink)
{
   xsink->raiseException("WAIT-ERROR", "cannot wait on %s objects", getName());
   return -1;
}

int AbstractSmartLock::extern_wait(class QoreCondition *cond, int timeout, class ExceptionSink *xsink)
{
   AutoLocker al(&asl_lock);
   return externWaitImpl(gettid(), cond, timeout, xsink);
}

int AbstractSmartLock::extern_wait(class QoreCondition *cond, class ExceptionSink *xsink)
{
   AutoLocker al(&asl_lock);
   return externWaitImpl(gettid(), cond, xsink);
}


int AbstractSmartLock::verify_wait_unlocked(int mtid, class ExceptionSink *xsink)
{
   if (tid == mtid)
      return 0;
   if (tid < 0)
      xsink->raiseException("WAIT-ERROR", "wait() called with unlocked %s argument", getName());
   else
      xsink->raiseException("WAIT-ERROR", "TID %d called wait() with %s lock argument held by TID %d", mtid, getName(), tid);
   return -1;
}
