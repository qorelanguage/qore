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
#define QP_WARN_WARNING_MASK_UNCHANGED   (1 << 0)   //!< when the warning mask is attempted to be changed but it's locked
#define QP_WARN_DUPLICATE_LOCAL_VARS     (1 << 1)   //!< duplicate local variable name
#define QP_WARN_UNKNOWN_WARNING          (1 << 2)   //!< when an unknown warning is encountered
#define QP_WARN_UNDECLARED_VAR           (1 << 3)   //!< when a variable is not declared
#define QP_WARN_DUPLICATE_GLOBAL_VARS    (1 << 4)   //!< when a global variable is declared more than once
#define QP_WARN_UNREACHABLE_CODE         (1 << 5)   //!< when unreachable code is encountered

//! list of strings of warning codes
DLLEXPORT extern const char *qore_warnings[];
//! number of warning codes
DLLEXPORT extern unsigned qore_num_warnings;
//! returns the warning code corresponding to the string passed (0 if no match was made)
DLLEXPORT int get_warning_code(const char *str);

class LocalVar;

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
      
      DLLLOCAL void initGlobalVars();
      DLLLOCAL void importUserFunction(QoreProgram *p, class UserFunction *uf, class ExceptionSink *xsink);
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

      //! runs the program (instantiates the program class if a program class has been set) and returns the return value (if any)
      /** @note if the program is run as a class it's not possible to return a value
	  @note any threads started by this call will continue running in the background,
	  to wait for them to terminate, call QoreProgram::waitForTermination() or
	  QoreProgram::waitForTerminationAndDeref()
	  @see QoreProgram::setExecClass()
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the value returned by the final return statement (if any, can be 0)
       */
      DLLEXPORT AbstractQoreNode *run(class ExceptionSink *xsink);

      //! tuns the top level code and returns any return value
      /**
	  @note any threads started by this call will continue running in the background,
	  to wait for them to terminate, call QoreProgram::waitForTermination() or
	  QoreProgram::waitForTerminationAndDeref()
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the value returned by the final return statement (if any, can be 0)
       */
      DLLEXPORT AbstractQoreNode *runTopLevel(class ExceptionSink *xsink);

      //! parses the given filename and runs the file
      /** any errors opening the file are added as Qore-language exceptions
	  the default exception handler is run on any Qore-language exceptions
	  raised during opening, parsing, and executing the file.
	  @note any threads started by this call will continue running in the background,
	  to wait for them to terminate, call QoreProgram::waitForTermination() or
	  QoreProgram::waitForTerminationAndDeref()
	  @note will also commit any pending changes added with QoreProgram::parsePending()
	  @param filename the filename to run
       */
      DLLEXPORT void parseFileAndRun(const char *filename);

      //! parses the given file and runs the file
      /** the default exception handler is run on any Qore-language exceptions
	  raised while parsing and executing the file.
	  @note any threads started by this call will continue running in the background,
	  to wait for them to terminate, call QoreProgram::waitForTermination() or
	  QoreProgram::waitForTerminationAndDeref()
	  @note will also commit any pending changes added with QoreProgram::parsePending()
	  @param fp the filename to run
	  @param name the name of the file being parsed and run
       */
      DLLEXPORT void parseAndRun(FILE *fp, const char *name);

      //! parses the given string and runs the code
      /** The default exception handler is run on any Qore-language exceptions
	  raised while parsing and executing the code.
	  @note any threads started by this call will continue running in the background,
	  to wait for them to terminate, call QoreProgram::waitForTermination() or
	  QoreProgram::waitForTerminationAndDeref()
	  @note will also commit any pending changes added with QoreProgram::parsePending()
	  @param str the Qore-language code to parse and run
	  @param name the label of the code being parsed and run (used as the file name)
       */
      DLLEXPORT void parseAndRun(const char *str, const char *name);

      //! instantiates the class given and runs its constructor
      /** @note any threads started by this call will continue running in the background,
	  to wait for them to terminate, call QoreProgram::waitForTermination() or
	  QoreProgram::waitForTerminationAndDeref()
	  @note will also commit any pending changes added with QoreProgram::parsePending()
	  @param classname the name of the class to instantiate
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void runClass(const char *classname, class ExceptionSink *xsink);

      //! parses the given filename and runs the program by instantiating the class given
      /** Any errors opening the file are added as Qore-language exceptions.
	  The default exception handler is run on any Qore-language exceptions
	  raised during opening, parsing, and executing the file.
	  @note any threads started by this call will continue running in the background,
	  to wait for them to terminate, call QoreProgram::waitForTermination() or
	  QoreProgram::waitForTerminationAndDeref()
	  @note will also commit any pending changes added with QoreProgram::parsePending()
	  @param filename the filename to run
	  @param classname the name of the class to instantiate
       */
      DLLEXPORT void parseFileAndRunClass(const char *filename, const char *classname);

      //! parses the given file and runs the code by instantiating the class given
      /** The default exception handler is run on any Qore-language exceptions
	  raised while parsing and executing the file.
	  @note any threads started by this call will continue running in the background,
	  to wait for them to terminate, call QoreProgram::waitForTermination() or
	  QoreProgram::waitForTerminationAndDeref()
	  @note will also commit any pending changes added with QoreProgram::parsePending()
	  @param fp the filename to run
	  @param name the name of the file being parsed and run
	  @param classname the name of the class to instantiate
       */
      DLLEXPORT void parseAndRunClass(FILE *, const char *name, const char *classname);

      //! parses the given string and runs the code by instantiating the class given
      /** The default exception handler is run on any Qore-language exceptions
	  raised while parsing and executing the code.
	  @note any threads started by this call will continue running in the background,
	  to wait for them to terminate, call QoreProgram::waitForTermination() or
	  QoreProgram::waitForTerminationAndDeref()
	  @note will also commit any pending changes added with QoreProgram::parsePending()
	  @param str the Qore-language code to parse and run
	  @param name the label of the code being parsed and run (used as the file name)
	  @param classname the name of the class to instantiate
       */
      DLLEXPORT void parseAndRunClass(const char *str, const char *name, const char *classname);      

      //! parses code from the file given and commits changes to the QoreProgram
      /**
	  @note will also commit any pending changes added with QoreProgram::parsePending()
	  @param fp the filename to parse
	  @param name the name of the file being parsed and run
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @param warn_sink if a warning is raised, the warning information will be added here
	  @param warn_mask the warning mask to set (-1 sets all possible warnings)
       */
      DLLEXPORT void parse(FILE *, const char *name, class ExceptionSink *xsink, class ExceptionSink *warn_sink = NULL, int warn_mask = -1);

      //! parses code from the given string and commits changes to the QoreProgram
      /**
	  @note will also commit any pending changes added with QoreProgram::parsePending()
	  @param str the code to parse
	  @param lstr the label of the code being parsed to be used as a file name
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @param warn_sink if a warning is raised, the warning information will be added here
	  @param warn_mask the warning mask to set (-1 sets all possible warnings)
       */
      DLLEXPORT void parse(const class QoreString *str, const class QoreString *lstr, class ExceptionSink *xsink, class ExceptionSink *warn_sink = NULL, int warn_mask = -1);

      //! parses code from the given string and commits changes to the QoreProgram
      /**
	  @note will also commit any pending changes added with QoreProgram::parsePending()
	  @param str the code to parse; the encoding of the string is assumed to be QCS_DEFAULT
	  @param lstr the label of the code being parsed to be used as a file name
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @param warn_sink if a warning is raised, the warning information will be added here
	  @param warn_mask the warning mask to set (-1 sets all possible warnings)
       */
      DLLEXPORT void parse(const char *str, const char *lstr, class ExceptionSink *xsink, class ExceptionSink *warn_sink = NULL, int warn_mask = -1);

      //! parses code from the file given and commits changes to the QoreProgram
      /** 
	  @note will also commit any pending changes added with QoreProgram::parsePending()
	  @param filename the filename to open and parse
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @param warn_sink if a warning is raised, the warning information will be added here
	  @param warn_mask the warning mask to set (-1 sets all possible warnings)
       */
      DLLEXPORT void parseFile(const char *filename, class ExceptionSink *xsink, class ExceptionSink *warn_sink = NULL, int warn_mask = -1);

      //! parses code from the given string but does not commit changes to the QoreProgram
      /**
	  @param code the code to parse; the encoding of the string is assumed to be QCS_DEFAULT
	  @param label the label of the code being parsed to be used as a file name
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @param warn_sink if a warning is raised, the warning information will be added here
	  @param warn_mask the warning mask to set (-1 sets all possible warnings)
	  @see QoreProgram::parseCommit()
	  @see QoreProgram::parseRollback()
       */
      DLLEXPORT void parsePending(const char *code, const char *label, class ExceptionSink *xsink, class ExceptionSink *warn_sink = NULL, int warn_mask = -1);

      //! parses code from the given string but does not commit changes to the QoreProgram
      /**
	  @param str the code to parse
	  @param lstr the label of the code being parsed to be used as a file name
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @param warn_sink if a warning is raised, the warning information will be added here
	  @param warn_mask the warning mask to set (-1 sets all possible warnings)
	  @see QoreProgram::parseCommit()
	  @see QoreProgram::parseRollback()
       */
      DLLEXPORT void parsePending(const class QoreString *str, const class QoreString *lstr, class ExceptionSink *xsink, class ExceptionSink *warn_sink = NULL, int warn_mask = -1);

      //! commits pending changes to the program
      /**
	  @see QoreProgram::parsePending()
	  @see QoreProgram::parseRollback()
       */
      DLLEXPORT void parseCommit(class ExceptionSink *xsink, class ExceptionSink *warn_sink = NULL, int warn_mask = -1);

      //! rolls back changes to the program object that were added with QoreProgram::parsePending()
      /**
	  @see QoreProgram::parsePending()
	  @see QoreProgram::parseCommit()
       */
      DLLEXPORT void parseRollback();

      //! returns true if the given function exists as a user function, false if not
      DLLEXPORT bool existsFunction(const char *name);

      //! dereferences the object and deletes it if the reference count reaches zero
      /** do not use this function if the program may be running, use QoreProgram::waitForTerminationAndDeref() instead
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @see QoreProgram::waitForTerminationAndDeref()
       */
      DLLEXPORT virtual void deref(class ExceptionSink *xsink);

      //! locks parse options so they may not be changed
      DLLEXPORT void lockOptions();
      
      //! sets the name of the application class to be executed (instantiated) instead of top-level code
      /** normally parse option PO_NO_TOP_LEVEL_STATEMENTS should be set as well
	  @note the string passed here will copied
	  @param ecn the name of the class to be executed as the program class
	  @see QoreProgram::parseSetParseOptions()
      */
      DLLEXPORT void setExecClass(const char *ecn = NULL);

      //! adds the parse options given to the parse option mask
      DLLEXPORT void parseSetParseOptions(qore_restrictions_t po);

      //! this call blocks until the program's last thread terminates
      DLLEXPORT void waitForTermination();

      //! this call blocks until the program's last thread terminates, and then calls QoreProgram::deref()
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void waitForTerminationAndDeref(class ExceptionSink *xsink);

      //! returns a pointer to the "Qore" namespace
      DLLEXPORT class QoreNamespace *getQoreNS() const;

      //! returns a pointer to the root namespace
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

      //! returns the parse options currently set for this program
      DLLEXPORT qore_restrictions_t getParseOptions() const;

      //! sets the parse options and adds Qore-language exception information if an error occurs
      /**
	 @param po the parse options to add to the parse option mask
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void setParseOptions(qore_restrictions_t po, class ExceptionSink *xsink);

      //! turns off the parse options given in the passed mask and adds Qore-language exception information if an error occurs
      /**
	 @param po the parse options to subtract from the parse option mask
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void disableParseOptions(qore_restrictions_t po, class ExceptionSink *xsink);

      //! returns a list of all user functions in this program
      /**
	 @return a list of all user functions in this program
       */
      DLLEXPORT class QoreListNode *getUserFunctionList();

      //! returns true if the warning code is set
      DLLEXPORT bool checkWarning(int code) const;

      //! returns the warning mask
      DLLEXPORT int getWarningMask() const;

      //! returns true if the given feature is present in the program object
      DLLEXPORT bool checkFeature(const char *f) const;

      //! returns a list of features in the program object
      DLLEXPORT class QoreListNode *getFeatureList() const;

      //! returns a pointed to the given user function if it exists (otherwise returns 0)
      DLLLOCAL class UserFunction *findUserFunction(const char *name);
      
      DLLLOCAL QoreProgram(QoreProgram *pgm, qore_restrictions_t po, bool ec = false, const char *ecn = NULL);

      //! calls a function from a UserFunction pointer and returns the return value
      /** if the function does not exist, an exception is added to "xsink"
       */
      DLLLOCAL AbstractQoreNode *callFunction(class UserFunction *func, const class QoreListNode *args, class ExceptionSink *xsink);

      DLLLOCAL LocalVar *createLocalVar(const char *name);

      DLLLOCAL void registerUserFunction(class UserFunction *u);
      DLLLOCAL void resolveFunction(class FunctionCallNode *f);      
      DLLLOCAL AbstractFunctionReferenceNode *resolveFunctionReference(class UnresolvedFunctionReferenceNode *fr);      
      DLLLOCAL void addGlobalVarDef(const char *name);
      DLLLOCAL void addStatement(class AbstractStatement *s);
      DLLLOCAL class Var *findGlobalVar(const char *name);
      DLLLOCAL class Var *checkGlobalVar(const char *name);
      DLLLOCAL class Var *createGlobalVar(const char *name);
      DLLLOCAL void importGlobalVariable(class Var *var, class ExceptionSink *xsink, bool readonly);
      DLLLOCAL void makeParseException(const char *err, class QoreStringNode *desc);
      DLLLOCAL void makeParseException(int sline, int eline, class QoreStringNode *desc);
      DLLLOCAL void makeParseException(class QoreStringNode *desc);
      DLLLOCAL void addParseException(class ExceptionSink *xsink);
      DLLLOCAL void makeParseWarning(int code, const char *warn, const char *fmt, ...);
      DLLLOCAL void addParseWarning(int code, class ExceptionSink *xsink);
      DLLLOCAL void cannotProvideFeature(class QoreStringNode *desc);
      DLLLOCAL void exportUserFunction(const char *name, QoreProgram *p, class ExceptionSink *xsink);
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
