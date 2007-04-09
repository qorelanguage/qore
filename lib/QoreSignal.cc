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
#include <qore/QoreSignal.h>

#include <stdlib.h>

class QoreSignalManager QSM;

LockedObject QoreSignalManager::mutex;
QoreSignalHandler QoreSignalManager::handlers[QORE_SIGNAL_MAX];
bool QoreSignalManager::sig_event[QORE_SIGNAL_MAX];
bool QoreSignalManager::sig_raised = false;

extern "C" void sighandler(int sig) //, siginfo_t *info, ucontext_t *uap)
{
   QoreSignalManager::sig_raised = true;
   QoreSignalManager::sig_event[sig] = true;
}

// must be called in the signal lock
void QoreSignalHandler::set(int sig, class AbstractFunctionReference *n_funcref)
{
   funcref = n_funcref->copy();
}

void QoreSignalHandler::init()
{
   funcref = 0;
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
   class List *l = new List();
   l->push(new QoreNode((int64)sig));
   class QoreNode *args = new QoreNode(l);
   discard(funcref->exec(args, xsink), xsink);
   args->deref(xsink);
}

QoreSignalManager::QoreSignalManager() 
{
   // zero out sig_event
   for (int i = 0; i < QORE_SIGNAL_MAX; ++i)
   {
      sig_event[i] = 0;
      handlers[i].init();
   }
}

QoreSignalManager::~QoreSignalManager()
{
}

void QoreSignalManager::setHandler(int sig, class AbstractFunctionReference *fr)
{
   bool already_set = false;

   SafeLocker sl(&mutex);

   if (handlers[sig].isSet())
   {
      //printd(5, "replacing handler for signal %d\n", sig);
      already_set = true;
   }

   //printd(5, "setting handler for signal %d, pgm=%08p\n", sig, pgm);
   handlers[sig].set(sig, fr);
   sl.unlock();

   if (!already_set)
   {
      struct sigaction sa;
      sa.sa_handler = sighandler;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags = SA_RESTART;
      sigaction(sig, &sa, NULL);
   }
}

int QoreSignalManager::removeHandler(int sig, class ExceptionSink *xsink)
{
   AutoLocker al(&mutex);

   if (!handlers[sig].isSet())
      return 0;

   //printd(5, "removing handler for signal %d\n", sig);
   
   struct sigaction sa;
   sa.sa_handler = (sig == SIGPIPE ? SIG_IGN : SIG_DFL);
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = SA_RESTART;
   sigaction(sig, &sa, NULL);

   // must be called in the signal lock
   handlers[sig].del(sig, xsink);

   return 0;
}

void QoreSignalManager::handleSignals()
{
   if (!sig_raised)
      return;

   ExceptionSink xsink;
   SafeLocker sl(&mutex);

   //printd(5, "handleSignals() called sig_raise=%d\n", sig_raised);   
   // check flag again inside the lock
   if (!sig_raised)
      return;
   sig_raised = false;
   
   for (int i = 1; i < QORE_SIGNAL_MAX; ++i)
   {
      if (sig_event[i])
      {
	 sig_event[i] = false;
	 if (!handlers[i].isSet())
	    continue;

	 sl.unlock();
	 handlers[i].runHandler(i, &xsink);
	 sl.lock();
      }
   }   
}

#define CPP_MAKE_STRING1(x) #x
#define CPP_MAKE_STRING_FROM_SYMBOL(x) CPP_MAKE_STRING1(x)

void QoreSignalManager::addSignalConstants(class Namespace *ns)
{
   class Hash *nh = new Hash();
   class Hash *sh = new Hash();
#ifdef SIGHUP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGHUP), new QoreNode("SIGHUP"), NULL);
   sh->setKeyValue("SIGHUP", new QoreNode((int64)SIGHUP), NULL);
   ns->addConstant("SIGHUP", new QoreNode((int64)SIGHUP));
#endif
#ifdef SIGINT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGINT), new QoreNode("SIGINT"), NULL);
   sh->setKeyValue("SIGINT", new QoreNode((int64)SIGINT), NULL);
   ns->addConstant("SIGINT", new QoreNode((int64)SIGINT));
#endif
#ifdef SIGQUIT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGQUIT), new QoreNode("SIGQUIT"), NULL);
   sh->setKeyValue("SIGQUIT", new QoreNode((int64)SIGQUIT), NULL);
   ns->addConstant("SIGQUIT", new QoreNode((int64)SIGQUIT));
#endif
#ifdef SIGILL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGILL), new QoreNode("SIGILL"), NULL);
   sh->setKeyValue("SIGILL", new QoreNode((int64)SIGILL), NULL);
   ns->addConstant("SIGILL", new QoreNode((int64)SIGILL));
#endif
#ifdef SIGTRAP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTRAP), new QoreNode("SIGTRAP"), NULL);
   sh->setKeyValue("SIGTRAP", new QoreNode((int64)SIGTRAP), NULL);
   ns->addConstant("SIGTRAP", new QoreNode((int64)SIGTRAP));
#endif
#ifdef SIGABRT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGABRT), new QoreNode("SIGABRT"), NULL);
   sh->setKeyValue("SIGABRT", new QoreNode((int64)SIGABRT), NULL);
   ns->addConstant("SIGABRT", new QoreNode((int64)SIGABRT));
#endif
#ifdef SIGPOLL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPOLL), new QoreNode("SIGPOLL"), NULL);
   sh->setKeyValue("SIGPOLL", new QoreNode((int64)SIGPOLL), NULL);
   ns->addConstant("SIGPOLL", new QoreNode((int64)SIGPOLL));
#endif
#ifdef SIGIOT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGIOT), new QoreNode("SIGIOT"), NULL);
   sh->setKeyValue("SIGIOT", new QoreNode((int64)SIGIOT), NULL);
   ns->addConstant("SIGIOT", new QoreNode((int64)SIGIOT));
#endif
#ifdef SIGEMT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGEMT), new QoreNode("SIGEMT"), NULL);
   sh->setKeyValue("SIGEMT", new QoreNode((int64)SIGEMT), NULL);
   ns->addConstant("SIGEMT", new QoreNode((int64)SIGEMT));
#endif
#ifdef SIGFPE
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGFPE), new QoreNode("SIGFPE"), NULL);
   sh->setKeyValue("SIGFPE", new QoreNode((int64)SIGFPE), NULL);
   ns->addConstant("SIGFPE", new QoreNode((int64)SIGFPE));
#endif
#ifdef SIGKILL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGKILL), new QoreNode("SIGKILL"), NULL);
   sh->setKeyValue("SIGKILL", new QoreNode((int64)SIGKILL), NULL);
   ns->addConstant("SIGKILL", new QoreNode((int64)SIGKILL));
#endif
#ifdef SIGBUS
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGBUS), new QoreNode("SIGBUS"), NULL);
   sh->setKeyValue("SIGBUS", new QoreNode((int64)SIGBUS), NULL);
   ns->addConstant("SIGBUS", new QoreNode((int64)SIGBUS));
#endif
#ifdef SIGSEGV
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSEGV), new QoreNode("SIGSEGV"), NULL);
   sh->setKeyValue("SIGSEGV", new QoreNode((int64)SIGSEGV), NULL);
   ns->addConstant("SIGSEGV", new QoreNode((int64)SIGSEGV));
#endif
#ifdef SIGSYS
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSYS), new QoreNode("SIGSYS"), NULL);
   sh->setKeyValue("SIGSYS", new QoreNode((int64)SIGSYS), NULL);
   ns->addConstant("SIGSYS", new QoreNode((int64)SIGSYS));
#endif
#ifdef SIGPIPE
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPIPE), new QoreNode("SIGPIPE"), NULL);
   sh->setKeyValue("SIGPIPE", new QoreNode((int64)SIGPIPE), NULL);
   ns->addConstant("SIGPIPE", new QoreNode((int64)SIGPIPE));
#endif
#ifdef SIGALRM
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGALRM), new QoreNode("SIGALRM"), NULL);
   sh->setKeyValue("SIGALRM", new QoreNode((int64)SIGALRM), NULL);
   ns->addConstant("SIGALRM", new QoreNode((int64)SIGALRM));
#endif
#ifdef SIGTERM
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTERM), new QoreNode("SIGTERM"), NULL);
   sh->setKeyValue("SIGTERM", new QoreNode((int64)SIGTERM), NULL);
   ns->addConstant("SIGTERM", new QoreNode((int64)SIGTERM));
#endif
#ifdef SIGURG
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGURG), new QoreNode("SIGURG"), NULL);
   sh->setKeyValue("SIGURG", new QoreNode((int64)SIGURG), NULL);
   ns->addConstant("SIGURG", new QoreNode((int64)SIGURG));
#endif
#ifdef SIGSTOP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSTOP), new QoreNode("SIGSTOP"), NULL);
   sh->setKeyValue("SIGSTOP", new QoreNode((int64)SIGSTOP), NULL);
   ns->addConstant("SIGSTOP", new QoreNode((int64)SIGSTOP));
#endif
#ifdef SIGTSTP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTSTP), new QoreNode("SIGTSTP"), NULL);
   sh->setKeyValue("SIGTSTP", new QoreNode((int64)SIGTSTP), NULL);
   ns->addConstant("SIGTSTP", new QoreNode((int64)SIGTSTP));
#endif
#ifdef SIGCONT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCONT), new QoreNode("SIGCONT"), NULL);
   sh->setKeyValue("SIGCONT", new QoreNode((int64)SIGCONT), NULL);
   ns->addConstant("SIGCONT", new QoreNode((int64)SIGCONT));
#endif
#ifdef SIGCHLD
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCHLD), new QoreNode("SIGCHLD"), NULL);
   sh->setKeyValue("SIGCHLD", new QoreNode((int64)SIGCHLD), NULL);
   ns->addConstant("SIGCHLD", new QoreNode((int64)SIGCHLD));
#endif
#ifdef SIGTTIN
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTTIN), new QoreNode("SIGTTIN"), NULL);
   sh->setKeyValue("SIGTTIN", new QoreNode((int64)SIGTTIN), NULL);
   ns->addConstant("SIGTTIN", new QoreNode((int64)SIGTTIN));
#endif
#ifdef SIGTTOU
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTTOU), new QoreNode("SIGTTOU"), NULL);
   sh->setKeyValue("SIGTTOU", new QoreNode((int64)SIGTTOU), NULL);
   ns->addConstant("SIGTTOU", new QoreNode((int64)SIGTTOU));
#endif
#ifdef SIGIO
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGIO), new QoreNode("SIGIO"), NULL);
   sh->setKeyValue("SIGIO", new QoreNode((int64)SIGIO), NULL);
   ns->addConstant("SIGIO", new QoreNode((int64)SIGIO));
#endif
#ifdef SIGXCPU
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGXCPU), new QoreNode("SIGXCPU"), NULL);
   sh->setKeyValue("SIGXCPU", new QoreNode((int64)SIGXCPU), NULL);
   ns->addConstant("SIGXCPU", new QoreNode((int64)SIGXCPU));
#endif
#ifdef SIGXFSZ
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGXFSZ), new QoreNode("SIGXFSZ"), NULL);
   sh->setKeyValue("SIGXFSZ", new QoreNode((int64)SIGXFSZ), NULL);
   ns->addConstant("SIGXFSZ", new QoreNode((int64)SIGXFSZ));
#endif
#ifdef SIGVTALRM
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGVTALRM), new QoreNode("SIGVTALRM"), NULL);
   sh->setKeyValue("SIGVTALRM", new QoreNode((int64)SIGVTALRM), NULL);
   ns->addConstant("SIGVTALRM", new QoreNode((int64)SIGVTALRM));
#endif
#ifdef SIGPROF
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPROF), new QoreNode("SIGPROF"), NULL);
   sh->setKeyValue("SIGPROF", new QoreNode((int64)SIGPROF), NULL);
   ns->addConstant("SIGPROF", new QoreNode((int64)SIGPROF));
#endif
#ifdef SIGWINCH
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGWINCH), new QoreNode("SIGWINCH"), NULL);
   sh->setKeyValue("SIGWINCH", new QoreNode((int64)SIGWINCH), NULL);
   ns->addConstant("SIGWINCH", new QoreNode((int64)SIGWINCH));
#endif
#ifdef SIGINFO
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGINFO), new QoreNode("SIGINFO"), NULL);
   sh->setKeyValue("SIGINFO", new QoreNode((int64)SIGINFO), NULL);
   ns->addConstant("SIGINFO", new QoreNode((int64)SIGINFO));
#endif
#ifdef SIGUSR1
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGUSR1), new QoreNode("SIGUSR1"), NULL);
   sh->setKeyValue("SIGUSR1", new QoreNode((int64)SIGUSR1), NULL);
   ns->addConstant("SIGUSR1", new QoreNode((int64)SIGUSR1));
#endif
#ifdef SIGUSR2
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGUSR2), new QoreNode("SIGUSR2"), NULL);
   sh->setKeyValue("SIGUSR2", new QoreNode((int64)SIGUSR2), NULL);
   ns->addConstant("SIGUSR2", new QoreNode((int64)SIGUSR2));
#endif
#ifdef SIGSTKSZ
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSTKSZ), new QoreNode("SIGSTKSZ"), NULL);
   sh->setKeyValue("SIGSTKSZ", new QoreNode((int64)SIGSTKSZ), NULL);
   ns->addConstant("SIGSTKSZ", new QoreNode((int64)SIGSTKSZ));
#endif
   
   ns->addConstant("SignalToName", new QoreNode(nh));
   ns->addConstant("NametoSignal", new QoreNode(sh));
}

