/*
  QoreSignal.cc

  Qore programming language

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

#include <qore/Qore.h>
#include <qore/intern/QoreSignal.h>

#include <stdlib.h>

class QoreSignalManager QSM;

int QoreSignalManager::num_handlers = 0;
LockedObject QoreSignalManager::mutex;
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

/*
extern "C" void sighandler(int sig) //, siginfo_t *info, ucontext_t *uap)
{
   QoreSignalManager::sig_raised = true;
   QoreSignalManager::sig_event[sig] = true;
}
*/

// must be called in the signal lock
void QoreSignalHandler::set(int sig, class AbstractFunctionReference *n_funcref)
{
   funcref = n_funcref->copy();
}

void QoreSignalHandler::init()
{
   funcref = 0;
   status = SH_OK;
}

// must be called in the signal lock
void QoreSignalHandler::del(int sig, class ExceptionSink *xsink)
{
   if (funcref)
      funcref->del(xsink);
   init();
}

void QoreSignalHandler::runHandler(int sig, class ExceptionSink *xsink)
{
   // create signal number argument
   class QoreList *l = new QoreList();
   l->push(new QoreNode((int64)sig));
   class QoreNode *args = new QoreNode(l);
   discard(funcref->exec(args, xsink), xsink);
   args->deref(xsink);
}

QoreSignalManager::QoreSignalManager() 
{
   // set command to none
   cmd = C_None;
   
   // initilize handlers
   for (int i = 0; i < QORE_SIGNAL_MAX; ++i)
      handlers[i].init();

   tid = -1;
}

void QoreSignalManager::init(bool disable_signal_mask) 
{
   // set SIGPIPE to ignore
   struct sigaction sa;
   sa.sa_handler = SIG_IGN;
   sigemptyset (&sa.sa_mask);
   sa.sa_flags = SA_RESTART;
   // ignore SIGPIPE signals
   sigaction(SIGPIPE, &sa, NULL);

   if (!disable_signal_mask)
   {
      // block all signals
      sigfillset(&mask);
#ifdef PROFILE
      // do not block SIGPROF if profiling is enabled
      sigdelset(&mask, SIGPROF);
#endif
      pthread_sigmask(SIG_SETMASK, &mask, NULL);

      // set up default handler mask
      sigemptyset(&mask);
      sigaddset(&mask, QORE_STATUS_SIGNAL);

      ExceptionSink xsink;
      if (start_signal_thread(&xsink))
      {
	 xsink.handleExceptions();
	 exit(1);
      }
   }
}

void QoreSignalManager::del()
{
   if (!enabled())
      return;
   stop_signal_thread();
}

// must only be called inside the lock
void QoreSignalManager::reload()
{
   cmd = C_Reload;
   if (thread_running && tid != ::gettid())
   {
#ifdef DEBUG
      int rc = 
#endif
	 pthread_kill(ptid, QORE_STATUS_SIGNAL);
      assert(!rc);
      // unlock lock and wait for condition
      cond.wait(&mutex);
   }
}

void QoreSignalManager::stop_signal_thread_unlocked()
{
   cmd = C_Exit;
   if (thread_running)
   {
#ifdef DEBUG
      int rc = 
#endif
	 pthread_kill(ptid, QORE_STATUS_SIGNAL);
      assert(!rc);
   }
}

void QoreSignalManager::stop_signal_thread()
{
   SafeLocker sl(&mutex);
   stop_signal_thread_unlocked();

   // wait for thread to exit (may be already gone)
   sl.unlock();
   tcount.waitForZero();   
}

void QoreSignalManager::pre_fork_block_and_stop()
{
   SafeLocker sl(&mutex);
   if (!enabled())
      return;

   // if another block is already in progress then wait for it to complete
   while (block)
   {
      ++waiting;
      cond.wait(&mutex);
      --waiting;
   }	 
   block = true;
   stop_signal_thread_unlocked();

   // wait for thread to exit (may be already gone)
   sl.unlock();
   tcount.waitForZero();
}

void QoreSignalManager::post_fork_unblock_and_start(bool new_process, class ExceptionSink *xsink)
{
   AutoLocker al(&mutex);
   if (!enabled())
      return;

   block = false;
   if (waiting)
      cond.signal();

   // set new default signal mask for new process
   if (new_process)
   {
      // block all signals
      sigset_t new_mask;
      sigfillset(&new_mask);
      pthread_sigmask(SIG_SETMASK, &new_mask, NULL);
   }

   start_signal_thread(xsink);
}

void QoreSignalManager::signal_handler_thread()
{
   register_thread(tid, ptid, 0);
   
   printd(5, "signal handler thread started (TID %d)\n", tid);

   sigset_t c_mask;
   int sig;
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

   while (true)
   {
      if (cmd != C_None)
      {
	 sig_cmd_e c = cmd;
	 cmd = C_None;
	 // check command
	 if (c == C_Exit)
	    break;
	 if (c == C_Reload)
	 {
	    memcpy(&c_mask, &mask, sizeof(sigset_t));
	    // block only signals we are catching in this thread
	    pthread_sigmask(SIG_SETMASK, &c_mask, NULL);
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
	 // FIXME: set thread-local stora
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
      else
      {
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

   printd(5, "signal handler thread terminating\n");
   
   // delete internal thread data structure
   delete_thread_data();

   // deregister_thread
   deregister_signal_thread();

   // run thread cleanup handlers
   tclist.exec();

   tcount.dec();
   //printf("signal handler thread %d stopped (count=%d)\n", c_tid, tcount.getCount());fflush(stdout);
   pthread_exit(NULL);
}

extern "C" void *sig_thread(void *x)
{
   QoreSignalManager::signal_handler_thread();
   return NULL;
}

int QoreSignalManager::start_signal_thread(class ExceptionSink *xsink)
{
   printd(5, "start_signal_thread() called\n");
   tid = get_signal_thread_entry();

   // if can't start thread, then throw exception
   if (tid == -1)
   {
      xsink->raiseException("THREAD-CREATION-FAILURE", "thread list is full with %d threads", MAX_QORE_THREADS);
      return -1;
   }
   printd(5, "start_signal_thread() got TID %d\n", tid);
   
   thread_running = true;
   tcount.inc();
   assert(tcount.getCount() == 1);
   int rc = pthread_create(&ptid, &ta_default, sig_thread, NULL);
   if (rc)
   {
      tcount.dec();
      deregister_signal_thread();
      tid = -1;
      xsink->raiseException("THREAD-CREATION-FAILURE", "could not create thread: %s", strerror(rc));
      thread_running = false;
   }
   //printd(5, "start_signal_thread() rc=%d\n", rc);
   return rc;
}

int QoreSignalManager::setHandler(int sig, class AbstractFunctionReference *fr, class ExceptionSink *xsink)
{
   AutoLocker al(&mutex);
   if (!enabled())
      return 0;

   // wait for any blocks to be lifted
   while (block)
   {
      ++waiting;
      cond.wait(&mutex);
      --waiting;
   }	 
   
   bool already_set = true;
   if (!handlers[sig].isSet())
   {
      already_set = false;
      // start signal thread for first handler
      if (!thread_running && start_signal_thread(xsink))
	 return -1;
      
      ++num_handlers;
   }

   //printd(5, "setting handler for signal %d, pgm=%08p\n", sig, pgm);
   handlers[sig].set(sig, fr);

   // add to the signal mask for signal thread if not already there
   if (!already_set && sig != QORE_STATUS_SIGNAL)
   {
      sigaddset(&mask, sig);
      reload();
   }
   
   return 0;
}

int QoreSignalManager::removeHandler(int sig, class ExceptionSink *xsink)
{
   AutoLocker al(&mutex);
   if (!enabled())
      return 0;

   // wait for any blocks to be lifted
   while (block)
   {
      ++waiting;
      cond.wait(&mutex);
      --waiting;
   }	 
   
   if (!handlers[sig].isSet())
      return 0;

   // remove from the signal mask for sigwait
   if (sig != QORE_STATUS_SIGNAL)
   {
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

#define CPP_MAKE_STRING1(x) #x
#define CPP_MAKE_STRING_FROM_SYMBOL(x) CPP_MAKE_STRING1(x)

void QoreSignalManager::addSignalConstants(class QoreNamespace *ns)
{
   class QoreHash *nh = new QoreHash();
   class QoreHash *sh = new QoreHash();
#ifdef SIGHUP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGHUP), new QoreStringNode("SIGHUP"), NULL);
   sh->setKeyValue("SIGHUP", new QoreNode((int64)SIGHUP), NULL);
   ns->addConstant("SIGHUP", new QoreNode((int64)SIGHUP));
#endif
#ifdef SIGINT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGINT), new QoreStringNode("SIGINT"), NULL);
   sh->setKeyValue("SIGINT", new QoreNode((int64)SIGINT), NULL);
   ns->addConstant("SIGINT", new QoreNode((int64)SIGINT));
#endif
#ifdef SIGQUIT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGQUIT), new QoreStringNode("SIGQUIT"), NULL);
   sh->setKeyValue("SIGQUIT", new QoreNode((int64)SIGQUIT), NULL);
   ns->addConstant("SIGQUIT", new QoreNode((int64)SIGQUIT));
#endif
#ifdef SIGILL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGILL), new QoreStringNode("SIGILL"), NULL);
   sh->setKeyValue("SIGILL", new QoreNode((int64)SIGILL), NULL);
   ns->addConstant("SIGILL", new QoreNode((int64)SIGILL));
#endif
#ifdef SIGTRAP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTRAP), new QoreStringNode("SIGTRAP"), NULL);
   sh->setKeyValue("SIGTRAP", new QoreNode((int64)SIGTRAP), NULL);
   ns->addConstant("SIGTRAP", new QoreNode((int64)SIGTRAP));
#endif
#ifdef SIGABRT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGABRT), new QoreStringNode("SIGABRT"), NULL);
   sh->setKeyValue("SIGABRT", new QoreNode((int64)SIGABRT), NULL);
   ns->addConstant("SIGABRT", new QoreNode((int64)SIGABRT));
#endif
#ifdef SIGPOLL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPOLL), new QoreStringNode("SIGPOLL"), NULL);
   sh->setKeyValue("SIGPOLL", new QoreNode((int64)SIGPOLL), NULL);
   ns->addConstant("SIGPOLL", new QoreNode((int64)SIGPOLL));
#endif
#ifdef SIGIOT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGIOT), new QoreStringNode("SIGIOT"), NULL);
   sh->setKeyValue("SIGIOT", new QoreNode((int64)SIGIOT), NULL);
   ns->addConstant("SIGIOT", new QoreNode((int64)SIGIOT));
#endif
#ifdef SIGEMT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGEMT), new QoreStringNode("SIGEMT"), NULL);
   sh->setKeyValue("SIGEMT", new QoreNode((int64)SIGEMT), NULL);
   ns->addConstant("SIGEMT", new QoreNode((int64)SIGEMT));
#endif
#ifdef SIGFPE
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGFPE), new QoreStringNode("SIGFPE"), NULL);
   sh->setKeyValue("SIGFPE", new QoreNode((int64)SIGFPE), NULL);
   ns->addConstant("SIGFPE", new QoreNode((int64)SIGFPE));
#endif
#ifdef SIGKILL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGKILL), new QoreStringNode("SIGKILL"), NULL);
   sh->setKeyValue("SIGKILL", new QoreNode((int64)SIGKILL), NULL);
   ns->addConstant("SIGKILL", new QoreNode((int64)SIGKILL));
#endif
#ifdef SIGBUS
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGBUS), new QoreStringNode("SIGBUS"), NULL);
   sh->setKeyValue("SIGBUS", new QoreNode((int64)SIGBUS), NULL);
   ns->addConstant("SIGBUS", new QoreNode((int64)SIGBUS));
#endif
#ifdef SIGSEGV
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSEGV), new QoreStringNode("SIGSEGV"), NULL);
   sh->setKeyValue("SIGSEGV", new QoreNode((int64)SIGSEGV), NULL);
   ns->addConstant("SIGSEGV", new QoreNode((int64)SIGSEGV));
#endif
#ifdef SIGSYS
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSYS), new QoreStringNode("SIGSYS"), NULL);
   sh->setKeyValue("SIGSYS", new QoreNode((int64)SIGSYS), NULL);
   ns->addConstant("SIGSYS", new QoreNode((int64)SIGSYS));
#endif
#ifdef SIGPIPE
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPIPE), new QoreStringNode("SIGPIPE"), NULL);
   sh->setKeyValue("SIGPIPE", new QoreNode((int64)SIGPIPE), NULL);
   ns->addConstant("SIGPIPE", new QoreNode((int64)SIGPIPE));
#endif
#ifdef SIGALRM
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGALRM), new QoreStringNode("SIGALRM"), NULL);
   sh->setKeyValue("SIGALRM", new QoreNode((int64)SIGALRM), NULL);
   ns->addConstant("SIGALRM", new QoreNode((int64)SIGALRM));
#endif
#ifdef SIGTERM
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTERM), new QoreStringNode("SIGTERM"), NULL);
   sh->setKeyValue("SIGTERM", new QoreNode((int64)SIGTERM), NULL);
   ns->addConstant("SIGTERM", new QoreNode((int64)SIGTERM));
#endif
#ifdef SIGURG
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGURG), new QoreStringNode("SIGURG"), NULL);
   sh->setKeyValue("SIGURG", new QoreNode((int64)SIGURG), NULL);
   ns->addConstant("SIGURG", new QoreNode((int64)SIGURG));
#endif
#ifdef SIGSTOP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSTOP), new QoreStringNode("SIGSTOP"), NULL);
   sh->setKeyValue("SIGSTOP", new QoreNode((int64)SIGSTOP), NULL);
   ns->addConstant("SIGSTOP", new QoreNode((int64)SIGSTOP));
#endif
#ifdef SIGTSTP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTSTP), new QoreStringNode("SIGTSTP"), NULL);
   sh->setKeyValue("SIGTSTP", new QoreNode((int64)SIGTSTP), NULL);
   ns->addConstant("SIGTSTP", new QoreNode((int64)SIGTSTP));
#endif
#ifdef SIGCONT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCONT), new QoreStringNode("SIGCONT"), NULL);
   sh->setKeyValue("SIGCONT", new QoreNode((int64)SIGCONT), NULL);
   ns->addConstant("SIGCONT", new QoreNode((int64)SIGCONT));
#endif
#ifdef SIGCHLD
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCHLD), new QoreStringNode("SIGCHLD"), NULL);
   sh->setKeyValue("SIGCHLD", new QoreNode((int64)SIGCHLD), NULL);
   ns->addConstant("SIGCHLD", new QoreNode((int64)SIGCHLD));
#endif
#ifdef SIGTTIN
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTTIN), new QoreStringNode("SIGTTIN"), NULL);
   sh->setKeyValue("SIGTTIN", new QoreNode((int64)SIGTTIN), NULL);
   ns->addConstant("SIGTTIN", new QoreNode((int64)SIGTTIN));
#endif
#ifdef SIGTTOU
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTTOU), new QoreStringNode("SIGTTOU"), NULL);
   sh->setKeyValue("SIGTTOU", new QoreNode((int64)SIGTTOU), NULL);
   ns->addConstant("SIGTTOU", new QoreNode((int64)SIGTTOU));
#endif
#ifdef SIGIO
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGIO), new QoreStringNode("SIGIO"), NULL);
   sh->setKeyValue("SIGIO", new QoreNode((int64)SIGIO), NULL);
   ns->addConstant("SIGIO", new QoreNode((int64)SIGIO));
#endif
#ifdef SIGXCPU
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGXCPU), new QoreStringNode("SIGXCPU"), NULL);
   sh->setKeyValue("SIGXCPU", new QoreNode((int64)SIGXCPU), NULL);
   ns->addConstant("SIGXCPU", new QoreNode((int64)SIGXCPU));
#endif
#ifdef SIGXFSZ
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGXFSZ), new QoreStringNode("SIGXFSZ"), NULL);
   sh->setKeyValue("SIGXFSZ", new QoreNode((int64)SIGXFSZ), NULL);
   ns->addConstant("SIGXFSZ", new QoreNode((int64)SIGXFSZ));
#endif
#ifdef SIGVTALRM
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGVTALRM), new QoreStringNode("SIGVTALRM"), NULL);
   sh->setKeyValue("SIGVTALRM", new QoreNode((int64)SIGVTALRM), NULL);
   ns->addConstant("SIGVTALRM", new QoreNode((int64)SIGVTALRM));
#endif
#ifdef SIGPROF
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPROF), new QoreStringNode("SIGPROF"), NULL);
   sh->setKeyValue("SIGPROF", new QoreNode((int64)SIGPROF), NULL);
   ns->addConstant("SIGPROF", new QoreNode((int64)SIGPROF));
#endif
#ifdef SIGWINCH
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGWINCH), new QoreStringNode("SIGWINCH"), NULL);
   sh->setKeyValue("SIGWINCH", new QoreNode((int64)SIGWINCH), NULL);
   ns->addConstant("SIGWINCH", new QoreNode((int64)SIGWINCH));
#endif
#ifdef SIGINFO
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGINFO), new QoreStringNode("SIGINFO"), NULL);
   sh->setKeyValue("SIGINFO", new QoreNode((int64)SIGINFO), NULL);
   ns->addConstant("SIGINFO", new QoreNode((int64)SIGINFO));
#endif
#ifdef SIGUSR1
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGUSR1), new QoreStringNode("SIGUSR1"), NULL);
   sh->setKeyValue("SIGUSR1", new QoreNode((int64)SIGUSR1), NULL);
   ns->addConstant("SIGUSR1", new QoreNode((int64)SIGUSR1));
#endif
#ifdef SIGUSR2
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGUSR2), new QoreStringNode("SIGUSR2"), NULL);
   sh->setKeyValue("SIGUSR2", new QoreNode((int64)SIGUSR2), NULL);
   ns->addConstant("SIGUSR2", new QoreNode((int64)SIGUSR2));
#endif
#ifdef SIGSTKFLT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSTKSZ), new QoreStringNode("SIGSTKFLT"), NULL);
   sh->setKeyValue("SIGSTKFLT", new QoreNode((int64)SIGSTKFLT), NULL);
   ns->addConstant("SIGSTKFLT", new QoreNode((int64)SIGSTKFLT));
#endif
#ifdef SIGCLD
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCHLD), new QoreStringNode("SIGCLD"), NULL);
   sh->setKeyValue("SIGCLD", new QoreNode((int64)SIGCLD), NULL);
   ns->addConstant("SIGCLD", new QoreNode((int64)SIGCLD));
#endif
#ifdef SIGPWR
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPWR), new QoreStringNode("SIGPWR"), NULL);
   sh->setKeyValue("SIGPWR", new QoreNode((int64)SIGPWR), NULL);
   ns->addConstant("SIGPWR", new QoreNode((int64)SIGPWR));
#endif
#ifdef SIGLOST
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGLOST), new QoreStringNode("SIGLOST"), NULL);
   sh->setKeyValue("SIGLOST", new QoreNode((int64)SIGLOST), NULL);
   ns->addConstant("SIGLOST", new QoreNode((int64)SIGLOST));
#endif
#ifdef SIGWAITING
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGWAITING), new QoreStringNode("SIGWAITING"), NULL);
   sh->setKeyValue("SIGWAITING", new QoreNode((int64)SIGWAITING), NULL);
   ns->addConstant("SIGWAITING", new QoreNode((int64)SIGWAITING));
#endif
#ifdef SIGLWP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGLWP), new QoreStringNode("SIGLWP"), NULL);
   sh->setKeyValue("SIGLWP", new QoreNode((int64)SIGLWP), NULL);
   ns->addConstant("SIGLWP", new QoreNode((int64)SIGLWP));
#endif
#ifdef SIGFREEZE
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGFREEZE), new QoreStringNode("SIGFREEZE"), NULL);
   sh->setKeyValue("SIGFREEZE", new QoreNode((int64)SIGFREEZE), NULL);
   ns->addConstant("SIGFREEZE", new QoreNode((int64)SIGFREEZE));
#endif
#ifdef SIGTHAW
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTHAW), new QoreStringNode("SIGTHAW"), NULL);
   sh->setKeyValue("SIGTHAW", new QoreNode((int64)SIGTHAW), NULL);
   ns->addConstant("SIGTHAW", new QoreNode((int64)SIGTHAW));
#endif
#ifdef SIGCANCEL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCANCEL), new QoreStringNode("SIGCANCEL"), NULL);
   sh->setKeyValue("SIGCANCEL", new QoreNode((int64)SIGCANCEL), NULL);
   ns->addConstant("SIGCANCEL", new QoreNode((int64)SIGCANCEL));
#endif
#ifdef SIGXRES
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGXRES), new QoreStringNode("SIGXRES"), NULL);
   sh->setKeyValue("SIGXRES", new QoreNode((int64)SIGXRES), NULL);
   ns->addConstant("SIGXRES", new QoreNode((int64)SIGXRES));
#endif
#ifdef SIGJVM1
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGJVM1), new QoreStringNode("SIGJVM1"), NULL);
   sh->setKeyValue("SIGJVM1", new QoreNode((int64)SIGJVM1), NULL);
   ns->addConstant("SIGJVM1", new QoreNode((int64)SIGJVM1));
#endif
#ifdef SIGJVM2
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGJVM2), new QoreStringNode("SIGJVM2"), NULL);
   sh->setKeyValue("SIGJVM2", new QoreNode((int64)SIGJVM2), NULL);
   ns->addConstant("SIGJVM2", new QoreNode((int64)SIGJVM2));
#endif
   
   ns->addConstant("SignalToName", new QoreNode(nh));
   ns->addConstant("NametoSignal", new QoreNode(sh));
}

