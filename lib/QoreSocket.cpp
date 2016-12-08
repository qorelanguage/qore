/* -*- indent-tabs-mode: nil -*- */
/*
  QoreSocket.cpp

  Socket Class for IPv4, IPv6 and UNIX domain sockets with SSL support

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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

// FIXME: change int to qore_size_t where applicable! (ex: int rc = recv())

#include <qore/Qore.h>
#include <qore/QoreSocket.h>

#include "qore/intern/qore_socket_private.h"

void se_in_op(const char* cname, const char* meth, ExceptionSink* xsink) {
   assert(xsink);
   xsink->raiseException("SOCKET-IN-CALLBACK", "calls to %s::%s() cannot be made from a callback on an operation on the same socket", cname, meth);
}

void se_in_op_thread(const char* cname, const char* meth, ExceptionSink* xsink) {
   assert(xsink);
   xsink->raiseException("SOCKET-IN-CALLBACK", "calls to %s::%s() cannot be made from another thread while a callback operation is in progress on the same socket", cname, meth);
}

void se_not_open(const char* cname, const char* meth, ExceptionSink* xsink) {
   assert(xsink);
   xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before %s::%s() call", cname, meth);
}

void se_timeout(const char* cname, const char* meth, int timeout_ms, ExceptionSink* xsink) {
   assert(xsink);
   xsink->raiseException("SOCKET-TIMEOUT", "timed out after %d millisecond%s in %s::%s() call", timeout_ms, timeout_ms == 1 ? "" : "s", cname, meth);
}

void se_closed(const char* cname, const char* mname, ExceptionSink* xsink) {
   xsink->raiseException("SOCKET-CLOSED", "error in %s::%s(): remote end closed the connection", cname, mname);
}

#ifdef _Q_WINDOWS
int sock_get_raw_error() {
   return WSAGetLastError();
}

int sock_get_error() {
   int rc = WSAGetLastError();

   switch (rc) {
      case 0:
	 errno = 0;
	 break;

      case WSANOTINITIALISED:
      case WSAEINVAL:
      case WSAENOTSOCK:
      case WSAEADDRNOTAVAIL:
      case WSAEAFNOSUPPORT:
      case WSAEOPNOTSUPP:
	 errno = EINVAL;
	 break;

      case WSAEADDRINUSE:
	 errno = EIO;
	 break;

      case WSAENETDOWN:
	 errno = ENODEV;
	 break;

      case WSAEFAULT:
	 errno = EFAULT;
	 break;

      case WSAENOBUFS:
	 errno = ENOMEM;
	 break;

      case WSAETIMEDOUT:
	 errno = ETIMEDOUT;
	 break;

      case WSAECONNREFUSED:
	 errno = ECONNREFUSED;
	 break;

      case WSAEBADF:
	 errno = EBADF;
	 break;

      case WSAECONNRESET:
      case WSAECONNABORTED:
	 errno = ECONNRESET;
	 break;

      case WSAEWOULDBLOCK:
	 errno = EAGAIN;
	 break;

#ifdef DEBUG
      case WSAEALREADY:
      case WSAEINTR:
      case WSAEINPROGRESS:
	 // should never get these here
	 printd(0, "sock_get_error() got unexpected error code %d; about to assert()\n", rc);
	 assert(false);
	 errno = EFAULT;
	 break;
#endif

      default:
	 printd(0, "sock_get_error() unknown code %d; about to assert()\n", rc);
	 assert(false);
	 errno = EFAULT;
	 break;
   }

   return errno;
}

int check_windows_rc(int rc) {
   if (rc != SOCKET_ERROR)
      return 0;

   sock_get_error();
   return -1;
}

void qore_socket_error_intern(int rc, ExceptionSink* xsink, const char* err, const char* cdesc, const char* mname, const char* host, const char* svc, const struct sockaddr *addr) {
   sock_get_error();
   if (!xsink)
      return;

   QoreStringNode* desc = new QoreStringNode;
   if (mname)
      desc->sprintf("error while executing Socket::%s(): ", mname);

   desc->concat(cdesc);

   if (addr) {
      assert(!host);
      assert(!svc);

      concat_target(*desc, addr);
   }
   else
      if (host && host[0]) {
         desc->sprintf(" (target: %s", host);
         if (svc)
            desc->sprintf(":%s", svc);
         desc->concat(")");
      }

   if (!errno) {
      xsink->raiseException(err, desc);
      return;
   }

   desc->concat(": ");
   char* buf;
   // get windows error message
   if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, rc, LANG_USER_DEFAULT, (LPTSTR)&buf, 0, 0)) {
      assert(!buf);
      desc->sprintf("Windows FormatMessage() failed on error code %d", rc);
   }
   else
      assert(buf);

   desc->concat(buf);
   free(buf);

   xsink->raiseException(err, desc);
}

void qore_socket_error(ExceptionSink* xsink, const char* err, const char* cdesc, const char* mname, const char* host, const char* svc, const struct sockaddr *addr) {
   qore_socket_error_intern(WSAGetLastError(), xsink, err, cdesc, mname, host, svc, addr);
}
#else
int sock_get_raw_error() {
   return errno;
}

int sock_get_error() {
   return errno;
}

void qore_socket_error_intern(int rc, ExceptionSink* xsink, const char* err, const char* cdesc, const char* mname, const char* host, const char* svc, const struct sockaddr *addr) {
   assert(rc);
   if (!xsink)
      return;

   QoreStringNode* desc = new QoreStringNode;
   if (mname)
      desc->sprintf("error while executing Socket::%s(): ", mname);

   desc->concat(cdesc);

   if (addr) {
      assert(!host);
      assert(!svc);

      concat_target(*desc, addr);
   }
   else
      if (host) {
         desc->sprintf(" (target: %s", host);
         if (svc)
            desc->sprintf(":%s", svc);
         desc->concat(")");
      }

   xsink->raiseErrnoException(err, rc, desc);
}

void qore_socket_error(ExceptionSink* xsink, const char* err, const char* cdesc, const char* mname, const char* host, const char* svc, const struct sockaddr *addr) {
   qore_socket_error_intern(errno, xsink, err, cdesc, mname, host, svc, addr);
}
#endif

int do_read_error(qore_offset_t rc, const char* method_name, int timeout_ms, ExceptionSink* xsink) {
   if (rc > 0)
      return 0;
   if (!*xsink)
      QoreSocket::doException(rc, method_name, timeout_ms, xsink);
   return -1;
}

void concat_target(QoreString& str, const struct sockaddr *addr, const char* type) {
   QoreString host;
   q_addr_to_string2(addr, host);
   if (!host.empty())
      str.sprintf(" (%s: %s:%d)", type, host.getBuffer(), q_get_port_from_addr(addr));
}

qore_socket_op_helper::qore_socket_op_helper(qore_socket_private* sock) : s(sock) {
   assert(s->in_op == -1);
   s->in_op = gettid();;
}

qore_socket_op_helper::~qore_socket_op_helper() {
   s->in_op = -1;
}

SSLSocketHelperHelper::SSLSocketHelperHelper(qore_socket_private* sock) : s(sock) {
   assert(!s->ssl);
   ssl = s->ssl = new SSLSocketHelper(*sock);
}

void SSLSocketHelperHelper::error() {
   ssl->deref();
   if (s->ssl)
      s->ssl = 0;
}

int SSLSocketHelper::setIntern(const char* mname, int sd, X509* cert, EVP_PKEY* pk, ExceptionSink* xsink) {
   SSLSocketReferenceHelper ssrh(this);

   assert(!ssl);
   assert(!ctx);
   ctx = SSL_CTX_new(meth);
   if (!ctx) {
      sslError(xsink, mname, "SSL_CTX_new");
      assert(*xsink);
      return -1;
   }
   if (cert) {
      if (!SSL_CTX_use_certificate(ctx, cert)) {
	 sslError(xsink, mname, "SSL_CTX_use_certificate");
	 assert(*xsink);
	 return -1;
      }
   }
   if (pk) {
      if (!SSL_CTX_use_PrivateKey(ctx, pk)) {
	 sslError(xsink, mname, "SSL_CTX_use_PrivateKey");
	 assert(*xsink);
	 return -1;
      }
   }

   ssl = SSL_new(ctx);
   if (!ssl) {
      sslError(xsink, mname, "SSL_new");
      assert(*xsink);
      return -1;
   }

   // turn on SSL_MODE_ENABLE_PARTIAL_WRITE
   SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE);

   // turn on SSL_MODE_AUTO_RETRY for blocking I/O
   SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

   SSL_set_fd(ssl, sd);
   return 0;
}

int SSLSocketHelper::setClient(const char* mname, int sd, X509* cert, EVP_PKEY* pk, ExceptionSink* xsink) {
   meth = SSLv23_client_method();
   return setIntern(mname, sd, cert, pk, xsink);
}

int SSLSocketHelper::setServer(const char* mname, int sd, X509* cert, EVP_PKEY* pk, ExceptionSink* xsink) {
   meth = SSLv23_server_method();
   return setIntern(mname, sd, cert, pk, xsink);
}

// returns 0 for success
int SSLSocketHelper::connect(const char* mname, int timeout_ms, ExceptionSink* xsink) {
   SSLSocketReferenceHelper ssrh(this);

   int rc;

   if (timeout_ms >= 0) {
      if (qs.set_non_blocking(true, xsink))
	 return qs.close_and_exit();

      while (true) {
	 rc = SSL_connect(ssl);

	 if (rc == -1 && !(rc = doSSLUpgradeNonBlockingIO(rc, mname, timeout_ms, "SSL_connect", xsink))) {
	    if (!qs.isOpen())
	       break;
	    continue;
	 }

	 break;
      }

      if (qs.isOpen() && qs.set_non_blocking(false, xsink))
	 return qs.close_and_exit();
   }
   else
      rc = SSL_connect(ssl);

   if (rc <= 0) {
      if (!*xsink)
	 sslError(xsink, mname, "SSL_connect", true);
      return -1;
   }

   return 0;
}

// returns 0 for success
int SSLSocketHelper::accept(const char* mname, int timeout_ms, ExceptionSink* xsink) {
   SSLSocketReferenceHelper ssrh(this);

   int rc;

   if (timeout_ms >= 0) {
      if (qs.set_non_blocking(true, xsink))
	 return qs.close_and_exit();

      while (true) {
	 rc = SSL_accept(ssl);

	 if (rc == -1 && !(rc = doSSLUpgradeNonBlockingIO(rc, mname, timeout_ms, "SSL_accept", xsink))) {
	    if (!qs.isOpen())
	       break;
	    continue;
	 }

	 break;
      }

      if (qs.isOpen() && qs.set_non_blocking(false, xsink))
	 return qs.close_and_exit();
   }
   else
      rc = SSL_accept(ssl);

   if (rc <= 0) {
      //printd(5, "SSLSocketHelper::accept() rc: %d\n", rc);
      if (!*xsink)
	 sslError(xsink, mname, "SSL_accept", true);
      assert(*xsink);
      return -1;
   }

   return 0;
}

// returns 0 for success
int SSLSocketHelper::shutdown() {
   if (SSL_shutdown(ssl) < 0)
      return -1;
   return 0;
}

// returns 0 for success
int SSLSocketHelper::shutdown(ExceptionSink* xsink) {
   if (SSL_shutdown(ssl) < 0) {
      SSLSocketReferenceHelper ssrh(this);
      sslError(xsink, "shutdownSSL", "SSL_shutdown");
      return -1;
   }
   return 0;
}

// returns 0 for success
int SSLSocketHelper::write(const char* mname, const void* buf, int size, int timeout_ms, ExceptionSink* xsink) {
   return doSSLRW(xsink, mname, (void*)buf, size, timeout_ms, false);
}

const char* SSLSocketHelper::getCipherName() const {
   return SSL_get_cipher_name(ssl);
}

const char* SSLSocketHelper::getCipherVersion() const {
   return SSL_get_cipher_version(ssl);
}

X509* SSLSocketHelper::getPeerCertificate() const {
   return SSL_get_peer_certificate(ssl);
}

long SSLSocketHelper::verifyPeerCertificate() const {
   X509* cert = SSL_get_peer_certificate(ssl);

   if (!cert)
      return -1;

   long rc = SSL_get_verify_result(ssl);
   X509_free(cert);
   return rc;
}

SocketSource::SocketSource() : priv(new qore_socketsource_private) {
}

SocketSource::~SocketSource() {
   delete priv;
}

QoreStringNode* SocketSource::takeAddress() {
   QoreStringNode* addr = priv->address;
   priv->address = 0;
   return addr;
}

QoreStringNode* SocketSource::takeHostName() {
   QoreStringNode* host = priv->hostname;
   priv->hostname = 0;
   return host;
}

const char* SocketSource::getAddress() const {
   return priv->address ? priv->address->getBuffer() : 0;
}

const char* SocketSource::getHostName() const {
   return priv->hostname ? priv->hostname->getBuffer() : 0;
}

void SocketSource::setAll(QoreObject *o, ExceptionSink* xsink) {
   return priv->setAll(o, xsink);
}

int qore_socket_private::send(int fd, qore_offset_t size, int timeout_ms, ExceptionSink* xsink) {
   if (!size)
      return 0;
   if (sock == QORE_INVALID_SOCKET) {
      printd(5, "QoreSocket::send() ERROR: sock: %d size: " QSD "\n", sock, size);
      se_not_open("Socket", "send", xsink);
      return -1;
   }

   char* buf = (char*)malloc(sizeof(char) * DEFAULT_SOCKET_BUFSIZE);
   ON_BLOCK_EXIT(free, buf);

   qore_offset_t rc = 0;
   qore_size_t bs = 0;
   while (true) {
      // calculate bytes needed
      qore_size_t bn;
      if (size < 0)
	 bn = DEFAULT_SOCKET_BUFSIZE;
      else {
	 bn = size - bs;
	 if (bn > DEFAULT_SOCKET_BUFSIZE)
	    bn = DEFAULT_SOCKET_BUFSIZE;
      }
      while (true) {
         rc = ::read(fd, buf, bn);
         if (rc >= 0)
            break;
         if (errno != EINTR) {
            xsink->raiseErrnoException("FILE-READ-ERROR", errno, "error reading file after " QLLD " bytes read in Socket::send()", bs);
            break;
         }
      }
      if (rc < 0) {
         //printd(5, "QoreSocket::send() read error: %s\n", strerror(errno));
         break;
      }

      // send buffer
      int src = send(xsink, "Socket", "send", buf, rc, timeout_ms);
      if (src < 0) {
	 printd(5, "QoreSocket::send() send error: %s\n", strerror(errno));
	 break;
      }
      bs += rc;
      if (size > 0 && bs >= (qore_size_t)size) {
	 rc = 0;
	 break;
      }
   }
   return rc;
}

int qore_socket_private::recv(int fd, qore_offset_t size, int timeout_ms, ExceptionSink* xsink) {
   if (!size)
      return 0;
   if (sock == QORE_INVALID_SOCKET) {
      printd(5, "QoreSocket::send() ERROR: sock: %d size: " QSD "\n", sock, size);
      se_not_open("Socket", "recv", xsink);
      return -1;
   }

   char* buf;
   qore_offset_t br = 0;
   qore_offset_t rc;
   while (true) {
      // calculate bytes needed
      int bn;
      if (size == -1)
	 bn = DEFAULT_SOCKET_BUFSIZE;
      else {
	 bn = size - br;
	 if (bn > DEFAULT_SOCKET_BUFSIZE)
	    bn = DEFAULT_SOCKET_BUFSIZE;
      }

      rc = brecv(xsink, "recv", buf, bn, 0, timeout_ms);
      if (rc <= 0)
	 break;
      br += rc;

      // write buffer to file descriptor
      while (true) {
         rc = ::write(fd, buf, rc);
         if (rc > 0)
            break;
         // write(2) should not return 0, but in case it does, it's treated as an error
         if (errno != EINTR) {
            xsink->raiseErrnoException("FILE-READ-ERROR", errno, "error reading file after " QLLD " bytes read in Socket::send()", br);
            break;
         }
      }

      if (size > 0 && br >= size) {
	 rc = 0;
	 break;
      }
   }
   return (int)rc;
}

void QoreSocket::doException(int rc, const char* meth, int timeout_ms, ExceptionSink* xsink) {
   switch (rc) {
      case 0:
	 se_closed("Socket", meth, xsink);
	 break;
      case QSE_RECV_ERR: // recv() error
	 xsink->raiseException("SOCKET-RECV-ERROR", q_strerror(errno));
	 break;
      case QSE_NOT_OPEN:
	 se_not_open("Socket", meth, xsink);
	 break;
      case QSE_TIMEOUT:
	 se_timeout("Socket", meth, timeout_ms, xsink);
	 break;
      case QSE_SSL_ERR:
	 xsink->raiseException("SOCKET-SSL-ERROR", "SSL error in Socket::%s() call", meth);
	 break;
      case QSE_IN_OP:
	 se_in_op("Socket", meth, xsink);
	 break;
      case QSE_IN_OP_THREAD:
	 se_in_op_thread("Socket", meth, xsink);
	 break;
      default:
	 xsink->raiseException("SOCKET-ERROR", "unknown internal error code %d in Socket::%s() call", rc, meth);
	 break;
   }
}

int SSLSocketHelper::doSSLRW(ExceptionSink* xsink, const char* mname, void* buf, int size, int timeout_ms, bool read, bool do_timeout) {
   SSLSocketReferenceHelper ssrh(this);

   if (timeout_ms < 0) {
      while (true) {
         int rc = read ? SSL_read(ssl, buf, size) : SSL_write(ssl, buf, size);
         if (rc < 0) {
	    // we set SSL_MODE_AUTO_RETRY so there should never be any need to retry
#ifdef DEBUG
            int err = SSL_get_error(ssl, rc);
            if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
	       assert(false);
#endif

            if (xsink) {
	       if (!sslError(xsink, mname, read ? "SSL_read" : "SSL_write", false))
		  rc = 0;
	    }
         }
         return rc;
      }
   }

   // set non blocking
   OptionalNonBlockingHelper nbh(qs, true, xsink);
   if (*xsink)
      return -1;

   int rc;
   while (true) {
      rc = read ? SSL_read(ssl, buf, size) : SSL_write(ssl, buf, size);

      if (rc > 0)
         break;

      int err = SSL_get_error(ssl, rc);

      if (err == SSL_ERROR_WANT_READ) {
         if (!qs.isSocketDataAvailable(timeout_ms, mname, xsink)) {
            if (xsink) {
               if (*xsink)
                  return -1;
               if (do_timeout)
                  se_timeout("Socket", mname, timeout_ms, xsink);
            }
            rc = QSE_TIMEOUT;
            break;
         }
      }
      else if (err == SSL_ERROR_WANT_WRITE) {
         if (!qs.isWriteFinished(timeout_ms, mname, xsink)) {
            if (xsink) {
               if (*xsink)
                  return -1;
               if (do_timeout)
                  se_timeout("Socket", mname, timeout_ms, xsink);
            }
            rc = QSE_TIMEOUT;
            break;
         }
      }
      // here we allow the remote side to disconnect and return 0 the first time just like regular recv()
      else if (err == SSL_ERROR_ZERO_RETURN) {
         if (read)
            rc = 0;
         else if (xsink) {
            if (!sslError(xsink, mname, "SSL_write"))
               xsink->raiseException("SOCKET-SSL-ERROR", "error in Socket::%s(): the socket was closed by the remote host while calling SSL_write()", mname);
            rc = QSE_SSL_ERR;
         }

         break;
      }
      else if (err == SSL_ERROR_SYSCALL) {
         if (xsink) {
            if (!sslError(xsink, mname, read ? "SSL_read" : "SSL_write")) {
               if (!rc)
                  xsink->raiseException("SOCKET-SSL-ERROR", "error in Socket::%s(): the openssl library reported an EOF condition that violates the SSL protocol while calling SSL_%s()", mname, read ? "read" : "write");
               else if (rc == -1) {
                  xsink->raiseErrnoException("SOCKET-SSL-ERROR", sock_get_error(), "error in Socket::%s(): the openssl library reported an I/O error while calling SSL_%s()", mname, read ? "read" : "write");

#ifdef ECONNRESET
                  // close the socket if connection reset received
                  if (qs.isOpen() && sock_get_error() == ECONNRESET)
                     qs.close();
#endif
               }
               else
                  xsink->raiseException("SOCKET-SSL-ERROR", "error in Socket::%s(): the openssl library reported error code %d in SSL_%s() but the error queue is empty", mname, rc, read ? "read" : "write");
               }
         }

         rc = xsink && !*xsink ? 0 : QSE_SSL_ERR;
         //rc = QSE_SSL_ERR;
         break;
      }
      else {
         //printd(5, "SSLSocketHelper::doSSLRW(buf: %p, size: %d, to: %d) rc: %d err: %d\n", buf, size, timeout_ms, rc, err);
         // always throw an exception if an error occurs while writing
         if (xsink) {
            if (!sslError(xsink, mname, read ? "SSL_read" : "SSL_write", !read))
               rc = 0;
         }
         else {
            rc = xsink && !*xsink ? 0 : QSE_SSL_ERR;
         }
         break;
      }
   }

   //printd(0, "SSLSocketHelper::doSSLRW(buf: %p, size: %d, to: %d, read: %d) rc: %d\n", buf, size, timeout_ms, (int)read, rc);
   return rc;
}

// if we close the connection due to a socket error, then the SSLSocketHelper object is deleted, therefore have to ensure that we do not access
// "this" after the connection is closed
int SSLSocketHelper::doSSLUpgradeNonBlockingIO(int rc, const char* mname, int timeout_ms, const char* ssl_func, ExceptionSink* xsink) {
   SSLSocketReferenceHelper ssrh(this);

   assert(xsink);

   int err = SSL_get_error(ssl, rc);

   if (err == SSL_ERROR_WANT_READ) {
      if (qs.isSocketDataAvailable(timeout_ms, mname, xsink))
	 return 0;

      if (*xsink)
	 return -1;
      se_timeout("Socket", mname, timeout_ms, xsink);
      return QSE_TIMEOUT;
   }

   if (err == SSL_ERROR_WANT_WRITE) {
      if (qs.isWriteFinished(timeout_ms, mname, xsink))
	 return 0;

      if (*xsink)
	 return -1;
      se_timeout("Socket", mname, timeout_ms, xsink);
      return QSE_TIMEOUT;
   }

   if (err == SSL_ERROR_SYSCALL) {
      if (!sslError(xsink, mname, ssl_func)) {
	 if (!rc)
	    xsink->raiseException("SOCKET-SSL-ERROR", "error in Socket::%s(): the openssl library reported an EOF condition that violates the SSL protocol while calling %s()", mname, ssl_func);
	 else if (rc == -1) {
	    xsink->raiseErrnoException("SOCKET-SSL-ERROR", sock_get_error(), "error in Socket::%s(): the openssl library reported an I/O error while calling %s()", mname, ssl_func);

#ifdef ECONNRESET
	    // close the socket if connection reset received
	    // do not access "this" after the connection is closed since the SSLSocketHelper has been deleted
	    if (qs.isOpen() && sock_get_error() == ECONNRESET)
	       qs.close();
#endif
	 }
	 else
	    xsink->raiseException("SOCKET-SSL-ERROR", "error in Socket::%s(): the openssl library reported error code %d in %s() but the error queue is empty", mname, rc, ssl_func);
      }

      return !*xsink ? 0 : QSE_SSL_ERR;
   }

   //printd(5, "SSLSocketHelper::doSSLNonBlockingIO(buf: %p, size: %d, to: %d) rc: %d err: %d\n", buf, size, timeout_ms, rc, err);
   // always throw an exception if an error occurs while writing
   if (!sslError(xsink, mname, ssl_func, true))
      return 0;

   return !*xsink ? 0 : QSE_SSL_ERR;
}

DLLLOCAL OptionalNonBlockingHelper::OptionalNonBlockingHelper(qore_socket_private& s, bool n_set, ExceptionSink* xs) : sock(s), xsink(xs), set(n_set) {
   if (set) {
      //printd(5, "OptionalNonBlockingHelper::OptionalNonBlockingHelper() this: %p\n", this);
      sock.set_non_blocking(true, xsink);
   }
}

DLLLOCAL OptionalNonBlockingHelper::~OptionalNonBlockingHelper() {
   if (set) {
      //printd(5, "OptionalNonBlockingHelper::~OptionalNonBlockingHelper() this: %p\n", this);
      sock.set_non_blocking(false, xsink);
   }
}

int SSLSocketHelper::read(const char* mname, char* buf, int size, int timeout_ms, ExceptionSink* xsink) {
   return doSSLRW(xsink, mname, buf, size, timeout_ms, true);
}

// returns true if an error was raised or the connection was closed, false if not
bool SSLSocketHelper::sslError(ExceptionSink* xsink, const char* mname, const char* func, bool always_error) {
   assert(refs > 1);

   long e = ERR_get_error();
   do {
      if (!e || e == SSL_ERROR_ZERO_RETURN) {
	 qs.close();
	 //printd(0, "SSLSocketHelper::sslError() Socket::%s() (%s) socket closed by remote end\n", mname, func);
	 if (always_error)
	    xsink->raiseException("SOCKET-SSL-ERROR", "error in Socket::%s(): the %s() call could not be completed because the TLS/SSL connection was terminated", mname, func);
      }
      else {
	 char buf[121];
	 ERR_error_string(e, buf);
	 xsink->raiseException("SOCKET-SSL-ERROR", "error in Socket::%s(): %s(): %s", mname, func, buf);
#ifdef ECONNRESET
	 // close the socket if connection reset received
	 if (e == SSL_ERROR_SYSCALL && sock_get_error() == ECONNRESET) {
	    //printd(5, "SSLSocketHelper::sslError() Socket::%s() (%s) socket closed by remote end\n", mname, func);
	    qs.close();
	 }
#endif
      }
   } while ((e = ERR_get_error()));

   return *xsink || !qs.isOpen();
}

PrivateQoreSocketTimeoutHelper::PrivateQoreSocketTimeoutHelper(qore_socket_private* s, const char* o) : PrivateQoreSocketTimeoutBase(s->tl_warning_us ? s : 0), op(o) {
}

PrivateQoreSocketTimeoutHelper::~PrivateQoreSocketTimeoutHelper() {
   if (!sock)
      return;

   int64 dt = q_clock_getmicros() - start;
   if (dt >= sock->tl_warning_us)
      sock->doTimeoutWarning(op, dt);
}

PrivateQoreSocketThroughputHelper::PrivateQoreSocketThroughputHelper(qore_socket_private* s, bool snd) : PrivateQoreSocketTimeoutBase(s), send(snd) {
}

PrivateQoreSocketThroughputHelper::~PrivateQoreSocketThroughputHelper() {
}

void PrivateQoreSocketThroughputHelper::finalize(int64 bytes) {
   //printd(5, "PrivateQoreSocketThroughputHelper::finalize() bytes: " QLLD " us: " QLLD " (min: " QLLD ") bs: %.6f threshold: %.6f\n", bytes, (q_clock_getmicros() - start), sock->tp_us_min, ((double)bytes / ((double)(q_clock_getmicros() - start) / (double)1000000.0)), sock->tp_warning_bs);

   if (bytes < DEFAULT_SOCKET_MIN_THRESHOLD_BYTES)
      return;

   if (send)
      sock->tp_bytes_sent += bytes;
   else
      sock->tp_bytes_recv += bytes;

   if (!sock->tp_warning_bs)
      return;

   int64 dt = q_clock_getmicros() - start;

   // ignore if less than event time threshold
   if (dt < sock->tp_us_min)
      return;

   double bs = (double)bytes / ((double)dt / (double)1000000.0);

   //printd(5, "PrivateQoreSocketThroughputHelper::finalize() bytes: " QLLD " us: " QLLD " bs: %.6f threshold: %.6f\n", bytes, dt, bs, sock->tp_warning_bs);

   if (bs <= (double)sock->tp_warning_bs)
      sock->doThroughputWarning(send, bytes, dt, bs);
}

QoreSocket::QoreSocket() : priv(new qore_socket_private) {
}

QoreSocket::QoreSocket(int n_sock, int n_sfamily, int n_stype, int n_prot, const QoreEncoding* n_enc) : priv(new qore_socket_private(n_sock, n_sfamily, n_stype, n_prot, n_enc)) {
}

QoreSocket::~QoreSocket() {
   delete priv;
}

int QoreSocket::setNoDelay(int nodelay) {
   return setsockopt(priv->sock, IPPROTO_TCP, TCP_NODELAY, (SETSOCKOPT_ARG_4)&nodelay, sizeof(int));
}

int QoreSocket::getNoDelay() const {
   int rc;
   socklen_t optlen = sizeof(int);
   int sorc = getsockopt(priv->sock, IPPROTO_TCP, TCP_NODELAY, (GETSOCKOPT_ARG_4)&rc, &optlen);
   //printd(5, "Socket::getNoDelay() sorc: %d rc: %d optlen: %d\n", sorc, rc, optlen);
   if (sorc)
       return sorc;
   return rc;
}

int QoreSocket::close() {
   return priv->close();
}

int QoreSocket::shutdown() {
   int rc;
   if (priv->sock != QORE_INVALID_SOCKET)
      rc = ::shutdown(priv->sock, SHUTDOWN_ARG);
   else
      rc = 0;

   return rc;
}

int QoreSocket::shutdownSSL(ExceptionSink* xsink) {
   if (priv->sock == QORE_INVALID_SOCKET)
      return 0;
   if (!priv->ssl)
      return 0;
   return priv->ssl->shutdown(xsink);
}

int QoreSocket::getSocket() const {
   return priv->sock;
}

const QoreEncoding* QoreSocket::getEncoding() const {
   return priv->enc;
}

void QoreSocket::setEncoding(const QoreEncoding* id) {
   priv->enc = id;
}

bool QoreSocket::isOpen() const {
   return (bool)(priv->sock != QORE_INVALID_SOCKET);
}

const char* QoreSocket::getSSLCipherName() const {
   if (!priv->ssl)
      return 0;
   return priv->ssl->getCipherName();
}

const char* QoreSocket::getSSLCipherVersion() const {
   if (!priv->ssl)
      return 0;
   return priv->ssl->getCipherVersion();
}

bool QoreSocket::isSecure() const {
   return (bool)priv->ssl;
}

long QoreSocket::verifyPeerCertificate() const {
   if (!priv->ssl)
      return -1;
   return priv->ssl->verifyPeerCertificate();
}

// hardcoded to SOCK_STREAM (tcp only)
int QoreSocket::connectINET(const char* host, int prt, int timeout_ms, ExceptionSink* xsink) {
   QoreString service;
   service.sprintf("%d", prt);

   return priv->connectINET(host, service.getBuffer(), timeout_ms, xsink);
}

int QoreSocket::connectINET(const char* host, int prt, ExceptionSink* xsink) {
   QoreString service;
   service.sprintf("%d", prt);

   return priv->connectINET(host, service.getBuffer(), -1, xsink);
}

int QoreSocket::connectINET2(const char* name, const char* service, int family, int socktype, int protocol, int timeout_ms, ExceptionSink* xsink) {
   return priv->connectINET(name, service, timeout_ms, xsink, family, socktype, protocol);
}

int QoreSocket::connectUNIX(const char* p, ExceptionSink* xsink) {
   return priv->connectUNIX(p, SOCK_STREAM, 0, xsink);
}

int QoreSocket::connectUNIX(const char* p, int sock_type, int protocol, ExceptionSink* xsink) {
   return priv->connectUNIX(p, sock_type, protocol, xsink);
}

// currently hardcoded to SOCK_STREAM (tcp-only)
// opens and connects to a remote socket
// for AF_INET sockets:
// * QoreSocket::connect("hostname:<port_number>");
// for AF_UNIX sockets:
// * QoreSocket::connect("filename");
int QoreSocket::connect(const char* name, int timeout_ms, ExceptionSink* xsink) {
   const char* p;
   int rc;

   if ((p = strrchr(name, ':'))) {
      QoreString host(name, p - name);
      QoreString service(p + 1);
      // if the address is an ipv6 address like: [<addr>], then connect as ipv6
      if (host.strlen() > 2 && host[0] == '[' && host[host.strlen() - 1] == ']') {
	 host.terminate(host.strlen() - 1);
	 //printd(5, "QoreSocket::connect(%s, %s) [ipv6]\n", host.getBuffer() + 1, service.getBuffer());
	 rc = priv->connectINET(host.getBuffer() + 1, service.getBuffer(), timeout_ms, xsink, AF_INET6);
      }
      else
	 rc = priv->connectINET(host.getBuffer(), service.getBuffer(), timeout_ms, xsink);
   }
   else {
      // else assume it's a file name for a UNIX domain socket
      rc = priv->connectUNIX(name, SOCK_STREAM, 0, xsink);
   }

   return rc;
}

int QoreSocket::connect(const char* name, ExceptionSink* xsink) {
   return connect(name, -1, xsink);
}

// currently hardcoded to SOCK_STREAM (tcp-only)
// opens and connects to a remote socket and negotiates an SSL connection
// for AF_INET sockets:
// * QoreSocket::connectSSL("hostname:<port_number>");
// for AF_UNIX sockets:
// * QoreSocket::connectSSL("filename");
int QoreSocket::connectSSL(const char* name, int timeout_ms, X509* cert, EVP_PKEY* pkey, ExceptionSink* xsink) {
   const char* p;
   int rc;

   if ((p = strchr(name, ':'))) {
      QoreString host(name, p - name);
      QoreString service(p + 1);
      // if the address is an ipv6 address like: [<addr>], then connect as ipv6
      if (host.strlen() > 2 && host[0] == '[' && host[host.strlen() - 1] == ']') {
	 host.terminate(host.strlen() - 1);
	 //printd(5, "QoreSocket::connect(%s, %s) [ipv6]\n", host.getBuffer() + 1, service.getBuffer());
	 rc = connectINET2SSL(host.getBuffer() + 1, service.getBuffer(), AF_INET6, SOCK_STREAM, 0, timeout_ms, cert, pkey, xsink);
      }
      else
	 rc = connectINET2SSL(host.getBuffer(), service.getBuffer(), AF_UNSPEC, SOCK_STREAM, 0, timeout_ms, cert, pkey, xsink);
   }
   else {
      // else assume it's a file name for a UNIX domain socket
      rc = connectUNIXSSL(name, SOCK_STREAM, 0, cert, pkey, xsink);
   }

   return rc;
}

int QoreSocket::connectSSL(const char* name, X509* cert, EVP_PKEY* pkey, ExceptionSink* xsink) {
   return connectSSL(name, -1, cert, pkey, xsink);
}

int QoreSocket::connectINETSSL(const char* host, int prt, int timeout_ms, X509* cert, EVP_PKEY* pkey, ExceptionSink* xsink) {
   QoreString service;
   service.sprintf("%d", prt);

   int rc = priv->connectINET(host, service.getBuffer(), timeout_ms, xsink);
   if (rc)
      return rc;
   return priv->upgradeClientToSSLIntern("connectINETSSL", cert, pkey, timeout_ms, xsink);
}

int QoreSocket::connectINETSSL(const char* host, int prt, X509* cert, EVP_PKEY* pkey, ExceptionSink* xsink) {
   return connectINETSSL(host, prt, -1, cert, pkey, xsink);
}

int QoreSocket::connectINET2SSL(const char* name, const char* service, int family, int sock_type, int protocol, int timeout_ms, X509* cert, EVP_PKEY* pkey, ExceptionSink* xsink) {
   int rc = connectINET2(name, service, family, sock_type, protocol, timeout_ms, xsink);
   if (rc)
      return rc;
   return priv->upgradeClientToSSLIntern("connectINET2SSL", cert, pkey, timeout_ms, xsink);
}

int QoreSocket::connectUNIXSSL(const char* p, int sock_type, int protocol, X509* cert, EVP_PKEY* pkey, ExceptionSink* xsink) {
   int rc = connectUNIX(p, sock_type, protocol, xsink);
   if (rc)
      return rc;
   return priv->upgradeClientToSSLIntern("connectUNIXSSL", cert, pkey, -1, xsink);
}

int QoreSocket::sendi1(char i) {
   if (priv->sock == QORE_INVALID_SOCKET)
      return -1;

   int rc = priv->send(0, "sendi1", &i, 1);

   if (rc < 0)
      return -1;

   return 0;
}

int QoreSocket::sendi2(short i) {
   if (priv->sock == QORE_INVALID_SOCKET)
      return -1;

   // convert to network byte order
   i = htons(i);
   return priv->send(0, "sendi2", (char*)&i, 2);
}

int QoreSocket::sendi4(int i) {
   if (priv->sock == QORE_INVALID_SOCKET)
      return -1;

   // convert to network byte order
   i = htonl(i);
   return priv->send(0, "sendi4", (char*)&i, 4);
}

int QoreSocket::sendi8(int64 i) {
   if (priv->sock == QORE_INVALID_SOCKET)
      return -1;

   // convert to network byte order
   i = i8MSB(i);
   return priv->send(0, "sendi8", (char*)&i, 8);
}

int QoreSocket::sendi2LSB(short i) {
   if (priv->sock == QORE_INVALID_SOCKET)
      return -1;

   // convert to LSB byte order
   i = i2LSB(i);
   return priv->send(0, "sendi2LSB", (char*)&i, 2);
}

int QoreSocket::sendi4LSB(int i) {
   if (priv->sock == QORE_INVALID_SOCKET)
      return -1;

   // convert to LSB byte order
   i = i4LSB(i);
   return priv->send(0, "sendi4LSB", (char*)&i, 4);
}

int QoreSocket::sendi8LSB(int64 i) {
   if (priv->sock == QORE_INVALID_SOCKET)
      return -1;

   // convert to LSB byte order
   i = i8LSB(i);
   return priv->send(0, "sendi8LSB", (char*)&i, 8);
}

int QoreSocket::sendi1(char i, int timeout_ms, ExceptionSink* xsink) {
   return priv->send(xsink, "sendi1", &i, 1, timeout_ms);
}

int QoreSocket::sendi2(short i, int timeout_ms, ExceptionSink* xsink) {
   // convert to network byte order
   i = htons(i);
   return priv->send(xsink, "sendi2", (char*)&i, 2, timeout_ms);
}

int QoreSocket::sendi4(int i, int timeout_ms, ExceptionSink* xsink) {
   // convert to network byte order
   i = htonl(i);
   return priv->send(xsink, "sendi4", (char*)&i, 4, timeout_ms);
}

int QoreSocket::sendi8(int64 i, int timeout_ms, ExceptionSink* xsink) {
   // convert to network byte order
   i = i8MSB(i);
   return priv->send(xsink, "sendi8", (char*)&i, 8, timeout_ms);
}

int QoreSocket::sendi2LSB(short i, int timeout_ms, ExceptionSink* xsink) {
   // convert to LSB byte order
   i = i2LSB(i);
   return priv->send(xsink, "sendi2LSB", (char*)&i, 2, timeout_ms);
}

int QoreSocket::sendi4LSB(int i, int timeout_ms, ExceptionSink* xsink) {
   // convert to LSB byte order
   i = i4LSB(i);
   return priv->send(xsink, "sendi4LSB", (char*)&i, 4, timeout_ms);
}

int QoreSocket::sendi8LSB(int64 i, int timeout_ms, ExceptionSink* xsink) {
   // convert to LSB byte order
   i = i8LSB(i);
   return priv->send(xsink, "sendi8LSB", (char*)&i, 8, timeout_ms);
}

// receive integer values and convert from network byte order
int QoreSocket::recvi1(int timeout, char* val) {
   return priv->recvix("recvi1", 1, val, timeout, 0);
}

// DLLLOCAL int recvix(const char* meth, int len, void* targ, int timeout_ms, ExceptionSink* xsink) {

int QoreSocket::recvi2(int timeout, short *val) {
   int rc = priv->recvix("recvi2", 2, val, timeout, 0);
   *val = ntohs(*val);
   return rc;
}

int QoreSocket::recvi4(int timeout, int *val) {
   int rc = priv->recvix("recvi4", 4, val, timeout, 0);
   *val = ntohl(*val);
   return rc;
}

int QoreSocket::recvi8(int timeout, int64 *val) {
   int rc = priv->recvix("recvi8", 8, val, timeout, 0);
   *val = MSBi8(*val);
   return rc;
}

int QoreSocket::recvi2LSB(int timeout, short *val) {
   int rc = priv->recvix("recvi2LSB", 2, val, timeout, 0);
   *val = LSBi2(*val);
   return rc;
}

int QoreSocket::recvi4LSB(int timeout, int *val) {
   int rc = priv->recvix("recvi4LSB", 4, val, timeout, 0);
   *val = LSBi4(*val);
   return rc;
}

int QoreSocket::recvi8LSB(int timeout, int64 *val) {
   int rc = priv->recvix("recvi8LSB", 8, val, timeout, 0);
   *val = LSBi8(*val);
   return rc;
}

int QoreSocket::recvu1(int timeout, unsigned char* val) {
   return priv->recvix("recvu1", 1, val, timeout, 0);
}

int QoreSocket::recvu2(int timeout, unsigned short *val) {
   int rc = priv->recvix("recvu2", 2, val, timeout, 0);
   *val = ntohs(*val);
   return rc;
}

int QoreSocket::recvu4(int timeout, unsigned int *val) {
   int rc = priv->recvix("recvu4", 4, val, timeout, 0);
   *val = ntohl(*val);
   return rc;
}

int QoreSocket::recvu2LSB(int timeout, unsigned short *val) {
   int rc = priv->recvix("recvu2LSB", 2, val, timeout, 0);
   *val = LSBi2(*val);
   return rc;
}

int QoreSocket::recvu4LSB(int timeout, unsigned int *val) {
   int rc = priv->recvix("recvu4LSB", 4, val, timeout, 0);
   *val = LSBi4(*val);
   return rc;
}

int64 QoreSocket::recvi1(int timeout, char* val, ExceptionSink* xsink) {
   return priv->recvix("recvi1", 1, val, timeout, xsink);
}

int64 QoreSocket::recvi2(int timeout, short *val, ExceptionSink* xsink) {
   int rc = priv->recvix("recvi2", 2, val, timeout, xsink);
   *val = ntohs(*val);
   return rc;
}

int64 QoreSocket::recvi4(int timeout, int *val, ExceptionSink* xsink) {
   int rc = priv->recvix("recvi4", 4, val, timeout, xsink);
   *val = ntohl(*val);
   return rc;
}

int64 QoreSocket::recvi8(int timeout, int64 *val, ExceptionSink* xsink) {
   int rc = priv->recvix("recvi8", 8, val, timeout, xsink);
   *val = MSBi8(*val);
   return rc;
}

int64 QoreSocket::recvi2LSB(int timeout, short *val, ExceptionSink* xsink) {
   int rc = priv->recvix("recvi2LSB", 2, val, timeout, xsink);
   *val = LSBi2(*val);
   return rc;
}

int64 QoreSocket::recvi4LSB(int timeout, int *val, ExceptionSink* xsink) {
   int rc = priv->recvix("recvi4LSB", 4, val, timeout, xsink);
   *val = LSBi4(*val);
   return rc;
}

int64 QoreSocket::recvi8LSB(int timeout, int64 *val, ExceptionSink* xsink) {
   int rc = priv->recvix("recvi8LSB", 8, val, timeout, xsink);
   *val = LSBi8(*val);
   return rc;
}

int64 QoreSocket::recvu1(int timeout, unsigned char* val, ExceptionSink* xsink) {
   return priv->recvix("recvu1", 1, val, timeout, xsink);
}

int64 QoreSocket::recvu2(int timeout, unsigned short *val, ExceptionSink* xsink) {
   int rc = priv->recvix("recvu2", 2, val, timeout, xsink);
   *val = ntohs(*val);
   return rc;
}

int64 QoreSocket::recvu4(int timeout, unsigned int *val, ExceptionSink* xsink) {
   int rc = priv->recvix("recvu4", 4, val, timeout, xsink);
   *val = ntohl(*val);
   return rc;
}

int64 QoreSocket::recvu2LSB(int timeout, unsigned short *val, ExceptionSink* xsink) {
   int rc = priv->recvix("recvu2LSB", 2, val, timeout, xsink);
   *val = LSBi2(*val);
   return rc;
}

int64 QoreSocket::recvu4LSB(int timeout, unsigned int *val, ExceptionSink* xsink) {
   int rc = priv->recvix("recvu4LSB", 4, val, timeout, xsink);
   *val = LSBi4(*val);
   return rc;
}

int QoreSocket::send(int fd, qore_offset_t size) {
   if (priv->sock == QORE_INVALID_SOCKET || !size) {
      printd(5, "QoreSocket::send() ERROR: sock: %d size: " QSD "\n", priv->sock, size);
      return -1;
   }

   char* buf = (char*)malloc(sizeof(char) * DEFAULT_SOCKET_BUFSIZE);
   ON_BLOCK_EXIT(free, buf);

   qore_offset_t rc = 0;
   qore_size_t bs = 0;
   while (true) {
      // calculate bytes needed
      qore_size_t bn;
      if (size < 0)
	 bn = DEFAULT_SOCKET_BUFSIZE;
      else {
	 bn = size - bs;
	 if (bn > DEFAULT_SOCKET_BUFSIZE)
	    bn = DEFAULT_SOCKET_BUFSIZE;
      }
      rc = read(fd, buf, bn);
      if (!rc)
	 break;
      if (rc < 0) {
	 printd(5, "QoreSocket::send() read error: %s\n", strerror(errno));
	 break;
      }

      // send buffer
      int src = priv->send(0, "send", buf, rc);
      if (src < 0) {
	 printd(5, "QoreSocket::send() send error: %s\n", strerror(errno));
	 break;
      }
      bs += rc;
      if (size > 0 && bs >= (qore_size_t)size) {
	 rc = 0;
	 break;
      }
   }
   return rc;
}

int QoreSocket::send(int fd, qore_offset_t size, int timeout_ms, ExceptionSink* xsink) {
   return priv->send(fd, size, timeout_ms, xsink);
}

BinaryNode* QoreSocket::recvBinary(qore_offset_t bufsize, int timeout, int *rc) {
   assert(rc);
   qore_offset_t nrc;
   BinaryNode* b = priv->recvBinary(bufsize, timeout, nrc, 0);
   *rc = (int)nrc;
   return b;
}

BinaryNode* QoreSocket::recvBinary(int timeout, int *rc) {
   assert(rc);
   qore_offset_t nrc;
   BinaryNode* b = priv->recvBinary(timeout, nrc, 0);
   *rc = (int)nrc;
   return b;
}

BinaryNode* QoreSocket::recvBinary(qore_offset_t bufsize, int timeout, ExceptionSink* xsink) {
   assert(xsink);
   qore_offset_t rc;
   BinaryNodeHolder b(priv->recvBinary(bufsize, timeout, rc, xsink));
   return *xsink ? 0 : b.release();
}

BinaryNode* QoreSocket::recvBinary(int timeout, ExceptionSink* xsink) {
   assert(xsink);
   qore_offset_t rc;
   BinaryNodeHolder b(priv->recvBinary(timeout, rc, xsink));
   return *xsink ? 0 : b.release();
}

QoreStringNode* QoreSocket::recv(qore_offset_t bufsize, int timeout, int *rc) {
   assert(rc);
   qore_offset_t nrc;
   QoreStringNode* str = priv->recv((ExceptionSink*)0, bufsize, timeout, nrc);
   *rc = (int)nrc;
   return str;
}

QoreStringNode* QoreSocket::recv(int timeout, int *rc) {
   assert(rc);
   qore_offset_t nrc;
   QoreStringNode* str = priv->recv((ExceptionSink*)0, timeout, nrc);
   *rc = (int)nrc;
   return str;
}

QoreStringNode* QoreSocket::recv(qore_offset_t bufsize, int timeout, ExceptionSink* xsink) {
   assert(xsink);
   qore_offset_t rc;
   QoreStringNodeHolder str(priv->recv(xsink, bufsize, timeout, rc));
   return *xsink ? 0 : str.release();
}

QoreStringNode* QoreSocket::recv(int timeout, ExceptionSink* xsink) {
   assert(xsink);
   qore_offset_t rc;
   QoreStringNodeHolder str(priv->recv(xsink, timeout, rc));
   return *xsink ? 0 : str.release();
}

// receive data and write to file descriptor
int QoreSocket::recv(int fd, qore_offset_t size, int timeout_ms, ExceptionSink* xsink) {
   return priv->recv(fd, size, timeout_ms, xsink);
}

// receive data and write to file descriptor
int QoreSocket::recv(int fd, qore_offset_t size, int timeout) {
   if (priv->sock == QORE_INVALID_SOCKET || !size)
      return -1;

   char* buf;
   qore_offset_t br = 0;
   qore_offset_t rc;
   while (true) {
      // calculate bytes needed
      int bn;
      if (size == -1)
	 bn = DEFAULT_SOCKET_BUFSIZE;
      else {
	 bn = size - br;
	 if (bn > DEFAULT_SOCKET_BUFSIZE)
	    bn = DEFAULT_SOCKET_BUFSIZE;
      }

      rc = priv->brecv(0, "recv", buf, bn, 0, timeout);
      if (rc <= 0)
	 break;
      br += rc;

      // write buffer to file descriptor
      rc = write(fd, buf, rc);
      if (rc <= 0)
	 break;

      if (size > 0 && br >= size) {
	 rc = 0;
	 break;
      }
   }
   return (int)rc;
}

// returns 0 for success
int QoreSocket::sendHTTPMessage(const char* method, const char* path, const char* http_version, const QoreHashNode* headers, const void *data, qore_size_t size, int source) {
   return priv->sendHttpMessage(0, 0, "Socket", "sendHTTPMessage", method, path, http_version, headers, data, size, 0, source);
}

// returns 0 for success
int QoreSocket::sendHTTPMessage(QoreHashNode* info, const char* method, const char* path, const char* http_version, const QoreHashNode* headers, const void *data, qore_size_t size, int source) {
   return priv->sendHttpMessage(0, info, "Socket", "sendHTTPMessage", method, path, http_version, headers, data, size, 0, source);
}

int QoreSocket::sendHTTPMessage(ExceptionSink* xsink, QoreHashNode* info, const char* method, const char* path, const char* http_version, const QoreHashNode* headers, const void *data, qore_size_t size, int source) {
   return priv->sendHttpMessage(xsink, info, "Socket", "sendHTTPMessage", method, path, http_version, headers, data, size, 0, source);
}

int QoreSocket::sendHTTPMessage(ExceptionSink* xsink, QoreHashNode* info, const char* method, const char* path, const char* http_version, const QoreHashNode* headers, const void *data, qore_size_t size, int source, int timeout_ms) {
   return priv->sendHttpMessage(xsink, info, "Socket", "sendHTTPMessage", method, path, http_version, headers, data, size, 0, source, timeout_ms);
}

int QoreSocket::sendHTTPMessageWithCallback(ExceptionSink* xsink, QoreHashNode *info, const char *method, const char *path, const char *http_version, const QoreHashNode *headers, const ResolvedCallReferenceNode& send_callback, int source, int timeout_ms) {
   return priv->sendHttpMessage(xsink, info, "Socket", "sendHTTPMessageWithCallback", method, path, http_version, headers, 0, 0, &send_callback, source, timeout_ms);
}

// returns 0 for success
int QoreSocket::sendHTTPResponse(int code, const char* desc, const char* http_version, const QoreHashNode* headers, const void *data, qore_size_t size, int source) {
   return priv->sendHttpResponse(0, "Socket", "sendHTTPResponse", code, desc, http_version, headers, data, size, 0, source);
}

int QoreSocket::sendHTTPResponse(ExceptionSink* xsink, int code, const char* desc, const char* http_version, const QoreHashNode* headers, const void *data, qore_size_t size, int source) {
   return priv->sendHttpResponse(xsink, "Socket", "sendHTTPResponse", code, desc, http_version, headers, data, size, 0, source);
}

int QoreSocket::sendHTTPResponse(ExceptionSink* xsink, int code, const char* desc, const char* http_version, const QoreHashNode* headers, const void *data, qore_size_t size, int source, int timeout_ms) {
   return priv->sendHttpResponse(xsink, "Socket", "sendHTTPResponse", code, desc, http_version, headers, data, size, 0, source, timeout_ms);
}

int QoreSocket::sendHTTPResponseWithCallback(ExceptionSink* xsink, int code, const char *desc, const char *http_version, const QoreHashNode *headers, const ResolvedCallReferenceNode& send_callback, int source, int timeout_ms) {
   return priv->sendHttpResponse(xsink, "Socket", "sendHTTPResponseWithCallback", code, desc, http_version, headers, 0, 0, &send_callback, source, timeout_ms);
}

AbstractQoreNode* QoreSocket::readHTTPHeader(int timeout, int *rc, int source) {
   assert(rc);
   qore_offset_t nrc;
   AbstractQoreNode* n = priv->readHTTPHeader(0, 0, timeout, nrc, source);
   *rc = (int)nrc;
   return n;
}

// rc is:
//    0 for remote end shutdown
//   -1 for socket error
//   -2 for socket not open
//   -3 for timeout
AbstractQoreNode* QoreSocket::readHTTPHeader(QoreHashNode* info, int timeout, int *rc, int source) {
   assert(rc);
   qore_offset_t nrc;
   AbstractQoreNode* n = priv->readHTTPHeader(0, info, timeout, nrc, source);
   *rc = (int)nrc;
   return n;
}

QoreHashNode* QoreSocket::readHTTPHeader(ExceptionSink* xsink, QoreHashNode* info, int timeout, int source) {
   assert(xsink);
   qore_offset_t rc;
   // qore_socket_private::readHTTPHeader() always returns a QoreHashNode* (or 0) if an ExceptionSink argument is passed
   return static_cast<QoreHashNode*>(priv->readHTTPHeader(xsink, info, timeout, rc, source));
}

QoreStringNode* QoreSocket::readHTTPHeaderString(ExceptionSink* xsink, int timeout, int source) {
   assert(xsink);
   return priv->readHTTPHeaderString(xsink, timeout, source);
}

// receive a binary message in HTTP chunked format
QoreHashNode* QoreSocket::readHTTPChunkedBodyBinary(int timeout, ExceptionSink* xsink, int source) {
   return priv->readHttpChunkedBodyBinary(timeout, xsink, "Socket", source);
}

// receive a message in HTTP chunked format
QoreHashNode* QoreSocket::readHTTPChunkedBody(int timeout, ExceptionSink* xsink, int source) {
   return priv->readHttpChunkedBody(timeout, xsink, "Socket", source);
}

bool QoreSocket::isDataAvailable(int timeout) const {
   return priv->isDataAvailable(timeout, 0, 0);
}

bool QoreSocket::isWriteFinished(int timeout) const {
   return priv->isWriteFinished(timeout, 0, 0);
}

bool QoreSocket::isDataAvailable(ExceptionSink* xsink, int timeout) const {
   return priv->isDataAvailable(timeout, "isDataAvailable", xsink);
}

bool QoreSocket::isWriteFinished(ExceptionSink* xsink, int timeout) const {
   return priv->isWriteFinished(timeout, "isWriteFinished", xsink);
}

int QoreSocket::asyncIoWait(int timeout_ms, bool read, bool write) const {
   return priv->asyncIoWait(timeout_ms, read, write);
}

int QoreSocket::upgradeClientToSSL(X509* cert, EVP_PKEY* pkey, ExceptionSink* xsink) {
   if (priv->sock == QORE_INVALID_SOCKET) {
      se_not_open("Socket", "upgradeClientToSSL", xsink);
      return -1;
   }
   if (priv->ssl)
      return 0;
   return priv->upgradeClientToSSLIntern("upgradeClientToSSL", cert, pkey, -1, xsink);
}

int QoreSocket::upgradeClientToSSL(X509* cert, EVP_PKEY* pkey, int timeout_ms, ExceptionSink* xsink) {
   if (priv->sock == QORE_INVALID_SOCKET) {
      se_not_open("Socket", "upgradeClientToSSL", xsink);
      return -1;
   }
   if (priv->ssl)
      return 0;
   return priv->upgradeClientToSSLIntern("upgradeClientToSSL", cert, pkey, timeout_ms, xsink);
}

int QoreSocket::upgradeServerToSSL(X509* cert, EVP_PKEY* pkey, ExceptionSink* xsink) {
   if (priv->sock == QORE_INVALID_SOCKET) {
      se_not_open("Socket", "upgradeServerToSSL", xsink);
      return -1;
   }
   if (priv->ssl)
      return 0;
   return priv->upgradeServerToSSLIntern("upgradeServerToSSL", cert, pkey, -1, xsink);
}

int QoreSocket::upgradeServerToSSL(X509* cert, EVP_PKEY* pkey, int timeout_ms, ExceptionSink* xsink) {
   if (priv->sock == QORE_INVALID_SOCKET) {
      se_not_open("Socket", "upgradeServerToSSL", xsink);
      return -1;
   }
   if (priv->ssl)
      return 0;
   return priv->upgradeServerToSSLIntern("upgradeServerToSSL", cert, pkey, timeout_ms, xsink);
}

/* currently hardcoded to SOCK_STREAM (tcp-only)
   if there is no port specifier, opens UNIX domain socket (if necessary)
   and binds to a local UNIX socket file
   for UNIX domain sockets: AF_UNIX
   - bind("filename");
   for ipv4 (unless an ipv6 address is detected in the host part): AF_INET
   - bind("interface:port");
   for ipv6 sockets: AF_INET6
   - bind("[interface]:port");
*/
int QoreSocket::bind(const char* name, bool reuseaddr) {
   //printd(5, "QoreSocket::bind(%s)\n", name);
   // see if there is a port specifier
   const char* p = strrchr(name, ':');
   if (p) {
      QoreString host(name, p - name);
      QoreString service(p + 1);

      // if the address is an ipv6 address like: [<addr>], then bind as ipv6
      if (host.strlen() > 2 && host[0] == '[' && host[host.strlen() - 1] == ']') {
	 host.terminate(host.strlen() - 1);
	 return priv->bindINET(host.getBuffer() + 1, service.getBuffer(), reuseaddr, AF_INET6, SOCK_STREAM);
      }

      // assume an ipv6 address if there is a ':' character in the hostname, otherwise bind ipv4
      return priv->bindINET(host.getBuffer(), service.getBuffer(), reuseaddr, strchr(host.getBuffer(), ':') ? AF_INET6 : AF_INET, SOCK_STREAM);
   }

   return priv->bindUNIX(name, SOCK_STREAM, 0);
}

int QoreSocket::bindUNIX(const char* name, int socktype, int protocol, ExceptionSink* xsink) {
   return priv->bindUNIX(name, socktype, protocol, xsink);
}

int QoreSocket::bindINET(const char* name, const char* service, bool reuseaddr, int family, int socktype, int protocol, ExceptionSink* xsink) {
   return priv->bindINET(name, service, reuseaddr, family, socktype, protocol, xsink);
}

// currently hardcoded to SOCK_STREAM (tcp-only)
// opens INET socket and binds to a tcp port on all interfaces
// closes socket if already open, because the socket will be
// bound to all interfaces
// * bind(port);
int QoreSocket::bind(int prt, bool reuseaddr) {
   priv->close();
   QoreString service;
   service.sprintf("%d", prt);
   return priv->bindINET(0, service.getBuffer(), reuseaddr);
}

// to bind to an INET tcp port on a specific interface
int QoreSocket::bind(const char* iface, int prt, bool reuseaddr) {
   printd(5, "QoreSocket::bind(%s, %d)\n", iface, prt);
   QoreString service;
   service.sprintf("%d", prt);
   return priv->bindINET(iface, service.getBuffer(), reuseaddr);
}

// to bind an INET socket to a particular address
int QoreSocket::bind(const struct sockaddr *addr, int size) {
   // close if it's already been opened as an INET socket or with different parameters
   if (priv->sock != QORE_INVALID_SOCKET && (priv->sfamily != AF_INET || priv->stype != SOCK_STREAM || priv->sprot != 0))
      close();

   // try to open socket if necessary
   if (priv->sock == QORE_INVALID_SOCKET && priv->openINET())
      return -1;

   if ((::bind(priv->sock, addr, size)) == QORE_SOCKET_ERROR) {
#ifdef _Q_WINDOWS
      // set errno from windows error
      sock_get_error();
#endif
      return -1;
   }

   // set port number to unknown
   priv->port = -1;
   //printd(5, "QoreSocket::bind(interface, port) returning 0 (success)\n");
   return 0;
}

int QoreSocket::bind(int family, const struct sockaddr *addr, int size, int sock_type, int protocol) {
   family = q_get_af(family);
   sock_type = q_get_sock_type(sock_type);

   // close if it's already been opened as an INET socket or with different parameters
   if (priv->sock != QORE_INVALID_SOCKET && (priv->sfamily != family || priv->stype != sock_type || priv->sprot != protocol))
      close();

   // try to open socket if necessary
   if (priv->sock == QORE_INVALID_SOCKET && priv->openINET(family, sock_type, protocol))
      return -1;

   if ((::bind(priv->sock, addr, size)) == -1) {
#ifdef _Q_WINDOWS
      // set errno from windows error
      sock_get_error();
#endif
      return -1;
   }

   // set port number
   int prt = q_get_port_from_addr(addr);
   priv->port = prt ? prt : -1;
   //printd(5, "QoreSocket::bind(interface, port) returning 0 (success)\n");
   return 0;
}

// find out what port we're connected to
int QoreSocket::getPort() {
   return priv->getPort();
}

// QoreSocket::accept()
// returns a new socket
QoreSocket* QoreSocket::accept(SocketSource* source, ExceptionSink* xsink) {
   int rc = priv->accept_internal(source, -1, xsink);
   if (rc < 0)
      return 0;

   QoreSocket* s = new QoreSocket(rc, priv->sfamily, priv->stype, priv->sprot, priv->enc);
   if (!priv->socketname.empty())
      s->priv->socketname = priv->socketname;
   return s;
}

// QoreSocket::acceptSSL()
// accepts a new connection, negotiates an SSL connection, and returns the new socket
QoreSocket* QoreSocket::acceptSSL(SocketSource* source, X509* cert, EVP_PKEY* pkey, ExceptionSink* xsink) {
   QoreSocket* s = accept(source, xsink);
   if (!s)
      return 0;

   if (s->priv->upgradeServerToSSLIntern("acceptSSL", cert, pkey, -1, xsink)) {
      assert(*xsink);
      delete s;
      return 0;
   }

   return s;
}

// accept a connection and replace the socket with the new connection
int QoreSocket::acceptAndReplace(SocketSource* source) {
   QORE_TRACE("QoreSocket::acceptAndReplace()");
   int rc = priv->accept_internal(source);
   if (rc < 0)
      return -1;
   priv->close_internal();
   priv->sock = rc;

   return 0;
}

QoreSocket* QoreSocket::accept(int timeout_ms, ExceptionSink* xsink) {
   int rc = priv->accept_internal(0, timeout_ms, xsink);
   if (rc < 0)
      return 0;

   QoreSocket* s = new QoreSocket(rc, priv->sfamily, priv->stype, priv->sprot, priv->enc);
   if (!priv->socketname.empty())
      s->priv->socketname = priv->socketname;

   return s;
}

QoreSocket* QoreSocket::acceptSSL(int timeout_ms, X509* cert, EVP_PKEY* pkey, ExceptionSink* xsink) {
   std::unique_ptr<QoreSocket> s(accept(timeout_ms, xsink));
   if (!s.get())
      return 0;

   if (s->priv->upgradeServerToSSLIntern("acceptSSL", cert, pkey, timeout_ms, xsink)) {
      assert(*xsink);
      return 0;
   }

   return s.release();
}

int QoreSocket::acceptAndReplace(int timeout_ms, ExceptionSink* xsink) {
   int rc = priv->accept_internal(0, timeout_ms, xsink);
   if (rc < 0)
      return -1;

   priv->close_internal();
   priv->sock = rc;
   return 0;
}

int QoreSocket::listen(int backlog) {
   return priv->listen(backlog);
}

int QoreSocket::listen() {
   return priv->listen();
}

int QoreSocket::send(const char* buf, qore_size_t size) {
   return priv->send(0, "send", buf, size);
}

int QoreSocket::send(const char* buf, qore_size_t size, ExceptionSink* xsink) {
   return priv->send(xsink, "send", buf, size);
}

int QoreSocket::send(const char* buf, qore_size_t size, int timeout_ms, ExceptionSink* xsink) {
   return priv->send(xsink, "send", buf, size, timeout_ms);
}

// converts to socket encoding if necessary
int QoreSocket::send(const QoreString* msg, ExceptionSink* xsink) {
   TempEncodingHelper tstr(msg, priv->enc, xsink);
   if (!tstr)
      return -1;

   return priv->send(xsink, "send", (const char*)tstr->getBuffer(), tstr->strlen());
}

// converts to socket encoding if necessary
int QoreSocket::send(const QoreString* msg, int timeout_ms, ExceptionSink* xsink) {
   TempEncodingHelper tstr(msg, priv->enc, xsink);
   if (!tstr)
      return -1;

   return priv->send(xsink, "send", (const char*)tstr->getBuffer(), tstr->strlen(), timeout_ms);
}

int QoreSocket::send(const BinaryNode* b) {
   return priv->send(0, "send", (char*)b->getPtr(), b->size());
}

int QoreSocket::send(const BinaryNode* b, ExceptionSink* xsink) {
   return priv->send(xsink, "send", (char*)b->getPtr(), b->size());
}

int QoreSocket::send(const BinaryNode* b, int timeout_ms, ExceptionSink* xsink) {
   return priv->send(xsink, "send", (char*)b->getPtr(), b->size(), timeout_ms);
}

int QoreSocket::setSendTimeout(int ms) {
   struct timeval tv;
   tv.tv_sec  = ms / 1000;
   tv.tv_usec = (ms % 1000) * 1000;

   return setsockopt(priv->sock, SOL_SOCKET, SO_SNDTIMEO, (SETSOCKOPT_ARG_4)&tv, sizeof(struct timeval));
}

int QoreSocket::setRecvTimeout(int ms) {
   struct timeval tv;
   tv.tv_sec  = ms / 1000;
   tv.tv_usec = (ms % 1000) * 1000;

   return setsockopt(priv->sock, SOL_SOCKET, SO_RCVTIMEO, (SETSOCKOPT_ARG_4)&tv, sizeof(struct timeval));
}

int QoreSocket::getSendTimeout() const {
   return priv->getSendTimeout();
}

int QoreSocket::getRecvTimeout() const {
   return priv->getRecvTimeout();
}

void QoreSocket::setEventQueue(Queue* cbq, ExceptionSink* xsink) {
   priv->setEventQueue(cbq, xsink);
}

Queue* QoreSocket::getQueue() {
   return priv->cb_queue;
}

void QoreSocket::cleanup(ExceptionSink* xsink) {
   priv->cleanup(xsink);
}

int64 QoreSocket::getObjectIDForEvents() const {
   return priv->getObjectIDForEvents();
}

QoreHashNode* QoreSocket::getPeerInfo(ExceptionSink* xsink) const {
   return priv->getPeerInfo(xsink);
}

QoreHashNode* QoreSocket::getSocketInfo(ExceptionSink* xsink) const {
   return priv->getSocketInfo(xsink);
}

QoreHashNode* QoreSocket::getPeerInfo(ExceptionSink* xsink, bool host_lookup) const {
   return priv->getPeerInfo(xsink, host_lookup);
}

QoreHashNode* QoreSocket::getSocketInfo(ExceptionSink* xsink, bool host_lookup) const {
   return priv->getSocketInfo(xsink, host_lookup);
}

void QoreSocket::setAccept(QoreObject *o) {
   priv->setAccept(o);
}

void QoreSocket::clearWarningQueue(ExceptionSink* xsink) {
   priv->clearWarningQueue(xsink);
}

void QoreSocket::setWarningQueue(ExceptionSink* xsink, int64 warning_ms, int64 warning_bs, Queue* wq, AbstractQoreNode* arg, int64 min_ms) {
   priv->setWarningQueue(xsink, warning_ms, warning_bs, wq, arg, min_ms);
}

QoreHashNode* QoreSocket::getUsageInfo() const {
   return priv->getUsageInfo();
}

void QoreSocket::clearStats() {
   priv->clearStats();
}

bool QoreSocket::pendingHttpChunkedBody() const {
   return priv->pendingHttpChunkedBody();
}

QoreSocketTimeoutHelper::QoreSocketTimeoutHelper(QoreSocket& s, const char* op) : priv(new PrivateQoreSocketTimeoutHelper(qore_socket_private::get(s), op)) {
}

QoreSocketTimeoutHelper::~QoreSocketTimeoutHelper() {
   delete priv;
}

QoreSocketThroughputHelper::QoreSocketThroughputHelper(QoreSocket& s, bool snd) : priv(new PrivateQoreSocketThroughputHelper(qore_socket_private::get(s), snd)) {
}

QoreSocketThroughputHelper::~QoreSocketThroughputHelper() {
   delete priv;
}

void QoreSocketThroughputHelper::finalize(int64 bytes) {
   priv->finalize(bytes);
}
