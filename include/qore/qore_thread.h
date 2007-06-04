/*
  qore_thread.h

  POSIX thread library for Qore

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

#ifndef _QORE_QORE_THREAD_H
#define _QORE_QORE_THREAD_H

#include <qore/LockedObject.h>

#include <stdio.h>
#include <pthread.h>

// FIXME: move to config.h or something like that
// not more than this number of threads can be running at the same time
#ifndef MAX_QORE_THREADS
#define MAX_QORE_THREADS 0x1000
#endif

#define CT_USER      0
#define CT_BUILTIN   1
#define CT_NEWTHREAD 2
#define CT_RETHROW   3

// pointer to a qore thread destructor function
typedef void (*qtdest_t)(void *);
// pointer to a qore thread resource destructor function
typedef void (*qtrdest_t)(void *, class ExceptionSink *);

DLLEXPORT extern class Operator *OP_BACKGROUND;

DLLEXPORT int gettid();
DLLEXPORT extern class ThreadCleanupList tclist;

// for thread resource handling
DLLEXPORT void set_thread_resource(class AbstractThreadResource *atr);
DLLEXPORT int remove_thread_resource(class AbstractThreadResource *atr);

DLLLOCAL void purge_thread_resources(class ExceptionSink *xsink);
DLLLOCAL void beginParsing(char *file, void *ps = NULL);
DLLLOCAL void *endParsing();
DLLLOCAL class Context *get_context_stack();
DLLLOCAL void update_context_stack(Context *cstack);
DLLLOCAL void get_pgm_counter(int &start_line, int &end_line);
DLLLOCAL const char *get_pgm_file();
DLLLOCAL void update_pgm_counter_pgm_file(int start_line, int end_line, const char *f);
DLLLOCAL void get_parse_location(int &start_line, int &end_line);
DLLLOCAL const char *get_parse_file();
DLLLOCAL void update_parse_location(int start_line, int end_line);
DLLLOCAL void update_parse_location(int start_line, int end_line, const char *f);
DLLLOCAL bool inMethod(const char *name, class Object *o);
DLLLOCAL void pushProgram(class QoreProgram *pgm);
DLLLOCAL void popProgram();
DLLLOCAL class QoreProgram *getProgram();
DLLLOCAL class RootNamespace *getRootNS();
DLLLOCAL int getParseOptions();
DLLLOCAL void updateCVarStack(class CVNode *ncvs);
DLLLOCAL class CVNode *getCVarStack();
DLLLOCAL void updateVStack(class VNode *nvs);
DLLLOCAL class VNode *getVStack();
DLLLOCAL class Object *getStackObject();
DLLLOCAL void setParseClass(class QoreClass *c);
DLLLOCAL class QoreClass *getParseClass();
DLLLOCAL void substituteObjectIfEqual(class Object *o);
DLLLOCAL class Object *substituteObject(class Object *o);
DLLLOCAL void catchSaveException(class Exception *e);
DLLLOCAL class Exception *catchGetException();
DLLLOCAL class VLock *getVLock();

DLLLOCAL class LVar *thread_instantiate_lvar();
DLLLOCAL void thread_uninstantiate_lvar(class ExceptionSink *xsink);
DLLLOCAL class LVar *thread_find_lvar(lvh_t id);

#ifdef DEBUG
DLLLOCAL void pushCall(const char *f, int type, class Object *o = NULL);
DLLLOCAL void popCall(class ExceptionSink *xsink);
DLLLOCAL class CallStack *getCallStack();
DLLLOCAL class List *getCallStackList();
#else
#ifdef __GNUC__
#define pushCall(args...)
#else
#define pushCall(args, ...)
#endif
#define popCall(x)
#endif

// acquires a TID and thread entry, returns -1 if not successful
DLLLOCAL int get_thread_entry();
// acquires TID 0 and sets up the signal thread entry, always returns 0
DLLLOCAL int get_signal_thread_entry();
DLLLOCAL void delete_thread_data();
DLLLOCAL void register_thread(int tid, pthread_t ptid, class QoreProgram *pgm);
DLLLOCAL void deregister_thread(int tid);
DLLLOCAL void deregister_signal_thread();

// called when a StatementBlock has "on block exit" blocks
DLLLOCAL void pushBlock(block_list_t::iterator i);
// called when a StatementBlock has "on block exit" blocks
DLLLOCAL block_list_t::iterator popBlock();
// called by each "on_block_exit" statement to activate it's code for the block exit
DLLLOCAL void advanceOnBlockExit();

DLLLOCAL extern pthread_attr_t ta_default;

// for object implementation
DLLLOCAL class Object *getStackObject();
// for methods that behave differently when called within the method itself
DLLLOCAL bool inMethod(const char *name, class Object *o);

class ThreadCleanupList {
   private:
      static class ThreadCleanupNode *head;

   public:
      DLLLOCAL ThreadCleanupList();
      DLLLOCAL ~ThreadCleanupList();
      DLLLOCAL void exec();

      DLLEXPORT void push(qtdest_t func, void *arg);
      DLLEXPORT void pop(int exec = 0);
};

class CodeContextHelper {
   private:
      const char *old_code;
      class Object *old_obj;
      class ExceptionSink *xsink;
	 
   public:
      DLLLOCAL CodeContextHelper(const char *code = NULL, class Object *obj = NULL, class ExceptionSink *xs = NULL);
      DLLLOCAL ~CodeContextHelper();
};

class ObjectSubstitutionHelper {
   private:
      class Object *old_obj;
   
   public:
      DLLLOCAL ObjectSubstitutionHelper(class Object *obj);
      DLLLOCAL ~ObjectSubstitutionHelper();
};

class ProgramContextHelper {
   private:
      class QoreProgram *old_pgm;
      bool restore;
   
   public:
      DLLLOCAL ProgramContextHelper(class QoreProgram *pgm);
      DLLLOCAL ~ProgramContextHelper();
};

class ArgvContextHelper {
   private:
      lvh_t old_argvid;
   
   public:
      DLLLOCAL ArgvContextHelper(lvh_t argvid);
      DLLLOCAL ~ArgvContextHelper();
};

DLLLOCAL void init_qore_threads();
DLLLOCAL class Namespace *get_thread_ns();
DLLLOCAL void delete_qore_threads();
DLLLOCAL class List *get_thread_list();
DLLLOCAL class Hash *getAllCallStacks();

#endif  // ifndef _QORE_THREAD_H
