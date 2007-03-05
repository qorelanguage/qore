/*
  QoreCounter.h

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

#ifndef _QORE_QORECOUNTER_H

#define _QORE_QORECOUNTER_H 1

#include <pthread.h>
class QoreCounter
{
   private:
      pthread_mutex_t ptm_lock;
      pthread_cond_t c;
      int cnt;

      inline void lock()
      {
	 pthread_mutex_lock(&ptm_lock);
      }
      inline void unlock()
      {
	 pthread_mutex_unlock(&ptm_lock);
      }
      /*
      inline int signal()
      {
	 return pthread_cond_signal(&c);
      }
      */
      inline int broadcast()
      {
	 return pthread_cond_broadcast(&c);
      }
      inline int wait()
      {
	 return pthread_cond_wait(&c, &ptm_lock);
      }

   public:
      inline QoreCounter(int nc = 0)
      {
	 cnt = nc;
	 pthread_mutex_init(&ptm_lock, NULL);
	 pthread_cond_init(&c, NULL);
      }
      inline ~QoreCounter()
      {
	 pthread_mutex_destroy(&ptm_lock);
	 pthread_cond_destroy(&c);
      }
      inline void inc()
      {
	 lock();
	 cnt++;
	 unlock();
      }
      inline void dec()
      {
	 lock();
	 cnt--;
	 if (!cnt)
	    broadcast();
	 unlock();
      }
      inline void waitForZero()
      {
	 lock();
	 if (cnt)
	    wait();
	 unlock();
      }
      inline int getCount()
      {
	 return cnt;
      }
};

#endif
