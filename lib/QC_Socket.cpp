/*
  QC_Socket.cpp

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

#include <qore/Qore.h>
#include <qore/intern/QC_Socket.h>
#include <qore/intern/ssl_constants.h>
#include <qore/intern/QC_Queue.h>

#include <errno.h>
#include <string.h>

qore_classid_t CID_SOCKET;
QoreClass *QC_SOCKET;

static AbstractQoreNode *doReadResult(int rc, int64 val, const char *method_name, int timeout_ms, ExceptionSink *xsink) {
   if (rc > 0)
      return new QoreBigIntNode(val);

   QoreSocket::doException(rc, method_name, timeout_ms, xsink);
   return 0;
}

static AbstractQoreNode *doSendResult(int rc, const char *method_name, ExceptionSink *xsink) {
   if (rc == -2)
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::%s() call", method_name);
   else if (rc)
      xsink->raiseErrnoException("SOCKET-SEND-ERROR", rc, "Socket::%s() failed with error code %d", method_name, rc);
   return 0;
}

static AbstractQoreNode *checkOpenResult(int rc, const char *method_name, ExceptionSink *xsink) {
   if (*xsink)
      return 0;
   if (rc == -2) {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be open before Socket::%s() call", method_name);
      return 0;
   }
   return new QoreBigIntNode(rc);
}

static void SOCKET_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   self->setPrivate(CID_SOCKET, new mySocket);
}

static void SOCKET_copy(QoreObject *self, QoreObject *old, AbstractPrivateData *obj, ExceptionSink *xsink) {
   self->setPrivate(CID_SOCKET, new mySocket);
}

// currently hardcoded to SOCK_STREAM
// opens and connects to a remote socket
// for AF_INET sockets:
// * connect("hostname:<port_number>");
// for AF_UNIX sockets:
// * connect("filename");

// Socket::connect(string $sock, timeout $timeout_ms = -1)
static AbstractQoreNode *SOCKET_connect_str_timeout(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   s->connect(p0->getBuffer(), (int)HARD_QORE_INT(params, 1), xsink);
   return 0;
}

// nothing Socket::connectINET(string $host, softstring $service, timeout $timeout_ms = -1, softint $family = AF_UNSPEC, softint $socktype = SOCK_STREAM, softint $protocol = 0)
static AbstractQoreNode *SOCKET_connectINET(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *host = HARD_QORE_STRING(params, 0);
   const QoreStringNode *service = HARD_QORE_STRING(params, 1);
   int timeout_ms = (int)HARD_QORE_INT(params, 2);
   int family = (int)HARD_QORE_INT(params, 3);
   int socktype = (int)HARD_QORE_INT(params, 4);
   int protocol = (int)HARD_QORE_INT(params, 5);
   s->connectINET2(host->getBuffer(), service->getBuffer(), family, socktype, protocol, timeout_ms, xsink);
   return 0;
}

// nothing Socket::connectUNIX(string $path, softint $socktype = SOCK_STREAM, softint $protocol = 0)
static AbstractQoreNode *SOCKET_connectUNIX(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   int socktype = (int)HARD_QORE_INT(params, 1);
   int protocol = (int)HARD_QORE_INT(params, 2);
   s->connectUNIX(p0->getBuffer(), socktype, protocol, xsink);
   return 0;
}

// currently hardcoded to SOCK_STREAM
// opens and connects to a remote socket and negotiates an SSL connection
// for AF_INET sockets:
// * connectSSL("hostname:<port_number>");
// for AF_UNIX sockets:
// * connectSSL("filename");
// Socket::connectSSL(string $sock, timeout $timeout_ms = -1)
static AbstractQoreNode *SOCKET_connectSSL_str_timeout(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   s->connectSSL(p0->getBuffer(), (int)HARD_QORE_INT(params, 1), xsink);
   return 0;
}

// nothing Socket::connectINETSSL(string $host, softstring $service, timeout $timeout_ms = -1, softint $family = AF_UNSPEC, softint $socktype = SOCK_STREAM, softint $protocol = 0)
static AbstractQoreNode *SOCKET_connectINETSSL(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *host = HARD_QORE_STRING(params, 0);
   const QoreStringNode *service = HARD_QORE_STRING(params, 1);
   int timeout_ms = (int)HARD_QORE_INT(params, 2);
   int family = (int)HARD_QORE_INT(params, 3);
   int socktype = (int)HARD_QORE_INT(params, 4);
   int protocol = (int)HARD_QORE_INT(params, 5);
   s->connectINET2SSL(host->getBuffer(), service->getBuffer(), family, socktype, protocol, timeout_ms, xsink);
   return 0;
}

// nothing Socket::connectUNIXSSL(string $path, softint $socktype = SOCK_STREAM, softint $protocol = 0)  
static AbstractQoreNode *SOCKET_connectUNIXSSL(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   int socktype = (int)HARD_QORE_INT(params, 1);
   int protocol = (int)HARD_QORE_INT(params, 2);
   s->connectUNIXSSL(p0->getBuffer(), socktype, protocol, xsink);
   return 0;
}

// opens and binds to a local socket
// for AF_INET (tcp) sockets:
// * Socket::bind(<port_number>);
// for AF_UNIX (domain, file-based) sockets:
// * Socket::bind("filename");
// for INET (tcp) sockets
// * Socket::bind("iterface:port");

// int Socket::bind(string $str, softbool $reuseaddr = False)  
static AbstractQoreNode *SOCKET_bind_str_bool(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   bool reuseaddr = HARD_QORE_BOOL(params, 1);
   return new QoreBigIntNode(s->bind(p0->getBuffer(), reuseaddr));
}

// int Socket::bind(int $port, softbool $reuseaddr = False)  
static AbstractQoreNode *SOCKET_bind_int_bool(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int port = (int)HARD_QORE_INT(params, 0);
   bool reuseaddr = HARD_QORE_BOOL(params, 1);
   return new QoreBigIntNode(s->bind(port, reuseaddr));
}

// nothing bindUNIX(string $name, softint $socktype = SOCK_STREAM, softint $protocol = 0)
static AbstractQoreNode *SOCKET_bindUNIX(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *name = HARD_QORE_STRING(params, 0);
   int socktype = (int)HARD_QORE_INT(params, 1);
   int prot = (int)HARD_QORE_INT(params, 2);

   s->bindUNIX(name->getBuffer(), socktype, prot, xsink);
   return 0;
}

// nothing bindINET(*string $interface, *softstring $service, softbool $reuseaddr = False, softint $family = AF_UNSPEC, softint $socktype = SOCK_STREAM, softint $protocol = 0)
static AbstractQoreNode *SOCKET_bindINET(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *interface = test_string_param(params, 0);
   const QoreStringNode *service = test_string_param(params, 1);

   if ((!interface || !interface->strlen())
       && (!service || !service->strlen())) {
      xsink->raiseException("SOCKET-BIND-ERROR", "both interace (first parameter) and service (second parameter) were either not present or empty strings; at least one of the first 2 parameters must be present for a successful call to Socket::bindINET()");
      return 0;
   }

   bool reuseaddr = HARD_QORE_BOOL(params, 2);
   int family = (int)HARD_QORE_INT(params, 3);
   int socktype = (int)HARD_QORE_INT(params, 4);
   int prot = (int)HARD_QORE_INT(params, 5);

   s->bindINET(interface ? interface->getBuffer() : 0, service ? service->getBuffer() : 0, reuseaddr, family, socktype, prot, xsink);
   return 0;
}

// Socket Socket::accept()
static AbstractQoreNode *SOCKET_accept(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   mySocket *n = s->accept(-1, xsink);
   if (!n) {
      assert(*xsink);
      return 0;
   }

   assert(n);

   // ensure that a socket object is returned (and not a subclass)
   QoreObject *ns = new QoreObject(QC_SOCKET, getProgram(), n);

   // save backwards-compatible peer parameters as members in new object (deprecated: Socket::getPeerInfo() should be used in the future)
   n->setAccept(ns);

   return ns;
}

// *Socket Socket::accept(timeout timeout_ms)
// returns a new Socket object, connection source address is in $.source, hostname in $.source_host
static AbstractQoreNode *SOCKET_accept_timeout(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int timeout_ms = (int)HARD_QORE_INT(params, 0);

   mySocket *n = s->accept(timeout_ms, xsink);
   if (!n)
      return 0;

   // ensure that a socket object is returned (and not a subclass)
   QoreObject *ns = new QoreObject(QC_SOCKET, getProgram(), n);

   // save backwards-compatible peer parameters as members in new object (deprecated: Socket::getPeerInfo() should be used in the future)
   n->setAccept(ns);

   return ns;
}

// Socket Socket::acceptSSL()
static AbstractQoreNode *SOCKET_acceptSSL(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   mySocket *n = s->acceptSSL(-1, xsink);
   if (!n) {
      assert(*xsink);
      return 0;
   }

   // ensure that a socket object is returned (and not a subclass)
   QoreObject *ns = new QoreObject(QC_SOCKET, getProgram(), n);

   // save backwards-compatible peer parameters as members in new object (deprecated: Socket::getPeerInfo() should be used in the future)
   n->setAccept(ns);
   
   return ns;
}

// *Socket Socket::acceptSSL(timeout timeout_ms)
/* accepts a new connection, negotiates an SSL connection, and returns the new socket
   the connection source string is in the "$.source" member of new object, hostname in "$.source_host"
*/
static AbstractQoreNode *SOCKET_acceptSSL_timeout(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int timeout_ms = (int)HARD_QORE_INT(params, 0);

   mySocket *n = s->acceptSSL(timeout_ms, xsink);
   if (!n)
      return 0;

   // ensure that a socket object is returned (and not a subclass)
   QoreObject *ns = new QoreObject(QC_SOCKET, getProgram(), n);

   // save backwards-compatible peer parameters as members in new object (deprecated: Socket::getPeerInfo() should be used in the future)
   n->setAccept(ns);
   
   return ns;
}

static AbstractQoreNode *SOCKET_listen(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return checkOpenResult(s->listen(), "listen", xsink);
}

// int Socket::send(binary $bin)  
static AbstractQoreNode *SOCKET_send_bin(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return checkOpenResult(s->send(HARD_QORE_BINARY(params, 0)), "send", xsink);
}

// int Socket::send(string $str)  
static AbstractQoreNode *SOCKET_send_str(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return checkOpenResult(s->send(HARD_QORE_STRING(params, 0), xsink), "send", xsink);
}

// int Socket::sendBinary(string $str)  
static AbstractQoreNode *SOCKET_sendBinary_str(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *str = HARD_QORE_STRING(params, 0);
   // send without any conversions
   return checkOpenResult(s->send(str->getBuffer(), str->strlen()), "send", xsink);
}

static AbstractQoreNode *SOCKET_sendi1(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   char i = (char)HARD_QORE_INT(params, 0);
   return checkOpenResult(s->sendi1(i), "sendi1", xsink);
}

static AbstractQoreNode *SOCKET_sendi2(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   short i = (short)HARD_QORE_INT(params, 0);
   return checkOpenResult(s->sendi2(i), "sendi2", xsink);
}

static AbstractQoreNode *SOCKET_sendi4(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int i = (int)HARD_QORE_INT(params, 0);
   return checkOpenResult(s->sendi4(i), "sendi4", xsink);
}

static AbstractQoreNode *SOCKET_sendi8(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int64 i = HARD_QORE_INT(params, 0);
   return checkOpenResult(s->sendi8(i), "sendi8", xsink);
}

static AbstractQoreNode *SOCKET_sendi2LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   short i = (short)HARD_QORE_INT(params, 0);
   return checkOpenResult(s->sendi2LSB(i), "sendi2LSB", xsink);
}

static AbstractQoreNode *SOCKET_sendi4LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int i = (int)HARD_QORE_INT(params, 0);
   return checkOpenResult(s->sendi4LSB(i), "sendi4LSB", xsink);
}

static AbstractQoreNode *SOCKET_sendi8LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int64 i = HARD_QORE_INT(params, 0);
   return checkOpenResult(s->sendi8LSB(i), "sendi8LSB", xsink);
}

// string Socket::recv(int $size = 0, timeout $timeout_ms -1)  
static AbstractQoreNode *SOCKET_recv(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int bs = (int)HARD_QORE_INT(params, 0);
   
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 1);
   int rc;
   QoreStringNodeHolder msg(bs > 0 ? s->recv(bs, timeout, &rc) : s->recv(timeout, &rc));

   //printd(5, "SOCKET_recv() rc=%d msglen=%d\n", rc, msg ? msg->strlen() : -1);

   if (rc > 0)
      return msg.release();

   QoreSocket::doException(rc, "recv", timeout, xsink);
   return 0;
}

// binary Socket::recvBinary(softint $size = 0, timeout $timeout = -1)  
static AbstractQoreNode *SOCKET_recvBinary(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int bs = (int)HARD_QORE_INT(params, 0);

   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 1);
   int rc;
   SimpleRefHolder<BinaryNode> b(bs > 0 ? s->recvBinary(bs, timeout, &rc) : s->recvBinary(timeout, &rc));

   if (rc > 0)
      return b.release();

   QoreSocket::doException(rc, "recvBinary", timeout, xsink);
   return 0;
}

// Socket::recvi1(timeout $timeout = -1)
static AbstractQoreNode *SOCKET_recvi1(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 0);
   char b;
   int rc = s->recvi1(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi1", timeout, xsink);
}

static AbstractQoreNode *SOCKET_recvi2(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 0);
   short b;
   int rc = s->recvi2(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi2", timeout, xsink);
}

static AbstractQoreNode *SOCKET_recvi4(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 0);
   int b;
   int rc = s->recvi4(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi4", timeout, xsink);
}

static AbstractQoreNode *SOCKET_recvi8(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 0);
   int64 b;
   int rc = s->recvi8(timeout, &b);
   return doReadResult(rc, b, "recvi8", timeout, xsink);
}

static AbstractQoreNode *SOCKET_recvi2LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 0);
   short b;
   int rc = s->recvi2LSB(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi2LSB", timeout, xsink);
}

static AbstractQoreNode *SOCKET_recvi4LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 0);
   int b;
   int rc = s->recvi4LSB(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi4LSB", timeout, xsink);
}

static AbstractQoreNode *SOCKET_recvi8LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 0);
   int64 b;
   int rc = s->recvi8LSB(timeout, &b);
   return doReadResult(rc, b, "recvi8LSB", timeout, xsink);
}

static AbstractQoreNode *SOCKET_recvu1(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 0);   
   unsigned char b;
   int rc = s->recvu1(timeout, &b);
   return doReadResult(rc, (int64)b, "recvu1", timeout, xsink);
}

static AbstractQoreNode *SOCKET_recvu2(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 0);
   unsigned short b;
   int rc = s->recvu2(timeout, &b);
   return doReadResult(rc, (int64)b, "recvu2", timeout, xsink);
}

static AbstractQoreNode *SOCKET_recvu4(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 0);   
   unsigned int b;
   int rc = s->recvu4(timeout, &b);
   return doReadResult(rc, (int64)b, "recvu4", timeout, xsink);
}

static AbstractQoreNode *SOCKET_recvu2LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 0);
   unsigned short b;
   int rc = s->recvu2LSB(timeout, &b);
   return doReadResult(rc, (int64)b, "recvu2LSB", timeout, xsink);
}

static AbstractQoreNode *SOCKET_recvu4LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = (int)HARD_QORE_INT(params, 0);
   
   unsigned b;
   int rc = s->recvu4LSB(timeout, &b);
   return doReadResult(rc, (int64)b, "recvu4LSB", timeout, xsink);
}

// int Socket::sendHTTPMessage(string $method, string $path, string $http_version, hash $headers, data $data = binary())  
static AbstractQoreNode *SOCKET_sendHTTPMessage(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const char *method = HARD_QORE_STRING(params, 0)->getBuffer();
   const char *path = HARD_QORE_STRING(params, 1)->getBuffer();
   const char *http_version = HARD_QORE_STRING(params, 2)->getBuffer();
   const QoreHashNode *headers = HARD_QORE_HASH(params, 3);

   // see if there is data to send as well
   const AbstractQoreNode *p4 = get_param(params, 4);
   const void *ptr = 0;
   int size = 0;

   if (p4->getType() == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p4);
      ptr = str->getBuffer();
      size = str->strlen();
   }
   else {
      assert(p4->getType() == NT_BINARY);
      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(p4);
      ptr = b->getPtr();
      size = b->size();
   }

   int rc = s->sendHTTPMessage(method, path, http_version, headers, ptr, size);
   return doSendResult(rc, "sendHTTPMessage", xsink);
}

// nothing Socket::sendHTTPResponse(softint $status_code, string $desc, string $http_version, hash $headers, data $data = binary())  
static AbstractQoreNode *SOCKET_sendHTTPResponse(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int status_code = (int)HARD_QORE_INT(params, 0);

   if (status_code < 100 || status_code >= 600) {
      xsink->raiseException("SOCKET-SENDHTTPRESPONSE-PARAMETER-ERROR", "expecting valid HTTP status code between 100 and 599 as first parameter of Socket::sendHTTPResponse() call, got value %d instead", status_code);
      return 0;
   }

   const char *status_desc = HARD_QORE_STRING(params, 1)->getBuffer();
   const char *http_version = HARD_QORE_STRING(params, 2)->getBuffer();
   const QoreHashNode *headers = HARD_QORE_HASH(params, 3);

   // see if there is data to send as well
   const AbstractQoreNode *p4 = get_param(params, 4);
   const void *ptr = 0;
   int size = 0;

   if (p4->getType() == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p4);
      ptr = str->getBuffer();
      size = str->strlen();
   }
   else {
      assert(p4->getType() == NT_BINARY);
      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(p4);
      ptr = b->getPtr();
      size = b->size();
   }

   int rc = s->sendHTTPResponse(status_code, status_desc, http_version, headers, ptr, size);
   return doSendResult(rc, "sendHTTPResponse", xsink);
}

// string Socket::readHTTPHeader(timeout $timeout_ms = -1)  |hash
static AbstractQoreNode *SOCKET_readHTTPHeader(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int timeout = (int)HARD_QORE_INT(params, 0);
   int rc;

   // when rc = -3 it's a timeout, but rv will be NULL anyway, so we do nothing
   AbstractQoreNode *rv = s->readHTTPHeader(timeout, &rc);
      
   if (rc <= 0)
      QoreSocket::doException(rc, "readHTTPHeader", timeout, xsink);

   return rv;
}

// hash Socket::readHTTPChunkedBody(timeout $timeout_ms = -1)  
static AbstractQoreNode *SOCKET_readHTTPChunkedBody(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int timeout = (int)HARD_QORE_INT(params, 0);

   // when rc = -3 it's a timeout, but rv will be NULL anyway, so we do nothing
   return s->readHTTPChunkedBody(timeout, xsink);
}

// hash Socket::readHTTPChunkedBodyBinary(timeout $timeout_ms = -1)  
static AbstractQoreNode *SOCKET_readHTTPChunkedBodyBinary(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int timeout = (int)HARD_QORE_INT(params, 0);

   // when rc = -3 it's a timeout, but rv will be NULL anyway, so we do nothing
   return s->readHTTPChunkedBodyBinary(timeout, xsink);
}

static AbstractQoreNode *SOCKET_close(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->close());
}

static AbstractQoreNode *SOCKET_shutdown(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->shutdown());
}

static AbstractQoreNode *SOCKET_shutdownSSL(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   s->shutdownSSL(xsink);
   return 0;
}

static AbstractQoreNode *SOCKET_getPort(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->getPort());
}

static AbstractQoreNode *SOCKET_getSocket(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->getSocket());
}

// int Socket::setSendTimeout(timeout $timeout_ms)  
static AbstractQoreNode *SOCKET_setSendTimeout_int(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int timeout_ms = (int)HARD_QORE_INT(params, 0);
   return new QoreBigIntNode(s->setSendTimeout(timeout_ms));
}

// int Socket::setRecvTimeout(timeout $timeout_ms)  
static AbstractQoreNode *SOCKET_setRecvTimeout_int(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   int timeout_ms = (int)HARD_QORE_INT(params, 0);
   return new QoreBigIntNode(s->setRecvTimeout(timeout_ms));
}

static AbstractQoreNode *SOCKET_getSendTimeout(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->getSendTimeout());
}

static AbstractQoreNode *SOCKET_getRecvTimeout(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->getRecvTimeout());
}

static AbstractQoreNode *SOCKET_getCharset(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreStringNode(s->getEncoding()->getCode());
}

static AbstractQoreNode *SOCKET_setCharset(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   s->setEncoding(QEM.findCreate(p0));
   return 0; 
}

// Socket::isDataAvailable(timeout $timeout_ms = 0)
static AbstractQoreNode *SOCKET_isDataAvailable(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(s->isDataAvailable((int)HARD_QORE_INT(params, 0)));
}

static AbstractQoreNode *SOCKET_isWriteFinished(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(s->isWriteFinished((int)HARD_QORE_INT(params, 0)));
}

static AbstractQoreNode *SOCKET_isOpen(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(s->isOpen());
}

// *string Socket::getSSLCipherName()  
static AbstractQoreNode *SOCKET_getSSLCipherName(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const char *str = s->getSSLCipherName();
   return str ? new QoreStringNode(str) : 0;
}

// *string Socket::getSSLCipherVersion()  
static AbstractQoreNode *SOCKET_getSSLCipherVersion(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const char *str = s->getSSLCipherVersion();
   return str ? new QoreStringNode(str) : 0;
}

static AbstractQoreNode *SOCKET_isSecure(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(s->isSecure());
}

// *string Socket::verifyPeerCertificate()  
static AbstractQoreNode *SOCKET_verifyPeerCertificate(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const char *c = getSSLCVCode(s->verifyPeerCertificate());
   return c ? new QoreStringNode(c) : 0;
}

// nothing Socket::setCertificate(SSLCertificate $cert)  
static AbstractQoreNode *SOCKET_setCertificate_cert(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(cert, QoreSSLCertificate, params, 0, CID_SSLCERTIFICATE, "Socket::setCertificate()", "SSLCertificate", xsink);
   if (*xsink)
      return 0;

   // pass reference from QoreObject::getReferencedPrivateData() to mySocket::setCertificate()
   s->setCertificate(cert);
   return 0;
}

// nothing Socket::setCertificate(string $cert_pem)  
static AbstractQoreNode *SOCKET_setCertificate_string(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   SimpleRefHolder<QoreSSLCertificate> cert(new QoreSSLCertificate(p0, xsink));
   if (*xsink)
      return 0;

   // mySocket::setCertificate() takes over ownership of certificate reference
   s->setCertificate(cert.release());
   return 0;   
}

// nothing Socket::setCertificate(binary $cert_der)  
static AbstractQoreNode *SOCKET_setCertificate_bin(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const BinaryNode *p0 = HARD_QORE_BINARY(params, 0);
   SimpleRefHolder<QoreSSLCertificate> cert(new QoreSSLCertificate(p0, xsink));
   if (*xsink)
      return 0;

   // mySocket::setCertificate() takes over ownership of certificate reference
   s->setCertificate(cert.release());
   return 0;   
}

// Socket::setPrivateKey(SSLPrivateKey $key)
static AbstractQoreNode *SOCKET_setPrivateKey_key(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(key, QoreSSLPrivateKey, params, 0, CID_SSLPRIVATEKEY, "Socket::setPrivateKey()", "SSLPrivateKey", xsink);
   if (*xsink)
      return 0;

   // pass reference from QoreObject::getReferencedPrivateData() to mySocket::setPrivateKey()
   s->setPrivateKey(key);
   return 0;
}

// Socket::setPrivateKey(string $key_pem)
static AbstractQoreNode *SOCKET_setPrivateKey_str(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   SimpleRefHolder<QoreSSLPrivateKey> key(new QoreSSLPrivateKey(p0, 0, xsink));
   if (*xsink)
      return 0;

   s->setPrivateKey(key.release());
   return 0;
}

// Socket::setPrivateKey(string $key_pem, string $pass)
static AbstractQoreNode *SOCKET_setPrivateKey_str_str(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const QoreStringNode *p1 = HARD_QORE_STRING(params, 1);
   SimpleRefHolder<QoreSSLPrivateKey> key(new QoreSSLPrivateKey(p0, p1->getBuffer(), xsink));
   if (*xsink)
      return 0;

   s->setPrivateKey(key.release());
   return 0;
}

// Socket::setPrivateKey(binary $key_der)
static AbstractQoreNode *SOCKET_setPrivateKey_bin(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const BinaryNode *p0 = HARD_QORE_BINARY(params, 0);
   SimpleRefHolder<QoreSSLPrivateKey> key(new QoreSSLPrivateKey(p0, xsink));
   if (*xsink)
      return 0;

   s->setPrivateKey(key.release());
   return 0;
}

static AbstractQoreNode *SOCKET_setEventQueue_nothing(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   s->setEventQueue(0, xsink);
   return 0;
}

static AbstractQoreNode *SOCKET_setEventQueue_queue(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(q, Queue, params, 0, CID_QUEUE, "Queue", "Socket::setEventQueue", xsink);
   if (*xsink)
      return 0;
   // pass reference from QoreObject::getReferencedPrivateData() to function
   s->setEventQueue(q, xsink);
   return 0;
}

static AbstractQoreNode *SOCKET_setNoDelay(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->setNoDelay(HARD_QORE_BOOL(params, 0)));
}

static AbstractQoreNode *SOCKET_getNoDelay(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(s->getNoDelay());
}

static AbstractQoreNode *SOCKET_getPeerInfo(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return s->getPeerInfo(xsink);
}

static AbstractQoreNode *SOCKET_getSocketInfo(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return s->getSocketInfo(xsink);
}

QoreClass *initSocketClass(QoreClass *SSLCert, QoreClass *SSLPrivKey) {
   QORE_TRACE("initSocketClass()");

   assert(QC_QUEUE);

   QC_SOCKET = new QoreClass("Socket", QDOM_NETWORK);
   CID_SOCKET = QC_SOCKET->getID();

   // register public members
   QC_SOCKET->addPublicMember("source", stringOrNothingTypeInfo);
   QC_SOCKET->addPublicMember("source_host", stringOrNothingTypeInfo);

   // unset the public member flag for backwards-compatibility
   // this allows older code where a user class inherits this class to still be able
   // to be run in new Qore >= 0.8
   QC_SOCKET->unsetPublicMemberFlag();
   
   QC_SOCKET->setConstructorExtended(SOCKET_constructor);

   QC_SOCKET->setCopy(SOCKET_copy);

   // Socket::connect(string $sock, timeout $timeout_ms = -1)
   QC_SOCKET->addMethodExtended("connect",                   (q_method_t)SOCKET_connect_str_timeout, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, timeoutTypeInfo, new QoreBigIntNode(-1));

   // nothing Socket::connectINET(string $host, softstring $service, timeout $timeout_ms = -1, softint $family = AF_UNSPEC, softint $socktype = SOCK_STREAM, softint $protocol = 0) 
   QC_SOCKET->addMethodExtended("connectINET",               (q_method_t)SOCKET_connectINET, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 6, stringTypeInfo, QORE_PARAM_NO_ARG, softStringTypeInfo, QORE_PARAM_NO_ARG, timeoutTypeInfo, new QoreBigIntNode(-1), softBigIntTypeInfo, new QoreBigIntNode(AF_UNSPEC), softBigIntTypeInfo, new QoreBigIntNode(SOCK_STREAM), softBigIntTypeInfo, zero());

   // nothing Socket::connectUNIX(string $path, softint $socktype = SOCK_STREAM, softint $protocol = 0)
   QC_SOCKET->addMethodExtended("connectUNIX",               (q_method_t)SOCKET_connectUNIX, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 3, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(SOCK_STREAM), softBigIntTypeInfo, zero());

   // Socket::connectSSL(string $sock, timeout $timeout_ms = -1)
   QC_SOCKET->addMethodExtended("connectSSL",                (q_method_t)SOCKET_connectSSL_str_timeout, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, timeoutTypeInfo, new QoreBigIntNode(-1));

   // nothing Socket::connectINETSSL(string $host, softstring $service, timeout $timeout_ms = -1, softint $family = AF_UNSPEC, softint $socktype = SOCK_STREAM, softint $protocol = 0)
   QC_SOCKET->addMethodExtended("connectINETSSL",            (q_method_t)SOCKET_connectINETSSL, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 5, stringTypeInfo, QORE_PARAM_NO_ARG, softStringTypeInfo, QORE_PARAM_NO_ARG, timeoutTypeInfo, new QoreBigIntNode(-1), softBigIntTypeInfo, new QoreBigIntNode(AF_UNSPEC), softBigIntTypeInfo, new QoreBigIntNode(SOCK_STREAM), softBigIntTypeInfo, zero());

   // nothing Socket::connectUNIXSSL(string $path, softint $socktype = SOCK_STREAM, softint $protocol = 0)  
   QC_SOCKET->addMethodExtended("connectUNIXSSL",            (q_method_t)SOCKET_connectUNIXSSL, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 3, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(SOCK_STREAM), softBigIntTypeInfo, zero());

   // int Socket::bind(string $str, softbool $reuseaddr = False)  
   QC_SOCKET->addMethodExtended("bind",                      (q_method_t)SOCKET_bind_str_bool, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBoolTypeInfo, &False);

   // int Socket::bind(int $port, softbool $reuseaddr = False)  
   QC_SOCKET->addMethodExtended("bind",                      (q_method_t)SOCKET_bind_int_bool, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 2, bigIntTypeInfo, QORE_PARAM_NO_ARG, softBoolTypeInfo, &False);

   // nothing bindUNIX(string $name, softint $socktype = SOCK_STREAM, softint $protocol = 0)
   QC_SOCKET->addMethodExtended("bindUNIX",                  (q_method_t)SOCKET_bindUNIX, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 3, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(SOCK_STREAM), bigIntTypeInfo, zero());

   // nothing bindINET(*string $interface, *softstring $service, softbool $reuseaddr = False, softint $family = AF_UNSPEC, softint $socktype = SOCK_STREAM, int $protocol = 0)
   QC_SOCKET->addMethodExtended("bindINET",                  (q_method_t)SOCKET_bindINET, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 6, stringOrNothingTypeInfo, QORE_PARAM_NO_ARG, softStringOrNothingTypeInfo, QORE_PARAM_NO_ARG, softBoolTypeInfo, &False, softBigIntTypeInfo, new QoreBigIntNode(AF_UNSPEC), softBigIntTypeInfo, new QoreBigIntNode(SOCK_STREAM), bigIntTypeInfo, zero());

   // *Socket Socket::accept(timeout timeout_ms)
   QC_SOCKET->addMethodExtended("accept",                    (q_method_t)SOCKET_accept_timeout, false, QC_NO_FLAGS, QDOM_DEFAULT, QC_SOCKET->getOrNothingTypeInfo(), 1, timeoutTypeInfo, QORE_PARAM_NO_ARG);
   // Socket Socket::accept()
   QC_SOCKET->addMethodExtended("accept",                    (q_method_t)SOCKET_accept, false, QC_NO_FLAGS, QDOM_DEFAULT, QC_SOCKET->getTypeInfo());

   // *Socket Socket::acceptSSL(timeout timeout_ms)
   QC_SOCKET->addMethodExtended("acceptSSL",                 (q_method_t)SOCKET_acceptSSL_timeout, false, QC_NO_FLAGS, QDOM_DEFAULT, QC_SOCKET->getOrNothingTypeInfo(), 1, timeoutTypeInfo, QORE_PARAM_NO_ARG);
   // Socket Socket::acceptSSL()
   QC_SOCKET->addMethodExtended("acceptSSL",                 (q_method_t)SOCKET_acceptSSL, false, QC_NO_FLAGS, QDOM_DEFAULT, QC_SOCKET->getTypeInfo());

   QC_SOCKET->addMethodExtended("listen",                    (q_method_t)SOCKET_listen, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   // int Socket::send(binary $bin)  
   QC_SOCKET->addMethodExtended("send",                      (q_method_t)SOCKET_send_bin, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);

   // int Socket::send(string $str)  
   QC_SOCKET->addMethodExtended("send",                      (q_method_t)SOCKET_send_str, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // int Socket::sendBinary(binary $bin)  
   QC_SOCKET->addMethodExtended("sendBinary",                (q_method_t)SOCKET_send_bin, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);

   // int Socket::sendBinary(string $str)  
   QC_SOCKET->addMethodExtended("sendBinary",                (q_method_t)SOCKET_sendBinary_str, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_SOCKET->addMethodExtended("sendi1",                    (q_method_t)SOCKET_sendi1, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());
   QC_SOCKET->addMethodExtended("sendi2",                    (q_method_t)SOCKET_sendi2, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());
   QC_SOCKET->addMethodExtended("sendi4",                    (q_method_t)SOCKET_sendi4, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());
   QC_SOCKET->addMethodExtended("sendi8",                    (q_method_t)SOCKET_sendi8, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());
   QC_SOCKET->addMethodExtended("sendi2LSB",                 (q_method_t)SOCKET_sendi2LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());
   QC_SOCKET->addMethodExtended("sendi4LSB",                 (q_method_t)SOCKET_sendi4LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());
   QC_SOCKET->addMethodExtended("sendi8LSB",                 (q_method_t)SOCKET_sendi8LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());

   // string Socket::recv(softint $size = 0, timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recv",                      (q_method_t)SOCKET_recv, false, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 2, softBigIntTypeInfo, zero(), timeoutTypeInfo, new QoreBigIntNode(-1));

   // binary Socket::recvBinary(softint $size = 0, timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvBinary",                (q_method_t)SOCKET_recvBinary, false, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 2, softBigIntTypeInfo, zero(), timeoutTypeInfo, new QoreBigIntNode(-1));

   // int Socket::recvi1(timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvi1",                    (q_method_t)SOCKET_recvi1, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // int Socket::recvi2(timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvi2",                    (q_method_t)SOCKET_recvi2, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // int Socket::recvi4(timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvi4",                    (q_method_t)SOCKET_recvi4, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // int Socket::recvi8(timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvi8",                    (q_method_t)SOCKET_recvi8, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // int Socket::recvi2LSB(timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvi2LSB",                 (q_method_t)SOCKET_recvi2LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // int Socket::recvi4LSB(timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvi4LSB",                 (q_method_t)SOCKET_recvi4LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // int Socket::recvi8LSB(timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvi8LSB",                 (q_method_t)SOCKET_recvi8LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // int Socket::recvu1(timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvu1",                    (q_method_t)SOCKET_recvu1, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // int Socket::recvu2(timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvu2",                    (q_method_t)SOCKET_recvu2, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // int Socket::recvu4(timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvu4",                    (q_method_t)SOCKET_recvu4, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // int Socket::recvu2LSB(timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvu2LSB",                 (q_method_t)SOCKET_recvu2LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // int Socket::recvu4LSB(timeout $timeout = -1)  
   QC_SOCKET->addMethodExtended("recvu4LSB",                 (q_method_t)SOCKET_recvu4LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // nothing Socket::sendHTTPMessage(string $method, string $path, string $http_version, hash $headers, data $data = binary())  
   QC_SOCKET->addMethodExtended("sendHTTPMessage",           (q_method_t)SOCKET_sendHTTPMessage, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 5, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, hashTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, new BinaryNode);

   // nothing Socket::sendHTTPResponse(softint $status_code, string $desc, string $http_version, hash $headers, data $data = binary())  
   QC_SOCKET->addMethodExtended("sendHTTPResponse",          (q_method_t)SOCKET_sendHTTPResponse, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 5, softBigIntTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, hashTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, new BinaryNode);

   // string Socket::readHTTPHeader(timeout $timeout_ms = -1)  |hash
   QC_SOCKET->addMethodExtended("readHTTPHeader",            (q_method_t)SOCKET_readHTTPHeader, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // hash Socket::readHTTPChunkedBody(timeout $timeout_ms = -1)  
   QC_SOCKET->addMethodExtended("readHTTPChunkedBody",       (q_method_t)SOCKET_readHTTPChunkedBody, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   // hash Socket::readHTTPChunkedBodyBinary(timeout $timeout_ms = -1)  
   QC_SOCKET->addMethodExtended("readHTTPChunkedBodyBinary", (q_method_t)SOCKET_readHTTPChunkedBodyBinary, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, timeoutTypeInfo, new QoreBigIntNode(-1));

   QC_SOCKET->addMethodExtended("close",                     (q_method_t)SOCKET_close, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   QC_SOCKET->addMethodExtended("shutdown",                  (q_method_t)SOCKET_shutdown, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   QC_SOCKET->addMethodExtended("shutdownSSL",               (q_method_t)SOCKET_shutdownSSL, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   QC_SOCKET->addMethodExtended("getPort",                   (q_method_t)SOCKET_getPort, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   QC_SOCKET->addMethodExtended("getSocket",                 (q_method_t)SOCKET_getSocket, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   // int Socket::setSendTimeout(timeout $timeout_ms)  
   QC_SOCKET->addMethodExtended("setSendTimeout",            (q_method_t)SOCKET_setSendTimeout_int, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, QORE_PARAM_NO_ARG);

   // int Socket::setRecvTimeout(timeout $timeout_ms)  
   QC_SOCKET->addMethodExtended("setRecvTimeout",            (q_method_t)SOCKET_setRecvTimeout_int, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, QORE_PARAM_NO_ARG);

   QC_SOCKET->addMethodExtended("getSendTimeout",            (q_method_t)SOCKET_getSendTimeout, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   QC_SOCKET->addMethodExtended("getRecvTimeout",            (q_method_t)SOCKET_getRecvTimeout, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   QC_SOCKET->addMethodExtended("getCharset",                (q_method_t)SOCKET_getCharset, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);

   QC_SOCKET->addMethodExtended("setCharset",                (q_method_t)SOCKET_setCharset, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // Socket::isDataAvailable(timeout $timeout_ms = 0)
   QC_SOCKET->addMethodExtended("isDataAvailable",           (q_method_t)SOCKET_isDataAvailable, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 1, timeoutTypeInfo, zero());

   // Socket::isWriteFinished(timeout $timeout_ms = 0)
   QC_SOCKET->addMethodExtended("isWriteFinished",           (q_method_t)SOCKET_isWriteFinished, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 1, timeoutTypeInfo, zero());

   // *string Socket::getSSLCipherName()  
   QC_SOCKET->addMethodExtended("getSSLCipherName",          (q_method_t)SOCKET_getSSLCipherName, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *string Socket::getSSLCipherVersion()  
   QC_SOCKET->addMethodExtended("getSSLCipherVersion",       (q_method_t)SOCKET_getSSLCipherVersion, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   QC_SOCKET->addMethodExtended("isSecure",                  (q_method_t)SOCKET_isSecure, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   // *string Socket::verifyPeerCertificate()  
   QC_SOCKET->addMethodExtended("verifyPeerCertificate",     (q_method_t)SOCKET_verifyPeerCertificate, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // nothing Socket::setCertificate(SSLCertificate $cert)  
   QC_SOCKET->addMethodExtended("setCertificate",            (q_method_t)SOCKET_setCertificate_cert, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, SSLCert->getTypeInfo(), QORE_PARAM_NO_ARG);
   // nothing Socket::setCertificate(string $cert_pem)  
   QC_SOCKET->addMethodExtended("setCertificate",            (q_method_t)SOCKET_setCertificate_string, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   // nothing Socket::setCertificate(binary $cert_der)  
   QC_SOCKET->addMethodExtended("setCertificate",            (q_method_t)SOCKET_setCertificate_bin, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);

   // Socket::setPrivateKey(SSLPrivateKey $key)
   QC_SOCKET->addMethodExtended("setPrivateKey",             (q_method_t)SOCKET_setPrivateKey_key, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, SSLPrivKey->getTypeInfo(), QORE_PARAM_NO_ARG);
   // Socket::setPrivateKey(string $key_pem)
   QC_SOCKET->addMethodExtended("setPrivateKey",             (q_method_t)SOCKET_setPrivateKey_str, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   // Socket::setPrivateKey(string $key_pem, string $pass)
   QC_SOCKET->addMethodExtended("setPrivateKey",             (q_method_t)SOCKET_setPrivateKey_str_str, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   // Socket::setPrivateKey(binary $key_der)
   QC_SOCKET->addMethodExtended("setPrivateKey",             (q_method_t)SOCKET_setPrivateKey_bin, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);

   QC_SOCKET->addMethodExtended("isOpen",                    (q_method_t)SOCKET_isOpen, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   // nothing Socket::setEventQueue()  
   QC_SOCKET->addMethodExtended("setEventQueue",             (q_method_t)SOCKET_setEventQueue_nothing, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // nothing Socket::setEventQueue(Queue $queue)  
   QC_SOCKET->addMethodExtended("setEventQueue",             (q_method_t)SOCKET_setEventQueue_queue, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, QC_QUEUE->getTypeInfo(), QORE_PARAM_NO_ARG);

   QC_SOCKET->addMethodExtended("setNoDelay",                (q_method_t)SOCKET_setNoDelay, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, boolTypeInfo, &True);

   QC_SOCKET->addMethodExtended("getNoDelay",                (q_method_t)SOCKET_getNoDelay, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   // hash Socket::getPeerInfo()
   QC_SOCKET->addMethodExtended("getPeerInfo",               (q_method_t)SOCKET_getPeerInfo, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo);

   // hash Socket::getSocketInfo()
   QC_SOCKET->addMethodExtended("getSocketInfo",             (q_method_t)SOCKET_getSocketInfo, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo);

   return QC_SOCKET;
}
