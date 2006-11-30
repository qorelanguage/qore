/*
  Statement.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

// parse context stack
class CVNode 
{
   public:
      char *name;
      class CVNode *next;
      CVNode(char *n) { name = n; }
};

// parse variable stack
class VNode {
  public:
      char *name;
      class VNode *next;
      inline VNode(char *nme) { name = nme; }
};

class LVList {
   public:
      int num_lvars;
      lvh_t *ids;
      LVList(int num);
      ~LVList();
};

class ContextMod {
   public:
      int type;
      class ContextMod *next;
      union ContextModUnion {
	    class QoreNode *exp;
      } c;

      ContextMod(int t, class QoreNode *e);
      ~ContextMod();
};

class ContextModList {
   private:
      class ContextMod *head, *tail; 
      int num;

   public:
      ContextModList(ContextMod *cm);
      ~ContextModList();
      void addContextMod(ContextMod *cm);
      class ContextMod *getHead()
      {
	 return head;
      }
      int size()
      {
	 return num;
      }
};

class ContextStatement {
   public:
      char *name;
      class QoreNode *exp;
      class ContextModList *mods;
      class StatementBlock *code;
      class LVList *lvars;

      inline ContextStatement(char *n, class QoreNode *expr, class ContextModList *cm, class StatementBlock *cd);
      inline ~ContextStatement();
      inline int exec(class QoreNode **return_value, class ExceptionSink *xsink);
      inline int execSummary(class QoreNode **return_value, class ExceptionSink *xsink);
      inline void parseInit(lvh_t oflag, int pflag = 0);
};

class IfStatement {
   private:
      class QoreNode *cond;
      class StatementBlock *if_code;
      class StatementBlock *else_code;
      class LVList *lvars;

   public:
      inline IfStatement(class QoreNode *c, class StatementBlock *i, class StatementBlock *e);
      inline ~IfStatement();
      inline int exec(class QoreNode **return_value, class ExceptionSink *xsink);
      inline void parseInit(lvh_t oflag, int pflag = 0);
};

class WhileStatement {
   private:
      class QoreNode *cond;
      class StatementBlock *code;
      class LVList *lvars;

   public:
      inline WhileStatement(class QoreNode *c, class StatementBlock *cd);
      inline ~WhileStatement();
      inline int execWhile(class QoreNode **return_value, class ExceptionSink *xsink);
      inline int execDoWhile(class QoreNode **return_value, class ExceptionSink *xsink);
      inline void parseWhileInit(lvh_t oflag, int pflag = 0);
      inline void parseDoWhileInit(lvh_t oflag, int pflag = 0);
};

class ForStatement {
      class QoreNode *assignment;
      class QoreNode *cond;
      class QoreNode *iterator;
      class StatementBlock *code;
      class LVList *lvars;

   public:
      inline ForStatement(class QoreNode *a, class QoreNode *c, class QoreNode *i, class StatementBlock *cd);
      inline ~ForStatement();
      inline int exec(class QoreNode **return_value, class ExceptionSink *xsink);
      inline void parseInit(lvh_t oflag, int pflag = 0);
};

class ForEachStatement {
   private:
      class QoreNode *var;
      class QoreNode *list;
      class StatementBlock *code;
      class LVList *lvars;

      inline int execRef(class QoreNode **return_value, class ExceptionSink *xsink);

   public:
      inline ForEachStatement(class QoreNode *v, class QoreNode *l, class StatementBlock *cd);
      inline ~ForEachStatement();
      inline int exec(class QoreNode **return_value, class ExceptionSink *xsink);
      inline void parseInit(lvh_t oflag, int pflag = 0);
};

class DeleteStatement {
   private:
      class QoreNode *var;

   public:
      inline DeleteStatement(class QoreNode *v);
      inline ~DeleteStatement();
      inline void exec(ExceptionSink *xsink);
      inline int parseInit(lvh_t oflag, int pflag = 0);
};

class ThrowStatement {
   private:
      class QoreNode *args;

   public:
      inline ThrowStatement(class QoreNode *v);
      inline ~ThrowStatement();
      inline void exec(ExceptionSink *xsink);
      inline int parseInit(lvh_t oflag, int pflag = 0);
};

class TryStatement {
   public:
      class StatementBlock *try_block;
      class StatementBlock *catch_block;
      //class StatementBlock *finally;
      char *param;
      lvh_t id;

   public:
      inline TryStatement(class StatementBlock *t, class StatementBlock *c, char *p);
      inline ~TryStatement();
      inline int exec(class QoreNode **return_value, class ExceptionSink *xsink);
      inline void parseInit(lvh_t oflag, int pflag = 0);
};

class Statement {
   private:
      inline void setPosition();

   public:
      int Type;
      int LineNumber;
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

      inline Statement(int type, class QoreNode *node);
      inline Statement(int type);
      inline Statement(class StatementBlock *block);
      ~Statement();
      int exec(class QoreNode **return_value, class ExceptionSink *xsink);
      int parseInit(lvh_t oflag, int pflag = 0);
      class Statement *next;
      //void parseInitBlock(lvh_t oflag);
};

class StatementBlock {
   private:
      int allocated;
      class Statement *head;
      class Statement *tail;      

   public:
      class LVList *lvars;
      inline StatementBlock(Statement *s);
      inline ~StatementBlock();
      inline void addStatement(Statement *s);
      int exec(class QoreNode **return_value, class ExceptionSink *xsink);
      inline class QoreNode *exec(ExceptionSink *xsink);
      void parseInit(lvh_t oflag, int pflag = 0);
      void parseInit(class Paramlist *params);
      // initialize subclass constructors with an explicit base class argument list
      void parseInit(class Paramlist *params, class BCList *bcl); 
      void exec();
};

#include <qore/QoreNode.h>
#include <qore/support.h>
#include <qore/Context.h>
#include <qore/Variable.h>
#include <qore/Function.h>
#include <qore/SwitchStatement.h>
#include <qore/qore_thread.h>

#include <stdio.h>
#include <stdlib.h>

inline class QoreNode *StatementBlock::exec(ExceptionSink *xsink)
{
   class QoreNode *return_value = NULL;
   exec(&return_value, xsink);
   return return_value;
}

inline IfStatement::IfStatement(class QoreNode *c, class StatementBlock *i, class StatementBlock *e)
{
   cond = c;
   if_code = i;
   else_code = e;
   lvars = NULL;
}

inline IfStatement::~IfStatement()
{
   cond->deref(NULL);
   if (if_code)
      delete if_code;
   if (else_code)
      delete else_code;
   if (lvars)
      delete lvars;
}

inline WhileStatement::WhileStatement(class QoreNode *c, class StatementBlock *cd)
{
   cond = c;
   code = cd;
   lvars = NULL;
}

inline WhileStatement::~WhileStatement()
{
   cond->deref(NULL);
   if (code)
      delete code;
   if (lvars)
      delete lvars;
}

inline ForStatement::ForStatement(class QoreNode *a, class QoreNode *c, class QoreNode *i, class StatementBlock *cd)
{
   assignment = a;
   cond = c;
   iterator = i;
   code = cd;
   lvars = NULL;
}

inline ForStatement::~ForStatement()
{
   if (assignment)
      assignment->deref(NULL);
   if (cond)
      cond->deref(NULL);
   if (iterator)
      iterator->deref(NULL);
   if (code)
      delete code;
   if (lvars)
      delete lvars;
}

inline ForEachStatement::ForEachStatement(class QoreNode *v, class QoreNode *l, class StatementBlock *cd)
{
   var = v;
   list = l;
   code = cd;
   lvars = NULL;
}

inline ForEachStatement::~ForEachStatement()
{
   var->deref(NULL);
   list->deref(NULL);
   if (code)
      delete code;
   if (lvars)
      delete lvars;
}

inline DeleteStatement::DeleteStatement(class QoreNode *v)
{
   var = v;
}

inline DeleteStatement::~DeleteStatement()
{
   var->deref(NULL);
}

inline TryStatement::TryStatement(class StatementBlock *t, class StatementBlock *c, char *p)
{
   try_block = t;
   catch_block = c;
   param = p;
   /*
   finally = f;
   */
}

inline TryStatement::~TryStatement()
{
   if (param)
      free(param);
   if (try_block)
      delete try_block;
   if (catch_block)
      delete catch_block;
   /*
   if (finally)
      delete finally;
   */
}

inline ThrowStatement::ThrowStatement(class QoreNode *v)
{
   if (!v)
   {
      args = NULL;
      return;
   }
   if (v->type == NT_LIST)
   {
      // take list
      args = v;
      return;
   }
   class List *l = new List(1);
   l->push(v);
   args = new QoreNode(l);
}

inline ThrowStatement::~ThrowStatement()
{
   if (args)
      args->deref(NULL);
}

#include <qore/Variable.h>
inline ContextStatement::ContextStatement(char *n, class QoreNode *expr, class ContextModList *cm, class StatementBlock *cd)
{
   name = n;
   exp = expr;
   mods = cm;
   code = cd;
   lvars = NULL;
}

inline ContextStatement::~ContextStatement()
{
   if (name)
      free(name);
   if (exp)
      exp->deref(NULL);
   if (mods)
      delete mods;
   if (code)
      delete code;
   if (lvars)
      delete lvars;
}

inline Statement::Statement(int type, class QoreNode *node)
{
   next = NULL;
   setPosition();
   Type       = type;
   s.node     = node;
}

inline Statement::Statement(int type)
{
   next = NULL;
   setPosition();
   Type       = type;
}

inline Statement::Statement(class StatementBlock *b)
{
   next = NULL;
   setPosition();
   Type        = S_SUB_BLOCK;
   s.sub_block = b;
}

inline void Statement::setPosition()
{
   LineNumber = get_pgm_counter();
   FileName   = get_pgm_file();
}

#define STATEMENT_BLOCK 20

inline StatementBlock::StatementBlock(Statement *s)
{
   allocated = STATEMENT_BLOCK;
   head = tail = s;
   lvars = NULL;
}

inline void StatementBlock::addStatement(class Statement *s)
{
   //tracein("StatementBlock::addStatement()");
   // if statement was blank then return already-present block
   if (!s) return;

   if (!head)
      head = s;
   else
      tail->next = s;
   tail = s;

   //traceout("StatementBlock::addStatement()");
}

inline StatementBlock::~StatementBlock()
{
   //tracein("StatementBlock::~StatementBlock()");

   while (head)
   {
      tail = head->next;
      delete head;
      head = tail;
   }

   if (lvars)
      delete lvars;
   //traceout("StatementBlock::~StatementBlock()");
}

inline LVList::LVList(int num)
{
   if (num)
      ids = new lvh_t[num];
   else
      ids = NULL;
   num_lvars = num;
}

inline LVList::~LVList()
{
   if (ids)
      delete [] ids;
}

#endif // _QORE_STATEMENT_H
