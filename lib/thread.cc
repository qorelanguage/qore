/*
  thread.cc

  POSIX thread library for Qore

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
#include <qore/ThreadResourceList.h>
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

DLLLOCAL LockedObject lThreadList;

// recursive mutex attribute
DLLLOCAL pthread_mutexattr_t ma_recursive;

DLLLOCAL pthread_key_t   thread_data_key;

#ifndef HAVE_GETHOSTBYNAME_R
DLLLOCAL class LockedObject lck_gethostbyname;
#endif

#ifndef HAVE_GETHOSTBYADDR_R
DLLLOCAL class LockedObject lck_gethostbyaddr;
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

static tid_node *tid_head = NULL, *tid_tail = NULL;
 
class ProgramLocation {
   public:
      const char *file;
      void *parseState;
      class ProgramLocation *next;
      
      DLLLOCAL ProgramLocation(const char *fname, void *ps = NULL) 
      { 
	 file       = fname; 
	 parseState = ps;
      }
};

struct ThreadVariableBlock {
   class LVar lvar[QORE_THREAD_STACK_BLOCK];
   int pos;
   struct ThreadVariableBlock *prev, *next;

   DLLLOCAL ThreadVariableBlock(struct ThreadVariableBlock *n_prev = 0) : pos(0), prev(n_prev), next(0)
   {
   }
   DLLLOCAL ~ThreadVariableBlock()
   {
   }
};

class ThreadVariableStack {
private:
   ThreadVariableBlock *curr;

public:
   DLLLOCAL ThreadVariableStack()
   {
      curr = new ThreadVariableBlock;
      //printf("this=%08p: first curr=%08p\n", this, curr);
   }
   DLLLOCAL ~ThreadVariableStack()
   {
      assert(!curr->prev);
      assert(!curr->pos);
      //printf("this=%08p: del curr=%08p\n", this, curr);
      if (curr->next)
	 delete curr->next;
      delete curr;
   }
   DLLLOCAL class LVar *instantiate()
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
   DLLLOCAL void uninstantiate()
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
      --curr->pos;
      printd(5, "uninstantiating lvar \"%s\"\n", curr->lvar[curr->pos].id);
   }
   DLLLOCAL void uninstantiate(ExceptionSink *xsink)
   {
      uninstantiate();
      curr->lvar[curr->pos].deref(xsink);
   }
   DLLLOCAL class LVar *find(lvh_t id)
   {
      class ThreadVariableBlock *w = curr;
      while (true)
      {
	 int p = w->pos;
	 while (p)
	 {
	    if (w->lvar[--p].id == id)
	       return &w->lvar[p];
	 }
	 w = w->prev;
	 assert(w);
      }
      // to avoid a warning
      return 0;
   }      
};

// this structure holds all thread-specific data
class ThreadData 
{
   private:
      class ThreadVariableStack lvstack;

   public:
      int tid;
      class VLock vlock;     // for deadlock detection
      class Context *context_stack;
      class ProgramLocation *plStack;
      int parse_line_start, parse_line_end;
      const char *parse_file;
      int pgm_counter_start, pgm_counter_end;
      const char *pgm_file;
      void *parseState;
      class VNode *vstack;  // used during parsing (local variable stack)
      class CVNode *cvarstack;
      class QoreClass *parseClass;
      class QoreException *catchException;
      std::list<block_list_t::iterator> on_block_exit_list;
      class ThreadResourceList trlist;
      // current function/method name
      const char *current_code;
      // current object context
      QoreObject *current_obj;
      // current program context
      QoreProgram *current_pgm;
      // current argvid
      lvh_t current_argvid;

      DLLLOCAL ThreadData(int ptid, QoreProgram *p);
      DLLLOCAL ~ThreadData();
      DLLLOCAL LVar *instantiate_lvar(lvh_t id, AbstractQoreNode *value)
      {
	 LVar *v = lvstack.instantiate();
	 v->set(id, value);
	 return v;
      }
      DLLLOCAL LVar *instantiate_lvar(lvh_t id, AbstractQoreNode *ve, QoreObject *o)
      {
	 LVar *v = lvstack.instantiate();
	 v->set(id, ve, o);
	 return v;
      }
      DLLLOCAL void uninstantiate_lvar(ExceptionSink *xsink)
      {
	 lvstack.uninstantiate(xsink);
      }
      DLLLOCAL void uninstantiate_lvar()
      {
	 lvstack.uninstantiate();
      }
      DLLLOCAL class LVar *find_lvar(lvh_t id)
      {
	 return lvstack.find(id);
      }
};

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

DLLLOCAL ThreadCleanupNode *ThreadCleanupList::head = NULL;

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
   next = NULL;
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

	 obj = NULL;
	 // get and reference the current stack object, if any, for the new call stack
	 callobj = getStackObject();

	 const QoreType *fctype = fc->getType();
	 if (callobj && fctype == NT_FUNCTION_CALL && reinterpret_cast<FunctionCallNode *>(fc)->getFunctionType() == FC_SELF)
	 {
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
	       obj = dynamic_cast<QoreObject *>(n);
	       if (obj)
		  obj->ref();
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
	 if (callobj)
	 {
	    callobj->tDeref();
	    callobj = NULL;
	 }
      }
      DLLLOCAL void derefObj(ExceptionSink *xsink)
      {
	 if (obj)
	 {
	    obj->deref(xsink);
	    obj = NULL;
	 }
      }
      DLLLOCAL AbstractQoreNode *exec(ExceptionSink *xsink)
      {
	 AbstractQoreNode *rv = fc->eval(xsink);
	 fc->deref(xsink);
	 fc = NULL;
	 return rv;
      }
};

ThreadCleanupList::ThreadCleanupList()
{
   //printf("ThreadCleanupList::ThreadCleanupList() head=NULL\n");

   head = NULL;
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

void ThreadCleanupList::pop(int exec)
{
   //printf("TCL::pop() this=%08p, &head=%08p, head=%08p\n", this, &head, head);
   // NOTE: if exit() is called, then somehow head = NULL !!!
   // I can't explain it, but that's why the if statement is there... :-(
   if (head)
   {
      if (exec)
	 head->func(head->arg);
      class ThreadCleanupNode *w = head->next;
      delete head;
      head = w;
   }
}

ThreadData::ThreadData(int ptid, QoreProgram *p) : tid(ptid), vlock(ptid), current_pgm(p)
{
   context_stack     = NULL;
   pgm_counter_start = 0;
   pgm_counter_end   = 0;
   parse_line_start  = 0;
   parse_line_end    = 0;
   pgm_file          = NULL;
   parse_file        = NULL;
   plStack           = NULL;
   parseState        = NULL;
   vstack            = NULL;
   cvarstack         = NULL;
   parseClass        = NULL;
   catchException    = 0;
   current_obj       = 0;
   current_code      = 0;
   current_argvid    = 0;
}

ThreadData::~ThreadData()
{
   assert(on_block_exit_list.empty());
}

void set_thread_resource(class AbstractThreadResource *atr)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->trlist.set(atr);
}

int remove_thread_resource(class AbstractThreadResource *atr)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   return td->trlist.remove(atr);
}

void purge_thread_resources(ExceptionSink *xsink)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->trlist.purge(xsink);
}

// called when a StatementBlock has "on block exit" blocks
void pushBlock(block_list_t::iterator i)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->on_block_exit_list.push_back(i);
}

// called when a StatementBlock has "on block exit" blocks
block_list_t::iterator popBlock()
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   block_list_t::iterator i = td->on_block_exit_list.back();
   td->on_block_exit_list.pop_back();
   return i;
}

// called by each "on_block_exit" statement to activate its code for the block exit
void advanceOnBlockExit()
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   --td->on_block_exit_list.back();
}

// new file name, current parse state
void beginParsing(char *file, void *ps)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   
   //printd(5, "beginParsing() of %08p (%s), (stack=%s)\n", file, file ? file : "null", (td->plStack ? td->plStack->file : "NONE"));
   
   // if current position exists, then save
   if (td->parse_file)
   {
      class ProgramLocation *pl = new ProgramLocation(td->parse_file, td->parseState);
      if (!td->plStack)
      {
	 pl->next = NULL;
	 td->plStack = pl;
      }
      else
      {
	 pl->next = td->plStack;
	 td->plStack = pl;
      }
   }
   td->parse_file = file;
   td->parseState = ps;
}

void *endParsing()
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   void *rv = td->parseState;
   
   printd(5, "endParsing() ending parsing of \"%s\", returning %08p\n", td->parse_file, rv);
   if (td->plStack)
   {
      class ProgramLocation *pl = td->plStack->next;
      td->parse_file  = td->plStack->file;
      td->parseState  = td->plStack->parseState;
      delete td->plStack;
      td->plStack = pl;
   }
   else
   {
      td->parse_file = NULL;
      td->parseState = NULL;
   }
   return rv;
}

// thread-local functions
int gettid()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->tid;
}

class VLock *getVLock()
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   return &td->vlock;
}

class LVar *thread_instantiate_lvar(lvh_t id, AbstractQoreNode *value)
{
   printd(3, "instantiating lvar '%s' by value (val=%08p)\n", id, value);
   return ((ThreadData *)pthread_getspecific(thread_data_key))->instantiate_lvar(id, value);
}

class LVar *thread_instantiate_lvar(lvh_t id, AbstractQoreNode *ve, QoreObject *o)
{
   printd(3, "instantiating lvar %08p '%s' by reference (ve=%08p, o=%08p)\n", id, id, ve, o);
   return ((ThreadData *)pthread_getspecific(thread_data_key))->instantiate_lvar(id, ve, o);
}

void thread_uninstantiate_lvar(ExceptionSink *xsink)
{
   ((ThreadData *)pthread_getspecific(thread_data_key))->uninstantiate_lvar(xsink);
}

void thread_uninstantiate_lvar()
{
   ((ThreadData *)pthread_getspecific(thread_data_key))->uninstantiate_lvar();
}

class LVar *thread_find_lvar(lvh_t id)
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->find_lvar(id);
}

Context *get_context_stack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->context_stack;
}

void update_context_stack(Context *cstack)
{
   ThreadData *td    = (ThreadData *)pthread_getspecific(thread_data_key);
   td->context_stack = cstack;
}

void get_pgm_counter(int &start_line, int &end_line)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   start_line = td->pgm_counter_start;
   end_line = td->pgm_counter_end;
}

void update_pgm_counter_pgm_file(int start_line, int end_line, const char *f)
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   td->pgm_counter_start = start_line;
   td->pgm_counter_end   = end_line;
   td->pgm_file          = f;
}

void update_pgm_counter(int start_line, int end_line)
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   td->pgm_counter_start = start_line;
   td->pgm_counter_end   = end_line;
}

const char *get_pgm_file()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->pgm_file;
}

void get_parse_location(int &start_line, int &end_line)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   start_line = td->parse_line_start;
   end_line = td->parse_line_end;
}

void update_parse_location(int start_line, int end_line, const char *f)
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   td->parse_line_start = start_line;
   td->parse_line_end   = end_line;
   td->parse_file       = f;
}

void update_parse_location(int start_line, int end_line)
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   td->parse_line_start = start_line;
   td->parse_line_end   = end_line;
}

const char *get_parse_file()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->parse_file;
}

ObjectSubstitutionHelper::ObjectSubstitutionHelper(QoreObject *obj)
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   old_obj = td->current_obj;
   td->current_obj = obj;
}

ObjectSubstitutionHelper::~ObjectSubstitutionHelper()
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   td->current_obj = old_obj;
}

CodeContextHelper::CodeContextHelper(const char *code, const QoreObject *obj, ExceptionSink *xs)
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
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
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   //printd(5, "CodeContextHelper::~CodeContextHelper() this=%08p current=(%s, %08p) restoring %s, %08p\n", this, td->current_code ? td->current_code : "null", td->current_obj, old_code ? old_code : "null", old_obj);
   if (td->current_obj)
      td->current_obj->deref(xsink);
   td->current_code = old_code;
   td->current_obj = old_obj;
}

ArgvContextHelper::ArgvContextHelper(lvh_t argvid)
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   old_argvid = td->current_argvid;
   td->current_argvid = argvid;
}

ArgvContextHelper::~ArgvContextHelper()
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   td->current_argvid = old_argvid;
}

#ifdef DEBUG
void pushCall(const char *f, int type, QoreObject *o)
{
   thread_list[gettid()].callStack->push(f, type, o);
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
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   if (td->current_obj == o && td->current_code == name)
      return true;
   return false;
}

QoreObject *getStackObject()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->current_obj;
}

ProgramContextHelper::ProgramContextHelper(QoreProgram *pgm)
{
   old_pgm = 0;
   restore = false;
   if (pgm)
   {
      ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
      if (pgm != td->current_pgm)
      {
	 restore = true;
	 old_pgm = td->current_pgm;
	 td->current_pgm = pgm;
      }
   }
}

ProgramContextHelper::~ProgramContextHelper()
{
   if (restore)
      ((ThreadData *)pthread_getspecific(thread_data_key))->current_pgm = old_pgm;
}

QoreProgram *getProgram()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->current_pgm;
   //return ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->getProgram();
}

class RootQoreNamespace *getRootNS()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->current_pgm->getRootNS();
   //return ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->getProgram()->getRootNS();
}

int getParseOptions()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->current_pgm->getParseOptions();
   //return ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->getProgram()->getParseOptions();
}

void updateCVarStack(class CVNode *ncvs)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->cvarstack = ncvs;
}

class CVNode *getCVarStack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->cvarstack;
}

void updateVStack(class VNode *nvs)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->vstack = nvs;
}

class VNode *getVStack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->vstack;
}

void setParseClass(class QoreClass *c)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->parseClass = c;
}

class QoreClass *getParseClass()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->parseClass;
}

// to save the exception for "rethrow"
void catchSaveException(class QoreException *e)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   //printd(5, "cSE() td=%08p e=%08p\n", td, e);
   td->catchException = e;
}

// for "rethrow"
class QoreException *catchGetException()
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   //printd(5, "cGE() td=%08p e=%08p\n", td, td->catchException);
   assert(td->catchException);
   return td->catchException;
}

// must be called in the thread list lock
static void allocate_thread_entry(int tid)
{
   thread_list[tid].ptid = (pthread_t)-1L;
#ifdef DEBUG
   thread_list[tid].callStack = NULL;
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
   if (current_tid == MAX_QORE_THREADS)
   {
      int i;
      // scan thread_list for free entry
      for (i = 1; i < MAX_QORE_THREADS; i++)
      {
	 if (!thread_list[i].ptid)
	 {
	    tid = i;
	    goto finish;
	 }
      }
      if (i == MAX_QORE_THREADS)
      {
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
   delete (ThreadData *)pthread_getspecific(thread_data_key);
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
   pthread_setspecific(thread_data_key, (void *)(new ThreadData(tid, p)));
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
	 CodeContextHelper cch(NULL, btp->callobj, &xsink);
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

      pthread_exit(NULL);
      return NULL;
   }
}

static AbstractQoreNode *op_background(AbstractQoreNode *left, AbstractQoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   if (!left)
      return NULL;

   //printd(2, "op_background() before crlr left = %08p\n", left);
   AbstractQoreNode *nl = copy_and_resolve_lvar_refs(left, xsink);
   //printd(2, "op_background() after crlr nl = %08p\n", nl);
   if (xsink->isEvent())
   {
      if (nl) nl->deref(xsink);
      return NULL;
   }
   if (!nl)
      return NULL;

   // now we are ready to create the new thread

   // get thread entry
   //printd(2, "calling get_thread_entry()\n");
   int tid = get_thread_entry();
   //printd(2, "got %d()\n", tid);

   // if can't start thread, then throw exception
   if (tid == -1)
   {
      if (nl) nl->deref(xsink);
      xsink->raiseException("THREAD-CREATION-FAILURE", "thread list is full with %d threads", MAX_QORE_THREADS);
      return NULL;
   }

   //printd(2, "creating BGThreadParams(%08p, %d)\n", nl, tid);
   BGThreadParams *tp = new BGThreadParams(nl, tid, xsink);
   if (xsink->isEvent())
   {
      if (nl) nl->deref(xsink);
      deregister_thread(tid);
      return NULL;
   }
   //printd(2, "tp = %08p\n", tp);
   // create thread
   pthread_t ptid;
   int rc;
   //printd(2, "calling pthread_create(%08p, %08p, %08p, %08p)\n", &ptid, &ta_default, op_background_thread, tp);
   if ((rc = pthread_create(&ptid, &ta_default, (void *(*)(void *))op_background_thread, tp)))
   {
      tp->cleanup(xsink);
      delete tp;

      deregister_thread(tid);
      xsink->raiseException("THREAD-CREATION-FAILURE", "could not create thread: %s", strerror(rc));
      return NULL;
   }
   printd(5, "pthread_create() new thread TID %d, pthread_create() returned %d\n", tid, rc);

   printd(5, "create_thread() created thread with TID %d\n", ptid);
   if (ref_rv)
      return new QoreBigIntNode(tid);
   return NULL;
}

void init_qore_threads()
{
   tracein("qore_init_threads()");

   // init thread data key
   pthread_key_create(&thread_data_key, NULL); //thread_data_cleanup);

   // setup parent thread data
   register_thread(get_thread_entry(), pthread_self(), NULL);

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

   // delete key
   pthread_key_delete(thread_data_key);

   traceout("delete_qore_threads()");
}

QoreListNode *get_thread_list()
{
   QoreListNode *l = new QoreListNode();
   lThreadList.lock();
   tid_node *w = tid_head;
   while (w)
   {
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
   while (w)
   {
      // get call stack
      if (thread_list[w->tid].callStack)
      {
	 QoreListNode *l = thread_list[w->tid].callStack->getCallStack();
	 if (l->size())
	 {
	    // make hash entry
	    str.clear();
	    str.sprintf("%d", w->tid);
	    h->setKeyValue(&str, l, NULL);
	 }
	 else
	    l->deref(NULL);
      }
      w = w->next;
   }   
   lThreadList.unlock();
   return h;
}
#endif
