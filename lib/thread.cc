/*
  thread.cc

  POSIX thread library for Qore

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/support.h>
#include <qore/common.h>
#include <qore/thread.h>
#include <qore/QoreNode.h>
#include <qore/QoreType.h>
#include <qore/List.h>
#include <qore/Object.h>
#include <qore/Operator.h>
#include <qore/QoreClass.h>
#include <qore/Variable.h>
#include <qore/QoreProgram.h>
#include <qore/Namespace.h>
#include <qore/LockedObject.h>

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

// FIXME: move to config.h or something like that
// not more than this number of threads can be running at the same time
#define MAX_THREADS 0x1000

#ifdef DEBUG
bool threads_initialized = false;
#endif

class Operator *OP_BACKGROUND;

LockedObject lThreadList;

ThreadCleanupList tclist;
ThreadResourceList trlist;

// default thread creation attribute
static pthread_attr_t ta_default;

// recursive mutex attribute
pthread_mutexattr_t ma_recursive;

static int      max_thread_list = 0;
static int      current_tid = 0;
ThreadEntry    *thread_list = NULL;
pthread_key_t   thread_data_key;

#ifndef HAVE_GETHOSTBYNAME_R
class LockedObject lck_gethostbyname;
#endif

#ifndef HAVE_GETHOSTBYADDR_R
class LockedObject lck_gethostbyaddr;
#endif

// condition var for num_threads (uses lThreadList mutex)
pthread_cond_t  ptc_num_threads;
int             num_threads = 0;

tid_node *tid_head = NULL, *tid_tail = NULL;

#define THREAD_LIST_BLOCK 100

inline ThreadCleanupList::ThreadCleanupList()
{
   //printf("ThreadCleanupList::ThreadCleanupList() head=NULL\n");

   head = NULL;
}

inline ThreadCleanupList::~ThreadCleanupList()
{
   //printf("ThreadCleanupList::~ThreadCleanupList() head=%08x\n", head);

   while (head)
   {
      class ThreadCleanupNode *w = head->next;
      delete head;
      head = w;
   }
}

void ThreadCleanupList::push(qtdest_t func, void *arg)
{
   class ThreadCleanupNode *w = new ThreadCleanupNode;
   w->next = head;
   w->func = func;
   w->arg = arg;
   head = w;
   //printf("TCL::push() this=%08x, &head=%08x, head=%08x, head->next=%08x\n", this, &head, head, head->next);
}

void ThreadCleanupList::pop(int exec)
{
   //printf("TCL::pop() this=%08x, &head=%08x, head=%08x\n", this, &head, head);
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

void ThreadResourceList::setIntern(class ThreadResourceNode *n)
{
   //printd(5, "TRL::setIntern(key=%08x, func=%08x)\n", n->key, n->func);
   n->next = head;
   if (head)
      head->prev = n;
   head = n;
}

void ThreadResourceList::set(void *key, qtrdest_t func)
{
   class ThreadResourceNode *n = new ThreadResourceNode(key, func);
   lock();
   setIntern(n);
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
   //printd(5, "removeIntern(%08x) starting (head=%08x)\n", w, head);
   if (w->prev)
      w->prev->next = w->next;
   else
      head = w->next;
   if (w->next)
      w->next->prev = w->prev;
   //printd(5, "removeIntern(%08x) done (head=%08x)\n", w, head);
}

inline void ThreadResourceList::purgeTID(int tid, class ExceptionSink *xsink)
{
   // we put all the nodes in a temporary list and then run them from there
   class ThreadResourceList trl;

   lock();
   class ThreadResourceNode *w = head;
   while (w)
   {
      //printd(5, "TRL::purgeTID(%d) w->tid=%d, w->key=%08x, w->next=%08x\n", tid,w->tid, w->key, w->next);
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
	 w = w->next;
      }
      // erase all nodes in temporary list
      w = trl.head;
      while (w)
      {
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
   //printd(5, "TRL::remove(key=%08x)\n", key);
   lock();
   class ThreadResourceNode *w = find(key);
   if (w)
   {
      removeIntern(w);
      delete w;
   }
   unlock();
}

static inline void grow_thread_list()
{
   int start = max_thread_list;
   max_thread_list += THREAD_LIST_BLOCK;
   thread_list = (ThreadEntry *)realloc(thread_list, sizeof(ThreadEntry) * max_thread_list);
   // zero out new entries
   for (int i = start; i < max_thread_list; i++)
      thread_list[i].ptid = 0L;
}

// returns tid allocated for thread
int get_thread_entry()
{
   int tid;

   lThreadList.lock();
   if (current_tid == max_thread_list)
   {
      if (max_thread_list < MAX_THREADS)
      {
	 grow_thread_list();
	 tid = current_tid++;
      }
      else
      {
	 int i;
	 // scan thread_list for free entry
	 for (i = 0; i < max_thread_list; i++)
	 {
	    if (!thread_list[i].ptid)
	    {
	       tid = i;
	       break;
	    }
	 }
	 if (i == max_thread_list)
	    tid = -1;
      }
   }
   else
      tid = current_tid++;
   
   if (tid != -1)
   {
      thread_list[tid].ptid = (pthread_t)-1L;
      thread_list[tid].tidnode = new tid_node(tid);
      thread_list[tid].callStack = NULL;

      num_threads++;
   }
   lThreadList.unlock();
   return tid;
}

void deregister_thread(int tid)
{
   // NOTE: cannot safely call printd here, because normally the thread_data has been deleted

   lThreadList.lock();

   thread_list[tid].cleanup();

   // signal num_threads cond var if last thread exiting
   if (--num_threads == 1)
      pthread_cond_signal(&ptc_num_threads);

   lThreadList.unlock();
}

static inline void delete_thread_data()
{
   delete (ThreadData *)pthread_getspecific(thread_data_key);
}

/*
static void thread_data_cleanup(void *vtd)
{
   ThreadData *td = (ThreadData *)vtd;
   class Exception *e = NULL;
   td->dereference(&e);
   if (e && e != (Exception *)1)
   {
      defaultExceptionHandler(xsink);
      delete e;
   }

   delete td;
}
*/

void wait_for_all_threads_to_terminate()
{
   tracein("wait_for_all_threads_to_terminate()");
   lThreadList.lock();
#ifdef DEBUG
   // if debugging, then print out a message every 5 seconds
   // when waiting for threads to terminate
   while (num_threads != 1)
   {
      struct timeval now;
      struct timespec timeout;

      gettimeofday(&now, NULL);
      timeout.tv_sec = now.tv_sec + 5;
      timeout.tv_nsec = now.tv_usec * 1000;

      if (!pthread_cond_timedwait(&ptc_num_threads, &lThreadList.ptm_lock, &timeout))
	 break;
      printd(1, "waiting for %d thread%s to terminate\n", num_threads - 1,
	     num_threads == 2 ? "" : "s");

      lThreadList.unlock();
      List *l = get_thread_list();
      for (int i = 0; i < l->size(); i++)
	 printd(1, "  TID %d is still running\n", l->retrieve_entry(i)->val.intval);
      if (l)
	 delete l;
      lThreadList.lock();
   }
#else
   if (num_threads != 1)
      pthread_cond_wait(&ptc_num_threads, &lThreadList.ptm_lock);
#endif
   lThreadList.unlock();
   traceout("wait_for_all_threads_to_terminate()");
}

static void *op_background_thread(void *vtp)
{
   class QoreNode *exp  = ((BGThreadParams *)vtp)->fc;
   int tid              = ((BGThreadParams *)vtp)->tid;
   class Object *co     = ((BGThreadParams *)vtp)->callobj;
   class Object *o      = ((BGThreadParams *)vtp)->obj;
   class QoreProgram *p = ((BGThreadParams *)vtp)->pgm;
   int line             = ((BGThreadParams *)vtp)->line;
   char *file           = ((BGThreadParams *)vtp)->file;

   // register thread
   register_thread(tid, pthread_self(), p);

   p->startThread();

   update_pgm_counter_pgm_file(line, file);

   // push this call on the thread stack
   pushCall("background operator", CT_NEWTHREAD, co);

   // dereference call object if present
   if (co)
      co->tDeref();

   //update_pgm_counter_pgm_file(line, file);

   class ExceptionSink xsink;

   class QoreNode *rv = exp->eval(&xsink);

   // if there is an object, we dereference the extra reference here
   if (o)
      o->dereference(&xsink);

   popCall(&xsink);

   if (rv)
      rv->deref(&xsink);

   exp->deref(&xsink);

   // delete any thread data
   p->endThread(&xsink);
   
   // cleanup thread resources
   trlist.purgeTID(tid, &xsink);

   xsink.handleExceptions();

   delete_thread_data();

   delete (BGThreadParams *)vtp;
   // deregister_thread
   deregister_thread(tid);
   // run any cleanup functions
   tclist.exec();

   // decrement program's thread count
   p->tcount.dec();

   pthread_exit(NULL);
   return NULL;
}

inline BGThreadParams::BGThreadParams(class QoreNode *f, int t, class ExceptionSink *xsink)
{ 
   tid = t;
   fc = f;
   pgm = getProgram();
   line = get_pgm_counter();
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
   else if (fc->type == NT_TREE && fc->val.tree.op == OP_OBJECT_FUNC_REF)
   {
      // evaluate object
      class QoreNode *n = fc->val.tree.left->eval(xsink);
      if (!n || xsink->isEvent())
	 return;

      fc->val.tree.left->deref(xsink);
      fc->val.tree.left = n;
      if (n->type == NT_OBJECT)
      {
	 obj = n->val.object;
	 obj->ref();
      }
   }

   // increment the program's thread counter
   pgm->tcount.inc();

   if (callobj)
      callobj->tRef();
} 

static class QoreNode *op_background(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink)
{
   if (!left)
      return NULL;

   //printd(2, "op_background() before crlr left = %08x\n", left);
   QoreNode *nl = copy_and_resolve_lvar_refs(left, xsink);
   //printd(2, "op_background() after crlr nl = %08x\n", nl);
   if (xsink->isEvent())
   {
      if (nl) nl->deref(xsink);
      return NULL;
   }
   if (!nl)
      return NULL;

   // now we are ready to create the new thread
   pthread_t ptid;
   int rc;

   // get thread entry
   //printd(2, "calling get_thread_entry()\n");
   int tid = get_thread_entry();
   //printd(2, "got %d()\n", tid);

   // if can't start thread, then throw exception
   if (tid == -1)
   {
      if (nl) nl->deref(xsink);
      xsink->raiseException("THREAD-CREATION-FAILURE", "thread list is full with %d threads", max_thread_list);
      return NULL;
   }

   //printd(2, "creating BGThreadParams(%08x, %d)\n", nl, tid);
   BGThreadParams *tp = new BGThreadParams(nl, tid, xsink);
   if (xsink->isEvent())
   {
      if (nl) nl->deref(xsink);
      deregister_thread(tid);
      return NULL;
   }
   //printd(2, "tp = %08x\n", tp);
   //printd(2, "calling pthread_create(%08x, %08x, %08x, %08x)\n", &ptid, &ta_default, op_background_thread, tp);
   // create thread
   if ((rc = pthread_create(&ptid, &ta_default, op_background_thread, tp)))
   {
      if (nl) nl->deref(xsink);

      // decrement program's thread count
      getProgram()->tcount.dec();

      deregister_thread(tid);
      xsink->raiseException("THREAD-CREATION-FAILURE", "could not create thread: %s", strerror(errno));
      return NULL;
   }
   //printd(2, "pthread_create() returned %d\n", rc);

   printd(5, "create_thread() created thread with TID %d\n", ptid);
   return new QoreNode(NT_INT, tid);
}

void init_qore_threads()
{
   tracein("qore_init_threads()");

   // init thread list
   grow_thread_list();

   // init num_threads cond var
   pthread_cond_init(&ptc_num_threads, NULL);

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

   pthread_mutexattr_destroy(&ma_recursive);

   //printd(2, "calling pthread_attr_destroy(%08x)\n", &ta_default);
   pthread_attr_destroy(&ta_default);
   //printd(2, "returned from pthread_attr_destroy(%08x)\n", &ta_default);

   class ExceptionSink xsink;
   // cleanup thread resources
   trlist.purgeTID(0, &xsink);
   xsink.handleExceptions();

   // delete ThreadData for parent thread
   //thread_data_cleanup(pthread_getspecific(thread_data_key));

   delete_thread_data();

   // destroy last thread condition var
   pthread_cond_destroy(&ptc_num_threads);

   thread_list[0].cleanup();

   // delete key
   pthread_key_delete(thread_data_key);

   // delete thread list
   free(thread_list);

   max_thread_list = 0;

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
	    str.terminate(0);
	    str.sprintf("%d", w->tid);
	    h->setKeyValue(&str, new QoreNode(l), NULL);
	 }
	 else
	    delete l;
      }
      w = w->next;
   }   
   lThreadList.unlock();
   return h;
}
