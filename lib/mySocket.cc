/*
 mySocket.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
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
#include <qore/intern/QC_Socket.h>

mySocket::mySocket(QoreSocket *s) : socket(s), cert(0), pk(0) {
}

mySocket::mySocket() : socket(new QoreSocket()), cert(0), pk(0) {
}

mySocket::~mySocket() {
   if (cert)
      cert->deref();
   if (pk)
      pk->deref();
   
   delete socket;
}

int mySocket::connect(const char *name, int timeout_ms, ExceptionSink *xsink) {
   AutoLocker al(this);
   return socket->connect(name, timeout_ms, xsink);
}

int mySocket::connectINET(const char *host, int port, int timeout_ms, ExceptionSink *xsink) {
   AutoLocker al(this);
   return socket->connectINET(host, port, timeout_ms, xsink);
}

int mySocket::connectUNIX(const char *p, ExceptionSink *xsink) {
   AutoLocker al(this);
   return socket->connectUNIX(p, xsink);
}

// to bind to either a UNIX socket or an INET interface:port
int mySocket::bind(const char *name, bool reuseaddr) {
   AutoLocker al(this);
   return socket->bind(name, reuseaddr);
}

// to bind to an INET tcp port on all interfaces
int mySocket::bind(int port, bool reuseaddr) {
   AutoLocker al(this);
   return socket->bind(port, reuseaddr);
}

// to bind an open socket to an INET tcp port on a specific interface
int mySocket::bind(const char *interface, int port, bool reuseaddr) {
   AutoLocker al(this);
   return socket->bind(interface, port, reuseaddr);
}

// get port number for INET sockets
int mySocket::getPort() {
   AutoLocker al(this);
   return socket->getPort();
}

mySocket *mySocket::accept(class SocketSource *source, ExceptionSink *xsink) {
   QoreSocket *s;
   {
      AutoLocker al(this);
      s = socket->accept(source, xsink);
   }
   return s ? new mySocket(s) : 0;
}

int mySocket::listen() {
   AutoLocker al(this);
   return socket->listen();
}

// send a buffer of a particular size
int mySocket::send(const char *buf, int size) {
   AutoLocker al(this);
   return socket->send(buf, size);
}

// send a null-terminated string
int mySocket::send(const QoreString *msg, ExceptionSink *xsink) {
   AutoLocker al(this);
   return socket->send(msg, xsink);
}

// send a binary object
int mySocket::send(const BinaryNode *b) {
   AutoLocker al(this);
   return socket->send(b);
}

// send from a file descriptor
int mySocket::send(int fd, int size) {
   AutoLocker al(this);
   return socket->send(fd, size);
}

// send bytes and convert to network order
int mySocket::sendi1(char b) {
   AutoLocker al(this);
   return socket->sendi1(b);
}

int mySocket::sendi2(short b) {
   AutoLocker al(this);
   return socket->sendi2(b);
}

int mySocket::sendi4(int b) {
   AutoLocker al(this);
   return socket->sendi4(b);
}

int mySocket::sendi8(int64 b) {
   AutoLocker al(this);
   return socket->sendi8(b);
}

int mySocket::sendi2LSB(short b) {
   AutoLocker al(this);
   return socket->sendi2LSB(b);
}

int mySocket::sendi4LSB(int b) {
   AutoLocker al(this);
   return socket->sendi4LSB(b);
}

int mySocket::sendi8LSB(int64 b) {
   AutoLocker al(this);
   return socket->sendi8LSB(b);
}

// receive a packet of bytes as a string
QoreStringNode *mySocket::recv(int timeout_ms, int *rc) {
   AutoLocker al(this);
   return socket->recv(timeout_ms, rc);
}

// receive a certain number of bytes as a string
QoreStringNode *mySocket::recv(int bufsize, int timeout_ms, int *rc) {
   AutoLocker al(this);
   return socket->recv(bufsize, timeout_ms, rc);
}

// receive a certain number of bytes as a binary object
BinaryNode *mySocket::recvBinary(int bufsize, int timeout_ms, int *rc) {
   AutoLocker al(this);
   return socket->recvBinary(bufsize, timeout_ms, rc);
}

// receive a packet of bytes as a binary object
BinaryNode *mySocket::recvBinary(int timeout_ms, int *rc) {
   AutoLocker al(this);
   return socket->recvBinary(timeout_ms, rc);
}

// receive and write data to a file descriptor
int mySocket::recv(int fd, int size, int timeout_ms) {
   AutoLocker al(this);
   return socket->recv(fd, size, timeout_ms);
}

// receive integers and convert from network byte order
int mySocket::recvi1(int timeout_ms, char *b) {
   AutoLocker al(this);
   return socket->recvi1(timeout_ms, b);
}

int mySocket::recvi2(int timeout_ms, short *b) {
   AutoLocker al(this);
   return socket->recvi2(timeout_ms, b);
}

int mySocket::recvi4(int timeout_ms, int *b) {
   AutoLocker al(this);
   return socket->recvi4(timeout_ms, b);
}

int mySocket::recvi8(int timeout_ms, int64 *b) {
   AutoLocker al(this);
   return socket->recvi8(timeout_ms, b);
}

int mySocket::recvi2LSB(int timeout_ms, short *b) {
   AutoLocker al(this);
   return socket->recvi2LSB(timeout_ms, b);
}

int mySocket::recvi4LSB(int timeout_ms, int *b) {
   AutoLocker al(this);
   return socket->recvi4LSB(timeout_ms, b);
}

int mySocket::recvi8LSB(int timeout_ms, int64 *b) {
   AutoLocker al(this);
   return socket->recvi8LSB(timeout_ms, b);
}

// receive integers and convert from network byte order
int mySocket::recvu1(int timeout_ms, unsigned char *b) {
   AutoLocker al(this);
   return socket->recvu1(timeout_ms, b);
}

int mySocket::recvu2(int timeout_ms, unsigned short *b) {
   AutoLocker al(this);
   return socket->recvu2(timeout_ms, b);
}

int mySocket::recvu4(int timeout_ms, unsigned int *b) {
   AutoLocker al(this);
   return socket->recvu4(timeout_ms, b);
}

int mySocket::recvu2LSB(int timeout_ms, unsigned short *b) {
   AutoLocker al(this);
   return socket->recvu2LSB(timeout_ms, b);
}

int mySocket::recvu4LSB(int timeout_ms, unsigned int *b) {
   AutoLocker al(this);
   return socket->recvu4LSB(timeout_ms, b);
}

// send HTTP message
int mySocket::sendHTTPMessage(const char *method, const char *path, const char *http_version, const QoreHashNode *headers, const void *ptr, int size) {
   AutoLocker al(this);
   return socket->sendHTTPMessage(method, path, http_version, headers, ptr, size);
}

// send HTTP response
int mySocket::sendHTTPResponse(int code, const char *desc, const char *http_version, const QoreHashNode *headers, const void *ptr, int size) {
   AutoLocker al(this);
   return socket->sendHTTPResponse(code, desc, http_version, headers, ptr, size);
}

// receive a binary message in HTTP chunked format
QoreHashNode *mySocket::readHTTPChunkedBodyBinary(int timeout_ms, ExceptionSink *xsink) {
   AutoLocker al(this);
   return socket->readHTTPChunkedBodyBinary(timeout_ms, xsink);
}

// receive a string message in HTTP chunked format
QoreHashNode *mySocket::readHTTPChunkedBody(int timeout_ms, ExceptionSink *xsink) {
   AutoLocker al(this);
   return socket->readHTTPChunkedBody(timeout_ms, xsink);
}

// read and parse HTTP header
AbstractQoreNode *mySocket::readHTTPHeader(int timeout_ms, int *rc) {
   AutoLocker al(this);
   return socket->readHTTPHeader(timeout_ms, rc);
}

int mySocket::setSendTimeout(int ms) {
   AutoLocker al(this);
   return socket->setSendTimeout(ms);
}

int mySocket::setRecvTimeout(int ms) {
   AutoLocker al(this);
   return socket->setRecvTimeout(ms);
}

int mySocket::getSendTimeout() {
   AutoLocker al(this);
   return socket->getSendTimeout();
}

int mySocket::getRecvTimeout() {
   AutoLocker al(this);
   return socket->getRecvTimeout();
}

int mySocket::close() { 
   AutoLocker al(this);
   return socket->close();
}

int mySocket::shutdown() { 
   AutoLocker al(this);
   return socket->shutdown();
}

int mySocket::shutdownSSL(ExceptionSink *xsink) { 
   AutoLocker al(this);
   return socket->shutdownSSL(xsink);
}

const char *mySocket::getSSLCipherName() { 
   AutoLocker al(this);
   return socket->getSSLCipherName();
}

const char *mySocket::getSSLCipherVersion() { 
   AutoLocker al(this);
   return socket->getSSLCipherVersion();
}

bool mySocket::isSecure() {
   AutoLocker al(this);
   return socket->isSecure();
}

long mySocket::verifyPeerCertificate() {
   AutoLocker al(this);
   return socket->verifyPeerCertificate();
}

int mySocket::getSocket() {
   AutoLocker al(this);
   return socket->getSocket();
}

void mySocket::setEncoding(const QoreEncoding *id) {
   socket->setEncoding(id);
}

const QoreEncoding *mySocket::getEncoding() const {
   return socket->getEncoding();
}

bool mySocket::isDataAvailable(int timeout_ms) {
   AutoLocker al(this);
   return socket->isDataAvailable(timeout_ms);
}

bool mySocket::isWriteFinished(int timeout_ms) {
   AutoLocker al(this);
   return socket->isWriteFinished(timeout_ms);
}

bool mySocket::isOpen() const {
   return socket->isOpen();
}

int mySocket::connectINETSSL(const char *host, int port, int timeout_ms, ExceptionSink *xsink) {
   AutoLocker al(this);
   return socket->connectINETSSL(host, port, timeout_ms, 
				 cert ? cert->getData() : 0,
				 pk ? pk->getData() : 0,
				 xsink);
}

int mySocket::connectUNIXSSL(const char *p, ExceptionSink *xsink) {
   AutoLocker al(this);
   return socket->connectUNIXSSL(p, 
				 cert ? cert->getData() : 0,
				 pk ? pk->getData() : 0,
				 xsink);
}

int mySocket::connectSSL(const char *name, int timeout_ms, ExceptionSink *xsink) {
   AutoLocker al(this);
   return socket->connectSSL(name, timeout_ms,
			     cert ? cert->getData() : 0,
			     pk ? pk->getData() : 0,
			     xsink);
}

mySocket *mySocket::acceptSSL(class SocketSource *source, ExceptionSink *xsink) {
   QoreSocket *s;
   {
      AutoLocker al(this);
      s = socket->acceptSSL(source, cert ? cert->getData() : 0, pk ? pk->getData() : 0, xsink);
   }
   return s ? new mySocket(s) : 0;
}

// c must be already referenced before this call
void mySocket::setCertificate(class QoreSSLCertificate *c) {
   AutoLocker al(this);
   if (cert)
      cert->deref();
   cert = c;
}

// p must be already referenced before this call
void mySocket::setPrivateKey(class QoreSSLPrivateKey *p) {
   AutoLocker al(this);
   if (pk)
      pk->deref();
   pk = p;
}

void mySocket::setEventQueue(Queue *cbq, ExceptionSink *xsink) {
   AutoLocker al(this);
   socket->setEventQueue(cbq, xsink);
}

int mySocket::setNoDelay(int nodelay) {   
   AutoLocker al(this);
   return socket->setNoDelay(nodelay);
}

int mySocket::getNoDelay() {   
   AutoLocker al(this);
   return socket->getNoDelay();
}
