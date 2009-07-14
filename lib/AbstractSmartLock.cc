/*
 AbstractSmartLock.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
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
#include <qore/intern/AbstractSmartLock.h>

void AbstractSmartLock::cleanupImpl()
{
   if (tid == gettid())
      release_and_signal();
}

void AbstractSmartLock::cleanup(ExceptionSink *xsink)
{
   xsink->raiseException("LOCK-ERROR", "TID %d terminated while holding a %s lock; the lock will be automatically released", gettid(), getName());
   AutoLocker al(&asl_lock);	    
   cleanupImpl();
}

void AbstractSmartLock::mark_and_push(int mtid, class VLock *nvl)
{
   nvl->push(this);
   
   tid = mtid;
   vl = nvl;
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
      tid = Lock_Unlocked;
   vl = 0;
   signalImpl();
}

void AbstractSmartLock::grab_intern(int mtid, class VLock *nvl)
{
   printd(5, "AbstractSmartLock::grab_intern() (%s) this=%08p grabbed lock (nvl=%08p)\n", getName(), this, nvl);
   mark_and_push(mtid, nvl);
   set_thread_resource(this);
}
      
void AbstractSmartLock::release_intern()
{
   remove_thread_resource(this);
   release_and_signal();
}

void AbstractSmartLock::destructorImpl(ExceptionSink *xsink)
{
}

void AbstractSmartLock::destructor(ExceptionSink *xsink)
{
   AutoLocker al(&asl_lock);
   destructorImpl(xsink);
   if (tid >= 0)
   {
      vl->pop(this);
      
      int mtid = gettid();
      if (mtid == tid)
      {
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
int AbstractSmartLock::grabIntern(ExceptionSink *xsink)
{
int mtid = gettid();
AutoLocker al(&asl_lock);
return grabInternImpl(mtid);
}
*/

int AbstractSmartLock::grab(ExceptionSink *xsink, int timeout_ms)
{
   int mtid = gettid();
   
   class VLock *nvl = getVLock();
   AutoLocker al(&asl_lock);
   int rc = grabImpl(mtid, nvl, xsink, timeout_ms);
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

int AbstractSmartLock::release(ExceptionSink *xsink)
{
   AutoLocker al(&asl_lock);
   int rc = releaseImpl(xsink);
   if (!rc)
      release_intern();
   return rc;
}

int AbstractSmartLock::externWaitImpl(int mtid, QoreCondition *cond, ExceptionSink *xsink, int timeout_ms)
{
   xsink->raiseException("WAIT-ERROR", "cannot wait on %s objects", getName());
   return -1;
}

int AbstractSmartLock::extern_wait(QoreCondition *cond, ExceptionSink *xsink, int timeout_ms) {
   AutoLocker al(&asl_lock);
   return externWaitImpl(gettid(), cond, xsink, timeout_ms);
}

int AbstractSmartLock::verify_wait_unlocked(int mtid, ExceptionSink *xsink)
{
   if (tid == mtid)
      return 0;
   if (tid < 0)
      xsink->raiseException("WAIT-ERROR", "wait() called with unlocked %s argument", getName());
   else
      xsink->raiseException("WAIT-ERROR", "TID %d called wait() with %s lock argument held by TID %d", mtid, getName(), tid);
   return -1;
}
