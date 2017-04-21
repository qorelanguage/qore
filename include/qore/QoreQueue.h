/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreQueue.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
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
   DLLEXPORT QoreQueue(const QoreQueue& orig);

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

   //! push at the end of the queue
   DLLEXPORT void push(ExceptionSink* xsink, AbstractQoreNode* n, int timeout_ms = 0, bool* to = 0);

   //! insert at the beginning of the queue
   DLLEXPORT void insert(ExceptionSink* xsink, AbstractQoreNode* n, int timeout_ms = 0, bool* to = 0);

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

   //! sets a queue error status and provides exception information to throw if queue operations are attempted; the queue is also cleared and can no longer be written to after this operation
   /** if called more than once, subsequent calls replace the data in the object

       @since Qore 0.8.12
    */
   DLLEXPORT void setError(const char* err, const QoreStringNode* desc, ExceptionSink* xsink);

   //! clears any queue error status to make the object usable again
   /**
      @since Qore 0.8.12
   */
   DLLEXPORT void clearError();
};

class Queue : public AbstractPrivateData, public QoreQueue {
protected:
   DLLEXPORT virtual ~Queue();

public:
   DLLEXPORT Queue(int max = -1);

   DLLEXPORT Queue(const Queue& old);

   DLLEXPORT Queue* queueRefSelf() const;

   DLLEXPORT virtual void deref(ExceptionSink* xsink);
};

#endif // _QORE_QOREQUEUE_H
