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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QoreQueue.h>
#include <qore/qore_thread.h>
#include <qore/support.h>
#include <qore/Exception.h>

#include <sys/time.h>

QoreQueueNode::QoreQueueNode(QoreNode *n, class QoreQueueNode *tail) 
{ 
   node = n; 
   next = NULL; 
   prev = tail;
}

void QoreQueueNode::del(class ExceptionSink *xsink)
{
   if (node)
      node->deref(xsink);
   delete this;
}

QoreQueue::QoreQueue()
{
   pthread_mutex_init(&qmutex, NULL);
   pthread_cond_init(&qcond, NULL);
   head = NULL;
   tail = NULL;
   len  = 0;
}

QoreQueue::QoreQueue(QoreNode *n)
{
   pthread_mutex_init(&qmutex, NULL);
   pthread_cond_init(&qcond, NULL);
   head = new QoreQueueNode(n, NULL);
   tail = head;
   len  = 1;
}

// queues should not be deleted when other threads might
// be accessing them
QoreQueue::~QoreQueue()
{
   tracein("QoreQueue::~QoreQueue()");
   //printd(5, "QoreQueue %08p has head=%08p tail=%08p len=%d\n", this, head, tail, len);
   pthread_cond_destroy(&qcond);
   pthread_mutex_destroy(&qmutex);
   traceout("QoreQueue::~QoreQueue()");
}

void QoreQueue::push(QoreNode *n)
{
   if (n) 
      n->ref();
   printd(5, "QoreQueue::push(%08p)\n", n);
   pthread_mutex_lock(&qmutex);
   if (!head)
   {
      head = new QoreQueueNode(n, NULL);
      tail = head;
      // signal waiting thread to wakeup and process event
      pthread_cond_signal(&qcond);
   }
   else
   {
      tail->next = new QoreQueueNode(n, tail);
      tail = tail->next;
   }
   
   len++;
   pthread_mutex_unlock(&qmutex);
}

QoreNode *QoreQueue::shift()
{
   pthread_mutex_lock(&qmutex);
   // if there is no data, then wait for condition variable
   while (!head)
      pthread_cond_wait(&qcond, &qmutex);
   QoreQueueNode *n = head;
   head = head->next;
   if (!head)
      tail = NULL;
   else
      head->prev = NULL;
   
   len--;
   pthread_mutex_unlock(&qmutex);
   QoreNode *rv = n->node;
   n->node = NULL;
   n->del(NULL);
   printd(5, "QoreQueue::shift() %08p\n", n);
   return rv;
}

QoreNode *QoreQueue::shift(int timeout_ms, bool *to)
{
   pthread_mutex_lock(&qmutex);
   // if there is no data, then wait for condition variable
   while (!head)
      while (true)
      {
	 struct timeval now;
	 struct timespec tmout;
	 int ts = timeout_ms / 1000;
	 timeout_ms -= ts * 1000;
	 
	 gettimeofday(&now, NULL);
	 tmout.tv_sec = now.tv_sec + ts;
	 tmout.tv_nsec = (now.tv_usec * 1000) + (timeout_ms * 1000000);
	 
	 if (!pthread_cond_timedwait(&qcond, &qmutex, &tmout))
	    break;
	 
	 // lock has timed out, unlock and return -1
	 pthread_mutex_unlock(&qmutex);
	 printd(5, "QoreQueue::shift() timed out after %dms waiting on another thread to release the lock\n", ts * 1000 + timeout_ms);
	 *to = true;
	 return NULL;
      }
	 *to = false;
   
   QoreQueueNode *n = head;
   head = head->next;
   if (!head)
      tail = NULL;
   else
      head->prev = NULL;
   
   len--;
   pthread_mutex_unlock(&qmutex);
   QoreNode *rv = n->node;
   n->node = NULL;
   n->del(NULL);
   printd(5, "QoreQueue::shift() %08p\n", n);
   return rv;
}

QoreNode *QoreQueue::pop()
{
   pthread_mutex_lock(&qmutex);
   // if there is no data, then wait for condition variable
   while (!head)
      pthread_cond_wait(&qcond, &qmutex);
   QoreQueueNode *n = tail;
   tail = tail->prev;
   if (!tail)
      head = NULL;
   else
      tail->next = NULL;
   
   len--;
   pthread_mutex_unlock(&qmutex);
   QoreNode *rv = n->node;
   n->node = NULL;
   n->del(NULL);
   printd(5, "QoreQueue::shift() %08p\n", n);
   return rv;
}

QoreNode *QoreQueue::pop(int timeout_ms, bool *to)
{
   pthread_mutex_lock(&qmutex);
   // if there is no data, then wait for condition variable
   while (!head)
      while (true)
      {
	 struct timeval now;
	 struct timespec tmout;
	 int ts = timeout_ms / 1000;
	 timeout_ms -= ts * 1000;
	 
	 gettimeofday(&now, NULL);
	 tmout.tv_sec = now.tv_sec + ts;
	 tmout.tv_nsec = (now.tv_usec * 1000) + (timeout_ms * 1000000);
	 
	 int rc;
	 if (!(rc = pthread_cond_timedwait(&qcond, &qmutex, &tmout)))
	    break;
	 
	 // lock has timed out, unlock and return -1
	 pthread_mutex_unlock(&qmutex);
	 printd(5, "rc = %d: QoreQueue::pop() timed out after %dms waiting on another thread to release the lock\n", rc, ts * 1000 + timeout_ms);
	 *to = true;
	 return NULL;
      }
	 *to = false;
   
   QoreQueueNode *n = tail;
   tail = tail->prev;
   if (!tail)
      head = NULL;
   else
      tail->next = NULL;
   
   len--;
   pthread_mutex_unlock(&qmutex);
   QoreNode *rv = n->node;
   n->node = NULL;
   n->del(NULL);
   printd(5, "QoreQueue::shift() %08p\n", n);
   return rv;
}

void QoreQueue::del(class ExceptionSink *xsink)
{
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
   len = 0;
}

int QoreQueue::size() const
{ 
   return len; 
}

