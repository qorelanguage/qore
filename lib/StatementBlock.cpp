/*
  Statement.cpp

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
   const QoreProgramLocation* loc;
   bool block_start;
   bool top_level;

public:
   LocalVar* lvar;
   VNode* next;

   DLLLOCAL VNode(LocalVar* lv, const QoreProgramLocation* n_loc = 0, int n_refs = 0, bool n_top_level = false) : refs(n_refs), loc(n_loc), block_start(false), top_level(n_top_level), lvar(lv), next(getVStack()) {
      updateVStack(this);

      //printd(5, "VNode::VNode() this: %p '%s' %p top_level: %d\n", this, lvar ? lvar->getName() : "n/a", lvar, top_level);

      if (top_level)
         save_global_vnode(this);
   }

   DLLLOCAL ~VNode() {
      //printd(5, "VNode::~VNode() this: %p '%s' %p top_level: %d\n", this, lvar ? lvar->getName() : "n/a", lvar, top_level);

      if (lvar && !refs)
	 qore_program_private::makeParseWarning(getProgram(), *loc, QP_WARN_UNREFERENCED_VARIABLE, "UNREFERENCED-VARIABLE", "local variable '%s' was declared in this block but not referenced; to disable this warning, use '%%disable-warning unreferenced-variable' in your code", lvar->getName());

      if (top_level) {
	 save_global_vnode(0);
	 //printd(0, "VNode::~VNode() this: %p deleting top-level global vnode\n", this);
      }
   }

   DLLLOCAL void appendLocation(QoreString& str) {
      if (loc) {
	 str.concat(" at ");
	 loc->toString(str);
      }
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

   DLLLOCAL const char* getName() const {
      return lvar->getName();
   }

   // searches to marker and then jumps to global thread-local variables
   DLLLOCAL VNode* nextSearch() const {
      //printd(5, "VNode::nextSearch() next->lvar: %p top_level: %d\n", next ? next->lvar : 0, top_level);

      if ((next && next->lvar) || top_level)
	 return !next || next->lvar ? next : 0;

      // skip to global thread-local variables
      VNode* rv = get_global_vnode();
      assert(!rv || rv->lvar);
      //printd(5, "VNode::nextSearch() returning global VNode %p '%s'\n", rv, rv ? rv->getName() : "n/a");
      return rv;
   }
};

class BlockStartHelper {
protected:
   bool bs;

public:
   DLLLOCAL BlockStartHelper() {
      VNode* v = getVStack();
      //printd(5, "BlockStartHelper::BlockStartHelper() v=%p ibs=%d\n", v, v ? v->isBlockStart() : 0);
      bs = v ? v->setBlockStart(true) : true;
   }
   DLLLOCAL ~BlockStartHelper() {
      //printd(5, "BlockStartHelper::~BlockStartHelper() bs=%d\n", bs);
      if (!bs)
	 getVStack()->setBlockStart(false);
   }
};

VariableBlockHelper::VariableBlockHelper() {
   new VNode(0);
   //printd(5, "VariableBlockHelper::VariableBlockHelper() this=%p pushed %p\n", this, 0);
}

VariableBlockHelper::~VariableBlockHelper() {
   std::auto_ptr<VNode> vnode(getVStack());
   assert(vnode.get());
   updateVStack(vnode->next);
   //printd(5, "VariableBlockHelper::~VariableBlockHelper() this=%p got %p\n", this, vnode->lvar);
}

QoreValue StatementBlock::exec(ExceptionSink* xsink) {
   QoreValue return_value;
   execImpl(return_value, xsink);
   return return_value;
}

void StatementBlock::addStatement(AbstractStatement* s) {
   //QORE_TRACE("StatementBlock::addStatement()");

   if (s) {
      statement_list.push_back(s);
      OnBlockExitStatement* obe = dynamic_cast<OnBlockExitStatement* >(s);
      if (obe)
	 on_block_exit_list.push_front(std::make_pair(obe->getType(), obe->getCode()));

      loc.end_line = s->loc.end_line;
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

int StatementBlock::execImpl(QoreValue& return_value, ExceptionSink* xsink) {
   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);

   return execIntern(return_value, xsink);
}

int StatementBlock::execIntern(QoreValue& return_value, ExceptionSink* xsink) {
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

static void push_top_level_local_var(LocalVar* lv, const QoreProgramLocation& loc) {
   new VNode(lv, &loc, 1, true);
}

// used for constructor methods sharing a common "self" local variable
void push_local_var(LocalVar* lv, const QoreProgramLocation& loc) {
   new VNode(lv, &loc, 1);
}

LocalVar* push_local_var(const char* name, const QoreProgramLocation& loc, const QoreTypeInfo* typeInfo, bool is_arg, int n_refs, bool top_level) {
   QoreProgram* pgm = getProgram();

   LocalVar* lv = pgm->createLocalVar(name, typeInfo);

   QoreString ls;
   loc.toString(ls);
   //printd(5, "push_local_var() lv: %p name: %s type: %s %s\n", lv, name, typeInfo->getName(), ls.getBuffer());

   bool found_block = false;
   // check stack for duplicate entries
   bool avs = parse_check_parse_option(PO_ASSUME_LOCAL);
   if (is_arg) {
      lv->parseAssigned();
      if (pgm->checkWarning(QP_WARN_DUPLICATE_LOCAL_VARS | QP_WARN_DUPLICATE_BLOCK_VARS) || avs) {
         VNode* vnode = getVStack();
         while (vnode) {
            if (!found_block && vnode->isBlockStart())
               found_block = true;
            if (!strcmp(vnode->getName(), name)) {
	       if (!found_block) {
		  QoreStringNode* desc = new QoreStringNodeMaker("local variable '%s' was already declared in the same block", name);
		  if (avs) {
		     vnode->appendLocation(*desc);
		     parseException(loc, "PARSE-ERRPR", desc);
		  }
		  else {
		     vnode->appendLocation(*desc);
                     qore_program_private::makeParseWarning(getProgram(), loc, QP_WARN_DUPLICATE_BLOCK_VARS, "DUPLICATE-BLOCK-VARIABLE", desc);
		  }
	       }
	       else if (top_level || !vnode->isTopLevel()) {
		  QoreStringNode* desc = new QoreStringNodeMaker("local variable '%s' was already declared in this lexical scope", name);
		  vnode->appendLocation(*desc);
		  qore_program_private::makeParseWarning(getProgram(), loc, QP_WARN_DUPLICATE_LOCAL_VARS, "DUPLICATE-LOCAL-VARIABLE", desc);
	       }
	       break;
	    }
            vnode = vnode->nextSearch();
         }
      }
   }

   //printd(5, "push_local_var(): pushing var %s\n", name);
   new VNode(lv, &loc, n_refs, top_level);
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

LocalVar* pop_local_var(bool set_unassigned) {
   std::auto_ptr<VNode> vnode(getVStack());
   assert(vnode.get());
   LocalVar* rc = vnode->lvar;
   if (set_unassigned)
      rc->parseUnassigned();
   printd(5, "pop_local_var(): popping var %s\n", rc->getName());
   updateVStack(vnode->next);
   return rc;
}

LocalVar* find_local_var(const char* name, bool& in_closure) {
   VNode* vnode = getVStack();
   ClosureParseEnvironment* cenv = thread_get_closure_parse_env();
   in_closure = false;

   if (vnode && !vnode->lvar)
      vnode = vnode->nextSearch();

   //printd(5, "find_local_var('%s' %p) vnode: %p\n", name, name, vnode);

   while (vnode) {
      assert(vnode->lvar);
      if (cenv && !in_closure && cenv->getHighWaterMark() == vnode)
	 in_closure = true;

      //printd(5, "find_local_var('%s' %p) v: '%s' %p in_closure: %d match: %d\n", name, name, vnode->getName(), vnode->getName(), in_closure, !strcmp(vnode->getName(), name));

      if (!strcmp(vnode->getName(), name)) {
         //printd(5, "find_local_var() %s in_closure: %d\n", name, in_closure);
         if (in_closure)
	    cenv->add(vnode->lvar);
	 vnode->setRef();
	 return vnode->lvar;
      }
      vnode = vnode->nextSearch();
   }

   //printd(5, "find_local_var('%s' %p) returning 0 NOT FOUND\n", name, name);
   return 0;
}

int StatementBlock::parseInitIntern(LocalVar* oflag, int pflag, statement_list_t::iterator start) {
   QORE_TRACE("StatementBlock::parseInitIntern");

   int lvids = 0;

   AbstractStatement* ret = 0;

   if (start != statement_list.end())
      ++start;
   else
      start = statement_list.begin();

   for (statement_list_t::iterator i = start, l = statement_list.last(), e = statement_list.end(); i != e; ++i) {
      lvids += (*i)->parseInit(oflag, pflag);
      if (!ret && i != l && (*i)->endsBlock()) {
	 // unreachable code found
	 qore_program_private::makeParseWarning(getProgram(), QP_WARN_UNREACHABLE_CODE, "UNREACHABLE-CODE", "code after this statement can never be reached");
	 ret = *i;
      }
   }

   return lvids;
}

int StatementBlock::parseInitImpl(LocalVar* oflag, int pflag) {
   QORE_TRACE("StatementBlock::parseInitImpl");

   printd(4, "StatementBlock::parseInitImpl(b=%p, oflag=%p)\n", this, oflag);

   BlockStartHelper bsh;

   int lvids = parseInitIntern(oflag, pflag & ~PF_TOP_LEVEL, statement_list.end());

   // this call will pop all local vars off the stack
   setupLVList(lvids);

   //printd(5, "StatementBlock::parseInitImpl(this=%p): done (lvars=%p, %d vars, vstack = %p)\n", this, lvars, lvids, getVStack());

   return 0;
}

void StatementBlock::parseInit(UserVariantBase* uvb) {
   QORE_TRACE("StatementBlock::parseInit");

   VariableBlockHelper vbh;

   UserParamListLocalVarHelper ph(uvb);

   // initialize code block
   parseInitImpl(0);

   parseCheckReturn();
}

void StatementBlock::parseCheckReturn() {
   const QoreTypeInfo* returnTypeInfo = getReturnTypeInfo();
   if (returnTypeInfo->hasType() && !returnTypeInfo->parseAccepts(nothingTypeInfo)) {
      // make sure the last statement is a return statement if the block has a return type
      if (statement_list.empty() || !(*statement_list.last())->hasFinalReturn()) {
	 QoreStringNode* desc = new QoreStringNode("this code block has declared return type ");
	 returnTypeInfo->getThisType(*desc);
	 desc->concat(" but does not have a return statement as the last statement in the block");
	 qore_program_private::makeParseException(getProgram(), loc, "MISSING-RETURN", desc);
      }
   }
}

void StatementBlock::parseInitMethod(const QoreTypeInfo* typeInfo, UserVariantBase* uvb) {
   QORE_TRACE("StatementBlock::parseInitMethod");

   VariableBlockHelper vbh;

   UserParamListLocalVarHelper ph(uvb, typeInfo);

   // initialize code block
   parseInitImpl(uvb->getUserSignature()->selfid);

   parseCheckReturn();
}

void StatementBlock::parseInitConstructor(const QoreTypeInfo* typeInfo, UserVariantBase* uvb, BCAList* bcal, const QoreClass& cls) {
   QORE_TRACE("StatementBlock::parseInitConstructor");

   BCList* bcl = qore_class_private::getBaseClassList(cls);

   VariableBlockHelper vbh;

   UserParamListLocalVarHelper ph(uvb, typeInfo);

   // if there is a base constructor list, resolve all classes and
   // ensure that all classes referenced are base classes of this class
   if (bcal) {
      // ensure that parse flags are set before initializing
      ParseWarnHelper pwh(pwo);

      for (bcalist_t::iterator i = bcal->begin(), e = bcal->end(); i != e; ++i) {
	 assert(typeInfo->getUniqueReturnClass());
	 (*i)->parseInit(bcl, typeInfo->getUniqueReturnClass()->getName());
      }
   }

   // initialize code block
   parseInitImpl(qore_class_private::getSelfId(cls));
}

void StatementBlock::parseInitClosure(UserVariantBase* uvb, const QoreTypeInfo* classTypeInfo, lvar_set_t* vlist) {
   QORE_TRACE("StatementBlock::parseInitClosure");

   ClosureParseEnvironment cenv(vlist);
   UserParamListLocalVarHelper ph(uvb, classTypeInfo);

   // initialize code block
   parseInitImpl(uvb->getUserSignature()->selfid);
   parseCheckReturn();
}

void TopLevelStatementBlock::parseInit(int64 po) {
   QORE_TRACE("TopLevelStatementBlock::parseInit");

   //printd(5, "TopLevelStatementBlock::parseInit(rns=%p) first=%d\n", &rns, first);

   if (!first && lvars) {
      // push already-registered local variables on the stack
      for (unsigned i = 0; i < lvars->size(); ++i)
         push_top_level_local_var(lvars->lv[i], loc);
   }

   // resolve global variables before initializing the top-level statements
   qore_root_ns_private::parseResolveGlobalVars();
   int lvids = parseInitIntern(0, PF_TOP_LEVEL, hwm);

   //printd(5, "TopLevelStatementBlock::parseInit(rns=%p) first=%d, lvids=%d\n", &rns, first, lvids);

   if (!first && lvids) {
      parseException("ILLEGAL-TOP-LEVEL-LOCAL-VARIABLE", "local variables declared with 'my' in the top-level block of a Program object can only be declared in the very first code block parsed");
      // discard variables immediately
      for (int i = 0; i < lvids; ++i)
	 pop_local_var();
   }

   // now initialize root namespace and functions before local variables are popped off the stack
   qore_root_ns_private::parseInit();

   if (first) {
      // if parsing a module, then initialize the init function
      QoreModuleDefContext* qmd = get_module_def_context();
      if (qmd)
	 qmd->parseInit();

      // this call will pop all local vars off the stack
      setupLVList(lvids);
      first = false;
   }
   else if (lvars) {
      for (unsigned i = 0; i < lvars->size(); ++i)
	 pop_local_var();
   }

   assert(!getVStack());

   //printd(5, "TopLevelStatementBlock::parseInitTopLevel(this=%p): done (lvars=%p, %d vars, vstack = %p)\n", this, lvars, lvids, getVStack());
   return;
}

int TopLevelStatementBlock::execImpl(QoreValue& return_value, ExceptionSink* xsink) {
   // do not instantiate local vars here; they are instantiated by the QoreProgram object for each thread
   return execIntern(return_value, xsink);
}
