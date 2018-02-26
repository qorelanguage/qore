/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  InputStreamBase.h

  Qore Programming Language

  Copyright (C) 2016 - 2018 Qore Technologies, s.r.o.

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

#ifndef _QORE_INPUTSTREAMBASE_H
#define _QORE_INPUTSTREAMBASE_H

#include "qore/InputStream.h"

#include <atomic>

DLLEXPORT extern QoreClass* QC_INPUTSTREAMBASE;

/**
 * @brief Base class for private data of input stream implementations in C++.
 */
class InputStreamBase : public InputStream {
public:
    /**
      * @brief Helper method that checks that the current thread is the same as when the instance was created,
      * calls read() and wraps the read data to Qore's `binary` value.
      * @param limit the maximum number of bytes to read
      * @param xsink the exception sink
      * @return the `binary` wrapping the read data or `NOTHING` if the end of the stream has been reached
      */
    DLLLOCAL BinaryNode *readHelper(int64 limit, ExceptionSink *xsink) {
        if (!check(xsink)) {
            return nullptr;
        }
        if (limit <= 0) {
            xsink->raiseException("INPUT-STREAM-ERROR", "%s::read() called with non-positive limit " QLLD,
                getName(), limit);
            return nullptr;
        }
        SimpleRefHolder<BinaryNode> result(new BinaryNode());
        result->preallocate(limit);
        int64 count = read(const_cast<void *>(result->getPtr()), limit, xsink);
        result->setSize(count);
        return count ? result.release() : nullptr;
    }

    /**
      * @brief Helper method that checks that the current thread is the same as when the instance was created,
      * calls peek() and wraps the result to Qore's `int` value.
      * @param xsink the exception sink
      * @return the `int` wrapping the result or `NOTHING` if the end of the stream has been reached
d       */
    DLLLOCAL QoreValue peekHelper(ExceptionSink *xsink) {
        if (!check(xsink)) {
            return QoreValue();
        }

        return peek(xsink);
    }

    /**
      * @brief Checks that the current thread is the same as when the instance was created or assigned
      * via @ref unassignThread() and @ref reassignThread() and that the stream has not yet been closed.
      * @param xsink the exception sink
      * @return true if the checks passed, false if an exception has been raised
      * @throws STREAM-THREAD-ERROR if the current thread is not the same as when the instance was created
      * @throws INPUT-STREAM-CLOSED-ERROR if the stream has been closed
      */
    bool check(ExceptionSink *xsink) {
        if (tid.load(std::memory_order_relaxed) != gettid()) {
            xsink->raiseException("STREAM-THREAD-ERROR", "this %s object was created in TID %d; it is an error "
                "to access it from any other thread (accessed from TID %d)", getName(), tid.load(std::memory_order_relaxed), gettid());
            return false;
        }
        return true;
    }

    /**
      * @brief Reassigns current thread as thread used for stream manipulation, see @ref check()
      * @param xsink the exception sink
      * @throws STREAM-THREAD-ERROR if the current thread is already assigned to another thread
      */
    DLLLOCAL void reassignThread(ExceptionSink *xsink) override {
        // use an atomic compare and exchange to ensure that we only update a stream where the thread is unassigned
        int chktid = -1;
        if (!tid.compare_exchange_strong(chktid, gettid(), std::memory_order_consume, std::memory_order_relaxed)) {
            xsink->raiseException("STREAM-THREAD-ERROR", "this %s object is assigned to TID %d;"
                "(accessed from TID %d)", getName(), tid.load(std::memory_order_relaxed), gettid());
            return;
        }
    }

    /**
      * @brief Unassigns current thread as thread used for stream manipulation, see @ref check()
      * @param xsink the exception sink
      * @throws STREAM-THREAD-ERROR if the current thread is not the same as assigned
      */
    DLLLOCAL void unassignThread(ExceptionSink *xsink) override {
        if (tid.load(std::memory_order_relaxed) != -1 && tid.load(std::memory_order_relaxed) != gettid()) {
            xsink->raiseException("STREAM-THREAD-ERROR", "this %s object is assigned to TID %d; unassignment may "
                "be processed from that thread (accessed from TID %d)", getName(), tid.load(std::memory_order_relaxed), gettid());
        }
        // we need to ensure that the write operation here is visible in the new thread in the reassignThread() call
        tid.store(-1, std::memory_order_release);
    }

    /**
      * @brief Get currently assigned thread id
      */
    DLLLOCAL int getThreadId() {
        return tid.load(std::memory_order_relaxed);
    }

    protected:
    /**
      * @brief Constructor.
      */
    InputStreamBase() : tid(gettid()) {
    }

    /**
      * @brief Returns the name of the class.
      * @return the name of the class
      */
    DLLLOCAL virtual const char *getName() = 0;

private:
    //! The id of the thread that created the instance
    /** only the unassignThread() and reassignThread() operations need to be synchronized;
        all other reads can use relaxed memory ordering (no cache flushes or inter-thread
        synchronization)
    */
    std::atomic<int> tid;
};

#endif // _QORE_INPUTSTREAMBASE_H
