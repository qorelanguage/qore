/*
  QoreSignal.cc

  Qore programming language

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

#include <qore/Qore.h>
#include <qore/intern/QoreSignal.h>

#include <stdlib.h>

QoreSignalManager QSM;

bool QoreSignalManager::is_enabled = false;
int QoreSignalManager::num_handlers = 0;
QoreThreadLock QoreSignalManager::mutex;
sigset_t QoreSignalManager::mask;
bool QoreSignalManager::thread_running = false;
QoreCondition QoreSignalManager::cond;
QoreSignalManager::sig_cmd_e QoreSignalManager::cmd = QoreSignalManager::C_None;
pthread_t QoreSignalManager::ptid;
int QoreSignalManager::tid = -1;
QoreCounter QoreSignalManager::tcount;
int QoreSignalManager::waiting = 0;
bool QoreSignalManager::block = false;

QoreSignalHandler QoreSignalManager::handlers[QORE_SIGNAL_MAX];

// must be called in the signal lock
void QoreSignalHandler::set(int sig, const ResolvedCallReferenceNode *n_funcref) {
   funcref = const_cast<ResolvedCallReferenceNode *>(n_funcref);
   funcref->ref();
}

void QoreSignalHandler::init() {
   funcref = 0;
   status = SH_OK;
}

// must be called in the signal lock
void QoreSignalHandler::del(int sig, ExceptionSink *xsink) {
   if (funcref)
      funcref->deref(xsink);
   init();
}

void QoreSignalHandler::runHandler(int sig, ExceptionSink *xsink) {
   // create signal number argument
   ReferenceHolder<QoreListNode> args(new QoreListNode(), xsink);
   args->push(new QoreBigIntNode(sig));
   discard(funcref->exec(*args, xsink), xsink);
}

QoreSignalManager::QoreSignalManager() {
   // set command to none
   cmd = C_None;
   
   // initilize handlers
   for (int i = 0; i < QORE_SIGNAL_MAX; ++i)
      handlers[i].init();

   tid = -1;
}

void QoreSignalManager::init(bool disable_signal_mask) {
   // set SIGPIPE to ignore
   struct sigaction sa;
   sa.sa_handler = SIG_IGN;
   sigemptyset (&sa.sa_mask);
   sa.sa_flags = SA_RESTART;
   // ignore SIGPIPE signals
   sigaction(SIGPIPE, &sa, 0);

   if (!disable_signal_mask) {
      is_enabled = true;

      // block all signals
      sigfillset(&mask);
#ifdef PROFILE
      // do not block SIGPROF if profiling is enabled
      sigdelset(&mask, SIGPROF);
#endif

      pthread_sigmask(SIG_SETMASK, &mask, 0);

      // set up default handler mask
      sigemptyset(&mask);
      sigaddset(&mask, QORE_STATUS_SIGNAL);

      ExceptionSink xsink;
      if (start_signal_thread(&xsink)) {
	 xsink.handleExceptions();
	 exit(1);
      }
   }
}

void QoreSignalManager::del() {
   if (!running())
      return;
   stop_signal_thread();
}

// must only be called inside the lock
void QoreSignalManager::reload() {
   cmd = C_Reload;
   if (thread_running && tid != ::gettid()) {
#ifdef DEBUG
      int rc = 
#endif
	 pthread_kill(ptid, QORE_STATUS_SIGNAL);
      assert(!rc);
      // unlock lock and wait for condition
      cond.wait(&mutex);
   }
}

void QoreSignalManager::stop_signal_thread_unlocked() {
   QORE_TRACE("QoreSignalManager::stop_signal_thread_unlocked()");

   printd(5, "QoreSignalManager::stop_signal_thread_unlocked() pid=%d, thread_running=%d\n", getpid(), thread_running);

   cmd = C_Exit;
   if (thread_running) {
#ifdef DEBUG
      int rc = 
#endif
	 pthread_kill(ptid, QORE_STATUS_SIGNAL);
      assert(!rc);
   }
}

void QoreSignalManager::stop_signal_thread() {
   QORE_TRACE("QoreSignalManager::stop_signal_thread()");

   SafeLocker sl(&mutex);
   stop_signal_thread_unlocked();

   // wait for thread to exit (may be already gone)
   sl.unlock();
   tcount.waitForZero();   
}

void QoreSignalManager::pre_fork_block_and_stop() {
   SafeLocker sl(&mutex);
   if (!running())
      return;

   // if another block is already in progress then wait for it to complete
   while (block) {
      ++waiting;
      cond.wait(&mutex);
      --waiting;
   }	 
   block = true;
   stop_signal_thread_unlocked();

   // wait for thread to exit (may be already gone)
   sl.unlock();
   tcount.waitForZero();
   
   printd(5, "QoreSignalManager::pre_fork_block_and_stop() pid=%d signal handling thread stopped\n", getpid());
}

void QoreSignalManager::post_fork_unblock_and_start(bool new_process, ExceptionSink *xsink) {
   AutoLocker al(&mutex);
   if (!enabled())
      return;

   block = false;
   if (waiting)
      cond.signal();

   // set new default signal mask for new process
   if (new_process) {
      // block all signals
      sigset_t new_mask;
      sigfillset(&new_mask);
      pthread_sigmask(SIG_SETMASK, &new_mask, 0);
   }

   printd(5, "QoreSignalManager::post_fork_unblock_and_start() pid=%d, new_process=%d, starting signal thread\n", getpid(), new_process);
   start_signal_thread(xsink);
}

void QoreSignalManager::signal_handler_thread() {
   register_thread(tid, ptid, 0);
   
   printd(5, "QoreSignalManager::signal_handler_thread() pid=%d, signal handler thread started (TID %d)\n", getpid(), tid);

   sigset_t c_mask;
   int sig;
   {
      ExceptionSink xsink;

      // acquire lock to copy signal mask
      SafeLocker sl(&mutex);

      // reload copy of the signal mask for the sigwait command
      memcpy(&c_mask, &mask, sizeof(sigset_t));
      // block only signals we are catching in this thread
      pthread_sigmask(SIG_SETMASK, &c_mask, 0);

#ifdef DARWIN
      /* why do we call sigprocmask on Darwin?
	 it seems that Darwin has a bug in handling per-thread signal masks.  
	 Even though we explicitly set this thread's signal mask to unblock all signals
	 we are not explicitly catching (including QORE_STATUS_SIGNAL, currently set to
	 SIGSYS), no signal is delivered that is not in our list.  For example (on
	 Darwin only), if we are catching SIGUSR1 with a Qore signal handler (therefore
	 it's included in c_mask and blocked in this thread, because it will be also
	 included in sigwait below) and have no handler for SIGUSR2, if we send a 
	 SIGUSR2 to the process, unless we cann sigprocmask here and below after 
	 pthread_sigmask, the SIGUSR2 will also be blocked (even through the signal 
	 thread's signal mask explicitly allows for it to be delivered to this thread),
	 instead of being delivered to the process and triggering the default action - 
	 terminate the process.  The workaround (discovered with trial and error) is to
	 call sigprocmask after every call to pthread_sigmask in the signal handler 
	 thread  */
      sigprocmask(SIG_SETMASK, &c_mask, 0);
#endif

      while (true) {
	 if (cmd != C_None) {
	    sig_cmd_e c = cmd;
	    cmd = C_None;
	    // check command
	    if (c == C_Exit)
	       break;
	    if (c == C_Reload) {
	       memcpy(&c_mask, &mask, sizeof(sigset_t));
	       // block only signals we are catching in this thread
	       pthread_sigmask(SIG_SETMASK, &c_mask, 0);
#ifdef DARWIN
	       // see above for reasoning behind calling sigprocmask on Darwin
	       sigprocmask(SIG_SETMASK, &c_mask, 0);
#endif
	       // confirm that the mask has been updated so updates are atomic
	       cond.signal();
	    }
	 }
	 
	 // unlock to call sigwait
	 sl.unlock();
	 
	 //printd(5, "about to call sigwait()\n");
	 sigwait(&c_mask, &sig);
	 
	 // reacquire lock to check command and handler status
	 sl.lock();
	 
	 //printd(5, "sigwait() sig=%d (cmd=%d)\n", sig, cmd);
	 if (sig == QORE_STATUS_SIGNAL && cmd != C_None)
	    continue;
	 
	 //printd(5, "signal %d received (handler=%d)\n", sig, handlers[sig].isSet());
	 if (!handlers[sig].isSet())
	    continue;
	 
	 // set in progress status while in the lock
	 assert(handlers[sig].status == QoreSignalHandler::SH_OK);
	 handlers[sig].status = QoreSignalHandler::SH_InProgress;
	 
	 // unlock to run handler code
	 sl.unlock();
	 
	 // create thread-local storage if possible
	 // FIXME: set thread-local storage
	 QoreProgram *pgm = handlers[sig].getProgram();
	 if (pgm)
	    pgm->startThread();
	 
	 handlers[sig].runHandler(sig, &xsink);
	 
	 // delete thread-local storage, if any
	 if (pgm)
	    pgm->endThread(&xsink);
	 
	 // cleanup thread resources
	 purge_thread_resources(&xsink);
	 
	 // consume exceptions and reset exception sink
	 xsink.handleExceptions();
	 
	 // reacquire lock to check handler status
	 sl.lock();
	 
	 if (handlers[sig].status == QoreSignalHandler::SH_InProgress)
	    handlers[sig].status = QoreSignalHandler::SH_OK;
	 else {
#ifdef DEBUG
	    if (handlers[sig].status != QoreSignalHandler::SH_Delete)
	       printd(0, "error: status=%d (sig=%d)\n", handlers[sig].status, sig);
#endif
	    assert(handlers[sig].status == QoreSignalHandler::SH_Delete);
	    handlers[sig].del(sig, &xsink);
	 }
      }
      
      thread_running = false;
      tid = -1;
      sl.unlock();
      
      printd(5, "QoreSignalManager::signal_handler_thread() pid=%d signal handler thread terminating\n", getpid());
      
      // delete internal thread data structure
      delete_thread_data();
      
      // deregister_thread
      deregister_signal_thread();
      
      // run thread cleanup handlers
      tclist.exec();
      
      tcount.dec();
      //printf("signal handler thread %d stopped (count=%d)\n", c_tid, tcount.getCount());fflush(stdout);
      pthread_exit(0);
   }
}

extern "C" void *sig_thread(void *x) {
   QoreSignalManager::signal_handler_thread();
   return 0;
}

int QoreSignalManager::start_signal_thread(ExceptionSink *xsink) {
   printd(5, "QoreSignalManager::start_signal_thread() pid=%d, start_signal_thread() called\n", getpid());
   tid = get_signal_thread_entry();

   // if can't start thread, then throw exception
   if (tid == -1) {
      xsink->raiseException("THREAD-CREATION-FAILURE", "thread list is full with %d threads", MAX_QORE_THREADS);
      return -1;
   }
   printd(5, "QoreSignalManager::start_signal_thread() pid=%d, got TID %d\n", getpid(), tid);
   
   thread_running = true;
   tcount.inc();
   assert(tcount.getCount() == 1);

   int rc = pthread_create(&ptid, ta_default.get_ptr(), sig_thread, 0);
   if (rc) {
      tcount.dec();
      deregister_signal_thread();
      tid = -1;
      xsink->raiseException("THREAD-CREATION-FAILURE", "could not create thread: %s", strerror(rc));
      thread_running = false;
   }

   printd(5, "QoreSignalManager::start_signal_thread() pid=%d, rc=%d\n", getpid(), rc);
   return rc;
}

int QoreSignalManager::setHandler(int sig, const ResolvedCallReferenceNode *fr, ExceptionSink *xsink) {
   AutoLocker al(&mutex);
   if (!enabled())
      return 0;

   // wait for any blocks to be lifted
   while (block) {
      ++waiting;
      cond.wait(&mutex);
      --waiting;
   }	 

   bool already_set = true;
   if (!handlers[sig].isSet()) {
      already_set = false;
      // start signal thread for first handler
      if (!thread_running && start_signal_thread(xsink))
	 return -1;
      
      ++num_handlers;
   }

   //printd(5, "setting handler for signal %d, pgm=%08p\n", sig, pgm);
   handlers[sig].set(sig, fr);

   // add to the signal mask for signal thread if not already there
   if (!already_set && sig != QORE_STATUS_SIGNAL) {
      sigaddset(&mask, sig);
      reload();
   }

   return 0;
}

int QoreSignalManager::removeHandler(int sig, ExceptionSink *xsink) {
   AutoLocker al(&mutex);
   if (!enabled())
      return 0;

   // wait for any blocks to be lifted
   while (block) {
      ++waiting;
      cond.wait(&mutex);
      --waiting;
   }	 
   
   if (!handlers[sig].isSet())
      return 0;

   // remove from the signal mask for sigwait
   if (sig != QORE_STATUS_SIGNAL) {
      sigdelset(&mask, sig);
      reload();
   }
   
   //printd(5, "removing handler for signal %d\n", sig);

   // ensure handler is not in progress, if so mark for deletion
   if (handlers[sig].status == QoreSignalHandler::SH_InProgress)
      handlers[sig].status = QoreSignalHandler::SH_Delete;
   else // must be called in the signal lock
      handlers[sig].del(sig, xsink);
   --num_handlers;
   
   return 0;
}

void QoreSignalManager::addSignalConstants(class QoreNamespace *ns) {
   QoreHashNode *nh = new QoreHashNode();
   QoreHashNode *sh = new QoreHashNode();
#ifdef SIGHUP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGHUP), new QoreStringNode("SIGHUP"), 0);
   sh->setKeyValue("SIGHUP", new QoreBigIntNode(SIGHUP), 0);
   ns->addConstant("SIGHUP", new QoreBigIntNode(SIGHUP));
#endif
#ifdef SIGINT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGINT), new QoreStringNode("SIGINT"), 0);
   sh->setKeyValue("SIGINT", new QoreBigIntNode(SIGINT), 0);
   ns->addConstant("SIGINT", new QoreBigIntNode(SIGINT));
#endif
#ifdef SIGQUIT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGQUIT), new QoreStringNode("SIGQUIT"), 0);
   sh->setKeyValue("SIGQUIT", new QoreBigIntNode(SIGQUIT), 0);
   ns->addConstant("SIGQUIT", new QoreBigIntNode(SIGQUIT));
#endif
#ifdef SIGILL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGILL), new QoreStringNode("SIGILL"), 0);
   sh->setKeyValue("SIGILL", new QoreBigIntNode(SIGILL), 0);
   ns->addConstant("SIGILL", new QoreBigIntNode(SIGILL));
#endif
#ifdef SIGTRAP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTRAP), new QoreStringNode("SIGTRAP"), 0);
   sh->setKeyValue("SIGTRAP", new QoreBigIntNode(SIGTRAP), 0);
   ns->addConstant("SIGTRAP", new QoreBigIntNode(SIGTRAP));
#endif
#ifdef SIGABRT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGABRT), new QoreStringNode("SIGABRT"), 0);
   sh->setKeyValue("SIGABRT", new QoreBigIntNode(SIGABRT), 0);
   ns->addConstant("SIGABRT", new QoreBigIntNode(SIGABRT));
#endif
#ifdef SIGPOLL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPOLL), new QoreStringNode("SIGPOLL"), 0);
   sh->setKeyValue("SIGPOLL", new QoreBigIntNode(SIGPOLL), 0);
   ns->addConstant("SIGPOLL", new QoreBigIntNode(SIGPOLL));
#endif
#ifdef SIGIOT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGIOT), new QoreStringNode("SIGIOT"), 0);
   sh->setKeyValue("SIGIOT", new QoreBigIntNode(SIGIOT), 0);
   ns->addConstant("SIGIOT", new QoreBigIntNode(SIGIOT));
#endif
#ifdef SIGEMT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGEMT), new QoreStringNode("SIGEMT"), 0);
   sh->setKeyValue("SIGEMT", new QoreBigIntNode(SIGEMT), 0);
   ns->addConstant("SIGEMT", new QoreBigIntNode(SIGEMT));
#endif
#ifdef SIGFPE
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGFPE), new QoreStringNode("SIGFPE"), 0);
   sh->setKeyValue("SIGFPE", new QoreBigIntNode(SIGFPE), 0);
   ns->addConstant("SIGFPE", new QoreBigIntNode(SIGFPE));
#endif
#ifdef SIGKILL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGKILL), new QoreStringNode("SIGKILL"), 0);
   sh->setKeyValue("SIGKILL", new QoreBigIntNode(SIGKILL), 0);
   ns->addConstant("SIGKILL", new QoreBigIntNode(SIGKILL));
#endif
#ifdef SIGBUS
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGBUS), new QoreStringNode("SIGBUS"), 0);
   sh->setKeyValue("SIGBUS", new QoreBigIntNode(SIGBUS), 0);
   ns->addConstant("SIGBUS", new QoreBigIntNode(SIGBUS));
#endif
#ifdef SIGSEGV
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSEGV), new QoreStringNode("SIGSEGV"), 0);
   sh->setKeyValue("SIGSEGV", new QoreBigIntNode(SIGSEGV), 0);
   ns->addConstant("SIGSEGV", new QoreBigIntNode(SIGSEGV));
#endif
#ifdef SIGSYS
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSYS), new QoreStringNode("SIGSYS"), 0);
   sh->setKeyValue("SIGSYS", new QoreBigIntNode(SIGSYS), 0);
   ns->addConstant("SIGSYS", new QoreBigIntNode(SIGSYS));
#endif
#ifdef SIGPIPE
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPIPE), new QoreStringNode("SIGPIPE"), 0);
   sh->setKeyValue("SIGPIPE", new QoreBigIntNode(SIGPIPE), 0);
   ns->addConstant("SIGPIPE", new QoreBigIntNode(SIGPIPE));
#endif
#ifdef SIGALRM
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGALRM), new QoreStringNode("SIGALRM"), 0);
   sh->setKeyValue("SIGALRM", new QoreBigIntNode(SIGALRM), 0);
   ns->addConstant("SIGALRM", new QoreBigIntNode(SIGALRM));
#endif
#ifdef SIGTERM
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTERM), new QoreStringNode("SIGTERM"), 0);
   sh->setKeyValue("SIGTERM", new QoreBigIntNode(SIGTERM), 0);
   ns->addConstant("SIGTERM", new QoreBigIntNode(SIGTERM));
#endif
#ifdef SIGURG
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGURG), new QoreStringNode("SIGURG"), 0);
   sh->setKeyValue("SIGURG", new QoreBigIntNode(SIGURG), 0);
   ns->addConstant("SIGURG", new QoreBigIntNode(SIGURG));
#endif
#ifdef SIGSTOP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSTOP), new QoreStringNode("SIGSTOP"), 0);
   sh->setKeyValue("SIGSTOP", new QoreBigIntNode(SIGSTOP), 0);
   ns->addConstant("SIGSTOP", new QoreBigIntNode(SIGSTOP));
#endif
#ifdef SIGTSTP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTSTP), new QoreStringNode("SIGTSTP"), 0);
   sh->setKeyValue("SIGTSTP", new QoreBigIntNode(SIGTSTP), 0);
   ns->addConstant("SIGTSTP", new QoreBigIntNode(SIGTSTP));
#endif
#ifdef SIGCONT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCONT), new QoreStringNode("SIGCONT"), 0);
   sh->setKeyValue("SIGCONT", new QoreBigIntNode(SIGCONT), 0);
   ns->addConstant("SIGCONT", new QoreBigIntNode(SIGCONT));
#endif
#ifdef SIGCHLD
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCHLD), new QoreStringNode("SIGCHLD"), 0);
   sh->setKeyValue("SIGCHLD", new QoreBigIntNode(SIGCHLD), 0);
   ns->addConstant("SIGCHLD", new QoreBigIntNode(SIGCHLD));
#endif
#ifdef SIGTTIN
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTTIN), new QoreStringNode("SIGTTIN"), 0);
   sh->setKeyValue("SIGTTIN", new QoreBigIntNode(SIGTTIN), 0);
   ns->addConstant("SIGTTIN", new QoreBigIntNode(SIGTTIN));
#endif
#ifdef SIGTTOU
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTTOU), new QoreStringNode("SIGTTOU"), 0);
   sh->setKeyValue("SIGTTOU", new QoreBigIntNode(SIGTTOU), 0);
   ns->addConstant("SIGTTOU", new QoreBigIntNode(SIGTTOU));
#endif
#ifdef SIGIO
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGIO), new QoreStringNode("SIGIO"), 0);
   sh->setKeyValue("SIGIO", new QoreBigIntNode(SIGIO), 0);
   ns->addConstant("SIGIO", new QoreBigIntNode(SIGIO));
#endif
#ifdef SIGXCPU
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGXCPU), new QoreStringNode("SIGXCPU"), 0);
   sh->setKeyValue("SIGXCPU", new QoreBigIntNode(SIGXCPU), 0);
   ns->addConstant("SIGXCPU", new QoreBigIntNode(SIGXCPU));
#endif
#ifdef SIGXFSZ
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGXFSZ), new QoreStringNode("SIGXFSZ"), 0);
   sh->setKeyValue("SIGXFSZ", new QoreBigIntNode(SIGXFSZ), 0);
   ns->addConstant("SIGXFSZ", new QoreBigIntNode(SIGXFSZ));
#endif
#ifdef SIGVTALRM
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGVTALRM), new QoreStringNode("SIGVTALRM"), 0);
   sh->setKeyValue("SIGVTALRM", new QoreBigIntNode(SIGVTALRM), 0);
   ns->addConstant("SIGVTALRM", new QoreBigIntNode(SIGVTALRM));
#endif
#ifdef SIGPROF
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPROF), new QoreStringNode("SIGPROF"), 0);
   sh->setKeyValue("SIGPROF", new QoreBigIntNode(SIGPROF), 0);
   ns->addConstant("SIGPROF", new QoreBigIntNode(SIGPROF));
#endif
#ifdef SIGWINCH
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGWINCH), new QoreStringNode("SIGWINCH"), 0);
   sh->setKeyValue("SIGWINCH", new QoreBigIntNode(SIGWINCH), 0);
   ns->addConstant("SIGWINCH", new QoreBigIntNode(SIGWINCH));
#endif
#ifdef SIGINFO
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGINFO), new QoreStringNode("SIGINFO"), 0);
   sh->setKeyValue("SIGINFO", new QoreBigIntNode(SIGINFO), 0);
   ns->addConstant("SIGINFO", new QoreBigIntNode(SIGINFO));
#endif
#ifdef SIGUSR1
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGUSR1), new QoreStringNode("SIGUSR1"), 0);
   sh->setKeyValue("SIGUSR1", new QoreBigIntNode(SIGUSR1), 0);
   ns->addConstant("SIGUSR1", new QoreBigIntNode(SIGUSR1));
#endif
#ifdef SIGUSR2
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGUSR2), new QoreStringNode("SIGUSR2"), 0);
   sh->setKeyValue("SIGUSR2", new QoreBigIntNode(SIGUSR2), 0);
   ns->addConstant("SIGUSR2", new QoreBigIntNode(SIGUSR2));
#endif
#ifdef SIGSTKFLT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSTKSZ), new QoreStringNode("SIGSTKFLT"), 0);
   sh->setKeyValue("SIGSTKFLT", new QoreBigIntNode(SIGSTKFLT), 0);
   ns->addConstant("SIGSTKFLT", new QoreBigIntNode(SIGSTKFLT));
#endif
#ifdef SIGCLD
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCHLD), new QoreStringNode("SIGCLD"), 0);
   sh->setKeyValue("SIGCLD", new QoreBigIntNode(SIGCLD), 0);
   ns->addConstant("SIGCLD", new QoreBigIntNode(SIGCLD));
#endif
#ifdef SIGPWR
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPWR), new QoreStringNode("SIGPWR"), 0);
   sh->setKeyValue("SIGPWR", new QoreBigIntNode(SIGPWR), 0);
   ns->addConstant("SIGPWR", new QoreBigIntNode(SIGPWR));
#endif
#ifdef SIGLOST
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGLOST), new QoreStringNode("SIGLOST"), 0);
   sh->setKeyValue("SIGLOST", new QoreBigIntNode(SIGLOST), 0);
   ns->addConstant("SIGLOST", new QoreBigIntNode(SIGLOST));
#endif
#ifdef SIGWAITING
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGWAITING), new QoreStringNode("SIGWAITING"), 0);
   sh->setKeyValue("SIGWAITING", new QoreBigIntNode(SIGWAITING), 0);
   ns->addConstant("SIGWAITING", new QoreBigIntNode(SIGWAITING));
#endif
#ifdef SIGLWP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGLWP), new QoreStringNode("SIGLWP"), 0);
   sh->setKeyValue("SIGLWP", new QoreBigIntNode(SIGLWP), 0);
   ns->addConstant("SIGLWP", new QoreBigIntNode(SIGLWP));
#endif
#ifdef SIGFREEZE
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGFREEZE), new QoreStringNode("SIGFREEZE"), 0);
   sh->setKeyValue("SIGFREEZE", new QoreBigIntNode(SIGFREEZE), 0);
   ns->addConstant("SIGFREEZE", new QoreBigIntNode(SIGFREEZE));
#endif
#ifdef SIGTHAW
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTHAW), new QoreStringNode("SIGTHAW"), 0);
   sh->setKeyValue("SIGTHAW", new QoreBigIntNode(SIGTHAW), 0);
   ns->addConstant("SIGTHAW", new QoreBigIntNode(SIGTHAW));
#endif
#ifdef SIGCANCEL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCANCEL), new QoreStringNode("SIGCANCEL"), 0);
   sh->setKeyValue("SIGCANCEL", new QoreBigIntNode(SIGCANCEL), 0);
   ns->addConstant("SIGCANCEL", new QoreBigIntNode(SIGCANCEL));
#endif
#ifdef SIGXRES
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGXRES), new QoreStringNode("SIGXRES"), 0);
   sh->setKeyValue("SIGXRES", new QoreBigIntNode(SIGXRES), 0);
   ns->addConstant("SIGXRES", new QoreBigIntNode(SIGXRES));
#endif
#ifdef SIGJVM1
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGJVM1), new QoreStringNode("SIGJVM1"), 0);
   sh->setKeyValue("SIGJVM1", new QoreBigIntNode(SIGJVM1), 0);
   ns->addConstant("SIGJVM1", new QoreBigIntNode(SIGJVM1));
#endif
#ifdef SIGJVM2
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGJVM2), new QoreStringNode("SIGJVM2"), 0);
   sh->setKeyValue("SIGJVM2", new QoreBigIntNode(SIGJVM2), 0);
   ns->addConstant("SIGJVM2", new QoreBigIntNode(SIGJVM2));
#endif
   
   ns->addConstant("SignalToName", nh);
   ns->addConstant("NameToSignal", sh);
}

