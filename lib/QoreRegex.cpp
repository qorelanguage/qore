/* -*-indent-tabs-mode: nil -*- */
/*
  QoreRegex.cpp

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
#include "qore/intern/qore_program_private.h"

#include <memory>

QoreRegex::QoreRegex() {
   init();
   str = new QoreString();
}

QoreRegex::QoreRegex(QoreString* s) {
   init();
   str = s;
   parse();
}

QoreRegex::QoreRegex(const QoreString& s, int64 opts, ExceptionSink* xsink) {
   init(opts);
   str = 0;

   if (check_re_options(options)) {
      xsink->raiseException("REGEX-OPTION-ERROR", "%d contains invalid option bits", opts);
      options = 0;
   }

   parseRT(&s, xsink);
}

QoreRegex::~QoreRegex() {
   if (p)
      pcre_free(p);
   if (str)
      delete str;
}

void QoreRegex::concat(char c) {
   str->concat(c);
}

void QoreRegex::parseRT(const QoreString* pattern, ExceptionSink* xsink) {
   const char* err;
   int eo;

   // convert to UTF-8 if necessary
   TempEncodingHelper t(pattern, QCS_UTF8, xsink);
   if (*xsink)
      return;

   //printd(5, "QoreRegex::parseRT(%s) this=%p\n", t->getBuffer(), this);

   p = pcre_compile(t->getBuffer(), options, &err, &eo, 0);
   if (err) {
      //printd(5, "QoreRegex::parse() error parsing '%s': %s", t->getBuffer(), (char* )err);
      xsink->raiseException("REGEX-COMPILATION-ERROR", (char* )err);
   }
}

void QoreRegex::parse() {
   ExceptionSink xsink;
   parseRT(str, &xsink);
   delete str;
   str = 0;
   if (xsink.isEvent())
      qore_program_private::addParseException(getProgram(), xsink);
}

bool QoreRegex::exec(const QoreString* target, ExceptionSink* xsink) const {
   TempEncodingHelper t(target, QCS_UTF8, xsink);
   if (!t)
      return false;

   return exec(t->getBuffer(), t->strlen());
}

// default subpattern buffer size; see pcre_exec() / pcreapi for more info
#define OVECCOUNT 30
// maximum subpattern buffer size; we must limit this to control the amount of stack space used; must be OVECCOUNT * a power of 2
#define OVECMAX 480
bool QoreRegex::exec(const char* str, size_t len) const {
   // the PCRE docs say that if we don't send an ovector here the library may have to malloc
   // memory, so, even though we don't need the results, we include the vector to avoid
   // extraneous malloc()s

   int rc;

   int vsize = OVECCOUNT;
   while (true) {
#ifdef HAVE_LOCAL_VARIADIC_ARRAYS
      int ovector[vsize];
#else
      std::vector<int> ovc(vsize, 0);
      int* ovector = &ovc[0];
#endif
      rc = pcre_exec(p, 0, str, len, 0, 0, ovector, vsize);
      if (!rc) {
         // rc == 0 means not enough space was available in ovector
         printd(0, "QoreRegex::exec() ovector too small: vsize: %d -> %d (max: %d)\n", vsize, vsize << 1, OVECMAX);
         vsize <<= 1;
         if (vsize >= OVECMAX) {
            rc = -1;
            break;
         }
         continue;
      }
      break;
   }

   //printd(5, "QoreRegex::exec(%s) this=%p pre_exec() rc=%d\n", str, this, rc);
   return rc >= 0;
}

QoreListNode* QoreRegex::extractSubstrings(const QoreString* target, ExceptionSink* xsink) const {
   TempEncodingHelper t(target, QCS_UTF8, xsink);
   if (!t)
      return 0;

   ReferenceHolder<QoreListNode> l(xsink);

   int offset = 0;

   int vsize = OVECCOUNT;

   while (true) {
      if (offset >= (int)t->strlen())
         break;
#ifdef HAVE_LOCAL_VARIADIC_ARRAYS
      int ovector[vsize];
#else
      std::vector<int> ovc(vsize, 0);
      int* ovector = &ovc[0];
#endif
      int rc = pcre_exec(p, 0, t->getBuffer(), t->strlen(), offset, 0, ovector, vsize);
      //printd(5, "QoreRegex::exec(%s) =~ /xxx/ = %d (global: %d)\n", t->getBuffer() + offset, rc, global);

      if (!rc) {
         // rc == 0 means not enough space was available in ovector
         //printd(5, "QoreRegex::extractSubstrings() ovector too small: vsize: %d -> %d (max: %d)\n", vsize, vsize << 1, OVECMAX);
         vsize <<= 1;
         if (vsize >= OVECMAX) {
            xsink->raiseException("REGEX-ERROR", "too many results required in regular expression (vsize: %d limit: %d)", vsize, OVECMAX);
            return 0;
         }
         continue;
      }

      if (rc < 1)
         break;

      if (rc > 1) {
         int x = 0;
         while (++x < rc) {
            int pos = x * 2;
            if (ovector[pos] == -1) {
               if (!l)
                  l = new QoreListNode(stringTypeInfo);
               l->push(nothing());
               continue;
            }
            QoreStringNode* tstr = new QoreStringNode;
            //printd(5, "substring %d: %d - %d (len %d)\n", x, ovector[pos], ovector[pos + 1], ovector[pos + 1] - ovector[pos]);
            tstr->concat(t->getBuffer() + ovector[pos], ovector[pos + 1] - ovector[pos]);
            if (!l)
               l = new QoreListNode(stringTypeInfo);
            l->push(tstr);
         }

         offset = ovector[(x - 1) * 2 + 1];
         //printd(5, "QoreRegex::exec() offset: %d size: %d ovector[%d]: %d\n", offset, t->strlen(), (x - 1) * 2 + 1, ovector[(x - 1) * 2 + 1]);
      }
      else
         break;

      if (!global)
         break;
   }

   return l.release();
}

void QoreRegex::init(int64 opt) {
   p = 0;
   options = (int)opt;
   global = opt & QRE_GLOBAL ? true : false;
}

QoreString* QoreRegex::getString() {
   QoreString* rs = str;
   str = 0;
   return rs;
}
