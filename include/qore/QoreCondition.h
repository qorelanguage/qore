/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreCondition.h

    Qore Programming Language

    Copyright (C) 2005 - 2020 Qore Technologies, s.r.o.

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

#ifndef _QORE_QORECONDITION_H

#define _QORE_QORECONDITION_H

#include <pthread.h>
#include <sys/time.h>

#include <qore/QoreThreadLock.h>

//! a thread condition class implementing a wrapper for pthread_cond_t
/** all threads that block on a given QoreCondition object should use the same mutex or QoreThreadLock object for blocking
    @see QoreThreadLock
 */
class QoreCondition {
private:
    //! the condition thread primitive
    pthread_cond_t c;

    //! this function is not implemented; it is here as a private function in order to prohibit it from being used
    DLLLOCAL QoreCondition(const QoreCondition&);

    //! this function is not implemented; it is here as a private function in order to prohibit it from being used
    DLLLOCAL QoreCondition& operator=(const QoreCondition&);

public:
    //! creates the condition object
    DLLEXPORT QoreCondition();

    //! destroys the condition object
    DLLEXPORT ~QoreCondition();

    //! signals a single waiting thread to wake up
    DLLEXPORT int signal();

    //! singles all threads blocked on this condition to wake up
    DLLEXPORT int broadcast();

    //! blocks a thread on a mutex until the condition is signaled
    /**
        @param m the mutex to wait on

        @return 0 for success, non-zero for error
    */
    DLLEXPORT int wait(pthread_mutex_t* m);

    //! blocks a thread on a mutex for a certain number of milliseconds until the condition is signaled
    /**
        @param m the mutext to wait on
        @param timeout_ms the timeout value is milliseconds

        @return a non-zero return value indicates a timeout occured

        @deprecated use wait2(pthread_mutex_t*, int64) instead
    */
    DLLEXPORT int wait(pthread_mutex_t* m, int timeout_ms);

    //! blocks a thread on a mutex for a certain number of milliseconds until the condition is signaled
    /**
        @param m the mutext to wait on
        @param timeout_ms the timeout value is milliseconds; must be >= 0; calling with a negative value will cause an
        abort in debug builds and for EINVAL to be set in non-debug builds

        @return a non-zero return value indicates a timeout occured
    */
    DLLEXPORT int wait2(pthread_mutex_t* m, int64 timeout_ms);

    //! blocks a thread on a lock until the condition is signaled
    /**
        @param l the QoreThreadLock to wait on

        @return 0 for success, non-zero for error
    */
    DLLLOCAL int wait(QoreThreadLock* l) {
        return wait(&l->ptm_lock);
    }

    //! blocks a thread on a lock for a certain number of milliseconds until the condition is signaled
    /**
        @param l the QoreThreadLock to wait on
        @param timeout_ms the timeout value is milliseconds; must be >= 0; calling with a negative value will cause an
        abort in debug builds and for EINVAL to be set in non-debug builds

        @return a non-zero return value indicates a timeout occured

        @deprecated use wait2(QoreThreadLock*, int64) instead
    */
    DLLLOCAL int wait(QoreThreadLock* l, int timeout_ms) {
        return wait(&l->ptm_lock, timeout_ms);
    }

    //! blocks a thread on a lock for a certain number of milliseconds until the condition is signaled
    /**
        @param l the QoreThreadLock to wait on
        @param timeout_ms the timeout value is milliseconds; must be >= 0; calling with a negative value will cause an
        abort in debug builds and for EINVAL to be set in non-debug builds

        @return a non-zero return value indicates a timeout occured
    */
    DLLLOCAL int wait2(QoreThreadLock* l, int64 timeout_ms) {
        return wait2(&l->ptm_lock, timeout_ms);
    }

    //! blocks a thread on a lock until the condition is signaled
    /**
        @param l the QoreThreadLock to wait on

        @return 0 for success, non-zero for error
    */
    DLLLOCAL int wait(QoreThreadLock& l) {
        return wait(&l);
    }

    //! blocks a thread on a lock for a certain number of milliseconds until the condition is signaled
    /**
        @param l the QoreThreadLock to wait on
        @param timeout_ms the timeout value is milliseconds; must be >= 0; calling with a negative value will cause an
        abort in debug builds and for EINVAL to be set in non-debug builds

        @return a non-zero return value indicates a timeout occured

        @deprecated use wait2(QoreThreadLock&, int64) instead
    */
    DLLLOCAL int wait(QoreThreadLock& l, int timeout_ms) {
        return wait(&l, timeout_ms);
    }

    //! blocks a thread on a lock for a certain number of milliseconds until the condition is signaled
    /**
        @param l the QoreThreadLock to wait on
        @param timeout_ms the timeout value is milliseconds; must be >= 0; calling with a negative value will cause an
        abort in debug builds and for EINVAL to be set in non-debug builds

        @return a non-zero return value indicates a timeout occured
    */
    DLLLOCAL int wait2(QoreThreadLock& l, int64 timeout_ms) {
        return wait2(&l, timeout_ms);
    }
};

#endif // QORE_CONDITION
