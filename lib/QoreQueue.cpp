/*
   QoreQueue.cpp

   Qore Programming Language

   Copyright (C) 2003 - 2016 David Nichols

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
#include <qore/QoreQueue.h>
#include <qore/intern/QoreQueueIntern.h>

#include <sys/time.h>
#include <errno.h>

void qore_queue_private::destructor(ExceptionSink* xsink) {
   AutoLocker al(&l);
   if (read_waiting) {
      xsink->raiseException("QUEUE-ERROR", "Queue deleted while there %s %d waiting thread%s for reading", read_waiting == 1 ? "is" : "are", read_waiting, read_waiting == 1 ? "" : "s");
      read_cond.broadcast();
   }
   if (write_waiting) {
      xsink->raiseException("QUEUE-ERROR", "Queue deleted while there %s %d waiting thread%s for writing", write_waiting == 1 ? "is" : "are", write_waiting, write_waiting == 1 ? "" : "s");
      write_cond.broadcast();
   }

   clearIntern(xsink);
   len = Queue_Deleted;
   if (desc)
      desc->deref();
#ifdef DEBUG
   desc = 0;
#endif
}

void qore_queue_private::clearIntern(ExceptionSink* xsink) {
   while (head) {
      printd(5, "qore_queue_private::clearIntern() this: %p deleting %p (node %p type %s)\n", this, head, head->node, get_node_type(head->node));
      QoreQueueNode* w = head->next;
      head->del(xsink);
      head = w;
   }
   head = 0;
   tail = 0;
}

int qore_queue_private::waitReadIntern(ExceptionSink *xsink, int timeout_ms) {
   // if there is no data, then wait for condition variable
   while (!head) {
      if (!err.empty()) {
         xsink->raiseException(err.c_str(), desc->stringRefSelf());
         return QW_ERROR;
      }

      ++read_waiting;
      int rc = timeout_ms ? read_cond.wait(l, timeout_ms) : read_cond.wait(l);
      --read_waiting;

      if (rc) {
#ifdef DEBUG
	 // if an error has occurred, then it must be due to a timeout
	 if (!timeout_ms)
	    printd(0, "qore_queue_private::waitReadIntern(timeout_ms=0) this: %p pthread_cond_wait() returned rc: %d\n", this, rc);
#endif
	 assert(timeout_ms);
	 assert(rc == ETIMEDOUT);
	 return QW_TIMEOUT;
      }

      if (len == Queue_Deleted) {
	 xsink->raiseException("QUEUE-ERROR", "Queue has been deleted in another thread");
	 return QW_DEL;
      }
   }

   if (!err.empty()) {
      xsink->raiseException(err.c_str(), desc->stringRefSelf());
      return QW_ERROR;
   }

   return 0;
}

int qore_queue_private::waitWriteIntern(ExceptionSink *xsink, int timeout_ms) {
   // if the queue is full, then wait for condition variable
   while (max > 0 && len >= max) {
      if (!err.empty()) {
         xsink->raiseException(err.c_str(), desc->stringRefSelf());
         return QW_ERROR;
      }

      ++write_waiting;
      int rc = timeout_ms ? write_cond.wait(l, timeout_ms) : write_cond.wait(l);
      --write_waiting;

      if (rc) {
#ifdef DEBUG
	 // if an error has occurred, then it must be due to a timeout
	 if (!timeout_ms)
	    printd(0, "qore_queue_private::waitWriteIntern(timeout_ms=0) this: %p pthread_cond_wait() returned rc: %d\n", this, rc);
#endif
	 assert(timeout_ms);
	 assert(rc == ETIMEDOUT);
	 return QW_TIMEOUT;
      }

      if (len == Queue_Deleted) {
	 xsink->raiseException("QUEUE-ERROR", "Queue has been deleted in another thread");
	 return QW_DEL;
      }
   }

   if (!err.empty()) {
      xsink->raiseException(err.c_str(), desc->stringRefSelf());
      return QW_ERROR;
   }

   return 0;
}

void qore_queue_private::pushNode(AbstractQoreNode* v) {
   if (!head) {
      head = new QoreQueueNode(v, 0, 0);
      tail = head;
   }
   else {
      QoreQueueNode* qn = new QoreQueueNode(v, tail, 0);
      tail->next = qn;
      tail = qn;
   }
   ++len;

   //printd(5, "qore_queue_private::pushNode(%p '%s') this: %p head: %p (%p) tail: %p (%p) read_waiting: %d len: %d\n", v, get_type_name(v), this, head, head->node, tail, tail->node, read_waiting, len);
}

void qore_queue_private::pushIntern(AbstractQoreNode* v) {
   pushNode(v);
   //printd(5, "qore_queue_private::push_internal(%p) this: %p head: %p (%p) tail: %p (%p) waiting: %d len: %d\n", v, this, head, head->node, tail, tail->node, waiting, len);

   // signal waiting thread to wakeup and process event
   if (read_waiting)
      read_cond.signal();
}

void qore_queue_private::insertIntern(AbstractQoreNode* v) {
   if (!head) {
      head = new QoreQueueNode(v, 0, 0);
      tail = head;
   }
   else {
      QoreQueueNode* qn = new QoreQueueNode(v, 0, head);
      head->prev = qn;
      head = qn;
   }
   len++;

   //printd(5, "qore_queue_private::insertIntern(%p) this: %p head: %p (%p) tail: %p (%p) waiting: %d len: %d\n", v, this, head, head->node, tail, tail->node, waiting, len);

   // signal waiting thread to wakeup and process event
   if (read_waiting)
      read_cond.signal();
}

int qore_queue_private::checkWriteIntern(ExceptionSink* xsink, bool always_error) {
   if (len == Queue_Deleted) {
      if (always_error)
	 xsink->raiseException("QUEUE-ERROR", "Queue has been deleted in another thread");
      return -1;
   }
   if (!err.empty()) {
      xsink->raiseException(err.c_str(), desc->stringRefSelf());
      return -1;
   }
   return 0;
}

void qore_queue_private::pushAndTakeRef(AbstractQoreNode* n) {
   AutoLocker al(&l);
   if (len == Queue_Deleted || !err.empty())
      return;

   assert(max == -1);

   printd(5, "qore_queue_private::pushAndTakeRef(%p) this: %p\n", n, this);
   // reference value for being stored in queue
   pushIntern(n);
}

void qore_queue_private::push(ExceptionSink* xsink, AbstractQoreNode* n, int timeout_ms, bool* to) {
   ReferenceHolder<> holder(n, xsink);

   AutoLocker al(&l);
   if (checkWriteIntern(xsink))
      return;

   {
      int rc = waitWriteIntern(xsink, timeout_ms);
      if (to)
         *to = rc == QW_TIMEOUT ? true : false;
      if (rc)
         return;
   }

   pushIntern(holder.release());
}

void qore_queue_private::insert(ExceptionSink* xsink, AbstractQoreNode* n, int timeout_ms, bool* to) {
   ReferenceHolder<> holder(n, xsink);

   AutoLocker al(&l);
   if (checkWriteIntern(xsink))
      return;

   {
      int rc = waitWriteIntern(xsink, timeout_ms);
      if (to)
         *to = rc == QW_TIMEOUT ? true : false;
      if (rc)
         return;
   }

   insertIntern(holder.release());
}

AbstractQoreNode* qore_queue_private::shift(ExceptionSink* xsink, int timeout_ms, bool* to) {
   SafeLocker sl(&l);

   if (checkWriteIntern(xsink, true))
      return 0;

#ifdef DEBUG
   //if (!head) printd(5, "qore_queue_private::shift(timeout_ms: %d) WAITING this: %p head: %p tail: %p waiting: %d len: %d\n", timeout_ms, this, head, tail, waiting, len);
#endif

   {
      int rc = waitReadIntern(xsink, timeout_ms);
      if (to)
         *to = rc == QW_TIMEOUT ? true : false;
      if (rc)
         return 0;
   }

   //printd(5, "qore_queue_private::shift() GOT DATA this: %p head: %p (rv: %p '%s') tail: %p (%p) write_waiting: %d len: %d\n", this, head, head->node, get_type_name(head->node), tail, tail->node, write_waiting, len);

   QoreQueueNode* n = head;
   head = head->next;
   if (!head)
      tail = 0;
   else
      head->prev = 0;

   len--;
   if (write_waiting)
      write_cond.signal();

   sl.unlock();
   return n->takeAndDel();
}

AbstractQoreNode* qore_queue_private::pop(ExceptionSink* xsink, int timeout_ms, bool* to) {
   SafeLocker sl(&l);

   if (checkWriteIntern(xsink, true))
      return 0;

   {
      int rc = waitReadIntern(xsink, timeout_ms);
      if (to)
         *to = rc == QW_TIMEOUT ? true : false;
      if (rc)
         return 0;
   }

   QoreQueueNode* n = tail;
   tail = tail->prev;
   if (!tail)
      head = 0;
   else
      tail->next = 0;

   len--;
   if (write_waiting)
      write_cond.signal();

   sl.unlock();
   return n->takeAndDel();
}

void qore_queue_private::clear(ExceptionSink* xsink) {
   AutoLocker al(&l);

   if (checkWriteIntern(xsink))
      return;

   if (read_waiting) {
      // the queue must be empty
      assert(!head);
      return;
   }

   clearIntern(xsink);
   len = 0;

   if (write_waiting)
      write_cond.signal();
}

void qore_queue_private::setError(const char* n_err, const QoreStringNode* n_desc, ExceptionSink* xsink) {
   AutoLocker al(&l);
   if (len == Queue_Deleted)
      return;

   err = n_err;
   if (desc)
      desc->deref();
   desc = n_desc->stringRefSelf();

   // clear the queue
   clearIntern(xsink);
   len = 0;

   if (read_waiting)
      read_cond.broadcast();
   if (write_waiting)
      write_cond.broadcast();
}

void qore_queue_private::clearError() {
   AutoLocker al(&l);
   if (len == Queue_Deleted)
      return;

   err.clear();
   if (desc) {
      desc->deref();
      desc = 0;
   }
}

QoreQueue::QoreQueue(int n_max) : priv(new qore_queue_private(n_max)) {
}

QoreQueue::QoreQueue(const QoreQueue &orig) : priv(new qore_queue_private(*orig.priv)) {
}

// queues should not be deleted when other threads might
// be accessing them
QoreQueue::~QoreQueue() {
   delete priv;
}

// push at the end of the queue and take the reference - can only be used when len == -1
void QoreQueue::pushAndTakeRef(AbstractQoreNode* n) {
   priv->pushAndTakeRef(n);
}

// push at the end of the queue
void QoreQueue::push(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms, bool* to) {
   priv->push(xsink, n ? n->refSelf() : 0, timeout_ms, to);
}

// insert at the beginning of the queue
void QoreQueue::insert(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms, bool* to) {
   priv->insert(xsink, n ? n->refSelf() : 0, timeout_ms, to);
}

// push at the end of the queue
void QoreQueue::push(ExceptionSink* xsink, AbstractQoreNode* n, int timeout_ms, bool* to) {
   priv->push(xsink, n, timeout_ms, to);
}

// insert at the beginning of the queue
void QoreQueue::insert(ExceptionSink* xsink, AbstractQoreNode* n, int timeout_ms, bool* to) {
   priv->insert(xsink, n, timeout_ms, to);
}

AbstractQoreNode* QoreQueue::shift(ExceptionSink* xsink, int timeout_ms, bool* to) {
   return priv->shift(xsink, timeout_ms, to);
}

AbstractQoreNode* QoreQueue::pop(ExceptionSink* xsink, int timeout_ms, bool* to) {
   return priv->pop(xsink, timeout_ms, to);
}

bool QoreQueue::empty() const {
   return priv->empty();
}

int QoreQueue::size() const {
   return priv->size();
}

int QoreQueue::getMax() const {
   return priv->getMax();
}

unsigned QoreQueue::getReadWaiting() const {
   return priv->getReadWaiting();
}

unsigned QoreQueue::getWriteWaiting() const {
   return priv->getWriteWaiting();
}

void QoreQueue::clear(ExceptionSink* xsink) {
   priv->clear(xsink);
}

void QoreQueue::setError(const char* err, const QoreStringNode* desc, ExceptionSink* xsink) {
   priv->setError(err, desc, xsink);
}

void QoreQueue::clearError() {
   priv->clearError();
}

Queue::Queue(int max) : QoreQueue(max) {
}

Queue::~Queue() {
}

Queue* Queue::queueRefSelf() const {
   ((Queue*)this)->ref();
   return (Queue*)this;
}
