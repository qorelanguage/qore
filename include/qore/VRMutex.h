/*
  VRMutex.h

  recursive lock object

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

#ifndef _QORE_VRMUTEX_H

#define _QORE_VRMUTEX_H

#include <qore/AbstractSmartLock.h>
#include <qore/VLock.h>

// reentrant thread lock using tiered locking and deadlock detection infrastructure
class VRMutex : public AbstractSmartLock
{
   private:
      int count;

      DLLLOCAL virtual int releaseImpl();
      DLLLOCAL virtual int releaseImpl(class ExceptionSink *xsink);
      DLLLOCAL virtual int grabImpl(int mtid, class VLock *nvl, class ExceptionSink *xsink, int timeout_ms = 0);
      DLLLOCAL virtual int tryGrabImpl(int mtid, class VLock *nvl);
      DLLLOCAL virtual void cleanupImpl();

   public:
      DLLLOCAL VRMutex();
      DLLLOCAL int enter(class ExceptionSink *);
      DLLLOCAL int exit();
      DLLLOCAL virtual const char *getName() const { return "VRMutex"; }
      DLLLOCAL int get_count() const { return count; }
};

#endif
