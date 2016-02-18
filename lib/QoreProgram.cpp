/*
  QoreProgram.cpp

  Program QoreObject Definition

  Qore Programming Language

  Copyright (C) 2003 - 2016 David Nichols

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
#include <qore/intern/QoreSignal.h>
#include <qore/intern/LocalVar.h>
#include <qore/intern/qore_program_private.h>
#include <qore/intern/QoreNamespaceIntern.h>
#include <qore/intern/ConstantList.h>

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
   std::auto_ptr<ExceptionSink> xs;
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
   //printd(5, "qore_program_private_base::setParent() this: %p parent: %p (parent lvl: %p) this: %p (this pgm: %p) parent po: %lld new po: %lld parent no_child_po_restrictions: %d\n", this, p_pgm, p_pgm->priv->sb.getLVList(), this, pgm, p_pgm->priv->pwo.parse_options, n_parse_options, p_pgm->priv->pwo.parse_options & PO_NO_CHILD_PO_RESTRICTIONS);

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

      printd(5, "qore_program_private::waitForTerminationAndClear() this: %p clr: %d\n", this, clr);
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

      // clear program location
      update_runtime_location(QoreProgramLocation());
   }
}

// called when the program's ref count = 0 (but the dc count may not go to 0 yet)
void qore_program_private::clear(ExceptionSink* xsink) {
   waitForTerminationAndClear(xsink);
   depDeref(xsink);
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
      sb.parseCommit();

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
   if (!*xsink) {
      runtimeImportSystemFunctionsIntern(*spgm->priv, xsink);
      if (!*xsink)
	 runtimeImportSystemConstantsIntern(*spgm->priv, xsink);
   }
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

   const qore_ns_private* vns = 0;
   const QoreClass* c;
   {
      // acquire safe access to parse structures in the source program
      ProgramRuntimeParseAccessHelper rah(xsink, from_pgm.pgm);
      if (*xsink)
	 return;
      c = qore_root_ns_private::runtimeFindClass(*from_pgm.RootNS, path, vns);
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
      qore_root_ns_private::runtimeImportClass(*RootNS, xsink, *tns, c, from_pgm.pgm, nscope.getIdentifier(), inject);
   }
   else {
      tns = vns->root ? RootNS : qore_root_ns_private::runtimeFindCreateNamespacePath(*RootNS, *vns);
      //printd(5, "qore_program_private::importClass() this: %p path: %s nspath: %s tns: %p %s RootNS: %p %s\n", this, path, nspath.c_str(), tns, tns->getName(), RootNS, RootNS->getName());
      qore_root_ns_private::runtimeImportClass(*RootNS, xsink, *tns, c, from_pgm.pgm, new_name, inject);
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
   printd(5, "QoreProgram::del() pgm: %p (base_object: %d)\n", pgm, base_object);

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
   //printd(5, "QoreProgram::~QoreProgram() this: %p deleting root ns %p\n", this, RootNS);
}

QoreClass* qore_program_private::runtimeFindClass(const char* class_name, ExceptionSink* xsink) const {
   // acquire safe access to parse structures in the source program
   ProgramRuntimeParseAccessHelper rah(xsink, pgm);
   if (*xsink)
      return 0;

   return qore_root_ns_private::runtimeFindClass(*RootNS, class_name);
}

QoreProgram::~QoreProgram() {
   printd(5, "QoreProgram::~QoreProgram() this: %p\n", this);
   delete priv;
}

// setup independent program object
QoreProgram::QoreProgram() : priv(new qore_program_private(this, PO_DEFAULT)) {
}

// setup independent program object
QoreProgram::QoreProgram(int64 po) : priv(new qore_program_private(this, po)) {
}

QoreProgram::QoreProgram(QoreProgram* pgm, int64 po, bool ec, const char* ecn) : priv(new qore_program_private(this, po, pgm)) {
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

LocalVar* QoreProgram::createLocalVar(const char* name, const QoreTypeInfo* typeInfo) {
   LocalVar* lv = new LocalVar(name, typeInfo);
   priv->local_var_list.push_back(lv);
   return lv;
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

void QoreProgram::depDeref(ExceptionSink* xsink) {
   priv->depDeref(xsink);
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
      return 0;
   }
   return qore_root_ns_private::runtimeExistsFunction(*priv->RootNS, name) ? true : false;
}

// DEPRECATED
void QoreProgram::parseSetParseOptions(int po) {
   priv->setParseOptions((int64)po);
}

void QoreProgram::parseSetParseOptions(int64 po) {
   priv->setParseOptions(po);
}

void QoreProgram::parseDisableParseOptions(int64 po) {
   priv->disableParseOptions(po);
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
   fc = new FunctionCallNode(qf, const_cast<QoreListNode*>(args), this);

   ProgramThreadCountContextHelper tch(xsink, this, true);
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
   QoreClass* qc = qore_root_ns_private::runtimeFindClass(*priv->RootNS, classname);
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

AbstractQoreNode* qore_parse_get_define_value(const char* str, QoreString& arg, bool& ok) {
   ok = true;
   char c = arg[0];
   // see if a string is being defined
   if (c == '"' || c == '\'') {
      // make sure the string is terminated in the same way
      char e = arg[arg.strlen() - 1];
      if (c != e || arg.strlen() == 1) {
	 parse_error("'%s' is defined with an unterminated string; %%define directives must be made on a single line", str);
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
	    parse_error("'%s' is defined with an invalid number: '%s'", str, arg.getBuffer());
	    ok = false;
	    return 0;
	 }
	 flt = true;
      }
      else if (isalpha(*p)) {
	 parse_error("'%s' has unquoted alphabetic characters in the value; use quotes (\" or ') to define strings", str);
	 ok = false;
	 return 0;
      }
      ++p;
   }

   p = arg.getBuffer();
   if (flt)
      return new QoreFloatNode(atof(p));
   return new QoreBigIntNode(strtoll(p, 0, 10));
}

void QoreProgram::parseDefine(const char* str, AbstractQoreNode* val) {
   priv->parseDefine(qoreCommandLineLocation, str, val);
}

void QoreProgram::parseDefine(const char* str, const char* val) {
   QoreString arg(val);
   arg.trim();

   bool ok;
   AbstractQoreNode* v = qore_parse_get_define_value(str, arg, ok);
   if (!ok)
      return;
   priv->parseDefine(qoreCommandLineLocation, str, v);
}

QoreProgram* QoreProgram::programRefSelf() const {
   const_cast<QoreProgram*>(this)->ref();
   return const_cast<QoreProgram*>(this);
}

int get_warning_code(const char* str) {
   for (unsigned i = 0; i < NUM_WARNINGS; i++)
      if (!strcasecmp(str, qore_warnings[i]))
         return 1 << i;
   return 0;
}
