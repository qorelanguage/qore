/*
  QoreSignal.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

// maximum number of signals
#ifndef QORE_SIGNAL_MAX
#ifdef NSIG
#define QORE_SIGNAL_MAX (NSIG+1)
#elif defined _NSIG
#define QORE_SIGNAL_MAX _NSIG
#elif defined _NSIGS
#define QORE_SIGNAL_MAX _NSIGS
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

class QoreSignalManager {
   friend class QoreSignalManagerBusyHelper;

   private:
      DLLLOCAL static bool is_enabled;      // signal handling enabled?
      DLLLOCAL static pthread_t ptid;       // handler thread
      DLLLOCAL static int tid;              // handler thread TID
      DLLLOCAL static QoreCounter tcount;   // thread counter, for synchronization only
      DLLLOCAL static QoreCondition cond;   // to ensure atomicity of set and remove calls
      DLLLOCAL static bool block;
      DLLLOCAL static int waiting;
      
      DLLLOCAL static void reload();
      DLLLOCAL static void stop_signal_thread_unlocked();
      DLLLOCAL static int start_signal_thread(ExceptionSink *xsink);
      DLLLOCAL static void stop_signal_thread();
      
   public:
      enum sig_cmd_e { C_None = 0, C_Reload = 1, C_Exit = 2 };

      static sigset_t mask;
      static int num_handlers;
      static bool thread_running;
      static QoreSignalHandler handlers[QORE_SIGNAL_MAX];
      static QoreThreadLock mutex;
      static sig_cmd_e cmd;
      
      DLLLOCAL QoreSignalManager();
      DLLLOCAL static void init(bool disable_signal_mask = false);
      DLLLOCAL static void del();
      DLLLOCAL static int setHandler(int sig, const ResolvedCallReferenceNode *fr, ExceptionSink *xsink);
      DLLLOCAL static int removeHandler(int sig, ExceptionSink *xsink);
      DLLLOCAL static void addSignalConstants(QoreNamespace *ns);
      DLLLOCAL static const char *getSignalName(int sig);
      DLLLOCAL static void signal_handler_thread();
      DLLLOCAL static void lock_idle();
      DLLLOCAL static void release_idle();
      DLLLOCAL static void start_handler();
      DLLLOCAL static void end_handler();
      DLLLOCAL static void pre_fork_block_and_stop();
      DLLLOCAL static void post_fork_unblock_and_start(bool new_process, ExceptionSink *xsink);
      DLLLOCAL static int gettid() {
	 return tid;
      }
      DLLLOCAL static void reset_default_signal_mask();
      DLLLOCAL static bool running() { return tid != -1; }
      DLLLOCAL static bool enabled() { return is_enabled; }
};

DLLLOCAL extern QoreSignalManager QSM;

#endif
