/* -*- mode: c++; indent-tabs-mode: nil -*- */
/* 
   QoreQueueIntern.h

   Qore Programming Language

   Copyright (C) 2003 - 2014 David Nichols

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

#ifndef _QORE_QOREQUEUEINTERN_H

#define _QORE_QOREQUEUEINTERN_H

#include <qore/QoreThreadLock.h>
#include <qore/QoreCondition.h>

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

   DLLLOCAL void del(ExceptionSink *xsink) {
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

class qore_queue_private {
private:
   enum queue_status_e { Queue_Deleted = -1 };

   mutable QoreThreadLock l;
   QoreCondition read_cond,   // read Condition variable
                 write_cond;  // write Condition variable
   QoreQueueNode* head,
                * tail;
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

public:
   DLLLOCAL qore_queue_private(int n_max = -1) : head(0), tail(0), len(0), max(n_max), read_waiting(0), write_waiting(0) {
      assert(max);
      //printd(5, "qore_queue_private::qore_queue_private() this: %p max: %d\n", this, max);
   }

   DLLLOCAL qore_queue_private(const qore_queue_private &orig) : head(0), tail(0), len(0), max(orig.max), read_waiting(0), write_waiting(0) {
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
   }

   // push at the end of the queue and take the reference - can only be used when len == -1
   DLLLOCAL void pushAndTakeRef(AbstractQoreNode* n);

   // push at the end of the queue
   DLLLOCAL void push(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms = 0, bool *to = 0);

   // insert at the beginning of the queue
   DLLLOCAL void insert(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms = 0, bool *to = 0);

   DLLLOCAL AbstractQoreNode* shift(ExceptionSink* xsink, int timeout_ms = 0, bool *to = 0);
   DLLLOCAL AbstractQoreNode* pop(ExceptionSink* xsink, int timeout_ms = 0, bool *to = 0);

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

   DLLLOCAL static void destructor(QoreQueue& q, ExceptionSink* xsink) {
      q.priv->destructor(xsink);
   }
};

#endif // _QORE_QOREQUEUEINTERN_H
