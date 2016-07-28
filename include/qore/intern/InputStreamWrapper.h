/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  InputStreamWrapper.h

  Qore Programming Language

  Copyright (C) 2016 Qore Technologies, sro

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
#include "qore/intern/core/Exception.h"
#include "qore/intern/core/StringBuilder.h"

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

   DLLLOCAL virtual int64 read(void *ptr, int64 limit) override {
      ExceptionSink xsink;
      int64 r = read(ptr, limit, &xsink);
      if (xsink) {
         throw qore::ExceptionWrapper(xsink.catchException());
      }
      return r;
   }

private:
   DLLLOCAL int64 read(void *ptr, int64 limit, ExceptionSink *xsink) {
      assert(limit > 0);
      ReferenceHolder<QoreListNode> args(new QoreListNode(), xsink);
      args->push(new QoreBigIntNode(limit));
      ValueHolder bufHolder(self->evalMethodValue("read", *args, xsink), xsink);
      if (!bufHolder) {
         return 0;
      }
      BinaryNode *buf = bufHolder->get<BinaryNode>();
      qore_size_t count = buf->size();
      if (count == 0) {
         throw qore::Exception("INPUT-STREAM-ERROR", qore::StringBuilder() << self->getClassName()
               << "::read() returned an empty binary; NOTHING should be used to indicate the end of the stream");
      }
      if (count > static_cast<qore_size_t>(limit)) {
         throw qore::Exception("INPUT-STREAM-ERROR", qore::StringBuilder() << self->getClassName()
               << "::read() returned " << count << " bytes which is more than the specified limit of " << limit);
      }
      memcpy(ptr, buf->getPtr(), count);
      return count;
   }

private:
   QoreObject *self;                    //!< The QoreObject this private data is associated with
};

#endif // _QORE_INPUTSTREAMWRAPPER_H
