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
#include <qore/intern/StatementBlock.h>
#include <qore/intern/OnBlockExitStatement.h>
#include <qore/intern/ParserSupport.h>
#include <qore/intern/QoreClassIntern.h>
#include <qore/minitest.hpp>

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
   if (num) {
      ids = new lvh_t[num];
      // pop variables off stack and save in reverse order
      for (int i = num - 1; i >= 0; --i)
	 ids[i] = pop_local_var();
   }
   else
      ids = 0;
   num_lvars = num;
}

LVList::~LVList()
{
   if (ids)
      delete [] ids;
}

AbstractQoreNode *StatementBlock::exec(ExceptionSink *xsink)
{
   AbstractQoreNode *return_value = NULL;
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

int StatementBlock::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink)
{
   tracein("StatementBlock::execImpl()");
   int rc = 0;
   // instantiate local variables
   for (int i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   assert(xsink);

   bool obe = on_block_exit_list.size();
   // push on block exit iterator if necessary
   if (obe)
      pushBlock(on_block_exit_list.end());
   
   // execute block
   for (statement_list_t::iterator i = statement_list.begin(), e = statement_list.end(); i != e; ++i)
      if ((rc = (*i)->exec(return_value, xsink)) || xsink->isEvent())
	 break;

   // execute on block exit code if applicable
   if (obe)
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
static inline void checkSelf(AbstractQoreNode *n, lvh_t selfid)
{
   // if it's a variable reference
   const QoreType *ntype = n->getType();
   if (ntype == NT_VARREF)
   {
      VarRefNode *v = reinterpret_cast<VarRefNode *>(n);
      if (v->type == VT_LOCAL && v->ref.id == selfid)
	 parse_error("illegal assignment to $self in an object context");
      return;
   }
   
   if (ntype != NT_TREE)
      return;

   QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(n);

   // otherwise it's a tree: go to root expression 
   while (tree->left->type == NT_TREE) {
      n = tree->left;
      tree = reinterpret_cast<QoreTreeNode *>(n);
   }

   VarRefNode *v = dynamic_cast<VarRefNode *>(tree->left);
   if (!v)
      return;

   // left must be variable reference, check if the tree is
   // a list reference; if so, it's invalid
   if (v->type == VT_LOCAL && v->ref.id == selfid  && tree->op == OP_LIST_REF)
      parse_error("illegal conversion of $self to a list");
}

static inline void checkLocalVariableChange(AbstractQoreNode *n)
{
   VarRefNode *v = dynamic_cast<VarRefNode *>(n);
   if (v && v->type == VT_LOCAL)
      parse_error("illegal local variable modification in background expression");
}

static inline int getBaseLVType(AbstractQoreNode *n)
{
   while (true)
   {
      const QoreType *ntype = n->getType();
      if (ntype == NT_SELF_VARREF)
	 return VT_OBJECT;
      if (ntype == NT_VARREF)
	 return reinterpret_cast<VarRefNode *>(n)->type;
      // must be a tree
      n = reinterpret_cast<QoreTreeNode *>(n)->left;
   }
}

int process_list_node_intern(QoreListNode *l, lvh_t oflag, int pflag)
{
   int lvids = 0;

   for (int i = 0; i < l->size(); i++)
      lvids += process_node(l->get_entry_ptr(i), oflag, pflag);
   
   return lvids;
}

// this function will put variables on the local stack but will not pop them
int process_list_node(QoreListNode **node, lvh_t oflag, int pflag)
{
   if (!(*node))
      return 0;

   // unset "reference ok" for next call
   return process_list_node_intern(*node, oflag, pflag & ~PF_REFERENCE_OK);
}

// this function will put variables on the local stack but will not pop them
int process_node(AbstractQoreNode **node, lvh_t oflag, int pflag)
{
   int lvids = 0, i;
   int current_pflag = pflag;
   pflag &= ~PF_REFERENCE_OK;  // unset "reference ok" for next call

   //printd(5, "process_node() %08p type=%s cp=%d, p=%d\n", *node, *node ? (*node)->getTypeName() : "(null)", current_pflag, pflag);
   if (!(*node))
      return 0;

   const QoreType *ntype = (*node)->getType();
   if (ntype == NT_REFERENCE)
   {
      // otherwise throw a parse exception if an illegal reference is used
      if (!(current_pflag & PF_REFERENCE_OK))
      {	 
	 parse_error("the reference operator can only be used in function and method call argument lists and in foreach statements");
      }
      else
      {
	 ReferenceNode *r = reinterpret_cast<ReferenceNode *>(*node);
	 lvids = process_node(&(r->lvexp), oflag, pflag);
	 // if a background expression is being parsed, then check that no references to local variables
	 // or object members are being used
	 if (pflag & PF_BACKGROUND)
	 {
	    int vtype = getBaseLVType(r->lvexp);

	    if (vtype == VT_LOCAL)
	       parse_error("the reference operator cannot be used with local variables in a background expression");
	     //else if (vtype == VT_OBJECT)
	     //parse_error("the reference operator cannot be used with object members in a background expression");
	 }
      }
      return lvids;
   }
   
   if (ntype == NT_VARREF)
   {
      VarRefNode *v = reinterpret_cast<VarRefNode *>(*node);
      // if it is a new variable being declared
      if (v->type == VT_LOCAL)
      {
	 v->ref.id = push_local_var(v->name);
	 lvids++;
	 //printd(5, "process_node(): local var %s declared (id=%08p)\n", v->name, v->ref.id);
      }
      else if (v->type == VT_GLOBAL)
	 v->ref.var = getProgram()->createVar(v->name);
      else // otherwise reference must be resolved
	 v->resolve();

      return lvids;
   }

   if (ntype == NT_BAREWORD)
   {
      // resolve simple constant
      printd(5, "process_node() resolving simple constant \"%s\"\n", reinterpret_cast<BarewordNode *>(*node)->str);
      getRootNS()->resolveSimpleConstant(node, 1);
      return lvids;
   }

   if (ntype == NT_CONSTANT)
   {
      printd(5, "process_node() resolving scoped constant \"%s\"\n", reinterpret_cast<ConstantNode *>(*node)->scoped_ref->ostr);
      getRootNS()->resolveScopedConstant(node, 1);
      return lvids;
   }

   if (ntype == NT_COMPLEXCONTEXTREF)
   {
      ComplexContextrefNode *c = reinterpret_cast<ComplexContextrefNode *>(*node);
      if (!getCVarStack())
      {
	 parse_error("complex context reference \"%s:%s\" encountered out of context", c->name, c->member);
	 return lvids;
      }
      
      int stack_offset = 0;
      int found = 0;
      class CVNode *cvn = getCVarStack();
      while (cvn)
      {
	 if (cvn->name && !strcmp(c->name, cvn->name))
	 {
	    found = 1;
	    break;
	 }
	 cvn = cvn->next;
	 stack_offset++;
      }
      if (!found)
	 parse_error("\"%s\" does not match any current context", c->name);
      else
	 c->stack_offset = stack_offset;

      return lvids;
   }

   if (ntype == NT_CONTEXTREF)
   {
      if (!getCVarStack())
	 parse_error("context reference \"%s\" out of context", reinterpret_cast<ContextrefNode *>((*node))->str);
      return lvids;
   }

   if (ntype == NT_CONTEXT_ROW)
   {
      if (!getCVarStack())
	 parse_error("context row reference \"%%\" encountered out of context");
      return lvids;
   }

   if (ntype == NT_TREE)
   {
      QoreTreeNode *tree =reinterpret_cast<QoreTreeNode *>(*node);

      // set "parsing background" flag if the background operator is being parsed
      if (tree->op == OP_BACKGROUND)
	 pflag |= PF_BACKGROUND;

      // process left branch of tree
      lvids += process_node(&(tree->left), oflag, pflag);
      // process right branch if it exists
      if (tree->right)
	 lvids += process_node(&(tree->right), oflag, pflag);
      
      // check for illegal changes to local variables in background expressions
      if (pflag & PF_BACKGROUND && tree->op->needsLValue())
	 checkLocalVariableChange(tree->left);	 

      // throw a parse exception if an assignment is attempted on $self
      if (tree->op == OP_ASSIGNMENT && oflag)
	 checkSelf(tree->left, oflag);

      return lvids;
   }

   if (ntype == NT_FUNCTION_CALL)
   {
      FunctionCallNode *f = reinterpret_cast<FunctionCallNode *>(*node);

      if (f->getFunctionType() == FC_SELF)
      {
	 if (!oflag)
	    parse_error("cannot call member function \"%s\" out of an object member function definition", 
			f->f.sfunc->name);
	 else
	    f->f.sfunc->resolve();
      }
      else if (f->getFunctionType() == FC_UNRESOLVED)
	 getProgram()->resolveFunction(f);
      
      if (f->args)
	 for (i = 0; i < f->args->size(); i++)
	 {
	    AbstractQoreNode **n = f->args->get_entry_ptr(i);
	    if ((*n)->getType() == NT_REFERENCE)
	    {
	       if (!f->existsUserParam(i))
		  parse_error("not enough parameters in \"%s\" to accept reference expression", f->getName());
	       lvids += process_node(n, oflag, pflag | PF_REFERENCE_OK);
	    }
	    else
	       lvids += process_node(n, oflag, pflag);
	 }

      return lvids;
   }

   // for the "new" operator
   if (ntype == NT_SCOPE_REF)
   {
      ScopedObjectCallNode *c = reinterpret_cast<ScopedObjectCallNode *>(*node);
      // find object class
      if ((c->oc = getRootNS()->parseFindScopedClass(c->name)))
      {
	 // check if parse options allow access to this class
	 if (c->oc->getDomain() & getProgram()->getParseOptions())
	    parseException("ILLEGAL-CLASS-INSTANTIATION", "parse options do not allow access to the '%s' class", c->oc->getName());
      }
      delete c->name;
      c->name = NULL;
      if (c->args)
	 lvids += process_list_node(&(c->args), oflag, pflag);
      
      return lvids;
   }

   if (ntype == NT_LIST) {
      QoreListNode *l = reinterpret_cast<QoreListNode *>(*node);
      return process_list_node_intern(l, oflag, pflag); 
   }

   if (ntype == NT_HASH) {
      QoreHashNode *h = reinterpret_cast<QoreHashNode *>(*node);
      HashIterator hi(h);
      while (hi.next()) {
	 const char *k = hi.getKey();
	 AbstractQoreNode **value = hi.getValuePtr();
	 
	 // resolve constant references in keys
	 if (k[0] == HE_TAG_CONST || k[0] == HE_TAG_SCOPED_CONST)
	 {
	    AbstractQoreNode *rv;
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
	       QoreStringValueHelper t(rv);
	       
	       // reference value for new hash key
	       (*value)->ref();
	       // not possible to have an exception here
	       h->setKeyValue(t->getBuffer(), *value, 0);
	       
	       // now reget new value ptr
	       value = h->getKeyValuePtr(t->getBuffer());
	    }
	    else
	       value = 0;
	    
	    // delete the old key (not possible to have an exception here)
	    hi.deleteKey(0);
	 }
	 if (value)
	    lvids += process_node(value, oflag, pflag);
      }
      return lvids;
   }

   if (ntype == NT_FIND) {
      FindNode *f = reinterpret_cast<FindNode *>(*node);
      push_cvar(NULL);
      lvids += process_node(&(f->find_exp), oflag, pflag);
      lvids += process_node(&(f->where),    oflag, pflag);
      lvids += process_node(&(f->exp),      oflag, pflag);
      pop_cvar();
      return lvids;
   }

   if (ntype == NT_SELF_VARREF)
   {
      SelfVarrefNode *v = reinterpret_cast<SelfVarrefNode *>(*node);
      //printd(0, "process_node() SELF_REF '%s'  oflag=%d\n", v->str, oflag);
      if (!oflag)
	 parse_error("cannot reference member \"%s\" out of an object member function definition", v->str);
      
      return lvids;
   }
   
   if (ntype == NT_CLASSREF)
      reinterpret_cast<ClassRefNode *>(*node)->resolve();
   else if (ntype == NT_OBJMETHREF)
      reinterpret_cast<AbstractParseObjectMethodReferenceNode *>(*node)->parseInit(oflag, pflag);
   else if (ntype == NT_FUNCREF)
      (*node) = reinterpret_cast<UnresolvedFunctionReferenceNode *>(*node)->resolve();
   else if (ntype == NT_FUNCREFCALL)
      reinterpret_cast<FunctionReferenceCallNode *>(*node)->parseInit(oflag, pflag);
   
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

   // this is a constructor method, push local $self variable
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

   // initialize base constructor arguments
   if (bcl)
   {
      int tlvids = 0;
      for (bclist_t::iterator i = bcl->begin(); i != bcl->end(); i++)
      {
	 if ((*i)->args)
	 {
	    QoreListNode *l = (*i)->args;
	    for (int j = 0; j < l->size(); j++)
	    {
	       AbstractQoreNode **n = l->get_entry_ptr(j);
	       tlvids += process_node(n, oflag, PF_REFERENCE_OK);
	    }
	 }
      }
      if (tlvids)
      {
	 parse_error("illegal local variable declaration in base constructor argument");
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
