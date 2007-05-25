/*
 mySocket.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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

#include <qore/Qore.h>
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

int mySocket::connect(const char *name, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   return socket->connect(name, xsink);
}

int mySocket::connectINET(const char *host, int port, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   return socket->connectINET(host, port, xsink);
}

int mySocket::connectUNIX(const char *p, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   return socket->connectUNIX(p, xsink);
}

// to bind to either a UNIX socket or an INET interface:port
int mySocket::bind(const char *name, bool reuseaddr)
{
   SafeLocker sl(this);
   return socket->bind(name, reuseaddr);
}

// to bind to an INET tcp port on all interfaces
int mySocket::bind(int port, bool reuseaddr)
{
   SafeLocker sl(this);
   return socket->bind(port, reuseaddr);
}

// to bind an open socket to an INET tcp port on a specific interface
int mySocket::bind(const char *interface, int port, bool reuseaddr)
{
   SafeLocker sl(this);
   return socket->bind(interface, port, reuseaddr);
}

// get port number for INET sockets
int mySocket::getPort()
{
   SafeLocker sl(this);
   return socket->getPort();
}

class mySocket *mySocket::accept(class SocketSource *source, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   QoreSocket *s = socket->accept(source, xsink);
   sl.unlock();
   if (s)
      return new mySocket(s);
   return NULL;
}

int mySocket::listen()
{
   SafeLocker sl(this);
   return socket->listen();
}

// send a buffer of a particular size
int mySocket::send(const char *buf, int size)
{
   SafeLocker sl(this);
   return socket->send(buf, size);
}

// send a null-terminated string
int mySocket::send(class QoreString *msg, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   return socket->send(msg, xsink);
}

// send a binary object
int mySocket::send(class BinaryObject *b)
{
   SafeLocker sl(this);
   return socket->send(b);
}

// send from a file descriptor
int mySocket::send(int fd, int size)
{
   SafeLocker sl(this);
   return socket->send(fd, size);
}

// send bytes and convert to network order
int mySocket::sendi1(char b)
{
   SafeLocker sl(this);
   return socket->sendi1(b);
}

int mySocket::sendi2(short b)
{
   SafeLocker sl(this);
   return socket->sendi2(b);
}

int mySocket::sendi4(int b)
{
   SafeLocker sl(this);
   return socket->sendi4(b);
}

int mySocket::sendi8(int64 b)
{
   SafeLocker sl(this);
   return socket->sendi8(b);
}

int mySocket::sendi2LSB(short b)
{
   SafeLocker sl(this);
   return socket->sendi2LSB(b);
}

int mySocket::sendi4LSB(int b)
{
   SafeLocker sl(this);
   return socket->sendi4LSB(b);
}

int mySocket::sendi8LSB(int64 b)
{
   SafeLocker sl(this);
   return socket->sendi8LSB(b);
}

// receive a certain number of bytes as a string
class QoreString *mySocket::recv(int bufsize, int timeout, int *rc)
{
   SafeLocker sl(this);
   return socket->recv(bufsize, timeout, rc);
}

// receive a certain number of bytes as a binary object
class BinaryObject *mySocket::recvBinary(int bufsize, int timeout, int *rc)
{
   SafeLocker sl(this);
   return socket->recvBinary(bufsize, timeout, rc);
}

// receive a message
class QoreString *mySocket::recv(int timeout, int *rc)
{
   SafeLocker sl(this);
   return socket->recv(timeout, rc);
}

// receive and write data to a file descriptor
int mySocket::recv(int fd, int size, int timeout)
{
   SafeLocker sl(this);
   return socket->recv(fd, size, timeout);
}

// receive integers and convert from network byte order
int mySocket::recvi1(int timeout, char *b)
{
   SafeLocker sl(this);
   return socket->recvi1(timeout, b);
}

int mySocket::recvi2(int timeout, short *b)
{
   SafeLocker sl(this);
   return socket->recvi2(timeout, b);
}

int mySocket::recvi4(int timeout, int *b)
{
   SafeLocker sl(this);
   return socket->recvi4(timeout, b);
}

int mySocket::recvi8(int timeout, int64 *b)
{
   SafeLocker sl(this);
   return socket->recvi8(timeout, b);
}

int mySocket::recvi2LSB(int timeout, short *b)
{
   SafeLocker sl(this);
   return socket->recvi2LSB(timeout, b);
}

int mySocket::recvi4LSB(int timeout, int *b)
{
   SafeLocker sl(this);
   return socket->recvi4LSB(timeout, b);
}

int mySocket::recvi8LSB(int timeout, int64 *b)
{
   SafeLocker sl(this);
   return socket->recvi8LSB(timeout, b);
}

// receive integers and convert from network byte order
int mySocket::recvu1(int timeout, unsigned char *b)
{
   SafeLocker sl(this);
   return socket->recvu1(timeout, b);
}

int mySocket::recvu2(int timeout, unsigned short *b)
{
   SafeLocker sl(this);
   return socket->recvu2(timeout, b);
}

int mySocket::recvu4(int timeout, unsigned int *b)
{
   SafeLocker sl(this);
   return socket->recvu4(timeout, b);
}

int mySocket::recvu2LSB(int timeout, unsigned short *b)
{
   SafeLocker sl(this);
   return socket->recvu2LSB(timeout, b);
}

int mySocket::recvu4LSB(int timeout, unsigned int *b)
{
   SafeLocker sl(this);
   return socket->recvu4LSB(timeout, b);
}

// send HTTP message
int mySocket::sendHTTPMessage(const char *method, const char *path, const char *http_version, class Hash *headers, const void *ptr, int size)
{
   SafeLocker sl(this);
   return socket->sendHTTPMessage(method, path, http_version, headers, ptr, size);
}

// send HTTP response
int mySocket::sendHTTPResponse(int code, const char *desc, const char *http_version, class Hash *headers, const void *ptr, int size)
{
   SafeLocker sl(this);
   return socket->sendHTTPResponse(code, desc, http_version, headers, ptr, size);
}

// receive a binary message in HTTP chunked format
class Hash *mySocket::readHTTPChunkedBodyBinary(int timeout, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   return socket->readHTTPChunkedBodyBinary(timeout, xsink);
}

// receive a string message in HTTP chunked format
class Hash *mySocket::readHTTPChunkedBody(int timeout, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   return socket->readHTTPChunkedBody(timeout, xsink);
}

// read and parse HTTP header
class QoreNode *mySocket::readHTTPHeader(int timeout, int *rc)
{
   SafeLocker sl(this);
   return socket->readHTTPHeader(timeout, rc);
}

int mySocket::setSendTimeout(int ms)
{
   SafeLocker sl(this);
   return socket->setSendTimeout(ms);
}

int mySocket::setRecvTimeout(int ms)
{
   SafeLocker sl(this);
   return socket->setRecvTimeout(ms);
}

int mySocket::getSendTimeout()
{
   SafeLocker sl(this);
   return socket->getSendTimeout();
}

int mySocket::getRecvTimeout()
{
   SafeLocker sl(this);
   return socket->getRecvTimeout();
}

int mySocket::close() 
{ 
   SafeLocker sl(this);
   return socket->close();
}

int mySocket::shutdown() 
{ 
   SafeLocker sl(this);
   return socket->shutdown();
}

int mySocket::shutdownSSL(class ExceptionSink *xsink) 
{ 
   SafeLocker sl(this);
   return socket->shutdownSSL(xsink);
}

const char *mySocket::getSSLCipherName()
{ 
   SafeLocker sl(this);
   return socket->getSSLCipherName();
}

const char *mySocket::getSSLCipherVersion()
{ 
   SafeLocker sl(this);
   return socket->getSSLCipherVersion();
}

bool mySocket::isSecure()
{
   SafeLocker sl(this);
   return socket->isSecure();
}

long mySocket::verifyPeerCertificate()
{
   SafeLocker sl(this);
   return socket->verifyPeerCertificate();
}

int mySocket::getSocket()
{
   SafeLocker sl(this);
   return socket->getSocket();
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
   SafeLocker sl(this);
   return socket->isDataAvailable(timeout);
}

bool mySocket::isOpen() const
{
   return socket->isOpen();
}

int mySocket::connectINETSSL(const char *host, int port, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   return socket->connectINETSSL(host, port, 
				 cert ? cert->getData() : NULL,
				 pk ? pk->getData() : NULL,
				 xsink);
}

int mySocket::connectUNIXSSL(const char *p, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   return socket->connectUNIXSSL(p, 
				 cert ? cert->getData() : NULL,
				 pk ? pk->getData() : NULL,
				 xsink);
}

int mySocket::connectSSL(const char *name, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   return socket->connectSSL(name, 
			     cert ? cert->getData() : NULL,
			     pk ? pk->getData() : NULL,
			     xsink);
}

class mySocket *mySocket::acceptSSL(class SocketSource *source, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   QoreSocket *s = socket->acceptSSL(source,
				     cert ? cert->getData() : NULL, 
				     pk ? pk->getData() : NULL, xsink);
   sl.unlock();
   if (s)
      return new mySocket(s);
   return NULL;
}

// c must be already referenced before this call
void mySocket::setCertificate(class QoreSSLCertificate *c)
{
   SafeLocker sl(this);
   if (cert)
      cert->deref();
   cert = c;
}

// p must be already referenced before this call
void mySocket::setPrivateKey(class QoreSSLPrivateKey *p)
{
   SafeLocker sl(this);
   if (pk)
      pk->deref();
   pk = p;
}
