/* 
 QoreQueue.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2012 David Nichols
 
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
#include <errno.h>

void QoreQueue::pushNode(AbstractQoreNode* v) {
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

   //printd(5, "QoreQueue::pushNode(%p) this=%p head=%p (%p) tail=%p (%p) waiting=%d len=%d\n", v, this, head, head->node, tail, tail->node, waiting, len);
}

void QoreQueue::pushIntern(AbstractQoreNode* v) {
   pushNode(v);
   //printd(5, "QoreQueue::push_internal(%p) this=%p head=%p (%p) tail=%p (%p) waiting=%d len=%d\n", v, this, head, head->node, tail, tail->node, waiting, len);

   // signal waiting thread to wakeup and process event
   if (read_waiting)
      read_cond.signal();
}

void QoreQueue::insertIntern(AbstractQoreNode* v) {
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

   //printd(5, "QoreQueue::insertIntern(%p) this=%p head=%p (%p) tail=%p (%p) waiting=%d len=%d\n", v, this, head, head->node, tail, tail->node, waiting, len);

   // signal waiting thread to wakeup and process event
   if (read_waiting)
      read_cond.signal();
}

void QoreQueue::pushAndTakeRef(AbstractQoreNode* n) {
   AutoLocker al(&l);
   if (len == Queue_Deleted)
      return;

   assert(max == -1);

   printd(5, "QoreQueue::pushAndTakeRef(%p) this=%p\n", n, this);
   // reference value for being stored in queue
   pushIntern(n);
}

void QoreQueue::push(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms, bool *to) {
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

void QoreQueue::insert(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms, bool *to) {
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

AbstractQoreNode* QoreQueue::shift(ExceptionSink* xsink, int timeout_ms, bool *to) {
   SafeLocker sl(&l);

#ifdef DEBUG
   //if (!head) printd(5, "QoreQueue::shift(timeout_ms=%d) WAITING this=%p head=%p tail=%p waiting=%d len=%d\n", timeout_ms, this, head, tail, waiting, len);
#endif

   {
      int rc = waitReadIntern(xsink, timeout_ms);
      if (to)
         *to = rc == QW_TIMEOUT ? true : false;
      if (rc)
         return 0;
   }

   //printd(5, "QoreQueue::shift() GOT DATA this=%p head=%p (rv=%p) tail=%p (%p) waiting=%d len=%d\n", this, head, head->node, tail, tail->node, waiting, len);

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

AbstractQoreNode* QoreQueue::pop(ExceptionSink* xsink, int timeout_ms, bool *to) {
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

void QoreQueue::clear(ExceptionSink* xsink) {
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

void QoreQueue::destructor(ExceptionSink* xsink) {
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
