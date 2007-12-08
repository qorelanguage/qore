/*
  StatementBlock.h

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

#ifndef _QORE_STATEMENT_BLOCK_H

#define _QORE_STATEMENT_BLOCK_H

#include <qore/intern/AbstractStatement.h>
#include <qore/safe_dslist> 

#include <list>

// all definitions in this file are private to the library and subject to change

class LVList {
   public:
      int num_lvars;
      lvh_t *ids;
      
      DLLLOCAL LVList(int num);
      DLLLOCAL ~LVList();
};

class StatementBlock : public AbstractStatement
{
   private:
      typedef safe_dslist<AbstractStatement *> statement_list_t;
      statement_list_t statement_list;
      block_list_t on_block_exit_list;

   public:
      class LVList *lvars;

      DLLLOCAL StatementBlock(AbstractStatement *s);
      DLLLOCAL virtual ~StatementBlock();
      DLLLOCAL virtual int execImpl(class QoreNode **return_value, class ExceptionSink *xsink);
      DLLLOCAL virtual int parseInitImpl(lvh_t oflag, int pflag = 0);

      DLLLOCAL void addStatement(AbstractStatement *s);
      DLLLOCAL class QoreNode *exec(ExceptionSink *xsink);
      DLLLOCAL void parseInit(class Paramlist *params);
      // initialize subclass constructors with an explicit base class argument list
      DLLLOCAL void parseInit(class Paramlist *params, class BCList *bcl); 
      DLLLOCAL void exec();
};

#endif // _QORE_STATEMENT_BLOCK_H
