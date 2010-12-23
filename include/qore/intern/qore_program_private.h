/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_program_private.h

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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

#ifndef _QORE_QORE_PROGRAM_PRIVATE_H
#define _QORE_QORE_PROGRAM_PRIVATE_H

extern QoreListNode *ARGV, *QORE_ARGV;
extern QoreHashNode *ENV;

#include <qore/intern/ParserSupport.h>
#include <qore/intern/UserFunctionList.h>
#include <qore/intern/GlobalVariableList.h>
#include <qore/intern/ImportedFunctionList.h>

#include <errno.h>

class CharPtrList : public safe_dslist<const char *> {
public:
   // returns true for found, false for not found
   // FIXME: use STL find algorithm
   DLLLOCAL bool find(const char *str) const {
      const_iterator i = begin();
      while (i != end()) {
	 if (!strcmp(*i, str))
	    return true;
	 i++;
      }
   
      return false;
   }
};

// local variable container
typedef safe_dslist<LocalVar *> local_var_list_t;

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

class ProgramThreadCountHelper {
private:
   QoreProgram *pgm;
public:
   DLLLOCAL ProgramThreadCountHelper(QoreProgram *n_pgm) : pgm(n_pgm) {
      pgm->tc_inc();
   }
   DLLLOCAL ~ProgramThreadCountHelper() {
      pgm->tc_dec();
   }
};

struct qore_program_private {
   UserFunctionList user_func_list;
   ImportedFunctionList imported_func_list;
   GlobalVariableList global_var_list;
   LocalVariableList local_var_list;

   // for the thread counter
   QoreCondition tcond;
   unsigned thread_count;

   // to save file names for later deleting
   cstr_vector_t fileList;
   // features present in this Program object
   CharPtrList featureList;
      
   // parse lock, making parsing actions atomic and thread-safe
   mutable QoreThreadLock plock;
   // depedency counter, when this hits zero, the object is deleted
   QoreReferenceCounter dc;
   ExceptionSink *parseSink, *warnSink;
   RootQoreNamespace *RootNS;
   QoreNamespace *QoreNS;

   // top level statements
   TopLevelStatementBlock sb;
      
   // parsing
   bool only_first_except;
   int exceptions_raised;

   ParseWarnOptions pwo;

   std::string exec_class_name, script_dir, script_path, script_name,
      include_path;
   bool po_locked, exec_class, base_object, requires_exception;

   // thread-local data (could be inherited from another program)
   qpgm_thread_local_storage_t *thread_local_storage;

   // time zone setting for the program
   const AbstractQoreZoneInfo *TZ;

   QoreProgram *pgm;

   DLLLOCAL qore_program_private(QoreProgram *n_pgm, int64 n_parse_options, const AbstractQoreZoneInfo *n_TZ = QTZM.getLocalZoneInfo()) : thread_count(0), pwo(n_parse_options), TZ(n_TZ), pgm(n_pgm) {
      printd(5, "QoreProgram::QoreProgram() (init()) this=%p\n", this);
      only_first_except = false;
      exceptions_raised = 0;
#ifdef DEBUG
      parseSink = 0;
#endif
      warnSink = 0;
      requires_exception = false;
	 
      // initialize global vars
      Var *var = global_var_list.newVar("ARGV", listTypeInfo);
      if (ARGV)
	 var->setValue(ARGV->copy(), 0);
	 
      var = global_var_list.newVar("QORE_ARGV", listTypeInfo);
      if (QORE_ARGV)
	 var->setValue(QORE_ARGV->copy(), 0);
	 
      var = global_var_list.newVar("ENV", hashTypeInfo);
      var->setValue(ENV->copy(), 0);
   }

   DLLLOCAL ~qore_program_private() {
   }

   // for independent programs (not inherited from another QoreProgram object)
   DLLLOCAL void new_program() {
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
   }

   DLLLOCAL void incThreadCount() {
      // grab program-level lock
      AutoLocker al(&plock);
      ++thread_count;
   }

   DLLLOCAL void decThreadCount() {
      // grab program-level lock
      AutoLocker al(&plock);
      assert(thread_count > 0);
      --thread_count;
      if (!thread_count)
	 tcond.broadcast();
   }

   DLLLOCAL void waitForAllThreadsToTerminate() {
      // grab program-level lock
      AutoLocker al(&plock);
      while (true) {
	 if (!thread_count)
	    return;
	 tcond.wait(plock);
      }
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

   DLLLOCAL QoreListNode *getVarList() {
      AutoLocker al(&plock);
      return global_var_list.getVarList();
   }

   DLLLOCAL QoreListNode *getFeatureList() const {
      QoreListNode *l = new QoreListNode();
	 
      for (CharPtrList::const_iterator i = featureList.begin(), e = featureList.end(); i != e; ++i)
	 l->push(new QoreStringNode(*i));
	 
      return l;
   }

   DLLLOCAL void internParseRollback();

   // call must push the current program on the stack and pop it afterwards
   DLLLOCAL int internParsePending(const char *code, const char *label) {
      printd(5, "QoreProgram::internParsePending(code=%p, label=%s)\n", code, label);
	 
      if (!(*code))
	 return 0;

      // save this file name for storage in the parse tree and deletion
      // when the QoreProgram object is deleted
      char *sname = strdup(label);
      fileList.push_back(sname);
      beginParsing(sname);

      // no need to save buffer, because it's deleted automatically in lexer
      //printd(5, "QoreProgram::internParsePending() parsing tag=%s (%p): '%s'\n", label, label, code);

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

   DLLLOCAL int parsePending(const char *code, const char *label, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
      //printd(5, "qore_program_private::parsePending() wm=0x%x UV=0x%x on=%d\n", wm, QP_WARN_UNREFERENCED_VARIABLE, wm & QP_WARN_UNREFERENCED_VARIABLE);

      // grab program-level parse lock
      AutoLocker al(&plock);
      warnSink = wS;
      pwo.warn_mask = wm;

      parseSink = xsink;
      int rc = internParsePending(code, label);
      warnSink = 0;
#ifdef DEBUG
      parseSink = 0;
#endif
      return rc;
   }

   // caller must have grabbed the lock and put the current program on the program stack
   DLLLOCAL int internParseCommit();

   // checks to see if parseCommit() can be called - updating existing runtime data structures
   DLLLOCAL int checkParseCommitUnlocked(ExceptionSink *xsink) {
      // if no threads are running, return 0
      if (!thread_count)
	 return 0;

      // if one thread is running, and it's the current thread, return 0
      if (thread_count == 1 && getProgram() == pgm)
	 return 0;

      xsink->raiseException("PROGRAM-PARSE-CONFLICT", "cannot execute any operation on a program object that modifies run-time data structures when another thread is currently executing in that program object");
      return -1;
   }

   DLLLOCAL int parseCommit(ExceptionSink *xsink, ExceptionSink *wS, int wm) {
      // grab program-level parse lock
      AutoLocker al(&plock);
      if (checkParseCommitUnlocked(xsink))
	 return -1;

      warnSink = wS;
      pwo.warn_mask = wm;
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

   DLLLOCAL void parseRollback() {
      ProgramContextHelper pch(pgm, 0);

      // grab program-level parse lock and release on exit
      AutoLocker al(&plock);
   
      // back out all pending changes
      internParseRollback();
   }

   DLLLOCAL void parse(FILE *fp, const char *name, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
      printd(5, "QoreProgram::parse(fp=%p, name=%s, xsink=%p, wS=%p, wm=%d)\n", fp, name, xsink, wS, wm);

      // if already at the end of file, then return
      // try to get one character from file
      int c = fgetc(fp);
      if (feof(fp)) {
	 printd(5, "QoreProgram::parse(fp=%p, name=%s) EOF\n", fp, name);
	 return;
      }
      // push back read character
      ungetc(c, fp);

      yyscan_t lexer;
	 
      // grab program-level parse lock
      {
	 AutoLocker al(&plock);
	 if (checkParseCommitUnlocked(xsink))
	    return;
	 
	 warnSink = wS;
	 pwo.warn_mask = wm;
	 parseSink = xsink;
	 
	 // save this file name for storage in the parse tree and deletion
	 // when the QoreProgram object is deleted
	 char *sname = strdup(name);
	 fileList.push_back(sname);
	 beginParsing(sname);
	 
	 ProgramContextHelper pch(pgm, xsink);
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

   DLLLOCAL void parse(const QoreString *str, const QoreString *lstr, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
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

   DLLLOCAL void parse(const char *code, const char *label, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
      if (!(*code))
	 return;

      ProgramContextHelper pch(pgm, xsink);
   
      // grab program-level parse lock
      AutoLocker al(&plock);
      if (checkParseCommitUnlocked(xsink))
	 return;

      warnSink = wS;
      pwo.warn_mask = wm;
      parseSink = xsink;

      // parse text given
      if (!internParsePending(code, label))
	 internParseCommit();   // finalize parsing, back out or commit all changes

#ifdef DEBUG
      parseSink = 0;
#endif
      warnSink = 0;
   }

   DLLLOCAL void parseFile(const char *filename, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
      QORE_TRACE("QoreProgram::parseFile()");

      printd(5, "QoreProgram::parseFile(%s)\n", filename);

      FILE *fp;
      if (!(fp = fopen(filename, "r"))) {
	 if ((only_first_except && !exceptions_raised) || !only_first_except)
	    xsink->raiseErrnoException("PARSE-EXCEPTION", errno, "cannot open qore script '%s'", filename);
	 exceptions_raised++;
	 return;
      }
      setScriptPath(filename);
	 
      ON_BLOCK_EXIT(fclose, fp);
      
      parse(fp, filename, xsink, wS, wm);
   }
   
   DLLLOCAL void parsePending(const QoreString *str, const QoreString *lstr, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
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

      ProgramContextHelper pch(pgm, xsink);
      
      parsePending(tstr->getBuffer(), tlstr->getBuffer(), xsink, wS, wm);
   }

   // called during run time (not during parsing)
   DLLLOCAL void importUserFunction(QoreProgram *p, UserFunction *u, ExceptionSink *xsink);

   // called during run time (not during parsing)
   DLLLOCAL void importUserFunction(QoreProgram *p, UserFunction *u, const char *new_name, ExceptionSink *xsink);

   DLLLOCAL void del(ExceptionSink *xsink);

   DLLLOCAL QoreHashNode *clearThreadData(ExceptionSink *xsink) {
      QoreHashNode *h = thread_local_storage->get();
      printd(5, "QoreProgram::clearThreadData() this=%p h=%p (size=%d)\n", this, h, h->size());
      h->clear(xsink);
      return h;
   }

   DLLLOCAL void endThread(ExceptionSink *xsink) {
      // delete thread local storage data
      QoreHashNode *h = clearThreadData(xsink);
      h->deref(xsink);
   }

   DLLLOCAL void start_thread() {
      assert(!thread_local_storage->get());
      thread_local_storage->set(new QoreHashNode);
   }

   DLLLOCAL const AbstractQoreZoneInfo *currentTZ() const {
      return TZ;
   }

   DLLLOCAL void setTZ(const AbstractQoreZoneInfo *n_TZ) {
      TZ = n_TZ;
   }

   DLLLOCAL UserFunction *findUserImportedFunctionUnlocked(const char *name, QoreProgram *&ipgm);

   DLLLOCAL void exportUserFunction(const char *name, qore_program_private *p, ExceptionSink *xsink) {
      if (this == p) {
	 xsink->raiseException("PROGRAM-IMPORTFUNCTION-PARAMETER-ERROR", "cannot import a function from the same Program object");
	 return;
      }

      UserFunction *u;
      QoreProgram *ipgm = pgm;

      {
	 AutoLocker al(plock);
	 u = findUserImportedFunctionUnlocked(name, ipgm);
      }

      if (!u)
	 xsink->raiseException("PROGRAM-IMPORTFUNCTION-NO-FUNCTION", "function \"%s\" does not exist in the current program scope", name);
      else
	 p->importUserFunction(ipgm, u, xsink);
   }

   DLLLOCAL void exportUserFunction(const char *name, const char *new_name, qore_program_private *p, ExceptionSink *xsink) {
      if (this == p) {
	 xsink->raiseException("PROGRAM-IMPORTFUNCTION-PARAMETER-ERROR", "cannot import a function from the same Program object");
	 return;
      }

      UserFunction *u;
      QoreProgram *ipgm = pgm;

      {
	 AutoLocker al(plock);
	 u = findUserImportedFunctionUnlocked(name, ipgm);
      }

      if (!u)
	 xsink->raiseException("PROGRAM-IMPORTFUNCTION-NO-FUNCTION", "function \"%s\" does not exist in the current program scope", name);
      else
	 p->importUserFunction(ipgm, u, new_name, xsink);
   }

   DLLLOCAL bool parseExceptionRaised() const {
      assert(parseSink);
      return *parseSink;
   }

   DLLLOCAL static const ParseWarnOptions &getParseWarnOptions(const QoreProgram *pgm) {
      return pgm->priv->pwo;
   }

   DLLLOCAL static void setSaveParseWarnOptions(const QoreProgram *pgm, const ParseWarnOptions &new_opts, ParseWarnOptions &old_opts) {
      old_opts = pgm->priv->pwo;
      pgm->priv->pwo = new_opts;
   }

   DLLLOCAL static void setParseWarnOptions(const QoreProgram *pgm, const ParseWarnOptions &new_opts) {
      pgm->priv->pwo = new_opts;
   }
};

class ParseWarnHelper : public ParseWarnOptions {
public:
   DLLLOCAL ParseWarnHelper(const ParseWarnOptions &new_opts) {
      QoreProgram *pgm = getProgram();
      if (pgm)
	 qore_program_private::setSaveParseWarnOptions(pgm, new_opts, *this);
   }
   DLLLOCAL ~ParseWarnHelper() {
      QoreProgram *pgm = getProgram();
      if (pgm)
	 qore_program_private::setParseWarnOptions(getProgram(), *this);
   }
};

#endif
