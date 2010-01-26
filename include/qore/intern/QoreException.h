/*
  QoreException.h

  Qore programming language exception handling support

  Copyright 2003 - 2010 David Nichols

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

// exception/callstack entry types
#define ET_SYSTEM     0
#define ET_USER       1

class QoreException {
   friend class ExceptionSink;

   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreException(const QoreException&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreException& operator=(const QoreException&);

   protected:
      int type;
      int start_line, end_line;
      char *file;
      QoreListNode *callStack;
      AbstractQoreNode *err, *desc, *arg;
      QoreException *next;

      DLLLOCAL ~QoreException();
      DLLLOCAL void addStackInfo(AbstractQoreNode *n);
      DLLLOCAL static QoreHashNode *getStackHash(int type, const char *class_name, const char *code, const char *file, int start_line, int end_line);

   public:
      // called for generic exceptions
      DLLLOCAL QoreHashNode *makeExceptionObjectAndDelete(ExceptionSink *xsink);
      DLLLOCAL QoreHashNode *makeExceptionObject();

      // called for runtime exceptions
      DLLLOCAL QoreException(const char *err, QoreStringNode *desc);
      // called for rethrow
      DLLLOCAL QoreException(QoreException *old, ExceptionSink *xsink);
      // called for user exceptions
      DLLLOCAL QoreException(const QoreListNode *n);
      // for derived classes
      DLLLOCAL QoreException();
      DLLLOCAL void del(ExceptionSink *xsink);
};

class ParseException : public QoreException {
   public:
      // called for parse exceptions
      DLLLOCAL ParseException(const char *err, QoreStringNode *desc);

      // called for parse exceptions
      DLLLOCAL ParseException(int s_line, int e_line, const char *err, QoreStringNode *desc);

      // called for parse exceptions
      DLLLOCAL ParseException(int s_line, int e_line, const char *file, const char *err, QoreStringNode *desc);
};

#endif
