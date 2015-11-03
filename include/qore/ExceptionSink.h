/*
  ExceptionSink.h

  Qore Programming Language ExceptionSink class definition

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

#ifndef _QORE_EXCEPTIONSINK_H

#define _QORE_EXCEPTIONSINK_H

#include <stdarg.h>
#include <stdio.h>

class QoreException;

//! container for holding Qore-language exception information and also for registering a "thread_exit" call
class ExceptionSink {
   private:
      //! private implementation of the class
      struct qore_es_private *priv;

      DLLLOCAL void insert(QoreException *e);
      DLLLOCAL void clearIntern();

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL ExceptionSink(const ExceptionSink&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL ExceptionSink& operator=(const ExceptionSink&);
      
   public:
      //! creates an empty ExceptionSink object
      DLLEXPORT ExceptionSink();

      //! calls ExceptionSink::defaultExceptionHandler() on all exceptions still present in the object and then deletes the exception list
      DLLEXPORT ~ExceptionSink();

      //! calls ExceptionSink::defaultExceptionHandler() on all exceptions still present in the object and then deletes the exception list
      DLLEXPORT void handleExceptions();

      //! calls ExceptionSink::defaultWarningHandler() on all exceptions still present in the object and then deletes the exception list
      DLLEXPORT void handleWarnings();

      //! returns true if at least one exception is present or thread_exit has been triggered
      DLLEXPORT bool isEvent() const;

      //! returns true if thread_exit has been triggered
      DLLEXPORT bool isThreadExit() const;

      //! returns true if at least one exception is present
      DLLEXPORT bool isException() const;

      //! returns true if at least one exception is present or thread_exit has been triggered
      /** Intended as a alternative to isEvent()
	  @code
	  ExceptionSink xsink;
	  if (xsink) { .. }
	  @endcode
      */
      DLLEXPORT operator bool () const;

      //! appends a Qore-language exception to the list
      /** The AbstractQoreNode pointer returned is always 0; used to simplify error handling code.
	  @param err the exception code string
	  @param fmt the format string for the description for the exception
	  @return always returns 0
       */
      DLLEXPORT class AbstractQoreNode *raiseException(const char *err, const char *fmt, ...);

      //! appends a Qore-language exception to the list, and sets the 'arg' member (this object takes over the reference count of 'arg')
      /** The AbstractQoreNode pointer returned is always 0; used to simplify error handling code.
	  @param err the exception code string
	  @param arg the 'arg' member of the Qore-language exception object; will be dereferenced when the QoreException object is destroyed
	  @param fmt the format string for the description for the exception
	  @return always returns 0
       */
      DLLEXPORT AbstractQoreNode* raiseExceptionArg(const char* err, AbstractQoreNode* arg, const char* fmt, ...);

      //! appends a Qore-language exception to the list; takes owenership of the "desc" argument reference
      /** The AbstractQoreNode pointer returned is always 0; used to simplify error handling code.
	  @param err the exception code string
	  @param desc the description string for the exception; the ExceptionSink argument takes ownership of the reference count
	  @return always returns 0
       */
      DLLEXPORT AbstractQoreNode *raiseException(const char *err, QoreStringNode *desc);

      //! sets the "thread_exit" flag; will cause the current thread to terminate
      DLLEXPORT void raiseThreadExit();

      //! assimilates all entries of the "xs" argument by appending them to the internal list and deletes the "xs" argument
      DLLEXPORT void assimilate(ExceptionSink *xs);

      //! intended to be used to handle out of memory errors FIXME: not yet fully implemented
      DLLEXPORT void outOfMemory();

      //! deletes the exception list immediately
      DLLEXPORT void clear();

      DLLLOCAL void raiseException(QoreException *e);
      DLLLOCAL void raiseException(const class QoreListNode *n);
      DLLLOCAL QoreException *catchException();
      DLLLOCAL void overrideLocation(int sline, int eline, const char *file);
      DLLLOCAL void rethrow(QoreException *old);
      DLLLOCAL void addStackInfo(int type, const char *class_name, const char *code, const char *file, int start_line, int end_line);
      DLLLOCAL void addStackInfo(int type, const char *class_name, const char *code);

      DLLLOCAL static void defaultExceptionHandler(QoreException *e);
      DLLLOCAL static void defaultWarningHandler(QoreException *e);
};


static inline void alreadyDeleted(ExceptionSink *xsink, const char *cmeth)
{
   xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s() cannot be executed because the object has already been deleted", cmeth);
}

static inline void makeAccessDeletedObjectException(ExceptionSink *xsink, const char *mem, const char *cname)
{
   xsink->raiseException("OBJECT-ALREADY-DELETED", "attempt to access member '%s' of an already-deleted object of class '%s'", mem, cname);
}

static inline void makeAccessDeletedObjectException(ExceptionSink *xsink, const char *cname)
{
   xsink->raiseException("OBJECT-ALREADY-DELETED", "attempt to access an already-deleted object of class '%s'", cname);
}

#endif
