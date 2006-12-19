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

int gettid();

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
      Gate();
      ~Gate();
      int enter(int c = gettid());
      int tryEnter(int c = gettid());
      int exit();
      int numInside();
      int numWaiting();
};

#endif // _QORE_GATE_H
