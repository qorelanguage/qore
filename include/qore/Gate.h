/*
  Gate.h

  Thread Gate object

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

#ifndef _QORE_GATE_H

#define _QORE_GATE_H

#include <pthread.h>

#define G_NOBODY -1

static inline int gettid();

class Gate
{
      friend class VRMutex;

   private:
      int code;
      int count;
      int waiting;
      pthread_mutex_t m;
      pthread_cond_t cwait;

   public:
      inline Gate();
      inline ~Gate();
      inline int enter(int c = gettid());
      inline int tryEnter(int c = gettid());
      inline int exit();
      inline int numInside();
      inline int numWaiting();
};

inline Gate::Gate()
{
   pthread_mutex_init(&m, NULL);
   pthread_cond_init(&cwait, NULL);
   code    = G_NOBODY;
   count   = 0;
   waiting = 0;
}

inline Gate::~Gate()
{
   pthread_mutex_destroy(&m);
   pthread_cond_destroy(&cwait);
}

inline int Gate::enter(int c)
{
   //fprintf(stderr, "Gate::enter(%d) %08p\n", c, this);
   pthread_mutex_lock(&m);
   if (c == G_NOBODY)
   {
      pthread_mutex_unlock(&m);
      return -1;
   }
   while (code != G_NOBODY && code != c)
   {
      waiting++;
      pthread_cond_wait(&cwait, &m);
      waiting--;
   }
   code = c;
   count++;
   pthread_mutex_unlock(&m);
   return 0;
}

inline int Gate::exit()
{
   //fprintf(stderr, "Gate::exit() %08p\n", this);
   pthread_mutex_lock(&m);
   // if the lock is not locked, then return an error
   if (!count)
   {
      pthread_mutex_unlock(&m);
      return -1;
   }
   count--;
   // if this is the last thread from the group to exit the lock
   // then unlock the lock
   if (!count)
   {
      code = G_NOBODY;
      // wake up a sleeping thread if there are threads waiting 
      // on the lock
      if (waiting)
	 pthread_cond_signal(&cwait);
   }
   pthread_mutex_unlock(&m);

   return 0;
}

inline int Gate::tryEnter(int c)
{
   pthread_mutex_lock(&m);
   if (c == G_NOBODY)
   {
      pthread_mutex_unlock(&m);
      return -1;
   }

   if (code != G_NOBODY && code != c)
   {
      pthread_mutex_unlock(&m);
      return -2;
   }
   code = c;
   count++;

   pthread_mutex_unlock(&m);
   return 0;
}

inline int Gate::numInside() 
{ 
   return count; 
}

inline int Gate::numWaiting() 
{ 
   return waiting;
}

#endif // _QORE_OBJECTS_GATE_H
