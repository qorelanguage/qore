/*
  Statement.cc

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

// parse context stack
class CVNode {
   public:
      const char *name;
      CVNode *next;
      
      DLLLOCAL CVNode(const char *n) { name = n; }
};

// parse variable stack
class VNode {
   public:
      LocalVar *lvar;
      VNode *next;
      
      DLLLOCAL VNode(LocalVar *lv) : lvar(lv) { }
      DLLLOCAL const char *getName() const { return lvar->getName(); }
};

typedef LocalVar *lvar_ptr_t;

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

void push_cvar(const char *name) {
   CVNode *cvn = new CVNode(name);
   cvn->next = getCVarStack();
   updateCVarStack(cvn);
}

void pop_cvar() {
   class CVNode *cvn = getCVarStack();
   updateCVarStack(cvn->next);
   delete cvn;
}

LocalVar *push_local_var(const char *name, bool check_dup) {
   VNode *vnode;

   QoreProgram *pgm = getProgram();

   LocalVar *lv = pgm->createLocalVar(name);

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
   vnode = new class VNode(lv);
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

// checks for illegal $self assignments in an object context
static inline void checkSelf(AbstractQoreNode *n, LocalVar *selfid) {
   // if it's a variable reference
   qore_type_t ntype = n->getType();
   if (ntype == NT_VARREF) {
      VarRefNode *v = reinterpret_cast<VarRefNode *>(n);
      if (v->type == VT_LOCAL && v->ref.id == selfid)
	 parse_error("illegal assignment to $self in an object context");
      return;
   }
   
   if (ntype != NT_TREE)
      return;

   QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(n);

   // otherwise it's a tree: go to root expression 
   while (tree->left->getType() == NT_TREE) {
      n = tree->left;
      tree = reinterpret_cast<QoreTreeNode *>(n);
   }

   if (tree->left->getType() != NT_VARREF)
      return;

   VarRefNode *v = reinterpret_cast<VarRefNode *>(tree->left);

   // left must be variable reference, check if the tree is
   // a list reference; if so, it's invalid
   if (v->type == VT_LOCAL && v->ref.id == selfid  && tree->op == OP_LIST_REF)
      parse_error("illegal conversion of $self to a list");
}

static inline void checkLocalVariableChange(AbstractQoreNode *n) {
   VarRefNode *v = dynamic_cast<VarRefNode *>(n);
   if (v && v->type == VT_LOCAL)
      parse_error("illegal local variable modification in background expression");
}

static inline int getBaseLVType(AbstractQoreNode *n) {
   while (true) {
      qore_type_t ntype = n->getType();
      if (ntype == NT_SELF_VARREF)
	 return VT_OBJECT;
      if (ntype == NT_VARREF)
	 return reinterpret_cast<VarRefNode *>(n)->type;
      // must be a tree
      n = reinterpret_cast<QoreTreeNode *>(n)->left;
   }
}

int process_list_node_intern(QoreListNode *l, LocalVar *oflag, int pflag) {
   int lvids = 0;

   // set needs_eval if previously 0 and one of the list members needs evaluation after being resolved
   // for example with a resolved function reference
   bool needs_eval = l->needs_eval();
   for (unsigned i = 0; i < l->size(); i++) {
      AbstractQoreNode **n = l->get_entry_ptr(i);
      lvids += process_node(n, oflag, pflag);
      if (!needs_eval && *n && (*n)->needs_eval()) {
	 //printd(5, "setting needs_eval on list %08p\n", l);
	 l->setNeedsEval();
	 needs_eval = true;
      }
   }
   
   return lvids;
}

// this function will put variables on the local stack but will not pop them
int process_list_node(QoreListNode **node, LocalVar *oflag, int pflag) {
   if (!(*node))
      return 0;

   // unset "reference ok" for next call
   return process_list_node_intern(*node, oflag, pflag & ~PF_REFERENCE_OK);
}

// this function will put variables on the local stack but will not pop them
// FIXME: this functionality should be replaced by:
//        AbstractQoreNode *AbstractQoreNode::parseInit(LocalVar *oflag, int pflag, int &lvids) = 0;
//        to be implemented in each subclass
int process_node(AbstractQoreNode **node, LocalVar *oflag, int pflag) {
   int lvids = 0;
   int current_pflag = pflag;
   pflag &= ~PF_REFERENCE_OK;  // unset "reference ok" for next call

   //printd(5, "process_node() %08p type=%s cp=%d, p=%d\n", *node, *node ? (*node)->getTypeName() : "(null)", current_pflag, pflag);
   if (!(*node))
      return 0;

   qore_type_t ntype = (*node)->getType();
   if (ntype == NT_REFERENCE) {
      // otherwise throw a parse exception if an illegal reference is used
      if (!(current_pflag & PF_REFERENCE_OK)) {	 
	 parse_error("the reference operator can only be used in argument lists and in foreach statements");
      }
      else {
	 ReferenceNode *r = reinterpret_cast<ReferenceNode *>(*node);
	 lvids = process_node(r->getExpressionPtr(), oflag, pflag);
	 // if a background expression is being parsed, then check that no references to local variables
	 // or object members are being used
	 if (pflag & PF_BACKGROUND) {
	    int vtype = getBaseLVType(r->getExpression());

	    if (vtype == VT_LOCAL)
	       parse_error("the reference operator cannot be used with local variables in a background expression");
	     //else if (vtype == VT_OBJECT)
	     //parse_error("the reference operator cannot be used with object members in a background expression");
	 }
      }
      return lvids;
   }
   
   if (ntype == NT_VARREF) {
      VarRefNode *v = reinterpret_cast<VarRefNode *>(*node);
      // if it is a new variable being declared
      if (v->type == VT_LOCAL) {
	 v->ref.id = push_local_var(v->name);
	 ++lvids;
	 //printd(5, "process_node(): local var '%s' declared (id=%08p)\n", v->name, v->ref.id);
      }
      else if (v->type == VT_GLOBAL)
	 v->ref.var = getProgram()->createGlobalVar(v->name);
      else // otherwise reference must be resolved
	 v->resolve();

      return lvids;
   }

   if (ntype == NT_BAREWORD) {
      // resolve simple constant
      printd(5, "process_node() resolving simple constant \"%s\"\n", reinterpret_cast<BarewordNode *>(*node)->str);
      getRootNS()->resolveSimpleConstant(node, 1);
      return lvids;
   }

   if (ntype == NT_CONSTANT) {
      printd(5, "process_node() resolving scoped constant \"%s\"\n", reinterpret_cast<ConstantNode *>(*node)->scoped_ref->ostr);
      getRootNS()->resolveScopedConstant(node, 1);
      return lvids;
   }

   if (ntype == NT_COMPLEXCONTEXTREF) {
      ComplexContextrefNode *c = reinterpret_cast<ComplexContextrefNode *>(*node);
      if (!getCVarStack()) {
	 parse_error("complex context reference \"%s:%s\" encountered out of context", c->name, c->member);
	 return lvids;
      }
      
      int stack_offset = 0;
      int found = 0;
      CVNode *cvn = getCVarStack();
      while (cvn) {
	 if (cvn->name && !strcmp(c->name, cvn->name)) {
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

   if (ntype == NT_CONTEXTREF) {
      if (!getCVarStack())
	 parse_error("context reference \"%s\" out of context", reinterpret_cast<ContextrefNode *>((*node))->str);
      return lvids;
   }

   if (ntype == NT_CONTEXT_ROW) {
      if (!getCVarStack())
	 parse_error("context row reference \"%%\" encountered out of context");
      return lvids;
   }

   if (ntype == NT_TREE) {
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

   if (ntype == NT_FUNCTION_CALL || ntype == NT_METHOD_CALL || ntype == NT_STATIC_METHOD_CALL) {
      AbstractFunctionCallNode *f = reinterpret_cast<AbstractFunctionCallNode *>(*node);

      return f->parseInit(oflag, pflag);
   }

   // for the "new" operator
   if (ntype == NT_SCOPE_REF) {
      ScopedObjectCallNode *c = reinterpret_cast<ScopedObjectCallNode *>(*node);
      // find object class
      if ((c->oc = getRootNS()->parseFindScopedClass(c->name))) {
	 // check if parse options allow access to this class
	 if (c->oc->getDomain() & getProgram()->getParseOptions())
	    parseException("ILLEGAL-CLASS-INSTANTIATION", "parse options do not allow access to the '%s' class", c->oc->getName());
      }
      delete c->name;
      c->name = 0;
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
      bool needs_eval = h->needs_eval();
      HashIterator hi(h);
      while (hi.next()) {
	 const char *k = hi.getKey();
	 AbstractQoreNode **value = hi.getValuePtr();

	 // resolve constant references in keys
	 if (k[0] == HE_TAG_CONST || k[0] == HE_TAG_SCOPED_CONST) {
	    AbstractQoreNode *rv;
	    if (k[0] == HE_TAG_CONST)
	       rv = getRootNS()->findConstantValue(k + 1, 1);
	    else {
	       NamedScope *nscope = new NamedScope(strdup(k + 1));
	       rv = getRootNS()->findConstantValue(nscope, 1);
	       delete nscope;
	    }
	    if (rv) {
	       QoreStringValueHelper t(rv);

	       // reference value for new hash key
	       (*value)->ref();
	       // not possible to have an exception here
	       h->setKeyValue(t->getBuffer(), *value, 0);
	       
	       // now reget new value ptr
	       value = h->getKeyValuePtr(t->getBuffer());
	    }
	    
	    // delete the old key (not possible to have an exception here)
	    hi.deleteKey(0);
	    continue;
	 }

	 if (value) {
	    lvids += process_node(value, oflag, pflag);
	    if (!needs_eval && *value && (*value)->needs_eval()) {
	       //printd(5, "setting needs_eval on hash %08p\n", h);
	       h->setNeedsEval();
	       needs_eval = true;
	    }
	 }
      }
      return lvids;
   }

   if (ntype == NT_FIND) {
      FindNode *f = reinterpret_cast<FindNode *>(*node);
      push_cvar(0);
      lvids += process_node(&(f->find_exp), oflag, pflag);
      lvids += process_node(&(f->where),    oflag, pflag);
      lvids += process_node(&(f->exp),      oflag, pflag);
      pop_cvar();
      return lvids;
   }

   if (ntype == NT_SELF_VARREF) {
      SelfVarrefNode *v = reinterpret_cast<SelfVarrefNode *>(*node);
      printd(5, "process_node() SELF_REF '%s' oflag=%08p\n", v->str, oflag);
      if (!oflag)
	 parse_error("cannot reference member \"%s\" out of an object member function definition", v->str);
      
      return lvids;
   }
   
   if (ntype == NT_CLASSREF)
      reinterpret_cast<ClassRefNode *>(*node)->resolve();
   else if (ntype == NT_FUNCREF)
      (*node) = reinterpret_cast<AbstractUnresolvedCallReferenceNode *>(*node)->resolve();
   // types perperly using AbstractQoreNode::parseInit()
   else if (ntype == NT_FUNCREFCALL || ntype == NT_OBJMETHREF || ntype == NT_CLOSURE)
      *node = (*node)->parseInit(oflag, pflag, lvids);
   
   return lvids;
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
void StatementBlock::parseInit(Paramlist *params) {
   QORE_TRACE("StatementBlock::parseInit");

   if (params->num_params)
      params->lv = new lvar_ptr_t[params->num_params];
   else
      params->lv = 0;

   // push $argv var on stack and save id
   params->argvid = push_local_var("argv", false);
   printd(5, "StatementBlock::parseInit() params=%08p argvid=%08p\n", params, params->argvid);

   // init param ids and push local param vars on stack
   for (int i = 0; i < params->num_params; i++) {
      params->lv[i] = push_local_var(params->names[i]);
      printd(3, "StatementBlock::parseInit() reg. local var %s (id=%08p)\n", 
	     params->names[i], params->lv[i]);
   }

   // initialize code block
   if (this)
      parseInitImpl(0);

   // pop local param vars from stack
   for (int i = 0; i < params->num_params; i++)
      pop_local_var();

   // pop argv param off stack
   pop_local_var();
}

// can also be called with this=NULL
void StatementBlock::parseInitMethod(Paramlist *params, BCList *bcl) {
   QORE_TRACE("StatementBlock::parseInitMethod");

   if (params->num_params)
      params->lv = new lvar_ptr_t[params->num_params];
   else
      params->lv = 0;

   // this is a class method, push local $self variable
   params->selfid = push_local_var("self", false);
   // set oflag to selfid
   LocalVar *oflag = params->selfid;

   // push $argv var on stack and save id
   params->argvid = push_local_var("argv", false);
   printd(5, "StatementBlock::parseInitMethod() params=%08p argvid=%08p oflag (selfid)=%08p\n", params, params->argvid, oflag);

   // init param ids and push local param vars on stack
   for (int i = 0; i < params->num_params; i++) {
      params->lv[i] = push_local_var(params->names[i]);
      printd(3, "StatementBlock::parseInitMethod() reg. local var %s (id=%08p)\n", 
	     params->names[i], params->lv[i]);
   }

   // initialize base constructor arguments
   if (bcl) {
      int tlvids = 0;
      for (bclist_t::iterator i = bcl->begin(); i != bcl->end(); i++) {
	 if ((*i)->args) {
	    QoreListNode *l = (*i)->args;
	    for (unsigned j = 0; j < l->size(); j++) {
	       AbstractQoreNode **n = l->get_entry_ptr(j);
	       tlvids += process_node(n, oflag, PF_REFERENCE_OK);
	    }
	 }
      }
      if (tlvids) {
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
}

// can also be called with this=NULL
void StatementBlock::parseInitClosure(Paramlist *params, bool in_method, lvar_set_t *vlist) {
   QORE_TRACE("StatementBlock::parseInitClosure");

   ClosureParseEnvironment cenv(vlist);

   if (params->num_params)
      params->lv = new lvar_ptr_t[params->num_params];
   else
      params->lv = 0;

   LocalVar *oflag = 0;

   // this is a class method, push local $self variable
   if (in_method) {
      params->selfid = push_local_var("self", false);
      // set oflag to selfid
      oflag = params->selfid;
   }

   // push $argv var on stack and save id
   params->argvid = push_local_var("argv", false);
   printd(5, "StatementBlock::parseInitClosure() params=%08p argvid=%08p\n", params, params->argvid);

   // init param ids and push local param vars on stack
   for (int i = 0; i < params->num_params; i++) {
      params->lv[i] = push_local_var(params->names[i]);
      printd(5, "StatementBlock::parseInitClosure() reg. local var %s (id=%08p)\n", params->names[i], params->lv[i]);
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
   if (in_method)
      pop_local_var();
}
