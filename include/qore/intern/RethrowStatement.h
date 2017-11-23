/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  RethrowStatement.h

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

#ifndef _QORE_RETHROWSTATEMENT_H

#define _QORE_RETHROWSTATEMENT_H

#include "qore/intern/AbstractStatement.h"

class RethrowStatement : public AbstractStatement {
private:
   DLLLOCAL virtual int execImpl(QoreValue& return_value, ExceptionSink *xsink) {
      xsink->rethrow(catchGetException());
      return 0;
   }
   DLLLOCAL virtual int parseInitImpl(LocalVar *oflag, int pflag = 0) {
      if (!(pflag & PF_RETHROW_OK))
         parseException(loc, "RETHROW-NOT-IN-CATCH-BLOCK", "rethrow statements are only allowed in catch blocks");
      return 0;
   }

public:
   DLLLOCAL RethrowStatement(int start_line, int end_line) : AbstractStatement(start_line, end_line) {
   }

   DLLLOCAL virtual ~RethrowStatement() {
   }

   DLLLOCAL virtual bool endsBlock() const {
      return true;
   }

   DLLLOCAL virtual bool hasFinalReturn() const {
      // throwing an exception trumps any return statement
      return true;
   }
};

#endif
