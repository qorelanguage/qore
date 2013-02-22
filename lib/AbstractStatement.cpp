/*
  AbstractStatement.cpp

  Qore Programming Language

  Copyright 2003 - 2013 David Nichols

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

