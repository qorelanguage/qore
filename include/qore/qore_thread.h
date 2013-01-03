/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_thread.h

  POSIX thread library for Qore

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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

class QoreProgram;
class AbstractQoreZoneInfo;
class ThreadCleanupNode;
class AbstractThreadResource;

//! pointer to a qore thread destructor function
typedef void (*qtdest_t)(void *);

//! pointer to a qore thread resource destructor function
typedef void (*qtrdest_t)(void *, ExceptionSink *);

//! returns true if the current thread is a valid qore thread; it is not safe to call most Qore functions unless the thread is registered with Qore
/**
    @see
    - QoreForeignThreadHelper
    - q_register_foreign_thread()
    - q_deregister_foreign_thread()
 */
DLLEXPORT bool is_valid_qore_thread();

//! returns the current TID number
DLLEXPORT int gettid();

//! returns the current QoreProgram
DLLEXPORT QoreProgram *getProgram();

//! returns the current local time zone, note that if 0 = UTC
DLLEXPORT const AbstractQoreZoneInfo *currentTZ();

//! save a resource against a thread for thread resource handling
/** @param atr a pointer to the thread resource to save
 */
DLLEXPORT void set_thread_resource(AbstractThreadResource *atr);

//! remove the resource from the thread resource list for the current thread
/** @param atr a pointer to the thread resource to remove
    @return 0 if successful (resource was found and removed), -1 if the resource was not found
 */
DLLEXPORT int remove_thread_resource(AbstractThreadResource *atr);

//! save a resource against a thread for thread resource handling using the thread resource id
/** by using the thread resource id, you can quickly check if the resource has already been saved for the thread
    @param trid thread resource id
    @param atr a pointer to the thread resource to save
    @see qore_get_trid()
    @see remove_thread_resource_id()
 */
DLLEXPORT void set_thread_resource_id(q_trid_t trid, AbstractThreadResource *atr);

//! remove the resource from the thread resource list for the current thread using the thread resource ID
/** @param trid thread resource id of the resource to remove, must have been set with set_thread_resource_id()
    @return 0 if successful (resource was found and removed), -1 if the resource was not found
    @see qore_get_trid()
    @see set_thread_resource_id()
 */
DLLEXPORT int remove_thread_resource_id(q_trid_t trid);

//! check if a thread resouce has been saved with the given resource id
/** @param trid thread resource id to check
    @return true if saved, false if not saved
*/
DLLEXPORT bool check_thread_resource_id(q_trid_t trid);

//! get a thread resource ID
DLLEXPORT q_trid_t qore_get_trid();

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
   static ThreadCleanupNode *head;

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

/** @defgroup q_register_foreign_thread_rv Return Value Codes for q_register_foreign_thread()
 */
//@{
#define QFT_OK          0  //!< OK response when calling q_register_foreign_thread()
#define QFT_ERROR      -1  //!< error when calling q_register_foreign_thread()
#define QFT_REGISTERED -2  //!< thread already registered when calling q_register_foreign_thread()
//@}

//! registers the current thread as a Qore thread; returns QFT_OK (0) if the thread was successfully registered, QFT_ERROR (-1) if an error occurred, or QFT_REGISTERED (-2) if it was already registered
/** call q_deregister_foreign_thread() when Qore functionality is no longer required; not calling q_deregister_foreign_thread() will cause a memory leak

    @return see @ref q_register_foreign_thread_rv for return values

    @see
    - QoreForeignThreadHelper
    - q_deregister_foreign_thread()
    - is_valid_qore_thread();
 */
DLLEXPORT int q_register_foreign_thread();

//! deregisters the current thread as a foreign thread
/** @return true if the foreign thread was deregistered, false if it was not registered or was not a foreign thread

    @see
    - QoreForeignThreadHelper
    - q_register_foreign_thread()
    - is_valid_qore_thread();
 */
DLLEXPORT bool q_deregister_foreign_thread();

//! use this class to temporarily register and deregister a foreign thread to allow Qore code to be executed and the Qore library to be used from threads not created by the Qore library
class QoreForeignThreadHelper {
private:
   //! not implemented; defined here as private to preclude usage
   DLLLOCAL QoreForeignThreadHelper& operator=(const QoreForeignThreadHelper&);
   DLLLOCAL QoreForeignThreadHelper(const QoreForeignThreadHelper&);
   DLLLOCAL void* operator new(size_t);

protected:
   class qore_foreign_thread_priv* priv;

public:
   DLLEXPORT QoreForeignThreadHelper();
   DLLEXPORT ~QoreForeignThreadHelper();
};

#endif  // ifndef _QORE_THREAD_H
