/*
  Exception.cc

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
#include <qore/CallStack.h>

#include <assert.h>

ExceptionSink::ExceptionSink()
{
   thread_exit = false;
   head = tail = NULL;
}

ExceptionSink::~ExceptionSink()
{
   handleExceptions();
}

void ExceptionSink::raiseThreadExit()
{
   thread_exit = true;
}

bool ExceptionSink::isEvent() const
{
   return head || thread_exit;
}

bool ExceptionSink::isThreadExit() const
{
   return thread_exit;
}

bool ExceptionSink::isException() const
{
   return head;
}

// static function
class Hash *Exception::getStackHash(int type, const char *class_name, const char *code, const char *file, int start_line, int end_line)
{
   class Hash *h = new Hash();

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

// creates a stack trace node and adds it to all exceptions in this sink
void ExceptionSink::addStackInfo(int type, const char *class_name, const char *code, const char *file, int start_line, int end_line)
{
   assert(head);
   class Hash *h = Exception::getStackHash(type, class_name, code, file, start_line, end_line);
   class QoreNode *n = new QoreNode(h);
   
   class Exception *w = head;
   while (w)
   {
      w->addStackInfo(n);
      w = w->next;
      if (w)
	 n->ref();
   }   
}

// creates a stack trace node and adds it to all exceptions in this sink
void ExceptionSink::addStackInfo(int type, const char *class_name, const char *code)
{
   assert(head);

   const char *file = get_pgm_file();
   int start_line, end_line;
   get_pgm_counter(start_line, end_line);
   class Hash *h = Exception::getStackHash(type, class_name, code, file, start_line, end_line);
   class QoreNode *n = new QoreNode(h);
   
   class Exception *w = head;
   while (w)
   {
      w->addStackInfo(n);
      w = w->next;
      if (w)
	 n->ref();
   }   
}

// Intended as a alternative to isException():
// ExceptionSink xsink;
// if (xsink) { .. }
ExceptionSink::operator bool () const
{
   return head;
}

void ExceptionSink::overrideLocation(int sline, int eline, const char *file)
{
   class Exception *w = head;
   while (w)
   {
      w->start_line = sline;
      w->end_line = eline;
      if (w->file)
	 free(w->file);
      w->file = file ? strdup(file) : NULL;
      w = w->next;
   }
}

class Exception *ExceptionSink::catchException()
{
   class Exception *e = head;
   head = tail = NULL;
   return e;
}

void ExceptionSink::handleExceptions()
{
   if (head)
   {
      defaultExceptionHandler(head);
      clear();
   }
   else
      thread_exit = false;
}

void ExceptionSink::handleWarnings()
{
   if (head)
   {
      defaultWarningHandler(head);
      clear();
   }
}

void ExceptionSink::clearIntern()
{
   // delete all exceptions
   ExceptionSink xs;
   if (head)
   {
      head->del(&xs);
      head = tail = NULL;
   }
}

void ExceptionSink::clear()
{
   clearIntern();
   head = tail = NULL;
   thread_exit = false;
}

void ExceptionSink::insert(class Exception *e)
{
   // append exception to the list
   if (!head)
      head = e;
   else
      tail->next = e;
   tail = e;
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
   insert(new Exception(err, desc));
   return NULL;
}

// returns NULL, takes ownership of the "desc" argument
QoreNode *ExceptionSink::raiseException(const char *err, class QoreString *desc)
{
   printd(5, "ExceptionSink::raiseException(%s, %s)\n", err, desc->getBuffer());
   insert(new Exception(err, desc));
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
   Exception* exc = new Exception(err, desc);
   exc->arg = arg;
   insert(exc);
   return NULL;
}

void ExceptionSink::raiseException(class Exception *e)
{
   insert(e);
}

void ExceptionSink::raiseException(class QoreNode *n)
{
   insert(new Exception(n));
}

void ExceptionSink::rethrow(class Exception *old)
{
   insert(new Exception(old, this));
}

void ExceptionSink::assimilate(class ExceptionSink *xs)
{
   if (xs->thread_exit)
   {
      thread_exit = xs->thread_exit;
      xs->thread_exit = false;
   }
   if (xs->tail)
   {
      if (tail)
	 tail->next = xs->head;
      else
	 head = xs->head;
      tail = xs->tail;
   }
   xs->head = xs->tail = NULL;
}

void ExceptionSink::outOfMemory()
{
#ifdef QORE_OOM
   // get pre-allocated out of memory exception for this thread
   class Exception *ex = getOutOfMemoryException();
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
Exception::Exception()
{
   //printd(5, "Exception::Exception() this=%08p\n", this);
}

// called for runtime errors
Exception::Exception(const char *e, class QoreString *d)
{
   //printd(5, "Exception::Exception() this=%08p\n", this);
   type = ET_SYSTEM;
   get_pgm_counter(start_line, end_line);
   const char *f = get_pgm_file();
   file = f ? strdup(f) : NULL;
   callStack = new QoreNode(new List()); //getCallStackList());

   err = new QoreNode(e);
   desc = new QoreNode(d);
   arg = NULL;

   next = NULL;
}

// called when parsing
ParseException::ParseException(const char *e, class QoreString *d)
{
   type = ET_SYSTEM;
   start_line = -1;
   end_line = -1;
   get_parse_location(start_line, end_line);
   const char *f = get_parse_file();
   file = f ? strdup(f) : NULL;
   callStack = new QoreNode(new List()); //getCallStackList());

   err = new QoreNode(e);
   desc = new QoreNode(d);
   arg = NULL;

   next = NULL;
}

// called when parsing
ParseException::ParseException(int s_line, int e_line, const char *e, class QoreString *d)
{
   type = ET_SYSTEM;
   start_line = s_line;
   end_line = e_line;
   const char *f = get_parse_file();
   file = f ? strdup(f) : NULL;
   callStack = new QoreNode(new List()); //getCallStackList());

   err = new QoreNode(e);
   desc = new QoreNode(d);
   arg = NULL;

   next = NULL;
}

Exception::~Exception()
{
   if (file)
      free(file);
}

void Exception::del(class ExceptionSink *xsink)
{
   if (callStack)
      callStack->deref(xsink);

   if (err)
      err->deref(xsink);
   if (desc)
      desc->deref(xsink);
   if (arg)
      arg->deref(xsink);

   if (next)
      next->del(xsink);
   
   delete this;
}

Exception::Exception(class QoreNode *n)
{
   type = ET_USER;
   get_pgm_counter(start_line, end_line);   
   const char *f = get_pgm_file();
   file = f ? strdup(f) : NULL;
   callStack = new QoreNode(new List()); //getCallStackList());
   next = NULL;

   // must be a list
   if (n)
   {
      class List *l = n->val.list;
      err = l->retrieve_entry(0);
      if (err)
	 err->ref();
      desc = l->retrieve_entry(1);
      if (desc)
	 desc->ref();
      if (l->size() > 3)
	 arg = new QoreNode(l->copyListFrom(2));
      else
      {
	 arg = l->retrieve_entry(2);
	 if (arg)
	    arg->ref();
      }
   }
   else
      err = desc = arg = NULL;
}

Exception::Exception(class Exception *old, class ExceptionSink *xsink)
{
   type       = old->type;
   start_line = old->start_line;
   end_line   = old->end_line;
   file       = old->file ? strdup(old->file) : NULL;
   callStack  = old->callStack->realCopy(xsink);
   // insert current position as a rethrow entry in the new callstack
   class List *l = callStack->val.list;
   const char *fn = NULL;
   class QoreNode *n = l->retrieve_entry(0);
   // get function name
   if (n)
      fn = n->val.hash->getKeyValue("function")->val.String->getBuffer();
   if (!fn)
      fn = "<unknown>";
   
   int sline, eline;
   get_pgm_counter(sline, eline);
   class Hash *h = getStackHash(CT_RETHROW, NULL, fn, get_pgm_file(), sline, eline);
   l->insert(new QoreNode(h));

   next = old->next ? new Exception(old->next, xsink) : NULL;

   err = old->err ? old->err->RefSelf() : NULL;
   desc = old->desc ? old->desc->RefSelf() : NULL;
   arg = old->arg ? old->arg->RefSelf() : NULL;
}

class QoreNode *Exception::makeExceptionObject()
{
   tracein("makeExceptionObject()");

   Hash *h = new Hash();

   h->setKeyValue("type", new QoreNode(type == ET_USER ? "User" : "System"), NULL);
   h->setKeyValue("file", new QoreNode(file), NULL);
   h->setKeyValue("line", new QoreNode((int64)start_line), NULL);
   h->setKeyValue("endline", new QoreNode((int64)end_line), NULL);
   h->setKeyValue("callstack", callStack->RefSelf(), NULL);

   if (err)
      h->setKeyValue("err", err->RefSelf(), NULL);
   if (desc)
      h->setKeyValue("desc", desc->RefSelf(), NULL);
   if (arg)
      h->setKeyValue("arg", arg->RefSelf(), NULL);

   // add chained exceptions with this "chain reaction" call
   if (next)
      h->setKeyValue("next", next->makeExceptionObject(), NULL);

   traceout("makeExceptionObject()");
   return new QoreNode(h);
}

class QoreNode *Exception::makeExceptionObjectAndDelete(ExceptionSink *xsink)
{
   tracein("makeExceptionObjectAndDelete()");
   class QoreNode *rv = makeExceptionObject();
   del(xsink);
   traceout("makeExceptionObjectAndDelete()");
   return rv;
}

void Exception::addStackInfo(class QoreNode *n)
{
   callStack->val.list->push(n);
}

// static member function
void ExceptionSink::defaultExceptionHandler(Exception *e)
{
   class ExceptionSink xsink;

   while (e)
   {
      printe("unhandled QORE %s exception thrown", e->type == ET_USER ? "User" : "System");

      class List *cs = e->callStack->val.list;
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
	    class Hash *h = cs->retrieve_entry(i)->val.hash;
	    if (e->start_line == e->end_line)
	       printe(" in %s() (%s:%d, %s code)\n",
		      h->getKeyValue("function")->val.String->getBuffer(),
		      e->file, e->start_line,
		      h->getKeyValue("type")->val.String->getBuffer());
	    else
	       printe(" in %s() (%s:%d-%d, %s code)\n",
		      h->getKeyValue("function")->val.String->getBuffer(),
		      e->file, e->start_line, e->end_line,
		      h->getKeyValue("type")->val.String->getBuffer());
	 }
      }

      if (!found)
      {
	 if (e->file)
	    if (e->start_line == e->end_line)
	       printe(" at %s:%d", e->file, e->start_line);
	    else
	       printe(" at %s:%d-%d", e->file, e->start_line, e->end_line);
	 else if (e->start_line)
	    if (e->start_line == e->end_line)
	       printe(" on line %d", e->start_line);
	    else
	       printe(" on lines %d through %d", e->start_line, e->end_line);
	 printe("\n");
      }
      
      if (e->type == ET_SYSTEM)
	 printe("%s: %s\n", e->err->val.String->getBuffer(), e->desc->val.String->getBuffer());
      else
      {
	 bool hdr = false;

	 if (e->err)
	 {
	    if (e->err->type == NT_STRING)
	       printe("%s", e->err->val.String->getBuffer());
	    else
	    {
	       QoreString *str = e->err->getAsString(FMT_NORMAL, &xsink);
	       printe("EXCEPTION: %s", str->getBuffer());
	       delete str;
	       hdr = true;
	    }
	 }
	 else
	    printe("EXCEPTION");
	 
	 if (e->desc)
	 {
	    if (e->desc->type == NT_STRING)
	       printe("%s%s", hdr ? ", desc: " : ": ", e->desc->val.String->getBuffer());
	    else
	    {
	       QoreString *str = e->desc->getAsString(FMT_NORMAL, &xsink);
	       printe(", desc: %s", str->getBuffer());
	       delete str;
	       hdr = true;
	    }
	 }
	 
	 if (e->arg)
	 {
	    if (e->arg->type == NT_STRING)
	       printe("%s%s", hdr ? ", arg: " : "", e->arg->val.String->getBuffer());
	    else
	    {
	       QoreString *str = e->arg->getAsString(FMT_NORMAL, &xsink);
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
	    class Hash *h = cs->retrieve_entry(i)->val.hash;
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
      e = e->next;
      if (e)
	 printe("chained exception:\n");
   }
}

// static member function
void ExceptionSink::defaultWarningHandler(Exception *e)
{
   class ExceptionSink xsink;

   while (e)
   {
      printe("warning encountered ");

      if (e->file)
	 if (e->start_line == e->end_line)
	    printe("at %s:%d", e->file, e->start_line);
	 else
	    printe("at %s:%d-%d", e->file, e->start_line, e->end_line);
      else if (e->start_line)
	 if (e->start_line == e->end_line)
	    printe("on line %d", e->start_line);
	 else
	    printe("on line %d-%d", e->start_line, e->end_line);
      printe("\n");
      
      printe("%s: %s\n", e->err->val.String->getBuffer(), e->desc->val.String->getBuffer());

      e = e->next;
      if (e)
	 printe("next warning:\n");
   }
}
