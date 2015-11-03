/*
  SSLSocketHelper.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#ifdef NEED_SSL_CTX_NEW_CONST
#define SSL_METHOD_CONST const
#else
#define SSL_METHOD_CONST
#endif

class SSLSocketHelper {
   private:
      SSL_METHOD_CONST SSL_METHOD *meth;
      SSL_CTX *ctx;
      SSL *ssl;

      DLLLOCAL int setIntern(int sd, X509* cert, EVP_PKEY *pk, ExceptionSink *xsink);

   public:
      DLLLOCAL SSLSocketHelper();
      DLLLOCAL ~SSLSocketHelper();
      DLLLOCAL void sslError(ExceptionSink *xsink, const char *msg);
      DLLLOCAL int setClient(int sd, X509* cert, EVP_PKEY *pk, ExceptionSink *xsink);
      DLLLOCAL int setServer(int sd, X509* cert, EVP_PKEY *pk, ExceptionSink *xsink);
      // returns 0 for success
      DLLLOCAL int connect(ExceptionSink *xsink);
      // returns 0 for success
      DLLLOCAL int accept(ExceptionSink *xsink);
      // returns 0 for success
      DLLLOCAL int shutdown();
      // returns 0 for success
      DLLLOCAL int shutdown(ExceptionSink *xsink);
      // returns 0 for success
      DLLLOCAL int read(char *buf, int size);
      // returns 0 for success
      DLLLOCAL int write(const void *buf, int size, ExceptionSink *xsink);
      DLLLOCAL int write(const void *buf, int size);
      DLLLOCAL const char *getCipherName() const;
      DLLLOCAL const char *getCipherVersion() const;
      DLLLOCAL X509 *getPeerCertificate() const;
      DLLLOCAL long verifyPeerCertificate() const;
};

#endif
