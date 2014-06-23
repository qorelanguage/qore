/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreException.h

  Qore programming language exception handling support

  Copyright (C) 2003 - 2014 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
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

   DLLLOCAL QoreExceptionBase(const QoreExceptionBase &old) :
               type(old.type), callStack(old.callStack->copy()),
               err(old.err ? old.err->refSelf() : 0), desc(old.desc ? old.desc->refSelf() : 0),
               arg(old.arg ? old.arg->refSelf() : 0) {
   }
};

struct QoreExceptionLocation : QoreProgramLineLocation {
   std::string file;
   std::string source;
   int offset;

   DLLLOCAL QoreExceptionLocation(const QoreProgramLocation &loc) : QoreProgramLineLocation(loc),
         file(loc.file ? loc.file : ""), source(loc.source ? loc.source : ""), offset(loc.offset) {
   }

   DLLLOCAL QoreExceptionLocation(const QoreExceptionLocation& old) : QoreProgramLineLocation(old),
         file(old.file), source(old.source), offset(old.offset) {
   }

   DLLLOCAL void set(const QoreProgramLocation& loc) {
      start_line = loc.start_line;
      end_line = loc.end_line;
      file = loc.file ? loc.file : "";
      source = loc.source ? loc.source : "";
      offset = loc.offset;
   }
};

class QoreException : public QoreExceptionBase, public QoreExceptionLocation {
   friend class ExceptionSink;
   friend struct qore_es_private;

private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreException& operator=(const QoreException&);

protected:
   DLLLOCAL ~QoreException() {
      assert(!callStack);
      assert(!err);
      assert(!desc);
      assert(!arg);
   }

   DLLLOCAL void addStackInfo(AbstractQoreNode *n);
   DLLLOCAL static QoreHashNode *getStackHash(int type, const char *class_name, const char *code, const QoreProgramLocation& loc);

public:
   QoreException *next;

   // called for generic exceptions
   DLLLOCAL QoreHashNode *makeExceptionObjectAndDelete(ExceptionSink *xsink);
   DLLLOCAL QoreHashNode *makeExceptionObject();

   // called for runtime exceptions
   DLLLOCAL QoreException(const char *n_err, AbstractQoreNode *n_desc, AbstractQoreNode *n_arg = 0) : QoreExceptionBase(new QoreStringNode(n_err), n_desc, n_arg), QoreExceptionLocation(QoreProgramLocation(RunTimeLocation)), next(0) {
   }

   DLLLOCAL QoreException(const QoreException &old) : QoreExceptionBase(old), QoreExceptionLocation(old), next(old.next ? new QoreException(*old.next) : 0) {
   }

   // called for user exceptions
   DLLLOCAL QoreException(const QoreListNode *n) : QoreExceptionBase(0, 0, 0, ET_USER), QoreExceptionLocation(QoreProgramLocation(RunTimeLocation)), next(0) {
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
   
      QoreHashNode *h = getStackHash(CT_RETHROW, 0, fn, get_runtime_location());
      l->insert(h);

      return e;
   }
};

class ParseException : public QoreException {
public:
   // called for parse exceptions
   DLLLOCAL ParseException(const char* err, QoreStringNode* desc) : QoreException(QoreProgramLocation(ParseLocation), err, desc) {
   }

   // called for parse exceptions
   DLLLOCAL ParseException(const QoreProgramLocation& loc, const char* err, QoreStringNode* desc) : QoreException(loc, err, desc) {
   }
};

struct qore_es_private {
   bool thread_exit;
   QoreException* head, * tail;

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

   DLLLOCAL void appendListIntern(QoreString& str) const {
      QoreException* w = head;
      while (w) {
	 QoreStringNodeValueHelper err(w->err);
	 QoreStringNodeValueHelper desc(w->desc);

	 str.concat(" * ");
	 if (!w->file.empty())
	    str.sprintf("%s:", w->file.c_str());
	 if (w->start_line) {
	    str.sprintf("%d", w->start_line);
            if (w->end_line && w->end_line != w->start_line)
               str.sprintf("-%d", w->end_line);
            str.concat(": ");
         }
	 str.sprintf("%s: %s", err->getBuffer(), desc->getBuffer());
         if (w != tail)
            str.concat('\n');

	 w = w->next;
      }
   }

   // creates a stack trace node and adds it to all exceptions in this sink
   DLLLOCAL void addStackInfo(int type, const char *class_name, const char *code, const QoreProgramLocation& loc) {
      assert(head);
      QoreHashNode* n = QoreException::getStackHash(type, class_name, code, loc);

      QoreException *w = head;
      while (w) {
         w->addStackInfo(n);
         w = w->next;
         if (w)
            n->ref();
      }
   }

   DLLLOCAL static void addStackInfo(ExceptionSink& xsink, int type, const char* class_name, const char* code, const QoreProgramLocation& loc = QoreProgramLocation(ParseLocation)) {
      xsink.priv->addStackInfo(type, class_name, code, loc);
   }

   DLLLOCAL static void appendList(ExceptionSink& xsink, QoreString& str) {
      xsink.priv->appendListIntern(str);
   }
};

class ParseExceptionSink {
protected:
   ExceptionSink xsink;

public:
   DLLLOCAL ~ParseExceptionSink();

   DLLLOCAL ExceptionSink *operator*() {
      return &xsink;
   }
};

#endif
