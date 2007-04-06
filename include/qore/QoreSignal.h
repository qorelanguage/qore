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

#include <signal.h>
#include <stdio.h>

#include <map>

// maximum number of signals
#ifndef QORE_SIGNAL_MAX
#ifdef NSIG
#define QORE_SIGNAL_MAX (NSIG+1)
#elif defined _NSIG
#define QORE_SIGNAL_MAX _NSIG
#elif defined _NSIGS
#define QORE_SIGNAL_MAX _NSIGS
#else
#error don't know maximum signal number on this platform
#endif
#endif

struct PgmFunc {
   int sig;
   class QoreProgram *pgm;
   class UserFunction *f;
   
   DLLLOCAL PgmFunc(int n_sig, class QoreProgram *n_pgm, class UserFunction *n_f);
   DLLLOCAL ~PgmFunc();
   void runHandler(class ExceptionSink *xsink);
};

typedef std::map<int, struct PgmFunc *> m_int_func_t;

class QoreSignalManager
{
   public:
      static bool sig_raised;
      static bool sig_event[QORE_SIGNAL_MAX];

   private:
      static class LockedObject mutex;
      static m_int_func_t smap;
      
   public:
      DLLLOCAL QoreSignalManager();
      DLLLOCAL ~QoreSignalManager();
      DLLLOCAL static void setHandler(int sig, class QoreProgram *pgm, class UserFunction *f);
      DLLLOCAL static int removeHandler(int sig);
      DLLLOCAL static int removeHandlerFromProgram(int sig);
      DLLLOCAL static class UserFunction *getHandler(int sig);
      DLLLOCAL static void handleSignals();
};

#endif
