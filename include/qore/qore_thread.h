/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_thread.h

  POSIX thread library for Qore

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
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

//! pointer to a function that can be started with q_start_thread()
typedef void (*q_thread_t)(ExceptionSink* xsink, void* arg);

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
DLLEXPORT QoreProgram* getProgram();

//! returns the current local time zone, note that if 0 = UTC
DLLEXPORT const AbstractQoreZoneInfo* currentTZ();

//! save a resource against a thread for thread resource handling
/** @param atr a pointer to the thread resource to save
 */
DLLEXPORT void set_thread_resource(AbstractThreadResource* atr);

//! remove the resource from the thread resource list for the current thread
/** @param atr a pointer to the thread resource to remove
    @return 0 if successful (resource was found and removed), -1 if the resource was not found
 */
DLLEXPORT int remove_thread_resource(AbstractThreadResource* atr);

//! check if a thread resouce has been saved
/** @param atr thread resource id to check
    @return true if saved, false if not saved
*/
DLLEXPORT bool check_thread_resource(AbstractThreadResource* atr);

//! save a callable resource against a thread for thread resource handling
/** @param rcr a pointer to a callable node for thread resource handling
 */
DLLEXPORT void set_thread_resource(const ResolvedCallReferenceNode* rcr, QoreValue arg);

//! remove the callable resource from the thread resource list for the current thread
/** @param rcr a pointer to the thread resource to remove
    @param xsink any Qore-language exceptions raised when dereferencing the callable object will be saved here

    @return 0 if successful (resource was found and removed), -1 if the resource was not found
 */
DLLEXPORT int remove_thread_resource(const ResolvedCallReferenceNode* rcr, ExceptionSink* xsink);

#if 0
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
#endif

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

    @since %Qore 0.8.7
 */
DLLEXPORT int q_register_foreign_thread();

//! deregisters the current thread as a foreign thread
/** @return 0 if the foreign thread was deregistered, -1 if it was not registered or was not a foreign thread

    @see
    - QoreForeignThreadHelper
    - q_register_foreign_thread()
    - is_valid_qore_thread();

    @since %Qore 0.8.7
 */
DLLEXPORT int q_deregister_foreign_thread();

//! reserves a thread ID for later registration and returns the TID reserved; use q_release_reserved_foreign_thread_id() to release the reservation
/** @return the TID reserved or -1 if an error occurred (thread table full)

    @since %Qore 0.8.7
 */
DLLEXPORT int q_reserve_foreign_thread_id();

//! releases a TID reserved with q_reserve_foreign_thread_id()
/** @param tid the TID reserved with q_reserve_foreign_thread_id()

    @return 0 for OK, -1 for error (TID invalid or not reserved)

    @since %Qore 0.8.7
 */
DLLEXPORT int q_release_reserved_foreign_thread_id(int tid);

//! registers a foreign qore thread as a Qore thread with a reserved TID
/** @param tid the TID reserved with q_reserve_foreign_thread_id()

    @return 0 for OK, -1 for error (TID invalid or not reserved)

    @since %Qore 0.8.7
 */
DLLEXPORT int q_register_reserved_foreign_thread(int tid);

//! deregisters a foreign qore thread but retains the TID as reserved
/** @returns 0 for OK, -1 for error (TID invalid or not foreign)

    @since %Qore 0.8.7
 */
DLLEXPORT int q_deregister_reserved_foreign_thread();

//! starts a new thread with the given arguments, when the thread terminates, it deregisters itself
/** @param arg the argument to the function
    @param f the function to start in the new thread
    @param xsink any errors starting the new thread will be raised here and cause -1 to be returned

    @return the new TID or -1 if an error occurred

    @since %Qore 0.8.8
 */
int q_start_thread(ExceptionSink* xsink, q_thread_t f, void* arg = 0);

//! use this class to temporarily register and deregister a foreign thread to allow Qore code to be executed and the Qore library to be used from threads not created by the Qore library
/** @since %Qore 0.8.7
 */
class QoreForeignThreadHelper {
private:
   //! not implemented; defined here as private to preclude usage
   DLLLOCAL QoreForeignThreadHelper& operator=(const QoreForeignThreadHelper&);
   DLLLOCAL QoreForeignThreadHelper(const QoreForeignThreadHelper&);
   DLLLOCAL void* operator new(size_t);

protected:
   class qore_foreign_thread_priv* priv;

public:
   //! registers the current thread as a foreign thread; the thread will be deregistered with q_deregister_foreign_thread() in the destructor
   DLLEXPORT QoreForeignThreadHelper();

   //! registers the current thread as a foreign thread; the tid given must be already reserved with q_reserve_foreign_thread_id(); the thread will be deregistered with q_deregister_reserved_foreign_thread() in the destructor
   DLLEXPORT explicit QoreForeignThreadHelper(int tid);

   //! deregisters the current thread if the registration was successful using either q_deregister_foreign_thread() or q_reserve_foreign_thread_id(), depending on the constructor used
   DLLEXPORT ~QoreForeignThreadHelper();
};

#endif  // ifndef _QORE_THREAD_H
