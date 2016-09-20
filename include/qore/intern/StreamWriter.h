/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  StreamWriter.h

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

#ifndef _QORE_STREAMWRITER_H
#define _QORE_STREAMWRITER_H

#include <cstdint>

#include "qore/qore_bitopts.h"
#include "qore/OutputStream.h"

/**
 * @brief Private data for the Qore::StreamWriter class.
 */
class StreamWriter : public AbstractPrivateData {

public:
   DLLLOCAL StreamWriter(ExceptionSink* xsink, OutputStream* os, const QoreEncoding* enc = QCS_DEFAULT) :
      out(os, xsink),
      encoding(enc) {
   }

   DLLLOCAL const QoreEncoding* getEncoding() const {
      return encoding;
   }

   DLLLOCAL void print(const QoreStringNode* str, ExceptionSink* xsink) {
      TempEncodingHelper stmp(str, encoding, xsink);
      out->write(stmp->getBuffer(), stmp->size(), xsink);
   }

   DLLLOCAL void printf(const QoreValueList* args, ExceptionSink* xsink) {
      SimpleRefHolder<QoreStringNode> str(q_sprintf(args, 0, 0, xsink));
      if (str)
         print(*str, xsink);
   }

   DLLLOCAL void vprintf(const QoreValueList* args, ExceptionSink* xsink) {
      SimpleRefHolder<QoreStringNode> str(q_vsprintf(args, 0, 0, xsink));
      if (str)
         print(*str, xsink);
   }

   DLLLOCAL void f_printf(const QoreValueList* args, ExceptionSink* xsink) {
      SimpleRefHolder<QoreStringNode> str(q_sprintf(args, 1, 0, xsink));
      if (str)
         print(*str, xsink);
   }

   DLLLOCAL void f_vprintf(const QoreValueList* args, ExceptionSink* xsink) {
      SimpleRefHolder<QoreStringNode> str(q_vsprintf(args, 1, 0, xsink));
      if (str)
         print(*str, xsink);
   }

   DLLLOCAL void write(const BinaryNode* b, ExceptionSink* xsink) {
      out->write(b->getPtr(), b->size(), xsink);
   }

   DLLLOCAL void writei1(char i, ExceptionSink* xsink) {
      out->write(&i, 1, xsink);
   }

   DLLLOCAL void writei2(int16_t i, ExceptionSink* xsink) {
      i = htons(i);
      out->write(&i, 2, xsink);
   }

   DLLLOCAL void writei4(int32_t i, ExceptionSink* xsink) {
      i = htonl(i);
      out->write(&i, 4, xsink);
   }

   DLLLOCAL void writei8(int64 i, ExceptionSink* xsink) {
      i = i8MSB(i);
      out->write(&i, 8, xsink);
   }

   DLLLOCAL void writei2LSB(int16_t i, ExceptionSink* xsink) {
      i = i2LSB(i);
      out->write(&i, 2, xsink);
   }

   DLLLOCAL void writei4LSB(int32_t i, ExceptionSink* xsink) {
      i = i4LSB(i);
      out->write(&i, 4, xsink);
   }

   DLLLOCAL void writei8LSB(int64 i, ExceptionSink* xsink) {
      i = i8LSB(i);
      out->write(&i, 8, xsink);
   }

   DLLLOCAL virtual const char* getName() const { return "StreamWriter"; }

private:
   ReferenceHolder<OutputStream> out;
   const QoreEncoding* encoding;
};

#endif // _QORE_STREAMWRITER_H
