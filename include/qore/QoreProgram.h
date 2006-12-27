/*
  QoreProgram.h

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

#ifndef _QORE_QOREPROGRAM_H

#define _QORE_QOREPROGRAM_H

#include <qore/config.h>
#include <qore/AbstractPrivateData.h>
#include <qore/LockedObject.h>
#include <qore/Restrictions.h>
#include <qore/QoreCounter.h>
#include <qore/StringList.h>
#include <qore/QoreWarnings.h>
#include <qore/UserFunctionList.h>
#include <qore/GlobalVariableList.h>
#include <qore/ImportedFunctionList.h>

// the two-layered reference counting is to eliminate problems from circular references
// when a program has a global variable that contains an object that references the program...
// objects now reference the dependency counter, so when the object's counter reaches zero and
// the global variable list is deleted, then the variables will in turn dereference the program
// so it can be deleted...

class QoreProgram : public AbstractPrivateData
{
   private:
      class UserFunctionList user_func_list;
      class ImportedFunctionList imported_func_list;
      class GlobalVariableList global_var_list;

      // for the thread counter
      class QoreCounter tcount;
      class StringList fileList;
      class charPtrList featureList;
      
      // parse lock, making parsing actions atomic and thread-safe
      class LockedObject plock;
      // depedency counter, when this hits zero, the object is deleted
      class ReferenceObject dc;
      class SBNode *sb_head, *sb_tail;
      class ExceptionSink *parseSink, *warnSink;
      class RootNamespace *RootNS;
      class Namespace *QoreNS;

      int parse_options, warn_mask;
      bool po_locked, exec_class, base_object, requires_exception;
      char *exec_class_name;
      pthread_key_t thread_local_storage;

      DLLLOCAL void init();
      DLLLOCAL void nextSB();
      DLLLOCAL void deleteSBList();
      DLLLOCAL void internParseCommit();
      DLLLOCAL void initGlobalVars();
      DLLLOCAL void importUserFunction(class QoreProgram *p, class UserFunction *uf, class ExceptionSink *xsink);
      DLLLOCAL void internParseRollback();
      DLLLOCAL int internParsePending(char *code, char *label);
      DLLLOCAL class Hash *clearThreadData(class ExceptionSink *xsink);

   protected:
      DLLLOCAL virtual ~QoreProgram();

   public:
      DLLEXPORT QoreProgram();
      DLLEXPORT class QoreNode *callFunction(char *name, class QoreNode *args, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *callFunction(class UserFunction *func, class QoreNode *args, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *run(class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *runTopLevel(class ExceptionSink *xsink);
      DLLEXPORT void parseFileAndRun(char *filename);
      DLLEXPORT void parseAndRun(FILE *, char *name);
      DLLEXPORT void parseAndRun(char *str, char *name);
      DLLEXPORT void runClass(char *classname, class ExceptionSink *xsink);
      DLLEXPORT void parseFileAndRunClass(char *filename, char *classname);
      DLLEXPORT void parseAndRunClass(FILE *, char *name, char *classname);
      DLLEXPORT void parseAndRunClass(char *str, char *name, char *classname);      
      DLLEXPORT void parse(FILE *, char *name, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parse(class QoreString *str, class QoreString *lstr, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parse(char *str, char *lstr, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parseFile(char *filename, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parsePending(char *code, char *label, class ExceptionSink *xsink, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parsePending(class QoreString *str, class QoreString *lstr, class ExceptionSink *xsink, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parseCommit(class ExceptionSink *xsink, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parseRollback();
      DLLEXPORT bool existsFunction(char *name);
      DLLEXPORT virtual void deref(class ExceptionSink *xsink);
      DLLEXPORT void lockOptions();
      // setExecClass() NOTE: string passed here will copied
      DLLEXPORT void setExecClass(char *ecn = NULL);
      DLLEXPORT void parseSetParseOptions(int po);
      DLLEXPORT void waitForTermination();
      DLLEXPORT void waitForTerminationAndDeref(class ExceptionSink *xsink);
      DLLEXPORT class Namespace *getQoreNS() const;
      DLLEXPORT class RootNamespace *getRootNS() const;
      // returns 0 for success, -1 for error
      DLLEXPORT int setWarningMask(int wm);
      // returns 0 for success, -1 for error
      DLLEXPORT int enableWarning(int code);
      // returns 0 for success, -1 for error
      DLLEXPORT int disableWarning(int code);
      DLLEXPORT int getParseOptions() const;
      DLLEXPORT void setParseOptions(int po, class ExceptionSink *xsink);
      DLLEXPORT void disableParseOptions(int po, class ExceptionSink *xsink);
      DLLEXPORT class List *getUserFunctionList();
      DLLEXPORT bool checkWarning(int code) const;
      DLLEXPORT int getWarningMask() const;
      DLLEXPORT bool checkFeature(char *f) const;
      DLLEXPORT class List *getFeatureList() const;
      DLLEXPORT class UserFunction *findUserFunction(char *name);
      
      // QoreProgram() NOTE: ecn is the exec_class_name and will be copied if it exists
      DLLLOCAL QoreProgram(class QoreProgram *pgm, int po, bool ec = false, char *ecn = NULL);
      DLLLOCAL void registerUserFunction(class UserFunction *u);
      DLLLOCAL void resolveFunction(class FunctionCall *f);      
      DLLLOCAL void addGlobalVarDef(char *name);
      DLLLOCAL void addStatement(class Statement *s);
      DLLLOCAL class Var *findVar(char *name);
      DLLLOCAL class Var *checkVar(char *name);
      DLLLOCAL class Var *createVar(char *name);
      DLLLOCAL void importGlobalVariable(class Var *var, class ExceptionSink *xsink, bool readonly);
      DLLLOCAL void makeParseException(char *err, class QoreString *desc);
      DLLLOCAL void makeParseException(class QoreString *desc);
      DLLLOCAL void addParseException(class ExceptionSink *xsink);
      DLLLOCAL void makeParseWarning(int code, char *warn, const char *fmt, ...);
      DLLLOCAL void addParseWarning(int code, class ExceptionSink *xsink);
      DLLLOCAL void cannotProvideFeature(class QoreString *desc);
      DLLLOCAL void exportUserFunction(char *name, class QoreProgram *p, class ExceptionSink *xsink);
      DLLLOCAL void endThread(class ExceptionSink *xsink);
      DLLLOCAL void startThread();
      DLLLOCAL class Hash *getThreadData();
      DLLLOCAL void del(class ExceptionSink *xsink);
      DLLLOCAL void depRef();
      DLLLOCAL void depDeref(class ExceptionSink *xsink);
      DLLLOCAL void addFeature(char *f);
      DLLLOCAL void addFile(char *f);
      DLLLOCAL class List *getVarList();
      // increment atomic thread counter
      DLLLOCAL void tc_inc();
      // decrement atomic thread counter
      DLLLOCAL void tc_dec();
};

DLLLOCAL void addProgramConstants(class Namespace *ns);

// private interface to bison/flex parser/scanner
typedef void *yyscan_t;
DLLLOCAL extern int yyparse(yyscan_t yyscanner);
DLLLOCAL extern struct yy_buffer_state *yy_scan_string(const char *, yyscan_t scanner);
DLLLOCAL int yylex_init(yyscan_t *scanner);
DLLLOCAL void yyset_in(FILE *in_str, yyscan_t yyscanner);
DLLLOCAL int yylex_destroy(yyscan_t yyscanner);
//extern FILE *yyin;

#endif // _QORE_QOREPROGRAM_H
