/*
  Statement.cc

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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

#include <qore/Qore.h>
#include <qore/intern/StatementBlock.h>
#include <qore/intern/OnBlockExitStatement.h>
#include <qore/intern/ParserSupport.h>
#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/UserFunctionList.h>
#include <qore/minitest.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef DEBUG_TESTS
#  include "tests/Statement_tests.cc"
#endif

// parse variable stack
class VNode {
   public:
      LocalVar *lvar;
      VNode *next;
      
      DLLLOCAL VNode(LocalVar *lv) : lvar(lv) { }
      DLLLOCAL const char *getName() const { return lvar->getName(); }
};

LVList::LVList(int num) {
   if (num) {
      lv = new lvar_ptr_t[num];
      // pop variables off stack and save in reverse order
      for (int i = num - 1; i >= 0; --i)
	 lv[i] = pop_local_var();
   }
   else
      lv = 0;

   num_lvars = num;
}

LVList::~LVList() {
   if (lv)
      delete [] lv;
}

AbstractQoreNode *StatementBlock::exec(ExceptionSink *xsink) {
   AbstractQoreNode *return_value = 0;
   execImpl(&return_value, xsink);
   return return_value;
}

// line numbers on statement blocks are set later
StatementBlock::StatementBlock(AbstractStatement *s) : AbstractStatement(-1, -1), lvars(0) {
   addStatement(s);
}

void StatementBlock::addStatement(class AbstractStatement *s) {
   //QORE_TRACE("StatementBlock::addStatement()");

   if (s) {
      statement_list.push_back(s);
      OnBlockExitStatement *obe = dynamic_cast<OnBlockExitStatement *>(s);
      if (obe)
	 on_block_exit_list.push_front(std::make_pair(obe->getType(), obe->getCode()));
   }
}

StatementBlock::~StatementBlock() {
   //QORE_TRACE("StatementBlock::~StatementBlock()");

   for (statement_list_t::iterator i = statement_list.begin(), e = statement_list.end(); i != e; ++i)
      delete *i;
   
   if (lvars)
      delete lvars;
}

int StatementBlock::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   QORE_TRACE("StatementBlock::execImpl()");
   int rc = 0;

   assert(xsink);

   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);

   //printd(5, "StatementBlock::execImpl() this=%08p, lvars=%08p, %d vars\n", this, lvars, lvars->num_lvars);

   bool obe = !on_block_exit_list.empty();
   // push on block exit iterator if necessary
   if (obe)
      pushBlock(on_block_exit_list.end());
   
   // execute block
   for (statement_list_t::iterator i = statement_list.begin(), e = statement_list.end(); i != e; ++i)
      if ((rc = (*i)->exec(return_value, xsink)) || xsink->isEvent())
	 break;

   // execute on block exit code if applicable
   if (obe) {
      ExceptionSink obe_xsink;
      int nrc = 0;
      bool error = *xsink;
      for (block_list_t::iterator i = popBlock(), e = on_block_exit_list.end(); i != e; ++i) {
	 enum obe_type_e type = (*i).first;
	 if (type == OBE_Unconditional || (!error && type == OBE_Success) || (error && type == OBE_Error))
	    nrc = (*i).second->execImpl(return_value, &obe_xsink);
      }
      if (obe_xsink)
	 xsink->assimilate(&obe_xsink);
      if (nrc)
	 rc = nrc;
   }

   return rc;
}

// top-level block (program) execution member function
void StatementBlock::exec() {
   ExceptionSink xsink;
   exec(&xsink);
}

LocalVar *push_local_var(const char *name, const QoreTypeInfo *typeInfo, bool check_dup) {
   VNode *vnode;

   QoreProgram *pgm = getProgram();

   LocalVar *lv = pgm->createLocalVar(name, typeInfo);

   // check stack for duplicate entries
   if (check_dup && pgm->checkWarning(QP_WARN_DUPLICATE_LOCAL_VARS)) {
      vnode = getVStack();
      while (vnode) {
	 if (!strcmp(vnode->getName(), name)) {
	    getProgram()->makeParseWarning(QP_WARN_DUPLICATE_LOCAL_VARS, "DUPLICATE-LOCAL-VARIABLE", "local variable '%s' was already declared in this lexical scope", name);
	    break;
	 }
	 vnode = vnode->next;
      }
   }
   
   //printd(5, "push_local_var(): pushing var %s\n", name);
   vnode = new VNode(lv);
   vnode->next = getVStack();
   updateVStack(vnode);
   return lv;
}

LocalVar *pop_local_var() {
   class VNode *vnode = getVStack();
   LocalVar *rc = vnode->lvar;

   assert(vnode);
   printd(5, "pop_local_var(): popping var %s\n", rc->getName());
   updateVStack(vnode->next);
   delete vnode;
   return rc;
}

LocalVar *find_local_var(const char *name, bool &in_closure) {
   VNode *vnode = getVStack();
   ClosureParseEnvironment *cenv = thread_get_closure_parse_env();
   in_closure = false;

   while (vnode) {
      if (cenv && cenv->getHighWaterMark() == vnode)
	 in_closure = true;
      if (!strcmp(vnode->getName(), name)) {
	 if (in_closure)
	    cenv->add(vnode->lvar);
	 return vnode->lvar;
      }
      vnode = vnode->next;
   }
   return 0;
}

int StatementBlock::parseInitIntern(LocalVar *oflag, int pflag) {
   QORE_TRACE("StatementBlock::parseInitIntern");

   int lvids = 0;

   AbstractStatement *ret = 0;
   for (statement_list_t::iterator i = statement_list.begin(), e = statement_list.end(), l = statement_list.last(); i != e; ++i) {
      lvids += (*i)->parseInit(oflag, pflag);
      if (!ret && i != l && (*i)->endsBlock()) {
	 // unreachable code found
	 getProgram()->makeParseWarning(QP_WARN_UNREACHABLE_CODE, "UNREACHABLE-CODE", "code after this statement can never be reached");
	 ret = *i;
      }
   }

   return lvids;
}

// may be called with this=0
int StatementBlock::parseInitTopLevel(RootQoreNamespace *rns, UserFunctionList *ufl, bool first) {
   QORE_TRACE("StatementBlock::parseInitTopLevel");

   if (!this) {
      rns->parseInit();
      ufl->parseInit();
      return 0;
   }

   int lvids = parseInitIntern(0);

   if (lvids && !first)
      parseException("ILLEGAL-TOP-LEVEL-LOCAL-VARIABLE", "local variables declared with 'my' in the top-level block of a Program object can only be declared in the very first block parsed");
   // now initialize root namespace and functions before local variables are popped off the stack
   rns->parseInit();
   ufl->parseInit();

   // this call will pop all local vars off the stack
   lvars = new LVList(lvids);

   //printd(5, "StatementBlock::parseInitTopLevel(this=%08p): done (lvars=%08p, %d vars, vstack = %08p)\n", this, lvars, lvids, getVStack());

   return 0;
}

int StatementBlock::parseInitImpl(LocalVar *oflag, int pflag) {
   QORE_TRACE("StatementBlock::parseInitImpl");

   printd(4, "StatementBlock::parseInitImpl(b=%08p, oflag=%08p)\n", this, oflag);

   int lvids = parseInitIntern(oflag, pflag);

   // this call will pop all local vars off the stack
   lvars = new LVList(lvids);

   //printd(5, "StatementBlock::parseInitImpl(this=%08p): done (lvars=%08p, %d vars, vstack = %08p)\n", this, lvars, lvids, getVStack());

   return 0;
}

// NOTE: can also be called with this = 0
void StatementBlock::parseInit(UserParamList *params) {
   QORE_TRACE("StatementBlock::parseInit");

   UserParamListLocalVarHelper ph(params);

   // initialize code block
   if (this)
      parseInitImpl(0);
}

// can also be called with this=NULL
void StatementBlock::parseInitMethod(const QoreTypeInfo *typeInfo, UserParamList *params, BCList *bcl) {
   QORE_TRACE("StatementBlock::parseInitMethod");

   UserParamListLocalVarHelper ph(params, typeInfo);

   // set oflag to selfid
   LocalVar *oflag = params->selfid;

   // initialize base constructor arguments
   if (bcl) {
      int tlvids = 0;
      for (bclist_t::iterator i = bcl->begin(); i != bcl->end(); i++) {
	 if ((*i)->args) {
	    QoreListNode *l = (*i)->args;
	    for (unsigned j = 0; j < l->size(); j++) {
	       AbstractQoreNode **n = l->get_entry_ptr(j);
	       if (*n) {
		  const QoreTypeInfo *argTypeInfo = 0;
		  (*n) = (*n)->parseInit(oflag, PF_REFERENCE_OK, tlvids, argTypeInfo);
	       }
	    }
	 }
      }
      if (tlvids) {
	 parse_error("illegal local variable declaration in base constructor argument");
	 while (tlvids--)
	    pop_local_var();
      }
   }

   // initialize code block
   if (this)
      parseInitImpl(oflag);
}

// can also be called with this=NULL
void StatementBlock::parseInitClosure(UserParamList *params, const QoreTypeInfo *classTypeInfo, lvar_set_t *vlist) {
   QORE_TRACE("StatementBlock::parseInitClosure");

   ClosureParseEnvironment cenv(vlist);

   UserParamListLocalVarHelper ph(params, classTypeInfo);

   // initialize code block
   if (this)
      parseInitImpl(params->selfid);
}
