/*
  Exception.h

  Qore programming language exception handling support

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

#ifndef _QORE_EXCEPTION_H

#define _QORE_EXCEPTION_H

#include <stdarg.h>
#include <stdio.h>

// exception/callstack entry types
#define ET_SYSTEM     0
#define ET_USER       1

class ExceptionSink {
   private:
      bool thread_exit;
      class Exception *head, *tail;

      inline void insert(class Exception *e);

   public:
      inline ExceptionSink()
      {
         thread_exit = false;
         head = tail = NULL;
      }
      inline ~ExceptionSink();
      inline void handleExceptions();
      inline bool isEvent()
      {
	 return head || thread_exit;
      }
      inline bool isThreadExit()
      {
	 return thread_exit;
      }
      inline bool isException()
      {
	 return head;
      }
      inline void raiseException(char *err, char *fmt, ...);
      inline void raiseException(class Exception *e);
      inline void raiseException(class QoreNode *n);
      inline void rethrow(class Exception *old);
      inline void raiseThreadExit()
      {
	 thread_exit = true;
      }
      inline class Exception *catchException()
      {
	 class Exception *e = head;
	 head = tail = NULL;
	 return e;
      }
      inline void assimilate(class ExceptionSink *xs);
      inline void del(class Exception *e);
};

class Exception {
   protected:
      ~Exception();

   public:
      int type;
      int line;
      char *file;
      class QoreNode *callStack, *err, *desc, *arg;
      class Exception *next;

      inline Exception(char *e, char *fmt, ...);
      inline Exception(char *e, int sline, class QoreString *desc);
      Exception(class QoreNode *l);
      Exception(class Exception *old, class ExceptionSink *xsink);

      inline void del(class ExceptionSink *xsink);
      inline void del();
};

void defaultExceptionHandler(class Exception *e);
class QoreNode *makeExceptionObject(class Exception *e);
/*
inline void make_exception(class ExceptionSink *xsink, class QoreNode *l);
inline void make_exception(ExceptionSink *xsink, Exception *ne);
*/

#include <qore/common.h>
#include <qore/List.h>
#include <qore/thread.h>
#include <qore/QoreNode.h>
#include <qore/QoreString.h>
#include <qore/Object.h>
#include <qore/support.h>

inline ExceptionSink::~ExceptionSink()
{
   handleExceptions();
}

inline void ExceptionSink::handleExceptions()
{
   if (head)
   {
      defaultExceptionHandler(head);
      // delete all exceptions
      while (head)
      {
	 tail = head->next;
	 head->del();
	 head = tail;
      }
   }
}

inline void ExceptionSink::insert(class Exception *e)
{
   // append exception to the list
   if (!head)
      head = e;
   else
      tail->next = e;
   tail = e;
}

inline void ExceptionSink::raiseException(char *err, char *fmt, ...)
{
   class QoreString *desc = new QoreString();

   va_list args;

   while (true)
   {
      va_start(args, fmt);
      int rc = desc->sprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }
   insert(new Exception(err, 0, desc));
}

inline void ExceptionSink::raiseException(class Exception *e)
{
   insert(e);
}

inline void ExceptionSink::raiseException(class QoreNode *n)
{
   insert(new Exception(n));
}

inline void ExceptionSink::rethrow(class Exception *old)
{
   insert(new Exception(old, this));
}

inline void ExceptionSink::assimilate(class ExceptionSink *xs)
{
   if (xs->thread_exit)
   {
      thread_exit = xs->thread_exit;
      xs->thread_exit = false;
   }
   if (xs->tail)
   {
      if (tail)
	 tail->next = xs->head;
      else
	 head = xs->head;
      tail = xs->tail;
   }
   xs->head = xs->tail = NULL;
}

inline void ExceptionSink::del(class Exception *e)
{
   while (e)
   {
      class Exception *n = e->next;
      e->del(this);
      e = n;
   }
}

inline Exception::Exception(char *e, int sline, class QoreString *d)
{
   if (sline)
      line = sline;
   else
      line = get_pgm_counter();
   
   char *f = get_pgm_file();
   file = f ? strdup(f) : NULL;
   callStack = new QoreNode(getCallStack());

   err = new QoreNode(e);
   desc = new QoreNode(d);
   arg = NULL;

   next = NULL;
}

inline Exception::Exception(char *e, char *fmt, ...)
{
   QoreString *str = new QoreString();
   va_list args;

   while (true)
   {
      va_start(args, fmt);
      int rc = str->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }

   type = ET_SYSTEM;
   line = get_pgm_counter();
   char *f = get_pgm_file();
   file = f ? strdup(f) : NULL;
   callStack = new QoreNode(getCallStack());

   err = new QoreNode(e);
   desc = new QoreNode(str);
   arg = NULL;

   next = NULL;
}

inline Exception::~Exception()
{
   if (file)
      free(file);
}

inline void Exception::del(class ExceptionSink *xsink)
{
   if (callStack)
      callStack->deref(xsink);

   if (err)
      err->deref(xsink);
   if (desc)
      desc->deref(xsink);
   if (arg)
      arg->deref(xsink);

   delete this;
}

inline void Exception::del()
{
   class ExceptionSink xsink;

   if (callStack)
      callStack->deref(&xsink);

   if (err)
      err->deref(&xsink);
   if (desc)
      desc->deref(&xsink);
   if (arg)
      arg->deref(&xsink);

   delete this;
}

#endif
