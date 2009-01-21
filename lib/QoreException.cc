/*
  QoreException.cc

  Qore programming language exception handling support

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

#include <qore/Qore.h>
#include <qore/intern/CallStack.h>

#include <qore/safe_dslist>

#include <assert.h>

struct qore_ex_private {

      DLLLOCAL qore_ex_private()
      {
      }

      DLLLOCAL ~qore_ex_private()
      {
      }
};



// only called with derived classes
QoreException::QoreException()
{
   //printd(5, "QoreException::QoreException() this=%08p\n", this);
}

// called for runtime errors
QoreException::QoreException(const char *e, QoreStringNode *d)
{
   //printd(5, "QoreException::QoreException() this=%08p\n", this);
   type = ET_SYSTEM;
   get_pgm_counter(start_line, end_line);
   const char *f = get_pgm_file();
   file = f ? strdup(f) : 0;
   callStack = new QoreListNode();

   err = new QoreStringNode(e);
   desc = d;
   arg = 0;

   next = 0;
}

// called when parsing
ParseException::ParseException(const char *e, QoreStringNode *d)
{
   type = ET_SYSTEM;
   start_line = -1;
   end_line = -1;
   get_parse_location(start_line, end_line);
   const char *f = get_parse_file();
   file = f ? strdup(f) : 0;
   callStack = new QoreListNode();

   err = new QoreStringNode(e);
   desc = d;
   arg = 0;

   next = 0;
}

// called when parsing
ParseException::ParseException(int s_line, int e_line, const char *e, QoreStringNode *d)
{
   type = ET_SYSTEM;
   start_line = s_line;
   end_line = e_line;
   const char *f = get_parse_file();
   file = f ? strdup(f) : 0;
   callStack = new QoreListNode();

   err = new QoreStringNode(e);
   desc = d;
   arg = 0;

   next = 0;
}

QoreException::~QoreException()
{
   if (file)
      free(file);
   assert(!callStack);
   assert(!err);
   assert(!desc);
   assert(!arg);
}

void QoreException::del(ExceptionSink *xsink)
{
   if (callStack) {
      callStack->deref(xsink);
      callStack = 0;
   }
   if (err) {
      err->deref(xsink);
      err = 0;
   }
   if (desc) {
      desc->deref(xsink);
      desc = 0;
   }
   if (arg) {
      arg->deref(xsink);
      arg = 0;
   }
   if (next)
      next->del(xsink);
   
   delete this;
}

QoreException::QoreException(const QoreListNode *l)
{
   type = ET_USER;
   get_pgm_counter(start_line, end_line);   
   const char *f = get_pgm_file();
   file = f ? strdup(f) : 0;
   callStack = new QoreListNode();
   next = 0;

   // must be a list
   if (l)
   {
      err = l->get_referenced_entry(0);
      desc = l->get_referenced_entry(1);
      if (l->size() > 3)
	 arg = l->copyListFrom(2);
      else
	 arg = l->get_referenced_entry(2);
   }
   else {
      err = desc = 0;
      arg = 0;
   }
}

QoreException::QoreException(class QoreException *old, ExceptionSink *xsink)
{
   type       = old->type;
   start_line = old->start_line;
   end_line   = old->end_line;
   file       = old->file ? strdup(old->file) : 0;
   callStack  = old->callStack->copy();
   // insert current position as a rethrow entry in the new callstack
   QoreListNode *l = callStack;
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
   get_pgm_counter(sline, eline);
   QoreHashNode *h = getStackHash(CT_RETHROW, 0, fn, get_pgm_file(), sline, eline);
   l->insert(h);

   next = old->next ? new QoreException(old->next, xsink) : 0;

   err = old->err ? old->err->refSelf() : 0;
   desc = old->desc ? old->desc->refSelf() : 0;
   arg = old->arg ? old->arg->refSelf() : 0;
}

QoreHashNode *QoreException::makeExceptionObject()
{
   QORE_TRACE("makeExceptionObject()");

   QoreHashNode *h = new QoreHashNode();

   h->setKeyValue("type", new QoreStringNode(type == ET_USER ? "User" : "System"), 0);
   h->setKeyValue("file", new QoreStringNode(file), 0);
   h->setKeyValue("line", new QoreBigIntNode(start_line), 0);
   h->setKeyValue("endline", new QoreBigIntNode(end_line), 0);
   h->setKeyValue("callstack", callStack->refSelf(), 0);

   if (err)
      h->setKeyValue("err", err->refSelf(), 0);
   if (desc)
      h->setKeyValue("desc", desc->refSelf(), 0);
   if (arg)
      h->setKeyValue("arg", arg->refSelf(), 0);

   // add chained exceptions with this "chain reaction" call
   if (next)
      h->setKeyValue("next", next->makeExceptionObject(), 0);


   return h;
}

QoreHashNode *QoreException::makeExceptionObjectAndDelete(ExceptionSink *xsink)
{
   QORE_TRACE("makeExceptionObjectAndDelete()");
   QoreHashNode *rv = makeExceptionObject();
   del(xsink);

   return rv;
}

void QoreException::addStackInfo(AbstractQoreNode *n)
{
   callStack->push(n);
}

// static member function
void ExceptionSink::defaultExceptionHandler(QoreException *e)
{
   ExceptionSink xsink;

   while (e) {
      //printd(5, "ExceptionSink::defaultExceptionHandler() cs size=%d\n", cs->size());
      printe("unhandled QORE %s exception thrown in TID %d", e->type == ET_USER ? "User" : "System", gettid());

      QoreListNode *cs = e->callStack;
      bool found = false;
      if (cs->size()) {
	 // find first non-rethrow element
	 unsigned i = 0;
	 
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

	 if (i < cs->size()) {
	    found = true;
	    QoreStringNode *func = reinterpret_cast<QoreStringNode *>(h->getKeyValue("function"));
	    QoreStringNode *type = reinterpret_cast<QoreStringNode *>(h->getKeyValue("type"));

	    if (e->start_line == e->end_line)
	       printe(" in %s() (%s:%d, %s code)\n", func->getBuffer(), e->file, e->start_line, type->getBuffer());
	    else
	       printe(" in %s() (%s:%d-%d, %s code)\n", func->getBuffer(), 
		      e->file, e->start_line, e->end_line, type->getBuffer());
	 }
      }

      if (!found) {
	 if (e->file) {
	    if (e->start_line == e->end_line)
	       printe(" at %s:%d", e->file, e->start_line);
	    else
	       printe(" at %s:%d-%d", e->file, e->start_line, e->end_line);
	 }
	 else if (e->start_line) {
	    if (e->start_line == e->end_line)
	       printe(" on line %d", e->start_line);
	    else
	       printe(" on lines %d through %d", e->start_line, e->end_line);
	 }
	 printe("\n");
      }
      
      if (e->type == ET_SYSTEM) {
	 QoreStringNode *err = reinterpret_cast<QoreStringNode *>(e->err);
	 QoreStringNode *desc = reinterpret_cast<QoreStringNode *>(e->desc);
	 printe("%s: %s\n", err->getBuffer(), desc->getBuffer());
      }
      else {
	 bool hdr = false;

	 if (e->err) {
	    if (e->err->getType() == NT_STRING) {
	       QoreStringNode *err = reinterpret_cast<QoreStringNode *>(e->err);
	       printe("%s", err->getBuffer());
	    }
	    else
	    {
	       QoreNodeAsStringHelper str(e->err, FMT_NORMAL, &xsink);
	       printe("EXCEPTION: %s", str->getBuffer());
	       hdr = true;
	    }
	 }
	 else
	    printe("EXCEPTION");
	 
	 if (e->desc)
	 {
	    if (e->desc->getType() == NT_STRING) {
	       QoreStringNode *desc = reinterpret_cast<QoreStringNode *>(e->desc);
	       printe("%s%s", hdr ? ", desc: " : ": ", desc->getBuffer());
	    }
	    else
	    {
	       QoreNodeAsStringHelper str(e->desc, FMT_NORMAL, &xsink);
	       printe(", desc: %s", str->getBuffer());
	       hdr = true;
	    }
	 }
	 
	 if (e->arg)
	 {
	    if (e->arg->getType() == NT_STRING) {
	       QoreStringNode *arg = reinterpret_cast<QoreStringNode *>(e->arg);
	       printe("%s%s", hdr ? ", arg: " : "", arg->getBuffer());
	    }
	    else
	    {
	       QoreNodeAsStringHelper str (e->arg, FMT_NORMAL, &xsink);
	       printe(", arg: %s", str->getBuffer());
	    }
	 }
	 printe("\n");
      }

      if (cs->size()) {
	 printe("call stack:\n");
	 for (unsigned i = 0; i < cs->size(); i++) {
	    int pos = cs->size() - i;
	    QoreHashNode *h = reinterpret_cast<QoreHashNode *>(cs->retrieve_entry(i));
	    QoreStringNode *strtype = reinterpret_cast<QoreStringNode *>(h->getKeyValue("type"));
	    const char *type = strtype->getBuffer();
	    if (!strcmp(type, "new-thread"))
	       printe(" %2d: *thread start*\n", pos);
	    else {
	       QoreStringNode *fn = reinterpret_cast<QoreStringNode *>(h->getKeyValue("file"));
	       const char *fns = fn ? fn->getBuffer() : 0;
	       int start_line = reinterpret_cast<QoreBigIntNode *>(h->getKeyValue("line"))->val;
	       int end_line = reinterpret_cast<QoreBigIntNode *>(h->getKeyValue("endline"))->val;
	       if (!strcmp(type, "rethrow")) {
		  if (fn)
		     printe(" %2d: RETHROW at %s:%d\n", pos, fn->getBuffer(), start_line);
		  else
		     printe(" %2d: RETHROW at line %d\n", pos, start_line);
	       }
	       else {
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
      e = e->next;
      if (e)
	 printe("chained exception:\n");
   }
}

// static member function
void ExceptionSink::defaultWarningHandler(QoreException *e)
{
   ExceptionSink xsink;

   while (e)
   {
      printe("warning encountered ");

      if (e->file) {
	 if (e->start_line == e->end_line)
	    printe("at %s:%d", e->file, e->start_line);
	 else
	    printe("at %s:%d-%d", e->file, e->start_line, e->end_line);
      }
      else if (e->start_line) {
	 if (e->start_line == e->end_line)
	    printe("on line %d", e->start_line);
	 else
	    printe("on line %d-%d", e->start_line, e->end_line);
      }
      printe("\n");

      QoreStringNode *err  = reinterpret_cast<QoreStringNode *>(e->err);
      QoreStringNode *desc = reinterpret_cast<QoreStringNode *>(e->desc);

      printe("%s: %s\n", err->getBuffer(), desc->getBuffer());

      e = e->next;
      if (e)
	 printe("next warning:\n");
   }
}

// static function
QoreHashNode *QoreException::getStackHash(int type, const char *class_name, const char *code, const char *file, int start_line, int end_line)
{
   QoreHashNode *h = new QoreHashNode();

   QoreStringNode *str = new QoreStringNode();
   if (class_name)
      str->sprintf("%s::", class_name);
   str->concat(code);
   
   h->setKeyValue("function", str, 0);
   h->setKeyValue("line",     new QoreBigIntNode(start_line), 0);
   h->setKeyValue("endline",  new QoreBigIntNode(end_line), 0);
   h->setKeyValue("file",     file ? new QoreStringNode(file) : 0, 0);
   h->setKeyValue("typecode", new QoreBigIntNode(type), 0);
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
   h->setKeyValue("type",  new QoreStringNode(tstr), 0);
   return h;
}
