/*
  QoreStandardException.h

  Qore Programming language

  Copyright (C) 2007 - 2017 Qore Technologies, s.r.o.

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

#include <qore/Qore.h>

QoreStandardException::QoreStandardException(const char* err, const char* desc_fmt, ...) : err(new QoreStringNode(err)), desc(new QoreStringNode) {
   if (desc_fmt) {
      va_list args;

      while (true) {
         va_start(args, desc_fmt);
         int rc = desc->vsprintf(desc_fmt, args);
         va_end(args);
         if (!rc)
            break;
      }
   }
}

QoreStandardException::QoreStandardException(QoreStringNode* err, QoreStringNode* desc) : err(err), desc(desc) {
}

QoreStandardException::~QoreStandardException() {
   if (err)
      err->deref();
   if (desc)
      desc->deref();
}

void QoreStandardException::convert(ExceptionSink* xsink) {
   xsink->raiseException(err, desc);
   err = 0;
   desc = 0;
}
