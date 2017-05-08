/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_program_private.h

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

#ifndef _QORE_QORE_PROGRAM_PRIVATE_H
#define _QORE_QORE_PROGRAM_PRIVATE_H

#define QPP_DBG_LVL 5

extern QoreListNode* ARGV, * QORE_ARGV;
extern QoreHashNode* ENV;

#include "qore/intern/ParserSupport.h"
#include "qore/intern/QoreNamespaceIntern.h"
#include "qore/intern/QC_AutoReadLock.h"
#include "qore/intern/QC_AutoWriteLock.h"
#include "qore/QoreDebugProgram.h"
#include "qore/QoreRWLock.h"

#include <stdarg.h>
#include <errno.h>

#include <map>

typedef std::map<int, unsigned> ptid_map_t;

class QoreParseLocationHelper {
public:
   DLLLOCAL QoreParseLocationHelper(const char* file, const char* src, int offset) {
      beginParsing(file, 0, src, offset);
   }

   DLLLOCAL ~QoreParseLocationHelper() {
      endParsing();
   }
};

class CharPtrList : public safe_dslist<std::string> {
public:
   // returns true for found, false for not found
   // FIXME: use STL find algorithm
   DLLLOCAL bool find(const char* str) const {
      const_iterator i = begin();
      while (i != end()) {
         if (*i == str)
            return true;
         ++i;
      }

      return false;
   }
};

// local variable container
typedef safe_dslist<LocalVar*> local_var_list_t;

class LocalVariableList : public local_var_list_t {
public:
   DLLLOCAL LocalVariableList() {
   }

   DLLLOCAL ~LocalVariableList() {
      for (local_var_list_t::iterator i = begin(), e = end(); i != e; ++i)
         delete *i;
   }
};

typedef QoreThreadLocalStorage<QoreHashNode> qpgm_thread_local_storage_t;

#include "qore/intern/ThreadLocalVariableData.h"
#include "qore/intern/ThreadClosureVariableStack.h"


struct ThreadLocalProgramData {
private:
   // not implemented
   DLLLOCAL ThreadLocalProgramData(const ThreadLocalProgramData& old);

   // thread debug types, field is read/write only in thread being debugged, no locking is needed
   ThreadDebugEnum stepBreakpoint;
   ThreadDebugEnum saveStepBreakpoint;
   // when stepover or until return we need calls function calls
   int functionCallLevel;
   inline void setStepBreakpoint(ThreadDebugEnum st) {
      assert(st < DBG_SB_STOPPED); // DBG_SB_STOPPED is wrong value when program is running
      if (st == DBG_SB_UNTIL_RETURN) {
         functionCallLevel = 1;  // function called only when stepBreakpoint is not DBG_SB_UNTIL_RETURN
      }
      stepBreakpoint = st;
   }
   // set to true by any process do break running program asap
   volatile bool breakFlag;
   // called from running thread
   inline void checkBreakFlag() {
      if (breakFlag && stepBreakpoint != DBG_SB_DETACH) {
         breakFlag = false;
         if (stepBreakpoint != DBG_SB_STOPPED) {
            stepBreakpoint = DBG_SB_STEP;
         }
         printd(5, "ThreadLocalProgramData::checkBreakFlag(), this: %p, sb: %d\n", this, stepBreakpoint);
      }
   }
   // to call onAttach when debug is attached or detached, -1 .. detach, 1 .. attach
   int attachFlag;
   inline void checkAttach(ExceptionSink* xsink);
public:

   // local variable data slots
   ThreadLocalVariableData lvstack;
   // closure variable stack
   ThreadClosureVariableStack cvstack;
   // current thread's time zone locale (if any)
   const AbstractQoreZoneInfo* tz;
   // the "time zone set" flag
   bool tz_set : 1;

   // top-level vars instantiated
   bool inst : 1;


   DLLLOCAL ThreadLocalProgramData() : stepBreakpoint(DBG_SB_DETACH), functionCallLevel(0), breakFlag(false), tz(0), tz_set(false), inst(false) {
      //printd(5, "ThreadLocalProgramData::ThreadLocalProgramData() this: %p\n", this);
   }

   DLLLOCAL ~ThreadLocalProgramData() {
      assert(lvstack.empty());
      assert(cvstack.empty());
   }

   DLLLOCAL void finalize(arg_vec_t*& cl) {
      lvstack.finalize(cl);
      cvstack.finalize(cl);
   }

   DLLLOCAL void del(ExceptionSink* xsink) {
      lvstack.del(xsink);
      cvstack.del(xsink);
      delete this;
   }

   DLLLOCAL void setTZ(const AbstractQoreZoneInfo* n_tz) {
      tz_set = true;
      tz = n_tz;
   }

   DLLLOCAL void clearTZ() {
      tz_set = false;
      tz = 0;
   }

/*   void setEnable(bool n_enabled) {
      enabled = n_enabled;
      if (!enabled) {
         stepBreakpoint = DBG_SB_RUN;
         functionCallLevel = 0;
      }
   }*/

   /**
    * Data local for each program and thread. dbgXXX function are called from
    * AbstractStatement places when particular action related to debugging is taken.
    * xsink is passed as debugger can raise exception to be passed to program.
    * When dbgXXX function is executed then stepBreakpoint is tested unless is DBG_SB_STOPPED
    * then is set to DBG_SB_STOPPED. It's simple lock and debugging is disabled
    * till returns from this event handler. To be precise it should be
    * locked by an atomic lock but it is good enough not to break performance.
    */

   /**
    * Executed every step in BlockStatement.
    * @param statement is step being processed
    * @return 0 as neutral value or RC_RETURN/BREAK/CONTINUE to terminate block
    */
   DLLLOCAL int dbgStep(const StatementBlock* blockStatement, const AbstractStatement* statement, ExceptionSink* xsink);
   /**
    * Executed when a function is entered. If step-over is requested then flag is cleared not to break
    */
   DLLLOCAL void dbgFunctionEnter(const StatementBlock* statement, ExceptionSink* xsink);
   /**
    * Executed when a function is exited.
    */
   DLLLOCAL void dbgFunctionExit(const StatementBlock* statement, QoreValue& returnValue, ExceptionSink* xsink);
   /**
    * Executed when an exception is raised.
    */
   DLLLOCAL void dbgException(const AbstractStatement* statement, ExceptionSink* xsink);

   /**
    * Executed from any thread to break running program
    */
   DLLLOCAL void dbgBreak() {
      printd(5, "ThreadLocalProgramData::dbgBreak(), this: %p\n", this);
      breakFlag = true;
   }
   /**
    * Executed from any thread to set pending attach flag
    */
   DLLLOCAL void dbgPendingAttach() {
      printd(5, "ThreadLocalProgramData::dbgPendingAttach(), this: %p\n", this);
      attachFlag = 1;
   }
   /**
    * Executed from any thread to set pending detach flag
    */
   DLLLOCAL void dbgPendingDetach() {
      printd(5, "ThreadLocalProgramData::dbgPendingDetach(), this: %p\n", this);
      attachFlag = -1;
   }
};

// maps from thread handles to thread-local data
typedef std::map<ThreadProgramData*, ThreadLocalProgramData*> pgm_data_map_t;

// map for "defines" in programs
typedef std::map<std::string, AbstractQoreNode*> dmap_t;

// map for pushed parse options
typedef std::map<const char*, int64, ltstr> ppo_t;

class AbstractQoreZoneInfo;

class qore_program_private_base {
   friend class QoreProgramAccessHelper;

protected:
   DLLLOCAL void setDefines();

   typedef std::map<const char*, AbstractQoreProgramExternalData*, ltstr> extmap_t;
   extmap_t extmap;

public:
   LocalVariableList local_var_list;

   // for the thread counter, used only with plock
   QoreCondition pcond;
   ptid_map_t tidmap;       // map of tids -> thread count in program object
   unsigned thread_count;   // number of threads currently running in this Program
   unsigned thread_waiting; // number of threads waiting on all threads to terminate or parsing to complete
   unsigned parse_count;    // recursive parse count

   // to save file names for later deleting
   cstr_vector_t fileList;
   // features present in this Program object
   CharPtrList featureList;
   // user modules present in this Program object
   CharPtrList userFeatureList;

   // parse lock, making parsing actions atomic and thread-safe, also for runtime thread attachment
   mutable QoreThreadLock plock;

   QoreThreadLock chl,      // complex hash lock
      chonl,                // complex hash or nothing lock
      cll,                  // complex list lock
      clonl,                // complex list or nothing lock
      crl,                  // complex reference lock
      cronl;                // complex reference or nothing lock

   typedef std::map<const QoreTypeInfo*, QoreTypeInfo*> tmap_t;
   tmap_t ch_map,          // complex hash map
      chon_map,            // complex hash or nothing map
      cl_map,              // complex list map
      clon_map,            // complex list or nothing map
      cr_map,              // complex reference map
      cron_map;            // complex reference or nothing map

   // set of signals being handled by code in this Program (to be deleted on exit)
   int_set_t sigset;

   // weak reference dependency counter, when this hits zero, the object is deleted
   QoreReferenceCounter dc;
   ExceptionSink* parseSink, *warnSink, *pendingParseSink;
   RootQoreNamespace* RootNS;
   QoreNamespace* QoreNS;

   // top level statements
   TopLevelStatementBlock sb;

   // bit field flags
   bool only_first_except : 1,
      po_locked : 1,
      po_allow_restrict : 1,
      exec_class : 1,
      base_object : 1,
      requires_exception : 1
      ;

   int tclear;   // clearing thread-local variables in progress? if so, this is the TID

   int exceptions_raised,
      ptid;      // TID of thread destroying the program's private data

   ParseWarnOptions pwo;

   int64 dom,    // a mask of functional domains used in this Program
      pend_dom;  // a mask of pending function domains used in this Program

   std::string exec_class_name, script_dir, script_path, script_name, include_path;

   // thread-local data (could be inherited from another program)
   qpgm_thread_local_storage_t* thread_local_storage;

   mutable QoreThreadLock tlock;  // thread variable data lock, for accessing the thread variable data map and the thr_init variable
   mutable QoreCondition tcond;   // cond variable for tclear to become false, used only with tlock
   mutable unsigned twaiting;     // threads waiting on tclear to become false

   // thread-local variable storage - map from thread ID to thread-local storage
   pgm_data_map_t pgm_data_map;

   // time zone setting for the program
   const AbstractQoreZoneInfo* TZ;

   // define map
   dmap_t dmap;

   // pushed parse option map
   ppo_t ppo;

   // thread initialization user code
   ResolvedCallReferenceNode* thr_init;

   // return value for use with %exec-class
   AbstractQoreNode* exec_class_rv;

   // public object that owns this private implementation
   QoreProgram* pgm;

   DLLLOCAL qore_program_private_base(QoreProgram* n_pgm, int64 n_parse_options, QoreProgram* p_pgm = 0)
      : thread_count(0), thread_waiting(0), parse_count(0), plock(&ma_recursive), parseSink(0), warnSink(0), pendingParseSink(0), RootNS(0), QoreNS(0),
        only_first_except(false), po_locked(false), po_allow_restrict(true), exec_class(false), base_object(false),
        requires_exception(false), tclear(0),
        exceptions_raised(0), ptid(0), pwo(n_parse_options), dom(0), pend_dom(0), thread_local_storage(0), twaiting(0),
        thr_init(0), exec_class_rv(0), pgm(n_pgm) {
      printd(QPP_DBG_LVL, "qore_program_private_base::qore_program_private_base() this: %p pgm: %p po: " QLLD "\n", this, pgm, n_parse_options);

#ifdef DEBUG
      pgm->priv = (qore_program_private*)this;
#endif

      if (p_pgm)
         setParent(p_pgm, n_parse_options);
      else {
         TZ = QTZM.getLocalZoneInfo();
         newProgram();
      }

      // initialize global vars
      Var *var = qore_root_ns_private::runtimeCreateVar(*RootNS, *QoreNS, "ARGV", listTypeInfo);
      if (var && ARGV)
         var->setInitial(ARGV->copy());

      var = qore_root_ns_private::runtimeCreateVar(*RootNS, *QoreNS, "QORE_ARGV", listTypeInfo);
      if (var && QORE_ARGV)
         var->setInitial(QORE_ARGV->copy());

      var = qore_root_ns_private::runtimeCreateVar(*RootNS, *QoreNS, "ENV", hashTypeInfo);
      if (var)
         var->setInitial(ENV->copy());
      setDefines();
   }

#ifdef DEBUG
   DLLLOCAL ~qore_program_private_base() {
      printd(QPP_DBG_LVL, "qore_program_private_base::~qore_program_private_base() this: %p pgm: %p\n", this, pgm);
   }
#endif

   DLLLOCAL void startThread(ExceptionSink& xsink);

protected:
   DLLLOCAL void setParent(QoreProgram* p_pgm, int64 n_parse_options);

   // for independent programs (not inherited from another QoreProgram object)
   DLLLOCAL void newProgram();
};

class PreParseHelper {
protected:
   qore_program_private_base *p;
   bool swapped;

public:
   DLLLOCAL PreParseHelper(qore_program_private_base *n_p) : p(n_p), swapped(false) {
      if (!p->parseSink) {
         if (!p->pendingParseSink)
            p->pendingParseSink = new ExceptionSink;
         p->parseSink = p->pendingParseSink;
         swapped = true;
      }
   }

   DLLLOCAL ~PreParseHelper() {
      if (swapped)
         p->parseSink = 0;
   }
};

class qore_debug_program_private;

class AutoQoreCounterDec {
private:
   QoreCounter* cnt;
   bool incFlag;
   AutoQoreCounterDec() {}
public:
   AutoQoreCounterDec(QoreCounter* n_cnt, bool incNow = true): cnt(n_cnt), incFlag(false) {
      if (incNow) {
         inc();
      }
   }
   ~AutoQoreCounterDec() {
      if (incFlag)
         cnt->dec();
   }
   void inc() {
      cnt->inc();
      incFlag = true;
   }
};

class QoreBreakpoint;
typedef std::list<QoreBreakpoint*> QoreBreakpointList_t;

class qore_program_private : public qore_program_private_base {
private:
   mutable QoreCounter debug_program_counter;  // number of thread calls to debug program instance.
   DLLLOCAL void init(QoreProgram* n_pgm, int64 n_parse_options, const AbstractQoreZoneInfo *n_TZ = QTZM.getLocalZoneInfo()) {
   }

   // only called from parseSetTimeZone
   DLLLOCAL void mergeParseException(ExceptionSink &xsink) {
      if (parseSink)
         parseSink->assimilate(xsink);
      else {
         if (!pendingParseSink)
            pendingParseSink = new ExceptionSink;
         pendingParseSink->assimilate(xsink);
      }
   }

   qore_debug_program_private* dpgm;
   QoreRWLock lck_breakpoint; // to protect breakpoint manipulation
   QoreBreakpointList_t breakpointList;

   // map for line to statement
   typedef std::map<int, AbstractStatement*> sline_statement_multimap_t;

   struct cmp_char_str
   {
      bool operator()(char const *a, char const *b)
      {
         return strcmp(a, b) < 0;
      }
   };

   // map for filenames
   typedef std::map<const char*, sline_statement_multimap_t*, cmp_char_str> name_sline_statement_map_t;

   // index source filename/label -> line -> statement
   name_sline_statement_map_t statementByFileIndex;
   name_sline_statement_map_t statementByLabelIndex;


   /**
    * get safely debug program pointer. The debug program instance itself must exists. It's not matter of lock as the flow goes to QoreDebugProgram
    * instance and may stay very long time.
    */

   DLLLOCAL qore_debug_program_private* getDebugProgram(AutoQoreCounterDec& ad) {
      QoreAutoRWReadLocker al(&lck_debug_program);
      qore_debug_program_private* ret = dpgm;
      if (ret) {
         // new debug call in progress
         ad.inc();
      }
      return ret;

   }

   // lck_breakpoint lock should be aquired
   DLLLOCAL bool isBreakpointRegistered(const QoreBreakpoint *bkpt) const {
      return std::find(breakpointList.begin(), breakpointList.end(), bkpt) != breakpointList.end();
   }
   friend class QoreBreakpoint;
public:
   DLLLOCAL qore_program_private(QoreProgram* n_pgm, int64 n_parse_options) : qore_program_private_base(n_pgm, n_parse_options), dpgm(0) {
      printd(5, "qore_program_private::qore_program_private() this: %p pgm: %p\n", this, pgm);
   }

   DLLLOCAL qore_program_private(QoreProgram* n_pgm, int64 n_parse_options, QoreProgram* p_pgm) : qore_program_private_base(n_pgm, n_parse_options, p_pgm), dpgm(0) {
      printd(5, "qore_program_private::qore_program_private() this: %p pgm: %p\n", this, pgm);
   }

   DLLLOCAL ~qore_program_private();

   DLLLOCAL void depRef() {
      printd(QPP_DBG_LVL, "qore_program_private::depRef() this: %p pgm: %p %d->%d\n", this, pgm, dc.reference_count(), dc.reference_count() + 1);
      dc.ROreference();
   }

   DLLLOCAL void depDeref() {
      printd(QPP_DBG_LVL, "qore_program_private::depDeref() this: %p pgm: %p %d->%d\n", this, pgm, dc.reference_count(), dc.reference_count() - 1);
      if (dc.ROdereference())
         delete pgm;
   }

   DLLLOCAL void clearProgramThreadData(ExceptionSink* xsink) {
      // grab all thread-local data in a vector and finalize it outside the lock
      arg_vec_t* cl = 0;
      {
         AutoLocker al(tlock);
         // twaiting must be 0 here, as it can only be incremented while clearProgramThreadData() is in progress, which can only be executed once
         assert(!twaiting);
         assert(!tclear);
         // while tclear is set, no threads can attach to this program object - pgm_data_map cannot be modified
         tclear = gettid();

         for (pgm_data_map_t::iterator i = pgm_data_map.begin(), e = pgm_data_map.end(); i != e; ++i)
            i->second->finalize(cl);
      }

      // dereference finalized thread-local data outside the lock to avoid deadlocks
      if (cl) {
         for (arg_vec_t::iterator i = cl->begin(), e = cl->end(); i != e; ++i)
            (*i)->deref(xsink);
         delete cl;
      }

      for (pgm_data_map_t::iterator i = pgm_data_map.begin(), e = pgm_data_map.end(); i != e; ++i) {
         i->second->del(xsink);
         i->first->delProgram(pgm);
      }
   }

   DLLLOCAL void waitForTerminationAndClear(ExceptionSink* xsink);

   // called when the program's ref count = 0 (but the dc count may not go to 0 yet)
   DLLLOCAL void clear(ExceptionSink* xsink);

   // called when starting a new thread before the new thread is started, to avoid race conditions
   // once the new thread has been started, the TID is registered in startThread()
   DLLLOCAL int preregisterNewThread(ExceptionSink* xsink) {
      // grab program-level lock
      AutoLocker al(plock);

      if (ptid) {
         xsink->raiseException("PROGRAM-ERROR", "the Program accessed has already been deleted and therefore no new threads can be started in it");
         return -1;
      }

      ++thread_count;
      return 0;
   }

   // called when thread startup fails after preregistration
   DLLLOCAL void cancelPreregistration() {
      // grab program-level lock
      AutoLocker al(plock);

      assert(thread_count > 0);
      if (!--thread_count && thread_waiting)
         pcond.broadcast();
   }

   // called from the new thread once the thread has been started (after preregisterNewThread())
   DLLLOCAL void registerNewThread(int tid) {
      // grab program-level lock
      AutoLocker al(plock);

      assert(thread_count);
      ++tidmap[tid];
   }

   /*
   DLLLOCAL int checkValid(ExceptionSink* xsink) {
      if (ptid && ptid != gettid()) {
         xsink->raiseException("PROGRAM-ERROR", "the Program accessed has already been deleted and therefore cannot be accessed at runtime");
         return -1;
      }
      return 0;
   }
   */

   // returns 0 for OK, -1 for error
   DLLLOCAL int incThreadCount(ExceptionSink* xsink) {
      int tid = gettid();

      // grab program-level lock
      AutoLocker al(plock);

      if (ptid && ptid != tid) {
         xsink->raiseException("PROGRAM-ERROR", "the Program accessed has already been deleted and therefore cannot be accessed at runtime");
         return -1;
      }

      ++tidmap[tid];
      ++thread_count;
      return 0;
   }

   // throws a QoreStandardException if there is an error
   DLLLOCAL void incThreadCount() {
      int tid = gettid();

      // grab program-level lock
      AutoLocker al(plock);

      if (ptid && ptid != tid)
         throw QoreStandardException("PROGRAM-ERROR", "the Program accessed has already been deleted and therefore cannot be accessed at runtime");

      ++tidmap[tid];
      ++thread_count;
   }

   DLLLOCAL void decThreadCount(int tid) {
      // grab program-level lock
      AutoLocker al(plock);

      ptid_map_t::iterator i = tidmap.find(tid);
      assert(i != tidmap.end());
      if (!--i->second)
         tidmap.erase(i);

      assert(thread_count > 0);
      if (!--thread_count && thread_waiting)
         pcond.broadcast();
   }

   DLLLOCAL int lockParsing(ExceptionSink* xsink) {
      // grab program-level lock
      AutoLocker al(plock);

      bool curr = (pgm == getProgram());
      if (!curr) {
         while (parse_count) {
            ++thread_waiting;
            pcond.wait(plock);
            --thread_waiting;
         }
      }

      if (ptid && ptid != gettid()) {
         if (xsink)
            xsink->raiseException("PROGRAM-ERROR", "the Program accessed has already been deleted and therefore cannot be accessed");
         return -1;
      }

      //printd(5, "qore_program_private::lockParsing() this: %p ptid: %d thread_count: %d parse_count: %d -> %d\n", this, ptid, thread_count, parse_count, parse_count + 1);
      ++parse_count;
      return 0;
   }

   DLLLOCAL void unlockParsing() {
      // grab program-level lock
      AutoLocker al(plock);
      if (!(--parse_count) && thread_waiting)
         pcond.broadcast();
   }

   // called only with plock held
   DLLLOCAL void waitForAllThreadsToTerminateIntern() {
      int tid = gettid();

      ptid_map_t::iterator i = tidmap.find(tid);
      unsigned adj = (i != tidmap.end() ? 1 : 0);

      while ((thread_count - adj) || parse_count) {
         ++thread_waiting;
         pcond.wait(plock);
         --thread_waiting;
      }
   }

   DLLLOCAL void waitForAllThreadsToTerminate() {
      // grab program-level lock
      AutoLocker al(&plock);
      waitForAllThreadsToTerminateIntern();
   }

   DLLLOCAL const char* parseGetScriptPath() const {
      return script_path.empty() ? 0 : script_path.c_str();
   }

   DLLLOCAL const char* parseGetScriptDir() const {
      return script_dir.empty() ? 0 : script_dir.c_str();
   }

   DLLLOCAL const char* parseGetScriptName() const {
      return script_name.empty() ? 0 : script_name.c_str();
   }

   DLLLOCAL QoreStringNode* getScriptPath() const {
      // grab program-level parse lock
      AutoLocker al(&plock);
      return script_path.empty() ? 0 : new QoreStringNode(script_path);
   }

   DLLLOCAL QoreStringNode* getScriptDir() const {
      // grab program-level parse lock
      AutoLocker al(&plock);
      return script_dir.empty() ? 0 : new QoreStringNode(script_dir);
   }

   DLLLOCAL QoreStringNode* getScriptName() const {
      // grab program-level parse lock
      AutoLocker al(&plock);
      return script_name.empty() ? 0 : new QoreStringNode(script_name);
   }

   DLLLOCAL void setScriptPathExtern(const char* path) {
      // grab program-level parse lock
      AutoLocker al(&plock);
      setScriptPath(path);
   }

   DLLLOCAL void setScriptPath(const char* path) {
      if (!path) {
         script_dir.clear();
         script_path.clear();
         script_name.clear();
      }
      else {
         // find file name
         const char* p = q_basenameptr(path);
         if (p == path) {
            script_name = path;
            script_dir = "." QORE_DIR_SEP_STR;
            script_path = script_dir + script_name;
         }
         else {
            script_path = path;
            script_name = p;
            script_dir.assign(path, p - path);
         }
      }
   }

   DLLLOCAL QoreListNode* getVarList() {
      //AutoLocker al(&plock);
      // FIXME: implement
      return new QoreListNode;
      //return global_var_list.getVarList();
   }

   DLLLOCAL QoreListNode* getFeatureList() const {
      QoreListNode* l = new QoreListNode;

      for (CharPtrList::const_iterator i = featureList.begin(), e = featureList.end(); i != e; ++i)
         l->push(new QoreStringNode(*i));

      for (CharPtrList::const_iterator i = userFeatureList.begin(), e = userFeatureList.end(); i != e; ++i)
         l->push(new QoreStringNode(*i));

      return l;
   }

   DLLLOCAL void internParseRollback();

   // call must push the current program on the stack and pop it afterwards
   DLLLOCAL int internParsePending(const char* code, const char* label, const char* orig_src = 0, int offset = 0) {
      //printd(5, "qore_program_private::internParsePending() code: %p %d bytes label: '%s' src: '%s' offset: %d\n", code, strlen(code), label, orig_src ? orig_src : "(null)", offset);

      assert(code && code[0]);

      // save this file name for storage in the parse tree and deletion
      // when the QoreProgram object is deleted
      char* sname = strdup(label);
      addFile(sname);
      char* src = orig_src ? strdup(orig_src) : 0;
      if (src)
         addFile(src);

      QoreParseLocationHelper qplh(sname, src, offset);

      beginParsing(sname, 0, src, offset);

      // no need to save buffer, because it's deleted automatically in lexer
      //printd(5, "qore_program_private::internParsePending() parsing tag: %s (%p): '%s'\n", label, label, code);

      yyscan_t lexer;
      yylex_init(&lexer);

      yy_scan_string(code, lexer);
      yyset_lineno(1, lexer);
      // yyparse() will call endParsing() and restore old pgm position
      yyparse(lexer);

      printd(5, "qore_program_private::internParsePending() returned from yyparse()\n");
      int rc = 0;
      if (parseSink->isException()) {
         rc = -1;
         printd(5, "qore_program_private::internParsePending() parse exception: calling parseRollback()\n");
         internParseRollback();
         requires_exception = false;
      }

      printd(5, "qore_program_private::internParsePending() about to call yylex_destroy()\n");
      yylex_destroy(lexer);
      printd(5, "qore_program_private::internParsePending() returned from yylex_destroy()\n");
      return rc;
   }

   DLLLOCAL void startParsing(ExceptionSink* xsink, ExceptionSink* wS, int wm) {
      warnSink = wS;
      pwo.warn_mask = wm;
      parseSink = xsink;

      if (pendingParseSink) {
         parseSink->assimilate(pendingParseSink);
         pendingParseSink = 0;
      }
   }

   DLLLOCAL int parsePending(const char* code, const char* label, ExceptionSink* xsink, ExceptionSink* wS, int wm, const char* orig_src = 0, int offset = 0) {
      //printd(5, "qore_program_private::parsePending() wm=0x%x UV=0x%x on: %d\n", wm, QP_WARN_UNREFERENCED_VARIABLE, wm & QP_WARN_UNREFERENCED_VARIABLE);

      ProgramRuntimeParseContextHelper pch(xsink, pgm);
      assert(xsink);
      if (*xsink)
         return -1;

      startParsing(xsink, wS, wm);

      int rc = internParsePending(code, label, orig_src, offset);
      warnSink = 0;
#ifdef DEBUG
      parseSink = 0;
#endif
      return rc;
   }

   // caller must have grabbed the lock and put the current program on the program stack
   DLLLOCAL int internParseCommit();

   DLLLOCAL int parseCommit(ExceptionSink* xsink, ExceptionSink* wS, int wm) {
      ProgramRuntimeParseCommitContextHelper pch(xsink, pgm);
      assert(xsink);
      if (*xsink)
         return -1;

      startParsing(xsink, wS, wm);

      // finalize parsing, back out or commit all changes
      int rc = internParseCommit();

#ifdef DEBUG
      parseSink = 0;
#endif
      warnSink = 0;
      // release program-level parse lock
      return rc;
   }

   DLLLOCAL int parseRollback(ExceptionSink* xsink) {
      ProgramRuntimeParseContextHelper pch(xsink, pgm);
      assert(xsink);
      if (*xsink)
         return -1;

      // back out all pending changes
      internParseRollback();
      return 0;
   }

   DLLLOCAL void parse(FILE *fp, const char* name, ExceptionSink* xsink, ExceptionSink* wS, int wm) {
      assert(xsink);
      printd(5, "QoreProgram::parse(fp: %p, name: %s, xsink: %p, wS: %p, wm: %d)\n", fp, name, xsink, wS, wm);

      // if already at the end of file, then return
      // try to get one character from file
      int c = fgetc(fp);
      if (feof(fp)) {
         printd(5, "QoreProgram::parse(fp: %p, name: %s) EOF\n", fp, name);
         return;
      }
      // push back read character
      ungetc(c, fp);

      yyscan_t lexer;

      {
         ProgramRuntimeParseCommitContextHelper pch(xsink, pgm);
         if (*xsink)
            return;

         startParsing(xsink, wS, wm);

         // save this file name for storage in the parse tree and deletion
         // when the QoreProgram object is deleted
         char* sname = strdup(name);
         addFile(sname);

         QoreParseLocationHelper qplh(sname, 0, 0);

         beginParsing(sname);

         //printd(5, "QoreProgram::parse(): about to call yyparse()\n");
         yylex_init(&lexer);
         yyset_in(fp, lexer);
         // yyparse() will call endParsing() and restore old pgm position
         yyparse(lexer);

         // finalize parsing, back out or commit all changes
         internParseCommit();

#ifdef DEBUG
         parseSink = 0;
#endif
         warnSink = 0;
         // release program-level parse lock
      }

      yylex_destroy(lexer);
      if (only_first_except && exceptions_raised > 1)
         fprintf(stderr, "\n%d exception(s) skipped\n\n", exceptions_raised);
   }

   DLLLOCAL void parse(const QoreString *str, const QoreString *lstr, ExceptionSink* xsink, ExceptionSink* wS, int wm, const QoreString* source = 0, int offset = 0) {
      assert(xsink);
      if (!str->strlen())
         return;

      // ensure code string has correct character set encoding
      TempEncodingHelper tstr(str, QCS_DEFAULT, xsink);
      if (*xsink)
         return;

      // ensure label string has correct character set encoding
      TempEncodingHelper tlstr(lstr, QCS_DEFAULT, xsink);
      if (*xsink)
         return;

      TempEncodingHelper src;
      if (source && !source->empty() && !src.set(source, QCS_DEFAULT, xsink))
         return;

      parse(tstr->getBuffer(), tlstr->getBuffer(), xsink, wS, wm, source ? src->getBuffer() : 0, offset);
   }

   DLLLOCAL void parse(const char* code, const char* label, ExceptionSink* xsink, ExceptionSink* wS, int wm, const char* orig_src = 0, int offset = 0) {
      //printd(5, "qore_program_private::parse(%s) pgm: %p po: %lld\n", label, pgm, pwo.parse_options);

      assert(code && code[0]);
      assert(xsink);

      ProgramRuntimeParseCommitContextHelper pch(xsink, pgm);
      if (*xsink)
         return;

      startParsing(xsink, wS, wm);

      // parse text given
      if (!internParsePending(code, label, orig_src, offset))
         internParseCommit();   // finalize parsing, back out or commit all changes

#ifdef DEBUG
      parseSink = 0;
#endif
      warnSink = 0;
   }

   DLLLOCAL void parseFile(const char* filename, ExceptionSink* xsink, ExceptionSink* wS, int wm) {
      QORE_TRACE("QoreProgram::parseFile()");

      printd(5, "QoreProgram::parseFile(%s)\n", filename);

      FILE *fp;
      if (!(fp = fopen(filename, "r"))) {
         if ((only_first_except && !exceptions_raised) || !only_first_except)
            xsink->raiseErrnoException("PARSE-EXCEPTION", errno, "cannot open qore script '%s'", filename);
         exceptions_raised++;
         return;
      }
      ON_BLOCK_EXIT(fclose, fp);

      setScriptPath(filename);

      ProgramRuntimeParseCommitContextHelper pch(xsink, pgm);
      if (*xsink)
         return;

      parse(fp, filename, xsink, wS, wm);
   }

   DLLLOCAL void parsePending(const QoreString *str, const QoreString *lstr, ExceptionSink* xsink, ExceptionSink* wS, int wm, const QoreString* source = 0, int offset = 0) {
      assert(!str->empty());
      assert(xsink);

      // ensure code string has correct character set encoding
      TempEncodingHelper tstr(str, QCS_DEFAULT, xsink);
      if (*xsink)
         return;

      // ensure label string has correct character set encoding
      TempEncodingHelper tlstr(lstr, QCS_DEFAULT, xsink);
      if (*xsink)
         return;

      TempEncodingHelper src;
      if (source && !source->empty() && !src.set(source, QCS_DEFAULT, xsink))
         return;

      parsePending(tstr->getBuffer(), tlstr->getBuffer(), xsink, wS, wm, source ? src->getBuffer() : 0, offset);
   }

   // called during run time (not during parsing)
   DLLLOCAL void importFunction(ExceptionSink* xsink, QoreFunction *u, const qore_ns_private& oldns, const char* new_name = 0, bool inject = false);

   DLLLOCAL void del(ExceptionSink* xsink);

   DLLLOCAL QoreHashNode* getThreadData() {
      QoreHashNode* h = thread_local_storage->get();
      if (!h) {
         h = new QoreHashNode;
         thread_local_storage->set(h);
      }

      return h;
   }

   DLLLOCAL QoreHashNode* clearThreadData(ExceptionSink* xsink) {
      QoreHashNode* h = thread_local_storage->get();
      printd(5, "QoreProgram::clearThreadData() this: %p h: %p (size: %d)\n", this, h, h ? h->size() : 0);
      if (h)
         h->clear(xsink);
      return h;
   }

   DLLLOCAL void deleteThreadData(ExceptionSink* xsink) {
      QoreHashNode* h = clearThreadData(xsink);
      if (h) {
         h->deref(xsink);
         thread_local_storage->set(0);
      }
   }

   DLLLOCAL void finalizeThreadData(ThreadProgramData* td, arg_vec_t*& cl) {
      QoreHashNode* h = thread_local_storage->get();
      if (h) {
         if (!cl)
            cl = new arg_vec_t;
         cl->push_back(h);
         thread_local_storage->set(0);
      }

      // delete all local variables for this thread
      AutoLocker al(tlock);
      if (tclear)
         return;

      pgm_data_map_t::iterator i = pgm_data_map.find(td);
      if (i != pgm_data_map.end())
         i->second->finalize(cl);
   }

   // TODO: xsink should not be necessary; vars should be emptied and finalized in the finalizeThreadData() call
   DLLLOCAL int endThread(ThreadProgramData* td, ExceptionSink* xsink) {
      ThreadLocalProgramData* tlpd = 0;

      // delete all local variables for this thread
      {
         AutoLocker al(tlock);
         if (tclear)
            return -1;

         pgm_data_map_t::iterator i = pgm_data_map.find(td);
         if (i == pgm_data_map.end())
            return -1;
         tlpd = i->second;
         pgm_data_map.erase(i);
      }

      tlpd->del(xsink);
      return 0;
   }

   DLLLOCAL void doTopLevelInstantiation(ThreadLocalProgramData& tlpd) {
      // instantiate top-level vars for this thread
      const LVList* lvl = sb.getLVList();
      if (lvl)
         for (unsigned i = 0; i < lvl->size(); ++i)
            lvl->lv[i]->instantiate();

      //printd(5, "qore_program_private::doTopLevelInstantiation() lvl: %p setup %ld local vars pgm: %p\n", lvl, lvl ? lvl->size() : 0, getProgram());

      tlpd.inst = true;
   }

   // returns true if setting for the first time, false if not
   DLLLOCAL bool setThreadVarData(ThreadProgramData* td, ThreadLocalProgramData*& new_tlpd, bool run) {
      SafeLocker sl(tlock);
      // wait for data to finished being cleared if applicable
      while (tclear) {
         if (tclear == gettid()) {
            // can be called recursively when destructors are run in local variable finalization
            assert(pgm_data_map.find(td) != pgm_data_map.end());
            assert(pgm_data_map[td]->inst);
            break;
         }
         ++twaiting;
         tcond.wait(tlock);
         --twaiting;
      }

      pgm_data_map_t::iterator i = pgm_data_map.find(td);
      if (i == pgm_data_map.end()) {
         assert(!tclear);
         ThreadLocalProgramData* tlpd = new ThreadLocalProgramData;

         printd(5, "qore_program_private::setThreadVarData() (first) this: %p pgm: %p td: %p run: %s inst: %s\n", this, pgm, td, run ? "true" : "false", tlpd->inst ? "true" : "false");

         new_tlpd = tlpd;

         pgm_data_map.insert(pgm_data_map_t::value_type(td, tlpd));

         sl.unlock();

         if (run) {
            printd(5, "qore_program_private::setThreadVarData() (first) this: %p pgm: %p td: %p\n", this, pgm, td);
            doTopLevelInstantiation(*tlpd);
         }

         return true;
      }

      ThreadLocalProgramData* tlpd = pgm_data_map[td];
      new_tlpd = tlpd;

      sl.unlock();

      printd(5, "qore_program_private::setThreadVarData() (not first) this: %p pgm: %p td: %p run: %s inst: %s\n", this, pgm, td, run ? "true" : "false", tlpd->inst ? "true" : "false");

      if (run && !tlpd->inst) {
         doTopLevelInstantiation(*tlpd);
      }

      return false;
   }

   DLLLOCAL const AbstractQoreZoneInfo* currentTZ(ThreadProgramData* tpd = get_thread_program_data()) const {
      AutoLocker al(tlock);
      pgm_data_map_t::const_iterator i = pgm_data_map.find(tpd);
      if (i != pgm_data_map.end() && i->second->tz_set)
         return i->second->tz;
      return TZ;
   }

   DLLLOCAL void setTZ(const AbstractQoreZoneInfo* n_TZ) {
      TZ = n_TZ;
   }

   DLLLOCAL void exportFunction(ExceptionSink* xsink, qore_program_private* p, const char* name, const char* new_name = 0, bool inject = false) {
      if (this == p) {
         xsink->raiseException("FUNCTION-IMPORT-ERROR", "cannot import a function from the same Program object");
         return;
      }

      if (inject && !(p->pwo.parse_options & PO_ALLOW_INJECTION)) {
         xsink->raiseException("FUNCTION-IMPORT-ERROR", "cannot import function \"%s\" in a Program object without PO_ALLOW_INJECTION set", name);
         return;
      }

      const QoreFunction* u;
      const qore_ns_private* ns = 0;

      {
         ProgramRuntimeParseAccessHelper rah(xsink, pgm);
         if (*xsink)
            return;
         u = qore_root_ns_private::runtimeFindFunction(*RootNS, name, ns);
      }

      if (!u)
         xsink->raiseException("PROGRAM-IMPORTFUNCTION-NO-FUNCTION", "function '%s' does not exist in the current program scope", name);
      else {
         assert(ns);
         p->importFunction(xsink, const_cast<QoreFunction*>(u), *ns, new_name, inject);
      }
   }

   DLLLOCAL bool parseExceptionRaised() const {
      assert(parseSink);
      return *parseSink;
   }

   DLLLOCAL void setParseOptionsIntern(int64 po) {
      pwo.parse_options |= po;
   }

   DLLLOCAL void disableParseOptionsIntern(int64 po) {
      pwo.parse_options &= ~po;
   }

   DLLLOCAL void replaceParseOptionsIntern(int64 po) {
      pwo.parse_options = po;
   }

   DLLLOCAL int setParseOptions(int64 po, ExceptionSink* xsink) {
      assert(xsink);
      // only raise the exception if parse options are locked and the option is not a "free option"
      // also check if options may be made more restrictive and the option also does so
      if (!((po & PO_FREE_OPTIONS) == po) && po_locked && (!po_allow_restrict || (po & PO_POSITIVE_OPTIONS))) {
         xsink->raiseException("OPTIONS-LOCKED", "parse options have been locked on this program object");
         return -1;
      }

      setParseOptionsIntern(po);
      return 0;
   }

   DLLLOCAL int disableParseOptions(int64 po, ExceptionSink* xsink) {
      assert(xsink);
      // only raise the exception if parse options are locked and the option is not a "free option"
      // also check if options may be made more restrictive and the option also does so
      if (!((po & PO_FREE_OPTIONS) == po) && po_locked && (!po_allow_restrict || (po & PO_POSITIVE_OPTIONS))) {
         xsink->raiseException("OPTIONS-LOCKED", "parse options have been locked on this program object");
         return -1;
      }

      disableParseOptionsIntern(po);
      return 0;
   }

   DLLLOCAL int replaceParseOptions(int64 po, ExceptionSink* xsink) {
      assert(xsink);
      if (!(getProgram()->priv->pwo.parse_options & PO_NO_CHILD_PO_RESTRICTIONS)) {
         xsink->raiseException("OPTION-ERROR", "the calling Program does not have the PO_NO_CHILD_PO_RESTRICTIONS option set, and therefore cannot call Program::replaceParseOptions()");
         return -1;
      }

      //printd(5, "qore_program_private::replaceParseOptions() this: %p pgm: %p replacing po: %lld with po: %lld\n", this, pgm, pwo.parse_options, po);
      replaceParseOptionsIntern(po);
      return 0;
   }

   DLLLOCAL int parseSetParseOptions(const QoreProgramLocation& loc, int64 po) {
      // only raise the exception if parse options are locked and the option is not a "free option"
      // also check if options may be made more restrictive and the option also does so
      if (!((po & PO_FREE_OPTIONS) == po) && po_locked && (!po_allow_restrict || (po & PO_POSITIVE_OPTIONS))) {
         parse_error(loc, "parse options have been locked on this program object");
         return -1;
      }

      setParseOptionsIntern(po);
      return 0;
   }

   DLLLOCAL int parseDisableParseOptions(const QoreProgramLocation& loc, int64 po) {
      // only raise the exception if parse options are locked and the option is not a "free option"
      // also check if options may be made more restrictive and the option also does so
      if (!((po & PO_FREE_OPTIONS) == po) && po_locked && (!po_allow_restrict || (po & PO_POSITIVE_OPTIONS))) {
         parse_error(loc, "parse options have been locked on this program object");
         return -1;
      }

      disableParseOptionsIntern(po);
      return 0;
   }

   DLLLOCAL void parseSetTimeZone(const char* zone) {
      // check PO_NO_LOCALE_CONTROL
      ExceptionSink xsink;
      if (pwo.parse_options & PO_NO_LOCALE_CONTROL) {
         mergeParseException(xsink);
         return;
      }

      const AbstractQoreZoneInfo *tz = (*zone == '-' || *zone == '+') ? QTZM.findCreateOffsetZone(zone, &xsink) : QTZM.findLoadRegion(zone, &xsink);
      if (xsink) {
         assert(!tz);
         mergeParseException(xsink);
         return;
      }
      // note that tz may be NULL in case the offset is UTC (ie 0)
      assert(tz || ((*zone == '-' || *zone == '+')));

      TZ = tz;
   }

   DLLLOCAL void makeParseException(const char* err, QoreStringNode* desc) {
      QoreStringNodeHolder d(desc);
      if (!requires_exception) {
         assert(parseSink);
         QoreException *ne = new ParseException(err, d.release());
         parseSink->raiseException(ne);
      }
   }

   DLLLOCAL void makeParseException(QoreStringNode* desc) {
      QoreStringNodeHolder d(desc);
      if (!requires_exception) {
         if ((only_first_except && !exceptions_raised) || !only_first_except) {
            QoreException *ne = new ParseException("PARSE-EXCEPTION", d.release());
            parseSink->raiseException(ne);
         }
         exceptions_raised++;
      }
   }

   DLLLOCAL void makeParseException(const QoreProgramLocation &loc, const char* err, QoreStringNode* desc) {
      QORE_TRACE("QoreProgram::makeParseException()");

      QoreStringNodeHolder d(desc);
      if (!requires_exception) {
         if ((only_first_except && !exceptions_raised) || !only_first_except) {
            QoreException *ne = new ParseException(loc, err, d.release());
            parseSink->raiseException(ne);
         }
         exceptions_raised++;
      }
   }

   DLLLOCAL void parseException(QoreProgramLocation &loc, const char* fmt, ...) {
      //printd(5, "qore_program_private::parseException(\"%s\", ...) called\n", fmt);

      // ignore if a "requires" exception has been raised
      if (requires_exception)
         return;

      QoreStringNode* desc = new QoreStringNode;
      while (true) {
         va_list args;
         va_start(args, fmt);
         int rc = desc->vsprintf(fmt, args);
         va_end(args);
         if (!rc)
            break;
      }
      makeParseException(loc, "PARSE-EXCEPTION", desc);
   }

   // internal method - does not bother with the parse lock
   // returns true if the define already existed
   DLLLOCAL bool setDefine(const char* name, AbstractQoreNode* v, ExceptionSink* xsink) {
      dmap_t::iterator i = dmap.find(name);
      if (i != dmap.end()) {
         if (i->second)
            i->second->deref(xsink);
         i->second = v;
         return true;
      }

      dmap[name] = v;
      return false;
   }

   // internal method - does not bother with the parse lock
   DLLLOCAL const AbstractQoreNode* getDefine(const char* name, bool& is_defined) {
      dmap_t::iterator i = dmap.find(name);
      if (i != dmap.end()) {
         is_defined = true;
         return i->second;
      }
      is_defined = false;
      return 0;
   }

   DLLLOCAL AbstractQoreNode* runTimeGetDefine(const char* name, bool& is_defined) {
      AutoLocker al(plock);
      const AbstractQoreNode* rv = getDefine(name, is_defined);
      return rv ? rv->refSelf() : 0;
   }

   // internal method - does not bother with the parse lock
   // returns true if the define existed
   DLLLOCAL bool unDefine(const char* name, ExceptionSink* xsink) {
      dmap_t::iterator i = dmap.find(name);
      if (i != dmap.end()) {
         if (i->second)
            i->second->deref(xsink);
         dmap.erase(i);
         return true;
      }
      return false;
   }

   DLLLOCAL bool parseUnDefine(const char* name) {
      PreParseHelper pph(this);
      return unDefine(name, parseSink);
   }

   DLLLOCAL bool runTimeUnDefine(const char* name, ExceptionSink* xsink) {
      AutoLocker al(plock);
      return unDefine(name, xsink);
   }

   // internal method - does not bother with the parse lock
   DLLLOCAL bool isDefined(const char* name) {
      dmap_t::iterator i = dmap.find(name);
      return i == dmap.end() ? false : true;
   }

   DLLLOCAL bool runTimeIsDefined(const char* name) {
      AutoLocker al(plock);
      return isDefined(name);
   }

   DLLLOCAL int checkDefine(const QoreProgramLocation &loc, const char* str, ExceptionSink* xsink) {
      const char* p = str;
      if (!isalpha(*p)) {
         xsink->raiseException(loc, "PARSE-EXCEPTION", 0, "illegal define variable '%s'; does not begin with an alphabetic character", p);
         return -1;
      }

      while (*(++p)) {
         if (!isalnum(*p) && *p != '_') {

            xsink->raiseException(loc, "PARSE-EXCEPTION", 0, "illegal character '%c' in define variable '%s'", *p, str);
            return -1;
         }
      }

      return 0;
   }

   DLLLOCAL void parseDefine(const QoreProgramLocation &loc, const char* str, AbstractQoreNode* val) {
      PreParseHelper pph(this);

      if (checkDefine(loc, str, parseSink))
         return;

      setDefine(str, val, parseSink);
   }

   DLLLOCAL void runTimeDefine(const char* str, AbstractQoreNode* val, ExceptionSink* xsink) {
      QoreProgramLocation loc(RunTimeLocation);

      if (checkDefine(loc, str, xsink))
         return;

      AutoLocker al(plock);
      setDefine(str, val, xsink);
   }

   DLLLOCAL ResolvedCallReferenceNode* runtimeGetCallReference(const char* name, ExceptionSink* xsink) {
      assert(xsink);
      ProgramRuntimeParseAccessHelper pah(xsink, pgm);
      if (*xsink)
         return 0;
      return qore_root_ns_private::runtimeGetCallReference(*RootNS, name, xsink);
   }

   DLLLOCAL void pushParseOptions(const char* pf) {
      // ignore %push-parse-options used multiple times in the same file
      ppo_t::iterator i = ppo.lower_bound(pf);
      if (i != ppo.end() && !strcmp(pf, i->first))
         return;
      ppo.insert(ppo_t::value_type(pf, pwo.parse_options));
      //printd(5, "qore_program_private::pushParseOptions() this: %p %p '%s' saving %lld\n", this, pf, pf, pwo.parse_options);
   }

   DLLLOCAL void restoreParseOptions(const char* pf) {
      ppo_t::iterator i = ppo.find(pf);
      if (i != ppo.end()) {
         //printd(5, "qore_program_private::restoreParseOptions() this: %p %p '%s' restoring %lld\n", this, pf, pf, pwo.parse_options);
         pwo.parse_options = i->second;
         ppo.erase(i);
      }
#if 0
      else {
         printd(5, "qore_program_private::restoreParseOptions() this: %p %p '%s' parse options not found\n", this, pf, pf, pwo.parse_options);
         for (ppo_t::iterator i = ppo.begin(), e = ppo.end(); i != e; ++i) {
            printd(5, " + ppo: %p '%s' = %lld\n", i->first, i->first, i->second);
         }
      }
#endif
   }

   DLLLOCAL void addParseException(ExceptionSink& xsink, QoreProgramLocation* loc = 0) {
      if (requires_exception) {
         xsink.clear();
         return;
      }

      // ensure that all exceptions reflect the current parse location
      if (!loc) {
         QoreProgramLocation pl(ParseLocation);
         xsink.overrideLocation(pl);
      }
      else
         xsink.overrideLocation(*loc);

      parseSink->assimilate(xsink);
   }

   // returns the mask of domain options not met in the current program
   DLLLOCAL int64 parseAddDomain(int64 n_dom) {
      assert(!(n_dom & PO_FREE_OPTIONS));
      int64 rv = 0;

      // handle negative and positive options separately / differently
      int64 pos = (n_dom & PO_POSITIVE_OPTIONS);
      if (pos) {
         int64 p_tmp = pwo.parse_options & PO_POSITIVE_OPTIONS;
         // make sure all positive arguments are set
         if ((pos & p_tmp) != pos) {
            rv = ((pos & p_tmp) ^ pos);
            pend_dom |= pos;
         }
      }
      int64 neg = (n_dom & ~PO_POSITIVE_OPTIONS);
      if (neg && (neg & pwo.parse_options)) {
         rv |= (neg & pwo.parse_options);
         pend_dom |= neg;
      }

      //printd(5, "qore_program_private::parseAddDomain() this: %p n_dom: " QLLD " po: " QLLD "\n", this, n_dom, pwo.parse_options);
      return rv;
   }

   DLLLOCAL void exportGlobalVariable(const char* name, bool readonly, qore_program_private& tpgm, ExceptionSink* xsink);

   DLLLOCAL int setGlobalVarValue(const char* name, QoreValue val, ExceptionSink* xsink);

   // returns true if there was already a thread init closure set, false if not
   DLLLOCAL bool setThreadInit(const ResolvedCallReferenceNode* n_thr_init, ExceptionSink* xsink);

   DLLLOCAL void setThreadTZ(ThreadProgramData* tpd, const AbstractQoreZoneInfo* tz) {
      AutoLocker al(tlock);
      pgm_data_map_t::iterator i = pgm_data_map.find(tpd);
      assert(i != pgm_data_map.end());
      i->second->setTZ(tz);
   }

   DLLLOCAL const AbstractQoreZoneInfo* getThreadTZ(ThreadProgramData* tpd, bool& set) const {
      AutoLocker al(tlock);
      pgm_data_map_t::const_iterator i = pgm_data_map.find(tpd);
      assert(i != pgm_data_map.end());
      set = i->second->tz_set;
      return i->second->tz;
   }

   DLLLOCAL void clearThreadTZ(ThreadProgramData* tpd) {
      AutoLocker al(tlock);
      pgm_data_map_t::iterator i = pgm_data_map.find(tpd);
      assert(i != pgm_data_map.end());
      i->second->clearTZ();
   }

   DLLLOCAL void addStatement(AbstractStatement* s) {
      sb.addStatement(s);

      // see if top level statements are allowed
      if (pwo.parse_options & PO_NO_TOP_LEVEL_STATEMENTS && !s->isDeclaration())
         parse_error(s->loc, "illegal top-level statement (conflicts with parse option NO_TOP_LEVEL_STATEMENTS)");
   }

   DLLLOCAL void importClass(ExceptionSink* xsink, qore_program_private& from_pgm, const char* path, const char* new_name = 0, bool inject = false);

   DLLLOCAL void addFile(char* f) {
      fileList.push_back(f);
   }

   DLLLOCAL void addUserFeature(const char* f) {
      userFeatureList.push_back(f);
   }

   DLLLOCAL void runtimeImportSystemClassesIntern(const qore_program_private& spgm, ExceptionSink* xsink);
   DLLLOCAL void runtimeImportSystemConstantsIntern(const qore_program_private& spgm, ExceptionSink* xsink);
   DLLLOCAL void runtimeImportSystemFunctionsIntern(const qore_program_private& spgm, ExceptionSink* xsink);
   DLLLOCAL void runtimeImportSystemHashDeclsIntern(const qore_program_private& spgm, ExceptionSink* xsink);

   DLLLOCAL void runtimeImportSystemClasses(ExceptionSink* xsink);
   DLLLOCAL void runtimeImportSystemConstants(ExceptionSink* xsink);
   DLLLOCAL void runtimeImportSystemFunctions(ExceptionSink* xsink);
   DLLLOCAL void runtimeImportSystemHashDecls(ExceptionSink* xsink);
   DLLLOCAL void runtimeImportSystemApi(ExceptionSink* xsink);

   DLLLOCAL void doThreadInit(ExceptionSink* xsink);

   DLLLOCAL QoreClass* runtimeFindClass(const char* class_name, ExceptionSink* xsink) const;

   DLLLOCAL void setExternalData(const char* owner, AbstractQoreProgramExternalData* pud) {
      AutoLocker al(plock);
      assert(extmap.find(owner) == extmap.end());
      extmap.insert(extmap_t::value_type(owner, pud));
   }

   DLLLOCAL AbstractQoreProgramExternalData* getExternalData(const char* owner) const {
      AutoLocker al(plock);
      extmap_t::const_iterator i = extmap.find(owner);
      return i == extmap.end() ? nullptr : i->second;
   }

   DLLLOCAL QoreHashNode* getGlobalVars() const {
      return qore_root_ns_private::getGlobalVars(*RootNS);
   }

   DLLLOCAL LocalVar* createLocalVar(const char* name, const QoreTypeInfo* typeInfo);

   DLLLOCAL const QoreTypeInfo* getComplexHashType(const QoreTypeInfo* vti);
   DLLLOCAL const QoreTypeInfo* getComplexHashOrNothingType(const QoreTypeInfo* vti);
   DLLLOCAL const QoreTypeInfo* getComplexListType(const QoreTypeInfo* vti);
   DLLLOCAL const QoreTypeInfo* getComplexListOrNothingType(const QoreTypeInfo* vti);
   DLLLOCAL const QoreTypeInfo* getComplexReferenceType(const QoreTypeInfo* vti);
   DLLLOCAL const QoreTypeInfo* getComplexReferenceOrNothingType(const QoreTypeInfo* vti);

   DLLLOCAL static QoreClass* runtimeFindClass(const QoreProgram& pgm, const char* class_name, ExceptionSink* xsink) {
      return pgm.priv->runtimeFindClass(class_name, xsink);
   }

   DLLLOCAL static void doThreadInit(QoreProgram& pgm, ExceptionSink* xsink) {
      pgm.priv->doThreadInit(xsink);
   }

   DLLLOCAL static int setReturnValue(QoreProgram& pgm, AbstractQoreNode* val, ExceptionSink* xsink) {
      ReferenceHolder<> rv(val, xsink);
      if (!pgm.priv->exec_class) {
         xsink->raiseException("SETRETURNVALUE-ERROR", "cannot set return value when not running in %%exec-class mode; in this case simply return the value directly (or call exit(<val>))");
         return -1;
      }
      discard(pgm.priv->exec_class_rv, xsink);
      pgm.priv->exec_class_rv = rv.release();
      return 0;
   }

   // called when starting a new thread before the new thread is started, to avoid race conditions
   DLLLOCAL static int preregisterNewThread(QoreProgram& pgm, ExceptionSink* xsink) {
      return pgm.priv->preregisterNewThread(xsink);
   }

   // called when thread startup fails after preregistration
   DLLLOCAL static void cancelPreregistration(QoreProgram& pgm) {
      pgm.priv->cancelPreregistration();
   }

   // called from the new thread once the thread has been started (after preregisterNewThread())
   DLLLOCAL static void registerNewThread(QoreProgram& pgm, int tid) {
      pgm.priv->registerNewThread(tid);
   }

   DLLLOCAL static void runtimeImportSystemClasses(QoreProgram& pgm, ExceptionSink* xsink) {
      pgm.priv->runtimeImportSystemClasses(xsink);
   }

   DLLLOCAL static void runtimeImportSystemHashDecls(QoreProgram& pgm, ExceptionSink* xsink) {
      pgm.priv->runtimeImportSystemHashDecls(xsink);
   }

   DLLLOCAL static void runtimeImportSystemConstants(QoreProgram& pgm, ExceptionSink* xsink) {
      pgm.priv->runtimeImportSystemConstants(xsink);
   }

   DLLLOCAL static void runtimeImportSystemFunctions(QoreProgram& pgm, ExceptionSink* xsink) {
      pgm.priv->runtimeImportSystemFunctions(xsink);
   }

   DLLLOCAL static void runtimeImportSystemApi(QoreProgram& pgm, ExceptionSink* xsink) {
      pgm.priv->runtimeImportSystemApi(xsink);
   }

   DLLLOCAL static qore_program_private* get(QoreProgram& pgm) {
      return pgm.priv;
   }

   DLLLOCAL static void clearThreadData(QoreProgram& pgm, ExceptionSink* xsink) {
      pgm.priv->clearThreadData(xsink);
   }

   DLLLOCAL static void addUserFeature(QoreProgram& pgm, const char* f) {
      pgm.priv->addUserFeature(f);
   }

   DLLLOCAL static void addFile(QoreProgram& pgm, char* f) {
      pgm.priv->addFile(f);
   }

   DLLLOCAL static void importClass(ExceptionSink* xsink, QoreProgram& pgm, QoreProgram& from_pgm, const char* path, const char* new_name = 0, bool inject = false) {
      pgm.priv->importClass(xsink, *(from_pgm.priv), path, new_name, inject);
   }

   DLLLOCAL static void addStatement(QoreProgram& pgm, AbstractStatement* s) {
      pgm.priv->addStatement(s);
   }

   DLLLOCAL static const AbstractQoreZoneInfo* currentTZIntern(QoreProgram& pgm) {
      return pgm.priv->TZ;
   }

   DLLLOCAL static int lockParsing(QoreProgram& pgm, ExceptionSink* xsink) {
      return pgm.priv->lockParsing(xsink);
   }

   DLLLOCAL static void unlockParsing(QoreProgram& pgm) {
      pgm.priv->unlockParsing();
   }

   DLLLOCAL static int incThreadCount(QoreProgram& pgm, ExceptionSink* xsink) {
      return pgm.priv->incThreadCount(xsink);
   }

   DLLLOCAL static void decThreadCount(QoreProgram& pgm, int tid) {
      pgm.priv->decThreadCount(tid);
   }

   // locking is done by the signal manager
   DLLLOCAL static void addSignal(QoreProgram& pgm, int sig) {
      assert(pgm.priv->sigset.find(sig) == pgm.priv->sigset.end());
      pgm.priv->sigset.insert(sig);
   }

   // locking is done by the signal manager
   DLLLOCAL static void delSignal(QoreProgram& pgm, int sig) {
      assert(pgm.priv->sigset.find(sig) != pgm.priv->sigset.end());
      pgm.priv->sigset.erase(sig);
   }

   DLLLOCAL static void startThread(QoreProgram& pgm, ExceptionSink& xsink) {
      pgm.priv->qore_program_private_base::startThread(xsink);
   }

   DLLLOCAL static bool setThreadInit(QoreProgram& pgm, const ResolvedCallReferenceNode* n_thr_init, ExceptionSink* xsink) {
      return pgm.priv->setThreadInit(n_thr_init, xsink);
   }

   DLLLOCAL static ResolvedCallReferenceNode* runtimeGetCallReference(QoreProgram* pgm, const char* name, ExceptionSink* xsink) {
      return pgm->priv->runtimeGetCallReference(name, xsink);
   }

   DLLLOCAL static const ParseWarnOptions& getParseWarnOptions(const QoreProgram* pgm) {
      return pgm->priv->pwo;
   }

   DLLLOCAL static bool setSaveParseWarnOptions(const QoreProgram* pgm, const ParseWarnOptions &new_opts, ParseWarnOptions &old_opts) {
      if (new_opts == pgm->priv->pwo)
         return false;
      old_opts = pgm->priv->pwo;
      pgm->priv->pwo = new_opts;
      return true;
   }

   DLLLOCAL static void setParseWarnOptions(const QoreProgram* pgm, const ParseWarnOptions &new_opts) {
      pgm->priv->pwo = new_opts;
   }

   DLLLOCAL static bool setThreadVarData(QoreProgram* pgm, ThreadProgramData* td, ThreadLocalProgramData* &tlpd, bool run) {
      return pgm->priv->setThreadVarData(td, tlpd, run);
   }

   DLLLOCAL static void finalizeThreadData(QoreProgram* pgm, ThreadProgramData* td, arg_vec_t*& cl) {
      pgm->priv->finalizeThreadData(td, cl);
   }

   DLLLOCAL static int endThread(QoreProgram* pgm, ThreadProgramData* td, ExceptionSink* xsink) {
      return pgm->priv->endThread(td, xsink);
   }

   DLLLOCAL static void makeParseException(QoreProgram* pgm, const char* err, QoreStringNode* desc) {
      pgm->priv->makeParseException(err, desc);
   }

   DLLLOCAL static void makeParseException(QoreProgram* pgm, QoreStringNode* desc) {
      pgm->priv->makeParseException(desc);
   }

   DLLLOCAL static void makeParseException(QoreProgram* pgm, const QoreProgramLocation &loc, QoreStringNode* desc) {
      pgm->priv->makeParseException(loc, "PARSE-EXCEPTION", desc);
   }

   DLLLOCAL static void makeParseException(QoreProgram* pgm, const QoreProgramLocation &loc, const char* err, QoreStringNode* desc) {
      pgm->priv->makeParseException(loc, err, desc);
   }

   DLLLOCAL static const AbstractQoreNode* parseGetDefine(QoreProgram* pgm, const char* name) {
      bool is_defined;
      return pgm->priv->getDefine(name, is_defined);
   }

   DLLLOCAL static const AbstractQoreNode* parseGetDefine(QoreProgram* pgm, const char* name, bool& is_defined) {
      return pgm->priv->getDefine(name, is_defined);
   }

   DLLLOCAL static AbstractQoreNode* runTimeGetDefine(QoreProgram* pgm, const char* name) {
      bool is_defined;
      return pgm->priv->runTimeGetDefine(name, is_defined);
   }

   DLLLOCAL static AbstractQoreNode* runTimeGetDefine(QoreProgram* pgm, const char* name, bool& is_defined) {
      return pgm->priv->runTimeGetDefine(name, is_defined);
   }

   DLLLOCAL static bool parseUnDefine(QoreProgram* pgm, const char* name) {
      return pgm->priv->parseUnDefine(name);
   }

   DLLLOCAL static bool runTimeUnDefine(QoreProgram* pgm, const char* name, ExceptionSink* xsink) {
      return pgm->priv->runTimeUnDefine(name, xsink);
   }

   DLLLOCAL static bool parseIsDefined(QoreProgram* pgm, const char* name) {
      return pgm->priv->isDefined(name);
   }

   DLLLOCAL static bool runTimeIsDefined(QoreProgram* pgm, const char* name) {
      return pgm->priv->runTimeIsDefined(name);
   }

   DLLLOCAL static void parseDefine(QoreProgram* pgm, QoreProgramLocation loc, const char* str, AbstractQoreNode* val) {
      pgm->priv->parseDefine(loc, str, val);
   }

   DLLLOCAL static void runTimeDefine(QoreProgram* pgm, const char* str, AbstractQoreNode* val, ExceptionSink* xsink) {
      pgm->priv->runTimeDefine(str, val, xsink);
   }

   DLLLOCAL static void pushParseOptions(QoreProgram* pgm, const char* pf) {
      pgm->priv->pushParseOptions(pf);
   }

   DLLLOCAL static void restoreParseOptions(QoreProgram* pgm, const char* pf) {
      pgm->priv->restoreParseOptions(pf);
   }

   DLLLOCAL static void addParseException(QoreProgram* pgm, ExceptionSink* xsink, QoreProgramLocation* loc = 0) {
      assert(xsink);
      pgm->priv->addParseException(*xsink, loc);
      delete xsink;
   }

   DLLLOCAL static void addParseException(QoreProgram* pgm, ExceptionSink& xsink, QoreProgramLocation* loc = 0) {
      pgm->priv->addParseException(xsink, loc);
   }

   DLLLOCAL static void exportFunction(QoreProgram* srcpgm, ExceptionSink* xsink, QoreProgram* trgpgm, const char* name, const char* new_name = 0, bool inject = false) {
      srcpgm->priv->exportFunction(xsink, trgpgm->priv, name, new_name, inject);
   }

   DLLLOCAL static int64 parseAddDomain(QoreProgram* pgm, int64 n_dom) {
      return pgm->priv->parseAddDomain(n_dom);
   }

   DLLLOCAL static int64 getDomain(const QoreProgram& pgm) {
      return pgm.priv->dom;
   }

   DLLLOCAL static void runtimeAddDomain(QoreProgram& pgm, int64 n_dom) {
      pgm.priv->dom |= n_dom;
   }

   DLLLOCAL static int64 forceReplaceParseOptions(QoreProgram& pgm, int64 po) {
      int64 rv = pgm.priv->pwo.parse_options;
      pgm.priv->pwo.parse_options = po;
      return rv;
   }

   DLLLOCAL static void makeParseWarning(QoreProgram* pgm, int code, const char* warn, const char* fmt, ...) {
      //printd(5, "QP::mPW(code: %d, warn: '%s', fmt: '%s') priv->pwo.warn_mask: %d priv->warnSink: %p %s\n", code, warn, fmt, priv->pwo.warn_mask, priv->warnSink, priv->warnSink && (code & priv->pwo.warn_mask) ? "OK" : "SKIPPED");
      if (!pgm->priv->warnSink || !(code & pgm->priv->pwo.warn_mask))
         return;

      QoreStringNode* desc = new QoreStringNode;
      while (true) {
         va_list args;
         va_start(args, fmt);
         int rc = desc->vsprintf(fmt, args);
         va_end(args);
         if (!rc)
            break;
      }
      QoreException *ne = new ParseException(warn, desc);
      pgm->priv->warnSink->raiseException(ne);
   }

   DLLLOCAL static void makeParseWarning(QoreProgram* pgm, const QoreProgramLocation &loc, int code, const char* warn, const char* fmt, ...) {
      //printd(5, "QP::mPW(code: %d, warn: '%s', fmt: '%s') priv->pwo.warn_mask: %d priv->warnSink: %p %s\n", code, warn, fmt, priv->pwo.warn_mask, priv->warnSink, priv->warnSink && (code & priv->pwo.warn_mask) ? "OK" : "SKIPPED");
      if (!pgm->priv->warnSink || !(code & pgm->priv->pwo.warn_mask))
         return;

      QoreStringNode* desc = new QoreStringNode;
      while (true) {
         va_list args;
         va_start(args, fmt);
         int rc = desc->vsprintf(fmt, args);
         va_end(args);
         if (!rc)
            break;
      }
      QoreException *ne = new ParseException(loc, warn, desc);
      pgm->priv->warnSink->raiseException(ne);
   }

   DLLLOCAL static void makeParseWarning(QoreProgram* pgm, int code, const char* warn, QoreStringNode* desc) {
      //printd(5, "QoreProgram::makeParseWarning(code: %d, warn: '%s', desc: '%s') priv->pwo.warn_mask: %d priv->warnSink: %p %s\n", code, warn, desc->getBuffer(), priv->pwo.warn_mask, priv->warnSink, priv->warnSink && (code & priv->pwo.warn_mask) ? "OK" : "SKIPPED");
      if (!pgm->priv->warnSink || !(code & pgm->priv->pwo.warn_mask)) {
         desc->deref();
         return;
      }

      QoreException *ne = new ParseException(warn, desc);
      pgm->priv->warnSink->raiseException(ne);
   }

   DLLLOCAL static void makeParseWarning(QoreProgram* pgm, const QoreProgramLocation &loc, int code, const char* warn, QoreStringNode* desc) {
      //printd(5, "QoreProgram::makeParseWarning(code: %d, warn: '%s', desc: '%s') priv->pwo.warn_mask: %d priv->warnSink: %p %s\n", code, warn, desc->getBuffer(), priv->pwo.warn_mask, priv->warnSink, priv->warnSink && (code & priv->pwo.warn_mask) ? "OK" : "SKIPPED");
      if (!pgm->priv->warnSink || !(code & pgm->priv->pwo.warn_mask)) {
         desc->deref();
         return;
      }

      QoreException *ne = new ParseException(loc, warn, desc);
      pgm->priv->warnSink->raiseException(ne);
   }

   DLLLOCAL static void exportGlobalVariable(QoreProgram* pgm, const char* name, bool readonly, QoreProgram* tpgm, ExceptionSink* xsink) {
      pgm->priv->exportGlobalVariable(name, readonly, *(tpgm->priv), xsink);
   }

   DLLLOCAL void attachDebug(const qore_debug_program_private* n_dpgm) {
      printd(5, "qore_program_private::attachDebug(n_dpgm: %p), dpgm: %p\n", n_dpgm, dpgm);
      QoreAutoRWWriteLocker al(&lck_debug_program);

      if (dpgm == n_dpgm) return;
      dpgm = const_cast<qore_debug_program_private*>(n_dpgm);
      printd(5, "qore_program_private::attachDebug, dpgm: %p, pgm_data_map: size:%d, begin: %p, end: %p\n", dpgm, pgm_data_map.size(), pgm_data_map.begin(), pgm_data_map.end());
      AutoLocker al2(tlock);
      for (pgm_data_map_t::iterator i = pgm_data_map.begin(), e = pgm_data_map.end(); i != e; ++i) {
         i->second->dbgPendingAttach();
         i->second->dbgBreak();
      }
   }

   DLLLOCAL void detachDebug(const qore_debug_program_private* n_dpgm) {
      printd(5, "qore_program_private::detachDebug(n_dpgm: %p), dpgm: %p\n", n_dpgm, dpgm);
      QoreAutoRWWriteLocker al(&lck_debug_program);
      assert(n_dpgm==dpgm);
      if (!n_dpgm) return;
      dpgm = 0;
      printd(5, "qore_program_private::detachDebug, dpgm: %p, pgm_data_map: size:%d, begin: %p, end: %p\n", dpgm, pgm_data_map.size(), pgm_data_map.begin(), pgm_data_map.end());
      AutoLocker al2(tlock);
      for (pgm_data_map_t::iterator i = pgm_data_map.begin(), e = pgm_data_map.end(); i != e; ++i) {
         i->second->dbgPendingDetach();
      }
      // debug_program_counter may be non zero to finish pending calls. Just this instance cannot be deleted, it's satisfied in destructor
   }

   DLLLOCAL void onAttach(ThreadDebugEnum &sb, ExceptionSink* xsink);
   DLLLOCAL void onDetach(ThreadDebugEnum &sb, ExceptionSink* xsink);
   DLLLOCAL void onStep(const StatementBlock *blockStatement, const AbstractStatement *statement, int &retCode, ThreadDebugEnum &sb, ExceptionSink* xsink);
   DLLLOCAL void onFunctionEnter(const StatementBlock *statement, ThreadDebugEnum &sb, ExceptionSink* xsink);
   DLLLOCAL void onFunctionExit(const StatementBlock *statement, QoreValue& returnValue, ThreadDebugEnum &sb, ExceptionSink* xsink);
   DLLLOCAL void onException(const AbstractStatement *statement, ThreadDebugEnum &sb, ExceptionSink* xsink);

   DLLLOCAL void breakProgramThread(int tid) {
      printd(5, "qore_program_private::breakProgramThread(), this: %p, tid: %d\n", this, gettid());
      AutoLocker al(tlock);
      for (pgm_data_map_t::iterator i = pgm_data_map.begin(), e = pgm_data_map.end(); i != e; ++i) {
         if (i->first->gettid() == tid) {
            i->second->dbgBreak();
            break;
         }
      }
   }

   DLLLOCAL void breakProgram() {
      printd(5, "qore_program_private::breakProgram(), this: %p\n", this);
      AutoLocker al(tlock);
      for (pgm_data_map_t::iterator i = pgm_data_map.begin(), e = pgm_data_map.end(); i != e; ++i) {
         i->second->dbgBreak();
      }
   }

   DLLLOCAL void assignBreakpoint(QoreBreakpoint *bkpt) {
      if (!bkpt || this == bkpt->pgm) return;
      if (bkpt->pgm) {
         bkpt->assignProgram(0, 0);
      }
      QoreAutoRWWriteLocker al(&lck_breakpoint);
      breakpointList.push_back(bkpt);
      bkpt->pgm = this;
      bkpt->ref();
   }

   DLLLOCAL void deleteAllBreakpoints() {
      QoreAutoRWWriteLocker al(&lck_breakpoint);
      for (std::list<QoreBreakpoint*>::iterator it = breakpointList.begin(); it != breakpointList.end(); ++it) {
         (*it)->unassignAllStatements();
         (*it)->pgm = 0;
         (*it)->deref();
      }
      breakpointList.clear();
   }

   DLLLOCAL void getBreakpoints(QoreBreakpointList_t &bkptList) {
      QoreAutoRWReadLocker al(&lck_breakpoint);
      bkptList.clear();
      for (std::list<QoreBreakpoint*>::iterator it = breakpointList.begin(); it != breakpointList.end(); ++it) {
         bkptList.push_back(*it);
         (*it)->ref();
      }
   }

   DLLLOCAL inline bool onCheckBreakpoint(const AbstractStatement *statement, ExceptionSink* xsink) {
      QoreAutoRWReadLocker al(&lck_breakpoint);
      return statement->getBreakpoint() != 0;
   }

   DLLLOCAL QoreStringNode* getStatementId(const AbstractStatement* statement) const {
      AutoLocker al(&plock);
      if (!statement)
         return 0;
      char buff[2*(sizeof(this))+5+ 2*sizeof(statement)+5 +1+1];  // printf %p is implementation specific
      snprintf(buff, sizeof(buff), "%p-%p", this, statement);
      return new QoreStringNode(buff);
   }

   DLLLOCAL AbstractStatement* resolveStatementId(const char *statementId) const {
      AutoLocker al(&plock);
      if (!statementId)
         return 0;
      qore_program_private *p;
      AbstractStatement *s;
      // we cannot use dynamic_cast to change if it is correct object as it required non general void* pointer
      int n;
      if ((n = sscanf(statementId, "%p-%p", &p, &s) != 2)) {
         printd(5, "AbstractStatement::resolveStatementId(%s), n:%d\n", statementId, n);
         return 0;
      }
      printd(5, "AbstractStatement::resolveStatementId(%s), pgm:%p, st:%p, n:%d\n", statementId, p, s, n);
      if (p != this)
         return 0;
      return s;
   }

   DLLLOCAL void addStatementToIndexIntern(name_sline_statement_map_t* statementIndex, const char* key, AbstractStatement *statement, int offs) {
      // index is being built when parsing
      if (!statement)
         return;
      sline_statement_multimap_t *ssm;
      std::map<const char*, sline_statement_multimap_t*>::iterator it = statementIndex->find(key);
      if (it == statementIndex->end()) {
         printd(5, "qore_program_private::addStatementToIndexIntern('%s',%d) key not found, this: %p\n", key, offs, this);
         ssm = new sline_statement_multimap_t();
         statementIndex->insert(std::pair<const char*, sline_statement_multimap_t*>(key, ssm));
      } else {
         ssm = it->second;
         std::map<int, AbstractStatement*>::iterator li = ssm->find(statement->loc.start_line+offs);
         while (li != ssm->end() && li->first == statement->loc.start_line+offs) {
            if (li->second->loc.end_line == statement->loc.end_line) {
               // order of multimap values is not defined, so unless we want create extra index by statement position at line then we need insert only the first statement
               printd(5, "qore_program_private::addStatementToIndexIntern('%s',%d) skipping line (%d-%d), this: %p\n", key, offs, statement->loc.start_line, statement->loc.end_line, this);
               return;
            }
            ++li;
         }
      }
      printd(5, "qore_program_private::addStatementToIndexIntern('%s',%d) insert line %d (%d-%d), this: %p\n", key, offs, statement->loc.start_line+offs, statement->loc.start_line, statement->loc.end_line, this);
      ssm->insert(std::pair<int, AbstractStatement*>(statement->loc.start_line+offs, statement));
   }

   DLLLOCAL static void addStatementToIndex(QoreProgram* pgm, AbstractStatement *statement) {
      // index is being built when parsing
      if (!statement)
         return;
      if (statement->loc.source) {
         printd(5, "qore_program_private::addStatementToIndex(file+source), this: %p, statement: %p\n", pgm->priv, statement);
         pgm->priv->addStatementToIndexIntern(&pgm->priv->statementByFileIndex, statement->loc.source, statement, statement->loc.offset);
         pgm->priv->addStatementToIndexIntern(&pgm->priv->statementByLabelIndex, statement->loc.file, statement, 0);
      } else {
         printd(5, "qore_program_private::addStatementToIndex(file), this: %p, statement: %p\n", pgm->priv, statement);
         pgm->priv->addStatementToIndexIntern(&pgm->priv->statementByFileIndex, statement->loc.file, statement, statement->loc.offset/*is zero*/);
      }
   }

   DLLLOCAL AbstractStatement* getStatementFromIndex(const char* name, int line) {
      printd(5, "qore_program_private::getStatementFromIndex('%s',%d), this: %p, file#: %d, label#: %d\n", name, line, this, statementByFileIndex.size(), statementByLabelIndex.size());
      AutoLocker al(&plock);
      std::map<const char*, sline_statement_multimap_t*>::iterator it;
      if (statementByFileIndex.empty()) {
         return 0;
      }
      bool addOffs = true;
      if (!name || *name == '\0') {
         if (statementByFileIndex.size() != 1)
            return 0;
         it = statementByFileIndex.begin();
      } else {
         size_t l = strlen(name);
         it = statementByFileIndex.find(name);
         if (it == statementByFileIndex.end()) {
            it = statementByLabelIndex.find(name);  // label only full name match
            if (it == statementByLabelIndex.end()) {

               // did not find exact match so try a heuristic
               it = statementByFileIndex.begin();
               while (it != statementByFileIndex.end()) {
                  size_t k = strlen(it->first);
                  if (k >= l) {
                     if (strcmp(name, it->first + k - l) == 0) {
                        // match against suffix following '/'
                        if (k == l /* should not happen as it is full match*/ || name[0] == '/' || it->first[k-l-1] == '/') {
                           break;
                        }
                     }
                  }
                  ++it;
               }
               if (it == statementByFileIndex.end()) {
                  printd(5, "qore_program_private::getStatementFromIndex('%s',%d) no suffix match, this: %p\n", name, line, this);
                  return 0;
               }
               printd(5, "qore_program_private::getStatementFromIndex('%s',%d) found by file suffix match, this: %p\n", name, line, this);
            } else {
               addOffs = false;
               printd(5, "qore_program_private::getStatementFromIndex('%s',%d) found by label full match, this: %p\n", name, line, this);
            }
         } else {
            printd(5, "qore_program_private::getStatementFromIndex('%s',%d) found by file full match, this: %p\n", name, line, this);
         }
      }
      sline_statement_multimap_t *ssm = it->second;
      printd(5, "qore_program_private::getStatementFromIndex('%s',%d) found '%s', this: %p, ssm#: %d\n", name, line, it->first, this, ssm->size());
      if (ssm->size() == 0)
         return 0;

      std::map<int, AbstractStatement*>::iterator li = ssm->upper_bound(line);
      if (li == ssm->begin()) {
         printd(5, "qore_program_private::getStatementFromIndex('%s',%d) no statement found by line #1, this: %p\n", name, line, this);
         return 0;
      }
      --li;
      int ln = li->first;
      int minnl = -1;
      AbstractStatement* st = 0;
      while (true) {
         // find the nearest statement, i.e. statement with smallest num of lines
         if (ln != li->first) {
            break;
            // we do not try to find outer statement when looking for line behind inner statement
         }
         if (li->second->loc.start_line + (addOffs ? li->second->loc.offset : 0) <= line && li->second->loc.end_line + (addOffs ? li->second->loc.offset : 0) >= line) {
            int n = li->second->loc.end_line - li->second->loc.start_line;
            if (minnl >= 0) {
               if (n < minnl) {
                  minnl = n;
                  st = li->second;
               }
            } else {
               minnl = n;
               st = li->second;
            }
            if (minnl == 0)
               break;
         }
         if (li == ssm->begin())
            break;
         --li;
      }
      if (st) {
         printd(5, "qore_program_private::getStatementFromIndex('%s',%d) statement:('file':%s,source:%s,offset:%d,line:%d-%d), this: %p\n", name, line, st->loc.file, st->loc.source, st->loc.offset, st->loc.start_line, st->loc.end_line, this);
      } else {
         printd(5, "qore_program_private::getStatementFromIndex('%s',%d) no statement found by line #2, this: %p\n", name, line, this);
      }
      return st;
   }
};

class ParseWarnHelper : public ParseWarnOptions {
protected:
   bool restore;
public:
   DLLLOCAL ParseWarnHelper(const ParseWarnOptions &new_opts) {
      QoreProgram* pgm = getProgram();
      restore = pgm ? qore_program_private::setSaveParseWarnOptions(pgm, new_opts, *this) : false;
   }
   DLLLOCAL ~ParseWarnHelper() {
      if (restore)
         qore_program_private::setParseWarnOptions(getProgram(), *this);
   }
};

typedef std::map<QoreProgram*, qore_program_private*> qore_program_map_t;
struct QoreDebugProgram;

class qore_debug_program_private {
private:
   mutable QoreThreadLock tlock;  // thread variable data lock, for accessing the program list variable
   QoreDebugProgram *dpgm;
   qore_program_map_t qore_program_map;
   mutable QoreCounter debug_program_counter;  // number of thread calls from program instance.
public:
   DLLLOCAL qore_debug_program_private(QoreDebugProgram *n_dpgm): dpgm(n_dpgm) {};
   DLLLOCAL ~qore_debug_program_private() {
      assert(qore_program_map.empty());
   }

   DLLLOCAL void addProgram(QoreProgram *pgm) {
      AutoLocker al(tlock);
      qore_program_map_t::iterator i = qore_program_map.find(pgm);
      printd(5, "qore_debug_program_private::addProgram(), this: %p, pgm: %p, i: %p, end: %p\n", this, pgm, i, qore_program_map.end());
      if (i != qore_program_map.end())
         return;  // already exists
      qore_program_map.insert(qore_program_map_t::value_type(pgm, pgm->priv));
      pgm->priv->attachDebug(this);
   }

   DLLLOCAL void removeProgram(QoreProgram *pgm) {
      AutoLocker al(tlock);
      qore_program_map_t::iterator i = qore_program_map.find(pgm);
      printd(5, "qore_debug_program_private::removeProgram(), this: %p, pgm: %p, i: %p, end: %p\n", this, pgm, i, qore_program_map.end());
      if (i == qore_program_map.end())
         return;
      pgm->priv->detachDebug(this);
      qore_program_map.erase(i);
      // onDetach will not be executed as program is removed
   }

   DLLLOCAL void removeAllPrograms() {
      AutoLocker al(tlock);
      printd(5, "qore_debug_program_private::removeAllPrograms(), this: %p\n", this);
      qore_program_map_t::iterator i;
      while ((i = qore_program_map.begin()) != qore_program_map.end()) {
         qore_program_private* qpp = i->second;
         qore_program_map.erase(i);
         qpp->detachDebug(this);
      }
   }

   DLLLOCAL void onAttach(QoreProgram *pgm, ThreadDebugEnum &sb, ExceptionSink* xsink) {
      AutoQoreCounterDec ad(&debug_program_counter);
      dpgm->onAttach(pgm, sb, xsink);
   }
   DLLLOCAL void onDetach(QoreProgram *pgm, ThreadDebugEnum &sb, ExceptionSink* xsink) {
      AutoQoreCounterDec ad(&debug_program_counter);
      dpgm->onDetach(pgm, sb, xsink);
   }
   /**
    * Executed on every step of StatementBlock.
    * @param blockStatement
    * @param statement current AbstractStatement of blockStatement being processed. Executed also when blockStatement is entered with value of NULL
    * @param retCode
    */
   DLLLOCAL void onStep(QoreProgram *pgm, const StatementBlock *blockStatement, const AbstractStatement *statement, int &retCode, ThreadDebugEnum &sb, ExceptionSink* xsink) {
      AutoQoreCounterDec ad(&debug_program_counter);
      dpgm->onStep(pgm, blockStatement, statement, retCode, sb, xsink);

   }
   DLLLOCAL void onFunctionEnter(QoreProgram *pgm, const StatementBlock *statement, ThreadDebugEnum &sb, ExceptionSink* xsink) {
      AutoQoreCounterDec ad(&debug_program_counter);
      dpgm->onFunctionEnter(pgm, statement, sb, xsink);
   }
   /**
    * Executed when a function is exited.
    */
   DLLLOCAL void onFunctionExit(QoreProgram *pgm, const StatementBlock *statement, QoreValue& returnValue, ThreadDebugEnum &sb, ExceptionSink* xsink) {
      AutoQoreCounterDec ad(&debug_program_counter);
      dpgm->onFunctionExit(pgm, statement, returnValue, sb, xsink);
   }
   /**
    * Executed when an exception is raised.
    */
   DLLLOCAL void onException(QoreProgram *pgm, const AbstractStatement *statement, ThreadDebugEnum &sb, ExceptionSink* xsink) {
      AutoQoreCounterDec ad(&debug_program_counter);
      dpgm->onException(pgm, statement, sb, xsink);
   }

   DLLLOCAL void breakProgramThread(QoreProgram *pgm, int tid) {
      AutoLocker al(tlock);
      qore_program_map_t::iterator i = qore_program_map.find(pgm);
      printd(5, "qore_debug_program_private::breakProgramThread(), this: %p, pgm: %p, i: %p, end: %p, tid: %d\n", this, pgm, i, qore_program_map.end(), tid);
      if (i == qore_program_map.end())
         return;
      i->second->breakProgramThread(tid);
   }

   DLLLOCAL void breakProgram(QoreProgram *pgm) {
      AutoLocker al(tlock);
      qore_program_map_t::iterator i = qore_program_map.find(pgm);
      printd(5, "qore_debug_program_private::breakProgram(), this: %p, pgm: %p, i: %p, end: %p\n", this, pgm, i, qore_program_map.end());
      if (i == qore_program_map.end())
         return;
      i->second->breakProgram();
   }

   DLLLOCAL void waitForTerminationAndClear(ExceptionSink *xsink) {
      removeAllPrograms();
      // wait till all debug calls finished, avoid deadlock as it might be handled in current thread
      debug_program_counter.waitForZero();
   }

};

#endif
