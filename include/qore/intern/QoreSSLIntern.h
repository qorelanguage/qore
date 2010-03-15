/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreSSLIntern.h

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _QORE_QORESSLINTERN_H

#define _QORE_QORESSLINTERN_H

class QoreBIO {
protected:
   BIO *b;

   DLLLOCAL QoreBIO(BIO *n_b) : b(n_b) {}

public:
   DLLLOCAL ~QoreBIO() { if (b) BIO_free(b); }
   DLLLOCAL int writePEMX509(X509 *cert) { return PEM_write_bio_X509(b, cert); }
   DLLLOCAL long getMemData(char **buf) { return BIO_get_mem_data(b, buf); }
   DLLLOCAL X509 *getX509(X509 **x) { return d2i_X509_bio(b, x); }
   DLLLOCAL BIO *getBIO() { return b; }
};

class QoreMemBIO : public QoreBIO {
public:
   DLLLOCAL QoreMemBIO() : QoreBIO(BIO_new(BIO_s_mem())) {}
   DLLLOCAL QoreMemBIO(const BinaryNode *b) : QoreBIO(BIO_new_mem_buf((void *)b->getPtr(), (int)b->size())) {}
   DLLLOCAL QoreMemBIO(const QoreString *str) : QoreBIO(BIO_new_mem_buf((void *)str->getBuffer(), (int)str->strlen())) {}
};

#endif
