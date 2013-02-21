/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AbstractStatement.h

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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

#define PF_RETURN_VALUE_IGNORED  (1 << 0)
#define PF_BACKGROUND            (1 << 1)
#define PF_RETHROW_OK            (1 << 2)
#define PF_FOR_ASSIGNMENT        (1 << 3)
#define PF_CONST_EXPRESSION      (1 << 4)
#define PF_TOP_LEVEL             (1 << 5) //!< parsing at the top-level of the program

// all definitions in this file are private to the library and subject to change

class AbstractStatement {
private:
   DLLLOCAL virtual int execImpl(AbstractQoreNode** return_value, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int parseInitImpl(LocalVar* oflag, int pflag = 0) = 0;

public:
   QoreProgramLocation loc;
   struct ParseWarnOptions pwo;

   DLLLOCAL AbstractStatement(int sline, int eline);

   DLLLOCAL virtual ~AbstractStatement() {}

   DLLLOCAL int exec(AbstractQoreNode** return_value, ExceptionSink* xsink);
   DLLLOCAL int parseInit(LocalVar* oflag, int pflag = 0);

   // statement should return true if it ends a block (break, continue, return, throw, etc)
   // meaning that any subsequent statements will be unconditionally skipped
   DLLLOCAL virtual bool endsBlock() const {
      return false;
   }

   // should return true if the statement is a declaration processed at parse time and should not go into the parse tree
   DLLLOCAL virtual bool isParseDeclaration() const {
      return false;
   }

   // should return true if the statement is a declaration and does not represent an executable statement
   DLLLOCAL virtual bool isDeclaration() const {
      return false;
   }

   DLLLOCAL virtual bool hasFinalReturn() const {
      return false;
   }
};

DLLLOCAL void push_cvar(const char* name);
DLLLOCAL void pop_cvar();
DLLLOCAL LocalVar* pop_local_var(bool set_unassigned = false);
DLLLOCAL int pop_local_var_get_id();
// used for constructor methods sharing a common "self" local variable and for top-level local variables
DLLLOCAL void push_local_var(LocalVar* lv);
DLLLOCAL LocalVar* push_local_var(const char* name, const QoreTypeInfo* typeInfo, bool is_arg = true, int n_refs = 0, bool top_level = false);
DLLLOCAL LocalVar* find_local_var(const char* name, bool &in_closure);

#endif // _QORE_ABSTRACTSTATEMENT_H
