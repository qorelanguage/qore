/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreProgram.h

  Program Object Definition

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_QOREPROGRAM_H

#define _QORE_QOREPROGRAM_H

#include <qore/AbstractPrivateData.h>
#include <qore/Restrictions.h>

// warnings - must correspond with the string order in QoreProgram.cpp
// new warnings must also be added as constants
#define QP_WARN_NONE                     0
#define QP_WARN_WARNING_MASK_UNCHANGED   (1 << 0)   //!< when the warning mask is attempted to be changed but it's locked
#define QP_WARN_DUPLICATE_LOCAL_VARS     (1 << 1)   //!< duplicate local variable name reachable in the same scope
#define QP_WARN_UNKNOWN_WARNING          (1 << 2)   //!< when an unknown warning is encountered
#define QP_WARN_UNDECLARED_VAR           (1 << 3)   //!< when a variable is not declared
#define QP_WARN_DUPLICATE_GLOBAL_VARS    (1 << 4)   //!< when a global variable is declared more than once
#define QP_WARN_UNREACHABLE_CODE         (1 << 5)   //!< when unreachable code is encountered
#define QP_WARN_NONEXISTENT_METHOD_CALL  (1 << 6)   //!< when a non-existent method call is encountered
#define QP_WARN_INVALID_OPERATION        (1 << 7)   //!< when an expression always returns NOTHING, for example
#define QP_WARN_CALL_WITH_TYPE_ERRORS    (1 << 8)   //!< when a function or method call always returns a fixed value due to type errors
#define QP_WARN_RETURN_VALUE_IGNORED     (1 << 9)   //!< when a function or method call has no side effects and the return value is ignored
#define QP_WARN_DEPRECATED               (1 << 10)  //!< when depcrecated functionality is accessed
#define QP_WARN_EXCESS_ARGS              (1 << 11)  //!< when excess arguments are given to a function that does not access them
#define QP_WARN_DUPLICATE_HASH_KEY       (1 << 12)  //!< when a hash key has been defined more than once in a literal hash
#define QP_WARN_UNREFERENCED_VARIABLE    (1 << 13)  //!< when a variable is declared but not referenced
#define QP_WARN_DUPLICATE_BLOCK_VARS     (1 << 14)  //!< when a variable is declared more than once in the same block; this would normally be an error, but for backwards-compatibility it's just a warning unless parse option 'assume-local' is set, in which case it's an error
#define QP_WARN_MODULE_ONLY              (1 << 15)  //!< when qualifiers that are only valid in modules are given when not parsing a module
#define QP_WARN_ALL                      -1         //!< for all possible warnings

#define QP_WARN_MODULES (QP_WARN_UNREACHABLE_CODE|QP_WARN_NONEXISTENT_METHOD_CALL|QP_WARN_INVALID_OPERATION|QP_WARN_CALL_WITH_TYPE_ERRORS|QP_WARN_RETURN_VALUE_IGNORED|QP_WARN_DUPLICATE_HASH_KEY|QP_WARN_DUPLICATE_BLOCK_VARS)

#define QP_WARN_DEFAULT (QP_WARN_UNKNOWN_WARNING|QP_WARN_MODULES|QP_WARN_DEPRECATED)

//! list of strings of warning codes
DLLEXPORT extern const char** qore_warnings;

//! number of warning codes
DLLEXPORT extern unsigned qore_num_warnings;

//! returns the warning code corresponding to the string passed (0 if no match was made)
DLLEXPORT int get_warning_code(const char* str);

class AbstractCallReferenceNode;
class LocalVar;
class ExceptionSink;
class QoreListNode;
class QoreNamespace;
class RootQoreNamespace;
class QoreStringNode;
class QoreHashNode;
class FunctionCallNode;
class AbstractStatement;
class UnresolvedProgramCallReferenceNode;
class Var;
class LVList;
class UserFunctionVariant;
class QoreTypeInfo;
class QoreParseTypeInfo;
class ParamList;
class AbstractQoreZoneInfo;
class qore_program_private;

//! supports parsing and executing Qore-language code, reference counted, dynamically-allocated only
/** This class implements a transaction and thread-safe container for qore-language code
    This class implements two-layered reference counting to address problems with circular references.
    When a program has a global variable that contains an object that references the program...
    objects now reference the dependency counter, so when the object's counter reaches zero and
    the global variable list is deleted, then the variables will in turn dereference the program
    so it can be deleted.
*/
class QoreProgram : public AbstractPrivateData {
   friend class qore_program_private_base;
   friend class qore_program_private;
private:
   //! private implementation
   qore_program_private* priv;

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreProgram(const QoreProgram&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreProgram& operator=(const QoreProgram&);

protected:
   //! the destructor is private in order to prohibit the object from being allocated on the stack
   /** the destructor is run when the reference count reaches 0
    */
   DLLLOCAL virtual ~QoreProgram();

public:
   //! creates the object
   DLLEXPORT QoreProgram();

   //! creates the object and sets the parse options
   /**
      @param parse_options the parse options mask for the QoreProgram object
   */
   DLLEXPORT QoreProgram(int64 parse_options);

   //! calls a function from the function name and returns the return value
   /** if the function does not exist, an exception is added to "xsink"
       @param name the name of the function to call
       @param args the argument to the function (can be 0)
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT AbstractQoreNode* callFunction(const char* name, const QoreListNode* args, ExceptionSink* xsink);

   //! runs the program (instantiates the program class if a program class has been set) and returns the return value (if any)
   /** @note if the program is run as a class it's not possible to return a value
       @note any threads started by this call will continue running in the background,
       to wait for them to terminate, call QoreProgram::waitForTermination() or
       QoreProgram::waitForTerminationAndDeref()
       @see QoreProgram::setExecClass()
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return the value returned by the final return statement (if any, can be 0)
   */
   DLLEXPORT AbstractQoreNode* run(ExceptionSink* xsink);

   //! tuns the top level code and returns any return value
   /**
      @note any threads started by this call will continue running in the background,
      to wait for them to terminate, call QoreProgram::waitForTermination() or
      QoreProgram::waitForTerminationAndDeref()
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @return the value returned by the final return statement (if any, can be 0)
   */
   DLLEXPORT AbstractQoreNode* runTopLevel(ExceptionSink* xsink);

   //! parses the given filename and runs the file
   /** any errors opening the file are added as Qore-language exceptions
       the default exception handler is run on any Qore-language exceptions
       raised during opening, parsing, and executing the file.
       @note any threads started by this call will continue running in the background,
       to wait for them to terminate, call QoreProgram::waitForTermination() or
       QoreProgram::waitForTerminationAndDeref().
       Also sets the script path.
       @note will also commit any pending changes added with QoreProgram::parsePending()
       @param filename the filename to run
   */
   DLLEXPORT void parseFileAndRun(const char* filename);

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
   DLLEXPORT void parseAndRun(FILE *fp, const char* name);

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
   DLLEXPORT void parseAndRun(const char* str, const char* name);

   //! instantiates the class given and runs its constructor
   /** @note any threads started by this call will continue running in the background,
       to wait for them to terminate, call QoreProgram::waitForTermination() or
       QoreProgram::waitForTerminationAndDeref()
       @note will also commit any pending changes added with QoreProgram::parsePending()
       @param classname the name of the class to instantiate
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT void runClass(const char* classname, ExceptionSink* xsink);

   //! parses the given filename and runs the program by instantiating the class given
   /** Any errors opening the file are added as Qore-language exceptions.
       The default exception handler is run on any Qore-language exceptions
       raised during opening, parsing, and executing the file.
       @note any threads started by this call will continue running in the background,
       to wait for them to terminate, call QoreProgram::waitForTermination() or
       QoreProgram::waitForTerminationAndDeref().
       Also sets the script path.
       @note will also commit any pending changes added with QoreProgram::parsePending()
       @param filename the filename to run
       @param classname the name of the class to instantiate
   */
   DLLEXPORT void parseFileAndRunClass(const char* filename, const char* classname);

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
   DLLEXPORT void parseAndRunClass(FILE *fp, const char* name, const char* classname);

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
   DLLEXPORT void parseAndRunClass(const char* str, const char* name, const char* classname);

   //! parses code from the file given and commits changes to the QoreProgram
   /**
      @note will also commit any pending changes added with QoreProgram::parsePending()
      @param fp the filename to parse
      @param name the name of the file being parsed and run
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @param warn_sink if a warning is raised, the warning information will be added here
      @param warn_mask the warning mask to set (-1 sets all possible warnings)
   */
   DLLEXPORT void parse(FILE *fp, const char* name, ExceptionSink* xsink, ExceptionSink* warn_sink = 0, int warn_mask = QP_WARN_ALL);

   //! parses code from the given string and commits changes to the QoreProgram
   /**
      @note will also commit any pending changes added with QoreProgram::parsePending()
      @param str the code to parse
      @param lstr the label of the code being parsed to be used as a file name
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @param warn_sink if a warning is raised, the warning information will be added here
      @param warn_mask the warning mask to set (-1 sets all possible warnings)
   */
   DLLEXPORT void parse(const QoreString* str, const QoreString* lstr, ExceptionSink* xsink, ExceptionSink* warn_sink = 0, int warn_mask = QP_WARN_ALL);

   //! parses code from the given string and commits changes to the QoreProgram
   /**
      @note will also commit any pending changes added with QoreProgram::parsePending()
      @param str the code to parse
      @param lstr the label of the code being parsed to be used as a file name
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @param warn_sink if a warning is raised, the warning information will be added here (0 = no warnings)
      @param warn_mask the warning mask to set (-1 sets all possible warnings)
      @param source the source file name (if lstr is a label representing a section of a file for example)
      @param offset the line offset from the label to the file
   */
   DLLEXPORT void parse(const QoreString* str, const QoreString* lstr, ExceptionSink* xsink, ExceptionSink* warn_sink, int warn_mask, const QoreString* source, int offset);

   //! parses code from the given string and commits changes to the QoreProgram
   /**
      @note will also commit any pending changes added with QoreProgram::parsePending()
      @param str the code to parse; the encoding of the string is assumed to be QCS_DEFAULT
      @param lstr the label of the code being parsed to be used as a file name
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @param warn_sink if a warning is raised, the warning information will be added here (0 = no warnings)
      @param warn_mask the warning mask to set (-1 sets all possible warnings)
   */
   DLLEXPORT void parse(const char* str, const char* lstr, ExceptionSink* xsink, ExceptionSink* warn_sink = 0, int warn_mask = QP_WARN_ALL);

   //! parses code from the given string and commits changes to the QoreProgram
   /**
      @note will also commit any pending changes added with QoreProgram::parsePending()
      @param str the code to parse; the encoding of the string is assumed to be QCS_DEFAULT
      @param lstr the label of the code being parsed to be used as a file name
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @param warn_sink if a warning is raised, the warning information will be added here (0 = no warnings)
      @param warn_mask the warning mask to set (-1 sets all possible warnings)
      @param source the source file name (if lstr is a label representing a section of a file for example)
      @param offset the line offset from the label to the file
   */
   DLLEXPORT void parse(const char* str, const char* lstr, ExceptionSink* xsink, ExceptionSink* warn_sink, int warn_mask, const char* source, int offset);

   //! parses code from the file given and commits changes to the QoreProgram
   /** Also sets the script path.
       @note will also commit any pending changes added with QoreProgram::parsePending()
       @param filename the filename to open and parse
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @param warn_sink if a warning is raised, the warning information will be added here (0 = no warnings)
       @param warn_mask the warning mask to set (-1 sets all possible warnings)
       @param only_first_except is flag to stop parsing exceptions printing after 1st exception
   */
   DLLEXPORT void parseFile(const char* filename, ExceptionSink* xsink, ExceptionSink* warn_sink = 0, int warn_mask = QP_WARN_ALL, bool only_first_except = false);

   //! parses code from the given string but does not commit changes to the QoreProgram
   /**
      @param code the code to parse; the encoding of the string is assumed to be QCS_DEFAULT
      @param label the label of the code being parsed to be used as a file name
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @param warn_sink if a warning is raised, the warning information will be added here (0 = no warnings)
      @param warn_mask the warning mask to set (-1 sets all possible warnings)
      @see QoreProgram::parseCommit()
      @see QoreProgram::parseRollback()
   */
   DLLEXPORT void parsePending(const char* code, const char* label, ExceptionSink* xsink, ExceptionSink* warn_sink = 0, int warn_mask = QP_WARN_ALL);

   //! parses code from the given string but does not commit changes to the QoreProgram
   /**
      @param code the code to parse; the encoding of the string is assumed to be QCS_DEFAULT
      @param label the label of the code being parsed to be used as a file name
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @param warn_sink if a warning is raised, the warning information will be added here (0 = no warnings)
      @param warn_mask the warning mask to set (-1 sets all possible warnings)
      @param source the source file name (if lstr is a label representing a section of a file for example)
      @param offset the line offset from the label to the file

      @see QoreProgram::parseCommit()
      @see QoreProgram::parseRollback()
   */
   DLLEXPORT void parsePending(const char* code, const char* label, ExceptionSink* xsink, ExceptionSink* warn_sink, int warn_mask, const char* source, int offset);

   //! parses code from the given string but does not commit changes to the QoreProgram
   /**
      @param str the code to parse
      @param lstr the label of the code being parsed to be used as a file name
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @param warn_sink if a warning is raised, the warning information will be added here (0 = no warnings)
      @param warn_mask the warning mask to set (-1 sets all possible warnings)

      @see QoreProgram::parseCommit()
      @see QoreProgram::parseRollback()
   */
   DLLEXPORT void parsePending(const QoreString* str, const QoreString* lstr, ExceptionSink* xsink, ExceptionSink* warn_sink = 0, int warn_mask = QP_WARN_ALL);

   //! parses code from the given string but does not commit changes to the QoreProgram
   /**
      @param str the code to parse
      @param lstr the label of the code being parsed to be used as a file name
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @param warn_sink if a warning is raised, the warning information will be added here (0 = no warnings)
      @param warn_mask the warning mask to set (-1 sets all possible warnings)
      @param source the source file name (if lstr is a label representing a section of a file for example)
      @param offset the line offset from the label to the file

      @see QoreProgram::parseCommit()
      @see QoreProgram::parseRollback()
   */
   DLLEXPORT void parsePending(const QoreString* str, const QoreString* lstr, ExceptionSink* xsink, ExceptionSink* warn_sink, int warn_mask, const QoreString* source, int offset);

   //! commits pending changes to the program
   /**
      @see QoreProgram::parsePending()
      @see QoreProgram::parseRollback()
   */
   DLLEXPORT void parseCommit(ExceptionSink* xsink, ExceptionSink* warn_sink = 0, int warn_mask = QP_WARN_ALL);

   //! rolls back changes to the program object that were added with QoreProgram::parsePending()
   /**
      @see QoreProgram::parsePending()
      @see QoreProgram::parseCommit()

      @deprecated use parseRollback(ExceptionSink*) instead; exceptions raised with this version cannot be caught
   */
   DLLEXPORT void parseRollback();

   //! rolls back changes to the program object that were added with QoreProgram::parsePending()
   /** a Qore-language exception could be raised if the parse lock could not be acquired (Program has running threads)

       @see QoreProgram::parsePending()
       @see QoreProgram::parseCommit()
   */
   DLLEXPORT int parseRollback(ExceptionSink* xsink);

   //! returns true if the given function exists as a user function, false if not
   DLLEXPORT bool existsFunction(const char* name);

   //! dereferences the object and deletes it if the reference count reaches zero
   /** do not use this function if the program may be running, use QoreProgram::waitForTerminationAndDeref() instead
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @see QoreProgram::waitForTerminationAndDeref()
   */
   using AbstractPrivateData::deref;
   DLLEXPORT virtual void deref(ExceptionSink* xsink);

   //! references "this" and returns a non-const pointer to itself
   DLLEXPORT QoreProgram* programRefSelf() const;

   //! locks parse options so they may not be changed
   DLLEXPORT void lockOptions();

   //! sets the name of the application class to be executed (instantiated) instead of top-level code
   /** normally parse option PO_NO_TOP_LEVEL_STATEMENTS should be set as well
       @note the string passed here will copied
       @param ecn the name of the class to be executed as the program class
       @see QoreProgram::parseSetParseOptions()
   */
   DLLEXPORT void setExecClass(const char* ecn = 0);

   //! adds the parse options given to the parse option mask; DEPRECATED: use parseSetParseOptions(int64) instead
   DLLEXPORT void parseSetParseOptions(int po);

   //! adds the parse options given to the parse option mask
   DLLEXPORT void parseSetParseOptions(int64 po);

   //! disables the parse options given to the parse option mask
   DLLEXPORT void parseDisableParseOptions(int64 po);

   //! this call blocks until the program's last thread terminates
   DLLEXPORT void waitForTermination();

   //! this call blocks until the program's last thread terminates, and then calls QoreProgram::deref()
   /**
      @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT void waitForTerminationAndDeref(ExceptionSink* xsink);

   //! returns a pointer to the "Qore" namespace
   DLLEXPORT QoreNamespace* getQoreNS() const;

   //! returns a pointer to the root namespace
   DLLEXPORT RootQoreNamespace* getRootNS() const;

   //! sets the warning mask
   /**
      @param wm the new warning mask
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

   //! returns the parse options currently set for this program; DEPRECATED; use getParseOptions64() instead
   DLLEXPORT int getParseOptions() const;

   //! returns the parse options currently set for this program
   DLLEXPORT int64 getParseOptions64() const;

   //! sets the parse options and adds Qore-language exception information if an error occurs
   /** DEPRECATED: use setParseOptions(int64, ...)
       @param po the parse options to add to the parse option mask
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT void setParseOptions(int po, ExceptionSink* xsink);

   //! sets the parse options and adds Qore-language exception information if an error occurs
   /**
      @param po the parse options to add to the parse option mask
      @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT void setParseOptions(int64 po, ExceptionSink* xsink);

   //! turns off the parse options given in the passed mask and adds Qore-language exception information if an error occurs
   /** DEPRECATED: use disableParseOptions(int64, ...) instead
       @param po the parse options to subtract from the parse option mask
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT void disableParseOptions(int po, ExceptionSink* xsink);

   //! turns off the parse options given in the passed mask and adds Qore-language exception information if an error occurs
   /**
      @param po the parse options to subtract from the parse option mask
      @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT void disableParseOptions(int64 po, ExceptionSink* xsink);

   //! replaces the parse options in the program with those given by the argument; adds Qore-language exception information if an error occurs
   /** An exception will be raised if the calling program does not have PO_NO_CHILD_PO_RESTRICTIONS set
       @param po the parse options to add to the parse option mask
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT void replaceParseOptions(int64 po, ExceptionSink* xsink);

   //! returns a list of all user functions in this program
   /**
      @return a list of all user functions in this program; returns 0 only if the Program is being destroyed, otherwise returns an empty list
   */
   DLLEXPORT QoreListNode* getUserFunctionList();

   //! returns true if the warning code is set
   DLLEXPORT bool checkWarning(int code) const;

   //! returns the warning mask
   DLLEXPORT int getWarningMask() const;

   //! returns true if the given feature is present in the program object
   DLLEXPORT bool checkFeature(const char* f) const;

   //! returns a list of features in the program object
   DLLEXPORT QoreListNode* getFeatureList() const;

   //! returns the script file name, if known (0 if not)
   /** @return the script file name, if known (0 if not)
    */
   DLLEXPORT QoreStringNode* getScriptName() const;

   //! returns the script path (directory and name), if known (0 if not)
   /** @return the script path (directory and name), if known (0 if not)
    */
   DLLEXPORT QoreStringNode* getScriptPath() const;

   //! returns the script directory, if known (0 if not)
   /** @return the script directory, if known (0 if not)
    */
   DLLEXPORT QoreStringNode* getScriptDir() const;

   //! sets the script path
   /** @param path the directory and filename of the script (set to 0 to clear)
    */
   DLLEXPORT void setScriptPath(const char* path);

   //! returns the value of the global variable given (do not include the "$" symbol), the caller owns the reference count returned
   /** @param var the variable name to return (do not include the "$" symbol)
       @param found returns true if the variable exists, false if not
       @return the value of the global variable given; if a non-zero pointer is returned, the caller owns the reference count returned
   */
   DLLEXPORT AbstractQoreNode* getGlobalVariableValue(const char* var, bool &found) const;

   //! returns the value of the global variable given (do not include the "$" symbol), the caller owns the reference count returned
   /** @param var the variable name to return (do not include the "$" symbol)
       @param found returns true if the variable exists, false if not
       @return the value of the global variable given; if a non-zero pointer is returned, the caller owns the reference count returned
   */
   DLLEXPORT QoreValue getGlobalVariableVal(const char* var, bool &found) const;

   // retrieves the time zone setting for the program
   DLLEXPORT const AbstractQoreZoneInfo *currentTZ() const;

   // sets the program's time zone
   DLLEXPORT void setTZ(const AbstractQoreZoneInfo *n_TZ);

   //! manually add the feature to the program
   /** useful for use with %module-cmd when manually merging in namespace changes to the program
       @param name the name of the feature to add to the QoreProgram's feature list
    */
   DLLEXPORT void addFeature(const char* name);

   //! sets the time zone during parsing
   /** @param zone can be either a region name (ex: 'Europe/Prague') or a UTC offset in the format SDD[:DD[:DD]] where S is + or - and D is an integer 0 - 9; the ':' characters are optional

       @note do not call this function if there are any running threads; a crash could result
    */
   DLLEXPORT void parseSetTimeZone(const char* zone);

   //! defines a parse-time variable; call only at parse time (or before parsing)
   /** @param str the name of the variable
       @param val the value of the variable; may be 0; if non-0, then the QoreProgram object assumes ownership of the reference

       @note if this function is called at runtime it could cause a crash

       @see runtimeDefine()
   */
   DLLEXPORT void parseDefine(const char* str, AbstractQoreNode* val);

   //! defines a parse-time variables
   /**
       @param xs exception sink for errors
       @param ws exception sink for warnings
       @param w warnings mask
       @param defmap a map of variable names to values
   */
   DLLEXPORT void parseCmdLineDefines(ExceptionSink& xs, ExceptionSink& ws, int w, const std::map<std::string, std::string>& defmap);

#ifdef _QORE_LIB_INTERN
   // deprecated function still part of the ABI 
   DLLEXPORT void parseCmdLineDefines(const std::map<std::string, std::string> defmap, ExceptionSink& xs, ExceptionSink& ws, int w);
#endif

   DLLLOCAL QoreProgram(QoreProgram* pgm, int64 po, bool ec = false, const char* ecn = 0);

   DLLLOCAL LocalVar *createLocalVar(const char* name, const QoreTypeInfo *typeInfo);

   // returns 0 if a "requires" exception has already occurred
   DLLLOCAL ExceptionSink* getParseExceptionSink();

   DLLLOCAL QoreHashNode* getThreadData();
   DLLLOCAL void depRef();
   DLLLOCAL void depDeref(ExceptionSink* xsink);
   DLLLOCAL void addFile(char* f);
   DLLLOCAL QoreListNode* getVarList();
   DLLLOCAL void parseSetIncludePath(const char* path);
   DLLLOCAL const char* parseGetIncludePath() const;

   /* for run-time module loading; the parse lock must be grabbed
      before loading new modules - note this should only be assigned
      to an AutoLock or SafeLocker object!
   */
   DLLLOCAL QoreThreadLock *getParseLock();
   DLLLOCAL const LVList* getTopLevelLVList() const;

   //! returns the script directory, if known (0 if not), does not grab the parse lock, only to be called while parsing
   /** @return the script directory, if known (0 if not)
    */
   DLLLOCAL const char* parseGetScriptDir() const;

   // can only be called while parsing from the same thread doing the parsing
   DLLLOCAL bool parseExceptionRaised() const;
};

//! safely manages QoreProgram objects; note the the destructor will block until all background threads in the qore library terminate and until the current QoreProgram terminates
/** not useful in embedded code due to the fact that the destructor blocks until all background threads in the entire qore library terminate
 */
class QoreProgramHelper {
private:
   QoreProgram* pgm;
   ExceptionSink& xsink;

public:
   //! creates the QoreProgram object: DEPRECATED: use QoreProgramHelper(int64, ExceptionSink&) instead
   DLLEXPORT QoreProgramHelper(ExceptionSink& xs);

   //! creates the QoreProgram object and sets the parse options
   DLLEXPORT QoreProgramHelper(int64 parse_options, ExceptionSink& xs);

   //! waits until all background threads in the Qore library have terminated and until the QoreProgram object is done executing and then dereferences the object
   /** QoreProgram objects are deleted when there reference count reaches 0.
    */
   DLLEXPORT ~QoreProgramHelper();

   //! returns the QoreProgram object being managed
   DLLEXPORT QoreProgram* operator->();

   //! returns the QoreProgram object being managed
   DLLEXPORT QoreProgram* operator*();
};

#endif  // _QORE_QOREPROGRAM_H
