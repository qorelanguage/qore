/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreException.h

  Qore programming language exception handling support

  Copyright 2003 - 2012 David Nichols

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

#include <string>

// exception/callstack entry types
#define ET_SYSTEM     0
#define ET_USER       1

struct QoreExceptionBase {
   int type;
   QoreListNode *callStack;
   AbstractQoreNode *err, *desc, *arg;

   DLLLOCAL QoreExceptionBase(AbstractQoreNode *n_err, AbstractQoreNode *n_desc, AbstractQoreNode *n_arg = 0, int n_type = ET_SYSTEM) 
      : type(n_type), callStack(new QoreListNode), err(n_err), desc(n_desc), arg(n_arg) {
   }

   DLLLOCAL QoreExceptionBase(const QoreExceptionBase &old) : type(old.type), callStack(old.callStack->copy()), 
                                                              err(old.err ? old.err->refSelf() : 0), desc(old.desc ? old.desc->refSelf() : 0), 
                                                              arg(old.arg ? old.arg->refSelf() : 0) {
   }
};

struct QoreExceptionLocation : QoreProgramLineLocation {
   std::string file;

   DLLLOCAL QoreExceptionLocation(int sline, int eline, const char *n_file) : QoreProgramLineLocation(sline, eline), file(n_file ? n_file : "") {
   }

   DLLLOCAL QoreExceptionLocation(const QoreProgramLocation &loc) : QoreProgramLineLocation(loc), file(loc.file ? loc.file : "") {
   }

   DLLLOCAL QoreExceptionLocation(prog_loc_e loc = ParseLocation);

   DLLLOCAL QoreExceptionLocation(const QoreExceptionLocation& old) : QoreProgramLineLocation(old), file(old.file) {
   }
};

class QoreException : public QoreExceptionBase, public QoreExceptionLocation {
   friend class ExceptionSink;
   friend struct qore_es_private;

private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreException& operator=(const QoreException&);

protected:
   QoreException *next;

   DLLLOCAL ~QoreException() {
      assert(!callStack);
      assert(!err);
      assert(!desc);
      assert(!arg);
   }

   DLLLOCAL void addStackInfo(AbstractQoreNode *n);
   DLLLOCAL static QoreHashNode *getStackHash(int type, const char *class_name, const char *code, const char *file, int start_line, int end_line);

public:
   // called for generic exceptions
   DLLLOCAL QoreHashNode *makeExceptionObjectAndDelete(ExceptionSink *xsink);
   DLLLOCAL QoreHashNode *makeExceptionObject();

   // called for runtime exceptions
   DLLLOCAL QoreException(const char *n_err, AbstractQoreNode *n_desc, AbstractQoreNode *n_arg = 0) : QoreExceptionBase(new QoreStringNode(n_err), n_desc, n_arg), QoreExceptionLocation(RunTimeLocation), next(0) {      
   }

   DLLLOCAL QoreException(const QoreException &old) : QoreExceptionBase(old), QoreExceptionLocation(old), next(old.next ? new QoreException(*old.next) : 0) {
   }

   // called for user exceptions
   DLLLOCAL QoreException(const QoreListNode *n) : QoreExceptionBase(0, 0, 0, ET_USER), QoreExceptionLocation(RunTimeLocation), next(0) {
      if (n) {
         err = n->get_referenced_entry(0);
         desc = n->get_referenced_entry(1);
         arg = n->size() > 3 ? n->copyListFrom(2) : n->get_referenced_entry(2);
      }
   }
;

   DLLLOCAL QoreException(const QoreProgramLocation &n_loc, const char *n_err, AbstractQoreNode *n_desc, AbstractQoreNode *n_arg = 0, int n_type = ET_SYSTEM) : QoreExceptionBase(new QoreStringNode(n_err), n_desc, n_arg, n_type), QoreExceptionLocation(n_loc), next(0) {
   }
   
   DLLLOCAL void del(ExceptionSink *xsink);

   DLLLOCAL QoreException *rethrow() {
      QoreException *e = new QoreException(*this);

      // insert current position as a rethrow entry in the new callstack
      QoreListNode *l = e->callStack;
      const char *fn = 0;
      QoreHashNode *n = reinterpret_cast<QoreHashNode *>(l->retrieve_entry(0));
      // get function name
      if (n) {
         QoreStringNode *func = reinterpret_cast<QoreStringNode *>(n->getKeyValue("function"));
         fn = func->getBuffer();
      }
      if (!fn)
         fn = "<unknown>";
   
      int sline, eline;
      const char *cf = get_pgm_counter(sline, eline);
      QoreHashNode *h = getStackHash(CT_RETHROW, 0, fn, cf, sline, eline);
      l->insert(h);

      return e;
   }
};

class ParseException : public QoreException {
public:
   // called for parse exceptions
   DLLLOCAL ParseException(const char *err, QoreStringNode *desc) : QoreException(QoreProgramLocation(ParseLocation), err, desc) {
   }

   // called for parse exceptions
   DLLLOCAL ParseException(const QoreProgramLocation &loc, const char *err, QoreStringNode *desc) : QoreException(loc, err, desc) {
   }

   // called for parse exceptions
   DLLLOCAL ParseException(int s_line, int e_line, const char *file, const char *err, QoreStringNode *desc) : QoreException(QoreProgramLocation(s_line, e_line, file), err, desc) {
   }
};

struct qore_es_private {
   bool thread_exit;
   QoreException *head, *tail;

   DLLLOCAL qore_es_private() {
      thread_exit = false;
      head = tail = 0;
   }
   
   DLLLOCAL ~qore_es_private() {
   }

   DLLLOCAL void clearIntern() {
      // delete all exceptions
      ExceptionSink xs;
      if (head) {
         head->del(&xs);
         head = tail = 0;
      }
   }

   DLLLOCAL void insert(QoreException *e) {
      // append exception to the list
      if (!head)
         head = e;
      else
         tail->next = e;
      tail = e;
   }
};

class ParseExceptionSink {
protected:
   ExceptionSink xsink;

public:
   DLLLOCAL ~ParseExceptionSink() {
      if (xsink)
         getProgram()->addParseException(xsink);
   }

   DLLLOCAL ExceptionSink *operator*() {
      return &xsink;
   }
};

#endif
