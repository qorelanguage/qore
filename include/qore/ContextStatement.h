/*
 ContextStatement.h
 
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

#ifndef _QORE_CONTEXTSTATEMENT_H

#define _QORE_CONTEXTSTATEMENT_H

#include <qore/safe_dslist>

// context mod types defined in Context.h

class ContextMod {
public:
   int type;
   union ContextModUnion {
      class QoreNode *exp;
   } c;
   
   DLLLOCAL ContextMod(int t, class QoreNode *e);
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

class ContextStatement {
public:
   char *name;
   class QoreNode *exp, *where_exp, *sort_ascending, *sort_descending, *summarize;
   class StatementBlock *code;
   class LVList *lvars;
   
   DLLLOCAL ContextStatement(char *n, class QoreNode *expr, class ContextModList *cm, class StatementBlock *cd, class QoreNode *summ_exp = NULL);
   DLLLOCAL ~ContextStatement();
   DLLLOCAL int exec(class QoreNode **return_value, class ExceptionSink *xsink);
   DLLLOCAL int execSummary(class QoreNode **return_value, class ExceptionSink *xsink);
   DLLLOCAL void parseInit(lvh_t oflag, int pflag = 0);
};

#endif
