/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreSignal.h

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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

#ifndef _QORE_QORESIGNAL_H

#define _QORE_QORESIGNAL_H

#include <qore/QoreThreadLock.h>
#include <qore/QoreCondition.h>
#include <qore/QoreCounter.h>

#include <signal.h>

#include <map>
#include <string>

// maximum number of signals
#ifndef QORE_SIGNAL_MAX
#ifdef NSIG
#define QORE_SIGNAL_MAX (NSIG+1)
#elif defined _NSIG
#define QORE_SIGNAL_MAX _NSIG
#elif defined _NSIGS
#define QORE_SIGNAL_MAX _NSIGS
#elif defined __DARWIN_NSIG
#define QORE_SIGNAL_MAX (__DARWIN_NSIG+1)
#else
#error do not know maximum signal number on this platform
#endif
#endif

// use SIGCHLD for the status signal
#define QORE_STATUS_SIGNAL SIGSYS //SIGCHLD

class QoreSignalHandler {
private:
   ResolvedCallReferenceNode *funcref;
   
public:
   enum sh_status_e { SH_OK = 0, SH_InProgress = 1, SH_Delete = 2 };
   sh_status_e status;

   DLLLOCAL void init();
   DLLLOCAL void set(int sig, const ResolvedCallReferenceNode *n_funcref);
   DLLLOCAL void del(int sig, ExceptionSink *xsink);
   DLLLOCAL void runHandler(int sig, ExceptionSink *xsink);
   DLLLOCAL bool isSet() const {
      return (bool)funcref;
   }
   DLLLOCAL QoreProgram *getProgram() const {
      return funcref->getProgram();
   }
};

// map of signals to module names
typedef std::map<int, std::string> sig_map_t;

class QoreSignalManager {
   friend class QoreSignalManagerBusyHelper;

private:
   bool is_enabled;      // signal handling enabled?
   pthread_t ptid;       // handler thread
   int tid;              // handler thread TID
   QoreCounter tcount;   // thread counter, for synchronization only
   QoreCondition cond;   // to ensure atomicity of set and remove calls
   bool block;
   int waiting;
      
   DLLLOCAL void reload();
   DLLLOCAL void stop_signal_thread_unlocked();
   DLLLOCAL int start_signal_thread(ExceptionSink *xsink);
   DLLLOCAL void stop_signal_thread();
      
public:
   enum sig_cmd_e { C_None = 0, C_Reload = 1, C_Exit = 2 };

   // set of signals we are managing
   sigset_t mask;

   // set of signals we do not manage (empty at start)
   sig_map_t fmap;

   int num_handlers;
   bool thread_running;
   QoreSignalHandler handlers[QORE_SIGNAL_MAX];
   QoreThreadLock mutex;
   sig_cmd_e cmd;
      
   DLLLOCAL QoreSignalManager();
   DLLLOCAL void init(bool disable_signal_mask = false);
   DLLLOCAL void del();
   DLLLOCAL int setHandler(int sig, const ResolvedCallReferenceNode *fr, ExceptionSink *xsink);
   DLLLOCAL int removeHandler(int sig, ExceptionSink *xsink);
   DLLLOCAL const char *getSignalName(int sig);
   DLLLOCAL void signal_handler_thread();
   DLLLOCAL void lock_idle();
   DLLLOCAL void release_idle();
   DLLLOCAL void start_handler();
   DLLLOCAL void end_handler();
   DLLLOCAL void pre_fork_block_and_stop();
   DLLLOCAL void post_fork_unblock_and_start(bool new_process, ExceptionSink *xsink);
   DLLLOCAL int gettid() {
      return tid;
   }
   DLLLOCAL void reset_default_signal_mask();
   DLLLOCAL bool running() { return tid != -1; }
   DLLLOCAL bool enabled() { return is_enabled; }

   // try to allow the signal to be managed externally (by a module)
   // sig = signal number, name = name of module to manage signal
   // returns 0 for OK, or an error string on error
   DLLLOCAL QoreStringNode *reassign_signal(int sig, const char *name);
};

DLLLOCAL extern QoreSignalManager QSM;

#endif
