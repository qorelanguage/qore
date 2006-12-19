/*
  qore_thread.h

  POSIX thread library for Qore

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

// pointer to a qore thread destructor function
typedef void (*qtdest_t)(void *);
typedef void (*qtrdest_t)(void *, class ExceptionSink *);

DLLEXPORT extern class Operator *OP_BACKGROUND;

DLLEXPORT int gettid();
DLLEXPORT extern class ThreadCleanupList tclist;
DLLEXPORT extern class ThreadResourceList trlist;

DLLLOCAL void beginParsing(char *file, void *ps = NULL);
DLLLOCAL void *endParsing();
DLLLOCAL class LVar *get_thread_stack();
DLLLOCAL void update_thread_stack(class LVar *lvstack);
DLLLOCAL class Context *get_context_stack();
DLLLOCAL void update_context_stack(Context *cstack);
DLLLOCAL class ArgvStack *get_argvstack();
DLLLOCAL void update_argvstack(class ArgvStack *as);
DLLLOCAL int get_pgm_counter();
DLLLOCAL int get_pgm_stmt();
DLLLOCAL char *get_pgm_file();
DLLLOCAL void update_pgm_stmt();
DLLLOCAL void update_pgm_counter_pgm_file(int p, char *f);
DLLLOCAL void increment_pgm_counter();
DLLLOCAL bool inMethod(char *name, class Object *o);
DLLLOCAL void pushCall(char *f, int type, class Object *o = NULL);
DLLLOCAL void popCall(class ExceptionSink *xsink = NULL);
DLLLOCAL class List *getCallStackList();
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
DLLLOCAL class CallStack *getCallStack();

class ThreadResourceList : public LockedObject {
   private:
      static class ThreadResourceNode *head;
   
      DLLLOCAL class ThreadResourceNode *find(void *key);
      DLLLOCAL void removeIntern(class ThreadResourceNode *w);
      DLLLOCAL void setIntern(class ThreadResourceNode *n);

   public:
      DLLLOCAL ThreadResourceList();
      DLLLOCAL ~ThreadResourceList();
   
      DLLEXPORT void set(void *key, qtrdest_t func);
      //returns 0 if not already set, 1 if already set
      DLLEXPORT int setOnce(void *key, qtrdest_t func);
      DLLEXPORT void remove(void *key);
      DLLEXPORT void purgeTID(int tid, class ExceptionSink *xsink);
};

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

DLLLOCAL void init_qore_threads();
DLLLOCAL class Namespace *get_thread_ns();
DLLLOCAL void delete_qore_threads();
DLLLOCAL class List *get_thread_list();
DLLLOCAL class Hash *getAllCallStacks();

#endif  // ifndef _QORE_THREAD_H
