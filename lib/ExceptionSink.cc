/*
  QoreException.cc

  Qore programming language exception handling support

  Copyright 2003 - 2009 David Nichols

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

struct qore_es_private {
      bool thread_exit;
      class QoreException *head, *tail;

      DLLLOCAL qore_es_private()
      {
	 thread_exit = false;
	 head = tail = 0;
      }

      DLLLOCAL ~qore_es_private()
      {
      }
};

ExceptionSink::ExceptionSink() : priv(new qore_es_private)
{
}

ExceptionSink::~ExceptionSink()
{
   handleExceptions();
   delete priv;
}

void ExceptionSink::raiseThreadExit()
{
   priv->thread_exit = true;
}

bool ExceptionSink::isEvent() const
{
   return priv->head || priv->thread_exit;
}

bool ExceptionSink::isThreadExit() const
{
   return priv->thread_exit;
}

bool ExceptionSink::isException() const
{
   return priv->head;
}

// creates a stack trace node and adds it to all exceptions in this sink
void ExceptionSink::addStackInfo(int type, const char *class_name, const char *code, const char *file, int start_line, int end_line)
{
   assert(priv->head);
   QoreHashNode *n = QoreException::getStackHash(type, class_name, code, file, start_line, end_line);

   class QoreException *w = priv->head;
   while (w)
   {
      w->addStackInfo(n);
      w = w->next;
      if (w)
	 n->ref();
   }   
}

// creates a stack trace node and adds it to all exceptions in this sink
void ExceptionSink::addStackInfo(int type, const char *class_name, const char *code)
{
   assert(priv->head);

   const char *file = get_pgm_file();
   int start_line, end_line;
   get_pgm_counter(start_line, end_line);
   QoreHashNode *n = QoreException::getStackHash(type, class_name, code, file, start_line, end_line);

   class QoreException *w = priv->head;
   while (w)
   {
      w->addStackInfo(n);
      w = w->next;
      if (w)
	 n->ref();
   }   
}

// Intended as a alternative to isException():
// ExceptionSink xsink;
// if (xsink) { .. }
ExceptionSink::operator bool () const
{
   return priv->head || priv->thread_exit;
}

void ExceptionSink::overrideLocation(int sline, int eline, const char *file)
{
   class QoreException *w = priv->head;
   while (w)
   {
      w->start_line = sline;
      w->end_line = eline;
      if (w->file)
	 free(w->file);
      w->file = file ? strdup(file) : 0;
      w = w->next;
   }
}

class QoreException *ExceptionSink::catchException()
{
   class QoreException *e = priv->head;
   priv->head = priv->tail = 0;
   return e;
}

void ExceptionSink::handleExceptions()
{
   if (priv->head)
   {
      defaultExceptionHandler(priv->head);
      clear();
   }
   else
      priv->thread_exit = false;
}

void ExceptionSink::handleWarnings()
{
   if (priv->head)
   {
      defaultWarningHandler(priv->head);
      clear();
   }
}

void ExceptionSink::clearIntern()
{
   // delete all exceptions
   ExceptionSink xs;
   if (priv->head)
   {
      priv->head->del(&xs);
      priv->head = priv->tail = 0;
   }
}

void ExceptionSink::clear()
{
   clearIntern();
   priv->head = priv->tail = 0;
   priv->thread_exit = false;
}

void ExceptionSink::insert(class QoreException *e)
{
   // append exception to the list
   if (!priv->head)
      priv->head = e;
   else
      priv->tail->next = e;
   priv->tail = e;
}

AbstractQoreNode* ExceptionSink::raiseException(const char *err, const char *fmt, ...)
{
   QoreStringNode *desc = new QoreStringNode();
   
   va_list args;
   
   while (true)
   {
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }
   printd(5, "ExceptionSink::raiseException(%s, %s)\n", err, desc->getBuffer());
   insert(new QoreException(err, desc));
   return 0;
}

// returns 0, takes ownership of the "desc" argument
AbstractQoreNode *ExceptionSink::raiseException(const char *err, QoreStringNode *desc)
{
   printd(5, "ExceptionSink::raiseException(%s, %s)\n", err, desc->getBuffer());
   insert(new QoreException(err, desc));
   return 0;
}

AbstractQoreNode* ExceptionSink::raiseExceptionArg(const char* err, AbstractQoreNode* arg, const char* fmt, ...)
{
   QoreStringNode *desc = new QoreStringNode();
   
   va_list args;
   
   while (true)
   {
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
   printd(5, "ExceptionSink::raiseExceptionArg(%s, %s)\n", err, desc->getBuffer());
   QoreException* exc = new QoreException(err, desc);
   exc->arg = arg;
   insert(exc);
   return 0;
}

void ExceptionSink::raiseException(QoreException *e)
{
   insert(e);
}

void ExceptionSink::raiseException(const QoreListNode *n)
{
   insert(new QoreException(n));
}

void ExceptionSink::rethrow(class QoreException *old)
{
   insert(new QoreException(old, this));
}

void ExceptionSink::assimilate(ExceptionSink *xs)
{
   if (xs->priv->thread_exit)
   {
      priv->thread_exit = xs->priv->thread_exit;
      xs->priv->thread_exit = false;
   }
   if (xs->priv->tail)
   {
      if (priv->tail)
	 priv->tail->next = xs->priv->head;
      else
	 priv->head = xs->priv->head;
      priv->tail = xs->priv->tail;
   }
   xs->priv->head = xs->priv->tail = 0;
}

void ExceptionSink::outOfMemory()
{
#ifdef QORE_OOM
   // get pre-allocated out of memory exception for this thread
   class QoreException *ex = getOutOfMemoryException();
   // if it's already been used then return
   if (!ex)
      return;
   // set line and file in exception
   get_pgm_counter(ex->start_line, ex->end_line);
   const char *f = get_pgm_file();
   ex->file = f ? strdup(f) : 0;
   // there is no callstack in an out-of-memory exception
   // add exception to list
   insert(ex);
#else
   printf("OUT OF MEMORY: aborting\n");
   exit(1);
#endif
}
