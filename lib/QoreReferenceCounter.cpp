/*
  QoreReferenceCounter.cpp

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

QoreReferenceCounter::QoreReferenceCounter() : references(1) {
}

QoreReferenceCounter::~QoreReferenceCounter() {
}

int QoreReferenceCounter::reference_count() const {
   return references.load();
}

bool QoreReferenceCounter::is_unique() const {
   return references.load() == 1;
}

void QoreReferenceCounter::ROreference() const {
#ifdef DEBUG
   if (references.load() < 0 || references.load() > 10000000) {
      printd(0, "QoreReferenceCounter::ROreference() this=%p references=%d\n", this, references.load());
      assert(false);
   }
#endif
   ++references;
}

// returns true when references reach zero
bool QoreReferenceCounter::ROdereference() const {
#ifdef DEBUG
   if (references.load() <= 0 || references.load() > 10000000) {
      printd(0, "QoreReferenceCounter::ROdereference() this=%p references=%d\n", this, references.load());
      assert(false);
   }
#endif
   return references.fetch_sub(1) == 1 ? true : false;
}
