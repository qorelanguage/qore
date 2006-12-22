/*
  Exception.cc

  Qore programming language exception handling support

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/QoreString.h>
#include <qore/List.h>
#include <qore/CallStack.h>
#include <qore/qore_thread.h>
#include <qore/QoreNode.h>
#include <qore/Object.h>

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
      clearIntern();
   }
}

void ExceptionSink::handleWarnings()
{
   if (head)
   {
      defaultWarningHandler(head);
      clearIntern();
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

QoreNode* ExceptionSink::raiseException(char *err, char *fmt, ...)
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
   insert(new Exception(err, 0, desc));
   return NULL;
}

QoreNode* ExceptionSink::raiseExceptionArg(char* err, QoreNode* arg, char* fmt, ...)
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
   Exception* exc = new Exception(err, 0, desc);
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
   ex->line = get_pgm_counter();
   char *f = get_pgm_file();
   ex->file = f ? strdup(f) : NULL;
   // there is no callstack in an out-of-memory exception
   // add exception to list
   insert(ex);
#else
   printf("OUT OF MEMORY: aborting\n");
   exit(1);
#endif
}

Exception::Exception(char *e, int sline, class QoreString *d)
{
   type = ET_SYSTEM;

   if (sline)
      line = sline;
   else
      line = get_pgm_counter();
   
   char *f = get_pgm_file();
   file = f ? strdup(f) : NULL;
   callStack = new QoreNode(getCallStackList());

   err = new QoreNode(e);
   desc = new QoreNode(d);
   arg = NULL;

   next = NULL;
}

Exception::Exception(char *e, class QoreString *d)
{
   type = ET_SYSTEM;

   line = get_pgm_counter();
   
   char *f = get_pgm_file();
   file = f ? strdup(f) : NULL;
   callStack = new QoreNode(getCallStackList());

   err = new QoreNode(e);
   desc = new QoreNode(d);
   arg = NULL;

   next = NULL;
}

Exception::Exception(char *e, char *fmt, ...)
{
   QoreString *str = new QoreString();
   va_list args;

   while (true)
   {
      va_start(args, fmt);
      int rc = str->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }

   type = ET_SYSTEM;
   line = get_pgm_counter();
   char *f = get_pgm_file();
   file = f ? strdup(f) : NULL;
   callStack = new QoreNode(getCallStackList());

   err = new QoreNode(e);
   desc = new QoreNode(str);
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
   line = get_pgm_counter();
   char *f = get_pgm_file();
   file = f ? strdup(f) : NULL;
   callStack = new QoreNode(getCallStackList());
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
   type = old->type;
   line = old->line;
   file = old->file ? strdup(old->file) : NULL;
   callStack = old->callStack->realCopy(xsink);
   // insert current position as a rethrow entry in the new callstack
   class List *l = callStack->val.list;
   char *fn = NULL;
   class QoreNode *n = l->retrieve_entry(0);
   // get function name
   if (n)
      fn = n->val.hash->getKeyValue("function")->val.String->getBuffer();
   if (!fn)
      fn = "<unknown>";
   class Hash *h = new Hash();
   h->setKeyValue("type", new QoreNode("rethrow"), NULL);
   h->setKeyValue("typecode", new QoreNode((int64)CT_RETHROW), NULL);
   h->setKeyValue("function", new QoreNode(fn), NULL);
   char *f = get_pgm_file();
   if (f)
      h->setKeyValue("file", new QoreNode(f), NULL);
   h->setKeyValue("line", new QoreNode((int64)get_pgm_counter()), NULL);
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

   if (type == ET_USER)
      h->setKeyValue("type", new QoreNode("User"), NULL);
   else
      h->setKeyValue("type", new QoreNode("System"), NULL);

   h->setKeyValue("file", new QoreNode(file), NULL);
   h->setKeyValue("line", new QoreNode((int64)line), NULL);
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
	    printe(" in %s() (%s:%d, %s code)\n",
		   h->getKeyValue("function")->val.String->getBuffer(),
		   e->file, e->line,
		   h->getKeyValue("type")->val.String->getBuffer());
	 }
      }

      if (!found)
      {
	 if (e->file)
	    printe(" at %s:%d", e->file, e->line);
	 else if (e->line)
	    printe(" on line %d", e->line);
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
	    char *type = h->getKeyValue("type")->val.String->getBuffer();
	    if (!strcmp(type, "new-thread"))
	       printe(" %2d: *thread start*\n", pos);
	    else
	    {
	       QoreNode *fn = h->getKeyValue("file");
	       int line = h->getKeyValue("line")->val.intval;
	       if (!strcmp(type, "rethrow"))
	       {
		  if (fn)
		     printe(" %2d: RETHROW at %s:%d\n", pos, fn->val.String->getBuffer(), line);
		  else
		     printe(" %2d: RETHROW at line %d\n", pos, line);
	       }
	       else
	       {
		  if (fn)
		     printe(" %2d: %s() (%s:%d, %s code)\n", pos,
			    h->getKeyValue("function")->val.String->getBuffer(),
			    fn->val.String->getBuffer(),
			    (int)h->getKeyValue("line")->val.intval,
			    type);
		  else
		     printe(" %2d: %s() (line %d, %s code)\n", pos,
			    h->getKeyValue("function")->val.String->getBuffer(),
			    (int)h->getKeyValue("line")->val.intval,
			    type);
		  
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
	 printe("at %s:%d", e->file, e->line);
      else if (e->line)
	 printe("on line %d", e->line);
      printe("\n");
      
      printe("%s: %s\n", e->err->val.String->getBuffer(), e->desc->val.String->getBuffer());

      e = e->next;
      if (e)
	 printe("next warning:\n");
   }
}
