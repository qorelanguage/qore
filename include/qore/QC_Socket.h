/*
  QC_Socket.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  provides a thread-safe interface to the QoreSocket object

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

#ifndef _QORE_CLASS_SOCKET_H

#define _QORE_CLASS_SOCKET_H

class QoreClass *initSocketClass();

extern int CID_SOCKET;

#include <qore/QoreSocket.h>
#include <qore/ReferenceObject.h>
#include <qore/LockedObject.h>

class mySocket : public ReferenceObject, public LockedObject
{
   private:
      class QoreSocket *socket;

      inline mySocket(class QoreSocket *s)
      {
	 socket = s;
      }

   protected:
      inline ~mySocket()
      {
	 delete socket;
      }

   public:
      inline mySocket()
      {
	 socket = new QoreSocket();
      }
      
      inline int connect(char *name, class ExceptionSink *xsink = NULL)
      {
	 lock();
	 int rc = socket->connect(name, xsink);
	 unlock();
	 return rc;
      }

      inline int connectINET(char *host, int port, class ExceptionSink *xsink = NULL)
      {
	 lock();
	 int rc = socket->connectINET(host, port, xsink);
	 unlock();
	 return rc;
      }

      inline int connectUNIX(char *p, class ExceptionSink *xsink = NULL)
      {
	 lock();
	 int rc = socket->connectUNIX(p, xsink);
	 unlock();
	 return rc;
      }

      inline int connectSSL(char *name, class ExceptionSink *xsink)
      {
	 lock();
	 int rc = socket->connectSSL(name, xsink);
	 unlock();
	 return rc;
      }

      inline int connectINETSSL(char *host, int port, class ExceptionSink *xsink)
      {
	 lock();
	 int rc = socket->connectINETSSL(host, port, xsink);
	 unlock();
	 return rc;
      }

      inline int connectUNIXSSL(char *p, class ExceptionSink *xsink)
      {
	 lock();
	 int rc = socket->connectUNIXSSL(p, xsink);
	 unlock();
	 return rc;
      }

      // to bind to either a UNIX socket or an INET interface:port
      inline int bind(char *name, bool reuseaddr = false)
      {
	 lock();
	 int rc = socket->bind(name, reuseaddr);
	 unlock();
	 return rc;
      }

      // to bind to an INET tcp port on all interfaces
      inline int bind(int port, bool reuseaddr = false)
      {
	 lock();
	 int rc = socket->bind(port, reuseaddr);
	 unlock();
	 return rc;
      }

      // to bind an open socket to an INET tcp port on a specific interface
      inline int bind(char *interface, int port, bool reuseaddr = false)
      {
	 lock();
	 int rc = socket->bind(interface, port, reuseaddr);
	 unlock();
	 return rc;
      }

      // get port number for INET sockets
      inline int getPort()
      {
	 lock();
	 int rc = socket->getPort();
	 unlock();
	 return rc;
      }

      inline class mySocket *accept(class QoreString *source, class ExceptionSink *xsink = NULL)
      {
	 lock();
	 QoreSocket *s = socket->accept(source, xsink);
	 unlock();
	 if (s)
	    return new mySocket(s);
	 return NULL;
      }

      inline class mySocket *acceptSSL(class QoreString *source, class ExceptionSink *xsink)
      {
	 lock();
	 QoreSocket *s = socket->acceptSSL(source, xsink);
	 unlock();
	 if (s)
	    return new mySocket(s);
	 return NULL;
      }

      inline int listen()
      {
	 lock();
	 int rc = socket->listen();
	 unlock();
	 return rc;
      }

      // send a buffer of a particular size
      inline int send(char *buf, int size)
      {
	 lock();
	 int rc = socket->send(buf, size);
	 unlock();
	 return rc;
      }

      // send a null-terminated string
      inline int send(class QoreString *msg, class ExceptionSink *xsink)
      {
	 lock();
	 int rc = socket->send(msg, xsink);
	 unlock();
	 return rc;
      }

      // send a binary object
      inline int send(class BinaryObject *b)
      {
	 lock();
	 int rc = socket->send(b);
	 unlock();
	 return rc;
      }

      // send from a file descriptor
      inline int send(int fd, int size = -1)
      {
	 lock();
	 int rc = socket->send(fd, size);
	 unlock();
	 return rc;
      }

      // send bytes and convert to network order
      inline int sendi1(char b)
      {
	 lock();
	 int rc = socket->sendi1(b);
	 unlock();
	 return rc;
      }
      inline int sendi2(short b)
      {
	 lock();
	 int rc = socket->sendi2(b);
	 unlock();
	 return rc;
      }
      inline int sendi4(int b)
      {
	 lock();
	 int rc = socket->sendi4(b);
	 unlock();
	 return rc;
      }
      inline int sendi8(int64 b)
      {
	 lock();
	 int rc = socket->sendi8(b);
	 unlock();
	 return rc;
      }
      inline int sendi2LSB(short b)
      {
	 lock();
	 int rc = socket->sendi2LSB(b);
	 unlock();
	 return rc;
      }
      inline int sendi4LSB(int b)
      {
	 lock();
	 int rc = socket->sendi4LSB(b);
	 unlock();
	 return rc;
      }
      inline int sendi8LSB(int64 b)
      {
	 lock();
	 int rc = socket->sendi8LSB(b);
	 unlock();
	 return rc;
      }

      // receive a certain number of bytes as a string
      inline class QoreString *recv(int bufsize, int timeout, int *rc)
      {
	 lock();
	 class QoreString *str = socket->recv(bufsize, timeout, rc);
	 unlock();
	 return str;
      }

      // receive a certain number of bytes as a binary object
      inline class BinaryObject *recvBinary(int bufsize, int timeout, int *rc)
      {
	 lock();
	 class BinaryObject *b = socket->recvBinary(bufsize, timeout, rc);
	 unlock();
	 return b;
      }

      // receive a message
      inline class QoreString *recv(int timeout, int *rc)
      {
	 lock();
	 class QoreString *str = socket->recv(timeout, rc);
	 unlock();
	 return str;
      }

      // receive and write data to a file descriptor
      inline int recv(int fd, int size, int timeout)
      {
	 lock();
	 int rc = socket->recv(fd, size, timeout);
	 unlock();
	 return rc;
      }

      // receive integers and convert from network byte order
      inline int recvi1(int timeout, char *b)
      {
	 lock();
	 int rc = socket->recvi1(timeout, b);
	 unlock();
	 return rc;
      }
      inline int recvi2(int timeout, short *b)
      {
	 lock();
	 int rc = socket->recvi2(timeout, b);
	 unlock();
	 return rc;
      }
      inline int recvi4(int timeout, int *b)
      {
	 lock();
	 int rc = socket->recvi4(timeout, b);
	 unlock();
	 return rc;
      }
      inline int recvi8(int timeout, int64 *b)
      {
	 lock();
	 int rc = socket->recvi8(timeout, b);
	 unlock();
	 return rc;
      }
      inline int recvi2LSB(int timeout, short *b)
      {
	 lock();
	 int rc = socket->recvi2LSB(timeout, b);
	 unlock();
	 return rc;
      }
      inline int recvi4LSB(int timeout, int *b)
      {
	 lock();
	 int rc = socket->recvi4LSB(timeout, b);
	 unlock();
	 return rc;
      }
      inline int recvi8LSB(int timeout, int64 *b)
      {
	 lock();
	 int rc = socket->recvi8LSB(timeout, b);
	 unlock();
	 return rc;
      }

      // send HTTP message
      inline int sendHTTPMessage(char *method, char *path, char *http_version, class Hash *headers, void *ptr, int size)
      {
	 lock();
	 int rc = socket->sendHTTPMessage(method, path, http_version, headers, ptr, size);
	 unlock();
	 return rc;
      }

      // send HTTP response
      inline int sendHTTPResponse(int code, char *desc, char *http_version, class Hash *headers, void *ptr, int size)
      {
	 lock();
	 int rc = socket->sendHTTPResponse(code, desc, http_version, headers, ptr, size);
	 unlock();
	 return rc;
      }

      // read and parse HTTP header
      inline class QoreNode *readHTTPHeader(int timeout, int *rc)
      {
	 lock();
	 class QoreNode *n = socket->readHTTPHeader(timeout, rc);
	 unlock();
	 return n;
      }

      inline int setSendTimeout(int ms)
      {
	 lock();
	 int rc = socket->setSendTimeout(ms);
	 unlock();
	 return rc;
      }

      inline int setRecvTimeout(int ms)
      {
	 lock();
	 int rc = socket->setRecvTimeout(ms);
	 unlock();
	 return rc;
      }

      inline int getSendTimeout()
      {
	 lock();
	 int rc = socket->getSendTimeout();
	 unlock();
	 return rc;
      }

      inline int getRecvTimeout()
      {
	 lock();
	 int rc = socket->getRecvTimeout();
	 unlock();
	 return rc;
      }

      inline int close() 
      { 
	 lock();
	 int rc = socket->close();
	 unlock();
	 return rc;
      }

      inline int shutdown() 
      { 
	 lock();
	 int rc = socket->shutdown();
	 unlock();
	 return rc;
      }

      inline int shutdownSSL(class ExceptionSink *xsink) 
      { 
	 lock();
	 int rc = socket->shutdownSSL(xsink);
	 unlock();
	 return rc;
      }

      inline const char *getSSLCipherName() 
      { 
	 lock();
	 const char *str = socket->getSSLCipherName();
	 unlock();
	 return str;
      }

      inline const char *getSSLCipherVersion() 
      { 
	 lock();
	 const char *str = socket->getSSLCipherVersion();
	 unlock();
	 return str;
      }

      inline bool isSecure()
      {
	 lock();
	 bool rc = socket->isSecure();
	 unlock();
	 return rc;
      }

      inline int getSocket()
      {
	 lock();
	 int rc = socket->getSocket();
	 unlock();
	 return rc;
      }

      inline void setEncoding(class QoreEncoding *id)
      {
	 socket->setEncoding(id);
      }

      inline class QoreEncoding *getEncoding()
      {
	 return socket->getEncoding();
      }
      
      inline bool isDataAvailable(int timeout = 0)
      {
	 lock();
	 bool b = socket->isDataAvailable(timeout);
	 unlock();
	 return b;
      }

      inline void deref();
};

inline void mySocket::deref()
{
   if (ROdereference())
      delete this;
}

#endif // _QORE_CLASS_QORESOCKET_H
