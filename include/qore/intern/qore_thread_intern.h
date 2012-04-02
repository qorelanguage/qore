/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_thread_intern.h

  POSIX thread library for Qore

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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

#include <vector>

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
struct ClosureVarValue;
class VLock;
class ConstantEntry;
class qore_ns_private;

DLLLOCAL extern Operator *OP_BACKGROUND;

class VNode;
class AbstractQoreZoneInfo;
class ThreadData;

struct ModuleContextCommit {
   class qore_ns_private* parent;
   class qore_ns_private* nns;

   DLLLOCAL ModuleContextCommit(class qore_ns_private* n_parent, class qore_ns_private* n_nns) : parent(n_parent), nns(n_nns) {
   }
};

typedef std::vector<ModuleContextCommit> mcl_t;

class QoreModuleContext {
protected:
   QoreStringNode* err;

public:
   mcl_t mcl;

   DLLLOCAL QoreModuleContext() : err(0) {
   }

   DLLLOCAL ~QoreModuleContext() {
      assert(!err);
   }

   DLLLOCAL void error(const char* fmt, ...);

   DLLLOCAL QoreStringNode* takeError() {
      QoreStringNode* rv = err;
      err = 0;
      return rv;
   }
};

// returns 0 if the last mark has been cleared, -1 if there are more marks to check
DLLLOCAL int purge_thread_resources_to_mark(ExceptionSink *xsink);
DLLLOCAL void purge_thread_resources(ExceptionSink *xsink);
DLLLOCAL void mark_thread_resources();
DLLLOCAL void beginParsing(char *file, void *ps = NULL);
DLLLOCAL void *endParsing();
DLLLOCAL Context *get_context_stack();
DLLLOCAL void update_context_stack(Context *cstack);
DLLLOCAL const char *get_pgm_counter(int &start_line, int &end_line);
DLLLOCAL const char *get_pgm_file();
DLLLOCAL void update_pgm_counter_pgm_file(int start_line, int end_line, const char *f);
DLLLOCAL void get_parse_location(int &start_line, int &end_line);
DLLLOCAL const char *get_parse_file();
DLLLOCAL const char *get_parse_code();
DLLLOCAL void update_parse_location(int start_line, int end_line);
DLLLOCAL void update_parse_location(int start_line, int end_line, const char *f);
DLLLOCAL bool inMethod(const char *name, const QoreObject *o);
DLLLOCAL RootQoreNamespace *getRootNS();
DLLLOCAL int64 getParseOptions();
DLLLOCAL void updateCVarStack(CVNode *ncvs);
DLLLOCAL CVNode *getCVarStack();
DLLLOCAL void updateVStack(VNode *nvs);
DLLLOCAL VNode *getVStack();
DLLLOCAL QoreObject *getStackObject();
DLLLOCAL void setParseClass(QoreClass *c);
DLLLOCAL QoreClass *getParseClass();
DLLLOCAL void substituteObjectIfEqual(QoreObject *o);
DLLLOCAL QoreObject *substituteObject(QoreObject *o);
DLLLOCAL QoreException* catchSwapException(QoreException *e);
DLLLOCAL QoreException* catchGetException();
DLLLOCAL VLock *getVLock();
DLLLOCAL void end_signal_thread(ExceptionSink *xsink);
DLLLOCAL void delete_thread_local_data();
DLLLOCAL void parse_cond_push(bool mark = false);
DLLLOCAL bool parse_cond_else();
DLLLOCAL bool parse_cond_pop();
DLLLOCAL void push_parse_options();
DLLLOCAL void parse_push_namespace_name(const char* name);
DLLLOCAL const char* parse_pop_namespace_name();
DLLLOCAL void parse_push_class_name(const char* name);
DLLLOCAL const char* parse_pop_class_name();
DLLLOCAL qore_ns_private* parse_set_ns(qore_ns_private* ns);
DLLLOCAL qore_ns_private* parse_get_ns();
DLLLOCAL void set_module_context(QoreModuleContext* qmc);
DLLLOCAL QoreModuleContext* get_module_context();

// pushes a new argv reference counter
DLLLOCAL void new_argv_ref();

// increments the parse argv reference counter
DLLLOCAL void inc_argv_ref();

// pushes an "ignore numeric reference" context
DLLLOCAL void push_ignore_numeric_argv_ref();

// pops an "ignore numeric reference" context
DLLLOCAL void pop_ignore_numeric_argv_ref();

// increments the parse argv reference counter for numeric references (ex: $1)
DLLLOCAL void inc_numeric_argv_ref();

// gets the parse argv reference counter and pops the context
DLLLOCAL int get_pop_argv_ref();

// clears the argv reference stack
DLLLOCAL void clear_argv_ref();

// ignore further local references to $argv
//DLLLOCAL void ignore_local_argv();

DLLLOCAL int set_constant(ConstantEntry *ce);
DLLLOCAL void remove_constant(ConstantEntry *ce);

DLLLOCAL void parseSetCodeInfo(const char *parse_code, const QoreTypeInfo *returnTypeInfo, const char *&old_code, const QoreTypeInfo *&old_returnTypeInfo);
DLLLOCAL void parseRestoreCodeInfo(const char *parse_code, const QoreTypeInfo *returnTypeInfo);
// sets the new type and returns the old
DLLLOCAL const QoreTypeInfo *saveReturnTypeInfo(const QoreTypeInfo *returnTypeInfo);
DLLLOCAL const QoreTypeInfo *getReturnTypeInfo();

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

class QoreParseClassHelper {
protected:
   QoreClass *old;
public:
   DLLLOCAL QoreParseClassHelper(QoreClass *cls) {
      old = getParseClass();
      setParseClass(cls);
   }
   DLLLOCAL ~QoreParseClassHelper() {
      setParseClass(old);
   }
};

class QoreProgramLocationHelper {
protected:
   int start_line, end_line;
   const char *file;
public:
   DLLLOCAL QoreProgramLocationHelper() {
      file = get_pgm_counter(start_line, end_line);
   }
   DLLLOCAL ~QoreProgramLocationHelper() {
      update_pgm_counter_pgm_file(start_line, end_line, file);
   }
};

// acquires a TID and thread entry, returns -1 if not successful
DLLLOCAL int get_thread_entry();
// acquires TID 0 and sets up the signal thread entry, always returns 0
DLLLOCAL int get_signal_thread_entry();
DLLLOCAL void deregister_signal_thread();
DLLLOCAL void register_thread(int tid, pthread_t ptid, QoreProgram *pgm);
DLLLOCAL void deregister_thread(int tid);
DLLLOCAL void delete_signal_thread();

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
DLLLOCAL ClosureVarValue *thread_instantiate_closure_var(const char *id, AbstractQoreNode *vexp, QoreObject *obj, QoreProgram *pgm);
DLLLOCAL void thread_uninstantiate_closure_var(ExceptionSink *xsink);
DLLLOCAL ClosureVarValue *thread_find_closure_var(const char *id);
DLLLOCAL ClosureVarValue *thread_get_runtime_closure_var(const LocalVar *id);

DLLLOCAL ClosureRuntimeEnvironment *thread_get_runtime_closure_env();
DLLLOCAL void thread_set_runtime_closure_env(ClosureRuntimeEnvironment *cenv);

DLLLOCAL int get_implicit_element();
DLLLOCAL int save_implicit_element(int n_element);

DLLLOCAL void save_global_vnode(VNode *vn);
DLLLOCAL VNode *get_global_vnode();

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
   DLLLOCAL QoreClosureRuntimeEnvironmentHelper(ClosureRuntimeEnvironment *n_cenv) {
      cenv = thread_get_runtime_closure_env();
      thread_set_runtime_closure_env(n_cenv);
   }
   
   DLLLOCAL ~QoreClosureRuntimeEnvironmentHelper() {
      thread_set_runtime_closure_env(cenv);
   }
};

DLLLOCAL const QoreListNode *thread_get_implicit_args();

DLLLOCAL LocalVarValue *thread_find_lvar(const char *id);

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

class ThreadLocalVariableData;
class ThreadClosureVariableStack;

class ProgramContextHelper {
private:
   QoreProgram *old_pgm;
   ThreadLocalVariableData *old_lvstack;
   ThreadClosureVariableStack *old_cvstack;
   bool restore;
   
public:
   DLLLOCAL ProgramContextHelper(QoreProgram *pgm, bool runtime = true);
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

class ImplicitElementHelper {
private:
   int element;

public:
   DLLLOCAL ImplicitElementHelper(int n_element) : element(save_implicit_element(n_element)) {
   }
   DLLLOCAL ~ImplicitElementHelper() {
      save_implicit_element(element);
   }
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
   DLLLOCAL CallStackHelper(const char *f, int t, QoreObject *o, ExceptionSink *n_xsink) : CallNode(f, t, o), xsink(n_xsink) {
      pushCall(this);
   }
   DLLLOCAL ~CallStackHelper() {
      popCall(xsink);
   }
};
#define CODE_CONTEXT_HELPER(type, name, self, xsink) CodeContextHelper cch_auto(name, self, xsink); CallStackHelper csh_auto(name, type, self, xsink)
#else
#define CODE_CONTEXT_HELPER(type, name, self, xsink) CodeContextHelper cch_auto(name, self, xsink)
#endif

DLLLOCAL void init_qore_threads();
DLLLOCAL QoreNamespace *get_thread_ns(QoreNamespace& qorens);
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

class ParseCodeInfoHelper {
private:
   const char *parse_code;
   const QoreTypeInfo *returnTypeInfo;

public:
   DLLLOCAL ParseCodeInfoHelper(const char *n_parse_code, const QoreTypeInfo *n_returnTypeInfo) {
      parseSetCodeInfo(n_parse_code, n_returnTypeInfo, parse_code, returnTypeInfo);
   }
   DLLLOCAL ~ParseCodeInfoHelper() {
      parseRestoreCodeInfo(parse_code, returnTypeInfo);
   }
};

class NamespaceParseContextHelper {
private:
   qore_ns_private* ns;
   bool restore;

public:
   DLLLOCAL NamespaceParseContextHelper(qore_ns_private* n_ns) : ns(parse_set_ns(n_ns)), restore(ns != n_ns) {
   }
   DLLLOCAL ~NamespaceParseContextHelper() {
      if (restore)
         parse_set_ns(ns);
   }
};

class ThreadData;

class ThreadProgramData : public QoreReferenceCounter {
private:
   // for the set of QoreProgram objects we have local variables in
   typedef std::set<QoreProgram *> pgm_set_t;
   pgm_set_t pgm_set;

   // lock for pgm_set data structure (which is accessed from multiple threads when QorePrograms deregister themselves)
   QoreThreadLock pslock;

   ThreadData *td;

   DLLLOCAL void ref() {
      ROreference();
   }

   DLLLOCAL ~ThreadProgramData() {
      assert(pgm_set.empty());
   }

public:
   DLLLOCAL ThreadProgramData(ThreadData *n_td) : td(n_td) {
   }

   DLLLOCAL void delProgram(QoreProgram *pgm);
   DLLLOCAL void saveProgram(bool runtime);
   DLLLOCAL void del(ExceptionSink *xsink);

   DLLLOCAL void deref() {
      if (ROdereference())
         delete this;
   }
};

DLLLOCAL extern pthread_mutexattr_t ma_recursive;

#endif
