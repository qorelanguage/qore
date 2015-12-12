/*
  OnBlockExitStatement.cpp

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

#include <qore/Qore.h>
#include <qore/intern/OnBlockExitStatement.h>
#include <qore/intern/StatementBlock.h>

OnBlockExitStatement::OnBlockExitStatement(int start_line, int end_line, class StatementBlock *n_code, enum obe_type_e n_type) : AbstractStatement(start_line, end_line), code(n_code), type(n_type) {
}

OnBlockExitStatement::~OnBlockExitStatement() {
   delete code;
}

int OnBlockExitStatement::execImpl(QoreValue& return_value, ExceptionSink *xsink) {
   // "activate" this block when the block exits in the thread "on block exit" stack
   advanceOnBlockExit();
   return 0;
}

int OnBlockExitStatement::parseInitImpl(LocalVar *oflag, int pflag) {
   if (code)
      code->parseInitImpl(oflag, pflag & ~PF_TOP_LEVEL);

   return 0;
}
