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
#include <qore/CallStack.h>
#include <qore/ArgvStack.h>
#include <qore/QoreProgramStack.h>
#include <qore/VLock.h>

// to register object types
#include <qore/QC_Queue.h>
#include <qore/QC_Mutex.h>
#include <qore/QC_Condition.h>
#include <qore/QC_RWLock.h>
#include <qore/QC_Gate.h>
#include <qore/QC_Sequence.h>
#include <qore/QC_Counter.h>
#include <qore/QC_RMutex.h>

#include <pthread.h>
#include <sys/time.h>
#include <assert.h>

#if defined(DARWIN) && MAX_QORE_THREADS > 2560
#warning Darwin cannot support more than 2560 threads, MAX_QORE_THREADS set to 2560
#undef MAX_QORE_THREADS
#define MAX_QORE_THREADS 2560
#endif

class Operator *OP_BACKGROUND;

ThreadCleanupList tclist;
ThreadResourceList trlist;

// default thread creation attribute
static pthread_attr_t ta_default;
static int      current_tid = 0;

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
   class CallStack *callStack;
   
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
      char *file;
      void *parseState;
      class ProgramLocation *next;
      
      DLLLOCAL ProgramLocation(char *fname, void *ps = NULL) 
      { 
	 file       = fname; 
	 parseState = ps;
      }
};

// this structure holds all thread-specific data
class ThreadData 
{
   public:
      int tid;
      class VLock vlock;     // for deadlock detection
      class LVar *lvstack;
      class Context *context_stack;
      class ArgvStack *argvstack;
      class QoreProgramStack *pgmStack;
      class ProgramLocation *plStack;
      int parse_line_start, parse_line_end;
      char *parse_file;
      int pgm_counter_start, pgm_counter_end;
      char *pgm_file;
      void *parseState;
      class VNode *vstack;  // used during parsing (local variable stack)
      class CVNode *cvarstack;
      class QoreClass *parseClass;
      class Exception *catchException;
      
      DLLLOCAL ThreadData(int ptid, QoreProgram *p);
      DLLLOCAL ~ThreadData();
};

void ThreadEntry::cleanup()
{
   // delete tidnode from tid_list
   delete tidnode;
   
   // delete call stack
   if (callStack)
      delete callStack;
   
   // set ptid to 0
   ptid = 0L;
}   

class ThreadCleanupNode {
public:
   qtdest_t func;
   void *arg;
   class ThreadCleanupNode *next;
};

class ThreadResourceNode
{
public:
   void *key;
   qtrdest_t func;
   int tid;
   class ThreadResourceNode *next, *prev;
   
   DLLLOCAL ThreadResourceNode(void *k, qtrdest_t f);
   DLLLOCAL void call(class ExceptionSink *xsink);
};

DLLLOCAL ThreadCleanupNode *ThreadCleanupList::head = NULL;

ThreadResourceNode::ThreadResourceNode(void *k, qtrdest_t f) : key(k), func(f), tid(gettid()), prev(NULL)
{
}

void ThreadResourceNode::call(class ExceptionSink *xsink)
{
   func(key, xsink);
}

ThreadResourceList::ThreadResourceList()
{
   head = NULL;
}

class ThreadParams {
   public:
      class QoreNode *fc;
      int tid;
      class QoreProgram *pgm;
   
      DLLLOCAL ThreadParams(class QoreNode *f, int t) 
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
      class Object *obj;
      class Object *callobj;
      class QoreNode *fc;
      class QoreProgram *pgm;
      int tid;
      int s_line, e_line;
      char *file;
      bool method_reference;

      DLLLOCAL BGThreadParams(class QoreNode *f, int t, class ExceptionSink *xsink)
      { 
	 tid = t;
	 fc = f;
	 pgm = getProgram();
	 get_pgm_counter(s_line, e_line);
	 file = get_pgm_file();

	 obj = NULL;
	 // get and reference the current stack object, if any, for the new call stack
	 callobj = getStackObject();

	 if (callobj && fc->type == NT_FUNCTION_CALL && fc->val.fcall->type == FC_SELF)
	 {
	    // we reference the object so it won't go out of scope while the thread is running
	    obj = callobj;
	    obj->ref();
	 }
	 else if (fc->type == NT_TREE && fc->val.tree->op == OP_OBJECT_FUNC_REF)
	 {
	    // evaluate object
	    class QoreNode *n = fc->val.tree->left->eval(xsink);
	    if (!n || xsink->isEvent())
	       return;
	    
	    fc->val.tree->left->deref(xsink);
	    fc->val.tree->left = n;
	    if (n->type == NT_OBJECT)
	    {
	       obj = n->val.object;
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
      DLLLOCAL void cleanup(class ExceptionSink *xsink)
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
      DLLLOCAL void derefObj(class ExceptionSink *xsink)
      {
	 if (obj)
	 {
	    obj->dereference(xsink);
	    obj = NULL;
	 }
      }
      DLLLOCAL class QoreNode *exec(class ExceptionSink *xsink)
      {
	 class QoreNode *rv = fc->eval(xsink);
	 fc->deref(xsink);
	 fc = NULL;
	 return rv;
      }
};

inline ThreadResourceList::~ThreadResourceList()
{
   assert(!head);
}

inline class ThreadResourceNode *ThreadResourceList::find(void *key)
{
   class ThreadResourceNode *w = head;
   while (w)
   {
      if (w->key == key)
	 return w;
      w = w->next;
   }
   return NULL;
}

inline class ThreadResourceNode *ThreadResourceList::find(void *key, int tid)
{
   class ThreadResourceNode *w = head;
   while (w)
   {
      if (w->key == key && w->tid == tid)
	 return w;
      w = w->next;
   }
   return NULL;
}

void ThreadResourceList::setIntern(class ThreadResourceNode *n)
{
   n->next = head;
   if (head)
      head->prev = n;
   head = n;
   printd(5, "TRL::setIntern(key=%08p, func=%08p) head=%08p\n", n->key, n->func, head);
}

void ThreadResourceList::set(void *key, qtrdest_t func)
{
   //printd(5, "TRL::set(key=%08p, func=%08p, tid=%d)\n", key, func, gettid());
   class ThreadResourceNode *n = new ThreadResourceNode(key, func);
   lock();
   assert(!find(key, gettid()));
   setIntern(n);
   //printd(5, "TRL::set(key=%08p, func=%08p, tid=%d) n=%08p, head=%08p, head->next=%08p\n", key, func, gettid(), n, head, head->next);
   unlock();
}

inline int ThreadResourceList::setOnce(void *key, qtrdest_t func)
{
   int rc = 0;
   lock();
   if (find(key))
      rc = -1;
   else
      setIntern(new ThreadResourceNode(key, func));
   unlock();
   return rc;
}

inline void ThreadResourceList::removeIntern(class ThreadResourceNode *w)
{
   //printd(5, "removeIntern(%08p) starting (head=%08p)\n", w, head);
   if (w->prev)
      w->prev->next = w->next;
   else
      head = w->next;
   if (w->next)
      w->next->prev = w->prev;
   //printd(5, "removeIntern(%08p) done (head=%08p)\n", w, head);
}

void ThreadResourceList::purgeTID(int tid, class ExceptionSink *xsink)
{
   // we put all the nodes in a temporary list and then run them from there
   class ThreadResourceList trl;

   lock();
   //printd(5, "purgeTID(%d) head=%08p\n", tid, head);
   class ThreadResourceNode *w = head;
   while (w)
   {
      printd(5, "TRL::purgeTID(%d) w->tid=%d, w->key=%08p, w->next=%08p\n", tid, w->tid, w->key, w->next);
      if (w->tid == tid)
      {
	 class ThreadResourceNode *n = w->next;
	 removeIntern(w);
	 w->prev = NULL;
	 trl.setIntern(w);
	 w = n;
      }
      else
	 w = w->next;
   }   
   unlock();

   if (trl.head)
   {
      class ThreadResourceNode *w = trl.head;
      while (w)
      {
	 w->call(xsink);
	 class ThreadResourceNode *n = w->next;
	 delete w;
	 w = n;
      }
#ifdef DEBUG
      trl.head = NULL;
#endif
   }

   //printd(5, "TRL::purgeTID() done\n");
}

void ThreadResourceList::remove(void *key)
{
   //printd(0, "TRL::remove(key=%08p)\n", key);
   lock();
   class ThreadResourceNode *w = head;
   while (w)
   {
      if (w->key == key)
      {
	 class ThreadResourceNode *n = w->next;
	 removeIntern(w);
	 delete w;
	 w = n;
      }
      else
	 w = w->next;
   }
   unlock();
}

// there must be only one of these
void ThreadResourceList::remove(void *key, int tid)
{
   lock();
   //printd(5, "TRL::remove(key=%08p, tid=%d) head=%08p\n", key, tid, head);
   class ThreadResourceNode *w = head;
   while (w)
   {
      //printd(5, "TRL::remove(key=%08p, tid=%d) w=%08p key=%08p, tid=%d\n", key, tid, w, w->key, w->tid);
      if (w->key == key && w->tid == tid)
      {
	 removeIntern(w);
	 delete w;
	 unlock();
	 return;
      }
      w = w->next;
   }
   unlock();
   assert(false);
}

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

ThreadData::ThreadData(int ptid, class QoreProgram *p)
{
   tid               = ptid;
   lvstack           = NULL;
   context_stack     = NULL;
   argvstack         = NULL;
   pgm_counter_start = 0;
   pgm_counter_end   = 0;
   parse_line_start  = 0;
   parse_line_end    = 0;
   pgm_file          = NULL;
   parse_file        = NULL;
   pgmStack          = new QoreProgramStack(p);
   plStack           = NULL;
   parseState        = NULL;
   vstack            = NULL;
   cvarstack         = NULL;
   parseClass        = NULL;
}

ThreadData::~ThreadData()
{
   delete pgmStack;
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
   pthread_setspecific(thread_data_key, td);
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
   pthread_setspecific(thread_data_key, td);
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

class LVar *get_thread_stack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->lvstack;
}

void update_thread_stack(class LVar *lvstack)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->lvstack    = lvstack;
   pthread_setspecific(thread_data_key, td);
}

Context *get_context_stack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->context_stack;
}

void update_context_stack(Context *cstack)
{
   ThreadData *td    = (ThreadData *)pthread_getspecific(thread_data_key);
   td->context_stack = cstack;
   pthread_setspecific(thread_data_key, td);
}

class ArgvStack *get_argvstack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->argvstack;
}

void update_argvstack(ArgvStack *as)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->argvstack  = as;
   pthread_setspecific(thread_data_key, td);
}

void get_pgm_counter(int &start_line, int &end_line)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   start_line = td->pgm_counter_start;
   end_line = td->pgm_counter_end;
}

void update_pgm_counter_pgm_file(int start_line, int end_line, char *f)
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   td->pgm_counter_start = start_line;
   td->pgm_counter_end   = end_line;
   td->pgm_file          = f;
   pthread_setspecific(thread_data_key, td);
}

void update_pgm_counter(int start_line, int end_line)
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   td->pgm_counter_start = start_line;
   td->pgm_counter_end   = end_line;
   pthread_setspecific(thread_data_key, td);
}

char *get_pgm_file()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->pgm_file;
}

void get_parse_location(int &start_line, int &end_line)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   start_line = td->parse_line_start;
   end_line = td->parse_line_end;
}

void update_parse_location(int start_line, int end_line, char *f)
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   td->parse_line_start = start_line;
   td->parse_line_end   = end_line;
   td->parse_file       = f;
   pthread_setspecific(thread_data_key, td);
}

void update_parse_location(int start_line, int end_line)
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   td->parse_line_start = start_line;
   td->parse_line_end   = end_line;
   pthread_setspecific(thread_data_key, td);
}

char *get_parse_file()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->parse_file;
}

bool inMethod(char *name, class Object *o)
{
   return thread_list[gettid()].callStack->inMethod(name, o);
}

void pushCall(char *f, int type, class Object *o)
{
   thread_list[gettid()].callStack->push(f, type, o);
   //OLD: ((ThreadData *)pthread_getspecific(thread_data_key))->callStack->push(f, type, o);
}

void popCall(class ExceptionSink *xsink)
{
   thread_list[gettid()].callStack->pop(xsink);
   //OLD: ((ThreadData *)pthread_getspecific(thread_data_key))->callStack->pop(xsink);
}

class List *getCallStackList()
{
   return thread_list[gettid()].callStack->getCallStack();
}

class CallStack *getCallStack()
{
   return thread_list[gettid()].callStack;
}

class Object *getStackObject()
{
   return thread_list[gettid()].callStack->getStackObject();
   //OLD: return ((ThreadData *)pthread_getspecific(thread_data_key))->callStack->getStackObject();
}

void pushProgram(class QoreProgram *pgm)
{
   ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->push(pgm);
}

void popProgram()
{
   ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->pop();
}

class QoreProgram *getProgram()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->getProgram();
}

class RootNamespace *getRootNS()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->getProgram()->getRootNS();
}

int getParseOptions()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->getProgram()->getParseOptions();
}

void updateCVarStack(class CVNode *ncvs)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->cvarstack = ncvs;
   pthread_setspecific(thread_data_key, td);
}

class CVNode *getCVarStack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->cvarstack;
}

void updateVStack(class VNode *nvs)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->vstack = nvs;
   pthread_setspecific(thread_data_key, td);
}

class VNode *getVStack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->vstack;
}

void setParseClass(class QoreClass *c)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->parseClass = c;
   pthread_setspecific(thread_data_key, td);
}

class QoreClass *getParseClass()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->parseClass;
}

void substituteObjectIfEqual(class Object *o)
{
   thread_list[gettid()].callStack->substituteObjectIfEqual(o);
}

class Object *substituteObject(class Object *o)
{
   return thread_list[gettid()].callStack->substituteObject(o);
}

// to save the exception for "rethrow"
void catchSaveException(class Exception *e)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->catchException = e;
}

// for "rethrow"
class Exception *catchGetException()
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   return td->catchException;
}

// returns tid allocated for thread
static int get_thread_entry()
{
   int tid = -1;

   lThreadList.lock();
   if (current_tid == MAX_QORE_THREADS)
   {
      int i;
      // scan thread_list for free entry
      for (i = 0; i < MAX_QORE_THREADS; i++)
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
   thread_list[tid].ptid = (pthread_t)-1L;
   thread_list[tid].tidnode = new tid_node(tid);
   thread_list[tid].callStack = NULL;
   num_threads++;
   lThreadList.unlock();
   //printf("t%d cs=0\n", tid);

   return tid;
}

static inline void delete_thread_data()
{
   delete (ThreadData *)pthread_getspecific(thread_data_key);
}

static void deregister_thread(int tid)
{
   // NOTE: cannot safely call printd here, because normally the thread_data has been deleted
   lThreadList.lock();

   thread_list[tid].cleanup();
   num_threads--;

   lThreadList.unlock();
}

// should only be called from new thread
static inline void register_thread(int tid, pthread_t ptid, class QoreProgram *p)
{
   thread_list[tid].ptid = ptid;
#ifdef DEBUG
   class CallStack *cs = new CallStack();
   thread_list[tid].callStack = cs;
   if (!thread_list[tid].callStack)
      printf("ERROR: TID %d: callStack is NULL (%p), thread_list=%p, cs = %p\n", tid, thread_list[tid].callStack, thread_list, cs);
#else
   thread_list[tid].callStack = new CallStack();
#endif
   pthread_setspecific(thread_data_key, (void *)(new ThreadData(tid, p)));
}

static void *op_background_thread(class BGThreadParams *btp)
{    
   // register thread
   register_thread(btp->tid, pthread_self(), btp->pgm);
   printd(5, "op_background_thread() btp=%08p TID %d started\n", btp, btp->tid);
   //printf("op_background_thread() btp=%08p TID %d started\n", btp, btp->tid);

#ifdef DEBUG
   if (!thread_list[btp->tid].callStack)
   {
      printf("TID %d: btp=%p, callstack = NULL, retry\n", btp->tid, btp);
      abort();
   }
#endif
   // create thread-local data for this thread in the program object
   btp->pgm->startThread();
   // set program counter for new thread
   update_pgm_counter_pgm_file(btp->s_line, btp->e_line, btp->file);

   // push this call on the thread stack
   pushCall("background operator", CT_NEWTHREAD, btp->callobj);

   // dereference call object if present
   btp->derefCallObj();

   class ExceptionSink xsink;

   // run thread expression
   class QoreNode *rv = btp->exec(&xsink);

   // if there is an object, we dereference the extra reference here
   btp->derefObj(&xsink);

   // pop the call from the stack
   popCall(&xsink);

   // dereference any return value from the background expression
   if (rv)
      rv->deref(&xsink);

   // delete any thread data
   btp->pgm->endThread(&xsink);
   
   // cleanup thread resources
   trlist.purgeTID(btp->tid, &xsink);

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

static class QoreNode *op_background(class QoreNode *left, class QoreNode *right, bool ref_rv, ExceptionSink *xsink)
{
   if (!left)
      return NULL;

   //printd(2, "op_background() before crlr left = %08p\n", left);
   QoreNode *nl = copy_and_resolve_lvar_refs(left, xsink);
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
      return new QoreNode((int64)tid);
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

class Namespace *get_thread_ns()
{
   // create Qore::Thread namespace
   class Namespace *Thread = new Namespace("Thread");

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
   trlist.purgeTID(0, &xsink);
   xsink.handleExceptions();

   pthread_mutexattr_destroy(&ma_recursive);

   //printd(2, "calling pthread_attr_destroy(%08p)\n", &ta_default);
   pthread_attr_destroy(&ta_default);
   //printd(2, "returned from pthread_attr_destroy(%08p)\n", &ta_default);

   delete_thread_data();

   thread_list[0].cleanup();

   // delete key
   pthread_key_delete(thread_data_key);

   traceout("delete_qore_threads()");
}

List *get_thread_list()
{
   List *l = new List();
   lThreadList.lock();
   tid_node *w = tid_head;
   while (w)
   {
      l->push(new QoreNode(NT_INT, w->tid));
      w = w->next;
   }
   lThreadList.unlock();
   return l;
}

Hash *getAllCallStacks()
{
   Hash *h = new Hash();
   QoreString str;
   lThreadList.lock();
   tid_node *w = tid_head;
   while (w)
   {
      // get call stack
      if (thread_list[w->tid].callStack)
      {
	 List *l = thread_list[w->tid].callStack->getCallStack();
	 if (l->size())
	 {
	    // make hash entry
	    str.clear();
	    str.sprintf("%d", w->tid);
	    h->setKeyValue(&str, new QoreNode(l), NULL);
	 }
	 else
	    l->derefAndDelete(NULL);
      }
      w = w->next;
   }   
   lThreadList.unlock();
   return h;
}
