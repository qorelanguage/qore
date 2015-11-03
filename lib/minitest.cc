/*
  lib/minitest.cc

  Support for unit testing

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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

