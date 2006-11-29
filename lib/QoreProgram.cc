/*
  QoreProgram.cc

  Program Object Definition

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

QoreProgram::~QoreProgram()
{
   printd(5, "QoreProgram::~QoreProgram() this=%08p\n", this);
}

void QoreProgram::init()
{
   //printd(5, "QoreProgram::init() this=%08p\n", this);
#ifdef DEBUG
   parseSink = NULL;
#endif
   warnSink = NULL;
   requires_exception = false;
   sb_head = sb_tail = NULL;
   nextSB();

   // initialize global vars
   class Var *var = newVar("ARGV");
   if (ARGV)
      var->setValue(ARGV->copy(), NULL);

   var = newVar("QORE_ARGV");
   if (QORE_ARGV)
      var->setValue(QORE_ARGV->copy(), NULL);

   var = newVar("ENV");
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

void QoreProgram::del(class ExceptionSink *xsink)
{
   printd(5, "QoreProgram::del() this=%08p (base_object=%d)\n", this, base_object);
   // wait for all threads to terminate
   tcount.waitForZero();

   // have to delete global variables first because of destructors.
   // method call can be repeated
   delete_all(xsink);

   // delete user functions in case there are constant objects which are 
   // instances of classes that may be deleted below (call can be repeated)
   deleteUserFunctions();

   // method call can be repeated
   deleteSBList();

   if (exec_class_name)
   {
      free(exec_class_name);
      exec_class_name = NULL;
   }

   if (RootNS)
   {
      delete RootNS;
      RootNS = NULL;
   }

   if (base_object)
   {
      endThread(xsink);

      // delete thread local storage key
      pthread_key_delete(thread_local_storage);
      base_object = false;
   }
}

void QoreProgram::endThread(class ExceptionSink *xsink)
{
   // delete thread local storage data
   class Hash *h = clearThreadData(xsink);
   h->derefAndDelete(xsink);
}

void QoreProgram::resolveFunction(class FunctionCall *f)
{
   tracein("QoreProgram::resolveFunction()");
   char *fname = f->f.c_str;

   class UserFunction *ufc;
   if ((ufc = findUserFunction(fname)))
   {
      printd(5, "resolved user function call to %s\n", fname);
      f->type = FC_USER;
      f->f.ufunc = ufc;
      free(fname);
      traceout("QoreProgram::resolveFunction()");
      return;
   }

   class ImportedFunctionNode *ifn;
   if ((ifn = findImportedFunctionNode(fname)))
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
   yyparse(lexer);
   // yyparse() will call endParsing() and restore old pgm position

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

   parse(fp, filename, xsink, wS, wm);

   fclose(fp);

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
      parseInitUserFunctions();
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
      mergePendingUserFunctions();

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
   // need to grab parse lock for safe acces to the user function map
   plock.lock();
   ufc = findUserFunction(name);
   plock.unlock();

   if (ufc || (ufc = findImportedFunction(name)))
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
   if (findUserFunction(u->name))
      xsink->raiseException("FUNCTION-IMPORT-ERROR", "user function '%s' already exists in this program object", u->name);
   else if (findImportedFunction(u->name))
      xsink->raiseException("FUNCTION-IMPORT-ERROR", "function '%s' has already been imported into this program object", u->name);
   else
      addImportedFunction(p, u);

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

