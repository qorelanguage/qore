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
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode(NT_INT, s->connect(p0->val.String->getBuffer()));
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::connect");
   }
   return rv;
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
	 rv = new QoreNode(NT_INT, s->bind(p0->val.intval, rua));
      else
	 rv = new QoreNode(NT_INT, s->bind(p0->val.String->getBuffer(), rua));
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
// returns a new Socket object, connection source string is in $.source
// member of new object
static QoreNode *SOCKET_accept(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened and in listening state before Socket::accept() call");
	 rv = NULL;
      }
      else
      {
	 QoreString *source = new QoreString();
	 mySocket *n = s->accept(source);
	 if (!n)
	 {
	    delete source;
	    rv = NULL;
	 }
	 else
	 {
	    // ensure that a socket object is returned (and not a subclass)
	    Object *ns = new Object(self->getClass(CID_SOCKET), getProgram());
	    ns->setPrivate(CID_SOCKET, n, getSocket);
	    ns->setValue("source", new QoreNode(source), xsink);

	    rv = new QoreNode(ns);
	 }
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

static QoreNode *SOCKET_listen(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::listen() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode(NT_INT, s->listen());

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
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::send() call");
	 rv = NULL;
      }
      else
      {
	 if (p0->type == NT_STRING)
	    rv = new QoreNode(NT_INT, s->send(p0->val.String, xsink));
	 else
	    rv = new QoreNode(NT_INT, s->send(p0->val.bin));
      }
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
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendBinary() call");
	 rv = NULL;
      }
      else
      {
	 if (p0->type == NT_STRING)
	    rv = new QoreNode(NT_INT, s->send(p0->val.String->getBuffer(), p0->val.String->strlen()));
	 else
	    rv = new QoreNode(NT_INT, s->send(p0->val.bin));
      }
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
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::send() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode(NT_INT, s->sendi1(i));
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
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi2() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode(NT_INT, s->sendi2(i));
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
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi4() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode(NT_INT, s->sendi4(i));
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
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi8() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode(NT_INT, s->sendi8(i));
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
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi2LSB() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode(NT_INT, s->sendi2LSB(i));
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
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi4LSB() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode(NT_INT, s->sendi4LSB(i));
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
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendi8LSB() call");
	 rv = NULL;
      }
      else
	 rv = new QoreNode(NT_INT, s->sendi8LSB(i));
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
   QoreNode *rv;

   if (s)
   {
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::recv() call");
	 rv = NULL;
      }
      else
      {
	 int rc;
	 QoreString *msg;

	 if (bs)
	    msg = s->recv(bs, timeout, &rc);
	 else
	    msg = s->recv(timeout, &rc);
	 
	 if (!rc)             // remote end has closed the connection
	 {
	    xsink->raiseException("SOCKET-CLOSED", "remote end has closed the connection");
	    rv = NULL;
	 }
	 else if (rc == -1)   // recv() error
	 {
	    xsink->raiseException("SOCKET-RECV-ERROR", strerror(errno));
	    rv = NULL;
	 }
	 else if (rc == -2)   // TIMEOUT returns NOTHING
	    rv = NULL;
	 else                 // the value read
	    rv = new QoreNode(msg);
      }
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::recv");
   }
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
   QoreNode *rv;

   if (s)
   {
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::recv() call");
	 rv = NULL;
      }
      else
      {
	 char b;
	 int rc = s->recvi1(timeout, &b);
	 if (!rc)
	 {
	    xsink->raiseException("SOCKET-CLOSED", "remote end has closed the connection");
	    rv = NULL;
	 }
	 else if (rc == -1)
	 {
	    xsink->raiseException("SOCKET-RECV-ERROR", strerror(errno));
	    rv = NULL;
	 }
	 else if (rc == -2)   // TIMEOUT
	    rv = NULL;
	 else                 // the value read
	    rv = new QoreNode((int64)b);
      }
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::revci1");
   }
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
   QoreNode *rv;

   if (s)
   {
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::recv() call");
	 rv = NULL;
      }
      else
      {
	 short b;
	 int rc = s->recvi2(timeout, &b); 
	 if (!rc)             // remote end has closed the connection
	 {
	    xsink->raiseException("SOCKET-CLOSED", "remote end has closed the connection");
	    rv = NULL;
	 }
	 else if (rc == -1)   // recv() error
	 {
	    xsink->raiseException("SOCKET-RECV-ERROR", strerror(errno));
	    rv = NULL;
	 }
	 else if (rc == -2)   // TIMEOUT returns NOTHING
	    rv = NULL;
	 else                 // the value read
	    rv = new QoreNode((int64)b);
      }
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::recvi2");
   }
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
   QoreNode *rv;

   if (s)
   {
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::recv() call");
	 rv = NULL;
      }
      else
      {
	 int b;
	 int rc = s->recvi4(timeout, &b);
	 if (!rc)             // remote end has closed the connection
	 {
	    xsink->raiseException("SOCKET-CLOSED", "remote end has closed the connection");
	    rv = NULL;
	 }
	 else if (rc == -1)   // recv() error
	 {
	    xsink->raiseException("SOCKET-RECV-ERROR", strerror(errno));
	    rv = NULL;
	 }
	 else if (rc == -2)   // TIMEOUT returns NOTHING
	    rv = NULL;
	 else                 // the value read
	    rv = new QoreNode((int64)b);
      }
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::recvi4");
   }
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
   QoreNode *rv;

   if (s)
   {
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::recvi8() call");
	 rv = NULL;
      }
      else
      {
	 int64 b;
	 int rc = s->recvi8(timeout, &b);
	 if (!rc)             // remote end has closed the connection
	 {
	    xsink->raiseException("SOCKET-CLOSED", "remote end has closed the connection");
	    rv = NULL;
	 }
	 else if (rc == -1)   // recv() error
	 {
	    xsink->raiseException("SOCKET-RECV-ERROR", strerror(errno));
	    rv = NULL;
	 }
	 else if (rc == -2)   // TIMEOUT returns NOTHING
	    rv = NULL;
	 else                 // the value read
	    rv = new QoreNode(b);
      }
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::recvi8");
   }
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
   QoreNode *rv;

   if (s)
   {
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::recv() call");
	 rv = NULL;
      }
      else
      {
	 short b;
	 int rc = s->recvi2LSB(timeout, &b); 
	 if (!rc)             // remote end has closed the connection
	 {
	    xsink->raiseException("SOCKET-CLOSED", "remote end has closed the connection");
	    rv = NULL;
	 }
	 else if (rc == -1)   // recv() error
	 {
	    xsink->raiseException("SOCKET-RECV-ERROR", strerror(errno));
	    rv = NULL;
	 }
	 else if (rc == -2)   // TIMEOUT returns NOTHING
	    rv = NULL;
	 else                 // the value read
	    rv = new QoreNode((int64)b);
      }
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::recvi2LSB");
   }
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
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::recv() call");
	 rv = NULL;
      }
      else
      {
	 int b;
	 int rc = s->recvi4LSB(timeout, &b);
	 if (!rc)             // remote end has closed the connection
	 {
	    xsink->raiseException("SOCKET-CLOSED", "remote end has closed the connection");
	    rv = NULL;
	 }
	 else if (rc == -1)   // recv() error
	 {
	    xsink->raiseException("SOCKET-RECV-ERROR", strerror(errno));
	    rv = NULL;
	 }
	 else if (rc == -2)   // TIMEOUT returns NOTHING
	    rv = NULL;
	 else                 // the value read
	    rv = new QoreNode((int64)b);
      }
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
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::recvi8LSB() call");
	 rv = NULL;
      }
      else
      {
	 int64 b;
	 int rc = s->recvi8LSB(timeout, &b);
	 if (!rc)             // remote end has closed the connection
	 {
	    xsink->raiseException("SOCKET-CLOSED", "remote end has closed the connection");
	    rv = NULL;
	 }
	 else if (rc == -1)   // recv() error
	 {
	    xsink->raiseException("SOCKET-RECV-ERROR", strerror(errno));
	    rv = NULL;
	 }
	 else if (rc == -2)   // TIMEOUT returns NOTHING
	    rv = NULL;
	 else                 // the value read
	    rv = new QoreNode(b);
      }
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
   QoreNode *rv;

   if (s)
   {
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::recvBinary() call");
	 rv = NULL;
      }
      else
      {
	 int rc;
	 BinaryObject *b = s->recvBinary(bs, timeout, &rc);
	 if (!rc)             // remote end has closed the connection
	 {
	    xsink->raiseException("SOCKET-CLOSED", "remote end has closed the connection");
	    rv = NULL;
	 }
	 else if (rc == -1)   // recv() error
	 {
	    xsink->raiseException("SOCKET-RECV-ERROR", strerror(errno));
	    rv = NULL;
	 }
	 else if (rc == -2)   // TIMEOUT returns NOTHING
	    rv = NULL;
	 else                 // the value read
	    rv = new QoreNode(b);
      }
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::recvBinary");
   }
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
      if (!s->getSocket())
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendHTTPMessage() call");
      else
      {
	 int rc;
	 if ((rc = s->sendHTTPMessage(method, path, http_version, headers, ptr, size)))
	    xsink->raiseException("SOCKET-SEND-ERROR", "send failed with error code %d: %s", rc, strerror(errno));
      }
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
      if (!s->getSocket())
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::sendHTTPResponse() call");
      else
      {
	 int rc;
	 if ((rc = s->sendHTTPResponse(status_code, status_desc, http_version, headers, ptr, size)))
	    xsink->raiseException("SOCKET-SEND-ERROR", "send failed with error code %d: %s", rc, strerror(errno));
      }
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
   QoreNode *rv;

   if (s)
   {
      if (!s->getSocket())
      {
	 xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened before Socket::readHTTPHeader() call");
	 rv = NULL;
      }
      else
      {
	 int rc;

	 // when rc = -2 it's a timeout, but rv will be NULL anyway, so we do nothing
	 rv = s->readHTTPHeader(timeout, &rc);

	 if (!rc)             // remote end has closed the connection
	    xsink->raiseException("SOCKET-CLOSED", "remote end has closed the connection");
	 else if (rc == -1)   // recv() error
	    xsink->raiseException("SOCKET-RECV-ERROR", strerror(errno));
      }

      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Socket::readHTTPHeader");
   }
   return rv;
}

static QoreNode *SOCKET_close(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode(NT_INT, s->close());
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
      alreadyDeleted(xsink, "Socket::close");
   }
   return rv;
}

static QoreNode *SOCKET_getPort(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class mySocket *s = (mySocket *)self->getReferencedPrivateData(CID_SOCKET);
   QoreNode *rv;

   if (s)
   {
      rv = new QoreNode(NT_INT, s->getPort());
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
      rv = new QoreNode(NT_INT, s->getSocket());
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
      rv = new QoreNode(NT_INT, s->setSendTimeout(p0->getAsInt()));
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
      rv = new QoreNode(NT_INT, s->setRecvTimeout(p0->getAsInt()));
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
      rv = new QoreNode(NT_INT, s->getSendTimeout());
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
      rv = new QoreNode(NT_INT, s->getRecvTimeout());
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

class QoreClass *initSocketClass()
{
   tracein("initSocketClass()");

   class QoreClass *QC_SOCKET = new QoreClass(strdup("Socket"));
   CID_SOCKET = QC_SOCKET->getID();
   QC_SOCKET->addMethod("constructor",      SOCKET_constructor);
   QC_SOCKET->addMethod("destructor",       SOCKET_destructor);
   QC_SOCKET->addMethod("copy",             SOCKET_constructor);
   QC_SOCKET->addMethod("connect",          SOCKET_connect);
   QC_SOCKET->addMethod("bind",             SOCKET_bind);
   QC_SOCKET->addMethod("accept",           SOCKET_accept);
   QC_SOCKET->addMethod("listen",           SOCKET_listen);
   QC_SOCKET->addMethod("send",             SOCKET_send);
   QC_SOCKET->addMethod("sendBinary",       SOCKET_sendBinary);
   QC_SOCKET->addMethod("sendi1",           SOCKET_sendi1);
   QC_SOCKET->addMethod("sendi2",           SOCKET_sendi2);
   QC_SOCKET->addMethod("sendi4",           SOCKET_sendi4);
   QC_SOCKET->addMethod("sendi8",           SOCKET_sendi8);
   QC_SOCKET->addMethod("sendi2LSB",        SOCKET_sendi2LSB);
   QC_SOCKET->addMethod("sendi4LSB",        SOCKET_sendi4LSB);
   QC_SOCKET->addMethod("sendi8LSB",        SOCKET_sendi8LSB);
   QC_SOCKET->addMethod("recv",             SOCKET_recv);
   QC_SOCKET->addMethod("recvBinary",       SOCKET_recvBinary);
   QC_SOCKET->addMethod("recvi1",           SOCKET_recvi1);
   QC_SOCKET->addMethod("recvi2",           SOCKET_recvi2);
   QC_SOCKET->addMethod("recvi4",           SOCKET_recvi4);
   QC_SOCKET->addMethod("recvi8",           SOCKET_recvi8);
   QC_SOCKET->addMethod("recvi2LSB",        SOCKET_recvi2LSB);
   QC_SOCKET->addMethod("recvi4LSB",        SOCKET_recvi4LSB);
   QC_SOCKET->addMethod("recvi8LSB",        SOCKET_recvi8LSB);
   QC_SOCKET->addMethod("sendHTTPMessage",  SOCKET_sendHTTPMessage);
   QC_SOCKET->addMethod("sendHTTPResponse", SOCKET_sendHTTPResponse);
   QC_SOCKET->addMethod("readHTTPHeader",   SOCKET_readHTTPHeader);
   QC_SOCKET->addMethod("getPort",          SOCKET_getPort);
   QC_SOCKET->addMethod("close",            SOCKET_close);
   QC_SOCKET->addMethod("shutdown",         SOCKET_shutdown);
   QC_SOCKET->addMethod("getSocket",        SOCKET_getSocket);
   QC_SOCKET->addMethod("setSendTimeout",   SOCKET_setSendTimeout);
   QC_SOCKET->addMethod("setRecvTimeout",   SOCKET_setRecvTimeout);
   QC_SOCKET->addMethod("getSendTimeout",   SOCKET_getSendTimeout);
   QC_SOCKET->addMethod("getRecvTimeout",   SOCKET_getRecvTimeout);
   QC_SOCKET->addMethod("getCharset",       SOCKET_getCharset);
   QC_SOCKET->addMethod("setCharset",       SOCKET_setCharset);
   QC_SOCKET->addMethod("isDataAvailable",  SOCKET_isDataAvailable);

   traceout("initSocketClass()");
   return QC_SOCKET;
}
