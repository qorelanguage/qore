/*
  QoreSignal.cpp

  Qore programming language

  Copyright (C) 2003 - 2015 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include <qore/intern/QoreSignal.h>
#include <qore/intern/qore_program_private.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

QoreSignalManager QSM;

void QoreSignalHandler::init() {
   funcref = 0;
   status = SH_OK;
}

// must be called in the signal lock
void QoreSignalHandler::runHandler(int sig, ExceptionSink *xsink) {
   // create signal number argument
   ReferenceHolder<QoreListNode> args(new QoreListNode, xsink);
   args->push(new QoreBigIntNode(sig));
   funcref->execValue(*args, xsink).discard(xsink);
}

QoreSignalManager::QoreSignalManager() : is_enabled(false), tid(-1), block(false), waiting(0), num_handlers(0), thread_running(false), cmd(C_None) {
   //printd(5, "QoreSignalManager::QoreSignalManager() QORE_SIGNAL_MAX: %d\n", QORE_SIGNAL_MAX);
   // initilize handlers
   for (int i = 0; i < QORE_SIGNAL_MAX; ++i)
      handlers[i].init();
}

void QoreSignalManager::setMask(sigset_t& mask) {
   // block all signals
   sigfillset(&mask);
#ifdef PROFILE
   // do not block SIGPROF if profiling is enabled
   sigdelset(&mask, SIGPROF);
   if (!is_enabled)
      fmap[SIGPROF] = "QORE (system profiling)";
#endif
   // do not block SIGALRM or SIGCHLD on UNIX platforms (any platform that supports signals)
   sigdelset(&mask, SIGALRM);
   sigdelset(&mask, SIGCHLD);
   if (!is_enabled) {
      fmap[QORE_STATUS_SIGNAL] = "QORE (SIGSYS for internal use)";
      fmap[SIGALRM] = "QORE (SIGALRM for sleep()/usleep())";
      fmap[SIGCHLD] = "QORE (SIGCHLD for system())";
   }
}

void QoreSignalManager::init(bool disable_signal_mask) {
   // set SIGPIPE to ignore
   struct sigaction sa;
   sa.sa_handler = SIG_IGN;
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = SA_RESTART;
   // ignore SIGPIPE signals
   sigaction(SIGPIPE, &sa, 0);

   if (!disable_signal_mask) {
      setMask(mask);
      is_enabled = true;

      pthread_sigmask(SIG_SETMASK, &mask, 0);

      // set up default handler mask
      sigemptyset(&mask);
      sigaddset(&mask, QORE_STATUS_SIGNAL);

      ExceptionSink xsink;
      if (start_signal_thread(&xsink)) {
	 xsink.handleExceptions();
	 _Exit(1);
      }
   }
}

void QoreSignalManager::del() {
   //printd(5, "QoreSignalManager::del() start\n");
   SafeLocker sl(&mutex);
   if (!enabled())
      return;

   is_enabled = false;

   ExceptionSink xsink;
   bool changed = false;

   // remove all signal handlers to ensure that all Programs are dereferenced
   for (int i = 0; i < QORE_SIGNAL_MAX; ++i) {
      if (i != QORE_STATUS_SIGNAL || !handlers[i].isSet())
	 continue;
      sigdelset(&mask, i);
      changed = true;
   }
   if (changed) {
      reload();

      for (int i = 0; i < QORE_SIGNAL_MAX; ++i) {
	 if (!handlers[i].isSet())
	    continue;

	 if (handlers[i].status == QoreSignalHandler::SH_InProgress)
	    handlers[i].status = QoreSignalHandler::SH_Delete;
	 else {
	    // must be called in the signal lock
	    CodePgm old = handlers[i].take();
	    qore_program_private::delSignal(*old.pgm, i);
	    --num_handlers;
	    assert(num_handlers >= 0);
	    sl.unlock();
	    old.del(&xsink);
	    sl.lock();
	 }
      }
   }

   // can be called if it's already stopped...
   stop_signal_thread_unlocked();
   sl.unlock();
   tcount.waitForZero();
   assert(!num_handlers);
   //printd(5, "QoreSignalManager::del() all handlers deleted\n");
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
      cond.wait(mutex);
   }
}

void QoreSignalManager::stop_signal_thread_unlocked() {
   QORE_TRACE("QoreSignalManager::stop_signal_thread_unlocked()");

   printd(5, "QoreSignalManager::stop_signal_thread_unlocked() pid: %d, thread_running: %d\n", getpid(), thread_running);

   cmd = C_Exit;
   if (thread_running) {
#ifdef DEBUG
      int rc =
#endif
	 pthread_kill(ptid, QORE_STATUS_SIGNAL);
#ifdef DEBUG
      if (rc)
	 printd(0, "pthread_kill() returned %d: %s\n", rc, strerror(rc));
      assert(!rc);
#endif
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

void QoreSignalManager::preFork() {
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

   printd(5, "QoreSignalManager::preFork() pid: %d signal handling thread stopped\n", getpid());
}

void QoreSignalManager::postFork(bool new_process, ExceptionSink *xsink) {
   if (!enabled())
      return;

   block = false;
   assert(!waiting);

   // set new default signal mask for new process
   if (new_process) {
      // do not start signal thread after a fork(), pthread_create() is not async-signal safe
      is_enabled = false;
      // enable all signals
      sigset_t new_mask;
      sigemptyset(&new_mask);
      sigprocmask(SIG_SETMASK, &new_mask, 0);
      return;
   }

   printd(5, "QoreSignalManager::postFork() pid: %d, new_process: %d, starting signal thread\n", getpid(), new_process);

   AutoLocker al(mutex);
   start_signal_thread(xsink);
}

void QoreSignalManager::signal_handler_thread() {
   register_thread(tid, ptid, 0);

   printd(5, "QoreSignalManager::signal_handler_thread() pid: %d, signal handler thread started (TID %d) &ptid: %p\n", getpid(), tid, &ptid);

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
	 SIGUSR2 to the process, unless we call sigprocmask here and below after
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
	    //printd(5, "QoreSignalManager::signal_handler_thread() cmd: %d\n", cmd);
	    sig_cmd_e c = cmd;
	    cmd = C_None;
	    // check command
	    if (c == C_Exit)
	       break;
	    if (c == C_Reload) {
	       //printd(5, "QoreSignalManager::signal_handler_thread() C_Reload called:\n");
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

	 //printd(5, "sigwait() sig: %d (cmd: %d) set: %d\n", sig, cmd, handlers[sig].isSet());
	 if (sig == QORE_STATUS_SIGNAL && cmd != C_None)
	    continue;

	 //printd(5, "signal %d received (handler: %d)\n", sig, handlers[sig].isSet());
	 if (!handlers[sig].isSet())
	    continue;

	 // set in progress status while in the lock
	 assert(handlers[sig].status == QoreSignalHandler::SH_OK);
	 handlers[sig].status = QoreSignalHandler::SH_InProgress;

	 // unlock to run handler code
	 sl.unlock();

	 // create thread-local storage if possible
	 // FIXME: set thread-local storage
	 QoreProgram* pgm = handlers[sig].getProgram();
	 if (pgm)
	    qore_program_private::startThread(*pgm, xsink);

	 {
	    ProgramThreadCountContextHelper tch(&xsink, pgm, true);
	    if (!xsink)
	       handlers[sig].runHandler(sig, &xsink);
	 }

	 // delete thread-local storage, if any
	 if (pgm)
	    end_signal_thread(&xsink);

	 // cleanup thread resources
	 purge_thread_resources(&xsink);

	 // consume exceptions and reset exception sink
	 xsink.handleExceptions();

	 // reacquire lock to check handler status
	 sl.lock();

	 if (handlers[sig].status == QoreSignalHandler::SH_InProgress) {
	    handlers[sig].status = QoreSignalHandler::SH_OK;
	    continue;
	 }

#ifdef DEBUG
	 if (handlers[sig].status != QoreSignalHandler::SH_Delete)
	    printd(0, "error: status: %d (sig: %d)\n", handlers[sig].status, sig);
#endif
	 assert(handlers[sig].status == QoreSignalHandler::SH_Delete);
	 CodePgm old = handlers[sig].take();
         qore_program_private::delSignal(*old.pgm, sig);
	 --num_handlers;
	 assert(num_handlers >= 0);
	 sl.unlock();
	 old.del(&xsink);
	 sl.lock();
      }

      thread_running = false;
      tid = -1;
      sl.unlock();
   }

   printd(5, "QoreSignalManager::signal_handler_thread() pid: %d signal handler thread terminating\n", getpid());

   // delete internal thread data structure
   delete_signal_thread();

   // run thread cleanup handlers
   tclist.exec();

   tcount.dec();

   //fprintf(stderr, "signal handler thread stopped (count: %d) &ptid: %p\n", tcount.getCount(), &ptid);fflush(stderr);
   pthread_exit(0);
}

extern "C" void *sig_thread(void *x) {
   QSM.signal_handler_thread();
   return 0;
}

int QoreSignalManager::start_signal_thread(ExceptionSink *xsink) {
   printd(5, "QoreSignalManager::start_signal_thread() pid: %d, start_signal_thread() called\n", getpid());
   tid = get_signal_thread_entry();
   assert(!tid);

   thread_running = true;
   tcount.inc();
   assert(tcount.getCount() == 1);

   int rc = pthread_create(&ptid, ta_default.get_ptr(), sig_thread, 0);
   if (rc) {
      tcount.dec();
      deregister_signal_thread();
      tid = -1;
      xsink->raiseErrnoException("THREAD-CREATION-FAILURE", rc, "could not create signal handler thread");
      thread_running = false;
   }

   printd(5, "QoreSignalManager::start_signal_thread() pid: %d, rc: %d\n", getpid(), rc);
   return rc;
}

int QoreSignalManager::setHandler(int sig, const ResolvedCallReferenceNode *fr, ExceptionSink *xsink) {
   //printd(5, "QoreSignalManager::setHandler(sig: %d fr: %p) pgm: %p\n", sig, fr, getProgram());
   CodePgm old;

   {
      AutoLocker al(&mutex);
      if (!enabled())
         return 0;

      // wait for any blocks to be lifted
      while (block) {
         ++waiting;
         cond.wait(&mutex);
         --waiting;
      }

      sig_map_t::iterator i = fmap.find(sig);
      if (i != fmap.end()) {
         xsink->raiseException("SIGNAL-HANDLER-ERROR", "cannot install a handler for signal %d because management for the signal has been reassigned to module '%s'", sig, i->second.c_str());
         return -1;
      }

      bool already_set = true;

      QoreProgram* pgm = getProgram();
      if (!handlers[sig].isSet()) {
         already_set = false;
         // start signal thread for first handler
         if (!thread_running && start_signal_thread(xsink))
            return -1;

         //printd(5, "setting handler for signal %d, pgm: %p\n", sig, pgm);
         qore_program_private::addSignal(*pgm, sig);
         handlers[sig].set(fr, pgm);
         ++num_handlers;
      }
      else {
         old = handlers[sig].replace(fr, pgm);
         // handle different Programs here inside the lock
         if (old.pgm != pgm) {
            qore_program_private::delSignal(*old.pgm, sig);
            qore_program_private::addSignal(*pgm, sig);
         }
      }

      // add to the signal mask for signal thread if not already there
      if (!already_set) {
         //printd(5, "adding signal %d to mask\n", sig);
         sigaddset(&mask, sig);
         reload();
      }
   }

   // dereference outside the lock
   old.del(xsink);

   //printd(5, "QoreSignalManager::setHandler() success: sig %d %p pid: %d\n", sig, fr, getpid());
   return 0;
}

int QoreSignalManager::removeHandler(int sig, ExceptionSink *xsink) {
   //printd(5, "QoreSignalManager::removeHandler(sig: %d)\n", sig);
   CodePgm old;

   {
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
      sigdelset(&mask, sig);
      reload();

      //printd(5, "removing handler for signal %d\n", sig);

      // ensure handler is not in progress, if so mark for deletion
      if (handlers[sig].status == QoreSignalHandler::SH_InProgress)
         handlers[sig].status = QoreSignalHandler::SH_Delete;
      else {
         // must be called in the signal lock
         old = handlers[sig].take();
         qore_program_private::delSignal(*old.pgm, sig);
         --num_handlers;
      }
   }

   // dereference outside the lock
   old.del(xsink);

   return 0;
}

QoreStringNode *QoreSignalManager::reassign_signal(int sig, const char* name) {
   AutoLocker al(&mutex);
   if (!enabled())
      return 0;

   // wait for any blocks to be lifted
   while (block) {
      ++waiting;
      cond.wait(&mutex);
      --waiting;
   }

   if (handlers[sig].isSet()) {
      QoreStringNode *err = new QoreStringNode("the Qore library cannot reassign signal %d because a handler has already been installed", sig);
      return err;
   }

   sig_map_t::iterator i = fmap.find(sig);
   if (i != fmap.end()) {
      QoreStringNode *err = new QoreStringNode("the Qore library cannot reassign signal ");
      err->sprintf("%d because it is already managed by module '%s'", sig, i->second.c_str());
      return err;
   }

   fmap[sig] = name;

   return 0;
}
