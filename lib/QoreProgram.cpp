/*
  QoreProgram.cpp

  Program QoreObject Definition

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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

#include <qore/Qore.h>
#include <qore/Restrictions.h>
#include <qore/QoreCounter.h>
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

// note the number and order of the warnings has to correspond to those in QoreProgram.h
static const char *qore_warnings_l[] = { 
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
   "duplicate-local-var-scope",
};
#define NUM_WARNINGS (sizeof(qore_warnings_l)/sizeof(const char *))

//public symbols
const char **qore_warnings = qore_warnings_l;
unsigned qore_num_warnings = NUM_WARNINGS;

void qore_program_private_base::newProgram() {
   base_object = true;
   po_locked = false;
   exec_class = false;

   // init thread local storage key
   thread_local_storage = new qpgm_thread_local_storage_t;

   // save thread local storage hash
   start_thread();

   // copy global feature list to local list
   for (FeatureList::iterator i = qoreFeatureList.begin(), e = qoreFeatureList.end(); i != e; ++i)
      featureList.push_back((*i).c_str());

   // setup namespaces
   RootNS = new RootQoreNamespace(QoreNS, pwo.parse_options);

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

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__ 
   dmap["Windows"] = &True;
#else
   dmap["Unix"] = &True;
#endif

   QoreNamespace *ns = QoreNS->findLocalNamespace("Option");
   assert(ns);
   ConstantListIterator cli(*qore_ns_private::getConstantList(ns));
   while (cli.next()) {
      AbstractQoreNode *v = cli.getValue();
      assert(v);
      // skip boolean options defined as False
      if (v->getType() == NT_BOOLEAN && !reinterpret_cast<QoreBoolNode *>(v)->getValue())
	 continue;

      dmap[cli.getName()] = v->refSelf();
   }

#ifdef DEBUG
   // if Qore library debugging is enabled, then set an option
   dmap["QoreDebug"] = &True;
#endif
}

void qore_program_private_base::setParent(QoreProgram *p_pgm, int64 n_parse_options) {
   //printd(5, "qore_program_private_base::setParent() parent=%p (parent lvl=%p) this=%p (this pgm=%p) parent po: %lld new po: %lld parent no_child_po_restrictions=%d\n", p_pgm, p_pgm->priv->sb.getLVList(), this, pgm, p_pgm->priv->pwo.parse_options, n_parse_options, p_pgm->priv->pwo.parse_options & PO_NO_CHILD_PO_RESTRICTIONS);
      
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
      // grab program's parse lock
      AutoLocker al(p_pgm->priv->plock);
      // setup derived namespaces
      RootNS = p_pgm->priv->RootNS->copy(n_parse_options);
   }
   QoreNS = RootNS->rootGetQoreNamespace();
      
   // copy parent feature list
   p_pgm->priv->featureList.populate(&featureList);
      
   // copy top-level local variables in case any are referenced in static methods in the parent program (static methods are executed in the child's space)
   const LVList *lvl = p_pgm->priv->sb.getLVList();
   if (lvl)
      sb.assignLocalVars(lvl);

   // copy program defines to child program
   for (dmap_t::const_iterator i = p_pgm->priv->dmap.begin(), e = p_pgm->priv->dmap.end(); i != e; ++i)
      dmap[i->first] = i->second ? i->second->refSelf() : 0;
}

void qore_program_private::internParseRollback() {
   // delete pending user functions
   user_func_list.parseRollback();
	 
   // delete pending changes to namespaces
   RootNS->parseRollback();

   // commit global variables
   global_var_list.parseRollback();
	 
   // delete pending statements 
   sb.parseRollback();
}

int qore_program_private::internParseCommit() {
   QORE_TRACE("qore_program_private::internParseCommit()");
   printd(5, "qore_program_private::internParseCommit() pgm=%p isEvent=%d\n", pgm, parseSink->isEvent());

   // if the first stage of parsing has already failed, 
   // then don't go forward
   if (!parseSink->isEvent()) {
      // initialize new statements second (for "our" and "my" declarations)
      // also initializes namespaces, constants, etc
      sb.parseInit(RootNS, &user_func_list);

      // initialize all new global variables (resolve types)
      global_var_list.parseInit(pwo.parse_options);

      printd(5, "QoreProgram::internParseCommit() this=%p RootNS=%p\n", pgm, RootNS);
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
      // merge pending user functions
      user_func_list.parseCommit();
	    
      // merge pending namespace additions
      RootNS->parseCommit();
	    
      // commit global variables
      global_var_list.parseCommit();

      // commit pending statements
      sb.parseCommit();

      rc = 0;
   }
   return rc;
}

void qore_program_private::importUserFunction(QoreProgram *p, UserFunction *u, ExceptionSink *xsink) {
   AutoLocker al(&plock);
   // check if a user function already exists with this name
   if (user_func_list.find(u->getName()))
      xsink->raiseException("FUNCTION-IMPORT-ERROR", "user function '%s' already exists in this program object", u->getName());
   else if (imported_func_list.findNode(u->getName()))
      xsink->raiseException("FUNCTION-IMPORT-ERROR", "function '%s' has already been imported into this program object", u->getName());
   else
      imported_func_list.add(p, u);
}

void qore_program_private::importUserFunction(QoreProgram *p, UserFunction *u, const char *new_name, ExceptionSink *xsink) {
   AutoLocker al(&plock);
   // check if a user function already exists with this name
   if (user_func_list.find(new_name))
      xsink->raiseException("FUNCTION-IMPORT-ERROR", "user function '%s' already exists in this program object", u->getName());
   else if (imported_func_list.findNode(new_name))
      xsink->raiseException("FUNCTION-IMPORT-ERROR", "function '%s' has already been imported into this program object", u->getName());
   else
      imported_func_list.add(p, new_name, u);
}

void qore_program_private::del(ExceptionSink *xsink) {
   printd(5, "QoreProgram::del() pgm=%p (base_object=%d)\n", pgm, base_object);

   if (base_object) {
      deleteThreadData(xsink);

      // delete thread local storage key
      delete thread_local_storage;
      base_object = false;
   }

   // have to delete global variables first because of destructors.
   // method call can be repeated
   global_var_list.delete_all(xsink);

   // delete all class static vars and constants
   RootNS->deleteData(xsink);

   // delete defines
   for (dmap_t::iterator i = dmap.begin(), e = dmap.end(); i != e; ++i)
      discard(i->second, xsink);
   dmap.clear();

   // delete user functions in case there are constant objects which are 
   // instances of classes that may be deleted below (call can be repeated)
   user_func_list.del();

   // method call can be repeated
   sb.del();

   //printd(5, "QoreProgram::~QoreProgram() this=%p deleting root ns %p\n", this, RootNS);

   delete RootNS;
   RootNS = 0;
}

UserFunction *qore_program_private::findUserImportedFunctionUnlocked(const char *name, QoreProgram *&ipgm) {
   UserFunction *u = user_func_list.find(name);
   if (!u)
      u = imported_func_list.find(name, ipgm);
   
   return u;
}

QoreProgram::~QoreProgram() {
   printd(5, "QoreProgram::~QoreProgram() this=%p\n", this);
   delete priv;
}

// setup independent program object
QoreProgram::QoreProgram() : priv(new qore_program_private(this, PO_DEFAULT)) {
}

// setup independent program object
QoreProgram::QoreProgram(int64 po) : priv(new qore_program_private(this, po)) {
}

QoreProgram::QoreProgram(QoreProgram *pgm, int64 po, bool ec, const char *ecn) : priv(new qore_program_private(this, po, pgm)) {
   priv->exec_class = ec;
   if (ecn)
      priv->exec_class_name = ecn;
}

QoreThreadLock *QoreProgram::getParseLock() {
   return &priv->plock;
}

void QoreProgram::deref(ExceptionSink *xsink) {
   printd(5, "QoreProgram::deref() this=%p %d->%d\n", this, reference_count(), reference_count() - 1);
   if (ROdereference())
      priv->clear(xsink);
}

Var *QoreProgram::findGlobalVar(const char *name) {
   return priv->global_var_list.findVar(name);
}

const Var *QoreProgram::findGlobalVar(const char *name) const {
   return priv->global_var_list.findVar(name);
}

Var *QoreProgram::checkGlobalVar(const char *name, const QoreTypeInfo *typeInfo) {
   int new_var = 0;
   Var *rv = priv->global_var_list.checkVar(name, typeInfo, &new_var);
   if (new_var) {
      printd(5, "QoreProgram::checkVar() new global var \"%s\" found\n", name);
      // check if unflagged global vars are allowed
      if (priv->pwo.parse_options & PO_REQUIRE_OUR)
	 parseException("UNDECLARED-GLOBAL-VARIABLE", "global variable '%s' must first be declared with 'our' (conflicts with parse option REQUIRE_OUR)", name);
      // check if new global variables are allowed to be created at all
      else if (priv->pwo.parse_options & PO_NO_GLOBAL_VARS)
	 parseException("ILLEGAL-GLOBAL-VARIABLE", "illegal reference to new global variable '%s' (conflicts with parse option NO_GLOBAL_VARS)", name);
      else if (new_var)
	 makeParseWarning(QP_WARN_UNDECLARED_VAR, "UNDECLARED-GLOBAL-VARIABLE", "global variable '%s' should be declared with 'our'", name);
   }
   return rv;
}

LocalVar *QoreProgram::createLocalVar(const char *name, const QoreTypeInfo *typeInfo) {
   LocalVar *lv = new LocalVar(name, typeInfo);
   priv->local_var_list.push_back(lv);
   return lv;
}

// if this global variable definition is illegal, then
// it will be flagged in the parseCommit stage
Var *QoreProgram::addGlobalVarDef(const char *name, QoreParseTypeInfo *typeInfo) {
   int new_var = 0;
   Var *v = priv->global_var_list.checkVar(name, typeInfo, &new_var);
   if (!new_var)
      makeParseWarning(QP_WARN_DUPLICATE_GLOBAL_VARS, "DUPLICATE-GLOBAL-VARIABLE", "global variable '%s' has already been declared", name);
   else if ((priv->pwo.parse_options & PO_NO_GLOBAL_VARS))
      parse_error("illegal reference to new global variable '%s' (conflicts with parse option NO_GLOBAL_VARS)", name);

   return v;
}

// if this global variable definition is illegal, then
// it will be flagged in the parseCommit stage
Var *QoreProgram::addResolvedGlobalVarDef(const char *name, const QoreTypeInfo *typeInfo) {
   int new_var = 0;
   Var *v = priv->global_var_list.checkVar(name, typeInfo, &new_var);
   if (!new_var)
      makeParseWarning(QP_WARN_DUPLICATE_GLOBAL_VARS, "DUPLICATE-GLOBAL-VARIABLE", "global variable '%s' has already been declared", name);
   else if ((priv->pwo.parse_options & PO_NO_GLOBAL_VARS))
      parse_error("illegal reference to new global variable '%s' (conflicts with parse option NO_GLOBAL_VARS)", name);

   return v;
}

ExceptionSink *QoreProgram::getParseExceptionSink() {
   if (priv->requires_exception)
      return 0;

   return priv->parseSink;
}

void QoreProgram::addParseException(ExceptionSink *xsink) {
   addParseException(*xsink);
   delete xsink;
}

void QoreProgram::addParseException(ExceptionSink &xsink) {
   if (priv->requires_exception) {
      xsink.clear();
      return;
   }

   // ensure that all exceptions reflect the current parse location
   int sline, eline;
   get_parse_location(sline, eline);
   xsink.overrideLocation(sline, eline, get_parse_file());
   priv->parseSink->assimilate(xsink);
}

void QoreProgram::makeParseWarning(int code, const char *warn, const char *fmt, ...) {
   QORE_TRACE("QoreProgram::makeParseWarning()");

   //printd(5, "QP::mPW(code=%d, warn='%s', fmt='%s') priv->pwo.warn_mask=%d priv->warnSink=%p %s\n", code, warn, fmt, priv->pwo.warn_mask, priv->warnSink, priv->warnSink && (code & priv->pwo.warn_mask) ? "OK" : "SKIPPED");
   if (!priv->warnSink || !(code & priv->pwo.warn_mask))
      return;

   QoreStringNode *desc = new QoreStringNode();
   while (true) {
      va_list args;
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
   QoreException *ne = new ParseException(warn, desc);
   priv->warnSink->raiseException(ne);
}

void QoreProgram::makeParseWarning(int sline, int eline, const char *file, int code, const char *warn, const char *fmt, ...) {
   QORE_TRACE("QoreProgram::makeParseWarning()");

   //printd(5, "QP::mPW(code=%d, warn='%s', fmt='%s') priv->pwo.warn_mask=%d priv->warnSink=%p %s\n", code, warn, fmt, priv->pwo.warn_mask, priv->warnSink, priv->warnSink && (code & priv->pwo.warn_mask) ? "OK" : "SKIPPED");
   if (!priv->warnSink || !(code & priv->pwo.warn_mask))
      return;

   QoreStringNode *desc = new QoreStringNode();
   while (true) {
      va_list args;
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
   QoreException *ne = new ParseException(sline, eline, file, warn, desc);
   priv->warnSink->raiseException(ne);
}

void QoreProgram::makeParseWarning(int code, const char *warn, QoreStringNode *desc) {
   QORE_TRACE("QoreProgram::makeParseWarning()");

   //printd(5, "QoreProgram::makeParseWarning(code=%d, warn='%s', desc='%s') priv->pwo.warn_mask=%d priv->warnSink=%p %s\n", code, warn, desc->getBuffer(), priv->pwo.warn_mask, priv->warnSink, priv->warnSink && (code & priv->pwo.warn_mask) ? "OK" : "SKIPPED");
   if (!priv->warnSink || !(code & priv->pwo.warn_mask)) {
      desc->deref();
      return;
   }

   QoreException *ne = new ParseException(warn, desc);
   priv->warnSink->raiseException(ne);
}

void QoreProgram::addParseWarning(int code, ExceptionSink *xsink) {
   if (!priv->warnSink || !(code & priv->pwo.warn_mask))
      return;
   priv->warnSink->assimilate(xsink);
}

void QoreProgram::cannotProvideFeature(QoreStringNode *desc) {
   QORE_TRACE("QoreProgram::cannotProvideFeature()");
   
   if (!priv->requires_exception) {
      priv->parseSink->clear();
      priv->requires_exception = true;
   }

   QoreException *ne = new ParseException("CANNOT-PROVIDE-FEATURE", desc);
   priv->parseSink->raiseException(ne);
}

UserFunction *QoreProgram::findUserFunction(const char *name) {
   AutoLocker al(&priv->plock);
   return priv->user_func_list.find(name);
}

void QoreProgram::exportUserFunction(const char *name, QoreProgram *p, ExceptionSink *xsink) {
   priv->exportUserFunction(name, p->priv, xsink);
}

void QoreProgram::exportUserFunction(const char *name, const char *new_name, QoreProgram *p, ExceptionSink *xsink) {
   priv->exportUserFunction(name, new_name, p->priv, xsink);
}

// called during parsing (priv->plock already grabbed)
void QoreProgram::registerUserFunctionVariant(char *name, UserFunctionVariant *v) {
   // check if an imported function already exists with this name
   if (priv->imported_func_list.findNode(name)) {
      parse_error("function '%s' has already been imported into this program", name);
      free(name);
      return;
   }
   UserFunction *u = priv->user_func_list.find(name);
   if (!u) {
      u = new UserFunction(name);
      u->parseAddVariant(v);
      priv->user_func_list.add(u);
      return;
   }
   free(name);
   u->parseAddVariant(v);
}

void QoreProgram::importGlobalVariable(class Var *var, ExceptionSink *xsink, bool readonly) {
   priv->plock.lock();
   priv->global_var_list.import(var, xsink, readonly);
   priv->plock.unlock();
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

RootQoreNamespace *QoreProgram::getRootNS() const {
   return priv->RootNS; 
}

int QoreProgram::getParseOptions() const { 
   return (int)priv->pwo.parse_options; 
}

int64 QoreProgram::getParseOptions64() const { 
   return priv->pwo.parse_options; 
}

QoreListNode *QoreProgram::getUserFunctionList() {
   AutoLocker al(&priv->plock);
   return priv->user_func_list.getList(); 
}

void QoreProgram::waitForTermination() {
   priv->waitForAllThreadsToTerminate();
}

void QoreProgram::waitForTerminationAndDeref(ExceptionSink *xsink) {
   priv->waitForAllThreadsToTerminate();
   deref(xsink);
}

void QoreProgram::lockOptions() { 
   priv->po_locked = true; 
}

// setExecClass() NOTE: string passed here will copied
void QoreProgram::setExecClass(const char *ecn) {
   priv->exec_class = true;
   if (ecn)
      priv->exec_class_name = ecn;
   else
      priv->exec_class_name.clear();
}

QoreNamespace *QoreProgram::getQoreNS() const {
   return priv->QoreNS;
}

void QoreProgram::depRef() {
   //printd(5, "QoreProgram::depRef() this=%p %d->%d\n", this, priv->dc.reference_count(), priv->dc.reference_count() + 1);
   priv->dc.ROreference();
}

void QoreProgram::depDeref(ExceptionSink *xsink) {
   priv->depDeref(xsink);
}

bool QoreProgram::checkWarning(int code) const {
   return priv->warnSink && (code & priv->pwo.warn_mask); 
}

int QoreProgram::getWarningMask() const { 
   return priv->warnSink ? priv->pwo.warn_mask : 0; 
}

void QoreProgram::addStatement(AbstractStatement *s) {
   priv->sb.addStatement(s);

   // see if top level statements are allowed
   if (priv->pwo.parse_options & PO_NO_TOP_LEVEL_STATEMENTS && !s->isDeclaration()) {
      parse_error("illegal top-level statement (conflicts with parse option NO_TOP_LEVEL_STATEMENTS)");
   }
}

bool QoreProgram::existsFunction(const char *name) {
   // need to grab the parse lock for safe access to the user function map
   AutoLocker al(&priv->plock);
   return priv->user_func_list.find(name);
}

// DEPRECATED
void QoreProgram::parseSetParseOptions(int po) {
   priv->setParseOptions((int64)po);
}

void QoreProgram::parseSetParseOptions(int64 po) {
   priv->setParseOptions(po);
}

// DEPRECATED
void QoreProgram::setParseOptions(int po, ExceptionSink *xsink) {
   priv->setParseOptions((int64)po, xsink);
}

void QoreProgram::setParseOptions(int64 po, ExceptionSink *xsink) {
   priv->setParseOptions(po, xsink);
}

// DEPRECATED
void QoreProgram::disableParseOptions(int po, ExceptionSink *xsink) {
   priv->disableParseOptions((int64)po, xsink);
}

void QoreProgram::disableParseOptions(int64 po, ExceptionSink *xsink) {
   priv->disableParseOptions(po, xsink);
}

void QoreProgram::replaceParseOptions(int64 po, ExceptionSink *xsink) {
   priv->replaceParseOptions(po, xsink);
}

void QoreProgram::parsePending(const char *code, const char *label, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
   if (!code || !code[0])
      return;

   ProgramContextHelper pch(this, false);

   priv->parsePending(code, label, xsink, wS, wm);
}

void QoreProgram::startThread() {
   priv->start_thread();
}

QoreHashNode *QoreProgram::getThreadData() {
   return priv->thread_local_storage->get();
}

AbstractQoreNode *QoreProgram::run(ExceptionSink *xsink) {
   if (!priv->exec_class_name.empty()) {
      runClass(priv->exec_class_name.c_str(), xsink);
      return 0;
   }
   return runTopLevel(xsink);
}

QoreHashNode *QoreProgram::clearThreadData(ExceptionSink *xsink) {
   return priv->clearThreadData(xsink);
}

const AbstractQoreFunction *QoreProgram::resolveFunction(const char *fname, QoreProgram *&pgm) {
   QORE_TRACE("QoreProgram::resolveFunction()");

   const AbstractQoreFunction *f;
   if ((f = priv->user_func_list.find(fname))) {
      printd(5, "resolved user function call to %s\n", fname);
      return f;
   }

   if ((f = priv->imported_func_list.find(fname, pgm))) {
      printd(5, "resolved imported function call to %s\n", fname);
      return f;
   }

   if ((f = builtinFunctions.find(fname))) {
      printd(5, "resolved builtin function call to %s\n", fname);
      return f;
   }

   // cannot find function, throw exception
   parse_error("function '%s()' cannot be found", fname);

   return 0;
}

// called during parsing (plock already grabbed)
AbstractCallReferenceNode *QoreProgram::resolveCallReference(UnresolvedProgramCallReferenceNode *fr) {
   std::auto_ptr<UnresolvedProgramCallReferenceNode> fr_holder(fr);
   char *fname = fr->str;

   {   
      UserFunction *ufc;
      if ((ufc = priv->user_func_list.find(fname))) {
	  printd(5, "QoreProgram::resolveCallReference() resolved function reference to user function %s (%p)\n", fname, ufc);
	 return new LocalUserCallReferenceNode(ufc);
      }
   }
   
   {
      ImportedFunctionEntry *ifn;
      if ((ifn = priv->imported_func_list.findNode(fname))) {
	 printd(5, "QoreProgram::resolveCallReference() resolved function reference to imported function %s (pgm=%p, func=%p)\n", fname, ifn->getProgram(), ifn->getFunction());
	 return new UserCallReferenceNode(ifn->getFunction(), ifn->getProgram());
      }
   }
   
   const BuiltinFunction *bfc;
   if ((bfc = builtinFunctions.find(fname))) {
      printd(5, "QoreProgram::resolveCallReference() resolved function reference to builtin function to %s\n", fname);
      
      // check parse options to see if access is allowed
      if (bfc->getUniqueFunctionality() & priv->pwo.parse_options)
	 parse_error("parse options do not allow access to builtin function '%s'", fname);
      else 
	 return new BuiltinCallReferenceNode(bfc);
   }
   else
      // cannot find function, throw exception
      parse_error("reference to function '%s()' cannot be resolved", fname);

   return fr_holder.release();
}

void QoreProgram::parse(FILE *fp, const char *name, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
   priv->parse(fp, name, xsink, wS, wm);
}

void QoreProgram::parse(const QoreString *str, const QoreString *lstr, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
   priv->parse(str, lstr, xsink, wS, wm);
}

void QoreProgram::parse(const char *code, const char *label, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
   priv->parse(code, label, xsink, wS, wm);
}

void QoreProgram::parseFile(const char *filename, ExceptionSink *xsink, ExceptionSink *wS, int wm, bool only_first_except) {
   priv->only_first_except = only_first_except;
   priv->parseFile(filename, xsink, wS, wm);
}

void QoreProgram::parsePending(const QoreString *str, const QoreString *lstr, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
   priv->parsePending(str, lstr, xsink, wS, wm);
}

AbstractQoreNode *QoreProgram::runTopLevel(ExceptionSink *xsink) {
   ProgramThreadCountHelper tch(this);
   ProgramContextHelper pch(this);
   return priv->sb.exec(xsink);
}

AbstractQoreNode *QoreProgram::callFunction(const char *name, const QoreListNode *args, ExceptionSink *xsink) {
   UserFunction *ufc;
   SimpleRefHolder<FunctionCallNode> fc;

   printd(5, "QoreProgram::callFunction() creating function call to %s()\n", name);
   // need to grab parse lock for safe access to the user function map and imported function map
   priv->plock.lock();
   ufc = priv->user_func_list.find(name);
   if (ufc) {
      priv->plock.unlock();
      // we assign the args to 0 below so that they will not be deleted
      fc = new FunctionCallNode(ufc, const_cast<QoreListNode *>(args), this);
   }
   else {
      QoreProgram *ipgm = 0;
      ufc = priv->imported_func_list.find(name, ipgm);
      priv->plock.unlock();
      if (ufc) {
	 // we assign the args to 0 below so that they will not be deleted
	 fc = new FunctionCallNode(ufc, const_cast<QoreListNode *>(args), ipgm);
      }
      else {
	 const BuiltinFunction *bfc;
	 if ((bfc = builtinFunctions.find(name))) {
	    // we assign the args to 0 below so that they will not be deleted
	    fc = new FunctionCallNode(bfc, const_cast<QoreListNode *>(args), this);
	 }
	 else {
	    xsink->raiseException("NO-FUNCTION", "function name '%s' does not exist", name);
	    return 0;
	 }
      }
   }
   
   ProgramThreadCountHelper tch(this);

   AbstractQoreNode *rv = fc->eval(xsink);

   // let caller delete function arguments if necessary
   fc->take_args();

   return rv;
}

/*
AbstractQoreNode *QoreProgram::callFunction(const UserFunction *ufc, const QoreListNode *args, ExceptionSink *xsink) {
   // we assign the args to 0 below so that they will not be deleted
   SimpleRefHolder<FunctionCallNode> fc(new FunctionCallNode(ufc, const_cast<QoreListNode *>(args)), 0);

   AbstractQoreNode *rv;

   ProgramThreadCountHelper tch(this);

   {
      ProgramContextHelper pch(this);
      rv = fc->eval(xsink);
   }
   
   // let caller delete function arguments if necessary
   fc->take_args();

   return rv;
}
*/

void QoreProgram::parseCommit(ExceptionSink *xsink, ExceptionSink *wS, int wm) {
   ProgramContextHelper pch(this, false);
   priv->parseCommit(xsink, wS, wm);
}

// this function cannot throw an exception because as long as the 
// parse lock is held
void QoreProgram::parseRollback() {
   priv->parseRollback();
}

void QoreProgram::runClass(const char *classname, ExceptionSink *xsink) {
   // find class
   QoreClass *qc = priv->RootNS->rootFindClass(classname);
   if (!qc) {
      xsink->raiseException("CLASS-NOT-FOUND", "cannot find any class '%s' in any namespace", classname);
      return;
   }
   //printd(5, "QoreProgram::runClass(%s)\n", classname);

   ProgramThreadCountHelper tch(this);

   {
      ProgramContextHelper pch(this);
      discard(qc->execConstructor(0, xsink), xsink); 
   }   
}

void QoreProgram::parseFileAndRunClass(const char *filename, const char *classname) {
   ExceptionSink xsink;

   parseFile(filename, &xsink);

   if (!xsink.isEvent())
      runClass(classname, &xsink);
}

void QoreProgram::parseAndRunClass(FILE *fp, const char *name, const char *classname) {
   ExceptionSink xsink;

   parse(fp, name, &xsink);

   if (!xsink.isEvent())
      runClass(classname, &xsink);
}

void QoreProgram::parseAndRunClass(const char *str, const char *name, const char *classname) {
   ExceptionSink xsink;

   parse(str, name, &xsink);

   if (!xsink.isEvent())
      runClass(classname, &xsink);
}

void QoreProgram::parseFileAndRun(const char *filename) {
   ExceptionSink xsink;

   parseFile(filename, &xsink);

   if (!xsink.isEvent()) {
      // get class name
      if (priv->exec_class) {
	 if (!priv->exec_class_name.empty())
	    runClass(priv->exec_class_name.c_str(), &xsink);
	 else {
	    char *c, *bn = q_basenameptr(filename);
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

void QoreProgram::parseAndRun(FILE *fp, const char *name) {
   ExceptionSink xsink;
   
   if (priv->exec_class && priv->exec_class_name.empty())
      xsink.raiseException("EXEC-CLASS-ERROR", "class name required if executing from stdin");
   else {
      parse(fp, name, &xsink);

      if (!xsink.isEvent())
	 run(&xsink);
   }
}

void QoreProgram::parseAndRun(const char *str, const char *name) {
   ExceptionSink xsink;

   if (priv->exec_class && priv->exec_class_name.empty())
      xsink.raiseException("EXEC-CLASS-ERROR", "class name required if executing from a direct string");
   else {
      parse(str, name, &xsink);

      if (!xsink.isEvent())
	 run(&xsink);
   }
}

bool QoreProgram::checkFeature(const char *f) const {
   return priv->featureList.find(f);
}

void QoreProgram::addFeature(const char *f) {
   priv->featureList.push_back(f);
}

void QoreProgram::addFile(char *f) {
   priv->fileList.push_back(f);
}

QoreListNode *QoreProgram::getFeatureList() const {
   return priv->getFeatureList();
}

QoreListNode *QoreProgram::getVarList() {
   return priv->getVarList();
}

void QoreProgram::tc_inc() {
   priv->incThreadCount();
}

void QoreProgram::tc_dec() {
   priv->decThreadCount();
}

const char *QoreProgram::parseGetScriptDir() const {
   return priv->parseGetScriptDir();
}

QoreStringNode *QoreProgram::getScriptDir() const {
   return priv->getScriptDir();
}

QoreStringNode *QoreProgram::getScriptPath() const {
   return priv->getScriptPath();
}

QoreStringNode *QoreProgram::getScriptName() const {
   return priv->getScriptName();
}

void QoreProgram::setScriptPath(const char *path) {
   priv->setScriptPathExtern(path);
}

const LVList *QoreProgram::getTopLevelLVList() const {
   return priv->sb.getLVList();
}

AbstractQoreNode *QoreProgram::getGlobalVariableValue(const char *var, bool &found) const {
   const Var *v = findGlobalVar(var);
   if (!v) {
      found = false;
      return 0;
   }
   found = true;
   
   return v->getReferencedValue();
}

// only called when parsing, therefore in the parse thread lock
void QoreProgram::parseSetIncludePath(const char *path) {
   priv->include_path = path;
}

// only called when parsing, therefore in the parse thread lock
const char *QoreProgram::parseGetIncludePath() const {
   return priv->include_path.empty() ? 0 : priv->include_path.c_str();
}

const AbstractQoreZoneInfo *QoreProgram::currentTZ() const {
   return priv->currentTZ();
}

void QoreProgram::setTZ(const AbstractQoreZoneInfo *n_TZ) {
   priv->setTZ(n_TZ);
}

bool QoreProgram::parseExceptionRaised() const {
   return priv->parseExceptionRaised();
}

void QoreProgram::parseSetTimeZone(const char *zone) {
   return priv->parseSetTimeZone(zone);
}

AbstractQoreNode *qore_parse_get_define_value(const char *str, QoreString &arg, bool &ok) {
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

   const char *p = arg.getBuffer();
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

void QoreProgram::parseDefine(const char *str, AbstractQoreNode *val) {
   priv->parseDefine(qoreCommandLineLocation, str, val);
}

void QoreProgram::parseDefine(const char *str, const char *val) {
   QoreString arg(val);
   arg.trim();

   bool ok;
   AbstractQoreNode *v = qore_parse_get_define_value(str, arg, ok);
   if (!ok)
      return;
   priv->parseDefine(qoreCommandLineLocation, str, v);
}

int get_warning_code(const char *str) {
   for (unsigned i = 0; i < NUM_WARNINGS; i++)
      if (!strcasecmp(str, qore_warnings[i]))
         return 1 << i;
   return 0;
}

DLLLOCAL void addProgramConstants(QoreNamespace *ns) {
   ns->addConstant("PO_DEFAULT",                  new QoreBigIntNode(PO_DEFAULT));
   ns->addConstant("PO_NO_GLOBAL_VARS",           new QoreBigIntNode(PO_NO_GLOBAL_VARS));
   ns->addConstant("PO_NO_SUBROUTINE_DEFS",       new QoreBigIntNode(PO_NO_SUBROUTINE_DEFS));  
   ns->addConstant("PO_NO_THREAD_CONTROL",        new QoreBigIntNode(PO_NO_THREAD_CONTROL));
   ns->addConstant("PO_NO_THREAD_CLASSES",        new QoreBigIntNode(PO_NO_THREAD_CLASSES));
   ns->addConstant("PO_NO_THREADS",               new QoreBigIntNode(PO_NO_THREADS));
   ns->addConstant("PO_NO_TOP_LEVEL_STATEMENTS",  new QoreBigIntNode(PO_NO_TOP_LEVEL_STATEMENTS));  
   ns->addConstant("PO_NO_CLASS_DEFS",            new QoreBigIntNode(PO_NO_CLASS_DEFS));
   ns->addConstant("PO_NO_NAMESPACE_DEFS",        new QoreBigIntNode(PO_NO_NAMESPACE_DEFS));
   ns->addConstant("PO_NO_CONSTANT_DEFS",         new QoreBigIntNode(PO_NO_CONSTANT_DEFS));
   ns->addConstant("PO_NO_NEW",                   new QoreBigIntNode(PO_NO_NEW));
   ns->addConstant("PO_NO_SYSTEM_CLASSES",        new QoreBigIntNode(PO_NO_SYSTEM_CLASSES));
   ns->addConstant("PO_NO_USER_CLASSES",          new QoreBigIntNode(PO_NO_USER_CLASSES));
   ns->addConstant("PO_NO_CHILD_PO_RESTRICTIONS", new QoreBigIntNode(PO_NO_CHILD_PO_RESTRICTIONS));
   ns->addConstant("PO_NO_EXTERNAL_PROCESS",      new QoreBigIntNode(PO_NO_EXTERNAL_PROCESS));
   ns->addConstant("PO_REQUIRE_OUR",              new QoreBigIntNode(PO_REQUIRE_OUR));
   ns->addConstant("PO_NO_PROCESS_CONTROL",       new QoreBigIntNode(PO_NO_PROCESS_CONTROL));
   ns->addConstant("PO_NO_NETWORK",               new QoreBigIntNode(PO_NO_NETWORK));
   ns->addConstant("PO_NO_FILESYSTEM",            new QoreBigIntNode(PO_NO_FILESYSTEM));
   ns->addConstant("PO_LOCK_WARNINGS",            new QoreBigIntNode(PO_LOCK_WARNINGS));
   ns->addConstant("PO_NO_GUI",                   new QoreBigIntNode(PO_NO_GUI));
   ns->addConstant("PO_REQUIRE_TYPES",            new QoreBigIntNode(PO_REQUIRE_TYPES));
   ns->addConstant("PO_NO_EXTERNAL_INFO",         new QoreBigIntNode(PO_NO_EXTERNAL_INFO));
   ns->addConstant("PO_REQUIRE_PROTOTYPES",       new QoreBigIntNode(PO_REQUIRE_PROTOTYPES));
   ns->addConstant("PO_STRICT_ARGS",              new QoreBigIntNode(PO_STRICT_ARGS));
   ns->addConstant("PO_ALLOW_BARE_REFS",          new QoreBigIntNode(PO_ALLOW_BARE_REFS));
   ns->addConstant("PO_ASSUME_LOCAL",             new QoreBigIntNode(PO_ASSUME_LOCAL));
   ns->addConstant("PO_NEW_STYLE",                new QoreBigIntNode(PO_NEW_STYLE));
}
