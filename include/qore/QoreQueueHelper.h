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
   DLLEXPORT QoreQueueHelper(QoreObject* obj, ExceptionSink* xs);

   DLLEXPORT ~QoreQueueHelper();

   // push at the end of the queue
   DLLEXPORT void push(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms = 0, bool* to = 0);

   // insert at the beginning of the queue
   DLLEXPORT void insert(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms = 0, bool* to = 0);

   DLLEXPORT AbstractQoreNode* shift(ExceptionSink* xsink, int timeout_ms = 0, bool* to = 0);
   DLLEXPORT AbstractQoreNode* pop(ExceptionSink* xsink, int timeout_ms = 0, bool* to = 0);

   DLLEXPORT int size() const;

   DLLEXPORT int getMax() const;

   DLLEXPORT unsigned getReadWaiting() const;

   DLLEXPORT unsigned getWriteWaiting() const;

   DLLEXPORT void clear(ExceptionSink* xsink);
};

#endif // _QORE_QOREQUEUEHELPER_H
