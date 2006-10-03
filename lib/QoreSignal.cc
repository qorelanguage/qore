/*
  QoreSignal.cc

  Qore programming language

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

#include <qore/config.h>
#include <qore/QoreSignal.h>
#include <qore/support.h>
#include <qore/qore_thread.h>
#include <qore/Function.h>

#include <stdlib.h>

class QoreSignalManager QSM;

extern "C" void sighandler(int sig, siginfo_t *info, ucontext_t *uap)
{
/*
   switch (sig)
   {
      case SIGINT:
         signal(sig, sighandler);
         printe("SIGINT: Interrupt from Keyboard\n");
	 sig_leave(1);
         break;
      case SIGHUP:
         signal(sig, sighandler);
         printe("SIGHUP: Hangup Detected\n");
	 sig_leave(1);
         break;
      case SIGTERM:
         signal(sig, sighandler);
         printe("SIGTERM: Termination Signal\n");
	 sig_leave(1);
         break;
      case SIGTRAP:
         signal(sig, sighandler);
         printe("SIGTRAP: Trace/Breakpoint Trap\n");
         break;
      case SIGQUIT:
         signal(sig, sighandler);
         printe("SIGQUIT: Quit from Keyboard\n");
	 sig_leave(1);
         break;
      case SIGSEGV:
         signal(sig, sighandler);
         printe("SIGSEGV: Segmentation Violation\n");
	 sig_leave(1);
         break;
      case SIGABRT:
         signal(sig, sighandler);
         printe("SIGABRT: Abort Program\n");
	 sig_leave(1);
         break;
      case SIGILL:
         signal(sig, sighandler);
         printe("SIGILL: Illegal Instruction\n");
	 sig_leave(1);
         break;
      case SIGFPE:
         signal(sig, sighandler);
         printe("SIGFPE: Floating Point Exception\n");
	 sig_leave(1);
         break;
      case SIGCONT:
         signal(sig, sighandler);
//         printe(""SIGCONT: Continue\n");
         break;
      case SIGIO:
         signal(sig, sighandler);
         printe("SIGIO: I/O Error\n");
         break;
      case SIGXCPU:
         signal(sig, sighandler);
         printe("SIGXCPU: CPU Time Limit Exceeded\n");
	 sig_leave(1);
         break;
      case SIGXFSZ:
         signal(sig, sighandler);
         printe("SIGXFSZ: File Size Limit Exceeded\n");
	 sig_leave(1);
         break;
      case SIGWINCH:
         signal(sig, sighandler);
         printe("SIGWINCH: Window Resized\n");
         break;
   }
*/
}

void init_signals(void)
{
   tracein("init_signals()");
#if 0
   signal(SIGINT,   sighandler);
   signal(SIGHUP,   sighandler);
   signal(SIGTERM,  sighandler);
   signal(SIGQUIT,  sighandler);
   signal(SIGSEGV,  sighandler);
   signal(SIGABRT,  sighandler);
   signal(SIGILL,   sighandler);
   signal(SIGFPE,   sighandler);
   signal(SIGIO,    sighandler);
   signal(SIGXCPU,  sighandler);
   signal(SIGXFSZ,  sighandler);

   signal(SIGPIPE,  SIG_IGN);
   signal(SIGWINCH, SIG_IGN);
   signal(SIGTRAP,  SIG_IGN);
   signal(SIGCONT,  SIG_IGN);
#endif
   traceout("init_signals()");
}


