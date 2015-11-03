/*
  OnBlockExitStatement.h

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

#ifndef _QORE_ONBLOCKEXIT_STATEMENT_H

#define _QORE_ONBLOCKEXIT_STATEMENT_H

#include <qore/intern/AbstractStatement.h>

class OnBlockExitStatement : public AbstractStatement
{
   private:
      class StatementBlock *code;
      enum obe_type_e type;

      DLLLOCAL virtual int execImpl(class AbstractQoreNode **return_value, class ExceptionSink *xsink);
      DLLLOCAL virtual int parseInitImpl(LocalVar *oflag, int pflag = 0);

   public:
      DLLLOCAL OnBlockExitStatement(int start_line, int end_line, class StatementBlock *n_code, enum obe_type_e type = OBE_Unconditional);
      DLLLOCAL ~OnBlockExitStatement();
      DLLLOCAL class StatementBlock *getCode() const
      {
	 return code;
      }
      DLLLOCAL enum obe_type_e getType() const
      {
	 return type;
      }
};

#endif
