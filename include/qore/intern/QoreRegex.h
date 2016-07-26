/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreRegex.h

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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

/*
  PCRE-based matching (Perl-compatible regular expression matching)
  see: http://www.pcre.org for more information on this library

  NOTE: all regular expression matching is done with UTF-8 encoding, so character set
  encodings are converted if necessary
 */

#ifndef _QORE_QOREREGEX_H

#define _QORE_QOREREGEX_H

#include <qore/intern/QoreRegexBase.h>

class QoreRegex : public QoreRegexBase, public QoreReferenceCounter {
private:
   bool global;

   DLLLOCAL void init(int64 opt = PCRE_UTF8);

public:
   DLLLOCAL QoreRegex();
   // this version is used while parsing, takes ownership of str
   DLLLOCAL QoreRegex(QoreString* str);
   // used at run-time, does not change str
   DLLLOCAL QoreRegex(const QoreString& str, int64 options, ExceptionSink* xsink);

   DLLLOCAL ~QoreRegex();

   DLLLOCAL void concat(char c);
   DLLLOCAL void parse();
   DLLLOCAL void parseRT(const QoreString* pattern, ExceptionSink* xsink);
   DLLLOCAL bool exec(const QoreString* target, ExceptionSink* xsink) const;
   DLLLOCAL bool exec(const char* str, size_t len) const;
   DLLLOCAL QoreListNode* extractSubstrings(const QoreString* target, ExceptionSink* xsink) const;
   // caller owns QoreString returned
   DLLLOCAL QoreString* getString();

   DLLLOCAL void setGlobal() {
      global = true;
   }

   DLLLOCAL void ref() const {
      ROreference();
   }

   DLLLOCAL void deref() {
      if (ROdereference())
         delete this;
   }

   DLLLOCAL QoreRegex* refSelf() const {
      ref();
      return const_cast<QoreRegex*>(this);
   }
};

#endif // _QORE_QOREREGEX_H
