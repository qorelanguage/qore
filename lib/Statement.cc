/*
  Statement.cc

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

#include <qore/Qore.h>
#include <qore/Statement.h>
#include <qore/IfStatement.h>
#include <qore/WhileStatement.h>
#include <qore/ForStatement.h>
#include <qore/ForEachStatement.h>
#include <qore/DeleteStatement.h>
#include <qore/TryStatement.h>
#include <qore/ThrowStatement.h>
#include <qore/SwitchStatement.h>
#include <qore/Variable.h>
#include <qore/Function.h>
#include <qore/Context.h>
#include <qore/Operator.h>
#include <qore/ParserSupport.h>
#include <qore/QoreWarnings.h>
#include <qore/minitest.hpp>
#include <qore/ContextStatement.h>
#include <qore/Tree.h>
#include <qore/Find.h>
#include <qore/ScopedObjectCall.h>
#include <qore/ClassRef.h>
#include <qore/NamedScope.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef DEBUG
#  include "tests/Statement_tests.cc"
#endif

// parse context stack
class CVNode 
{
public:
   char *name;
   class CVNode *next;
   
   DLLLOCAL CVNode(char *n) { name = n; }
};

// parse variable stack
class VNode {
public:
   char *name;
   class VNode *next;
   
   DLLLOCAL VNode(char *nme) { name = nme; }
};

LVList::LVList(int num)
{
   if (num)
      ids = new lvh_t[num];
   else
      ids = NULL;
   num_lvars = num;
}

LVList::~LVList()
{
   if (ids)
      delete [] ids;
}

class QoreNode *StatementBlock::exec(ExceptionSink *xsink)
{
   class QoreNode *return_value = NULL;
   exec(&return_value, xsink);
   return return_value;
}

Statement::Statement(int sline, int eline, int type, class QoreNode *node)
{
   LineNumber = sline;
   EndLineNumber = eline;
   FileName = get_parse_file();
   next = NULL;
   Type       = type;
   s.node     = node;
}

Statement::Statement(int sline, int eline, int type)
{
   LineNumber = sline;
   EndLineNumber = eline;
   FileName = get_parse_file();
   next = NULL;
   Type       = type;
}

Statement::Statement(int sline, int eline, class StatementBlock *b)
{
   LineNumber = sline;
   EndLineNumber = eline;
   FileName = get_parse_file();
   next = NULL;
   Type        = S_SUB_BLOCK;
   s.sub_block = b;
}

#define STATEMENT_BLOCK 20

StatementBlock::StatementBlock(Statement *s)
{
   allocated = STATEMENT_BLOCK;
   head = tail = s;
   lvars = NULL;
}

void StatementBlock::addStatement(class Statement *s)
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

StatementBlock::~StatementBlock()
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

// only executed by Statement::exec()
inline void exec_rethrow(ExceptionSink *xsink)
{
   xsink->rethrow(catchGetException());
}

Statement::~Statement()
{
   switch (Type)
   {
      case S_RETURN:
	 if (s.node)
	    s.node->deref(NULL);
	 break;
      case S_EXPRESSION:
	 s.node->deref(NULL);
	 break;
      case S_SUBCONTEXT:
      case S_SUMMARY:
      case S_CONTEXT:
	 delete s.SContext;
	 break;
      case S_IF:
	 delete s.If;
	 break;
      case S_WHILE:
      case S_DO_WHILE:
	 delete s.While;
	 break;
      case S_FOR:
	 delete s.For;
	 break;
      case S_FOREACH:
	 delete s.ForEach;
	 break;
      case S_DELETE:
	 delete s.Delete;
	 break;
      case S_SUB_BLOCK:
	 if (s.sub_block)
	    delete s.sub_block;
	 break;
      case S_THROW:
	 delete s.Throw;
	 break;
      case S_TRY:
	 delete s.Try;
	 break;
      case S_SWITCH:
	 delete s.Switch;
	 break;
#ifdef DEBUG
      case S_RETHROW:
      case S_TEMP:
      case S_BREAK:
      case S_CONTINUE:
      case S_THREAD_EXIT:
	 break;
      default:
	 assert(false);
#endif
   }
}

int Statement::exec(class QoreNode **return_value, ExceptionSink *xsink)
{
   class QoreNode *rv;

   printd(1, "Statement::exec() file=%s line=%d type=%d\n", FileName, LineNumber, Type);
   update_pgm_counter_pgm_file(LineNumber, EndLineNumber, FileName);
   switch (Type)
   {
      case S_EXPRESSION:
	 if ((rv = s.node->eval(xsink)))
	    rv->deref(xsink);
	 break;
      case S_CONTEXT:
      case S_SUBCONTEXT:
	 return s.SContext->exec(return_value, xsink);
      case S_SUMMARY:
	 return s.SContext->execSummary(return_value, xsink);
      case S_IF:
	 return s.If->exec(return_value, xsink);
      case S_WHILE:
	 return s.While->execWhile(return_value, xsink);
      case S_DO_WHILE:
	 return s.While->execDoWhile(return_value, xsink);
      case S_FOR:
	 return s.For->exec(return_value, xsink);
      case S_FOREACH:
	 return s.ForEach->exec(return_value, xsink);
      case S_DELETE:
	 s.Delete->exec(xsink);
	 return 0;
      case S_SUB_BLOCK:
	 return s.sub_block->exec(return_value, xsink);
      case S_RETURN:
	 if (s.node)
	    (*return_value) = s.node->eval(xsink);
	 return RC_RETURN;
      case S_BREAK:
	 return RC_BREAK;
      case S_CONTINUE:
	 return RC_CONTINUE;
      case S_TRY:
	 return s.Try->exec(return_value, xsink);
      case S_RETHROW:
	 exec_rethrow(xsink);
	 return 0;
      case S_THROW:
	 s.Throw->exec(xsink);
	 return 0;
      case S_THREAD_EXIT:
	 xsink->raiseThreadExit();
	 return 0;
      case S_SWITCH:
	 return s.Switch->exec(return_value, xsink);
   }
   return 0;
}

int StatementBlock::exec(class QoreNode **return_value, class ExceptionSink *xsink)
{
   tracein("StatementBlock::exec()");
   int rc = 0;
   // instantiate local variables
   for (int i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   assert(xsink);

   // execute block
   class Statement *where = head;
   while (where && !xsink->isEvent())
   {
      if ((rc = where->exec(return_value, xsink)))
	 break;
      where = where->next;
   }

   // delete all variables local to this block
   for (int i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("StatementBlock::exec()");
   return rc;
}

// top-level block (program) execution member function
void StatementBlock::exec()
{
   ExceptionSink xsink;
   exec(&xsink);
}

void push_cvar(char *name)
{
   class CVNode *cvn = new CVNode(name);
   cvn->next = getCVarStack();
   updateCVarStack(cvn);
}

void pop_cvar()
{
   class CVNode *cvn = getCVarStack();
   updateCVarStack(cvn->next);
   delete cvn;
}

lvh_t push_local_var(char *name)
{
   class VNode *vnode;

   // check stack for duplicate entries
   if (getProgram()->checkWarning(QP_WARN_DUPLICATE_LOCAL_VARS))
   {
      vnode = getVStack();
      while (vnode)
      {
	 if (!strcmp(vnode->name, name))
	 {
	    getProgram()->makeParseWarning(QP_WARN_DUPLICATE_LOCAL_VARS, "DUPLICATE-LOCAL-VARIABLE", "local variable '%s' was already declared in this lexical scope", name);
	    break;
	 }
	 vnode = vnode->next;
      }
   }
   
   //printd(5, "push_local_var(): pushing var %s\n", name);
   vnode = new class VNode(name);
   vnode->next = getVStack();
   updateVStack(vnode);
   return vnode->name;
}

lvh_t pop_local_var()
{
   class VNode *vnode = getVStack();
   lvh_t rc = vnode->name;

   assert(vnode);
   printd(5, "pop_local_var(): popping var %s\n", vnode->name);
   updateVStack(vnode->next);
   delete vnode;
   return rc;
}

lvh_t find_local_var(char *name)
{
   class VNode *vnode = getVStack();

   while (vnode)
   {
      if (!strcmp(vnode->name, name))
	 return vnode->name;
      vnode = vnode->next;
   }
   return 0;
}

// checks for illegal $self assignments in an object context
static inline void checkSelf(class QoreNode *n, lvh_t selfid)
{
   // if it's a variable reference
   if (n->type == NT_VARREF)
   {
      if (n->val.vref->type == VT_LOCAL && n->val.vref->ref.id == selfid)
	 parse_error("illegal assignment to $self in an object context");
      return;
   }
   
   if (n->type != NT_TREE)
      return;

   // otherwise it's a tree: go to root expression 
   while (n->val.tree->left->type == NT_TREE)
      n = n->val.tree->left;

   if (n->val.tree->left->type != NT_VARREF)
      return;

   // left must be variable reference, check if the tree is
   // a list reference; if so, it's invalid
   if (n->val.tree->left->val.vref->type == VT_LOCAL && n->val.tree->left->val.vref->ref.id == selfid
       && n->val.tree->op == OP_LIST_REF)
      parse_error("illegal conversion of $self to a list");
}

static inline void checkLocalVariableChange(class QoreNode *n)
{
   if (n->type == NT_VARREF && n->val.vref->type == VT_LOCAL)
      parse_error("illegal local variable modification in background expression");
}

static inline int getBaseLVType(class QoreNode *n)
{
   while (true)
   {
      if (n->type == NT_SELF_VARREF)
	 return VT_OBJECT;
      if (n->type == NT_VARREF)
	 return n->val.vref->type;
      n = n->val.tree->left;
   }
}

// this function will put variables on the local stack but will not pop them
int process_node(class QoreNode **node, lvh_t oflag, int pflag)
{
   int lvids = 0, i;
   int current_pflag = pflag;
   pflag &= ~PF_REFERENCE_OK;  // unset "reference ok" for next call

   printd(4, "process_node() %08p type=%s cp=%d, p=%d\n", *node, *node ? (*node)->type->getName() : "(null)", current_pflag, pflag);
   if (!(*node))
      return 0;

   if ((*node)->type == NT_REFERENCE)
   {
       // otherwise throw a parse exception if an illegal reference is used
       if (!(current_pflag & PF_REFERENCE_OK))
	  parse_error("the reference operator can only be used in function and method call argument lists and in foreach statements");
       else
       {
	  lvids = process_node(&((*node)->val.lvexp), oflag, pflag);
	  // if a background expression is being parsed, then check that no references to local variables
	  // or object members are being used
	  if (pflag & PF_BACKGROUND)
	  {
	     int vtype = getBaseLVType((*node)->val.lvexp);

	     if (vtype == VT_LOCAL)
		parse_error("the reference operator cannot be used with local variables in a background expression");
	     //else if (vtype == VT_OBJECT)
	     //parse_error("the reference operator cannot be used with object members in a background expression");
	  }
       }
       return lvids;
   }
   
   if ((*node)->type == NT_VARREF)
   {
      // if it is a new variable being declared
      if ((*node)->val.vref->type == VT_LOCAL)
      {
	 (*node)->val.vref->ref.id = push_local_var((*node)->val.vref->name);
	 lvids++;
	 //printd(5, "process_node(): local var %s declared (id=%08p)\n", (*node)->val.vref->name, (*node)->val.vref->ref.id);
      }
      else if ((*node)->val.vref->type == VT_GLOBAL)
	 (*node)->val.vref->ref.var = getProgram()->createVar((*node)->val.vref->name);
      else // otherwise reference must be resolved
	 (*node)->val.vref->resolve();

      return lvids;
   }

   if ((*node)->type == NT_BAREWORD)
   {
      // resolve simple constant
      printd(5, "process_node() resolving simple constant \"%s\"\n", (*node)->val.c_str);
      getRootNS()->resolveSimpleConstant(node, 1);
      return lvids;
   }

   if ((*node)->type == NT_CONSTANT)
   {
      printd(5, "process_node() resolving scoped constant \"%s\"\n", (*node)->val.scoped_ref->ostr);
      getRootNS()->resolveScopedConstant(node, 1);
      return lvids;
   }

   if ((*node)->type == NT_COMPLEXCONTEXTREF)
   {
      if (!getCVarStack())
      {
	 parse_error("complex context reference \"%s.%s\" encountered out of context", 
		     (*node)->val.complex_cref->name, (*node)->val.complex_cref->member);
	 return lvids;
      }
      
      int stack_offset = 0;
      int found = 0;
      class CVNode *cvn = getCVarStack();
      while (cvn)
      {
	 if (cvn->name && !strcmp((*node)->val.complex_cref->name, cvn->name))
	 {
	    found = 1;
	    break;
	 }
	 cvn = cvn->next;
	 stack_offset++;
      }
      if (!found)
	 parse_error("\"%s\" does not match any current context", (*node)->val.complex_cref->name);
      else
	 (*node)->val.complex_cref->stack_offset = stack_offset;

      return lvids;
   }

   if ((*node)->type == NT_CONTEXTREF)
   {
      if (!getCVarStack())
	 parse_error("context reference \"%s\" out of context", (*node)->val.c_str);
      return lvids;
   }

   if ((*node)->type == NT_CONTEXT_ROW)
   {
      if (!getCVarStack())
	 parse_error("context row reference \"%%\" encountered out of context");
      return lvids;
   }

   if ((*node)->type == NT_TREE)
   {
      // set "parsing background" flag if the background operator is being parsed
      if ((*node)->val.tree->op == OP_BACKGROUND)
	 pflag |= PF_BACKGROUND;

      // process left branch of tree
      lvids += process_node(&((*node)->val.tree->left), oflag, pflag);
      // process right branch if it exists
      if ((*node)->val.tree->right)
	 lvids += process_node(&((*node)->val.tree->right), oflag, pflag);
      
      // check for illegal changes to local variables in background expressions
      if (pflag & PF_BACKGROUND && (*node)->val.tree->op->needsLValue())
	 checkLocalVariableChange((*node)->val.tree->left);	 

      // throw a parse exception if an assignment is attempted on $self
      if ((*node)->val.tree->op == OP_ASSIGNMENT && oflag)
	    checkSelf((*node)->val.tree->left, oflag);
      return lvids;
   }

   if ((*node)->type == NT_FUNCTION_CALL)
   {
      if ((*node)->val.fcall->type == FC_SELF)
      {
	 if (!oflag)
	    parse_error("cannot call member function \"%s\" out of an object member function definition", 
			(*node)->val.fcall->f.sfunc->name);
	 else
	    (*node)->val.fcall->f.sfunc->resolve();
      }
      else if ((*node)->val.fcall->type == FC_UNRESOLVED)
	 getProgram()->resolveFunction((*node)->val.fcall);
      
      if ((*node)->val.fcall->args)
	 for (i = 0; i < (*node)->val.fcall->args->val.list->size(); i++)
	 {
	    class QoreNode **n = (*node)->val.fcall->args->val.list->get_entry_ptr(i);
	    if ((*n)->type == NT_REFERENCE)
	    {
	       if (!(*node)->val.fcall->existsUserParam(i))
		  parse_error("not enough parameters in \"%s\" to accept reference expression", (*node)->val.fcall->getName());
	       lvids += process_node(n, oflag, pflag | PF_REFERENCE_OK);
	    }
	    else
	       lvids += process_node(n, oflag, pflag);
	 }

      return lvids;
   }

   // for the "new" operator
   if ((*node)->type == NT_SCOPE_REF)
   {
      // find object class
      if (((*node)->val.socall->oc = getRootNS()->parseFindScopedClass((*node)->val.socall->name)))
      {
	 // check if parse options allow access to this class
	 if ((*node)->val.socall->oc->getDomain() & getProgram()->getParseOptions())
	    parseException("ILLEGAL-CLASS-INSTANTIATION", "parse options do not allow access to the '%s' class", (*node)->val.socall->oc->getName());
      }
      delete (*node)->val.socall->name;
      (*node)->val.socall->name = NULL;
      if ((*node)->val.socall->args)
	 lvids += process_node(&((*node)->val.socall->args), oflag, pflag);
      
      return lvids;
   }

   if ((*node)->type == NT_FLIST)
      (*node)->type = NT_LIST;

   if ((*node)->type == NT_LIST || (*node)->type == NT_VLIST)
   {
      for (i = 0; i < (*node)->val.list->size(); i++)
	 lvids += process_node((*node)->val.list->get_entry_ptr(i), oflag, pflag);

      return lvids;
   }

   if ((*node)->type == NT_HASH)
   {
      List *keys = (*node)->val.hash->getKeys();
      for (i = 0; i < keys->size(); i++)
      {
	 const char *k = keys->retrieve_entry(i)->val.String->getBuffer();
	 class QoreNode **value = (*node)->val.hash->getKeyValuePtr(k);
	 // resolve constant references in keys
	 if (k[0] == HE_TAG_CONST || k[0] == HE_TAG_SCOPED_CONST)
	 {
	    QoreNode *rv;
	    if (k[0] == HE_TAG_CONST)
	       rv = getRootNS()->findConstantValue(k + 1, 1);
	    else
	    {
	       class NamedScope *nscope = new NamedScope(strdup(k + 1));
	       rv = getRootNS()->findConstantValue(nscope, 1);
	       delete nscope;
	    }
	    if (rv)
	    {
	       QoreNode *t;
	       if (rv->type != NT_STRING)
		  t = rv->convert(NT_STRING);
	       else
		  t = rv;
	       
	       // reference value for new hash key
	       (*value)->ref();
	       // not possible to have an exception here
	       (*node)->val.hash->setKeyValue(t->val.String->getBuffer(), *value, NULL);

	       // now reget new value ptr
	       value = (*node)->val.hash->getKeyValuePtr(t->val.String->getBuffer());

	       // or here
	       if (rv != t)
		  t->deref(NULL);
	    }
	    else
	       value = NULL;
	    // delete the old key (not possible to have an exception here)
	    (*node)->val.hash->deleteKey(k, NULL);
	 }
	 if (value)
	    lvids += process_node(value, oflag, pflag);
      }
      keys->derefAndDelete(NULL);
      return lvids;
   }

   if ((*node)->type == NT_FIND)
   {
      push_cvar(NULL);
      lvids += process_node(&((*node)->val.find->find_exp), oflag, pflag);
      lvids += process_node(&((*node)->val.find->where),    oflag, pflag);
      lvids += process_node(&((*node)->val.find->exp),      oflag, pflag);
      pop_cvar();
      return lvids;
   }

   if ((*node)->type == NT_SELF_VARREF)
   {
      //printd(0, "process_node() SELF_REF '%s'  oflag=%d\n", (*node)->val.c_str, oflag);
      if (!oflag)
	 parse_error("cannot reference member \"%s\" out of an object member function definition", 
		     (*node)->val.c_str);
      
      return lvids;
   }
   
   if ((*node)->type == NT_CLASSREF)
      (*node)->val.classref->resolve();

   return lvids;
}

// processes a single statement; does not pop variables off stack
int Statement::parseInit(lvh_t oflag, int pflag)
{
   int lvids = 0;

   tracein("Statement::parseInit()");
   printd(2, "Statement::parseInit() %08p type=%d line %d file %s\n", this, Type, LineNumber, FileName);
   // set pgm position in case of errors
   update_parse_location(LineNumber, EndLineNumber, FileName);
   switch (Type)
   {
      case S_SUBCONTEXT:
      case S_SUMMARY:
      case S_CONTEXT:
	 s.SContext->parseInit(oflag, pflag);
	 break;
      case S_EXPRESSION:
	 lvids += process_node(&(s.node), oflag, pflag);
	 break;
      case S_IF:
	 s.If->parseInit(oflag, pflag);
	 break;
      case S_WHILE:
	 s.While->parseWhileInit(oflag, pflag);
	 break;
      case S_DO_WHILE:
	 s.While->parseDoWhileInit(oflag, pflag);
	 break;
      case S_FOR:
	 s.For->parseInit(oflag, pflag);
	 break;
      case S_FOREACH:
	 s.ForEach->parseInit(oflag, pflag);
	 break;
      case S_TRY:
	 s.Try->parseInit(oflag, pflag);
	 break;
      case S_DELETE:
	 lvids += s.Delete->parseInit(oflag, pflag);
	 break;
      case S_SUB_BLOCK:
	 s.sub_block->parseInit(oflag, pflag);
	 break;
      case S_RETURN:
	 if (s.node)
	    lvids += process_node(&(s.node), oflag, pflag);
	 break;
      case S_THROW:
	 lvids += s.Throw->parseInit(oflag, pflag);
	 break;
      case S_RETHROW:
	 if (!(pflag & PF_RETHROW_OK))
	    parse_error("rethrow statements only legal in catch block");
	 break;
      case S_SWITCH:
	 s.Switch->parseInit(oflag, pflag);
	 break;
   }
   traceout("Statement::parseInit()");
   return lvids;
}

void StatementBlock::parseInit(lvh_t oflag, int pflag)
{
   int lvids = 0;

   tracein("StatementBlock::parseInit()");
   printd(4, "StatementBlock::parseInit(b=%08p, oflag=%d) head=%08p tail=%08p\n", this, oflag, head, tail);

   class Statement *where = head, *ret = NULL;
   while (where)
   {
      lvids += where->parseInit(oflag, pflag);
      if (!ret && where->next && where->Type == S_RETURN)
      {
	 // return statement not found at end of block
	 getProgram()->makeParseWarning(QP_WARN_UNREACHABLE_CODE, "UNREACHABLE-CODE", "code after this statement can never be reached");
	 ret = where;
      }
      where = where->next;
   }

   lvars = new LVList(lvids);
   for (int i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();

   printd(4, "StatementBlock::parseInit(): done (lvars = %d, vstack = %08p)\n", lvids, getVStack());
   traceout("StatementBlock::parseInit()");
}

// can also be called with this=NULL
void StatementBlock::parseInit(class Paramlist *params)
{
   tracein("StatementBlock::parseInit()");
   if (params->num_params)
      params->ids = new lvh_t[params->num_params];
   else
      params->ids = NULL;

   // push $argv var on stack and save id
   params->argvid = push_local_var("argv");
   printd(5, "StatementBlock::parseInit() params=%08p argvid=%08p\n", params, params->argvid);

   // init param ids and push local param vars on stack
   for (int i = 0; i < params->num_params; i++)
   {
      params->ids[i] = push_local_var(params->names[i]);
      printd(3, "StatementBlock::parseInit() reg. local var %s (id=%08p)\n", 
	     params->names[i], params->ids[i]);
   }

   // initialize code block
   if (this)
      parseInit((lvh_t)NULL);

   // pop local param vars from stack
   for (int i = 0; i < params->num_params; i++)
      pop_local_var();

   // pop argv param off stack
   pop_local_var();

   traceout("StatementBlock::parseInit()");
}

// can also be called with this=NULL
void StatementBlock::parseInit(class Paramlist *params, class BCList *bcl)
{
   tracein("StatementBlock::parseInit()");
   if (params->num_params)
      params->ids = new lvh_t[params->num_params];
   else
      params->ids = NULL;

   // this is a class constructor method, push local $self variable
   params->selfid = push_local_var("self");
   // set oflag to selfid
   lvh_t oflag = params->selfid;

   // push $argv var on stack and save id
   params->argvid = push_local_var("argv");
   printd(5, "StatementBlock::parseInit() params=%08p argvid=%08p\n", params, params->argvid);

   // init param ids and push local param vars on stack
   for (int i = 0; i < params->num_params; i++)
   {
      params->ids[i] = push_local_var(params->names[i]);
      printd(3, "StatementBlock::parseInit() reg. local var %s (id=%08p)\n", 
	     params->names[i], params->ids[i]);
   }

   // initialize base class constructor arguments
   if (bcl)
   {
      int tlvids = 0;
      for (bclist_t::iterator i = bcl->begin(); i != bcl->end(); i++)
      {
	 if ((*i)->args)
	 {
	    class List *l = (*i)->args->val.list;
	    for (int j = 0; j < l->size(); j++)
	    {
	       class QoreNode **n = l->get_entry_ptr(j);
	       tlvids += process_node(n, oflag, PF_REFERENCE_OK);
	    }
	 }
      }
      if (tlvids)
      {
	 parse_error("illegal local variable declaration in base class constructor argument");
	 for (int i = 0; i < tlvids; i++)
	    pop_local_var();
      }
   }

   // initialize code block
   if (this)
      parseInit(oflag);

   // pop local param vars from stack
   for (int i = 0; i < params->num_params; i++)
      pop_local_var();

   // pop argv param off stack
   pop_local_var();

   // pop $self id off stack
   pop_local_var();

   traceout("StatementBlock::parseInit()");
}
