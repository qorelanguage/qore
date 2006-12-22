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

      DLLLOCAL inline void insert(class Exception *e);
      DLLLOCAL inline void clearIntern();

   public:
      DLLEXPORT ExceptionSink();
      DLLEXPORT ~ExceptionSink();
      DLLEXPORT void handleExceptions();
      DLLEXPORT void handleWarnings();
      DLLEXPORT bool isEvent() const
      {
	 return head || thread_exit;
      }
      DLLEXPORT bool isThreadExit() const
      {
	 return thread_exit;
      }
      DLLEXPORT bool isException() const
      {
	 return head;
      }
      // Intended as a alternative to isException():
      // ExceptionSink xsink;
      // if (xsink) { .. }
      DLLEXPORT operator bool () const
      {
         return head;
      }
      // The QoreNode* returned value is always NULL. Used to simplify error handling code.
      DLLEXPORT class QoreNode *raiseException(char *err, char *fmt, ...);
      // Raise exception with additional argument (the 'arg' member). Always returns 0.
      DLLEXPORT QoreNode* raiseExceptionArg(char* err, QoreNode* arg, char* fmt, ...);
      DLLEXPORT void rethrow(class Exception *old);
      DLLEXPORT void raiseThreadExit();
      DLLEXPORT void assimilate(class ExceptionSink *xs);
      DLLEXPORT void outOfMemory();
      DLLEXPORT void clear();

      DLLLOCAL void raiseException(class Exception *e);
      DLLLOCAL void raiseException(class QoreNode *n);
      DLLLOCAL class Exception *catchException();
      DLLLOCAL static void defaultExceptionHandler(class Exception *e);
      DLLLOCAL static void defaultWarningHandler(class Exception *e);
};

class Exception {
   friend class ExceptionSink;

   private:
      int type;
      int line;
      char *file;
      class QoreNode *callStack, *err, *desc, *arg;
      class Exception *next;
   
   protected:
      ~Exception();

   public:
      DLLEXPORT Exception(char *e, char *fmt, ...);
      DLLEXPORT Exception(char *e, class QoreString *desc);
      // FIXME: remove this function when parsing line number big is fixed
      DLLEXPORT Exception(char *e, int sline, class QoreString *desc);
      DLLEXPORT Exception(class Exception *old, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *makeExceptionObjectAndDelete(class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *makeExceptionObject();

      DLLLOCAL Exception(class QoreNode *l);
      DLLLOCAL void del(class ExceptionSink *xsink);
};

static inline void alreadyDeleted(class ExceptionSink *xsink, char *cmeth)
{
   xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s() cannot be executed because the object has already been deleted", cmeth);
}

static inline void makeAccessDeletedObjectException(class ExceptionSink *xsink, char *mem, char *cname)
{
   xsink->raiseException("OBJECT-ALREADY-DELETED", "attempt to access member '%s' of an already-deleted object of class '%s'", mem, cname);
}

static inline void makeAccessDeletedObjectException(class ExceptionSink *xsink, char *cname)
{
   xsink->raiseException("OBJECT-ALREADY-DELETED", "attempt to an already-deleted object of class '%s'", cname);
}

#endif
