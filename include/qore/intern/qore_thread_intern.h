/*
  qore_thread_intern.h

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

#ifndef _QORE_QORE_THREAD_INTERN_H
#define _QORE_QORE_THREAD_INTERN_H

// FIXME: move to config.h or something like that
// not more than this number of threads can be running at the same time
#ifndef MAX_QORE_THREADS
#define MAX_QORE_THREADS 0x1000
#endif

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
DLLLOCAL bool inMethod(const char *name, const class QoreObject *o);
DLLLOCAL void pushProgram(class QoreProgram *pgm);
DLLLOCAL void popProgram();
DLLLOCAL class RootQoreNamespace *getRootNS();
DLLLOCAL int getParseOptions();
DLLLOCAL void updateCVarStack(class CVNode *ncvs);
DLLLOCAL class CVNode *getCVarStack();
DLLLOCAL void updateVStack(class VNode *nvs);
DLLLOCAL class VNode *getVStack();
DLLLOCAL class QoreObject *getStackObject();
DLLLOCAL void setParseClass(class QoreClass *c);
DLLLOCAL class QoreClass *getParseClass();
DLLLOCAL void substituteObjectIfEqual(class QoreObject *o);
DLLLOCAL class QoreObject *substituteObject(class QoreObject *o);
DLLLOCAL void catchSaveException(class QoreException *e);
DLLLOCAL class QoreException *catchGetException();
DLLLOCAL class VLock *getVLock();

#ifdef DEBUG
DLLLOCAL void pushCall(class CallNode *cn);
DLLLOCAL void popCall(class ExceptionSink *xsink);
DLLLOCAL class CallStack *getCallStack();
DLLLOCAL class QoreListNode *getCallStackList();
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

DLLLOCAL class LocalVarValue *thread_instantiate_lvar();
DLLLOCAL void thread_uninstantiate_lvar(ExceptionSink *xsink);

#ifndef HAVE_UNLIMITED_THREAD_KEYS
DLLLOCAL class LocalVarValue *thread_find_lvar(const char *id);
DLLLOCAL class LocalVarValue *thread_find_current_lvar(const char *id);
#endif

DLLLOCAL extern pthread_attr_t ta_default;

// for object implementation
DLLLOCAL class QoreObject *getStackObject();
// for methods that behave differently when called within the method itself
DLLLOCAL bool inMethod(const char *name, const QoreObject *o);

class CodeContextHelper {
   private:
      const char *old_code;
      class QoreObject *old_obj;
      class ExceptionSink *xsink;
	 
   public:
      DLLLOCAL CodeContextHelper(const char *code = NULL, const QoreObject *obj = NULL, ExceptionSink *xs = NULL);
      DLLLOCAL ~CodeContextHelper();
};

class ObjectSubstitutionHelper {
   private:
      class QoreObject *old_obj;
   
   public:
      DLLLOCAL ObjectSubstitutionHelper(class QoreObject *obj);
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
      LocalVar *old_argvid;
   
   public:
      DLLLOCAL ArgvContextHelper(LocalVar *argvid);
      DLLLOCAL ~ArgvContextHelper();
};

#include <qore/intern/CallStack.h>

#ifdef DEBUG
class CallStackHelper : public CallNode {
      ExceptionSink *xsink;

      // not implemented
      DLLLOCAL CallStackHelper(const CallStackHelper&);
      DLLLOCAL CallStackHelper& operator=(const CallStackHelper&);
      DLLLOCAL void *operator new(size_t);
      
   public:
      DLLLOCAL CallStackHelper(const char *f, int t, QoreObject *o, ExceptionSink *n_xsink) : CallNode(f, t, o), xsink(n_xsink)
      {
	 pushCall(this);
      }
      DLLLOCAL ~CallStackHelper()
      {
	 popCall(xsink);
      }
};
#endif

DLLLOCAL void init_qore_threads();
DLLLOCAL class QoreNamespace *get_thread_ns();
DLLLOCAL void delete_qore_threads();
DLLLOCAL class QoreListNode *get_thread_list();
DLLLOCAL class QoreHashNode *getAllCallStacks();

#endif
