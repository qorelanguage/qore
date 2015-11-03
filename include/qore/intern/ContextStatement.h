/*
 ContextStatement.h
 
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

#ifndef _QORE_CONTEXTSTATEMENT_H

#define _QORE_CONTEXTSTATEMENT_H

#include "intern/AbstractStatement.h"

#include <qore/safe_dslist>

// context mod types defined in Context.h

class ContextMod {
public:
   int type;
   union ContextModUnion {
      class AbstractQoreNode *exp;
   } c;
   
   DLLLOCAL ContextMod(int t, class AbstractQoreNode *e);
   DLLLOCAL ~ContextMod();
};

typedef safe_dslist<ContextMod *> cxtmod_list_t;

class ContextModList : public cxtmod_list_t
{
public:
   DLLLOCAL ContextModList(ContextMod *cm);
   DLLLOCAL ~ContextModList();
   DLLLOCAL void addContextMod(ContextMod *cm);
};

class ContextStatement :public AbstractStatement
{
   protected:
      DLLLOCAL virtual int execImpl(class AbstractQoreNode **return_value, class ExceptionSink *xsink);
      DLLLOCAL virtual int parseInitImpl(LocalVar *oflag, int pflag = 0);

   public:
      char *name;
      class AbstractQoreNode *exp, *where_exp, *sort_ascending, *sort_descending;
      class StatementBlock *code;
      class LVList *lvars;
      
      DLLLOCAL ContextStatement(int start_line, int end_line, char *n, class AbstractQoreNode *expr, class ContextModList *cm, class StatementBlock *cd);
      DLLLOCAL virtual ~ContextStatement();
};

#endif
