/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_socket_private.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 David Nichols

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

#ifndef _QORE_QORE_SOCKET_PRIVATE_H
#define _QORE_QORE_SOCKET_PRIVATE_H

#include <qore/intern/SSLSocketHelper.h>

#include <qore/intern/QC_Queue.h>

#include <ctype.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#if defined HAVE_POLL
#include <poll.h>
#elif defined HAVE_SELECT
#include <sys/select.h>
#elif (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
#define HAVE_SELECT 1
#else
#error no async socket I/O APIs available
#endif

#ifndef DEFAULT_SOCKET_BUFSIZE
#define DEFAULT_SOCKET_BUFSIZE 4096
#endif

#ifndef QORE_MAX_HEADER_SIZE
#define QORE_MAX_HEADER_SIZE 16384
#endif

#define CHF_HTTP11  (1 << 0)
#define CHF_PROCESS (1 << 1)
#define CHF_REQUEST (1 << 2)

#ifndef DEFAULT_SOCKET_MIN_THRESHOLD_BYTES
#define DEFAULT_SOCKET_MIN_THRESHOLD_BYTES 1024
#endif

DLLLOCAL void concat_target(QoreString& str, const struct sockaddr *addr, const char* type = "target");
DLLLOCAL int do_read_error(qore_offset_t rc, const char* method_name, int timeout_ms, ExceptionSink* xsink);
DLLLOCAL int sock_get_raw_error();
DLLLOCAL int sock_get_error();
DLLLOCAL void qore_socket_error(ExceptionSink* xsink, const char* err, const char* cdesc, const char* mname = 0, const char* host = 0, const char* svc = 0, const struct sockaddr *addr = 0);
DLLLOCAL void qore_socket_error_intern(int rc, ExceptionSink* xsink, const char* err, const char* cdesc, const char* mname = 0, const char* host = 0, const char* svc = 0, const struct sockaddr *addr = 0);
DLLLOCAL void se_in_op(const char* cname, const char* meth, ExceptionSink* xsink);
DLLLOCAL void se_in_op_thread(const char* cname, const char* meth, ExceptionSink* xsink);
DLLLOCAL void se_not_open(const char* cname, const char* meth, ExceptionSink* xsink);
DLLLOCAL void se_timeout(const char* cname, const char* meth, int timeout_ms, ExceptionSink* xsink);
DLLLOCAL void se_closed(const char* cname, const char* mname, ExceptionSink* xsink);

#ifdef _Q_WINDOWS
#define GETSOCKOPT_ARG_4 char*
#define SETSOCKOPT_ARG_4 const char*
#define SHUTDOWN_ARG SD_BOTH
#define QORE_INVALID_SOCKET ((int)INVALID_SOCKET)
#define QORE_SOCKET_ERROR SOCKET_ERROR
DLLLOCAL int check_windows_rc(int rc);

#ifndef ECONNRESET
#define ECONNRESET WSAECONNRESET
#endif

#else
// UNIX/Cygwin
#define GETSOCKOPT_ARG_4 void*
#define SETSOCKOPT_ARG_4 void*
#define SHUTDOWN_ARG SHUT_RDWR
#define QORE_INVALID_SOCKET -1
#define QORE_SOCKET_ERROR -1
#endif

struct qore_socketsource_private {
   QoreStringNode* address;
   QoreStringNode* hostname;

   DLLLOCAL qore_socketsource_private() : address(0), hostname(0) {
   }

   DLLLOCAL ~qore_socketsource_private() {
      if (address)  address->deref();
      if (hostname) hostname->deref();
   }

   DLLLOCAL void setAddress(QoreStringNode* addr) {
      assert(!address);
      address = addr;
   }

   DLLLOCAL void setAddress(const char* addr) {
      assert(!address);
      address = new QoreStringNode(addr);
   }

   DLLLOCAL void setHostName(const char* host) {
      assert(!hostname);
      hostname = new QoreStringNode(host);
   }

   DLLLOCAL void setAll(QoreObject* o, ExceptionSink* xsink) {
      if (address) {
	 o->setValue("source", address, xsink);
	 address = 0;
      }

      if (hostname) {
	 o->setValue("source_host", hostname, xsink);
	 hostname = 0;
      }
   }
};

class OptionalNonBlockingHelper {
public:
   qore_socket_private& sock;
   ExceptionSink* xsink;
   bool set;

   DLLLOCAL OptionalNonBlockingHelper(qore_socket_private& s, bool n_set, ExceptionSink* xs);
   DLLLOCAL ~OptionalNonBlockingHelper();
};

class PrivateQoreSocketTimeoutBase {
protected:
   struct qore_socket_private* sock;
   int64 start;

public:
   DLLLOCAL PrivateQoreSocketTimeoutBase(qore_socket_private* s) : sock(s), start(sock ? q_clock_getmicros() : 0) {
   }
};

class PrivateQoreSocketTimeoutHelper : public PrivateQoreSocketTimeoutBase {
protected:
   const char* op;
public:
   DLLLOCAL PrivateQoreSocketTimeoutHelper(qore_socket_private* s, const char* op);
   DLLLOCAL ~PrivateQoreSocketTimeoutHelper();
};

class PrivateQoreSocketThroughputHelper : public PrivateQoreSocketTimeoutBase {
protected:
   bool send;
public:
   DLLLOCAL PrivateQoreSocketThroughputHelper(qore_socket_private* s, bool snd);
   DLLLOCAL ~PrivateQoreSocketThroughputHelper();

   DLLLOCAL void finalize(int64 bytes);
};

struct qore_socket_private;

struct qore_socket_op_helper {
protected:
   qore_socket_private* s;

public:
   DLLLOCAL qore_socket_op_helper(qore_socket_private* sock);
   DLLLOCAL ~qore_socket_op_helper();
};

class SSLSocketHelperHelper {
protected:
   qore_socket_private* s;
   SSLSocketHelper* ssl;

public:
   DLLLOCAL SSLSocketHelperHelper(qore_socket_private* sock);

   DLLLOCAL void error();
};

struct qore_socket_private {
   friend class PrivateQoreSocketTimeoutHelper;
   friend class PrivateQoreSocketThroughputHelper;

   int sock, sfamily, port, stype, sprot; //, sendTimeout, recvTimeout;
   const QoreEncoding* enc;
   std::string socketname;
   SSLSocketHelper* ssl;
   Queue* cb_queue,
      * warn_queue;
   // socket buffer for buffered reads
   char rbuf[DEFAULT_SOCKET_BUFSIZE];
   // current buffer size
   size_t buflen, bufoffset;
   int64 tl_warning_us;     // timeout threshold for network action warning in microseconds
   double tp_warning_bs;    // throughput warning threshold in B/s
   int64 tp_bytes_sent,     // throughput: bytes sent
      tp_bytes_recv,        // throughput: bytes received
      tp_us_sent,           // throughput: time sending
      tp_us_recv,           // throughput: time receiving
      tp_us_min             // throughput: minimum time for transfer to be considered
      ;
   AbstractQoreNode* callback_arg;
   bool del, http_exp_chunked_body;
   int in_op;

   DLLLOCAL qore_socket_private(int n_sock = QORE_INVALID_SOCKET, int n_sfamily = AF_UNSPEC, int n_stype = SOCK_STREAM, int n_prot = 0, const QoreEncoding* n_enc = QCS_DEFAULT) :
      sock(n_sock), sfamily(n_sfamily), port(-1), stype(n_stype), sprot(n_prot), enc(n_enc),
      ssl(0), cb_queue(0), warn_queue(0), buflen(0), bufoffset(0), tl_warning_us(0), tp_warning_bs(0),
      tp_bytes_sent(0), tp_bytes_recv(0), tp_us_sent(0), tp_us_recv(0), tp_us_min(0),
      callback_arg(0), del(false), http_exp_chunked_body(false), in_op(-1) {
      //sendTimeout = recvTimeout = -1
   }

   DLLLOCAL ~qore_socket_private() {
      close_internal();

      // must be dereferenced and removed before deleting
      assert(!cb_queue);
      assert(!warn_queue);
   }

   DLLLOCAL bool isOpen() {
      return sock != QORE_INVALID_SOCKET;
   }

   DLLLOCAL int close() {
      int rc = close_internal();
      if (in_op >= 0)
         in_op = -1;
      if (http_exp_chunked_body)
         http_exp_chunked_body = false;
      sfamily = AF_UNSPEC;
      stype = SOCK_STREAM;
      sprot = 0;

      return rc;
   }

   DLLLOCAL int close_and_reset() {
      assert(sock != QORE_INVALID_SOCKET);
      int rc;
      while (true) {
#ifdef _Q_WINDOWS
	 rc = ::closesocket(sock);
#else
	 rc = ::close(sock);
#endif
	 // try again if close was interrupted by a signal
	 if (!rc || sock_get_error() != EINTR)
	    break;
      }
      //printd(5, "qore_socket_private::close_and_reset(this: %p) close(%d) returned %d\n", this, sock, rc);
      sock = QORE_INVALID_SOCKET;
      if (buflen)
	 buflen = 0;
      if (bufoffset)
	 bufoffset = 0;
      if (del)
	 del = false;
      if (port != -1)
	 port = -1;
      return rc;
   }

   DLLLOCAL int close_internal() {
      //printd(5, "qore_socket_private::close_internal(this=%p) sock=%d\n", this, sock);
      if (sock >= 0) {
	 // if an SSL connection has been established, shut it down first
	 if (ssl) {
	    ssl->shutdown();
	    ssl->deref();
	    ssl = 0;
	 }

	 if (!socketname.empty()) {
	    if (del)
	       unlink(socketname.c_str());
	    socketname.clear();
	 }
	 do_close_event();
	 return close_and_reset();
      }
      else
	 return 0;
   }

   DLLLOCAL int getSendTimeout() const {
      struct timeval tv;

#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
      // on HPUX 64-bit the OS defines socklen_t to be 8 bytes
      // but the library expects a 32-bit value
      int size = sizeof(struct timeval);
#else
      socklen_t size = sizeof(struct timeval);
#endif

      if (getsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (GETSOCKOPT_ARG_4)&tv, (socklen_t *)&size))
	 return -1;

      return tv.tv_sec * 1000 + tv.tv_usec / 1000;
   }

   DLLLOCAL int getRecvTimeout() const {
      struct timeval tv;

#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
      // on HPUX 64-bit the OS defines socklen_t to be 8 bytes
      // but the library expects a 32-bit value
      int size = sizeof(struct timeval);
#else
      socklen_t size = sizeof(struct timeval);
#endif

      if (getsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (GETSOCKOPT_ARG_4)&tv, (socklen_t *)&size))
	 return -1;

      return tv.tv_sec * 1000 + tv.tv_usec / 1000;
   }

   DLLLOCAL int getPort() {
      // if we don't need to find out what port we are, then return current value
      if (sock == QORE_INVALID_SOCKET || (sfamily != AF_INET && sfamily != AF_INET6) || port > 0)
	 return port;

      // otherwise find out what port we're connected to
      struct sockaddr_storage addr;
#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
      // on HPUX 64-bit the OS defines socklen_t to be 8 bytes, but the library expects a 32-bit value
      int size = sizeof addr;
#else
      socklen_t size = sizeof addr;
#endif

      if (getsockname(sock, (struct sockaddr *)&addr, (socklen_t *)&size) < 0)
	 return -1;

      port = q_get_port_from_addr((const struct sockaddr *)&addr);
      return port;
   }

   DLLLOCAL static void do_header(const char* key, QoreString& hdr, const AbstractQoreNode* v) {
      switch (get_node_type(v)) {
	 case NT_STRING:
	    hdr.sprintf("%s: %s\r\n", key, reinterpret_cast<const QoreStringNode*>(v)->getBuffer());
	    break;
	 case NT_INT:
	    hdr.sprintf("%s: "QLLD"\r\n", key, reinterpret_cast<const QoreBigIntNode*>(v)->val);
	    break;
	 case NT_FLOAT:
	    hdr.sprintf("%s: %f\r\n", key, reinterpret_cast<const QoreFloatNode*>(v)->f);
	    break;
	 case NT_NUMBER:
	    hdr.sprintf("%s: ", key);
	    reinterpret_cast<const QoreNumberNode*>(v)->toString(hdr);
	    hdr.concat("\r\n");
	    break;
	 case NT_BOOLEAN:
	    hdr.sprintf("%s: %d\r\n", key, reinterpret_cast<const QoreBoolNode*>(v)->getValue());
	    break;
      }
   }

   DLLLOCAL static void do_headers(QoreString& hdr, const QoreHashNode* headers, qore_size_t size, bool addsize = false) {
      // RFC-2616 4.4 (http://tools.ietf.org/html/rfc2616#section-4.4)
      // add Content-Length: 0 to headers for responses without a body where there is no transfer-encoding
      if (headers) {
	 ConstHashIterator hi(headers);

	 while (hi.next()) {
	    const AbstractQoreNode* v = hi.getValue();
	    const char* key = hi.getKey();
	    if (addsize && !strcasecmp(key, "transfer-encoding"))
	       addsize = false;
	    if (v && v->getType() == NT_LIST) {
	       ConstListIterator li(reinterpret_cast<const QoreListNode* >(v));
	       while (li.next())
		  do_header(key, hdr, li.getValue());
	    }
	    else
	       do_header(key, hdr, hi.getValue());
	 }
      }
      // add data and content-length header if necessary
      if (size || addsize)
	 hdr.sprintf("Content-Length: "QSD"\r\n", size);

      hdr.concat("\r\n");
   }

   DLLLOCAL static void *get_in_addr(struct sockaddr *sa) {
      if (sa->sa_family == AF_INET)
	 return &(((struct sockaddr_in *)sa)->sin_addr);
      return &(((struct sockaddr_in6 *)sa)->sin6_addr);
   }

   DLLLOCAL static size_t get_in_len(struct sockaddr *sa) {
      if (sa->sa_family == AF_INET)
	 return sizeof(struct sockaddr_in);
      return sizeof(struct sockaddr_in6);
   }

   DLLLOCAL int listen(int backlog = 20) {
      if (sock == QORE_INVALID_SOCKET)
	 return QSE_NOT_OPEN;
      if (in_op >= 0)
         return QSE_IN_OP;
#ifdef _Q_WINDOWS
      if (::listen(sock, backlog)) {
	 // set errno
	 sock_get_error();
	 return -1;
      }
      return 0;
#else
      return ::listen(sock, backlog);
#endif
   }

   DLLLOCAL int accept_intern(struct sockaddr *addr, socklen_t *size, int timeout_ms = -1, ExceptionSink* xsink = 0) {
      while (true) {
	 if (timeout_ms >= 0 && !isDataAvailable(timeout_ms, "accept", xsink)) {
	    if (xsink && *xsink)
	       return -1;
	    // do not throw exception here, NOTHING will be returned in Qore on timeout
	    return QSE_TIMEOUT; // -3
	 }

	 int rc = ::accept(sock, addr, size);
	 if (rc != QORE_INVALID_SOCKET)
	    return rc;

	 // retry if interrupted by a signal
	 if (sock_get_error() == EINTR)
	    continue;

	 qore_socket_error(xsink, "SOCKET-ACCEPT-ERROR", "error in accept()", 0, 0, 0, addr);
	 return -1;
      }
   }

   // returns a new socket
   DLLLOCAL int accept_internal(SocketSource *source, int timeout_ms = -1, ExceptionSink* xsink = 0) {
      if (sock == QORE_INVALID_SOCKET) {
	 if (xsink)
	    xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened, bound, and in a listening state before new connections can be accepted");
	 return QSE_NOT_OPEN;
      }
      if (in_op >= 0) {
         if (in_op == gettid()) {
            if (xsink)
               se_in_op("Socket", "accept", xsink);
            return QSE_IN_OP;
         }
         if (xsink)
            se_in_op_thread("Socket", "accept", xsink);
         return QSE_IN_OP_THREAD;
      }

      int rc;
      if (sfamily == AF_UNIX) {
#ifdef _Q_WINDOWS
	 if (xsink)
	    xsink->raiseException("SOCKET-ACCEPT-ERROR", "UNIX sockets are not available under Windows");
	 return -1;
#else
	 struct sockaddr_un addr_un;

#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
	 // on HPUX 64-bit the OS defines socklen_t to be 8 bytes
	 // but the library expects a 32-bit value
	 int size = sizeof(struct sockaddr_un);
#else
	 socklen_t size = sizeof(struct sockaddr_un);
#endif
	 rc = accept_intern((struct sockaddr *)&addr_un, (socklen_t *)&size, timeout_ms, xsink);
	 //printd(1, "qore_socket_private::accept_internal() "QSD" bytes returned\n", size);

	 if (rc >= 0 && source) {
	    QoreStringNode* addr = new QoreStringNode(enc);
	    addr->sprintf("UNIX socket: %s", socketname.c_str());
	    source->priv->setAddress(addr);
	    source->priv->setHostName("localhost");
	 }
#endif // windows
      }
      else if (sfamily == AF_INET || sfamily == AF_INET6) {
	 struct sockaddr_storage addr_in;
#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
	 // on HPUX 64-bit the OS defines socklen_t to be 8 bytes
	 // but the library expects a 32-bit value
	 int size = sizeof(addr_in);
#else
	 socklen_t size = sizeof(addr_in);
#endif

	 rc = accept_intern((struct sockaddr *)&addr_in, (socklen_t *)&size, timeout_ms, xsink);
	 //printd(1, "qore_socket_private::accept_internal() rc=%d, %d bytes returned\n", rc, size);

	 if (rc >= 0 && source) {
	    char host[NI_MAXHOST + 1];
	    char service[NI_MAXSERV + 1];

	    if (!getnameinfo((struct sockaddr *)&addr_in, get_in_len((struct sockaddr *)&addr_in), host, sizeof(host), service, sizeof(service), NI_NUMERICSERV)) {
	       source->priv->setHostName(host);
	    }

	    // get ipv4 or ipv6 address
	    char ifname[INET6_ADDRSTRLEN];
	    if (inet_ntop(addr_in.ss_family, get_in_addr((struct sockaddr *)&addr_in), ifname, sizeof(ifname))) {
	       //printd(5, "inet_ntop() '%s' host: '%s'\n", ifname, host);
	       source->priv->setAddress(ifname);
	    }
	 }
      }
      else {
	 // should not happen
	 if (xsink)
	    xsink->raiseException("SOCKET-ACCEPT-ERROR", "do not know how to accept connections with address family %d", sfamily);
	 rc = -1;
      }
      return rc;
   }

   DLLLOCAL void cleanup(ExceptionSink* xsink) {
      if (cb_queue) {
	 // close the socket before the delete message is put on the queue
	 // the socket would be closed anyway in the destructor
	 close_internal();

	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_DELETED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_SOCKET), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 cb_queue->pushAndTakeRef(h);

	 // deref and remove event queue
	 cb_queue->deref(xsink);
	 cb_queue = 0;
      }
      if (warn_queue) {
	 warn_queue->deref(xsink);
	 warn_queue = 0;
         if (callback_arg) {
            callback_arg->deref(xsink);
            callback_arg = 0;
         }
      }
   }

   DLLLOCAL void setEventQueue(Queue* cbq, ExceptionSink* xsink) {
      if (cb_queue)
	 cb_queue->deref(xsink);
      cb_queue = cbq;
   }

   DLLLOCAL void do_start_ssl_event() {
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_START_SSL), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_SOCKET), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_ssl_established_event() {
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_SSL_ESTABLISHED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_SOCKET), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("cipher", new QoreStringNode(ssl->getCipherName()), 0);
	 h->setKeyValue("cipher_version", new QoreStringNode(ssl->getCipherVersion()), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_connect_event(int af, const struct sockaddr* addr, const char* target, const char* service = 0, int prt = -1) {
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_CONNECTING), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_SOCKET), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
         QoreStringNode* str = q_addr_to_string2(addr);
          if (str)
             h->setKeyValue("address", str, 0);
          else
             h->setKeyValue("error", q_strerror(sock_get_error()), 0);
         q_af_to_hash(af, *h, 0);
	 h->setKeyValue("target", new QoreStringNode(target), 0);
	 if (service)
	    h->setKeyValue("service", new QoreStringNode(service), 0);
	 if (prt != -1)
	    h->setKeyValue("port", new QoreBigIntNode(prt), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_connected_event() {
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_CONNECTED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_SOCKET), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_chunked_read(int event, qore_size_t bytes, qore_size_t total_read, int source) {
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(event), 0);
	 h->setKeyValue("source", new QoreBigIntNode(source), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 if (event == QORE_EVENT_HTTP_CHUNKED_DATA_RECEIVED)
	    h->setKeyValue("read", new QoreBigIntNode(bytes), 0);
	 else
	    h->setKeyValue("size", new QoreBigIntNode(bytes), 0);
	 h->setKeyValue("total_read", new QoreBigIntNode(total_read), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_read_http_header(int event, const QoreHashNode* headers, int source) {
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(event), 0);
	 h->setKeyValue("source", new QoreBigIntNode(source), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("headers", headers->hashRefSelf(), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_send_http_message(const QoreString& str, const QoreHashNode* headers, int source) {
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_HTTP_SEND_MESSAGE), 0);
	 h->setKeyValue("source", new QoreBigIntNode(source), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("message", new QoreStringNode(str), 0);
	 //printd(0, "do_send_http_message() str='%s' headers=%p (%d %s)\n", str.getBuffer(), headers, headers->getType(), headers->getTypeName());
	 h->setKeyValue("headers", headers->hashRefSelf(), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_close_event() {
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_CHANNEL_CLOSED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_SOCKET), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_read_event(qore_size_t bytes_read, qore_size_t total_read, qore_size_t bufsize = 0) {
      // post bytes read on event queue, if any
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_PACKET_READ), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_SOCKET), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("read", new QoreBigIntNode(bytes_read), 0);
	 h->setKeyValue("total_read", new QoreBigIntNode(total_read), 0);
	 // set total bytes to read and remaining bytes if bufsize > 0
	 if (bufsize > 0)
	    h->setKeyValue("total_to_read", new QoreBigIntNode(bufsize), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_send_event(int bytes_sent, int total_sent, int bufsize) {
      // post bytes sent on event queue, if any
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_PACKET_SENT), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_SOCKET), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("sent", new QoreBigIntNode(bytes_sent), 0);
	 h->setKeyValue("total_sent", new QoreBigIntNode(total_sent), 0);
	 h->setKeyValue("total_to_send", new QoreBigIntNode(bufsize), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_resolve_event(const char* host, const char* service = 0) {
      // post bytes sent on event queue, if any
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_HOSTNAME_LOOKUP), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_SOCKET), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 if (host)
	    h->setKeyValue("name", new QoreStringNode(host), 0);
	 if (service)
	    h->setKeyValue("service", new QoreStringNode(service), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_resolved_event(const struct sockaddr* addr) {
      // post bytes sent on event queue, if any
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_HOSTNAME_RESOLVED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_SOCKET), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 QoreStringNode* str = q_addr_to_string2(addr);
	 if (str)
	    h->setKeyValue("address", str, 0);
	 else
	    h->setKeyValue("error", q_strerror(sock_get_error()), 0);
	 int prt = q_get_port_from_addr(addr);
	 if (prt > 0)
	    h->setKeyValue("port", new QoreBigIntNode(prt), 0);
	 q_af_to_hash(addr->sa_family, *h, 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL int64 getObjectIDForEvents() const {
      return (int64)this;
   }

   DLLLOCAL int connectUNIX(const char* p, int sock_type, int protocol, ExceptionSink* xsink) {
      QORE_TRACE("connectUNIX()");

#ifdef _Q_WINDOWS
      xsink->raiseException("SOCKET-CONNECTUNIX-ERROR", "UNIX sockets are not available under Windows");
      return -1;
#else
      // close socket if already open
      close();

      printd(5, "qore_socket_private::connectUNIX(%s)\n", p);

      struct sockaddr_un addr;

      addr.sun_family = AF_UNIX;
      // copy path and terminate if necessary
      strncpy(addr.sun_path, p, sizeof(addr.sun_path) - 1);
      addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';
      if ((sock = socket(AF_UNIX, sock_type, protocol)) == QORE_SOCKET_ERROR) {
	 if (xsink)
	    xsink->raiseException("SOCKET-CONNECT-ERROR", q_strerror(errno));

	 return -1;
      }

      do_connect_event(AF_UNIX, (sockaddr*)&addr, p);
      while (true) {
	 if (!::connect(sock, (const sockaddr *)&addr, sizeof(struct sockaddr_un)))
	    break;

	 // try again if we were interrupted by a signal
	 if (sock_get_error() == EINTR)
	    continue;

	 // otherwise close the socket and return an exception with the error code
	 // do not have to worry about windows API calls here; this is a UNIX-only function
	 close_and_reset();
	 qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in connect()", 0, p);

	 return -1;
      }

      // save file name for deleting when socket is closed
      socketname = addr.sun_path;
      sfamily = AF_UNIX;

      do_connected_event();

      return 0;
#endif // windows
   }

   // socket must be open or -1 is returned and a Qore-language exception is raised
   /* return values:
      -1: error
      0: timeout
      > 0: I/O can continue
    */
   DLLLOCAL int select(int timeout_ms, bool read, const char* mname, ExceptionSink* xsink) {
      if (sock == QORE_INVALID_SOCKET) {
	 if (xsink)
	    se_not_open("Socket", mname, xsink);
	 return -1;
      }

#if defined HAVE_POLL
      return poll_intern(timeout_ms, read, mname, xsink);
#elif defined HAVE_SELECT
      return select_intern(timeout_ms, read, mname, xsink);
#else
#error no async socket operations supported
#endif
   }

#if defined HAVE_POLL
   DLLLOCAL int poll_intern(int timeout_ms, bool read, const char* mname, ExceptionSink* xsink) {
      int rc;
      pollfd fds = {sock, (short)(read ? POLLIN : POLLOUT), 0};
      while (true) {
         rc = poll(&fds, 1, timeout_ms);
         if (rc == -1 && errno == EINTR)
            continue;
         break;
      }
      if (rc < 0)
         qore_socket_error(xsink, "SOCKET-SELECT-ERROR", "poll(2) returned an error");
      else if (!rc && ((fds.revents & POLLHUP) || (fds.revents & (POLLERR|POLLNVAL))))
         rc = -1;

      return rc;
   }
#elif defined HAVE_SELECT
   DLLLOCAL int select_intern(int timeout_ms, bool read, const char* mname, ExceptionSink* xsink) {
      // windows does not use FD_SETSIZE to limit the value of the highest socket descriptor in the set
      // instead it has a maximum of 64 sockets in the set; we only need one anyway
#ifndef _Q_WINDOWS
      // select is inherently broken since it can only handle descriptors < FD_SETSIZE, which is 1024 on Linux for example
      if (sock >= FD_SETSIZE) {
         if (xsink)
            xsink->raiseException("SOCKET-SELECT-ERROR", "fd is %d which is >= %d; contact the Qore developers to implement an alternative to select() on this platform", sock, FD_SETSIZE);
         return -1;
      }
#endif
      struct timeval tv;
      int rc;
      while (true) {
         // to be safe, we set the file descriptor arg after each EINTR (required on Linux for example)
         fd_set sfs;

         FD_ZERO(&sfs);
         FD_SET(sock, &sfs);

	 tv.tv_sec  = timeout_ms / 1000;
	 tv.tv_usec = (timeout_ms % 1000) * 1000;

	 rc = read ? ::select(sock + 1, &sfs, 0, 0, &tv) : ::select(sock + 1, 0, &sfs, 0, &tv);
	 if (rc != QORE_SOCKET_ERROR || sock_get_error() != EINTR)
	    break;
      }
      if (rc == QORE_SOCKET_ERROR) {
         rc = 0;
         switch (sock_get_error()) {
#ifdef EBADF
            // mark the socket as closed if the select call fails due to a bad file descriptor error
            case EBADF:
               close();
               if (xsink)
                  se_closed("Socket", mname, xsink);
               break;
#endif
            default:
               qore_socket_error(xsink, "SOCKET-SELECT-ERROR", "select(2) returned an error");
               break;
         }
      }

      return rc;
   }
#endif

   DLLLOCAL bool tryReadSocketData(const char* mname, ExceptionSink* xsink) {
      assert(!buflen);
      bool b = select(0, true, mname, xsink);
      if (b || !ssl)
         return b;
      // select can return true if there is protocol negotiation data available,
      // so we try to read 1 byte with a timeout of 0 with the SSL connection
      int rc = ssl->doSSLRW(xsink, mname, rbuf, 1, 0, true, false);
      if (*xsink || (rc = QSE_TIMEOUT))
         return false;
      if (rc == 1)
         buflen = 1;
      else
         assert(!rc);
      return !rc;
   }

   DLLLOCAL bool isSocketDataAvailable(int timeout_ms, const char* mname, ExceptionSink* xsink) {
      return select(timeout_ms, true, mname, xsink);
   }

   DLLLOCAL bool isDataAvailable(int timeout_ms, const char* mname, ExceptionSink* xsink) {
      if (buflen)
	 return true;
      return isSocketDataAvailable(timeout_ms, mname, xsink);
   }

   DLLLOCAL bool isWriteFinished(int timeout_ms, const char* mname, ExceptionSink* xsink) {
      return select(timeout_ms, false, mname, xsink);
   }

   DLLLOCAL int close_and_exit() {
      if (sock != QORE_INVALID_SOCKET)
         close_and_reset();
      return -1;
   }

   DLLLOCAL int connectINETTimeout(int timeout_ms, const struct sockaddr* ai_addr, qore_size_t ai_addrlen, ExceptionSink* xsink, bool only_timeout) {
      PrivateQoreSocketTimeoutHelper toh(this, "connect");

      while (true) {
	 if (!::connect(sock, ai_addr, ai_addrlen))
	    return 0;

#ifdef _Q_WINDOWS
	 if (WSAGetLastError() != WSAEWOULDBLOCK) {
	    qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in connect()", 0, 0, 0, ai_addr);
	    break;
	 }
#else
	 // try again if we were interrupted by a signal
	 if (errno == EINTR)
	    continue;

	 if (errno != EINPROGRESS)
	    break;
#endif

	 //printd(5, "qore_socket_private::connectINETTimeout() errno: %d\n", errno);

	 // check for timeout or connection with EINPROGRESS
	 while (true) {
	    int rc = select(timeout_ms, false, "connectINETTimeout", xsink);
	    if (xsink && *xsink)
	       return -1;

	    //printd(0, "select(%d) returned %d\n", timeout_ms, rc);
	    if (rc == QORE_SOCKET_ERROR && sock_get_error() != EINTR) {
	       if (xsink && !only_timeout)
		  qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in select() with Socket::connect() with timeout", 0, 0, 0, ai_addr);
	       return -1;
	    }
	    else if (rc > 0) {
	       // socket selected for write
	       socklen_t lon = sizeof(int);
	       int val;

	       if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (GETSOCKOPT_ARG_4)(&val), &lon) == QORE_SOCKET_ERROR) {
		  if (xsink && !only_timeout)
		     qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in getsockopt()", 0, 0, 0, ai_addr);
		  return -1;
	       }

	       if (val) {
		  if (only_timeout) {
		     errno = val;
		     return -1;
		  }
		  qore_socket_error_intern(val, xsink, "SOCKET-CONNECT-ERROR", "error in getsockopt()", 0, 0, 0, ai_addr);
		  return -1;
	       }

	       // connected successfully within the timeout period
	       return 0;
	    }
	    else {
	       if (xsink) {
	          QoreStringNode* desc = new QoreStringNodeMaker("timeout in connection after %dms", timeout_ms);
	          concat_target(*desc, ai_addr);
	          xsink->raiseException("SOCKET-CONNECT-ERROR", desc);
	       }
	       return -1;
	    }
	 }
      }

      return -1;
   }

   DLLLOCAL int sock_errno_err(const char* err, const char* desc, ExceptionSink* xsink) {
      sock = QORE_INVALID_SOCKET;
      qore_socket_error(xsink, err, desc);
      return -1;
   }

   DLLLOCAL int set_non_blocking(bool non_blocking, ExceptionSink* xsink = 0) {
      // ignore call when socket already closed
      if (sock == QORE_INVALID_SOCKET) {
         assert(!xsink || *xsink);
         return -1;
      }

#ifdef _Q_WINDOWS
      u_long mode = non_blocking ? 1 : 0;
      int rc = ioctlsocket(sock, FIONBIO, &mode);
      if (check_windows_rc(rc))
	 return sock_errno_err("SOCKET-CONNECT-ERROR", "error in ioctlsocket(FIONBIO)", xsink);
#else
      int arg;

      // get socket descriptor status flags
      if ((arg = fcntl(sock, F_GETFL, 0)) < 0)
	 return sock_errno_err("SOCKET-CONNECT-ERROR", "error in fcntl() getting socket descriptor status flag", xsink);

      if (non_blocking) // set non-blocking
	 arg |= O_NONBLOCK;
      else // set blocking
	 arg &= ~O_NONBLOCK;

      if (fcntl(sock, F_SETFL, arg) < 0)
	 return sock_errno_err("SOCKET-CONNECT-ERROR", "error in fcntl() setting socket descriptor status flag", xsink);
#endif

      return 0;
   }

   DLLLOCAL int connectINET(const char* host, const char* service, int timeout_ms, ExceptionSink* xsink = 0, int family = AF_UNSPEC, int type = SOCK_STREAM, int protocol = 0) {
      family = q_get_af(family);
      type = q_get_sock_type(type);

      QORE_TRACE("qore_socket_private::connectINET()");

      // close socket if already open
      close();

      printd(5, "qore_socket_private::connectINET(%s:%s, %dms)\n", host, service, timeout_ms);

      do_resolve_event(host, service);

      QoreAddrInfo ai;
      if (ai.getInfo(xsink, host, service, family, 0, type, protocol))
	 return -1;

      struct addrinfo *aip = ai.getAddrInfo();

      // emit all "resolved" events
      if (cb_queue)
	 for (struct addrinfo *p = aip; p; p = p->ai_next)
	    do_resolved_event(p->ai_addr);

      int prt = q_get_port_from_addr(aip->ai_addr);

      for (struct addrinfo *p = aip; p; p = p->ai_next) {
	 if (!connectINETIntern(host, service, p->ai_family, p->ai_addr, p->ai_addrlen, p->ai_socktype, p->ai_protocol, prt, timeout_ms, xsink, true))
	    return 0;
	 if (xsink && *xsink)
	    break;
      }

      if (xsink && !*xsink)
	 qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in connect()", 0, host, service);
      return -1;
   }

   DLLLOCAL int connectINETIntern(const char* host, const char* service, int ai_family, struct sockaddr* ai_addr, size_t ai_addrlen, int ai_socktype, int ai_protocol, int prt, int timeout_ms, ExceptionSink* xsink, bool only_timeout = false) {
      //printd(5, "qore_socket_private::connectINETIntern() host=%s service=%s family=%d\n", host, service, ai_family);
      if ((sock = socket(ai_family, ai_socktype, ai_protocol)) == QORE_INVALID_SOCKET) {
	 if (xsink)
	    xsink->raiseErrnoException("SOCKET-CONNECT-ERROR", errno, "cannot establish a connection to %s:%s", host, service);

	 return -1;
      }

      //printd(5, "qore_socket_private::connectINETIntern(this=%p, host='%s', port=%d, timeout_ms=%d) sock=%d\n", this, host, port, timeout_ms, sock);

      int rc;

      // perform connect with timeout if a non-negative timeout was passed
      if (timeout_ms >= 0) {
	 // set non-blocking
	 if (set_non_blocking(true, xsink))
	    return close_and_exit();

	 do_connect_event(ai_family, ai_addr, host, service, prt);

	 rc = connectINETTimeout(timeout_ms, ai_addr, ai_addrlen, xsink, only_timeout);
	 //printd(5, "qore_socket_private::connectINETIntern() errno=%d rc=%d, xsink=%d\n", errno, rc, xsink && *xsink);

	 // set blocking
	 if (set_non_blocking(false, xsink))
	    return close_and_exit();
      }
      else {
	 do_connect_event(ai_family, ai_addr, host, service, prt);

	 while (true) {
	    rc = ::connect(sock, ai_addr, ai_addrlen);

	    // try again if rc == -1 and errno == EINTR
	    if (!rc || sock_get_error() != EINTR)
	       break;
	 }
      }

      if (rc < 0) {
	 if (xsink && (!only_timeout || errno == ETIMEDOUT))
	    qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in connect()", 0, host, service);

	 return close_and_exit();
      }

      sfamily = ai_family;
      stype = ai_socktype;
      sprot = ai_protocol;
      port = prt;
      //printd(5, "qore_socket_private::connectINETIntern(this=%p, host='%s', port=%d, timeout_ms=%d) success, rc=%d, sock=%d\n", this, host, port, timeout_ms, rc, sock);

      do_connected_event();
      return 0;
   }

   DLLLOCAL int upgradeClientToSSLIntern(const char* mname, X509* cert, EVP_PKEY* pkey, int timeout_ms, ExceptionSink* xsink) {
      assert(!ssl);
      SSLSocketHelperHelper sshh(this);

      int rc;
      do_start_ssl_event();
      if ((rc = ssl->setClient(mname, sock, cert, pkey, xsink)) || ssl->connect(mname, timeout_ms, xsink)) {
         sshh.error();
	 return rc ? rc : -1;
      }
      do_ssl_established_event();

      return 0;
   }

   DLLLOCAL int upgradeServerToSSLIntern(const char* mname, X509* cert, EVP_PKEY* pkey, int timeout_ms, ExceptionSink* xsink) {
      assert(!ssl);
      SSLSocketHelperHelper sshh(this);

      do_start_ssl_event();
      if (ssl->setServer(mname, sock, cert, pkey, xsink) || ssl->accept(mname, timeout_ms, xsink)) {
         sshh.error();
	 return -1;
      }
      do_ssl_established_event();

      return 0;
   }

   // returns 0 = success, -1 = error
   DLLLOCAL int openUNIX(int sock_type = SOCK_STREAM, int protocol = 0) {
      if (sock != QORE_INVALID_SOCKET)
	 close();

      if ((sock = socket(AF_UNIX, sock_type, protocol)) == QORE_INVALID_SOCKET) {
	 return -1;
      }

      sfamily = AF_UNIX;
      stype = sock_type;
      sprot = protocol;
      port = -1;
      return 0;
   }

   // returns 0 = success, -1 = error
   DLLLOCAL int openINET(int family = AF_INET, int sock_type = SOCK_STREAM, int protocol = 0) {
      if (sock != QORE_INVALID_SOCKET)
	 close();

      if ((sock = socket(family, sock_type, protocol)) == QORE_INVALID_SOCKET)
	 return -1;

      sfamily = family;
      stype = sock_type;
      sprot = protocol;
      port = -1;
      return 0;
   }

   DLLLOCAL int reuse(int opt) {
      //printf("qore_socket_private::reuse(%s)\n", opt ? "true" : "false");
      return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (SETSOCKOPT_ARG_4)&opt, sizeof(int));
   }

   DLLLOCAL int bindIntern(struct sockaddr* ai_addr, size_t ai_addrlen, int prt, bool reuseaddr, ExceptionSink* xsink = 0) {
      reuse(reuseaddr);

      if ((::bind(sock, ai_addr, ai_addrlen)) == QORE_SOCKET_ERROR) {
	 qore_socket_error(xsink, "SOCKET-BIND-ERROR", "error in bind()", 0, 0, 0, ai_addr);
	 close();
	 return -1;
      }

      // set port number
      if (prt)
	 port = prt;
      else {
	 // get port number
#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
	 // on HPUX 64-bit the OS defines socklen_t to be 8 bytes, but the library expects a 32-bit value
	 int len = ai_addrlen;
#else
	 socklen_t len = ai_addrlen;
#endif

	 if (getsockname(sock, ai_addr, &len))
	    port = -1;
	 else
	    port = q_get_port_from_addr(ai_addr);
      }
      return 0;
   }

   // bind to UNIX domain socket file
   DLLLOCAL int bindUNIX(const char* name, int socktype = SOCK_STREAM, int protocol = 0, ExceptionSink* xsink = 0) {
#ifdef _Q_WINDOWS
      xsink->raiseException("SOCKET-BINDUNIX-ERROR", "UNIX sockets are not available under Windows");
      return -1;
#else
      close();

      // try to open socket if necessary
      if (openUNIX(socktype, protocol)) {
	 if (xsink)
	    xsink->raiseErrnoException("SOCKET-BIND-ERROR", errno, "error opening UNIX socket ('%s') for bind", name);
	 return -1;
      }

      struct sockaddr_un addr;
      addr.sun_family = AF_UNIX;
      // copy path and terminate if necessary
      strncpy(addr.sun_path, name, sizeof(addr.sun_path) - 1);
      addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

      if (bindIntern((sockaddr*)&addr, sizeof(struct sockaddr_un), -1, false, xsink))
	 return -1;

      // save socket file name for deleting on close
      socketname = addr.sun_path;
      // delete UNIX domain socket on close
      del = true;
      return 0;
#endif // windows
   }

   DLLLOCAL int bindINET(const char* name, const char* service, bool reuseaddr = true, int family = AF_UNSPEC, int socktype = SOCK_STREAM, int protocol = 0, ExceptionSink* xsink = 0) {
      family = q_get_af(family);
      socktype = q_get_sock_type(socktype);

      close();

      QoreAddrInfo ai;
      do_resolve_event(name, service);
      if (ai.getInfo(xsink, name, service, family, AI_PASSIVE, socktype, protocol))
	 return -1;

      struct addrinfo *aip = ai.getAddrInfo();
      // first emit all "resolved" events
      if (cb_queue)
	 for (struct addrinfo *p = aip; p; p = p->ai_next)
	    do_resolved_event(p->ai_addr);

      // try to open socket if necessary
      if (openINET(aip->ai_family, aip->ai_socktype, protocol)) {
	 qore_socket_error(xsink, "SOCKET-BINDINET-ERROR", "error opening socket for bind", 0, name, service);
	 return -1;
      }

      int prt = q_get_port_from_addr(aip->ai_addr);

      int en = 0;
      // iterate through addresses and bind to the first interface possible
      for (struct addrinfo *p = aip; p; p = p->ai_next) {
	 if (!bindIntern(p->ai_addr, p->ai_addrlen, prt, reuseaddr)) {
	   //printd(0, "qore_socket_private::bindINET(family: %d) bound: name: %s service: %s f: %d st: %d p: %d\n", family, name ? name : "(null)", service ? service : "(null)", p->ai_family, p->ai_socktype, p->ai_protocol);
	    return 0;
	 }

	 en = sock_get_raw_error();
	 //printd(0, "qore_socket_private::bindINET() failed to bind: name: %s service: %s f: %d st: %d p: %d, errno: %d (%s)\n", name ? name : "(null)", service ? service : "(null)", p->ai_family, p->ai_socktype, p->ai_protocol, en, strerror(en));
      }

      // if no bind was possible, then raise an exception
      qore_socket_error_intern(en, xsink, "SOCKET-BIND-ERROR", "error binding on socket", 0, name, service);
      return -1;
   }

   DLLLOCAL QoreHashNode* getPeerInfo(ExceptionSink* xsink, bool host_lookup = true) const {
      if (sock == QORE_INVALID_SOCKET) {
	 xsink->raiseException("SOCKET-GETPEERINFO-ERROR", "socket is not open()");
	 return 0;
      }

      struct sockaddr_storage addr;
      socklen_t len = sizeof addr;
      if (getpeername(sock, (struct sockaddr*)&addr, &len)) {
	 qore_socket_error(xsink, "SOCKET-GETPEERINFO-ERROR", "error in getpeername()");
	 return 0;
      }

      return getAddrInfo(addr, len, host_lookup);
   }

   DLLLOCAL QoreHashNode* getSocketInfo(ExceptionSink* xsink, bool host_lookup = true) const {
      if (sock == QORE_INVALID_SOCKET) {
	 xsink->raiseException("SOCKET-GETSOCKETINFO-ERROR", "socket is not open()");
	 return 0;
      }

      struct sockaddr_storage addr;
#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
      // on HPUX 64-bit the OS defines socklen_t to be 8 bytes, but the library expects a 32-bit value
      int len = sizeof addr;
#else
      socklen_t len = sizeof addr;
#endif

      if (getsockname(sock, (struct sockaddr*)&addr, &len)) {
	 qore_socket_error(xsink, "SOCKET-GETSOCKETINFO-ERROR", "error in getsockname()");
	 return 0;
      }

      return getAddrInfo(addr, len, host_lookup);
   }

   DLLLOCAL QoreHashNode* getAddrInfo(const struct sockaddr_storage& addr, socklen_t len, bool host_lookup = true) const {
      QoreHashNode* h = new QoreHashNode;

      if (addr.ss_family == AF_INET || addr.ss_family == AF_INET6) {
	 if (host_lookup) {
	    char host[NI_MAXHOST + 1];

	    if (!getnameinfo((struct sockaddr*)&addr, get_in_len((struct sockaddr*)&addr), host, sizeof(host), 0, 0, 0)) {
	       QoreStringNode* hoststr = new QoreStringNode(host);
	       h->setKeyValue("hostname", hoststr, 0);
	       h->setKeyValue("hostname_desc", QoreAddrInfo::getAddressDesc(addr.ss_family, hoststr->getBuffer()), 0);
	    }
	 }

	 // get ipv4 or ipv6 address
	 char ifname[INET6_ADDRSTRLEN];
	 if (inet_ntop(addr.ss_family, get_in_addr((struct sockaddr*)&addr), ifname, sizeof(ifname))) {
	    QoreStringNode* addrstr = new QoreStringNode(ifname);
	    h->setKeyValue("address", addrstr, 0);
	    h->setKeyValue("address_desc", QoreAddrInfo::getAddressDesc(addr.ss_family, addrstr->getBuffer()), 0);
	 }

	 int tport;
	 if (addr.ss_family == AF_INET) {
	    struct sockaddr_in* s = (struct sockaddr_in*)&addr;
	    tport = ntohs(s->sin_port);
	 }
	 else {
	    struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
	    tport = ntohs(s->sin6_port);
	 }

	 h->setKeyValue("port", new QoreBigIntNode(tport), 0);
      }
#ifndef _Q_WINDOWS
      else if (addr.ss_family == AF_UNIX) {
	 assert(!socketname.empty());
	 QoreStringNode* addrstr = new QoreStringNode(socketname);
	 h->setKeyValue("address", addrstr, 0);
	 h->setKeyValue("address_desc", QoreAddrInfo::getAddressDesc(addr.ss_family, addrstr->getBuffer()), 0);
      }
#endif

      h->setKeyValue("family", new QoreBigIntNode(addr.ss_family), 0);
      h->setKeyValue("familystr", new QoreStringNode(QoreAddrInfo::getFamilyName(addr.ss_family)), 0);

      return h;
   }

   // set backwards-compatible object members on accept
   // to be (hopefully) deleted in a future version of qore
   DLLLOCAL void setAccept(QoreObject* o) {
      struct sockaddr_storage addr;

      socklen_t len = sizeof addr;
      if (getpeername(sock, (struct sockaddr*)&addr, &len))
	 return;

      if (addr.ss_family == AF_INET || addr.ss_family == AF_INET6) {
	 // get ipv4 or ipv6 address
	 char ifname[INET6_ADDRSTRLEN];
	 if (inet_ntop(addr.ss_family, get_in_addr((struct sockaddr *)&addr), ifname, sizeof(ifname))) {
	    //printd(5, "inet_ntop() '%s' host: '%s'\n", ifname, host);
	    o->setValue("source", new QoreStringNode(ifname), 0);
	 }

	 char host[NI_MAXHOST + 1];
	 if (!getnameinfo((struct sockaddr *)&addr, get_in_len((struct sockaddr *)&addr), host, sizeof(host), 0, 0, 0))
	    o->setValue("source_host", new QoreStringNode(host), 0);
      }
#ifndef _Q_WINDOWS
      else if (addr.ss_family == AF_UNIX) {
	 QoreStringNode* astr = new QoreStringNode(enc);
	 struct sockaddr_un *addr_un = (struct sockaddr_un *)&addr;
	 astr->sprintf("UNIX socket: %s", addr_un->sun_path);
	 o->setValue("source", astr, 0);
	 o->setValue("source_host", new QoreStringNode("localhost"), 0);
      }
#endif
   }

   // buffered reads for high performance
   DLLLOCAL qore_offset_t brecv(ExceptionSink* xsink, const char* meth, char*& buf, qore_size_t bs, int flags, int timeout, bool do_event = true) {
      // must be checked if open/connected before this function is called
      assert(sock != QORE_INVALID_SOCKET);
      assert(meth);

      // always returned buffered data first
      if (buflen) {
	 buf = rbuf + bufoffset;
	 if (buflen <= bs) {
	    bs = buflen;
	    buflen = 0;
	    bufoffset = 0;
	 }
	 else {
	    buflen -= bs;
	    bufoffset += bs;
	 }
	 return (qore_offset_t)bs;
      }

      // real socket reads are only done when the buffer is empty

      //printd(5, "qore_socket_private::brecv(buf=%p, bs=%d, flags=%d, timeout=%d, do_event=%d) this=%p ssl=%d\n", buf, (int)bs, flags, timeout, (int)do_event, this, ssl);

      qore_offset_t rc;
      if (!ssl) {
	 if (timeout != -1 && !isDataAvailable(timeout, meth, xsink)) {
	    if (xsink) {
	       if (*xsink)
		  return -1;
	       se_timeout("Socket", meth, timeout, xsink);
	    }

	    return QSE_TIMEOUT;
	 }

	 while (true) {
#ifdef DEBUG
	    errno = 0;
#endif
	    rc = ::recv(sock, rbuf, DEFAULT_SOCKET_BUFSIZE, flags);
	    if (rc == QORE_SOCKET_ERROR) {
	       sock_get_error();
	       if (errno == EINTR)
		  continue;
#ifdef ECONNRESET
	       if (errno == ECONNRESET) {
		  if (xsink)
		     se_closed("Socket", meth, xsink);
		  close();
	       }
	       else
#endif
	       if (xsink)
		  qore_socket_error(xsink, "SOCKET-RECV-ERROR", "error in recv()", meth);
	       break;
	    }
	    //printd(5, "qore_socket_private::brecv(%d, %p, %ld, %d) rc=%ld errno=%d\n", sock, buf, bs, flags, rc, errno);
	    // try again if we were interrupted by a signal
	    if (rc >= 0)
	       break;
	 }
      }
      else
	 rc = ssl->read(meth, rbuf, DEFAULT_SOCKET_BUFSIZE, timeout, xsink);

      //printd(5, "qore_socket_private::brecv(%d, %p, %ld, %d) rc: %ld errno: %d\n", sock, buf, bs, flags, rc, errno);
      if (rc > 0) {
	 buf = rbuf;
	 assert(!buflen);
	 assert(!bufoffset);
	 if (rc > (qore_offset_t)bs) {
	    buflen = rc - bs;
	    bufoffset = bs;
	    rc = bs;
	 }

	 // register event
	 if (do_event)
	    do_read_event(rc, rc);
      }
      else {
#ifdef DEBUG
	 buf = 0;
#endif
	 if (!rc)
	    close();
      }

      return rc;
   }

   //! read until \\r\\n\\r\\n and return the string
   DLLLOCAL QoreStringNode* readHTTPData(ExceptionSink* xsink, const char* meth, int timeout, qore_offset_t& rc, bool exit_early = false) {
      assert(meth);
      if (sock == QORE_INVALID_SOCKET) {
	 if (xsink)
	    se_not_open("Socket", meth, xsink);
	 rc = QSE_NOT_OPEN;
	 return 0;
      }

      PrivateQoreSocketThroughputHelper th(this, false);

      // state:
      //   0 = '\r' received
      //   1 = '\r\n' received
      //   2 = '\r\n\r' received
      //   3 = '\n' received
      // read in HHTP header until \r\n\r\n or \n\n from socket
      int state = -1;
      QoreStringNodeHolder hdr(new QoreStringNode(enc));

      qore_size_t count = 0;

      while (true) {
	 char* buf;
	 rc = brecv(xsink, meth, buf, 1, 0, timeout, false);
	 //printd(5, "qore_socket_private::readHTTPData() this: %p Socket::%s(): rc: "QLLD" read char: %c (%03d) (old state: %d)\n", this, meth, rc, rc > 0 && buf[0] > 31 ? buf[0] : '?', rc > 0 ? buf[0] : 0, state);
	 if (rc <= 0) {
	    //printd(5, "qore_socket_private::readHTTPData(timeout=%d) hdr='%s' (len: %d), rc="QSD", errno=%d: '%s'\n", timeout, hdr->getBuffer(), hdr->strlen(), rc, errno, strerror(errno));

	    if (xsink && !*xsink) {
	       if (!count) {
		  //printd(5, "qore_socket_private::readHTTPData() this: %p rc: %d count: %d (%d) timeout: %d\n", this, rc, count, hdr->size(), timeout);
		  se_closed("Socket", meth, xsink);
	       }
	       else
		  xsink->raiseExceptionArg("SOCKET-HTTP-ERROR", hdr.release(), "socket closed on remote end while reading header data after reading "QSD" byte%s", count, count == 1 ? "" : "s");
	    }
	    return 0;
	 }
	 char c = buf[0];
	 if (++count == QORE_MAX_HEADER_SIZE) {
	    if (xsink)
	       xsink->raiseException("SOCKET-HTTP-ERROR", "header size cannot exceed "QSD" bytes", count);
	    return 0;
	 }

	 // check if we can progress to the next state
	 if (c == '\n') {
	    if (state == -1) {
	       state = 3;
	       continue;
	    }
	    if (!state) {
               if (exit_early && hdr->empty())
                  return 0;
	       state = 1;
	       continue;
	    }
	    assert(state > 0);
	    break;
	 }
	 else if (c == '\r') {
	    if (state == -1) {
	       state = 0;
	       continue;
	    }
	    if (!state)
	       break;
	    if (state == 1) {
	       state = 2;
	       continue;
	    }
	 }

	 if (state != -1) {
	    switch (state) {
	       case 0: hdr->concat('\r'); break;
	       case 1: hdr->concat("\r\n"); break;
	       case 2: hdr->concat("\r\n\r"); break;
	       case 3: hdr->concat('\n'); break;
	    }
	    state = -1;
	 }
	 hdr->concat(c);
      }
      hdr->concat('\n');

      //printd(5, "qore_socket_private::readHTTPData(timeout: %d) hdr='%s' (%d)\n", timeout, hdr->getBuffer(), hdr->size());

      th.finalize(hdr->size());

      return hdr.release();
   }

   DLLLOCAL QoreStringNode* recv(qore_offset_t bufsize, int timeout, qore_offset_t& rc, ExceptionSink* xsink) {
      if (sock == QORE_INVALID_SOCKET) {
	 if (xsink)
	    se_not_open("Socket", "recv", xsink);
	 rc = QSE_NOT_OPEN;
	 return 0;
      }
      if (in_op >= 0) {
         if (in_op == gettid()) {
            if (xsink)
               se_in_op("Socket", "recv", xsink);
            return 0;
         }
         if (xsink)
            se_in_op_thread("Socket", "recv", xsink);
         return 0;
      }

      PrivateQoreSocketThroughputHelper th(this, false);

      qore_size_t bs = bufsize > 0 && bufsize < DEFAULT_SOCKET_BUFSIZE ? bufsize : DEFAULT_SOCKET_BUFSIZE;

      QoreStringNodeHolder str(new QoreStringNode(enc));

      char* buf;

      while (true) {
	 rc = brecv(xsink, "recv", buf, bs, 0, timeout, false);

	 if (rc <= 0) {
	    printd(5, "qore_socket_private::recv(%d, %d) bs="QSD", br="QSD", rc="QSD", errno=%d (%s)\n", bufsize, timeout, bs, str->size(), rc, errno, strerror(errno));
	    break;
	 }

	 str->concat(buf, rc);

	 // register event
	 do_read_event(rc, str->size(), bufsize);

	 if (bufsize > 0) {
	    if (str->size() >= (qore_size_t)bufsize)
	       break;
	    if ((bufsize - str->size()) < bs)
	       bs = bufsize - str->size();
	 }
      }

      printd(5, "qore_socket_private::recv() received "QSD" byte(s), bufsize="QSD", strlen="QSD" str='%s'\n", str->size(), bufsize, (size_t)(str ? str->strlen() : 0), str ? str->getBuffer() : "n/a");

      // "fix" return code value if no error occurred
      if (rc >= 0)
	 rc = str->size();

      th.finalize(str->size());

      return *xsink ? 0 : str.release();
   }

   DLLLOCAL QoreStringNode* recv(int timeout, qore_offset_t& rc, ExceptionSink* xsink) {
      if (sock == QORE_INVALID_SOCKET) {
	 if (xsink)
	    se_not_open("Socket", "recv", xsink);
	 rc = QSE_NOT_OPEN;
	 return 0;
      }
      if (in_op >= 0) {
         if (in_op == gettid()) {
            if (xsink)
               se_in_op("Socket", "recv", xsink);
            return 0;
         }
         if (xsink)
            se_in_op_thread("Socket", "recv", xsink);
         return 0;
      }

      PrivateQoreSocketThroughputHelper th(this, false);

      QoreStringNodeHolder str(new QoreStringNode(enc));

      // perform first read with timeout
      char* buf;
      rc = brecv(xsink, "recv", buf, DEFAULT_SOCKET_BUFSIZE, 0, timeout, false);
      if (rc <= 0)
	 return 0;

      str->concat(buf, rc);

      // register event
      do_read_event(rc, rc);

      // keep reading data until no more data is available without a timeout
      if (isDataAvailable(0, "recv", xsink)) {
	 do {
	    rc = brecv(xsink, "recv", buf, DEFAULT_SOCKET_BUFSIZE, 0, 0, false);
	    //printd(5, "qore_socket_private::recv(to=%d) rc="QSD" rd="QSD"\n", timeout, rc, str->size());
	    // if the remote end has closed the connection, return what we have
	    if (!rc)
	       break;
	    if (rc < 0) {
	       th.finalize(str->size());
	       return 0;
	    }
	    str->concat(buf, rc);

	    // register event
	    do_read_event(rc, str->size());
	 } while (isDataAvailable(0, "recv", xsink));
      }

      th.finalize(str->size());

      if (*xsink)
	 return 0;

      rc = str->size();
      return str.release();
   }

   DLLLOCAL BinaryNode* recvBinary(qore_offset_t bufsize, int timeout, qore_offset_t& rc, ExceptionSink* xsink) {
      if (sock == QORE_INVALID_SOCKET) {
	 if (xsink)
	    se_not_open("Socket", "recvBinary", xsink);
	 rc = QSE_NOT_OPEN;
	 return 0;
      }
      if (in_op >= 0) {
         if (in_op == gettid()) {
            if (xsink)
               se_in_op("Socket", "recvBinary", xsink);
            return 0;
         }
         if (xsink)
            se_in_op_thread("Socket", "recvBinary", xsink);
         return 0;
      }

      PrivateQoreSocketThroughputHelper th(this, false);

      qore_size_t bs = bufsize > 0 && bufsize < DEFAULT_SOCKET_BUFSIZE ? bufsize : DEFAULT_SOCKET_BUFSIZE;

      SimpleRefHolder<BinaryNode> b(new BinaryNode);

      char* buf;
      while (true) {
	 rc = brecv(xsink, "recvBinary", buf, bs, 0, timeout);
	 if (rc <= 0)
	    break;

	 b->append(buf, rc);

	 if (bufsize > 0) {
	    if (b->size() >= (qore_size_t)bufsize)
	       break;
	    if ((bufsize - b->size()) < bs)
	       bs = bufsize - b->size();
	 }
      }

      th.finalize(b->size());

      if (*xsink)
	 return 0;

      // "fix" return code value if no error occurred
      if (rc >= 0)
	 rc = b->size();

      printd(5, "qore_socket_private::recvBinary() received "QSD" byte(s), bufsize="QSD", blen="QSD"\n", b->size(), bufsize, b->size());
      return b.release();
   }

   DLLLOCAL BinaryNode* recvBinary(int timeout, qore_offset_t& rc, ExceptionSink* xsink) {
      if (sock == QORE_INVALID_SOCKET) {
	 if (xsink)
	    se_not_open("Socket", "recvBinary", xsink);
	 rc = QSE_NOT_OPEN;
	 return 0;
      }
      if (in_op >= 0) {
         if (in_op == gettid()) {
            if (xsink)
               se_in_op("Socket", "recvBinary", xsink);
            return 0;
         }
         if (xsink)
            se_in_op_thread("Socket", "recvBinary", xsink);
         return 0;
      }

      PrivateQoreSocketThroughputHelper th(this, false);

      SimpleRefHolder<BinaryNode> b(new BinaryNode);

      //printd(5, "QoreSocket::recvBinary(%d, "QSD") this=%p\n", timeout, rc, this);
      // perform first read with timeout
      char* buf;
      rc = brecv(xsink, "recvBinary", buf, DEFAULT_SOCKET_BUFSIZE, 0, timeout, false);
      if (rc <= 0)
	 return 0;

      b->append(buf, rc);

      // register event
      do_read_event(rc, rc);

      // keep reading data until no more data is available without a timeout
      if (isDataAvailable(0, "recvBinary", xsink)) {
	 do {
	    rc = brecv(xsink, "recvBinary", buf, DEFAULT_SOCKET_BUFSIZE, 0, 0, false);
	    // if the remote end has closed the connection, return what we have
	    if (!rc)
	       break;
	    if (rc < 0) {
	       th.finalize(b->size());
	       return 0;
	    }

	    b->append(buf, rc);

	    // register event
	    do_read_event(rc, b->size());
	 } while (isDataAvailable(0, "recvBinary", xsink));
      }

      th.finalize(b->size());

      if (*xsink)
	 return 0;

      rc = b->size();
      //printd(5, "qore_socket_private() this: %p b: %p size: %lld\n", this, b->getPtr(), rc);
      return b.release();
   }

   DLLLOCAL QoreStringNode* readHTTPHeaderString(ExceptionSink* xsink, int timeout, int source) {
      qore_offset_t rc;
      QoreStringNodeHolder hdr(readHTTPData(xsink, "readHTTPHeaderString", timeout, rc));
      if (!hdr) {
	 assert(*xsink);
	 return 0;
      }
      assert(rc > 0);
      return hdr.release();
   }

   DLLLOCAL AbstractQoreNode* readHTTPHeader(ExceptionSink* xsink, QoreHashNode* info, int timeout, qore_offset_t& rc, int source) {
      QoreStringNodeHolder hdr(readHTTPData(xsink, "readHTTPHeader", timeout, rc));
      if (!hdr) {
	 assert(*xsink);
	 return 0;
      }
      assert(rc > 0);

      const char* buf = hdr->getBuffer();

      char* p;
      if ((p = (char*)strstr(buf, "\r\n"))) {
	 *p = '\0';
	 p += 2;
      }
      else if ((p = (char*)strchr(buf, '\n'))) {
	 *p = '\0';
	 ++p;
      }
      else if ((p = (char*)strchr(buf, '\r'))) {
	 *p = '\0';
	 ++p;
      }
      // readHTTPData will only return a string that satisifies one of the above conditions,
      // however an embedded 0 could have been sent which would make the above searches invalid
      else {
	 if (xsink)
	    xsink->raiseException("SOCKET-HTTP-ERROR", "invalid header received with embedded nulls in Socket::readHTTPHeader()");
	 return 0;
      }

      char* t1;
      if (!(t1 = (char*)strstr(buf, "HTTP/"))) {
	 if (xsink) {
	    xsink->raiseExceptionArg("SOCKET-HTTP-ERROR", hdr.release(), "missing HTTP version string in first header line in Socket::readHTTPHeader()");
	    return 0;
	 }
	 return hdr.release();
      }

      QoreHashNode* h = new QoreHashNode;

#if 0
      h->setKeyValue("dbg_hdr", new QoreStringNode(buf), 0);
#endif

      // process header flags
      int flags = CHF_PROCESS;

      // get version
      {
	 QoreStringNode* hv = new QoreStringNode(t1 + 5, 3, enc);
	 h->setKeyValue("http_version", hv, 0);
	 if (*hv == "1.1")
	    flags |= CHF_HTTP11;
      }

      // if we are getting a response
      if (t1 == buf) {
	 char* t2 = (char*)strchr(buf + 8, ' ');
	 if (t2) {
	    t2++;
	    if (isdigit(*(t2))) {
	       h->setKeyValue("status_code", new QoreBigIntNode(atoi(t2)), 0);
	       if (strlen(t2) > 4) {
		  h->setKeyValue("status_message", new QoreStringNode(t2 + 4), 0);
	       }
	    }
	 }
	 if (info)
	    info->setKeyValue("response-uri", new QoreStringNode(buf), 0);
      }
      else { // get method and path
	 char* t2 = (char*)strchr(buf, ' ');
	 if (t2) {
	    *t2 = '\0';
	    h->setKeyValue("method", new QoreStringNode(buf), 0);
	    t2++;
	    t1 = strchr(t2, ' ');
	    if (t1) {
	       *t1 = '\0';
	       //printd(5, "found path '%s'\n", t2);
	       // the path is returned as-is with no decodings - use decode_url() to decode
	       h->setKeyValue("path", new QoreStringNode(t2, enc), 0);
	    }
	    if (info)
	       info->setKeyValue("request-uri", new QoreStringNode(buf), 0);
	 }
	 flags |= CHF_REQUEST;
      }

      bool close = convertHeaderToHash(h, p, flags, info, &http_exp_chunked_body);
      do_read_http_header(QORE_EVENT_HTTP_MESSAGE_RECEIVED, h, source);

      // process header info
      if ((flags & CHF_REQUEST) && info)
	 info->setKeyValue("close", get_bool_node(close), 0);

      return h;
   }

   DLLLOCAL int runHeaderCallback(ExceptionSink* xsink, const char* cname, const char* mname, const ResolvedCallReferenceNode& callback, QoreThreadLock* l, const QoreHashNode* hdr, bool send_aborted = false, QoreObject* obj = 0) {
      assert(obj);
      ReferenceHolder<QoreListNode> args(new QoreListNode, xsink);
      QoreHashNode* arg = new QoreHashNode;
      arg->setKeyValue("hdr", hdr ? hdr->refSelf() : 0, xsink);
      if (obj)
         arg->setKeyValue("obj", obj->refSelf(), xsink);
      arg->setKeyValue("send_aborted", get_bool_node(send_aborted), xsink);
      args->push(arg);

      ValueHolder rv(xsink);
      return runCallback(xsink, cname, mname, rv, callback, l, *args);
   }

   DLLLOCAL int runDataCallback(ExceptionSink* xsink, const char* cname, const char* mname, const ResolvedCallReferenceNode& callback, QoreThreadLock* l, const AbstractQoreNode* data, bool chunked) {
      ReferenceHolder<QoreListNode> args(new QoreListNode, xsink);
      QoreHashNode* arg = new QoreHashNode;
      arg->setKeyValue("data", data->realCopy(), xsink);
      arg->setKeyValue("chunked", get_bool_node(chunked), xsink);
      args->push(arg);

      ValueHolder rv(xsink);
      return runCallback(xsink, cname, mname, rv, callback, l, *args);
   }

   DLLLOCAL int runCallback(ExceptionSink* xsink, const char* cname, const char* mname, ValueHolder& res, const ResolvedCallReferenceNode& callback, QoreThreadLock* l, const QoreListNode* args = 0) {
      // FIXME: subtract callback execution time from socket performance measurement

      // unlock and execute callback
      {
         AutoUnlocker al(l);
         res = callback.execValue(args, xsink);
      }

      // check exception and socket status
      if (*xsink)
         return -1;

      if (sock == QORE_INVALID_SOCKET) {
         se_not_open(cname, mname, xsink);
         return QSE_NOT_OPEN;
      }

      return 0;
   }

   DLLLOCAL int sendHttpChunkedWithCallback(ExceptionSink* xsink, const char* cname, const char* mname, const ResolvedCallReferenceNode& send_callback, QoreThreadLock& l, int source, int timeout_ms = -1, bool* aborted = 0) {
      assert(xsink);
      assert(!aborted || !(*aborted));

      if (sock == QORE_INVALID_SOCKET) {
         se_not_open(cname, mname, xsink);
	 return QSE_NOT_OPEN;
      }
      if (in_op >= 0) {
         if (in_op == gettid()) {
            if (xsink)
               se_in_op(cname, mname, xsink);
            return 0;
         }
         if (xsink)
            se_in_op_thread(cname, mname, xsink);
         return 0;
      }

      PrivateQoreSocketThroughputHelper th(this, true);

      // set the non-blocking flag (for use with non-ssl connections)
      bool nb = (timeout_ms >= 0);
      // set non-blocking I/O (and restore on exit) if we have a timeout and a non-ssl connection
      OptionalNonBlockingHelper onbh(*this, !ssl && nb, xsink);
      if (*xsink)
         return -1;

      qore_socket_op_helper oh(this);

      qore_offset_t rc;
      int64 total = 0;
      bool done = false;

      while (!done) {
         // if we have response data already, then we assume an error and abort
         //if (aborted && isDataAvailable(0, mname, xsink)) {
         if (aborted) {
            bool data_available = tryReadSocketData(mname, xsink);
            //printd(5, "qore_socket_private::sendHttpChunkedWithCallback() this: %p aborted: %p iDA: %d\n", this, aborted, data_available);
            if (data_available || *xsink) {
               *aborted = true;
               return *xsink ? -1 : 0;
            }
         }

         // FIXME: subtract callback execution time from socket performance measurement
         ValueHolder res(xsink);
         rc = runCallback(xsink, cname, mname, res, send_callback, &l);
         if (rc)
            return rc;

         //printd(5, "qore_socket_private::sendHttpChunkedWithCallback() this: %p res: %s\n", this, get_type_name(*res));

         // check callback return val
         QoreString buf;

         switch (res->getType()) {
            case NT_STRING: {
               const QoreStringNode* str = res->get<const QoreStringNode>();
               if (str->empty()) {
                  done = true;
                  break;
               }
               buf.sprintf("%x\r\n", (int)str->size());
               buf.concat(str->getBuffer(), str->size());
               break;
            }

            case NT_BINARY: {
               const BinaryNode* b = res->get<const BinaryNode>();
               if (b->empty()) {
                  done = true;
                  break;
               }
               buf.sprintf("%x\r\n", (int)b->size());
               buf.concat((const char*)b->getPtr(), b->size());
               break;
            }

            case NT_HASH: {
               buf.concat("0\r\n");

               ConstHashIterator hi(res->get<const QoreHashNode>());

               while (hi.next()) {
                  const AbstractQoreNode* v = hi.getValue();
                  const char* key = hi.getKey();

                  //printd(5, "qore_socket_private::sendHttpChunkedWithCallback() this: %p trailer %s\n", this, key);

                  if (v && v->getType() == NT_LIST) {
                     ConstListIterator li(reinterpret_cast<const QoreListNode* >(v));
                     while (li.next())
                        do_header(key, buf, li.getValue());
                  }
                  else
                     do_header(key, buf, hi.getValue());
               }

               // fall through to next case
            }

            case NT_NOTHING:
            case NT_NULL:
               done = true;
               break;

            default:
               xsink->raiseException("SOCKET-CALLBACK-ERROR", "HTTP chunked data callback returned type '%s'; expecting one of: 'string', 'binary', 'hash', 'nothing' (or 'NULL')", res->getTypeName());
               return -1;
         }

         if (buf.empty())
            buf.concat("0\r\n");

         // add trailing \r\n
         buf.concat("\r\n");

         // send chunk buffer data
         rc = sendIntern(xsink, cname, mname, buf.getBuffer(), buf.size(), timeout_ms, total, true);

         if (rc < 0) {
            // if we have a socket I/O error, but also data to be read on the socket, then clear the exception and return 0
            if (aborted && *xsink) {
               bool data_available = tryReadSocketData(mname, xsink);
               //printd(5, "qore_socket_private::sendHttpChunkedWithCallback() this: %p aborted: %p iDA: %d\n", this, aborted, data_available);
               if (data_available) {
                  *aborted = true;
                  return *xsink ? -1 : 0;
               }
            }

            //printd(5, "qore_socket_private::sendHttpChunkedWithCallback() this: %p rc: %d sock: %d xsink: %d\n", this, rc, sock, xsink->isException());
         }

         //printd(5, "qore_socket_private::sendHttpChunkedWithCallback() this: %p sent: %s\n", this, buf.getBuffer());

         if (rc < 0 || sock == QORE_INVALID_SOCKET)
            break;
      }

      th.finalize(total);

      return rc < 0 || sock == QORE_INVALID_SOCKET ? -1 : 0;
   }

   DLLLOCAL int sendIntern(ExceptionSink* xsink, const char* cname, const char* mname, const char* buf, qore_size_t size, int timeout_ms, int64& total, bool stream = false) {
      qore_offset_t rc;
      qore_size_t bs = 0;

      // set the non-blocking flag (for use with non-ssl connections)
      bool nb = (timeout_ms >= 0);

      while (true) {
         if (ssl) {
            // SSL_MODE_ENABLE_PARTIAL_WRITE is enabled so we can get finer-grained socket events for do_send_event() below
            rc = ssl->write(mname, buf + bs, size - bs, timeout_ms, xsink);
         }
         else {
            while (true) {
               rc = ::send(sock, buf + bs, size - bs, 0);
               //printd(5, "qore_socket_private::send() this: %p Socket::%s() buf: %p size: "QLLD" timeout_ms: %d ssl: %p nb: %d bs: "QLLD" rc: "QLLD"\n", this, mname, buf, size, timeout_ms, ssl, nb, bs, rc);
               // try again if we were interrupted by a signal
               if (rc >= 0)
                  break;
	       sock_get_error();
               // check that the send finishes before the timeout if we are using non-blocking I/O
               if (nb && (errno == EAGAIN
#ifdef EWOULDBLOCK
			  || errno == EWOULDBLOCK
#endif
		      )) {
                  if (!isWriteFinished(timeout_ms, mname, xsink)) {
                     if (xsink) {
			if (*xsink)
			   return -1;
                        se_timeout("Socket", mname, timeout_ms, xsink);
		     }
                     rc = QSE_TIMEOUT;
                     break;
                  }
                  continue;
               }
               if (errno != EINTR) {
		  //printd(5, "qore_socket_private::send() bs: %ld rc: "QSD" len: "QSD" (total: "QSD") errno: %d sock: %d\n", bs, rc, size - bs, size, errno, sock);
                  if (xsink)
                     xsink->raiseErrnoException("SOCKET-SEND-ERROR", errno, "error while executing %s::%s()", cname, mname);

                  // do not close the socket even if we have EPIPE or ECONNRESET in case there is data to be read when streaming
#ifdef EPIPE
                  if (!stream && errno == EPIPE)
                     close();
#endif
#ifdef ECONNRESET
                  if (!stream && errno == ECONNRESET)
                     close();
#endif
                  break;
               }
            }
         }

	 total += rc;

	 //printd(5, "qore_socket_private::send() bs: %ld rc: "QSD" len: "QSD" (total: "QSD") errno: %d\n", bs, rc, size - bs, size, errno);
	 if (rc < 0 || sock == QORE_INVALID_SOCKET)
            break;

	 bs += rc;

	 do_send_event(rc, bs, size);

	 if (bs >= size)
	    break;
      }

      return rc;
   }

   DLLLOCAL int send(ExceptionSink* xsink, const char* mname, const char* buf, qore_size_t size, int timeout_ms = -1) {
      return send(xsink, "Socket", mname, buf, size, timeout_ms);
   }

   DLLLOCAL int send(ExceptionSink* xsink, const char* cname, const char* mname, const char* buf, qore_size_t size, int timeout_ms = -1) {
      if (sock == QORE_INVALID_SOCKET) {
	 if (xsink)
	    se_not_open(cname, mname, xsink);

	 return QSE_NOT_OPEN;
      }
      if (in_op >= 0) {
         if (in_op == gettid()) {
            if (xsink)
               se_in_op(cname, mname, xsink);
            return 0;
         }
         if (xsink)
            se_in_op_thread(cname, mname, xsink);
         return 0;
      }

      PrivateQoreSocketThroughputHelper th(this, true);

      // set the non-blocking flag (for use with non-ssl connections)
      bool nb = (timeout_ms >= 0);
      // set non-blocking I/O (and restore on exit) if we have a timeout and a non-ssl connection
      OptionalNonBlockingHelper onbh(*this, !ssl && nb, xsink);
      if (*xsink)
         return -1;

      int64 total = 0;
      qore_offset_t rc = sendIntern(xsink, cname, mname, buf, size, timeout_ms, total);
      th.finalize(total);

      return rc < 0 || sock == QORE_INVALID_SOCKET ? rc : 0;
   }

   DLLLOCAL int sendHttpMessage(ExceptionSink* xsink, QoreHashNode* info, const char* cname, const char* mname, const char* method, const char* path, const char* http_version, const QoreHashNode* headers, const void *data, qore_size_t size, const ResolvedCallReferenceNode* send_callback, int source, int timeout_ms = -1, QoreThreadLock* l = 0, bool* aborted = 0) {
      assert(!(data && send_callback));
      // prepare header string
      QoreString hdr(enc);

      hdr.sprintf("%s %s HTTP/%s", method, path && path[0] ? path : "/", http_version);

      // write request-uri key if info hash is non-null
      if (info)
	 info->setKeyValue("request-uri", new QoreStringNode(hdr), 0);

      do_send_http_message(hdr, headers, source);
      hdr.concat("\r\n");

      // insert headers
      do_headers(hdr, headers, size && data ? size : 0);

      //printd(5, "qore_socket_private::sendHttpMessage() hdr: %s\n", hdr.getBuffer());

      int rc;
      if ((rc = send(xsink, cname, mname, hdr.getBuffer(), hdr.strlen(), timeout_ms)))
	 return rc;

      if (size && data)
         return send(xsink, cname, mname, (char*)data, size, timeout_ms);
      else if (send_callback) {
         assert(l);
         assert(!aborted || !(*aborted));
         return sendHttpChunkedWithCallback(xsink, cname, mname, *send_callback, *l, source, timeout_ms, aborted);
      }

      return 0;
   }

   DLLLOCAL int sendHttpResponse(ExceptionSink* xsink, const char* cname, const char* mname, int code, const char* desc, const char* http_version, const QoreHashNode* headers, const void *data, qore_size_t size, const ResolvedCallReferenceNode* send_callback, int source, int timeout_ms = -1, QoreThreadLock* l = 0, bool* aborted = 0) {
      assert(!(data && send_callback));
      // prepare header string
      QoreString hdr(enc);

      hdr.sprintf("HTTP/%s %03d %s", http_version, code, desc);

      do_send_http_message(hdr, headers, source);

      hdr.concat("\r\n");

      do_headers(hdr, headers, size && data ? size : 0, true);

      //printd(5, "QoreSocket::sendHTTPResponse() this: %p data: %p size: %ld send_callback: %p hdr: %s", this, data, size, send_callback, hdr.getBuffer());

      int rc;
      if ((rc = send(xsink, cname, mname, hdr.getBuffer(), hdr.strlen(), timeout_ms)))
	 return rc;

      if (size && data)
         return send(xsink, cname, mname, (char*)data, size, timeout_ms);
      else if (send_callback) {
         assert(l);
         assert(!aborted || !(*aborted));
         return sendHttpChunkedWithCallback(xsink, cname, mname, *send_callback, *l, source, timeout_ms, aborted);
      }

      return 0;
   }

   DLLLOCAL QoreHashNode* readHttpChunkedBodyBinary(int timeout, ExceptionSink* xsink, const char* cname, int source, const ResolvedCallReferenceNode* recv_callback = 0, QoreThreadLock* l = 0, QoreObject* obj = 0) {
      assert(xsink);

      if (sock == QORE_INVALID_SOCKET) {
         se_not_open(cname, "readHTTPChunkedBodyBinary", xsink);
         return 0;
      }
      if (in_op >= 0) {
         if (in_op == gettid()) {
            se_in_op(cname, "readHTTPChunkedBodyBinary", xsink);
            return 0;
         }
         se_in_op_thread(cname, "readHTTPChunkedBodyBinary", xsink);
         return 0;
      }

      // reset "expecting HTTP chunked body" flag
      if (http_exp_chunked_body)
         http_exp_chunked_body = false;

      qore_socket_op_helper oh(this);

      SimpleRefHolder<BinaryNode> b(new BinaryNode);
      QoreString str; // for reading the size of each chunk

      qore_offset_t rc;
      // read the size then read the data and append to buffer
      while (true) {
         // state = 0, nothing
         // state = 1, \r received
         int state = 0;
         while (true) {
            char* buf;
            rc = brecv(xsink, "readHTTPChunkedBodyBinary", buf, 1, 0, timeout, false);
            if (rc <= 0) {
               if (!*xsink) {
                  assert(!rc);
                  se_closed(cname, "readHTTPChunkedBodyBinary", xsink);
               }
               return 0;
            }

            char c = buf[0];

            if (!state && c == '\r')
               state = 1;
            else if (state && c == '\n')
               break;
            else {
               if (state) {
                  state = 0;
                  str.concat('\r');
               }
               str.concat(c);
            }
         }
         // DEBUG
         //printd(5, "QoreSocket::readHTTPChunkedBodyBinary(): got chunk size ("QSD" bytes) string: %s\n", str.strlen(), str.getBuffer());

         // terminate string at ';' char if present
         char* p = (char*)strchr(str.getBuffer(), ';');
         if (p)
            *p = '\0';
         long size = strtol(str.getBuffer(), 0, 16);
         do_chunked_read(QORE_EVENT_HTTP_CHUNK_SIZE, size, str.strlen(), source);

         if (!size)
            break;

         if (size < 0) {
            xsink->raiseException("READ-HTTP-CHUNK-ERROR", "negative value given for chunk size (%ld)", size);
            return 0;
         }

         // prepare string for chunk
         //str.allocate(size + 1);

         qore_offset_t bs = size < DEFAULT_SOCKET_BUFSIZE ? size : DEFAULT_SOCKET_BUFSIZE;
         qore_offset_t br = 0; // bytes received
         while (true) {
            char* buf;
            rc = brecv(xsink, "readHTTPChunkedBodyBinary", buf, bs, 0, timeout, false);
            if (rc <= 0) {
               if (!*xsink) {
                  assert(!rc);
                  se_closed(cname, "readHTTPChunkedBodyBinary", xsink);
               }
               return 0;
            }

            b->append(buf, rc);
            br += rc;

            if (br >= size)
               break;
            if (size - br < bs)
               bs = size - br;
         }

         // DEBUG
         //printd(5, "QoreSocket::readHTTPChunkedBodyBinary(): received binary chunk: size=%d br="QSD" total="QSD"\n", size, br, b->size());

         // read crlf after chunk
         // FIXME: bytes read are not checked if they equal CRLF
         br = 0;
         while (br < 2) {
            char* buf;
            rc = brecv(xsink, "readHTTPChunkedBodyBinary", buf, 2 - br, 0, timeout, false);
            if (rc <= 0) {
               if (!*xsink) {
                  assert(!rc);
                  se_closed(cname, "readHTTPChunkedBodyBinary", xsink);
               }
               return 0;
            }
            br += rc;
         }

         do_chunked_read(QORE_EVENT_HTTP_CHUNKED_DATA_RECEIVED, size, size + 2, source);

         if (recv_callback) {
            if (runDataCallback(xsink, cname, "readHTTPChunkedBodyBinary", *recv_callback, l, *b, true))
               return 0;
            b->clear();
         }

         // ensure string is blanked for next read
         str.clear();
      }

      // read footers or nothing
      QoreStringNodeHolder hdr(readHTTPData(xsink, "readHTTPChunkedBodyBinary", timeout, rc, true));
      if (*xsink)
         return 0;

      ReferenceHolder<QoreHashNode> h(new QoreHashNode, xsink);
      if (!recv_callback)
         h->setKeyValue("body", b.release(), xsink);

      if (hdr) {
         if (hdr->strlen() >= 2 && hdr->strlen() <= 4)
            return recv_callback ? 0 : h.release();

         convertHeaderToHash(*h, (char*)hdr->getBuffer());
         do_read_http_header(QORE_EVENT_HTTP_FOOTERS_RECEIVED, *h, source);
      }

      if (recv_callback) {
         runHeaderCallback(xsink, cname, "readHTTPChunkedBodyBinary", *recv_callback, l, h->empty() ? 0 : *h, false, obj);
         return 0;
      }

      return h.release();
   }

   // receive a message in HTTP chunked format
   DLLLOCAL QoreHashNode* readHttpChunkedBody(int timeout, ExceptionSink* xsink, const char* cname, int source, const ResolvedCallReferenceNode* recv_callback = 0, QoreThreadLock* l = 0, QoreObject* obj = 0) {
      assert(xsink);

      if (sock == QORE_INVALID_SOCKET) {
         se_not_open(cname, "readHTTPChunkedBody", xsink);
         return 0;
      }
      if (in_op >= 0) {
         if (in_op == gettid()) {
            se_in_op(cname, "readHTTPChunkedBody", xsink);
            return 0;
         }
         se_in_op_thread(cname, "readHTTPChunkedBody", xsink);
         return 0;
      }

      // reset "expecting HTTP chunked body" flag
      if (http_exp_chunked_body)
         http_exp_chunked_body = false;

      qore_socket_op_helper oh(this);

      QoreStringNodeHolder buf(new QoreStringNode(enc));
      QoreString str; // for reading the size of each chunk

      qore_offset_t rc;
      // read the size then read the data and append to buf
      while (true) {
         // state = 0, nothing
         // state = 1, \r received
         int state = 0;
         while (true) {
            char* tbuf;
            rc = brecv(xsink, "readHTTPChunkedBody", tbuf, 1, 0, timeout, false);
            if (rc <= 0) {
               if (!*xsink) {
                  assert(!rc);
                  se_closed(cname, "readHTTPChunkedBody", xsink);
               }
               return 0;
            }

            char c = tbuf[0];

            if (!state && c == '\r')
               state = 1;
            else if (state && c == '\n')
               break;
            else {
               if (state) {
                  state = 0;
                  str.concat('\r');
               }
               str.concat(c);
            }
         }
         // DEBUG
         //printd(5, "got chunk size ("QSD" bytes) string: %s\n", str.strlen(), str.getBuffer());

         // terminate string at ';' char if present
         char* p = (char*)strchr(str.getBuffer(), ';');
         if (p)
            *p = '\0';
         qore_offset_t size = strtol(str.getBuffer(), 0, 16);
         do_chunked_read(QORE_EVENT_HTTP_CHUNK_SIZE, size, str.strlen(), source);

         if (!size)
            break;

         if (size < 0) {
            xsink->raiseException("READ-HTTP-CHUNK-ERROR", "negative value given for chunk size (%ld)", size);
            return 0;
         }
         // ensure string is blanked for next read
         str.clear();

         // prepare string for chunk
         //buf->allocate((unsigned)(buf->strlen() + size + 1));

         // read chunk directly into string buffer
         qore_offset_t bs = size < DEFAULT_SOCKET_BUFSIZE ? size : DEFAULT_SOCKET_BUFSIZE;
         qore_offset_t br = 0; // bytes received
         str.clear();
         while (true) {
            char* tbuf;
            rc = brecv(xsink, "readHTTPChunkedBody", tbuf, bs, 0, timeout, false);
            if (rc <= 0) {
               if (!*xsink) {
                  assert(!rc);
                  se_closed(cname, "readHTTPChunkedBody", xsink);
               }
               return 0;
            }
            br += rc;
            buf->concat(tbuf, rc);

            if (br >= size)
               break;
            if (size - br < bs)
               bs = size - br;
         }

         // DEBUG
         //printd(5, "got chunk ("QSD" bytes): %s\n", br, buf->getBuffer() + buf->strlen() -  size);

         // read crlf after chunk
         // FIXME: bytes read are not checked if they equal CRLF
         br = 0;
         while (br < 2) {
            char* tbuf;
            rc = brecv(xsink, "readHTTPChunkedBody", tbuf, 2 - br, 0, timeout, false);
            if (rc <= 0) {
               if (!*xsink) {
                  assert(!rc);
                  se_closed(cname, "readHTTPChunkedBody", xsink);
               }
               return 0;
            }
            br += rc;
         }

         do_chunked_read(QORE_EVENT_HTTP_CHUNKED_DATA_RECEIVED, size, size + 2, source);

         if (recv_callback) {
            if (runDataCallback(xsink, cname, "readHTTPChunkedBody", *recv_callback, l, *buf, true))
               return 0;
            buf->clear();
         }
      }

      // read footers or nothing
      QoreStringNodeHolder hdr(readHTTPData(xsink, "readHTTPChunkedBody", timeout, rc, true));
      if (*xsink)
         return 0;

      //printd(5, "chunked body encoding=%s\n", buf->getEncoding()->getCode());
      ReferenceHolder<QoreHashNode> h(new QoreHashNode, xsink);
      if (!recv_callback)
         h->setKeyValue("body", buf.release(), xsink);

      if (hdr) {
         if (hdr->strlen() >= 2 && hdr->strlen() <= 4)
            return recv_callback ? 0 : h.release();

         convertHeaderToHash(*h, (char*)hdr->getBuffer());
         do_read_http_header(QORE_EVENT_HTTP_FOOTERS_RECEIVED, *h, source);
      }

      if (recv_callback) {
         runHeaderCallback(xsink, cname, "readHTTPChunkedBody", *recv_callback, l, h->empty() ? 0 : *h, false, obj);
         return 0;
      }

      return h.release();
   }

   DLLLOCAL static void do_accept_encoding(char* t, QoreHashNode& info) {
      ReferenceHolder<QoreListNode> l(new QoreListNode, 0);

      char* a = t;
      bool ok = true;
      while (*a) {
	 if (ok) {
	    ok = false;
	    SimpleRefHolder<QoreStringNode> str(new QoreStringNode);
	    while (*a && *a != ';' && *a != ',')
	       str->concat(*(a++));
	    str->trim();
	    if (!str->empty())
	       l->push(str.release());
	    continue;
	 }
	 else if (*a == ',')
	    ok = true;

	 ++a;
      }

      if (!l->empty())
	 info.setKeyValue("accept-encoding", l.release(), 0);
   }

   DLLLOCAL bool do_accept_charset(char* t, QoreHashNode& info) {
      bool acceptcharset = false;

      // see if we have "*" or utf8 or utf-8, in which case set it
      // otherwise set the first charset in the list
      char* a = t;
      char* div = 0;
      bool utf8 = false;
      bool ok = true;
      while (*a) {
	 if (ok) {
	    if (*a == '*') {
	       utf8 = true;
	       break;
	    }
	    ok = false;
	    if (*a == 'u' || *a == 'U') {
	       ++a;
	       if (*a == 't' || *a == 'T') {
		  ++a;
		  if (*a == 'f' || *a == 'F') {
		     ++a;
		     if (*a == '-')
			++a;
		     if (*a == '8') {
			utf8 = true;
			break;
		     }
		  }
	       }
	       continue;
	    }
	 }
	 else if (*a == ',') {
	    if (!div)
	       div = a;
	    ok = true;
	 }
	 else if (*a == ';') {
	    if (!div)
	       div = a;
	 }

	 ++a;
      }
      if (utf8) {
	 info.setKeyValue("accept-charset", new QoreStringNode("utf8"), 0);
	 acceptcharset = true;
      }
      else {
	 SimpleRefHolder<QoreStringNode> ac(new QoreStringNode);
	 if (div)
	    ac->concat(t, div - t);
	 else
	    ac->concat(t);
	 ac->trim();
	 if (!ac->empty()) {
	    info.setKeyValue("accept-charset", ac.release(), 0);
	    acceptcharset = true;
	 }
      }

      return acceptcharset;
   }

   // returns true if the connection should be closed, false if not
   DLLLOCAL bool convertHeaderToHash(QoreHashNode* h, char* p, int flags = 0, QoreHashNode* info = 0, bool* chunked = 0) {
      bool close = !(flags & CHF_HTTP11);
      // socket encoding
      const char* senc = 0;
      // accept-charset
      bool acceptcharset = false;
      while (*p) {
	 char* buf = p;

	 if ((p = strstr(buf, "\r\n"))) {
	    *p = '\0';
	    p += 2;
	 }
	 else if ((p = strchr(buf, '\n'))) {
	    *p = '\0';
	    p++;
	 }
	 else if ((p = strchr(buf, '\r'))) {
	    *p = '\0';
	    p++;
	 }
	 else
	    break;
	 char* t = strchr(buf, ':');
	 if (!t)
	    break;
	 *t = '\0';
	 t++;
	 while (t && isblank(*t))
	    t++;
	 strtolower(buf);
	 //printd(5, "setting %s = '%s'\n", buf, t);

	 AbstractQoreNode* val = new QoreStringNode(t);

	 if (flags & CHF_PROCESS) {
	    if (!strcmp(buf, "connection")) {
	       if (flags & CHF_HTTP11) {
		  if (strcasestr(t, "close"))
		     close = true;
	       }
	       else {
		  if (strcasestr(t, "keep-alive"))
		     close = false;
	       }
	    }
	    else if (!strcmp(buf, "content-type")) {
	       char* a = strcasestr(t, "charset=");
	       if (a) {
		  // find end
		  char* e = strchr(a + 8, ';');

		  QoreString cs;
		  if (e)
		     cs.concat(a + 8, e - a - 8);
		  else
		     cs.concat(a + 8);
		  cs.trim();
		  senc = cs.getBuffer();
		  //printd(5, "got encoding '%s' from request\n", senc);
		  enc = QEM.findCreate(senc);

		  if (info) {
		     qore_size_t len = cs.size();
		     info->setKeyValue("charset", new QoreStringNode(cs.giveBuffer(), len, len + 1, QCS_DEFAULT), 0);
		  }

		  if (info) {
                     SimpleRefHolder<QoreStringNode> ct(new QoreStringNode);
		     // remove any whitespace and ';' before charset=
		     if (a != t) {
			do {
			   --a;
			} while (a > t && (*a == ' ' || *a == ';'));
		     }

		     if (a == t) {
			if (e)
			   ct->concat(e + 1);
		     }
		     else {
			ct->concat(t, a - t + 1);
			if (e)
			   ct->concat(e);
		     }
		     ct->trim();
		     if (!ct->empty())
			info->setKeyValue("body-content-type", ct.release(), 0);
		  }
	       }
	       else if (info) {
		  info->setKeyValue("charset", new QoreStringNode("iso-8859-1"), 0);
		  info->setKeyValue("body-content-type", val->refSelf(), 0);
	       }
	    }
            else if (chunked && !strcmp(buf, "transfer-encoding") && !strcasecmp(t, "chunked")) {
               *chunked = true;
            }
	    else if (info) {
	       if (!strcmp(buf, "accept-charset"))
		  acceptcharset = do_accept_charset(t, *info);
	       else if ((flags & CHF_REQUEST) && !strcmp(buf, "accept-encoding"))
		  do_accept_encoding(t, *info);
	    }
	 }

	 // see if header exists, and if so make it a list and add value to the list
	 hash_assignment_priv ha(*h, buf);
	 if (*ha) {
	    QoreListNode* l;
	    if ((*ha)->getType() == NT_LIST)
	       l = reinterpret_cast<QoreListNode* >(*ha);
	    else {
	       l = new QoreListNode;
	       l->push(ha.swap(l));
	    }
	    l->push(val);
	 }
	 else // otherwise set header normally
	    ha.assign(val, 0);
      }

      if ((flags & CHF_PROCESS)) {
	 if (!senc)
	    enc = QEM.findCreate("iso-8859-1");
	 // according to RFC-2616 section 14.2, "If no Accept-Charset header is present, the default is that any character set is acceptable" so we will use utf-8
	 if (info && !acceptcharset)
	    info->setKeyValue("accept-charset", new QoreStringNode("utf8"), 0);
      }

      return close;
   }

   DLLLOCAL int recvix(const char* meth, int len, void* targ, int timeout_ms, ExceptionSink* xsink) {
      if (sock == QORE_INVALID_SOCKET) {
	 if (xsink)
	    se_not_open("Socket", meth, xsink);
	 return QSE_NOT_OPEN;
      }
      if (in_op >= 0) {
         if (in_op == gettid()) {
            if (xsink)
               se_in_op("Socket", meth, xsink);
            return 0;
         }
         if (xsink)
            se_in_op_thread("Socket", meth, xsink);
         return 0;
      }

      PrivateQoreSocketThroughputHelper th(this, false);

      char* buf;
      qore_offset_t br = 0;
      while (true) {
	 qore_offset_t rc = brecv(xsink, meth, buf, len - br, 0, timeout_ms);
	 if (rc <= 0) {
	    do_read_error(rc, meth, timeout_ms, xsink);
	    return (int)rc;
	 }

	 memcpy(targ, buf, rc);

	 br += rc;
	 if (br >= len)
	    break;
      }

      th.finalize(br);

      return (int)br;
   }

   DLLLOCAL void clearWarningQueue(ExceptionSink* xsink) {
      if (warn_queue) {
         if (callback_arg) {
            callback_arg->deref(xsink);
            callback_arg = 0;
         }
	 warn_queue->deref(xsink);
	 warn_queue = 0;
	 tl_warning_us = 0;
	 tp_warning_bs = 0.0;
         tp_us_min = 0;
      }
   }

   DLLLOCAL void setWarningQueue(ExceptionSink* xsink, int64 warning_ms, int64 warning_bs, Queue* wq, AbstractQoreNode* arg, int64 min_ms = 1000) {
      ReferenceHolder<Queue> qholder(wq, xsink);
      ReferenceHolder<> holder(arg, xsink);
      if (warning_ms <=0 && warning_bs <= 0) {
	 xsink->raiseException("SOCKET-SETWARNINGQUEUE-ERROR", "Socket::setWarningQueue() at least one of warning ms argument: "QLLD" and warning B/s argument: "QLLD" must be greater than zero; to clear, call Socket::clearWarningQueue() with no arguments", warning_ms, warning_bs);
	 return;
      }

      if (warning_ms < 0)
	 warning_ms = 0;
      if (warning_bs < 0)
	 warning_bs = 0;

      if (warn_queue) {
	 warn_queue->deref(xsink);
	 discard(callback_arg, xsink);
      }

      warn_queue = qholder.release();
      callback_arg = holder.release();
      tl_warning_us = (int64)warning_ms * 1000;
      tp_warning_bs = warning_bs;
      tp_us_min = min_ms * 1000;
   }

   DLLLOCAL void getUsageInfo(QoreHashNode& h, qore_socket_private& s) const {
      if (warn_queue) {
	 h.setKeyValue("arg", callback_arg ? callback_arg->refSelf() : 0, 0);
	 h.setKeyValue("timeout", new QoreBigIntNode(tl_warning_us), 0);
	 h.setKeyValue("min_throughput", new QoreBigIntNode((int64)tp_warning_bs), 0);
         h.setKeyValue("min_throughput_us", new QoreBigIntNode((int64)tp_us_min), 0);
      }

      h.setKeyValue("bytes_sent", new QoreBigIntNode(tp_bytes_sent + s.tp_bytes_sent), 0);
      h.setKeyValue("bytes_recv", new QoreBigIntNode(tp_bytes_recv + s.tp_bytes_sent), 0);
      h.setKeyValue("us_sent", new QoreBigIntNode(tp_us_sent + s.tp_us_sent), 0);
      h.setKeyValue("us_recv", new QoreBigIntNode(tp_us_recv + s.tp_us_recv), 0);
   }

   DLLLOCAL void getUsageInfo(QoreHashNode& h) const {
      if (warn_queue) {
	 h.setKeyValue("arg", callback_arg ? callback_arg->refSelf() : 0, 0);
	 h.setKeyValue("timeout", new QoreBigIntNode(tl_warning_us), 0);
	 h.setKeyValue("min_throughput", new QoreBigIntNode((int64)tp_warning_bs), 0);
         h.setKeyValue("min_throughput_us", new QoreBigIntNode((int64)tp_us_min), 0);
      }

      h.setKeyValue("bytes_sent", new QoreBigIntNode(tp_bytes_sent), 0);
      h.setKeyValue("bytes_recv", new QoreBigIntNode(tp_bytes_recv), 0);
      h.setKeyValue("us_sent", new QoreBigIntNode(tp_us_sent), 0);
      h.setKeyValue("us_recv", new QoreBigIntNode(tp_us_recv), 0);
   }

   DLLLOCAL QoreHashNode* getUsageInfo() const {
      QoreHashNode* h = new QoreHashNode;
      getUsageInfo(*h);
      return h;
   }

   DLLLOCAL void clearStats() {
      tp_bytes_sent = 0;
      tp_bytes_recv = 0;
      tp_us_sent = 0;
      tp_us_recv = 0;
   }

   DLLLOCAL void doTimeoutWarning(const char* op, int64 dt) {
      assert(warn_queue);
      assert(dt > tl_warning_us);

      QoreHashNode* h = new QoreHashNode;

      h->setKeyValue("type", new QoreStringNode("SOCKET-OPERATION-WARNING"), 0);
      h->setKeyValue("operation", new QoreStringNode(op), 0);
      h->setKeyValue("us", new QoreBigIntNode(dt), 0);
      h->setKeyValue("timeout", new QoreBigIntNode(tl_warning_us), 0);
      if (callback_arg)
	 h->setKeyValue("arg", callback_arg->refSelf(), 0);

      warn_queue->pushAndTakeRef(h);
   }

   DLLLOCAL void doThroughputWarning(bool send, int64 bytes, int64 dt, double bs) {
      assert(warn_queue);
      assert(bs < tp_warning_bs);

      QoreHashNode* h = new QoreHashNode;

      h->setKeyValue("type", new QoreStringNode("SOCKET-THROUGHPUT-WARNING"), 0);
      h->setKeyValue("dir", new QoreStringNode(send ? "send" : "recv"), 0);
      h->setKeyValue("bytes", new QoreBigIntNode(bytes), 0);
      h->setKeyValue("us", new QoreBigIntNode(dt), 0);
      h->setKeyValue("bytes_sec", new QoreFloatNode(bs), 0);
      h->setKeyValue("threshold", new QoreBigIntNode((int64)tp_warning_bs), 0);
      if (callback_arg)
	 h->setKeyValue("arg", callback_arg->refSelf(), 0);

      warn_queue->pushAndTakeRef(h);
   }

   DLLLOCAL bool pendingHttpChunkedBody() const {
      return http_exp_chunked_body && sock != QORE_INVALID_SOCKET;
   }

   DLLLOCAL static void getUsageInfo(const QoreSocket& sock, QoreHashNode& h, const QoreSocket& s) {
      sock.priv->getUsageInfo(h, *s.priv);
   }

   DLLLOCAL static qore_socket_private* get(QoreSocket& sock) {
      return sock.priv;
   }

   DLLLOCAL static const qore_socket_private* get(const QoreSocket& sock) {
      return sock.priv;
   }
};

#endif
