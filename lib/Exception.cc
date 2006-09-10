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
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/QoreString.h>
#include <qore/List.h>
#include <qore/Object.h>
#include <qore/CallStack.h>

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

Exception::Exception(class QoreNode *n)
{
   type = ET_USER;
   line = get_pgm_counter();
   char *f = get_pgm_file();
   file = f ? strdup(f) : NULL;
   callStack = new QoreNode(getCallStack());
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

class QoreNode *makeExceptionObject(Exception *e)
{
   tracein("makeExceptionObject()");

   Hash *h = new Hash();

   if (e->type == ET_USER)
      h->setKeyValue("type", new QoreNode("User"), NULL);
   else
      h->setKeyValue("type", new QoreNode("System"), NULL);

   h->setKeyValue("file", new QoreNode(e->file), NULL);
   h->setKeyValue("line", new QoreNode((int64)e->line), NULL);
   h->setKeyValue("callstack", e->callStack->RefSelf(), NULL);

   if (e->err)
      h->setKeyValue("err", e->err->RefSelf(), NULL);
   if (e->desc)
      h->setKeyValue("desc", e->desc->RefSelf(), NULL);
   if (e->arg)
      h->setKeyValue("arg", e->arg->RefSelf(), NULL);

   // add chained exceptions with this "chain reaction" call
   if (e->next)
      h->setKeyValue("next", makeExceptionObject(e->next), NULL);

   traceout("makeExceptionObject()");
   return new QoreNode(h);
}

void defaultExceptionHandler(Exception *e)
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

void defaultWarningHandler(Exception *e)
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
