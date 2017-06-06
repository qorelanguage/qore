/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_encoding_private.h

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

#ifndef _QORE_INTERN_QORE_ENCODING_PRIVATE_H
#define _QORE_INTERN_QORE_ENCODING_PRIVATE_H

struct qore_encoding_private {
   mbcs_get_unicode_t get_unicode;
   unsigned char minwidth;
   bool ascii_compat;

   DLLLOCAL qore_encoding_private(unsigned char n_minwidth = 1, mbcs_get_unicode_t gu = 0, bool ac = true) : get_unicode(gu), minwidth(n_minwidth), ascii_compat(ac) {
   }

   DLLLOCAL unsigned getMinCharWidth() const {
      return minwidth;
   }

   DLLLOCAL bool isAsciiCompat() const {
      return ascii_compat;
   }

   DLLLOCAL unsigned getUnicode(const char* p) const {
      assert(get_unicode);
      return get_unicode(p);
   }

   DLLLOCAL static qore_encoding_private* get(QoreEncoding& enc) {
      return enc.priv;
   }

   DLLLOCAL static const qore_encoding_private* get(const QoreEncoding& enc) {
      return enc.priv;
   }
};

#endif
