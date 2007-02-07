/*
  Exception.h

  Qore programming language exception handling support

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
      DLLEXPORT bool isEvent() const;
      DLLEXPORT bool isThreadExit() const;
      DLLEXPORT bool isException() const;
      // Intended as a alternative to isException():
      // ExceptionSink xsink;
      // if (xsink) { .. }
      DLLEXPORT operator bool () const;
      // The QoreNode* returned value is always NULL. Used to simplify error handling code.
      DLLEXPORT class QoreNode *raiseException(char *err, char *fmt, ...);
      // Raise exception with additional argument (the 'arg' member). Always returns 0.
      DLLEXPORT QoreNode* raiseExceptionArg(char* err, QoreNode* arg, char* fmt, ...);
      // returns NULL, takes owenership of the "desc" argument
      DLLEXPORT QoreNode *raiseException(char *err, class QoreString *desc);
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
      DLLLOCAL void overrideLocation(int sline, int eline, char *file);
};

class Exception {
   friend class ExceptionSink;

   private:
   
   protected:
      int type;
      int start_line, end_line;
      char *file;
      class QoreNode *callStack, *err, *desc, *arg;
      class Exception *next;

      ~Exception();

   public:
      // called for generic exceptions
      DLLEXPORT class QoreNode *makeExceptionObjectAndDelete(class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *makeExceptionObject();

      // called for runtime exceptions
      DLLLOCAL Exception(char *err, class QoreString *desc);
      // called for rethrow
      DLLLOCAL Exception(class Exception *old, class ExceptionSink *xsink);
      // called for user exceptions
      DLLLOCAL Exception(class QoreNode *n);
      // for derived classes
      DLLLOCAL Exception();
      DLLLOCAL void del(class ExceptionSink *xsink);
};

class ParseException : public Exception
{
   public:
      // called for parse exceptions
      DLLLOCAL ParseException(char *err, class QoreString *desc);
      // called for parse exceptions
      DLLLOCAL ParseException(int s_line, int e_line, char *err, class QoreString *desc);
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
