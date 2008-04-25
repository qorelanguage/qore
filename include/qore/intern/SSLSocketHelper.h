/*
  SSLSocketHelper.h

  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

  will unlink (delete) UNIX domain socket files when closed

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

#ifndef _QORE_SSLSOCKETHELPER_H

#define _QORE_SSLSOCKETHELPER_H

class SSLSocketHelper
{
   private:
      SSL_METHOD *meth;
      SSL_CTX *ctx;
      SSL *ssl;

      DLLLOCAL int setIntern(int sd, X509* cert, EVP_PKEY *pk, class ExceptionSink *xsink);

   public:
      DLLLOCAL SSLSocketHelper();
      DLLLOCAL ~SSLSocketHelper();
      DLLLOCAL void sslError(class ExceptionSink *xsink);
      DLLLOCAL int setClient(int sd, X509* cert, EVP_PKEY *pk, class ExceptionSink *xsink);
      DLLLOCAL int setServer(int sd, X509* cert, EVP_PKEY *pk, class ExceptionSink *xsink);
      // returns 0 for success
      DLLLOCAL int connect(class ExceptionSink *xsink);
      // returns 0 for success
      DLLLOCAL int accept(class ExceptionSink *xsink);
      // returns 0 for success
      DLLLOCAL int shutdown();
      // returns 0 for success
      DLLLOCAL int shutdown(class ExceptionSink *xsink);
      // returns 0 for success
      DLLLOCAL int read(char *buf, int size);
      // returns 0 for success
      DLLLOCAL int write(const void *buf, int size, class ExceptionSink *xsink);
      DLLLOCAL int write(const void *buf, int size);
      DLLLOCAL const char *getCipherName() const;
      DLLLOCAL const char *getCipherVersion() const;
      DLLLOCAL X509 *getPeerCertificate() const;
      DLLLOCAL long verifyPeerCertificate() const;
};

#endif
