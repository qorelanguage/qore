/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreQueueHelper.h

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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

#ifndef _QORE_QOREQUEUEHELPER_H

#define _QORE_QOREQUEUEHELPER_H

class QoreQueueHelper : QorePrivateObjectAccessHelper {
protected:

public:
   //! acquires an internal Queue object from an object and maintains a reference count during the lifetime of the QoreQueueHelper object, throws a Qore-language exception if no Queue object is available
   /** @param obj the object that inherits the Queue class
       @param xs if any errors occur, the Qore-language exception information is stored here
    */
   DLLEXPORT QoreQueueHelper(QoreObject* obj, ExceptionSink* xs);

   //! dereferences the internal Queue object
   DLLEXPORT ~QoreQueueHelper();

   //! pushes a value at the end of the queue
   DLLEXPORT void push(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms = 0, bool* to = 0);

   //! inserts a value at the beginning of the queue
   DLLEXPORT void insert(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms = 0, bool* to = 0);

   //! returns a value from the beginning of the queue and removes it from the queue
   DLLEXPORT AbstractQoreNode* shift(ExceptionSink* xsink, int timeout_ms = 0, bool* to = 0);

   //! returns a value from the end of the queue and removes it from the queue
   DLLEXPORT AbstractQoreNode* pop(ExceptionSink* xsink, int timeout_ms = 0, bool* to = 0);

   //! returns the number of elements in the queue
   DLLEXPORT int size() const;

   //! returns the maximum size of the queue or -1 if there is no limit
   DLLEXPORT int getMax() const;

   //! returns the number of threads waiting on data in the queue
   DLLEXPORT unsigned getReadWaiting() const;

   //! returns the number of threads waiting on the thread to have capacity to store more values
   DLLEXPORT unsigned getWriteWaiting() const;

   //! clears the queue
   DLLEXPORT void clear(ExceptionSink* xsink);
};

#endif // _QORE_QOREQUEUEHELPER_H
