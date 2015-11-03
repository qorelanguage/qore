/*
 QC_AutoLock.h
 
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

#ifndef _QORE_QC_AUTOLOCK_H

#define _QORE_QC_AUTOLOCK_H

#include <qore/Qore.h>
#include <qore/intern/QC_Mutex.h>

DLLEXPORT extern qore_classid_t CID_AUTOLOCK;

DLLLOCAL class QoreClass *initAutoLockClass();

class QoreAutoLock : public AbstractPrivateData
{
   class SmartMutex *m;

public:
   DLLLOCAL QoreAutoLock(class SmartMutex *mt, class ExceptionSink *xsink)
   {
      m = mt;
      m->grab(xsink);
   }

   DLLLOCAL virtual void deref(class ExceptionSink *xsink) 
   {
      if (ROdereference())
      {
	 m->deref(xsink);
	 delete this;
      }
   }

   DLLLOCAL virtual void destructor(class ExceptionSink *xsink) 
   {
      if (m->owns_lock())
	 m->release(xsink);
   }
   
   DLLLOCAL int lock(class ExceptionSink *xsink)
   {
      return m->grab(xsink);
   }
   DLLLOCAL int unlock(class ExceptionSink *xsink)
   {
      return m->release(xsink);
   }
   DLLLOCAL int trylock()
   {
      return m->tryGrab();
   }
};


#endif
