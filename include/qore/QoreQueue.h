/* 
   QoreQueue.h

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

#ifndef _QORE_QOREQUEUE_H

#define _QORE_QOREQUEUE_H

#include <qore/LockedObject.h>
#include <qore/QoreCondition.h>

class QoreQueueNode 
{
   public:
      class QoreNode *node;
      class QoreQueueNode *next;
      class QoreQueueNode *prev;

      DLLLOCAL QoreQueueNode(QoreNode *n);
      DLLLOCAL void del(class ExceptionSink *xsink);
};

class QoreQueue
{
   private:
      enum queue_status_e { Queue_Deleted = -1 };

      LockedObject l;
      QoreCondition cond;
      QoreQueueNode *head, *tail;
      int len;
      int waiting;

   public:
      DLLLOCAL QoreQueue();
      DLLLOCAL ~QoreQueue();
      DLLLOCAL QoreQueue(QoreNode *n);
      // push at the end of the queue
      DLLLOCAL void push(QoreNode *n);
      // insert at the beginning of the queue
      DLLLOCAL void insert(QoreNode *n);
      DLLLOCAL QoreNode *shift(class ExceptionSink *xsink, int timeout_ms = 0, bool *to = 0);
      DLLLOCAL QoreNode *pop(class ExceptionSink *xsink, int timeout_ms = 0, bool *to = 0);
      DLLLOCAL int size() const
      {
	 return len;
      }
      DLLLOCAL int getWaiting() const
      {
	 return waiting;
      }
      DLLLOCAL void destructor(class ExceptionSink *xsink);
};

#endif // _QORE_QOREQUEUE_H
