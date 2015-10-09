/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  StatementBlock.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_STATEMENT_BLOCK_H

#define _QORE_STATEMENT_BLOCK_H

#include <qore/intern/AbstractStatement.h>
#include <qore/safe_dslist>

#include <set>
#include <vector>
#include <typeinfo>

// all definitions in this file are private to the library and subject to change
class BCAList;
class BCList;

class LVList {
public:
   typedef std::vector<LocalVar*> lv_vec_t;
   lv_vec_t lv;

   DLLLOCAL LVList(int num) {
      add(num);
   }

   DLLLOCAL LVList(const LVList& old) {
      lv.resize(old.size());
      for (unsigned i = 0; i < old.size(); ++i)
         lv[i] = old.lv[i];

      //printd(5, "LVList::LVList() populated with %d vars\n", lv.size());
   }

   DLLLOCAL ~LVList() {
   }

   DLLLOCAL qore_size_t size() const {
      return lv.size();
   }

   DLLLOCAL void add(int num) {
      assert(num > 0);
      unsigned start = lv.size();
      //printd(5, "LVList::add(num: %d) this: %p start: %d\n", num, this, start);
      lv.resize(start + num);
      // pop variables off stack and save in reverse order
      for (int i = (int)(start + num - 1); i >= (int)start; --i) {
         lv[i] = pop_local_var();
         //printd(5, "LVList::add() %d = %p: %s\n", i, lv[i], lv[i]->getName());
      }
   }
};

class LVListInstantiator {
   const LVList* l;
   ExceptionSink* xsink;

public:
   DLLLOCAL LVListInstantiator(const LVList* n_l, ExceptionSink* xs) : l(n_l), xsink(xs) {
      if (!l) return;
      for (unsigned i = 0; i < l->size(); ++i) {
         //printd(5, "LVListInstantiator::LVListInstantiator() this: %p v: %p %s\n", this, l->lv[i], l->lv[i]->getName());
         l->lv[i]->instantiate();
      }
   }

   DLLLOCAL ~LVListInstantiator() {
      if (!l) return;
      for (int i = (int)l->size() - 1; i >= 0; --i) {
         //printd(5, "LVListInstantiator::~LVListInstantiator() this: %p v: %p %s\n", this, l->lv[i], l->lv[i]->getName());
         l->lv[i]->uninstantiate(xsink);
      }
   }
};

class StatementBlock : public AbstractStatement {
protected:
   typedef safe_dslist<AbstractStatement*> statement_list_t;
   statement_list_t statement_list;
   block_list_t on_block_exit_list;
   LVList* lvars;

   // start must be the element before the start position
   DLLLOCAL int parseInitIntern(LocalVar* oflag, int pflag, statement_list_t::iterator start);
   DLLLOCAL bool hasLastReturn(AbstractStatement* as);
   DLLLOCAL void parseCheckReturn();

   DLLLOCAL int execIntern(QoreValue& return_value, ExceptionSink* xsink);

   DLLLOCAL StatementBlock() : AbstractStatement(-1, -1), lvars(0) {
   }

public:
   DLLLOCAL StatementBlock(int sline, int eline) : AbstractStatement(sline, eline), lvars(0) {
   }

   // line numbers on statement blocks are set later
   DLLLOCAL StatementBlock(int sline, int eline, AbstractStatement* s) : AbstractStatement(sline, eline), lvars(0) {
      addStatement(s);
   }

   DLLLOCAL virtual ~StatementBlock() {
      del();
   }

   DLLLOCAL virtual int execImpl(QoreValue& return_value, ExceptionSink* xsink);
   DLLLOCAL virtual int parseInitImpl(LocalVar* oflag, int pflag = 0);

   DLLLOCAL void del();

   DLLLOCAL void addStatement(AbstractStatement* s);

   using AbstractStatement::exec;
   DLLLOCAL QoreValue exec(ExceptionSink* xsink);

   using AbstractStatement::parseInit;
   DLLLOCAL void parseInit(UserVariantBase* uvb);

   // initialize methods
   DLLLOCAL void parseInitMethod(const QoreTypeInfo* typeInfo, UserVariantBase* uvb);
   DLLLOCAL void parseInitConstructor(const QoreTypeInfo* typeInfo, UserVariantBase* uvb, BCAList* bcal, const QoreClass& cls);

   // initialize closure blocks
   DLLLOCAL void parseInitClosure(UserVariantBase* uvb, const QoreTypeInfo* classTypeInfo, lvar_set_t *vlist);

   DLLLOCAL void exec();

   DLLLOCAL const LVList* getLVList() const {
      return lvars;
   }

   DLLLOCAL virtual bool hasFinalReturn() const {
      if (statement_list.empty())
         return false;

      return (*statement_list.last())->hasFinalReturn();
   }

   DLLLOCAL void setupLVList(int lvids) {
      assert(!lvars);
      if (!lvids)
         return;

      lvars = new LVList(lvids);
   }
};

class TopLevelStatementBlock : public StatementBlock {
protected:
   // iterator to last commit element in statement list
   statement_list_t::iterator hwm;
   // true only the first time parseInit() is called
   bool first;

public:
   DLLLOCAL TopLevelStatementBlock() : hwm(statement_list.end()), first(true) {
   }

   DLLLOCAL virtual ~TopLevelStatementBlock() {
   }

   using StatementBlock::parseInit;
   DLLLOCAL void parseInit(int64 po);

   DLLLOCAL void parseCommit() {
      hwm = statement_list.last();
   }

   DLLLOCAL void parseRollback() {
      // delete all statements after the high water mark (hwm) to the end of the list
      statement_list_t::iterator start = hwm;
      if (start != statement_list.end())
         ++start;
      else
         start = statement_list.begin();

      for (statement_list_t::iterator i = start, e = statement_list.end(); i != e; ++i)
         delete *i;

      statement_list.erase_to_end(hwm);
   }

   // local vars are not instantiated here because they are instantiated by the QoreProgram object
   DLLLOCAL virtual int execImpl(QoreValue& return_value, ExceptionSink* xsink);

   // assign inherited local var list from parent program
   DLLLOCAL void assignLocalVars(const LVList* lvl) {
      assert(!lvars);
      lvars = new LVList(*lvl);
   }

   DLLLOCAL void setupLVList(int lvids) {
      if (!lvids)
         return;

      if (lvars)
         lvars->add(lvids);
      else
         lvars = new LVList(lvids);
   }
};

#endif // _QORE_STATEMENT_BLOCK_H
