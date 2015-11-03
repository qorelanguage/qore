/*
  qore_thread_intern.h

  POSIX thread library for Qore

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

#ifndef _QORE_QORE_THREAD_INTERN_H
#define _QORE_QORE_THREAD_INTERN_H

// FIXME: move to config.h or something like that
// not more than this number of threads can be running at the same time
#ifndef MAX_QORE_THREADS
#define MAX_QORE_THREADS 0x1000
#endif

#ifndef QORE_THREAD_STACK_SIZE
#define QORE_THREAD_STACK_SIZE 1024*512
#endif

// the values here are subject to change and come from purely empirical testing
#ifndef QORE_STACK_GUARD
#ifdef CPU_X86_64
// for some reason we need 20K of stack guard on x86_64
#define QORE_STACK_GUARD (1024 * 20)
#else

#ifdef SPARC
// also we need at least 22K of stack guard on sparc for background threads for some reason
#define QORE_STACK_GUARD (1024 * 22)
#else

#if defined(HPUX) && !defined(__ia64) && !defined(__LP64__)
// need 10KB on HPUX on 32-bit pa-risc
#define QORE_STACK_GUARD (1024 * 10)
#else

// "generic" value (tested on OSX i386 and ppc and Linux i386)
#define QORE_STACK_GUARD (1024 * 8)
#endif // HPUX

#endif // SPARC
#endif // CPU_X86_64
#endif // QORE_STACK_GUARD

class Operator;
class Context;
class CVNode;
class CallNode;
class CallStack;
class LocalVarValue;
class ClosureParseEnvironment;
class ClosureRuntimeEnvironment;
class ClosureVarValue;

DLLLOCAL extern Operator *OP_BACKGROUND;

enum qore_call_t {
   CT_USER       = 0,
   CT_BUILTIN    = 1,
   CT_NEWTHREAD  = 2,
   CT_RETHROW    = 3
};

class VNode;

DLLLOCAL void purge_thread_resources(ExceptionSink *xsink);
DLLLOCAL void beginParsing(char *file, void *ps = NULL);
DLLLOCAL void *endParsing();
DLLLOCAL Context *get_context_stack();
DLLLOCAL void update_context_stack(Context *cstack);
DLLLOCAL void get_pgm_counter(int &start_line, int &end_line);
DLLLOCAL const char *get_pgm_file();
DLLLOCAL void update_pgm_counter_pgm_file(int start_line, int end_line, const char *f);
DLLLOCAL void get_parse_location(int &start_line, int &end_line);
DLLLOCAL const char *get_parse_file();
DLLLOCAL void update_parse_location(int start_line, int end_line);
DLLLOCAL void update_parse_location(int start_line, int end_line, const char *f);
DLLLOCAL bool inMethod(const char *name, const QoreObject *o);
DLLLOCAL void pushProgram(QoreProgram *pgm);
DLLLOCAL void popProgram();
DLLLOCAL RootQoreNamespace *getRootNS();
DLLLOCAL int getParseOptions();
DLLLOCAL void updateCVarStack(CVNode *ncvs);
DLLLOCAL CVNode *getCVarStack();
DLLLOCAL void updateVStack(VNode *nvs);
DLLLOCAL VNode *getVStack();
DLLLOCAL QoreObject *getStackObject();
DLLLOCAL void setParseClass(QoreClass *c);
DLLLOCAL QoreClass *getParseClass();
DLLLOCAL void substituteObjectIfEqual(QoreObject *o);
DLLLOCAL QoreObject *substituteObject(QoreObject *o);
DLLLOCAL void catchSaveException(QoreException *e);
DLLLOCAL QoreException *catchGetException();
DLLLOCAL VLock *getVLock();

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
DLLLOCAL void pushCall(CallNode *cn);
DLLLOCAL void popCall(ExceptionSink *xsink);
DLLLOCAL CallStack *getCallStack();
DLLLOCAL QoreListNode *getCallStackList();
#else
#ifdef __GNUC__
#define pushCall(args...)
#else
#define pushCall(args, ...)
#endif
#define popCall(x)
#endif

// acquires a TID and thread entry, returns -1 if not successful
DLLLOCAL int get_thread_entry();
// acquires TID 0 and sets up the signal thread entry, always returns 0
DLLLOCAL int get_signal_thread_entry();
DLLLOCAL void delete_thread_data();
DLLLOCAL void register_thread(int tid, pthread_t ptid, QoreProgram *pgm);
DLLLOCAL void deregister_thread(int tid);
DLLLOCAL void deregister_signal_thread();

// returns 1 if data structure is already on stack, 0 if not (=OK)
DLLLOCAL int thread_push_container(const AbstractQoreNode *n);
DLLLOCAL void thread_pop_container(const AbstractQoreNode *n);

// called when a StatementBlock has "on block exit" blocks
DLLLOCAL void pushBlock(block_list_t::iterator i);
// called when a StatementBlock has "on block exit" blocks
DLLLOCAL block_list_t::iterator popBlock();
// called by each "on_block_exit" statement to activate it's code for the block exit
DLLLOCAL void advanceOnBlockExit();

DLLLOCAL LocalVarValue *thread_instantiate_lvar();
DLLLOCAL void thread_uninstantiate_lvar(ExceptionSink *xsink);

DLLLOCAL void thread_set_closure_parse_env(ClosureParseEnvironment *cenv);
DLLLOCAL ClosureParseEnvironment *thread_get_closure_parse_env();

DLLLOCAL ClosureVarValue *thread_instantiate_closure_var(const char *id, AbstractQoreNode *value);
DLLLOCAL ClosureVarValue *thread_instantiate_closure_var(const char *id, AbstractQoreNode *vexp, QoreObject *obj);
DLLLOCAL void thread_uninstantiate_closure_var(ExceptionSink *xsink);
DLLLOCAL ClosureVarValue *thread_find_closure_var(const char *id);
DLLLOCAL ClosureVarValue *thread_get_runtime_closure_var(const LocalVar *id);

DLLLOCAL ClosureRuntimeEnvironment *thread_get_runtime_closure_env();
DLLLOCAL void thread_set_runtime_closure_env(ClosureRuntimeEnvironment *cenv);

class QoreContainerHelper {
   const AbstractQoreNode *n;
   bool err;

  public:
   DLLLOCAL QoreContainerHelper(const AbstractQoreNode *n_n) {
      // FIXME! need to have an AbstactQoreNode::isContainer() function!
      qore_type_t t = n_n ? n_n->getType() : NT_NOTHING;
      if ((t == NT_LIST || t == NT_HASH || t == NT_OBJECT || t >= QORE_NUM_TYPES)) {
	 if (!thread_push_container(n_n)) {
	    n = n_n;	    
	    err = false;
	 }
	 else {
	    n = 0;
	    err = true;
	 }
      }
      else {
	 n = 0;
	 err = false;
      }
   }
   DLLLOCAL ~QoreContainerHelper() {
      if (n)
	 thread_pop_container(n);
   }
   DLLLOCAL operator bool () const {
      return !err;
   }
};

class QoreClosureRuntimeEnvironmentHelper {
   private:
      ClosureRuntimeEnvironment *cenv;
      
   public:
      DLLLOCAL QoreClosureRuntimeEnvironmentHelper(ClosureRuntimeEnvironment *n_cenv)
      {
	 cenv = thread_get_runtime_closure_env();
	 thread_set_runtime_closure_env(n_cenv);
      }

      DLLLOCAL ~QoreClosureRuntimeEnvironmentHelper()
      {
	 thread_set_runtime_closure_env(cenv);
      }
};

DLLLOCAL const QoreListNode *thread_get_implicit_args();

#ifndef HAVE_UNLIMITED_THREAD_KEYS
DLLLOCAL LocalVarValue *thread_find_lvar(const char *id);
#endif

// for object implementation
DLLLOCAL QoreObject *getStackObject();
// for methods that behave differently when called within the method itself
DLLLOCAL bool inMethod(const char *name, const QoreObject *o);

class CodeContextHelper {
   private:
      const char *old_code;
      QoreObject *old_obj;
      ExceptionSink *xsink;
	 
   public:
      DLLLOCAL CodeContextHelper(const char *code = NULL, const QoreObject *obj = NULL, ExceptionSink *xs = NULL);
      DLLLOCAL ~CodeContextHelper();
};

class ObjectSubstitutionHelper {
   private:
      QoreObject *old_obj;

   public:
      DLLLOCAL ObjectSubstitutionHelper(QoreObject *obj);
      DLLLOCAL ~ObjectSubstitutionHelper();
};

class ProgramContextHelper {
   private:
      QoreProgram *old_pgm;
      ProgramContextHelper *last;
      ExceptionSink *xsink;  // to keep for uninstantiating program thread-local variables if necessary
      bool restore;
   
   public:
      DLLLOCAL ProgramContextHelper(QoreProgram *pgm, ExceptionSink *xsink);
      DLLLOCAL ~ProgramContextHelper();
};

class ArgvContextHelper {
   private:
      QoreListNode *old_argv;
      ExceptionSink *xsink;
   
   public:
      DLLLOCAL ArgvContextHelper(QoreListNode *argv, ExceptionSink *n_xsink);
      // calls deref(xsink) on list in destructor
      DLLLOCAL ~ArgvContextHelper();
};

class SingleArgvContextHelper {
   private:
      QoreListNode *old_argv;
      ExceptionSink *xsink;
   
   public:
      DLLLOCAL SingleArgvContextHelper(const AbstractQoreNode *val, ExceptionSink *n_xsink);
      // calls deref(xsink) on list in destructor
      DLLLOCAL ~SingleArgvContextHelper();
};

#include <qore/intern/CallStack.h>

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
class CallStackHelper : public CallNode {
      ExceptionSink *xsink;

      // not implemented
      DLLLOCAL CallStackHelper(const CallStackHelper&);
      DLLLOCAL CallStackHelper& operator=(const CallStackHelper&);
      DLLLOCAL void *operator new(size_t);
      
   public:
      DLLLOCAL CallStackHelper(const char *f, int t, QoreObject *o, ExceptionSink *n_xsink) : CallNode(f, t, o), xsink(n_xsink)
      {
	 pushCall(this);
      }
      DLLLOCAL ~CallStackHelper()
      {
	 popCall(xsink);
      }
};
#endif

DLLLOCAL void init_qore_threads();
DLLLOCAL QoreNamespace *get_thread_ns();
DLLLOCAL void delete_qore_threads();
DLLLOCAL QoreListNode *get_thread_list();
DLLLOCAL QoreHashNode *getAllCallStacks();

class QorePThreadAttr {
   private:
      pthread_attr_t attr;

   public:
      DLLLOCAL QorePThreadAttr() {
	 pthread_attr_init(&attr);
	 pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
      }

      DLLLOCAL ~QorePThreadAttr() {
	 //printd(2, "calling pthread_attr_destroy(%08p)\n", &attr);
	 pthread_attr_destroy(&attr);
	 //printd(2, "returned from pthread_attr_destroy(%08p)\n", &attr);
      }

#ifdef HAVE_PTHREAD_ATTR_GETSTACK
      DLLLOCAL void getstack(void *&ptr, size_t &ssize) {
	 pthread_attr_getstack(&attr, &ptr, &ssize);
      }
#endif
      
      DLLLOCAL size_t getstacksize() const {
	 size_t ssize;
	 pthread_attr_getstacksize(&attr, &ssize);
	 return ssize;
      }

      DLLLOCAL int setstacksize(size_t ssize) {
	 return pthread_attr_setstacksize(&attr, ssize);
      }

      DLLLOCAL pthread_attr_t *get_ptr() {
	 return &attr;
      }
};

DLLLOCAL extern QorePThreadAttr ta_default;

#ifdef QORE_MANAGE_STACK
DLLLOCAL int check_stack(ExceptionSink *xsink);
#endif

#endif
