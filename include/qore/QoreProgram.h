/*
  QoreProgram.h

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

#ifndef _QORE_QOREPROGRAM_H

#define _QORE_QOREPROGRAM_H

#include <qore/AbstractPrivateData.h>
#include <qore/Restrictions.h>

class AbstractFunctionReferenceNode;

// warnings - must correspond with the string order in QoreProgram.cc
#define QP_WARN_WARNING_MASK_UNCHANGED   (1 << 0)
#define QP_WARN_DUPLICATE_LOCAL_VARS     (1 << 1)
#define QP_WARN_UNKNOWN_WARNING          (1 << 2)
#define QP_WARN_UNDECLARED_VAR           (1 << 3)
#define QP_WARN_DUPLICATE_GLOBAL_VARS    (1 << 4)
#define QP_WARN_UNREACHABLE_CODE         (1 << 5)

// defined in QoreProgram.cc
DLLEXPORT extern const char *qore_warnings[];
DLLEXPORT int get_warning_code(const char *str);
DLLEXPORT extern unsigned qore_num_warnings;

//! supports parsing and executing Qore-language code, reference counted, dynamically-allocated only
/** This class implements a transaction and thread-safe container for qore-language code
    This class implements two-layered reference counting to address problems with circular references.
    When a program has a global variable that contains an object that references the program...
    objects now reference the dependency counter, so when the object's counter reaches zero and
    the global variable list is deleted, then the variables will in turn dereference the program
    so it can be deleted.
 */
class QoreProgram : public AbstractPrivateData
{
   private:
      //! private implementation
      struct qore_program_private *priv;
      
      DLLLOCAL void nextSB();
      DLLLOCAL void deleteSBList();
      DLLLOCAL void internParseCommit();
      DLLLOCAL void initGlobalVars();
      DLLLOCAL void importUserFunction(class QoreProgram *p, class UserFunction *uf, class ExceptionSink *xsink);
      DLLLOCAL void internParseRollback();
      DLLLOCAL int internParsePending(const char *code, const char *label);
      DLLLOCAL void del(class ExceptionSink *xsink);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreProgram(const QoreProgram&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreProgram& operator=(const QoreProgram&);
      
   protected:
      //! the destructor is private in order to prohibit the objkect from being allocated on the stack
      /** the destructor is run when the reference count reaches 0 
       */
      DLLLOCAL virtual ~QoreProgram();

   public:
      //! creates the object
      DLLEXPORT QoreProgram();
      //! calls a function from the function name and returns the return value
      /** if the function does not exist, an exception is added to "xsink"
       */
      DLLEXPORT AbstractQoreNode *callFunction(const char *name, const class QoreListNode *args, class ExceptionSink *xsink);

      //! runs the program (instantiates the program class if a program class has been set
      /**
	 @see setExecClass()
       */
      DLLEXPORT AbstractQoreNode *run(class ExceptionSink *xsink);
      DLLEXPORT AbstractQoreNode *runTopLevel(class ExceptionSink *xsink);
      DLLEXPORT void parseFileAndRun(const char *filename);
      DLLEXPORT void parseAndRun(FILE *, const char *name);
      DLLEXPORT void parseAndRun(const char *str, const char *name);
      DLLEXPORT void runClass(const char *classname, class ExceptionSink *xsink);
      DLLEXPORT void parseFileAndRunClass(const char *filename, const char *classname);
      DLLEXPORT void parseAndRunClass(FILE *, const char *name, const char *classname);
      DLLEXPORT void parseAndRunClass(const char *str, const char *name, const char *classname);      
      DLLEXPORT void parse(FILE *, const char *name, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parse(const class QoreString *str, const class QoreString *lstr, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parse(const char *str, const char *lstr, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parseFile(const char *filename, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parsePending(const char *code, const char *label, class ExceptionSink *xsink, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parsePending(const class QoreString *str, const class QoreString *lstr, class ExceptionSink *xsink, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parseCommit(class ExceptionSink *xsink, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      DLLEXPORT void parseRollback();
      DLLEXPORT bool existsFunction(const char *name);
      DLLEXPORT virtual void deref(class ExceptionSink *xsink);
      DLLEXPORT void lockOptions();
      
      //! sets the name of the application class to be executed (instantiated) instead of top-level code
      /** normally parse option PO_NO_TOP_LEVEL_STATEMENTS should be set as well
	  NOTE: string passed here will copied
	  @param ecn the name of the class to be executed as the program class
      */
      DLLEXPORT void setExecClass(const char *ecn = NULL);
      DLLEXPORT void parseSetParseOptions(qore_restrictions_t po);
      DLLEXPORT void waitForTermination();
      DLLEXPORT void waitForTerminationAndDeref(class ExceptionSink *xsink);
      DLLEXPORT class QoreNamespace *getQoreNS() const;
      DLLEXPORT class RootQoreNamespace *getRootNS() const;

      //! sets the warning mask
      /**
	 @param code the new warning mask
	 @return 0 for success, -1 for error
      */
      DLLEXPORT int setWarningMask(int wm);

      //! enables a warning by its code
      /**
	 @param code the warning code to enable
	 @return 0 for success, -1 for error
      */
      DLLEXPORT int enableWarning(int code);

      //! disables a warning by its code
      /**
	 @param code the warning code to disable
	 @return 0 for success, -1 for error
      */
      DLLEXPORT int disableWarning(int code);
      DLLEXPORT qore_restrictions_t getParseOptions() const;
      DLLEXPORT void setParseOptions(qore_restrictions_t po, class ExceptionSink *xsink);
      DLLEXPORT void disableParseOptions(qore_restrictions_t po, class ExceptionSink *xsink);
      DLLEXPORT class QoreListNode *getUserFunctionList();
      DLLEXPORT bool checkWarning(int code) const;
      DLLEXPORT int getWarningMask() const;
      DLLEXPORT bool checkFeature(const char *f) const;
      DLLEXPORT class QoreListNode *getFeatureList() const;
      DLLEXPORT class UserFunction *findUserFunction(const char *name);
      
      DLLLOCAL QoreProgram(class QoreProgram *pgm, qore_restrictions_t po, bool ec = false, const char *ecn = NULL);

      //! calls a function from a UserFunction pointer and returns the return value
      /** if the function does not exist, an exception is added to "xsink"
       */
      DLLLOCAL AbstractQoreNode *callFunction(class UserFunction *func, const class QoreListNode *args, class ExceptionSink *xsink);

      DLLLOCAL void registerUserFunction(class UserFunction *u);
      DLLLOCAL void resolveFunction(class FunctionCallNode *f);      
      DLLLOCAL AbstractFunctionReferenceNode *resolveFunctionReference(class UnresolvedFunctionReferenceNode *fr);      
      DLLLOCAL void addGlobalVarDef(const char *name);
      DLLLOCAL void addStatement(class AbstractStatement *s);
      DLLLOCAL class Var *findVar(const char *name);
      DLLLOCAL class Var *checkVar(const char *name);
      DLLLOCAL class Var *createVar(const char *name);
      DLLLOCAL void importGlobalVariable(class Var *var, class ExceptionSink *xsink, bool readonly);
      DLLLOCAL void makeParseException(const char *err, class QoreStringNode *desc);
      DLLLOCAL void makeParseException(int sline, int eline, class QoreStringNode *desc);
      DLLLOCAL void makeParseException(class QoreStringNode *desc);
      DLLLOCAL void addParseException(class ExceptionSink *xsink);
      DLLLOCAL void makeParseWarning(int code, const char *warn, const char *fmt, ...);
      DLLLOCAL void addParseWarning(int code, class ExceptionSink *xsink);
      DLLLOCAL void cannotProvideFeature(class QoreStringNode *desc);
      DLLLOCAL void exportUserFunction(const char *name, class QoreProgram *p, class ExceptionSink *xsink);
      DLLLOCAL void endThread(class ExceptionSink *xsink);
      DLLLOCAL void startThread();
      DLLLOCAL class QoreHashNode *getThreadData();
      DLLLOCAL void depRef();
      DLLLOCAL void depDeref(class ExceptionSink *xsink);
      DLLLOCAL void addFeature(const char *f);
      DLLLOCAL void addFile(char *f);
      DLLLOCAL class QoreListNode *getVarList();
      // increment atomic thread counter
      DLLLOCAL void tc_inc();
      // decrement atomic thread counter
      DLLLOCAL void tc_dec();
      // for run-time module loading; the parse lock must be grabbed
      // before loading new modules - note this should only be assigned
      // to a AutoLock or SafeLocker object!
      DLLLOCAL class QoreThreadLock *getParseLock();
      DLLLOCAL class QoreHashNode *clearThreadData(class ExceptionSink *xsink);
};

DLLLOCAL void addProgramConstants(class QoreNamespace *ns);

#endif // _QORE_QOREPROGRAM_H
