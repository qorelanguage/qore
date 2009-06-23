/*
  QoreProgram.cc

  Program QoreObject Definition

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

#include <qore/Qore.h>
#include <qore/intern/ParserSupport.h>
#include <qore/Restrictions.h>
#include <qore/QoreCounter.h>
#include <qore/intern/UserFunctionList.h>
#include <qore/intern/GlobalVariableList.h>
#include <qore/intern/ImportedFunctionList.h>
#include <qore/intern/LocalVar.h>

#include <errno.h>

#include <string>
#include <set>
#include <typeinfo>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>

extern QoreListNode *ARGV, *QORE_ARGV;
extern QoreHashNode *ENV;

class CharPtrList : public safe_dslist<const char *> {
   public:
      // returns 0 for found, -1 for not found
      // FIXME: use STL find algorithm
      DLLLOCAL int find(const char *str) const
      {
	 const_iterator i = begin();
	 while (i != end())
	 {
	    if (!strcmp(*i, str))
	       return 0;
	    i++;
	 }
   
	 return -1;
      }
};

class SBNode {
   public:
      StatementBlock *statements;
      SBNode *next;
   
      DLLLOCAL SBNode() { next = 0; statements = 0; }
      DLLLOCAL ~SBNode();
      DLLLOCAL void reset();
};

// local variable container
typedef safe_dslist<LocalVar *> local_var_list_t;

class LocalVariableList : public local_var_list_t
{
   public:
      DLLLOCAL LocalVariableList()
      {
      }

      DLLLOCAL ~LocalVariableList()
      {
	 for (local_var_list_t::iterator i = begin(), e = end(); i != e; ++i)
	    delete *i;
      }
};

typedef QoreThreadLocalStorage<QoreHashNode> qpgm_thread_local_storage_t;

struct qore_program_private {
      UserFunctionList user_func_list;
      ImportedFunctionList imported_func_list;
      GlobalVariableList global_var_list;
      LocalVariableList local_var_list;

      // for the thread counter
      QoreCounter tcount;
      // to save file names for later deleting
      cstr_vector_t fileList;
      // features present in this Program object
      CharPtrList featureList;
      
      // parse lock, making parsing actions atomic and thread-safe
      mutable QoreThreadLock plock;
      // depedency counter, when this hits zero, the object is deleted
      QoreReferenceCounter dc;
      SBNode *sb_head, *sb_tail;
      ExceptionSink *parseSink, *warnSink;
      RootQoreNamespace *RootNS;
      QoreNamespace *QoreNS;

      int parse_options;
      int warn_mask;
      std::string exec_class_name, script_dir, script_path, script_name,
	  include_path;
      bool po_locked, exec_class, base_object, requires_exception;

      qpgm_thread_local_storage_t *thread_local_storage;

      DLLLOCAL qore_program_private() {
	 printd(5, "QoreProgram::QoreProgram() (init()) this=%08p\n", this);
#ifdef DEBUG
	 parseSink = 0;
#endif
	 warnSink = 0;
	 requires_exception = false;
	 sb_head = sb_tail = 0;
	 nextSB();
	 
	 // initialize global vars
	 Var *var = global_var_list.newVar("ARGV");
	 if (ARGV)
	    var->setValue(ARGV->copy(), 0);
	 
	 var = global_var_list.newVar("QORE_ARGV");
	 if (QORE_ARGV)
	    var->setValue(QORE_ARGV->copy(), 0);
	 
	 var = global_var_list.newVar("ENV");
	 var->setValue(ENV->copy(), 0);
      }

      DLLLOCAL ~qore_program_private()
      {
      }

      DLLLOCAL const char *parseGetScriptDir() const {
	 return script_dir.empty() ? 0 : script_dir.c_str();
      }

      DLLLOCAL QoreStringNode *getScriptPath() const {
	 // grab program-level parse lock
	 AutoLocker al(&plock);
	 return script_path.empty() ? 0 : new QoreStringNode(script_path);
      }

      DLLLOCAL QoreStringNode *getScriptDir() const {
	 // grab program-level parse lock
	 AutoLocker al(&plock);
	 return script_dir.empty() ? 0 : new QoreStringNode(script_dir);
      }

      DLLLOCAL QoreStringNode *getScriptName() const {
	 // grab program-level parse lock
	 AutoLocker al(&plock);
	 return script_name.empty() ? 0 : new QoreStringNode(script_name);
      }

      DLLLOCAL void setScriptPathExtern(const char *path) {
	 // grab program-level parse lock
	 AutoLocker al(&plock);
	 setScriptPath(path);
      }

      DLLLOCAL void setScriptPath(const char *path) {
	 if (!path) {
	    script_dir.clear();
	    script_path.clear();
	    script_name.clear();
	 }
	 else {
	    // find file name
	    const char *p = q_basenameptr(path);
	    if (p == path) {
	       script_name = path;
	       script_dir = "./";
	       script_path = script_dir + script_name;
	    }
	    else {
	       script_path = path;
	       script_name = p;
	       script_dir.assign(path, p - path);
	    }
	 }
      }

      DLLLOCAL QoreListNode *getVarList()
      {
	 plock.lock();
	 QoreListNode *l = global_var_list.getVarList();
	 plock.unlock();
	 return l;
      }

      DLLLOCAL void deleteSBList() {
	 SBNode *w = sb_head;
	 while (w) {
	    w = w->next;
	    delete sb_head;
	    sb_head = w;
	 }
      }

      DLLLOCAL void nextSB() {
	 if (sb_tail && !sb_tail->statements)
	    return;
	 SBNode *sbn = new SBNode();
	 if (!sb_tail)
	    sb_head = sbn;
	 else
	    sb_tail->next = sbn;
	 sb_tail = sbn;
      }

      DLLLOCAL QoreListNode *getFeatureList() const {
	 QoreListNode *l = new QoreListNode();
	 
	 for (CharPtrList::const_iterator i = featureList.begin(), e = featureList.end(); i != e; ++i)
	    l->push(new QoreStringNode(*i));
	 
	 return l;
      }

      void internParseRollback() {
	 // delete pending user functions
	 user_func_list.parseRollback();
	 
	 // delete pending changes to namespaces
	 RootNS->parseRollback();
	 
	 // delete pending statements 
	 sb_tail->reset();
      }

      // call must push the current program on the stack and pop it afterwards
      int internParsePending(const char *code, const char *label) {
	 printd(5, "QoreProgram::internParsePending(code=%08p, label=%s)\n", code, label);
	 
	 if (!(*code))
	    return 0;

	 // save this file name for storage in the parse tree and deletion
	 // when the QoreProgram object is deleted
	 char *sname = strdup(label);
	 fileList.push_back(sname);
	 beginParsing(sname);

	 // no need to save buffer, because it's deleted automatically in lexer

	 printd(5, "QoreProgram::internParsePending() parsing tag=%s (%08p): '%s'\n", label, label, code);

	 yyscan_t lexer;
	 yylex_init(&lexer);
	 yy_scan_string(code, lexer);
	 yyset_lineno(1, lexer);
	 // yyparse() will call endParsing() and restore old pgm position
	 yyparse(lexer);

	 printd(5, "QoreProgram::internParsePending() returned from yyparse()\n");
	 int rc = 0;
	 if (parseSink->isException()) {
	    rc = -1;
	    printd(5, "QoreProgram::internParsePending() parse exception: calling parseRollback()\n");
	    internParseRollback();
	    requires_exception = false;
	 }

	 printd(5, "QoreProgram::internParsePending() about to call yylex_destroy()\n");
	 yylex_destroy(lexer);
	 printd(5, "QoreProgram::internParsePending() returned from yylex_destroy()\n");
	 return rc;
      }

      int parsePending(const char *code, const char *label, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
	 // grab program-level parse lock
	 AutoLocker al(&plock);
	 warnSink = wS;
	 warn_mask = wm;

	 parseSink = xsink;
	 int rc = internParsePending(code, label);
	 warnSink = 0;
#ifdef DEBUG
	 parseSink = 0;
#endif
	 return rc;
      }

      // caller must have grabbed the lock and put the current program on the program stack
      int internParseCommit()
      {
	 QORE_TRACE("QoreProgram::internParseCommit()");
	 printd(5, "QoreProgram::internParseCommit() this=%08p isEvent=%d\n", this, parseSink->isEvent());

	 // if the first stage of parsing has already failed, 
	 // then don't go forward
	 if (!parseSink->isEvent()) {
	    // initialize constants first
	    RootNS->parseInitConstants();
	    
	    // initialize new statements second (for "our" and "my" declarations)
	    // note: sb_tail->statements may be 0
	    sb_tail->statements->parseInitTopLevel(RootNS, &user_func_list, sb_head == sb_tail);

	    printd(5, "QoreProgram::internParseCommit() this=%08p priv->RootNS=%08p\n", this, RootNS);
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
	    
	    // commit pending statements
	    nextSB();

	    rc = 0;
	 }
	 return rc;
      }

      int parseCommit(ExceptionSink *xsink, ExceptionSink *wS, int wm) {
	 // grab program-level parse lock
	 AutoLocker al(&plock);
	 warnSink = wS;
	 warn_mask = wm;
	 parseSink = xsink;

	 // finalize parsing, back out or commit all changes
	 int rc = internParseCommit();

#ifdef DEBUG
	 parseSink = 0;
#endif   
	 warnSink = 0;
	 // release program-level parse lock
	 return rc;
      }
};

// note the number and order of the warnings has to correspond to those in QoreProgram.h
static const char *qore_warnings_l[] = { 
   "warning-mask-unchanged",
   "duplicate-local-vars",
   "unknown-warning",
   "undeclared-var",
   "duplicate-global-vars",
   "unreachable-code"
};
#define NUM_WARNINGS (sizeof(qore_warnings_l)/sizeof(const char *))

//public symbols
const char **qore_warnings = qore_warnings_l;
unsigned qore_num_warnings = NUM_WARNINGS;

int get_warning_code(const char *str)
{
   for (unsigned i = 0; i < NUM_WARNINGS; i++)
      if (!strcasecmp(str, qore_warnings[i]))
         return 1 << i;
   return 0;
}

DLLLOCAL void addProgramConstants(class QoreNamespace *ns)
{
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
}

inline SBNode::~SBNode()
{
   reset();
}

inline void SBNode::reset()
{
   delete statements;
   statements = 0;
}

QoreProgram::~QoreProgram()
{
   printd(5, "QoreProgram::~QoreProgram() this=%08p\n", this);
   delete priv;
}

// setup first program object
QoreProgram::QoreProgram() : priv(new qore_program_private) {
   priv->base_object = true;
   priv->parse_options = PO_DEFAULT;
   priv->po_locked = false;
   priv->exec_class = false;

   // init thread local storage key
   priv->thread_local_storage = new qpgm_thread_local_storage_t;

   // save thread local storage hash
   startThread();

   // copy global feature list to local list
   for (FeatureList::iterator i = qoreFeatureList.begin(), e = qoreFeatureList.end(); i != e; ++i)
      priv->featureList.push_back((*i).c_str());

   // setup namespaces
   priv->RootNS = new RootQoreNamespace(&priv->QoreNS);
}

QoreProgram::QoreProgram(QoreProgram *pgm, int po, bool ec, const char *ecn) : priv(new qore_program_private)
{
   // flag as derived object
   priv->base_object = false;

   // set parse options for child object
   priv->parse_options = po;
   // if children inherit restrictions, then set all child restrictions
   if (!(pgm->priv->parse_options & PO_NO_CHILD_PO_RESTRICTIONS)) {
      // lock child parse options
      priv->po_locked = true;
      // turn on all restrictions in the child that are set in the parent
      priv->parse_options |= pgm->priv->parse_options;
      // make sure all options that give more freedom and are off in the parent program are turned off in the child
      priv->parse_options &= (pgm->priv->parse_options | ~PO_POSITIVE_OPTIONS);
   }
   else
      priv->po_locked = false;

   priv->exec_class = ec;
   if (ecn)
      priv->exec_class_name = ecn;

   // inherit parent's thread local storage key
   priv->thread_local_storage = pgm->priv->thread_local_storage;

   // setup derived namespaces
   priv->RootNS = pgm->priv->RootNS->copy(po);
   priv->QoreNS = priv->RootNS->rootGetQoreNamespace();

   // copy parent feature list
   pgm->priv->featureList.populate(&priv->featureList);
}

class QoreThreadLock *QoreProgram::getParseLock()
{
   return &priv->plock;
}

void QoreProgram::deref(ExceptionSink *xsink)
{
   //printd(5, "QoreProgram::deref() this=%08p %d->%d\n", this, reference_count(), reference_count() - 1);
   if (ROdereference())
   {
      // delete all global variables
      priv->global_var_list.clear_all(xsink);
      // clear thread data if base object
      if (priv->base_object)
	 clearThreadData(xsink);
      depDeref(xsink);
   }
}

void QoreProgram::del(ExceptionSink *xsink) {
   printd(5, "QoreProgram::del() this=%08p (priv->base_object=%d)\n", this, priv->base_object);
   // wait for all threads to terminate
   priv->tcount.waitForZero();

   // have to delete global variables first because of destructors.
   // method call can be repeated
   priv->global_var_list.delete_all(xsink);

   // delete user functions in case there are constant objects which are 
   // instances of classes that may be deleted below (call can be repeated)
   priv->user_func_list.del();

   // method call can be repeated
   priv->deleteSBList();

   if (priv->base_object) {
      endThread(xsink);

      // delete thread local storage key
      delete priv->thread_local_storage;
      priv->base_object = false;
   }

   //printd(5, "QoreProgram::~QoreProgram() this=%p deleting root ns %p\n", this, priv->RootNS);

   delete priv->RootNS;
   priv->RootNS = 0;
}

Var *QoreProgram::findGlobalVar(const char *name) {
   return priv->global_var_list.findVar(name);
}

const Var *QoreProgram::findGlobalVar(const char *name) const {
   return priv->global_var_list.findVar(name);
}

class Var *QoreProgram::checkGlobalVar(const char *name)
{
   int new_var = 0;
   class Var *rv = priv->global_var_list.checkVar(name, &new_var);
   if (new_var)
   {
      printd(5, "QoreProgram::checkVar() new global var \"%s\" found\n", name);
      // check if unflagged global vars are allowed
      if (priv->parse_options & PO_REQUIRE_OUR)
	 parseException("UNDECLARED-GLOBAL-VARIABLE", "global variable '%s' must first be declared with 'our' (conflicts with parse option REQUIRE_OUR)", name);
      // check if new global variables are allowed to be created at all
      else if (priv->parse_options & PO_NO_GLOBAL_VARS)
	 parseException("ILLEGAL-GLOBAL-VARIABLE", "illegal reference to new global variable '%s' (conflicts with parse option NO_GLOBAL_VARS)", name);
      else if (new_var)
	 makeParseWarning(QP_WARN_UNDECLARED_VAR, "UNDECLARED-GLOBAL-VARIABLE", "global variable '%s' should be declared with 'our'", name);
   }
   return rv;
}

Var *QoreProgram::createGlobalVar(const char *name)
{
   int new_var = 0;
   class Var *rv = priv->global_var_list.checkVar(name, &new_var);
   // it's a new global variable: check if global variables are allowed
   if ((priv->parse_options & PO_NO_GLOBAL_VARS) && new_var)
      parse_error("illegal reference to new global variable '%s' (conflicts with parse option NO_GLOBAL_VARS)", name);

   printd(5, "QoreProgram::createVar() global var '%s' processed, new_var=%d (val=%08p)\n", name, new_var, rv);

   return rv;
}

LocalVar *QoreProgram::createLocalVar(const char *name)
{
   LocalVar *lv = new LocalVar(name);
   priv->local_var_list.push_back(lv);
   return lv;
}

// if this global variable definition is illegal, then
// it will be flagged in the parseCommit stage
void QoreProgram::addGlobalVarDef(const char *name)
{
   int new_var = 0;
   priv->global_var_list.checkVar(name, &new_var);
   if (!new_var)
      makeParseWarning(QP_WARN_DUPLICATE_GLOBAL_VARS, "DUPLICATE-GLOBAL-VARIABLE", "global variable '%s' has already been declared", name);
}

void QoreProgram::makeParseException(const char *err, QoreStringNode *desc) {
   QORE_TRACE("QoreProgram::makeParseException()");

   QoreStringNodeHolder d(desc);
   if (!priv->requires_exception) {
      class QoreException *ne = new ParseException(err, d.release());
      priv->parseSink->raiseException(ne);
   }
}

void QoreProgram::makeParseException(QoreStringNode *desc) {
   QORE_TRACE("QoreProgram::makeParseException()");

   QoreStringNodeHolder d(desc);
   if (!priv->requires_exception) {
      class QoreException *ne = new ParseException("PARSE-EXCEPTION", d.release());
      priv->parseSink->raiseException(ne);
   }
}

void QoreProgram::makeParseException(int sline, int eline, QoreStringNode *desc) {
   QORE_TRACE("QoreProgram::makeParseException()");

   QoreStringNodeHolder d(desc);
   if (!priv->requires_exception) {
      class QoreException *ne = new ParseException(sline, eline, "PARSE-EXCEPTION", d.release());
      priv->parseSink->raiseException(ne);
   }
}

void QoreProgram::addParseException(ExceptionSink *xsink) {
   if (priv->requires_exception) {
      delete xsink;
      return;
   }

   // ensure that all exceptions reflect the current parse location
   int sline, eline;
   get_parse_location(sline, eline);
   xsink->overrideLocation(sline, eline, get_parse_file());
   priv->parseSink->assimilate(xsink);
}

void QoreProgram::makeParseWarning(int code, const char *warn, const char *fmt, ...) {
   QORE_TRACE("QoreProgram::makeParseWarning()");

   //printd(5, "QP::mPW(code=%d, warn='%s', fmt='%s') priv->warn_mask=%d priv->warnSink=%08p %s\n", code, warn, fmt, priv->warn_mask, priv->warnSink, priv->warnSink && (code & priv->warn_mask) ? "OK" : "SKIPPED");
   if (!priv->warnSink || !(code & priv->warn_mask))
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

void QoreProgram::addParseWarning(int code, ExceptionSink *xsink) {
   if (!priv->warnSink || !(code & priv->warn_mask))
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
   if (this == p)
      xsink->raiseException("PROGRAM-IMPORTFUNCTION-PARAMETER-ERROR", "cannot import a function from the same Program object");
   else {
      UserFunction *u;
      priv->plock.lock();
      u = priv->user_func_list.find(name);
      if (!u)
	 u = priv->imported_func_list.find(name);
      priv->plock.unlock();
      if (!u)
	 xsink->raiseException("PROGRAM-IMPORTFUNCTION-NO-FUNCTION", "function \"%s\" does not exist in the current program scope", name);
      else
	 p->importUserFunction(this, u, xsink);
   }
}

// called during parsing (priv->plock already grabbed)
void QoreProgram::registerUserFunction(UserFunction *u) {
   // check if an imported function already exists with this name
   if (priv->imported_func_list.findNode(u->getName()))
      parse_error("function \"%s\" has already been imported into this program", u->getName());
   else
      priv->user_func_list.add(u);
}

void QoreProgram::importGlobalVariable(class Var *var, ExceptionSink *xsink, bool readonly) {
   priv->plock.lock();
   priv->global_var_list.import(var, xsink, readonly);
   priv->plock.unlock();
}

int QoreProgram::setWarningMask(int wm) {
   if (!(priv->parse_options & PO_LOCK_WARNINGS)) { 
      priv->warn_mask = wm; 
      return 0;
   }
   return -1;
}

// returns 0 for success, -1 for error
int QoreProgram::enableWarning(int code) { 
   if (!(priv->parse_options & PO_LOCK_WARNINGS)) {
      priv->warn_mask |= code; 
      return 0; 
   } 
   return -1; 
}

// returns 0 for success, -1 for error
int QoreProgram::disableWarning(int code) { 
   if (!(priv->parse_options & PO_LOCK_WARNINGS)) {
      priv->warn_mask &= ~code; 
      return 0; 
   } 
   return -1; 
}

RootQoreNamespace *QoreProgram::getRootNS() const {
   return priv->RootNS; 
}

int QoreProgram::getParseOptions() const { 
   return priv->parse_options; 
}

QoreListNode *QoreProgram::getUserFunctionList() {
   AutoLocker al(&priv->plock);
   return priv->user_func_list.getList(); 
}

void QoreProgram::waitForTermination() {
   priv->tcount.waitForZero();
}

void QoreProgram::waitForTerminationAndDeref(ExceptionSink *xsink) {
   priv->tcount.waitForZero();
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
   //printd(5, "QoreProgram::depRef() this=%08p %d->%d\n", this, priv->dc.reference_count(), priv->dc.reference_count() + 1);
   priv->dc.ROreference();
}

void QoreProgram::depDeref(ExceptionSink *xsink) {
   //printd(5, "QoreProgram::depDeref() this=%08p %d->%d\n", this, priv->dc.reference_count(), priv->dc.reference_count() - 1);
   if (priv->dc.ROdereference()) {
      del(xsink);
      delete this;
   }
}

bool QoreProgram::checkWarning(int code) const {
   return priv->warnSink && (code & priv->warn_mask); 
}

int QoreProgram::getWarningMask() const { 
   return priv->warnSink ? priv->warn_mask : 0; 
}

void QoreProgram::addStatement(class AbstractStatement *s) {
   if (!priv->sb_tail->statements) {
      if (typeid(s) != typeid(StatementBlock))
	 priv->sb_tail->statements = new StatementBlock(s);
      else
	 priv->sb_tail->statements = dynamic_cast<StatementBlock *>(s);
   }
   else
      priv->sb_tail->statements->addStatement(s);

   // see if top level statements are allowed
   if (priv->parse_options & PO_NO_TOP_LEVEL_STATEMENTS && !s->isDeclaration()) {
      parse_error("illegal top-level statement (conflicts with parse option NO_TOP_LEVEL_STATEMENTS)");
   }
}

bool QoreProgram::existsFunction(const char *name)
{
   // need to grab the parse lock for safe access to the user function map
   AutoLocker al(&priv->plock);
   return priv->user_func_list.find(name);
}

void QoreProgram::parseSetParseOptions(int po) {
   if (priv->po_locked) {
      parse_error("parse options have been locked on this program object");
      return;
   }
   priv->parse_options |= po;
}

void QoreProgram::setParseOptions(int po, ExceptionSink *xsink) {
   if (priv->po_locked) {
      xsink->raiseException("OPTIONS-LOCKED", "parse options have been locked on this program object");
      return;
   }
   priv->parse_options |= po;
}

void QoreProgram::disableParseOptions(int po, ExceptionSink *xsink) {
   if (priv->po_locked) {
      xsink->raiseException("OPTIONS-LOCKED", "parse options have been locked on this program object");
      return;
   }
   priv->parse_options &= ~po;
}

void QoreProgram::parsePending(const char *code, const char *label, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
   if (!code || !code[0])
      return;

   ProgramContextHelper pch(this, xsink);

   priv->parsePending(code, label, xsink, wS, wm);
}

void QoreProgram::startThread() {
   priv->thread_local_storage->set(new QoreHashNode());
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
   QoreHashNode *h = priv->thread_local_storage->get();
   printd(5, "QoreProgram::clearThreadData() this=%08p h=%08p (size=%d)\n", this, h, h->size());
   h->clear(xsink);
   return h;
}

void QoreProgram::endThread(ExceptionSink *xsink) {
   // delete thread local storage data
   QoreHashNode *h = clearThreadData(xsink);
   h->deref(xsink);
}

// called during parsing (priv->plock already grabbed)
void QoreProgram::resolveFunction(FunctionCallNode *f) {
   QORE_TRACE("QoreProgram::resolveFunction()");
   char *fname = f->f.c_str;

   class UserFunction *ufc;
   if ((ufc = priv->user_func_list.find(fname)))
   {
      printd(5, "resolved user function call to %s\n", fname);
      f->ftype = FC_USER;
      f->f.ufunc = ufc;
      free(fname);

      return;
   }

   class ImportedFunctionNode *ifn;
   if ((ifn = priv->imported_func_list.findNode(fname)))
   {
      printd(5, "resolved imported function call to %s (pgm=%08p, func=%08p)\n", fname, ifn->pgm, ifn->func);
      f->ftype = FC_IMPORTED;
      f->f.ifunc = new ImportedFunctionCall(ifn->pgm, ifn->func);
      free(fname);

      return;
   }

   const BuiltinFunction *bfc;
   if ((bfc = builtinFunctions.find(fname)))
   {
      printd(5, "resolved builtin function call to %s\n", fname);
      f->ftype = FC_BUILTIN;
      f->f.bfunc = bfc;

      // check parse options to see if access is allowed
      if (bfc->getType() & priv->parse_options)
	 parse_error("parse options do not allow access to builtin function '%s'", fname);

      free(fname);

      return;
   }

   // cannot find function, throw exception
   parse_error("function '%s()' cannot be found", fname);

}

// called during parsing (plock already grabbed)
AbstractCallReferenceNode *QoreProgram::resolveCallReference(UnresolvedCallReferenceNode *fr) {
   SimpleRefHolder<UnresolvedCallReferenceNode> fr_holder(fr);
   char *fname = fr->str;

   {   
      UserFunction *ufc;
      if ((ufc = priv->user_func_list.find(fname))) {
	  printd(5, "QoreProgram::resolveCallReference() resolved function reference to user function %s (%p)\n", fname, ufc);
	 return new StaticUserCallReferenceNode(ufc, this);
      }
   }
   
   {
      ImportedFunctionNode *ifn;
      if ((ifn = priv->imported_func_list.findNode(fname))) {
	 printd(5, "QoreProgram::resolveCallReference() resolved function reference to imported function %s (pgm=%08p, func=%08p)\n", fname, ifn->pgm, ifn->func);
	 return new ImportedCallReferenceNode(new ImportedFunctionCall(ifn->pgm, ifn->func));
      }
   }
   
   const BuiltinFunction *bfc;
   if ((bfc = builtinFunctions.find(fname))) {
      printd(5, "QoreProgram::resolveCallReference() resolved function reference to builtin function to %s\n", fname);
      
      // check parse options to see if access is allowed
      if (bfc->getType() & priv->parse_options)
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
   printd(5, "QoreProgram::parse(fp=%08p, name=%s, xsink=%08p, wS=%08p, wm=%d)\n", fp, name, xsink, wS, wm);

   // if already at the end of file, then return
   // try to get one character from file
   int c = fgetc(fp);
   if (feof(fp)) {
      printd(5, "QoreProgram::parse(fp=%08p, name=%s) EOF\n", fp, name);
      return;
   }
   // push back read character
   ungetc(c, fp);

   // grab program-level parse lock
   priv->plock.lock();
   priv->warnSink = wS;
   priv->warn_mask = wm;
   priv->parseSink = xsink;

   // save this file name for storage in the parse tree and deletion
   // when the QoreProgram object is deleted
   char *sname = strdup(name);
   priv->fileList.push_back(sname);
   beginParsing(sname);

   ProgramContextHelper pch(this, xsink);
   printd(2, "QoreProgram::parse(): about to call yyparse()\n");
   yyscan_t lexer;
   yylex_init(&lexer);
   yyset_in(fp, lexer);
   // yyparse() will call endParsing() and restore old pgm position
   yyparse(lexer);

   // finalize parsing, back out or commit all changes
   priv->internParseCommit();

#ifdef DEBUG
   priv->parseSink = 0;
#endif
   priv->warnSink = 0;
   // release program-level parse lock
   priv->plock.unlock();

   yylex_destroy(lexer);
}

void QoreProgram::parse(const QoreString *str, const QoreString *lstr, ExceptionSink *xsink, ExceptionSink *wS, int wm)
{
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

   parse(tstr->getBuffer(), tlstr->getBuffer(), xsink, wS, wm);
}

void QoreProgram::parse(const char *code, const char *label, ExceptionSink *xsink, ExceptionSink *wS, int wm)
{
   if (!(*code))
      return;

   ProgramContextHelper pch(this, xsink);
   
   // grab program-level parse lock
   priv->plock.lock();
   priv->warnSink = wS;
   priv->warn_mask = wm;
   priv->parseSink = xsink;

   // parse text given
   if (!priv->internParsePending(code, label))
      priv->internParseCommit();   // finalize parsing, back out or commit all changes

#ifdef DEBUG
   priv->parseSink = 0;
#endif
   priv->warnSink = 0;
   // release program-level parse lock
   priv->plock.unlock();
}

void QoreProgram::parseFile(const char *filename, ExceptionSink *xsink, ExceptionSink *wS, int wm)
{
   QORE_TRACE("QoreProgram::parseFile()");

   printd(5, "QoreProgram::parseFile(%s)\n", filename);

   FILE *fp;
   if (!(fp = fopen(filename, "r"))) {
      xsink->raiseException("PARSE-EXCEPTION", "cannot open qore script '%s': %s", filename, strerror(errno));
      return;
   }
   priv->setScriptPath(filename);

   ON_BLOCK_EXIT(fclose, fp);

   parse(fp, filename, xsink, wS, wm);
}

void QoreProgram::parsePending(const QoreString *str, const QoreString *lstr, ExceptionSink *xsink, ExceptionSink *wS, int wm)
{
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

   ProgramContextHelper pch(this, xsink);

   priv->parsePending(tstr->getBuffer(), tlstr->getBuffer(), xsink, wS, wm);
}

AbstractQoreNode *QoreProgram::runTopLevel(ExceptionSink *xsink) {
   priv->tcount.inc();

   AbstractQoreNode *rv = 0;
   SBNode *w = priv->sb_head;

   {
      ProgramContextHelper pch(this, xsink);
      while (w && !xsink->isEvent() && !rv) {
	 if (w->statements)
	    rv = w->statements->exec(xsink);
	 else
	    rv = 0;
	 w = w->next;
      } 
   }
   priv->tcount.dec();
   return rv;
}

AbstractQoreNode *QoreProgram::callFunction(const char *name, const QoreListNode *args, ExceptionSink *xsink)
{
   UserFunction *ufc;
   SimpleRefHolder<FunctionCallNode> fc;

   printd(5, "QoreProgram::callFunction() creating function call to %s()\n", name);
   // need to grab parse lock for safe access to the user function map and imported function map
   priv->plock.lock();
   ufc = priv->user_func_list.find(name);
   if (!ufc)
      ufc = priv->imported_func_list.find(name);
   priv->plock.unlock();

   if (ufc) // we assign the args to 0 below so that they will not be deleted
      fc = new FunctionCallNode(ufc, const_cast<QoreListNode *>(args));
   else {
      const BuiltinFunction *bfc;
      if ((bfc = builtinFunctions.find(name))) {
	 // check parse options & function type
	 if (bfc->getType() & priv->parse_options) {
	    xsink->raiseException("INVALID-FUNCTION-ACCESS", "parse options do not allow access to builtin function '%s'", name);
	    return 0;
	 }
	 // we assign the args to 0 below so that they will not be deleted
	 fc = new FunctionCallNode(bfc, const_cast<QoreListNode *>(args));
      }
      else {
	 xsink->raiseException("NO-FUNCTION", "function name '%s' does not exist", name);
	 return 0;
      }
   }

   AbstractQoreNode *rv;
   {
      ProgramContextHelper pch(this, xsink);
      rv = fc->eval(xsink);
   }

   // let caller delete function arguments if necessary
   fc->take_args();

   return rv;
}

AbstractQoreNode *QoreProgram::callFunction(const UserFunction *ufc, const QoreListNode *args, ExceptionSink *xsink)
{
   // we assign the args to 0 below so that they will not be deleted
   SimpleRefHolder<FunctionCallNode> fc(new FunctionCallNode(ufc, const_cast<QoreListNode *>(args)));

   AbstractQoreNode *rv;
   {
      ProgramContextHelper pch(this, xsink);
      rv = fc->eval(xsink);
   }
   
   // let caller delete function arguments if necessary
   fc->take_args();

   return rv;
}

// called during run time (not during parsing)
void QoreProgram::importUserFunction(QoreProgram *p, UserFunction *u, ExceptionSink *xsink)
{
   AutoLocker al(&priv->plock);
   // check if a user function already exists with this name
   if (priv->user_func_list.find(u->getName()))
      xsink->raiseException("FUNCTION-IMPORT-ERROR", "user function '%s' already exists in this program object", u->getName());
   else if (priv->imported_func_list.findNode(u->getName()))
      xsink->raiseException("FUNCTION-IMPORT-ERROR", "function '%s' has already been imported into this program object", u->getName());
   else
      priv->imported_func_list.add(p, u);
}

void QoreProgram::parseCommit(ExceptionSink *xsink, ExceptionSink *wS, int wm)
{
   ProgramContextHelper pch(this, xsink);
   priv->parseCommit(xsink, wS, wm);
}

// this function cannot throw an exception because as long as the 
// parse lock is held
void QoreProgram::parseRollback()
{
   ProgramContextHelper pch(this, 0);

   // grab program-level parse lock
   priv->plock.lock();
   
   // back out all pending changes
   priv->internParseRollback();

   // release program-level parse lock
   priv->plock.unlock();   
}

void QoreProgram::runClass(const char *classname, ExceptionSink *xsink)
{
   // find class
   class QoreClass *qc = priv->RootNS->rootFindClass(classname);
   if (!qc)
   {
      xsink->raiseException("CLASS-NOT-FOUND", "cannot find any class '%s' in any namespace", classname);
      return;
   }
   //printd(5, "QoreProgram::runClass(%s)\n", classname);

   priv->tcount.inc();

   {
      ProgramContextHelper pch(this, xsink);
      discard(qc->execConstructor(0, xsink), xsink); 
   }
   
   priv->tcount.dec();
}

void QoreProgram::parseFileAndRunClass(const char *filename, const char *classname)
{
   ExceptionSink xsink;

   parseFile(filename, &xsink);

   if (!xsink.isEvent())
      runClass(classname, &xsink);
}

void QoreProgram::parseAndRunClass(FILE *fp, const char *name, const char *classname)
{
   ExceptionSink xsink;

   parse(fp, name, &xsink);

   if (!xsink.isEvent())
      runClass(classname, &xsink);
}

void QoreProgram::parseAndRunClass(const char *str, const char *name, const char *classname)
{
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
   priv->tcount.inc();
}

void QoreProgram::tc_dec() {
   priv->tcount.dec();
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
   return (priv->sb_head && priv->sb_head->statements) ? priv->sb_head->statements->getLVList() : 0;
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
