/*
  QoreProgram.cc

  Program Object Definition

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

#include <qore/config.h>
#include <qore/QoreProgram.h>
#include <qore/QoreProgramStack.h>
#include <qore/support.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/QoreLib.h>
#include <qore/Function.h>
#include <qore/qore_thread.h>
#include <qore/Exception.h>
#include <qore/Statement.h>
#include <qore/Variable.h>
#include <qore/Namespace.h>
#include <qore/charset.h>
#include <qore/ScopeGuard.h>

extern class List *ARGV, *QORE_ARGV;
extern class Hash *ENV;

// note the number and order of the warnings has to correspond to those in QoreProgram.h
const char *qore_warnings[] = { 
   "warning-mask-unchanged",
   "duplicate-local-vars",
   "unknown-warning",
   "undeclared-var",
   "duplicate-global-vars",
   "unreachable-code"
};
#define NUM_WARNINGS (sizeof(qore_warnings)/sizeof(char *))

int get_warning_code(char *str)
{
   for (unsigned i = 0; i < NUM_WARNINGS; i++)
      if (!strcasecmp((const char *)str, qore_warnings[i]))
         return 1 << i;
   return 0;
}

DLLLOCAL void addProgramConstants(class Namespace *ns)
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
}

class SBNode {
public:
   class StatementBlock *statements;
   class SBNode *next;
   
   inline SBNode() { next = NULL; statements = NULL; }
   inline ~SBNode();
   inline void reset();
};

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
   //printd(5, "QoreProgram::~QoreProgram() this=%08p\n", this);
}

void QoreProgram::init()
{
   //printd(5, "QoreProgram::QoreProgram() (init()) this=%08p\n", this);
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

// setup first program object
QoreProgram::QoreProgram()
{
   init();
   base_object = true;
   parse_options = PO_DEFAULT;
   po_locked = false;
   exec_class = false;
   exec_class_name = NULL;

   // init thread local storage key
   pthread_key_create(&thread_local_storage, NULL);
   // save thread local storage hash
   startThread();

   // copy global feature list
   qoreFeatureList.populate(&featureList);

   // setup namespaces
   RootNS = new RootNamespace(&QoreNS);
}

QoreProgram::QoreProgram(class QoreProgram *pgm, int po, bool ec, char *ecn)
{
   init();

   // flag as derived object
   base_object = false;

   // set parse options for child object
   parse_options = po;
   // if children inherit restrictions, then set all child restrictions
   if (!(pgm->parse_options & PO_NO_CHILD_PO_RESTRICTIONS))
   {
      // lock child parse options
      po_locked = true;
      // turn on all restrictions in the child that are set in the parent
      parse_options |= pgm->parse_options;
      // make sure all options that give more freedom and are off in the parent program are turned off in the child
      parse_options &= (pgm->parse_options | ~PO_POSITIVE_OPTIONS);
   }
   else
      po_locked = false;

   exec_class = ec;
   exec_class_name = ecn ? strdup(ecn) : NULL;

   // inherit parent's thread local storage key
   thread_local_storage = pgm->thread_local_storage;

   // setup derived namespaces
   RootNS = pgm->RootNS->copy(po);
   QoreNS = RootNS->rootGetQoreNamespace();

   // copy parent feature list
   pgm->featureList.populate(&featureList);
}

void QoreProgram::deref(class ExceptionSink *xsink)
{
   //printd(5, "QoreProgram::deref() this=%08p %d->%d\n", this, reference_count(), reference_count() - 1);
   if (ROdereference())
   {
      // delete all global variables
      global_var_list.clear_all(xsink);
      // clear thread data if base object
      if (base_object)
	 clearThreadData(xsink);
      depDeref(xsink);
   }
}

void QoreProgram::del(class ExceptionSink *xsink)
{
   printd(5, "QoreProgram::del() this=%08p (base_object=%d)\n", this, base_object);
   // wait for all threads to terminate
   tcount.waitForZero();

   // have to delete global variables first because of destructors.
   // method call can be repeated
   global_var_list.delete_all(xsink);

   // delete user functions in case there are constant objects which are 
   // instances of classes that may be deleted below (call can be repeated)
   user_func_list.del();

   // method call can be repeated
   deleteSBList();

   if (exec_class_name)
   {
      free(exec_class_name);
      exec_class_name = NULL;
   }

   delete RootNS;
   RootNS = 0;

   if (base_object)
   {
      endThread(xsink);

      // delete thread local storage key
      pthread_key_delete(thread_local_storage);
      base_object = false;
   }
}

class Var *QoreProgram::findVar(char *name)
{
   return global_var_list.findVar(name);
}

class Var *QoreProgram::checkVar(char *name)
{
   int new_var = 0;
   class Var *rv = global_var_list.checkVar(name, &new_var);
   if (new_var)
   {
      printd(5, "QoreProgram::checkVar() new global var \"%s\" found\n", name);
      // check if unflagged global vars are allowed
      if (parse_options & PO_REQUIRE_OUR)
	 parseException("UNDECLARED-GLOBAL-VARIABLE", "global variable '%s' must first be declared with 'our' (conflicts with parse option REQUIRE_OUR)", name);
      // check if new global variables are allowed to be created at all
      else if (parse_options & PO_NO_GLOBAL_VARS)
	 parseException("ILLEGAL-GLOBAL-VARIABLE", "illegal reference to new global variable '%s' (conflicts with parse option NO_GLOBAL_VARS)", name);
      else if (new_var)
	 makeParseWarning(QP_WARN_UNDECLARED_VAR, "UNDECLARED-GLOBAL-VARIABLE", "global variable '%s' should be declared with 'our'", name);
   }
   return rv;
}

class Var *QoreProgram::createVar(char *name)
{
   int new_var = 0;
   class Var *rv = global_var_list.checkVar(name, &new_var);
   // it's a new global variable: check if global variables are allowed
   if ((parse_options & PO_NO_GLOBAL_VARS) && new_var)
      parse_error("illegal reference to new global variable \"%s\" (conflicts with parse option NO_GLOBAL_VARS)", name);

   printd(5, "QoreProgram::createVar() global var \"%s\" processed, new_var=%d\n", name, new_var);

   return rv;
}

// if this global variable definition is illegal, then
// it will be flagged in the parseCommit stage
void QoreProgram::addGlobalVarDef(char *name)
{
   int new_var = 0;
   global_var_list.checkVar(name, &new_var);
   if (!new_var)
      makeParseWarning(QP_WARN_DUPLICATE_GLOBAL_VARS, "DUPLICATE-GLOBAL-VARIABLE", "global variable '%s' has already been declared", name);
}

void QoreProgram::makeParseException(char *err, class QoreString *desc)
{
   tracein("QoreProgram::makeParseException()");
   if (!requires_exception)
   {
      class Exception *ne = new ParseException(err, desc);
      parseSink->raiseException(ne);
   }
   else
      delete desc;
   traceout("QoreProgram::makeParseException()");
}

void QoreProgram::makeParseException(class QoreString *desc)
{
   tracein("QoreProgram::makeParseException()");
   if (!requires_exception)
   {
      class Exception *ne = new ParseException("PARSE-EXCEPTION", desc);
      parseSink->raiseException(ne);
   }
   else
      delete desc;
   traceout("QoreProgram::makeParseException()");
}

void QoreProgram::makeParseException(int sline, int eline, class QoreString *desc)
{
   tracein("QoreProgram::makeParseException()");
   if (!requires_exception)
   {
      class Exception *ne = new ParseException(sline, eline, "PARSE-EXCEPTION", desc);
      parseSink->raiseException(ne);
   }
   else
      delete desc;
   traceout("QoreProgram::makeParseException()");
}

void QoreProgram::addParseException(class ExceptionSink *xsink)
{
   if (requires_exception)
   {
      delete xsink;
      return;
   }
   // ensure that all exceptions reflect the current parse location
   int sline, eline;
   get_parse_location(sline, eline);
   xsink->overrideLocation(sline, eline, get_parse_file());
   parseSink->assimilate(xsink);
}

void QoreProgram::makeParseWarning(int code, char *warn, const char *fmt, ...)
{
   //printd(5, "QP::mPW(code=%d, warn='%s', fmt='%s') warn_mask=%d warnSink=%08p %s\n", code, warn, fmt, warn_mask, warnSink, warnSink && (code & warn_mask) ? "OK" : "SKIPPED");
   if (!warnSink || !(code & warn_mask))
      return;
   tracein("QoreProgram::makeParseWarning()");
   class QoreString *desc = new QoreString();
   while (true)
   {
      va_list args;
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
   class Exception *ne = new ParseException(warn, desc);
   warnSink->raiseException(ne);
   traceout("QoreProgram::makeParseWarning()");
}

void QoreProgram::addParseWarning(int code, class ExceptionSink *xsink)
{
   if (!warnSink || !(code & warn_mask))
      return;
   warnSink->assimilate(xsink);
}

void QoreProgram::cannotProvideFeature(class QoreString *desc)
{
   tracein("QoreProgram::cannotProvideFeature()");
   
   if (!requires_exception)
   {
      parseSink->clear();
      requires_exception = true;
   }

   class Exception *ne = new ParseException("CANNOT-PROVIDE-FEATURE", desc);
   parseSink->raiseException(ne);
   
   traceout("QoreProgram::cannotProvideFeature()");
}

class UserFunction *QoreProgram::findUserFunction(char *name)
{
   plock.lock();
   class UserFunction *uf = user_func_list.find(name);
   plock.unlock();
   return uf;
}

void QoreProgram::exportUserFunction(char *name, class QoreProgram *p, class ExceptionSink *xsink)
{
   if (this == p)
      xsink->raiseException("PROGRAM-IMPORTFUNCTION-PARAMETER-ERROR", "cannot import a function from the same Program object");
   else
   {
      class UserFunction *u;
      plock.lock();
      u = user_func_list.find(name);
      if (!u)
	 u = imported_func_list.find(name);
      plock.unlock();
      if (!u)
	 xsink->raiseException("PROGRAM-IMPORTFUNCTION-NO-FUNCTION", "function \"%s\" does not exist in the current program scope", name);
      else
	 p->importUserFunction(this, u, xsink);
   }
}

void QoreProgram::deleteSBList()
{
   SBNode *w = sb_head;
   while (w)
   {
      w = w->next;
      delete sb_head;
      sb_head = w;
   }
}

// called during parsing (plock already grabbed)
void QoreProgram::registerUserFunction(UserFunction *u)
{
   // check if an imported function already exists with this name
   if (imported_func_list.findNode(u->name))
      parse_error("function \"%s\" has already been imported into this program", u->name);
   else
      user_func_list.add(u);
}

void QoreProgram::internParseRollback()
{
   // delete pending user functions
   user_func_list.parseRollback();
   
   // delete pending changes to namespaces
   RootNS->parseRollback();
   
   // delete pending statements 
   sb_tail->reset();
}

void QoreProgram::nextSB()
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

void QoreProgram::importGlobalVariable(class Var *var, class ExceptionSink *xsink, bool readonly)
{
   plock.lock();
   global_var_list.import(var, xsink, readonly);
   plock.unlock();
}

int QoreProgram::setWarningMask(int wm)
{
   if (!(parse_options & PO_LOCK_WARNINGS)) 
   { 
      warn_mask = wm; 
      return 0; 
   } 
   return -1; 
}

// returns 0 for success, -1 for error
int QoreProgram::enableWarning(int code) 
{ 
   if (!(parse_options & PO_LOCK_WARNINGS)) 
   { 
      warn_mask |= code; 
      return 0; 
   } 
   return -1; 
}

// returns 0 for success, -1 for error
int QoreProgram::disableWarning(int code) 
{ 
   if (!(parse_options & PO_LOCK_WARNINGS)) 
   { 
      warn_mask &= ~code; 
      return 0; 
   } 
   return -1; 
}

class RootNamespace *QoreProgram::getRootNS() const
{
   return RootNS; 
}

int QoreProgram::getParseOptions() const
{ 
   return parse_options; 
}

class List *QoreProgram::getUserFunctionList()
{
   return user_func_list.getList(); 
}

void QoreProgram::waitForTermination()
{
   tcount.waitForZero();
}

void QoreProgram::waitForTerminationAndDeref(class ExceptionSink *xsink)
{
   tcount.waitForZero();
   deref(xsink);
}

void QoreProgram::lockOptions() 
{ 
   po_locked = true; 
}

// setExecClass() NOTE: string passed here will copied
void QoreProgram::setExecClass(char *ecn)
{
   exec_class = true;
   exec_class_name = ecn ? strdup(ecn) : NULL;
}

class Namespace *QoreProgram::getQoreNS() const
{
   return QoreNS;
}

void QoreProgram::depRef()
{
   //printd(5, "QoreProgram::depRef() this=%08p %d->%d\n", this, dc.reference_count(), dc.reference_count() + 1);
   dc.ROreference();
}

void QoreProgram::depDeref(class ExceptionSink *xsink)
{
   //printd(5, "QoreProgram::depDeref() this=%08p %d->%d\n", this, dc.reference_count(), dc.reference_count() - 1);
   if (dc.ROdereference())
   {
      del(xsink);
      delete this;
   }
}

bool QoreProgram::checkWarning(int code) const 
{ 
   return warnSink && (code & warn_mask); 
}

int QoreProgram::getWarningMask() const 
{ 
   return warnSink ? warn_mask : 0; 
}

void QoreProgram::addStatement(class Statement *s)
{
   if (!sb_tail->statements)
      sb_tail->statements = new StatementBlock(s);
   else
      sb_tail->statements->addStatement(s);

   // see if top level statements are allowed
   if (parse_options & PO_NO_TOP_LEVEL_STATEMENTS)
      parse_error("illegal top-level statement (conflicts with parse option NO_TOP_LEVEL_STATEMENTS)");
}

bool QoreProgram::existsFunction(char *name)
{
   // need to grab the parse lock for safe access to the user function map
   plock.lock();
   bool b = user_func_list.find(name);
   plock.unlock();
   return b;
}

void QoreProgram::parseSetParseOptions(int po)
{
   if (po_locked)
   {
      parse_error("parse options have been locked on this program object");
      return;
   }
   parse_options |= po;
}

void QoreProgram::setParseOptions(int po, class ExceptionSink *xsink)
{
   if (po_locked)
   {
      xsink->raiseException("OPTIONS-LOCKED", "parse options have been locked on this program object");
      return;
   }
   parse_options |= po;
}

void QoreProgram::disableParseOptions(int po, class ExceptionSink *xsink)
{
   if (po_locked)
   {
      xsink->raiseException("OPTIONS-LOCKED", "parse options have been locked on this program object");
      return;
   }
   parse_options &= (~po);
}

void QoreProgram::parsePending(char *code, char *label, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
{
   if (!code || !code[0])
      return;

   pushProgram(this);

   // grab program-level parse lock
   plock.lock();
   warnSink = wS;
   warn_mask = wm;

   parseSink = xsink;
   internParsePending(code, label);
   warnSink = NULL;
#ifdef DEBUG
   parseSink = NULL;
#endif
   // release program-level parse lock
   plock.unlock();

   popProgram();
}

void QoreProgram::startThread()
{
   pthread_setspecific(thread_local_storage, new Hash());
}

class Hash *QoreProgram::getThreadData()
{
   return (class Hash *)pthread_getspecific(thread_local_storage);
}

class QoreNode *QoreProgram::run(class ExceptionSink *xsink)
{
   if (exec_class_name)
   {
      runClass(exec_class_name, xsink);
      return NULL;
   }
   return runTopLevel(xsink);
}

class Hash *QoreProgram::clearThreadData(class ExceptionSink *xsink)
{
   class Hash *h = (class Hash *)pthread_getspecific(thread_local_storage);
   printd(5, "QoreProgram::clearThreadData() this=%08p h=%08p (size=%d)\n", this, h, h->size());
   h->dereference(xsink);
   return h;
}

void QoreProgram::endThread(class ExceptionSink *xsink)
{
   // delete thread local storage data
   class Hash *h = clearThreadData(xsink);
   h->derefAndDelete(xsink);
}

// called during parsing (plock already grabbed)
void QoreProgram::resolveFunction(class FunctionCall *f)
{
   tracein("QoreProgram::resolveFunction()");
   char *fname = f->f.c_str;

   class UserFunction *ufc;
   if ((ufc = user_func_list.find(fname)))
   {
      printd(5, "resolved user function call to %s\n", fname);
      f->type = FC_USER;
      f->f.ufunc = ufc;
      free(fname);
      traceout("QoreProgram::resolveFunction()");
      return;
   }

   class ImportedFunctionNode *ifn;
   if ((ifn = imported_func_list.findNode(fname)))
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
      if (bfc->getType() & parse_options)
	 parse_error("parse options do not allow access to builtin function '%s'", fname);

      free(fname);
      traceout("QoreProgram::resolveFunction()");
      return;
   }

   // cannot find function, throw exception
   parse_error("function '%s()' cannot be found", fname);
   traceout("QoreProgram::resolveFunction()");
}

void QoreProgram::parse(FILE *fp, char *name, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
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
   plock.lock();
   warnSink = wS;
   warn_mask = wm;
   parseSink = xsink;
   
   // save this file name for storage in the parse tree and deletion
   // when the QoreProgram object is deleted
   char *sname = strdup(name);
   fileList.push_front(sname);
   beginParsing(sname);

   pushProgram(this);
   printd(2, "QoreProgram::parse(): about to call yyparse()\n");
   yyscan_t lexer;
   yylex_init(&lexer);
   yyset_in(fp, lexer);
   // yyparse() will call endParsing() and restore old pgm position
   yyparse(lexer);

   // finalize parsing, back out or commit all changes
   internParseCommit();

#ifdef DEBUG
   parseSink = NULL;
#endif
   warnSink = NULL;
   // release program-level parse lock
   plock.unlock();

   yylex_destroy(lexer);

   popProgram();
}

void QoreProgram::parse(class QoreString *str, class QoreString *lstr, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
{
   if (!str->strlen())
      return;

   class QoreString *tstr, *tlstr;

   // ensure code string has correct character set encoding
   if (str->getEncoding() != QCS_DEFAULT)
   {
      tstr = str->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return;
   }
   else
      tstr = str;

   // ensure label string has correct character set encoding
   if (lstr->getEncoding() != QCS_DEFAULT)
   {
      tlstr = lstr->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return;
   }
   else
      tlstr = lstr;

   parse(tstr->getBuffer(), tlstr->getBuffer(), xsink, wS, wm);

   // cleanup temporary strings
   if (tstr != str)
      delete tstr;
   if (tlstr != lstr)
      delete tlstr;
}

void QoreProgram::parse(char *code, char *label, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
{
   if (!(*code))
      return;

   pushProgram(this);
   
   // grab program-level parse lock
   plock.lock();
   warnSink = wS;
   warn_mask = wm;
   parseSink = xsink;
   
   // parse text given
   if (!internParsePending(code, label))
      internParseCommit();   // finalize parsing, back out or commit all changes

#ifdef DEBUG
   parseSink = NULL;
#endif
   warnSink = NULL;
   // release program-level parse lock
   plock.unlock();

   popProgram();
}

void QoreProgram::parseFile(char *filename, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
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
int QoreProgram::internParsePending(char *code, char *label)
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
   fileList.push_front(sname);
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
   if (parseSink->isException())
   {
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

// caller must have grabbed the lock and put the current program on the program stack
void QoreProgram::internParseCommit()
{
   tracein("QoreProgram::internParseCommit()");
   printd(5, "QoreProgram::internParseCommit() this=%08p isEvent=%d\n", this, parseSink->isEvent());
   // if the first stage of parsing has already failed, 
   // then don't go forward
   if (!parseSink->isEvent())
   {
      // initialize constants first
      RootNS->parseInitConstants();

      // initialize new statements second (for $our declarations)
      if (sb_tail->statements)
	 sb_tail->statements->parseInit((lvh_t)0);

      printd(5, "QoreProgram::internParseCommit() this=%08p RootNS=%08p\n", this, RootNS);
      // initialize new objects, etc in namespaces
      RootNS->parseInit();

      // initialize new user functions
      user_func_list.parseInit();
   }

   // if a parse exception has occurred, then back out all new
   // changes to the QoreProgram atomically
   if (parseSink->isEvent())
   {
      internParseRollback();
      requires_exception = false;
   }
   else // otherwise commit them
   {
      // merge pending user functions
      user_func_list.parseCommit();

      // merge pending namespace additions
      RootNS->parseCommit();

      // commit pending statements
      nextSB();
   }
   traceout("QoreProgram::internParseCommit()");
}

void QoreProgram::parsePending(class QoreString *str, class QoreString *lstr, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
{
   if (!str->strlen())
      return;

   class QoreString *tstr, *tlstr;

   // ensure code string has correct character set
   if (str->getEncoding() != QCS_DEFAULT)
   {
      tstr = str->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return;
   }
   else
      tstr = str;

   // ensure label string has correct character set
   if (lstr->getEncoding() != QCS_DEFAULT)
   {
      tlstr = lstr->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return;
   }
   else
      tlstr = lstr;

   parsePending(tstr->getBuffer(), tlstr->getBuffer(), xsink, wS, wm);

   // cleanup temporary strings
   if (tstr != str)
      delete tstr;
   if (tlstr != lstr)
      delete tlstr;
}

class QoreNode *QoreProgram::runTopLevel(class ExceptionSink *xsink)
{
   tcount.inc();

   QoreNode *rv = NULL;
   SBNode *w = sb_head;

   pushProgram(this);
   while (w && !xsink->isEvent() && !rv)
   {
      if (w->statements)
	 rv = w->statements->exec(xsink);
      else
	 rv = NULL;
      w = w->next;
   } 
   popProgram();

   tcount.dec();
   return rv;
}

class QoreNode *QoreProgram::callFunction(char *name, class QoreNode *args, class ExceptionSink *xsink)
{
   class UserFunction *ufc;
   QoreNode *fc;

   printd(5, "QoreProgram::callFunction() creating function call to %s()\n", name);
   // need to grab parse lock for safe acces to the user function map and imported function map
   plock.lock();
   ufc = user_func_list.find(name);
   if (!ufc)
      ufc = imported_func_list.find(name);
   plock.unlock();

   if (ufc)
      fc = new QoreNode(ufc, args);
   else
   {
      class BuiltinFunction *bfc;
      if ((bfc = builtinFunctions.find(name)))
      {
	 // check parse options & function type
	 if (bfc->getType() & parse_options) 
	 {
	    xsink->raiseException("INVALID-FUNCTION-ACCESS", "parse options do not allow access to builtin function '%s'", name);
	    return NULL;
	 }
	 fc = new QoreNode(bfc, args);
      }
      else
      {
	 xsink->raiseException("NO-FUNCTION", "function name '%s' does not exist", name);
	 return NULL;
      }
   }

   pushProgram(this);
   QoreNode *rv = fc->val.fcall->eval(xsink);
   popProgram();

   // let caller delete function arguments if necessary
   fc->val.fcall->args = NULL;
   fc->deref(xsink);

   return rv;
}

class QoreNode *QoreProgram::callFunction(class UserFunction *ufc, class QoreNode *args, class ExceptionSink *xsink)
{
   QoreNode *fc = new QoreNode(ufc, args);

   pushProgram(this);
   QoreNode *rv = fc->val.fcall->eval(xsink);
   popProgram();

   // let caller delete function arguments if necessary
   fc->val.fcall->args = NULL;
   fc->deref(xsink);

   return rv;
}

// called during run time (not during parsing)
void QoreProgram::importUserFunction(class QoreProgram *p, class UserFunction *u, class ExceptionSink *xsink)
{
   plock.lock();
   // check if a user function already exists with this name
   if (user_func_list.find(u->name))
      xsink->raiseException("FUNCTION-IMPORT-ERROR", "user function '%s' already exists in this program object", u->name);
   else if (imported_func_list.findNode(u->name))
      xsink->raiseException("FUNCTION-IMPORT-ERROR", "function '%s' has already been imported into this program object", u->name);
   else
      imported_func_list.add(p, u);
   plock.unlock();
}

void QoreProgram::parseCommit(class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
{
   pushProgram(this);

   // grab program-level parse lock
   plock.lock();
   warnSink = wS;
   warn_mask = wm;
   parseSink = xsink;
   
   // finalize parsing, back out or commit all changes
   internParseCommit();

#ifdef DEBUG
   parseSink = NULL;
#endif   
   warnSink = NULL;
   // release program-level parse lock
   plock.unlock();

   popProgram();
}

// this function cannot throw an exception because as long as the 
// parse lock is held
void QoreProgram::parseRollback()
{
   pushProgram(this);

   // grab program-level parse lock
   plock.lock();
   
   // back out all pending changes
   internParseRollback();

   // release program-level parse lock
   plock.unlock();   

   popProgram();
}

void QoreProgram::runClass(char *classname, class ExceptionSink *xsink)
{
   // find class
   class QoreClass *qc = RootNS->rootFindClass(classname);
   if (!qc)
   {
      xsink->raiseException("CLASS-NOT-FOUND", "cannot find any class '%s' in any namespace", classname);
      return;
   }
   //printd(5, "QoreProgram::runClass(%s)\n", classname);

   tcount.inc();

   pushProgram(this);

   discard(qc->execConstructor(NULL, xsink), xsink); 

   popProgram();

   tcount.dec();
}

void QoreProgram::parseFileAndRunClass(char *filename, char *classname)
{
   ExceptionSink xsink;

   parseFile(filename, &xsink);

   if (!xsink.isEvent())
      runClass(classname, &xsink);
}

void QoreProgram::parseAndRunClass(FILE *fp, char *name, char *classname)
{
   ExceptionSink xsink;

   parse(fp, name, &xsink);

   if (!xsink.isEvent())
      runClass(classname, &xsink);
}

void QoreProgram::parseAndRunClass(char *str, char *name, char *classname)
{
   ExceptionSink xsink;

   parse(str, name, &xsink);

   if (!xsink.isEvent())
      runClass(classname, &xsink);
}

void QoreProgram::parseFileAndRun(char *filename)
{
   ExceptionSink xsink;

   parseFile(filename, &xsink);

   if (!xsink.isEvent())
   {
      // get class name
      if (exec_class)
      {
	 if (exec_class_name)
	    runClass(exec_class_name, &xsink);
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

void QoreProgram::parseAndRun(FILE *fp, char *name)
{
   ExceptionSink xsink;
   
   if (exec_class && !exec_class_name)
      xsink.raiseException("EXEC-CLASS-ERROR", "class name required if executing from stdin");
   else
   {
      parse(fp, name, &xsink);

      if (!xsink.isEvent())
	 run(&xsink);
   }
}

void QoreProgram::parseAndRun(char *str, char *name)
{
   ExceptionSink xsink;

   if (exec_class && !exec_class_name)
      xsink.raiseException("EXEC-CLASS-ERROR", "class name required if executing from a direct string");
   else
   {
      parse(str, name, &xsink);

      if (!xsink.isEvent())
	 run(&xsink);
   }
}

bool QoreProgram::checkFeature(char *f) const
{
   return featureList.find(f);
}

void QoreProgram::addFeature(char *f)
{
   featureList.push_back(f);
}

void QoreProgram::addFile(char *f)
{
   fileList.push_front(f);
}

class List *QoreProgram::getFeatureList() const
{
   class List *l = new List();

   for (charPtrList::const_iterator i = featureList.begin(); i != featureList.end(); i++)
      l->push(new QoreNode(*i));

   return l;
}

class List *QoreProgram::getVarList()
{
   plock.lock();
   class List *l = global_var_list.getVarList();
   plock.unlock();
   return l;
}

void QoreProgram::tc_inc()
{
   tcount.inc();
}

void QoreProgram::tc_dec()
{
   tcount.dec();
}
