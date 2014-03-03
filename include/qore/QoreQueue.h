/* -*- mode: c++; indent-tabs-mode: nil -*- */
/* 
   QoreQueue.h

   Qore Programming Language

   Copyright 2003 - 2014 David Nichols

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

class QoreQueueNode;

class qore_queue_private;

class QoreQueue {
   friend class qore_queue_private;
protected:
   // private implementation
   qore_queue_private* priv;

public:
   //! creates the queue with the given maximum size; -1 means no maximum size
   DLLEXPORT QoreQueue(int n_max = -1);

   //! copy constructor
   DLLEXPORT QoreQueue(const QoreQueue &orig);

   //! destructor
   /** queues should not be deleted when other threads might be accessing them
    */
   DLLEXPORT ~QoreQueue();

   //! push at the end of the queue and take the reference; can only be used when len == -1
   DLLEXPORT void pushAndTakeRef(AbstractQoreNode* n);

   //! push at the end of the queue
   DLLEXPORT void push(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms = 0, bool* to = 0);

   //! insert at the beginning of the queue
   DLLEXPORT void insert(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms = 0, bool* to = 0);

   //! remove a node from the beginning of the queue
   DLLEXPORT AbstractQoreNode* shift(ExceptionSink* xsink, int timeout_ms = 0, bool* to = 0);

   //! remove a node from the end of the queue
   DLLEXPORT AbstractQoreNode* pop(ExceptionSink* xsink, int timeout_ms = 0, bool* to = 0);

   //! returns true if the queue is empty
   DLLEXPORT bool empty() const;

   //! returns the number of elements in the queue
   /** FIXME: change to size_t
    */
   DLLEXPORT int size() const;

   //! returns the maximum size of the queue
   DLLEXPORT int getMax() const;

   //! returns the number of threads currently waiting to read data
   DLLEXPORT unsigned getReadWaiting() const;

   //! returns the number of threads currently waiting to write data
   DLLEXPORT unsigned getWriteWaiting() const;

   //! clears the queue
   DLLEXPORT void clear(ExceptionSink* xsink);
};

class Queue : public AbstractPrivateData, public QoreQueue {
protected:
   DLLEXPORT virtual ~Queue() {}

public:
   DLLEXPORT Queue(int max = -1) : QoreQueue(max) {}

   DLLEXPORT Queue* eventRefSelf() const {
      ((Queue*)this)->ref();
      return (Queue*)this;
   }
};

#endif // _QORE_QOREQUEUE_H
