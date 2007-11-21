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
#include <qore/StatementBlock.h>
#include <qore/Variable.h>
#include <qore/Function.h>
#include <qore/Context.h>
#include <qore/Operator.h>
#include <qore/ParserSupport.h>
#include <qore/QoreWarnings.h>
#include <qore/minitest.hpp>
#include <qore/Tree.h>
#include <qore/Find.h>
#include <qore/ScopedObjectCall.h>
#include <qore/ClassRef.h>
#include <qore/NamedScope.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef DEBUG_TESTS
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
   execImpl(&return_value, xsink);
   return return_value;
}

// line numbers on statement blocks are set later
StatementBlock::StatementBlock(AbstractStatement *s) : AbstractStatement(-1, -1), lvars(0)
{
   addStatement(s);
}

void StatementBlock::addStatement(class AbstractStatement *s)
{
   //tracein("StatementBlock::addStatement()");
   if (s)
   {
      statement_list.push_back(s);
      OnBlockExitStatement *obe = dynamic_cast<OnBlockExitStatement *>(s);
      if (obe)
	 on_block_exit_list.push_front(std::make_pair(obe->getType(), obe->getCode()));
   }
   
   //traceout("StatementBlock::addStatement()");
}

StatementBlock::~StatementBlock()
{
   //tracein("StatementBlock::~StatementBlock()");

   for (statement_list_t::iterator i = statement_list.begin(), e = statement_list.end(); i != e; ++i)
      delete *i;
   
   if (lvars)
      delete lvars;
   //traceout("StatementBlock::~StatementBlock()");
}

int StatementBlock::execImpl(class QoreNode **return_value, class ExceptionSink *xsink)
{
   tracein("StatementBlock::execImpl()");
   int rc = 0;
   // instantiate local variables
   for (int i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   assert(xsink);

   // push on block exit iterator if necessary
   if (on_block_exit_list.size())
      pushBlock(on_block_exit_list.end());
   
   // execute block
   for (statement_list_t::iterator i = statement_list.begin(), e = statement_list.end(); i != e; ++i)
      if ((rc = (*i)->exec(return_value, xsink)) || xsink->isEvent())
	 break;

   // execute on block exit code if applicable
   if (on_block_exit_list.size())
   {
      ExceptionSink obe_xsink;
      int nrc = 0;
      bool error = *xsink;
      for (block_list_t::iterator i = popBlock(), e = on_block_exit_list.end(); i != e; ++i)
      {
	 enum obe_type_e type = (*i).first;
	 if (type == OBE_Unconditional || (!error && type == OBE_Success) || (error && type == OBE_Error))
	    nrc = (*i).second->execImpl(return_value, &obe_xsink);
      }
      if (obe_xsink)
	 xsink->assimilate(&obe_xsink);
      if (nrc)
	 rc = nrc;
   }

   // delete all variables local to this block
   for (int i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("StatementBlock::execImpl()");
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

   //printd(5, "process_node() %08p type=%s cp=%d, p=%d\n", *node, *node ? (*node)->type->getName() : "(null)", current_pflag, pflag);
   if (!(*node))
      return 0;

   if ((*node)->type == NT_REFERENCE)
   {
      // otherwise throw a parse exception if an illegal reference is used
      if (!(current_pflag & PF_REFERENCE_OK))
      {	 
	 parse_error("the reference operator can only be used in function and method call argument lists and in foreach statements");
      }
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
   else if ((*node)->type == NT_OBJMETHREF)
      (*node)->val.objmethref->parseInit(oflag, pflag);
   else if ((*node)->type == NT_FUNCREF)
      (*node)->val.funcref->resolve();
   else if ((*node)->type == NT_FUNCREFCALL)
      (*node)->val.funcrefcall->parseInit(oflag, pflag);
   
   return lvids;
}

int StatementBlock::parseInitImpl(lvh_t oflag, int pflag)
{
   int lvids = 0;

   tracein("StatementBlock::parseInit()");
   printd(4, "StatementBlock::parseInit(b=%08p, oflag=%d)\n", this, oflag);

   class AbstractStatement *ret = NULL;
   for (statement_list_t::iterator i = statement_list.begin(), e = statement_list.end(), l = statement_list.last(); i != e; ++i)
   {
      lvids += (*i)->parseInit(oflag, pflag);
      if (!ret && i != l && (*i)->endsBlock())
      {
	 // unreachable code found
	 getProgram()->makeParseWarning(QP_WARN_UNREACHABLE_CODE, "UNREACHABLE-CODE", "code after this statement can never be reached");
	 ret = *i;
      }
   }

   lvars = new LVList(lvids);
   for (int i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();

   printd(4, "StatementBlock::parseInit(): done (lvars = %d, vstack = %08p)\n", lvids, getVStack());
   traceout("StatementBlock::parseInit()");
   return 0;
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
      parseInitImpl((lvh_t)NULL);

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
      parseInitImpl(oflag);

   // pop local param vars from stack
   for (int i = 0; i < params->num_params; i++)
      pop_local_var();

   // pop argv param off stack
   pop_local_var();

   // pop $self id off stack
   pop_local_var();

   traceout("StatementBlock::parseInit()");
}
