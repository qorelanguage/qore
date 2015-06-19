/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  SSLSocketHelper.h

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

  will unlink (delete) UNIX domain socket files when closed

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

   DLLLOCAL bool sslError(ExceptionSink *xsink, bool& closed, const char* meth, const char *msg, bool always_error = true);
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
