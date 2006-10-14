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
#include <qore/QoreWarnings.h>

#include <qore/hash_map.h>

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <strings.h>

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

// all read and write access to this list is done within the program object's parse lock
class UserFunctionList
{
   private:
      hm_uf_t fmap, pmap;   // maps of functions for quick lookups

   public:
      inline UserFunctionList() {}
      inline ~UserFunctionList();
      inline void deleteUserFunctions();
      inline class UserFunction *findUserFunction(char *name);
      inline void addUserFunction(class UserFunction *func);
      inline void parseInitUserFunctions();
      inline void deletePendingUserFunctions();
      inline void mergePendingUserFunctions();
      inline class List *getUFList();
};

// this is a "grow-only" container
// all reading and writing is done withing the parse lock on the contining program object
class GlobalVariableList
{
   private:
      hm_var_t vmap; // iterators are not invalidated on inserts

   public:
      inline GlobalVariableList() {}
#ifdef DEBUG
      inline ~GlobalVariableList()
      {
	 if (vmap.size())
	    run_time_error("~GlobalVariableList(): FIXME: size = %d", vmap.size());
      }
#endif
      
      inline void delete_all(class ExceptionSink *xsink);
      inline void clear_all(class ExceptionSink *xsink);
      inline void importGlobalVariable(class Var *var, class ExceptionSink *xsink, bool readonly = false);
      inline class Var *newVar(char *name);
      inline class Var *newVar(class Var *v, bool readonly);
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
      class ExceptionSink *parseSink, *warnSink;
      class Namespace *RootNS, *QoreNS;

      int parse_options, warn_mask;
      bool po_locked, exec_class, base_object, requires_exception;
      char *exec_class_name;
      pthread_key_t thread_local_storage;

      void init();
      inline void nextSB();
      inline void deleteSBList();
      void internParseCommit();
      inline void initGlobalVars();
      void importUserFunction(class QoreProgram *p, class UserFunction *uf, class ExceptionSink *xsink);
      inline void internParseRollback();
      int internParsePending(char *code, char *label);
      inline class Hash *clearThreadData(class ExceptionSink *xsink);

   protected:
      inline ~QoreProgram();

   public:
      // for the thread counter
      class QoreCounter tcount;
      class StringList fileList;

      class charPtrList featureList;
      // QoreProgram() NOTE: ecn is the exec_class_name and will be copied if it exists
      QoreProgram();
      QoreProgram(class QoreProgram *pgm, int po, bool ec = false, char *ecn = NULL);
      inline void registerUserFunction(class UserFunction *u);
      class QoreNode *callFunction(char *name, class QoreNode *args, class ExceptionSink *xsink);
      class QoreNode *callFunction(class UserFunction *func, class QoreNode *args, class ExceptionSink *xsink);
      void resolveFunction(class FunctionCall *f);

      inline class QoreNode *run(class ExceptionSink *xsink);
      class QoreNode *runTopLevel(class ExceptionSink *xsink);
      void parseFileAndRun(char *filename);
      void parseAndRun(FILE *, char *name);
      //void parseAndRun(class QoreString *str, class QoreString *name);
      void parseAndRun(char *str, char *name);
      void runClass(char *classname, class ExceptionSink *xsink);
      void parseFileAndRunClass(char *filename, char *classname);
      void parseAndRunClass(FILE *, char *name, char *classname);
      void parseAndRunClass(char *str, char *name, char *classname);
      
      void parse(FILE *, char *name, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      void parse(class QoreString *str, class QoreString *lstr, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      void parse(char *str, char *lstr, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      void parseFile(char *filename, class ExceptionSink *, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      inline void parsePending(char *code, char *label, class ExceptionSink *xsink, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      void parsePending(class QoreString *str, class QoreString *lstr, class ExceptionSink *xsink, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
      void parseCommit(class ExceptionSink *xsink, class ExceptionSink *warnSink = NULL, int warn_mask = -1);
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
	    // clear thread data if base object
	    if (base_object)
	       clearThreadData(xsink);
	    depDeref(xsink);
	 }
      }

      inline class Var *findVar(char *name);
      inline class Var *checkVar(char *name);
      inline class Var *createVar(char *name);
      inline void importGlobalVariable(class Var *var, class ExceptionSink *xsink, bool readonly)
      {
	 plock.lock();
	 GlobalVariableList::importGlobalVariable(var, xsink, readonly);
	 plock.unlock();
      }
      inline void makeParseException(char *err, class QoreString *desc);
      inline void makeParseException(class QoreString *desc);
      inline void addParseException(class ExceptionSink *xsink);
      inline void makeParseWarning(int code, char *warn, const char *fmt, ...);
      inline void addParseWarning(int code, class ExceptionSink *xsink);
      // returns 0 for success, -1 for error
      inline int setWarningMask(int wm) { if (!(parse_options & PO_LOCK_WARNINGS)) { warn_mask = wm; return 0; } return -1; }
      // returns 0 for success, -1 for error
      inline int enableWarning(int code) { if (!(parse_options & PO_LOCK_WARNINGS)) { warn_mask |= code; return 0; } return -1; }
      // returns 0 for success, -1 for error
      inline int disableWarning(int code) { if (!(parse_options & PO_LOCK_WARNINGS)) { warn_mask &= ~code; return 0; } return -1; }
      inline void cannotProvideFeature(class QoreString *desc);
      inline class Namespace *getRootNS() { return RootNS; }
      inline int getParseOptions() { return parse_options; }
      inline void exportUserFunction(char *name, class QoreProgram *p, class ExceptionSink *xsink);
      inline class List *getUserFunctionList() { return getUFList(); }
      inline void waitForTermination()
      {
	 tcount.waitForZero();
      }
      inline void waitForTerminationAndDeref(class ExceptionSink *xsink)
      {
	 tcount.waitForZero();
	 deref(xsink);
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

      void depRef()
      {
	 //printd(5, "QoreProgram::depRef() this=%08p %d->%d\n", this, dc.reference_count(), dc.reference_count() + 1);
	 dc.ROreference();
      }
      void depDeref(class ExceptionSink *xsink)
      {
	 //printd(5, "QoreProgram::depDeref() this=%08p %d->%d\n", this, dc.reference_count(), dc.reference_count() - 1);
	 if (dc.ROdereference())
	 {
	    del(xsink);
	    delete this;
	 }
      }
      void del(class ExceptionSink *xsink);
      bool checkWarning(int code) { return warnSink && (code & warn_mask); }
      int getWarningMask() { return warnSink ? warn_mask : 0; }
};

#include <qore/Function.h>
#include <qore/qore_thread.h>
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
      // clear thread data if base object
      if (base_object)
	 clearThreadData(&xsink);
      depDeref(&xsink);
   }
}

static inline void addProgramConstants(class Namespace *ns)
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

inline UserFunctionList::~UserFunctionList()
{
   deletePendingUserFunctions();
   deleteUserFunctions();
}

inline void UserFunctionList::deleteUserFunctions()
{
   hm_uf_t::iterator i;
   while ((i = fmap.begin()) != fmap.end())
   {
      class UserFunction *uf = i->second;
      fmap.erase(i);
      uf->deref();
   }
}

inline void UserFunctionList::addUserFunction(class UserFunction *func)
{
   tracein("UserFunctionList::addUserFunction()");
   
   if (findUserFunction(func->name))
      parse_error("user function \"%s\" has already been defined", func->name);
   else
      pmap[func->name] = func;

   traceout("UserFunctionList::addUserFunction()");
}

inline class UserFunction *UserFunctionList::findUserFunction(char *name)
{
   printd(5, "UserFunctionList::findUserFunction(%s)\n", name);
   // first look in pending functions
   hm_uf_t::iterator i = pmap.find(name);
   if (i != pmap.end())
      return i->second;
   
   i = fmap.find(name);
   if (i != fmap.end())
      return i->second;

   //printd(5, "UserFunctionList::findUserFunction(%s) returning %08p\n", name, w);
   return NULL;
}

inline class List *UserFunctionList::getUFList()
{
   tracein("UserFunctionList::getUFList()");

   class List *l = new List();
   hm_uf_t::iterator i = fmap.begin();
   while (i != fmap.end())
   {
      l->push(new QoreNode(i->first));      
      i++;
   }

   traceout("UserFunctionList::getUFList()");
   return l;
}

// unlocked
inline void UserFunctionList::parseInitUserFunctions()
{
   tracein("UserFunctionList::parseInitUserFunctions()");

   hm_uf_t::iterator i = pmap.begin();
   while (i != pmap.end())
   {
      // can (and must) be called if if w->statements is NULL
      i->second->statements->parseInit(i->second->params);
      i++;
   }

   traceout("UserFunctionList::parseInitUserFunctions()");
}

// unlocked
inline void UserFunctionList::mergePendingUserFunctions()
{
   hm_uf_t::iterator i;
   while ((i = pmap.begin()) != pmap.end())
   {
      fmap[i->first] = i->second;
      pmap.erase(i);
   }
}

// unlocked
inline void UserFunctionList::deletePendingUserFunctions()
{
   tracein("UserFunctionList::deletePendingUserFunctions()");
   hm_uf_t::iterator i;
   while ((i = pmap.begin()) != pmap.end())
   {
      class UserFunction *uf = i->second;
      pmap.erase(i);
      uf->deref();
   }
   traceout("UserFunctionList::deletePendingUserFunctions()");
}

// sets all non-imported variables to NULL (dereferences contents if any)
inline void GlobalVariableList::clear_all(class ExceptionSink *xsink)
{
   //printd(5, "GlobalVariableList::clear_all() this=%08p (size=%d)\n", this, vmap.size());
   hm_var_t::iterator i = vmap.begin();

   while (i != vmap.end())
   {
      if (!i->second->isImported())
      {
	 printd(5, "GlobalVariableList::clear_all() clearing '%s' (%08p)\n", i->first, i->second);
	 i->second->setValue(NULL, xsink);
      }
#ifdef DEBUG
      else printd(5, "GlobalVariableList::clear_all() skipping imported var '%s' (%08p)\n", i->first, i->second);
#endif
      i++;
   }
}

inline void GlobalVariableList::delete_all(class ExceptionSink *xsink)
{
   hm_var_t::iterator i;
   while ((i = vmap.begin()) != vmap.end())
   {
      class Var *v = i->second;
      vmap.erase(i);
      v->deref(xsink);
   }
}

inline class Var *GlobalVariableList::newVar(char *name)
{
   class Var *var = new Var(name);
   vmap[var->getName()] = var;
   
   printd(5, "GlobalVariableList::newVar(): %s (%08p) added\n", name, var);
   return var;
}

inline class Var *GlobalVariableList::newVar(class Var *v, bool readonly)
{
   class Var *var = new Var(v, readonly);
   vmap[var->getName()] = var;
   
   printd(5, "GlobalVariableList::newVar(): reference to %s (%08p) added\n", v->getName(), var);
   return var;
}

inline class Var *GlobalVariableList::findVar(char *name)
{
   hm_var_t::iterator i = vmap.find(name);
   if (i != vmap.end())
      return i->second;
   return NULL;
}

// used for resolving unflagged global variables
inline class Var *GlobalVariableList::checkVar(char *name, int *new_var)
{
   tracein("GlobalVariableList::checkVar()");
   class Var *var;

   if (!(var = findVar(name)))
   {
      *new_var = 1;
      var = newVar(name);
   }
   traceout("GlobalVariableList::checkVar()");
   return var;
}

#ifdef DEBUG
class List *GlobalVariableList::getVarList()
{
   List *l = new List();

   hm_var_t::iterator i = vmap.begin();
   while (i != vmap.end())
   {
      l->push(new QoreNode(i->first));
      i++;
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
	 parseException("UNDECLARED-GLOBAL-VARIABLE", "global variable '%s' must first be declared with 'our' (conflicts with parse option REQUIRE_OUR)", name);
      // check if new global variables are allowed to be created at all
      else if (parse_options & PO_NO_GLOBAL_VARS)
	 parseException("ILLEGAL-GLOBAL-VARIABLE", "illegal reference to new global variable '%s' (conflicts with parse option NO_GLOBAL_VARS)", name);
      else if (new_var)
	 makeParseWarning(QP_WARN_UNDECLARED_VAR, "UNDECLARED-GLOBAL-VARIABLE", "global variable '%s' should be declared with 'our'", name);
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

// if this global variable definition is illegal, then
// it will be flagged in the parseCommit stage
inline void QoreProgram::addGlobalVarDef(char *name)
{
   int new_var = 0;
   GlobalVariableList::checkVar(name, &new_var);
   if (!new_var)
      makeParseWarning(QP_WARN_DUPLICATE_GLOBAL_VARS, "DUPLICATE-GLOBAL-VARIABLE", "global variable '%s' has already been declared", name);
}
/*
inline void QoreProgram::addGlobalVarDef(char *name)
{
   class Statement *s = new Statement(S_EXPRESSION, new QoreNode(new VarRef(name, VT_GLOBAL)));
   if (!sb_tail->statements)
      sb_tail->statements = new StatementBlock(s);
   else
      sb_tail->statements->addStatement(s);
}
*/

inline void QoreProgram::makeParseException(char *err, class QoreString *desc)
{
   tracein("QoreProgram::makeParseException()");
   if (!requires_exception)
   {
      class Exception *ne = new Exception(err, get_pgm_stmt(), desc);
      parseSink->raiseException(ne);
   }
   else
      delete desc;
   traceout("QoreProgram::makeParseException()");
}

inline void QoreProgram::makeParseException(class QoreString *desc)
{
   tracein("QoreProgram::makeParseException()");
   if (!requires_exception)
   {
      class Exception *ne = new Exception("PARSE-EXCEPTION", get_pgm_stmt(), desc);
      parseSink->raiseException(ne);
   }
   else
      delete desc;
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

inline void QoreProgram::makeParseWarning(int code, char *warn, const char *fmt, ...)
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
   class Exception *ne = new Exception(warn, get_pgm_stmt(), desc);
   warnSink->raiseException(ne);
   traceout("QoreProgram::makeParseWarning()");
}

inline void QoreProgram::addParseWarning(int code, class ExceptionSink *xsink)
{
   if (!warnSink || !(code & warn_mask))
      return;
   warnSink->assimilate(xsink);
}

inline void QoreProgram::cannotProvideFeature(class QoreString *desc)
{
   tracein("QoreProgram::cannotProvideFeature()");
   
   if (!requires_exception)
   {
      parseSink->clear();
      requires_exception = true;
   }

   class Exception *ne = new Exception("CANNOT-PROVIDE-FEATURE", desc);
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
   hm_var_t::iterator i = vmap.find(var->getName());
   if (i == vmap.end())
      newVar(var, readonly);
   else
   {
      class Var *v = i->second;
      vmap.erase(i);
      v->makeReference(var, xsink, readonly);
      vmap[v->getName()] = v;
   }
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
   // need to grab the parse lock for safe access to the user function map
   plock.lock();
   bool b = (bool)(findUserFunction(name) != NULL);
   plock.unlock();
   return b;
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

inline void QoreProgram::parsePending(char *code, char *label, class ExceptionSink *xsink, class ExceptionSink *wS, int wm)
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

inline void QoreProgram::startThread()
{
   pthread_setspecific(thread_local_storage, new Hash());
}

inline class Hash *QoreProgram::getThreadData()
{
   return (class Hash *)pthread_getspecific(thread_local_storage);
}

inline class QoreNode *QoreProgram::run(class ExceptionSink *xsink)
{
   if (exec_class_name)
   {
      runClass(exec_class_name, xsink);
      return NULL;
   }
   return runTopLevel(xsink);
}

inline class Hash *QoreProgram::clearThreadData(class ExceptionSink *xsink)
{
   class Hash *h = (class Hash *)pthread_getspecific(thread_local_storage);
   printd(5, "QoreProgram::clearThreadData() this=%08p h=%08p (size=%d)\n", this, h, h->size());
   h->dereference(xsink);
   return h;
}

#endif // _QORE_QOREPROGRAM_H
