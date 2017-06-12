/*
  QoreTransliteration.cpp

  regex-like transliteration class definition

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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
#include "qore/intern/QoreTransliteration.h"

#include <stdlib.h>
#include <strings.h>
#include <ctype.h>

// constructor used when parsing
QoreTransliteration::QoreTransliteration(const QoreProgramLocation& loc) : loc(loc) {
   //printd(5, "QoreTransliteration::QoreTransliteration() this: %p\n", this);
}

QoreTransliteration::~QoreTransliteration() {
   //printd(5, "QoreTransliteration::~QoreTransliteration() this: %p\n", this);
}

void QoreTransliteration::setTargetRange() {
   tr = true;
}

void QoreTransliteration::setSourceRange() {
   sr = true;
}

void QoreTransliteration::finishSource() {
   if (sr)
      parse_error(loc, "no end character for character range at end of source pattern in transliteration");
}

void QoreTransliteration::finishTarget() {
   if (tr)
      parse_error(loc, "no end character for character range at end of target pattern in transliteration");
}

void QoreTransliteration::doRange(QoreString& str, char end) {
   if (!str.strlen()) {
      parse_error(loc, "no start character for character range in transliteration");
      return;
   }
   char start = str.c_str()[str.strlen() - 1];
   str.terminate(str.strlen() - 1);
   if (start > end) {
      parse_error(loc, "invalid range '%c' - '%c' in transliteration operator", start, end);
      return;
   }
   do
      str.concat(start++);
   while (start <= end);
}

void QoreTransliteration::concatSource(char c) {
   if (sr) {
      doRange(source, c);
      sr = false;
   }
   else
      source.concat(c);
}

void QoreTransliteration::concatTarget(char c) {
   if (tr) {
      doRange(target, c);
      tr = false;
   }
   else
      target.concat(c);
}

QoreStringNode* QoreTransliteration::exec(const QoreString* str, ExceptionSink* xsink) const {
   //printd(5, "source='%s' target='%s' ('%s')\n", source->getBuffer(), target->getBuffer(), str->getBuffer());
   TempEncodingHelper tstr(str, QCS_DEFAULT, xsink);
   if (*xsink)
      return nullptr;

   SimpleRefHolder<QoreStringNode> ns(new QoreStringNode);
   for (qore_size_t i = 0; i < tstr->strlen(); ++i) {
      char c = (**tstr)[i];
      const char *p = strchr(source.c_str(), c);
      if (p) {
         qore_size_t pos = p - source.c_str();
         if (target.strlen() <= pos)
            pos = target.strlen() - 1;
         ns->concat(target[pos]);
      }
      else
         ns->concat(c);
   }

   return ns.release();
}
