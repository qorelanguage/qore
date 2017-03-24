/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  thread.cpp

  threading functionality for Qore

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include <openssl/err.h>

#include "qore/intern/ThreadResourceList.h"
#include "qore/intern/ConstantList.h"
#include "qore/intern/QoreSignal.h"
#include "qore/intern/qore_program_private.h"
#include "qore/intern/ModuleInfo.h"

// to register object types
#include "qore/intern/QC_Queue.h"
#include "qore/intern/QC_Mutex.h"
#include "qore/intern/QC_Condition.h"
#include "qore/intern/QC_RWLock.h"
#include "qore/intern/QC_Gate.h"
#include "qore/intern/QC_Sequence.h"
#include "qore/intern/QC_Counter.h"
#include "qore/intern/QC_AutoLock.h"
#include "qore/intern/QC_AutoGate.h"
#include "qore/intern/QC_AutoReadLock.h"
#include "qore/intern/QC_AutoWriteLock.h"
#include "qore/intern/QC_AbstractSmartLock.h"
#include "qore/intern/QC_AbstractThreadResource.h"

#include <pthread.h>
#include <sys/time.h>
#include <assert.h>

#include <vector>
#include <map>
#include <set>
#include <string>

#if defined(__ia64) && defined(__LP64__)
#define IA64_64
#endif

// global background thread counter
QoreCounter thread_counter;

ThreadCleanupList tclist;

DLLLOCAL bool threads_initialized = false;

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

// default thread creation attribute
QorePThreadAttr ta_default;

DLLLOCAL QoreThreadList thread_list;

DLLLOCAL QoreClass* initThreadPoolClass(QoreNamespace& ns);

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

struct ParseCountHelper {
   unsigned count;

   DLLLOCAL ParseCountHelper() : count(0) {
   }

   DLLLOCAL void inc() {
      ++count;
   }

   DLLLOCAL bool dec() {
      if (!count) {
         parse_error("unmatched %%endtry");
         return false;
      }
      return !--count;
   }

   DLLLOCAL void purge() {
      if (count) {
         parse_error("%d %%try-module block%s left open at end of file", count, count == 1 ? "" : "s");
         count = 0;
      }
   }
};

struct ParseConditionalStack {
   unsigned count;
   typedef std::vector<unsigned> ui_vec_t;
   ui_vec_t markvec;

   DLLLOCAL ParseConditionalStack() : count(0) {
   }

   DLLLOCAL void push(bool do_mark = false) {
      if (do_mark) {
         markvec.push_back(count);
      }
      ++count;
   }

   DLLLOCAL bool checkElse() {
      return count;
   }

   DLLLOCAL bool test() const {
      if (!count) {
         parse_error("%%else without %%ifdef");
         return false;
      }
      if (markvec.empty())
         return false;
      return markvec.back() == (count - 1);
   }

   DLLLOCAL bool pop() {
      if (!count) {
         parse_error("unmatched %%endif");
         return false;
      }
      --count;
      assert(!markvec.empty());
      if (count == markvec.back()) {
         markvec.pop_back();
         return true;
      }
      return false;
   }

   DLLLOCAL void purge() {
      if (count) {
         parse_error("%d conditional block%s left open at end of file", count, count == 1 ? "" : "s");
         count = 0;
         markvec.clear();
      }
   }
};

class ProgramParseContext {
public:
   const char* file, * src;
   void* parseState;
   int offset;
   ParseConditionalStack* pcs;
   ProgramParseContext* next;

   DLLLOCAL ProgramParseContext(const char* fname, void* ps, const char* psrc, int off, ParseConditionalStack* ppcs, ProgramParseContext* n) : file(fname), src(psrc), parseState(ps), offset(off), pcs(ppcs), next(n) {
   }
};

// for detecting circular references at runtime
typedef std::set<const lvalue_ref*> ref_set_t;

// this structure holds all thread-specific data
class ThreadData {
public:
   int64 runtime_po;
   int tid;
   VLock vlock;     // for deadlock detection
   Context* context_stack;
   ProgramParseContext* plStack;
   QoreProgramLocation parse_loc;
   QoreProgramLocation runtime_loc;
   const char* parse_code; // the current function, method, or closure being parsed
   void* parseState;
   VNode* vstack;  // used during parsing (local variable stack)
   CVNode* cvarstack;
   QoreClass* parseClass; // current class being parsed
   QoreException* catchException;
   std::list<block_list_t::iterator> on_block_exit_list;
   ThreadResourceList* trlist;

   // for detecting circular references at runtime
   ref_set_t ref_set;

   // current function/method name
   const char* current_code;

   // current object context
   QoreObject* current_obj;

   // current class context
   const qore_class_private* current_class;

   // current program context
   QoreProgram* current_pgm;

   // current namespace context for parsing
   qore_ns_private* current_ns;

   // current implicit argument
   QoreListNode* current_implicit_arg;

   // this data structure is stored in the current Program object on a per-thread basis
   ThreadLocalProgramData* tlpd;

   // this data structure contains the set of Program objects that this thread has data in
   ThreadProgramData* tpd;

   // current parsing closure environment
   ClosureParseEnvironment* closure_parse_env;

   // current runtime closure environment
   const QoreClosureBase* closure_rt_env;

   ArgvRefStack argv_refs;

#ifdef QORE_MANAGE_STACK
   size_t stack_limit;
#ifdef IA64_64
   size_t rse_limit;
#endif
#endif

   // used to detect output of recursive data structures at runtime
   const_node_set_t node_set;

   // currently-executing/parsing block's return type
   const QoreTypeInfo* returnTypeInfo;

   // parse-time block return type
   const QoreTypeInfo* parse_return_type_info;

   // current implicit element offset
   int element;

   // start of global thread-local variables for the current thread and program being parsed
   VNode* global_vnode;

   // Maintains the conditional parse block count for each file parsed
   ParseConditionalStack* pcs;

   // Maintains the %try-module block count for each file
   ParseCountHelper tm;

   // for capturing namespace and class names while parsing
   typedef std::vector<std::string> npvec_t;
   npvec_t npvec;

   // used for error handling when merging module code into a Program object
   QoreModuleContext* qmc;

   // used to capture the module definition in user modules
   QoreModuleDefContext* qmd;

   // user to track the current user module context
   const char* user_module_context_name;

   // AbstractQoreModule* with boolean ptr in bit 0
   uintptr_t qmi;

   bool
   foreign : 1; // true if the thread is a foreign thread

   DLLLOCAL ThreadData(int ptid, QoreProgram* p, bool n_foreign = false) :
      runtime_po(0), tid(ptid), vlock(ptid), context_stack(0), plStack(0),
      parse_code(0), parseState(0), vstack(0), cvarstack(0),
      parseClass(0), catchException(0), trlist(new ThreadResourceList), current_code(0),
      current_obj(0), current_class(0),
      current_pgm(p), current_ns(0), current_implicit_arg(0), tlpd(0), tpd(new ThreadProgramData(this)),
      closure_parse_env(0), closure_rt_env(0),
      returnTypeInfo(0), parse_return_type_info(0), element(0), global_vnode(0), pcs(0),
      qmc(0), qmd(0), user_module_context_name(0), qmi(0), foreign(n_foreign) {

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

   DLLLOCAL ~ThreadData() {
      assert(on_block_exit_list.empty());
      assert(!tpd);
      assert(!trlist->prev);
      delete pcs;
      delete trlist;
   }

   DLLLOCAL void endFileParsing() {
      if (pcs) {
         pcs->purge();
         delete pcs;
         pcs = 0;
      }
      tm.purge();
   }

   DLLLOCAL int getElement() {
      return element;
   }

   DLLLOCAL int saveElement(int n_element) {
      int rc = element;
      element = n_element;
      return rc;
   }

   DLLLOCAL void del(ExceptionSink* xsink) {
      tpd->del(xsink);
      tpd->deref();
      tpd = 0;
   }

   DLLLOCAL void pushName(const char* name) {
      npvec.push_back(name);
   }

   DLLLOCAL std::string popName() {
      assert(!npvec.empty());
      std::string rv = npvec.back();
      npvec.pop_back();
      return rv;
   }

   DLLLOCAL void parseRollback() {
      npvec.clear();
   }

   DLLLOCAL qore_ns_private* set_ns(qore_ns_private* ns) {
      if (ns == current_ns)
         return ns;

      qore_ns_private* rv = current_ns;
      current_ns = ns;
      return rv;
   }
};

static QoreThreadLocalStorage<ThreadData> thread_data;

void ThreadEntry::allocate(tid_node* tn, int stat) {
   assert(status == QTS_AVAIL);
   status = stat;
   tidnode = tn;
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   assert(!callStack);
   callStack = new CallStack;
#endif
   joined = false;
   assert(!thread_data);
}

void ThreadEntry::activate(int tid, pthread_t n_ptid, QoreProgram* p, bool foreign) {
   assert(status == QTS_NA || status == QTS_RESERVED);
   ptid = n_ptid;
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   assert(callStack);
#endif
   assert(!thread_data);
   thread_data = new ThreadData(tid, p, foreign);
   ::thread_data.set(thread_data);
   status = QTS_ACTIVE;
   // set lvstack if QoreProgram set
   if (p)
      thread_data->tpd->saveProgram(true, 0);
}

void ThreadProgramData::delProgram(QoreProgram* pgm) {
   //printd(5, "ThreadProgramData::delProgram() this: %p pgm: %p\n", this, pgm);
   {
      AutoLocker al(pslock);
      pgm_set_t::iterator i = pgm_set.find(pgm);
      if (i == pgm_set.end())
         return;
      pgm_set.erase(i);
   }
   //printd(5, "ThreadProgramData::delProgram() this: %p deref pgm: %p\n", this, pgm);
   // this can never cause the program to go out of scope because it's always called
   // when the reference count > 1, therefore *xsink = 0 is OK
   pgm->depDeref();
   deref();
}

void ThreadProgramData::saveProgram(bool runtime, ExceptionSink* xsink) {
   if (!qore_program_private::setThreadVarData(td->current_pgm, this, td->tlpd, runtime))
      return;
   //printd(5, "ThreadProgramData::saveProgram() this: %p pgm: %p\n", this, td->current_pgm);
   ref();
   td->current_pgm->depRef();
   {
      AutoLocker al(pslock);
      assert(pgm_set.find(td->current_pgm) == pgm_set.end());
      pgm_set.insert(td->current_pgm);
   }
   if (runtime)
      qore_program_private::doThreadInit(*td->current_pgm, xsink);
}

void ThreadProgramData::del(ExceptionSink* xsink) {
   // first purge all data
   arg_vec_t* cl = 0;

   // remove and finalize all thread-local data in all referenced programs
   {
      AutoLocker al(pslock);
      for (pgm_set_t::iterator i = pgm_set.begin(), e = pgm_set.end(); i != e; ++i)
         qore_program_private::finalizeThreadData(*i, this, cl);
   }

   // delete thread-local data
   if (cl) {
      for (arg_vec_t::iterator i = cl->begin(), e = cl->end(); i != e; ++i)
         (*i)->deref(xsink);
      delete cl;
   }

   // purge thread data in contained programs
   while (true) {
      QoreProgram* pgm;
      {
         AutoLocker al(pslock);
         pgm_set_t::iterator i = pgm_set.begin();
         if (i == pgm_set.end())
            break;

         pgm = (*i);
         pgm_set.erase(i);
      }
      //printd(5, "ThreadProgramData::del() this: %p pgm: %p\n", this, pgm);
      pgm->depDeref();
      // only dereference the current object if the thread was deleted from the program
      if (!qore_program_private::endThread(pgm, this, xsink))
         deref();
   }
}

class ThreadCleanupNode {
public:
   qtdest_t func;
   void* arg;
   ThreadCleanupNode* next;
};

DLLLOCAL ThreadCleanupNode* ThreadCleanupList::head = 0;

class ThreadParams {
public:
   AbstractQoreNode* fc;
   int tid;
   QoreProgram* pgm;

   DLLLOCAL ThreadParams(AbstractQoreNode* f, int t) {
      fc = f;
      tid = t;
      pgm = getProgram();
   }
};

// this constructor must only be called with the QoreThreadList lock held
tid_node::tid_node(int ntid) {
   tid = ntid;
   next = 0;
   prev = thread_list.tid_tail;
   if (!thread_list.tid_head)
      thread_list.tid_head = this;
   else
      thread_list.tid_tail->next = this;
   thread_list.tid_tail = this;
}

// this destructor must only be called with the QoreThreadList lock held
tid_node::~tid_node() {
   if (prev)
      prev->next = next;
   else
      thread_list.tid_head = next;
   if (next)
      next->prev = prev;
   else
      thread_list.tid_tail = prev;
}

class BGThreadParams {
private:
   // call_obj: get and reference the current stack object, if any, for the new call stack
   QoreObject* call_obj;

   DLLLOCAL ~BGThreadParams() {
   }

public:
   QoreObject* obj;
   const qore_class_private* class_ctx;

   AbstractQoreNode* fc;
   QoreProgram* pgm;
   int tid;
   QoreProgramLocation loc;
   bool registered, started;

   DLLLOCAL BGThreadParams(AbstractQoreNode* f, int t, ExceptionSink* xsink)
      : obj(0),
        fc(f), pgm(getProgram()), tid(t), loc(RunTimeLocation), registered(false), started(false) {
      {
         ThreadData* td = thread_data.get();
         call_obj = td->current_obj;
         class_ctx = td->current_class;
      }

      //printd(5, "BGThreadParams::BGThreadParams(f: %p (%s %d), t: %d) this: %p call_obj: %p '%s' cc: %p '%s' fct: %d\n", f, f->getTypeName(), f->getType(), t, this, call_obj, call_obj ? call_obj->getClassName() : "n/a", class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a", fc->getType());

      // first try to preregister the new thread
      if (qore_program_private::preregisterNewThread(*pgm, xsink)) {
         call_obj = 0;
         return;
      }

      registered = true;

      qore_type_t fctype = fc->getType();
      if (fctype == NT_SELF_CALL) {
         {
            const QoreClass* qc = reinterpret_cast<SelfFunctionCallNode*>(fc)->getClass();
            if (qc)
               class_ctx = qore_class_private::get(*qc);
         }

	 // must have a current object if an in-object method call is being executed
	 // (i.e. $.method())
	 // we reference the object so it won't go out of scope while the thread is running
	 obj = call_obj;
         assert(obj);
	 obj->realRef();
         call_obj = 0;
      }

      if (call_obj)
	 call_obj->tRef();
   }

   DLLLOCAL void del() {
      // decrement program's thread count
      if (started) {
         qore_program_private::decThreadCount(*pgm, tid);
         //printd(5, "BGThreadParams::del() this: %p pgm: %p\n", this, pgm);
         pgm->depDeref();
      }
      else if (registered)
         qore_program_private::cancelPreregistration(*pgm);

      delete this;
   }

   DLLLOCAL void startThread(ExceptionSink& xsink) {
      // register the new tid
      qore_program_private::registerNewThread(*pgm, tid);
      // create thread-local data in the program object
      qore_program_private::startThread(*pgm, xsink);
      // set program counter for new thread
      update_runtime_location(loc);
      started = true;
      //printd(5, "BGThreadParams::startThread() this: %p pgm: %p\n", this, pgm);
      pgm->depRef();
   }

   DLLLOCAL QoreObject* getCallObject() {
      return obj ? obj : call_obj;
   }

   DLLLOCAL void cleanup(ExceptionSink* xsink) {
      if (fc) fc->deref(xsink);
      derefObj(xsink);
      derefCallObj();
   }

   DLLLOCAL void derefCallObj() {
      // dereference call object if present
      if (call_obj) {
	 call_obj->tDeref();
         call_obj = 0;
      }
   }

   DLLLOCAL void derefObj(ExceptionSink* xsink) {
      if (obj) {
         obj->realDeref(xsink);
         obj = 0;
      }
   }

   DLLLOCAL AbstractQoreNode* exec(ExceptionSink* xsink) {
      //printd(5, "BGThreadParams::exec() this: %p fc: %p (%s %d)\n", this, fc, fc->getTypeName(), fc->getType());
      AbstractQoreNode* rv = fc->eval(xsink);
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
   //printf("ThreadCleanupList::~ThreadCleanupList() head: %p\n", head);

   while (head) {
      ThreadCleanupNode* w = head->next;
      delete head;
      head = w;
   }
}

void ThreadCleanupList::exec() {
   class ThreadCleanupNode* w = head;
   while (w) {
      w->func(w->arg);
      w = w->next;
   }
}

void ThreadCleanupList::push(qtdest_t func, void* arg) {
   ThreadCleanupNode* w = new ThreadCleanupNode;
   w->next = head;
   w->func = func;
   w->arg = arg;
   head = w;
   //printf("TCL::push() this: %p, &head: %p, head: %p, head->next: %p\n", this, &head, head, head->next);
}

void ThreadCleanupList::pop(bool exec) {
   if (exec)
      head->func(head->arg);
   ThreadCleanupNode* w = head->next;
   delete head;
   head = w;
}

#ifdef QORE_MANAGE_STACK
int check_stack(ExceptionSink* xsink) {
   ThreadData* td = thread_data.get();
   printd(5, "check_stack() current: %p limit: %p\n", get_stack_pos(), td->stack_limit);
#ifdef IA64_64
   //printd(5, "check_stack() bsp current: %p limit: %p\n", get_rse_bsp(), td->rse_limit);
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

QoreAbstractModule* set_reexport(QoreAbstractModule* m, bool current_reexport, bool& old_reexport) {
   ThreadData* td = thread_data.get();
   uintptr_t rv = td->qmi;
   if (rv & 1) {
      old_reexport = true;
      rv ^= 1;
   }
   else
      old_reexport = false;

   td->qmi = (uintptr_t)m;
   if (current_reexport)
      td->qmi |= 1;

   return (QoreAbstractModule*)rv;
}

void set_reexport(QoreAbstractModule* m, bool reexport) {
   ThreadData* td = thread_data.get();
   td->qmi = (uintptr_t)m;
   if (reexport)
      td->qmi |= 1;
}

// returns 1 if data structure is already on stack, 0 if not (=OK)
int thread_push_container(const AbstractQoreNode* n) {
   std::pair<const_node_set_t::iterator, bool> rv = thread_data.get()->node_set.insert(n);
   return !rv.second;
}

void thread_pop_container(const AbstractQoreNode* n) {
   ThreadData* td = thread_data.get();

   const_node_set_t::iterator i = td->node_set.find(n);
   assert(i != td->node_set.end());
   td->node_set.erase(i);
}

int thread_ref_set(const lvalue_ref* r) {
   ThreadData* td = thread_data.get();
   return !td->ref_set.insert(r).second ? -1 : 0;
}

void thread_ref_remove(const lvalue_ref* r) {
   ThreadData* td = thread_data.get();
   assert(td->ref_set.find(r) != td->ref_set.end());
   td->ref_set.erase(r);
}

LocalVarValue* thread_instantiate_lvar() {
   return thread_data.get()->tlpd->lvstack.instantiate();
}

void thread_uninstantiate_lvar(ExceptionSink* xsink) {
   ThreadData* td = thread_data.get();
   td->tlpd->lvstack.uninstantiate(xsink);
}

void thread_uninstantiate_self() {
   ThreadData* td = thread_data.get();
   td->tlpd->lvstack.uninstantiateSelf();
}

LocalVarValue* thread_find_lvar(const char* id) {
   ThreadData* td = thread_data.get();
   return td->tlpd->lvstack.find(id);
}

ClosureVarValue* thread_instantiate_closure_var(const char* n_id, const QoreTypeInfo* typeInfo, QoreValue& nval) {
   return thread_data.get()->tlpd->cvstack.instantiate(n_id, typeInfo, nval);
}

void thread_instantiate_closure_var(ClosureVarValue* cvar) {
   return thread_data.get()->tlpd->cvstack.instantiate(cvar);
}

void thread_uninstantiate_closure_var(ExceptionSink* xsink) {
   thread_data.get()->tlpd->cvstack.uninstantiate(xsink);
}

ClosureVarValue* thread_find_closure_var(const char* id) {
   return thread_data.get()->tlpd->cvstack.find(id);
}

const QoreClosureBase* thread_set_runtime_closure_env(const QoreClosureBase* current) {
   ThreadData* td = thread_data.get();
   const QoreClosureBase* rv = td->closure_rt_env;
   td->closure_rt_env = current;
   return rv;
}

cvv_vec_t* thread_get_all_closure_vars() {
   return thread_data.get()->tlpd->cvstack.getAll();
}

void thread_set_closure_parse_env(ClosureParseEnvironment* cenv) {
   thread_data.get()->closure_parse_env = cenv;
}

ClosureVarValue* thread_get_runtime_closure_var(const LocalVar* id) {
   return thread_data.get()->closure_rt_env->find(id);
}

ClosureParseEnvironment* thread_get_closure_parse_env() {
   return thread_data.get()->closure_parse_env;
}

void thread_push_frame_boundary() {
   ThreadData* td = thread_data.get();
   td->tlpd->lvstack.pushFrameBoundary();
   td->tlpd->cvstack.pushFrameBoundary();
}

void thread_pop_frame_boundary() {
   ThreadData* td = thread_data.get();
   td->tlpd->lvstack.popFrameBoundary();
   td->tlpd->cvstack.popFrameBoundary();
}

QoreHashNode* thread_get_local_vars(int frame, ExceptionSink* xsink) {
   ReferenceHolder<QoreHashNode> rv(new QoreHashNode, xsink);
   ThreadData* td = thread_data.get();
   if (frame >= 0) {
      td->tlpd->lvstack.getLocalVars(**rv, frame, xsink);
      if (*xsink)
         return nullptr;
      td->tlpd->cvstack.getLocalVars(**rv, frame, xsink);
      if (*xsink)
         return nullptr;
   }
   return rv.release();
}

int thread_set_local_var_value(const char* name, const QoreValue& val, ExceptionSink* xsink) {
   return thread_data.get()->tlpd->lvstack.setVarValue(name, val, xsink);
}

int thread_set_closure_var_value(const char* name, const QoreValue& val, ExceptionSink* xsink) {
   return thread_data.get()->tlpd->cvstack.setVarValue(name, val, xsink);
}

void parse_push_name(const char* name) {
   ThreadData* td = thread_data.get();
   td->pushName(name);
}

std::string parse_pop_name() {
   ThreadData* td = thread_data.get();
   return td->popName();
}

void set_thread_resource(AbstractThreadResource* atr) {
   ThreadData* td = thread_data.get();
   td->trlist->set(atr);
}

int remove_thread_resource(AbstractThreadResource* atr) {
   ThreadData* td = thread_data.get();
   return td->trlist->remove(atr);
}

bool check_thread_resource(AbstractThreadResource* atr) {
   ThreadData* td = thread_data.get();
   return td->trlist->check(atr);
}

void set_thread_resource(const ResolvedCallReferenceNode* rcr, QoreValue arg) {
   thread_data.get()->trlist->set(rcr, arg);
}

int remove_thread_resource(const ResolvedCallReferenceNode* rcr, ExceptionSink* xsink) {
   return thread_data.get()->trlist->remove(rcr, xsink);
}

void mark_thread_resources() {
   ThreadData* td = thread_data.get();
   ThreadResourceList* trl = new ThreadResourceList(td->trlist);
   td->trlist = trl;
}


// returns 0 if the last mark has been cleared, -1 if there are more marks to check
static int purge_thread_resources_to_mark(ThreadData* td, ExceptionSink* xsink) {
   td->trlist->purge(xsink);

   if (td->trlist->prev) {
      ThreadResourceList* tr = td->trlist;
      td->trlist = tr->prev;
      delete tr;
      return -1;
   }
   return 0;
}

// returns 0 if the last mark has been cleared, -1 if there are more marks to check
int purge_thread_resources_to_mark(ExceptionSink* xsink) {
   ThreadData* td = thread_data.get();
   return purge_thread_resources_to_mark(td, xsink);
}

void purge_thread_resources(ExceptionSink* xsink) {
   ThreadData* td = thread_data.get();
   while (purge_thread_resources_to_mark(td, xsink));
}

void purge_pgm_thread_resources(const QoreProgram* pgm, ExceptionSink* xsink) {
   ThreadData* td = thread_data.get();

   ThreadResourceList* tr = td->trlist;
   while (tr) {
      tr->purge(pgm, xsink);
      tr = tr->prev;
   }
}

void parse_try_module_inc() {
   ThreadData* td = thread_data.get();
   td->tm.inc();
}

bool parse_try_module_dec() {
   ThreadData* td = thread_data.get();
   return td->tm.dec();
}

unsigned parse_try_module_get() {
   return thread_data.get()->tm.count;
}

void parse_try_module_set(unsigned c) {
   thread_data.get()->tm.count = c;
}

void parse_cond_push(bool mark) {
   ThreadData* td = thread_data.get();
   if (!td->pcs)
      td->pcs = new ParseConditionalStack;
   td->pcs->push(mark);
}

bool parse_cond_else() {
   ThreadData* td = thread_data.get();
   return td->pcs ? td->pcs->checkElse() : 0;
}

bool parse_cond_pop() {
   ThreadData* td = thread_data.get();
   if (!td->pcs) {
      parse_error("unmatched %%endif");
      return false;
   }
   return td->pcs->pop();
}

bool parse_cond_test() {
   ThreadData* td = thread_data.get();
   if (!td->pcs) {
      parse_error("%%else without %%ifdef");
      return false;
   }
   return td->pcs->test();
}

void push_parse_options() {
   ThreadData* td = thread_data.get();
   qore_program_private::pushParseOptions(td->current_pgm, td->parse_loc.file);
}

// called when a StatementBlock has "on_exit" blocks
void pushBlock(block_list_t::iterator i) {
   ThreadData* td = thread_data.get();
   td->on_block_exit_list.push_back(i);
}

// called when a StatementBlock has "on_exit" blocks
block_list_t::iterator popBlock() {
   ThreadData* td = thread_data.get();
   block_list_t::iterator i = td->on_block_exit_list.back();
   td->on_block_exit_list.pop_back();
   return i;
}

// called by each "on_exit" statement to activate its code for the block exit
void advanceOnBlockExit() {
   ThreadData* td = thread_data.get();
   --td->on_block_exit_list.back();
}

// new file name, current parse state
void beginParsing(const char* file, void* ps, const char* src, int offset) {
   ThreadData* td = thread_data.get();
   //printd(5, "beginParsing() td: %p of %p (%s), (stack: %s) src: %s:%d\n", td, file, file ? file : "(null)", (td->plStack ? td->plStack->file : "n/a"), src ? src : "(null)", offset);

   // store current position
   ProgramParseContext* pl = new ProgramParseContext(td->parse_loc.file, td->parseState, td->parse_loc.source, td->parse_loc.offset, td->pcs, td->plStack);
   td->plStack = pl;

   // set new position
   td->parse_loc.file = file;
   td->parseState = ps;
   td->parse_loc.source = src;
   td->parse_loc.offset = offset;
   td->pcs = 0;
}

void* endParsing() {
   ThreadData* td = thread_data.get();
   //printd(5, "endParsing() td: %p restoreParseOptions pgm: %p parse_file: %p '%s' src: %s:%d\n", td, td->current_pgm, td->parse_loc.file, td->parse_loc.file, td->parse_loc.source ? td->parse_loc.source : "(null)", td->parse_loc.offset);
   qore_program_private::restoreParseOptions(td->current_pgm, td->parse_loc.file);

   void* rv = td->parseState;

   // ensure there are no conditional blocks left open at EOF
   td->endFileParsing();

   assert(td->plStack);
   assert(!td->pcs);

   ProgramParseContext* pl = td->plStack->next;
   //printd(5, "endParsing() td: %p ending parsing of '%s', returning %p, setting file: %p '%s'\n", td, td->parse_file, rv, td->plStack->file, td->plStack->file);

   td->parse_loc.file   = td->plStack->file;
   td->parseState       = td->plStack->parseState;
   td->parse_loc.source = td->plStack->src;
   td->parse_loc.offset = td->plStack->offset;
   td->pcs              = td->plStack->pcs;
   delete td->plStack;
   td->plStack = pl;

   return rv;
}

// thread-local functions
bool is_valid_qore_thread() {
   return (bool)thread_data.get();
}

int gettid() {
   return (thread_data.get())->tid;
}

VLock* getVLock() {
   ThreadData* td = thread_data.get();
   return &td->vlock;
}

Context* get_context_stack() {
   return (thread_data.get())->context_stack;
}

void update_context_stack(Context* cstack) {
   ThreadData* td    = thread_data.get();
   td->context_stack = cstack;
}

QoreProgramLocation get_runtime_location() {
   return thread_data.get()->runtime_loc;
}

QoreProgramLocation update_get_runtime_location(const QoreProgramLocation& loc) {
   QoreProgramLocation rv = thread_data.get()->runtime_loc;
   thread_data.get()->runtime_loc = loc;
   return rv;
}

void update_runtime_location(const QoreProgramLocation& loc) {
   thread_data.get()->runtime_loc = loc;
}

void set_parse_file_info(QoreProgramLocation& loc) {
   ThreadData* td = thread_data.get();
   loc.file       = td->parse_loc.file;
   loc.source     = td->parse_loc.source;
   loc.offset     = td->parse_loc.offset;
   //printd(5, "set_parse_file_info() setting %s src: %s:%d\n", loc.file, loc.source ? loc.source : "(null)", loc.offset);
}

QoreProgramLocation get_parse_location() {
   return thread_data.get()->parse_loc;
}

void update_parse_location(const QoreProgramLocation& loc) {
   //printd(5, "update_parse_location() setting %s:%d-%d src: %s%d\n", loc.file, loc.start_line, loc.end_line, loc.source ? loc.source : "(null)", loc.offset);
   thread_data.get()->parse_loc = loc;
}

void update_parse_line_location(int start_line, int end_line) {
   ThreadData* td = thread_data.get();
   td->parse_loc.start_line = start_line;
   td->parse_loc.end_line = end_line;
}

const char* get_parse_code() {
   return (thread_data.get())->parse_code;
}

void parseSetCodeInfo(const char* parse_code, const QoreTypeInfo* returnTypeInfo, const char*& old_code, const QoreTypeInfo*& old_returnTypeInfo) {
   ThreadData* td = thread_data.get();
   old_code = td->parse_code;
   old_returnTypeInfo = td->parse_return_type_info;
   td->parse_code = parse_code;
   td->parse_return_type_info = returnTypeInfo;
}

void parseRestoreCodeInfo(const char* parse_code, const QoreTypeInfo* returnTypeInfo) {
   ThreadData* td = thread_data.get();
   td->parse_code = parse_code;
   td->parse_return_type_info = returnTypeInfo;
}

const QoreTypeInfo* parse_get_return_type_info() {
   return (thread_data.get())->parse_return_type_info;
}

const QoreTypeInfo* getReturnTypeInfo() {
   return (thread_data.get())->returnTypeInfo;
}

const QoreTypeInfo* saveReturnTypeInfo(const QoreTypeInfo* returnTypeInfo) {
   ThreadData* td = thread_data.get();
   const QoreTypeInfo* rv = td->returnTypeInfo;
   td->returnTypeInfo = returnTypeInfo;
   return rv;
}

const AbstractQoreZoneInfo* currentTZ() {
   ThreadData* td = thread_data.get();
   if (td->tpd) {
      if (td->tlpd && td->tlpd->tz_set)
         return td->tlpd->tz;
      if (td->current_pgm)
         return qore_program_private::currentTZIntern(*(td->current_pgm));
   }
   return QTZM.getLocalZoneInfo();
}

void set_thread_tz(const AbstractQoreZoneInfo* tz) {
   ThreadData* td = thread_data.get();
   if (!td->tlpd) {
      printd(0, "set_thread_tz(%p '%s') ignored - no current thread-local program data\n", tz, tz ? tz->getRegionName() : "(null)");
      return;
   }
   td->tlpd->setTZ(tz);
}

const AbstractQoreZoneInfo* get_thread_tz(bool& set) {
   ThreadData* td = thread_data.get();
   if (!td->tlpd) {
      printd(0, "get_thread_tz() ignored - no current thread-local program data\n");
      set = false;
      return 0;
   }
   set = td->tlpd->tz_set;
   return td->tlpd->tz;
}

void clear_thread_tz() {
   ThreadData* td = thread_data.get();
   if (!td->tlpd) {
      printd(0, "clear_thread_tz() ignored - no current thread-local program data\n");
      return;
   }
   td->tlpd->clearTZ();
}

ThreadProgramData* get_thread_program_data() {
   ThreadData* td = thread_data.get();
   assert(td);
   return td->tpd;
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

int get_implicit_element() {
   return thread_data.get()->getElement();
}

int save_implicit_element(int n_element) {
   return thread_data.get()->saveElement(n_element);
}

void end_signal_thread(ExceptionSink* xsink) {
   thread_data.get()->tpd->del(xsink);
}

qore_ns_private* parse_set_ns(qore_ns_private* ns) {
   return thread_data.get()->set_ns(ns);
}

qore_ns_private* parse_get_ns() {
   return thread_data.get()->current_ns;
}

void set_module_context(QoreModuleContext* qmc) {
   thread_data.get()->qmc = qmc;
}

QoreModuleContext* get_module_context() {
   return thread_data.get()->qmc;
}

QoreModuleDefContext* set_module_def_context(QoreModuleDefContext* qmd) {
   ThreadData* td = thread_data.get();
   QoreModuleDefContext* rv = td->qmd;
   td->qmd = qmd;
   return rv;
}

QoreModuleDefContext* get_module_def_context() {
   return thread_data.get()->qmd;
}

void parse_set_module_def_context_name(const char* name) {
   ThreadData* td = thread_data.get();
   if (td->qmd) {
      QoreUserModuleDefContextHelper* uqmd = static_cast<QoreUserModuleDefContextHelper*>(td->qmd);
      // set name and setup for parsing any header closures / call references
      uqmd->setNameInit(name);
   }
}

const char* set_user_module_context_name(const char* n) {
   ThreadData* td = thread_data.get();
   const char* rv = td->user_module_context_name;
   td->user_module_context_name = n;
   return rv;
}

const char* get_user_module_context_name() {
   return thread_data.get()->user_module_context_name;
}

void ModuleContextNamespaceList::clear() {
   for (mcnl_t::iterator i = begin(), e = end(); i != e; ++i)
      delete (*i).nns;
   mcnl_t::clear();
}

void ModuleContextFunctionList::clear() {
   for (mcfl_t::iterator i = begin(), e = end(); i != e; ++i)
      (*i).v->deref();
   mcfl_t::clear();
}

QoreProgramContextHelper::QoreProgramContextHelper(QoreProgram* pgm) {
   ThreadData* td  = thread_data.get();
   old_pgm = td->current_pgm;
   td->current_pgm = pgm;
}

QoreProgramContextHelper::~QoreProgramContextHelper() {
   ThreadData* td  = thread_data.get();
   td->current_pgm = old_pgm;
}

ObjectSubstitutionHelper::ObjectSubstitutionHelper(QoreObject* obj, const qore_class_private* c) {
   ThreadData* td  = thread_data.get();
   old_obj = td->current_obj;
   old_class = td->current_class;
   td->current_obj = obj;
   td->current_class = c;
}

ObjectSubstitutionHelper::~ObjectSubstitutionHelper() {
   ThreadData* td  = thread_data.get();
   td->current_obj = old_obj;
   td->current_class = old_class;
}

OptionalClassObjSubstitutionHelper::OptionalClassObjSubstitutionHelper(const qore_class_private* qc) : subst(qc ? true : false) {
   if (qc) {
      ThreadData* td  = thread_data.get();
      old_obj = td->current_obj;
      old_class = td->current_class;
      td->current_obj = 0;
      td->current_class = qc;
   }
}

OptionalClassObjSubstitutionHelper::~OptionalClassObjSubstitutionHelper() {
   if (subst) {
      ThreadData* td  = thread_data.get();
      td->current_obj = old_obj;
      td->current_class = old_class;
   }
}

CodeContextHelperBase::CodeContextHelperBase(const char* code, QoreObject* obj, const qore_class_private* c, ExceptionSink* xsink) {
   ThreadData* td  = thread_data.get();
   old_code = td->current_code;
   td->current_code = code;

   old_obj = td->current_obj;
   td->current_obj = obj;

   old_class = td->current_class;
   td->current_class = c;

   if (obj && obj != old_obj && !qore_object_private::get(*obj)->startCall(code, xsink))
      do_ref = true;
   else
      do_ref = false;
}

CodeContextHelperBase::~CodeContextHelperBase() {
   ThreadData* td = thread_data.get();
   if (do_ref) {
      assert(td->current_obj);
      qore_object_private::get(*td->current_obj)->endCall(xsink);
   }
   td->current_code = old_code;
   td->current_obj = old_obj;
   td->current_class = old_class;
}

ArgvContextHelper::ArgvContextHelper(QoreListNode* argv, ExceptionSink* n_xsink) : xsink(n_xsink) {
   ThreadData* td  = thread_data.get();
   old_argv = td->current_implicit_arg;
   td->current_implicit_arg = argv;
   //printd(5, "ArgvContextHelper::ArgvContextHelper() setting argv: %p\n", argv);
}

ArgvContextHelper::~ArgvContextHelper() {
   ThreadData* td  = thread_data.get();
   if (td->current_implicit_arg)
      td->current_implicit_arg->deref(xsink);
   td->current_implicit_arg = old_argv;
   //printd(5, "ArgvContextHelper::~ArgvContextHelper() setting argv: %p\n", old_argv);
}

SingleArgvContextHelper::SingleArgvContextHelper(QoreValue val, ExceptionSink* n_xsink) : xsink(n_xsink) {
   //printd(5, "SingleArgvContextHelper::SingleArgvContextHelper() this: %p arg: %p (%s)\n", this, val, val ? val->getTypeName() : 0);
   ThreadData* td  = thread_data.get();
   old_argv = td->current_implicit_arg;
   QoreListNode* argv;
   if (!val.isNothing()) {
      argv = new QoreListNode;
      argv->push(val.takeNode());
   }
   else
      argv = 0;
   td->current_implicit_arg = argv;
}

SingleArgvContextHelper::~SingleArgvContextHelper() {
   ThreadData* td  = thread_data.get();
   if (td->current_implicit_arg)
      td->current_implicit_arg->deref(xsink);
   td->current_implicit_arg = old_argv;
}

const QoreListNode* thread_get_implicit_args() {
   //printd(5, "thread_get_implicit_args() returning %p\n", thread_data.get()->current_implicit_arg);
   return thread_data.get()->current_implicit_arg;
}

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
void pushCall(CallNode* cn) {
   thread_list.pushCall(cn);
}

void popCall(ExceptionSink* xsink) {
   thread_list.popCall(xsink);
}

QoreListNode* getCallStackList() {
   return thread_list.getCallStackList();
}

CallStack* getCallStack() {
   return thread_list.getCallStack();
}
#endif

bool runtime_in_object_method(const char* name, const QoreObject* o) {
   ThreadData* td = thread_data.get();
   return (td->current_obj == o && td->current_code == name) ? true : false;
}

QoreObject* runtime_get_stack_object() {
   return (thread_data.get())->current_obj;
}

const qore_class_private* runtime_get_class() {
   return (thread_data.get())->current_class;
}

void runtime_get_object_and_class(QoreObject*& obj, const qore_class_private*& qc) {
   ThreadData* td = thread_data.get();
   obj = td->current_obj;
   qc = td->current_class;
}

QoreProgramBlockParseOptionHelper::QoreProgramBlockParseOptionHelper(int64 n_po) {
   ThreadData* td = thread_data.get();
   if (td->runtime_po != n_po) {
      po = td->runtime_po;
      td->runtime_po = n_po;
   }
   else
      po = -1;
}

QoreProgramBlockParseOptionHelper::~QoreProgramBlockParseOptionHelper() {
   if (po != -1) {
      ThreadData* td = thread_data.get();
      td->runtime_po = po;
   }
}

ProgramThreadCountContextHelper::ProgramThreadCountContextHelper(ExceptionSink* xsink, QoreProgram* pgm, bool runtime) :
      old_pgm(0), old_tlpd(0), restore(false) {
   if (!pgm)
      return;

   qore_program_private* pp = qore_program_private::get(*pgm);

   ThreadData* td = thread_data.get();
   //printd(5, "ProgramThreadCountContextHelper::ProgramThreadCountContextHelper() current_pgm: %p new_pgm: %p\n", td->current_pgm, pgm);
   if (pgm != td->current_pgm) {
      // try to increment thread count
      if (pp->incThreadCount(xsink)) {
         //printd(5, "ProgramThreadCountContextHelper::ProgramThreadCountContextHelper() failed\n");
         return;
      }
      //printd(5, "ProgramThreadCountContextHelper::ProgramThreadCountContextHelper() OK\n");

      // set up thread stacks
      restore = true;
      old_pgm = td->current_pgm;
      old_tlpd = td->tlpd;
      td->current_pgm = pgm;
      td->tpd->saveProgram(runtime, xsink);
   }
}

ProgramThreadCountContextHelper::~ProgramThreadCountContextHelper() {
   if (!restore)
      return;

   // restore thread stacks
   ThreadData* td = thread_data.get();

   QoreProgram* pgm = td->current_pgm;
   //printd(5, "ProgramThreadCountContextHelper::~ProgramThreadCountContextHelper() current_pgm: %p restoring old pgm: %p old tlpd: %p\n", td->current_pgm, old_pgm, old_tlpd);
   td->current_pgm = old_pgm;
   td->tlpd = old_tlpd;

   qore_program_private::decThreadCount(*pgm, td->tid);
}

ProgramRuntimeParseCommitContextHelper::ProgramRuntimeParseCommitContextHelper(ExceptionSink* xsink, QoreProgram* pgm) :
      old_pgm(0), old_tlpd(0), restore(false) {
   if (!pgm)
      return;

   ThreadData* td = thread_data.get();
   printd(5, "ProgramRuntimeParseCommitContextHelper::ProgramRuntimeParseCommitContextHelper() current_pgm: %p new_pgm: %p\n", td->current_pgm, pgm);
   if (pgm != td->current_pgm) {
      // try to increment thread count
      if (qore_program_private::lockParsing(*pgm, xsink))
         return;

      // set up thread stacks
      restore = true;
      old_pgm = td->current_pgm;
      old_tlpd = td->tlpd;
      td->current_pgm = pgm;
      td->tpd->saveProgram(false, 0);
   }
}

ProgramRuntimeParseCommitContextHelper::~ProgramRuntimeParseCommitContextHelper() {
   if (!restore)
      return;

   // restore thread stacks
   ThreadData* td = thread_data.get();

   QoreProgram* pgm = td->current_pgm;
   //printd(5, "ProgramRuntimeParseCommitContextHelper::~ProgramRuntimeParseCommitContextHelper() current_pgm: %p restoring old pgm: %p old tlpd: %p\n", td->current_pgm, old_pgm, old_tlpd);
   td->current_pgm = old_pgm;
   td->tlpd        = old_tlpd;

   qore_program_private::unlockParsing(*pgm);
}

ProgramRuntimeParseContextHelper::ProgramRuntimeParseContextHelper(ExceptionSink* xsink, QoreProgram* pgm) : restore(false) {
   if (!pgm)
      return;

   // attach to and lock program for parsing
   if (qore_program_private::lockParsing(*pgm, xsink))
      return;

   restore = true;

   ThreadData* td = thread_data.get();
   old_pgm = td->current_pgm;
   td->current_pgm = pgm;
}

ProgramRuntimeParseContextHelper::~ProgramRuntimeParseContextHelper() {
   if (!restore)
      return;

   ThreadData* td = thread_data.get();
   qore_program_private::unlockParsing(*td->current_pgm);
   td->current_pgm = old_pgm;
}

CurrentProgramRuntimeParseContextHelper::CurrentProgramRuntimeParseContextHelper() {
   ThreadData* td = thread_data.get();
   // attach to and lock current program for parsing - cannot fail with a running program
   // but current_pgm can be null when loading binary modules
   if (td->current_pgm)
      qore_program_private::lockParsing(*td->current_pgm, 0);
}

CurrentProgramRuntimeParseContextHelper::~CurrentProgramRuntimeParseContextHelper() {
   ThreadData* td = thread_data.get();
   // current_pgm can be null when loading binary modules
   if (td->current_pgm)
      qore_program_private::unlockParsing(*td->current_pgm);
}

CurrentProgramRuntimeExternalParseContextHelper::CurrentProgramRuntimeExternalParseContextHelper() {
   ThreadData* td = thread_data.get();
   // attach to and lock current program for parsing - cannot fail with a running program
   // but current_pgm can be null when loading binary modules
   if (!td->current_pgm || qore_program_private::lockParsing(*td->current_pgm, 0))
      valid = false;
}

CurrentProgramRuntimeExternalParseContextHelper::~CurrentProgramRuntimeExternalParseContextHelper() {
   if (valid) {
      ThreadData* td = thread_data.get();
      // current_pgm can be null when loading binary modules
      if (td->current_pgm)
         qore_program_private::unlockParsing(*td->current_pgm);
   }
}

CurrentProgramRuntimeExternalParseContextHelper::operator bool() const {
   return valid;
}

ProgramRuntimeParseAccessHelper::ProgramRuntimeParseAccessHelper(ExceptionSink* xsink, QoreProgram* pgm) : restore(false) {
   ThreadData* td = thread_data.get();
   if (pgm != td->current_pgm) {
      if (qore_program_private::incThreadCount(*pgm, xsink))
         return;

      restore = true;
      old_pgm = td->current_pgm;
      td->current_pgm = pgm;
   }
}

ProgramRuntimeParseAccessHelper::~ProgramRuntimeParseAccessHelper() {
   if (!restore)
      return;

   ThreadData* td = thread_data.get();
   qore_program_private::decThreadCount(*td->current_pgm, td->tid);
   td->current_pgm = old_pgm;
}

QoreProgram* getProgram() {
   return (thread_data.get())->current_pgm;
   //return (thread_data.get())->pgmStack->getProgram();
}

RootQoreNamespace* getRootNS() {
   return (thread_data.get())->current_pgm->getRootNS();
   //return (thread_data.get())->pgmStack->getProgram()->getRootNS();
}

int64 parse_get_parse_options() {
   return (thread_data.get())->current_pgm->getParseOptions64();
}

int64 runtime_get_parse_options() {
   return (thread_data.get())->runtime_po;
}

bool parse_check_parse_option(int64 o) {
   return (parse_get_parse_options() & o) == o;
}

bool runtime_check_parse_option(int64 o) {
   return (runtime_get_parse_options() & o) == o;
}

void updateCVarStack(CVNode* ncvs) {
   ThreadData* td = thread_data.get();
   td->cvarstack = ncvs;
}

CVNode* getCVarStack() {
   return (thread_data.get())->cvarstack;
}

void updateVStack(VNode* nvs) {
   ThreadData* td = thread_data.get();
   td->vstack = nvs;
}

VNode* getVStack() {
   return (thread_data.get())->vstack;
}

void save_global_vnode(VNode* vn) {
   ThreadData* td = thread_data.get();
   td->global_vnode = vn;
}

VNode* get_global_vnode() {
   return (thread_data.get())->global_vnode;
}

void setParseClass(QoreClass* c) {
   ThreadData* td = thread_data.get();
   td->parseClass = c;
}

QoreClass* parse_get_class() {
   return (thread_data.get())->parseClass;
}

qore_class_private* parse_get_class_priv() {
   QoreClass* qc = parse_get_class();
   return qc ? qore_class_private::get(*qc) : 0;
}

// to save the exception for "rethrow"
QoreException* catchSwapException(QoreException* e) {
   ThreadData* td = thread_data.get();
   QoreException* old = td->catchException;
   td->catchException = e;
   return old;
}

// for "rethrow"
QoreException* catchGetException() {
   ThreadData* td = thread_data.get();
   //printd(5, "cGE() td: %p e: %p\n", td, td->catchException);
   assert(td->catchException);
   return td->catchException;
}

void qore_exit_process(int rc) {
   // call exit() in a single-threaded process; flushes file buffers, etc
   if (thread_list.getNumThreads() <= 1)
      exit(rc);
   // do not call exit here since it will try to execute cleanup, which will cause crashes
   // in multithreaded programs; call quick_exit() instead (issue
   _Exit(rc);
}

// sets up the signal thread entry in the thread list
int get_signal_thread_entry() {
   return thread_list.getSignalThreadEntry();
}

// returns tid allocated for thread
int get_thread_entry() {
   return thread_list.get();
}

void deregister_thread(int tid) {
   thread_list.release(tid);
}

void deregister_signal_thread() {
   thread_list.release(0);
}

void delete_signal_thread() {
   thread_list.deleteDataReleaseSignalThread();
}

// should only be called from the new thread
void register_thread(int tid, pthread_t ptid, QoreProgram* p, bool foreign) {
   thread_list.activate(tid, ptid, p, foreign);
}

static void qore_thread_cleanup(void* n = 0) {
#ifdef HAVE_MPFR_BUILDOPT_TLS_T
   // only call mpfr_free_cache if MPFR uses TLS
   if (mpfr_buildopt_tls_p())
      mpfr_free_cache();
#endif
   ERR_remove_state(0);
}

int q_register_foreign_thread() {
   // see if the current thread has already been registered
   ThreadData* td = thread_data.get();
   if (td)
      return QFT_REGISTERED;

   // get a TID for the new thread
   int tid = get_thread_entry();

   if (tid == -1)
      return QFT_ERROR;

   thread_list.activate(tid, pthread_self(), 0, true);

   return QFT_OK;
}

int q_deregister_foreign_thread() {
   ThreadData* td = thread_data.get();
   if (!td || !td->foreign)
      return -1;

   // set thread entry as not available while it's being deleted
   thread_list.setStatus(td->tid, QTS_NA);

   ExceptionSink xsink;

   // delete any thread data
   td->del(&xsink);

   // cleanup thread resources
   purge_thread_resources(&xsink);

   xsink.handleExceptions();

   // save tid for freeing the thread entry later
   int tid = td->tid;

   // run any thread cleanup functions
   tclist.exec();

   // delete internal thread data structure and release TID entry
   thread_list.deleteDataRelease(tid);

   qore_thread_cleanup();

   return 0;
}

int q_reserve_foreign_thread_id() {
   return thread_list.get(QTS_RESERVED);
}

int q_release_reserved_foreign_thread_id(int tid) {
   if (tid < 0 || tid >= MAX_QORE_THREADS)
      return -1;

   // release the thread entry
   return thread_list.releaseReserved(tid);
}

int q_register_reserved_foreign_thread(int tid) {
   if (tid < 0 || tid >= MAX_QORE_THREADS)
      return -1;

   return thread_list.activateReserved(tid);
}

int q_deregister_reserved_foreign_thread() {
   ThreadData* td = thread_data.get();
   if (!td || !td->foreign)
      return -1;

   // set thread entry as RESERVED immediately
   thread_list.setStatus(td->tid, QTS_RESERVED);

   ExceptionSink xsink;

   // delete any thread data
   td->del(&xsink);

   // cleanup thread resources
   purge_thread_resources(&xsink);

   xsink.handleExceptions();

   // run any thread cleanup functions
   tclist.exec();

   // delete internal thread data structure (do not release TID entry)
   thread_list.deleteData(td->tid);

   qore_thread_cleanup();

   return 0;
}

class qore_foreign_thread_priv {};

QoreForeignThreadHelper::QoreForeignThreadHelper() : priv(!q_register_foreign_thread() ? (qore_foreign_thread_priv*)1 : 0) {
}

QoreForeignThreadHelper::QoreForeignThreadHelper(int tid) : priv(!q_register_reserved_foreign_thread(tid) ? (qore_foreign_thread_priv*)2 : 0) {
}

QoreForeignThreadHelper::~QoreForeignThreadHelper() {
   if (!priv)
      return;
   if (priv == (qore_foreign_thread_priv*)1)
      q_deregister_foreign_thread();
   else {
      assert(priv == (qore_foreign_thread_priv*)2);
      q_deregister_reserved_foreign_thread();
   }
}

struct ThreadArg {
   q_thread_t f;
   void* arg;
   int tid;

   DLLLOCAL ThreadArg(q_thread_t n_f, void* a, int n_tid) : f(n_f), arg(a), tid(n_tid) {
   }

   DLLLOCAL void run(ExceptionSink* xsink) {
      f(xsink, arg);
   }
};

// put functions in an unnamed namespace to make them 'static extern "C"'
namespace {
   extern "C" void* q_run_thread(void* arg) {
      ThreadArg* ta = (ThreadArg*)arg;

      register_thread(ta->tid, pthread_self(), 0);
      printd(5, "q_run_thread() ta: %p TID %d started\n", ta, ta->tid);

      pthread_cleanup_push(qore_thread_cleanup, (void*)0);

      {
         ExceptionSink xsink;

         {
            ta->run(&xsink);

            // cleanup thread resources
            purge_thread_resources(&xsink);

            // delete any thread data
            thread_data.get()->del(&xsink);

            xsink.handleExceptions();

            printd(4, "q_run_thread(): thread terminating");

            // run any cleanup functions
            tclist.exec();

            // delete internal thread data structure and release TID entry
            thread_list.deleteDataRelease(ta->tid);

            //printd(5, "q_run_thread(): deleting thread params %p\n", ta);
            delete ta;
         }
      }

      pthread_cleanup_pop((int)1);
      thread_counter.dec();
      pthread_exit(0);
      return 0;
   }

   extern "C" void* op_background_thread(void* x) {
      BGThreadParams* btp = (BGThreadParams*) x;
      // register thread
      register_thread(btp->tid, pthread_self(), btp->pgm);
      printd(5, "op_background_thread() btp: %p TID %d started\n", btp, btp->tid);
      //printf("op_background_thread() btp: %p TID %d started\n", btp, btp->tid);

      pthread_cleanup_push(qore_thread_cleanup, (void*)0);

      {
         ExceptionSink xsink;

         // register thread in Program object
         btp->startThread(xsink);

         {
            AbstractQoreNode* rv;
            {
               CodeContextHelper cch(&xsink, CT_NEWTHREAD, "background operator", btp->getCallObject(), btp->class_ctx);

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

            // cleanup thread resources
            purge_thread_resources(&xsink);

            int tid = btp->tid;
            // dereference current Program object
            btp->del();

            // delete any thread data
            thread_data.get()->del(&xsink);

            xsink.handleExceptions();

            printd(4, "thread terminating");

            // run any cleanup functions
            tclist.exec();

            // delete internal thread data structure and release TID entry
            thread_list.deleteDataRelease(tid);
         }
      }

      pthread_cleanup_pop(1);
      thread_counter.dec();
      pthread_exit(0);
      return 0;
   }
}

QoreValue do_op_background(const AbstractQoreNode* left, ExceptionSink* xsink) {
   if (!left)
      return QoreValue();

   //printd(2, "op_background() before crlr left = %p\n", left);
   ReferenceHolder<AbstractQoreNode> nl(copy_and_resolve_lvar_refs(left, xsink), xsink);
   //printd(2, "op_background() after crlr nl = %p\n", nl);
   if (*xsink || !nl)
      return QoreValue();

   // now we are ready to create the new thread

   // get thread entry
   //printd(2, "calling get_thread_entry()\n");
   int tid = get_thread_entry();
   //printd(2, "got %d()\n", tid);

   // if can't start thread, then throw exception
   if (tid == -1) {
      xsink->raiseException("THREAD-CREATION-FAILURE", "thread list is full with %d threads", MAX_QORE_THREADS);
      return QoreValue();
   }

   BGThreadParams* tp = new BGThreadParams(nl.release(), tid, xsink);
   //printd(5, "created BGThreadParams(%p, %d) = %p\n", *nl, tid, tp);
   if (*xsink) {
      deregister_thread(tid);
      return QoreValue();
   }
   //printd(5, "tp = %p\n", tp);
   // create thread
   int rc;
   pthread_t ptid;

   //printd(5, "calling pthread_create(%p, %p, %p, %p)\n", &ptid, &ta_default, op_background_thread, tp);
   thread_counter.inc();

   if ((rc = pthread_create(&ptid, ta_default.get_ptr(), op_background_thread, tp))) {
      tp->cleanup(xsink);
      tp->del();

      thread_counter.dec();
      deregister_thread(tid);
      xsink->raiseErrnoException("THREAD-CREATION-FAILURE", rc, "could not create thread");
      return QoreValue();
   }
   //printd(5, "pthread_create() new thread TID %d, pthread_create() returned %d\n", tid, rc);
   return tid;
}

int q_start_thread(ExceptionSink* xsink, q_thread_t f, void* arg) {
   int tid = get_thread_entry();
   //printd(2, "got %d()\n", tid);

   // if can't start thread, then throw exception
   if (tid == -1) {
      xsink->raiseException("THREAD-CREATION-FAILURE", "thread list is full with %d threads", MAX_QORE_THREADS);
      return -1;
   }

   ThreadArg* ta = new ThreadArg(f, arg, tid);

   //printd(5, "tp = %p\n", tp);
   // create thread
   int rc;
   pthread_t ptid;

   //printd(5, "calling pthread_create(%p, %p, %p, %p)\n", &ptid, &ta_default, op_background_thread, tp);
   thread_counter.inc();
   if ((rc = pthread_create(&ptid, ta_default.get_ptr(), q_run_thread, ta))) {
      delete ta;
      thread_counter.dec();
      deregister_thread(tid);
      xsink->raiseErrnoException("THREAD-CREATION-FAILURE", rc, "could not create thread");
      return -1;
   }

   return tid;
}

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
#include <qore/QoreRWLock.h>

#ifdef _Q_WINDOWS
extern QoreRWLock* thread_stack_lock;
#else
extern QoreRWLock thread_stack_lock;
#endif
#endif

static int initial_thread;

void init_qore_threads() {
   QORE_TRACE("qore_init_threads()");

#if defined(QORE_RUNTIME_THREAD_STACK_TRACE) && defined(_Q_WINDOWS)
   thread_stack_lock = new QoreRWLock;
#endif

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
#ifdef _Q_WINDOWS
   // windows stacks are extended automatically; here we set a limit of 1 MB per thread
   qore_thread_stack_size = 1024 * 1024;
#else // !_Q_WINDOWS && !SOLARIS
   qore_thread_stack_size = ta_default.getstacksize();
   assert(qore_thread_stack_size);
   //printd(5, "getstacksize() returned: %ld\n", qore_thread_stack_size);
#endif // #ifdef _Q_WINDOWS
#endif // #ifdef SOLARIS

#ifdef IA64_64
   // the top half of the stack is for the normal stack, the bottom half is for the register stack
   qore_thread_stack_size /= 2;
#endif // #ifdef IA64_64
   qore_thread_stack_limit = qore_thread_stack_size - QORE_STACK_GUARD;
   //printd(8, "default stack size %ld, limit %ld\n", qore_thread_stack_size, qore_thread_stack_limit);
#endif // #ifdef QORE_MANAGE_STACK

   // setup parent thread data
   thread_list.activate(initial_thread = get_thread_entry());

   // initialize recursive mutex attribute
   pthread_mutexattr_init(&ma_recursive);
   pthread_mutexattr_settype(&ma_recursive, PTHREAD_MUTEX_RECURSIVE);

   // mark threading as active
   threads_initialized = true;
}

QoreNamespace* get_thread_ns(QoreNamespace &qorens) {
   // create Qore::Thread namespace
   QoreNamespace* Thread = new QoreNamespace("Thread");

   Thread->addSystemClass(initQueueClass(*Thread));
   Thread->addSystemClass(initAbstractSmartLockClass(*Thread));
   Thread->addSystemClass(initMutexClass(*Thread));
   Thread->addSystemClass(initConditionClass(*Thread));
   Thread->addSystemClass(initRWLockClass(*Thread));
   Thread->addSystemClass(initGateClass(*Thread));
   Thread->addSystemClass(initSequenceClass(*Thread));
   Thread->addSystemClass(initCounterClass(*Thread));

   Thread->addSystemClass(initAutoLockClass(*Thread));
   Thread->addSystemClass(initAutoGateClass(*Thread));
   Thread->addSystemClass(initAutoReadLockClass(*Thread));
   Thread->addSystemClass(initAutoWriteLockClass(*Thread));

   Thread->addSystemClass(initThreadPoolClass(*Thread));

   Thread->addSystemClass(initAbstractThreadResourceClass(*Thread));

   return Thread;
}

void delete_thread_local_data() {
   ThreadData* td = thread_data.get();

   // clear runtime location
   td->runtime_loc.clear();

   ExceptionSink xsink;
   // delete any thread data
   thread_data.get()->del(&xsink);

   purge_thread_resources(&xsink);
   xsink.handleExceptions();
}

void delete_qore_threads() {
   QORE_TRACE("delete_qore_threads()");

   // mark threading as inactive
   threads_initialized = false;

   pthread_mutexattr_destroy(&ma_recursive);

   assert(initial_thread);
   thread_list.deleteDataRelease(initial_thread);

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
#ifdef _Q_WINDOWS
   delete thread_stack_lock;
#endif
#endif

#ifdef HAVE_MPFR_BUILDOPT_TLS_T
   // only call mpfr_free_cache if MPFR uses TLS
   if (mpfr_buildopt_tls_p())
      mpfr_free_cache();
#endif
}

QoreListNode* get_thread_list() {
   QoreListNode* l = new QoreListNode;

   QoreThreadListIterator i;

   while (i.next())
      l->push(new QoreBigIntNode(*i));

   return l;
}

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
#include <qore/QoreRWLock.h>

#ifdef _Q_WINDOWS
extern QoreRWLock* thread_stack_lock;
#else
extern QoreRWLock thread_stack_lock;
#endif

QoreHashNode* getAllCallStacks() {
   return thread_list.getAllCallStacks();
}

QoreHashNode* QoreThreadList::getAllCallStacks() {
   QoreHashNode* h = new QoreHashNode;
   QoreString str;

   // grab the call stack write lock
   QoreAutoRWWriteLocker wl(thread_stack_lock);

   QoreThreadListIterator i;
   if (exiting)
      return h;

   while (i.next()) {
      // get call stack
      if (entry[*i].callStack) {
         QoreListNode* l = entry[*i].callStack->getCallStack();
         if (!l->empty()) {
            // make hash entry
            str.clear();
            str.sprintf("%d", *i);
            h->setKeyValue(str.getBuffer(), l, 0);
         }
         else
            l->deref(0);
      }
   }

   return h;
}
#endif

void ThreadEntry::cleanup() {
   //printf("ThreadEntry::cleanup() TID %d\n", tidnode ? tidnode->tid : 0);
   assert(status != QTS_AVAIL);
   // delete tidnode from tid_list
   delete tidnode;

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   // delete call stack
   delete callStack;
#ifdef DEBUG
   callStack = 0;
#endif
#endif
#ifdef DEBUG
   assert(!thread_data);
#endif

   if (status != QTS_NA && status != QTS_RESERVED) {
      if (!joined)
         pthread_detach(ptid);
   }
   status = QTS_AVAIL;
}

void QoreThreadList::deleteData(int tid) {
   delete thread_data.get();
   thread_data.set(0);

#ifdef DEBUG
   AutoLocker al(l);
   entry[tid].thread_data = 0;
#endif
}

void QoreThreadList::deleteDataRelease(int tid) {
   delete thread_data.get();
   thread_data.set(0);

   AutoLocker al(l);
#ifdef DEBUG
   entry[tid].thread_data = 0;
#endif

   releaseIntern(tid);
}

void QoreThreadList::deleteDataReleaseSignalThread() {
   thread_data.get()->del(0);
   deleteDataRelease(0);
}

unsigned QoreThreadList::cancelAllActiveThreads() {
   int tid = gettid();

   // thread cancel count
   unsigned tcc = 0;

   QoreThreadListIterator i;

   assert(!exiting);
   exiting = true;

   while (i.next()) {
      if (*i != (unsigned)tid) {
         //printf("QoreThreadList::cancelAllActiveThreads() canceling TID %d ptid: %p (this TID: %d)\n", *i, entry[*i].ptid, tid);
         int trc = pthread_cancel(entry[*i].ptid);
         if (!trc)
            ++tcc;
#ifdef DEBUG
         else
            printd(0, "pthread_cancel() returned %d (%s) on tid %d (%p)\n", trc, strerror(trc), tid, entry[*i].ptid);
#endif
      }
   }

   return tcc;
}

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
void QoreThreadList::pushCall(CallNode* cn) {
   entry[gettid()].callStack->push(cn);
}

void QoreThreadList::popCall(ExceptionSink* xsink) {
   entry[gettid()].callStack->pop(xsink);
}

QoreListNode* QoreThreadList::getCallStackList() {
   return entry[gettid()].callStack->getCallStack();
}
#endif
