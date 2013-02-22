/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  SSLSocketHelper.h

  Qore Programming Language

  Copyright 2003 - 2013 David Nichols

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

struct qore_socket_private;

class SSLSocketHelper {
private:
   qore_socket_private& qs;
   SSL_METHOD_CONST SSL_METHOD* meth;
   SSL_CTX* ctx;
   SSL* ssl;

   DLLLOCAL int setIntern(const char* meth, int sd, X509* cert, EVP_PKEY* pk, ExceptionSink* xsink);

   // do blocking or non-blocking SSL I/O and handle SSL_ERROR_WANT_READ and SSL_ERROR_WANT_WRITE properly
   DLLLOCAL int doSSLRW(const char* mname, void* buf, int num, int timeout_ms, bool read, ExceptionSink* xsink);

public:
   DLLLOCAL SSLSocketHelper(qore_socket_private& n_qs) : qs(n_qs), meth(0), ctx(0), ssl(0) {
   }

   ~SSLSocketHelper() {
      if (ssl)
         SSL_free(ssl);
      if (ctx)
         SSL_CTX_free(ctx);
   }

   DLLLOCAL bool sslError(ExceptionSink *xsink, const char* meth, const char *msg, bool always_error = true);
   DLLLOCAL int setClient(const char* mname, int sd, X509* cert, EVP_PKEY *pk, ExceptionSink *xsink);
   DLLLOCAL int setServer(const char* mname, int sd, X509* cert, EVP_PKEY *pk, ExceptionSink *xsink);
   // returns 0 for success
   DLLLOCAL int connect(const char* mname, ExceptionSink *xsink);
   // returns 0 for success
   DLLLOCAL int accept(const char* mname, ExceptionSink *xsink);
   // returns 0 for success
   DLLLOCAL int shutdown();
   // returns 0 for success
   DLLLOCAL int shutdown(ExceptionSink *xsink);
   // read with optional timeout in milliseconds
   DLLLOCAL int read(const char* mname, char *buf, int size, int timeout_ms, ExceptionSink* xsink);
   // returns 0 for success
   DLLLOCAL int write(const char* mname, const void* buf, int size, int timeout_ms, ExceptionSink* xsink);
   DLLLOCAL const char *getCipherName() const;
   DLLLOCAL const char *getCipherVersion() const;
   DLLLOCAL X509 *getPeerCertificate() const;
   DLLLOCAL long verifyPeerCertificate() const;
};

#endif
