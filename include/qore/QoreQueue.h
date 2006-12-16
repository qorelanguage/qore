/* 
   QoreQueue.h

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

#ifndef _QORE_QOREQUEUE_H

#define _QORE_QOREQUEUE_H


class QoreQueueNode 
{
   public:
      class QoreNode *node;
      class QoreQueueNode *next;
      class QoreQueueNode *prev;

      DLLLOCAL QoreQueueNode(QoreNode *n, class QoreQueueNode *tail);
      DLLLOCAL void del(class ExceptionSink *xsink);
};

class QoreQueue
{
   private:
      pthread_mutex_t qmutex;
      pthread_cond_t  qcond;
      QoreQueueNode *head, *tail;
      int len;

   public:
      DLLEXPORT QoreQueue();
      DLLEXPORT ~QoreQueue();
      DLLEXPORT QoreQueue(QoreNode *n);
      DLLEXPORT void push(QoreNode *n);
      DLLEXPORT QoreNode *shift();
      DLLEXPORT QoreNode *shift(int timeout_ms, bool *to);
      DLLEXPORT QoreNode *pop();
      DLLEXPORT QoreNode *pop(int timeout_ms, bool *to);
      DLLEXPORT int size() const;
      DLLEXPORT void del(class ExceptionSink *xsink);
};

#endif // _QORE_QOREQUEUE_H
