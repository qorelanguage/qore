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
#include <qore/ReferenceObject.h>
#include <qore/LockedObject.h>
#include <qore/Restrictions.h>
#include <qore/QoreCounter.h>
#include <qore/StringList.h>
#include <qore/support.h>

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef CP_UFUNC_BLOCK
#define CP_UFUNC_BLOCK 200
#endif

static inline void addProgramConstants(class Namespace *ns);

class SBNode {
   public:
      class StatementBlock *statements;
      class SBNode *next;

      inline SBNode() { next = NULL; statements = NULL; }
      inline ~SBNode();
      inline void reset();
};

class ImportedFunctionNode
{
   public:
      class QoreProgram *pgm;
      class UserFunction *func;
      class ImportedFunctionNode *next;

      inline ImportedFunctionNode(class QoreProgram *p, class UserFunction *u)
      {
	 pgm = p;
	 func = u;
	 next = NULL;
      }
};

class ImportedFunctionList
{
   private:
      class ImportedFunctionNode *head, *tail;

   public:
      inline ImportedFunctionList();
      inline ~ImportedFunctionList();
      inline void addImportedFunction(class QoreProgram *pgm, class UserFunction *func);
      inline class UserFunction *findImportedFunction(char *name);
      inline class ImportedFunctionNode *findImportedFunctionNode(char *name);
};

class UserFunctionList
{
   private:
      class UserFunction *head, *tail, *phead, *ptail;

   public:
      inline UserFunctionList();
      inline ~UserFunctionList();
      inline class UserFunction *findUserFunction(char *name);
      inline void addUserFunction(class UserFunction *func);
      inline void parseInitUserFunctions();
      inline void deletePendingUserFunctions();
      inline void mergePendingUserFunctions();
      inline class List *getUFList();
};

// this is a "grow-only" linked list, so locking is only done on additions
// locking is necessary because of the importGlobalVariable() function
class GlobalVariableList : public LockedObject
{
   private:
      class Var *head;
      class Var *tail;

   public:
      inline GlobalVariableList();
      inline ~GlobalVariableList();
      inline void delete_all(class ExceptionSink *xsink);
      inline void clear_all(class ExceptionSink *xsink);
      inline void importGlobalVariable(class Var *var, class ExceptionSink *xsink, bool readonly = false);
      inline class Var *newVar(char *name);
      inline class Var *findVar(char *name);
      inline class Var *checkVar(char *name, int *new_vars);
#ifdef DEBUG
      inline class List *getVarList();
#endif
};

// the two-layered reference counting is to eliminate problems from circular references
// when a program has a global variable that contains an object that references the program...
// objects now reference the dependency counter, so when the object's counter reaches zero and
// the global variable list is deleted, then the variables will in turn dereference the program
// so it can be deleted...

class QoreProgram : public ReferenceObject, private UserFunctionList, private ImportedFunctionList, public GlobalVariableList
{
   private:
      // parse lock, making parsing actions atomic and thread-safe
      class LockedObject plock;
      // depedency counter, when this hits zero, the object is deleted
      class ReferenceObject dc;

      class SBNode *sb_head, *sb_tail;
      class ExceptionSink *parseSink;
      class Namespace *RootNS, *QoreNS;

      int parse_options;
      bool po_locked, exec_class, base_object, requires_exception;
      char *exec_class_name;
      pthread_key_t thread_local_storage;

      void init();
      inline void nextSB();
      inline void deleteSBList();
      inline void deleteUserFunctions();
      void internParseCommit(class ExceptionSink *xsink);
      inline void initGlobalVars();
      void importUserFunction(class QoreProgram *p, class UserFunction *uf, class ExceptionSink *xsink);
      inline void internParseRollback();
      int internParsePending(char *code, char *label, class ExceptionSink *xsink);

   protected:
      inline ~QoreProgram();

   public:
      // for the thread counter
      class QoreCounter tcount;
      class HFStringList fileList;

      class charPtrList featureList;
      // QoreProgram() NOTE: ecn is the exec_class_name and will be copied if it exists
      QoreProgram();
      QoreProgram(class QoreProgram *pgm, int po, bool ec = false, char *ecn = NULL);
      inline void registerUserFunction(class UserFunction *u);
      class QoreNode *callFunction(char *name, class QoreNode *args, class ExceptionSink *xsink);
      class QoreNode *callFunction(class UserFunction *func, class QoreNode *args, class ExceptionSink *xsink);
      void resolveFunction(class FunctionCall *f);
      class QoreNode *run(class ExceptionSink *xsink);
      void parse(FILE *, char *name, class ExceptionSink *);
      void parse(class QoreString *str, class QoreString *lstr, class ExceptionSink *);
      void parse(char *str, char *lstr, class ExceptionSink *);
      void parseFile(char *filename, class ExceptionSink *);
      inline void parsePending(char *code, char *label, class ExceptionSink *xsink);
      void parsePending(class QoreString *str, class QoreString *lstr, class ExceptionSink *xsink);
      void parseCommit(class ExceptionSink *xsink);
      void parseRollback();
      inline void addGlobalVarDef(char *name);
      inline void addStatement(class Statement *s);
      inline bool existsFunction(char *name);
      inline void ref()
      {
	 //printd(5, "QoreProgram::ref() this=%08p %d->%d\n", this, reference_count(), reference_count() + 1);
	 ROreference();
      }
      inline void deref();
      inline void deref(class ExceptionSink *xsink)
      {
	 //printd(5, "QoreProgram::deref() this=%08p %d->%d\n", this, reference_count(), reference_count() - 1);
	 if (ROdereference())
	 {
	    // delete all global variables
	    clear_all(xsink);
	    depDeref(xsink);
	 }
      }

      inline class Var *findVar(char *name);
      inline class Var *checkVar(char *name);
      inline class Var *createVar(char *name);
      inline void makeParseException(class QoreString *desc);
      inline void addParseException(class ExceptionSink *xsink);
      inline void cannotProvideFeature(char *feature);
      inline class Namespace *getRootNS() { return RootNS; }
      inline int getParseOptions() { return parse_options; }
      inline void exportUserFunction(char *name, class QoreProgram *p, class ExceptionSink *xsink);
      inline class List *getUserFunctionList() { return getUFList(); }
      inline void waitForTerminationAndDeref()
      {
	 tcount.waitForZero();
	 deref();
      }
      inline void parseSetParseOptions(int po);
      inline void setParseOptions(int po, class ExceptionSink *xsink);
      inline void disableParseOptions(int po, class ExceptionSink *xsink);
      inline void lockOptions() { po_locked = true; }

      // setExecClass() NOTE: string passed here will copied
      inline void setExecClass(char *ecn = NULL)
      {
	 exec_class = true;
	 exec_class_name = ecn ? strdup(ecn) : NULL;
      }
      class Namespace *getQoreNS()
      {
	 return QoreNS;
      }

      void endThread(class ExceptionSink *xsink);
      inline void startThread();
      inline class Hash *getThreadData();

      void parseFileAndRun(char *filename);
      void parseAndRun(FILE *, char *name);
      //void parseAndRun(class QoreString *str, class QoreString *name);
      void parseAndRun(char *str, char *name);

      void runClass(char *classname, class ExceptionSink *xsink);
      void parseFileAndRunClass(char *filename, char *classname);
      void parseAndRunClass(FILE *, char *name, char *classname);
      void parseAndRunClass(char *str, char *name, char *classname);
      void depRef()
      {
	 dc.ROreference();
      }
      void depDeref(class ExceptionSink *xsink)
      {
	 if (dc.ROdereference())
	 {
	    del(xsink);
	    delete this;
	 }
      }
      void del(class ExceptionSink *xsink);
};

#include <qore/Function.h>
#include <qore/thread.h>
#include <qore/support.h>
#include <qore/Exception.h>
#include <qore/Statement.h>
#include <qore/Variable.h>
#include <qore/Namespace.h>
#include <qore/charset.h>

inline QoreProgram::~QoreProgram()
{
   printd(5, "QoreProgram::~QoreProgram() this=%08p\n", this);
}

inline void QoreProgram::deref()
{
   //printd(5, "QoreProgram::deref() this=%08p %d->%d\n", this, reference_count(), reference_count() - 1);
   if (ROdereference())
   {
      // delete all global variables with default exception handler
      ExceptionSink xsink;
      clear_all(&xsink);
      depDeref(&xsink);
   }
}

static inline void addProgramConstants(class Namespace *ns)
{
   ns->addConstant("PO_DEFAULT",                  new QoreNode(NT_INT, PO_DEFAULT));
   ns->addConstant("PO_NO_GLOBAL_VARS",           new QoreNode(NT_INT, PO_NO_GLOBAL_VARS));
   ns->addConstant("PO_NO_SUBROUTINE_DEFS",       new QoreNode(NT_INT, PO_NO_SUBROUTINE_DEFS));  
   ns->addConstant("PO_NO_THREADS",               new QoreNode(NT_INT, PO_NO_THREADS));
   ns->addConstant("PO_NO_TOP_LEVEL_STATEMENTS",  new QoreNode(NT_INT, PO_NO_TOP_LEVEL_STATEMENTS));  
   ns->addConstant("PO_NO_CLASS_DEFS",            new QoreNode(NT_INT, PO_NO_CLASS_DEFS));
   ns->addConstant("PO_NO_NAMESPACE_DEFS",        new QoreNode(NT_INT, PO_NO_NAMESPACE_DEFS));
   ns->addConstant("PO_NO_CONSTANT_DEFS",         new QoreNode(NT_INT, PO_NO_CONSTANT_DEFS));
   ns->addConstant("PO_NO_NEW",                   new QoreNode(NT_INT, PO_NO_NEW));
   ns->addConstant("PO_NO_SYSTEM_CLASSES",        new QoreNode(NT_INT, PO_NO_SYSTEM_CLASSES));
   ns->addConstant("PO_NO_USER_CLASSES",          new QoreNode(NT_INT, PO_NO_USER_CLASSES));
   ns->addConstant("PO_NO_CHILD_PO_RESTRICTIONS", new QoreNode(NT_INT, PO_NO_CHILD_PO_RESTRICTIONS));
   ns->addConstant("PO_NO_EXTERNAL_PROCESS",      new QoreNode(NT_INT, PO_NO_EXTERNAL_PROCESS));
   ns->addConstant("PO_REQUIRE_OUR",              new QoreNode(NT_INT, PO_REQUIRE_OUR));
   ns->addConstant("PO_NO_PROCESS_CONTROL",       new QoreNode(NT_INT, PO_NO_PROCESS_CONTROL));
   ns->addConstant("PO_NO_NETWORK",               new QoreNode(NT_INT, PO_NO_NETWORK));
   ns->addConstant("PO_NO_FILESYSTEM",            new QoreNode(NT_INT, PO_NO_FILESYSTEM));
}

inline int checkParseOption(int o)
{
   return getParseOptions() & o;
}

inline SBNode::~SBNode()
{
   if (statements) delete statements; 
}

inline void SBNode::reset()
{
   if (statements)
   {
      delete statements;
      statements = NULL;
   }
}

inline ImportedFunctionList::ImportedFunctionList()
{
   head = tail = NULL;
}

inline ImportedFunctionList::~ImportedFunctionList()
{
   while (head)
   {
      tail = head->next;
      delete head;
      head = tail;
   }
}

inline void ImportedFunctionList::addImportedFunction(class QoreProgram *pgm, class UserFunction *func)
{
   tracein("ImportedFunctionList::addImportedFunction()");

   ImportedFunctionNode *n = new ImportedFunctionNode(pgm, func);

   if (tail)
      tail->next = n;
   else
      head = n;
   tail = n;

   traceout("ImportedFunctionList::addImportedFunction()");
}

inline class ImportedFunctionNode *ImportedFunctionList::findImportedFunctionNode(char *name)
{
   printd(5, "ImportedFunctionList::findImportedFunctionNode(%s)\n", name);
   class ImportedFunctionNode *w = head;

   while (w)
   {
      if (!strcmp(w->func->name, name))
	 return w;

      w = w->next;
   }
   
   return NULL;
}

inline class UserFunction *ImportedFunctionList::findImportedFunction(char *name)
{
   printd(5, "ImportedFunctionList::findImportedFunction(%s)\n", name);
   class ImportedFunctionNode *w = head;

   while (w)
   {
      if (!strcmp(w->func->name, name))
	 return w->func;

      w = w->next;
   }
   
   return NULL;
}

inline UserFunctionList::UserFunctionList()
{
   head = tail = phead = ptail = NULL;
}

inline UserFunctionList::~UserFunctionList()
{
   deletePendingUserFunctions();
   while (head)
   {
      tail = head->next;
      head->deref();
      head = tail;
   }
}

inline void UserFunctionList::addUserFunction(class UserFunction *func)
{
   tracein("UserFunctionList::addUserFunction()");
   
   if (findUserFunction(func->name))
      parse_error("user function \"%s\" has already been defined", func->name);
   else
   {
      if (ptail)
	 ptail->next = func;
      else
	 phead = func;
      ptail = func;
   }

   traceout("UserFunctionList::addUserFunction()");
}

inline class UserFunction *UserFunctionList::findUserFunction(char *name)
{
   printd(5, "UserFunctionList::findUserFunction(%s)\n", name);
   // first look in pending functions
   class UserFunction *w = phead;

   while (w)
   {
      if (!strcmp(w->name, name))
	 break;
      w = w->next;
   }

   if (w)
      return w;

   w = head;

   while (w)
   {
      if (!strcmp(w->name, name))
	 break;
      w = w->next;
   }

   printd(5, "UserFunctionList::findUserFunction(%s) returning %08p\n", name, w);
   return w;
}

inline class List *UserFunctionList::getUFList()
{
   tracein("UserFunctionList::getUFList()");

   class List *l = new List();

   class UserFunction *w = head;
   while (w)
   {
      l->push(new QoreNode(w->name));
      w = w->next;
   }

   traceout("UserFunctionList::getUFList()");
   return l;
}

// unlocked
inline void UserFunctionList::parseInitUserFunctions()
{
   tracein("UserFunctionList::parseInitUserFunctions()");

   class UserFunction *w = phead;
   while (w)
   {
      // can (and must) be called if if w->statements is NULL
      w->statements->parseInit(w->params);
      w = w->next;
   }

   traceout("UserFunctionList::parseInitUserFunctions()");
}

// unlocked
inline void UserFunctionList::mergePendingUserFunctions()
{
   if (tail)
      tail->next = phead;
   else
      head = phead;
   if (ptail)
   {
      tail = ptail;
      ptail = phead = NULL;
   }
}

// unlocked
inline void UserFunctionList::deletePendingUserFunctions()
{
   tracein("UserFunctionList::deletePendingUserFunctions()");
   while (phead)
   {
      ptail = phead->next;
      phead->deref();
      phead = ptail;
   }
   phead = ptail = NULL;
   traceout("UserFunctionList::deletePendingUserFunctions()");
}

inline GlobalVariableList::GlobalVariableList()
{
   head = tail = NULL;
   printd(5, "GlobalVariableList::GlobalVariableList() %08p (%08p %08p)\n",
	  this, head, tail);
}

inline GlobalVariableList::~GlobalVariableList()
{
#ifdef DEBUG
   if (head)
      run_time_error("~GlobalVariableList(): FIXME: head = %08p", head);
#endif
}

// sets all non-imported variables to NULL (dereferences contents if any)
inline void GlobalVariableList::clear_all(class ExceptionSink *xsink)
{
   class Var *var = head;

   while (var)
   {
      if (!var->isImported())
      {
	 printd(5, "GlobalVariableList::clear_all() clearing '%s' (%08p)\n", var->getName(), var);
	 var->setValue(NULL, xsink);
      }
#ifdef DEBUG
      else printd(5, "GlobalVariableList::clear_all() skipping imported var '%s' (%08p)\n", var->getName(), var);
#endif
      var = var->next;
   }
}

inline void GlobalVariableList::delete_all(class ExceptionSink *xsink)
{
   class Var *var = head;

   while (var)
   {
      printd(5, "GlobalVariableList::delete_all() deleting \"%s\"\n", var->getName());
      head = var->next;
      var->deref(xsink);
      var = head;
   }
}

inline class Var *GlobalVariableList::newVar(char *name)
{
   class Var *var = new Var(name);

   if (tail)
      tail->next = var;
   else
      head = var;
   tail = var;

   printd(5, "GlobalVariableList::newVar(): %s (%08p) added (head=%08p, tail=%08p\n", name, var, head, tail);
   return var;
}

inline class Var *GlobalVariableList::findVar(char *name)
{
   tracein("GlobalVariableList::findVar()");

   class Var *var = head;

   while (var)
   {
      //printd(5, "GlobalVariableList::findVar() head=%08p var=%08p (%s == %s)\n", head, var, var->getName(), name);
      if (!strcmp(var->getName(), name))
	 break;
      var = var->next;
   }
   //if (var)
   //   printf("GlobalVariableList::findVar(): var %s found (addr=%08p)\n", name, var);
   traceout("GlobalVariableList::findVar()");
   return var;
}

// used for resolving unflagged global variables
inline class Var *GlobalVariableList::checkVar(char *name, int *new_var)
{
   tracein("GlobalVariableList::checkVar()");
   class Var *var;

   lock();
   if (!(var = findVar(name)))
   {
      *new_var = 1;
      var = newVar(name);
   }
   unlock();
   traceout("GlobalVariableList::checkVar()");
   return var;
}

#ifdef DEBUG
class List *GlobalVariableList::getVarList()
{
   List *l = new List();

   class Var *var = head;

   while (var)
   {
      //printd(5, "name=%s\n", var->name);
      l->push(new QoreNode(var->getName()));
      var = var->next;
   }
   return l;
}
#endif

inline class Var *QoreProgram::findVar(char *name)
{
   return GlobalVariableList::findVar(name);
}

inline class Var *QoreProgram::checkVar(char *name)
{
   int new_var = 0;
   class Var *rv = GlobalVariableList::checkVar(name, &new_var);
   if (new_var)
   {
      printd(5, "QoreProgram::checkVar() new global var \"%s\" found\n", name);
      // check if unflagged global vars are allowed
      if (parse_options & PO_REQUIRE_OUR)
	 parse_error("global variable \"%s\" must first be declared with \"our\" (conflicts with parse option REQUIRE_OUR)", name);
      // check if new global variables are allowed to be created at all
      else if (parse_options & PO_NO_GLOBAL_VARS)
	 parse_error("illegal reference to new global variable \"%s\" (conflicts with parse option NO_GLOBAL_VARS)", name);
   }
   return rv;
}

inline class Var *QoreProgram::createVar(char *name)
{
   int new_var = 0;
   class Var *rv = GlobalVariableList::checkVar(name, &new_var);
   // it's a new global variable: check if global variables are allowed
   if ((parse_options & PO_NO_GLOBAL_VARS) && new_var)
      parse_error("illegal reference to new global variable \"%s\" (conflicts with parse option NO_GLOBAL_VARS)", name);

   printd(5, "QoreProgram::createVar() global var \"%s\" processed, new_var=%d\n", name, new_var);

   return rv;
}

inline void QoreProgram::makeParseException(class QoreString *desc)
{
   tracein("QoreProgram::makeParseException()");
   if (!requires_exception)
   {
      class Exception *ne = new Exception("PARSE-EXCEPTION", get_pgm_stmt(), desc);
      parseSink->raiseException(ne);
   }
   traceout("QoreProgram::makeParseException()");
}

inline void QoreProgram::addParseException(class ExceptionSink *xsink)
{
   if (requires_exception)
   {
      delete xsink;
      return;
   }
   parseSink->assimilate(xsink);
}

inline void QoreProgram::cannotProvideFeature(char *feature)
{
   tracein("QoreProgram::cannotProvideFeature()");
   
   if (!requires_exception)
   {
      parseSink->clear();
      requires_exception = true;
   }
   class Exception *ne = new Exception("CANNOT-PROVIDE-FEATURE", "feature '%s' is not builtin and no module with this name can be loaded", feature);
   parseSink->raiseException(ne);
   
   traceout("QoreProgram::cannotProvideFeature()");
}

inline void QoreProgram::exportUserFunction(char *name, class QoreProgram *p, class ExceptionSink *xsink)
{
   if (this == p)
      xsink->raiseException("PROGRAM-IMPORTFUNCTION-PARAMETER-ERROR", "cannot import a function from the same Program object");
   else
   {
      class UserFunction *u;
      if (!(u = findUserFunction(name)) && !(u = findImportedFunction(name)))
	 xsink->raiseException("PROGRAM-IMPORTFUNCTION-NO-FUNCTION", "function \"%s\" does not exist in the current program scope", name);
      else
	 p->importUserFunction(this, u, xsink);
   }
}

inline void QoreProgram::deleteSBList()
{
   SBNode *w = sb_head;
   while (w)
   {
      w = w->next;
      delete sb_head;
      sb_head = w;
   }
}

inline void GlobalVariableList::importGlobalVariable(class Var *var, class ExceptionSink *xsink, bool readonly)
{
   lock();

   class Var *nvar = findVar(var->getName());
   if (!nvar)
      nvar = newVar(var->getName());

   unlock();
   nvar->makeReference(var, xsink, readonly);
}

// called during parsing (plock already grabbed)
inline void QoreProgram::registerUserFunction(UserFunction *u)
{
   // check if an imported function already exists with this name
   if (findImportedFunction(u->name))
      parse_error("function \"%s\" has already been imported into this program", u->name);
   else
      addUserFunction(u);
}

inline void QoreProgram::internParseRollback()
{
   // delete pending user functions
   deletePendingUserFunctions();
   
   // delete pending changes to namespaces
   RootNS->parseRollback();
   
   // delete pending statements 
   sb_tail->reset();
}

// interface to bison/flex parser/scanner
typedef void *yyscan_t;
extern int yyparse(yyscan_t yyscanner);
extern struct yy_buffer_state *yy_scan_string(const char *, yyscan_t scanner);
int yylex_init(yyscan_t *scanner);
void yyset_in(FILE *in_str, yyscan_t yyscanner);
int yylex_destroy(yyscan_t yyscanner);
//extern FILE *yyin;

inline void QoreProgram::nextSB()
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

// if this global variable definition is illegal, then
// it will be flagged in the parseCommit stage
inline void QoreProgram::addGlobalVarDef(char *name)
{
   class Statement *s = new Statement(S_EXPRESSION, new QoreNode(new VarRef(name, VT_GLOBAL)));
   if (!sb_tail->statements)
      sb_tail->statements = new StatementBlock(s);
   else
      sb_tail->statements->addStatement(s);
}

inline void QoreProgram::addStatement(class Statement *s)
{
   if (!sb_tail->statements)
      sb_tail->statements = new StatementBlock(s);
   else
      sb_tail->statements->addStatement(s);

   // see if top level statements are allowed
   if (parse_options & PO_NO_TOP_LEVEL_STATEMENTS)
      parse_error("illegal top-level statement (conflicts with parse option NO_TOP_LEVEL_STATEMENTS)");
}

inline bool QoreProgram::existsFunction(char *name)
{
   return (bool)(findUserFunction(name) != NULL);
}

inline void QoreProgram::parseSetParseOptions(int po)
{
   if (po_locked)
   {
      parse_error("parse options have been locked on this program object");
      return;
   }
   parse_options |= po;
}

inline void QoreProgram::setParseOptions(int po, class ExceptionSink *xsink)
{
   if (po_locked)
   {
      xsink->raiseException("OPTIONS-LOCKED", "parse options have been locked on this program object");
      return;
   }
   parse_options |= po;
}

inline void QoreProgram::disableParseOptions(int po, class ExceptionSink *xsink)
{
   if (po_locked)
   {
      xsink->raiseException("OPTIONS-LOCKED", "parse options have been locked on this program object");
      return;
   }
   parse_options &= (~po);
}

inline void QoreProgram::parsePending(char *code, char *label, class ExceptionSink *xsink)
{
   if (!code || !code[0])
      return;

   pushProgram(this);

   // grab program-level parse lock
   plock.lock();

   internParsePending(code, label, xsink);

   // release program-level parse lock
   plock.unlock();

   popProgram();
}

inline void QoreProgram::startThread()
{
   pthread_setspecific(thread_local_storage, new Hash());
}

inline class Hash *QoreProgram::getThreadData()
{
   return (class Hash *)pthread_getspecific(thread_local_storage);
}

#endif // _QORE_QOREPROGRAM_H
