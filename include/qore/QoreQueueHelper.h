/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreQueueHelper.h

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
