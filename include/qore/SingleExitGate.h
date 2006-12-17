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
      DLLLOCAL SingleExitGate();
      DLLLOCAL ~SingleExitGate();
      DLLLOCAL int enter(int timeout = 0);
      DLLLOCAL int tryEnter();
      DLLLOCAL int exit();
      DLLLOCAL int forceExit();
      DLLLOCAL int getLockTID() const;
};


#endif // _QORE_OBJECTS_SINGLEEXITGATE_H
