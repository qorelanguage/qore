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
m_int_func_t QoreSignalManager::smap;
bool QoreSignalManager::sig_event[QORE_SIGNAL_MAX];
bool QoreSignalManager::sig_raised = false;

extern "C" void sighandler(int sig) //, siginfo_t *info, ucontext_t *uap)
{
   QoreSignalManager::sig_raised = true;
   QoreSignalManager::sig_event[sig] = true;
}

// must be called in the signal lock
PgmFunc::PgmFunc(int n_sig, class QoreProgram *n_pgm, class UserFunction *n_f) : sig(n_sig), pgm(n_pgm), f(n_f)
{
   pgm->registerSignalHandler(sig);
}

// must be called in the signal lock
PgmFunc::~PgmFunc()
{
   if (pgm)
      pgm->deregisterSignalHandler(sig);
}

void PgmFunc::runHandler(class ExceptionSink *xsink)
{
   // create signal number argument
   class List *l = new List();
   l->push(new QoreNode((int64)sig));
   class QoreNode *args = new QoreNode(l);
   pushProgram(pgm);
   f->eval(args, NULL, xsink);
   popProgram();
   args->deref(xsink);
}

QoreSignalManager::QoreSignalManager() 
{
   for (int i = 0; i < QORE_SIGNAL_MAX; ++i)
      sig_event[i] = 0;
}

QoreSignalManager::~QoreSignalManager()
{
   assert(smap.empty());
}

void QoreSignalManager::setHandler(int sig, class QoreProgram *pgm, class UserFunction *f)
{
   bool already_set = false;

   SafeLocker sl(&mutex);

   m_int_func_t::iterator i = smap.find(sig);
   if (i != smap.end())
   {
      //printd(5, "replacing handler for signal %d\n", sig);
      already_set = true;
      class PgmFunc *pf = i->second;
      smap.erase(i);
      delete pf;
   }
   
   //printd(5, "setting handler for signal %d, pgm=%08p\n", sig, pgm);
   smap[sig] = new PgmFunc(sig, pgm, f);
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

int QoreSignalManager::removeHandlerFromProgram(int sig)
{
   SafeLocker sl(&mutex);
   
   m_int_func_t::iterator i = smap.find(sig);
   if (i == smap.end())
      return 0;
   
   //printd(5, "removing handler for signal %d\n", sig);
   
   class PgmFunc *pf = i->second;
   smap.erase(i);
   
   pf->pgm = NULL;
   // does not have to be called in the signal lock because the program object will not be resynchronized
   delete pf;
   sl.unlock();
   
   struct sigaction sa;
   sa.sa_handler = (sig == SIGPIPE ? SIG_IGN : SIG_DFL);
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = SA_RESTART;
   sigaction(sig, &sa, NULL);
      
   return 0;
}

int QoreSignalManager::removeHandler(int sig)
{
   AutoLocker al(&mutex);

   m_int_func_t::iterator i = smap.find(sig);
   if (i == smap.end())
      return 0;

   //printd(5, "removing handler for signal %d\n", sig);

   class PgmFunc *pf = i->second;
   smap.erase(i);

   struct sigaction sa;
   sa.sa_handler = (sig == SIGPIPE ? SIG_IGN : SIG_DFL);
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = SA_RESTART;
   sigaction(sig, &sa, NULL);

   // must be called in the signal lock
   delete pf;

   return 0;
}

class UserFunction *QoreSignalManager::getHandler(int sig)
{
   AutoLocker al(&mutex);

   m_int_func_t::const_iterator i = smap.find(sig);
   if (i == smap.end())
      return NULL;

   return i->second->f;
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
	 m_int_func_t::const_iterator j = smap.find(i);
	 // ignore signal if handler was removed
	 if (j == smap.end())
	    continue;

	 sl.unlock();
	 j->second->runHandler(&xsink);
	 sl.lock();
      }
   }   
}
