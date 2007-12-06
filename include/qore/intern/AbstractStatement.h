/*
  AbstractStatement.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#ifndef _QORE_ABSTRACTSTATEMENT_H

#define _QORE_ABSTRACTSTATEMENT_H

#include <qore/common.h>

#define RC_RETURN       1
#define RC_BREAK        2
#define RC_CONTINUE     3

#define PF_BACKGROUND   1
#define PF_REFERENCE_OK 2
#define PF_RETHROW_OK   4

// all definitions in this file are private to the library and subject to change

DLLLOCAL int process_node(class QoreNode **node, lvh_t oflag, int pflag);

class AbstractStatement
{
   private:
      DLLLOCAL virtual int execImpl(class QoreNode **return_value, class ExceptionSink *xsink) = 0;
      DLLLOCAL virtual int parseInitImpl(lvh_t oflag, int pflag = 0) = 0;   

   public:
      int LineNumber;
      int EndLineNumber;
      const char *FileName;
      class AbstractStatement *next;

      DLLLOCAL AbstractStatement(int start_line, int end_line);
      DLLLOCAL virtual ~AbstractStatement()
      {
      }
      DLLLOCAL int exec(class QoreNode **return_value, class ExceptionSink *xsink);
      DLLLOCAL int parseInit(lvh_t oflag, int pflag = 0);
      // statement should return true if it ends a block (break, continue, return, throw, etc)
      // meaning that any subsequent statements will be unconditionally skipped
      DLLLOCAL virtual bool endsBlock() const
      {
	 return false;
      }
      // should return true if the statement is a declaration processed at parse time and should not go into the parse tree
      DLLLOCAL virtual bool isDeclaration() const
      {
	 return false;
      }
};

DLLLOCAL void push_cvar(char *name);
DLLLOCAL void pop_cvar();
DLLLOCAL lvh_t pop_local_var();
DLLLOCAL lvh_t push_local_var(char *name);
DLLLOCAL lvh_t find_local_var(char *name);

#endif // _QORE_ABSTRACTSTATEMENT_H
