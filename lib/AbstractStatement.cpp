/*
  AbstractStatement.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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
#include <qore/intern/AbstractStatement.h>
#include <qore/intern/qore_program_private.h>

#include <typeinfo>

AbstractStatement::AbstractStatement(int sline, int eline) : loc(sline, eline) {
   QoreProgram *pgm = getProgram();
   if (pgm)
      pwo = qore_program_private::getParseWarnOptions(pgm);
}

int AbstractStatement::exec(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   printd(1, "AbstractStatement::exec() this: %p type: %s file: %s line: %d\n", this, typeid(this).name(), loc.file, loc.start_line);
   update_runtime_location(loc);

#ifdef QORE_MANAGE_STACK
   if (check_stack(xsink))
      return 0;
#endif
   pthread_testcancel();

   QoreProgramBlockParseOptionHelper bh(pwo.parse_options);
   return execImpl(return_value, xsink);
}

int AbstractStatement::parseInit(LocalVar *oflag, int pflag) {
   printd(2, "AbstractStatement::parseInit() this: %p type: %s file: %s line: %d\n", this, typeid(this).name(), loc.file, loc.start_line);
   // set parse options and warning mask for this statement
   ParseWarnHelper pwh(pwo);

   // set pgm position in case of errors
   update_parse_location(loc);
   return parseInitImpl(oflag, pflag);
}

