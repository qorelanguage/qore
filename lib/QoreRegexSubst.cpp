/*
    QoreRegexSubst.cpp

    regular expression substitution node definition

    Qore Programming Language

    Copyright (C) 2003 - 2018 Qore Technologies, s.r.o.

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
#include "qore/intern/QoreRegexSubst.h"
#include "qore/intern/qore_program_private.h"

#include <cctype>
#include <cstdlib>
#include <strings.h>

void QoreRegexSubst::init() {
   p = 0;
   global = false;
   options = PCRE_UTF8;
}

// constructor used when parsing
QoreRegexSubst::QoreRegexSubst() {
   //printd(5, "QoreRegexSubst::QoreRegexSubst() this=%p\n", this);
   init();
   str = new QoreString;
   newstr = new QoreString;
}

// constructor when used at run-time
QoreRegexSubst::QoreRegexSubst(const QoreString *pstr, int opts, ExceptionSink *xsink) {
   init();
   if (check_re_options(opts))
      xsink->raiseException("REGEX-OPTION-ERROR", "%d contains invalid option bits", opts);
   else
      options |= opts;

   newstr = 0;
   str = 0;
   parseRT(pstr, xsink);
}

QoreRegexSubst::~QoreRegexSubst() {
   //printd(5, "QoreRegexSubst::~QoreRegexSubst() this=%p\n", this);
   delete newstr;
   if (p)
      pcre_free(p);
   delete str;
}

void QoreRegexSubst::concatSource(char c) {
   str->concat(c);
}

void QoreRegexSubst::concatTarget(char c) {
   newstr->concat(c);
}

// returns 0 for OK, -1 if parse error raised
void QoreRegexSubst::parseRT(const QoreString *pstr, ExceptionSink *xsink) {
   // convert to UTF-8 if necessary
   TempEncodingHelper t(pstr, QCS_UTF8, xsink);
   if (*xsink)
      return;

   const char *err;
   int eo;
   p = pcre_compile(t->getBuffer(), options, &err, &eo, 0);
   if (err)
      xsink->raiseException("REGEX-COMPILATION-ERROR", (char *)err);
}

void QoreRegexSubst::parse() {
   //printd(5, "QoreRegexSubst() this=%p: str='%s', divider=%d\n", this, str->getBuffer(), divider);
   ExceptionSink xsink;
   parseRT(str, &xsink);
   if (xsink.isEvent())
      qore_program_private::addParseException(getProgram(), xsink);

   //printd(5, "QoreRegexSubst::parse() this=%p: pstr=%s, newstr=%s, global=%s\n", this, pstr->getBuffer(), newstr->getBuffer(), global ? "true" : "false");

   delete str;
   str = 0;
}

// static function
void QoreRegexSubst::concat(QoreString *cstr, int *ovector, int olen, const char *ptr, const char *target, int rc) {
   while (*ptr) {
      if (*ptr == '\\' && *(ptr+1) == '\\') {
	 ++ptr;
	 cstr->concat(*(ptr++));
      }
      else if (*ptr == '\\' && *(ptr+1) == '$') {
	 ++ptr;
	 cstr->concat(*(ptr++));
      }
      else if (*ptr == '$' && isdigit(ptr[1])) {
	 QoreString n;
	 ++ptr;
	 do
	    n.concat(*(ptr++));
	 while (isdigit(*ptr));
	 int num = atoi(n.getBuffer());
	 int pos = num * 2;
	 //printd(5, "QoreRegexSubst::concat() pos: %d olen: %d ovector[%d]: %d ovector[%d]: %d rc: %d\n", pos, olen, pos, ovector[pos], pos + 1, ovector[pos + 1], rc);
	 if (pos > 0 && pos < olen && num < rc && ovector[pos] != -1)
	    cstr->concat(target + ovector[pos], ovector[pos + 1] - ovector[pos]);
      }
      else
	 cstr->concat(*(ptr++));
   }
   //printd(5, "QoreRegexSubst::concat() target: '%s' cstr: '%s'\n", target, cstr->c_str());
}

#define SUBST_OVECSIZE 30
#define SUBST_LASTELEM 20
// called directly for run-time evaluation
QoreStringNode *QoreRegexSubst::exec(const QoreString *target, const QoreString *nstr, ExceptionSink *xsink) const {
   TempEncodingHelper t(target, QCS_UTF8, xsink);
   if (*xsink)
      return 0;

   SimpleRefHolder<QoreStringNode> tstr(new QoreStringNode);

   const char *ptr = t->getBuffer();
   // detect infinite recursion (empty pattern matches)
   int last_match = -1;

   //printd(5, "QoreRegexSubst::exec(%s) this=%p: global=%s\n", ptr, this, global ? "true" : "false");
   while (true) {
      int ovector[SUBST_OVECSIZE];
      int offset = ptr - t->getBuffer();
      if ((unsigned)offset >= t->size())
         break;
      int rc = pcre_exec(p, 0, t->getBuffer(), t->strlen(), offset, 0, ovector, SUBST_OVECSIZE);

      //printd(5, "QoreRegexSubst::exec() prec_exec() rc: %d ovector[0]: %d\n", rc, ovector[0]);
      // FIXME: rc = 0 means that not enough space was available in ovector!
      if (rc < 1)
	 break;

      // detect infinite recursion
      if (ovector[0] == last_match) {
	 xsink->raiseException("REGEX-SUBST-ERROR", "infinite recursion detected in regex substitution string; this normally happens with an empty pattern with use with the global option (RE_GLOBAL)");
	 return 0;
      }
      else
	 last_match = ovector[0];

      if (ovector[0] > offset)
	 tstr->concat(ptr, ovector[0] - offset);

      concat(*tstr, ovector, SUBST_LASTELEM, nstr->getBuffer(), t->getBuffer(), rc);

      //printd(5, "QoreRegexSubst::exec() '%s' =~ s/?/%s/%s offset=%d, 0=%d, 1=%d ('%s')\n", t->getBuffer(), nstr->getBuffer(), global ? "g" : "", offset, ovector[0], ovector[1], tstr->getBuffer());

      ptr = t->getBuffer() + ovector[1];

      if (!global)
	 break;
   }

   //printd(5, "QoreRegexSubst::exec() *ptr=%d ('%s') tstr='%s'\n", *ptr, ptr, tstr->getBuffer());
   if (*ptr)
      tstr->concat(ptr);

   //printd(5, "QoreRegexSubst::exec() this=%p: returning '%s'\n", this, tstr->getBuffer());
   return tstr.release();
}

// called for run-time evaluation of parse-time-created objects
QoreStringNode *QoreRegexSubst::exec(const QoreString *target, ExceptionSink *xsink) const {
   return exec(target, newstr, xsink);
}

void QoreRegexSubst::setGlobal() {
   global = true;
}

QoreString *QoreRegexSubst::getPattern() const {
   return str;
}
