/* 
 QoreQueue.cpp
 
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
      ++read_waiting;
      int rc = timeout_ms ? read_cond.wait(l, timeout_ms) : read_cond.wait(l);
      --read_waiting;
      
      if (rc) {
#ifdef DEBUG
	 // if an error has occurred, then it must be due to a timeout
	 if (!timeout_ms)
	    printd(0, "qore_queue_private::waitReadIntern(timeout_ms=0) this: %p pthread_cond_wait() returned rc=%d\n", this, rc);
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
   
   return 0;
}

int qore_queue_private::waitWriteIntern(ExceptionSink *xsink, int timeout_ms) {
   // if the queue is full, then wait for condition variable
   while (max > 0 && len >= max) {
      ++write_waiting;
      int rc = timeout_ms ? write_cond.wait(l, timeout_ms) : write_cond.wait(l);
      --write_waiting;
      
      if (rc) {
#ifdef DEBUG
	 // if an error has occurred, then it must be due to a timeout
	 if (!timeout_ms)
	    printd(0, "qore_queue_private::waitWriteIntern(timeout_ms=0) this: %p pthread_cond_wait() returned rc=%d\n", this, rc);
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

   //printd(5, "qore_queue_private::pushNode(%p) this=%p head=%p (%p) tail=%p (%p) waiting=%d len=%d\n", v, this, head, head->node, tail, tail->node, waiting, len);
}

void qore_queue_private::pushIntern(AbstractQoreNode* v) {
   pushNode(v);
   //printd(5, "qore_queue_private::push_internal(%p) this=%p head=%p (%p) tail=%p (%p) waiting=%d len=%d\n", v, this, head, head->node, tail, tail->node, waiting, len);

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

   //printd(5, "qore_queue_private::insertIntern(%p) this=%p head=%p (%p) tail=%p (%p) waiting=%d len=%d\n", v, this, head, head->node, tail, tail->node, waiting, len);

   // signal waiting thread to wakeup and process event
   if (read_waiting)
      read_cond.signal();
}

void qore_queue_private::pushAndTakeRef(AbstractQoreNode* n) {
   AutoLocker al(&l);
   if (len == Queue_Deleted)
      return;

   assert(max == -1);

   printd(5, "qore_queue_private::pushAndTakeRef(%p) this=%p\n", n, this);
   // reference value for being stored in queue
   pushIntern(n);
}

void qore_queue_private::push(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms, bool* to) {
   AutoLocker al(&l);
   if (len == Queue_Deleted)
      return;

   {
      int rc = waitWriteIntern(xsink, timeout_ms);
      if (to)
         *to = rc == QW_TIMEOUT ? true : false;
      if (rc)
         return;
   }

   pushIntern(n ? n->refSelf() : 0);
}

void qore_queue_private::insert(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms, bool* to) {
   AutoLocker al(&l);
   if (len == Queue_Deleted)
      return;

   {
      int rc = waitWriteIntern(xsink, timeout_ms);
      if (to)
         *to = rc == QW_TIMEOUT ? true : false;
      if (rc)
         return;
   }

   insertIntern(n ? n->refSelf() : 0);
}

AbstractQoreNode* qore_queue_private::shift(ExceptionSink* xsink, int timeout_ms, bool* to) {
   SafeLocker sl(&l);

#ifdef DEBUG
   //if (!head) printd(5, "qore_queue_private::shift(timeout_ms=%d) WAITING this=%p head=%p tail=%p waiting=%d len=%d\n", timeout_ms, this, head, tail, waiting, len);
#endif

   {
      int rc = waitReadIntern(xsink, timeout_ms);
      if (to)
         *to = rc == QW_TIMEOUT ? true : false;
      if (rc)
         return 0;
   }

   //printd(5, "qore_queue_private::shift() GOT DATA this=%p head=%p (rv=%p) tail=%p (%p) waiting=%d len=%d\n", this, head, head->node, tail, tail->node, waiting, len);

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
   priv->push(xsink, n, timeout_ms, to);
}

// insert at the beginning of the queue
void QoreQueue::insert(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms, bool* to) {
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
