/*
  Statement.cpp

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

#include <qore/Qore.h>
#include <qore/intern/StatementBlock.h>
#include <qore/intern/OnBlockExitStatement.h>
#include <qore/intern/ParserSupport.h>
#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/qore_program_private.h>
#include <qore/intern/QoreNamespaceIntern.h>
#include <qore/minitest.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef DEBUG_TESTS
#  include "tests/Statement_tests.cpp"
#endif

// parse variable stack
class VNode {
protected:
   // # of times this variable is referenced in code
   int refs;

   // to store parse location in case of errors
   int first_line, last_line;
   const char *file;
   bool block_start;
   bool top_level;
   
public:
   LocalVar *lvar;
   VNode *next;

   DLLLOCAL VNode(LocalVar *lv, int n_refs = 0, bool n_top_level = false) : refs(n_refs), file(get_parse_file()), block_start(false), top_level(n_top_level), lvar(lv), next(getVStack()) {
      get_parse_location(first_line, last_line);
      updateVStack(this);
      //printd(5, "VNode::VNode() id=%p %s\n", lvar, lvar ? lvar->getName() : "n/a");
   }

   DLLLOCAL ~VNode() {
      //printd(5, "VNode::~VNode() id=%p %s\n", lvar, lvar ? lvar->getName() : "n/a");
      if (lvar && !refs)
	 getProgram()->makeParseWarning(first_line, last_line, file, QP_WARN_UNREFERENCED_VARIABLE, "UNREFERENCED-VARIABLE", "local variable '%s' was declared in this block but not referenced; to disable this warning, use '%%disable-warning unreferenced-variable' in your code", lvar->getName());
   }

   DLLLOCAL void setRef() {
      ++refs;
   }

   DLLLOCAL bool setBlockStart(bool bs = true) {
      bool rc = block_start;
      block_start = bs;
      return rc;
   }

   DLLLOCAL bool isBlockStart() const {
      return block_start;
   }

   DLLLOCAL bool isReferenced() const {
      return refs;
   }

   DLLLOCAL int refCount() const {
      return refs;
   }

   DLLLOCAL bool isTopLevel() const {
      return top_level;
   }

   DLLLOCAL const char *getName() const {
      return lvar->getName();
   }

   // searches to marker and then jumps to global thread-local variables
   DLLLOCAL VNode *nextSearch() const {
      if (!next || next->lvar)
	 return next;

      // skip to global thread-local variables
      VNode *rv = get_global_vnode();
      //printd(5, "VNode::nextSearch() returning global VNode %p\n", rv);
      return rv;
   }
};

class BlockStartHelper {
protected:
   bool bs;

public:
   DLLLOCAL BlockStartHelper() {
      VNode *v = getVStack();
      //printd(5, "BlockStartHelper::BlockStartHelper() v=%p ibs=%d\n", v, v ? v->isBlockStart() : 0);
      bs = v ? v->setBlockStart(true) : true;
   }
   DLLLOCAL ~BlockStartHelper() {
      //printd(5, "BlockStartHelper::~BlockStartHelper() bs=%d\n", bs);
      if (!bs)
	 getVStack()->setBlockStart(false);
   }
};

// pushes a marker on the local variable parse stack so that searches can skip to global thread-local variables when the search hits the marker
class VariableBlockHelper {
public:
   DLLLOCAL VariableBlockHelper() {
      new VNode(0);
      //printd(5, "VariableBlockHelper::VariableBlockHelper() this=%p pushed %p\n", this, 0);
   }
   DLLLOCAL ~VariableBlockHelper() {
      std::auto_ptr<VNode> vnode(getVStack()); 
      assert(vnode.get()); 
      updateVStack(vnode->next);
      //printd(5, "VariableBlockHelper::~VariableBlockHelper() this=%p got %p\n", this, vnode->lvar);
   }
};

AbstractQoreNode *StatementBlock::exec(ExceptionSink *xsink) {
   AbstractQoreNode *return_value = 0;
   execImpl(&return_value, xsink);
   return return_value;
}

void StatementBlock::addStatement(AbstractStatement *s) {
   //QORE_TRACE("StatementBlock::addStatement()");

   if (s) {
      statement_list.push_back(s);
      OnBlockExitStatement *obe = dynamic_cast<OnBlockExitStatement *>(s);
      if (obe)
	 on_block_exit_list.push_front(std::make_pair(obe->getType(), obe->getCode()));

      EndLineNumber = s->EndLineNumber;
   }
}

void StatementBlock::del() {
   //QORE_TRACE("StatementBlock::del()");

   for (statement_list_t::iterator i = statement_list.begin(), e = statement_list.end(); i != e; ++i)
      delete *i;

   statement_list.clear();
   
   if (lvars) {
      delete lvars;
      lvars = 0;
   }
}

int StatementBlock::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);

   return execIntern(return_value, xsink);
}

int StatementBlock::execIntern(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   QORE_TRACE("StatementBlock::execImpl()");
   int rc = 0;

   assert(xsink);

   //printd(5, "StatementBlock::execImpl() this=%p, lvars=%p, %ld vars\n", this, lvars, lvars->size());

   bool obe = !on_block_exit_list.empty();
   // push "on block exit" iterator if necessary
   if (obe)
      pushBlock(on_block_exit_list.end());
   
   // execute block
   for (statement_list_t::iterator i = statement_list.begin(), e = statement_list.end(); i != e; ++i)
      if ((rc = (*i)->exec(return_value, xsink)) || xsink->isEvent())
	 break;

   // execute "on block exit" code if applicable
   if (obe) {
      ExceptionSink obe_xsink;
      int nrc = 0;
      bool error = *xsink;
      for (block_list_t::iterator i = popBlock(), e = on_block_exit_list.end(); i != e; ++i) {
	 enum obe_type_e type = (*i).first;
	 if (type == OBE_Unconditional || (!error && type == OBE_Success) || (error && type == OBE_Error))
	    if ((*i).second)
	       nrc = (*i).second->execImpl(return_value, &obe_xsink);
      }
      if (obe_xsink)
	 xsink->assimilate(obe_xsink);
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

static void push_top_level_local_var(LocalVar *lv) {
   new VNode(lv, 1, true);
}

// used for constructor methods sharing a common "self" local variable
void push_local_var(LocalVar *lv) {
   new VNode(lv, 1);
}

LocalVar *push_local_var(const char *name, const QoreTypeInfo *typeInfo, bool check_dup, int n_refs, bool top_level) {
   QoreProgram *pgm = getProgram();

   LocalVar *lv = pgm->createLocalVar(name, typeInfo);

   bool found_block = false;
   // check stack for duplicate entries
   bool avs = checkParseOption(PO_ASSUME_LOCAL);
   if (check_dup && (pgm->checkWarning(QP_WARN_DUPLICATE_LOCAL_VARS | QP_WARN_DUPLICATE_BLOCK_VARS) || avs)) {
      VNode *vnode = getVStack();
      while (vnode) {
	 printd(5, "push_local_var() vnode=%p %s (top: %s) ibs=%d found_block=%d\n", vnode, vnode->getName(), vnode->isTopLevel() ? "true" : "false", vnode->isBlockStart(), found_block);
	 if (!found_block && vnode->isBlockStart())
	    found_block = true;
	 if (!strcmp(vnode->getName(), name)) {
	    if (!found_block && avs) {
	       parse_error("local variable '%s' was already declared in the same block", name);
	    }
	    else {
	       if (!found_block)
		  getProgram()->makeParseWarning(QP_WARN_DUPLICATE_BLOCK_VARS, "DUPLICATE-BLOCK-VARIABLE", "local variable '%s' was already declared in the same block", name);
	       else {
		  if (top_level || !vnode->isTopLevel())
		     getProgram()->makeParseWarning(QP_WARN_DUPLICATE_LOCAL_VARS, "DUPLICATE-LOCAL-VARIABLE", "local variable '%s' was already declared in this lexical scope", name);
	       }
	       break;
	    }
	 }
	 vnode = vnode->nextSearch();
      }
   }
   
   //printd(5, "push_local_var(): pushing var %s\n", name);
   new VNode(lv, n_refs, top_level);
   return lv;
}

int pop_local_var_get_id() {
   std::auto_ptr<VNode> vnode(getVStack());
   assert(vnode.get());
   int refs = vnode->refCount();
   printd(5, "pop_local_var_get_id(): popping var %s (refs=%d)\n", vnode->lvar->getName(), refs);
   updateVStack(vnode->next);
   return refs;
}

LocalVar *pop_local_var() {
   std::auto_ptr<VNode> vnode(getVStack());
   assert(vnode.get());
   LocalVar *rc = vnode->lvar;
   printd(5, "pop_local_var(): popping var %s\n", rc->getName());
   updateVStack(vnode->next);
   return rc;
}

LocalVar *find_local_var(const char *name, bool &in_closure) {
   VNode *vnode = getVStack();
   ClosureParseEnvironment *cenv = thread_get_closure_parse_env();
   in_closure = false;

   while (vnode) {
      if (cenv && !in_closure && cenv->getHighWaterMark() == vnode)
	 in_closure = true;
      if (!strcmp(vnode->getName(), name)) {
	 if (in_closure)
	    cenv->add(vnode->lvar);
	 vnode->setRef();
	 return vnode->lvar;
      }
      vnode = vnode->nextSearch();
   }
   return 0;
}

int StatementBlock::parseInitIntern(LocalVar *oflag, int pflag, statement_list_t::iterator start) {
   QORE_TRACE("StatementBlock::parseInitIntern");

   int lvids = 0;

   AbstractStatement *ret = 0;

   if (start != statement_list.end())
      ++start;
   else
      start = statement_list.begin();

   for (statement_list_t::iterator i = start, l = statement_list.last(), e = statement_list.end(); i != e; ++i) {
      lvids += (*i)->parseInit(oflag, pflag);
      if (!ret && i != l && (*i)->endsBlock()) {
	 // unreachable code found
	 getProgram()->makeParseWarning(QP_WARN_UNREACHABLE_CODE, "UNREACHABLE-CODE", "code after this statement can never be reached");
	 ret = *i;
      }
   }

   return lvids;
}

int StatementBlock::parseInitImpl(LocalVar *oflag, int pflag) {
   QORE_TRACE("StatementBlock::parseInitImpl");

   printd(4, "StatementBlock::parseInitImpl(b=%p, oflag=%p)\n", this, oflag);

   BlockStartHelper bsh;
   
   int lvids = parseInitIntern(oflag, pflag & ~PF_TOP_LEVEL, statement_list.end());

   // this call will pop all local vars off the stack
   setupLVList(lvids);

   //printd(5, "StatementBlock::parseInitImpl(this=%p): done (lvars=%p, %d vars, vstack = %p)\n", this, lvars, lvids, getVStack());

   return 0;
}

// can also be called with this = 0
void StatementBlock::parseInit(UserVariantBase *uvb) {
   QORE_TRACE("StatementBlock::parseInit");

   VariableBlockHelper vbh;

   UserParamListLocalVarHelper ph(uvb);

   // initialize code block
   if (this)
      parseInitImpl(0);

   parseCheckReturn();
}

// can also be called with this = 0
void StatementBlock::parseCheckReturn() {
   const QoreTypeInfo *returnTypeInfo = getReturnTypeInfo();
   if (returnTypeInfo->hasType() && !returnTypeInfo->parseAccepts(nothingTypeInfo)) {
      // make sure the last statement is a return statement if the block has a return type
      if (!this || statement_list.empty() || !(*statement_list.last())->hasFinalReturn()) {
	 QoreStringNode *desc = new QoreStringNode("this code block has declared return type ");
	 returnTypeInfo->getThisType(*desc);
	 desc->concat(" but does not have a return statement as the last statement in the block");
	 if (!this)
	    qore_program_private::makeParseException(getProgram(), "MISSING-RETURN", desc);
	 else
	    qore_program_private::makeParseException(getProgram(), QoreProgramLocation(LineNumber, EndLineNumber, FileName), "MISSING-RETURN", desc);
      }
   }
}

// can also be called with this=NULL
void StatementBlock::parseInitMethod(const QoreTypeInfo *typeInfo, UserVariantBase *uvb) {
   QORE_TRACE("StatementBlock::parseInitMethod");

   VariableBlockHelper vbh;

   UserParamListLocalVarHelper ph(uvb, typeInfo);

   // initialize code block
   if (this)
      parseInitImpl(uvb->getUserSignature()->selfid);

   parseCheckReturn();
}

// can also be called with this=NULL
void StatementBlock::parseInitConstructor(const QoreTypeInfo *typeInfo, UserVariantBase *uvb, BCAList *bcal, BCList *bcl) {
   QORE_TRACE("StatementBlock::parseInitConstructor");

   VariableBlockHelper vbh;

   UserParamListLocalVarHelper ph(uvb, typeInfo);

   // if there is a base constructor list, resolve all classes and 
   // ensure that all classes referenced are base classes of this class
   if (bcal) {
      for (bcalist_t::iterator i = bcal->begin(), e = bcal->end(); i != e; ++i) {
	 assert(typeInfo->getUniqueReturnClass());
	 (*i)->parseInit(bcl, typeInfo->getUniqueReturnClass()->getName());
      }
   }

   // initialize code block
   if (this)
      parseInitImpl(uvb->getUserSignature()->selfid);
}

// can also be called with this=NULL
void StatementBlock::parseInitClosure(UserVariantBase *uvb, const QoreTypeInfo *classTypeInfo, lvar_set_t *vlist) {
   QORE_TRACE("StatementBlock::parseInitClosure");

   ClosureParseEnvironment cenv(vlist);

   UserParamListLocalVarHelper ph(uvb, classTypeInfo);

   // initialize code block
   if (this)
      parseInitImpl(uvb->getUserSignature()->selfid);
   parseCheckReturn();
}

// never called with this=0
void TopLevelStatementBlock::parseInit() {
   QORE_TRACE("TopLevelStatementBlock::parseInit");

   assert(this);

   //printd(5, "TopLevelStatementBlock::parseInit(rns=%p) first=%d\n", &rns, first);

   if (!first && lvars) {
      // push already-registered local variables on the stack
      for (unsigned i = 0; i < lvars->size(); ++i)
	 push_top_level_local_var(lvars->lv[i]);
   }

   int lvids = parseInitIntern(0, PF_TOP_LEVEL, hwm);

   //printd(5, "TopLevelStatementBlock::parseInit(rns=%p) first=%d, lvids=%d\n", &rns, first, lvids);

   if (!first && lvids) {
      parseException("ILLEGAL-TOP-LEVEL-LOCAL-VARIABLE", "local variables declared with 'my' in the top-level block of a Program object can only be declared in the very first code block parsed");
      // discard variables immediately
      for (int i = 0; i < lvids; ++i)
	 pop_local_var();
   }

   // save local variable position for searches
   VNode *vn = getVStack();
   //printd(5, "TopLevelStatementBlock::parseInit() saving global vnode=%p\n", vn);
   save_global_vnode(vn);
   
   // now initialize root namespace and functions before local variables are popped off the stack
   qore_root_ns_private::parseInit();

   if (first) {
      // this call will pop all local vars off the stack
      setupLVList(lvids);
      first = false;
   }
   else if (lvars) {
      for (unsigned i = 0; i < lvars->size(); ++i)
	 pop_local_var();
   }

   // reset variable position to 0
   save_global_vnode(0);

   assert(!getVStack());

   //printd(5, "TopLevelStatementBlock::parseInitTopLevel(this=%p): done (lvars=%p, %d vars, vstack = %p)\n", this, lvars, lvids, getVStack());
   return;
}

int TopLevelStatementBlock::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   // do not instantiate local vars here; they are instantiated by the QoreProgram object for each thread
   return execIntern(return_value, xsink);
}

