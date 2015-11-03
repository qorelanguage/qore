/*
  qore_thread.h

  POSIX thread library for Qore

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

#ifndef _QORE_QORE_THREAD_H
#define _QORE_QORE_THREAD_H

/** @file qore_thread.h
    Provides definitions relative to threading in Qore.
 */

#include <stdio.h>
#include <pthread.h>

//! pointer to a qore thread destructor function
typedef void (*qtdest_t)(void *);

//! pointer to a qore thread resource destructor function
typedef void (*qtrdest_t)(void *, class ExceptionSink *);

//! returns true if the current thread is a valid qore thread; it is not safe to call most Qore functions unless the thread is registered with Qore
DLLEXPORT bool is_valid_qore_thread();

//! returns the current TID number
DLLEXPORT int gettid();

//! returns the current QoreProgram
DLLEXPORT class QoreProgram *getProgram();

//! save a resource against a thread for thread resource handling
/** @param atr a pointer to the thread resource to save
 */
DLLEXPORT void set_thread_resource(class AbstractThreadResource *atr);

//! remove the resource from the thread resource list for the current thread
/** @param atr a pointer to the thread resource to remove
    @return 0 if successful (resource was found and removed), -1 if the resource was not found
 */
DLLEXPORT int remove_thread_resource(class AbstractThreadResource *atr);

//! list of functions to be run when a thread ends; required for some external libraries that require explicit cleanup when a thread terminates
/** this list is not locked and therefore the ThreadCleanupList::push() and 
    ThreadCleanupList::pop() functions must only be called in module initialization
    and module deletion.  However this list is implemented in such a way that thread
    cleanup list execution may be safely called interally while push() is being
    executed in a module initialization function, for example.
    @note this is a global object and not an attribute of a thread
 */
class ThreadCleanupList {
   private:
      static class ThreadCleanupNode *head;

   public:
      DLLLOCAL ThreadCleanupList();
      DLLLOCAL ~ThreadCleanupList();
      DLLLOCAL void exec();

      //! must only be called in the module initialization function
      /** @param func the cleanup function to be run whenever a thread ends
	  @param arg the argument to the function (can be 0)
       */
      DLLEXPORT void push(qtdest_t func, void *arg);

      //! must only be called in the module destructor/deletion function
      /** @param exec if true the cleanup function will be executed immediately, if false it will not
       */
      DLLEXPORT void pop(bool exec = true);
};

//! the interface to the thread cleanup list
DLLEXPORT extern ThreadCleanupList tclist;

#endif  // ifndef _QORE_THREAD_H
