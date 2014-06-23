/*
  lib/minitest.cpp

  Support for unit testing

  Qore Programming Language

  Copyright (C) 2006 - 2013 Qore Technologies

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

#ifdef DEBUG
#include <qore/minitest.hpp>

//-----------------------------------------------------------------------------
// Support for execution of QoreString as a QoreProgram.
// The function needs to be named 'test'.
void run_Qore_test(QoreString& str, const char* file, int line, const char* details)
{
  assert(str.length());
  assert(file && file[0]);
  assert(line);

  ExceptionSink xsink;
  ReferenceHolder<QoreProgram> pgm(new QoreProgram(), &xsink);
  pgm->parse(str.getBuffer(), "test", &xsink); // function name needs to be always "test"
  if (xsink.isEvent()) {
    printf("QoreException throwm when parsing Qore program,\nfile %s, line %d.\n", file, line);
    if (details && details[0]) {
      printf("Details: %s\n", details);
    }
    xsink.handleExceptions();
    assert(false);
  }

  ReferenceHolder<AbstractQoreNode> rv(pgm->callFunction("test", 0, &xsink), &xsink);
  if (xsink.isEvent()) {
    printf("QoreException throwm when running Qore program,\nfile %s, line %d.\n", file, line);
    if (details && details[0]) {
      printf("Details: %s\n", details);
    }
    xsink.handleExceptions();
    assert(false);
  }

  if (!rv) {
    assert(false); // this would be Qore bug
  }

  QoreBoolNode *b = dynamic_cast<QoreBoolNode *>(*rv);
  if (!b) {
    printf("The Qore function 'test' should return boolean (true on success),\nfile %s, line %d.\n", file, line);
    assert(false);
  }

  if (!b->getValue()) {
    printf("Qore test program failed by returning 'False',\nfile %s, line %d.\n", file, line);
    if (details && details[0]) {
      printf("Details: %s\n", details);
    }
    assert(false);
  }
}  

#endif // DEBUG

// EOF

