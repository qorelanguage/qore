/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  InputStreamWrapper.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 David Nichols

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

#ifndef _QORE_INPUTSTREAMWRAPPER_H
#define _QORE_INPUTSTREAMWRAPPER_H

#include "qore/InputStream.h"

/**
 * @brief Implements the private data for all subclasses of Qore::InputStream implemented in the Qore language.
 */
class InputStreamWrapper : public InputStream {

public:
   /**
    * @brief Constructor.
    * @param self the QoreObject this private data is associated with
    */
   InputStreamWrapper(QoreObject *self) : self(self) {
   }

   DLLLOCAL virtual void close(ExceptionSink* xsink) /*override*/ {
      self->evalMethodValue("close", 0, xsink);
   }

   DLLLOCAL virtual int64 read(int64 timeout, ExceptionSink* xsink) /*override*/ {
      ReferenceHolder<QoreListNode> args(new QoreListNode(), xsink);
      args->push(new QoreBigIntNode(timeout));
      return self->evalMethodValue("read", *args, xsink).getAsBigInt();
   }

   DLLLOCAL virtual int64 bulkRead(void *ptr, int64 limit, int64 timeout, ExceptionSink *xsink) /*override*/ {
      assert(limit > 0);
      ReferenceHolder<QoreListNode> args(new QoreListNode(), xsink);
      args->push(new QoreBigIntNode(limit));
      args->push(new QoreBigIntNode(timeout));
      ValueHolder bufHolder(self->evalMethodValue("bulkRead", *args, xsink), xsink);
      if (!bufHolder) {
         return 0;
      }
      BinaryNode *buf = bufHolder->get<BinaryNode>();
      qore_size_t count = buf->size();
      if (count == 0) {
         xsink->raiseException("INPUT-STREAM-ERROR",
               "%s::bulkRead() returned an empty binary; NOTHING should be used to indicate the end of the stream",
               self->getClassName());
         return 0;
      }
      if (count > static_cast<qore_size_t>(limit)) {
         xsink->raiseException("INPUT-STREAM-ERROR",
               "%s::bulkRead() returned %lu bytes which is more than the specified limit of %lu",
               self->getClassName(), count, static_cast<qore_size_t>(limit));
         return 0;
      }
      memcpy(ptr, buf->getPtr(), count);
      return count;
   }

private:
   QoreObject *self;                    //!< The QoreObject this private data is associated with
};

#endif // _QORE_INPUTSTREAMWRAPPER_H
