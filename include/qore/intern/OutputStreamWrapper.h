/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  OutputStreamWrapper.h

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

#ifndef _QORE_OUTPUTSTREAMWRAPPER_H
#define _QORE_OUTPUTSTREAMWRAPPER_H

#include "qore/OutputStream.h"

/**
 * @brief Implements the private data for all subclasses of Qore::OutputStream implemented in the Qore language.
 */
class OutputStreamWrapper : public OutputStream {

public:
   /**
    * @brief Constructor.
    * @param self the QoreObject this private data is associated with
    */
   OutputStreamWrapper(QoreObject *self) : self(self) {
   }

   DLLLOCAL virtual void close() override {
      ExceptionSink xsink;
      self->evalMethodValue("close", 0, &xsink);
      if (xsink) {
         throw qore::ExceptionWrapper(xsink.catchException());
      }
   }

   DLLLOCAL virtual void write(const void *ptr, int64 count) override {
      ExceptionSink xsink;
      write(ptr, count, &xsink);
      if (xsink) {
         throw qore::ExceptionWrapper(xsink.catchException());
      }
   }

private:
   DLLLOCAL void write(const void *ptr, int64 count, ExceptionSink *xsink) {
      assert(count >= 0);

      SimpleRefHolder<BinaryNode> buf(new BinaryNode());
      buf->preallocate(count);
      memcpy(const_cast<void *>(buf->getPtr()), ptr, count);

      ReferenceHolder<QoreListNode> args(new QoreListNode(), xsink);
      args->push(buf.release());
      self->evalMethodValue("write", *args, xsink);
   }

private:
   QoreObject *self;                    //!< The QoreObject this private data is associated with
};

#endif // _QORE_OUTPUTSTREAMWRAPPER_H
