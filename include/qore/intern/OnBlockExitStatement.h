/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  OnBlockExitStatement.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

#ifndef _QORE_ONBLOCKEXIT_STATEMENT_H

#define _QORE_ONBLOCKEXIT_STATEMENT_H

#include <qore/intern/AbstractStatement.h>

class OnBlockExitStatement : public AbstractStatement {
private:
   class StatementBlock *code;
   enum obe_type_e type;

   DLLLOCAL virtual int execImpl(QoreValue& return_value, class ExceptionSink *xsink);
   DLLLOCAL virtual int parseInitImpl(LocalVar *oflag, int pflag = 0);

public:
   DLLLOCAL OnBlockExitStatement(int start_line, int end_line, class StatementBlock *n_code, enum obe_type_e type = OBE_Unconditional);
   DLLLOCAL ~OnBlockExitStatement();
   DLLLOCAL class StatementBlock *getCode() const {
      return code;
   }
   DLLLOCAL enum obe_type_e getType() const {
      return type;
   }
   // faked here, checked at runtime
   DLLLOCAL virtual bool hasFinalReturn() const {
      return true;
   }
};

#endif
