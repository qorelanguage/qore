/*
  thread.h

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

#ifndef _QORE_THREAD_H
#define _QORE_THREAD_H

#include <stdio.h>

extern class Operator *OP_BACKGROUND;

static inline int gettid();
static inline class LVar *get_thread_stack();
static inline void update_thread_stack(class LVar *lvs);
static inline class Context *get_context_stack();
static inline void update_context_stack(class Context *cstack);
static inline class ArgvStack *get_argvstack();
static inline void update_argvstack(class ArgvStack *as);
static inline int get_pgm_counter();
static inline int get_pgm_stmt();
static inline char *get_pgm_file();
static inline void update_pgm_stmt();
static inline void update_pgm_counter_pgm_file(int p, char *f);
static inline void increment_pgm_counter();
static inline void pushCall(char *f, int type, class Object *o = NULL);
static inline void popCall(class ExceptionSink *xsink = NULL);
static inline class List *getCallStack();
static inline void pushProgram(class QoreProgram *pgm);
static inline void popProgram();
static inline class QoreProgram *getProgram();
static inline class Namespace *getRootNS();
static inline int getParseOptions();
static inline class ProgramLocation *getPLStack();
static inline void updatePLStack(class ProgramLocation *pl);
static inline void beginParsing(char *file, void *ps = NULL);
static inline void *endParsing();
static inline void updateCVarStack(class CVNode *ncvs);
static inline class CVNode *getCVarStack();
static inline void updateVStack(class VNode *nvs);
static inline class VNode *getVStack();
static inline class Object *getStackObject();
static inline void setParseClass(class QoreClass *c);
static inline class QoreClass *getParseClass();
static inline void substituteObjectIfEqual(class Object *o);
static inline class Object *substituteObject(class Object *o);
static inline void catchSaveException(class Exception *e);
static inline class Exception *catchGetException();

class ProgramLocation {
   public:
      int line;
      char *file;
      void *parseState;
      class ProgramLocation *next;

      inline ProgramLocation(int l, char *fname, void *ps = NULL) 
      { 
	 line = l; 
	 file = fname; 
	 parseState = ps;
      }
};

#include <pthread.h>

// pointer to a qore thread destructor function
typedef void (*qtdest_t) (void *);

// this structure holds all thread-specific data
class ThreadData 
{
   public:
      int tid;
      class LVar *lvstack;
      class Context *context_stack;
      class ArgvStack *argvstack;
      class QoreProgramStack *pgmStack;
      class ProgramLocation *plStack;
      int pgm_stmt;
      int pgm_counter;
      char *pgm_file;
      void *parseState;
      class VNode *vstack;
      class CVNode *cvarstack;
      class QoreClass *parseClass;
      class Exception *catchException;
      
      inline ThreadData(int ptid, QoreProgram *p);
      inline ~ThreadData();
};

class tid_node {
   public:
      int tid;
      class tid_node *next;
      class tid_node *prev;
      inline tid_node(int ntid);
      inline ~tid_node();
};

// this structure holds all thread data
class ThreadEntry {
   public:
      pthread_t ptid;
      class tid_node *tidnode;
      class CallStack *callStack;

      inline void cleanup();
};

class ThreadParams {
   public:
      class QoreNode *fc;
      int tid;
      class QoreProgram *pgm;
      inline ThreadParams(class QoreNode *f, int t) 
      { 
	 fc = f; 
	 tid = t;
	 pgm = getProgram();
      } 

};

class BGThreadParams {
   public:
      class Object *obj;
      class Object *callobj;
      class QoreNode *fc;
      class QoreProgram *pgm;
      int tid;
      int line;
      char *file;
      bool method_reference;
      inline BGThreadParams(class QoreNode *f, int t, class ExceptionSink *xsink);
};

class ThreadCleanupNode {
   public:
      class ThreadCleanupNode *next;
      qtdest_t func;
      void *arg;
};

class ThreadCleanupList {
   private:
      class ThreadCleanupNode *head;

   public:
      ThreadCleanupList();
      ~ThreadCleanupList();
      void push(qtdest_t func, void *arg);
      void pop(int exec = 0);
      inline void exec()
      {
	 class ThreadCleanupNode *w = head;
	 while (w)
	 {
	    w->func(w->arg);
	    w = w->next;
	 }
      }
};

extern ThreadCleanupList tclist;

void init_qore_threads();
class Namespace *get_thread_ns();
void delete_qore_threads();
int get_thread_entry();
void deregister_thread(int tid);
void wait_for_all_threads_to_terminate();
class List *get_thread_list();
class Hash *getAllCallStacks();

extern pthread_key_t thread_data_key;
extern class ThreadEntry *thread_list;
extern class tid_node *tid_head, *tid_tail;

#include <qore/CallStack.h>
#include <qore/ArgvStack.h>
#include <qore/Hash.h>
#include <qore/Object.h>
#include <qore/QoreClass.h>
#include <qore/Variable.h>
#include <qore/QoreProgramStack.h>
#include <qore/QoreProgram.h>
#include <qore/Statement.h>

inline void ThreadEntry::cleanup()
{
   // delete tidnode from tid_list
   delete tidnode;
   
   // delete call stack
   if (callStack)
      delete callStack;
   
   // set ptid to 0
   ptid = 0L;
}

// this constructor must only be called in the ptm_thread_list mutex
inline tid_node::tid_node(int ntid)
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

// this destructor must only be called in the ptm_thread_list mutex
inline tid_node::~tid_node()
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

inline ThreadData::ThreadData(int ptid, class QoreProgram *p)
{
   tid           = ptid;
   lvstack       = NULL;
   context_stack = NULL;
   argvstack     = NULL;
   pgm_counter   = 0;
   pgm_stmt      = 0;
   pgm_file      = NULL;
   pgmStack      = new QoreProgramStack(p);
   plStack       = NULL;
   parseState    = NULL;
   vstack        = NULL;
   cvarstack     = NULL;
   parseClass    = NULL;
}

inline ThreadData::~ThreadData()
{
   delete pgmStack;
}

static inline int gettid()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->tid;
}

static inline class LVar *get_thread_stack()
{

   return ((ThreadData *)pthread_getspecific(thread_data_key))->lvstack;
}

static inline void update_thread_stack(class LVar *lvstack)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->lvstack    = lvstack;
   pthread_setspecific(thread_data_key, td);
}

static inline Context *get_context_stack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->context_stack;
}

static inline void update_context_stack(Context *cstack)
{
   ThreadData *td    = (ThreadData *)pthread_getspecific(thread_data_key);
   td->context_stack = cstack;
   pthread_setspecific(thread_data_key, td);
}

static inline class ArgvStack *get_argvstack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->argvstack;
}

static inline void update_argvstack(ArgvStack *as)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->argvstack  = as;
   pthread_setspecific(thread_data_key, td);
}

static inline int get_pgm_counter()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->pgm_counter;
}

static inline int get_pgm_stmt()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->pgm_stmt;
}

static inline void update_pgm_counter_pgm_file(int p, char *f)
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   td->pgm_counter = p;
   td->pgm_file    = f;
   pthread_setspecific(thread_data_key, td);
}

static inline void update_pgm_stmt()
{
   ThreadData *td  = (ThreadData *)pthread_getspecific(thread_data_key);
   td->pgm_stmt = td->pgm_counter;
   pthread_setspecific(thread_data_key, td);
}

static inline void increment_pgm_counter()
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->pgm_counter++;
   pthread_setspecific(thread_data_key, td);
}

static inline char *get_pgm_file()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->pgm_file;
}

static inline void pushCall(char *f, int type, class Object *o)
{
   thread_list[gettid()].callStack->push(f, type, o);
   //OLD: ((ThreadData *)pthread_getspecific(thread_data_key))->callStack->push(f, type, o);
}

static inline void popCall(class ExceptionSink *xsink)
{
   thread_list[gettid()].callStack->pop(xsink);
   //OLD: ((ThreadData *)pthread_getspecific(thread_data_key))->callStack->pop(xsink);
}

static inline class List *getCallStack()
{
   return thread_list[gettid()].callStack->getCallStack();
   //OLD: return ((ThreadData *)pthread_getspecific(thread_data_key))->callStack->getCallStack();
}

static inline class Object *getStackObject()
{
   return thread_list[gettid()].callStack->getStackObject();
   //OLD: return ((ThreadData *)pthread_getspecific(thread_data_key))->callStack->getStackObject();
}

static inline void pushProgram(class QoreProgram *pgm)
{
   ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->push(pgm);
}

static inline void popProgram()
{
   ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->pop();
}

static inline class QoreProgram *getProgram()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->getProgram();
}

static inline class Namespace *getRootNS()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->getProgram()->getRootNS();
}

static inline int getParseOptions()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->pgmStack->getProgram()->getParseOptions();
}

// should only be called from new thread
static inline void register_thread(int tid, pthread_t ptid, class QoreProgram *p)
{
   thread_list[tid].ptid = ptid;
   thread_list[tid].callStack = new CallStack();
   pthread_setspecific(thread_data_key, (void *)(new ThreadData(tid, p)));
}

static inline class ProgramLocation *getPLStack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->plStack;
}

static inline void updatePLStack(class ProgramLocation *pl)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->plStack = pl;
   pthread_setspecific(thread_data_key, td);
}

// new file name, current parse state
static inline void beginParsing(char *file, void *ps)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);

   printd(5, "beginParsing() of \"%s\", (stack=%s)\n",
	  file, (td->plStack ? td->plStack->file : "NONE"));

   // if current position exists, then save
   if (td->pgm_file)
   {
      class ProgramLocation *pl = new ProgramLocation(td->pgm_counter, td->pgm_file, td->parseState);
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
   td->pgm_counter = 1;
   td->pgm_stmt = 0;
   td->pgm_file = file;
   td->parseState = ps;
   pthread_setspecific(thread_data_key, td);
}

static inline void *endParsing()
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   void *rv = td->parseState;

   printd(5, "endParsing() ending parsing of \"%s\", returning %08x\n",
	  td->pgm_file, rv);
   if (td->plStack)
   {
      class ProgramLocation *pl = td->plStack->next;
      td->pgm_counter = td->plStack->line;
      td->pgm_file    = td->plStack->file;
      td->parseState  = td->plStack->parseState;
      delete td->plStack;
      td->plStack = pl;
   }
   else
   {
      td->pgm_counter = 0;
      td->pgm_file    = NULL;
      td->parseState  = NULL;
   }
   pthread_setspecific(thread_data_key, td);
   return rv;
}

static inline void updateCVarStack(class CVNode *ncvs)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->cvarstack = ncvs;
   pthread_setspecific(thread_data_key, td);
}

static inline class CVNode *getCVarStack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->cvarstack;
}

static inline void updateVStack(class VNode *nvs)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->vstack = nvs;
   pthread_setspecific(thread_data_key, td);
}

static inline class VNode *getVStack()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->vstack;
}

static inline void setParseClass(class QoreClass *c)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->parseClass = c;
   pthread_setspecific(thread_data_key, td);
}

static inline class QoreClass *getParseClass()
{
   return ((ThreadData *)pthread_getspecific(thread_data_key))->parseClass;
}

static inline void substituteObjectIfEqual(class Object *o)
{
   thread_list[gettid()].callStack->substituteObjectIfEqual(o);
}

static inline class Object *substituteObject(class Object *o)
{
   return thread_list[gettid()].callStack->substituteObject(o);
}

static inline void catchSaveException(class Exception *e)
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   td->catchException = e;
}

static inline class Exception *catchGetException()
{
   ThreadData *td = (ThreadData *)pthread_getspecific(thread_data_key);
   return td->catchException;
}

#endif  // ifndef _QORE_THREAD_H
