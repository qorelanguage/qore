/*
  QoreSignal.h

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

#ifndef _QORE_QORESIGNAL_H

#define _QORE_QORESIGNAL_H

#include <qore/LockedObject.h>
#include <qore/QoreCondition.h>
#include <qore/QoreCounter.h>

#include <signal.h>

#include <set>

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
      class AbstractFunctionReference *funcref;
   
   public:
      enum sh_status_e { SH_OK = 0, SH_InProgress = 1, SH_Delete = 2 };
      sh_status_e status;

      DLLLOCAL void init();
      DLLLOCAL void set(int sig, class AbstractFunctionReference *n_funcref);
      DLLLOCAL void del(int sig, class ExceptionSink *xsink);
      void runHandler(int sig, class ExceptionSink *xsink);
      bool isSet() const
      {
	 return (bool)funcref;
      }
      class QoreProgram *getProgram() const
      {
	 return funcref->getProgram();
      }
};

typedef std::set<QoreProgram *> pgm_set_t;

class QoreSignalManager
{
   friend class QoreSignalManagerBusyHelper;

   private:
      DLLLOCAL static pthread_t ptid;       // handler thread
      DLLLOCAL static int tid;              // handler thread TID
      DLLLOCAL static pgm_set_t pgm_set;    // set of program objects used to manage thread-local storage
      DLLLOCAL static QoreCounter tcount;   // thread counter, for synchronization only
      DLLLOCAL static QoreCondition cond;   // to ensure atomicity of set and remove calls
      DLLLOCAL static bool busy;            // busy flag
      DLLLOCAL static int waiting;          // waiting count
      DLLLOCAL static QoreCondition bcond;  // busy condition
      
      DLLLOCAL static int start_signal_thread(class ExceptionSink *xsink);
      DLLLOCAL static void reload();
      DLLLOCAL static void kill();
      DLLLOCAL static void check_busy();
      DLLLOCAL static void done();
      
   public:
      enum sig_cmd_e { C_None = 0, C_Reload = 1, C_Exit = 2 };

      static sigset_t mask;
      static int num_handlers;
      static bool thread_running;
      static QoreSignalHandler handlers[QORE_SIGNAL_MAX];
      static class LockedObject mutex;
      static sig_cmd_e cmd;
      
      DLLLOCAL QoreSignalManager();
      DLLLOCAL static void init();
      DLLLOCAL static void del();
      DLLLOCAL static int setHandler(int sig, class AbstractFunctionReference *fr, class ExceptionSink *xsink);
      DLLLOCAL static int removeHandler(int sig, class ExceptionSink *xsink);
      //DLLLOCAL static void handleSignals();
      DLLLOCAL static void addSignalConstants(class Namespace *ns);
      DLLLOCAL static const char *getSignalName(int sig);
      DLLLOCAL static void signal_handler_thread();
};

class QoreSignalManagerBusyHelper {
   private:
      // not implemented
      DLLLOCAL QoreSignalManagerBusyHelper(const QoreSignalManagerBusyHelper&);
      DLLLOCAL QoreSignalManagerBusyHelper& operator=(const QoreSignalManagerBusyHelper&);
      DLLLOCAL void *operator new(size_t);
   public:
      DLLLOCAL QoreSignalManagerBusyHelper()
      {
	 QoreSignalManager::check_busy();
      }
      DLLLOCAL ~QoreSignalManagerBusyHelper()
      {
	 QoreSignalManager::done();
      }
};

DLLLOCAL extern class QoreSignalManager QSM;

#endif
