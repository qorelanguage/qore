/* 
  SmartMutex.h

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

#ifndef _QORE_SMARTMUTEX

#define _QORE_SMARTMUTEX

#include <qore/Qore.h>
#include <qore/intern/AbstractSmartLock.h>
#include <qore/QoreCondition.h>

#include <map>

// track all external condition variables waiting on this
// so we can signal them in case the object is destroyed

class SmartMutex : public AbstractSmartLock {
   private:
      cond_map_t cmap;

      DLLLOCAL virtual int releaseImpl();
      DLLLOCAL virtual int grabImpl(int mtid, class VLock *nvl, class ExceptionSink *xsink, int timeout_ms = 0);
      DLLLOCAL virtual int releaseImpl(class ExceptionSink *xsink);
      DLLLOCAL virtual int tryGrabImpl(int mtid, class VLock *nvl);
      DLLLOCAL virtual int externWaitImpl(int mtid, class QoreCondition *cond, class ExceptionSink *xsink, int timeout = 0);
      DLLLOCAL virtual void destructorImpl(class ExceptionSink *xsink);

   public:
      DLLLOCAL SmartMutex() {}
#ifdef DEBUG
      DLLLOCAL virtual ~SmartMutex();
#endif

      DLLLOCAL bool owns_lock();
      DLLLOCAL virtual const char *getName() const { return "Mutex"; }
      // returns how many condition variables are waiting on this mutex
      DLLLOCAL int cond_count(class QoreCondition *cond);
};

#endif // _QORE_SMARTMUTEX
