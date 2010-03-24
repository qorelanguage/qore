/*
  QC_Socket.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

static inline AbstractQoreNode *doReadResult(int rc, int64 val, const char *method_name, ExceptionSink *xsink) {
   if (rc > 0)
      return new QoreBigIntNode(val);

   QoreSocket::doException(rc, method_name, xsink);
   return 0;
}

static void SOCKET_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   self->setPrivate(CID_SOCKET, new mySocket());
}

static void SOCKET_copy(QoreObject *self, QoreObject *old, AbstractPrivateData *obj, ExceptionSink *xsink) {
   self->setPrivate(CID_SOCKET, new mySocket());
}

// currently hardcoded to SOCK_STREAM
// opens and connects to a remote socket
// for AF_INET sockets:
// * connect("hostname:<port_number>");
// for AF_UNIX sockets:
// * connect("filename");
static AbstractQoreNode *SOCKET_connect(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   // if parameters are not correct
   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException("SOCKET-CONNECT-PARAMETER-ERROR",
			    "expecting string parameter (INET: 'hostname:port', UNIX: 'path/filename') for Socket::connect() call");
      return 0;
   }

   s->connect(p0->getBuffer(), getMsMinusOneInt(get_param(params, 1)), xsink);
   return 0;
}

// currently hardcoded to SOCK_STREAM
// opens and connects to a remote socket and negotiates an SSL connection
// for AF_INET sockets:
// * connectSSL("hostname:<port_number>");
// for AF_UNIX sockets:
// * connectSSL("filename");
static AbstractQoreNode *SOCKET_connectSSL(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   // if parameters are not correct
   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException("SOCKET-CONNECTSSL-PARAMETER-ERROR",
			    "expecting string parameter (INET: 'hostname:port', UNIX: 'path/filename') for Socket::connectSSL() call");
      return 0;
   }

   s->connectSSL(p0->getBuffer(), getMsMinusOneInt(get_param(params, 1)), xsink);
   return 0;
}

// currently hardcoded to SOCK_STREAM
// opens and binds to a local socket
// for AF_INET (tcp) sockets:
// * Socket::bind(<port_number>);
// for AF_UNIX (domain, file-based) sockets:
// * Socket::bind("filename");
// for INET (tcp) sockets
// * Socket::bind("iterface:port");
static AbstractQoreNode *SOCKET_bind(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0, *p1;
   // if parameters are not correct
   p0 = get_param(params, 0);
   qore_type_t p0_type = p0 ? p0->getType() : 0;
   if (!p0 || (p0_type != NT_STRING && p0_type != NT_INT))
   {
      xsink->raiseException("SOCKET-BIND-PARAMETER-ERROR", "no parameter passed to Socket::bind() call, expecing string for UNIX socket ('path/file') or int for INET socket (port number)");
      return 0;
   }
   bool rua;
   if ((p1 = get_param(params, 1)))
      rua = p1->getAsBool();
   else
      rua = false;

   // create and bind tcp socket to all interfaces on port given
   if (p0_type == NT_INT)
      return new QoreBigIntNode(s->bind((reinterpret_cast<const QoreBigIntNode *>(p0))->val, rua));
   else
      return new QoreBigIntNode(s->bind((reinterpret_cast<const QoreStringNode *>(p0))->getBuffer(), rua));
}

// Socket::accept()
// returns a new Socket object, connection source address is in $.source
// member of new object, hostname in $.source_host
static AbstractQoreNode *SOCKET_accept(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   SocketSource source;
   mySocket *n = s->accept(&source, xsink);
   if (xsink->isEvent())
      return 0;

   // ensure that a socket object is returned (and not a subclass)
   QoreObject *ns = new QoreObject(self->getClass(CID_SOCKET), getProgram());
   ns->setPrivate(CID_SOCKET, n);
   source.setAll(ns, xsink);
      
   return ns;
}

// Socket::acceptSSL()
// accepts a new connection, negotiates an SSL connection, and returns the new socket
// the connection source string is in the "$.source" member of new object,
// hostname in "$.source_host"
static AbstractQoreNode *SOCKET_acceptSSL(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   class SocketSource source;
   mySocket *n = s->acceptSSL(&source, xsink);
   if (xsink->isEvent())
      return 0;

   // ensure that a socket object is returned (and not a subclass)
   QoreObject *ns = new QoreObject(self->getClass(CID_SOCKET), getProgram());
   ns->setPrivate(CID_SOCKET, n);
   source.setAll(ns, xsink);
   
   return ns;
}

static AbstractQoreNode *SOCKET_listen(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   int rc = s->listen();

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::listen() call");
      return 0;
   }

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *SOCKET_send(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->getType() != NT_STRING && p0->getType() != NT_BINARY)) {
      xsink->raiseException("SOCKET-SEND-PARAMETER-ERROR", "expecting string or binary data as first parameter of Socket::send() call");
      return 0;
   }

   int rc;

   if (p0->getType() == NT_STRING)
      rc = s->send((reinterpret_cast<const QoreStringNode *>(p0)), xsink);
   else
      rc = s->send(reinterpret_cast<const BinaryNode *>(p0));
      
   if (rc == -2) {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::send() call");
      return 0;
   }

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *SOCKET_sendBinary(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->getType() != NT_STRING && p0->getType() != NT_BINARY))
   {
      xsink->raiseException("SOCKET-SEND-BINARY-PARAMETER-ERROR", "expecting string or binary data as first parameter of Socket::sendBinary() call");
      return 0;
   }

   int rc = 0;

   // send strings with no conversions
   if (p0->getType() == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p0);
      rc = s->send(str->getBuffer(), str->strlen());
   }
   else
      rc = s->send(reinterpret_cast<const BinaryNode *>(p0));

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendBinary() call");
      return 0;
   }

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *SOCKET_sendi1(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   char i;
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   int rc = s->sendi1(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi1() call");
      return 0;
   }

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *SOCKET_sendi2(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   short i;
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   int rc = s->sendi2(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi2() call");
      return  NULL;
   }

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *SOCKET_sendi4(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   int i;
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   int rc = s->sendi4(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi4() call");
      return 0;
   }

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *SOCKET_sendi8(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   int64 i;
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsBigInt();
   else
      i = 0;

   int rc = s->sendi8(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi8() call");
      return 0;
   }

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *SOCKET_sendi2LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   short i;
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   int rc = s->sendi2LSB(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi2LSB() call");
      return 0;
   }

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *SOCKET_sendi4LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   int i;
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   int rc = s->sendi4LSB(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi4LSB() call");
      return 0;
   }

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *SOCKET_sendi8LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   int64 i;
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsBigInt();
   else
      i = 0;

   int rc = s->sendi8LSB(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi8LSB() call");
      return 0;
   }

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *SOCKET_recv(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   int bs;
   if (p0)
      bs = p0->getAsInt();
   else
      bs = 0;
   
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 1));
   int rc;
   QoreStringNodeHolder msg(bs > 0 ? s->recv(bs, timeout, &rc) : s->recv(timeout, &rc));
	
   if (rc > 0)
      return msg.release();

   QoreSocket::doException(rc, "recv", xsink);
   return 0;
}

static AbstractQoreNode *SOCKET_recvi1(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   char b;
   int rc = s->recvi1(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi1", xsink);
}

static AbstractQoreNode *SOCKET_recvi2(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   short b;
   int rc = s->recvi2(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi2", xsink);
}

static AbstractQoreNode *SOCKET_recvi4(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   int b;
   int rc = s->recvi4(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi4", xsink);
}

static AbstractQoreNode *SOCKET_recvi8(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   int64 b;
   int rc = s->recvi8(timeout, &b);
   return doReadResult(rc, b, "recvi8", xsink);
}

static AbstractQoreNode *SOCKET_recvi2LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   short b;
   int rc = s->recvi2LSB(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi2LSB", xsink);
}

static AbstractQoreNode *SOCKET_recvi4LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   int b;
   int rc = s->recvi4LSB(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi4LSB", xsink);
}

static AbstractQoreNode *SOCKET_recvi8LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   int64 b;
   int rc = s->recvi8LSB(timeout, &b);
   return doReadResult(rc, b, "recvi8LSB", xsink);
}

static AbstractQoreNode *SOCKET_recvu1(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));
   
   unsigned char b;
   int rc = s->recvu1(timeout, &b);
   return doReadResult(rc, (int64)b, "recvu1", xsink);
}

static AbstractQoreNode *SOCKET_recvu2(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));
   
   unsigned short b;
   int rc = s->recvu2(timeout, &b);
   return doReadResult(rc, (int64)b, "recvu2", xsink);
}

static AbstractQoreNode *SOCKET_recvu4(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));
   
   unsigned int b;
   int rc = s->recvu4(timeout, &b);
   return doReadResult(rc, (int64)b, "recvu4", xsink);
}

static AbstractQoreNode *SOCKET_recvu2LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));
   
   unsigned short b;
   int rc = s->recvu2LSB(timeout, &b);
   return doReadResult(rc, (int64)b, "recvu2LSB", xsink);
}

static AbstractQoreNode *SOCKET_recvu4LSB(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));
   
   unsigned b;
   int rc = s->recvu4LSB(timeout, &b);
   return doReadResult(rc, (int64)b, "recvu4LSB", xsink);
}

static AbstractQoreNode *SOCKET_recvBinary(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   int bs = p0 ? p0->getAsInt() : 0;

   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 1));

   int rc;
   SimpleRefHolder<BinaryNode> b(bs > 0 ? s->recvBinary(bs, timeout, &rc) : s->recvBinary(timeout, &rc));

   if (rc > 0)
      return b.release();

   QoreSocket::doException(rc, "recvBinary", xsink);
   return 0;
}

// params: method, path, http_version, hash (http headers), data
static AbstractQoreNode *SOCKET_sendHTTPMessage(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0) {
      xsink->raiseException("SOCKET-SENDHTTPMESSAGE-PARAMETER-ERROR", "expecting method (string) as first parameter of Socket::sendHTTPMessage() call");
      return 0;
   }

   const QoreStringNode *p1 = test_string_param(params, 1);
   if (!p1)
   {
      xsink->raiseException("SOCKET-SENDHTTPMESSAGE-PARAMETER-ERROR", "expecting path (string) as second parameter of Socket::sendHTTPMessage() call");
      return 0;
   }

   const QoreStringNode *p2 = test_string_param(params, 2);
   if (!p2)
   {
      xsink->raiseException("SOCKET-SENDHTTPMESSAGE-PARAMETER-ERROR", "expecting HTTP version (string) as third parameter of Socket::sendHTTPMessage() call");
      return 0;
   }

   const QoreHashNode *headers = test_hash_param(params, 3);
   if (!headers)
   {
      xsink->raiseException("SOCKET-SENDHTTPMESSAGE-PARAMETER-ERROR", "expecting HTTP headers (hash) as fourth parameter of Socket::sendHTTPMessage() call");
      return 0;
   }

   const char *method, *path, *http_version;
   method = p0->getBuffer();
   path = p1->getBuffer();
   http_version = p2->getBuffer();

   // see if there is data to send as well
   const AbstractQoreNode *p4 = get_param(params, 4);
   const void *ptr = 0;
   int size = 0;

   if (p4) {
      if (p4->getType() == NT_STRING)
      {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p4);
	 ptr = str->getBuffer();
	 size = str->strlen();
      }
      else if (p4->getType() == NT_BINARY)
      {
	 const BinaryNode *b = reinterpret_cast<const BinaryNode *>(p4);
	 ptr = b->getPtr();
	 size = b->size();
      }
   }

   int rc = s->sendHTTPMessage(method, path, http_version, headers, ptr, size);
   if (rc == -2)
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendHTTPMessage() call");
   else if (rc)
      xsink->raiseException("SOCKET-SEND-ERROR", "send failed with error code %d: %s", rc, strerror(errno));

   return 0;
}

// params: status_code, status_desc, http_version, hash (http headers), data
static AbstractQoreNode *SOCKET_sendHTTPResponse(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   int status_code;
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (p0)
      status_code = p0->getAsInt();
   else
      status_code = 0;

   if (status_code < 100 || status_code >= 600) 
   {
      xsink->raiseException("SOCKET-SENDHTTPRESPONSE-PARAMETER-ERROR", "expecting valid HTTP status code (integer) as first parameter of Socket::sendHTTPResponse() call");
      return 0;
   }

   const QoreStringNode *p1 = test_string_param(params, 1);
   if (!p1)
   {
      xsink->raiseException("SOCKET-SENDHTTPRESPONSE-PARAMETER-ERROR", "expecting status description (string) as second parameter of Socket::sendHTTPResponse() call");
      return 0;
   }

   const QoreStringNode *p2 = test_string_param(params, 2);
   if (!p2)
   {
      xsink->raiseException("SOCKET-SENDHTTPRESPONSE-PARAMETER-ERROR", "expecting HTTP version (string) as third parameter of Socket::sendHTTPResponse() call");
      return 0;
   }

   const QoreHashNode *headers = test_hash_param(params, 3);
   if (!headers)
   {
      xsink->raiseException("SOCKET-SENDHTTPRESPONSE-PARAMETER-ERROR", "expecting HTTP headers (hash) as fourth parameter of Socket::sendHTTPResponse() call");
      return 0;
   }

   const char *status_desc, *http_version;
   status_desc = p1->getBuffer();
   http_version = p2->getBuffer();

   // see if there is data to send as well
   const AbstractQoreNode *p4 = get_param(params, 4);
   const void *ptr = 0;
   int size = 0;

   if (p4) {
      if (p4->getType() == NT_STRING)
      {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p4);
	 ptr = str->getBuffer();
	 size = str->strlen();
      }
      else if (p4->getType() == NT_BINARY)
      {
	 const BinaryNode *b = reinterpret_cast<const BinaryNode *>(p4);
	 ptr = b->getPtr();
	 size = b->size();
      }
   }

   int rc = s->sendHTTPResponse(status_code, status_desc, http_version, headers, ptr, size);
   if (rc == -2)
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendHTTPResponse() call");
   else if (rc)
      xsink->raiseException("SOCKET-SEND-ERROR", "send failed with error code %d: %s", rc, strerror(errno));

   return 0;
}

static AbstractQoreNode *SOCKET_readHTTPHeader(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   int timeout = getMsMinusOneInt(get_param(params, 0));
   int rc;

   // when rc = -3 it's a timeout, but rv will be NULL anyway, so we do nothing
   AbstractQoreNode *rv = s->readHTTPHeader(timeout, &rc);
      
   if (rc <= 0)
      QoreSocket::doException(rc, "readHTTPHeader", xsink);

   return rv;
}

static AbstractQoreNode *SOCKET_readHTTPChunkedBody(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   int timeout = getMsMinusOneInt(get_param(params, 0));

   // when rc = -3 it's a timeout, but rv will be NULL anyway, so we do nothing
   return s->readHTTPChunkedBody(timeout, xsink);
}

static AbstractQoreNode *SOCKET_readHTTPChunkedBodyBinary(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   int timeout = getMsMinusOneInt(get_param(params, 0));

   // when rc = -3 it's a timeout, but rv will be NULL anyway, so we do nothing
   return s->readHTTPChunkedBodyBinary(timeout, xsink);
}

static AbstractQoreNode *SOCKET_close(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(s->close());
}

static AbstractQoreNode *SOCKET_shutdown(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(s->shutdown());
}

static AbstractQoreNode *SOCKET_shutdownSSL(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(s->shutdownSSL(xsink));
}

static AbstractQoreNode *SOCKET_getPort(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(s->getPort());
}

static AbstractQoreNode *SOCKET_getSocket(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(s->getSocket());
}

static AbstractQoreNode *SOCKET_setSendTimeout(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
   {
      xsink->raiseException("SOCKET-SET-SEND-TIMEOUT-PARAMETER-ERROR", "expecting milliseconds(int) as parameter of Socket::setSendTimeout() call");
      return 0;
   }

   return new QoreBigIntNode(s->setSendTimeout(p0->getAsInt()));
}

static AbstractQoreNode *SOCKET_setRecvTimeout(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
   {
      xsink->raiseException("SOCKET-SET-SEND-TIMEOUT-PARAMETER-ERROR", "expecting milliseconds(int) as parameter of Socket::setRecvTimeout() call");
      return 0;
   }

   return new QoreBigIntNode(s->setRecvTimeout(p0->getAsInt()));
}

static AbstractQoreNode *SOCKET_getSendTimeout(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(s->getSendTimeout());
}

static AbstractQoreNode *SOCKET_getRecvTimeout(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(s->getRecvTimeout());
}

static AbstractQoreNode *SOCKET_setCharset(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
   {
      xsink->raiseException("SOCKET-SET-CHARSET-PARAMETER-ERROR", "expecting charset name (string) as parameter of Socket::setCharset() call");
      return 0;
   }

   s->setEncoding(QEM.findCreate(p0));
   return 0; 
}

static AbstractQoreNode *SOCKET_getCharset(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreStringNode(s->getEncoding()->getCode());
}

static AbstractQoreNode *SOCKET_isDataAvailable(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(s->isDataAvailable(getMsZeroInt(get_param(params, 0))));
}

static AbstractQoreNode *SOCKET_isWriteFinished(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(s->isWriteFinished(getMsZeroInt(get_param(params, 0))));
}

static AbstractQoreNode *SOCKET_isOpen(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(s->isOpen());
}

static AbstractQoreNode *SOCKET_getSSLCipherName(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const char *str = s->getSSLCipherName();
   if (str)
      return new QoreStringNode(str);

   return 0;
}

static AbstractQoreNode *SOCKET_getSSLCipherVersion(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const char *str = s->getSSLCipherVersion();
   if (str)
      return new QoreStringNode(str);

   return 0;
}

static AbstractQoreNode *SOCKET_isSecure(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(s->isSecure());
}

static AbstractQoreNode *SOCKET_verifyPeerCertificate(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   const char *c = getSSLCVCode(s->verifyPeerCertificate());
   return c ? new QoreStringNode(c) : 0;
}

static AbstractQoreNode *SOCKET_setCertificate(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // first check parameters
   const AbstractQoreNode *p0 = get_param(params, 0);
   class QoreSSLCertificate *cert;
   
   qore_type_t p0_type = p0 ? p0->getType() : 0;
   if (p0_type == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p0);
      // try and create object
      cert = new QoreSSLCertificate(str->getBuffer(), xsink);
      if (xsink->isEvent())
      {
	 cert->deref();
	 return 0;
      }
   }
   else {
      const QoreObject *o = p0_type == NT_OBJECT ? reinterpret_cast<const QoreObject *>(p0) : 0;
      cert = o ? (QoreSSLCertificate *)o->getReferencedPrivateData(CID_SSLCERTIFICATE, xsink) : 0;
      if (!cert)
      {
	 if (!*xsink)
	    xsink->raiseException("SOCKET-SETCERTIFICATE-ERROR", "expecting SSLCertificate object parameter");
	 return 0;
      }
   }

   s->setCertificate(cert);
   return 0;
}

// syntax: object|string
static AbstractQoreNode *SOCKET_setPrivateKey(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
   // first check parameters
   const AbstractQoreNode *p0 = get_param(params, 0);
   QoreSSLPrivateKey *pk;
   qore_type_t p0_type = p0 ? p0->getType() : 0;
   if (p0_type == NT_STRING) {
      // get passphrase if present
      const QoreStringNode *p1 = test_string_param(params, 1);
      const char *pp = p1 ? p1->getBuffer() : 0;
      // Try and create object
      pk = new QoreSSLPrivateKey(reinterpret_cast<const QoreStringNode *>(p0)->getBuffer(), (char *)pp, xsink);
      if (xsink->isEvent())
      {
	 pk->deref();
	 return 0;
      }
   }
   else {
      const QoreObject *o = p0_type == NT_OBJECT ? reinterpret_cast<const QoreObject *>(p0) : 0;
      pk = o ? (QoreSSLPrivateKey *)o->getReferencedPrivateData(CID_SSLPRIVATEKEY, xsink) : 0;
      if (!pk) {
	 if (!*xsink)
	    xsink->raiseException("SOCKET-SETPRIVATEKEY-ERROR", "expecting SSLPrivateKey object parameter");
	 return 0;
      }
   }

   s->setPrivateKey(pk);
   return 0;
}

static AbstractQoreNode *SOCKET_setEventQueue(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
    const QoreObject *o = test_object_param(params, 0);
    Queue *q = o ? (Queue *)o->getReferencedPrivateData(CID_QUEUE, xsink) : 0;
    if (*xsink)
	return 0;
    // pass reference from QoreObject::getReferencedPrivateData() to function
    s->setEventQueue(q, xsink);
    return 0;
}

static AbstractQoreNode *SOCKET_setNoDelay(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
    return new QoreBigIntNode(s->setNoDelay(get_int_param(params, 0)));
}

static AbstractQoreNode *SOCKET_getNoDelay(QoreObject *self, mySocket *s, const QoreListNode *params, ExceptionSink *xsink) {
    return get_bool_node(s->getNoDelay());
}

QoreClass *initSocketClass() {
   QORE_TRACE("initSocketClass()");

   QoreClass *QC_SOCKET = new QoreClass("Socket", QDOM_NETWORK);
   CID_SOCKET = QC_SOCKET->getID();
   
   QC_SOCKET->setConstructor(SOCKET_constructor);
   QC_SOCKET->setCopy(SOCKET_copy);
   QC_SOCKET->addMethod("connect",                   (q_method_t)SOCKET_connect);
   QC_SOCKET->addMethod("connectSSL",                (q_method_t)SOCKET_connectSSL);
   QC_SOCKET->addMethod("bind",                      (q_method_t)SOCKET_bind);
   QC_SOCKET->addMethod("accept",                    (q_method_t)SOCKET_accept);
   QC_SOCKET->addMethod("acceptSSL",                 (q_method_t)SOCKET_acceptSSL);
   QC_SOCKET->addMethod("listen",                    (q_method_t)SOCKET_listen);
   QC_SOCKET->addMethod("send",                      (q_method_t)SOCKET_send);
   QC_SOCKET->addMethod("sendBinary",                (q_method_t)SOCKET_sendBinary);
   QC_SOCKET->addMethod("sendi1",                    (q_method_t)SOCKET_sendi1);
   QC_SOCKET->addMethod("sendi2",                    (q_method_t)SOCKET_sendi2);
   QC_SOCKET->addMethod("sendi4",                    (q_method_t)SOCKET_sendi4);
   QC_SOCKET->addMethod("sendi8",                    (q_method_t)SOCKET_sendi8);
   QC_SOCKET->addMethod("sendi2LSB",                 (q_method_t)SOCKET_sendi2LSB);
   QC_SOCKET->addMethod("sendi4LSB",                 (q_method_t)SOCKET_sendi4LSB);
   QC_SOCKET->addMethod("sendi8LSB",                 (q_method_t)SOCKET_sendi8LSB);
   QC_SOCKET->addMethod("recv",                      (q_method_t)SOCKET_recv);
   QC_SOCKET->addMethod("recvBinary",                (q_method_t)SOCKET_recvBinary);
   QC_SOCKET->addMethod("recvi1",                    (q_method_t)SOCKET_recvi1);
   QC_SOCKET->addMethod("recvi2",                    (q_method_t)SOCKET_recvi2);
   QC_SOCKET->addMethod("recvi4",                    (q_method_t)SOCKET_recvi4);
   QC_SOCKET->addMethod("recvi8",                    (q_method_t)SOCKET_recvi8);
   QC_SOCKET->addMethod("recvi2LSB",                 (q_method_t)SOCKET_recvi2LSB);
   QC_SOCKET->addMethod("recvi4LSB",                 (q_method_t)SOCKET_recvi4LSB);
   QC_SOCKET->addMethod("recvi8LSB",                 (q_method_t)SOCKET_recvi8LSB);
   QC_SOCKET->addMethod("recvu1",                    (q_method_t)SOCKET_recvu1);
   QC_SOCKET->addMethod("recvu2",                    (q_method_t)SOCKET_recvu2);
   QC_SOCKET->addMethod("recvu4",                    (q_method_t)SOCKET_recvu4);
   QC_SOCKET->addMethod("recvu2LSB",                 (q_method_t)SOCKET_recvu2LSB);
   QC_SOCKET->addMethod("recvu4LSB",                 (q_method_t)SOCKET_recvu4LSB);
   QC_SOCKET->addMethod("sendHTTPMessage",           (q_method_t)SOCKET_sendHTTPMessage);
   QC_SOCKET->addMethod("sendHTTPResponse",          (q_method_t)SOCKET_sendHTTPResponse);
   QC_SOCKET->addMethod("readHTTPHeader",            (q_method_t)SOCKET_readHTTPHeader);
   QC_SOCKET->addMethod("readHTTPChunkedBody",       (q_method_t)SOCKET_readHTTPChunkedBody);
   QC_SOCKET->addMethod("readHTTPChunkedBodyBinary", (q_method_t)SOCKET_readHTTPChunkedBodyBinary);
   QC_SOCKET->addMethod("getPort",                   (q_method_t)SOCKET_getPort);
   QC_SOCKET->addMethod("close",                     (q_method_t)SOCKET_close);
   QC_SOCKET->addMethod("shutdown",                  (q_method_t)SOCKET_shutdown);
   QC_SOCKET->addMethod("shutdownSSL",               (q_method_t)SOCKET_shutdownSSL);
   QC_SOCKET->addMethod("getSocket",                 (q_method_t)SOCKET_getSocket);
   QC_SOCKET->addMethod("setSendTimeout",            (q_method_t)SOCKET_setSendTimeout);
   QC_SOCKET->addMethod("setRecvTimeout",            (q_method_t)SOCKET_setRecvTimeout);
   QC_SOCKET->addMethod("getSendTimeout",            (q_method_t)SOCKET_getSendTimeout);
   QC_SOCKET->addMethod("getRecvTimeout",            (q_method_t)SOCKET_getRecvTimeout);
   QC_SOCKET->addMethod("getCharset",                (q_method_t)SOCKET_getCharset);
   QC_SOCKET->addMethod("setCharset",                (q_method_t)SOCKET_setCharset);
   QC_SOCKET->addMethod("isDataAvailable",           (q_method_t)SOCKET_isDataAvailable);
   QC_SOCKET->addMethod("isWriteFinished",           (q_method_t)SOCKET_isWriteFinished);
   QC_SOCKET->addMethod("getSSLCipherName",          (q_method_t)SOCKET_getSSLCipherName);
   QC_SOCKET->addMethod("getSSLCipherVersion",       (q_method_t)SOCKET_getSSLCipherVersion);
   QC_SOCKET->addMethod("isSecure",                  (q_method_t)SOCKET_isSecure);
   QC_SOCKET->addMethod("verifyPeerCertificate",     (q_method_t)SOCKET_verifyPeerCertificate);
   QC_SOCKET->addMethod("setCertificate",            (q_method_t)SOCKET_setCertificate);
   QC_SOCKET->addMethod("setPrivateKey",             (q_method_t)SOCKET_setPrivateKey);
   QC_SOCKET->addMethod("isOpen",                    (q_method_t)SOCKET_isOpen);
   QC_SOCKET->addMethod("setEventQueue",             (q_method_t)SOCKET_setEventQueue);
   QC_SOCKET->addMethod("setNoDelay",                (q_method_t)SOCKET_setNoDelay);
   QC_SOCKET->addMethod("getNoDelay",                (q_method_t)SOCKET_getNoDelay);

   return QC_SOCKET;
}
