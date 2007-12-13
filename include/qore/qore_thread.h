/*
  qore_thread.h

  POSIX thread library for Qore

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

#ifndef _QORE_QORE_THREAD_H
#define _QORE_QORE_THREAD_H

#include <stdio.h>
#include <pthread.h>

#define CT_USER      0
#define CT_BUILTIN   1
#define CT_NEWTHREAD 2
#define CT_RETHROW   3

// pointer to a qore thread destructor function
typedef void (*qtdest_t)(void *);
// pointer to a qore thread resource destructor function
typedef void (*qtrdest_t)(void *, class ExceptionSink *);

DLLEXPORT extern class Operator *OP_BACKGROUND;

DLLEXPORT int gettid();
DLLEXPORT class QoreProgram *getProgram();

DLLEXPORT extern class ThreadCleanupList tclist;

// for thread resource handling
DLLEXPORT void set_thread_resource(class AbstractThreadResource *atr);
DLLEXPORT int remove_thread_resource(class AbstractThreadResource *atr);

class ThreadCleanupList {
   private:
      static class ThreadCleanupNode *head;

   public:
      DLLLOCAL ThreadCleanupList();
      DLLLOCAL ~ThreadCleanupList();
      DLLLOCAL void exec();

      DLLEXPORT void push(qtdest_t func, void *arg);
      DLLEXPORT void pop(int exec = 0);
};

#endif  // ifndef _QORE_THREAD_H
