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

static inline void *getSocket(void *obj)
{
   ((mySocket *)obj)->ROreference();
   return obj;
}

static QoreNode *SOCKET_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_SOCKET, new mySocket(), getSocket);
   return NULL;
}

static QoreNode *SOCKET_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   mySocket *s = (mySocket *)self->getAndClearPrivateData(CID_SOCKET);
   if (s)
      s->deref();
   return NULL;
}

// currently hardcoded to SOCK_STREAM
// opens and connects to a remote socket
// for AF_INET sockets:
// * connect("hostname:<port_number>");
// for AF_UNIX sockets:
// * connect("filename");
static QoreNode *SOCKET_connect(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   // if parameters are not correct
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("SOCKET-CONNECT-PARAMETER-ERROR",
			    "expecting string parameter (INET: 'hostname:port', UNIX: 'path/filename') for Socket::connect() call");
      return NULL;
   }

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);

   if (s)
   {
      s->connect(p0->val.String->getBuffer(), xsink);
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::connect");
   return NULL;
}

// currently hardcoded to SOCK_STREAM
// opens and connects to a remote socket and negotiates an SSL connection
// for AF_INET sockets:
// * connectSSL("hostname:<port_number>");
// for AF_UNIX sockets:
// * connectSSL("filename");
static QoreNode *SOCKET_connectSSL(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   // if parameters are not correct
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("SOCKET-CONNECTSSL-PARAMETER-ERROR",
		     "expecting string parameter (INET: 'hostname:port', UNIX: 'path/filename') for Socket::connectSSL() call");
      return NULL;
   }

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);

   if (s)
   {
      s->connectSSL(p0->val.String->getBuffer(), xsink);
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::connectSSL");
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
static QoreNode *SOCKET_bind(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      // create and bind tcp socket to all interfaces on port given
      if (p0->type == NT_INT)
	 rv = new QoreNode((int64)s->bind(p0->val.intval, rua));
      else
	 rv = new QoreNode((int64)s->bind(p0->val.String->getBuffer(), rua));
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::bind");
   }
   return rv;
}

// Socket::accept()
// returns a new Socket object, connection source address is in $.source
// member of new object, hostname in $.source_host
static QoreNode *SOCKET_accept(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      class SocketSource source;
      mySocket *n = s->accept(&source, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
      {
	 // ensure that a socket object is returned (and not a subclass)
	 Object *ns = new Object(self->getClass(CID_SOCKET), getProgram());
	 ns->setPrivate(CID_SOCKET, n, getSocket);
	 source.setAll(ns, xsink);
	 
	 rv = new QoreNode(ns);
      }
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::accept");
   }
   return rv;
}

// Socket::acceptSSL()
// accepts a new connection, negotiates an SSL connection, and returns the new socket
// the connection source string is in the "$.source" member of new object,
// hostname in "$.source_host"
static QoreNode *SOCKET_acceptSSL(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      class SocketSource source;
      mySocket *n = s->acceptSSL(&source, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
      {
	 // ensure that a socket object is returned (and not a subclass)
	 Object *ns = new Object(self->getClass(CID_SOCKET), getProgram());
	 ns->setPrivate(CID_SOCKET, n, getSocket);
	 source.setAll(ns, xsink);

	 rv = new QoreNode(ns);
      }
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::acceptSSL");
   }
   return rv;
}

static QoreNode *SOCKET_listen(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      int rc = s->listen();

      if (rc == -2)
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::listen() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode((int64)rc);

      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::listen");
   }
   return rv;
}

static QoreNode *SOCKET_send(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->type != NT_STRING && p0->type != NT_BINARY))
   {
      xsink->raiseException("SOCKET-SEND-PARAMETER-ERROR", "expecting string or binary data as first parameter of Socket::send() call");
      return NULL;
   }

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      int rc;

      if (p0->type == NT_STRING)
	 rc = s->send(p0->val.String, xsink);
      else
	 rc = s->send(p0->val.bin);
      
      if (rc == -2)
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::send() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode((int64)rc);

      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::send");
   }
   return rv;
}

static QoreNode *SOCKET_sendBinary(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->type != NT_STRING && p0->type != NT_BINARY))
   {
      xsink->raiseException("SOCKET-SEND-BINARY-PARAMETER-ERROR", "expecting string or binary data as first parameter of Socket::sendBinary() call");
      return NULL;
   }

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      int rc = 0;

      // send strings with no conversions
      if (p0->type == NT_STRING)
	 rc = s->send(p0->val.String->getBuffer(), p0->val.String->strlen());
      else
	 rc = s->send(p0->val.bin);

      if (rc == -2)
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendBinary() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode((int64)rc);

      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::sendBinary");
   }
   return rv;
}

static QoreNode *SOCKET_sendi1(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   char i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      int rc = s->sendi1(i);

      if (rc == -2)
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi1() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode((int64)rc);

      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::sendi1");
   }
   return rv;
}

static QoreNode *SOCKET_sendi2(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   short i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      int rc = s->sendi2(i);

      if (rc == -2)
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi2() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode((int64)rc);

      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::sendi2");
   }
   return rv;
}

static QoreNode *SOCKET_sendi4(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   int i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      int rc = s->sendi4(i);

      if (rc == -2)
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi4() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode((int64)rc);

      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::sendi4");
   }
   return rv;
}

static QoreNode *SOCKET_sendi8(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   int64 i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsBigInt();
   else
      i = 0;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      int rc = s->sendi8(i);

      if (rc == -2)
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi8() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode((int64)rc);

      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::sendi8");
   }
   return rv;
}

static QoreNode *SOCKET_sendi2LSB(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   short i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      int rc = s->sendi2LSB(i);

      if (rc == -2)
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi2LSB() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode((int64)rc);

      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::sendi2LSB");
   }
   return rv;
}

static QoreNode *SOCKET_sendi4LSB(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   int i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsInt();
   else
      i = 0;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      int rc = s->sendi4LSB(i);

      if (rc == -2)
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi4LSB() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode((int64)rc);

      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::sendi4LSB");
   }
   return rv;
}

static QoreNode *SOCKET_sendi8LSB(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   int64 i;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      i = p0->getAsBigInt();
   else
      i = 0;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      int rc = s->sendi8LSB(i);

      if (rc == -2)
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi8LSB() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode((int64)rc);

      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::sendi8LSB");
   }
   return rv;
}

static QoreNode *SOCKET_recv(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   int bs;
   if (p0)
      bs = p0->getAsInt();
   else
      bs = 0;
   
   // get timeout
   QoreNode *p1 = get_param(params, 1);
   int timeout;
   if (p1)
      timeout = p1->getAsInt();
   else
      timeout = -1;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv = NULL;

   if (s)
   {
      int rc;
      QoreString *msg;

      if (bs)
	 msg = s->recv(bs, timeout, &rc);
      else
	 msg = s->recv(timeout, &rc);
	 
      if (rc <= 0)
	 doException(rc, "recv", xsink);
      else
	 rv = new QoreNode(msg);
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::recv");

   return rv;
}

static QoreNode *SOCKET_recvi1(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   QoreNode *p0 = get_param(params, 0);
   int timeout;
   if (p0)
      timeout = p0->getAsInt();
   else
      timeout = -1;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv = NULL;

   if (s)
   {
      char b;
      int rc = s->recvi1(timeout, &b);
      rv = doReadResult(rc, (int64)b, "recvi1", xsink);
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::revci1");

   return rv;
}

static QoreNode *SOCKET_recvi2(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   QoreNode *p0 = get_param(params, 0);
   int timeout;
   if (p0)
      timeout = p0->getAsInt();
   else
      timeout = -1;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv = NULL;

   if (s)
   {
      short b;
      int rc = s->recvi2(timeout, &b);
      rv = doReadResult(rc, (int64)b, "recvi2", xsink);
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::recvi2");

   return rv;
}

static QoreNode *SOCKET_recvi4(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   QoreNode *p0 = get_param(params, 0);
   int timeout;
   if (p0)
      timeout = p0->getAsInt();
   else
      timeout = -1;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv = NULL;

   if (s)
   {
      int b;
      int rc = s->recvi4(timeout, &b);
      rv = doReadResult(rc, (int64)b, "recvi4", xsink);
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::recvi4");

   return rv;
}

static QoreNode *SOCKET_recvi8(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   QoreNode *p0 = get_param(params, 0);
   int timeout;
   if (p0)
      timeout = p0->getAsInt();
   else
      timeout = -1;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv = NULL;

   if (s)
   {
      int64 b;
      int rc = s->recvi8(timeout, &b);
      rv = doReadResult(rc, b, "recvi8", xsink);
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::recvi8");

   return rv;
}

static QoreNode *SOCKET_recvi2LSB(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   QoreNode *p0 = get_param(params, 0);
   int timeout;
   if (p0)
      timeout = p0->getAsInt();
   else
      timeout = -1;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv = NULL;

   if (s)
   {
      short b;
      int rc = s->recvi2LSB(timeout, &b);
      rv = doReadResult(rc, (int64)b, "recvi2LSB", xsink);
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::recvi2LSB");

   return rv;
}

static QoreNode *SOCKET_recvi4LSB(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   QoreNode *p0 = get_param(params, 0);
   int timeout;
   if (p0)
      timeout = p0->getAsInt();
   else
      timeout = -1;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      int b;
      int rc = s->recvi4LSB(timeout, &b);
      rv = doReadResult(rc, (int64)b, "recvi4LSB", xsink);
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::recvi4LSB");
   }
   return rv;
}

static QoreNode *SOCKET_recvi8LSB(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // get timeout
   QoreNode *p0 = get_param(params, 0);
   int timeout;
   if (p0)
      timeout = p0->getAsInt();
   else
      timeout = -1;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      int64 b;
      int rc = s->recvi8LSB(timeout, &b);
      rv = doReadResult(rc, b, "recvi8LSB", xsink);
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::recvi4LSB");
   }
   return rv;
}

static QoreNode *SOCKET_recvBinary(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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
   QoreNode *p1 = get_param(params, 1);
   int timeout;
   if (p1)
      timeout = p1->getAsInt();
   else
      timeout = -1;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv = NULL;

   if (s)
   {
      int rc;
      BinaryObject *b = s->recvBinary(bs, timeout, &rc);

      if (rc <= 0)
	 doException(rc, "recvBinary", xsink);
      else
	 rv = new QoreNode(b);
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::recvBinary");

   return rv;
}

// params: method, path, http_version, hash (http headers), data
static QoreNode *SOCKET_sendHTTPMessage(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);

   if (s)
   {
      int rc = s->sendHTTPMessage(method, path, http_version, headers, ptr, size);
      if (rc == -2)
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendHTTPMessage() call");
      else if (rc)
	 xsink->raiseException("SOCKET-SEND-ERROR", "send failed with error code %d: %s", rc, strerror(errno));
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::sendHTTPMessage");

   return NULL;
}

// params: status_code, status_desc, http_version, hash (http headers), data
static QoreNode *SOCKET_sendHTTPResponse(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);

   if (s)
   {
      int rc = s->sendHTTPResponse(status_code, status_desc, http_version, headers, ptr, size);
      if (rc == -2)
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendHTTPResponse() call");
      else if (rc)
	 xsink->raiseException("SOCKET-SEND-ERROR", "send failed with error code %d: %s", rc, strerror(errno));
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::sendHTTPResponse");

   return NULL;
}

static QoreNode *SOCKET_readHTTPHeader(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   int timeout;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      timeout = p0->getAsInt();
   else
      timeout = -1;

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv = NULL;

   if (s)
   {
      xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::readHTTPHeader() call");
      int rc;

      // when rc = -2 it's a timeout, but rv will be NULL anyway, so we do nothing
      rv = s->readHTTPHeader(timeout, &rc);
      
      if (rc <= 0)
	 doException(rc, "readHTTPHeader", xsink);

      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::readHTTPHeader");

   return rv;
}

static QoreNode *SOCKET_close(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode((int64)s->close());
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::close");
   }
   return rv;
}

static QoreNode *SOCKET_shutdown(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode((int64)s->shutdown());
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::shutdown");
   }
   return rv;
}

static QoreNode *SOCKET_shutdownSSL(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode((int64)s->shutdownSSL(xsink));
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::shutdownSSL");
   }
   return rv;
}

static QoreNode *SOCKET_getPort(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode((int64)s->getPort());
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::getPort");
   }
   return rv;
}

static QoreNode *SOCKET_getSocket(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode((int64)s->getSocket());
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::getSocket");
   }
   return rv;
}

static QoreNode *SOCKET_setSendTimeout(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
   {
      xsink->raiseException("SOCKET-SET-SEND-TIMEOUT-PARAMETER-ERROR", "expecting milliseconds(int) as parameter of Socket::setSendTimeout() call");
      return NULL;
   }
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode((int64)s->setSendTimeout(p0->getAsInt()));
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::setSendTimeout");
   }
   return rv;
}

static QoreNode *SOCKET_setRecvTimeout(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
   {
      xsink->raiseException("SOCKET-SET-SEND-TIMEOUT-PARAMETER-ERROR", "expecting milliseconds(int) as parameter of Socket::setRecvTimeout() call");
      return NULL;
   }
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode((int64)s->setRecvTimeout(p0->getAsInt()));
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::setRecvTimeout");
   }
   return rv;
}

static QoreNode *SOCKET_getSendTimeout(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode((int64)s->getSendTimeout());
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::getSendTimeout");
   }
   return rv;
}

static QoreNode *SOCKET_getRecvTimeout(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode((int64)s->getRecvTimeout());
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::getRecvTimeout");
   }
   return rv;
}

static QoreNode *SOCKET_setCharset(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("SOCKET-SET-CHARSET-PARAMETER-ERROR", "expecting charset name (string) as parameter of Socket::setCharset() call");
      return NULL;
   }
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);

   if (s)
   {
      s->setEncoding(QEM.findCreate(p0->val.String));
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Socket::setCharset");

   return NULL; 
}

static QoreNode *SOCKET_getCharset(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode(s->getEncoding() ? s->getEncoding()->code : "(unknown)");
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::getCharset");
   }
   return rv;
}

static QoreNode *SOCKET_isDataAvailable(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv, *p0;

   p0 = get_param(params, 0);

   int to;
   if (p0)
      to = p0->getAsInt();
   else
      to = 0;

   if (s)
   {
      rv = new QoreNode(s->isDataAvailable(to));
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::isDataAvailable");
   }
   return rv;
}

static QoreNode *SOCKET_getSSLCipherName(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      const char *str = s->getSSLCipherName();
      rv = str ? new QoreNode(str) : NULL;
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::getSSLCipherName");
   }
   return rv;
}

static QoreNode *SOCKET_getSSLCipherVersion(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      const char *str = s->getSSLCipherVersion();
      rv = str ? new QoreNode(str) : NULL;
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::getSSLCipherVersion");
   }
   return rv;
}

static QoreNode *SOCKET_isSecure(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode(s->isSecure());
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::isSecure");
   }
   return rv;
}

static QoreNode *SOCKET_verifyPeerCertificate(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode(s->verifyPeerCertificate());
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::verifyPeerCertificate");
   }
   return rv;
}

static QoreNode *SOCKET_setCertificate(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);

   if (s)
   {
      s->setCertificate(cert);
      s->deref();
   }
   else
   {
      cert->deref();
      alreadyDeleted(xsink, "Socket::setCertificate");
   }
   return NULL;
}

// syntax: object|string
static QoreNode *SOCKET_setPrivateKey(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // first check parameters
   QoreNode *p0 = get_param(params, 0);
   class QoreSSLPrivateKey *pk;

   if (p0 && p0->type == NT_STRING)
   {
      // try and create object
      pk = new QoreSSLPrivateKey(p0->val.String->getBuffer(), xsink);
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

   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);

   if (s)
   {
      s->setPrivateKey(pk);
      s->deref();
   }
   else
   {
      pk->deref();
      alreadyDeleted(xsink, "Socket::setPrivateKey");
   }
   return NULL;
}

class QoreClass *initSocketClass()
{
   tracein("initSocketClass()");

   class QoreClass *QC_SOCKET = new QoreClass(strdup("Socket"));
   CID_SOCKET = QC_SOCKET->getID();
   QC_SOCKET->addMethod("constructor",           SOCKET_constructor);
   QC_SOCKET->addMethod("destructor",            SOCKET_destructor);
   QC_SOCKET->addMethod("copy",                  SOCKET_constructor);
   QC_SOCKET->addMethod("connect",               SOCKET_connect);
   QC_SOCKET->addMethod("connectSSL",            SOCKET_connectSSL);
   QC_SOCKET->addMethod("bind",                  SOCKET_bind);
   QC_SOCKET->addMethod("accept",                SOCKET_accept);
   QC_SOCKET->addMethod("acceptSSL",             SOCKET_acceptSSL);
   QC_SOCKET->addMethod("listen",                SOCKET_listen);
   QC_SOCKET->addMethod("send",                  SOCKET_send);
   QC_SOCKET->addMethod("sendBinary",            SOCKET_sendBinary);
   QC_SOCKET->addMethod("sendi1",                SOCKET_sendi1);
   QC_SOCKET->addMethod("sendi2",                SOCKET_sendi2);
   QC_SOCKET->addMethod("sendi4",                SOCKET_sendi4);
   QC_SOCKET->addMethod("sendi8",                SOCKET_sendi8);
   QC_SOCKET->addMethod("sendi2LSB",             SOCKET_sendi2LSB);
   QC_SOCKET->addMethod("sendi4LSB",             SOCKET_sendi4LSB);
   QC_SOCKET->addMethod("sendi8LSB",             SOCKET_sendi8LSB);
   QC_SOCKET->addMethod("recv",                  SOCKET_recv);
   QC_SOCKET->addMethod("recvBinary",            SOCKET_recvBinary);
   QC_SOCKET->addMethod("recvi1",                SOCKET_recvi1);
   QC_SOCKET->addMethod("recvi2",                SOCKET_recvi2);
   QC_SOCKET->addMethod("recvi4",                SOCKET_recvi4);
   QC_SOCKET->addMethod("recvi8",                SOCKET_recvi8);
   QC_SOCKET->addMethod("recvi2LSB",             SOCKET_recvi2LSB);
   QC_SOCKET->addMethod("recvi4LSB",             SOCKET_recvi4LSB);
   QC_SOCKET->addMethod("recvi8LSB",             SOCKET_recvi8LSB);
   QC_SOCKET->addMethod("sendHTTPMessage",       SOCKET_sendHTTPMessage);
   QC_SOCKET->addMethod("sendHTTPResponse",      SOCKET_sendHTTPResponse);
   QC_SOCKET->addMethod("readHTTPHeader",        SOCKET_readHTTPHeader);
   QC_SOCKET->addMethod("getPort",               SOCKET_getPort);
   QC_SOCKET->addMethod("close",                 SOCKET_close);
   QC_SOCKET->addMethod("shutdown",              SOCKET_shutdown);
   QC_SOCKET->addMethod("shutdownSSL",           SOCKET_shutdownSSL);
   QC_SOCKET->addMethod("getSocket",             SOCKET_getSocket);
   QC_SOCKET->addMethod("setSendTimeout",        SOCKET_setSendTimeout);
   QC_SOCKET->addMethod("setRecvTimeout",        SOCKET_setRecvTimeout);
   QC_SOCKET->addMethod("getSendTimeout",        SOCKET_getSendTimeout);
   QC_SOCKET->addMethod("getRecvTimeout",        SOCKET_getRecvTimeout);
   QC_SOCKET->addMethod("getCharset",            SOCKET_getCharset);
   QC_SOCKET->addMethod("setCharset",            SOCKET_setCharset);
   QC_SOCKET->addMethod("isDataAvailable",       SOCKET_isDataAvailable);
   QC_SOCKET->addMethod("getSSLCipherName",      SOCKET_getSSLCipherName);
   QC_SOCKET->addMethod("getSSLCipherVersion",   SOCKET_getSSLCipherVersion);
   QC_SOCKET->addMethod("isSecure",              SOCKET_isSecure);
   QC_SOCKET->addMethod("verifyPeerCertificate", SOCKET_verifyPeerCertificate);
   QC_SOCKET->addMethod("setCertificate",        SOCKET_setCertificate);
   QC_SOCKET->addMethod("setPrivateKey",         SOCKET_setPrivateKey);

   traceout("initSocketClass()");
   return QC_SOCKET;
}
