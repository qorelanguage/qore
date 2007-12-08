/*
  QoreProgram.h

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

#ifndef _QORE_QOREPROGRAM_H

#define _QORE_QOREPROGRAM_H

#include <qore/AbstractPrivateData.h>

// the two-layered reference counting is to eliminate problems from circular references
// when a program has a global variable that contains an object that references the program...
// objects now reference the dependency counter, so when the object's counter reaches zero and
// the global variable list is deleted, then the variables will in turn dereference the program
// so it can be deleted...

struct qore_program_private;

class QoreProgram : public AbstractPrivateData
{
   private:
      struct qore_program_private *priv;
      
      DLLLOCAL void nextSB();
      DLLLOCAL void deleteSBList();
      DLLLOCAL void internParseCommit();
      DLLLOCAL void initGlobalVars();
      DLLLOCAL void importUserFunction(class QoreProgram *p, class UserFunction *uf, class ExceptionSink *xsink);
      DLLLOCAL void internParseRollback();
      DLLLOCAL int internParsePending(const char *code, const char *label);
      DLLLOCAL class QoreHash *clearThreadData(class ExceptionSink *xsink);
      DLLLOCAL void del(class ExceptionSink *xsink);
      
   protected:
      DLLLOCAL virtual ~QoreProgram();

   public:
      DLLEXPORT QoreProgram();
      DLLEXPORT class QoreNode *callFunction(const char *name, class QoreNode *args, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *callFunction(class UserFunction *func, class QoreNode *args, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *run(class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *runTopLevel(class ExceptionSink *xsink);
      DLLEXPORT void parseFileAndRun(const char *filename);
      DLLEXPORT void parseAndRun(FILE *, const char *name);
      DLLEXPORT void parseAndRun(const char *str, const char *name);
      DLLEXPORT void runClass(const char *classname, class ExceptionSink *xsink);
      DLLEXPORT void parseFileAndRunClass(const char *filename, const char *classname);
      DLLEXPORT void parseAndRunClass(FILE *, const char *name, const char *classname);
      DLLEXPORT void parseAndRunClass(const char *str, const char *name, const char *classname);      
      DLLEXPORT void parse(FILE *, const char *name, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parse(class QoreString *str, class QoreString *lstr, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parse(const char *str, const char *lstr, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parseFile(const char *filename, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parsePending(const char *code, const char *label, class ExceptionSink *xsink, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parsePending(class QoreString *str, class QoreString *lstr, class ExceptionSink *xsink, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parseCommit(class ExceptionSink *xsink, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parseRollback();
      DLLEXPORT bool existsFunction(const char *name);
      DLLEXPORT virtual void deref(class ExceptionSink *xsink);
      DLLEXPORT void lockOptions();
      // setExecClass() NOTE: string passed here will copied
      DLLEXPORT void setExecClass(const char *ecn = NULL);
      DLLEXPORT void parseSetParseOptions(int po);
      DLLEXPORT void waitForTermination();
      DLLEXPORT void waitForTerminationAndDeref(class ExceptionSink *xsink);
      DLLEXPORT class QoreNamespace *getQoreNS() const;
      DLLEXPORT class RootQoreNamespace *getRootNS() const;
      // returns 0 for success, -1 for error
      DLLEXPORT int setWarningMask(int wm);
      // returns 0 for success, -1 for error
      DLLEXPORT int enableWarning(int code);
      // returns 0 for success, -1 for error
      DLLEXPORT int disableWarning(int code);
      DLLEXPORT int getParseOptions() const;
      DLLEXPORT void setParseOptions(int po, class ExceptionSink *xsink);
      DLLEXPORT void disableParseOptions(int po, class ExceptionSink *xsink);
      DLLEXPORT class QoreList *getUserFunctionList();
      DLLEXPORT bool checkWarning(int code) const;
      DLLEXPORT int getWarningMask() const;
      DLLEXPORT bool checkFeature(const char *f) const;
      DLLEXPORT class QoreList *getFeatureList() const;
      DLLEXPORT class UserFunction *findUserFunction(const char *name);
      
      DLLLOCAL QoreProgram(class QoreProgram *pgm, int po, bool ec = false, const char *ecn = NULL);
      DLLLOCAL void registerUserFunction(class UserFunction *u);
      DLLLOCAL void resolveFunction(class FunctionCall *f);      
      DLLLOCAL void resolveFunctionReference(class FunctionReference *fr);      
      DLLLOCAL void addGlobalVarDef(const char *name);
      DLLLOCAL void addStatement(class AbstractStatement *s);
      DLLLOCAL class Var *findVar(const char *name);
      DLLLOCAL class Var *checkVar(const char *name);
      DLLLOCAL class Var *createVar(const char *name);
      DLLLOCAL void importGlobalVariable(class Var *var, class ExceptionSink *xsink, bool readonly);
      DLLLOCAL void makeParseException(const char *err, class QoreString *desc);
      DLLLOCAL void makeParseException(int sline, int eline, class QoreString *desc);
      DLLLOCAL void makeParseException(class QoreString *desc);
      DLLLOCAL void addParseException(class ExceptionSink *xsink);
      DLLLOCAL void makeParseWarning(int code, const char *warn, const char *fmt, ...);
      DLLLOCAL void addParseWarning(int code, class ExceptionSink *xsink);
      DLLLOCAL void cannotProvideFeature(class QoreString *desc);
      DLLLOCAL void exportUserFunction(const char *name, class QoreProgram *p, class ExceptionSink *xsink);
      DLLLOCAL void endThread(class ExceptionSink *xsink);
      DLLLOCAL void startThread();
      DLLLOCAL class QoreHash *getThreadData();
      DLLLOCAL void depRef();
      DLLLOCAL void depDeref(class ExceptionSink *xsink);
      DLLLOCAL void addFeature(const char *f);
      DLLLOCAL void addFile(char *f);
      DLLLOCAL class QoreList *getVarList();
      // increment atomic thread counter
      DLLLOCAL void tc_inc();
      // decrement atomic thread counter
      DLLLOCAL void tc_dec();
      // for run-time module loading; the parse lock must be grabbed
      // before loading new modules - note this should only be assigned
      // to a AutoLock or SafeLocker object!
      DLLLOCAL class LockedObject *getParseLock();
};

DLLLOCAL void addProgramConstants(class QoreNamespace *ns);

#endif // _QORE_QOREPROGRAM_H
