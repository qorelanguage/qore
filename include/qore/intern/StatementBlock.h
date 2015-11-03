/*
  StatementBlock.h

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

#ifndef _QORE_STATEMENT_BLOCK_H

#define _QORE_STATEMENT_BLOCK_H

#include <qore/intern/AbstractStatement.h>
#include <qore/safe_dslist> 

#include <set>

// all definitions in this file are private to the library and subject to change
typedef std::set<LocalVar *> lvar_set_t;

class RootQoreNamespace;
class UserFunctionList;

class LVList {
   public:
      int num_lvars;
      LocalVar **lv;
      
      DLLLOCAL LVList(int num);
      DLLLOCAL ~LVList();
};

class LVListInstantiator {
      const LVList *l;
      ExceptionSink *xsink;

   public:
      DLLLOCAL LVListInstantiator(const LVList *n_l, ExceptionSink *xs) : l(n_l), xsink(xs) {
	 if (!l) return;
	 for (int i = 0; i < l->num_lvars; ++i)
	    l->lv[i]->instantiate(0);
      }

      DLLLOCAL ~LVListInstantiator() {
	 if (!l) return;
	 for (int i = 0; i < l->num_lvars; ++i)
	    l->lv[i]->uninstantiate(xsink);
      }
};

class StatementBlock : public AbstractStatement
{
   private:
      typedef safe_dslist<AbstractStatement *> statement_list_t;
      statement_list_t statement_list;
      block_list_t on_block_exit_list;
      LVList *lvars;

      DLLLOCAL int parseInitIntern(LocalVar *oflag, int pflag = 0);

   public:
      DLLLOCAL StatementBlock(AbstractStatement *s);
      DLLLOCAL virtual ~StatementBlock();
      DLLLOCAL virtual int execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink);
      DLLLOCAL virtual int parseInitImpl(LocalVar *oflag, int pflag = 0);
      DLLLOCAL int parseInitTopLevel(RootQoreNamespace *rns, UserFunctionList *ufl, bool first);

      DLLLOCAL void addStatement(AbstractStatement *s);
      DLLLOCAL AbstractQoreNode *exec(ExceptionSink *xsink);
      DLLLOCAL void parseInit(Paramlist *params);

      // initialize methods (bcl = subclass constructors with an explicit base class argument list)
      DLLLOCAL void parseInitMethod(Paramlist *params, class BCList *bcl); 

      // initialize closure blocks
      DLLLOCAL void parseInitClosure(Paramlist *params, bool in_method, lvar_set_t *vlist);

      DLLLOCAL void exec();

      DLLLOCAL const LVList *getLVList() const {
	 return lvars;
      }
};

#endif // _QORE_STATEMENT_BLOCK_H
