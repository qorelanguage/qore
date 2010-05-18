/*
  QoreSignal.cpp

  Qore programming language

  Copyright 2003 - 2010 David Nichols

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
   }

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
      xsink->raiseErrnoException("THREAD-CREATION-FAILURE", rc, "could not create thread");
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
