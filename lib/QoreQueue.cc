/* 
 QoreQueue.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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

#include <sys/time.h>

QoreQueueNode::QoreQueueNode(QoreNode *n) : node(n)
{ 
}

void QoreQueueNode::del(class ExceptionSink *xsink)
{
   if (node)
      node->deref(xsink);
   delete this;
}

QoreQueue::QoreQueue() : head(0), tail(0), len(0), waiting(0)
{
}

QoreQueue::QoreQueue(QoreNode *n) : waiting(0)
{
   head = new QoreQueueNode(n);
   head->next = NULL; 
   head->prev = NULL;

   tail = head;
   len  = 1;
}

// queues should not be deleted when other threads might
// be accessing them
QoreQueue::~QoreQueue()
{
   tracein("QoreQueue::~QoreQueue()");
   //printd(5, "QoreQueue %08p has head=%08p tail=%08p len=%d\n", this, head, tail, len);
   traceout("QoreQueue::~QoreQueue()");
}

void QoreQueue::push(QoreNode *n)
{
   AutoLocker al(&l);
   if (len == Queue_Deleted)
      return;

   // reference value for being stored in queue
   if (n) 
      n->ref();
   printd(5, "QoreQueue::push(%08p)\n", n);

   if (!head)
   {
      head = new QoreQueueNode(n);
      head->next = NULL; 
      head->prev = NULL;

      tail = head;
   }
   else
   {
      QoreQueueNode *qn = new QoreQueueNode(n);
      tail->next = qn;
      qn->next = NULL; 
      qn->prev = tail;

      tail = qn;
   }
   // signal waiting thread to wakeup and process event
   if (waiting)
      cond.signal();
   
   len++;
}

void QoreQueue::insert(QoreNode *n)
{
   AutoLocker al(&l);
   if (len == Queue_Deleted)
      return;

   // reference value for being stored in queue
   if (n) 
      n->ref();
   printd(5, "QoreQueue::push(%08p)\n", n);

   if (!head)
   {
      head = new QoreQueueNode(n);
      head->next = NULL; 
      head->prev = NULL;

      tail = head;
   }
   else
   {
      QoreQueueNode *qn = new QoreQueueNode(n);
      qn->next = head;
      qn->prev = NULL;
      head->prev = qn;

      head = qn;
   }
   // signal waiting thread to wakeup and process event
   if (waiting)
      cond.signal();
   
   len++;
}

QoreNode *QoreQueue::shift(class ExceptionSink *xsink, int timeout_ms, bool *to)
{
   SafeLocker sl(&l);
   // if there is no data, then wait for condition variable
   while (!head)
   {
      int rc;
      ++waiting;
      if (timeout_ms)
	 rc = cond.wait(&l, timeout_ms);
      else
	 rc = cond.wait(&l);
      --waiting;
      if (rc)
      {	 
	 // lock has timed out, unlock and return -1
	 sl.unlock();
	 printd(5, "QoreQueue::shift() timed out after %dms waiting on another thread to release the lock\n", timeout_ms);
	 if (to)
	    *to = true;
	 return NULL;
      }
      if (len == Queue_Deleted)
      {
	 xsink->raiseException("QUEUE-ERROR", "Queue has been deleted in another thread");
	 return NULL;
      }
      // check for signals on spurious wakeup
      if (!head)
      {
	 sl.unlock();
	 QoreSignalManager::handleSignals();
	 sl.lock();
      }
   }
   if (to)
      *to = false;
   
   QoreQueueNode *n = head;
   head = head->next;
   if (!head)
      tail = NULL;
   else
      head->prev = NULL;
   
   len--;
   sl.unlock();
   QoreNode *rv = n->node;
   n->node = NULL;
   n->del(NULL);
   printd(5, "QoreQueue::shift() %08p\n", n);
   return rv;
}

QoreNode *QoreQueue::pop(class ExceptionSink *xsink, int timeout_ms, bool *to)
{
   SafeLocker sl(&l);
   // if there is no data, then wait for condition variable
   while (!head)
   {
      int rc;
      ++waiting;
      if (timeout_ms)
	 rc = cond.wait(&l, timeout_ms);
      else
	 rc = cond.wait(&l);
      --waiting;
      if (rc)
      {
	 // lock has timed out, unlock and return NULL
	 sl.unlock();
	 printd(5, "QoreQueue::pop() timed out after %dms waiting on another thread to release the lock\n", rc, timeout_ms);
	 if (to) 
	    *to = true;
	 return NULL;
      }
      if (len == Queue_Deleted)
      {
	 xsink->raiseException("QUEUE-ERROR", "Queue has been deleted in another thread");
	 return NULL;
      }
      // check for signals on spurious wakeup
      if (!head)
      {
	 sl.unlock();
	 QoreSignalManager::handleSignals();
	 sl.lock();
      }
   }
   if (to)
      *to = false;
   
   QoreQueueNode *n = tail;
   tail = tail->prev;
   if (!tail)
      head = NULL;
   else
      tail->next = NULL;
   
   len--;
   sl.unlock();
   QoreNode *rv = n->node;
   n->node = NULL;
   n->del(NULL);
   printd(5, "QoreQueue::shift() %08p\n", n);
   return rv;
}

void QoreQueue::destructor(class ExceptionSink *xsink)
{
   AutoLocker al(&l);
   if (waiting)
   {
      xsink->raiseException("QUEUE-ERROR", "Queue deleted while there %s %d waiting thread%s",
                            waiting == 1 ? "is" : "are", waiting, waiting == 1 ? "" : "s");
      cond.broadcast();
   }

   while (head)
   {
      printd(5, "QoreQueue::~QoreQueue() deleting %08p (node %08p type %s)\n",
	     head, head->node, head->node ? head->node->type->getName() : "(null)");
      QoreQueueNode *w = head->next;
      head->del(xsink);
      head = w;
   }
   head = NULL;
   tail = NULL;
   len = Queue_Deleted;
}
