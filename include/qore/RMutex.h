/*
  RMutex.h

  recursive lock object

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

#ifndef _QORE_RMUTEX_H

#define _QORE_RMUTEX_H

#include <pthread.h>

extern pthread_mutexattr_t ma_recursive;

class RMutex
{
   private:
      pthread_mutex_t m;

   public:
      inline RMutex()
      {
	 // create mutex and set recursive attribute
	 pthread_mutex_init(&m, &ma_recursive);
      }
      inline ~RMutex()
      {
	 pthread_mutex_destroy(&m);
      }
      inline int enter()
      {
	 return pthread_mutex_lock(&m);
      }
      inline int tryEnter()
      {
	 return pthread_mutex_trylock(&m);
      }
      inline int exit()
      {
	 return pthread_mutex_unlock(&m);
      }
};

#endif
