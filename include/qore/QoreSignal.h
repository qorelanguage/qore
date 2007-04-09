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

class QoreSignalHandler {
   class AbstractFunctionReference *funcref;
   
public:
   DLLLOCAL void init();
   DLLLOCAL void set(int sig, class AbstractFunctionReference *n_funcref);
   DLLLOCAL void del(int sig, class ExceptionSink *xsink);
   void runHandler(int sig, class ExceptionSink *xsink);
   bool isSet() const
   {
      return (bool)funcref;
   }
};

class QoreSignalManager
{
   public:
      static bool sig_raised;
      static bool sig_event[QORE_SIGNAL_MAX];
      static QoreSignalHandler handlers[QORE_SIGNAL_MAX];
      
   private:
      static class LockedObject mutex;
      
   public:
      DLLLOCAL QoreSignalManager();
      DLLLOCAL ~QoreSignalManager();
      DLLLOCAL static void setHandler(int sig, class AbstractFunctionReference *fr);
      DLLLOCAL static int removeHandler(int sig, class ExceptionSink *xsink);
      DLLLOCAL static void handleSignals();
      DLLLOCAL static void addSignalConstants(class Namespace *ns);
      DLLLOCAL static const char *getSignalName(int sig);
};

#endif
