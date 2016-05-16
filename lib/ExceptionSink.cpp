/*
  QoreException.cpp

  Qore programming language exception handling support

  Copyright (C) 2003 - 2015 David Nichols

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

#include <qore/Qore.h>

ExceptionSink::ExceptionSink() : priv(new qore_es_private) {
}

ExceptionSink::~ExceptionSink() {
   handleExceptions();
   delete priv;
}

void ExceptionSink::raiseThreadExit() {
   priv->thread_exit = true;
}

bool ExceptionSink::isEvent() const {
   return priv->head || priv->thread_exit;
}

bool ExceptionSink::isThreadExit() const {
   return priv->thread_exit;
}

bool ExceptionSink::isException() const {
   return priv->head;
}

// Intended as a alternative to isException():
// ExceptionSink xsink;
// if (xsink) { .. }
ExceptionSink::operator bool () const {
   return qore_check_this(this) && (priv->head || priv->thread_exit);
}

void ExceptionSink::overrideLocation(const QoreProgramLocation& loc) {
   QoreException *w = priv->head;
   while (w) {
      w->set(loc);
      w = w->next;
   }
}

QoreException *ExceptionSink::catchException() {
   QoreException *e = priv->head;
   priv->head = priv->tail = 0;
   return e;
}

void ExceptionSink::handleExceptions() {
   if (priv->head) {
      defaultExceptionHandler(priv->head);
      clear();
   }
   else
      priv->thread_exit = false;
}

void ExceptionSink::handleWarnings() {
   if (priv->head) {
      defaultWarningHandler(priv->head);
      clear();
   }
}

void ExceptionSink::clear() {
   priv->clearIntern();
   priv->head = priv->tail = 0;
   priv->thread_exit = false;
}

AbstractQoreNode* ExceptionSink::raiseException(const char *err, const char *fmt, ...) {
   QoreStringNode *desc = new QoreStringNode;
   
   va_list args;
   
   while (true) {
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }
   printd(5, "ExceptionSink::raiseException(%s, %s)\n", err, desc->getBuffer());
   priv->insert(new QoreException(err, desc));
   return 0;
}

AbstractQoreNode* ExceptionSink::raiseErrnoException(const char *err, int en, QoreStringNode* desc) {
   // append strerror(en) to description
   desc->concat(": ");
   q_strerror(*desc, en);

   printd(5, "ExceptionSink::raiseException(%s, %s)\n", err, desc->getBuffer());
   priv->insert(new QoreException(err, desc, new QoreBigIntNode(en)));
   return 0;
}

AbstractQoreNode* ExceptionSink::raiseErrnoException(const char *err, int en, const char *fmt, ...) {
   QoreStringNode *desc = new QoreStringNode;
   
   va_list args;
   
   while (true) {
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }

   return raiseErrnoException(err, en, desc);
}

// returns 0, takes ownership of the "desc" argument
AbstractQoreNode *ExceptionSink::raiseException(const char *err, QoreStringNode *desc) {
   printd(5, "ExceptionSink::raiseException(%s, %s)\n", err, desc->getBuffer());
   priv->insert(new QoreException(err, desc));
   return 0;
}

AbstractQoreNode* ExceptionSink::raiseExceptionArg(const char* err, AbstractQoreNode* arg, QoreStringNode *desc) {
   printd(5, "ExceptionSink::raiseExceptionArg(%s, %s)\n", err, desc->getBuffer());
   QoreException* exc = new QoreException(err, desc);
   exc->arg = arg;
   priv->insert(exc);
   return 0;
}

AbstractQoreNode* ExceptionSink::raiseExceptionArg(const char* err, AbstractQoreNode* arg, const char* fmt, ...) {
   QoreStringNode *desc = new QoreStringNode;
   
   va_list args;
   
   while (true) {
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
   printd(5, "ExceptionSink::raiseExceptionArg(%s, %s)\n", err, desc->getBuffer());
   QoreException* exc = new QoreException(err, desc);
   exc->arg = arg;
   priv->insert(exc);
   return 0;
}

void ExceptionSink::raiseException(QoreException *e) {
   priv->insert(e);
}

void ExceptionSink::raiseException(const QoreListNode *n) {
   priv->insert(new QoreException(n));
}

void ExceptionSink::raiseException(const QoreProgramLocation &loc, const char *err, AbstractQoreNode *arg, AbstractQoreNode *desc) {
   printd(5, "ExceptionSink::raiseExceptionArg(%s, %s)\n", err, get_node_type(desc) == NT_STRING ? reinterpret_cast<QoreStringNode *>(desc)->getBuffer() : get_type_name(desc));
   priv->insert(new QoreException(loc, err, desc, arg));
}

void ExceptionSink::raiseException(const QoreProgramLocation &loc, const char *err, AbstractQoreNode *arg, const char *fmt, ...) {
   QoreStringNode *desc = new QoreStringNode;

   va_list args;

   while (true) {
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }

   raiseException(loc, err, arg, desc);
}

void ExceptionSink::rethrow(QoreException *old) {
   priv->insert(old->rethrow());
}

void ExceptionSink::assimilate(ExceptionSink* xs) {
   assimilate(*xs);
   delete xs;
}

void ExceptionSink::assimilate(ExceptionSink& xs) {
   if (xs.priv->thread_exit) {
      priv->thread_exit = xs.priv->thread_exit;
      xs.priv->thread_exit = false;
   }
   if (xs.priv->tail) {
      if (priv->tail)
	 priv->tail->next = xs.priv->head;
      else
	 priv->head = xs.priv->head;
      priv->tail = xs.priv->tail;
   }
   xs.priv->head = xs.priv->tail = 0;
}

void ExceptionSink::outOfMemory() {
#ifdef QORE_OOM
   // get pre-allocated out of memory exception for this thread
   QoreException* ex = getOutOfMemoryException();
   // if it's already been used then return
   if (!ex)
      return;
   ex->set(QoreProgramLocation(RuntimeLocation));
   // there is no callstack in an out-of-memory exception
   // add exception to list
   priv->insert(ex);
#else
   printf("OUT OF MEMORY: aborting\n");
   exit(1);
#endif
}
