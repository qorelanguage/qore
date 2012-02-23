/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_program_private.h

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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
#include <qore/intern/GlobalVariableList.h>

#include <stdarg.h>
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

class ThreadLocalVariableData : public ThreadLocalData<LocalVarValue> {
public:
   // marks all variables as finalized on the stack
   DLLLOCAL void finalize(ExceptionSink *xsink) {
      ThreadLocalVariableData::iterator i(curr);
      while (i.next())
         i.get().finalize(xsink);
   }

   // deletes everything on the stack
   DLLLOCAL void del(ExceptionSink *xsink) {
      // then we uninstantiate
      while (curr->prev || curr->pos)
         uninstantiate(xsink);
   }

   DLLLOCAL LocalVarValue *instantiate() {
      if (curr->pos == QORE_THREAD_STACK_BLOCK) {
	 if (curr->next)
	    curr = curr->next;
	 else {
	    curr->next = new Block(curr);
	    //printf("this=%p: add curr=%p, curr->next=%p\n", this, curr, curr->next);
	    curr = curr->next;
	 }
      }
      return &curr->var[curr->pos++];
   }

   DLLLOCAL void uninstantiate(ExceptionSink *xsink) {
      if (!curr->pos) {
	 if (curr->next) {
	    //printf("this %p: del curr=%p, curr->next=%p\n", this, curr, curr->next);
	    delete curr->next;
	    curr->next = 0;
	 }
	 curr = curr->prev;
      }
      curr->var[--curr->pos].uninstantiate(xsink);
   }

   DLLLOCAL LocalVarValue *find(const char *id) {
      Block *w = curr;
      while (true) {
	 int p = w->pos;
	 while (p) {
	    if (w->var[--p].id == id && !w->var[p].skip)
	       return &w->var[p];
	 }
	 w = w->prev;
#ifdef DEBUG
	 if (!w) {
            printd(0, "ThreadLocalVariableData::find() this=%p no local variable '%s' (%p) on stack (pgm=%p) p=%d\n", this, id, id, getProgram(), p);
            p = curr->pos - 1;
            while (p >= 0) {
               printd(0, "var p=%d: %s (%p) (skip=%d)\n", p, curr->var[p].id, curr->var[p].id, curr->var[p].skip);
               --p;
            }
         }
#endif
	 assert(w);
      }
      // to avoid a warning on most compilers - note that this generates a warning on recent versions of aCC!
      return 0;
   }
};

class ThreadClosureVariableStack : public ThreadLocalData<ClosureVarValue*> {
private:
   DLLLOCAL void instantiate(ClosureVarValue *cvar) {
      //printd(5, "ThreadClosureVariableStack::instantiate(%p = %s) this=%p pgm=%p\n", cvar->id, cvar->id, this, getProgram());

      if (curr->pos == QORE_THREAD_STACK_BLOCK) {
	 if (curr->next)
	    curr = curr->next;
	 else {
	    curr->next = new Block(curr);
	    //printf("this=%p: add curr=%p, curr->next=%p\n", this, curr, curr->next);
	    curr = curr->next;
	 }
      }
      curr->var[curr->pos++] = cvar;
   }

public:
   // marks all variables as finalized on the stack
   DLLLOCAL void finalize(ExceptionSink *xsink) {
      ThreadClosureVariableStack::iterator i(curr);
      while (i.next())
         i.get()->finalize(xsink);
   }

   // deletes everything on the stack
   DLLLOCAL void del(ExceptionSink *xsink) {
      while (curr->prev || curr->pos)
         uninstantiate(xsink);
   }

   DLLLOCAL ClosureVarValue *instantiate(const char *id, AbstractQoreNode *value) {
      ClosureVarValue *cvar = new ClosureVarValue(id, value);
      instantiate(cvar);
      return cvar;
   }

   DLLLOCAL ClosureVarValue *instantiate(const char *id, AbstractQoreNode *vexp, QoreObject *obj, QoreProgram *pgm) {
      ClosureVarValue *cvar = new ClosureVarValue(id, vexp, obj, pgm);
      instantiate(cvar);
      return cvar;
   }

   DLLLOCAL void uninstantiate(ExceptionSink *xsink) {
#ifdef DEBUG_1
      if (!curr->pos)
         printd(5, "ThreadClosureVariableStack::uninstantiate() this=%p pos=%d %p %s\n", this, curr->prev->pos - 1, curr->prev->var[curr->prev->pos - 1]->id, curr->prev->var[curr->prev->pos - 1]->id);
      else
         printd(5, "ThreadClosureVariableStack::uninstantiate() this=%p pos=%d %p %s\n", this, curr->pos - 1, curr->var[curr->pos - 1]->id, curr->var[curr->pos - 1]->id);
#endif
      if (!curr->pos) {
	 if (curr->next) {
	    //printf("this %p: del curr=%p, curr->next=%p\n", this, curr, curr->next);
	    delete curr->next;
	    curr->next = 0;
	 }
	 curr = curr->prev;
      }
      curr->var[--curr->pos]->deref(xsink);
   }

   DLLLOCAL ClosureVarValue *find(const char *id) {
      Block *w = curr;
      while (true) {
	 int p = w->pos;
	 while (p) {
	    if (w->var[--p]->id == id && !w->var[p]->skip)
	       return w->var[p];
	 }
	 w = w->prev;
#ifdef DEBUG
	 if (!w) {
            printd(0, "ThreadClosureVariableStack::find() this=%p no local variable '%s' (%p) on stack (pgm=%p) p=%d curr->prev=%p\n", this, id, id, getProgram(), p, curr->prev);
            p = curr->pos - 1;
            while (p >= 0) {
               printd(0, "var p=%d: %s (%p) (skip=%d)\n", p, curr->var[p]->id, curr->var[p]->id, curr->var[p]->skip);
               --p;
            }
         }
#endif
	 assert(w);
      }
      // to avoid a warning on most compilers - note that this generates a warning on recent versions of aCC!
      return 0;
   }
};

struct ThreadLocalProgramData {
private:
   // not implemented
   DLLLOCAL ThreadLocalProgramData(const ThreadLocalProgramData &old);

public:
   // local variable data slots
   ThreadLocalVariableData lvstack;
   // closure variable stack
   ThreadClosureVariableStack cvstack;

   // top-level vars instantiated
   bool inst;

   DLLLOCAL ThreadLocalProgramData() : inst(false) {
      //printd(5, "ThreadLocalProgramData::ThreadLocalProgramData() this=%p\n", this);
   }

   DLLLOCAL ~ThreadLocalProgramData() {
      assert(lvstack.empty());
      assert(cvstack.empty());
   }

   DLLLOCAL void finalize(ExceptionSink *xsink) {
      lvstack.finalize(xsink);
      cvstack.finalize(xsink);
   }

   DLLLOCAL void del(ExceptionSink *xsink) {
      lvstack.del(xsink);
      cvstack.del(xsink);
      delete this;
   }
};

// maps from thread handles to thread-local data
typedef std::map<ThreadProgramData *, ThreadLocalProgramData *> pgm_data_map_t;

// map for "defines" in programs
typedef std::map<std::string, AbstractQoreNode *> dmap_t;

// map for pushed parse options
typedef std::map<const char*, int64> ppo_t;

struct qore_program_private_base {
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
   ExceptionSink *parseSink, *warnSink, *pendingParseSink;
   RootQoreNamespace *RootNS;
   QoreNamespace *QoreNS;

   // top level statements
   TopLevelStatementBlock sb;
      
   // parsing
   bool only_first_except;
   int exceptions_raised;

   ParseWarnOptions pwo;

   std::string exec_class_name, script_dir, script_path, script_name, include_path;
   bool po_locked, po_allow_restrict, exec_class, base_object, requires_exception;

   // thread-local data (could be inherited from another program)
   qpgm_thread_local_storage_t *thread_local_storage;

   // thread variable data lock, for accessing the thread variable data map
   mutable QoreThreadLock tlock;

   // thread-local variable storage - map from thread ID to thread-local storage
   pgm_data_map_t pgm_data_map;

   // time zone setting for the program
   const AbstractQoreZoneInfo *TZ;

   // define map
   dmap_t dmap;

   // pushed parse option map
   ppo_t ppo;

   QoreProgram *pgm;

   DLLLOCAL qore_program_private_base(QoreProgram *n_pgm, int64 n_parse_options, QoreProgram *p_pgm = 0) 
      : thread_count(0), plock(&ma_recursive), parseSink(0), warnSink(0), pendingParseSink(0), RootNS(0), QoreNS(0), only_first_except(false),
        exceptions_raised(0), pwo(n_parse_options), po_locked(false), po_allow_restrict(true), exec_class(false), base_object(false),
        requires_exception(false), thread_local_storage(0), pgm(n_pgm) {
      printd(5, "qore_program_private::init() this=%p pgm=%p\n", this, pgm);
	 
      // initialize global vars
      Var *var = global_var_list.newVar("ARGV", listTypeInfo);
      if (ARGV)
	 var->setValue(ARGV->copy(), 0);
	 
      var = global_var_list.newVar("QORE_ARGV", listTypeInfo);
      if (QORE_ARGV)
	 var->setValue(QORE_ARGV->copy(), 0);
	 
      var = global_var_list.newVar("ENV", hashTypeInfo);
      var->setValue(ENV->copy(), 0);

      if (p_pgm)
	 setParent(p_pgm, n_parse_options);
      else {
	 TZ = QTZM.getLocalZoneInfo();
	 newProgram();
      }
   }

   DLLLOCAL void start_thread() {
      assert(!thread_local_storage->get());
      thread_local_storage->set(new QoreHashNode);
   }

protected:
   DLLLOCAL void setParent(QoreProgram *p_pgm, int64 n_parse_options);

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

struct qore_program_private : public qore_program_private_base {
private:
   DLLLOCAL void init(QoreProgram *n_pgm, int64 n_parse_options, const AbstractQoreZoneInfo *n_TZ = QTZM.getLocalZoneInfo()) {
   }

public:
   DLLLOCAL qore_program_private(QoreProgram *n_pgm, int64 n_parse_options) : qore_program_private_base(n_pgm, n_parse_options) {
      printd(5, "qore_program_private::qore_program_private() this=%p pgm=%p\n", this, pgm);
   }

   DLLLOCAL qore_program_private(QoreProgram *n_pgm, int64 n_parse_options, QoreProgram *p_pgm) : qore_program_private_base(n_pgm, n_parse_options, p_pgm) {
      printd(5, "qore_program_private::qore_program_private() this=%p pgm=%p\n", this, pgm);
   }
   
   DLLLOCAL ~qore_program_private() {
      printd(5, "qore_program_private::~qore_program_private() this=%p pgm=%p\n", this, pgm);
      assert(!parseSink);
      assert(!warnSink);
      assert(!pendingParseSink);
      assert(pgm_data_map.empty());
   }

   DLLLOCAL void depDeref(ExceptionSink *xsink) {
      printd(5, "qore_program_private::depDeref() this=%p pgm=%p %d->%d\n", this, pgm, dc.reference_count(), dc.reference_count() - 1);
      if (dc.ROdereference()) {
         del(xsink);
         delete pgm;
      }
   }

   DLLLOCAL void clearProgramThreadData(ExceptionSink *xsink) {
      // delete local variables for all threads that have used this program
      pgm_data_map_t pdm_copy;
      // copy the map and run on the copy to avoid deadlocks
      {
         AutoLocker al(tlock);
         pdm_copy = pgm_data_map;
      }
      for (pgm_data_map_t::iterator i = pdm_copy.begin(), e = pdm_copy.end(); i != e; ++i) {
         i->second->finalize(xsink);
      }

      for (pgm_data_map_t::iterator i = pdm_copy.begin(), e = pdm_copy.end(); i != e; ++i) {
         i->second->del(xsink);
         i->first->delProgram(pgm);
      }
      
      // now clear the original map
      {
         AutoLocker al(tlock);
         assert(pgm_data_map.size() == pdm_copy.size());
         pgm_data_map.clear();
      }
   }

   // called when the program's ref count = 0 (but the dc count may not go to 0 yet)
   DLLLOCAL void clear(ExceptionSink *xsink) {
      // wait for all threads to terminate
      waitForAllThreadsToTerminate();

      // merge pending parse exceptions into the passed exception sink, if any
      if (pendingParseSink) {
         xsink->assimilate(pendingParseSink);
         pendingParseSink = 0;
      }

      // delete all global variables
      global_var_list.clear_all(xsink);
      // clear thread data if base object
      if (base_object)
         clearThreadData(xsink);

      clearProgramThreadData(xsink);
      
      depDeref(xsink);
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
      while (thread_count)
	 tcond.wait(&plock);
   }

   DLLLOCAL const char *parseGetScriptPath() const {
      return script_path.empty() ? 0 : script_path.c_str();
   }

   DLLLOCAL const char *parseGetScriptDir() const {
      return script_dir.empty() ? 0 : script_dir.c_str();
   }

   DLLLOCAL const char *parseGetScriptName() const {
      return script_name.empty() ? 0 : script_name.c_str();
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

   DLLLOCAL void startParsing(ExceptionSink *xsink, ExceptionSink *wS, int wm) {
      warnSink = wS;
      pwo.warn_mask = wm;
      parseSink = xsink;

      if (pendingParseSink) {
         parseSink->assimilate(pendingParseSink);
         pendingParseSink = 0;
      }
   }

   DLLLOCAL int parsePending(const char *code, const char *label, ExceptionSink *xsink, ExceptionSink *wS, int wm) {
      //printd(5, "qore_program_private::parsePending() wm=0x%x UV=0x%x on=%d\n", wm, QP_WARN_UNREFERENCED_VARIABLE, wm & QP_WARN_UNREFERENCED_VARIABLE);

      // grab program-level parse lock
      AutoLocker al(&plock);

      startParsing(xsink, wS, wm);

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

   DLLLOCAL void parseRollback() {
      ProgramContextHelper pch(pgm, false);

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
	 
         startParsing(xsink, wS, wm);
	 
	 // save this file name for storage in the parse tree and deletion
	 // when the QoreProgram object is deleted
	 char *sname = strdup(name);
	 fileList.push_back(sname);
	 beginParsing(sname);
	 
	 ProgramContextHelper pch(pgm, false);
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

      ProgramContextHelper pch(pgm, false);
   
      // grab program-level parse lock
      AutoLocker al(&plock);
      if (checkParseCommitUnlocked(xsink))
	 return;

      startParsing(xsink, wS, wm);

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

      ProgramContextHelper pch(pgm, false);
      
      parsePending(tstr->getBuffer(), tlstr->getBuffer(), xsink, wS, wm);
   }

   // called during run time (not during parsing)
   DLLLOCAL void importUserFunction(QoreProgram *p, UserFunction *u, ExceptionSink *xsink);

   // called during run time (not during parsing)
   DLLLOCAL void importUserFunction(QoreProgram *p, UserFunction *u, const char *new_name, ExceptionSink *xsink);

   DLLLOCAL void del(ExceptionSink *xsink);

   DLLLOCAL QoreHashNode *clearThreadData(ExceptionSink *xsink) {
      QoreHashNode *h = thread_local_storage->get();
      printd(5, "QoreProgram::clearThreadData() this=%p h=%p (size=%d)\n", this, h, h ? h->size() : 0);
      if (h) 
         h->clear(xsink);
      return h;
   }

   DLLLOCAL void deleteThreadData(ExceptionSink *xsink) {
      QoreHashNode *h = clearThreadData(xsink);
      if (h) {
         h->deref(xsink);
         thread_local_storage->set(0);
      }
   }

   DLLLOCAL void endThread(ThreadProgramData *td, ExceptionSink *xsink) {
      //printd(5, "qore_program_private::endThread() this=%p pgm=%p\n", this, pgm);
      // delete thread local storage data
      deleteThreadData(xsink);

      // delete all local variables for this thread
      SafeLocker sl(tlock);
      pgm_data_map_t::iterator i = pgm_data_map.find(td);
      if (i != pgm_data_map.end()) {
         // remove from map and delete outside of lock to avoid deadlocks (or the need for recursive locking)
         ThreadLocalProgramData *tlpd = i->second;
         sl.unlock();
         tlpd->finalize(xsink);
         sl.lock();
         pgm_data_map.erase(i);
         sl.unlock();
         tlpd->del(xsink);
      }
   }

   DLLLOCAL void doTopLevelInstantiation(ThreadLocalProgramData &tlpd) {
      // instantiate top-level vars for this thread
      const LVList *lvl = sb.getLVList();
      if (lvl)
         for (unsigned i = 0; i < lvl->size(); ++i)
            lvl->lv[i]->instantiate();

      //printd(5, "qore_program_private::doTopLevelInstantiation() lvl=%p setup %ld local vars pgm=%p\n", lvl, lvl ? lvl->size() : 0, getProgram());

      tlpd.inst = true;
   }

   // returns true if setting for the first time, false if not
   DLLLOCAL bool setThreadVarData(ThreadProgramData *td, ThreadLocalVariableData *&lvstack, ThreadClosureVariableStack *&cvstack, bool run) {
      SafeLocker sl(tlock);

      pgm_data_map_t::iterator i = pgm_data_map.find(td);
      if (i == pgm_data_map.end()) {
         ThreadLocalProgramData *tlpd = new ThreadLocalProgramData;

         //printd(5, "qore_program_private::setThreadVarData() (first) this=%p pgm=%p td=%p run=%s inst=%s\n", this, pgm, td, run ? "true" : "false", tlpd->inst ? "true" : "false");

         lvstack = &tlpd->lvstack;
         cvstack = &tlpd->cvstack;

         pgm_data_map.insert(pgm_data_map_t::value_type(td, tlpd));

         sl.unlock();

         if (run) {
            //printd(5, "qore_program_private::setThreadVarData() (first) this=%p pgm=%p td=%p\n", this, pgm, td);
            doTopLevelInstantiation(*tlpd);
         }

         return true;
      }
      
      ThreadLocalProgramData *tlpd = pgm_data_map[td];
      lvstack = &tlpd->lvstack;
      cvstack = &tlpd->cvstack;
      
      sl.unlock();

      //printd(5, "qore_program_private::setThreadVarData() (not first) this=%p pgm=%p td=%p run=%s inst=%s\n", this, pgm, td, run ? "true" : "false", tlpd->inst ? "true" : "false");

      if (run && !tlpd->inst) {
         doTopLevelInstantiation(*tlpd);
      }

      return false;
   }

   DLLLOCAL const AbstractQoreZoneInfo *currentTZ() const {
      return TZ;
   }

   DLLLOCAL void setTZ(const AbstractQoreZoneInfo *n_TZ) {
      TZ = n_TZ;
   }

   DLLLOCAL void exportUserFunction(const char *name, qore_program_private *p, ExceptionSink *xsink) {
      if (this == p) {
	 xsink->raiseException("PROGRAM-IMPORTFUNCTION-PARAMETER-ERROR", "cannot import a function from the same Program object");
	 return;
      }

      UserFunction *u;
      QoreProgram *ipgm = pgm;

      {
	 AutoLocker al(plock);
	 u = qore_ns_private::findUserImportedFunction(*RootNS, name, ipgm);
      }

      if (!u)
	 xsink->raiseException("PROGRAM-IMPORTFUNCTION-NO-FUNCTION", "function '%s' does not exist in the current program scope", name);
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
	 u = qore_ns_private::findUserImportedFunction(*RootNS, name, ipgm);
      }

      if (!u)
	 xsink->raiseException("PROGRAM-IMPORTFUNCTION-NO-FUNCTION", "function '%s' does not exist in the current program scope", name);
      else
	 p->importUserFunction(ipgm, u, new_name, xsink);
   }

   DLLLOCAL bool parseExceptionRaised() const {
      assert(parseSink);
      return *parseSink;
   }

   DLLLOCAL void setParseOptions(int64 po, ExceptionSink *xsink = 0) {
      // only raise the exception if parse options are locked and the option is not a "free option"
      // also check if options may be made more restrictive and the option also does so
      if (!(po & PO_FREE_OPTIONS) && po_locked && (!po_allow_restrict || (po & PO_POSITIVE_OPTIONS))) {
         if (xsink)
            xsink->raiseException("OPTIONS-LOCKED", "parse options have been locked on this program object");
         else
            parse_error("parse options have been locked on this program object");
         return;
      }
      pwo.parse_options |= po;
   }
   
   DLLLOCAL void disableParseOptions(int64 po, ExceptionSink* xsink = 0) {
      // only raise the exception if parse options are locked and the option is not a "free option"
      // also check if options may be made more restrictive and the option also does so
      if (po_locked && (!po_allow_restrict || !(po & PO_POSITIVE_OPTIONS))) {
         if (xsink)
            xsink->raiseException("OPTIONS-LOCKED", "parse options have been locked on this program object");
         else
            parse_error("parse options have been locked on this program object");
         return;
      }
      pwo.parse_options &= ~po;
   }

   DLLLOCAL void replaceParseOptions(int64 po, ExceptionSink *xsink) {
      if (!(getProgram()->priv->pwo.parse_options & PO_NO_CHILD_PO_RESTRICTIONS)) {
         xsink->raiseException("OPTION-ERROR", "the calling Program does not have the PO_NO_CHILD_PO_RESTRICTIONS option set, and therefore cannot call Program::replaceParseOptions()");
         return;
      }
      
      pwo.parse_options = po;
   }

   DLLLOCAL void mergeParseException(ExceptionSink &xsink) {
      if (parseSink)
         parseSink->assimilate(xsink);
      else {
         // grab program-level parse lock if we are not already parsing; just in case
         AutoLocker al(&plock);
         
         if (!pendingParseSink)
            pendingParseSink = new ExceptionSink;
         pendingParseSink->assimilate(xsink);
      }
   }

   DLLLOCAL void parseSetTimeZone(const char *zone) {
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

   DLLLOCAL void makeParseException(const char *err, QoreStringNode *desc) {
      QoreStringNodeHolder d(desc);
      if (!requires_exception) {
         assert(parseSink);
         QoreException *ne = new ParseException(err, d.release());
         parseSink->raiseException(ne);
      }
   }

   DLLLOCAL void makeParseException(QoreStringNode *desc) {
      QoreStringNodeHolder d(desc);
      if (!requires_exception) {
         QoreException *ne = new ParseException("PARSE-EXCEPTION", d.release());
         if ((only_first_except && !exceptions_raised) || !only_first_except)
            parseSink->raiseException(ne);
         exceptions_raised++;
      }
   }

   DLLLOCAL void makeParseException(const QoreProgramLocation &loc, const char *err, QoreStringNode *desc) {
      QORE_TRACE("QoreProgram::makeParseException()");

      QoreStringNodeHolder d(desc);
      if (!requires_exception) {
         QoreException *ne = new ParseException(loc.start_line, loc.end_line, loc.file, err, d.release());
         if ((only_first_except && !exceptions_raised) || !only_first_except)
            parseSink->raiseException(ne);
         exceptions_raised++;
      }
   }

   DLLLOCAL void parseException(QoreProgramLocation &loc, const char *fmt, ...) {
      //printd(5, "qore_program_private::parseException(\"%s\", ...) called\n", fmt);

      // ignore if a "requires" exception has been raised
      if (requires_exception)
         return;

      QoreStringNode *desc = new QoreStringNode;
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
   DLLLOCAL bool setDefine(const char *name, AbstractQoreNode *v, ExceptionSink *xsink) {
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
   DLLLOCAL const AbstractQoreNode *getDefine(const char *name, bool &is_defined) {
      dmap_t::iterator i = dmap.find(name);
      if (i != dmap.end()) {
         is_defined = true;
         return i->second;
      }
      is_defined = false;
      return 0;
   }

   DLLLOCAL AbstractQoreNode *runTimeGetDefine(const char *name, bool &is_defined) {
      AutoLocker al(plock);
      const AbstractQoreNode *rv = getDefine(name, is_defined);
      return rv ? rv->refSelf() : 0;
   }

   // internal method - does not bother with the parse lock
   // returns true if the define existed
   DLLLOCAL bool unDefine(const char *name, ExceptionSink *xsink) {
      dmap_t::iterator i = dmap.find(name);
      if (i != dmap.end()) {
         if (i->second)
            i->second->deref(xsink);
         dmap.erase(i);
         return true;
      }
      return false;
   }

   DLLLOCAL bool parseUnDefine(const char *name) {
      PreParseHelper pph(this);
      return unDefine(name, parseSink);
   }

   DLLLOCAL bool runTimeUnDefine(const char *name, ExceptionSink *xsink) {
      AutoLocker al(plock);
      return unDefine(name, xsink);
   }

   // internal method - does not bother with the parse lock
   DLLLOCAL bool isDefined(const char *name) {
      dmap_t::iterator i = dmap.find(name);
      return i == dmap.end() ? false : true;
   }

   DLLLOCAL bool runTimeIsDefined(const char *name) {
      AutoLocker al(plock);
      return isDefined(name);
   }

   DLLLOCAL int checkDefine(const QoreProgramLocation &loc, const char *str, ExceptionSink *xsink) {
      const char *p = str;
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

   DLLLOCAL void parseDefine(const QoreProgramLocation &loc, const char *str, AbstractQoreNode *val) {
      PreParseHelper pph(this);

      if (checkDefine(loc, str, parseSink))
         return;

      setDefine(str, val, parseSink);
   }

   DLLLOCAL void runTimeDefine(const char *str, AbstractQoreNode *val, ExceptionSink *xsink) {
      QoreProgramLocation loc(RunTimeLocation);

      if (checkDefine(loc, str, xsink))
         return;

      AutoLocker al(plock);
      setDefine(str, val, xsink);
   }

   DLLLOCAL static const ParseWarnOptions &getParseWarnOptions(const QoreProgram *pgm) {
      return pgm->priv->pwo;
   }

   DLLLOCAL static bool setSaveParseWarnOptions(const QoreProgram *pgm, const ParseWarnOptions &new_opts, ParseWarnOptions &old_opts) {
      if (new_opts == pgm->priv->pwo)
         return false;
      old_opts = pgm->priv->pwo;
      pgm->priv->pwo = new_opts;
      return true;
   }

   DLLLOCAL static void setParseWarnOptions(const QoreProgram *pgm, const ParseWarnOptions &new_opts) {
      pgm->priv->pwo = new_opts;
   }

   DLLLOCAL static bool setThreadVarData(QoreProgram *pgm, ThreadProgramData *td, ThreadLocalVariableData *&lvstack, ThreadClosureVariableStack *&cvstack, bool run) {
      return pgm->priv->setThreadVarData(td, lvstack, cvstack, run);
   }

   DLLLOCAL static void endThread(QoreProgram *pgm, ThreadProgramData *td, ExceptionSink *xsink) {
      pgm->priv->endThread(td, xsink);
   }

   DLLLOCAL static void makeParseException(QoreProgram *pgm, const char *err, QoreStringNode *desc) {
      pgm->priv->makeParseException(err, desc);
   }

   DLLLOCAL static void makeParseException(QoreProgram *pgm, QoreStringNode *desc) {
      pgm->priv->makeParseException(desc);
   }

   DLLLOCAL static void makeParseException(QoreProgram *pgm, const QoreProgramLocation &loc, QoreStringNode *desc) {
      pgm->priv->makeParseException(loc, "PARSE-EXCEPTION", desc);
   }

   DLLLOCAL static void makeParseException(QoreProgram *pgm, const QoreProgramLocation &loc, const char *err, QoreStringNode *desc) {
      pgm->priv->makeParseException(loc, err, desc);
   }

   DLLLOCAL static const AbstractQoreNode *parseGetDefine(QoreProgram *pgm, const char *name) {
      bool is_defined;
      return pgm->priv->getDefine(name, is_defined);
   }

   DLLLOCAL static const AbstractQoreNode *parseGetDefine(QoreProgram *pgm, const char *name, bool &is_defined) {
      return pgm->priv->getDefine(name, is_defined);
   }

   DLLLOCAL static AbstractQoreNode *runTimeGetDefine(QoreProgram *pgm, const char *name) {
      bool is_defined;
      return pgm->priv->runTimeGetDefine(name, is_defined);
   }

   DLLLOCAL static AbstractQoreNode *runTimeGetDefine(QoreProgram *pgm, const char *name, bool &is_defined) {
      return pgm->priv->runTimeGetDefine(name, is_defined);
   }

   DLLLOCAL static bool parseUnDefine(QoreProgram *pgm, const char *name) {
      return pgm->priv->parseUnDefine(name);
   }

   DLLLOCAL static bool runTimeUnDefine(QoreProgram *pgm, const char *name, ExceptionSink *xsink) {
      return pgm->priv->runTimeUnDefine(name, xsink);
   }

   DLLLOCAL static bool parseIsDefined(QoreProgram *pgm, const char *name) {
      return pgm->priv->isDefined(name);
   }

   DLLLOCAL static bool runTimeIsDefined(QoreProgram *pgm, const char *name) {
      return pgm->priv->runTimeIsDefined(name);
   }

   DLLLOCAL static void parseDefine(QoreProgram *pgm, QoreProgramLocation loc, const char *str, AbstractQoreNode *val) {
      pgm->priv->parseDefine(loc, str, val);
   }

   DLLLOCAL static void runTimeDefine(QoreProgram *pgm, const char *str, AbstractQoreNode *val, ExceptionSink *xsink) {
      pgm->priv->runTimeDefine(str, val, xsink);
   }

   DLLLOCAL void pushParseOptions(const char* pf) {
      assert(ppo.find(pf) == ppo.end());
      ppo[pf] = pwo.parse_options;
      //printd(5, "pushParseOptions() %p (%s) saving %lld\n", pf, pf, pwo.parse_options);
   }

   DLLLOCAL void restoreParseOptions(const char* pf) {
      ppo_t::iterator i = ppo.find(pf);
      if (i != ppo.end()) {
         //printd(5, "restoreParseOptions() %p (%s) restoring %lld\n", pf, pf, pwo.parse_options);         
         pwo.parse_options = i->second;
         ppo.erase(i);
      }
   }

   DLLLOCAL static void pushParseOptions(QoreProgram* pgm, const char* pf) {
      pgm->priv->pushParseOptions(pf);
   }

   DLLLOCAL static void restoreParseOptions(QoreProgram* pgm, const char* pf) {
      pgm->priv->restoreParseOptions(pf);
   }
};

class ParseWarnHelper : public ParseWarnOptions {
protected:
   bool restore;
public:
   DLLLOCAL ParseWarnHelper(const ParseWarnOptions &new_opts) {
      QoreProgram *pgm = getProgram();
      restore = pgm ? qore_program_private::setSaveParseWarnOptions(pgm, new_opts, *this) : false;
   }
   DLLLOCAL ~ParseWarnHelper() {
      if (restore)
	 qore_program_private::setParseWarnOptions(getProgram(), *this);
   }
};

#endif
