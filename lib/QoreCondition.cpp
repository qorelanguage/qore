/*
    QoreCondition.cpp

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

#include <qore/Qore.h>
#include <qore/QoreCondition.h>

#include <cerrno>
#include <cstring>

QoreCondition::QoreCondition() {
   pthread_cond_init(&c, 0);
}

QoreCondition::~QoreCondition() {
   pthread_cond_destroy(&c);
}

int QoreCondition::signal() {
   return pthread_cond_signal(&c);
}

int QoreCondition::broadcast() {
   return pthread_cond_broadcast(&c);
}

int QoreCondition::wait(pthread_mutex_t *m) {
#ifdef DEBUG
    int rc = pthread_cond_wait(&c, m);
    if (rc) {
        printd(0, "QoreCondition::wait(%p) pthread_cond_wait() returned %d %s\n", m, rc, strerror(rc));
        // print out a backtrace if possible
        qore_machine_backtrace();
    }
    assert(!rc);
    return rc;
#else
    return pthread_cond_wait(&c, m);
#endif
}

// timeout is in milliseconds
int QoreCondition::wait(pthread_mutex_t* m, int timeout_ms) {
    return wait2(m, timeout_ms);
}

// timeout is in milliseconds
int QoreCondition::wait2(pthread_mutex_t* m, int64 timeout_ms) {
    // negative timeouts will cause an error
    assert(timeout_ms >= 0);
    if (timeout_ms < 0) {
        return EINVAL;
    }
#ifdef DARWIN
    // use more efficient pthread_cond_timedwait_relative_np() on Darwin
    struct timespec tmout;
    tmout.tv_sec = timeout_ms / 1000;
    tmout.tv_nsec = (timeout_ms - tmout.tv_sec * 1000) * 1000000;

#ifndef DEBUG
    return pthread_cond_timedwait_relative_np(&c, m, &tmout);
#else // !DEBUG
    //printd(5, "QoreCondition::wait(%p, %lld) this=%p +trigger=%d.%09d\n", m, timeout_ms, this, tmout.tv_sec, tmout.tv_nsec);
    int rc = pthread_cond_timedwait_relative_np(&c, m, &tmout);
    if (rc && rc != ETIMEDOUT) {
        printd(0, "QoreCondition::wait(m=%p, timeout_ms=%lld) this=%p pthread_cond_timedwait_relative_np() returned %d %s (errno=%d %s)\n", m, timeout_ms, this, rc, strerror(rc), errno, strerror(errno));
        // print out a backtrace if possible
        qore_machine_backtrace();
    }
    assert(!rc || rc == ETIMEDOUT);
    return rc;
#endif // DEBUG
#else // !DARWIN
    struct timeval now;
    struct timespec tmout;

#ifdef DEBUG
    int64 timeout_ms_orig = timeout_ms;
#endif // DEBUG

    gettimeofday(&now, 0);
    int64 secs = timeout_ms / 1000;
    timeout_ms -= secs * 1000;
    int64 nsecs = now.tv_usec * 1000 + timeout_ms * 1000000;
    int64 dsecs = nsecs / 1000000000;
    nsecs -= dsecs * 1000000000;
    tmout.tv_sec = now.tv_sec + secs + dsecs;
    tmout.tv_nsec = nsecs;

    // make sure mutex is locked
    assert(pthread_mutex_trylock(m) == EBUSY);

#ifndef DEBUG
    return pthread_cond_timedwait(&c, m, &tmout);
#else // !DEBUG
    //printd(5, "QoreCondition::wait(%p, " QLLD ") this=%p now=%d.%09d trigger=%d.%09d\n", m, timeout_ms, this, now.tv_sec, now.tv_usec * 1000, tmout.tv_sec, tmout.tv_nsec);
    int rc = pthread_cond_timedwait(&c, m, &tmout);
    if (rc && rc != ETIMEDOUT) {
        printd(0, "QoreCondition::wait(m=%p, timeout_ms=%d) pthread_cond_timedwait() returned %d %s\n", m, timeout_ms_orig, rc, strerror(rc));
        // print out a backtrace if possible
        qore_machine_backtrace();
    }
    assert(!rc || rc == ETIMEDOUT);
    return rc;
#endif // DEBUG
#endif // DARWIN
}
