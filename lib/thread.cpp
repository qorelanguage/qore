/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  thread.cpp

  threading functionality for Qore

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
#include <qore/intern/ThreadResourceList.h>
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
#include <qore/intern/CallStack.h>
#endif

#include <qore/intern/ConstantList.h>
#include <qore/intern/QoreSignal.h>

// to register object types
#include <qore/intern/QC_Queue.h>
#include <qore/intern/QC_Mutex.h>
#include <qore/intern/QC_Condition.h>
#include <qore/intern/QC_RWLock.h>
#include <qore/intern/QC_Gate.h>
#include <qore/intern/QC_Sequence.h>
#include <qore/intern/QC_Counter.h>
#include <qore/intern/QC_AutoLock.h>
#include <qore/intern/QC_AutoGate.h>
#include <qore/intern/QC_AutoReadLock.h>
#include <qore/intern/QC_AutoWriteLock.h>
#include <qore/intern/QC_AbstractSmartLock.h>

#include <pthread.h>
#include <sys/time.h>
#include <assert.h>

#include <vector>

#if defined(DARWIN) && MAX_QORE_THREADS > 2560
// testing has revealed that darwin's pthread_create will not return an error when more than 2560 threads
// are running, however the threads are not actually started, therefore we set MAX_QORE_THREADS to 2560 on 
// Darwin.  This should be much more than any program/script should need (famous last words? :-) )
#warning Darwin cannot support more than 2560 threads, MAX_QORE_THREADS set to 2560
#undef MAX_QORE_THREADS
#define MAX_QORE_THREADS 2560
#endif

#if defined(__ia64) && defined(__LP64__)
#define IA64_64
#endif

Operator *OP_BACKGROUND;

ThreadCleanupList tclist;

// TID 0 is reserved for the signal handler thread
static int current_tid = 1;

DLLLOCAL bool threads_initialized = false;

DLLLOCAL QoreThreadLock lThreadList;

// recursive mutex attribute
DLLLOCAL pthread_mutexattr_t ma_recursive;

#ifndef HAVE_GETHOSTBYNAME_R
DLLLOCAL QoreThreadLock lck_gethostbyname;
#endif

#ifndef HAVE_GETHOSTBYADDR_R
DLLLOCAL QoreThreadLock lck_gethostbyaddr;
#endif

#ifdef QORE_MANAGE_STACK
// default size and limit for qore threads; to be set in init_qore_threads()
size_t qore_thread_stack_size = 0;
size_t qore_thread_stack_limit = 0;
#endif

// total number of threads running
DLLLOCAL int num_threads = 0;

// default thread creation attribute
QorePThreadAttr ta_default;

class tid_node {
public:
   int tid;
   tid_node *next, *prev;
   
   DLLLOCAL tid_node(int ntid);
   DLLLOCAL ~tid_node();
};

static tid_node *tid_head = 0, *tid_tail = 0;
 
// for recursive constant reference detection while parsing
typedef std::set<ConstantEntry *> constant_set_t;

// this structure holds all thread data that can be addressed with the qore tid
class ThreadEntry {
public:
   pthread_t ptid;
   tid_node *tidnode;
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   CallStack *callStack;
#endif
   bool joined; // if set to true then pthread_detach should not be called on exit
   
   DLLLOCAL void cleanup() {
      // delete tidnode from tid_list
      delete tidnode;

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      // delete call stack
      delete callStack;
#endif
  
      if (ptid != (pthread_t)-1L) {
	 if (!joined)
	    pthread_detach(ptid);
	    
	 // set ptid to 0
	 ptid = 0L;
      }
   }

   DLLLOCAL void allocate(tid_node *tn) {
      ptid = (pthread_t)-1L;
      tidnode = tn;
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
      callStack = 0;
#endif
      joined = false;
   }
};

DLLLOCAL ThreadEntry thread_list[MAX_QORE_THREADS];

class ProgramLocation {
public:
   const char *file;
   void *parseState;
   ProgramLocation *next;
      
   DLLLOCAL ProgramLocation(const char *fname, void *ps = 0) { 
      file       = fname; 
      parseState = ps;
   }
};

#ifndef QORE_THREAD_STACK_BLOCK
#define QORE_THREAD_STACK_BLOCK 256
#endif

struct ThreadVariableBlock {
   LocalVarValue lvar[QORE_THREAD_STACK_BLOCK];
   int pos;
   ThreadVariableBlock *prev, *next;

   DLLLOCAL ThreadVariableBlock(struct ThreadVariableBlock *n_prev = 0) : pos(0), prev(n_prev), next(0) { }
   DLLLOCAL ~ThreadVariableBlock() { }
};

class ThreadLocalVariableData {
private:
   ThreadVariableBlock *curr;
      
public:
   DLLLOCAL ThreadLocalVariableData() {
      curr = new ThreadVariableBlock;
      //printf("this=%p: first curr=%p\n", this, curr);
   }

   DLLLOCAL ~ThreadLocalVariableData() {
#ifdef DEBUG
      if (curr->pos)
	 printf("~ThreadLocalVariableData::~~ThreadLocalVariableData() this=%p: del curr=%p pos=%d next=%p prev=%p\n", this, curr, curr->pos, curr->next, curr->prev);
#endif
      assert(!curr->prev);
      assert(!curr->pos);
      if (curr->next)
	 delete curr->next;
      delete curr;
   }

   DLLLOCAL LocalVarValue *instantiate() {
      if (curr->pos == QORE_THREAD_STACK_BLOCK) {
	 if (curr->next)
	    curr = curr->next;
	 else {
	    curr->next = new ThreadVariableBlock(curr);
	    //printf("this=%p: add curr=%p, curr->next=%p\n", this, curr, curr->next);
	    curr = curr->next;
	 }
      }
      return &curr->lvar[curr->pos++];
   }

   DLLLOCAL void uninstantiate(ExceptionSink *xsink) {
      if (!curr->pos) {
	 if (curr->next) {
	    //printf("this %p: del curr=%p, curr->next=%p\n", this, curr, curr->next);
	    delete curr->next;
	    curr->next = 0;
	 }
	 curr = curr->prev;
      }
      curr->lvar[--curr->pos].uninstantiate(xsink);
   }

#ifndef HAVE_UNLIMITED_THREAD_KEYS
   DLLLOCAL LocalVarValue *find(const char *id) {
      ThreadVariableBlock *w = curr;
      while (true) {
	 int p = w->pos;
	 while (p) {
	    if (w->lvar[--p].id == id && !w->lvar[p].skip)
	       return &w->lvar[p];
	 }
	 w = w->prev;
#ifdef DEBUG
	 if (!w) printd(0, "no local variable '%s' on stack\n", id);
#endif
	 assert(w);
      }
      // to avoid a warning on most compilers - note that this generates a warning on recent versions of aCC!
      return 0;
   }
#endif
};

struct ThreadClosureVariableBlock {
   ClosureVarValue *cvar[QORE_THREAD_STACK_BLOCK];
   int pos;
   ThreadClosureVariableBlock *prev, *next;

   DLLLOCAL ThreadClosureVariableBlock(ThreadClosureVariableBlock *n_prev = 0) : pos(0), prev(n_prev), next(0) { }

   DLLLOCAL ~ThreadClosureVariableBlock() { }
};

class ThreadClosureVariableStack {
private:
   ThreadClosureVariableBlock *curr;

   DLLLOCAL void instantiate(ClosureVarValue *cvar) {
      if (curr->pos == QORE_THREAD_STACK_BLOCK) {
	 if (curr->next)
	    curr = curr->next;
	 else {
	    curr->next = new ThreadClosureVariableBlock(curr);
	    //printf("this=%p: add curr=%p, curr->next=%p\n", this, curr, curr->next);
	    curr = curr->next;
	 }
      }
      curr->cvar[curr->pos++] = cvar;
   }
      
public:
   DLLLOCAL ThreadClosureVariableStack() {
      curr = new ThreadClosureVariableBlock;
      //printf("this=%p: first curr=%p\n", this, curr);
   }

   DLLLOCAL ~ThreadClosureVariableStack() {
      assert(!curr->prev);
      assert(!curr->pos);
      //printf("this=%p: del curr=%p\n", this, curr);
      if (curr->next)
	 delete curr->next;
      delete curr;
   }

   DLLLOCAL ClosureVarValue *instantiate(const char *id, AbstractQoreNode *value) {
      ClosureVarValue *cvar = new ClosureVarValue(id, value);
      instantiate(cvar);
      return cvar;
   }

   DLLLOCAL ClosureVarValue *instantiate(const char *id, AbstractQoreNode *vexp, QoreObject *obj) {
      ClosureVarValue *cvar = new ClosureVarValue(id, vexp, obj);
      instantiate(cvar);
      return cvar;
   }

   DLLLOCAL void uninstantiate(ExceptionSink *xsink) {
      if (!curr->pos) {
	 if (curr->next) {
	    //printf("this %p: del curr=%p, curr->next=%p\n", this, curr, curr->next);
	    delete curr->next;
	    curr->next = 0;
	 }
	 curr = curr->prev;
      }
      curr->cvar[--curr->pos]->deref(xsink);
   }

   DLLLOCAL ClosureVarValue *find(const char *id) {
      ThreadClosureVariableBlock *w = curr;
      while (true) {
	 int p = w->pos;
	 while (p) {
	    if (w->cvar[--p]->id == id && !w->cvar[p]->skip)
	       return w->cvar[p];
	 }
	 w = w->prev;
#ifdef DEBUG
	 if (!w) printd(0, "no closure variable '%s' on stack\n", id);
#endif
	 assert(w);
      }
      // to avoid a warning on most compilers - note that this generates a warning on recent versions of aCC!
      return 0;
   }
};

class ArgvRefStack {
protected:
   typedef std::vector<int> rvec_t;
   rvec_t stack;
   // ignore numeric count
   int in;

public:
   DLLLOCAL ArgvRefStack() : in(0) {
      stack.push_back(0);
   }
   DLLLOCAL ~ArgvRefStack() {
   }
   DLLLOCAL void push() {
      stack.push_back(0);
   }
   DLLLOCAL int pop() {
      int rc = stack[stack.size() - 1];
      if (stack.size() > 1) 
	 stack.pop_back();
      else
	 stack[0] = 0;
      return rc;
   }
   DLLLOCAL int get() {
      assert(stack.size() == 1);
      int rc = stack[0];
      stack[0] = 0;
      return rc;
   }
   DLLLOCAL void push_numeric() {
      ++in;
   }
   DLLLOCAL void pop_numeric() {
      --in;
      assert(in >= 0);
   }
   DLLLOCAL void inc_numeric() {
      if (in)
	 return;
      inc();
   }
   DLLLOCAL void inc() {
      ++stack[stack.size() - 1];
   }
   DLLLOCAL void clear() {
      stack.clear();
      stack.push_back(0);
   }
};

// this structure holds all thread-specific data
class ThreadData {
public:
   int tid;
   VLock vlock;     // for deadlock detection
   Context *context_stack;
   ProgramLocation *plStack;
   int parse_line_start, parse_line_end;
   const char *parse_file;
   int pgm_counter_start, pgm_counter_end;
   const char *pgm_file;
   const char *parse_code; // the current function, method, or closure being parsed
   void *parseState;
   VNode *vstack;  // used during parsing (local variable stack)
   CVNode *cvarstack;
   QoreClass *parseClass; // current class being parsed
   QoreException *catchException;
   std::list<block_list_t::iterator> on_block_exit_list;
   ThreadResourceList trlist;

   // current function/method name
   const char *current_code;

   // current object context
   QoreObject *current_obj;

   // current program context
   QoreProgram *current_pgm;

   // current implicit argument
   QoreListNode *current_implicit_arg;

   // local variable data slots
   ThreadLocalVariableData lvstack;

   // closure variable stack
   ThreadClosureVariableStack cvstack;

   // current parsing closure environment
   ClosureParseEnvironment *closure_parse_env;

   // current runtime closure environment
   ClosureRuntimeEnvironment *closure_rt_env;

   // link to last ProgramContextHelp to check for
   // already-instantiated thread-local variables
   ProgramContextHelper *pch_link;

   ArgvRefStack argv_refs;

#ifdef QORE_MANAGE_STACK
   size_t stack_limit;
#ifdef IA64_64
   size_t rse_limit;
#endif
#endif

   // used to detect output of recursive data structures at runtime
   const_node_set_t node_set;

   // for recursive constant reference detection while parsing
   constant_set_t cset;

   // currently-executing/parsing block's return type
   const QoreTypeInfo *returnTypeInfo;

   // current implicit element offset
   int element;

   // start of global thread-local variables for the current thread and program being parsed
   VNode *global_vnode;

   DLLLOCAL ThreadData(int ptid, QoreProgram *p) : 
      tid(ptid), vlock(ptid), context_stack(0), plStack(0), 
      parse_line_start(0), parse_line_end(0), parse_file(0), 
      pgm_counter_start(0), pgm_counter_end(0), pgm_file(0), 
      parse_code(0), parseState(0), vstack(0), cvarstack(0),
      parseClass(0), catchException(0), current_code(0),
      current_obj(0), current_pgm(p), current_implicit_arg(0),
      closure_parse_env(0), closure_rt_env(0), pch_link(0),
      returnTypeInfo(0), element(0), global_vnode(0) {
#ifdef QORE_MANAGE_STACK

#ifdef STACK_DIRECTION_DOWN
      stack_limit = get_stack_pos() - qore_thread_stack_limit;
#else
      stack_limit = get_stack_pos() + qore_thread_stack_limit;
#endif // #ifdef STACK_DIRECTION_DOWN

#ifdef IA64_64
      // RSE stack grows up
      rse_limit = get_rse_bsp() + qore_thread_stack_limit;
#endif // #ifdef IA64_64

#endif // #ifdef QORE_MANAGE_STACK
   }

   DLLLOCAL ~ThreadData();

   DLLLOCAL int get_element() {
      return element;
   }

   DLLLOCAL int save_element(int n_element) {
      int rc = element;
      element = n_element;
      return rc;
   }
};

static QoreThreadLocalStorage<ThreadData> thread_data;

class ThreadCleanupNode {
public:
   qtdest_t func;
   void *arg;
   ThreadCleanupNode *next;
};

DLLLOCAL ThreadCleanupNode *ThreadCleanupList::head = 0;

class ThreadParams {
public:
   AbstractQoreNode *fc;
   int tid;
   QoreProgram *pgm;
   
   DLLLOCAL ThreadParams(AbstractQoreNode *f, int t) { 
      fc = f; 
      tid = t;
      pgm = getProgram();
   } 
};

// this constructor must only be called with the lThreadList lock held
tid_node::tid_node(int ntid) {
   tid = ntid;
   next = 0;
   prev = tid_tail;
   if (!tid_head)
      tid_head = this;
   else
      tid_tail->next = this;
   tid_tail = this;
}

// this destructor must only be called with the lThreadList lock held
tid_node::~tid_node() {
   if (prev)
      prev->next = next;
   else
      tid_head = next;
   if (next)
      next->prev = prev;
   else
      tid_tail = prev;
}

class BGThreadParams {
private:
   QoreObject *callobj;
public:
   QoreObject *obj;
   AbstractQoreNode *fc;
   QoreProgram *pgm;
   int tid;
   int s_line, e_line;
   const char *file;
   bool method_reference;
   
   DLLLOCAL BGThreadParams(AbstractQoreNode *f, int t, ExceptionSink *xsink) : callobj(getStackObject()), obj(0),
									       fc(f), pgm(getProgram()), tid(t) {
      // callobj: get and reference the current stack object, if any, for the new call stack
      // save program location
      file = get_pgm_counter(s_line, e_line);

      qore_type_t fctype = fc->getType();
      //printd(5, "BGThreadParams::BGThreadParams(f=%p (%s %d), t=%d) this=%p callobj=%p\n", f, f->getTypeName(), f->getType(), t, this, callobj);
      if (fctype == NT_SELF_CALL) {
	 // must have a current object if an in-object method call is being executed
	 // (i.e. $.method())
	 assert(callobj);
	 // we reference the object so it won't go out of scope while the thread is running
	 obj = callobj;
	 obj->ref();
	 callobj = 0;
      }
      else if (fctype == NT_TREE) {
	 QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(fc);
	 if (tree->getOp() == OP_OBJECT_FUNC_REF) {
	    // evaluate object
	    QoreNodeEvalOptionalRefHolder n(tree->left, xsink);
	    if (*xsink || is_nothing(*n))
	       return;

	    // if we have actually evaluated something, then we save the result in the tree
	    if (n.isTemp()) {
	       tree->left->deref(xsink);
	       tree->left = n.getReferencedValue();
	    } else if (n->getType() == NT_OBJECT) {
	       // we reference the object so it won't go out of scope while the thread is running
	       obj = reinterpret_cast<QoreObject *>(n.getReferencedValue());
	       callobj = 0;
	    }
	 }
      }

      if (callobj)
	 callobj->tRef();

      // increment the program's thread counter
      pgm->tc_inc();
   }

   DLLLOCAL ~BGThreadParams() {
      // decrement program's thread count
      pgm->tc_dec();
   }

   DLLLOCAL QoreObject *getCallObject() {
      return obj ? obj : callobj;
   }
   
   DLLLOCAL void cleanup(ExceptionSink *xsink) {
      if (fc) fc->deref(xsink);
      derefObj(xsink);
      derefCallObj();
   }
   
   DLLLOCAL void derefCallObj() {
      // dereference call object if present
      if (callobj) {
	 callobj->tDeref();
	 callobj = 0;
      }
   }

   DLLLOCAL void derefObj(ExceptionSink *xsink) {
      if (obj) {
	 obj->deref(xsink);
	 obj = 0;
      }
   }
   
   DLLLOCAL AbstractQoreNode *exec(ExceptionSink *xsink) {
      //printd(5, "BGThreadParams::exec() this=%p fc=%p (%s %d)\n", this, fc, fc->getTypeName(), fc->getType());
      AbstractQoreNode *rv = fc->eval(xsink);
      fc->deref(xsink);
      fc = 0;
      return rv;
   }
};

ThreadCleanupList::ThreadCleanupList() {
   //printf("ThreadCleanupList::ThreadCleanupList() head=NULL\n");
   head = 0;
}

ThreadCleanupList::~ThreadCleanupList() {
   //printf("ThreadCleanupList::~ThreadCleanupList() head=%p\n", head);

   while (head) {
      class ThreadCleanupNode *w = head->next;
      delete head;
      head = w;
   }
}

void ThreadCleanupList::exec() {
   class ThreadCleanupNode *w = head;
   while (w) {
      w->func(w->arg);
      w = w->next;
   }
}

void ThreadCleanupList::push(qtdest_t func, void *arg) {
   class ThreadCleanupNode *w = new ThreadCleanupNode;
   w->next = head;
   w->func = func;
   w->arg = arg;
   head = w;
   //printf("TCL::push() this=%p, &head=%p, head=%p, head->next=%p\n", this, &head, head, head->next);
}

void ThreadCleanupList::pop(bool exec) {
   //printf("TCL::pop() this=%p, &head=%p, head=%p\n", this, &head, head);
   // NOTE: if exit() is called, then somehow head = 0 !!!
   // I can't explain it, but that's why the if statement is there... :-(
   if (head) {
      if (exec)
	 head->func(head->arg);
      ThreadCleanupNode *w = head->next;
      delete head;
      head = w;
   }
}

ThreadData::~ThreadData() {
   assert(on_block_exit_list.empty());
}

#ifdef QORE_MANAGE_STACK
int check_stack(ExceptionSink *xsink) {
   ThreadData *td = thread_data.get();
   printd(5, "check_stack() current=%p limit=%p\n", get_stack_pos(), td->stack_limit);
#ifdef IA64_64
   //printd(5, "check_stack() bsp current=%p limit=%p\n", get_rse_bsp(), td->rse_limit);
   if (td->rse_limit < get_rse_bsp()) {
      xsink->raiseException("STACK-LIMIT-EXCEEDED", "this thread's stack has exceeded the IA-64 RSE (Register Stack Engine) stack size limit (%ld bytes)", qore_thread_stack_limit);
      return -1;
   }

#endif

   if (td->stack_limit
#ifdef STACK_DIRECTION_DOWN
   >
#else
   <
#endif
   get_stack_pos()) {
      xsink->raiseException("STACK-LIMIT-EXCEEDED", "this thread's stack has exceeded the stack size limit (%ld bytes)", qore_thread_stack_limit);
      return -1;
   }
   
   return 0;
}
#endif

// returns 1 if data structure is already on stack, 0 if not (=OK)
int thread_push_container(const AbstractQoreNode *n) {
   std::pair<const_node_set_t::iterator, bool> rv = thread_data.get()->node_set.insert(n);
   return !rv.second;
}

void thread_pop_container(const AbstractQoreNode *n) {
   ThreadData *td = thread_data.get();
   
   const_node_set_t::iterator i = td->node_set.find(n);
   assert(i != td->node_set.end());
   td->node_set.erase(i);
}

LocalVarValue *thread_instantiate_lvar() {
   return thread_data.get()->lvstack.instantiate();
}

void thread_uninstantiate_lvar(ExceptionSink *xsink) {
   ThreadData *td = thread_data.get();
   td->lvstack.uninstantiate(xsink);
}

LocalVarValue *thread_find_lvar(const char *id) {
   ThreadData *td = thread_data.get();
   return td->lvstack.find(id);
}

ClosureVarValue *thread_instantiate_closure_var(const char *n_id, AbstractQoreNode *value) {
   return thread_data.get()->cvstack.instantiate(n_id, value);
}

ClosureVarValue *thread_instantiate_closure_var(const char *n_id, AbstractQoreNode *vexp, QoreObject *obj) {
   return thread_data.get()->cvstack.instantiate(n_id, vexp, obj);
}

void thread_uninstantiate_closure_var(ExceptionSink *xsink) {
   thread_data.get()->cvstack.uninstantiate(xsink);
}

ClosureVarValue *thread_find_closure_var(const char *id) {
   return thread_data.get()->cvstack.find(id);
}

ClosureRuntimeEnvironment *thread_get_runtime_closure_env() {
   return thread_data.get()->closure_rt_env;
}

void thread_set_runtime_closure_env(ClosureRuntimeEnvironment *cenv) {
   thread_data.get()->closure_rt_env = cenv;
}

void thread_set_closure_parse_env(ClosureParseEnvironment *cenv) {
   thread_data.get()->closure_parse_env = cenv;
}

ClosureVarValue *thread_get_runtime_closure_var(const LocalVar *id) {
   return thread_data.get()->closure_rt_env->find(id);
}

ClosureParseEnvironment *thread_get_closure_parse_env() {
   return thread_data.get()->closure_parse_env;
}

void set_thread_resource(AbstractThreadResource *atr) {
   ThreadData *td = thread_data.get();
   td->trlist.set(atr);
}

int remove_thread_resource(AbstractThreadResource *atr) {
   ThreadData *td = thread_data.get();
   return td->trlist.remove(atr);
}

void set_thread_resource_id(q_trid_t trid, AbstractThreadResource *atr) {
   ThreadData *td = thread_data.get();
   td->trlist.set(trid, atr);   
}

int remove_thread_resource_id(q_trid_t trid) {
   ThreadData *td = thread_data.get();
   return td->trlist.remove_id(trid);
}

bool check_thread_resource_id(q_trid_t trid) {
   return thread_data.get()->trlist.check(trid);
}

q_trid_t qore_get_trid() {
   return ThreadResourceList::get_resource_id();
}

void purge_thread_resources(ExceptionSink *xsink) {
   ThreadData *td = thread_data.get();
   td->trlist.purge(xsink);
}

// called when a StatementBlock has "on_exit" blocks
void pushBlock(block_list_t::iterator i) {
   ThreadData *td = thread_data.get();
   td->on_block_exit_list.push_back(i);
}

// called when a StatementBlock has "on_exit" blocks
block_list_t::iterator popBlock() {
   ThreadData *td = thread_data.get();
   block_list_t::iterator i = td->on_block_exit_list.back();
   td->on_block_exit_list.pop_back();
   return i;
}

// called by each "on_exit" statement to activate its code for the block exit
void advanceOnBlockExit() {
   ThreadData *td = thread_data.get();
   --td->on_block_exit_list.back();
}

// new file name, current parse state
void beginParsing(char *file, void *ps) {
   ThreadData *td = thread_data.get();
   
   //printd(5, "beginParsing() of %p (%s), (stack=%s)\n", file, file ? file : "null", (td->plStack ? td->plStack->file : "NONE"));
   
   // if current position exists, then save
   if (td->parse_file) {
      ProgramLocation *pl = new ProgramLocation(td->parse_file, td->parseState);
      if (!td->plStack) {
	 pl->next = 0;
	 td->plStack = pl;
      }
      else {
	 pl->next = td->plStack;
	 td->plStack = pl;
      }
   }
   td->parse_file = file;
   td->parseState = ps;
}

void *endParsing() {
   ThreadData *td = thread_data.get();
   void *rv = td->parseState;
   
   printd(5, "endParsing() ending parsing of \"%s\", returning %p\n", td->parse_file, rv);
   if (td->plStack) {
      ProgramLocation *pl = td->plStack->next;
      td->parse_file  = td->plStack->file;
      td->parseState  = td->plStack->parseState;
      delete td->plStack;
      td->plStack = pl;
   }
   else {
      td->parse_file = 0;
      td->parseState = 0;
   }
   return rv;
}

// thread-local functions
bool is_valid_qore_thread() {
   return (bool)thread_data.get();
}

int gettid() {
   return (thread_data.get())->tid;
}

VLock *getVLock() {
   ThreadData *td = thread_data.get();
   return &td->vlock;
}

Context *get_context_stack() {
   return (thread_data.get())->context_stack;
}

void update_context_stack(Context *cstack) {
   ThreadData *td    = thread_data.get();
   td->context_stack = cstack;
}

const char *get_pgm_counter(int &start_line, int &end_line) {
   ThreadData *td = thread_data.get();
   start_line = td->pgm_counter_start;
   end_line = td->pgm_counter_end;
   return td->pgm_file;
}

void update_pgm_counter_pgm_file(int start_line, int end_line, const char *f) {
   ThreadData *td  = thread_data.get();
   td->pgm_counter_start = start_line;
   td->pgm_counter_end   = end_line;
   td->pgm_file          = f;
}

void update_pgm_counter(int start_line, int end_line) {
   ThreadData *td  = thread_data.get();
   td->pgm_counter_start = start_line;
   td->pgm_counter_end   = end_line;
}

const char *get_pgm_file() {
   return (thread_data.get())->pgm_file;
}

void get_parse_location(int &start_line, int &end_line) {
   ThreadData *td = thread_data.get();
   start_line = td->parse_line_start;
   end_line = td->parse_line_end;
}

void update_parse_location(int start_line, int end_line, const char *f) {
   ThreadData *td  = thread_data.get();
   td->parse_line_start = start_line;
   td->parse_line_end   = end_line;
   td->parse_file       = f;
}

void update_parse_location(int start_line, int end_line) {
   ThreadData *td  = thread_data.get();
   td->parse_line_start = start_line;
   td->parse_line_end   = end_line;
}

const char *get_parse_file() {
   return (thread_data.get())->parse_file;
}

const char *get_parse_code() {
   return (thread_data.get())->parse_code;
}

void parseSetCodeInfo(const char *parse_code, const QoreTypeInfo *returnTypeInfo, const char *&old_code, const QoreTypeInfo *&old_returnTypeInfo) {
   ThreadData *td = thread_data.get();
   old_code = td->parse_code;
   old_returnTypeInfo = td->returnTypeInfo;
   td->parse_code = parse_code;
   td->returnTypeInfo = returnTypeInfo;
}

void parseRestoreCodeInfo(const char *parse_code, const QoreTypeInfo *returnTypeInfo) {
   ThreadData *td = thread_data.get();
   td->parse_code = parse_code;
   td->returnTypeInfo = returnTypeInfo;
}

const QoreTypeInfo *getReturnTypeInfo() {
   return (thread_data.get())->returnTypeInfo;
}

const QoreTypeInfo *saveReturnTypeInfo(const QoreTypeInfo *returnTypeInfo) {
   ThreadData *td  = thread_data.get();
   const QoreTypeInfo *rv = td->returnTypeInfo;
   td->returnTypeInfo = returnTypeInfo;
   return rv;
}

const AbstractQoreZoneInfo *currentTZ() {
   QoreProgram *pgm = (thread_data.get())->current_pgm;
   return pgm ? pgm->currentTZ() : QTZM.getLocalZoneInfo();
}

// pushes a new argv reference counter
void new_argv_ref() {
   thread_data.get()->argv_refs.push();
}

// increments the parse argv reference counter
void inc_argv_ref() {
   thread_data.get()->argv_refs.inc();
}

// pushes an "ignore numeric reference" context
void push_ignore_numeric_argv_ref() {
   thread_data.get()->argv_refs.push_numeric();
}

// pops an "ignore numeric reference" context
void pop_ignore_numeric_argv_ref() {
   thread_data.get()->argv_refs.pop_numeric();
}

// increments the parse argv numeric reference counter
void inc_numeric_argv_ref() {
   thread_data.get()->argv_refs.inc_numeric();
}

// gets the parse argv reference counter and pops the context
int get_pop_argv_ref() {
   return thread_data.get()->argv_refs.pop();
}

// clears the argv reference stack
void clear_argv_ref() {
   thread_data.get()->argv_refs.clear();
}

int set_constant(ConstantEntry *ce) {
   ThreadData *td  = thread_data.get();
   if (td->cset.find(ce) != td->cset.end())
      return -1;

   td->cset.insert(ce);
   return 0;
}

void remove_constant(ConstantEntry *ce) {
   ThreadData *td  = thread_data.get();
   assert(td->cset.find(ce) != td->cset.end());
   td->cset.erase(ce);
}

int get_implicit_element() {
   return thread_data.get()->get_element();
}

int save_implicit_element(int n_element) {
   return thread_data.get()->save_element(n_element);
}

ObjectSubstitutionHelper::ObjectSubstitutionHelper(QoreObject *obj) {
   ThreadData *td  = thread_data.get();
   old_obj = td->current_obj;
   td->current_obj = obj;
}

ObjectSubstitutionHelper::~ObjectSubstitutionHelper() {
   ThreadData *td  = thread_data.get();
   td->current_obj = old_obj;
}

CodeContextHelper::CodeContextHelper(const char *code, const QoreObject *obj, ExceptionSink *xs) {
   ThreadData *td  = thread_data.get();
   old_code = td->current_code;
   old_obj = td->current_obj;
   xsink = xs;
   if (obj)
      obj->ref();
   td->current_code = code;
   td->current_obj = const_cast<QoreObject *>(obj);
   //printd(5, "CodeContextHelper::CodeContextHelper(code=%s, obj=%p) this=%p td=%p, old_code=%s, old_obj=%p\n", code ? code : "null", obj, this, td, old_code ? old_code : "null", old_obj);
}

CodeContextHelper::~CodeContextHelper() {
   ThreadData *td  = thread_data.get();
   //printd(5, "CodeContextHelper::~CodeContextHelper() this=%p td=%p current=(code=%s, obj=%p) restoring code=%s, obj=%p\n", this, td, td->current_code ? td->current_code : "null", td->current_obj, old_code ? old_code : "null", old_obj);

   if (td->current_obj)
      td->current_obj->deref(xsink);
   td->current_code = old_code;
   td->current_obj = old_obj;
}

ArgvContextHelper::ArgvContextHelper(QoreListNode *argv, ExceptionSink *n_xsink) : xsink(n_xsink) {
   ThreadData *td  = thread_data.get();
   old_argv = td->current_implicit_arg;
   td->current_implicit_arg = argv;
   //printd(5, "ArgvContextHelper::ArgvContextHelper() setting argv=%p\n", argv);
}

ArgvContextHelper::~ArgvContextHelper() {
   ThreadData *td  = thread_data.get();
   if (td->current_implicit_arg)
      td->current_implicit_arg->deref(xsink);
   td->current_implicit_arg = old_argv;
   //printd(5, "ArgvContextHelper::~ArgvContextHelper() setting argv=%p\n", old_argv);
}

SingleArgvContextHelper::SingleArgvContextHelper(const AbstractQoreNode *val, ExceptionSink *n_xsink) : xsink(n_xsink) {
   //printd(0, "SingleArgvContextHelper::SingleArgvContextHelper() this=%p arg=%p (%s)\n", this, val, val ? val->getTypeName() : 0);
   ThreadData *td  = thread_data.get();
   old_argv = td->current_implicit_arg;
   QoreListNode *argv;
   if (!is_nothing(val)) {
      argv = new QoreListNode();
      argv->push(val->refSelf());
   }
   else
      argv = 0;
   td->current_implicit_arg = argv;
}

SingleArgvContextHelper::~SingleArgvContextHelper() {
   ThreadData *td  = thread_data.get();
   if (td->current_implicit_arg)
      td->current_implicit_arg->deref(xsink);
   td->current_implicit_arg = old_argv;
}

const QoreListNode *thread_get_implicit_args() {
   //printd(5, "thread_get_implicit_args() returning %p\n", thread_data.get()->current_implicit_arg);
   return thread_data.get()->current_implicit_arg;
}

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
void pushCall(CallNode *cn) {
   thread_list[gettid()].callStack->push(cn);
}

void popCall(ExceptionSink *xsink) {
   thread_list[gettid()].callStack->pop(xsink);
}

QoreListNode *getCallStackList() {
   return thread_list[gettid()].callStack->getCallStack();
}

CallStack *getCallStack() {
   return thread_list[gettid()].callStack;
}
#endif

bool inMethod(const char *name, const QoreObject *o) {
   ThreadData *td = thread_data.get();
   if (td->current_obj == o && td->current_code == name)
      return true;
   return false;
}

QoreObject *getStackObject() {
   return (thread_data.get())->current_obj;
}

ProgramContextHelper::ProgramContextHelper(QoreProgram *pgm, ExceptionSink *xs) : 
   old_pgm(0), last(0), xsink (0), restore(false) {
   if (pgm) {
      ThreadData *td = thread_data.get();
      //printd(5, "ProgramContextHelper::ProgramContextHelper() current_pgm=%p new_pgm=%p\n", td->current_pgm, pgm);
      if (pgm != td->current_pgm) {
	 // push link to last ProgramContextHelper
	 last = td->pch_link;
	 td->pch_link = this;

	 // instantiate program thread-local variables
	 // (top-level local variables) if necessary
	 const LVList *l = pgm->getTopLevelLVList();
	 if (l && xs) {
	    ProgramContextHelper *w = last;
	    while (w && w->old_pgm != pgm)
	       w = w->last;

	    // instantiate variables if program not found
	    if (!w) {
	       xsink = xs; // <- marks for uninstantiation
	       for (int i = 0; i < l->num_lvars; ++i)
		  l->lv[i]->instantiate();
	    }
	 }

	 restore = true;
	 old_pgm = td->current_pgm;
	 td->current_pgm = pgm;
      }
   }
}

ProgramContextHelper::~ProgramContextHelper() {
   if (restore) {
      ThreadData *td = thread_data.get();      

      // uninstantiate thread-local variables if necessary
      if (xsink) {
	 const LVList *l = td->current_pgm->getTopLevelLVList();
	 for (int i = 0; i < l->num_lvars; ++i)
            l->lv[i]->uninstantiate(xsink);
      }
      //printd(5, "ProgramContextHelper::~ProgramContextHelper() current_pgm=%p restoring old pgm=%p\n", td->current_pgm, old_pgm);
      td->current_pgm = old_pgm;
      td->pch_link = last;
   }
}

QoreProgram *getProgram() {
   return (thread_data.get())->current_pgm;
   //return (thread_data.get())->pgmStack->getProgram();
}

RootQoreNamespace *getRootNS() {
   return (thread_data.get())->current_pgm->getRootNS();
   //return (thread_data.get())->pgmStack->getProgram()->getRootNS();
}

int64 getParseOptions() {
   return (thread_data.get())->current_pgm->getParseOptions64();
   //return (thread_data.get())->pgmStack->getProgram()->getParseOptions();
}

bool checkParseOption(int64 o) {
   return (getParseOptions() & o) == o;
}

void updateCVarStack(CVNode *ncvs) {
   ThreadData *td = thread_data.get();
   td->cvarstack = ncvs;
}

CVNode *getCVarStack() {
   return (thread_data.get())->cvarstack;
}

void updateVStack(VNode *nvs) {
   ThreadData *td = thread_data.get();
   td->vstack = nvs;
}

VNode *getVStack() {
   return (thread_data.get())->vstack;
}

void save_global_vnode(VNode *vn) {
   ThreadData *td = thread_data.get();
   td->global_vnode = vn;
}

VNode *get_global_vnode() {
   return (thread_data.get())->global_vnode;
}

void setParseClass(QoreClass *c) {
   ThreadData *td = thread_data.get();
   td->parseClass = c;
}

QoreClass *getParseClass() {
   return (thread_data.get())->parseClass;
}

// to save the exception for "rethrow"
void catchSaveException(QoreException *e) {
   ThreadData *td = thread_data.get();
   //printd(5, "cSE() td=%p e=%p\n", td, e);
   td->catchException = e;
}

// for "rethrow"
QoreException *catchGetException() {
   ThreadData *td = thread_data.get();
   //printd(5, "cGE() td=%p e=%p\n", td, td->catchException);
   assert(td->catchException);
   return td->catchException;
}

void qore_exit_process(int rc) {
   int tid = gettid();
   SafeLocker sl(lThreadList);

   // call pthread_cancel on all threads so the call to exit() will not
   // cause a core dump
   for (int i = 1; i < MAX_QORE_THREADS; ++i) {
      if (i != tid && thread_list[i].ptid) {
	 int trc = pthread_cancel(thread_list[i].ptid);
	 if (!trc) {
	    thread_list[i].joined = true;

	    sl.unlock();
	    pthread_join(thread_list[i].ptid, 0);
	    sl.lock();
	 }
#ifdef DEBUG
	 else
	    printd(0, "pthread_cancel() returned %d (%s) on tid %d (%p)\n", rc, strerror(rc), tid, thread_list[i].ptid);
#endif
      }
   }
   sl.unlock();

   // stop signal handling thread
   QSM.del();

   threads_initialized = false;
   exit(rc);
}

// sets up the signal thread entry in the thread list
int get_signal_thread_entry() {
   AutoLocker al(lThreadList);
   thread_list[0].allocate(0);
   return 0;
}

// returns tid allocated for thread
int get_thread_entry() {
   int tid = -1;
   AutoLocker al(lThreadList);

   if (current_tid == MAX_QORE_THREADS) {
      int i;
      // scan thread_list for free entry
      for (i = 1; i < MAX_QORE_THREADS; i++) {
	 if (!thread_list[i].ptid) {
	    tid = i;
	    goto finish;
	 }
      }
      if (i == MAX_QORE_THREADS)
	 return -1;
   }
   else
      tid = current_tid++;

  finish:
   thread_list[tid].allocate(new tid_node(tid));
   num_threads++;
   //printf("t%d cs=0\n", tid);

   return tid;
}

void delete_thread_data() {
   delete thread_data.get();
}

void deregister_thread(int tid) {
   // NOTE: cannot safely call printd here, because normally the thread_data has been deleted
   AutoLocker al(lThreadList);
   thread_list[tid].cleanup();
   --num_threads;
}

void deregister_signal_thread() {
   // NOTE: cannot safely call printd here, because normally the thread_data has been deleted
   AutoLocker al(lThreadList);
   thread_list[0].cleanup();
}

// should only be called from new thread
void register_thread(int tid, pthread_t ptid, QoreProgram *p) {
   thread_list[tid].ptid = ptid;
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   thread_list[tid].callStack = new CallStack();
#endif
   thread_data.set(new ThreadData(tid, p));
}

// put "op_background_thread" in an unnamed namespace to make it 'static extern "C"'
namespace {
   extern "C" void *op_background_thread(void *x) {    
      BGThreadParams *btp = (BGThreadParams *)x;
      // register thread
      register_thread(btp->tid, pthread_self(), btp->pgm);
      printd(5, "op_background_thread() btp=%p TID %d started\n", btp, btp->tid);
      //printf("op_background_thread() btp=%p TID %d started\n", btp, btp->tid);
      
      // create thread-local data for this thread in the program object
      btp->pgm->startThread();
      // set program counter for new thread
      update_pgm_counter_pgm_file(btp->s_line, btp->e_line, btp->file);

      ExceptionSink xsink;
      AbstractQoreNode *rv;
      {
	 CodeContextHelper cch(0, btp->getCallObject(), &xsink);
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
	 // push this call on the thread stack
	 CallStackHelper csh("background operator", CT_NEWTHREAD, btp->getCallObject(), &xsink);
#endif

	 // instantiate top-level local variables with no value
	 LVListInstantiator lv_helper(getProgram()->getTopLevelLVList(), &xsink);

	 // dereference call object if present
	 btp->derefCallObj();

	 // run thread expression
	 rv = btp->exec(&xsink);

	 // if there is an object, we dereference the extra reference here
	 btp->derefObj(&xsink);
      }

      // dereference any return value from the background expression
      if (rv)
	 rv->deref(&xsink);

      // delete any thread data
      btp->pgm->endThread(&xsink);
      
      // cleanup thread resources
      purge_thread_resources(&xsink);
      
      xsink.handleExceptions();

      printd(4, "thread terminating");

      // delete internal thread data structure
      delete_thread_data();

      // deregister_thread
      deregister_thread(btp->tid);

      // run any cleanup functions
      tclist.exec();

      //printd(5, "deleting thread params %p\n", btp);
      delete btp;

      pthread_exit(0);
      return 0;
   }
}

static AbstractQoreNode *op_background(const AbstractQoreNode *left, const AbstractQoreNode *ignored, bool ref_rv, ExceptionSink *xsink) {
   if (!left)
      return 0;

   //printd(2, "op_background() before crlr left = %p\n", left);
   ReferenceHolder<AbstractQoreNode> nl(copy_and_resolve_lvar_refs(left, xsink), xsink);
   //printd(2, "op_background() after crlr nl = %p\n", nl);
   if (*xsink || !nl)
      return 0;

   // now we are ready to create the new thread

   // get thread entry
   //printd(2, "calling get_thread_entry()\n");
   int tid = get_thread_entry();
   //printd(2, "got %d()\n", tid);

   // if can't start thread, then throw exception
   if (tid == -1) {
      xsink->raiseException("THREAD-CREATION-FAILURE", "thread list is full with %d threads", MAX_QORE_THREADS);
      return 0;
   }

   BGThreadParams *tp = new BGThreadParams(nl.release(), tid, xsink);
   //printd(5, "created BGThreadParams(%p, %d) = %p\n", *nl, tid, tp);
   if (*xsink) {
      deregister_thread(tid);
      return 0;
   }
   //printd(5, "tp = %p\n", tp);
   // create thread
   int rc;
   pthread_t ptid;

   //printd(5, "calling pthread_create(%p, %p, %p, %p)\n", &ptid, &ta_default, op_background_thread, tp);

   if ((rc = pthread_create(&ptid, ta_default.get_ptr(), op_background_thread, tp))) {
      tp->cleanup(xsink);
      delete tp;

      deregister_thread(tid);
      xsink->raiseErrnoException("THREAD-CREATION-FAILURE", rc, "could not create thread");
      return 0;
   }
   //printd(5, "pthread_create() new thread TID %d, pthread_create() returned %d\n", tid, rc);

   return ref_rv ? new QoreBigIntNode(tid) : 0;
}

static AbstractQoreNode *check_op_background(QoreTreeNode *tree, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo, const char *name, const char *descr) {
   returnTypeInfo = bigIntTypeInfo;

   if (pflag & PF_CONST_EXPRESSION)
      parseException("ILLEGAL-OPERATION", "the background operator may not be used in an expression initializing a constant value executed at parse time");

   return tree->defaultParseInit(oflag, pflag, lvids, returnTypeInfo);
}

void init_qore_threads() {
   QORE_TRACE("qore_init_threads()");

#ifdef QORE_MANAGE_STACK
   // get default stack size
#ifdef SOLARIS
#if TARGET_BITS == 32
   // pthread_attr_getstacksize() on default attributes returns 0 on Solaris
   // so we set according to defaults - 1MB on 32-bit builds
   qore_thread_stack_size = 1024*1024;
#else
   // 2MB on 64-bit builds
   qore_thread_stack_size = 1024*1024*2;
#endif // #if TARGET_BITS == 32
#else
#ifdef WIN32
   // FIXME: get real thread stack size via win API call
   qore_thread_stack_size = 1024*1024;
#else // !WIN32 && !SOLARIS
   qore_thread_stack_size = ta_default.getstacksize();
   assert(qore_thread_stack_size);
   //printd(0, "getstacksize() returned: %ld\n", qore_thread_stack_size);
#endif // #ifdef WIN32
#endif // #ifdef SOLARIS

#ifdef IA64_64
   // the top half of the stack is for the normal stack, the bottom half is for the register stack
   qore_thread_stack_size /= 2;
#endif // #ifdef IA64_64
   qore_thread_stack_limit = qore_thread_stack_size - QORE_STACK_GUARD;
   //printd(5, "default stack size %ld, limit %ld\n", qore_thread_stack_size, qore_thread_stack_limit);
#endif // #ifdef QORE_MANAGE_STACK

   // setup parent thread data
   register_thread(get_thread_entry(), pthread_self(), 0);

   // register "background" Operator.handler
   OP_BACKGROUND = oplist.add(new Operator(1, "background", "run in background thread", 0, true, false, check_op_background));
   OP_BACKGROUND->addFunction(NT_ALL, NT_NONE, op_background);

   // initialize recursive mutex attribute
   pthread_mutexattr_init(&ma_recursive);
   pthread_mutexattr_settype(&ma_recursive, PTHREAD_MUTEX_RECURSIVE);

#ifdef DEBUG
   // mark threading as active
   threads_initialized = true;
#endif
}

QoreNamespace *get_thread_ns() {
   // create Qore::Thread namespace
   QoreNamespace *Thread = new QoreNamespace("Thread");

   QoreClass *Mutex, *Gate, *RWLock, *AbstractSmartLock;
   Thread->addSystemClass(initQueueClass());
   Thread->addSystemClass(AbstractSmartLock = initAbstractSmartLockClass());
   Thread->addSystemClass(Mutex = initMutexClass(AbstractSmartLock));
   Thread->addSystemClass(initRMutexClass());
   Thread->addSystemClass(initConditionClass(AbstractSmartLock));
   Thread->addSystemClass(RWLock = initRWLockClass(AbstractSmartLock));
   Thread->addSystemClass(Gate = initGateClass());
   Thread->addSystemClass(initSequenceClass());
   Thread->addSystemClass(initCounterClass());

   Thread->addSystemClass(initAutoLockClass(Mutex));
   Thread->addSystemClass(initAutoGateClass(Gate));
   Thread->addSystemClass(initAutoReadLockClass(RWLock));
   Thread->addSystemClass(initAutoWriteLockClass(RWLock));


   return Thread;
}

void delete_qore_threads() {
   QORE_TRACE("delete_qore_threads()");

#ifdef DEBUG
   // mark threading as inactive
   threads_initialized = false;
#endif

   // set no program location
   update_pgm_counter_pgm_file(0, 0, 0);

   ExceptionSink xsink;
   purge_thread_resources(&xsink);
   xsink.handleExceptions();

   pthread_mutexattr_destroy(&ma_recursive);

   delete_thread_data();

   thread_list[1].cleanup();
}

QoreListNode *get_thread_list() {
   QoreListNode *l = new QoreListNode();

   AutoLocker al(lThreadList);
   tid_node *w = tid_head;
   while (w) {
      l->push(new QoreBigIntNode(w->tid));
      w = w->next;
   }
   return l;
}

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
#include <qore/QoreRWLock.h>

extern QoreRWLock thread_stack_lock;

QoreHashNode *getAllCallStacks() {
   QoreHashNode *h = new QoreHashNode();
   QoreString str;

   // grab the call stack write lock
   QoreAutoRWWriteLocker wl(thread_stack_lock);

   // grab thread list lock
   AutoLocker al(lThreadList);

   tid_node *w = tid_head;
   while (w) {
      // get call stack
      if (thread_list[w->tid].callStack) {
	 QoreListNode *l = thread_list[w->tid].callStack->getCallStack();
	 if (l->size()) {
	    // make hash entry
	    str.clear();
	    str.sprintf("%d", w->tid);
	    h->setKeyValue(str.getBuffer(), l, 0);
	 }
	 else
	    l->deref(0);
      }
      w = w->next;
   }   
   return h;
}
#endif
