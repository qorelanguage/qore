/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  InputStreamLineIterator.h

  Qore Programming Language

  Copyright (C) 2016 Qore Technologies, sro

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

#ifndef _QORE_INPUTSTREAMLINEITERATOR_H
#define _QORE_INPUTSTREAMLINEITERATOR_H

#include <string.h>
#include <errno.h>

#include "qore/InputStream.h"
#include "qore/intern/StreamReader.h"
#include "qore/intern/EncodingConversionInputStream.h"

/**
 * @brief Private data for the Qore::InputStreamLineIterator class.
 */
class InputStreamLineIterator : public QoreIteratorBase {

public:
   DLLLOCAL InputStreamLineIterator(ExceptionSink* xsink, InputStream* is, const QoreEncoding* encoding = QCS_DEFAULT, const QoreStringNode* n_eol = 0, bool n_trim = true) :
      src(is, xsink),
      reader(xsink),
      srcEnc(encoding),
      enc((encoding && encoding->isAsciiCompat()) ? encoding : QCS_UTF8),
      line(0),
      eol(0),
      num(0),
      validp(false),
      trim(n_trim) {
      if (n_eol) {
         if (enc != n_eol->getEncoding()) {
            SimpleRefHolder<QoreStringNode> neol(n_eol->convertEncoding(enc, xsink));
            if (*xsink)
               return;
            eol = neol.release();
         }
         else {
            eol = n_eol->stringRefSelf();
         }
      }

      if (srcEnc != enc) {
         src = new EncodingConversionInputStream(src.release(), srcEnc, enc, xsink);
      }

      reader = new StreamReader(xsink, *src, enc);
   }

   DLLLOCAL ~InputStreamLineIterator() {
      if (eol)
         eol->deref();
      if (line)
         line->deref();
   }

   DLLLOCAL bool next(ExceptionSink* xsink) {
      // Make sure to use a new string if the iterator was already valid.
      if (validp && line && !line->empty()) {
         line->deref();
         line = 0;
      }
      validp = getLine(xsink);
      if (validp) {
         ++num;   // Increment line number.
      }
      else {
         num = 0;   // Reset iterator.
      }
      //printd(5, "InputStreamLineIterator::next() this: %p line: %d offset: %lld validp: %d '%s'\n", this, num, offset, validp, line->getBuffer());
      return validp;
   }

   DLLLOCAL int64 index() const {
      return num;
   }

   DLLLOCAL QoreStringNode* getValue() {
      assert(validp);
      return line ? line->stringRefSelf() : 0;
   }

   DLLLOCAL bool valid() const {
      return validp;
   }

   DLLLOCAL int checkValid(ExceptionSink* xsink) const {
      if (!validp) {
         xsink->raiseException("ITERATOR-ERROR", "the %s is not pointing at a valid element; make sure %s::next() returns True before calling this method", getName(), getName());
         return -1;
      }
      return 0;
   }

   DLLLOCAL const QoreEncoding* getEncoding() const {
      return enc;
   }

   DLLLOCAL virtual void deref() {
      if (ROdereference())
         delete this;
   }

   DLLLOCAL virtual const char* getName() const { return "InputStreamLineIterator"; }

private:
   DLLLOCAL bool getLine(ExceptionSink* xsink) {
      if (line)
         line->deref();
      line = reader->readLine(eol, trim, xsink);
      return (line != 0);
   }

private:
   ReferenceHolder<InputStream> src;
   ReferenceHolder<StreamReader> reader;
   const QoreEncoding* srcEnc;
   const QoreEncoding* enc;
   QoreStringNode* line;
   QoreStringNode* eol;
   int64 num;
   bool validp;
   bool trim;
};

#endif // _QORE_INPUTSTREAMLINEITERATOR_H
