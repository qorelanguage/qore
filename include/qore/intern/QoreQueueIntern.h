/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreQueueIntern.h

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

#ifndef _QORE_QOREQUEUEINTERN_H

#define _QORE_QOREQUEUEINTERN_H

#include <qore/QoreThreadLock.h>
#include <qore/QoreCondition.h>

#include <string>

class QoreQueueNode {
public:
   AbstractQoreNode* node;
   QoreQueueNode* prev,
                * next;
   DLLLOCAL QoreQueueNode(AbstractQoreNode* n, QoreQueueNode* p, QoreQueueNode* nx) : node(n), prev(p), next(nx) {
   }

#ifdef DEBUG
   DLLLOCAL ~QoreQueueNode() {
      assert(!node);
   }
#endif

   DLLLOCAL void del(ExceptionSink* xsink) {
      if (node)
         node->deref(xsink);

#ifdef DEBUG
      node = 0;
#endif
      delete this;
   }

   DLLLOCAL AbstractQoreNode* takeAndDel() {
      AbstractQoreNode* rv = node;
#ifdef DEBUG
      node = 0;
#endif
      delete this;
      return rv;
   }
};

#define QW_DEL     -1
#define QW_TIMEOUT -2
#define QW_ERROR   -3

class qore_queue_private {
private:
   enum queue_status_e { Queue_Deleted = -1 };

   mutable QoreThreadLock l;
   QoreCondition read_cond,   // read Condition variable
                 write_cond;  // write Condition variable
   QoreQueueNode* head,
                * tail;
   std::string err;
   QoreStringNode* desc;
   int len,   // the number of elements currently in the queue (or -1 for deleted)
       max;   // the maximum size of the queue (or -1 for unlimited)
   unsigned read_waiting,   // number of threads waiting on reads
            write_waiting;  // number of threads waiting on writes

   DLLLOCAL int waitReadIntern(ExceptionSink *xsink, int timeout_ms);
   DLLLOCAL int waitWriteIntern(ExceptionSink *xsink, int timeout_ms);

   DLLLOCAL void pushNode(AbstractQoreNode* v);
   DLLLOCAL void pushIntern(AbstractQoreNode* v);
   DLLLOCAL void insertIntern(AbstractQoreNode* v);

   DLLLOCAL void clearIntern(ExceptionSink* xsink);

   // called in the lock; returns -1 if not possible (cannot write to the queue) or 0 of OK
   DLLLOCAL int checkWriteIntern(ExceptionSink* xsink, bool always_error = false);

public:
   DLLLOCAL qore_queue_private(int n_max = -1) : head(0), tail(0), desc(0), len(0), max(n_max), read_waiting(0), write_waiting(0) {
      assert(max);
      //printd(5, "qore_queue_private::qore_queue_private() this: %p max: %d\n", this, max);
   }

   DLLLOCAL qore_queue_private(const qore_queue_private &orig) : head(0), tail(0), err(orig.err), desc(orig.desc ? orig.desc->stringRefSelf() : 0), len(0), max(orig.max), read_waiting(0), write_waiting(0) {
      AutoLocker al(orig.l);
      if (orig.len == Queue_Deleted)
         return;

      QoreQueueNode* w = orig.head;
      while (w) {
         pushIntern(w->node ? w->node->refSelf() : 0);
         w = w->next;
      }

      //printd(5, "qore_queue_private::qore_queue_private() this=%p head=%p tail=%p waiting=%d len=%d\n", this, head, tail, waiting, len);
   }

   // queues should not be deleted when other threads might
   // be accessing them
   DLLLOCAL ~qore_queue_private() {
      //QORE_TRACE("qore_queue_private::~qore_queue_private()");
      //printd(5, "qore_queue_private::~qore_queue_private() this=%p head=%p tail=%p len=%d\n", this, head, tail, len);
      assert(!head);
      assert(!tail);
      assert(len == Queue_Deleted);
      assert(!desc);
   }

   // push at the end of the queue and take the reference - can only be used when len == -1
   DLLLOCAL void pushAndTakeRef(AbstractQoreNode* n);

   // push at the end of the queue
   DLLLOCAL void push(ExceptionSink* xsink, AbstractQoreNode* n, int timeout_ms, bool& to);

   // insert at the beginning of the queue
   DLLLOCAL void insert(ExceptionSink* xsink, AbstractQoreNode* n, int timeout_ms, bool& to);

   DLLLOCAL AbstractQoreNode* shift(ExceptionSink* xsink, int timeout_ms, bool& to);
   DLLLOCAL AbstractQoreNode* pop(ExceptionSink* xsink, int timeout_ms, bool& to);

   DLLLOCAL bool empty() const {
      return !len;
   }

   DLLLOCAL int size() const {
      return len;
   }

   DLLLOCAL int getMax() const {
      return max;
   }

   DLLLOCAL unsigned getReadWaiting() const {
      return read_waiting;
   }

   DLLLOCAL unsigned getWriteWaiting() const {
      return write_waiting;
   }

   DLLLOCAL void clear(ExceptionSink* xsink);
   DLLLOCAL void destructor(ExceptionSink* xsink);

   DLLLOCAL void setError(const char* err, const QoreStringNode* desc, ExceptionSink* xsink);

   DLLLOCAL void clearError();

   DLLLOCAL static void destructor(QoreQueue& q, ExceptionSink* xsink) {
      q.priv->destructor(xsink);
   }
};

#endif // _QORE_QOREQUEUEINTERN_H
