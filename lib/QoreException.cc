/*
  QoreException.cc

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

#include <qore/Qore.h>
#include <qore/intern/CallStack.h>

#include <qore/safe_dslist>

#include <assert.h>

struct qore_ex_private {
      int type;
      int start_line, end_line;
      char *file;
      QoreListNode *callStack;
      QoreNode *err, *desc, *arg;
      class QoreException *next;

      DLLLOCAL qore_ex_private()
      {
      }

      DLLLOCAL ~qore_ex_private()
      {
	 if (file)
	    free(file);
	 assert(!callStack);
	 assert(!err);
	 assert(!desc);
	 assert(!arg);
      }
};

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

      DLLLOCAL void assimilate(qore_es_private *xs)
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
   class QoreHashNode *n = QoreException::getStackHash(type, class_name, code, file, start_line, end_line);

   class QoreException *w = priv->head;
   while (w)
   {
      w->addStackInfo(n);
      w = w->priv->next;
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
   class QoreHashNode *n = QoreException::getStackHash(type, class_name, code, file, start_line, end_line);

   class QoreException *w = priv->head;
   while (w)
   {
      w->addStackInfo(n);
      w = w->priv->next;
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
      w->priv->start_line = sline;
      w->priv->end_line = eline;
      if (w->priv->file)
	 free(w->priv->file);
      w->priv->file = file ? strdup(file) : NULL;
      w = w->priv->next;
   }
}

class QoreException *ExceptionSink::catchException()
{
   class QoreException *e = priv->head;
   priv->head = priv->tail = NULL;
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
      priv->head = priv->tail = NULL;
   }
}

void ExceptionSink::clear()
{
   clearIntern();
   priv->head = priv->tail = NULL;
   priv->thread_exit = false;
}

void ExceptionSink::insert(class QoreException *e)
{
   // append exception to the list
   if (!priv->head)
      priv->head = e;
   else
      priv->tail->priv->next = e;
   priv->tail = e;
}

QoreNode* ExceptionSink::raiseException(const char *err, const char *fmt, ...)
{
   class QoreStringNode *desc = new QoreStringNode();
   
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
   return NULL;
}

// returns NULL, takes ownership of the "desc" argument
QoreNode *ExceptionSink::raiseException(const char *err, QoreStringNode *desc)
{
   printd(5, "ExceptionSink::raiseException(%s, %s)\n", err, desc->getBuffer());
   insert(new QoreException(err, desc));
   return NULL;
}

QoreNode* ExceptionSink::raiseExceptionArg(const char* err, QoreNode* arg, const char* fmt, ...)
{
   class QoreStringNode *desc = new QoreStringNode();
   
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
   exc->priv->arg = arg;
   insert(exc);
   return NULL;
}

void ExceptionSink::raiseException(QoreException *e)
{
   insert(e);
}

void ExceptionSink::raiseException(QoreListNode *n)
{
   insert(new QoreException(n));
}

void ExceptionSink::rethrow(class QoreException *old)
{
   insert(new QoreException(old, this));
}

void ExceptionSink::assimilate(class ExceptionSink *xs)
{
   if (xs->priv->thread_exit)
   {
      priv->thread_exit = xs->priv->thread_exit;
      xs->priv->thread_exit = false;
   }
   if (xs->priv->tail)
   {
      if (priv->tail)
	 priv->tail->priv->next = xs->priv->head;
      else
	 priv->head = xs->priv->head;
      priv->tail = xs->priv->tail;
   }
   xs->priv->head = xs->priv->tail = NULL;
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
   ex->file = f ? strdup(f) : NULL;
   // there is no callstack in an out-of-memory exception
   // add exception to list
   insert(ex);
#else
   printf("OUT OF MEMORY: aborting\n");
   exit(1);
#endif
}

// only called with derived classes
QoreException::QoreException() : priv(new qore_ex_private)
{
   //printd(5, "QoreException::QoreException() this=%08p\n", this);
}

// called for runtime errors
QoreException::QoreException(const char *e, class QoreStringNode *d) : priv(new qore_ex_private)
{
   //printd(5, "QoreException::QoreException() this=%08p\n", this);
   priv->type = ET_SYSTEM;
   get_pgm_counter(priv->start_line, priv->end_line);
   const char *f = get_pgm_file();
   priv->file = f ? strdup(f) : NULL;
   priv->callStack = new QoreListNode(); //getCallStackList());

   priv->err = new QoreStringNode(e);
   priv->desc = d;
   priv->arg = NULL;

   priv->next = NULL;
}

// called when parsing
ParseException::ParseException(const char *e, class QoreStringNode *d)
{
   priv->type = ET_SYSTEM;
   priv->start_line = -1;
   priv->end_line = -1;
   get_parse_location(priv->start_line, priv->end_line);
   const char *f = get_parse_file();
   priv->file = f ? strdup(f) : NULL;
   priv->callStack = new QoreListNode(); //getCallStackList());

   priv->err = new QoreStringNode(e);
   priv->desc = d;
   priv->arg = NULL;

   priv->next = NULL;
}

// called when parsing
ParseException::ParseException(int s_line, int e_line, const char *e, class QoreStringNode *d)
{
   priv->type = ET_SYSTEM;
   priv->start_line = s_line;
   priv->end_line = e_line;
   const char *f = get_parse_file();
   priv->file = f ? strdup(f) : NULL;
   priv->callStack = new QoreListNode(); //getCallStackList());

   priv->err = new QoreStringNode(e);
   priv->desc = d;
   priv->arg = NULL;

   priv->next = NULL;
}

QoreException::~QoreException()
{
   delete priv;
}

void QoreException::del(class ExceptionSink *xsink)
{
   if (priv->callStack) {
      priv->callStack->deref(xsink);
      priv->callStack = 0;
   }
   if (priv->err) {
      priv->err->deref(xsink);
      priv->err = 0;
   }
   if (priv->desc) {
      priv->desc->deref(xsink);
      priv->desc = 0;
   }
   if (priv->arg) {
      priv->arg->deref(xsink);
      priv->arg = 0;
   }
   if (priv->next)
      priv->next->del(xsink);
   
   delete this;
}

QoreException::QoreException(QoreListNode *l) : priv(new qore_ex_private)
{
   priv->type = ET_USER;
   get_pgm_counter(priv->start_line, priv->end_line);   
   const char *f = get_pgm_file();
   priv->file = f ? strdup(f) : NULL;
   priv->callStack = new QoreListNode(); //getCallStackList());
   priv->next = NULL;

   // must be a list
   if (l)
   {
      priv->err = l->retrieve_entry(0);
      if (priv->err)
	 priv->err->ref();
      priv->desc = l->retrieve_entry(1);
      if (priv->desc)
	 priv->desc->ref();
      if (l->size() > 3)
	 priv->arg = l->copyListFrom(2);
      else
      {
	 priv->arg = l->retrieve_entry(2);
	 if (priv->arg)
	    priv->arg->ref();
      }
   }
   else {
      priv->err = priv->desc = 0;
      priv->arg = 0;
   }
}

QoreException::QoreException(class QoreException *old, class ExceptionSink *xsink) : priv(new qore_ex_private)
{
   priv->type       = old->priv->type;
   priv->start_line = old->priv->start_line;
   priv->end_line   = old->priv->end_line;
   priv->file       = old->priv->file ? strdup(old->priv->file) : NULL;
   priv->callStack  = old->priv->callStack->copy();
   // insert current position as a rethrow entry in the new callstack
   class QoreListNode *l = priv->callStack;
   const char *fn = NULL;
   QoreHashNode *n = reinterpret_cast<QoreHashNode *>(l->retrieve_entry(0));
   // get function name
   if (n) {
      QoreStringNode *func = reinterpret_cast<QoreStringNode *>(n->getKeyValue("function"));
      fn = func->getBuffer();
   }
   if (!fn)
      fn = "<unknown>";
   
   int sline, eline;
   get_pgm_counter(sline, eline);
   class QoreHashNode *h = getStackHash(CT_RETHROW, NULL, fn, get_pgm_file(), sline, eline);
   l->insert(h);

   priv->next = old->priv->next ? new QoreException(old->priv->next, xsink) : NULL;

   priv->err = old->priv->err ? old->priv->err->RefSelf() : NULL;
   priv->desc = old->priv->desc ? old->priv->desc->RefSelf() : NULL;
   priv->arg = old->priv->arg ? old->priv->arg->RefSelf() : NULL;
}

QoreHashNode *QoreException::makeExceptionObject()
{
   tracein("makeExceptionObject()");

   QoreHashNode *h = new QoreHashNode();

   h->setKeyValue("type", new QoreStringNode(priv->type == ET_USER ? "User" : "System"), NULL);
   h->setKeyValue("file", new QoreStringNode(priv->file), NULL);
   h->setKeyValue("line", new QoreBigIntNode(priv->start_line), NULL);
   h->setKeyValue("endline", new QoreBigIntNode(priv->end_line), NULL);
   h->setKeyValue("callstack", priv->callStack->RefSelf(), NULL);

   if (priv->err)
      h->setKeyValue("err", priv->err->RefSelf(), NULL);
   if (priv->desc)
      h->setKeyValue("desc", priv->desc->RefSelf(), NULL);
   if (priv->arg)
      h->setKeyValue("arg", priv->arg->RefSelf(), NULL);

   // add chained exceptions with this "chain reaction" call
   if (priv->next)
      h->setKeyValue("next", priv->next->makeExceptionObject(), NULL);

   traceout("makeExceptionObject()");
   return h;
}

class QoreHashNode *QoreException::makeExceptionObjectAndDelete(ExceptionSink *xsink)
{
   tracein("makeExceptionObjectAndDelete()");
   QoreHashNode *rv = makeExceptionObject();
   del(xsink);
   traceout("makeExceptionObjectAndDelete()");
   return rv;
}

void QoreException::addStackInfo(class QoreNode *n)
{
   priv->callStack->push(n);
}

// static member function
void ExceptionSink::defaultExceptionHandler(QoreException *e)
{
   class ExceptionSink xsink;

   while (e)
   {
      printe("unhandled QORE %s exception thrown", e->priv->type == ET_USER ? "User" : "System");

      class QoreListNode *cs = e->priv->callStack;
      bool found = false;
      if (cs->size())
      {
	 // find first non-rethrow element
	 int i = 0;
	 
	 QoreHashNode *h;
	 while (true) {
	    h = reinterpret_cast<QoreHashNode *>(cs->retrieve_entry(i));
	    assert(h);	    
	    if ((reinterpret_cast<QoreBigIntNode *>(h->getKeyValue("typecode")))->val != CT_RETHROW)
	       break;
	    i++;
	    if (i == cs->size())
	       break;
	 }

	 if (i < cs->size())
	 {
	    found = true;
	    QoreStringNode *func = reinterpret_cast<QoreStringNode *>(h->getKeyValue("function"));
	    QoreStringNode *type = reinterpret_cast<QoreStringNode *>(h->getKeyValue("type"));

	    if (e->priv->start_line == e->priv->end_line)
	       printe(" in %s() (%s:%d, %s code)\n", func->getBuffer(), e->priv->file, e->priv->start_line, type->getBuffer());
	    else
	       printe(" in %s() (%s:%d-%d, %s code)\n", func->getBuffer(), 
		      e->priv->file, e->priv->start_line, e->priv->end_line, type->getBuffer());
	 }
      }

      if (!found)
      {
	 if (e->priv->file)
	    if (e->priv->start_line == e->priv->end_line)
	       printe(" at %s:%d", e->priv->file, e->priv->start_line);
	    else
	       printe(" at %s:%d-%d", e->priv->file, e->priv->start_line, e->priv->end_line);
	 else if (e->priv->start_line)
	    if (e->priv->start_line == e->priv->end_line)
	       printe(" on line %d", e->priv->start_line);
	    else
	       printe(" on lines %d through %d", e->priv->start_line, e->priv->end_line);
	 printe("\n");
      }
      
      if (e->priv->type == ET_SYSTEM) {
	 QoreStringNode *err = reinterpret_cast<QoreStringNode *>(e->priv->err);
	 QoreStringNode *desc = reinterpret_cast<QoreStringNode *>(e->priv->desc);
	 printe("%s: %s\n", err->getBuffer(), desc->getBuffer());
      }
      else
      {
	 bool hdr = false;

	 if (e->priv->err)
	 {
	    if (e->priv->err->type == NT_STRING) {
	       QoreStringNode *err = reinterpret_cast<QoreStringNode *>(e->priv->err);
	       printe("%s", err->getBuffer());
	    }
	    else
	    {
	       QoreNodeAsStringHelper str(e->priv->err, FMT_NORMAL, &xsink);
	       printe("EXCEPTION: %s", str->getBuffer());
	       hdr = true;
	    }
	 }
	 else
	    printe("EXCEPTION");
	 
	 if (e->priv->desc)
	 {
	    if (e->priv->desc->type == NT_STRING) {
	       QoreStringNode *desc = reinterpret_cast<QoreStringNode *>(e->priv->desc);
	       printe("%s%s", hdr ? ", desc: " : ": ", desc->getBuffer());
	    }
	    else
	    {
	       QoreNodeAsStringHelper str(e->priv->desc, FMT_NORMAL, &xsink);
	       printe(", desc: %s", str->getBuffer());
	       hdr = true;
	    }
	 }
	 
	 if (e->priv->arg)
	 {
	    if (e->priv->arg->type == NT_STRING) {
	       QoreStringNode *arg = reinterpret_cast<QoreStringNode *>(e->priv->arg);
	       printe("%s%s", hdr ? ", arg: " : "", arg->getBuffer());
	    }
	    else
	    {
	       QoreNodeAsStringHelper str (e->priv->arg, FMT_NORMAL, &xsink);
	       printe(", arg: %s", str->getBuffer());
	    }
	 }
	 printe("\n");
      }
      if (cs->size())
      {
	 printe("call stack:\n");
	 for (int i = 0; i < cs->size(); i++)
	 {
	    int pos = cs->size() - i;
	    class QoreHashNode *h = reinterpret_cast<QoreHashNode *>(cs->retrieve_entry(i));
	    QoreStringNode *strtype = reinterpret_cast<QoreStringNode *>(h->getKeyValue("type"));
	    const char *type = strtype->getBuffer();
	    if (!strcmp(type, "new-thread"))
	       printe(" %2d: *thread start*\n", pos);
	    else
	    {
	       QoreStringNode *fn = reinterpret_cast<QoreStringNode *>(h->getKeyValue("file"));
	       const char *fns = fn ? fn->getBuffer() : NULL;
	       int start_line = reinterpret_cast<QoreBigIntNode *>(h->getKeyValue("line"))->val;
	       int end_line = reinterpret_cast<QoreBigIntNode *>(h->getKeyValue("endline"))->val;
	       if (!strcmp(type, "rethrow"))
	       {
		  if (fn)
		     printe(" %2d: RETHROW at %s:%d\n", pos, fn->getBuffer(), start_line);
		  else
		     printe(" %2d: RETHROW at line %d\n", pos, start_line);
	       }
	       else
	       {
		  QoreStringNode *fs = reinterpret_cast<QoreStringNode *>(h->getKeyValue("function"));
		  const char *func = fs->getBuffer();
		  if (fns)
		     if (start_line == end_line)
			printe(" %2d: %s() (%s:%d, %s code)\n", pos, func, fns, start_line, type);
		     else
			printe(" %2d: %s() (%s:%d-%d, %s code)\n", pos, func, fns, start_line, end_line, type);
		  else
		     if (start_line == end_line)
			printe(" %2d: %s() (line %d, %s code)\n", pos, func, start_line, type);
		     else
			printe(" %2d: %s() (line %d - %d, %s code)\n", pos, func, start_line, end_line, type);
	       }
	    }
	 }
      }
      e = e->priv->next;
      if (e)
	 printe("chained exception:\n");
   }
}

// static member function
void ExceptionSink::defaultWarningHandler(QoreException *e)
{
   class ExceptionSink xsink;

   while (e)
   {
      printe("warning encountered ");

      if (e->priv->file)
	 if (e->priv->start_line == e->priv->end_line)
	    printe("at %s:%d", e->priv->file, e->priv->start_line);
	 else
	    printe("at %s:%d-%d", e->priv->file, e->priv->start_line, e->priv->end_line);
      else if (e->priv->start_line)
	 if (e->priv->start_line == e->priv->end_line)
	    printe("on line %d", e->priv->start_line);
	 else
	    printe("on line %d-%d", e->priv->start_line, e->priv->end_line);
      printe("\n");

      QoreStringNode *err  = reinterpret_cast<QoreStringNode *>(e->priv->err);
      QoreStringNode *desc = reinterpret_cast<QoreStringNode *>(e->priv->desc);

      printe("%s: %s\n", err->getBuffer(), desc->getBuffer());

      e = e->priv->next;
      if (e)
	 printe("next warning:\n");
   }
}

// static function
class QoreHashNode *QoreException::getStackHash(int type, const char *class_name, const char *code, const char *file, int start_line, int end_line)
{
   class QoreHashNode *h = new QoreHashNode();

   class QoreStringNode *str = new QoreStringNode();
   if (class_name)
      str->sprintf("%s::", class_name);
   str->concat(code);
   
   h->setKeyValue("function", str, NULL);
   h->setKeyValue("line",     new QoreBigIntNode(start_line), NULL);
   h->setKeyValue("endline",  new QoreBigIntNode(end_line), NULL);
   h->setKeyValue("file",     file ? new QoreStringNode(file) : NULL, NULL);
   h->setKeyValue("typecode", new QoreBigIntNode(type), NULL);
   const char *tstr = 0;
   switch (type)
   {
      case CT_USER:
	 tstr = "user";
         break;
      case CT_BUILTIN:
	 tstr = "builtin";
         break;
      case CT_RETHROW:
	 tstr = "rethrow";
         break;
/*
      case CT_NEWTHREAD:
	 tstr = "new-thread";
         break;
*/
      default:
	 assert(false);
   }
   h->setKeyValue("type",  new QoreStringNode(tstr), NULL);
   return h;
}
