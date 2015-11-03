/*
  OnBlockExitStatement.cc

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
#include <qore/intern/OnBlockExitStatement.h>
#include <qore/intern/StatementBlock.h>

OnBlockExitStatement::OnBlockExitStatement(int start_line, int end_line, class StatementBlock *n_code, enum obe_type_e n_type) : AbstractStatement(start_line, end_line), code(n_code), type(n_type)
{
}

OnBlockExitStatement::~OnBlockExitStatement()
{
   delete code;
}

int OnBlockExitStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink)
{
   // "activate" this block when the block exits in the thread "on block exit" stack
   advanceOnBlockExit();
   return 0;
}

int OnBlockExitStatement::parseInitImpl(LocalVar *oflag, int pflag)
{
   if (code)
      code->parseInitImpl(oflag, pflag);

   return 0;
}

