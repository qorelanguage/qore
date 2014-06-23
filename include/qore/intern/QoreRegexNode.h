/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreRegexNode.h

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

/*
  PCRE-based matching (Perl-compatible regular expression matching)
  see: http://www.pcre.org for more information on this library

  NOTE: all regular expression matching is done with UTF-8 encoding, so character set
  encodings are converted if necessary
 */

#ifndef _QORE_QOREREGEXNODE_H

#define _QORE_QOREREGEXNODE_H

#include <qore/intern/QoreRegexBase.h>

class QoreRegexNode : public ParseNoEvalNode, public QoreRegexBase {
private:
   bool global;

   DLLLOCAL void init(int64 opt = PCRE_UTF8);

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      // FIXME: implement a type for this
      typeInfo = 0;
      return this;
   }

   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      // FIXME: implement a type for this
      return 0;
   }

public:
   DLLLOCAL QoreRegexNode();
   // this version is used while parsing, takes ownership of str
   DLLLOCAL QoreRegexNode(QoreString *str);
   // used at run-time, does not change str
   DLLLOCAL QoreRegexNode(const QoreString &str, int64 options, ExceptionSink *xsink);
   DLLLOCAL virtual ~QoreRegexNode();

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;
   // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;

   // returns the data type
   DLLLOCAL virtual qore_type_t getType() const;
   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const;      

   DLLLOCAL void concat(char c);
   DLLLOCAL void parse();
   DLLLOCAL void parseRT(const QoreString *pattern, ExceptionSink *xsink);
   DLLLOCAL bool exec(const QoreString *target, ExceptionSink *xsink) const;
   DLLLOCAL bool exec(const char *str, size_t len) const;
   DLLLOCAL QoreListNode *extractSubstrings(const QoreString *target, ExceptionSink *xsink) const;
   // caller owns QoreString returned
   DLLLOCAL QoreString *getString();

   DLLLOCAL void setGlobal() {
      global = true;
   }
};

#endif // _QORE_QOREREGEXNODE_H
