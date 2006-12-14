/*
 mySocket.cc
 
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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QC_Socket.h>

void mySocket::init()
{
   cert = NULL;
   pk = NULL;
}

mySocket::mySocket(class QoreSocket *s)
{
   socket = s;
   init();
}

mySocket::~mySocket()
{
   if (cert)
      cert->deref();
   if (pk)
      pk->deref();
   
   delete socket;
}

mySocket::mySocket()
{
   socket = new QoreSocket();
   init();
}

int mySocket::connect(char *name, class ExceptionSink *xsink)
{
   lock();
   int rc = socket->connect(name, xsink);
   unlock();
   return rc;
}

int mySocket::connectINET(char *host, int port, class ExceptionSink *xsink)
{
   lock();
   int rc = socket->connectINET(host, port, xsink);
   unlock();
   return rc;
}

int mySocket::connectUNIX(char *p, class ExceptionSink *xsink)
{
   lock();
   int rc = socket->connectUNIX(p, xsink);
   unlock();
   return rc;
}

// to bind to either a UNIX socket or an INET interface:port
int mySocket::bind(char *name, bool reuseaddr)
{
   lock();
   int rc = socket->bind(name, reuseaddr);
   unlock();
   return rc;
}

// to bind to an INET tcp port on all interfaces
int mySocket::bind(int port, bool reuseaddr)
{
   lock();
   int rc = socket->bind(port, reuseaddr);
   unlock();
   return rc;
}

// to bind an open socket to an INET tcp port on a specific interface
int mySocket::bind(char *interface, int port, bool reuseaddr)
{
   lock();
   int rc = socket->bind(interface, port, reuseaddr);
   unlock();
   return rc;
}

// get port number for INET sockets
int mySocket::getPort()
{
   lock();
   int rc = socket->getPort();
   unlock();
   return rc;
}

class mySocket *mySocket::accept(class SocketSource *source, class ExceptionSink *xsink)
{
   lock();
   QoreSocket *s = socket->accept(source, xsink);
   unlock();
   if (s)
      return new mySocket(s);
   return NULL;
}

int mySocket::listen()
{
   lock();
   int rc = socket->listen();
   unlock();
   return rc;
}

// send a buffer of a particular size
int mySocket::send(char *buf, int size)
{
   lock();
   int rc = socket->send(buf, size);
   unlock();
   return rc;
}

// send a null-terminated string
int mySocket::send(class QoreString *msg, class ExceptionSink *xsink)
{
   lock();
   int rc = socket->send(msg, xsink);
   unlock();
   return rc;
}

// send a binary object
int mySocket::send(class BinaryObject *b)
{
   lock();
   int rc = socket->send(b);
   unlock();
   return rc;
}

// send from a file descriptor
int mySocket::send(int fd, int size)
{
   lock();
   int rc = socket->send(fd, size);
   unlock();
   return rc;
}

// send bytes and convert to network order
int mySocket::sendi1(char b)
{
   lock();
   int rc = socket->sendi1(b);
   unlock();
   return rc;
}

int mySocket::sendi2(short b)
{
   lock();
   int rc = socket->sendi2(b);
   unlock();
   return rc;
}

int mySocket::sendi4(int b)
{
   lock();
   int rc = socket->sendi4(b);
   unlock();
   return rc;
}

int mySocket::sendi8(int64 b)
{
   lock();
   int rc = socket->sendi8(b);
   unlock();
   return rc;
}

int mySocket::sendi2LSB(short b)
{
   lock();
   int rc = socket->sendi2LSB(b);
   unlock();
   return rc;
}

int mySocket::sendi4LSB(int b)
{
   lock();
   int rc = socket->sendi4LSB(b);
   unlock();
   return rc;
}

int mySocket::sendi8LSB(int64 b)
{
   lock();
   int rc = socket->sendi8LSB(b);
   unlock();
   return rc;
}

// receive a certain number of bytes as a string
class QoreString *mySocket::recv(int bufsize, int timeout, int *rc)
{
   lock();
   class QoreString *str = socket->recv(bufsize, timeout, rc);
   unlock();
   return str;
}

// receive a certain number of bytes as a binary object
class BinaryObject *mySocket::recvBinary(int bufsize, int timeout, int *rc)
{
   lock();
   class BinaryObject *b = socket->recvBinary(bufsize, timeout, rc);
   unlock();
   return b;
}

// receive a message
class QoreString *mySocket::recv(int timeout, int *rc)
{
   lock();
   class QoreString *str = socket->recv(timeout, rc);
   unlock();
   return str;
}

// receive and write data to a file descriptor
int mySocket::recv(int fd, int size, int timeout)
{
   lock();
   int rc = socket->recv(fd, size, timeout);
   unlock();
   return rc;
}

// receive integers and convert from network byte order
int mySocket::recvi1(int timeout, char *b)
{
   lock();
   int rc = socket->recvi1(timeout, b);
   unlock();
   return rc;
}

int mySocket::recvi2(int timeout, short *b)
{
   lock();
   int rc = socket->recvi2(timeout, b);
   unlock();
   return rc;
}

int mySocket::recvi4(int timeout, int *b)
{
   lock();
   int rc = socket->recvi4(timeout, b);
   unlock();
   return rc;
}

int mySocket::recvi8(int timeout, int64 *b)
{
   lock();
   int rc = socket->recvi8(timeout, b);
   unlock();
   return rc;
}

int mySocket::recvi2LSB(int timeout, short *b)
{
   lock();
   int rc = socket->recvi2LSB(timeout, b);
   unlock();
   return rc;
}

int mySocket::recvi4LSB(int timeout, int *b)
{
   lock();
   int rc = socket->recvi4LSB(timeout, b);
   unlock();
   return rc;
}

int mySocket::recvi8LSB(int timeout, int64 *b)
{
   lock();
   int rc = socket->recvi8LSB(timeout, b);
   unlock();
   return rc;
}

// send HTTP message
int mySocket::sendHTTPMessage(char *method, char *path, char *http_version, class Hash *headers, void *ptr, int size)
{
   lock();
   int rc = socket->sendHTTPMessage(method, path, http_version, headers, ptr, size);
   unlock();
   return rc;
}

// send HTTP response
int mySocket::sendHTTPResponse(int code, char *desc, char *http_version, class Hash *headers, void *ptr, int size)
{
   lock();
   int rc = socket->sendHTTPResponse(code, desc, http_version, headers, ptr, size);
   unlock();
   return rc;
}

// read and parse HTTP header
class QoreNode *mySocket::readHTTPHeader(int timeout, int *rc)
{
   lock();
   class QoreNode *n = socket->readHTTPHeader(timeout, rc);
   unlock();
   return n;
}

int mySocket::setSendTimeout(int ms)
{
   lock();
   int rc = socket->setSendTimeout(ms);
   unlock();
   return rc;
}

int mySocket::setRecvTimeout(int ms)
{
   lock();
   int rc = socket->setRecvTimeout(ms);
   unlock();
   return rc;
}

int mySocket::getSendTimeout()
{
   lock();
   int rc = socket->getSendTimeout();
   unlock();
   return rc;
}

int mySocket::getRecvTimeout()
{
   lock();
   int rc = socket->getRecvTimeout();
   unlock();
   return rc;
}

int mySocket::close() 
{ 
   lock();
   int rc = socket->close();
   unlock();
   return rc;
}

int mySocket::shutdown() 
{ 
   lock();
   int rc = socket->shutdown();
   unlock();
   return rc;
}

int mySocket::shutdownSSL(class ExceptionSink *xsink) 
{ 
   lock();
   int rc = socket->shutdownSSL(xsink);
   unlock();
   return rc;
}

const char *mySocket::getSSLCipherName()
{ 
   lock();
   const char *str = socket->getSSLCipherName();
   unlock();
   return str;
}

const char *mySocket::getSSLCipherVersion()
{ 
   lock();
   const char *str = socket->getSSLCipherVersion();
   unlock();
   return str;
}

bool mySocket::isSecure()
{
   lock();
   bool rc = socket->isSecure();
   unlock();
   return rc;
}

long mySocket::verifyPeerCertificate()
{
   lock();
   long rc = socket->verifyPeerCertificate();
   unlock();
   return rc;
}

int mySocket::getSocket()
{
   lock();
   int rc = socket->getSocket();
   unlock();
   return rc;
}

void mySocket::setEncoding(class QoreEncoding *id)
{
   socket->setEncoding(id);
}

class QoreEncoding *mySocket::getEncoding() const
{
   return socket->getEncoding();
}

bool mySocket::isDataAvailable(int timeout)
{
   lock();
   bool b = socket->isDataAvailable(timeout);
   unlock();
   return b;
}

bool mySocket::isOpen() const
{
   return socket->isOpen();
}

int mySocket::connectINETSSL(char *host, int port, class ExceptionSink *xsink)
{
   lock();
   int rc = socket->connectINETSSL(host, port, 
				   cert ? cert->getData() : NULL,
				   pk ? pk->getData() : NULL,
				   xsink);
   unlock();
   return rc;
}

int mySocket::connectUNIXSSL(char *p, class ExceptionSink *xsink)
{
   lock();
   int rc = socket->connectUNIXSSL(p, 
				   cert ? cert->getData() : NULL,
				   pk ? pk->getData() : NULL,
				   xsink);
   unlock();
   return rc;
}

int mySocket::connectSSL(char *name, class ExceptionSink *xsink)
{
   lock();
   int rc = socket->connectSSL(name, 
			       cert ? cert->getData() : NULL,
			       pk ? pk->getData() : NULL,
			       xsink);
   unlock();
   return rc;
}

class mySocket *mySocket::acceptSSL(class SocketSource *source, class ExceptionSink *xsink)
{
   lock();
   QoreSocket *s = socket->acceptSSL(source,
				     cert ? cert->getData() : NULL, 
				     pk ? pk->getData() : NULL, xsink);
   unlock();
   if (s)
      return new mySocket(s);
   return NULL;
}

// c must be already referenced before this call
void mySocket::setCertificate(class QoreSSLCertificate *c)
{
   lock();
   if (cert)
      cert->deref();
   cert = c;
   unlock();
}

// p must be already referenced before this call
void mySocket::setPrivateKey(class QoreSSLPrivateKey *p)
{
   lock();
   if (pk)
      pk->deref();
   pk = p;
   unlock();
}
