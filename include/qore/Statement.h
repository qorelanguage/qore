/*
  Statement.h

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

#ifndef _QORE_STATEMENT_H

#define _QORE_STATEMENT_H

#include <qore/common.h>

#define S_IF            1
#define S_EXPRESSION    2
#define S_WHILE         3
#define S_DO_WHILE      4
#define S_FOR           5
#define S_SUB_BLOCK     6
#define S_RETURN        7
#define S_BREAK         8
#define S_CONTINUE      9
#define S_CONTEXT      10
#define S_SUMMARY      11
#define S_SUBCONTEXT   12
#define S_FOREACH      13
#define S_DELETE       14
#define S_TRY          15
#define S_THROW        16
#define S_THREAD_EXIT  17
#define S_SWITCH       18
#define S_RETHROW      19
#define S_TEMP         -1

#define RC_RETURN       1
#define RC_BREAK        2
#define RC_CONTINUE     3

// all definitions in this file are private to the library and subject to change

DLLLOCAL int process_node(class QoreNode **node, lvh_t oflag, int pflag);

class LVList {
   public:
      int num_lvars;
      lvh_t *ids;
      
      DLLLOCAL LVList(int num);
      DLLLOCAL ~LVList();
};

class Statement {
   public:
      int Type;
      int LineNumber;
      int EndLineNumber;
      char *FileName;
      union StatementTypeUnion
      {
	    // for if statements
	    class IfStatement *If;
	    // for while and do {} while loops
	    class WhileStatement *While;
	    // for for loops
	    class ForStatement *For;
	    // for variable assigments and function calls
	    class QoreNode *node;
	    // for a sub-block of code
	    class StatementBlock *sub_block;
	    // for foreach statements
	    class ForEachStatement *ForEach;
	    // for delete statements
	    class DeleteStatement *Delete;
	    // for try/catch/finish statements
	    class TryStatement *Try;
	    // for throw statements
	    class ThrowStatement *Throw;
	    // for context statements
	    class ContextStatement *SContext;
	    // for switch statements
	    class SwitchStatement *Switch;
      } s;
      class Statement *next;

      DLLLOCAL Statement(int sline, int eline, int type, class QoreNode *node);
      DLLLOCAL Statement(int sline, int eline, int type);
      DLLLOCAL Statement(int sline, int eline, class StatementBlock *block);
      DLLLOCAL ~Statement();
      DLLLOCAL int exec(class QoreNode **return_value, class ExceptionSink *xsink);
      DLLLOCAL int parseInit(lvh_t oflag, int pflag = 0);
};

class StatementBlock {
   private:
      int allocated;
      class Statement *head;
      class Statement *tail;      

   public:
      class LVList *lvars;

      DLLLOCAL StatementBlock(Statement *s);
      DLLLOCAL ~StatementBlock();
      DLLLOCAL void addStatement(Statement *s);
      DLLLOCAL int exec(class QoreNode **return_value, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *exec(ExceptionSink *xsink);
      DLLLOCAL void parseInit(lvh_t oflag, int pflag = 0);
      DLLLOCAL void parseInit(class Paramlist *params);
      // initialize subclass constructors with an explicit base class argument list
      DLLLOCAL void parseInit(class Paramlist *params, class BCList *bcl); 
      DLLLOCAL void exec();
};

DLLLOCAL void push_cvar(char *name);
DLLLOCAL void pop_cvar();
DLLLOCAL lvh_t pop_local_var();

#endif // _QORE_STATEMENT_H
