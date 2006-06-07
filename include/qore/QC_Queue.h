/* 
   QC_Queue.h

   Qore Programming Language

   Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#ifndef _QORE_CLASS_QUEUE

#define _QORE_CLASS_QUEUE

#include <qore/common.h>
#include <qore/thread.h>
#include <qore/support.h>
#include <qore/ReferenceObject.h>
#include <qore/Exception.h>

#include <sys/time.h>

extern int CID_QUEUE;
class QoreClass *initQueueClass();

// FIXME: need to explicitly clear queue before
// deletion and halde exception properly
class QoreQueueNode 
{
   protected:
      inline ~QoreQueueNode() {}

   public:
      class QoreNode *node;
      class QoreQueueNode *next;
      class QoreQueueNode *prev;

      inline QoreQueueNode(QoreNode *n, class QoreQueueNode *tail) 
      { 
	 node = n; 
	 next = NULL; 
	 prev = tail;
      }
      inline void del(class ExceptionSink *xsink)
      {
	 if (node)
	    node->deref(xsink);
	 delete this;
      }
};

class Queue : public ReferenceObject 
{
   private:
      pthread_mutex_t qmutex;
      pthread_cond_t  qcond;
      QoreQueueNode *head;
      QoreQueueNode *tail;
      int len;

   protected:
      inline ~Queue();

   public:
      inline Queue();
      inline Queue(QoreNode *n);
      inline void push(QoreNode *n);
      inline QoreNode *shift();
      inline QoreNode *shift(int timeout_ms, bool *to);
      inline QoreNode *pop();
      inline QoreNode *pop(int timeout_ms, bool *to);
      inline int size() { return len; }
      inline void deref(class ExceptionSink *xsink);
};

inline Queue::Queue()
{
   pthread_mutex_init(&qmutex, NULL);
   pthread_cond_init(&qcond, NULL);
   head = NULL;
   tail = NULL;
   len  = 0;
}

inline Queue::Queue(QoreNode *n)
{
   pthread_mutex_init(&qmutex, NULL);
   pthread_cond_init(&qcond, NULL);
   head = new QoreQueueNode(n, NULL);
   tail = head;
   len  = 0;
}

// queues should not be deleted when other threads might
// be accessing them
inline Queue::~Queue()
{
   tracein("Queue::~Queue()");
   pthread_cond_destroy(&qcond);
   pthread_mutex_destroy(&qmutex);
   traceout("Queue::~Queue()");
}

inline void Queue::push(QoreNode *n)
{
   if (n) 
      n->ref();
   printd(5, "Queue::push(%08x)\n", n);
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

inline QoreNode *Queue::shift()
{
   pthread_mutex_lock(&qmutex);
   // if there is no data, then wait for condition variable
   while (!head)
      pthread_cond_wait(&qcond, &qmutex);
   QoreQueueNode *n = head;
   head = head->next;
   if (!head)
      tail = NULL;

   len--;
   pthread_mutex_unlock(&qmutex);
   QoreNode *rv = n->node;
   n->node = NULL;
   n->del(NULL);
   printd(5, "Queue::shift() %08x\n", n);
   return rv;
}

inline QoreNode *Queue::shift(int timeout_ms, bool *to)
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

	 // lock has timed out, return -1
	 printd(0, "Queue::shift() timed out after %dms waiting on another thread to release the lock\n", timeout_ms);
	 *to = true;
	 return NULL;
      }
   *to = false;

   QoreQueueNode *n = head;
   head = head->next;
   if (!head)
      tail = NULL;

   len--;
   pthread_mutex_unlock(&qmutex);
   QoreNode *rv = n->node;
   n->node = NULL;
   n->del(NULL);
   printd(5, "Queue::shift() %08x\n", n);
   return rv;
}

inline QoreNode *Queue::pop()
{
   pthread_mutex_lock(&qmutex);
   // if there is no data, then wait for condition variable
   while (!head)
      pthread_cond_wait(&qcond, &qmutex);
   QoreQueueNode *n = tail;
   tail = tail->prev;
   if (!tail)
      head = NULL;

   len--;
   pthread_mutex_unlock(&qmutex);
   QoreNode *rv = n->node;
   n->node = NULL;
   n->del(NULL);
   printd(5, "Queue::shift() %08x\n", n);
   return rv;
}

inline QoreNode *Queue::pop(int timeout_ms, bool *to)
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

	 // lock has timed out, return -1
	 printd(0, "rc = %d: Queue::pop() timed out after %dms waiting on another thread to release the lock\n", rc, timeout_ms);
	 *to = true;
	 return NULL;
      }
   *to = false;

   QoreQueueNode *n = tail;
   tail = tail->prev;
   if (!tail)
      head = NULL;

   len--;
   pthread_mutex_unlock(&qmutex);
   QoreNode *rv = n->node;
   n->node = NULL;
   n->del(NULL);
   printd(5, "Queue::shift() %08x\n", n);
   return rv;
}

inline void Queue::deref(class ExceptionSink *xsink)
{
   if (ROdereference())
   {
      while (head)
      {
	 printd(5, "Queue::~Queue() deleting %08x (node %08x type %s)\n",
		head, head->node, head->node ? head->node->type->name : "(null)");
	 QoreQueueNode *w = head->next;
	 head->del(xsink);
	 head = w;
      }
      delete this;
   }
}

#endif // _QORE_CLASS_QUEUE
