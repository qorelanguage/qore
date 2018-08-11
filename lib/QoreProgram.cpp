/*
  QoreProgram.cpp

  Program QoreObject Definition

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
#include <qore/Restrictions.h>
#include <qore/QoreCounter.h>
#include "qore/intern/QoreSignal.h"
#include "qore/intern/LocalVar.h"
#include "qore/intern/qore_program_private.h"
#include "qore/intern/QoreNamespaceIntern.h"
#include "qore/intern/ConstantList.h"
#include "qore/intern/QoreTypeInfo.h"
#include "qore/intern/QC_Breakpoint.h"

#include <string>
#include <set>
#include <typeinfo>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <map>

ParseOptionMaps pomaps;

// note the number and order of the warnings has to correspond to those in QoreProgram.h
static const char* qore_warnings_l[] = {
   "warning-mask-unchanged",
   "duplicate-local-vars",
   "unknown-warning",
   "undeclared-var",
   "duplicate-global-vars",
   "unreachable-code",
   "non-existent-method-call",
   "invalid-operation",
   "call-with-type-errors",
   "return-value-ignored",
   "deprecated",
   "excess-args",
   "duplicate-hash-key",
   "unreferenced-variable",
   "duplicate-block-vars",
   "module-only",
   "broken-logic-precedence",
};
#define NUM_WARNINGS (sizeof(qore_warnings_l)/sizeof(const char* ))

void ParseOptionMaps::doMap(int64 code, const char* desc) {
   assert(pomap.find(code) == pomap.end());
   assert(pormap.find(desc) == pormap.end());
   pomap[code] = desc;
   pormap[desc] = code;
}

ParseOptionMaps::ParseOptionMaps() {
      doMap(PO_NO_GLOBAL_VARS, "PO_NO_GLOBAL_VARS");
      doMap(PO_NO_SUBROUTINE_DEFS, "PO_NO_SUBROUTINE_DEFS");
      doMap(PO_NO_THREAD_CONTROL, "PO_NO_THREAD_CONTROL");
      doMap(PO_NO_THREAD_CLASSES, "PO_NO_THREAD_CLASSES");
      doMap(PO_NO_TOP_LEVEL_STATEMENTS, "PO_NO_TOP_LEVEL_STATEMENTS");
      doMap(PO_NO_CLASS_DEFS, "PO_NO_CLASS_DEFS");
      doMap(PO_NO_NAMESPACE_DEFS, "PO_NO_NAMESPACE_DEFS");
      doMap(PO_NO_CONSTANT_DEFS, "PO_NO_CONSTANT_DEFS");
      doMap(PO_NO_NEW, "PO_NO_NEW");
      doMap(PO_NO_INHERIT_SYSTEM_CLASSES, "PO_NO_INHERIT_SYSTEM_CLASSES");
      doMap(PO_NO_INHERIT_USER_CLASSES, "PO_NO_INHERIT_USER_CLASSES");
      doMap(PO_NO_CHILD_PO_RESTRICTIONS, "PO_NO_CHILD_PO_RESTRICTIONS");
      doMap(PO_NO_EXTERNAL_PROCESS, "PO_NO_EXTERNAL_PROCESS");
      doMap(PO_REQUIRE_OUR, "PO_REQUIRE_OUR");
      doMap(PO_NO_PROCESS_CONTROL, "PO_NO_PROCESS_CONTROL");
      doMap(PO_NO_NETWORK, "PO_NO_NETWORK");
      doMap(PO_NO_FILESYSTEM, "PO_NO_FILESYSTEM");
      doMap(PO_LOCK_WARNINGS, "PO_LOCK_WARNINGS");
      doMap(PO_NO_DATABASE, "PO_NO_DATABASE");
      doMap(PO_NO_GUI, "PO_NO_GUI");
      doMap(PO_NO_TERMINAL_IO, "PO_NO_TERMINAL_IO");
      doMap(PO_REQUIRE_TYPES, "PO_REQUIRE_TYPES");
      doMap(PO_NO_EXTERNAL_INFO, "PO_NO_EXTERNAL_INFO");
      doMap(PO_NO_THREAD_INFO, "PO_NO_THREAD_INFO");
      doMap(PO_NO_LOCALE_CONTROL, "PO_NO_LOCALE_CONTROL");
      doMap(PO_REQUIRE_PROTOTYPES, "PO_REQUIRE_PROTOTYPES");
      doMap(PO_STRICT_ARGS, "PO_STRICT_ARGS");
      //doMap(PO_REQUIRE_BARE_REFS, "PO_REQUIRE_BARE_REFS");
      doMap(PO_ASSUME_LOCAL, "PO_ASSUME_LOCAL");
      doMap(PO_NO_MODULES, "PO_NO_MODULES");
      doMap(PO_NO_INHERIT_USER_FUNC_VARIANTS, "PO_NO_INHERIT_USER_FUNC_VARIANTS");
      doMap(PO_NO_INHERIT_SYSTEM_FUNC_VARIANTS, "PO_NO_INHERIT_SYSTEM_FUNC_VARIANTS");
      doMap(PO_NO_INHERIT_GLOBAL_VARS, "PO_NO_INHERIT_GLOBAL_VARS");
      doMap(PO_IN_MODULE, "PO_IN_MODULE");
      doMap(PO_NO_EMBEDDED_LOGIC, "PO_NO_EMBEDDED_LOGIC");
      doMap(PO_STRICT_BOOLEAN_EVAL, "PO_STRICT_BOOLEAN_EVAL");
      doMap(PO_DEFAULT, "PO_DEFAULT");
      //doMap(PO_SYSTEM_OPS, "PO_SYSTEM_OPS");
      doMap(PO_ALLOW_BARE_REFS, "PO_ALLOW_BARE_REFS");
      doMap(PO_NO_THREADS, "PO_NO_THREADS");
      doMap(PO_NO_EXTERNAL_ACCESS, "PO_NO_EXTERNAL_ACCESS");
      doMap(PO_NO_IO, "PO_NO_IO");
      doMap(PO_LOCKDOWN, "PO_LOCKDOWN");
      doMap(PO_NEW_STYLE, "PO_NEW_STYLE");
      doMap(PO_ALLOW_INJECTION, "PO_ALLOW_INJECTION");
      doMap(PO_NO_INHERIT_SYSTEM_CONSTANTS, "PO_NO_INHERIT_SYSTEM_CONSTANTS");
      doMap(PO_NO_INHERIT_USER_CONSTANTS, "PO_NO_INHERIT_USER_CONSTANTS");
      doMap(PO_BROKEN_LIST_PARSING, "PO_BROKEN_LIST_PARSING");
      doMap(PO_BROKEN_LOGIC_PRECEDENCE, "PO_BROKEN_LOGIC_PRECEDENCE");
      doMap(PO_BROKEN_LOOP_STATEMENT, "PO_BROKEN_LOOP_STATEMENT");
      doMap(PO_BROKEN_REFERENCES, "PO_BROKEN_REFERENCES");
      doMap(PO_NO_DEBUGGING, "PO_NO_DEBUGGING");
      doMap(PO_ALLOW_DEBUGGER, "PO_ALLOW_DEBUGGER");
}

QoreHashNode* ParseOptionMaps::getCodeToStringMap() const {
   QoreHashNode* h = new QoreHashNode;

   QoreString key;
   for (pomap_t::const_iterator i = pomap.begin(), e = pomap.end(); i != e; ++i) {
      key.clear();
      key.sprintf(QLLD, i->first);
      h->setKeyValue(key.c_str(), new QoreStringNode(i->second), 0);
   }

   return h;
}

QoreHashNode* ParseOptionMaps::getStringToCodeMap() const {
   QoreHashNode* h = new QoreHashNode;

   for (pormap_t::const_iterator i = pormap.begin(), e = pormap.end(); i != e; ++i) {
      h->setKeyValue(i->first, new QoreBigIntNode(i->second), 0);
   }

   return h;
}

//public symbols
const char** qore_warnings = qore_warnings_l;
unsigned qore_num_warnings = NUM_WARNINGS;
qore_program_private::qore_program_to_object_map_t qore_program_private::qore_program_to_object_map;
QoreRWLock qore_program_private::lck_programMap;
volatile unsigned qore_program_private::programIdCounter = 1;


qore_program_private::qore_program_private(QoreProgram* n_pgm, int64 n_parse_options, QoreProgram* p_pgm) : qore_program_private_base(n_pgm, n_parse_options, p_pgm), dpgm(0) {
   QoreAutoRWWriteLocker al(&qore_program_private::lck_programMap);
   qore_program_to_object_map_t::iterator i = qore_program_private::qore_program_to_object_map.find(pgm);
   assert(i == qore_program_private::qore_program_to_object_map.end());
   if (i == qore_program_private::qore_program_to_object_map.end()) {
      programId = programIdCounter++;
      qore_program_private::qore_program_to_object_map.insert(qore_program_to_object_map_t::value_type(pgm, 0));
   }
   printd(5, "qore_program_private::qore_program_private() this: %p pgm: %p, pgmid: %d\n", this, pgm, programId);
}

qore_program_private::~qore_program_private() {
   printd(5, "qore_program_private::~qore_program_private() this: %p pgm: %p, pgmid: %d\n", this, pgm, programId);
   if (dpgm) {
      // if the object is being destroyed when thread is terminating via a deref then we send sync detach event to this thread
      // the thread can be stopped.
      ThreadLocalProgramData *tlpd = get_thread_local_program_data();
      if (tlpd) {
         tlpd->dbgDetach(nullptr);
      }
      dpgm->removeProgram(pgm);
   }
   // wait till all debug calls are finished, no new calls possible as dpgm->removeProgram() set dpmg to NULL
   debug_program_counter.waitForZero();
   deleteAllBreakpoints();
   QoreAutoRWWriteLocker al(&qore_program_private::lck_programMap);
   qore_program_to_object_map_t::iterator i = qore_program_to_object_map.find(pgm);
   if (i == qore_program_to_object_map.end()) {
      assert(false);
   } else {
      assert(i->second == 0);
      qore_program_to_object_map.erase(i);
      printd(5, "qore_program_private::~qore_program_private() this: %p pgm: %p, pgmid: %d removed\n", this, pgm, programId);
   }
   statementByFileIndex.clear();
   statementIds.clear();
   reverseStatementIds.clear();
   statementByLabelIndex.clear();

   assert(!parseSink);
   assert(!warnSink);
   assert(!pendingParseSink);
   assert(pgm_data_map.empty());
   assert(!exec_class_rv);
   assert(!dpgm);
}

void qore_program_private_base::setDefines() {
   for (ParseOptionMaps::pomap_t::iterator i = pomaps.pomap.begin(), e = pomaps.pomap.end(); i != e; ++i) {
      if ((pwo.parse_options & i->first) == i->first) {
         dmap[i->second] = &True;
      }
   }
}

void qore_program_private_base::startThread(ExceptionSink& xsink) {
   if (!thread_local_storage->get())
      thread_local_storage->set(new QoreHashNode);
}

void qore_program_private::doThreadInit(ExceptionSink* xsink) {
   // create/destroy temporary ExceptionSink object if necessary
   std::unique_ptr<ExceptionSink> xs;
   if (!xsink) {
      xs.reset(new ExceptionSink);
      xsink = xs.get();
   }

   // if there is any thread-initialization code, execute it here
   ReferenceHolder<ResolvedCallReferenceNode> ti(xsink);
   {
      AutoLocker al(tlock);
      ti = thr_init ? thr_init->refRefSelf() : 0;
   }
   if (ti) {
      ValueHolder v(ti->execValue(0, xsink), xsink);
   }
}

// returns true if there was already a thread init closure set, false if not
bool qore_program_private::setThreadInit(const ResolvedCallReferenceNode* n_thr_init, ExceptionSink* xsink) {
   // check
   ReferenceHolder<ResolvedCallReferenceNode> old(xsink);
   {
      AutoLocker al(tlock);
      old = thr_init;
      thr_init = n_thr_init ? n_thr_init->refRefSelf() : 0;

      //printd(5, "qore_program_private::setThreadInit() this: %p thr_init: %p %d '%s'\n", this, thr_init, get_node_type(thr_init), get_type_name(thr_init));
   }
   return (bool)old;
}

void qore_program_private_base::newProgram() {
   base_object = true;
   po_locked = false;
   exec_class = false;

   // init thread local storage key
   thread_local_storage = new qpgm_thread_local_storage_t;

   // save thread local storage hash
   assert(!thread_local_storage->get());
   thread_local_storage->set(new QoreHashNode);

   //printd(5, "qore_program_private_base::newProgram() this: %p\n", this);

   // copy global feature list to local list
   for (FeatureList::iterator i = qoreFeatureList.begin(), e = qoreFeatureList.end(); i != e; ++i)
      featureList.push_back((*i).c_str());

   QoreProgramContextHelper pch(pgm);

   // setup namespaces
   RootNS = qore_root_ns_private::copy(*staticSystemNamespace, pwo.parse_options);
   QoreNS = RootNS->rootGetQoreNamespace();
   assert(QoreNS);

   // setup initial defines
   // add platform defines
   dmap["QoreVersionString"] = new QoreStringNode(qore_version_string);
   dmap["QoreVersionMajor"] = new QoreBigIntNode(qore_version_major);
   dmap["QoreVersionMinor"] = new QoreBigIntNode(qore_version_minor);
   dmap["QoreVersionSub"] = new QoreBigIntNode(qore_version_sub);
   dmap["QoreVersionBuild"] = new QoreBigIntNode(qore_build_number);
   dmap["QoreVersionBits"] = new QoreBigIntNode(qore_target_bits);
   dmap["QorePlatformCPU"] = new QoreStringNode(TARGET_ARCH);
   dmap["QorePlatformOS"] = new QoreStringNode(TARGET_OS);

#ifdef _Q_WINDOWS
   dmap["Windows"] = &True;
#else
   dmap["Unix"] = &True;
#endif

   if (pwo.parse_options & PO_IN_MODULE)
      dmap["QoreHasUserModuleLicense"] = &True;

   QoreNamespace* ns = QoreNS->findLocalNamespace("Option");
   assert(ns);
   ConstantListIterator cli(qore_ns_private::getConstantList(ns));
   while (cli.next()) {
      AbstractQoreNode* v = cli.getValue();
      assert(v);
      // skip boolean options defined as False
      if (v->getType() == NT_BOOLEAN && !reinterpret_cast<QoreBoolNode*>(v)->getValue())
         continue;

      dmap[cli.getName()] = v->refSelf();
   }

#ifdef DEBUG
   // if Qore library debugging is enabled, then set an option
   dmap["QoreDebug"] = &True;
#endif
}

void qore_program_private_base::setParent(QoreProgram* p_pgm, int64 n_parse_options) {
   printd(5, "qore_program_private_base::setParent() this: %p parent: %p (parent lvl: %p) this: %p (this pgm: %p) parent po: %lld new po: %lld parent no_child_po_restrictions: %d\n", this, p_pgm, p_pgm->priv->sb.getLVList(), this, pgm, p_pgm->priv->pwo.parse_options, n_parse_options, p_pgm->priv->pwo.parse_options & PO_NO_CHILD_PO_RESTRICTIONS);

   TZ = p_pgm->currentTZ();

   // if children inherit restrictions, then set all child restrictions
   if (!(p_pgm->priv->pwo.parse_options & PO_NO_CHILD_PO_RESTRICTIONS)) {
      // lock child parse options
      po_locked = true;
      // turn on all restrictions in the child that are set in the parent
      pwo.parse_options |= p_pgm->priv->pwo.parse_options;
      // make sure all options that give more freedom and are off in the parent program are turned off in the child
      pwo.parse_options &= (p_pgm->priv->pwo.parse_options | ~PO_POSITIVE_OPTIONS);
   }
   else {
      pwo.parse_options = n_parse_options;
      po_locked = !(n_parse_options & PO_NO_CHILD_PO_RESTRICTIONS);
   }

   // inherit parent's thread local storage key
   thread_local_storage = p_pgm->priv->thread_local_storage;

   {
      // make sure no parsing is running in the parent while we copy namespaces
      ProgramRuntimeParseAccessHelper rah(0, p_pgm);
      // setup derived namespaces
      RootNS = qore_root_ns_private::copy(*p_pgm->priv->RootNS, n_parse_options);
   }
   QoreNS = RootNS->rootGetQoreNamespace();

   // copy parent feature list
   p_pgm->priv->featureList.populate(&featureList);

   // copy top-level local variables in case any are referenced in static methods in the parent program (static methods are executed in the child's space)
   const LVList* lvl = p_pgm->priv->sb.getLVList();
   if (lvl)
      sb.assignLocalVars(lvl);

   // copy program defines to child program
   for (dmap_t::const_iterator i = p_pgm->priv->dmap.begin(), e = p_pgm->priv->dmap.end(); i != e; ++i)
      dmap[i->first] = i->second ? i->second->refSelf() : 0;

   // copy external data if present
   {
      AutoLocker al(p_pgm->priv->plock);
      for (auto& i : p_pgm->priv->extmap) {
         extmap.insert(extmap_t::value_type(i.first, i.second->copy(pgm)));
      }
   }
}

void qore_program_private::internParseRollback() {
   // delete pending changes to namespaces
   qore_root_ns_private::parseRollback(*RootNS);

   // delete pending statements
   sb.parseRollback();

   // roll back pending domain
   pend_dom = 0;
}

void qore_program_private::waitForTerminationAndClear(ExceptionSink* xsink) {
   // detach itself from debug
   if (dpgm) {
      // if the object is being destroyed when thread is terminating via a deref then we send sync detach event to this thread
      // the thread can be stopped.
      ThreadLocalProgramData *tlpd = get_thread_local_program_data();
      if (tlpd) {
         tlpd->dbgDetach(xsink);
      }
      dpgm->removeProgram(pgm);
   }
   debug_program_counter.waitForZero(xsink, 0);  // it is probably obsolete as the next waiting for thread termination will wait for the same threads as well
   // we only clear the internal data structures once
   bool clr = false;
   {
      ReferenceHolder<QoreListNode> l(xsink);
      {
         AutoLocker al(plock);
         // wait for all threads to terminate
         waitForAllThreadsToTerminateIntern();
         if (!ptid) {
            l = new QoreListNode;
            qore_root_ns_private::clearConstants(*RootNS, **l);
            // mark the program so that only code from this thread can run during data destruction
            ptid = gettid();
            clr = true;
         }
      }
   }

   if (clr) {
      // purge thread resources before clearing pgm
      purge_pgm_thread_resources(pgm, xsink);

      printd(5, "qore_program_private::waitForTerminationAndClear() this: %p pgm: %p clr: %d\n", this, pgm, clr);
      // delete all global variables, etc
      qore_root_ns_private::clearData(*RootNS, xsink);

      // clear thread init code reference if any
      {
         ReferenceHolder<ResolvedCallReferenceNode> old(xsink);

         {
            AutoLocker al(tlock);

            // clear thread init code reference
            old = thr_init;
            thr_init = 0;
         }
      }

      // clear thread data if base object
      if (base_object)
         clearThreadData(xsink);

      clearProgramThreadData(xsink);

      {
         AutoLocker al(plock);
         ptid = -1;
      }

      // now clear the original map
      {
         AutoLocker al(tlock);
         pgm_data_map.clear();
         tclear = 0;

         if (twaiting)
            tcond.broadcast();
      }
#ifdef HAVE_SIGNAL_HANDLING
      {
         int_set_t ns = sigset;
         // clear all signal handlers managed by this program
         for (int_set_t::iterator i = ns.begin(), e = ns.end(); i != e; ++i)
            QSM.removeHandler(*i, xsink);
      }
#endif

      // merge pending parse exceptions into the passed exception sink, if any
      if (pendingParseSink) {
         xsink->assimilate(pendingParseSink);
         pendingParseSink = 0;
      }

      // clear any exec-class return value
      discard(exec_class_rv, xsink);
      exec_class_rv = 0;

      // delete code
      // method call can be repeated
      sb.del();
      //printd(5, "QoreProgram::~QoreProgram() this: %p deleting root ns %p\n", this, RootNS);

      del(xsink);

      // clear program location
      update_runtime_location(QoreProgramLocation());
   }
}

// called when the program's ref count = 0 (but the dc count may not go to 0 yet)
void qore_program_private::clear(ExceptionSink* xsink) {
   waitForTerminationAndClear(xsink);
   depDeref();
}

struct SaveParseLocationHelper : QoreProgramLocation {
   DLLLOCAL SaveParseLocationHelper() : QoreProgramLocation(get_parse_location()) {
   }

   DLLLOCAL ~SaveParseLocationHelper() {
      update_parse_location(*this);
   }
};

int qore_program_private::internParseCommit() {
   QORE_TRACE("qore_program_private::internParseCommit()");
   printd(5, "qore_program_private::internParseCommit() pgm: %p isEvent: %d\n", pgm, parseSink->isEvent());

   // save and restore parse location on exit
   // FIXME: remove this when all parseInit code sets the location manually (remove calls to update_parse_location in parseInit code)
   SaveParseLocationHelper plh;

   // if the first stage of parsing has already failed,
   // then don't go forward
   if (!parseSink->isEvent()) {
      // initialize new statements second (for "our" and "my" declarations)
      // also initializes namespaces, constants, etc
      sb.parseInit(pwo.parse_options);

      printd(5, "QoreProgram::internParseCommit() this: %p RootNS: %p\n", pgm, RootNS);
   }

   // if a parse exception has occurred, then back out all new
   // changes to the QoreProgram atomically
   int rc;
   if (parseSink->isEvent()) {
      internParseRollback();
      requires_exception = false;
      rc = -1;
   }
   else { // otherwise commit them
      // merge pending namespace additions
      qore_root_ns_private::parseCommit(*RootNS);

      // commit pending statements
      sb.parseCommit(pgm);

      // commit pending domain
      dom |= pend_dom;
      pend_dom = 0;

      rc = 0;
   }
   return rc;
}

void qore_program_private::runtimeImportSystemClassesIntern(const qore_program_private& spgm, ExceptionSink* xsink) {
   assert(&spgm != pgm->priv);
   if (!(pwo.parse_options & PO_NO_INHERIT_SYSTEM_CLASSES)) {
      xsink->raiseException("IMPORT-SYSTEM-CLASSES-ERROR", "cannot import system classes in a Program container where system classes have already been imported");
      return;
   }
   pwo.parse_options &= ~PO_NO_INHERIT_SYSTEM_CLASSES;
   qore_root_ns_private::runtimeImportSystemClasses(*RootNS, *spgm.RootNS, xsink);
}

void qore_program_private::runtimeImportSystemHashDeclsIntern(const qore_program_private& spgm, ExceptionSink* xsink) {
   assert(&spgm != pgm->priv);
   if (!(pwo.parse_options & PO_NO_INHERIT_SYSTEM_HASHDECLS)) {
      xsink->raiseException("IMPORT-SYSTEM-CLASSES-ERROR", "cannot import system classes in a Program container where system classes have already been imported");
      return;
   }
   pwo.parse_options &= ~PO_NO_INHERIT_SYSTEM_HASHDECLS;
   qore_root_ns_private::runtimeImportSystemHashDecls(*RootNS, *spgm.RootNS, xsink);
}

void qore_program_private::runtimeImportSystemConstantsIntern(const qore_program_private& spgm, ExceptionSink* xsink) {
   assert(&spgm != pgm->priv);
   if (!(pwo.parse_options & PO_NO_INHERIT_SYSTEM_CONSTANTS)) {
      xsink->raiseException("IMPORT-SYSTEM-CONSTANTS-ERROR", "cannot import system constants in a Program container where system constants have already been imported");
      return;
   }
   pwo.parse_options &= ~PO_NO_INHERIT_SYSTEM_CONSTANTS;
   qore_root_ns_private::runtimeImportSystemConstants(*RootNS, *spgm.RootNS, xsink);
}

void qore_program_private::runtimeImportSystemFunctionsIntern(const qore_program_private& spgm, ExceptionSink* xsink) {
   assert(&spgm != pgm->priv);
   if (!(pwo.parse_options & PO_NO_INHERIT_SYSTEM_FUNC_VARIANTS)) {
      xsink->raiseException("IMPORT-SYSTEM-API-ERROR", "cannot import system functions in a Program container where system functions have already been imported");
      return;
   }
   if (po_locked)
      xsink->raiseException("IMPORT-SYSTEM-API-ERROR", "parse options have been locked on this program object");

   pwo.parse_options &= ~PO_NO_INHERIT_SYSTEM_FUNC_VARIANTS;
   qore_root_ns_private::runtimeImportSystemFunctions(*RootNS, *spgm.RootNS, xsink);
}

void qore_program_private::runtimeImportSystemClasses(ExceptionSink* xsink) {
   // must acquire current program before setting program context below
   const QoreProgram* spgm = getProgram();
   // acquire safe access to parse structures in the source program
   ProgramRuntimeParseAccessHelper rah(xsink, pgm);

   runtimeImportSystemClassesIntern(*spgm->priv, xsink);
}

void qore_program_private::runtimeImportSystemHashDecls(ExceptionSink* xsink) {
   // must acquire current program before setting program context below
   const QoreProgram* spgm = getProgram();
   // acquire safe access to parse structures in the source program
   ProgramRuntimeParseAccessHelper rah(xsink, pgm);

   runtimeImportSystemHashDeclsIntern(*spgm->priv, xsink);
}

void qore_program_private::runtimeImportSystemConstants(ExceptionSink* xsink) {
   // must acquire current program before setting program context below
   const QoreProgram* spgm = getProgram();
   // acquire safe access to parse structures in the source program
   ProgramRuntimeParseAccessHelper rah(xsink, pgm);

   runtimeImportSystemConstantsIntern(*spgm->priv, xsink);
}

void qore_program_private::runtimeImportSystemFunctions(ExceptionSink* xsink) {
   // must acquire current program before setting program context below
   const QoreProgram* spgm = getProgram();
   // acquire safe access to parse structures in the source program
   ProgramRuntimeParseAccessHelper rah(xsink, pgm);
   runtimeImportSystemFunctionsIntern(*spgm->priv, xsink);
}

void qore_program_private::runtimeImportSystemApi(ExceptionSink* xsink) {
   // must acquire current program before setting program context below
   const QoreProgram* spgm = getProgram();
   // acquire safe access to parse structures in the source program
   ProgramRuntimeParseAccessHelper rah(xsink, pgm);
   runtimeImportSystemClassesIntern(*spgm->priv, xsink);
   if (*xsink)
      return;
   runtimeImportSystemFunctionsIntern(*spgm->priv, xsink);
   if (*xsink)
      return;
   runtimeImportSystemConstantsIntern(*spgm->priv, xsink);
   if (*xsink)
      return;
   runtimeImportSystemHashDeclsIntern(*spgm->priv, xsink);
}

void qore_program_private::importClass(ExceptionSink* xsink, qore_program_private& from_pgm, const char* path, const char* new_name, bool inject) {
   if (&from_pgm == this) {
      xsink->raiseException("CLASS-IMPORT-ERROR", "cannot import class \"%s\" with the same source and target Program objects", path);
      return;
   }

   if (inject && !(pwo.parse_options & PO_ALLOW_INJECTION)) {
      xsink->raiseException("CLASS-IMPORT-ERROR", "cannot import class \"%s\" with the injection flag set in a Program object without PO_ALLOW_INJECTION set", path);
      return;
   }

   const qore_class_private* injectedClass = nullptr;
   const qore_ns_private* vns = nullptr;
   const QoreClass* c;
   {
      // acquire safe access to parse structures in the source program
      ProgramRuntimeParseAccessHelper rah(xsink, from_pgm.pgm);
      if (*xsink)
         return;
      c = qore_root_ns_private::runtimeFindClass(*from_pgm.RootNS, path, vns);

      // must mark injected class so it can claim type compatibility with the class it's substituting
      if (inject) {
         const qore_ns_private* tcns = nullptr;
         const QoreClass* oc = qore_root_ns_private::runtimeFindClass(*from_pgm.RootNS, new_name ? new_name : path, tcns);
         if (oc) {
            // get injected target class pointer for new injected class
            injectedClass = qore_class_private::get(*oc);
            // mark source class as compatible with the injected target class as well
            const_cast<qore_class_private*>(qore_class_private::get(*c))->injectedClass = injectedClass;
         }
         //printd(5, "qore_program_private::importClass() this: %p path: '%s' new_name: '%s' oc: %p\n", this, path, new_name ? new_name : "n/a", oc);
      }
   }

   if (!c) {
       xsink->raiseException("CLASS-IMPORT-ERROR", "can't find class \"%s\" in source Program", path);
       return;
   }

   // get exclusive access to program object for parsing
   ProgramRuntimeParseContextHelper pch(xsink, pgm);
   if (*xsink)
      return;

   // find/create target namespace based on source namespace
   QoreNamespace* tns;
   if (new_name && strstr(new_name, "::")) {
      NamedScope nscope(new_name);

      tns = qore_root_ns_private::runtimeFindCreateNamespacePath(*RootNS, nscope, qore_class_private::isPublic(*c));
      qore_root_ns_private::runtimeImportClass(*RootNS, xsink, *tns, c, from_pgm.pgm, nscope.getIdentifier(), inject, injectedClass);
   }
   else {
      tns = vns->root ? RootNS : qore_root_ns_private::runtimeFindCreateNamespacePath(*RootNS, *vns);
      //printd(5, "qore_program_private::importClass() this: %p path: %s nspath: %s tns: %p %s RootNS: %p %s\n", this, path, nspath.c_str(), tns, tns->getName(), RootNS, RootNS->getName());
      qore_root_ns_private::runtimeImportClass(*RootNS, xsink, *tns, c, from_pgm.pgm, new_name, inject, injectedClass);
   }
}

void qore_program_private::importHashDecl(ExceptionSink* xsink, qore_program_private& from_pgm, const char* path, const char* new_name) {
      if (&from_pgm == this) {
         xsink->raiseException("HASHDECL-IMPORT-ERROR", "cannot import hashdecl \"%s\" with the same source and target Program objects", path);
         return;
      }

      const qore_ns_private* vns = nullptr;
      const TypedHashDecl* hd;
      {
         // acquire safe access to parse structures in the source program
         ProgramRuntimeParseAccessHelper rah(xsink, from_pgm.pgm);
         if (*xsink)
            return;
         hd = qore_root_ns_private::runtimeFindHashDecl(*from_pgm.RootNS, path, vns);
      }

      if (!hd) {
          xsink->raiseException("HASHDECL-IMPORT-ERROR", "can't find hashdecl \"%s\" in source Program", path);
          return;
      }

      // get exclusive access to program object for parsing
      ProgramRuntimeParseContextHelper pch(xsink, pgm);
      if (*xsink)
         return;

      // find/create target namespace based on source namespace
      QoreNamespace* tns;
      if (new_name && strstr(new_name, "::")) {
         NamedScope nscope(new_name);

         tns = qore_root_ns_private::runtimeFindCreateNamespacePath(*RootNS, nscope, typed_hash_decl_private::get(*hd)->isPublic());
         qore_root_ns_private::runtimeImportHashDecl(*RootNS, xsink, *tns, hd, from_pgm.pgm, nscope.getIdentifier());
      }
      else {
         tns = vns->root ? RootNS : qore_root_ns_private::runtimeFindCreateNamespacePath(*RootNS, *vns);
         //printd(5, "qore_program_private::importHashDecl() this: %p path: %s nspath: %s tns: %p %s RootNS: %p %s\n", this, path, nspath.c_str(), tns, tns->getName(), RootNS, RootNS->getName());
         qore_root_ns_private::runtimeImportHashDecl(*RootNS, xsink, *tns, hd, from_pgm.pgm, new_name);
      }
   }

   void qore_program_private::importFunction(ExceptionSink* xsink, QoreFunction* u, const qore_ns_private& oldns, const char* new_name, bool inject) {
   // get exclusive access to program object for parsing
   ProgramRuntimeParseContextHelper pch(xsink, pgm);
   if (*xsink)
      return;

   if (new_name && strstr(new_name, "::")) {
      NamedScope nscope(new_name);
      QoreNamespace* tns = qore_root_ns_private::runtimeFindCreateNamespacePath(*RootNS, nscope, u->hasPublic());
      qore_root_ns_private::runtimeImportFunction(*RootNS, xsink, *tns, u, nscope.getIdentifier());
      return;
   }

   // find/create target namespace based on source namespace
   QoreNamespace* tns = oldns.root ? RootNS : qore_root_ns_private::runtimeFindCreateNamespacePath(*RootNS, oldns);
   //printd(5, "qore_program_private::importFunction() this: %p tns: %p '%s' oldns: '%s' RootNS: %p %s\n", this, tns, tns->getName(), oldns.name.c_str(), RootNS, RootNS->getName());
   assert(oldns.name == tns->getName());
   qore_root_ns_private::runtimeImportFunction(*RootNS, xsink, *tns, u, new_name, inject);
}

void qore_program_private::exportGlobalVariable(const char* vname, bool readonly, qore_program_private& tpgm, ExceptionSink* xsink) {
   if (&tpgm == this) {
      xsink->raiseException("PROGRAM-IMPORTGLOBALVARIABLE-EXCEPTION", "cannot import global variable \"%s\" with the same source and target Program objects", vname);
      return;
   }

   const qore_ns_private* vns = 0;
   Var* v;
   {
      ProgramRuntimeParseAccessHelper pah(xsink, pgm);
      if (*xsink)
         return;
      v = qore_root_ns_private::runtimeFindGlobalVar(*RootNS, vname, vns);
   }

   if (!v) {
      xsink->raiseException("PROGRAM-IMPORTGLOBALVARIABLE-EXCEPTION", "there is no global variable \"%s\"", vname);
      return;
   }

   // get exclusive access to program object for parsing
   ProgramRuntimeParseContextHelper pch(xsink, tpgm.pgm);
   if (*xsink)
      return;

   // find/create target namespace based on source namespace
   QoreNamespace* tns = vns->root ? tpgm.RootNS : qore_root_ns_private::runtimeFindCreateNamespacePath(*tpgm.RootNS, *vns);
   //printd(5, "qore_program_private::exportGlobalVariable() this: %p vname: '%s' ro: %d vns: %p '%s::' RootNS: %p '%s::'\n", this, vname, readonly, vns, vns->name.c_str(), RootNS, RootNS->getName());
   qore_root_ns_private::runtimeImportGlobalVariable(*tpgm.RootNS, *tns, v, readonly, xsink);
}

void qore_program_private::del(ExceptionSink* xsink) {
   printd(5, "qore_program_private::del() pgm: %p (base_object: %d)\n", pgm, base_object);

   // dereference all external data
   for (auto& i : extmap) {
      i.second->doDeref();
   }
   extmap.clear();

   if (thr_init)
      thr_init->deref(xsink);

   if (base_object) {
      deleteThreadData(xsink);

      // delete thread local storage key
      delete thread_local_storage;
      base_object = false;
   }

   // delete defines
   for (dmap_t::iterator i = dmap.begin(), e = dmap.end(); i != e; ++i)
      discard(i->second, xsink);
   dmap.clear();

   assert(RootNS);
   // have to delete global variables first because of destructors.
   // method call can be repeated
   qore_root_ns_private::clearData(*RootNS, xsink);

   // delete all global variables, class static vars and constants
   RootNS->deleteData(xsink);

   delete RootNS;
   RootNS = 0;

   // delete all root code
   // method call can be repeated
   sb.del();

   // delete stored type information
   for (auto& i : ch_map)
      delete i.second;
   for (auto& i : chon_map)
      delete i.second;
   for (auto& i : cl_map)
      delete i.second;
   for (auto& i : clon_map)
      delete i.second;
   for (auto& i : cr_map)
      delete i.second;
   for (auto& i : cron_map)
      delete i.second;
   for (auto& i : csl_map)
      delete i.second;
   for (auto& i : cslon_map)
      delete i.second;

   //printd(5, "QoreProgram::~QoreProgram() this: %p deleting root ns %p\n", this, RootNS);

   for (std::map<const char*, section_sline_statement_map_t*>::iterator it = statementByFileIndex.begin(); it != statementByFileIndex.end(); it++) {
      it->second->sectionMap.clear();
      it->second->statementMap.clear();
      delete it->second;
   }
   statementByFileIndex.clear();
   for (std::map<const char*, section_sline_statement_map_t*>::iterator it = statementByLabelIndex.begin(); it != statementByLabelIndex.end(); it++) {
      it->second->sectionMap.clear();
      it->second->statementMap.clear();
      delete it->second;
   }
   statementByLabelIndex.clear();
}

const QoreClass* qore_program_private::runtimeFindClass(const char* class_name, ExceptionSink* xsink) const {
   // acquire safe access to parse structures in the source program
   ProgramRuntimeParseAccessHelper rah(xsink, pgm);
   if (*xsink)
      return 0;

   return qore_root_ns_private::runtimeFindClass(*RootNS, class_name);
}

int qore_program_private::setGlobalVarValue(const char* name, QoreValue val, ExceptionSink* xsink) {
   ValueHolder holder(val, xsink);

   const qore_ns_private* vns = 0;
   Var* v;
   {
      ProgramRuntimeParseAccessHelper pah(xsink, pgm);
      if (*xsink)
         return -1;
      v = qore_root_ns_private::runtimeFindGlobalVar(*RootNS, name, vns);
   }

   if (!v) {
      xsink->raiseException("UNKNOWN-VARIABLE", "there is no global variable \"%s\"", name);
      return -1;
   }

   LValueHelper lvh(xsink);
   if (v->getLValue(lvh, false))
      return -1;

   return lvh.assign(holder.release(), "<API assignment>");
}

LocalVar* qore_program_private::createLocalVar(const char* name, const QoreTypeInfo* typeInfo) {
   LocalVar* lv = new LocalVar(name, typeInfo);
   local_var_list.push_back(lv);
   return lv;
}

const QoreTypeInfo* qore_program_private::getComplexHashType(const QoreTypeInfo* vti) {
    assert(vti && vti != anyTypeInfo);
    AutoLocker al(chl);

    tmap_t::iterator i = ch_map.lower_bound(vti);
    if (i != ch_map.end() && i->first == vti)
        return i->second;

    QoreComplexHashTypeInfo* ti = new QoreComplexHashTypeInfo(vti);
    ch_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_program_private::getComplexHashOrNothingType(const QoreTypeInfo* vti) {
    assert(vti && vti != anyTypeInfo);
    AutoLocker al(chonl);

    tmap_t::iterator i = chon_map.lower_bound(vti);
    if (i != chon_map.end() && i->first == vti)
        return i->second;

    QoreComplexHashOrNothingTypeInfo* ti = new QoreComplexHashOrNothingTypeInfo(vti);
    chon_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_program_private::getComplexListType(const QoreTypeInfo* vti) {
    assert(vti && vti != anyTypeInfo);
    AutoLocker al(cll);

    tmap_t::iterator i = cl_map.lower_bound(vti);
    if (i != cl_map.end() && i->first == vti)
        return i->second;

    QoreComplexListTypeInfo* ti = new QoreComplexListTypeInfo(vti);
    cl_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_program_private::getComplexListOrNothingType(const QoreTypeInfo* vti) {
    assert(vti && vti != anyTypeInfo);
    AutoLocker al(clonl);

    tmap_t::iterator i = clon_map.lower_bound(vti);
    if (i != clon_map.end() && i->first == vti)
        return i->second;

    QoreComplexListOrNothingTypeInfo* ti = new QoreComplexListOrNothingTypeInfo(vti);
    clon_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_program_private::getComplexSoftListType(const QoreTypeInfo* vti) {
    assert(vti && vti != anyTypeInfo);
    AutoLocker al(csll);

    tmap_t::iterator i = csl_map.lower_bound(vti);
    if (i != csl_map.end() && i->first == vti)
        return i->second;

    QoreComplexSoftListTypeInfo* ti = new QoreComplexSoftListTypeInfo(vti);
    csl_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_program_private::getComplexSoftListOrNothingType(const QoreTypeInfo* vti) {
    assert(vti && vti != anyTypeInfo);
    AutoLocker al(cslonl);

    tmap_t::iterator i = cslon_map.lower_bound(vti);
    if (i != cslon_map.end() && i->first == vti)
        return i->second;

    QoreComplexSoftListOrNothingTypeInfo* ti = new QoreComplexSoftListOrNothingTypeInfo(vti);
    cslon_map.insert(i, tmap_t::value_type(vti, ti));
    return ti;
}

const QoreTypeInfo* qore_program_private::getComplexReferenceType(const QoreTypeInfo* vti) {
   AutoLocker al(crl);

   tmap_t::iterator i = cr_map.lower_bound(vti);
   if (i != cr_map.end() && i->first == vti)
      return i->second;

   QoreComplexReferenceTypeInfo* ti = new QoreComplexReferenceTypeInfo(vti);
   cr_map.insert(i, tmap_t::value_type(vti, ti));
   return ti;
}

const QoreTypeInfo* qore_program_private::getComplexReferenceOrNothingType(const QoreTypeInfo* vti) {
   AutoLocker al(cronl);

   tmap_t::iterator i = cron_map.lower_bound(vti);
   if (i != cron_map.end() && i->first == vti)
      return i->second;

   QoreComplexReferenceOrNothingTypeInfo* ti = new QoreComplexReferenceOrNothingTypeInfo(vti);
   cron_map.insert(i, tmap_t::value_type(vti, ti));
   return ti;
}

void qore_program_private::addStatementToIndexIntern(name_section_sline_statement_map_t* statementIndex, const char* key, AbstractStatement *statement, int offs, const char* section, int sectionOffs) {
   // index is being built when parsing
   if ((!statement && !section) || !key)
      return;
   section_sline_statement_map_t *sssm;
   std::map<const char*, section_sline_statement_map_t*>::iterator it = statementIndex->find(key);
   if (it == statementIndex->end()) {
      printd(5, "qore_program_private::addStatementToIndexIntern(%p,'%s',%d,'%s',%d) key not found, this: %p\n", statementIndex, key, offs, section ? section : (const char*)"(null)",sectionOffs, this);
      sssm = new section_sline_statement_map_t();
      statementIndex->insert(std::pair<const char*, section_sline_statement_map_t*>(key, sssm));
   } else {
      sssm = it->second;
      if (statement) {
         std::map<int, AbstractStatement*>::iterator li = sssm->statementMap.find(statement->loc.start_line+offs);
         while (li != sssm->statementMap.end() && li->first == statement->loc.start_line+offs) {
            if (li->second->loc.end_line == statement->loc.end_line) {
                // order of multimap values is not defined, so unless we want create extra index by statement position at line then we need insert only the first statement
                printd(5, "qore_program_private::addStatementToIndexIntern(%p,'%s',%d) skipping line (%d-%d), this: %p\n", statementIndex, key, offs, statement->loc.start_line, statement->loc.end_line, this);
                return;
            }
            ++li;
         }
      }
   }
   if (statement) {
      printd(5, "qore_program_private::addStatementToIndexIntern(%p,'%s',%d) insert line %d (%d-%d), this: %p\n", statementIndex, key, offs, statement->loc.start_line+offs, statement->loc.start_line, statement->loc.end_line, this);
      sssm->statementMap.insert(std::pair<int, AbstractStatement*>(statement->loc.start_line+offs, statement));
   }
   if (section) {
      if (sssm->sectionMap.find(section) == sssm->sectionMap.end()) {
         printd(5, "qore_program_private::addStatementToIndexIntern(%p,'%s','%s',%d) insert section, this: %p\n", statementIndex, key, section, sectionOffs, this);
         sssm->sectionMap.insert(std::pair<const char*, int>(section, sectionOffs));
      }
   }
}

void qore_program_private::registerStatement(QoreProgram *pgm, AbstractStatement *statement, bool addToIndex) {
   if (pgm && statement) {
      // plock must already be held
      ReverseStatementIdMap_t::iterator i = pgm->priv->reverseStatementIds.find(statement);
      if (i == pgm->priv->reverseStatementIds.end()) {
         pgm->priv->statementIds.push_back(statement);
         pgm->priv->reverseStatementIds.insert(std::pair<AbstractStatement*, unsigned long>(statement, pgm->priv->statementIds.size()));
      }
      if (addToIndex) {
         if (statement->loc.source) {
            printd(5, "qore_program_private::registerStatement(file+source), this: %p, statement: %p\n", pgm->priv, statement);
            pgm->priv->addStatementToIndexIntern(&pgm->priv->statementByFileIndex, statement->loc.source, statement, statement->loc.offset, statement->loc.file, statement->loc.offset);
            pgm->priv->addStatementToIndexIntern(&pgm->priv->statementByLabelIndex, statement->loc.file, statement, 0, statement->loc.source, statement->loc.offset);
         } else {
            printd(5, "qore_program_private::registerStatement(file), this: %p, statement: %p\n", pgm->priv, statement);
            pgm->priv->addStatementToIndexIntern(&pgm->priv->statementByFileIndex, statement->loc.file, statement, statement->loc.offset/*is zero*/, nullptr, -1);
         }
      }
   }
}

QoreHashNode* qore_program_private::getSourceIndicesIntern(name_section_sline_statement_map_t* statementIndex, ExceptionSink* xsink) const {
   ReferenceHolder<QoreHashNode> rv(new QoreHashNode, xsink);
   for (std::map<const char*, section_sline_statement_map_t*>::iterator it = statementIndex->begin(), e = statementIndex->end(); it != e; it++) {
      QoreHashNode* h2 = new QoreHashNode();
      for (std::map<const char*, int>::iterator it2 = it->second->sectionMap.begin(), e2 = it->second->sectionMap.end(); it2 != e2; it2++) {
         h2->setKeyValue(it2->first, new QoreBigIntNode(it2->second), xsink);
         if (*xsink) {
            return nullptr;
         }
      }
      rv->setKeyValue(it->first, h2, xsink);
      if (*xsink) {
         return nullptr;
      }
   }
   return rv.release();
}

void qore_program_private::onAttach(DebugRunStateEnum &rs, const AbstractStatement* &rts, ExceptionSink* xsink) {
   AutoQoreCounterDec ad(&debug_program_counter, false);
   qore_debug_program_private* p = getDebugProgram(ad);
   printd(5, "qore_program_private::onAttach() this: %p, dp: %p\n", this, p);
   if (p) {
      p->onAttach(pgm, rs, rts, xsink);
   } else {
      rs = DBG_RS_DETACH;
   }
}
void qore_program_private::onDetach(DebugRunStateEnum &rs, const AbstractStatement *&rts, ExceptionSink* xsink) {
   AutoQoreCounterDec ad(&debug_program_counter, false);
   qore_debug_program_private* p = getDebugProgram(ad);
   printd(5, "qore_program_private::onDetach() this: %p, dp: %p\n", this, p);
   if (p) {
      p->onDetach(pgm, rs, rts, xsink);
   } else {
      rs = DBG_RS_DETACH;
   }
}

void qore_program_private::onStep(const StatementBlock *blockStatement, const AbstractStatement *statement, unsigned bkptId, int &flow, DebugRunStateEnum &rs, const AbstractStatement *&rts, ExceptionSink* xsink) {
   AutoQoreCounterDec ad(&debug_program_counter, false);
   qore_debug_program_private* p = getDebugProgram(ad);
   if (p) {
      p->onStep(pgm, blockStatement, statement, bkptId, flow, rs, rts, xsink);
   } else {
      rs = DBG_RS_DETACH;
   }
}
void qore_program_private::onFunctionEnter(const StatementBlock *statement, DebugRunStateEnum &rs, const AbstractStatement *&rts, ExceptionSink* xsink) {
   AutoQoreCounterDec ad(&debug_program_counter, false);
   qore_debug_program_private* p = getDebugProgram(ad);
   if (p) {
      p->onFunctionEnter(pgm, statement, rs, rts, xsink);
   } else {
      rs = DBG_RS_DETACH;
   }
}

void qore_program_private::onFunctionExit(const StatementBlock *statement, QoreValue& returnValue, DebugRunStateEnum &rs, const AbstractStatement *&rts, ExceptionSink* xsink) {
   AutoQoreCounterDec ad(&debug_program_counter, false);
   qore_debug_program_private* p = getDebugProgram(ad);
   if (p) {
      p->onFunctionExit(pgm, statement, returnValue, rs, rts, xsink);
   } else {
      rs = DBG_RS_DETACH;
   }
}

void qore_program_private::onException(const AbstractStatement *statement, DebugRunStateEnum &rs, const AbstractStatement *&rts, ExceptionSink* xsink) {
   AutoQoreCounterDec ad(&debug_program_counter, false);
   qore_debug_program_private* p = getDebugProgram(ad);
   if (p) {
      p->onException(pgm, statement, rs, rts, xsink);
   } else {
      rs = DBG_RS_DETACH;
   }
}

void qore_program_private::onExit(const StatementBlock *statement, QoreValue& returnValue, DebugRunStateEnum &rs, const AbstractStatement *&rts, ExceptionSink* xsink) {
   AutoQoreCounterDec ad(&debug_program_counter, false);
   qore_debug_program_private* p = getDebugProgram(ad);
   if (p) {
      p->onExit(pgm, statement, returnValue, rs, rts, xsink);
   } else {
      rs = DBG_RS_DETACH;
   }
}

const AbstractQoreFunctionVariant* qore_program_private::runtimeFindCall(const char* name, const QoreValueList* params, ExceptionSink* xsink) {
   // acquire safe access to parse structures in the source program
   ProgramRuntimeParseAccessHelper rah(xsink, pgm);

   return qore_root_ns_private::get(*RootNS)->runtimeFindCall(name, params, xsink);
}

QoreValueList* qore_program_private::runtimeFindCallVariants(const char* name, ExceptionSink* xsink) {
   // acquire safe access to parse structures in the source program
   ProgramRuntimeParseAccessHelper rah(xsink, pgm);

   return qore_root_ns_private::get(*RootNS)->runtimeFindCallVariants(name, xsink);
}

void ThreadLocalProgramData::dbgAttach(ExceptionSink* xsink) {
   if (runState == DBG_RS_DETACH) {
      DebugRunStateEnum rs = runState;
      const AbstractStatement* rts = runToStatement;
      functionCallLevel = 0;
      printd(5, "ThreadLocalProgramData::dbgAttach this: %p, rs: %d, rts: %p, tid: %d\n", this, runState, runToStatement, gettid());
      runState = DBG_RS_STOPPED;
      runToStatement = nullptr;
      getProgram()->priv->onAttach(rs, rts, xsink);
      setRunState(rs, rts);
      printd(5, "ThreadLocalProgramData::dbgAttach this: %p, rs: %d, rts: %p, xsink:%d\n", this, runState, runToStatement, xsink && xsink->isEvent());
   }
}

void ThreadLocalProgramData::dbgDetach(ExceptionSink* xsink) {
   if (runState != DBG_RS_STOPPED && runState != DBG_RS_DETACH) {
      printd(5, "ThreadLocalProgramData::dbgDetach this: %p, rs: %d, rts: %p, tid: %d\n", this, runState, runToStatement, gettid());
      DebugRunStateEnum rs = DBG_RS_DETACH;
      const AbstractStatement* rts = runToStatement;
      runState = DBG_RS_STOPPED;
      runToStatement = nullptr;
      getProgram()->priv->onDetach(rs, rts, xsink);
      setRunState(rs, rts);
      printd(5, "ThreadLocalProgramData::dbgDetach this: %p, rs: %d, rts: %p, xsink:%d\n", this, runState, runToStatement, xsink && xsink->isEvent());
   }
}

int ThreadLocalProgramData::dbgStep(const StatementBlock* blockStatement, const AbstractStatement* statement, ExceptionSink* xsink) {
   checkAttach(xsink);
   checkBreakFlag();
   int rc = 0;
   if (runState != DBG_RS_STOPPED && runState != DBG_RS_DETACH) {
      unsigned bkptId = 0;
      bool cond = runState == DBG_RS_STEP || (runState == DBG_RS_STEP_OVER && functionCallLevel == 0);
      if (!cond) {
          const AbstractStatement *st = statement ? statement : blockStatement;
          cond = st == runToStatement;
          if (!cond && st->getBreakpointFlag()) {   // fast breakpoint check
              printd(5, "ThreadLocalProgramData::dbgStep() this: %p, rs: %d, tid: %d, breakpoint phase-1\n", this, runState, gettid());
              bkptId = getProgram()->priv->onCheckBreakpoint(st, xsink);  // more precise check requiring lock
              cond = bkptId > 0;
          }
      }
      if (cond) {
          printd(5, "ThreadLocalProgramData::dbgStep() this: %p, bkptId: %d, rs: %d, rts: %p, tid: %d\n", this, bkptId, runState, runToStatement, gettid());
          functionCallLevel = 0;
          DebugRunStateEnum rs = runState;
          const AbstractStatement* rts = runToStatement;
          runState = DBG_RS_STOPPED;
          runToStatement = nullptr;
          getProgram()->priv->onStep(blockStatement, statement, bkptId, rc, rs, rts, xsink);
          setRunState(rs, rts);
          printd(5, "ThreadLocalProgramData::dbgStep() this: %p, rs: %d, rts: %p, rc: %d, xsink:%d\n", this, runState, runToStatement, rc, xsink && xsink->isEvent());
      }
   }
   return rc;
}

void ThreadLocalProgramData::dbgFunctionEnter(const StatementBlock* statement, ExceptionSink* xsink) {
   checkAttach(xsink);
   checkBreakFlag();
   if (runState == DBG_RS_STOPPED || runState == DBG_RS_DETACH) {

   } else if (runState == DBG_RS_STEP || statement == runToStatement) {
      printd(5, "ThreadLocalProgramData::dbgFunctionEnter() this: %p, rs: %d, rts: %p, tid: %d\n", this, runState, runToStatement, gettid());
      functionCallLevel = 0;
      DebugRunStateEnum rs = runState;
      const AbstractStatement* rts = runToStatement;
      runState = DBG_RS_STOPPED;
      runToStatement = nullptr;
      getProgram()->priv->onFunctionEnter(statement, rs, rts, xsink);
      setRunState(rs, rts);
      printd(5, "ThreadLocalProgramData::dbgFunctionEnter() this: %p, rs: %d, rts: %p, xsink: %d\n", this, runState, runToStatement, xsink && xsink->isEvent());
   } else if (runState == DBG_RS_STEP_OVER && functionCallLevel == 0) {
      functionCallLevel = 1;
      printd(5, "ThreadLocalProgramData::dbgFunctionEnter(), stepping over, this: %p, rs: %d, tid: %d\n", this, runState, gettid());
   } else if (functionCallLevel > 0) {
      functionCallLevel++;
   }
}

void ThreadLocalProgramData::dbgFunctionExit(const StatementBlock* statement, QoreValue& returnValue, ExceptionSink* xsink) {
   if (runState == DBG_RS_STOPPED || runState == DBG_RS_DETACH) {

   } else if ((runState == DBG_RS_UNTIL_RETURN && functionCallLevel == 1) || runState == DBG_RS_STEP || (runState == DBG_RS_STEP_OVER && functionCallLevel == 0) || statement == runToStatement) {
      printd(5, "ThreadLocalProgramData::dbgFunctionExit() this: %p, rs: %d, rts: %p, tid: %d\n", this, runState, runToStatement, gettid());
      DebugRunStateEnum rs = runState;
      const AbstractStatement* rts = runToStatement;
      functionCallLevel = 0;
      runState = DBG_RS_STOPPED;
      runToStatement = nullptr;
      getProgram()->priv->onFunctionExit(statement, returnValue, rs, rts, xsink);
      setRunState(rs, rts);
      printd(5, "ThreadLocalProgramData::dbgFunctionExit() this: %p, rs: %d, rts: %p, xsink: %d\n", this, runState, runToStatement, xsink && xsink->isEvent());
   } else if (functionCallLevel > 0) {
      functionCallLevel--;
      printd(5, "ThreadLocalProgramData::dbgFunctionExit() exit, this: %p, rs: %d, level: %d, tid: %d\n", this, runState, functionCallLevel, gettid());
   } else {
      checkAttach(xsink);
      checkBreakFlag();
   }
}

void ThreadLocalProgramData::dbgException(const AbstractStatement* statement, ExceptionSink* xsink) {
   if (runState != DBG_RS_STOPPED && runState != DBG_RS_DETACH) {
      printd(5, "ThreadLocalProgramData::dbgException() this: %p, rs: %d, rts: %p, tid: %d\n", this, runState, runToStatement, gettid());
      checkAttach(xsink);
      functionCallLevel = 0;
      DebugRunStateEnum rs = runState;
      const AbstractStatement* rts = runToStatement;
      runState = DBG_RS_STOPPED;
      runToStatement = nullptr;
      getProgram()->priv->onException(statement, rs, rts, xsink);
      setRunState(rs, rts);
      printd(5, "ThreadLocalProgramData::dbgException() this: %p, rs: %d, rts: %p, xsink: %d\n", this, runState, runToStatement, xsink && xsink->isEvent());
   }
}

void ThreadLocalProgramData::dbgExit(const StatementBlock* statement, QoreValue& returnValue, ExceptionSink* xsink) {
   if (runState != DBG_RS_STOPPED && runState != DBG_RS_DETACH) {
      printd(5, "ThreadLocalProgramData::dbgExit() this: %p, rs: %d, rts: %p, tid: %d\n", this, runState, runToStatement, gettid());
      DebugRunStateEnum rs = runState;
      const AbstractStatement* rts = runToStatement;
      functionCallLevel = 0;
      runState = DBG_RS_STOPPED;
      runToStatement = nullptr;
      getProgram()->priv->onExit(statement, returnValue, rs, rts, xsink);
      setRunState(rs, rts);
      printd(5, "ThreadLocalProgramData::dbgExit() this: %p, rs: %d, rts: %p, xsink: %d\n", this, runState, runToStatement, xsink && xsink->isEvent());
   }
}

QoreProgram::~QoreProgram() {
   printd(5, "QoreProgram::~QoreProgram() this: %p, pgmid: %d\n", this, priv->getProgramId());
   delete priv;
}

// setup independent program object
QoreProgram::QoreProgram() : priv(new qore_program_private(this, PO_DEFAULT)) {
   printd(5, "QoreProgram::QoreProgram() this: %p, pgmid: %d\n", this, priv->getProgramId());
}

// setup independent program object
QoreProgram::QoreProgram(int64 po) : priv(new qore_program_private(this, po)) {
}

QoreProgram::QoreProgram(QoreProgram* pgm, int64 po, bool ec, const char* ecn) : priv(new qore_program_private(this, po, pgm)) {
   printd(QPP_DBG_LVL, "QoreProgram::QoreProgram(), this: %p, pgm: %p, priv: %p, pgmid: %d\n", this, pgm, priv, priv->getProgramId());
   priv->exec_class = ec;
   if (ecn)
      priv->exec_class_name = ecn;
}

QoreThreadLock* QoreProgram::getParseLock() {
   return &priv->plock;
}

void QoreProgram::deref(ExceptionSink* xsink) {
   printd(QPP_DBG_LVL, "QoreProgram::deref() this: %p priv: %p %d->%d\n", this, priv, reference_count(), reference_count() - 1);
   if (ROdereference())
      priv->clear(xsink);
}

ExceptionSink* QoreProgram::getParseExceptionSink() {
   if (priv->requires_exception)
      return 0;

   return priv->parseSink;
}

int QoreProgram::setWarningMask(int wm) {
   if (!(priv->pwo.parse_options & PO_LOCK_WARNINGS)) {
      priv->pwo.warn_mask = wm;
      return 0;
   }
   return -1;
}

// returns 0 for success, -1 for error
int QoreProgram::enableWarning(int code) {
   if (!(priv->pwo.parse_options & PO_LOCK_WARNINGS)) {
      priv->pwo.warn_mask |= code;
      return 0;
   }
   return -1;
}

// returns 0 for success, -1 for error
int QoreProgram::disableWarning(int code) {
   if (!(priv->pwo.parse_options & PO_LOCK_WARNINGS)) {
      priv->pwo.warn_mask &= ~code;
      return 0;
   }
   return -1;
}

RootQoreNamespace* QoreProgram::getRootNS() const {
   return priv->RootNS;
}

int QoreProgram::getParseOptions() const {
   return (int)priv->pwo.parse_options;
}

int64 QoreProgram::getParseOptions64() const {
   return priv->pwo.parse_options;
}

QoreListNode* QoreProgram::getUserFunctionList() {
   ExceptionSink xsink;
   ProgramRuntimeParseAccessHelper pah(&xsink, this);
   if (xsink) {
      xsink.clear();
      return 0;
   }
   return qore_ns_private::getUserFunctionList(*priv->RootNS);
}

void QoreProgram::waitForTermination() {
   priv->waitForAllThreadsToTerminate();
}

void QoreProgram::waitForTerminationAndDeref(ExceptionSink* xsink) {
   priv->waitForTerminationAndClear(xsink);
   deref(xsink);
}

void QoreProgram::lockOptions() {
   priv->po_locked = true;
}

// setExecClass() NOTE: string passed here will copied
void QoreProgram::setExecClass(const char* ecn) {
   priv->exec_class = true;
   if (ecn)
      priv->exec_class_name = ecn;
   else
      priv->exec_class_name.clear();
}

QoreNamespace* QoreProgram::getQoreNS() const {
   return priv->QoreNS;
}

void QoreProgram::depRef() {
   priv->depRef();
}

void QoreProgram::depDeref() {
   priv->depDeref();
}

bool QoreProgram::checkWarning(int code) const {
   return priv->warnSink && (code & priv->pwo.warn_mask);
}

int QoreProgram::getWarningMask() const {
   return priv->warnSink ? priv->pwo.warn_mask : 0;
}

bool QoreProgram::existsFunction(const char* name) {
   ExceptionSink xsink;
   ProgramRuntimeParseAccessHelper pah(&xsink, this);
   if (xsink) {
      xsink.clear();
      return false;
   }
   return qore_root_ns_private::runtimeExistsFunction(*priv->RootNS, name) ? true : false;
}

// DEPRECATED
void QoreProgram::parseSetParseOptions(int po) {
   priv->parseSetParseOptions(QoreProgramLocation(), (int64)po);
}

void QoreProgram::parseSetParseOptions(int64 po) {
   priv->parseSetParseOptions(QoreProgramLocation(), po);
}

void QoreProgram::parseDisableParseOptions(int64 po) {
   priv->parseDisableParseOptions(QoreProgramLocation(), po);
}

// DEPRECATED
void QoreProgram::setParseOptions(int po, ExceptionSink* xsink) {
   priv->setParseOptions((int64)po, xsink);
}

void QoreProgram::setParseOptions(int64 po, ExceptionSink* xsink) {
   priv->setParseOptions(po, xsink);
}

// DEPRECATED
void QoreProgram::disableParseOptions(int po, ExceptionSink* xsink) {
   priv->disableParseOptions((int64)po, xsink);
}

void QoreProgram::disableParseOptions(int64 po, ExceptionSink* xsink) {
   priv->disableParseOptions(po, xsink);
}

void QoreProgram::replaceParseOptions(int64 po, ExceptionSink* xsink) {
   priv->replaceParseOptions(po, xsink);
}

QoreHashNode* QoreProgram::getThreadData() {
   return priv->getThreadData();
}

AbstractQoreNode* QoreProgram::run(ExceptionSink* xsink) {
   if (!priv->exec_class_name.empty()) {
      runClass(priv->exec_class_name.c_str(), xsink);
      AbstractQoreNode* rv = priv->exec_class_rv;
      priv->exec_class_rv = 0;
      return rv;
   }
   return runTopLevel(xsink);
}

void QoreProgram::parse(FILE* fp, const char* name, ExceptionSink* xsink, ExceptionSink* wS, int wm) {
   priv->parse(fp, name, xsink, wS, wm);
}

void QoreProgram::parse(const QoreString* str, const QoreString* lstr, ExceptionSink* xsink, ExceptionSink* wS, int wm) {
   if (!str || str->empty())
      return;

   priv->parse(str, lstr, xsink, wS, wm);
}

void QoreProgram::parse(const QoreString* str, const QoreString* lstr, ExceptionSink* xsink, ExceptionSink* wS, int wm, const QoreString* source, int offset) {
   if (!str || str->empty())
      return;

   priv->parse(str, lstr, xsink, wS, wm, source, offset);
}

void QoreProgram::parse(const char* code, const char* label, ExceptionSink* xsink, ExceptionSink* wS, int wm) {
   if (!code || !code[0])
      return;

   priv->parse(code, label, xsink, wS, wm);
}

void QoreProgram::parse(const char* code, const char* label, ExceptionSink* xsink, ExceptionSink* wS, int wm, const char* source, int offset) {
   if (!code || !code[0])
      return;

   priv->parse(code, label, xsink, wS, wm, source, offset);
}

void QoreProgram::parseFile(const char* filename, ExceptionSink* xsink, ExceptionSink* wS, int wm, bool only_first_except) {
   priv->only_first_except = only_first_except;
   priv->parseFile(filename, xsink, wS, wm);
}

void QoreProgram::parsePending(const QoreString* str, const QoreString* lstr, ExceptionSink* xsink, ExceptionSink* wS, int wm) {
   if (!str || str->empty())
      return;

   priv->parsePending(str, lstr, xsink, wS, wm);
}

void QoreProgram::parsePending(const QoreString* str, const QoreString* lstr, ExceptionSink* xsink, ExceptionSink* wS, int wm, const QoreString* source, int offset) {
   if (!str || str->empty())
      return;

   priv->parsePending(str, lstr, xsink, wS, wm, source, offset);
}

void QoreProgram::parsePending(const char* code, const char* label, ExceptionSink* xsink, ExceptionSink* wS, int wm) {
   if (!code || !code[0])
      return;

   priv->parsePending(code, label, xsink, wS, wm);
}

void QoreProgram::parsePending(const char* code, const char* label, ExceptionSink* xsink, ExceptionSink* wS, int wm, const char* source, int offset) {
   if (!code || !code[0])
      return;

   priv->parsePending(code, label, xsink, wS, wm, source, offset);
}

AbstractQoreNode* QoreProgram::runTopLevel(ExceptionSink* xsink) {
   ProgramThreadCountContextHelper tch(xsink, this, true);
   if (*xsink)
      return 0;
   return priv->sb.exec(xsink).takeNode();
}

AbstractQoreNode* QoreProgram::callFunction(const char* name, const QoreListNode* args, ExceptionSink* xsink) {
   SimpleRefHolder<FunctionCallNode> fc;

   printd(5, "QoreProgram::callFunction() creating function call to %s()\n", name);

   const qore_ns_private* ns = 0;
   const QoreFunction* qf;

   // need to grab parse lock for safe access to the user function map and imported function map
   {
      ProgramRuntimeParseAccessHelper pah(xsink, this);
      if (*xsink)
         return 0;
      qf = qore_root_ns_private::runtimeFindFunction(*priv->RootNS, name, ns);
   }

   if (!qf) {
      xsink->raiseException("NO-FUNCTION", "function name '%s' does not exist", name);
      return 0;
   }

   // we assign the args to 0 below so that they will not be deleted
   fc = new FunctionCallNode(get_runtime_location(), qf, const_cast<QoreListNode*>(args), this);
   AbstractQoreNode* rv = !*xsink ? fc->eval(xsink) : 0;

   // let caller delete function arguments if necessary
   fc->takeArgs();

   return rv;
}

void QoreProgram::parseCommit(ExceptionSink* xsink, ExceptionSink* wS, int wm) {
   ProgramRuntimeParseCommitContextHelper pch(xsink, this);
   if (*xsink)
      return;
   priv->parseCommit(xsink, wS, wm);
}

void QoreProgram::parseRollback() {
   ExceptionSink xsink;
   priv->parseRollback(&xsink);
}

int QoreProgram::parseRollback(ExceptionSink* xsink) {
   return priv->parseRollback(xsink);
}

void QoreProgram::runClass(const char* classname, ExceptionSink* xsink) {
   // find class
   const QoreClass* qc = qore_root_ns_private::runtimeFindClass(*priv->RootNS, classname);
   if (!qc) {
      xsink->raiseException("CLASS-NOT-FOUND", "cannot find any class '%s' in any namespace", classname);
      return;
   }

   if (qore_class_private::runtimeCheckInstantiateClass(*qc, xsink))
      return;

   //printd(5, "QoreProgram::runClass(%s)\n", classname);

   ProgramThreadCountContextHelper tch(xsink, this, true);
   if (!*xsink)
      discard(qc->execConstructor((QoreValueList*)0, xsink), xsink);
}

void QoreProgram::parseFileAndRunClass(const char* filename, const char* classname) {
   ExceptionSink xsink;

   parseFile(filename, &xsink);

   if (!xsink.isEvent())
      runClass(classname, &xsink);
}

void QoreProgram::parseAndRunClass(FILE* fp, const char* name, const char* classname) {
   ExceptionSink xsink;

   parse(fp, name, &xsink);

   if (!xsink.isEvent())
      runClass(classname, &xsink);
}

void QoreProgram::parseAndRunClass(const char* str, const char* name, const char* classname) {
   ExceptionSink xsink;

   parse(str, name, &xsink);

   if (!xsink.isEvent())
      runClass(classname, &xsink);
}

void QoreProgram::parseFileAndRun(const char* filename) {
   ExceptionSink xsink;

   parseFile(filename, &xsink);

   if (!xsink.isEvent()) {
      // get class name
      if (priv->exec_class) {
         if (!priv->exec_class_name.empty())
            runClass(priv->exec_class_name.c_str(), &xsink);
         else {
            char* c, *bn = q_basenameptr(filename);
            if (!(c = strrchr(bn, '.')))
               runClass(filename, &xsink);
            else {
               QoreString qcn; // for possible class name
               qcn.concat(bn, c - bn);
               runClass(qcn.getBuffer(), &xsink);
            }
         }
      }
      else
         run(&xsink);
   }
}

void QoreProgram::parseAndRun(FILE* fp, const char* name) {
   ExceptionSink xsink;

   if (priv->exec_class && priv->exec_class_name.empty())
      xsink.raiseException("EXEC-CLASS-ERROR", "class name required if executing from stdin");
   else {
      parse(fp, name, &xsink);

      if (!xsink.isEvent())
         run(&xsink);
   }
}

void QoreProgram::parseAndRun(const char* str, const char* name) {
   ExceptionSink xsink;

   if (priv->exec_class && priv->exec_class_name.empty())
      xsink.raiseException("EXEC-CLASS-ERROR", "class name required if executing from a direct string");
   else {
      parse(str, name, &xsink);

      if (!xsink.isEvent())
         run(&xsink);
   }
}

bool QoreProgram::checkFeature(const char* f) const {
   bool b = priv->featureList.find(f);
   if (!b)
      b = priv->userFeatureList.find(f);
   return b;
}

void QoreProgram::addFeature(const char* f) {
   priv->featureList.push_back(f);
}

QoreListNode* QoreProgram::getFeatureList() const {
   return priv->getFeatureList();
}

QoreListNode* QoreProgram::getVarList() {
   return priv->getVarList();
}

const char* QoreProgram::parseGetScriptDir() const {
   return priv->parseGetScriptDir();
}

QoreStringNode* QoreProgram::getScriptDir() const {
   return priv->getScriptDir();
}

QoreStringNode* QoreProgram::getScriptPath() const {
   return priv->getScriptPath();
}

QoreStringNode* QoreProgram::getScriptName() const {
   return priv->getScriptName();
}

void QoreProgram::setScriptPath(const char* path) {
   priv->setScriptPathExtern(path);
}

const LVList* QoreProgram::getTopLevelLVList() const {
   return priv->sb.getLVList();
}

QoreValue QoreProgram::getGlobalVariableVal(const char* var, bool& found) const {
   const qore_ns_private* vns = 0;
   Var* v = qore_root_ns_private::runtimeFindGlobalVar(*(priv->RootNS), var, vns);
   if (!v) {
      found = false;
      return QoreValue();
   }
   found = true;
   return v->eval();
}

AbstractQoreNode* QoreProgram::getGlobalVariableValue(const char* var, bool& found) const {
   return getGlobalVariableVal(var, found).takeNode();
}

// only called when parsing, therefore in the parse thread lock
void QoreProgram::parseSetIncludePath(const char* path) {
   priv->include_path = path;
}

// only called when parsing, therefore in the parse thread lock
const char* QoreProgram::parseGetIncludePath() const {
   return priv->include_path.empty() ? 0 : priv->include_path.c_str();
}

const AbstractQoreZoneInfo* QoreProgram::currentTZ() const {
   return priv->currentTZ();
}

void QoreProgram::setTZ(const AbstractQoreZoneInfo* n_TZ) {
   priv->setTZ(n_TZ);
}

bool QoreProgram::parseExceptionRaised() const {
   return priv->parseExceptionRaised();
}

void QoreProgram::parseSetTimeZone(const char* zone) {
   return priv->parseSetTimeZone(zone);
}

AbstractQoreNode* qore_parse_get_define_value(const QoreProgramLocation& loc, const char* str, QoreString& arg, bool& ok) {
   ok = true;
   char c = arg[0];
   // see if a string is being defined
   if (c == '"' || c == '\'') {
      // make sure the string is terminated in the same way
      char e = arg[arg.strlen() - 1];
      if (c != e || arg.strlen() == 1) {
         parse_error(loc, "'%s' is defined with an unterminated string; %%define directives must be made on a single line", str);
         ok = false;
         return 0;
      }

      // string is OK, remove quotes
      arg.trim_single_trailing(c);
      arg.trim_single_leading(c);
      qore_size_t len = arg.strlen();
      return new QoreStringNode(arg.giveBuffer(), len, len + 1, QCS_DEFAULT);
   }

   const char* p = arg.getBuffer();
   // check for 'true' and 'false'
   if (!strcasecmp(p, "true"))
      return &True;
   if (!strcasecmp(p, "false"))
      return &False;

   // see if there are non-numeric characters in the string
   bool flt = false;
   while (*p) {
      if (*p == '.') {
         if (flt) {
            parse_error(loc, "'%s' is defined with an invalid number: '%s'", str, arg.getBuffer());
            ok = false;
            return 0;
         }
         flt = true;
      }
      else if (isalpha(*p)) {
         parse_error(loc, "'%s' has unquoted alphabetic characters in the value; use quotes (\" or ') to define strings", str);
         ok = false;
         return 0;
      }
      ++p;
   }

   p = arg.getBuffer();
   if (flt)
      return new QoreFloatNode(q_strtod(p));
   return new QoreBigIntNode(strtoll(p, 0, 10));
}

void QoreProgram::parseDefine(const char* str, AbstractQoreNode* val) {
   priv->parseDefine(qoreCommandLineLocation, str, val);
}

void QoreProgram::parseDefine(const char* str, const char* val) {
   priv->parseDefine(qoreCommandLineLocation, str, new QoreStringNode(val));
}

void QoreProgram::parseCmdLineDefines(const std::map<std::string, std::string> defmap, ExceptionSink& xs, ExceptionSink& ws, int wm) {
   parseCmdLineDefines(xs, ws, wm, defmap);
}

void QoreProgram::parseCmdLineDefines(ExceptionSink& xs, ExceptionSink& ws, int wm, const std::map<std::string, std::string>& defmap) {
   ProgramRuntimeParseCommitContextHelper pch(&xs, this);
   if (xs)
      return;

   priv->startParsing(&xs, &ws, wm);

   for (std::map<std::string, std::string>::const_iterator it = defmap.begin(); it != defmap.end(); ++it) {
      const char *str = it->first.c_str();
      const char *val = it->second.c_str();
      QoreString arg(val);
      arg.trim();

      bool ok;
      AbstractQoreNode* v = qore_parse_get_define_value(qoreCommandLineLocation, str, arg, ok);
      if (!ok)
         break;
      priv->parseDefine(qoreCommandLineLocation, str, v);
   }

   priv->parseSink = 0;
   priv->warnSink = 0;
}

QoreProgram* QoreProgram::programRefSelf() const {
   const_cast<QoreProgram*>(this)->ref();
   return const_cast<QoreProgram*>(this);
}

void QoreProgram::setExternalData(const char* owner, AbstractQoreProgramExternalData* pud) {
   priv->setExternalData(owner, pud);
}

AbstractQoreProgramExternalData* QoreProgram::getExternalData(const char* owner) const {
   return priv->getExternalData(owner);
}

QoreHashNode* QoreProgram::getGlobalVars() const {
   return priv->getGlobalVars();
}

int QoreProgram::setGlobalVarValue(const char* name, QoreValue val, ExceptionSink* xsink) {
   return priv->setGlobalVarValue(name, val, xsink);
}

QoreListNode* QoreProgram::getThreadList() const {
   ReferenceHolder<QoreListNode> rv(new QoreListNode(bigIntTypeInfo), nullptr);
   priv->getThreadList(**rv);
   return rv.release();
}

AbstractQoreProgramExternalData::~AbstractQoreProgramExternalData() {
}

int get_warning_code(const char* str) {
   for (unsigned i = 0; i < NUM_WARNINGS; i++)
      if (!strcasecmp(str, qore_warnings[i]))
         return 1 << i;
   return 0;
}

QoreDebugProgram::QoreDebugProgram(): priv(new qore_debug_program_private(this)) {};

QoreDebugProgram::~QoreDebugProgram() {
   printd(5, "QoreDebugProgram::~QoreDebugProgram() this: %p\n", this);
   delete priv;
}

void QoreDebugProgram::onAttach(QoreProgram *pgm, DebugRunStateEnum &rs, const AbstractStatement* &rts, ExceptionSink* xsink) {
    printd(5, "QoreDebugProgram::onAttach() this: %p\n", this);
    rs = DBG_RS_RUN;
}

void QoreDebugProgram::onDetach(QoreProgram *pgm, DebugRunStateEnum &rs, const AbstractStatement* &rts, ExceptionSink* xsink) {
    printd(5, "QoreDebugProgram::onDetach() this: %p\n", this);
    rs = DBG_RS_DETACH;
}

void QoreDebugProgram::onStep(QoreProgram *pgm, const StatementBlock *blockStatement, const AbstractStatement *statement, unsigned bkptId, int &flow, DebugRunStateEnum &rs, const AbstractStatement* &rts, ExceptionSink* xsink) {
}

void QoreDebugProgram::onFunctionEnter(QoreProgram *pgm, const StatementBlock *statement, DebugRunStateEnum &rs, const AbstractStatement* &rts, ExceptionSink* xsink) {
}

void QoreDebugProgram::onFunctionExit(QoreProgram *pgm, const StatementBlock *statement, QoreValue& returnValue, DebugRunStateEnum &rs, const AbstractStatement* &rts, ExceptionSink* xsink) {
}

void QoreDebugProgram::onException(QoreProgram *pgm, const AbstractStatement *statement, DebugRunStateEnum &rs, const AbstractStatement* &rts, ExceptionSink* xsink) {
}

void QoreDebugProgram::onExit(QoreProgram *pgm, const StatementBlock *statement, QoreValue& returnValue, DebugRunStateEnum &rs, const AbstractStatement* &rts, ExceptionSink* xsink) {
}

void QoreDebugProgram::addProgram(QoreProgram *pgm, ExceptionSink* xsink) {
   printd(5, "QoreDebugProgram::addProgram(), this: %p, pgm: %p\n", this, pgm);
   priv->addProgram(pgm, xsink);
}

void QoreDebugProgram::removeProgram(QoreProgram *pgm) {
   printd(5, "QoreDebugProgram::removeProgram(), this: %p, pgm: %p\n", this, pgm);
   priv->removeProgram(pgm);
}

QoreListNode* QoreDebugProgram::getAllProgramObjects() {
   return priv->getAllProgramObjects();
}

int QoreDebugProgram::breakProgramThread(QoreProgram *pgm, int tid) const {
   printd(5, "QoreDebugProgram::breakProgramThread(), this: %p, pgm: %p, tid: %d\n", this, pgm, tid);
   return priv->breakProgramThread(pgm, tid);
}

int QoreDebugProgram::breakProgram(QoreProgram *pgm) const {
   printd(5, "QoreDebugProgram::breakProgram(), this: %p, pgm: %p\n", this, pgm);
   return priv->breakProgram(pgm);
}

void QoreDebugProgram::waitForTerminationAndClear(ExceptionSink* xsink) {
   printd(5, "QoreDebugProgram::waitForTerminationAndClear(), this: %p\n", this);
   priv->waitForTerminationAndClear(xsink);
}

int QoreDebugProgram::getInterruptedCount() {
   return priv->getInterruptedCount();
}

void QoreProgram::assignBreakpoint(QoreBreakpoint *bkpt, ExceptionSink *xsink) {
   priv->assignBreakpoint(bkpt, xsink);
}

void QoreProgram::deleteAllBreakpoints() {
   priv->deleteAllBreakpoints();
}

void QoreProgram::getBreakpoints(QoreBreakpointList_t &bkptList) {
   priv->getBreakpoints(bkptList);
}

void QoreProgram::getStatementBreakpoints(const AbstractStatement* statement, QoreBreakpointList_t &bkptList) {
   priv->getStatementBreakpoints(statement, bkptList);
}

AbstractStatement* QoreProgram::findStatement(const char* fileName, int line) const {
   return priv->getStatementFromIndex(fileName, line);
}

AbstractStatement* QoreProgram::findFunctionStatement(const char* functionName, const QoreValueList* params, ExceptionSink* xsink) const {
   const AbstractQoreFunctionVariant* uv = runtimeFindCall(functionName, params, xsink);
   if (!uv)
      return nullptr;
   const UserVariantBase* uvb = uv->getUserVariantBase();
   //printd(5, "QoreProgram::findFunctionStatement() '%s' -> %p\n", functionName, uvb);
   if (!uvb)
      return nullptr;
   return uvb->getStatementBlock();
}

unsigned long QoreProgram::getStatementId(const AbstractStatement* statement) const {
   return priv->getStatementId(statement);
}

AbstractStatement* QoreProgram::resolveStatementId(unsigned long statementId) const {
   return priv->resolveStatementId(statementId);
}

QoreHashNode* QoreProgram::getSourceFileNames(ExceptionSink* xsink) const {
   return priv->getSourceFileNames(xsink);
}

QoreHashNode* QoreProgram::getSourceLabels(ExceptionSink* xsink) const {
   return priv->getSourceLabels(xsink);
}

unsigned QoreProgram::getProgramId() const {
   return priv->getProgramId();
}

QoreProgram* QoreProgram::resolveProgramId(unsigned programId) {
   return qore_program_private::resolveProgramId(programId);
}

void QoreProgram::registerQoreObject(QoreObject *o, ExceptionSink* xsink) const {
   priv->registerQoreObject(o, xsink);
}

void QoreProgram::unregisterQoreObject(QoreObject *o, ExceptionSink* xsink) const {
   priv->unregisterQoreObject(o, xsink);
}

QoreObject* QoreProgram::findQoreObject() const {
   return priv->findQoreObject();
}

QoreObject* QoreProgram::getQoreObject(QoreProgram* pgm) {
   return qore_program_private::getQoreObject(pgm);
}

QoreListNode* QoreProgram::getAllQoreObjects(ExceptionSink* xsink) {
   return qore_program_private::getAllQoreObjects(xsink);
}

bool QoreProgram::checkAllowDebugging(ExceptionSink* xsink) {
   return priv->checkAllowDebugging(xsink);
}

const AbstractQoreFunctionVariant* QoreProgram::runtimeFindCall(const char* name, const QoreValueList* params, ExceptionSink* xsink) const {
   return priv->runtimeFindCall(name, params, xsink);
}

QoreValueList* QoreProgram::runtimeFindCallVariants(const char* name, ExceptionSink* xsink) const {
   return priv->runtimeFindCallVariants(name, xsink);
}

QoreRWLock QoreBreakpoint::lck_breakpoint;
QoreBreakpoint::QoreBreakpointList_t QoreBreakpoint::breakpointList;
volatile unsigned QoreBreakpoint::breakpointIdCounter = 1;

void QoreBreakpoint::unassignAllStatements() {
    for (std::list<AbstractStatement*>::iterator it = statementList.begin(); it != statementList.end(); ++it) {
        (*it)->unassignBreakpoint(this);
    }
    statementList.clear();
}

QoreBreakpoint& QoreBreakpoint::operator=(const QoreBreakpoint& other) {
    qo = 0;
    QoreAutoRWReadLocker al(&pgm->lck_breakpoint);
    enabled = other.enabled;
    policy = other.policy;
    pgm = other.pgm;
    statementList = other.statementList;
    tidMap = other.tidMap;

    return *this;
}

QoreBreakpoint::QoreBreakpoint(): pgm(nullptr), qo(0), enabled(false), policy(BKP_PO_NONE) {
    QoreAutoRWWriteLocker al(&QoreBreakpoint::lck_breakpoint);
    QoreBreakpoint::breakpointList.push_back(this);
    breakpointId = breakpointIdCounter++;
}

QoreBreakpoint::~QoreBreakpoint() {
    if (pgm) {
        QoreAutoRWWriteLocker al(&pgm->lck_breakpoint);
        pgm->breakpointList.remove(this);
        unassignAllStatements();
    } else {
        unassignAllStatements();
    }
    QoreAutoRWWriteLocker al(&QoreBreakpoint::lck_breakpoint);
    QoreBreakpoint::breakpointList.remove(this);
}

// lck_breakpoint lock should be acquired
bool QoreBreakpoint::isStatementAssigned(const AbstractStatement *statement) const {
    return std::find(statementList.begin(), statementList.end(), statement) != statementList.end();
}

bool QoreBreakpoint::checkBreak() const {
    if (!enabled)
        return false;
    switch (policy) {
    case BKP_PO_NONE:
        return true;
    case BKP_PO_ACCEPT:
        return tidMap.find(gettid()) != tidMap.end();
    case BKP_PO_REJECT:
        return tidMap.find(gettid()) == tidMap.end();
    default:
        return false;
    }
}

bool QoreBreakpoint::checkPgm(ExceptionSink* xsink) const {
    if (pgm) {
        return true;
    } else {
        if (xsink) {
            xsink->raiseException("BREAKPOINT-ERROR", "QoreProgram is not assigned");
        }
        return false;
    }
}

void QoreBreakpoint::assignProgram(QoreProgram *new_pgm, ExceptionSink* xsink) {
    if (new_pgm) {
        new_pgm->assignBreakpoint(this, xsink);
    } else {
        if (pgm) {
            QoreAutoRWWriteLocker al(&pgm->lck_breakpoint);
            pgm->breakpointList.remove(this);
            unassignAllStatements();
            pgm = nullptr;
            deref();
        }
    }
}

QoreProgram* QoreBreakpoint::getProgram() const {
    return pgm->pgm;
}

void QoreBreakpoint::assignStatement(AbstractStatement* statement, ExceptionSink* xsink) {
    if (checkPgm(xsink)) {
        QoreAutoRWWriteLocker al(&pgm->lck_breakpoint);
        if (pgm->isBreakpointRegistered(this)) {
            if (statement && !isStatementAssigned(statement)) {
                statementList.push_back(statement);
                statement->assignBreakpoint(this);
            }
        }
    }
}

void QoreBreakpoint::unassignStatement(AbstractStatement* statement, ExceptionSink* xsink) {
    if (checkPgm(xsink)) {
        QoreAutoRWWriteLocker al(&pgm->lck_breakpoint);
        if (pgm->isBreakpointRegistered(this)) {
            if (statement && isStatementAssigned(statement)) {
                statementList.remove(statement);
                statement->unassignBreakpoint(this);
            }
        }
    }
}

void QoreBreakpoint::getStatements(AbstractStatementList_t &statList, ExceptionSink* xsink) {
    statList.clear();
    QoreAutoRWReadLocker al(pgm ? &pgm->lck_breakpoint : nullptr);
    for (AbstractStatementList_t::iterator it = statementList.begin(); it != statementList.end(); ++it) {
        statList.push_back(*it);
    }
}

QoreListNode* QoreBreakpoint::getStatementIds(ExceptionSink* xsink) {
    QoreAutoRWReadLocker al(pgm ? &pgm->lck_breakpoint : nullptr);
    ReferenceHolder<QoreListNode> l(new QoreListNode, xsink);
    for (AbstractStatementList_t::iterator it = statementList.begin(); it != statementList.end(); ++it) {
        (*l)->push(new QoreBigIntNode(pgm->getStatementId(*it)));
    }
    return l.release();
}

AbstractStatement* QoreBreakpoint::resolveStatementId(unsigned long statementId, ExceptionSink* xsink) const {
    AbstractStatement *s = nullptr;
    if (checkPgm(xsink)) {
        s = pgm->resolveStatementId(statementId);
        if (!s) {
            if (xsink) {
                xsink->raiseException("BREAKPOINT-ERROR", "Cannot resolve statement (%lu)", statementId);
            }
        }
    }
    return s;
}

void QoreBreakpoint::getThreadIds(TidList_t &tidList, ExceptionSink* xsink) {
    tidList.clear();
    QoreAutoRWReadLocker al(pgm ? &pgm->lck_breakpoint : nullptr);
    for (TidMap_t::iterator it = tidMap.begin(); it != tidMap.end(); ++it) {
        for (int j = 0; j < it->second; j++) {
            tidList.push_back(it->first+j);
        }
    }
}

void QoreBreakpoint::setThreadIds(TidList_t tidList, ExceptionSink* xsink) {
    QoreAutoRWWriteLocker al(pgm ? &pgm->lck_breakpoint : nullptr);
    tidMap.clear();
    tidList.sort();  // or create copy not to sort argument list ?
    TidList_t::iterator it = tidList.begin();
    while (it != tidList.end()) {
        int tid = *it;
        int cnt = 1;
        ++it;
        while (it != tidList.end()) {
            if (tid + cnt != *it) break;
            ++it;
            cnt++;
        }
        //printd(5, "QoreBreakpoint::setThreadIds(), tid: %d, cnt: %d\n", tid, cnt);
        tidMap.insert(TidMap_t::value_type(tid, cnt));
    }
}

void QoreBreakpoint::addThreadId(int tid, ExceptionSink* xsink) {
    QoreAutoRWWriteLocker al(pgm ? &pgm->lck_breakpoint : nullptr);
    if (!tidMap.empty()) {
        TidMap_t::iterator it2 = tidMap.upper_bound(tid);
        if (it2 != tidMap.begin()) {
            TidMap_t::iterator it = it2;
            --it;
            if ((tid >= it->first && tid < it->first + it->second) ) {
                // already exists
                return;
            }
            if (tid == it->first + it->second) {
                // extend existing range
                it->second++;
                if (it2 != tidMap.end() && tid == it2->first - 1) {
                    // join
                    it->second += it2->second;
                    tidMap.erase(it2);
                }
                return;
            }
        }
        if (it2 != tidMap.end() && tid == it2->first - 1) {
            // extend lower bound
            tidMap.insert(TidMap_t::value_type(it2->first - 1, it2->second + 1));
            tidMap.erase(it2);
            return;
        }
    }
    tidMap.insert(TidMap_t::value_type(tid, 1));
}

void QoreBreakpoint::removeThreadId(int tid, ExceptionSink* xsink) {
    QoreAutoRWWriteLocker al(pgm ? &pgm->lck_breakpoint : nullptr);
    if (!tidMap.empty()) {
        TidMap_t::iterator it = tidMap.upper_bound(tid);
        if (it != tidMap.begin()) {
            --it;  // we can iterate from end() as well
            if (tid >= it->first && tid < it->first + it->second) {
                if (tid == it->first) {
                    tidMap.insert(TidMap_t::value_type(it->first + 1, it->second - 1));
                    tidMap.erase(it);
                } else if (tid == it->first + it->second - 1) {
                    it->second--;
                } else {
                    tidMap.insert(TidMap_t::value_type(tid + 1, it->first + it->second - tid - 1));
                    it->second = tid - it->first;
                }
            }
        }
    }
}

bool QoreBreakpoint::isThreadId(int tid, ExceptionSink* xsink) {
    QoreAutoRWReadLocker al(pgm ? &pgm->lck_breakpoint : nullptr);
    //printd(5, "QoreBreakpoint::isThreadId(%d), tidMap.empty()==%d\n", tid, tidMap.empty());
    if (!tidMap.empty()) {
        TidMap_t::iterator it = tidMap.upper_bound(tid);
        if (it != tidMap.begin()) {
            --it;  // we can iterate from end() as well
            //printd(5, "QoreBreakpoint::isThreadId(%d), it.tid: %d, it.cnt: %d\n", tid, it->first, it->second);
            return (tid >= it->first && tid < it->first + it->second);
        }
        //printd(5, "QoreBreakpoint::isThreadId(%d), it@begin()\n", tid);
    }
    return false;
}

void QoreBreakpoint::clearThreadIds(ExceptionSink* xsink) {
    QoreAutoRWWriteLocker al(pgm ? &pgm->lck_breakpoint : nullptr);
    tidMap.clear();
}

unsigned QoreBreakpoint::getBreakpointId() const {
    return breakpointId;
}

QoreBreakpoint* QoreBreakpoint::resolveBreakpointId(unsigned breakpointId) {
    QoreAutoRWReadLocker al(&QoreBreakpoint::lck_breakpoint);
    for (QoreBreakpointList_t::const_iterator i = QoreBreakpoint::breakpointList.begin(); i != QoreBreakpoint::breakpointList.end(); i++) {
        if ((*i)->breakpointId == breakpointId)
            return *i;
    }
    return nullptr;
}

QoreObject* QoreBreakpoint::getQoreObject() {
    QoreAutoRWWriteLocker al(&QoreBreakpoint::lck_breakpoint);   // reuse global lock, local would be enough
    if (qo) {
        qo->ref();
    } else {
        qo = new QoreObject(QC_BREAKPOINT, ::getProgram(), this);
        this->ref();
    }
    return qo;
}

void QoreBreakpoint::setQoreObject(QoreObject *n_qo) {
    qo = n_qo;
}
