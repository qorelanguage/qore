/*
 QoreQueue.cpp

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

#include <qore/Qore.h>
#include <qore/intern/QC_Queue.h>

QoreQueueHelper::QoreQueueHelper(QoreObject* obj, ExceptionSink* xs) : QorePrivateObjectAccessHelper(xs) {
   Queue* q = reinterpret_cast<Queue*>(obj->getReferencedPrivateData(CID_QUEUE, xs));
   if (!q) {
      if (!*xsink)
         xsink->raiseException("QUEUE-ERROR", "expecting an object derived from Queue; got class '%s' instead", obj->getClassName());
      return;
   }
   ptr = (void*)q;
}

QoreQueueHelper::~QoreQueueHelper() {
}

void QoreQueueHelper::push(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms, bool* to) {
   reinterpret_cast<Queue*>(ptr)->push(xsink, n, timeout_ms, to);
}

void QoreQueueHelper::insert(ExceptionSink* xsink, const AbstractQoreNode* n, int timeout_ms, bool* to) {
   reinterpret_cast<Queue*>(ptr)->insert(xsink, n, timeout_ms, to);
}

AbstractQoreNode* QoreQueueHelper::shift(ExceptionSink* xsink, int timeout_ms, bool* to) {
   return reinterpret_cast<Queue*>(ptr)->shift(xsink, timeout_ms, to);
}

AbstractQoreNode* QoreQueueHelper::pop(ExceptionSink* xsink, int timeout_ms, bool* to) {
   return reinterpret_cast<Queue*>(ptr)->pop(xsink, timeout_ms, to);
}

int QoreQueueHelper::size() const {
   return reinterpret_cast<Queue*>(ptr)->size();
}

int QoreQueueHelper::getMax() const {
   return reinterpret_cast<Queue*>(ptr)->getMax();
}

unsigned QoreQueueHelper::getReadWaiting() const {
   return reinterpret_cast<Queue*>(ptr)->getReadWaiting();
}

unsigned QoreQueueHelper::getWriteWaiting() const {
   return reinterpret_cast<Queue*>(ptr)->getWriteWaiting();
}

void QoreQueueHelper::clear(ExceptionSink* xsink) {
   return reinterpret_cast<Queue*>(ptr)->clear(xsink);
}
