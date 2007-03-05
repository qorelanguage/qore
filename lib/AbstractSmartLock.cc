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
   release_intern_intern();
}

void AbstractSmartLock::grab_intern_intern(int mtid, class VLock *nvl)
{
   tid = mtid;
   vl = nvl;
   nvl->push(this);
}

void AbstractSmartLock::release_intern_intern()
{
   vl->pop(this);
   tid = -1;
   vl = NULL;
   if (waiting)
      asl_cond.signal();
}

void AbstractSmartLock::grab_intern(int mtid, class VLock *nvl)
{
   printd(5, "AbstractSmartLock::grab() (%s) this=%08p grabbed lock (nvl=%08p)\n", getName(), this, nvl);
   grab_intern_intern(mtid, nvl);
   trlist.set(this, (qtrdest_t)abstract_smart_lock_cleanup);
}
      
void AbstractSmartLock::release_intern()
{
   trlist.remove(this, tid);
   release_intern_intern();
}

void AbstractSmartLock::destructor(class ExceptionSink *xsink)
{
   AutoLocker al(&asl_lock);	    
   if (tid >= 0)
   {
      vl->pop(this);
      xsink->raiseException("LOCK-ERROR", "%s object destroyed while locked by TID %d", getName(), gettid());
      tid = -2;
      trlist.remove(this);
      asl_cond.broadcast();
   }   
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
      release_intern_intern();
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
