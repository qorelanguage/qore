/*
  QoreException.cpp

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

#include <qore/Qore.h>
#include <qore/intern/qore_program_private.h>

#include <qore/safe_dslist>

#include <assert.h>

#define Q_MAX_EXCEPTIONS 10

void QoreException::del(ExceptionSink *xsink) {
   if (callStack) {
      callStack->deref(xsink);
#ifdef DEBUG
      callStack = 0;
#endif
   }
   if (err) {
      err->deref(xsink);
#ifdef DEBUG
      err = 0;
#endif
   }
   if (desc) {
      desc->deref(xsink);
#ifdef DEBUG
      desc = 0;
#endif
   }
   if (arg) {
      arg->deref(xsink);
#ifdef DEBUG
      arg = 0;
#endif
   }
   if (next)
      next->del(xsink);
   
   delete this;
}

QoreHashNode *QoreException::makeExceptionObject() {
   QORE_TRACE("makeExceptionObject()");

   QoreHashNode *h = new QoreHashNode;

   h->setKeyValue("type", new QoreStringNode(type == ET_USER ? "User" : "System"), 0);
   h->setKeyValue("file", new QoreStringNode(file), 0);
   h->setKeyValue("line", new QoreBigIntNode(start_line), 0);
   h->setKeyValue("endline", new QoreBigIntNode(end_line), 0);
   h->setKeyValue("source", new QoreStringNode(source), 0);
   h->setKeyValue("offset", new QoreBigIntNode(offset), 0);
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

QoreHashNode *QoreException::makeExceptionObjectAndDelete(ExceptionSink *xsink) {
   QORE_TRACE("makeExceptionObjectAndDelete()");
   QoreHashNode *rv = makeExceptionObject();
   del(xsink);

   return rv;
}

void QoreException::addStackInfo(AbstractQoreNode *n) {
   callStack->push(n);
}

// static member function
void ExceptionSink::defaultExceptionHandler(QoreException* e) {
   ExceptionSink xsink;

   QoreString nstr;
   {
      DateTime now;
      now.setNow();
      now.format(nstr, "YYYY-MM-DD HH:mm:SS.xx Dy Z (z)");
   }

   unsigned ecnt = 0;

   while (e) {
      //printd(5, "ExceptionSink::defaultExceptionHandler() cs size=%d\n", cs->size());
      printe("unhandled QORE %s exception thrown in TID %d at %s", e->type == ET_USER ? "User" : "System", gettid(), nstr.getBuffer());

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

	    printe(" in %s() (%s:%d", func->getBuffer(), e->file.c_str(), e->start_line);

	    if (e->start_line == e->end_line) {
	       if (!e->source.empty())
	          printe(", source %s:%d", e->source.c_str(), e->start_line + e->offset);
	    }
	    else {
               printe("-%d", e->end_line);
	       if (!e->source.empty())
                  printe(", source %s:%d-%d", e->source.c_str(), e->start_line + e->offset, e->end_line + e->offset);
	    }
	    printe(", %s code)\n", type->getBuffer());
	 }
      }

      if (!found) {
	 if (!e->file.empty()) {
	    printe(" at %s:", e->file.c_str());
	    if (e->start_line == e->end_line) {
	       if (!e->start_line) {
		  printe("<init>");
		  if (!e->source.empty())
		     printe(" (source %s)", e->source.c_str());
	       }
	       else {
		  printe("%d", e->start_line);
                  if (!e->source.empty())
                     printe(" (source %s:%d)", e->source.c_str(), e->start_line + e->offset);
	       }
	    }
	    else {
	       printe("%d-%d", e->start_line, e->end_line);
               if (!e->source.empty())
                  printe(" (source %s:%d-%d)", e->source.c_str(), e->start_line + e->offset, e->end_line + e->offset);
	    }
	 }
	 else if (e->start_line) {
	    if (e->start_line == e->end_line) {
	       if (!e->start_line)
		  printe(" at <init>");
	       else
		  printe(" on line %d", e->start_line);
	    }
	    else
	       printe(" on lines %d through %d", e->start_line, e->end_line);
	 }
	 printe("\n");
      }
      
      if (e->type == ET_SYSTEM) {
	 QoreStringNode* err = reinterpret_cast<QoreStringNode *>(e->err);
	 QoreStringNode* desc = reinterpret_cast<QoreStringNode *>(e->desc);
	 printe("%s: %s\n", err->getBuffer(), desc->getBuffer());
      }
      else {
	 bool hdr = false;

	 if (e->err) {
	    if (e->err->getType() == NT_STRING) {
	       QoreStringNode *err = reinterpret_cast<QoreStringNode *>(e->err);
	       printe("%s", err->getBuffer());
	    }
	    else {
	       QoreNodeAsStringHelper str(e->err, FMT_NORMAL, &xsink);
	       printe("EXCEPTION: %s", str->getBuffer());
	       hdr = true;
	    }
	 }
	 else
	    printe("EXCEPTION");
	 
	 if (e->desc) {
	    if (e->desc->getType() == NT_STRING) {
	       QoreStringNode *desc = reinterpret_cast<QoreStringNode *>(e->desc);
	       printe("%s%s", hdr ? ", desc: " : ": ", desc->getBuffer());
	    }
	    else {
	       QoreNodeAsStringHelper str(e->desc, FMT_NORMAL, &xsink);
	       printe(", desc: %s", str->getBuffer());
	       hdr = true;
	    }
	 }
	 
	 if (e->arg) {
	    if (e->arg->getType() == NT_STRING) {
	       QoreStringNode *arg = reinterpret_cast<QoreStringNode *>(e->arg);
	       printe("%s%s", hdr ? ", arg: " : "", arg->getBuffer());
	    }
	    else {
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
	    QoreHashNode* h = reinterpret_cast<QoreHashNode*>(cs->retrieve_entry(i));
	    QoreStringNode* strtype = reinterpret_cast<QoreStringNode*>(h->getKeyValue("type"));
	    const char* type = strtype->getBuffer();
	    int typecode = (int)reinterpret_cast<QoreBigIntNode*>(h->getKeyValue("typecode"))->val;
	    if (!strcmp(type, "new-thread"))
	       printe(" %2d: *thread start*\n", pos);
	    else {
	       QoreStringNode* fn = reinterpret_cast<QoreStringNode*>(h->getKeyValue("file"));
	       const char* fns = fn && !fn->empty() ? fn->getBuffer() : 0;
	       int start_line = (int)reinterpret_cast<QoreBigIntNode*>(h->getKeyValue("line"))->val;
	       int end_line = (int)reinterpret_cast<QoreBigIntNode*>(h->getKeyValue("endline"))->val;

	       QoreStringNode* src = reinterpret_cast<QoreStringNode*>(h->getKeyValue("source"));
	       const char* srcs = src && !src->empty() ? src->getBuffer() : 0;
	       int offset = (int)reinterpret_cast<QoreBigIntNode*>(h->getKeyValue("offset"))->val;

	       printe(" %2d: ", pos);

	       if (typecode == CT_RETHROW) {
	          printe("RETHROW at ");
	          if (fn) {
	             printe("%s:", fn->getBuffer());
	          }
	          else
	             printe("line");
	          printe("%d", start_line);
                  if (srcs)
                     printe(" (source %s:%d)", srcs, offset + start_line);
	       }
	       else {
	          QoreStringNode* fs = reinterpret_cast<QoreStringNode *>(h->getKeyValue("function"));
		  printe("%s() (", fs->getBuffer());
		  if (fns) {
		     if (start_line == end_line) {
			if (!start_line)
			   printe("%s:<init>", fns);
			else {
                           printe("%s:%d", fns, start_line);
			   if (srcs)
			      printe(" (source %s:%d)", srcs, start_line + offset);
			}
		     }
		     else {
			printe("%s:%d-%d", fns, start_line, end_line);
                        if (srcs)
                           printe(" (source %s:%d-%d)", srcs, start_line + offset, end_line + offset);
		     }
		  }
		  else {
		     if (start_line == end_line) {
			if (!start_line)
			   printe("<init>");
			else
			   printe("line %d", start_line);
		     }
		     else
			printe("line %d - %d", start_line, end_line);
		  }
		  printe(", %s code)", type);
	       }
	       printe("\n");
	    }
	 }
      }
      e = e->next;
      if (e) {
	 ++ecnt;
	 if (ecnt == Q_MAX_EXCEPTIONS) {
	    printe("*** maximum exception count reached (%d); supressing further output\n", ecnt);
	    break;
	 }
	 printe("chained exception:\n");
      }
   }
}

// static member function
void ExceptionSink::defaultWarningHandler(QoreException *e) {
   ExceptionSink xsink;

   while (e) {
      printe("warning encountered ");

      if (!e->file.empty()) {
         printe("at %s:", e->file.c_str());
	 if (e->start_line == e->end_line) {
	    if (!e->start_line) {
	       printe("<init>");
               if (!e->source.empty())
                  printe(" (source %s)", e->source.c_str());
	    }
	    else {
	       printe("%d", e->start_line);
	       if (!e->source.empty())
	          printe(" (source %s:%d)", e->source.c_str(), e->start_line + e->offset);
	    }
	 }
	 else {
	    printe("%d-%d", e->start_line, e->end_line);
            if (!e->source.empty())
               printe(" (source %s:%d-%d)", e->source.c_str(), e->start_line + e->offset, e->end_line + e->offset);
	 }
      }
      else if (e->start_line) {
	 if (e->start_line == e->end_line) {
	    if (!e->start_line)
	       printe("at <init>");
	    else
	       printe("on line %d", e->start_line);
	 }
	 else
	    printe("on line %d-%d", e->start_line, e->end_line);
      }
      printe("\n");

      QoreStringNode* err  = reinterpret_cast<QoreStringNode*>(e->err);
      QoreStringNode* desc = reinterpret_cast<QoreStringNode*>(e->desc);

      printe("%s: %s\n", err->getBuffer(), desc->getBuffer());

      e = e->next;
      if (e)
	 printe("next warning:\n");
   }
}

// static function
QoreHashNode *QoreException::getStackHash(int type, const char *class_name, const char *code, const QoreProgramLocation& loc) {
   QoreHashNode *h = new QoreHashNode;

   QoreStringNode *str = new QoreStringNode;
   if (class_name)
      str->sprintf("%s::", class_name);
   str->concat(code);

   //printd(5, "QoreException::getStackHash() %s at %s:%d-%d src: %s+%d\n", str->getBuffer(), loc.file ? loc.file : "n/a", loc.start_line, loc.end_line, loc.source ? loc.source : "n/a", loc.offset);
   
   h->setKeyValue("function", str, 0);
   h->setKeyValue("line",     new QoreBigIntNode(loc.start_line), 0);
   h->setKeyValue("endline",  new QoreBigIntNode(loc.end_line), 0);
   h->setKeyValue("file",     loc.file ? new QoreStringNode(loc.file) : 0, 0);
   h->setKeyValue("source",   loc.source ? new QoreStringNode(loc.source) : 0, 0);
   h->setKeyValue("offset",   new QoreBigIntNode(loc.offset), 0);
   h->setKeyValue("typecode", new QoreBigIntNode(type), 0);
   const char *tstr = 0;
   switch (type) {
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

DLLLOCAL ParseExceptionSink::~ParseExceptionSink() {
   if (xsink)
      qore_program_private::addParseException(getProgram(), xsink);
}
