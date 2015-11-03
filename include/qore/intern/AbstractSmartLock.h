/*
 AbstractSmartLock.h
 
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

#ifndef _QORE_ABSTRACTSMARTLOCK_H

#define _QORE_ABSTRACTSMARTLOCK_H

#include <qore/Qore.h>
#include <qore/QoreThreadLock.h>
#include <qore/QoreCondition.h>
#include <qore/AbstractThreadResource.h>

class VLock;

class AbstractSmartLock : public AbstractThreadResource {
   protected:
      enum lock_status_e { Lock_Deleted = -2, Lock_Unlocked = -1 };
   
      VLock *vl;
      int tid, waiting;

      virtual int releaseImpl() = 0;
      virtual int releaseImpl(ExceptionSink *xsink) = 0;
      virtual int grabImpl(int mtid, VLock *nvl, ExceptionSink *xsink, int timeout_ms = 0) = 0;
      virtual int tryGrabImpl(int mtid, VLock *nvl) = 0;

      DLLLOCAL virtual int externWaitImpl(int mtid, class QoreCondition *cond, ExceptionSink *xsink, int timeout_ms = 0);
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
      QoreThreadLock asl_lock;
      QoreCondition asl_cond;

      DLLLOCAL AbstractSmartLock() : vl(NULL), tid(-1), waiting(0)  {}
      DLLLOCAL virtual ~AbstractSmartLock() {}
      DLLLOCAL void destructor(ExceptionSink *xsink);
      DLLLOCAL virtual void cleanup(ExceptionSink *xsink);

      DLLLOCAL int grab(ExceptionSink *xsink, int timeout_ms = 0);
      DLLLOCAL int tryGrab();
      DLLLOCAL int release();
      DLLLOCAL int release(ExceptionSink *xsink);

      DLLLOCAL int self_wait(int timeout_ms) { 
	 return timeout_ms ? asl_cond.wait(&asl_lock, timeout_ms) : asl_cond.wait(&asl_lock); 
      }

      DLLLOCAL int self_wait(QoreCondition *cond, int timeout_ms = 0) { 
	 return timeout_ms ? cond->wait(&asl_lock, timeout_ms) : cond->wait(&asl_lock); 
      }

      DLLLOCAL int extern_wait(QoreCondition *cond, ExceptionSink *xsink, int timeout_ms = 0);

      DLLLOCAL int get_tid() const { return tid; }
      DLLLOCAL int get_waiting() const { return waiting; }
      DLLLOCAL virtual const char *getName() const = 0;
};

#endif
