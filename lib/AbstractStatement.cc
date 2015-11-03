/*
  AbstractStatement.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#include <typeinfo>

AbstractStatement::AbstractStatement(int start_line, int end_line) : LineNumber(start_line), EndLineNumber(end_line) {
   FileName = get_parse_file();
}

int AbstractStatement::exec(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   printd(1, "AbstractStatement::exec() type=%s file=%s line=%d\n", typeid(this).name(), FileName, LineNumber);   
   update_pgm_counter_pgm_file(LineNumber, EndLineNumber, FileName);

#ifdef QORE_MANAGE_STACK
   if (check_stack(xsink))
      return 0;
#endif
   pthread_testcancel();

   return execImpl(return_value, xsink);
}

int AbstractStatement::parseInit(LocalVar *oflag, int pflag) {
   printd(2, "AbstractStatement::parseInit() %08p type=%s line %d file %s\n", this, typeid(this).name(), LineNumber, FileName);
   // set pgm position in case of errors
   update_parse_location(LineNumber, EndLineNumber, FileName);
   return parseInitImpl(oflag, pflag);
}

