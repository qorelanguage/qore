/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    qore_thread_intern.h

    POSIX thread library for Qore

    Qore Programming Language

    Copyright (C) 2003 - 2020 Qore Technologies, s.r.o.

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
struct ThreadLocalProgramData;
class QoreAbstractModule;
class QoreRWLock;

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
public:
    ModuleContextNamespaceList mcnl;
    ModuleContextFunctionList mcfl;

    DLLLOCAL QoreModuleContext(const char* n, qore_root_ns_private* n_rns, ExceptionSink& xs) : name(n), rns(n_rns), xsink(xs) {
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

    DLLLOCAL qore_root_ns_private* getRootNS() const {
        return rns;
    }

    DLLLOCAL const char* getName() const {
        return name;
    }

protected:
    const char* name;
    qore_root_ns_private* rns;
    QoreStringNode* err = nullptr;
    ExceptionSink& xsink;
};

class QoreModuleDefContext {
public:
    typedef std::set<std::string> strset_t;
    typedef std::map<std::string, std::string> strmap_t;

    QoreValue init_c, // the initialization closure
        del_c;         // the destructor closure

    const QoreProgramLocation* init_loc,
        * del_loc;

    DLLLOCAL QoreModuleDefContext() {
    }

    DLLLOCAL ~QoreModuleDefContext() {
        init_c.discard(nullptr);
        del_c.discard(nullptr);
    }

    // set of valid tags
    static strset_t vset;

    // set of tag definitions
    strmap_t vmap;

    DLLLOCAL void set(const QoreProgramLocation* loc, const char* key, QoreValue val);

    DLLLOCAL const char* get(const char* str) const {
        strmap_t::const_iterator i = vmap.find(str);
        return i == vmap.end() || i->second.empty() ? nullptr : i->second.c_str();
    }

    DLLLOCAL void parseInit();

    DLLLOCAL int init(QoreProgram& pgm, ExceptionSink& xsink);

    DLLLOCAL AbstractQoreNode* takeDel();

protected:
    DLLLOCAL void initClosure(const QoreProgramLocation* loc, QoreValue& c, const char* n);
};

DLLLOCAL QoreValue do_op_background(const QoreValue left, ExceptionSink* xsink);

// returns 0 if the last mark has been cleared, -1 if there are more marks to check
DLLLOCAL int purge_thread_resources_to_mark(ExceptionSink* xsink);
DLLLOCAL void purge_thread_resources(ExceptionSink* xsink);
DLLLOCAL void purge_pgm_thread_resources(const QoreProgram* pgm, ExceptionSink* xsink);
DLLLOCAL void mark_thread_resources();
DLLLOCAL void beginParsing(const char* file, void* ps = NULL, const char* src = nullptr, int offset = 0);
DLLLOCAL void* endParsing();
DLLLOCAL Context* get_context_stack();
DLLLOCAL void update_context_stack(Context* cstack);

DLLLOCAL const QoreStackLocation* get_runtime_stack_location();
DLLLOCAL const QoreStackLocation* update_get_runtime_stack_location(QoreStackLocation* stack_loc,
    const AbstractStatement*& current_stmt, QoreProgram*& current_pgm);
DLLLOCAL const QoreStackLocation* update_get_runtime_stack_builtin_location(QoreStackLocation* stack_loc,
    const AbstractStatement*& current_stmt, QoreProgram*& current_pgm, const QoreProgramLocation*& old_runtime_loc);
DLLLOCAL void update_runtime_stack_location(const QoreStackLocation* stack_loc);
DLLLOCAL void update_runtime_stack_location(const QoreStackLocation* stack_loc, const QoreProgramLocation* runtime_loc);

DLLLOCAL const QoreProgramLocation* get_runtime_location();
DLLLOCAL void update_get_runtime_statement_location(const AbstractStatement* stmt,
    const QoreProgramLocation* loc, const AbstractStatement*& old_stmt, const QoreProgramLocation*& old_loc);
DLLLOCAL void update_runtime_statement_location(const AbstractStatement* stmt, const QoreProgramLocation* loc);

DLLLOCAL void set_parse_file_info(QoreProgramLocation& loc);
DLLLOCAL const char* get_parse_code();

DLLLOCAL const AbstractStatement* get_runtime_statement();

DLLLOCAL const QoreTypeInfo* parse_set_implicit_arg_type_info(const QoreTypeInfo* ti);
DLLLOCAL const QoreTypeInfo* parse_get_implicit_arg_type_info();

DLLLOCAL int64 parse_get_parse_options();
DLLLOCAL int64 runtime_get_parse_options();

DLLLOCAL bool parse_check_parse_option(int64 o);
DLLLOCAL bool runtime_check_parse_option(int64 o);

DLLLOCAL RootQoreNamespace* getRootNS();
DLLLOCAL void updateCVarStack(CVNode* ncvs);
DLLLOCAL CVNode* getCVarStack();
DLLLOCAL void updateVStack(VNode* nvs);
DLLLOCAL VNode* getVStack();

//DLLLOCAL void setParseClass(QoreClass* c);
DLLLOCAL QoreClass* parse_get_class();
DLLLOCAL qore_class_private* parse_get_class_priv();
DLLLOCAL void thread_set_class_and_ns(const qore_class_private* new_cls, qore_ns_private* new_ns, const qore_class_private*& old_cls, qore_ns_private*& old_ns);
DLLLOCAL void thread_set_class_and_ns(const qore_class_private* new_cls, qore_ns_private* new_ns);
DLLLOCAL void thread_set_ns(qore_ns_private* new_ns, qore_ns_private*& old_ns);
DLLLOCAL void thread_set_ns(qore_ns_private* new_ns);
DLLLOCAL qore_ns_private* parse_get_ns();

DLLLOCAL void substituteObjectIfEqual(QoreObject* o);
DLLLOCAL QoreObject* substituteObject(QoreObject* o);
DLLLOCAL QoreException* catchSwapException(QoreException* e);
DLLLOCAL QoreException* catchGetException();
DLLLOCAL VLock* getVLock();
DLLLOCAL void end_signal_thread(ExceptionSink* xsink);
DLLLOCAL void delete_thread_local_data();
DLLLOCAL void parse_cond_push(bool mark = false);
DLLLOCAL bool parse_cond_else();
DLLLOCAL bool parse_cond_pop(const QoreProgramLocation* loc);
DLLLOCAL bool parse_cond_test(const QoreProgramLocation* loc);
DLLLOCAL void push_parse_options();
DLLLOCAL void parse_try_module_inc();
DLLLOCAL bool parse_try_module_dec(const QoreProgramLocation* loc);
DLLLOCAL unsigned parse_try_module_get();
DLLLOCAL void parse_try_module_set(unsigned c);

DLLLOCAL void parse_push_name(const char* name);
DLLLOCAL std::string parse_pop_name();

DLLLOCAL void set_module_context(QoreModuleContext* qmc);
DLLLOCAL QoreModuleContext* get_module_context();
DLLLOCAL QoreModuleDefContext* set_module_def_context(QoreModuleDefContext* qmd);
DLLLOCAL QoreModuleDefContext* get_module_def_context();
DLLLOCAL void parse_set_module_def_context_name(const char* name);
DLLLOCAL const char* set_user_module_context_name(const char* n);
DLLLOCAL const char* get_user_module_context_name();
DLLLOCAL const char* get_module_context_name();

DLLLOCAL void parse_set_try_reexport(bool tr);
DLLLOCAL bool parse_get_try_reexport();

DLLLOCAL void set_thread_tz(const AbstractQoreZoneInfo* tz);
DLLLOCAL const AbstractQoreZoneInfo* get_thread_tz(bool& set);
DLLLOCAL void clear_thread_tz();

DLLLOCAL ThreadProgramData* get_thread_program_data();
DLLLOCAL ThreadLocalProgramData* get_thread_local_program_data();

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

DLLLOCAL QoreProgram* get_set_program_call_context(QoreProgram* new_pgm);
DLLLOCAL void set_program_call_context(QoreProgram* new_pgm);

// issue #3242: make sure we can temporarily set any current lvar stack to nullptr when parsing out of order
class LVarStackBreakHelper {
public:
    DLLLOCAL LVarStackBreakHelper();
    DLLLOCAL ~LVarStackBreakHelper();

private:
    VNode* vnode;
};

class ProgramCallContextHelper {
public:
    DLLLOCAL ProgramCallContextHelper(QoreProgram* new_pgm);
    DLLLOCAL ~ProgramCallContextHelper();

private:
    QoreProgram* pgm;
};

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

class QoreProgramStackLocationHelper {
public:
    DLLLOCAL QoreProgramStackLocationHelper(QoreStackLocation* stack_loc, const AbstractStatement*& current_stmt,
        QoreProgram*& current_pgm) :
        stack_loc(update_get_runtime_stack_location(stack_loc, current_stmt, current_pgm)) {
    }

    DLLLOCAL ~QoreProgramStackLocationHelper() {
        update_runtime_stack_location(stack_loc);
    }

protected:
    const QoreStackLocation* stack_loc;
};

class QoreInternalCallStackLocationHelperBase : public QoreStackLocation, public QoreProgramStackLocationHelper {
public:
    DLLLOCAL QoreInternalCallStackLocationHelperBase() : QoreProgramStackLocationHelper(this, stmt, pgm) {
    }

    DLLLOCAL virtual QoreProgram* getProgram() const {
        return pgm;
    }

    DLLLOCAL virtual const AbstractStatement* getStatement() const {
        return stmt;
    }

protected:
    const AbstractStatement* stmt;
    QoreProgram* pgm;
};

class QoreInternalCallStackLocationHelper : public QoreInternalCallStackLocationHelperBase {
public:
    DLLLOCAL QoreInternalCallStackLocationHelper(const QoreProgramLocation& loc, const std::string& call,
        qore_call_t call_type) : loc(loc), call(call), call_type(call_type) {
    }

    //! returns the source location of the element
    DLLLOCAL virtual const QoreProgramLocation& getLocation() const {
        return loc;
    }

    //! returns the name of the function or method call
    DLLLOCAL virtual const std::string& getCallName() const {
        return call;
    }

    DLLLOCAL virtual qore_call_t getCallType() const {
        return call_type;
    }

protected:
    const QoreProgramLocation& loc;
    const std::string call;
    qore_call_t call_type;
};

class QoreProgramLocationHelper {
public:
    DLLLOCAL QoreProgramLocationHelper(const QoreProgramLocation* n_loc, const AbstractStatement* n_stat = nullptr) {
        update_get_runtime_statement_location(n_stat, n_loc, statement, loc);
    }

    DLLLOCAL ~QoreProgramLocationHelper() {
        update_runtime_statement_location(statement, loc);
    }

protected:
    const QoreProgramLocation* loc;
    const AbstractStatement* statement;
};

class QoreProgramOptionalLocationHelper {
public:
    DLLLOCAL QoreProgramOptionalLocationHelper(const QoreProgramLocation* n_loc, const AbstractStatement* n_stat = nullptr) : restore((bool)n_loc) {
        if (n_loc) {
            update_get_runtime_statement_location(n_stat, n_loc, statement, loc);
        }
    }

    DLLLOCAL ~QoreProgramOptionalLocationHelper() {
        if (restore) {
            update_runtime_statement_location(statement, loc);
        }
    }

protected:
    const QoreProgramLocation* loc;
    const AbstractStatement* statement;
    bool restore;
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

// allows for implicit argument types to be set at parse time
class ParseImplicitArgTypeHelper {
public:
   DLLLOCAL ParseImplicitArgTypeHelper(const QoreTypeInfo* ti) : ati(parse_set_implicit_arg_type_info(ti)) {
   }

   DLLLOCAL ~ParseImplicitArgTypeHelper() {
      parse_set_implicit_arg_type_info(ati);
   }

private:
   const QoreTypeInfo* ati;
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

DLLLOCAL ClosureVarValue* thread_instantiate_closure_var(const char* id, const QoreTypeInfo* typeInfo, QoreValue& nval, bool assign);
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
DLLLOCAL int thread_set_local_var_value(int frame, const char* name, const QoreValue& val, ExceptionSink* xsink);
// returns 0 = OK, 1 = no such variable, -1 exception setting variable
DLLLOCAL int thread_set_closure_var_value(int frame, const char* name, const QoreValue& val, ExceptionSink* xsink);

DLLLOCAL int get_implicit_element();
DLLLOCAL int save_implicit_element(int n_element);

DLLLOCAL VNode* update_get_vstack(VNode* vn);
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
                n = nullptr;
                err = true;
            }
        }
        else {
            n = nullptr;
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
    QoreProgram* old_call_program_context;
    bool do_ref,
        do_program_context;

    ExceptionSink* xsink;

public:
    DLLLOCAL CodeContextHelperBase(const char* code, QoreObject* obj, const qore_class_private* c, ExceptionSink* xsink, bool ref_obj = true);
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
public:
    DLLLOCAL OptionalClassObjSubstitutionHelper(const qore_class_private* qc);
    DLLLOCAL ~OptionalClassObjSubstitutionHelper();

private:
    QoreObject* old_obj;
    const qore_class_private* old_class;
    bool subst;
};

class OptionalClassOnlySubstitutionHelper {
public:
    DLLLOCAL OptionalClassOnlySubstitutionHelper(const qore_class_private* qc);
    DLLLOCAL ~OptionalClassOnlySubstitutionHelper();

private:
    const qore_class_private* old_class;
    bool subst;
};

class OptionalObjectOnlySubstitutionHelper {
public:
    DLLLOCAL OptionalObjectOnlySubstitutionHelper(QoreObject* obj);
    DLLLOCAL ~OptionalObjectOnlySubstitutionHelper();

private:
    bool subst;
    QoreObject* old_obj;
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
public:
    DLLLOCAL QoreProgramBlockParseOptionHelper(int64 n_po);
    DLLLOCAL ~QoreProgramBlockParseOptionHelper();

protected:
    int64 po;
};

class ProgramThreadCountContextHelper {
public:
    DLLLOCAL ProgramThreadCountContextHelper(ExceptionSink* xsink, QoreProgram* pgm, bool runtime);
    DLLLOCAL ~ProgramThreadCountContextHelper();
    static ThreadLocalProgramData* getContextFrame(int& frame, ExceptionSink* xsink);
    bool isFirstThreadLocalProgramData(const ThreadLocalProgramData* tlpd) const;

protected:
    QoreProgram* old_pgm = nullptr;
    ThreadLocalProgramData* old_tlpd = nullptr;
    ProgramThreadCountContextHelper* old_ctx = nullptr;
    // frame count of tlpd when context is started
    int save_frameCount = 0;
    int old_frameCount;
    bool restore = false;
    bool init_tlpd = false;
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
         ref = nullptr;
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

class CodeContextHelper : public CodeContextHelperBase {
public:
   DLLLOCAL CodeContextHelper(ExceptionSink* xs, int t, const char* c, QoreObject* obj = nullptr, const qore_class_private* cls = nullptr, bool ref_obj = true) :
      CodeContextHelperBase(c, obj, cls, xs, ref_obj) {
   }
};

DLLLOCAL void init_qore_threads();
DLLLOCAL QoreNamespace* get_thread_ns(QoreNamespace& qorens);
DLLLOCAL void delete_qore_threads();
DLLLOCAL QoreListNode* get_thread_list();
DLLLOCAL QoreHashNode* getAllCallStacks();
DLLLOCAL QoreListNode* qore_get_thread_call_stack();

#if defined(HAVE_PTHREAD_GET_STACKSIZE_NP) || (defined(QORE_HAVE_PTHREAD_GETATTR_NP) && defined(HAVE_PTHREAD_ATTR_GETSTACKSIZE))
#define QORE_HAVE_GET_STACK_SIZE
#endif

#if defined(QORE_HAVE_PTHREAD_SETNAME_NP_1) || defined(QORE_HAVE_PTHREAD_SETNAME_NP_2) || defined(QORE_HAVE_PTHREAD_SETNAME_NP_3) || defined(QORE_HAVE_PTHREAD_SET_NAME_NP)
#if defined(HAVE_PTHREAD_GET_NAME_NP) || defined(HAVE_PTHREAD_GETNAME_NP)
#define QORE_HAVE_THREAD_NAME
#endif
#endif

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

#ifdef QORE_HAVE_GET_STACK_SIZE
    DLLLOCAL static size_t getCurrentThreadStackSize() {
#ifdef HAVE_PTHREAD_GET_STACKSIZE_NP
        return pthread_get_stacksize_np(pthread_self());
#else
        pthread_attr_t attr;
        if (pthread_getattr_np(pthread_self(), &attr)) {
            return 0;
        }
        ON_BLOCK_EXIT(pthread_attr_destroy, &attr);
        size_t size = 0;
        if (pthread_attr_getstacksize(&attr, &size)) {
            return 0;
        }
        return size;
#endif
    }
#endif
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
    DLLLOCAL NamespaceParseContextHelper(qore_ns_private* n_ns) {
        thread_set_ns(n_ns, ns);
        restore = (ns != n_ns);
    }

    DLLLOCAL ~NamespaceParseContextHelper() {
        if (restore) {
            thread_set_ns(ns);
        }
    }
};

class OptionalNamespaceParseContextHelper {
private:
    qore_ns_private* ns;
    bool restore;

public:
    DLLLOCAL OptionalNamespaceParseContextHelper(qore_ns_private* n_ns) {
        if (n_ns) {
            thread_set_ns(n_ns, ns);
            restore = (ns != n_ns);
        } else {
            restore = false;
        }
    }

    DLLLOCAL ~OptionalNamespaceParseContextHelper() {
        if (restore) {
            thread_set_ns(ns);
        }
    }
};

class QoreParseClassHelper {
protected:
    const qore_class_private* cls;
    qore_ns_private* ns;
    bool restore;

public:
    DLLLOCAL QoreParseClassHelper(QoreClass* new_cls, qore_ns_private* new_ns = nullptr);

    DLLLOCAL ~QoreParseClassHelper();
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
   DLLLOCAL bool saveProgram(bool runtime, ExceptionSink* xsink);
   DLLLOCAL void del(ExceptionSink* xsink);

   DLLLOCAL void deref() {
      if (ROdereference())
         delete this;
   }
   DLLLOCAL int gettid();
};

class ThreadFrameBoundaryHelper {
public:
    DLLLOCAL ThreadFrameBoundaryHelper(bool doit) : doit(doit) {
        if (doit) {
            //printd(5, "ThreadFrameBoundaryHelper::ThreadFrameBoundaryHelper: this:%p\n", this);
            thread_push_frame_boundary();
        }
    }

    DLLLOCAL ~ThreadFrameBoundaryHelper() {
        if (doit) {
            //printd(5, "ThreadFrameBoundaryHelper::~ThreadFrameBoundaryHelper: this:%p\n", this);
            thread_pop_frame_boundary();
        }
    }

private:
    bool doit;
};

DLLLOCAL extern pthread_mutexattr_t ma_recursive;
DLLLOCAL extern QoreRWLock lck_debug_program;

#ifdef QORE_HAVE_THREAD_NAME
DLLLOCAL void q_set_thread_name(const char* name);
DLLLOCAL void q_get_thread_name(QoreString& str);
#endif

#endif
