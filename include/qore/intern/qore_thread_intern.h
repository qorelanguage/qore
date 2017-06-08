/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_thread_intern.h

  POSIX thread library for Qore

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

#ifndef _QORE_QORE_THREAD_INTERN_H
#define _QORE_QORE_THREAD_INTERN_H

#include <vector>
#include <set>
#include <map>

#ifndef QORE_THREAD_STACK_SIZE
#define QORE_THREAD_STACK_SIZE 1024*512
#endif

// the values here are subject to change and come from purely empirical testing
#ifndef QORE_STACK_GUARD
#ifdef CPU_X86_64
// for some reason we need 32K of stack guard on x86_64
#define QORE_STACK_GUARD (1024 * 32)
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
class LocalVar;
class LocalVarValue;
class ClosureParseEnvironment;
class QoreClosureBase;
struct ClosureVarValue;
class VLock;
class ConstantEntry;
class qore_ns_private;
class qore_root_ns_private;
class qore_class_private;
class AbstractQoreFunctionVariant;
class AbstractQoreZoneInfo;
class ThreadProgramData;
class QoreAbstractModule;

DLLLOCAL extern Operator* OP_BACKGROUND;

class VNode;
class AbstractQoreZoneInfo;
class ThreadData;

struct ModuleContextNamespaceCommit {
   qore_ns_private* parent;
   qore_ns_private* nns;

   DLLLOCAL ModuleContextNamespaceCommit(qore_ns_private* n_parent, qore_ns_private* n_nns) : parent(n_parent), nns(n_nns) {
   }
};

typedef std::vector<ModuleContextNamespaceCommit> mcnl_t;

class ModuleContextNamespaceList : public mcnl_t {
private:
   // not implemented
   DLLLOCAL ModuleContextNamespaceList(const ModuleContextNamespaceList&);

public:
   DLLLOCAL ModuleContextNamespaceList() {
   }

   DLLLOCAL ~ModuleContextNamespaceList() {
      assert(empty());
   }

   DLLLOCAL void clear();
};

struct ModuleContextFunctionCommit {
   qore_ns_private* parent;
   const char* name;
   AbstractQoreFunctionVariant* v;

   DLLLOCAL ModuleContextFunctionCommit(qore_ns_private* n_parent, const char* n_name, AbstractQoreFunctionVariant* n_v) : parent(n_parent), name(n_name), v(n_v) {
   }
};

typedef std::vector<ModuleContextFunctionCommit> mcfl_t;

class ModuleContextFunctionList : public mcfl_t {
private:
   // not implemented
   DLLLOCAL ModuleContextFunctionList(const ModuleContextFunctionList&);

public:
   DLLLOCAL ModuleContextFunctionList() {
   }

   DLLLOCAL ~ModuleContextFunctionList() {
      assert(empty());
   }

   DLLLOCAL void clear();
};

class QoreModuleContext {
protected:
   const char* name;
   qore_root_ns_private* rns;
   QoreStringNode* err;
   ExceptionSink& xsink;

public:
   ModuleContextNamespaceList mcnl;
   ModuleContextFunctionList mcfl;

   DLLLOCAL QoreModuleContext(const char* n, qore_root_ns_private* n_rns, ExceptionSink& xs) : name(n), rns(n_rns), err(0), xsink(xs) {
   }

   DLLLOCAL ~QoreModuleContext() {
      assert(!err);
   }

   DLLLOCAL void error(const char* fmt, ...);

   DLLLOCAL bool hasError() const {
      return xsink;
   }

   DLLLOCAL void commit();

   DLLLOCAL void rollback() {
      mcnl.clear();
      mcfl.clear();
   }

   DLLLOCAL qore_root_ns_private* getRootNS() {
      return rns;
   }
};

class QoreClosureParseNode;

class QoreModuleDefContext {
protected:
   DLLLOCAL void initClosure(const QoreProgramLocation& loc, AbstractQoreNode*& c, const char* n);

public:
   typedef std::set<std::string> strset_t;
   typedef std::map<std::string, std::string> strmap_t;

   AbstractQoreNode* init_c = nullptr, // the initialization closure
      * del_c = nullptr;               // the destructor closure

   QoreProgramLocation init_loc,
      del_loc;

   DLLLOCAL QoreModuleDefContext() {
   }

   DLLLOCAL ~QoreModuleDefContext() {
      if (init_c)
         init_c->deref(0);
      if (del_c)
         del_c->deref(0);
   }

   // set of valid tags
   static strset_t vset;

   // set of tag definitions
   strmap_t vmap;

   DLLLOCAL void set(const QoreProgramLocation& loc, const char* key, const AbstractQoreNode* val);

   DLLLOCAL const char* get(const char* str) const {
      strmap_t::const_iterator i = vmap.find(str);
      return i == vmap.end() || i->second.empty() ? 0 : i->second.c_str();
   }

   DLLLOCAL void parseInit();

   DLLLOCAL int init(QoreProgram& pgm, ExceptionSink& xsink);

   DLLLOCAL QoreClosureParseNode* takeDel();
};

DLLLOCAL QoreValue do_op_background(const AbstractQoreNode* left, ExceptionSink* xsink);

// returns 0 if the last mark has been cleared, -1 if there are more marks to check
DLLLOCAL int purge_thread_resources_to_mark(ExceptionSink* xsink);
DLLLOCAL void purge_thread_resources(ExceptionSink* xsink);
DLLLOCAL void purge_pgm_thread_resources(const QoreProgram* pgm, ExceptionSink* xsink);
DLLLOCAL void mark_thread_resources();
DLLLOCAL void beginParsing(const char* file, void* ps = NULL, const char* src = 0, int offset = 0);
DLLLOCAL void* endParsing();
DLLLOCAL Context* get_context_stack();
DLLLOCAL void update_context_stack(Context* cstack);

DLLLOCAL QoreProgramLocation get_runtime_location();
DLLLOCAL QoreProgramLocation update_get_runtime_location(const QoreProgramLocation& loc);
DLLLOCAL void update_runtime_location(const QoreProgramLocation& loc);

DLLLOCAL void update_parse_line_location(int start_line, int end_line);
DLLLOCAL void set_parse_file_info(QoreProgramLocation& loc);
DLLLOCAL const char* get_parse_code();
DLLLOCAL QoreProgramLocation get_parse_location();
DLLLOCAL void update_parse_location(const QoreProgramLocation& loc);

DLLLOCAL int64 parse_get_parse_options();
DLLLOCAL int64 runtime_get_parse_options();

DLLLOCAL bool parse_check_parse_option(int64 o);
DLLLOCAL bool runtime_check_parse_option(int64 o);

DLLLOCAL RootQoreNamespace* getRootNS();
DLLLOCAL void updateCVarStack(CVNode* ncvs);
DLLLOCAL CVNode* getCVarStack();
DLLLOCAL void updateVStack(VNode* nvs);
DLLLOCAL VNode* getVStack();
DLLLOCAL void setParseClass(QoreClass* c);
DLLLOCAL QoreClass* parse_get_class();
DLLLOCAL qore_class_private* parse_get_class_priv();
DLLLOCAL void substituteObjectIfEqual(QoreObject* o);
DLLLOCAL QoreObject* substituteObject(QoreObject* o);
DLLLOCAL QoreException* catchSwapException(QoreException* e);
DLLLOCAL QoreException* catchGetException();
DLLLOCAL VLock* getVLock();
DLLLOCAL void end_signal_thread(ExceptionSink* xsink);
DLLLOCAL void delete_thread_local_data();
DLLLOCAL void parse_cond_push(bool mark = false);
DLLLOCAL bool parse_cond_else();
DLLLOCAL bool parse_cond_pop(const QoreProgramLocation& loc);
DLLLOCAL bool parse_cond_test(const QoreProgramLocation& loc);
DLLLOCAL void push_parse_options();
DLLLOCAL void parse_try_module_inc();
DLLLOCAL bool parse_try_module_dec(const QoreProgramLocation& loc);
DLLLOCAL unsigned parse_try_module_get();
DLLLOCAL void parse_try_module_set(unsigned c);

DLLLOCAL void parse_push_name(const char* name);
DLLLOCAL std::string parse_pop_name();

DLLLOCAL qore_ns_private* parse_set_ns(qore_ns_private* ns);
DLLLOCAL qore_ns_private* parse_get_ns();
DLLLOCAL void set_module_context(QoreModuleContext* qmc);
DLLLOCAL QoreModuleContext* get_module_context();
DLLLOCAL QoreModuleDefContext* set_module_def_context(QoreModuleDefContext* qmd);
DLLLOCAL QoreModuleDefContext* get_module_def_context();
DLLLOCAL void parse_set_module_def_context_name(const char* name);
DLLLOCAL const char* set_user_module_context_name(const char* n);
DLLLOCAL const char* get_user_module_context_name();

DLLLOCAL void set_thread_tz(const AbstractQoreZoneInfo* tz);
DLLLOCAL const AbstractQoreZoneInfo* get_thread_tz(bool& set);
DLLLOCAL void clear_thread_tz();

DLLLOCAL ThreadProgramData* get_thread_program_data();

DLLLOCAL int thread_ref_set(const lvalue_ref* r);
DLLLOCAL void thread_ref_remove(const lvalue_ref* r);

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

DLLLOCAL int set_constant(ConstantEntry* ce);
DLLLOCAL void remove_constant(ConstantEntry* ce);

DLLLOCAL QoreAbstractModule* set_reexport(QoreAbstractModule* m, bool current_reexport, bool& old_reexport);
DLLLOCAL void set_reexport(QoreAbstractModule* m, bool reexport);

DLLLOCAL void parseSetCodeInfo(const char* parse_code, const QoreTypeInfo* returnTypeInfo, const char*& old_code, const QoreTypeInfo*& old_returnTypeInfo);
DLLLOCAL void parseRestoreCodeInfo(const char* parse_code, const QoreTypeInfo* returnTypeInfo);
// sets the new type and returns the old
DLLLOCAL const QoreTypeInfo* saveReturnTypeInfo(const QoreTypeInfo* returnTypeInfo);
DLLLOCAL const QoreTypeInfo* getReturnTypeInfo();

DLLLOCAL const QoreTypeInfo* parse_get_return_type_info();

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
DLLLOCAL void pushCall(CallNode* cn);
DLLLOCAL void popCall(ExceptionSink* xsink);
DLLLOCAL CallStack* getCallStack();
DLLLOCAL QoreListNode* getCallStackList();
#else
#ifdef __GNUC__
#define pushCall(args...)
#else
#define pushCall(args, ...)
#endif
#define popCall(x)
#endif

class ModuleReExportHelper {
protected:
   QoreAbstractModule* m;
   bool reexport;

public:
   DLLLOCAL ModuleReExportHelper(QoreAbstractModule* mi, bool reexp);
   DLLLOCAL ~ModuleReExportHelper();
};

class QoreParseCountContextHelper {
protected:
   unsigned count;

public:
   DLLLOCAL QoreParseCountContextHelper() : count(parse_try_module_get()) {
      parse_try_module_set(0);
   }

   DLLLOCAL ~QoreParseCountContextHelper() {
      parse_try_module_set(count);
   }
};

class QoreParseClassHelper {
protected:
   QoreClass* old;
   qore_ns_private* oldns;
   bool rn; // restore namespace

public:
   DLLLOCAL QoreParseClassHelper(QoreClass* cls);

   DLLLOCAL ~QoreParseClassHelper();
};

class QoreProgramLocationHelper {
protected:
   QoreProgramLocation loc;
public:
   DLLLOCAL QoreProgramLocationHelper(QoreProgramLocation& n_loc) : loc(update_get_runtime_location(n_loc)) {
   }

   DLLLOCAL ~QoreProgramLocationHelper() {
      update_runtime_location(loc);
   }
};

class QoreProgramOptionalLocationHelper {
protected:
   QoreProgramLocation loc;
   bool restore;

public:
   DLLLOCAL QoreProgramOptionalLocationHelper(QoreProgramLocation* n_loc) : restore((bool)n_loc) {
      if (n_loc)
         loc = update_get_runtime_location(*n_loc);
   }

   DLLLOCAL ~QoreProgramOptionalLocationHelper() {
      if (restore)
         update_runtime_location(loc);
   }
};

// allows for the parse lock for the current program to be acquired by binary modules
class CurrentProgramRuntimeParseContextHelper {
public:
   // acquires the parse lock; if already acquired by another thread, then this call blocks until the lock can be acquired
   DLLEXPORT CurrentProgramRuntimeParseContextHelper();
   // releases the parse lock for the current program
   DLLEXPORT ~CurrentProgramRuntimeParseContextHelper();

private:
   // not implemented
   CurrentProgramRuntimeParseContextHelper(const CurrentProgramRuntimeParseContextHelper&) = delete;
   void* operator new(size_t) = delete;
};

// acquires a TID and thread entry, returns -1 if not successful
DLLLOCAL int get_thread_entry();
// acquires TID 0 and sets up the signal thread entry, always returns 0
DLLLOCAL int get_signal_thread_entry();
DLLLOCAL void deregister_signal_thread();
DLLLOCAL void register_thread(int tid, pthread_t ptid, QoreProgram* pgm, bool foreign = false);
DLLLOCAL void deregister_thread(int tid);
DLLLOCAL void delete_signal_thread();

// returns 1 if data structure is already on stack, 0 if not (=OK)
DLLLOCAL int thread_push_container(const AbstractQoreNode* n);
DLLLOCAL void thread_pop_container(const AbstractQoreNode* n);

// called when a StatementBlock has "on block exit" blocks
DLLLOCAL void pushBlock(block_list_t::iterator i);
// called when a StatementBlock has "on block exit" blocks
DLLLOCAL block_list_t::iterator popBlock();
// called by each "on_block_exit" statement to activate it's code for the block exit
DLLLOCAL void advanceOnBlockExit();

DLLLOCAL LocalVarValue* thread_instantiate_lvar();
DLLLOCAL void thread_uninstantiate_lvar(ExceptionSink* xsink);
DLLLOCAL void thread_uninstantiate_self();

DLLLOCAL void thread_set_closure_parse_env(ClosureParseEnvironment* cenv);
DLLLOCAL ClosureParseEnvironment* thread_get_closure_parse_env();

DLLLOCAL ClosureVarValue* thread_instantiate_closure_var(const char* id, const QoreTypeInfo* typeInfo, QoreValue& nval);
DLLLOCAL void thread_instantiate_closure_var(ClosureVarValue* cvar);
DLLLOCAL void thread_uninstantiate_closure_var(ExceptionSink* xsink);
DLLLOCAL ClosureVarValue* thread_find_closure_var(const char* id);

DLLLOCAL ClosureVarValue* thread_get_runtime_closure_var(const LocalVar* id);
DLLLOCAL const QoreClosureBase* thread_set_runtime_closure_env(const QoreClosureBase* current);

typedef std::vector<ClosureVarValue*> cvv_vec_t;
DLLLOCAL cvv_vec_t* thread_get_all_closure_vars();

DLLLOCAL void thread_push_frame_boundary();
DLLLOCAL void thread_pop_frame_boundary();

DLLLOCAL QoreHashNode* thread_get_local_vars(int frame, ExceptionSink* xsink);
// returns 0 = OK, 1 = no such variable, -1 exception setting variable
DLLLOCAL int thread_set_local_var_value(const char* name, const QoreValue& val, ExceptionSink* xsink);
// returns 0 = OK, 1 = no such variable, -1 exception setting variable
DLLLOCAL int thread_set_closure_var_value(const char* name, const QoreValue& val, ExceptionSink* xsink);

DLLLOCAL int get_implicit_element();
DLLLOCAL int save_implicit_element(int n_element);

DLLLOCAL void save_global_vnode(VNode* vn);
DLLLOCAL VNode* get_global_vnode();

class QoreContainerHelper {
   const AbstractQoreNode* n;
   bool err;

public:
   DLLLOCAL QoreContainerHelper(const AbstractQoreNode* n_n) {
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

DLLLOCAL const QoreListNode* thread_get_implicit_args();

DLLLOCAL LocalVarValue* thread_find_lvar(const char* id);

// to get the current runtime object
DLLLOCAL QoreObject* runtime_get_stack_object();
// to get the current runtime class
DLLLOCAL const qore_class_private* runtime_get_class();
DLLLOCAL void runtime_get_object_and_class(QoreObject*& obj, const qore_class_private*& qc);
// for methods that behave differently when called within the method itself (methodGate(), memberGate(), etc)
DLLLOCAL bool runtime_in_object_method(const char* name, const QoreObject* o);

class CodeContextHelperBase {
private:
   const char* old_code;
   QoreObject* old_obj;
   const qore_class_private* old_class;
   bool do_ref;
   ExceptionSink* xsink;

public:
   DLLLOCAL CodeContextHelperBase(const char* code, QoreObject* obj, const qore_class_private* c, ExceptionSink* xsink);
   DLLLOCAL ~CodeContextHelperBase();
};

class ObjectSubstitutionHelper {
private:
   QoreObject* old_obj;
   const qore_class_private* old_class;

public:
   DLLLOCAL ObjectSubstitutionHelper(QoreObject* obj, const qore_class_private* c);
   DLLLOCAL ~ObjectSubstitutionHelper();
};

class OptionalClassObjSubstitutionHelper {
private:
   QoreObject* old_obj;
   const qore_class_private* old_class;
   bool subst;

public:
   DLLLOCAL OptionalClassObjSubstitutionHelper(const qore_class_private* qc);
   DLLLOCAL ~OptionalClassObjSubstitutionHelper();
};

class ThreadSafeLocalVarRuntimeEnvironmentHelper {
private:
   const QoreClosureBase* prev;

public:
   DLLLOCAL ThreadSafeLocalVarRuntimeEnvironmentHelper(const QoreClosureBase* current);
   DLLLOCAL ~ThreadSafeLocalVarRuntimeEnvironmentHelper();
};

typedef std::map<const LocalVar*, ClosureVarValue*> cvar_map_t;
typedef std::set<ClosureVarValue*> cvv_set_t;

class ThreadSafeLocalVarRuntimeEnvironment {
private:
   cvar_map_t cmap;
   cvv_set_t cvvset;

public:
   DLLLOCAL ThreadSafeLocalVarRuntimeEnvironment(const lvar_set_t* vlist);
   DLLLOCAL ~ThreadSafeLocalVarRuntimeEnvironment();
   DLLLOCAL ClosureVarValue* find(const LocalVar* id) const;
   DLLLOCAL bool hasVar(ClosureVarValue* cvv) const;
   DLLLOCAL void del(ExceptionSink* xsink);

   DLLLOCAL bool empty() {
      return cmap.empty();
   }

   DLLLOCAL const cvar_map_t& getMap() const {
      return cmap;
   }
};

struct ThreadLocalProgramData;

class QoreProgramBlockParseOptionHelper {
protected:
   int64 po;

public:
   DLLLOCAL QoreProgramBlockParseOptionHelper(int64 n_po);
   DLLLOCAL ~QoreProgramBlockParseOptionHelper();
};

class ProgramThreadCountContextHelper {
protected:
   QoreProgram* old_pgm;
   ThreadLocalProgramData* old_tlpd;
   bool restore;

public:
   DLLLOCAL ProgramThreadCountContextHelper(ExceptionSink* xsink, QoreProgram* pgm, bool runtime);
   DLLLOCAL ~ProgramThreadCountContextHelper();
};

class ProgramRuntimeParseContextHelper {
protected:
   QoreProgram* old_pgm;
   bool restore;

public:
   DLLLOCAL ProgramRuntimeParseContextHelper(ExceptionSink* xsink, QoreProgram* pgm);
   DLLLOCAL ~ProgramRuntimeParseContextHelper();
};

// ensures the program is locked for parsing and that thread-local data is available for execution at parse commit time
class ProgramRuntimeParseCommitContextHelper {
protected:
   QoreProgram* old_pgm;
   ThreadLocalProgramData* old_tlpd;
   bool restore;

public:
   DLLLOCAL ProgramRuntimeParseCommitContextHelper(ExceptionSink* xsink, QoreProgram* pgm);
   DLLLOCAL ~ProgramRuntimeParseCommitContextHelper();
};

class ProgramRuntimeParseAccessHelper {
protected:
   QoreProgram* old_pgm;
   bool restore;

public:
   DLLLOCAL ProgramRuntimeParseAccessHelper(ExceptionSink* xsink, QoreProgram* pgm);
   DLLLOCAL ~ProgramRuntimeParseAccessHelper();
};

class RuntimeReferenceHelperBase {
protected:
   const lvalue_ref* ref;
   ProgramThreadCountContextHelper pch;
   ObjectSubstitutionHelper osh;
   bool valid = true;

public:
   DLLLOCAL RuntimeReferenceHelperBase(const lvalue_ref& r, ExceptionSink* n_xsink)
      : ref(&r), pch(n_xsink, r.pgm, true), osh(r.self, r.cls) {
      //printd(5, "RuntimeReferenceHelperBase::RuntimeReferenceHelperBase() this: %p vexp: %p %s %d\n", this, r.vexp, get_type_name(r.vexp), get_node_type(r.vexp));
      if (thread_ref_set(&r)) {
         ref = 0;
         n_xsink->raiseException("CIRCULAR-REFERENCE-ERROR", "a circular lvalue reference was detected");
         valid = false;
      }
   }

   DLLLOCAL ~RuntimeReferenceHelperBase() {
      if (ref)
         thread_ref_remove(ref);
   }

   DLLLOCAL operator bool() const {
      return valid;
   }
};

class RuntimeReferenceHelper : public RuntimeReferenceHelperBase {
public:
   DLLLOCAL RuntimeReferenceHelper(const ReferenceNode& r, ExceptionSink* n_xsink) : RuntimeReferenceHelperBase(*lvalue_ref::get(&r), n_xsink) {
   }

   DLLLOCAL RuntimeReferenceHelper(const lvalue_ref& r, ExceptionSink* n_xsink) : RuntimeReferenceHelperBase(r, n_xsink) {
   }
};

class ArgvContextHelper {
private:
   QoreListNode* old_argv;
   ExceptionSink* xsink;

public:
   DLLLOCAL ArgvContextHelper(QoreListNode* argv, ExceptionSink* n_xsink);
   // calls deref(xsink) on list in destructor
   DLLLOCAL ~ArgvContextHelper();
};

class SingleArgvContextHelper {
private:
   QoreListNode* old_argv;
   ExceptionSink* xsink;

public:
   // any reference in val will be overtaken by the SingleArgvContextHelper object
   DLLLOCAL SingleArgvContextHelper(QoreValue val, ExceptionSink* n_xsink);
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

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
class CallNode {
public:
   const char* func;
   QoreProgramLocation loc;
   int type;

   QoreObject* obj;
   const qore_class_private* cls;
   CallNode* next, *prev;

   DLLLOCAL CallNode(const char* f, int t, QoreObject* o, const qore_class_private* c);
   DLLLOCAL QoreHashNode* getInfo() const;
};

class CallStack {
private:
   CallNode* tail;

public:
   DLLLOCAL CallStack();
   DLLLOCAL ~CallStack();
   DLLLOCAL QoreListNode* getCallStack() const;
   DLLLOCAL void push(CallNode* cn);
   DLLLOCAL void pop(ExceptionSink* xsink);
   /*
   DLLLOCAL void substituteObjectIfEqual(QoreObject* o);
   DLLLOCAL QoreObject* getStackObject() const;
   DLLLOCAL const qore_class_private* getStackClass() const;
   DLLLOCAL QoreObject* substituteObject(QoreObject* o);
   DLLLOCAL bool inMethod(const char* name, QoreObject* o) const;
   */
};

class CallStackHelper : public CallNode {
   ExceptionSink* xsink;

   // not implemented
   DLLLOCAL CallStackHelper(const CallStackHelper&);
   DLLLOCAL CallStackHelper& operator=(const CallStackHelper&);
   DLLLOCAL void* operator new(size_t);

public:
   DLLLOCAL CallStackHelper(const char* f, int t, QoreObject* o, const qore_class_private* c, ExceptionSink* n_xsink) : CallNode(f, t, o, c), xsink(n_xsink) {
      pushCall(this);
   }
   DLLLOCAL ~CallStackHelper() {
      popCall(xsink);
   }
};

class CodeContextHelper : public CodeContextHelperBase, public CallStackHelper {
public:
   DLLLOCAL CodeContextHelper(ExceptionSink* xs, int t, const char* c, QoreObject* obj = 0, const qore_class_private* cls = 0) :
      CodeContextHelperBase(c, obj, cls, xs),
      CallStackHelper(c, t, obj, cls, xs) {
   }
};

#else
class CodeContextHelper : public CodeContextHelperBase {
public:
   DLLLOCAL CodeContextHelper(ExceptionSink* xs, int t, const char* c, QoreObject* obj = 0, const qore_class_private* cls = 0) :
      CodeContextHelperBase(c, obj, cls, xs) {
   }
};
#endif

DLLLOCAL void init_qore_threads();
DLLLOCAL QoreNamespace* get_thread_ns(QoreNamespace& qorens);
DLLLOCAL void delete_qore_threads();
DLLLOCAL QoreListNode* get_thread_list();
DLLLOCAL QoreHashNode* getAllCallStacks();

class QorePThreadAttr {
private:
   pthread_attr_t attr;

public:
   DLLLOCAL QorePThreadAttr() {
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   }

   DLLLOCAL ~QorePThreadAttr() {
      //printd(2, "calling pthread_attr_destroy(%p)\n", &attr);
      pthread_attr_destroy(&attr);
      //printd(2, "returned from pthread_attr_destroy(%p)\n", &attr);
   }

#ifdef HAVE_PTHREAD_ATTR_GETSTACK
   DLLLOCAL void getstack(void*& ptr, size_t& ssize) {
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

   DLLLOCAL pthread_attr_t* get_ptr() {
      return &attr;
   }
};

DLLLOCAL extern QorePThreadAttr ta_default;

#ifdef QORE_MANAGE_STACK
DLLLOCAL int check_stack(ExceptionSink* xsink);
#endif

class ParseCodeInfoHelper {
private:
   const char* parse_code;
   const QoreTypeInfo* returnTypeInfo;

public:
   DLLLOCAL ParseCodeInfoHelper(const char* n_parse_code, const QoreTypeInfo* n_returnTypeInfo) {
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

class OptionalNamespaceParseContextHelper {
private:
   qore_ns_private* ns;
   bool restore;

public:
   DLLLOCAL OptionalNamespaceParseContextHelper(qore_ns_private* n_ns) {
      if (n_ns) {
         ns = parse_set_ns(n_ns);
         restore = (ns != n_ns);
      }
      else
         restore = false;
   }
   DLLLOCAL ~OptionalNamespaceParseContextHelper() {
      if (restore)
         parse_set_ns(ns);
   }
};

class ThreadData;

class ThreadProgramData : public QoreReferenceCounter {
private:
   // for the set of QoreProgram objects we have local variables in
   typedef std::set<QoreProgram*> pgm_set_t;
   pgm_set_t pgm_set;

   // lock for pgm_set data structure (which is accessed from multiple threads when QorePrograms deregister themselves)
   QoreThreadLock pslock;

   ThreadData* td;

   DLLLOCAL void ref() {
      ROreference();
   }

   DLLLOCAL ~ThreadProgramData() {
      assert(pgm_set.empty());
   }

public:
   DLLLOCAL ThreadProgramData(ThreadData* n_td) : td(n_td) {
   }

   DLLLOCAL void delProgram(QoreProgram* pgm);
   DLLLOCAL void saveProgram(bool runtime, ExceptionSink* xsink);
   DLLLOCAL void del(ExceptionSink* xsink);

   DLLLOCAL void deref() {
      if (ROdereference())
         delete this;
   }
};

class ThreadFrameBoundaryHelper {
public:
    DLLLOCAL ThreadFrameBoundaryHelper() {
        thread_push_frame_boundary();
    }

    DLLLOCAL ~ThreadFrameBoundaryHelper() {
        thread_pop_frame_boundary();
    }
};

DLLLOCAL extern pthread_mutexattr_t ma_recursive;

#endif
