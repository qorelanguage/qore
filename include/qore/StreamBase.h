/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    StreamBase.h

    Qore Programming Language

    Copyright (C) 2016 - 2023 Qore Technologies, s.r.o.

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

#ifndef _QORE_STREAMBASE_H
#define _QORE_STREAMBASE_H

#include "qore/AbstractPrivateData.h"
#include "qore/qore_thread.h"
#include "qore/QoreString.h"
#include "qore/ExceptionSink.h"
#include "qore/BinaryNode.h"

#include <atomic>

DLLEXPORT extern QoreClass* QC_STREAMBASE;

/**
 * @brief Base class for private data of stream implementations in C++.
 */
class StreamBase : public AbstractPrivateData {
public:
    /**
      * @brief Checks that the current thread is the same as when the instance was created or assigned
      * via @ref unassignThread() and @ref reassignThread() and that the stream has not yet been closed.
      * @param xsink the exception sink
      * @return true if the checks passed, false if an exception has been raised
      * @throws STREAM-THREAD-ERROR if the current thread is not the same as when the instance was created
      */
    DLLLOCAL bool check(ExceptionSink *xsink) {
        if (tid.load(std::memory_order_relaxed) != q_gettid()) {
            xsink->raiseException("STREAM-THREAD-ERROR", "this %s object was created in TID %d; it is an error " \
                "to access it from any other thread (accessed from TID %d)", getName(),
                tid.load(std::memory_order_relaxed), q_gettid());
            return false;
        }
        return true;
    }

    /**
      * @brief Reassigns current thread as thread used for stream manipulation, see @ref check()
      * @param xsink the exception sink
      * @throws STREAM-THREAD-ERROR if the current thread is already assigned to another thread
      */
    DLLLOCAL void reassignThread(ExceptionSink *xsink) {
        // use an atomic compare and exchange to ensure that we only update a stream where the thread is unassigned
        int chktid = -1;
        if (!tid.compare_exchange_strong(chktid, q_gettid(), std::memory_order_consume, std::memory_order_relaxed)) {
            // do not raise an exception if reassigning to the same thread
            if (chktid != q_gettid()) {
                xsink->raiseException("STREAM-THREAD-ERROR", "this %s object is assigned to TID %d; it is an error " \
                    "to access it from any other thread (accessed from TID %d)", getName(), chktid, q_gettid());
            }
        }
    }

    /**
      * @brief Unassigns current thread as thread used for stream manipulation, see @ref check()
      * @param xsink the exception sink
      * @throws STREAM-THREAD-ERROR if the current thread is not the same as assigned
      */
    DLLLOCAL void unassignThread(ExceptionSink *xsink) {
        // use an atomic compare and exchange to ensure that we only update a stream where the thread is assigned to this thread
        int chktid = q_gettid();
        if (!tid.compare_exchange_strong(chktid, -1, std::memory_order_release, std::memory_order_relaxed)) {
            // do not raise an exception if already unassigned
            if (chktid != -1) {
                xsink->raiseException("STREAM-THREAD-ERROR", "this %s object is assigned to TID %d; unassignment may " \
                    "be processed from that thread (accessed from TID %d)", getName(), chktid, q_gettid());
            }
        }
    }

    /**
      * @brief Get currently assigned thread id
      */
    DLLLOCAL int getThreadId() {
        return tid.load(std::memory_order_relaxed);
    }

    /**
      * @brief Returns the name of the class.
      * @return the name of the class
      */
    DLLLOCAL virtual const char *getName() = 0;

protected:
    /**
      * @brief Constructor.
      */
    StreamBase() : tid(q_gettid()) {
    }

private:
    //! The id of the thread that created the instance
    /** only the unassignThread() and reassignThread() operations need to be synchronized;
        all other reads can use relaxed memory ordering (no cache flushes or inter-thread
        synchronization)
    */
    std::atomic<int> tid;
};

#endif // _QORE_STREAMBASE_H
