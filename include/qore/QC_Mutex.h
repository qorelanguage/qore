/* 
   QC_Mutex.h

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

#ifndef _QORE_CLASS_MUTEX

#define _QORE_CLASS_MUTEX

#include <qore/Qore.h>
#include <qore/AbstractSmartLock.h>

DLLEXPORT extern int CID_MUTEX;

DLLLOCAL class QoreClass *initMutexClass();

class Mutex : public AbstractPrivateData, public AbstractSmartLock
{
   private:

      DLLLOCAL virtual int releaseImpl()
      {
	 if (tid < 0)
	    return -1;
	 return 0;
      }

      DLLLOCAL virtual int grabImpl(int mtid, class VLock *nvl, class ExceptionSink *xsink)
      {
	 if (tid != -1)
	 {
	    if (tid == mtid)
	    {
	       // use getName for possible inheritance
	       xsink->raiseException("LOCK-ERROR", "TID %d called %s::lock() twice without an intervening %s::unlock()", tid, getName(), getName());
	       return -1;
	    }
	    while (tid >= 0)
	    {
	       waiting++;
	       int rc =  nvl->waitOn((AbstractSmartLock *)this, vl, mtid, xsink);
	       waiting--;
	       if (rc)
		  return -1;
	    }
	 }
	 if (tid == -2)
	 {
	    // use getName for possible inheritance
	    xsink->raiseException("LOCK-ERROR", "%s has been deleted in another thread", getName());
	    return -1;
	 }

	 return 0;
      }

      DLLLOCAL virtual int grabImpl(int mtid, int timeout_ms, class VLock *nvl, class ExceptionSink *xsink)
      {
	 if (tid != -1)
	 {
	    if (tid == mtid)
	    {
	       // getName() for possible inheritance
	       xsink->raiseException("LOCK-ERROR", "TID %d called %s::lock() twice without an intervening %s::unlock()", tid, getName(), getName());
	       return -1;
	    }
	    while (tid >= 0)
	    {
	       waiting++;
	       int rc =  nvl->waitOn((AbstractSmartLock *)this, vl, mtid, timeout_ms, xsink);
	       waiting--;
	       if (rc)
		  return -1;
	    }
	 }
	 if (tid == -2)
	 {
	    // getName() for possible inheritance
	    xsink->raiseException("LOCK-ERROR", "%s has been deleted in another thread", getName());
	    return -1;
	 }
	 return 0;
      }
      DLLLOCAL virtual int releaseImpl(class ExceptionSink *xsink)
      {
	 int mtid = gettid();
	 if (tid < 0)
	 {
	    // getName() for possible inheritance
	    xsink->raiseException("LOCK-ERROR", "TID %d called %s::unlock() while the lock was already unlocked", mtid, getName());
	    return -1;
	 }
	 if (tid != mtid)
	 {
	    // getName() for possible inheritance
	    xsink->raiseException("LOCK-ERROR", "TID %d called %s::unlock() while the lock is held by tid %d", mtid, tid, getName());
	    return -1;
	 }
	 return 0;
      }

      DLLLOCAL virtual int tryGrabImpl(int mtid, class VLock *nvl)
      {
	 if (tid != -1)
	    return -1;
	 return 0;
      }

   public:
      DLLLOCAL Mutex() {}

      DLLLOCAL int verify_lock_tid(char *meth, class ExceptionSink *xsink)
      {
	 AutoLocker al(&asl_lock);
	 int mtid = gettid();
	 if (tid == mtid)
	    return 0;
	 if (tid < 0)
	    xsink->raiseException("LOCK-ERROR", "%s() with unlocked %s argument", meth, getName());
	 else
	    xsink->raiseException("LOCK-ERROR", "TID called %s with %s lock argument held by TID %d", mtid, getName(), tid);
	 return -1;
      }
      DLLLOCAL virtual const char *getName() const { return "Mutex"; }
};

#endif // _QORE_CLASS_MUTEX
