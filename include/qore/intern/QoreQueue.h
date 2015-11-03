/* 
   QoreQueue.h

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

#ifndef _QORE_QOREQUEUE_H

#define _QORE_QOREQUEUE_H

#include <qore/QoreThreadLock.h>
#include <qore/QoreCondition.h>

class QoreQueueNode {
   public:
      AbstractQoreNode *node;
      QoreQueueNode *next;
      QoreQueueNode *prev;

      DLLLOCAL QoreQueueNode(AbstractQoreNode *n);
      DLLLOCAL void del(ExceptionSink *xsink);
};

class QoreQueue {
   private:
      enum queue_status_e { Queue_Deleted = -1 };

      mutable QoreThreadLock l;
      QoreCondition cond;
      QoreQueueNode *head, *tail;
      int len;
      int waiting;

      DLLLOCAL void push_internal(AbstractQoreNode *v);
      DLLLOCAL void insert_internal(AbstractQoreNode *v);

   public:
      DLLLOCAL QoreQueue();
      DLLLOCAL QoreQueue(const QoreQueue &orig);
      DLLLOCAL ~QoreQueue();

      // push at the end of the queue and take the reference
      DLLLOCAL void push_and_take_ref(AbstractQoreNode *n);

      // push at the end of the queue
      DLLLOCAL void push(const AbstractQoreNode *n);

      // insert at the beginning of the queue and take the reference
      DLLLOCAL void insert_and_take_ref(AbstractQoreNode *n);

      // insert at the beginning of the queue
      DLLLOCAL void insert(const AbstractQoreNode *n);
      DLLLOCAL AbstractQoreNode *shift(ExceptionSink *xsink, int timeout_ms = 0, bool *to = 0);
      DLLLOCAL AbstractQoreNode *pop(ExceptionSink *xsink, int timeout_ms = 0, bool *to = 0);
      DLLLOCAL int size() const {
	 return len;
      }
      DLLLOCAL int getWaiting() const {
	 return waiting;
      }
      DLLLOCAL void destructor(ExceptionSink *xsink);
};

#endif // _QORE_QOREQUEUE_H
