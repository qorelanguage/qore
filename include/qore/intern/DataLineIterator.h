/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  DataLineIterator.h

  Qore Programming Language

  Copyright (C) 2016 Qore Technologies, s.r.o.

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

#ifndef _QORE_DATALINEITERATOR_H
#define _QORE_DATALINEITERATOR_H

#include <string.h>
#include <errno.h>

#include "qore/intern/StringInputStream.h"
#include "qore/intern/InputStreamLineIterator.h"

/**
 * @brief Private data for the Qore::DataLineIterator class.
 */
class DataLineIterator : public QoreIteratorBase {

public:
   DLLLOCAL DataLineIterator(ExceptionSink* xsink, const QoreStringNode* n_str, const QoreStringNode* n_eol = 0, bool n_trim = true) :
      src(0),
      str(n_str->stringRefSelf()),
      eol(n_eol ? n_eol->stringRefSelf() : 0),
      trim(n_trim) {
      doReset(xsink);
   }

   DLLLOCAL DataLineIterator(ExceptionSink* xsink, const DataLineIterator& old) :
      src(0),
      str(old.str->stringRefSelf()),
      eol(old.eol ? old.eol->stringRefSelf() : 0),
      trim(old.trim) {
      doReset(xsink);
   }

   DLLLOCAL ~DataLineIterator() {
      if (eol)
         eol->deref();
      if (str)
         str->deref();
   }

   DLLLOCAL bool next(ExceptionSink* xsink) {
      bool validp = src->next(xsink);
      if (!validp) {
         doReset(xsink);
      }
      return validp;
   }

   DLLLOCAL int64 index() {
      return src->index();
   }

   DLLLOCAL QoreStringNode* getValue() {
      return src->getValue();
   }

   DLLLOCAL bool valid() {
      return src->valid();
   }

   DLLLOCAL int checkValid(ExceptionSink* xsink) {
      return src->checkValid(xsink);
   }

   DLLLOCAL void reset(ExceptionSink* xsink) {
      if (src->valid()) {
         doReset(xsink);
      }
   }

   DLLLOCAL const QoreEncoding* getEncoding() {
      return src->getEncoding();
   }

   DLLLOCAL virtual void deref() {
      if (ROdereference())
         delete this;
   }

   DLLLOCAL virtual const char* getName() const { return "DataLineIterator"; }

private:
   DLLLOCAL void doReset(ExceptionSink* xsink) {
      src = new InputStreamLineIterator(xsink, new StringInputStream(str), str->getEncoding(), eol, trim);
   }

private:
   SimpleRefHolder<InputStreamLineIterator> src;
   QoreStringNode* str;
   QoreStringNode* eol;
   bool trim;
};

#endif // _QORE_DATALINEITERATOR_H
