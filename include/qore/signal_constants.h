/*
  signal_constants.h

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

#ifndef _QORE_SIGNAL_CONSTANTS_H

#define _QORE_SIGNAL_CONSTANTS_H

#include <signal.h>

static inline void addSignalConstants(class Namespace *ns)
{
#ifdef SIGHUP
   ns->addConstant("SIGHUP", new QoreNode((int64)SIGHUP));
#endif
#ifdef SIGINT
   ns->addConstant("SIGINT", new QoreNode((int64)SIGINT));
#endif
#ifdef SIGQUIT
   ns->addConstant("SIGQUIT", new QoreNode((int64)SIGQUIT));
#endif
#ifdef SIGILL
   ns->addConstant("SIGILL", new QoreNode((int64)SIGILL));
#endif
#ifdef SIGTRAP
   ns->addConstant("SIGTRAP", new QoreNode((int64)SIGTRAP));
#endif
#ifdef SIGABRT
   ns->addConstant("SIGABRT", new QoreNode((int64)SIGABRT));
#endif
#ifdef SIGPOLL
   ns->addConstant("SIGPOLL", new QoreNode((int64)SIGPOLL));
#endif
#ifdef SIGIOT
   ns->addConstant("SIGIOT", new QoreNode((int64)SIGIOT));
#endif
#ifdef SIGEMT
   ns->addConstant("SIGEMT", new QoreNode((int64)SIGEMT));
#endif
#ifdef SIGFPE
   ns->addConstant("SIGFPE", new QoreNode((int64)SIGFPE));
#endif
#ifdef SIGKILL
   ns->addConstant("SIGKILL", new QoreNode((int64)SIGKILL));
#endif
#ifdef SIGBUS
   ns->addConstant("SIGBUS", new QoreNode((int64)SIGBUS));
#endif
#ifdef SIGSEGV
   ns->addConstant("SIGSEGV", new QoreNode((int64)SIGSEGV));
#endif
#ifdef SIGSYS
   ns->addConstant("SIGSYS", new QoreNode((int64)SIGSYS));
#endif
#ifdef SIGPIPE
   ns->addConstant("SIGPIPE", new QoreNode((int64)SIGPIPE));
#endif
#ifdef SIGALRM
   ns->addConstant("SIGALRM", new QoreNode((int64)SIGALRM));
#endif
#ifdef SIGTERM
   ns->addConstant("SIGTERM", new QoreNode((int64)SIGTERM));
#endif
#ifdef SIGURG
   ns->addConstant("SIGURG", new QoreNode((int64)SIGURG));
#endif
#ifdef SIGSTOP
   ns->addConstant("SIGSTOP", new QoreNode((int64)SIGSTOP));
#endif
#ifdef SIGTSTP
   ns->addConstant("SIGTSTP", new QoreNode((int64)SIGTSTP));
#endif
#ifdef SIGCONT
   ns->addConstant("SIGCONT", new QoreNode((int64)SIGCONT));
#endif
#ifdef SIGCHLD
   ns->addConstant("SIGCHLD", new QoreNode((int64)SIGCHLD));
#endif
#ifdef SIGTTIN
   ns->addConstant("SIGTTIN", new QoreNode((int64)SIGTTIN));
#endif
#ifdef SIGTTOU
   ns->addConstant("SIGTTOU", new QoreNode((int64)SIGTTOU));
#endif
#ifdef SIGIO
   ns->addConstant("SIGIO", new QoreNode((int64)SIGIO));
#endif
#ifdef SIGXCPU
   ns->addConstant("SIGXCPU", new QoreNode((int64)SIGXCPU));
#endif
#ifdef SIGXFSZ
   ns->addConstant("SIGXFSZ", new QoreNode((int64)SIGXFSZ));
#endif
#ifdef SIGVTALRM
   ns->addConstant("SIGVTALRM", new QoreNode((int64)SIGVTALRM));
#endif
#ifdef SIGPROF
   ns->addConstant("SIGPROF", new QoreNode((int64)SIGPROF));
#endif
#ifdef SIGWINCH
   ns->addConstant("SIGWINCH", new QoreNode((int64)SIGWINCH));
#endif
#ifdef SIGINFO
   ns->addConstant("SIGINFO", new QoreNode((int64)SIGINFO));
#endif
#ifdef SIGUSR1
   ns->addConstant("SIGUSR1", new QoreNode((int64)SIGUSR1));
#endif
#ifdef SIGUSR2
   ns->addConstant("SIGUSR2", new QoreNode((int64)SIGUSR2));
#endif
}

#endif
