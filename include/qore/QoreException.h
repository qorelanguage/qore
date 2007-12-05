/*
  QoreException.h

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

#ifndef _QORE_QOREEXCEPTION_H

#define _QORE_QOREEXCEPTION_H

#include <stdarg.h>
#include <stdio.h>

// exception/callstack entry types
#define ET_SYSTEM     0
#define ET_USER       1

class ExceptionSink {
   private:
      bool thread_exit;
      class QoreException *head, *tail;

      DLLLOCAL void insert(class QoreException *e);
      DLLLOCAL void clearIntern();
      
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
      DLLEXPORT class QoreNode *raiseException(const char *err, const char *fmt, ...);
      // Raise exception with additional argument (the 'arg' member). Always returns 0.
      DLLEXPORT QoreNode* raiseExceptionArg(const char* err, QoreNode* arg, const char* fmt, ...);
      // returns NULL, takes owenership of the "desc" argument
      DLLEXPORT QoreNode *raiseException(const char *err, class QoreString *desc);
      DLLEXPORT void rethrow(class QoreException *old);
      DLLEXPORT void raiseThreadExit();
      DLLEXPORT void assimilate(class ExceptionSink *xs);
      DLLEXPORT void outOfMemory();
      DLLEXPORT void clear();
      DLLEXPORT void addStackInfo(int type, const char *class_name, const char *code, const char *file, int start_line, int end_line);
      DLLEXPORT void addStackInfo(int type, const char *class_name, const char *code);

      DLLLOCAL void raiseException(class QoreException *e);
      DLLLOCAL void raiseException(class QoreNode *n);
      DLLLOCAL class QoreException *catchException();
      DLLLOCAL void overrideLocation(int sline, int eline, const char *file);

      DLLLOCAL static void defaultExceptionHandler(class QoreException *e);
      DLLLOCAL static void defaultWarningHandler(class QoreException *e);
};

class QoreException {
   friend class ExceptionSink;

   private:
   
   protected:
      int type;
      int start_line, end_line;
      char *file;
      class QoreNode *callStack, *err, *desc, *arg;
      class QoreException *next;

      DLLLOCAL ~QoreException();
      DLLLOCAL void addStackInfo(class QoreNode *n);
      DLLLOCAL static class QoreHash *getStackHash(int type, const char *class_name, const char *code, const char *file, int start_line, int end_line);
      
   public:
      // called for generic exceptions
      DLLEXPORT class QoreNode *makeExceptionObjectAndDelete(class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *makeExceptionObject();

      // called for runtime exceptions
      DLLLOCAL QoreException(const char *err, class QoreString *desc);
      // called for rethrow
      DLLLOCAL QoreException(class QoreException *old, class ExceptionSink *xsink);
      // called for user exceptions
      DLLLOCAL QoreException(class QoreNode *n);
      // for derived classes
      DLLLOCAL QoreException();
      DLLLOCAL void del(class ExceptionSink *xsink);
};

class ParseException : public QoreException
{
   public:
      // called for parse exceptions
      DLLLOCAL ParseException(const char *err, class QoreString *desc);
      // called for parse exceptions
      DLLLOCAL ParseException(int s_line, int e_line, const char *err, class QoreString *desc);
};

static inline void alreadyDeleted(class ExceptionSink *xsink, const char *cmeth)
{
   xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s() cannot be executed because the object has already been deleted", cmeth);
}

static inline void makeAccessDeletedObjectException(class ExceptionSink *xsink, const char *mem, const char *cname)
{
   xsink->raiseException("OBJECT-ALREADY-DELETED", "attempt to access member '%s' of an already-deleted object of class '%s'", mem, cname);
}

static inline void makeAccessDeletedObjectException(class ExceptionSink *xsink, const char *cname)
{
   xsink->raiseException("OBJECT-ALREADY-DELETED", "attempt to an already-deleted object of class '%s'", cname);
}

#endif
