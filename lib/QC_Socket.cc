/*
  QC_Socket.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QC_Socket.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/Object.h>
#include <qore/ssl_constants.h>

#include <errno.h>
#include <string.h>

int CID_SOCKET;

static inline void doException(int rc, char *method_name, class ExceptionSink *xsink)
{
   if (!rc)             // remote end has closed the connection
      xsink->raiseException("SOCKET-CLOSED", "remote end has closed the connection");
   else if (rc == -1)   // recv() error
      xsink->raiseException("SOCKET-RECV-ERROR", strerror(errno));
   else if (rc == -2)   // TIMEOUT returns NOTHING
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::%s() call", method_name);
}

static inline class QoreNode *doReadResult(int rc, int64 val, char *method_name, class ExceptionSink *xsink)
{
   class QoreNode *rv = NULL;
   if (rc <= 0)
      doException(rc, method_name, xsink);
   else
      rv = new QoreNode(val);
   return rv;
}

static void getSocket(void *obj)
{
   ((mySocket *)obj)->ROreference();
}

static void releaseSocket(void *obj)
{
   ((mySocket *)obj)->deref();
}

static void SOCKET_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_SOCKET, new mySocket(), getSocket, releaseSocket);
}

static void SOCKET_destructor(class Object *self, void *obj, ExceptionSink *xsink)
{
   ((mySocket *)obj)->deref();
}

static void SOCKET_copy(class Object *self, class Object *old, void *obj, ExceptionSink *xsink)
{
   self->setPrivate(CID_SOCKET, new mySocket(), getSocket, releaseSocket);
}

// currently hardcoded to SOCK_STREAM
// opens and connects to a remote socket
// for AF_INET sockets:
// * connect("hostname:<port_number>");
// for AF_UNIX sockets:
// * connect("filename");
static QoreNode *SOCKET_connect(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   // if parameters are not correct
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("SOCKET-CONNECT-PARAMETER-ERROR",
			    "expecting string parameter (INET: 'hostname:port', UNIX: 'path/filename') for Socket::connect() call");
      return NULL;
   }

   s->connect(p0->val.String->getBuffer(), xsink);
   return NULL;
}

// currently hardcoded to SOCK_STREAM
// opens and connects to a remote socket and negotiates an SSL connection
// for AF_INET sockets:
// * connectSSL("hostname:<port_number>");
// for AF_UNIX sockets:
// * connectSSL("filename");
static QoreNode *SOCKET_connectSSL(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   // if parameters are not correct
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("SOCKET-CONNECTSSL-PARAMETER-ERROR",
		     "expecting string parameter (INET: 'hostname:port', UNIX: 'path/filename') for Socket::connectSSL() call");
      return NULL;
   }

   s->connectSSL(p0->val.String->getBuffer(), xsink);
   return NULL;
}

// currently hardcoded to SOCK_STREAM
// opens and binds to a local socket
// for AF_INET (tcp) sockets:
// * Socket::bind(<port_number>);
// for AF_UNIX (domain, file-based) sockets:
// * Socket::bind("filename");
// for INET (tcp) sockets
// * Socket::bind("iterface:port");
static QoreNode *SOCKET_bind(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;
   // if parameters are not correct
   if (!(p0 = get_param(params, 0)) || ((p0->type != NT_STRING && p0->type != NT_INT)))
   {
      xsink->raiseException("SOCKET-BIND-PARAMETER-ERROR", "no parameter passed to Socket::bind() call, expecing string for UNIX socket ('path/file') or int for INET socket (port number)");
      return NULL;
   }
   bool rua;
   if ((p1 = get_param(params, 1)))
      rua = p1->getAsBool();
   else
      rua = false;

   // create and bind tcp socket to all interfaces on port given
   if (p0->type == NT_INT)
      return new QoreNode((int64)s->bind(p0->val.intval, rua));
   else
      return new QoreNode((int64)s->bind(p0->val.String->getBuffer(), rua));
}

// Socket::accept()
// returns a new Socket object, connection source address is in $.source
// member of new object, hostname in $.source_host
static QoreNode *SOCKET_accept(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   class SocketSource source;
   mySocket *n = s->accept(&source, xsink);
   if (xsink->isEvent())
      return NULL;

   // ensure that a socket object is returned (and not a subclass)
   Object *ns = new Object(self->getClass(CID_SOCKET), getProgram());
   ns->setPrivate(CID_SOCKET, n, getSocket, releaseSocket);
   source.setAll(ns, xsink);
      
   return new QoreNode(ns);
}

// Socket::acceptSSL()
// accepts a new connection, negotiates an SSL connection, and returns the new socket
// the connection source string is in the "$.source" member of new object,
// hostname in "$.source_host"
static QoreNode *SOCKET_acceptSSL(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   class SocketSource source;
   mySocket *n = s->acceptSSL(&source, xsink);
   if (xsink->isEvent())
      return NULL;

   // ensure that a socket object is returned (and not a subclass)
   Object *ns = new Object(self->getClass(CID_SOCKET), getProgram());
   ns->setPrivate(CID_SOCKET, n, getSocket, releaseSocket);
   source.setAll(ns, xsink);
   
   return new QoreNode(ns);
}

static QoreNode *SOCKET_listen(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   int rc = s->listen();

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::listen() call");
      return NULL;
   }

   return new QoreNode((int64)rc);
}

static QoreNode *SOCKET_send(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->type != NT_STRING && p0->type != NT_BINARY))
   {
      xsink->raiseException("SOCKET-SEND-PARAMETER-ERROR", "expecting string or binary data as first parameter of Socket::send() call");
      return NULL;
   }

   int rc;

   if (p0->type == NT_STRING)
      rc = s->send(p0->val.String, xsink);
   else
      rc = s->send(p0->val.bin);
      
   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::send() call");
      return NULL;
   }

   return new QoreNode((int64)rc);
}

static QoreNode *SOCKET_sendBinary(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->type != NT_STRING && p0->type != NT_BINARY))
   {
      xsink->raiseException("SOCKET-SEND-BINARY-PARAMETER-ERROR", "expecting string or binary data as first parameter of Socket::sendBinary() call");
      return NULL;
   }

   int rc = 0;

   // send strings with no conversions
   if (p0->type == NT_STRING)
      rc = s->send(p0->val.String->getBuffer(), p0->val.String->strlen());
   else
      rc = s->send(p0->val.bin);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendBinary() call");
      return NULL;
   }

   return new QoreNode((int64)rc);
}

static QoreNode *SOCKET_sendi1(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   char i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   int rc = s->sendi1(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi1() call");
      return NULL;
   }

   return new QoreNode((int64)rc);
}

static QoreNode *SOCKET_sendi2(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   short i;
   QoreNode *p0 = get_param(params, 0);
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

   return new QoreNode((int64)rc);
}

static QoreNode *SOCKET_sendi4(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   int i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   int rc = s->sendi4(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi4() call");
      return NULL;
   }

   return new QoreNode((int64)rc);
}

static QoreNode *SOCKET_sendi8(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   int64 i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsBigInt();
   else
      i = 0;

   int rc = s->sendi8(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi8() call");
      return NULL;
   }

   return new QoreNode((int64)rc);
}

static QoreNode *SOCKET_sendi2LSB(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   short i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   int rc = s->sendi2LSB(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi2LSB() call");
      return NULL;
   }

   return new QoreNode((int64)rc);
}

static QoreNode *SOCKET_sendi4LSB(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   int i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   int rc = s->sendi4LSB(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi4LSB() call");
      return NULL;
   }

   return new QoreNode((int64)rc);
}

static QoreNode *SOCKET_sendi8LSB(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   int64 i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsBigInt();
   else
      i = 0;

   int rc = s->sendi8LSB(i);

   if (rc == -2)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi8LSB() call");
      return NULL;
   }

   return new QoreNode((int64)rc);
}

static QoreNode *SOCKET_recv(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   int bs;
   if (p0)
      bs = p0->getAsInt();
   else
      bs = 0;
   
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 1));

   int rc;
   QoreString *msg;

   if (bs)
      msg = s->recv(bs, timeout, &rc);
   else
      msg = s->recv(timeout, &rc);
	
   if (rc > 0)
      return new QoreNode(msg);

   doException(rc, "recv", xsink);
   return NULL;
}

static QoreNode *SOCKET_recvi1(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   char b;
   int rc = s->recvi1(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi1", xsink);
}

static QoreNode *SOCKET_recvi2(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   short b;
   int rc = s->recvi2(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi2", xsink);
}

static QoreNode *SOCKET_recvi4(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   int b;
   int rc = s->recvi4(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi4", xsink);
}

static QoreNode *SOCKET_recvi8(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   int64 b;
   int rc = s->recvi8(timeout, &b);
   return doReadResult(rc, b, "recvi8", xsink);
}

static QoreNode *SOCKET_recvi2LSB(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   short b;
   int rc = s->recvi2LSB(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi2LSB", xsink);
}

static QoreNode *SOCKET_recvi4LSB(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   int b;
   int rc = s->recvi4LSB(timeout, &b);
   return doReadResult(rc, (int64)b, "recvi4LSB", xsink);
}

static QoreNode *SOCKET_recvi8LSB(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 0));

   int64 b;
   int rc = s->recvi8LSB(timeout, &b);
   return doReadResult(rc, b, "recvi8LSB", xsink);
}

static QoreNode *SOCKET_recvBinary(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   int bs = 0;
   if (p0)
      bs = p0->getAsInt();
   if (!bs)
   {
      xsink->raiseException("SOCKET-RECVBINARY-PARAMETER-ERROR", "missing positive buffer size parameter");
      return NULL;
   }

   // get timeout
   int timeout = getMsMinusOneInt(get_param(params, 1));

   int rc;
   BinaryObject *b = s->recvBinary(bs, timeout, &rc);

   if (rc > 0)
      return new QoreNode(b);

   doException(rc, "recvBinary", xsink);
   return NULL;
}

// params: method, path, http_version, hash (http headers), data
static QoreNode *SOCKET_sendHTTPMessage(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("SOCKET-SENDHTTPMESSAGE-PARAMETER-ERROR", "expecting method (string) as first parameter of Socket::sendHTTPMessage() call");
      return NULL;
   }

   QoreNode *p1 = test_param(params, NT_STRING, 1);
   if (!p1)
   {
      xsink->raiseException("SOCKET-SENDHTTPMESSAGE-PARAMETER-ERROR", "expecting path (string) as second parameter of Socket::sendHTTPMessage() call");
      return NULL;
   }

   QoreNode *p2 = test_param(params, NT_STRING, 2);
   if (!p2)
   {
      xsink->raiseException("SOCKET-SENDHTTPMESSAGE-PARAMETER-ERROR", "expecting HTTP version (string) as third parameter of Socket::sendHTTPMessage() call");
      return NULL;
   }

   QoreNode *p3 = test_param(params, NT_HASH, 3);
   if (!p3)
   {
      xsink->raiseException("SOCKET-SENDHTTPMESSAGE-PARAMETER-ERROR", "expecting HTTP headers (hash) as fourth parameter of Socket::sendHTTPMessage() call");
      return NULL;
   }

   char *method, *path, *http_version;
   method = p0->val.String->getBuffer();
   path = p1->val.String->getBuffer();
   http_version = p2->val.String->getBuffer();

   class Hash *headers = p3->val.hash;

   // see if there is data to send as well
   QoreNode *p4 = get_param(params, 4);
   void *ptr = NULL;
   int size = 0;

   if (p4)
      if (p4->type == NT_STRING)
      {
	 ptr = p4->val.String->getBuffer();
	 size = p4->val.String->strlen();
      }
      else if (p4->type == NT_BINARY)
      {
	 ptr = p4->val.bin->getPtr();
	 size = p4->val.bin->size();
      }

   int rc = s->sendHTTPMessage(method, path, http_version, headers, ptr, size);
   if (rc == -2)
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendHTTPMessage() call");
   else if (rc)
      xsink->raiseException("SOCKET-SEND-ERROR", "send failed with error code %d: %s", rc, strerror(errno));

   return NULL;
}

// params: status_code, status_desc, http_version, hash (http headers), data
static QoreNode *SOCKET_sendHTTPResponse(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   int status_code;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      status_code = p0->getAsInt();
   else
      status_code = 0;

   if (status_code < 100 || status_code >= 600) 
   {
      xsink->raiseException("SOCKET-SENDHTTPRESPONSE-PARAMETER-ERROR", "expecting valid HTTP status code (integer) as first parameter of Socket::sendHTTPResponse() call");
      return NULL;
   }

   QoreNode *p1 = test_param(params, NT_STRING, 1);
   if (!p1)
   {
      xsink->raiseException("SOCKET-SENDHTTPRESPONSE-PARAMETER-ERROR", "expecting status description (string) as second parameter of Socket::sendHTTPResponse() call");
      return NULL;
   }

   QoreNode *p2 = test_param(params, NT_STRING, 2);
   if (!p2)
   {
      xsink->raiseException("SOCKET-SENDHTTPRESPONSE-PARAMETER-ERROR", "expecting HTTP version (string) as third parameter of Socket::sendHTTPResponse() call");
      return NULL;
   }

   QoreNode *p3 = test_param(params, NT_HASH, 3);
   if (!p3)
   {
      xsink->raiseException("SOCKET-SENDHTTPRESPONSE-PARAMETER-ERROR", "expecting HTTP headers (hash) as fourth parameter of Socket::sendHTTPResponse() call");
      return NULL;
   }

   char *status_desc, *http_version;
   status_desc = p1->val.String->getBuffer();
   http_version = p2->val.String->getBuffer();

   class Hash *headers = p3->val.hash;

   // see if there is data to send as well
   QoreNode *p4 = get_param(params, 4);
   void *ptr = NULL;
   int size = 0;

   if (p4)
      if (p4->type == NT_STRING)
      {
	 ptr = p4->val.String->getBuffer();
	 size = p4->val.String->strlen();
      }
      else if (p4->type == NT_BINARY)
      {
	 ptr = p4->val.bin->getPtr();
	 size = p4->val.bin->size();
      }

   int rc = s->sendHTTPResponse(status_code, status_desc, http_version, headers, ptr, size);
   if (rc == -2)
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendHTTPResponse() call");
   else if (rc)
      xsink->raiseException("SOCKET-SEND-ERROR", "send failed with error code %d: %s", rc, strerror(errno));

   return NULL;
}

static QoreNode *SOCKET_readHTTPHeader(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   int timeout = getMsMinusOneInt(get_param(params, 0));
   int rc;

   // when rc = -3 it's a timeout, but rv will be NULL anyway, so we do nothing
   class QoreNode *rv = s->readHTTPHeader(timeout, &rc);
      
   if (rc <= 0)
      doException(rc, "readHTTPHeader", xsink);

   return rv;
}

static QoreNode *SOCKET_close(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)s->close());
}

static QoreNode *SOCKET_shutdown(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)s->shutdown());
}

static QoreNode *SOCKET_shutdownSSL(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)s->shutdownSSL(xsink));
}

static QoreNode *SOCKET_getPort(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)s->getPort());
}

static QoreNode *SOCKET_getSocket(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)s->getSocket());
}

static QoreNode *SOCKET_setSendTimeout(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
   {
      xsink->raiseException("SOCKET-SET-SEND-TIMEOUT-PARAMETER-ERROR", "expecting milliseconds(int) as parameter of Socket::setSendTimeout() call");
      return NULL;
   }

   return new QoreNode((int64)s->setSendTimeout(p0->getAsInt()));
}

static QoreNode *SOCKET_setRecvTimeout(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
   {
      xsink->raiseException("SOCKET-SET-SEND-TIMEOUT-PARAMETER-ERROR", "expecting milliseconds(int) as parameter of Socket::setRecvTimeout() call");
      return NULL;
   }

   return new QoreNode((int64)s->setRecvTimeout(p0->getAsInt()));
}

static QoreNode *SOCKET_getSendTimeout(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)s->getSendTimeout());
}

static QoreNode *SOCKET_getRecvTimeout(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)s->getRecvTimeout());
}

static QoreNode *SOCKET_setCharset(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("SOCKET-SET-CHARSET-PARAMETER-ERROR", "expecting charset name (string) as parameter of Socket::setCharset() call");
      return NULL;
   }

   s->setEncoding(QEM.findCreate(p0->val.String));
   return NULL; 
}

static QoreNode *SOCKET_getCharset(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getEncoding()->code);
}

static QoreNode *SOCKET_isDataAvailable(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   return new QoreNode(s->isDataAvailable(p0 ? p0->getAsInt() : 0));
}

static QoreNode *SOCKET_isOpen(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->isOpen());
}

static QoreNode *SOCKET_getSSLCipherName(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   const char *str = s->getSSLCipherName();
   if (str)
      return new QoreNode(str);

   return NULL;
}

static QoreNode *SOCKET_getSSLCipherVersion(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   const char *str = s->getSSLCipherVersion();
   if (str)
      return new QoreNode(str);

   return NULL;
}

static QoreNode *SOCKET_isSecure(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->isSecure());
}

static QoreNode *SOCKET_verifyPeerCertificate(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   char *c = getSSLCVCode(s->verifyPeerCertificate());
   return c ? new QoreNode(c) : NULL;
}

static QoreNode *SOCKET_setCertificate(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   // first check parameters
   QoreNode *p0 = get_param(params, 0);
   class QoreSSLCertificate *cert;

   if (p0 && p0->type == NT_STRING)
   {
      // try and create object
      cert = new QoreSSLCertificate(p0->val.String->getBuffer(), xsink);
      if (xsink->isEvent())
      {
	 cert->deref();
	 return NULL;
      }
   }
   else if (!p0 || p0->type != NT_OBJECT || !(cert = (QoreSSLCertificate *)p0->val.object->getReferencedPrivateData(CID_SSLCERTIFICATE)))
   {
      xsink->raiseException("SOCKET-SETCERTIFICATE-ERROR", "expecting SSLCertificate object parameter");
      return NULL;
   }

   s->setCertificate(cert);
   return NULL;
}

// syntax: object|string
static QoreNode *SOCKET_setPrivateKey(class Object *self, class mySocket *s, class QoreNode *params, ExceptionSink *xsink)
{
   // first check parameters
   QoreNode *p0 = get_param(params, 0);
   class QoreSSLPrivateKey *pk;

   if (p0 && p0->type == NT_STRING)
   {
      // get passphrase if present
      QoreNode *p1 = test_param(params, NT_STRING, 1);
      char *pp = p1 ? p1->val.String->getBuffer() : NULL;
      // Try and create object
      pk = new QoreSSLPrivateKey(p0->val.String->getBuffer(), pp, xsink);
      if (xsink->isEvent())
      {
	 pk->deref();
	 return NULL;
      }
   }
   else if (!p0 || (p0->type != NT_OBJECT) || !(pk = (QoreSSLPrivateKey *)p0->val.object->getReferencedPrivateData(CID_SSLPRIVATEKEY)))
   {
      xsink->raiseException("SOCKET-SETPRIVATEKEY-ERROR", "expecting SSLPrivateKey object parameter");
      return NULL;
   }

   s->setPrivateKey(pk);
   return NULL;
}

class QoreClass *initSocketClass()
{
   tracein("initSocketClass()");

   class QoreClass *QC_SOCKET = new QoreClass(QDOM_NETWORK, strdup("Socket"));
   CID_SOCKET = QC_SOCKET->getID();
   QC_SOCKET->setConstructor(SOCKET_constructor);
   QC_SOCKET->setDestructor(SOCKET_destructor);
   QC_SOCKET->setCopy(SOCKET_copy);
   QC_SOCKET->addMethod("connect",               (q_method_t)SOCKET_connect);
   QC_SOCKET->addMethod("connectSSL",            (q_method_t)SOCKET_connectSSL);
   QC_SOCKET->addMethod("bind",                  (q_method_t)SOCKET_bind);
   QC_SOCKET->addMethod("accept",                (q_method_t)SOCKET_accept);
   QC_SOCKET->addMethod("acceptSSL",             (q_method_t)SOCKET_acceptSSL);
   QC_SOCKET->addMethod("listen",                (q_method_t)SOCKET_listen);
   QC_SOCKET->addMethod("send",                  (q_method_t)SOCKET_send);
   QC_SOCKET->addMethod("sendBinary",            (q_method_t)SOCKET_sendBinary);
   QC_SOCKET->addMethod("sendi1",                (q_method_t)SOCKET_sendi1);
   QC_SOCKET->addMethod("sendi2",                (q_method_t)SOCKET_sendi2);
   QC_SOCKET->addMethod("sendi4",                (q_method_t)SOCKET_sendi4);
   QC_SOCKET->addMethod("sendi8",                (q_method_t)SOCKET_sendi8);
   QC_SOCKET->addMethod("sendi2LSB",             (q_method_t)SOCKET_sendi2LSB);
   QC_SOCKET->addMethod("sendi4LSB",             (q_method_t)SOCKET_sendi4LSB);
   QC_SOCKET->addMethod("sendi8LSB",             (q_method_t)SOCKET_sendi8LSB);
   QC_SOCKET->addMethod("recv",                  (q_method_t)SOCKET_recv);
   QC_SOCKET->addMethod("recvBinary",            (q_method_t)SOCKET_recvBinary);
   QC_SOCKET->addMethod("recvi1",                (q_method_t)SOCKET_recvi1);
   QC_SOCKET->addMethod("recvi2",                (q_method_t)SOCKET_recvi2);
   QC_SOCKET->addMethod("recvi4",                (q_method_t)SOCKET_recvi4);
   QC_SOCKET->addMethod("recvi8",                (q_method_t)SOCKET_recvi8);
   QC_SOCKET->addMethod("recvi2LSB",             (q_method_t)SOCKET_recvi2LSB);
   QC_SOCKET->addMethod("recvi4LSB",             (q_method_t)SOCKET_recvi4LSB);
   QC_SOCKET->addMethod("recvi8LSB",             (q_method_t)SOCKET_recvi8LSB);
   QC_SOCKET->addMethod("sendHTTPMessage",       (q_method_t)SOCKET_sendHTTPMessage);
   QC_SOCKET->addMethod("sendHTTPResponse",      (q_method_t)SOCKET_sendHTTPResponse);
   QC_SOCKET->addMethod("readHTTPHeader",        (q_method_t)SOCKET_readHTTPHeader);
   QC_SOCKET->addMethod("getPort",               (q_method_t)SOCKET_getPort);
   QC_SOCKET->addMethod("close",                 (q_method_t)SOCKET_close);
   QC_SOCKET->addMethod("shutdown",              (q_method_t)SOCKET_shutdown);
   QC_SOCKET->addMethod("shutdownSSL",           (q_method_t)SOCKET_shutdownSSL);
   QC_SOCKET->addMethod("getSocket",             (q_method_t)SOCKET_getSocket);
   QC_SOCKET->addMethod("setSendTimeout",        (q_method_t)SOCKET_setSendTimeout);
   QC_SOCKET->addMethod("setRecvTimeout",        (q_method_t)SOCKET_setRecvTimeout);
   QC_SOCKET->addMethod("getSendTimeout",        (q_method_t)SOCKET_getSendTimeout);
   QC_SOCKET->addMethod("getRecvTimeout",        (q_method_t)SOCKET_getRecvTimeout);
   QC_SOCKET->addMethod("getCharset",            (q_method_t)SOCKET_getCharset);
   QC_SOCKET->addMethod("setCharset",            (q_method_t)SOCKET_setCharset);
   QC_SOCKET->addMethod("isDataAvailable",       (q_method_t)SOCKET_isDataAvailable);
   QC_SOCKET->addMethod("getSSLCipherName",      (q_method_t)SOCKET_getSSLCipherName);
   QC_SOCKET->addMethod("getSSLCipherVersion",   (q_method_t)SOCKET_getSSLCipherVersion);
   QC_SOCKET->addMethod("isSecure",              (q_method_t)SOCKET_isSecure);
   QC_SOCKET->addMethod("verifyPeerCertificate", (q_method_t)SOCKET_verifyPeerCertificate);
   QC_SOCKET->addMethod("setCertificate",        (q_method_t)SOCKET_setCertificate);
   QC_SOCKET->addMethod("setPrivateKey",         (q_method_t)SOCKET_setPrivateKey);
   QC_SOCKET->addMethod("isOpen",                (q_method_t)SOCKET_isOpen);

   traceout("initSocketClass()");
   return QC_SOCKET;
}
