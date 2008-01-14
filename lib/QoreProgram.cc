/*
  QoreProgram.cc

  Program QoreObject Definition

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
#include <qore/StringList.h>
#include <qore/intern/UserFunctionList.h>
#include <qore/intern/GlobalVariableList.h>
#include <qore/intern/ImportedFunctionList.h>

#include <string>
#include <set>

#include <errno.h>
#include <typeinfo>

extern class QoreList *ARGV, *QORE_ARGV;
extern class QoreHash *ENV;

class SBNode {
   public:
      class StatementBlock *statements;
      class SBNode *next;
   
      DLLLOCAL SBNode() { next = NULL; statements = NULL; }
      DLLLOCAL ~SBNode();
      DLLLOCAL void reset();
};

struct qore_program_private {
      class UserFunctionList user_func_list;
      class ImportedFunctionList imported_func_list;
      class GlobalVariableList global_var_list;

      // for the thread counter
      class QoreCounter tcount;
      // to save file names for later deleting
      class TempCharPtrStore fileList;
      // features present in this Program object
      class CharPtrList featureList;
      
      // parse lock, making parsing actions atomic and thread-safe
      class LockedObject plock;
      // depedency counter, when this hits zero, the object is deleted
      class ReferenceObject dc;
      class SBNode *sb_head, *sb_tail;
      class ExceptionSink *parseSink, *warnSink;
      class RootQoreNamespace *RootNS;
      class QoreNamespace *QoreNS;

      int parse_options, warn_mask;
      bool po_locked, exec_class, base_object, requires_exception;
      std::string exec_class_name;
      pthread_key_t thread_local_storage;

      DLLLOCAL qore_program_private()
      {
	 printd(5, "QoreProgram::QoreProgram() (init()) this=%08p\n", this);
#ifdef DEBUG
	 parseSink = NULL;
#endif
	 warnSink = NULL;
	 requires_exception = false;
	 sb_head = sb_tail = NULL;
	 nextSB();
	 
	 // initialize global vars
	 class Var *var = global_var_list.newVar("ARGV");
	 if (ARGV)
	    var->setValue(ARGV->copy(), NULL);
	 
	 var = global_var_list.newVar("QORE_ARGV");
	 if (QORE_ARGV)
	    var->setValue(QORE_ARGV->copy(), NULL);
	 
	 var = global_var_list.newVar("ENV");
	 var->setValue(new QoreNode(ENV->copy()), NULL);
      }

      DLLLOCAL ~qore_program_private()
      {
      }

      DLLLOCAL class QoreList *getVarList()
      {
	 plock.lock();
	 class QoreList *l = global_var_list.getVarList();
	 plock.unlock();
	 return l;
      }

      DLLLOCAL void deleteSBList()
      {
	 SBNode *w = sb_head;
	 while (w)
	 {
	    w = w->next;
	    delete sb_head;
	    sb_head = w;
	 }
      }

      void nextSB()
      {
	 if (sb_tail && !sb_tail->statements)
	    return;
	 class SBNode *sbn = new SBNode();
	 if (!sb_tail)
	    sb_head = sbn;
	 else
	    sb_tail->next = sbn;
	 sb_tail = sbn;
      }

      class QoreList *getFeatureList() const
      {
	 class QoreList *l = new QoreList();
	 
	 for (CharPtrList::const_iterator i = featureList.begin(), e = featureList.end(); i != e; ++i)
	    l->push(new QoreStringNode(*i));
	 
	 return l;
      }

};

// note the number and order of the warnings has to correspond to those in QoreProgram.h
const char *qore_warnings[] = { 
   "warning-mask-unchanged",
   "duplicate-local-vars",
   "unknown-warning",
   "undeclared-var",
   "duplicate-global-vars",
   "unreachable-code"
};
#define NUM_WARNINGS (sizeof(qore_warnings)/sizeof(const char *))

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
   ns->addConstant("PO_DEFAULT",                  new QoreNode((int64)PO_DEFAULT));
   ns->addConstant("PO_NO_GLOBAL_VARS",           new QoreNode((int64)PO_NO_GLOBAL_VARS));
   ns->addConstant("PO_NO_SUBROUTINE_DEFS",       new QoreNode((int64)PO_NO_SUBROUTINE_DEFS));  
   ns->addConstant("PO_NO_THREAD_CONTROL",        new QoreNode((int64)PO_NO_THREAD_CONTROL));
   ns->addConstant("PO_NO_THREAD_CLASSES",        new QoreNode((int64)PO_NO_THREAD_CLASSES));
   ns->addConstant("PO_NO_THREADS",               new QoreNode((int64)PO_NO_THREADS));
   ns->addConstant("PO_NO_TOP_LEVEL_STATEMENTS",  new QoreNode((int64)PO_NO_TOP_LEVEL_STATEMENTS));  
   ns->addConstant("PO_NO_CLASS_DEFS",            new QoreNode((int64)PO_NO_CLASS_DEFS));
   ns->addConstant("PO_NO_NAMESPACE_DEFS",        new QoreNode((int64)PO_NO_NAMESPACE_DEFS));
   ns->addConstant("PO_NO_CONSTANT_DEFS",         new QoreNode((int64)PO_NO_CONSTANT_DEFS));
   ns->addConstant("PO_NO_NEW",                   new QoreNode((int64)PO_NO_NEW));
   ns->addConstant("PO_NO_SYSTEM_CLASSES",        new QoreNode((int64)PO_NO_SYSTEM_CLASSES));
   ns->addConstant("PO_NO_USER_CLASSES",          new QoreNode((int64)PO_NO_USER_CLASSES));
   ns->addConstant("PO_NO_CHILD_PO_RESTRICTIONS", new QoreNode((int64)PO_NO_CHILD_PO_RESTRICTIONS));
   ns->addConstant("PO_NO_EXTERNAL_PROCESS",      new QoreNode((int64)PO_NO_EXTERNAL_PROCESS));
   ns->addConstant("PO_REQUIRE_OUR",              new QoreNode((int64)PO_REQUIRE_OUR));
   ns->addConstant("PO_NO_PROCESS_CONTROL",       new QoreNode((int64)PO_NO_PROCESS_CONTROL));
   ns->addConstant("PO_NO_NETWORK",               new QoreNode((int64)PO_NO_NETWORK));
   ns->addConstant("PO_NO_FILESYSTEM",            new QoreNode((int64)PO_NO_FILESYSTEM));
   ns->addConstant("PO_LOCK_WARNINGS",            new QoreNode((int64)PO_LOCK_WARNINGS));
   ns->addConstant("PO_NO_GUI",                   new QoreNode((int64)PO_NO_GUI));
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
QoreProgram::QoreProgram() : priv(new qore_program_private)
{
   priv->base_object = true;
   priv->parse_options = PO_DEFAULT;
   priv->po_locked = false;
   priv->exec_class = false;

   // init thread local storage key
   pthread_key_create(&priv->thread_local_storage, NULL);
   // save thread local storage hash
   startThread();

   // copy global feature list to local list
   for (FeatureList::iterator i = qoreFeatureList.begin(), e = qoreFeatureList.end(); i != e; ++i)
      priv->featureList.push_back((*i).c_str());

   // setup namespaces
   priv->RootNS = new RootQoreNamespace(&priv->QoreNS);
}

QoreProgram::QoreProgram(class QoreProgram *pgm, int po, bool ec, const char *ecn) : priv(new qore_program_private)
{
   // flag as derived object
   priv->base_object = false;

   // set parse options for child object
   priv->parse_options = po;
   // if children inherit restrictions, then set all child restrictions
   if (!(pgm->priv->parse_options & PO_NO_CHILD_PO_RESTRICTIONS))
   {
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

class LockedObject *QoreProgram::getParseLock()
{
   return &priv->plock;
}

void QoreProgram::deref(class ExceptionSink *xsink)
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

void QoreProgram::del(class ExceptionSink *xsink)
{
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
   deleteSBList();

   delete priv->RootNS;
   priv->RootNS = 0;

   if (priv->base_object)
   {
      endThread(xsink);

      // delete thread local storage key
      pthread_key_delete(priv->thread_local_storage);
      priv->base_object = false;
   }
}

class Var *QoreProgram::findVar(const char *name)
{
   return priv->global_var_list.findVar(name);
}

class Var *QoreProgram::checkVar(const char *name)
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

class Var *QoreProgram::createVar(const char *name)
{
   int new_var = 0;
   class Var *rv = priv->global_var_list.checkVar(name, &new_var);
   // it's a new global variable: check if global variables are allowed
   if ((priv->parse_options & PO_NO_GLOBAL_VARS) && new_var)
      parse_error("illegal reference to new global variable '%s' (conflicts with parse option NO_GLOBAL_VARS)", name);

   printd(5, "QoreProgram::createVar() global var '%s' processed, new_var=%d (val=%08p)\n", name, new_var, rv);

   return rv;
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

void QoreProgram::makeParseException(const char *err, class QoreStringNode *desc)
{
   tracein("QoreProgram::makeParseException()");
   TempQoreStringNode d(desc);
   if (!priv->requires_exception)
   {
      class QoreException *ne = new ParseException(err, d.release());
      priv->parseSink->raiseException(ne);
   }
   traceout("QoreProgram::makeParseException()");
}

void QoreProgram::makeParseException(class QoreStringNode *desc)
{
   tracein("QoreProgram::makeParseException()");
   TempQoreStringNode d(desc);
   if (!priv->requires_exception)
   {
      class QoreException *ne = new ParseException("PARSE-EXCEPTION", d.release());
      priv->parseSink->raiseException(ne);
   }
   traceout("QoreProgram::makeParseException()");
}

void QoreProgram::makeParseException(int sline, int eline, class QoreStringNode *desc)
{
   tracein("QoreProgram::makeParseException()");
   TempQoreStringNode d(desc);
   if (!priv->requires_exception)
   {
      class QoreException *ne = new ParseException(sline, eline, "PARSE-EXCEPTION", d.release());
      priv->parseSink->raiseException(ne);
   }
   traceout("QoreProgram::makeParseException()");
}

void QoreProgram::addParseException(class ExceptionSink *xsink)
{
   if (priv->requires_exception)
   {
      delete xsink;
      return;
   }
   // ensure that all exceptions reflect the current parse location
   int sline, eline;
   get_parse_location(sline, eline);
   xsink->overrideLocation(sline, eline, get_parse_file());
   priv->parseSink->assimilate(xsink);
}

void QoreProgram::makeParseWarning(int code, const char *warn, const char *fmt, ...)
{
   //printd(5, "QP::mPW(code=%d, warn='%s', fmt='%s') priv->warn_mask=%d priv->warnSink=%08p %s\n", code, warn, fmt, priv->warn_mask, priv->warnSink, priv->warnSink && (code & priv->warn_mask) ? "OK" : "SKIPPED");
   if (!priv->warnSink || !(code & priv->warn_mask))
      return;
   tracein("QoreProgram::makeParseWarning()");
   class QoreStringNode *desc = new QoreStringNode();
   while (true)
   {
      va_list args;
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
   class QoreException *ne = new ParseException(warn, desc);
   priv->warnSink->raiseException(ne);
   traceout("QoreProgram::makeParseWarning()");
}

void QoreProgram::addParseWarning(int code, class ExceptionSink *xsink)
{
   if (!priv->warnSink || !(code & priv->warn_mask))
      return;
   priv->warnSink->assimilate(xsink);
}

void QoreProgram::cannotProvideFeature(class QoreStringNode *desc)
{
   tracein("QoreProgram::cannotProvideFeature()");
   
   if (!priv->requires_exception)
   {
      priv->parseSink->clear();
      priv->requires_exception = true;
   }

   class QoreException *ne = new ParseException("CANNOT-PROVIDE-FEATURE", desc);
   priv->parseSink->raiseException(ne);
   
   traceout("QoreProgram::cannotProvideFeature()");
}

class UserFunction *QoreProgram::findUserFunction(const char *name)
{
   AutoLocker al(&priv->plock);
   return priv->user_func_list.find(name);
}

void QoreProgram::exportUserFunction(const char *name, class QoreProgram *p, class ExceptionSink *xsink)
{
   if (this == p)
      xsink->raiseException("PROGRAM-IMPORTFUNCTION-PARAMETER-ERROR", "cannot import a function from the same Program object");
   else
   {
      class UserFunction *u;
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

void QoreProgram::deleteSBList()
{
   priv->deleteSBList();
}

// called during parsing (priv->plock already grabbed)
void QoreProgram::registerUserFunction(UserFunction *u)
{
   // check if an imported function already exists with this name
   if (priv->imported_func_list.findNode(u->getName()))
      parse_error("function \"%s\" has already been imported into this program", u->getName());
   else
      priv->user_func_list.add(u);
}

void QoreProgram::internParseRollback()
{
   // delete pending user functions
   priv->user_func_list.parseRollback();
   
   // delete pending changes to namespaces
   priv->RootNS->parseRollback();
   
   // delete pending statements 
   priv->sb_tail->reset();
}

void QoreProgram::nextSB()
{
   priv->nextSB();
}

void QoreProgram::importGlobalVariable(class Var *var, class ExceptionSink *xsink, bool readonly)
{
   priv->plock.lock();
   priv->global_var_list.import(var, xsink, readonly);
   priv->plock.unlock();
}

int QoreProgram::setWarningMask(int wm)
{
   if (!(priv->parse_options & PO_LOCK_WARNINGS)) 
   { 
      priv->warn_mask = wm; 
      return 0; 
   } 
   return -1; 
}

// returns 0 for success, -1 for error
int QoreProgram::enableWarning(int code) 
{ 
   if (!(priv->parse_options & PO_LOCK_WARNINGS)) 
   { 
      priv->warn_mask |= code; 
      return 0; 
   } 
   return -1; 
}

// returns 0 for success, -1 for error
int QoreProgram::disableWarning(int code) 
{ 
   if (!(priv->parse_options & PO_LOCK_WARNINGS)) 
   { 
      priv->warn_mask &= ~code; 
      return 0; 
   } 
   return -1; 
}

class RootQoreNamespace *QoreProgram::getRootNS() const
{
   return priv->RootNS; 
}

int QoreProgram::getParseOptions() const
{ 
   return priv->parse_options; 
}

class QoreList *QoreProgram::getUserFunctionList()
{
   AutoLocker al(&priv->plock);
   return priv->user_func_list.getList(); 
}

void QoreProgram::waitForTermination()
{
   priv->tcount.waitForZero();
}

void QoreProgram::waitForTerminationAndDeref(class ExceptionSink *xsink)
{
   priv->tcount.waitForZero();
   deref(xsink);
}

void QoreProgram::lockOptions() 
{ 
   priv->po_locked = true; 
}

// setExecClass() NOTE: string passed here will copied
void QoreProgram::setExecClass(const char *ecn)
{
   priv->exec_class = true;
   if (ecn)
      priv->exec_class_name = ecn;
   else
      priv->exec_class_name.clear();
}

class QoreNamespace *QoreProgram::getQoreNS() const
{
   return priv->QoreNS;
}

void QoreProgram::depRef()
{
   //printd(5, "QoreProgram::depRef() this=%08p %d->%d\n", this, priv->dc.reference_count(), priv->dc.reference_count() + 1);
   priv->dc.ROreference();
}

void QoreProgram::depDeref(class ExceptionSink *xsink)
{
   //printd(5, "QoreProgram::depDeref() this=%08p %d->%d\n", this, priv->dc.reference_count(), priv->dc.reference_count() - 1);
   if (priv->dc.ROdereference())
   {
      del(xsink);
      delete this;
   }
}

bool QoreProgram::checkWarning(int code) const 
{ 
   return priv->warnSink && (code & priv->warn_mask); 
}

int QoreProgram::getWarningMask() const 
{ 
   return priv->warnSink ? priv->warn_mask : 0; 
}

void QoreProgram::addStatement(class AbstractStatement *s)
{
   if (!priv->sb_tail->statements)
   {
      if (typeid(s) != typeid(StatementBlock))
	 priv->sb_tail->statements = new StatementBlock(s);
      else
	 priv->sb_tail->statements = dynamic_cast<StatementBlock *>(s);
   }
   else
      priv->sb_tail->statements->addStatement(s);

   // see if top level statements are allowed
   if (priv->parse_options & PO_NO_TOP_LEVEL_STATEMENTS)
      parse_error("illegal top-level statement (conflicts with parse option NO_TOP_LEVEL_STATEMENTS)");
}

bool QoreProgram::existsFunction(const char *name)
{
   // need to grab the parse lock for safe access to the user function map
   AutoLocker al(&priv->plock);
   return priv->user_func_list.find(name);
}

void QoreProgram::parseSetParseOptions(int po)
{
   if (priv->po_locked)
   {
      parse_error("parse options have been locked on this program object");
      return;
   }
   priv->parse_options |= po;
}

void QoreProgram::setParseOptions(int po, class ExceptionSink *xsink)
{
   if (priv->po_locked)
   {
      xsink->raiseException("OPTIONS-LOCKED", "parse options have been locked on this program object");
      return;
   }
   priv->parse_options |= po;
}

void QoreProgram::disableParseOptions(int po, class ExceptionSink *xsink)
{
   if (priv->po_locked)
   {
      xsink->raiseException("OPTIONS-LOCKED", "parse options have been locked on this program object");
      return;
   }
   priv->parse_options &= (~po);
}

void QoreProgram::parsePending(const char *code, const char *label, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
{
   if (!code || !code[0])
      return;

   ProgramContextHelper pch(this);

   // grab program-level parse lock
   priv->plock.lock();
   priv->warnSink = wS;
   priv->warn_mask = wm;

   priv->parseSink = xsink;
   internParsePending(code, label);
   priv->warnSink = NULL;
#ifdef DEBUG
   priv->parseSink = NULL;
#endif
   // release program-level parse lock
   priv->plock.unlock();
}

void QoreProgram::startThread()
{
   pthread_setspecific(priv->thread_local_storage, new QoreHash());
}

class QoreHash *QoreProgram::getThreadData()
{
   return (class QoreHash *)pthread_getspecific(priv->thread_local_storage);
}

class QoreNode *QoreProgram::run(class ExceptionSink *xsink)
{
   if (!priv->exec_class_name.empty())
   {
      runClass(priv->exec_class_name.c_str(), xsink);
      return NULL;
   }
   return runTopLevel(xsink);
}

class QoreHash *QoreProgram::clearThreadData(class ExceptionSink *xsink)
{
   class QoreHash *h = (class QoreHash *)pthread_getspecific(priv->thread_local_storage);
   printd(5, "QoreProgram::clearThreadData() this=%08p h=%08p (size=%d)\n", this, h, h->size());
   h->dereference(xsink);
   return h;
}

void QoreProgram::endThread(class ExceptionSink *xsink)
{
   // delete thread local storage data
   class QoreHash *h = clearThreadData(xsink);
   h->derefAndDelete(xsink);
}

// called during parsing (priv->plock already grabbed)
void QoreProgram::resolveFunction(class FunctionCall *f)
{
   tracein("QoreProgram::resolveFunction()");
   char *fname = f->f.c_str;

   class UserFunction *ufc;
   if ((ufc = priv->user_func_list.find(fname)))
   {
      printd(5, "resolved user function call to %s\n", fname);
      f->type = FC_USER;
      f->f.ufunc = ufc;
      free(fname);
      traceout("QoreProgram::resolveFunction()");
      return;
   }

   class ImportedFunctionNode *ifn;
   if ((ifn = priv->imported_func_list.findNode(fname)))
   {
      printd(5, "resolved imported function call to %s (pgm=%08p, func=%08p)\n",
	     fname, ifn->pgm, ifn->func);
      f->type = FC_IMPORTED;
      f->f.ifunc = new ImportedFunctionCall(ifn->pgm, ifn->func);
      free(fname);
      traceout("QoreProgram::resolveFunction()");
      return;
   }

   class BuiltinFunction *bfc;
   if ((bfc = builtinFunctions.find(fname)))
   {
      printd(5, "resolved builtin function call to %s\n", fname);
      f->type = FC_BUILTIN;
      f->f.bfunc = bfc;

      // check parse options to see if access is allowed
      if (bfc->getType() & priv->parse_options)
	 parse_error("parse options do not allow access to builtin function '%s'", fname);

      free(fname);
      traceout("QoreProgram::resolveFunction()");
      return;
   }

   // cannot find function, throw exception
   parse_error("function '%s()' cannot be found", fname);
   traceout("QoreProgram::resolveFunction()");
}

// called during parsing (priv->plock already grabbed)
void QoreProgram::resolveFunctionReference(class FunctionReference *fr)
{
   tracein("QoreProgram::resolveFunctionReference()");
   char *fname = fr->f.str;
   
   class UserFunction *ufc;
   if ((ufc = priv->user_func_list.find(fname)))
   {
      printd(5, "resolved function reference to user function %s\n", fname);
      fr->set_static(ufc, this);
      free(fname);
      traceout("QoreProgram::resolveFunction()");
      return;
   }
   
   class ImportedFunctionNode *ifn;
   if ((ifn = priv->imported_func_list.findNode(fname)))
   {
      printd(5, "resolved function reference to imported function %s (pgm=%08p, func=%08p)\n",
	     fname, ifn->pgm, ifn->func);
      fr->type = FC_IMPORTED;
      fr->f.ifunc = new ImportedFunctionCall(ifn->pgm, ifn->func);
      free(fname);
      traceout("QoreProgram::resolveFunction()");
      return;
   }
   
   class BuiltinFunction *bfc;
   if ((bfc = builtinFunctions.find(fname)))
   {
      printd(5, "resolved function reference to builtin function to %s\n", fname);
      fr->type = FC_BUILTIN;
      fr->f.bf = bfc;
      
      // check parse options to see if access is allowed
      if (bfc->getType() & priv->parse_options)
	 parse_error("parse options do not allow access to builtin function '%s'", fname);
      
      free(fname);
      traceout("QoreProgram::resolveFunction()");
      return;
   }
   
   // cannot find function, throw exception
   parse_error("reference to function '%s()' cannot be resolved", fname);
   traceout("QoreProgram::resolveFunction()");
}

void QoreProgram::parse(FILE *fp, const char *name, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
{
   printd(5, "QoreProgram::parse(fp=%08p, name=%s, xsink=%08p, wS=%08p, wm=%d)\n", fp, name, xsink, wS, wm);

   // if already at the end of file, then return
   // try to get one character from file
   int c = fgetc(fp);
   if (feof(fp))
   {
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

   ProgramContextHelper pch(this);
   printd(2, "QoreProgram::parse(): about to call yyparse()\n");
   yyscan_t lexer;
   yylex_init(&lexer);
   yyset_in(fp, lexer);
   // yyparse() will call endParsing() and restore old pgm position
   yyparse(lexer);

   // finalize parsing, back out or commit all changes
   internParseCommit();

#ifdef DEBUG
   priv->parseSink = NULL;
#endif
   priv->warnSink = NULL;
   // release program-level parse lock
   priv->plock.unlock();

   yylex_destroy(lexer);
}

void QoreProgram::parse(const QoreString *str, const QoreString *lstr, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
{
   if (!str->strlen())
      return;

   // ensure code string has correct character set encoding
   ConstTempEncodingHelper tstr(str, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   // ensure label string has correct character set encoding
   ConstTempEncodingHelper tlstr(lstr, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   parse(tstr->getBuffer(), tlstr->getBuffer(), xsink, wS, wm);
}

void QoreProgram::parse(const char *code, const char *label, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
{
   if (!(*code))
      return;

   ProgramContextHelper pch(this);
   
   // grab program-level parse lock
   priv->plock.lock();
   priv->warnSink = wS;
   priv->warn_mask = wm;
   priv->parseSink = xsink;
   
   // parse text given
   if (!internParsePending(code, label))
      internParseCommit();   // finalize parsing, back out or commit all changes

#ifdef DEBUG
   priv->parseSink = NULL;
#endif
   priv->warnSink = NULL;
   // release program-level parse lock
   priv->plock.unlock();
}

void QoreProgram::parseFile(const char *filename, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
{
   tracein("QoreProgram::parseFile()");

   printd(5, "QoreProgram::parseFile(%s)\n", filename);

   FILE *fp;
   if (!(fp = fopen(filename, "r")))
   {
      xsink->raiseException("PARSE-EXCEPTION", "cannot open qore script '%s': %s", filename, strerror(errno));
      traceout("QoreProgram::parseFile()");
      return;
   }
   ON_BLOCK_EXIT(fclose, fp);

   parse(fp, filename, xsink, wS, wm);
   traceout("QoreProgram::parseFile()");
}

// call must push the current program on the stack and pop it afterwards
int QoreProgram::internParsePending(const char *code, const char *label)
{
   printd(5, "QoreProgram::internParsePending(code=%08p, label=%s)\n", code, label);

   if (!(*code))
      return 0;

   // insert name for buffer in case of errors
   QoreString s;
   s.sprintf("<run-time-loaded: %s>", label);

   // save this file name for storage in the parse tree and deletion
   // when the QoreProgram object is deleted
   char *sname = strdup(s.getBuffer());
   priv->fileList.push_back(sname);
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
   if (priv->parseSink->isException())
   {
      rc = -1;
      printd(5, "QoreProgram::internParsePending() parse exception: calling parseRollback()\n");
      internParseRollback();
      priv->requires_exception = false;
   }

   printd(5, "QoreProgram::internParsePending() about to call yylex_destroy()\n");
   yylex_destroy(lexer);
   printd(5, "QoreProgram::internParsePending() returned from yylex_destroy()\n");
   return rc;
}

// caller must have grabbed the lock and put the current program on the program stack
void QoreProgram::internParseCommit()
{
   tracein("QoreProgram::internParseCommit()");
   printd(5, "QoreProgram::internParseCommit() this=%08p isEvent=%d\n", this, priv->parseSink->isEvent());
   // if the first stage of parsing has already failed, 
   // then don't go forward
   if (!priv->parseSink->isEvent())
   {
      // initialize constants first
      priv->RootNS->parseInitConstants();

      // initialize new statements second (for $our declarations)
      if (priv->sb_tail->statements)
	 priv->sb_tail->statements->parseInitImpl((lvh_t)0);

      printd(5, "QoreProgram::internParseCommit() this=%08p priv->RootNS=%08p\n", this, priv->RootNS);
      // initialize new objects, etc in namespaces
      priv->RootNS->parseInit();

      // initialize new user functions
      priv->user_func_list.parseInit();
   }

   // if a parse exception has occurred, then back out all new
   // changes to the QoreProgram atomically
   if (priv->parseSink->isEvent())
   {
      internParseRollback();
      priv->requires_exception = false;
   }
   else // otherwise commit them
   {
      // merge pending user functions
      priv->user_func_list.parseCommit();

      // merge pending namespace additions
      priv->RootNS->parseCommit();

      // commit pending statements
      nextSB();
   }
   traceout("QoreProgram::internParseCommit()");
}

void QoreProgram::parsePending(const QoreString *str, const QoreString *lstr, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
{
   if (!str->strlen())
      return;

   // ensure code string has correct character set encoding
   ConstTempEncodingHelper tstr(str, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   // ensure label string has correct character set encoding
   ConstTempEncodingHelper tlstr(lstr, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   parsePending(tstr->getBuffer(), tlstr->getBuffer(), xsink, wS, wm);
}

class QoreNode *QoreProgram::runTopLevel(class ExceptionSink *xsink)
{
   priv->tcount.inc();

   QoreNode *rv = NULL;
   SBNode *w = priv->sb_head;

   {
      ProgramContextHelper pch(this);
      while (w && !xsink->isEvent() && !rv)
      {
	 if (w->statements)
	    rv = w->statements->exec(xsink);
	 else
	    rv = NULL;
	 w = w->next;
      } 
   }
   priv->tcount.dec();
   return rv;
}

class QoreNode *QoreProgram::callFunction(const char *name, const QoreNode *args, class ExceptionSink *xsink)
{
   class UserFunction *ufc;
   QoreNode *fc;

   printd(5, "QoreProgram::callFunction() creating function call to %s()\n", name);
   // need to grab parse lock for safe acces to the user function map and imported function map
   priv->plock.lock();
   ufc = priv->user_func_list.find(name);
   if (!ufc)
      ufc = priv->imported_func_list.find(name);
   priv->plock.unlock();

   if (ufc) // we assign the args to NULL below so that the caller will delete
      fc = new QoreNode(ufc, const_cast<QoreNode *>(args));
   else
   {
      class BuiltinFunction *bfc;
      if ((bfc = builtinFunctions.find(name)))
      {
	 // check parse options & function type
	 if (bfc->getType() & priv->parse_options) 
	 {
	    xsink->raiseException("INVALID-FUNCTION-ACCESS", "parse options do not allow access to builtin function '%s'", name);
	    return NULL;
	 }
	 // we assign the args to NULL below so that the caller will delete
	 fc = new QoreNode(bfc, const_cast<QoreNode *>(args));
      }
      else
      {
	 xsink->raiseException("NO-FUNCTION", "function name '%s' does not exist", name);
	 return NULL;
      }
   }

   QoreNode *rv;
   {
      ProgramContextHelper pch(this);
      rv = fc->val.fcall->eval(xsink);
   }

   // let caller delete function arguments if necessary
   fc->val.fcall->args = NULL;
   fc->deref(xsink);

   return rv;
}

class QoreNode *QoreProgram::callFunction(class UserFunction *ufc, const QoreNode *args, class ExceptionSink *xsink)
{
   // we assign the args to NULL below so that the caller will delete
   QoreNode *fc = new QoreNode(ufc, const_cast<QoreNode *>(args));

   QoreNode *rv;
   {
      ProgramContextHelper pch(this);
      rv = fc->val.fcall->eval(xsink);
   }
   
   // let caller delete function arguments if necessary
   fc->val.fcall->args = NULL;
   fc->deref(xsink);

   return rv;
}

// called during run time (not during parsing)
void QoreProgram::importUserFunction(class QoreProgram *p, class UserFunction *u, class ExceptionSink *xsink)
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

void QoreProgram::parseCommit(class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
{
   ProgramContextHelper pch(this);

   // grab program-level parse lock
   priv->plock.lock();
   priv->warnSink = wS;
   priv->warn_mask = wm;
   priv->parseSink = xsink;
   
   // finalize parsing, back out or commit all changes
   internParseCommit();

#ifdef DEBUG
   priv->parseSink = NULL;
#endif   
   priv->warnSink = NULL;
   // release program-level parse lock
   priv->plock.unlock();
}

// this function cannot throw an exception because as long as the 
// parse lock is held
void QoreProgram::parseRollback()
{
   ProgramContextHelper pch(this);

   // grab program-level parse lock
   priv->plock.lock();
   
   // back out all pending changes
   internParseRollback();

   // release program-level parse lock
   priv->plock.unlock();   
}

void QoreProgram::runClass(const char *classname, class ExceptionSink *xsink)
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
      ProgramContextHelper pch(this);
      discard(qc->execConstructor(NULL, xsink), xsink); 
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

void QoreProgram::parseFileAndRun(const char *filename)
{
   ExceptionSink xsink;

   parseFile(filename, &xsink);

   if (!xsink.isEvent())
   {
      // get class name
      if (priv->exec_class)
      {
	 if (!priv->exec_class_name.empty())
	    runClass(priv->exec_class_name.c_str(), &xsink);
	 else
	 {
	    char *c, *bn = q_basenameptr(filename);
	    if (!(c = strrchr(bn, '.')))
	       runClass(filename, &xsink);
	    else
	    {
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

void QoreProgram::parseAndRun(FILE *fp, const char *name)
{
   ExceptionSink xsink;
   
   if (priv->exec_class && priv->exec_class_name.empty())
      xsink.raiseException("EXEC-CLASS-ERROR", "class name required if executing from stdin");
   else
   {
      parse(fp, name, &xsink);

      if (!xsink.isEvent())
	 run(&xsink);
   }
}

void QoreProgram::parseAndRun(const char *str, const char *name)
{
   ExceptionSink xsink;

   if (priv->exec_class && priv->exec_class_name.empty())
      xsink.raiseException("EXEC-CLASS-ERROR", "class name required if executing from a direct string");
   else
   {
      parse(str, name, &xsink);

      if (!xsink.isEvent())
	 run(&xsink);
   }
}

bool QoreProgram::checkFeature(const char *f) const
{
   return priv->featureList.find(f);
}

void QoreProgram::addFeature(const char *f)
{
   priv->featureList.push_back(f);
}

void QoreProgram::addFile(char *f)
{
   priv->fileList.push_back(f);
}

class QoreList *QoreProgram::getFeatureList() const
{
   return priv->getFeatureList();
}

class QoreList *QoreProgram::getVarList()
{
   return priv->getVarList();
}

void QoreProgram::tc_inc()
{
   priv->tcount.inc();
}

void QoreProgram::tc_dec()
{
   priv->tcount.dec();
}
