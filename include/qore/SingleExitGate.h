/* 
   SingleExitGate.h
   
   re-entrant thread lock

   Qore Programming Language

   Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#ifndef _QORE_SINGLEEXITGATE_H

#define _QORE_SINGLEEXITGATE_H

#include <pthread.h>
#include <sys/time.h>

static inline int gettid();

class SingleExitGate
{
   private:
      int tid, waiting;
      pthread_mutex_t m;
      pthread_cond_t cwait;

   public:
      inline SingleExitGate();
      inline ~SingleExitGate();
      inline int enter(int timeout = 0);
      inline int tryEnter();
      inline int exit();
      inline int forceExit();
      inline int getLockTID() { return tid; }
};

inline SingleExitGate::SingleExitGate()
{
   pthread_mutex_init(&m, NULL);
   pthread_cond_init(&cwait, NULL);
   tid = -1;
   waiting = 0;
}

inline SingleExitGate::~SingleExitGate()
{
   pthread_mutex_destroy(&m);
   pthread_cond_destroy(&cwait);
}

inline int SingleExitGate::enter(int timeout)
{
   int ctid = gettid();

   //fprintf(stderr, "SingleExitGate::enter(%d) %08p\n", c, this);
   pthread_mutex_lock(&m);

   while (tid != -1 && tid != ctid)
   {
      waiting++;
      if (timeout)
	 while (true)
	 {
	    struct timeval now;
	    struct timespec tmout;
	    
	    gettimeofday(&now, NULL);
	    tmout.tv_sec = now.tv_sec + timeout;
	    tmout.tv_nsec = now.tv_usec * 1000;
	    
	    if (!pthread_cond_timedwait(&cwait, &m, &tmout))
	       break;

	    // lock has timed out, unlock and return -1
	    pthread_mutex_unlock(&m);
	    printd(1, "SingleExitGate %08p timed out after %ds waiting for tid %d to release lock\n", timeout, tid);
	    return -1;
	 }
      else
	 pthread_cond_wait(&cwait, &m);
      waiting--;
   }

   tid = ctid;

   pthread_mutex_unlock(&m);
   return 0;
}

inline int SingleExitGate::exit()
{
   int ctid = gettid();

   //fprintf(stderr, "SingleExitGate::exit() %08p\n", this);
   pthread_mutex_lock(&m);

   // if the lock is not locked by this thread, then return an error
   if (tid != ctid)
   {
      pthread_mutex_unlock(&m);
      return -1;
   }

   // unlock the lock
   tid = -1;
   if (waiting)
      pthread_cond_signal(&cwait);

   pthread_mutex_unlock(&m);

   return 0;
}

inline int SingleExitGate::forceExit()
{
   //fprintf(stderr, "SingleExitGate::exit() %08p\n", this);
   pthread_mutex_lock(&m);

   // if the lock is not locked, then return
   if (tid == -1)
   {
      pthread_mutex_unlock(&m);
      return 0;
   }

   // unlock the lock
   tid = -1;
   if (waiting)
      pthread_cond_signal(&cwait);

   pthread_mutex_unlock(&m);

   return 0;
}

inline int SingleExitGate::tryEnter()
{
   int ctid = gettid();

   pthread_mutex_lock(&m);

   if (tid != -1 && tid != ctid)
   {
      pthread_mutex_unlock(&m);
      return -1;
   }
   tid = ctid;

   pthread_mutex_unlock(&m);
   return 0;
}

#endif // _QORE_OBJECTS_SINGLEEXITGATE_H
