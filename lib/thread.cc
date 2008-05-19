/*
  thread.cc

  threading functionality for Qore

  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

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
#ifdef DEBUG
#include <qore/intern/CallStack.h>
#endif

// to register object types
#include <qore/intern/QC_Queue.h>
#include <qore/intern/QC_Mutex.h>
#include <qore/intern/QC_Condition.h>
#include <qore/intern/QC_RWLock.h>
#include <qore/intern/QC_Gate.h>
#include <qore/intern/QC_Sequence.h>
#include <qore/intern/QC_Counter.h>

#include <pthread.h>
#include <sys/time.h>
#include <assert.h>

#if defined(DARWIN) && MAX_QORE_THREADS > 2560
// testing has revealed that darwin's pthread_create will not return an error when more than 2560 threads
// are running, however the threads are not actually started, therefore we set MAX_QORE_THREADS to 2560 on 
// Darwin.  This should be much more than any program/script should need (famous last words? :-) )
#warning Darwin cannot support more than 2560 threads, MAX_QORE_THREADS set to 2560
#undef MAX_QORE_THREADS
#define MAX_QORE_THREADS 2560
#endif

class Operator *OP_BACKGROUND;

ThreadCleanupList tclist;

// default thread creation attribute
pthread_attr_t ta_default;
// TID 0 is reserved for the signal handler thread
static int     current_tid = 1;

DLLLOCAL bool threads_initialized = false;

DLLLOCAL QoreThreadLock lThreadList;

// recursive mutex attribute
DLLLOCAL pthread_mutexattr_t ma_recursive;

#ifndef HAVE_GETHOSTBYNAME_R
DLLLOCAL class QoreThreadLock lck_gethostbyname;
#endif

#ifndef HAVE_GETHOSTBYADDR_R
DLLLOCAL class QoreThreadLock lck_gethostbyaddr;
#endif

// total number of threads running
DLLLOCAL int num_threads = 0;

// this structure holds all thread data that can be addressed with the qore tid
class ThreadEntry {
public:
   pthread_t ptid;
   class tid_node *tidnode;
#ifdef DEBUG
   class CallStack *callStack;
#endif
   
   DLLLOCAL void cleanup();
};

DLLLOCAL class ThreadEntry thread_list[MAX_QORE_THREADS];

class tid_node {
public:
   int tid;
   class tid_node *next, *prev;
   
   DLLLOCAL tid_node(int ntid);
   DLLLOCAL ~tid_node();
};

static tid_node *tid_head = 0, *tid_tail = 0;
 
class ProgramLocation {
   public:
      const char *file;
      void *parseState;
      class ProgramLocation *next;
      
      DLLLOCAL ProgramLocation(const char *fname, void *ps = 0) 
      { 
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

      DLLLOCAL ThreadVariableBlock(struct ThreadVariableBlock *n_prev = 0) : pos(0), prev(n_prev), next(0)
      {
      }
      DLLLOCAL ~ThreadVariableBlock()
      {
      }
};

class ThreadLocalVariableData {
   private:
      ThreadVariableBlock *curr;
      
   public:
      DLLLOCAL ThreadLocalVariableData()
      {
	 curr = new ThreadVariableBlock;
	 //printf("this=%08p: first curr=%08p\n", this, curr);
      }
      DLLLOCAL ~ThreadLocalVariableData()
      {
	 assert(!curr->prev);
	 assert(!curr->pos);
	 //printf("this=%08p: del curr=%08p\n", this, curr);
	 if (curr->next)
	    delete curr->next;
	 delete curr;
      }
      DLLLOCAL LocalVarValue *instantiate()
      {
	 if (curr->pos == QORE_THREAD_STACK_BLOCK)
	 {
	    if (curr->next)
	       curr = curr->next;
	    else
	    {
	       curr->next = new ThreadVariableBlock(curr);
	       //printf("this=%08p: add curr=%08p, curr->next=%08p\n", this, curr, curr->next);
	       curr = curr->next;
	    }
	 }
	 return &curr->lvar[curr->pos++];
      }
      DLLLOCAL void uninstantiate(ExceptionSink *xsink)
      {
	 if (!curr->pos)
	 {
	    if (curr->next)
	    {
	       //printf("this %08p: del curr=%08p, curr->next=%08p\n", this, curr, curr->next);
	       delete curr->next;
	       curr->next = 0;
	    }
	    curr = curr->prev;
	 }
	 curr->lvar[--curr->pos].uninstantiate(xsink);
      }
#ifndef HAVE_UNLIMITED_THREAD_KEYS
      DLLLOCAL LocalVarValue *find(const char *id)
      {
	 ThreadVariableBlock *w = curr;
	 while (true)
	 {
	    int p = w->pos;
	    while (p)
	    {
	       if (w->lvar[--p].id == id && !w->lvar[p].skip)
		  return &w->lvar[p];
	    }
	    w = w->prev;
#ifdef DEBUG
	    if (!w) printd(0, "no local variable '%s' on stack\n", id);
#endif
	    assert(w);
	 }
	 // to avoid a warning
	 return 0;
      }
#endif
};

struct ThreadClosureVariableBlock {
      ClosureVarValue *cvar[QORE_THREAD_STACK_BLOCK];
      int pos;
      ThreadClosureVariableBlock *prev, *next;

      DLLLOCAL ThreadClosureVariableBlock(ThreadClosureVariableBlock *n_prev = 0) : pos(0), prev(n_prev), next(0)
      {
      }

      DLLLOCAL ~ThreadClosureVariableBlock()
      {
      }
};

class ThreadClosureVariableStack {
   private:
      ThreadClosureVariableBlock *curr;

      DLLLOCAL void instantiate(ClosureVarValue *cvar)
      {
	 if (curr->pos == QORE_THREAD_STACK_BLOCK)
	 {
	    if (curr->next)
	       curr = curr->next;
	    else
	    {
	       curr->next = new ThreadClosureVariableBlock(curr);
	       //printf("this=%08p: add curr=%08p, curr->next=%08p\n", this, curr, curr->next);
	       curr = curr->next;
	    }
	 }
	 curr->cvar[curr->pos++] = cvar;
      }
      
   public:
      DLLLOCAL ThreadClosureVariableStack()
      {
	 curr = new ThreadClosureVariableBlock;
	 //printf("this=%08p: first curr=%08p\n", this, curr);
      }

      DLLLOCAL ~ThreadClosureVariableStack()
      {
	 assert(!curr->prev);
	 assert(!curr->pos);
	 //printf("this=%08p: del curr=%08p\n", this, curr);
	 if (curr->next)
	    delete curr->next;
	 delete curr;
      }

      DLLLOCAL ClosureVarValue *instantiate(const char *id, AbstractQoreNode *value)
      {
	 ClosureVarValue *cvar = new ClosureVarValue(id, value);
	 instantiate(cvar);
	 return cvar;
      }

      DLLLOCAL ClosureVarValue *instantiate(const char *id, AbstractQoreNode *vexp, QoreObject *obj)
      {
	 ClosureVarValue *cvar = new ClosureVarValue(id, vexp, obj);
	 instantiate(cvar);
	 return cvar;
      }

      DLLLOCAL void uninstantiate(ExceptionSink *xsink)
      {
	 if (!curr->pos)
	 {
	    if (curr->next)
	    {
	       //printf("this %08p: del curr=%08p, curr->next=%08p\n", this, curr, curr->next);
	       delete curr->next;
	       curr->next = 0;
	    }
	    curr = curr->prev;
	 }
	 curr->cvar[--curr->pos]->deref(xsink);
      }

      DLLLOCAL ClosureVarValue *find(const char *id)
      {
	 ThreadClosureVariableBlock *w = curr;
	 while (true)
	 {
	    int p = w->pos;
	    while (p)
	    {
	       if (w->cvar[--p]->id == id && !w->cvar[p]->skip)
		  return w->cvar[p];
	    }
	    w = w->prev;
#ifdef DEBUG
	    if (!w) printd(0, "no closure variable '%s' on stack\n", id);
#endif
	    assert(w);
	 }
	 // to avoid a warning
	 return 0;
      }
};

// this structure holds all thread-specific data
class ThreadData 
{
   public:
      int tid;
      VLock vlock;     // for deadlock detection
      Context *context_stack;
      ProgramLocation *plStack;
      int parse_line_start, parse_line_end;
      const char *parse_file;
      int pgm_counter_start, pgm_counter_end;
      const char *pgm_file;
      void *parseState;
      VNode *vstack;  // used during parsing (local variable stack)
      CVNode *cvarstack;
      QoreClass *parseClass;
      QoreException *catchException;
      std::list<block_list_t::iterator> on_block_exit_list;
      ThreadResourceList trlist;
      // current function/method name
      const char *current_code;
      // current object context
      QoreObject *current_obj;
      // current program context
      QoreProgram *current_pgm;
      // current argvid
      LocalVar *current_argvid;

      // local variable data slots
      ThreadLocalVariableData lvstack;

      // closure variable stack
      ThreadClosureVariableStack cvstack;

      // current parsing closure environment
      ClosureParseEnvironment *closure_parse_env;

      // current runtime closure environment
      ClosureRuntimeEnvironment *closure_rt_env;

      DLLLOCAL ThreadData(int ptid, QoreProgram *p) : tid(ptid), vlock(ptid), context_stack(0), plStack(0),
						      parse_line_start(0), parse_line_end(0), 
						      parse_file(0), pgm_counter_start(0), pgm_counter_end(0),
						      pgm_file(0), parseState(0), vstack(0), cvarstack(0),
						      parseClass(0), catchException(0), current_code(0),
						      current_obj(0), current_pgm(p), current_argvid(0),
						      closure_parse_env(0), closure_rt_env(0)
      {
      }

      DLLLOCAL ~ThreadData();
};

static QoreThreadLocalStorage<ThreadData> thread_data;

void ThreadEntry::cleanup()
{
   // delete tidnode from tid_list
   delete tidnode;

#ifdef DEBUG   
   // delete call stack
   delete callStack;
#endif
   
   // set ptid to 0
   ptid = 0L;
}   

class ThreadCleanupNode {
public:
   qtdest_t func;
   void *arg;
   class ThreadCleanupNode *next;
};

DLLLOCAL ThreadCleanupNode *ThreadCleanupList::head = 0;

class ThreadParams {
   public:
      AbstractQoreNode *fc;
      int tid;
      QoreProgram *pgm;
   
      DLLLOCAL ThreadParams(AbstractQoreNode *f, int t) 
      { 
	 fc = f; 
	 tid = t;
	 pgm = getProgram();
      } 
};

// this constructor must only be called with the lThreadList lock held
tid_node::tid_node(int ntid)
{
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
tid_node::~tid_node()
{
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
   public:
      QoreObject *obj;
      QoreObject *callobj;
      AbstractQoreNode *fc;
      QoreProgram *pgm;
      int tid;
      int s_line, e_line;
      const char *file;
      bool method_reference;

      DLLLOCAL BGThreadParams(AbstractQoreNode *f, int t, ExceptionSink *xsink)
      { 
	 tid = t;
	 fc = f;
	 pgm = getProgram();
	 get_pgm_counter(s_line, e_line);
	 file = get_pgm_file();

	 obj = 0;
	 // get and reference the current stack object, if any, for the new call stack
	 callobj = getStackObject();

	 qore_type_t fctype = fc->getType();
	 if (callobj && fctype == NT_FUNCTION_CALL && reinterpret_cast<FunctionCallNode *>(fc)->getFunctionType() == FC_SELF) {
	    // we reference the object so it won't go out of scope while the thread is running
	    obj = callobj;
	    obj->ref();
	 }
	 else if (fctype == NT_TREE) {
	    QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(fc);
	    if (tree->op == OP_OBJECT_FUNC_REF) {
	       // evaluate object
	       AbstractQoreNode *n = tree->left->eval(xsink);
	       if (!n || xsink->isEvent())
		  return;

	       tree->left->deref(xsink);
	       tree->left = n;
	       if (n->getType() == NT_OBJECT) {
		  obj = reinterpret_cast<QoreObject *>(n);
		  obj->ref();
	       }
	    }
	 }
 
	 if (callobj)
	    callobj->tRef();

	 // increment the program's thread counter
	 pgm->tc_inc();
      }
      DLLLOCAL ~BGThreadParams()
      {
	 // decrement program's thread count
	 pgm->tc_dec();
      }
      DLLLOCAL void cleanup(ExceptionSink *xsink)
      {
	 if (fc) fc->deref(xsink);
	 derefObj(xsink);
	 derefCallObj();
      }
      DLLLOCAL void derefCallObj()
      {
	 // dereference call object if present
	 if (callobj) {
	    callobj->tDeref();
	    callobj = 0;
	 }
      }
      DLLLOCAL void derefObj(ExceptionSink *xsink)
      {
	 if (obj) {
	    obj->deref(xsink);
	    obj = 0;
	 }
      }
      DLLLOCAL AbstractQoreNode *exec(ExceptionSink *xsink)
      {
	 AbstractQoreNode *rv = fc->eval(xsink);
	 fc->deref(xsink);
	 fc = 0;
	 return rv;
      }
};

ThreadCleanupList::ThreadCleanupList()
{
   //printf("ThreadCleanupList::ThreadCleanupList() head=NULL\n");

   head = 0;
}

ThreadCleanupList::~ThreadCleanupList()
{
   //printf("ThreadCleanupList::~ThreadCleanupList() head=%08p\n", head);

   while (head)
   {
      class ThreadCleanupNode *w = head->next;
      delete head;
      head = w;
   }
}

void ThreadCleanupList::exec()
{
   class ThreadCleanupNode *w = head;
   while (w)
   {
      w->func(w->arg);
      w = w->next;
   }
}

void ThreadCleanupList::push(qtdest_t func, void *arg)
{
   class ThreadCleanupNode *w = new ThreadCleanupNode;
   w->next = head;
   w->func = func;
   w->arg = arg;
   head = w;
   //printf("TCL::push() this=%08p, &head=%08p, head=%08p, head->next=%08p\n", this, &head, head, head->next);
}

void ThreadCleanupList::pop(bool exec)
{
   //printf("TCL::pop() this=%08p, &head=%08p, head=%08p\n", this, &head, head);
   // NOTE: if exit() is called, then somehow head = 0 !!!
   // I can't explain it, but that's why the if statement is there... :-(
   if (head) {
      if (exec)
	 head->func(head->arg);
      class ThreadCleanupNode *w = head->next;
      delete head;
      head = w;
   }
}

ThreadData::~ThreadData()
{
   assert(on_block_exit_list.empty());
}

LocalVarValue *thread_instantiate_lvar()
{
   return thread_data.get()->lvstack.instantiate();
}

void thread_uninstantiate_lvar(ExceptionSink *xsink)
{
   ThreadData *td = thread_data.get();
   td->lvstack.uninstantiate(xsink);
}

LocalVarValue *thread_find_lvar(const char *id)
{
   ThreadData *td = thread_data.get();
   return td->lvstack.find(id);
}

ClosureVarValue *thread_instantiate_closure_var(const char *n_id, AbstractQoreNode *value)
{
   return thread_data.get()->cvstack.instantiate(n_id, value);
}

ClosureVarValue *thread_instantiate_closure_var(const char *n_id, AbstractQoreNode *vexp, QoreObject *obj)
{
   return thread_data.get()->cvstack.instantiate(n_id, vexp, obj);
}

void thread_uninstantiate_closure_var(ExceptionSink *xsink)
{
   thread_data.get()->cvstack.uninstantiate(xsink);
}

ClosureVarValue *thread_find_closure_var(const char *id)
{
   return thread_data.get()->cvstack.find(id);
}

ClosureRuntimeEnvironment *thread_get_runtime_closure_env()
{
   return thread_data.get()->closure_rt_env;
}

void thread_set_runtime_closure_env(ClosureRuntimeEnvironment *cenv)
{
   thread_data.get()->closure_rt_env = cenv;
}

void thread_set_closure_parse_env(ClosureParseEnvironment *cenv)
{
   thread_data.get()->closure_parse_env = cenv;
}

ClosureVarValue *thread_get_runtime_closure_var(const LocalVar *id)
{
   return thread_data.get()->closure_rt_env->find(id);
}

ClosureParseEnvironment *thread_get_closure_parse_env()
{
   return thread_data.get()->closure_parse_env;
}

void set_thread_resource(AbstractThreadResource *atr)
{
   ThreadData *td = thread_data.get();
   td->trlist.set(atr);
}

int remove_thread_resource(class AbstractThreadResource *atr)
{
   ThreadData *td = thread_data.get();
   return td->trlist.remove(atr);
}

void purge_thread_resources(ExceptionSink *xsink)
{
   ThreadData *td = thread_data.get();
   td->trlist.purge(xsink);
}

// called when a StatementBlock has "on block exit" blocks
void pushBlock(block_list_t::iterator i)
{
   ThreadData *td = thread_data.get();
   td->on_block_exit_list.push_back(i);
}

// called when a StatementBlock has "on block exit" blocks
block_list_t::iterator popBlock()
{
   ThreadData *td = thread_data.get();
   block_list_t::iterator i = td->on_block_exit_list.back();
   td->on_block_exit_list.pop_back();
   return i;
}

// called by each "on_block_exit" statement to activate its code for the block exit
void advanceOnBlockExit()
{
   ThreadData *td = thread_data.get();
   --td->on_block_exit_list.back();
}

// new file name, current parse state
void beginParsing(char *file, void *ps)
{
   ThreadData *td = thread_data.get();
   
   //printd(5, "beginParsing() of %08p (%s), (stack=%s)\n", file, file ? file : "null", (td->plStack ? td->plStack->file : "NONE"));
   
   // if current position exists, then save
   if (td->parse_file) {
      class ProgramLocation *pl = new ProgramLocation(td->parse_file, td->parseState);
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

void *endParsing()
{
   ThreadData *td = thread_data.get();
   void *rv = td->parseState;
   
   printd(5, "endParsing() ending parsing of \"%s\", returning %08p\n", td->parse_file, rv);
   if (td->plStack) {
      class ProgramLocation *pl = td->plStack->next;
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
int gettid()
{
   return (thread_data.get())->tid;
}

class VLock *getVLock()
{
   ThreadData *td = thread_data.get();
   return &td->vlock;
}

Context *get_context_stack()
{
   return (thread_data.get())->context_stack;
}

void update_context_stack(Context *cstack)
{
   ThreadData *td    = thread_data.get();
   td->context_stack = cstack;
}

void get_pgm_counter(int &start_line, int &end_line)
{
   ThreadData *td = thread_data.get();
   start_line = td->pgm_counter_start;
   end_line = td->pgm_counter_end;
}

void update_pgm_counter_pgm_file(int start_line, int end_line, const char *f)
{
   ThreadData *td  = thread_data.get();
   td->pgm_counter_start = start_line;
   td->pgm_counter_end   = end_line;
   td->pgm_file          = f;
}

void update_pgm_counter(int start_line, int end_line)
{
   ThreadData *td  = thread_data.get();
   td->pgm_counter_start = start_line;
   td->pgm_counter_end   = end_line;
}

const char *get_pgm_file()
{
   return (thread_data.get())->pgm_file;
}

void get_parse_location(int &start_line, int &end_line)
{
   ThreadData *td = thread_data.get();
   start_line = td->parse_line_start;
   end_line = td->parse_line_end;
}

void update_parse_location(int start_line, int end_line, const char *f)
{
   ThreadData *td  = thread_data.get();
   td->parse_line_start = start_line;
   td->parse_line_end   = end_line;
   td->parse_file       = f;
}

void update_parse_location(int start_line, int end_line)
{
   ThreadData *td  = thread_data.get();
   td->parse_line_start = start_line;
   td->parse_line_end   = end_line;
}

const char *get_parse_file()
{
   return (thread_data.get())->parse_file;
}

ObjectSubstitutionHelper::ObjectSubstitutionHelper(QoreObject *obj)
{
   ThreadData *td  = thread_data.get();
   old_obj = td->current_obj;
   td->current_obj = obj;
}

ObjectSubstitutionHelper::~ObjectSubstitutionHelper()
{
   ThreadData *td  = thread_data.get();
   td->current_obj = old_obj;
}

CodeContextHelper::CodeContextHelper(const char *code, const QoreObject *obj, ExceptionSink *xs)
{
   ThreadData *td  = thread_data.get();
   old_code = td->current_code;
   old_obj = td->current_obj;
   xsink = xs;
   if (obj)
      obj->ref();
   td->current_code = code;
   td->current_obj = const_cast<QoreObject *>(obj);
   //printd(5, "CodeContextHelper::CodeContextHelper(%s, %08p) this=%08p, old=%s, %08p\n", code ? code : "null", obj, this, old_code ? old_code : "null", old_obj);
}

CodeContextHelper::~CodeContextHelper()
{
   ThreadData *td  = thread_data.get();
   //printd(5, "CodeContextHelper::~CodeContextHelper() this=%08p current=(%s, %08p) restoring %s, %08p\n", this, td->current_code ? td->current_code : "null", td->current_obj, old_code ? old_code : "null", old_obj);
   if (td->current_obj)
      td->current_obj->deref(xsink);
   td->current_code = old_code;
   td->current_obj = old_obj;
}

ArgvContextHelper::ArgvContextHelper(LocalVar *argvid)
{
   ThreadData *td  = thread_data.get();
   old_argvid = td->current_argvid;
   td->current_argvid = argvid;
}

ArgvContextHelper::~ArgvContextHelper()
{
   ThreadData *td  = thread_data.get();
   td->current_argvid = old_argvid;
}

#ifdef DEBUG
void pushCall(class CallNode *cn)
{
   thread_list[gettid()].callStack->push(cn);
}

void popCall(ExceptionSink *xsink)
{
   thread_list[gettid()].callStack->pop(xsink);
}

QoreListNode *getCallStackList()
{
   return thread_list[gettid()].callStack->getCallStack();
}

class CallStack *getCallStack()
{
   return thread_list[gettid()].callStack;
}
#endif

bool inMethod(const char *name, const QoreObject *o)
{
   ThreadData *td = thread_data.get();
   if (td->current_obj == o && td->current_code == name)
      return true;
   return false;
}

QoreObject *getStackObject()
{
   return (thread_data.get())->current_obj;
}

ProgramContextHelper::ProgramContextHelper(QoreProgram *pgm)
{
   assert(pgm);
   old_pgm = 0;
   restore = false;
   if (pgm) {
      ThreadData *td = thread_data.get();
      if (pgm != td->current_pgm) {
	 restore = true;
	 old_pgm = td->current_pgm;
	 td->current_pgm = pgm;
      }
   }
}

ProgramContextHelper::~ProgramContextHelper()
{
   if (restore)
      (thread_data.get())->current_pgm = old_pgm;
}

QoreProgram *getProgram()
{
   return (thread_data.get())->current_pgm;
   //return (thread_data.get())->pgmStack->getProgram();
}

class RootQoreNamespace *getRootNS()
{
   return (thread_data.get())->current_pgm->getRootNS();
   //return (thread_data.get())->pgmStack->getProgram()->getRootNS();
}

int getParseOptions()
{
   return (thread_data.get())->current_pgm->getParseOptions();
   //return (thread_data.get())->pgmStack->getProgram()->getParseOptions();
}

void updateCVarStack(class CVNode *ncvs)
{
   ThreadData *td = thread_data.get();
   td->cvarstack = ncvs;
}

class CVNode *getCVarStack()
{
   return (thread_data.get())->cvarstack;
}

void updateVStack(class VNode *nvs)
{
   ThreadData *td = thread_data.get();
   td->vstack = nvs;
}

class VNode *getVStack()
{
   return (thread_data.get())->vstack;
}

void setParseClass(class QoreClass *c)
{
   ThreadData *td = thread_data.get();
   td->parseClass = c;
}

class QoreClass *getParseClass()
{
   return (thread_data.get())->parseClass;
}

// to save the exception for "rethrow"
void catchSaveException(class QoreException *e)
{
   ThreadData *td = thread_data.get();
   //printd(5, "cSE() td=%08p e=%08p\n", td, e);
   td->catchException = e;
}

// for "rethrow"
class QoreException *catchGetException()
{
   ThreadData *td = thread_data.get();
   //printd(5, "cGE() td=%08p e=%08p\n", td, td->catchException);
   assert(td->catchException);
   return td->catchException;
}

// must be called in the thread list lock
static void allocate_thread_entry(int tid)
{
   thread_list[tid].ptid = (pthread_t)-1L;
#ifdef DEBUG
   thread_list[tid].callStack = 0;
#endif
}

// sets up the signal thread entry in the thread list
int get_signal_thread_entry()
{
   lThreadList.lock();
   allocate_thread_entry(0);
   thread_list[0].tidnode = 0;
   lThreadList.unlock();
   return 0;
}

// returns tid allocated for thread
int get_thread_entry()
{
   int tid = -1;

   lThreadList.lock();
   if (current_tid == MAX_QORE_THREADS) {
      int i;
      // scan thread_list for free entry
      for (i = 1; i < MAX_QORE_THREADS; i++) {
	 if (!thread_list[i].ptid) {
	    tid = i;
	    goto finish;
	 }
      }
      if (i == MAX_QORE_THREADS) {
	 lThreadList.unlock();
	 return -1;
      }
   }
   else
      tid = current_tid++;

  finish:
   allocate_thread_entry(tid);
   thread_list[tid].tidnode = new tid_node(tid);
   num_threads++;
   lThreadList.unlock();
   //printf("t%d cs=0\n", tid);

   return tid;
}

void delete_thread_data()
{
   delete thread_data.get();
}

void deregister_thread(int tid)
{
   // NOTE: cannot safely call printd here, because normally the thread_data has been deleted
   lThreadList.lock();

   thread_list[tid].cleanup();
   num_threads--;

   lThreadList.unlock();
}

void deregister_signal_thread()
{
   // NOTE: cannot safely call printd here, because normally the thread_data has been deleted
   lThreadList.lock();
   thread_list[0].cleanup();
   lThreadList.unlock();
}

// should only be called from new thread
void register_thread(int tid, pthread_t ptid, QoreProgram *p)
{
   thread_list[tid].ptid = ptid;
#ifdef DEBUG
   thread_list[tid].callStack = new CallStack();
#endif
   thread_data.set(new ThreadData(tid, p));
}

// put "op_background_thread" in an unnamed namespace to make it 'static extern "C"'
namespace {
   extern "C" void *op_background_thread(class BGThreadParams *btp)
   {    
      // register thread
      register_thread(btp->tid, pthread_self(), btp->pgm);
      printd(5, "op_background_thread() btp=%08p TID %d started\n", btp, btp->tid);
      //printf("op_background_thread() btp=%08p TID %d started\n", btp, btp->tid);

      // create thread-local data for this thread in the program object
      btp->pgm->startThread();
      // set program counter for new thread
      update_pgm_counter_pgm_file(btp->s_line, btp->e_line, btp->file);

      ExceptionSink xsink;
      AbstractQoreNode *rv;
      {
	 CodeContextHelper cch(0, btp->callobj, &xsink);
#ifdef DEBUG
	 // push this call on the thread stack
	 CallStackHelper csh("background operator", CT_NEWTHREAD, btp->callobj, &xsink);
#endif
	 
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

      delete btp;

      pthread_exit(0);
      return 0;
   }
}

static AbstractQoreNode *op_background(const AbstractQoreNode *left, const AbstractQoreNode *ignored, bool ref_rv, ExceptionSink *xsink)
{
   if (!left)
      return 0;

   //printd(2, "op_background() before crlr left = %08p\n", left);
   AbstractQoreNode *nl = copy_and_resolve_lvar_refs(left, xsink);
   //printd(2, "op_background() after crlr nl = %08p\n", nl);
   if (xsink->isEvent()) {
      if (nl) nl->deref(xsink);
      return 0;
   }
   if (!nl)
      return 0;

   // now we are ready to create the new thread

   // get thread entry
   //printd(2, "calling get_thread_entry()\n");
   int tid = get_thread_entry();
   //printd(2, "got %d()\n", tid);

   // if can't start thread, then throw exception
   if (tid == -1) {
      if (nl) nl->deref(xsink);
      xsink->raiseException("THREAD-CREATION-FAILURE", "thread list is full with %d threads", MAX_QORE_THREADS);
      return 0;
   }

   //printd(2, "creating BGThreadParams(%08p, %d)\n", nl, tid);
   BGThreadParams *tp = new BGThreadParams(nl, tid, xsink);
   if (xsink->isEvent()) {
      if (nl) nl->deref(xsink);
      deregister_thread(tid);
      return 0;
   }
   //printd(2, "tp = %08p\n", tp);
   // create thread
   pthread_t ptid;
   int rc;
   //printd(2, "calling pthread_create(%08p, %08p, %08p, %08p)\n", &ptid, &ta_default, op_background_thread, tp);
   if ((rc = pthread_create(&ptid, &ta_default, (void *(*)(void *))op_background_thread, tp))) {
      tp->cleanup(xsink);
      delete tp;

      deregister_thread(tid);
      xsink->raiseException("THREAD-CREATION-FAILURE", "could not create thread: %s", strerror(rc));
      return 0;
   }
   //printd(5, "pthread_create() new thread TID %d, pthread_create() returned %d\n", tid, rc);

   return ref_rv ? new QoreBigIntNode(tid) : 0;
}

void init_qore_threads()
{
   tracein("qore_init_threads()");

   // setup parent thread data
   register_thread(get_thread_entry(), pthread_self(), 0);

   // register "background" Operator.handler
   OP_BACKGROUND = oplist.add(new Operator(1, "background", "run in background thread", 0, true));
   OP_BACKGROUND->addFunction(NT_ALL, NT_NONE, op_background);

   // initialize default thread creation attribute
   pthread_attr_init(&ta_default);
   pthread_attr_setdetachstate(&ta_default, PTHREAD_CREATE_DETACHED);

   // initialize recursive mutex attribute
   pthread_mutexattr_init(&ma_recursive);
   pthread_mutexattr_settype(&ma_recursive, PTHREAD_MUTEX_RECURSIVE);

#ifdef DEBUG
   // mark threading as active
   threads_initialized = true;
#endif

   traceout("qore_init_threads()");
}

class QoreNamespace *get_thread_ns()
{
   // create Qore::Thread namespace
   class QoreNamespace *Thread = new QoreNamespace("Thread");

   Thread->addSystemClass(initQueueClass());
   Thread->addSystemClass(initMutexClass());
   Thread->addSystemClass(initRMutexClass());
   Thread->addSystemClass(initConditionClass());
   Thread->addSystemClass(initRWLockClass());
   Thread->addSystemClass(initGateClass());
   Thread->addSystemClass(initSequenceClass());
   Thread->addSystemClass(initCounterClass());

   return Thread;
}

void delete_qore_threads()
{
#ifdef DEBUG
   // mark threading as inactive
   threads_initialized = false;
#endif
   tracein("delete_qore_threads()");

   ExceptionSink xsink;
   purge_thread_resources(&xsink);
   xsink.handleExceptions();

   pthread_mutexattr_destroy(&ma_recursive);

   //printd(2, "calling pthread_attr_destroy(%08p)\n", &ta_default);
   pthread_attr_destroy(&ta_default);
   //printd(2, "returned from pthread_attr_destroy(%08p)\n", &ta_default);

   delete_thread_data();

   thread_list[1].cleanup();

   traceout("delete_qore_threads()");
}

QoreListNode *get_thread_list()
{
   QoreListNode *l = new QoreListNode();
   lThreadList.lock();
   tid_node *w = tid_head;
   while (w) {
      l->push(new QoreBigIntNode(w->tid));
      w = w->next;
   }
   lThreadList.unlock();
   return l;
}

#ifdef DEBUG
QoreHashNode *getAllCallStacks()
{
   QoreHashNode *h = new QoreHashNode();
   QoreString str;
   lThreadList.lock();
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
   lThreadList.unlock();
   return h;
}
#endif
