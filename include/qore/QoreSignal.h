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

#include <signal.h>
#include <stdio.h>

#include <map>

// maximum number of signals
#ifndef QORE_SIGNAL_MAX
#define QORE_SIGNAL_MAX 128
#endif

typedef std::map<int, class Function *> m_int_func_t;

class QoreSignalManager {
   private:
      m_int_func_t smap;
   
   public:
      DLLLOCAL QoreSignalManager();
      DLLLOCAL ~QoreSignalManager();
      DLLLOCAL void setHandler(int sig, class Function *f);
      DLLLOCAL void removeHandler(int sig);
      DLLLOCAL class Function *getHandler(int sig);
      DLLLOCAL void handleSignal(int sig);
};

#endif
