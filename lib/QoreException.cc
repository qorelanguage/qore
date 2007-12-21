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
      class QoreNode *callStack, *err, *desc, *arg;
      class QoreException *next;

      DLLLOCAL qore_ex_private()
      {
      }

      DLLLOCAL ~qore_ex_private()
      {
	 if (file)
	    free(file);
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
   class QoreHash *h = QoreException::getStackHash(type, class_name, code, file, start_line, end_line);
   class QoreNode *n = new QoreNode(h);

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
   class QoreHash *h = QoreException::getStackHash(type, class_name, code, file, start_line, end_line);
   class QoreNode *n = new QoreNode(h);

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
   class QoreString *desc = new QoreString();
   
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
QoreNode *ExceptionSink::raiseException(const char *err, QoreString *desc)
{
   printd(5, "ExceptionSink::raiseException(%s, %s)\n", err, desc->getBuffer());
   insert(new QoreException(err, desc));
   return NULL;
}

QoreNode* ExceptionSink::raiseExceptionArg(const char* err, QoreNode* arg, const char* fmt, ...)
{
   class QoreString *desc = new QoreString();
   
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

void ExceptionSink::raiseException(class QoreException *e)
{
   insert(e);
}

void ExceptionSink::raiseException(class QoreNode *n)
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
QoreException::QoreException(const char *e, class QoreString *d) : priv(new qore_ex_private)
{
   //printd(5, "QoreException::QoreException() this=%08p\n", this);
   priv->type = ET_SYSTEM;
   get_pgm_counter(priv->start_line, priv->end_line);
   const char *f = get_pgm_file();
   priv->file = f ? strdup(f) : NULL;
   priv->callStack = new QoreNode(new QoreList()); //getCallStackList());

   priv->err = new QoreNode(e);
   priv->desc = new QoreNode(d);
   priv->arg = NULL;

   priv->next = NULL;
}

// called when parsing
ParseException::ParseException(const char *e, class QoreString *d)
{
   priv->type = ET_SYSTEM;
   priv->start_line = -1;
   priv->end_line = -1;
   get_parse_location(priv->start_line, priv->end_line);
   const char *f = get_parse_file();
   priv->file = f ? strdup(f) : NULL;
   priv->callStack = new QoreNode(new QoreList()); //getCallStackList());

   priv->err = new QoreNode(e);
   priv->desc = new QoreNode(d);
   priv->arg = NULL;

   priv->next = NULL;
}

// called when parsing
ParseException::ParseException(int s_line, int e_line, const char *e, class QoreString *d)
{
   priv->type = ET_SYSTEM;
   priv->start_line = s_line;
   priv->end_line = e_line;
   const char *f = get_parse_file();
   priv->file = f ? strdup(f) : NULL;
   priv->callStack = new QoreNode(new QoreList()); //getCallStackList());

   priv->err = new QoreNode(e);
   priv->desc = new QoreNode(d);
   priv->arg = NULL;

   priv->next = NULL;
}

QoreException::~QoreException()
{
   delete priv;
}

void QoreException::del(class ExceptionSink *xsink)
{
   if (priv->callStack)
      priv->callStack->deref(xsink);

   if (priv->err)
      priv->err->deref(xsink);
   if (priv->desc)
      priv->desc->deref(xsink);
   if (priv->arg)
      priv->arg->deref(xsink);

   if (priv->next)
      priv->next->del(xsink);
   
   delete this;
}

QoreException::QoreException(class QoreNode *n) : priv(new qore_ex_private)
{
   priv->type = ET_USER;
   get_pgm_counter(priv->start_line, priv->end_line);   
   const char *f = get_pgm_file();
   priv->file = f ? strdup(f) : NULL;
   priv->callStack = new QoreNode(new QoreList()); //getCallStackList());
   priv->next = NULL;

   // must be a list
   if (n)
   {
      class QoreList *l = n->val.list;
      priv->err = l->retrieve_entry(0);
      if (priv->err)
	 priv->err->ref();
      priv->desc = l->retrieve_entry(1);
      if (priv->desc)
	 priv->desc->ref();
      if (l->size() > 3)
	 priv->arg = new QoreNode(l->copyListFrom(2));
      else
      {
	 priv->arg = l->retrieve_entry(2);
	 if (priv->arg)
	    priv->arg->ref();
      }
   }
   else
      priv->err = priv->desc = priv->arg = NULL;
}

QoreException::QoreException(class QoreException *old, class ExceptionSink *xsink) : priv(new qore_ex_private)
{
   priv->type       = old->priv->type;
   priv->start_line = old->priv->start_line;
   priv->end_line   = old->priv->end_line;
   priv->file       = old->priv->file ? strdup(old->priv->file) : NULL;
   priv->callStack  = old->priv->callStack->realCopy(xsink);
   // insert current position as a rethrow entry in the new callstack
   class QoreList *l = priv->callStack->val.list;
   const char *fn = NULL;
   class QoreNode *n = l->retrieve_entry(0);
   // get function name
   if (n)
      fn = n->val.hash->getKeyValue("function")->val.String->getBuffer();
   if (!fn)
      fn = "<unknown>";
   
   int sline, eline;
   get_pgm_counter(sline, eline);
   class QoreHash *h = getStackHash(CT_RETHROW, NULL, fn, get_pgm_file(), sline, eline);
   l->insert(new QoreNode(h));

   priv->next = old->priv->next ? new QoreException(old->priv->next, xsink) : NULL;

   priv->err = old->priv->err ? old->priv->err->RefSelf() : NULL;
   priv->desc = old->priv->desc ? old->priv->desc->RefSelf() : NULL;
   priv->arg = old->priv->arg ? old->priv->arg->RefSelf() : NULL;
}

class QoreNode *QoreException::makeExceptionObject()
{
   tracein("makeExceptionObject()");

   QoreHash *h = new QoreHash();

   h->setKeyValue("type", new QoreNode(priv->type == ET_USER ? "User" : "System"), NULL);
   h->setKeyValue("file", new QoreNode(priv->file), NULL);
   h->setKeyValue("line", new QoreNode((int64)priv->start_line), NULL);
   h->setKeyValue("endline", new QoreNode((int64)priv->end_line), NULL);
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
   return new QoreNode(h);
}

class QoreNode *QoreException::makeExceptionObjectAndDelete(ExceptionSink *xsink)
{
   tracein("makeExceptionObjectAndDelete()");
   class QoreNode *rv = makeExceptionObject();
   del(xsink);
   traceout("makeExceptionObjectAndDelete()");
   return rv;
}

void QoreException::addStackInfo(class QoreNode *n)
{
   priv->callStack->val.list->push(n);
}

// static member function
void ExceptionSink::defaultExceptionHandler(QoreException *e)
{
   class ExceptionSink xsink;

   while (e)
   {
      printe("unhandled QORE %s exception thrown", e->priv->type == ET_USER ? "User" : "System");

      class QoreList *cs = e->priv->callStack->val.list;
      bool found = false;
      if (cs->size())
      {
	 // find first non-rethrow element
	 int i = 0;
	 while (i < cs->size() && cs->retrieve_entry(i)->val.hash->getKeyValue("typecode")->val.intval == CT_RETHROW)
	    i++;
	 if (i < cs->size())
	 {
	    found = true;
	    class QoreHash *h = cs->retrieve_entry(i)->val.hash;
	    if (e->priv->start_line == e->priv->end_line)
	       printe(" in %s() (%s:%d, %s code)\n",
		      h->getKeyValue("function")->val.String->getBuffer(),
		      e->priv->file, e->priv->start_line,
		      h->getKeyValue("type")->val.String->getBuffer());
	    else
	       printe(" in %s() (%s:%d-%d, %s code)\n",
		      h->getKeyValue("function")->val.String->getBuffer(),
		      e->priv->file, e->priv->start_line, e->priv->end_line,
		      h->getKeyValue("type")->val.String->getBuffer());
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
      
      if (e->priv->type == ET_SYSTEM)
	 printe("%s: %s\n", e->priv->err->val.String->getBuffer(), e->priv->desc->val.String->getBuffer());
      else
      {
	 bool hdr = false;

	 if (e->priv->err)
	 {
	    if (e->priv->err->type == NT_STRING)
	       printe("%s", e->priv->err->val.String->getBuffer());
	    else
	    {
	       QoreString *str = e->priv->err->getAsString(FMT_NORMAL, &xsink);
	       printe("EXCEPTION: %s", str->getBuffer());
	       delete str;
	       hdr = true;
	    }
	 }
	 else
	    printe("EXCEPTION");
	 
	 if (e->priv->desc)
	 {
	    if (e->priv->desc->type == NT_STRING)
	       printe("%s%s", hdr ? ", desc: " : ": ", e->priv->desc->val.String->getBuffer());
	    else
	    {
	       QoreString *str = e->priv->desc->getAsString(FMT_NORMAL, &xsink);
	       printe(", desc: %s", str->getBuffer());
	       delete str;
	       hdr = true;
	    }
	 }
	 
	 if (e->priv->arg)
	 {
	    if (e->priv->arg->type == NT_STRING)
	       printe("%s%s", hdr ? ", arg: " : "", e->priv->arg->val.String->getBuffer());
	    else
	    {
	       QoreString *str = e->priv->arg->getAsString(FMT_NORMAL, &xsink);
	       printe(", arg: %s", str->getBuffer());
	       delete str;
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
	    class QoreHash *h = cs->retrieve_entry(i)->val.hash;
	    const char *type = h->getKeyValue("type")->val.String->getBuffer();
	    if (!strcmp(type, "new-thread"))
	       printe(" %2d: *thread start*\n", pos);
	    else
	    {
	       QoreNode *fn = h->getKeyValue("file");
	       const char *fns = fn ? fn->val.String->getBuffer() : NULL;
	       int start_line = h->getKeyValue("line")->val.intval;
	       int end_line = h->getKeyValue("endline")->val.intval;
	       if (!strcmp(type, "rethrow"))
	       {
		  if (fn)
		     printe(" %2d: RETHROW at %s:%d\n", pos, fn->val.String->getBuffer(), start_line);
		  else
		     printe(" %2d: RETHROW at line %d\n", pos, start_line);
	       }
	       else
	       {
		  const char *func = h->getKeyValue("function")->val.String->getBuffer();
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
      
      printe("%s: %s\n", e->priv->err->val.String->getBuffer(), e->priv->desc->val.String->getBuffer());

      e = e->priv->next;
      if (e)
	 printe("next warning:\n");
   }
}

// static function
class QoreHash *QoreException::getStackHash(int type, const char *class_name, const char *code, const char *file, int start_line, int end_line)
{
   class QoreHash *h = new QoreHash();

   class QoreString *str = new QoreString();
   if (class_name)
      str->sprintf("%s::", class_name);
   str->concat(code);
   
   h->setKeyValue("function", str ? new QoreNode(str) : NULL, NULL);
   h->setKeyValue("line",     new QoreNode((int64)start_line), NULL);
   h->setKeyValue("endline",  new QoreNode((int64)end_line), NULL);
   h->setKeyValue("file",     file ? new QoreNode(file) : NULL, NULL);
   h->setKeyValue("typecode", new QoreNode((int64)type), NULL);
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
   h->setKeyValue("type",  new QoreNode(tstr), NULL);
   return h;
}
