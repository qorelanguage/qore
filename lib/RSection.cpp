/* -*- indent-tabs-mode: nil -*- */
/*
  RSection.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2018 Qore Technologies, s.r.o.

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
#include <qore/intern/RSection.h>

// does not block if there is an rsection conflict, returns -1 if the lock cannot be acquired and sets a notification
int qore_rsection_priv::tryRSectionLockNotifyWaitRead(RNotifier* rn) {
    assert(has_notify);

    int tid = q_gettid();

    AutoLocker al(l);
    assert(write_tid != tid);

    // if we already have the rsection, then return
    if (rs_tid == tid)
        return 0;

    while (true) {
        // if the write lock is acquired, then wait
        if (write_tid != -1) {
            ++read_waiting;
            read_cond.wait(l);
            --read_waiting;
            continue;
        }

        // if another thread is holding the rsection lock, then we have to abort & rollback
        if (rs_tid != -1) {
            setNotificationIntern(rn);
            return -1;
        }

        break;
    }

    // grab the read lock
    ++readers;

    // grab the rsection
    rs_tid = tid;
    return 0;
}
