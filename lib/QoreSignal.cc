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

VRMutex QoreSignalManager::gate;
m_int_func_t QoreSignalManager::smap;
bool QoreSignalManager::sig_event[QORE_SIGNAL_MAX];
bool QoreSignalManager::sig_raised = false;

extern "C" void sighandler(int sig) //, siginfo_t *info, ucontext_t *uap)
{
   QoreSignalManager::sig_raised = true;
   QoreSignalManager::sig_event[sig] = true;
}

QoreSignalManager::QoreSignalManager() 
{
   for (int i = 0; i < QORE_SIGNAL_MAX; ++i)
      sig_event[i] = 0;
}

QoreSignalManager::~QoreSignalManager()
{
}

void QoreSignalManager::setHandler(int sig, class QoreProgram *pgm, class UserFunction *f, class ExceptionSink *xsink)
{
   bool already_set = false;

   if (gate.enter(xsink))
      return;

   m_int_func_t::iterator i = smap.find(sig);
   if (i != smap.end())
   {
      already_set = true;
      class PgmFunc *pf = i->second;
      smap.erase(i);

      // FIXME: change ProgramObject to deregister any signal handlers when the QoreProgram object is deleted
      pf->pgm->deref(xsink);
      delete pf;
      if (xsink->isException())
      {
	 struct sigaction sa;
	 sa.sa_handler = (sig == SIGPIPE ? SIG_IGN : SIG_DFL);
	 sigemptyset(&sa.sa_mask);
	 sa.sa_flags = SA_RESTART;
	 sigaction(sig, &sa, NULL);

	 gate.exit();
	 return;
      }
   }
   
   smap.erase(sig);
   smap[sig] = new PgmFunc(pgm, f);
   gate.exit();

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
   if (gate.enter(xsink))
      return -1;

   m_int_func_t::iterator i = smap.find(sig);
   if (i == smap.end())
   {
      gate.exit();
      return 0;
   }
   
   class PgmFunc *pf = i->second;
   smap.erase(i);
   gate.exit();
   
   struct sigaction sa;
   sa.sa_handler = (sig == SIGPIPE ? SIG_IGN : SIG_DFL);
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = SA_RESTART;
   sigaction(sig, &sa, NULL);

   pf->pgm->deref(xsink);
   delete pf;
   return xsink->isException() ? -1 : 0;
}

class UserFunction *QoreSignalManager::getHandler(int sig, class ExceptionSink *xsink)
{
   if (gate.enter(xsink))
      return NULL;

   m_int_func_t::const_iterator i = smap.find(sig);
   UserFunction *uf;
   if (i == smap.end())
      uf= NULL;
   else
      uf = i->second->f;
   gate.exit();
   return uf;
}

void QoreSignalManager::handleSignals()
{
   if (!sig_raised)
      return;

   ExceptionSink xsink;
   if (gate.enter(&xsink))
      return;

   // check flag again inside the lock
   if (!sig_raised)
   {
      gate.exit();
      return;
   }
   sig_raised = false;
   
   for (int i = 1; i < QORE_SIGNAL_MAX; ++i)
   {
      if (sig_event[i])
      {
	 sig_event[i] = false;
	 m_int_func_t::const_iterator j = smap.find(i);
	 assert(j != smap.end());
	 UserFunction *uf = j->second->f;
	 
	 // create signal number argument
	 class List *l = new List();
	 l->push(new QoreNode((int64)i));
	 class QoreNode *args = new QoreNode(l);
	 uf->eval(args, NULL, &xsink);
	 args->deref(&xsink);
      }
   }
   
   gate.exit();
}
