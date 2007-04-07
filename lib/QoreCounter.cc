/*
  QoreCounter.cc

  Qore Programming Language

  Copyright (C) David Nichols 2005

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
#include <qore/QoreCounter.h>
#include <qore/QoreSignal.h>

QoreCounter::QoreCounter(int nc) : cnt(nc), waiting(0)
{
}

QoreCounter::~QoreCounter()
{
}

void QoreCounter::destructor(class ExceptionSink *xsink)
{
   AutoLocker al(&l);
   cnt = Cond_Deleted;
   if (waiting)
   {
      xsink->raiseException("COUNTER-ERROR", "Counter deleted while there %s %d waiting thread%s",
			    waiting == 1 ? "is" : "are", waiting, waiting == 1 ? "" : "s");
      cond.broadcast();
   }
}

void QoreCounter::inc()
{
   AutoLocker al(&l);
   if (cnt >= 0)
      cnt++;
}

void QoreCounter::dec(class ExceptionSink *xsink)
{
   AutoLocker al(&l);
   if (cnt == Cond_Deleted)
   {
      xsink->raiseException("COUNTER-ERROR", "Counter has been deleted in another thread");
      return;
   }
   if (!--cnt && waiting)
      cond.broadcast();
}

int QoreCounter::waitForZero(class ExceptionSink *xsink, int timeout_ms)
{
   // NOTE that we do not do a while(true) { cond.wait(); } because any broadcast means that the
   // counter hit zero, so even it it's bigger than zero by the time we are allowed to execute, it's ok
   // --- synchronization must be done externally
   SafeLocker sl(&l);
   ++waiting;
   while (cnt && cnt != Cond_Deleted)
   {
      if (!timeout_ms)
	 cond.wait(&l);
      else
	 if (cond.wait(&l, timeout_ms))
	    break;
      // check for signals to be handled on a spurious wakeup
      if (cnt && cnt != Cond_Deleted)
      {
	 sl.unlock();
	 QoreSignalManager::handleSignals();
	 sl.lock();
      }
   }
   --waiting;
   if (cnt == Cond_Deleted)
   {
      xsink->raiseException("COUNTER-ERROR", "Counter was deleted in another thread while waiting");
      return -1;
   }
   return 0;
}

void QoreCounter::waitForZero()
{
   // NOTE that we do not do a while(true) { cond.wait(); } because any broadcast means that the
   // counter hit zero, so even it it's bigger than zero by the time we are allowed to execute, it's ok
   // --- synchronization must be done externally
   SafeLocker sl(&l);
   ++waiting;
   while (cnt)
   {
      cond.wait(&l);
      // check for signals to be handled on a spurious wakeup
      if (cnt)
      {
	 sl.unlock();
	 QoreSignalManager::handleSignals();
	 sl.lock();
      }
   }
   --waiting;
}

void QoreCounter::dec()
{
   AutoLocker al(&l);
   if (!--cnt && waiting)
      cond.broadcast();
}
