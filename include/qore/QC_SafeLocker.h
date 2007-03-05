/*
 QC_SafeLocker.h
 
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

#ifndef _QORE_QC_SAFELOCKER_H

#define _QORE_QC_SAFELOCKER_H

#include <qore/Qore.h>
#include <qore/QC_Mutex.h>

DLLEXPORT extern int CID_SAFELOCKER;

DLLLOCAL class QoreClass *initSafeLockerClass();

class QoreSafeLocker : public AbstractPrivateData
{
   class Mutex *m;
   bool locked;

public:
   DLLLOCAL QoreSafeLocker(class Mutex *mt, class ExceptionSink *xsink)
   {
      m = mt;
      m->grab(xsink);
      if (!*xsink)
	 locked = true;
   }

   DLLLOCAL virtual void deref(class ExceptionSink *xsink) 
   {
      if (ROdereference())
      {
	 if (locked)
	    m->release(xsink);
	 m->deref(xsink);
      }
   }
   
   DLLLOCAL int lock(class ExceptionSink *xsink)
   {
      if (locked)
      {
	 xsink->raiseException("SAFELOCKER-ERROR", "SafeLocker::lock() called while lock already held");
	 return -1;
      }
      int rc = m->grab(xsink);
      if (!rc)
	 locked = true;
      return rc;
   }
   DLLLOCAL int unlock(class ExceptionSink *xsink)
   {
      if (!locked)
      {
	 xsink->raiseException("SAFELOCKER-ERROR", "SafeLocker::unlock() called while lock not held");
	 return -1;
      }
      // set locked to false here in every case, so we don't try again if there is an error
      locked = false;
      return m->release(xsink);
   }
   DLLLOCAL int trylock()
   {
      return m->tryGrab();
   }
};


#endif
