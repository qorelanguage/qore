/* 
 QoreQueue.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
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
#include <qore/intern/QoreQueue.h>

#include <sys/time.h>

QoreQueueNode::QoreQueueNode(AbstractQoreNode *n) : node(n) { 
}

void QoreQueueNode::del(ExceptionSink *xsink) {
   if (node)
      node->deref(xsink);
   delete this;
}

QoreQueue::QoreQueue() : head(0), tail(0), len(0), waiting(0) {
   //printd(5, "QoreQueue::QoreQueue() this=%p\n", this);
}

QoreQueue::QoreQueue(const QoreQueue &orig) : head(0), tail(0), len(0), waiting(0) {
   AutoLocker al(orig.l);
   if (orig.len == Queue_Deleted)
      return;

   QoreQueueNode *w = orig.head;
   while (w) {
      push_internal(w->node ? w->node->refSelf() : 0);
      w = w->next;
   }

   //printd(5, "QoreQueue::QoreQueue() this=%p head=%p tail=%p waiting=%d len=%d\n", this, head, tail, waiting, len);
}

// queues should not be deleted when other threads might
// be accessing them
QoreQueue::~QoreQueue() {
   //QORE_TRACE("QoreQueue::~QoreQueue()");
   //printd(5, "QoreQueue::~QoreQueue() this=%p head=%p tail=%p len=%d\n", this, head, tail, len);
   assert(!head);
   assert(!tail);
   assert(len == Queue_Deleted);
}

void QoreQueue::push_internal(AbstractQoreNode *v) {
   if (!head) {
      head = new QoreQueueNode(v);
      head->next = 0; 
      head->prev = 0;

      tail = head;
   }
   else {
      QoreQueueNode *qn = new QoreQueueNode(v);
      tail->next = qn;
      qn->next = 0; 
      qn->prev = tail;

      tail = qn;
   }

   //printd(5, "QoreQueue::push_internal(%p) this=%p head=%p (%p) tail=%p (%p) waiting=%d len=%d\n", v, this, head, head->node, tail, tail->node, waiting, len);

   // signal waiting thread to wakeup and process event
   if (waiting)
      cond.signal();
   
   ++len;
}

void QoreQueue::push_and_take_ref(AbstractQoreNode *n) {
   AutoLocker al(&l);
   if (len == Queue_Deleted)
      return;

   printd(5, "QoreQueue::push_and_take_ref(%p) this=%p\n", n, this);
   // reference value for being stored in queue
   push_internal(n);
}

void QoreQueue::push(const AbstractQoreNode *n) {
   AutoLocker al(&l);
   if (len == Queue_Deleted)
      return;

   //printd(5, "QoreQueue::push(%p) this=%p\n", n, this);
   // reference value for being stored in queue
   push_internal(n ? n->refSelf() : 0);
}

void QoreQueue::insert_internal(AbstractQoreNode *v) {
   if (!head) {
      head = new QoreQueueNode(v);
      head->next = 0; 
      head->prev = 0;

      tail = head;
   }
   else {
      QoreQueueNode *qn = new QoreQueueNode(v);
      qn->next = head;
      qn->prev = 0;
      head->prev = qn;

      head = qn;
   }
   // signal waiting thread to wakeup and process event
   if (waiting)
      cond.signal();
   
   len++;
}

void QoreQueue::insert_and_take_ref(AbstractQoreNode *n) {
   AutoLocker al(&l);
   if (len == Queue_Deleted)
      return;

   printd(5, "QoreQueue::insert_and_take_ref(%p) this=%p\n", n, this);
   // reference value for being stored in queue
   insert_internal(n);
}

void QoreQueue::insert(const AbstractQoreNode *n) {
   AutoLocker al(&l);
   if (len == Queue_Deleted)
      return;

   printd(5, "QoreQueue::insert(%p) this=%p\n", n, this);
   // reference value for being stored in queue
   insert_internal(n ? n->refSelf() : 0);
}

AbstractQoreNode *QoreQueue::shift(ExceptionSink *xsink, int timeout_ms, bool *to) {
   SafeLocker sl(&l);

#ifdef DEBUG
   //if (!head) printd(5, "QoreQueue::shift(timeout_ms=%d) WAITING this=%p head=%p tail=%p waiting=%d len=%d\n", timeout_ms, this, head, tail, waiting, len);
#endif

   // if there is no data, then wait for condition variable
   while (!head) {
      ++waiting;
      int rc = timeout_ms ? cond.wait(l, timeout_ms) : cond.wait(l);
      --waiting;
      if (rc) {	 
	 // lock has timed out, unlock and return -1
	 sl.unlock();
#ifdef DEBUG
	 // if an error has occurred, then it must be due to a timeout
	 if (!timeout_ms)
	    printd(0, "QoreQueue::shift(timeout_ms=0) this=%p pthread_cond_wait() returned rc=%d\n", this, rc);
#endif
	 assert(timeout_ms);
	 assert(rc == ETIMEDOUT);
	 if (to)
	    *to = true;
	 return 0;
      }
      if (len == Queue_Deleted) {
	 xsink->raiseException("QUEUE-ERROR", "Queue has been deleted in another thread");
	 return 0;
      }
   }
   if (to)
      *to = false;

   printd(5, "QoreQueue::shift() GOT DATA this=%p head=%p (rv=%p) tail=%p (%p) waiting=%d len=%d\n", this, head, head->node, tail, tail->node, waiting, len);
   
   QoreQueueNode *n = head;
   head = head->next;
   if (!head)
      tail = 0;
   else
      head->prev = 0;
   
   len--;
   sl.unlock();
   AbstractQoreNode *rv = n->node;
   n->node = 0;
   n->del(0);
   return rv;
}

AbstractQoreNode *QoreQueue::pop(ExceptionSink *xsink, int timeout_ms, bool *to) {
   SafeLocker sl(&l);
   // if there is no data, then wait for condition variable
   while (!head) {
      int rc;
      ++waiting;
      if (timeout_ms)
	 rc = cond.wait(&l, timeout_ms);
      else
	 rc = cond.wait(&l);
      --waiting;
      if (rc) {
	 // lock has timed out, unlock and return 0
	 sl.unlock();
	 printd(5, "QoreQueue::pop() this=%p timed out after %dms waiting on another thread to release the lock\n", this, rc, timeout_ms);
	 if (to) 
	    *to = true;
	 return 0;
      }
      if (len == Queue_Deleted) {
	 xsink->raiseException("QUEUE-ERROR", "Queue has been deleted in another thread");
	 return 0;
      }
   }
   if (to)
      *to = false;
   
   QoreQueueNode *n = tail;
   tail = tail->prev;
   if (!tail)
      head = 0;
   else
      tail->next = 0;
   
   len--;
   sl.unlock();
   AbstractQoreNode *rv = n->node;
   n->node = 0;
   n->del(0);
   printd(5, "QoreQueue::pop() this=%p rv=%p\n", this, n);
   return rv;
}

void QoreQueue::destructor(ExceptionSink *xsink) {
   AutoLocker al(&l);
   if (waiting) {
      xsink->raiseException("QUEUE-ERROR", "Queue deleted while there %s %d waiting thread%s",
                            waiting == 1 ? "is" : "are", waiting, waiting == 1 ? "" : "s");
      cond.broadcast();
   }

   while (head) {
      printd(5, "QoreQueue::~QoreQueue() this=%p deleting %p (node %p type %s)\n", this,
	     head, head->node, head->node ? head->node->getTypeName() : "(null)");
      QoreQueueNode *w = head->next;
      head->del(xsink);
      head = w;
   }
   head = 0;
   tail = 0;
   len = Queue_Deleted;
}
