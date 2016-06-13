/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreSocketObject.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  provides a thread-safe interface to the QoreSocket object

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

#ifndef _QORE_QORE_SOCKET_OBJECT_H

#define _QORE_QORE_SOCKET_OBJECT_H

#include <qore/QoreSocket.h>
#include <qore/AbstractPrivateData.h>
#include <qore/QoreThreadLock.h>

class QoreSSLCertificate;
class QoreSSLPrivateKey;
class Queue;
class my_socket_priv;

class QoreSocketObject : public AbstractPrivateData {
private:
   friend class my_socket_priv;
   friend struct qore_httpclient_priv;

   DLLLOCAL QoreSocketObject(QoreSocket* s, QoreSSLCertificate* cert = 0, QoreSSLPrivateKey* pk = 0);

protected:
   my_socket_priv* priv;

   DLLLOCAL virtual ~QoreSocketObject();

public:
   DLLEXPORT QoreSocketObject();

   DLLEXPORT virtual void deref(ExceptionSink* xsink);
   DLLEXPORT virtual void deref();

   DLLEXPORT int connect(const char* name, int timeout_ms, ExceptionSink* xsink = NULL);
   DLLEXPORT int connectINET(const char* host, int port, int timeout_ms, ExceptionSink* xsink = NULL);
   DLLEXPORT int connectINET2(const char* host, const char* service, int family, int sock_type, int protocol, int timeout_ms = -1, ExceptionSink* xsink = NULL);
   DLLEXPORT int connectUNIX(const char* p, int socktype, int protocol, ExceptionSink* xsink = NULL);
   DLLEXPORT int connectSSL(const char* name, int timeout_ms, ExceptionSink* xsink);
   DLLEXPORT int connectINETSSL(const char* host, int port, int timeout_ms, ExceptionSink* xsink);
   DLLEXPORT int connectINET2SSL(const char* host, const char* service, int family, int sock_type, int protocol, int timeout_ms = -1, ExceptionSink* xsink = NULL);
   DLLEXPORT int connectUNIXSSL(const char* p, int socktype, int protocol, ExceptionSink* xsink);
   // to bind to either a UNIX socket or an INET interface:port
   DLLEXPORT int bind(const char* name, bool reuseaddr = false);
   // to bind to an INET tcp port on all interfaces
   DLLEXPORT int bind(int port, bool reuseaddr = false);
   // to bind an open socket to an INET tcp port on a specific interface
   DLLEXPORT int bind(const char* iface, int port, bool reuseaddr = false);

   DLLEXPORT int bindUNIX(const char* name, int socktype, int protocol, ExceptionSink* xsink);
   DLLEXPORT int bindINET(const char* name, const char* service, bool reuseaddr, int family, int socktype, int protocol, ExceptionSink* xsink);

   // get port number for INET sockets
   DLLEXPORT int getPort();
   DLLEXPORT QoreSocketObject *accept(SocketSource *source, ExceptionSink* xsink);
   DLLEXPORT QoreSocketObject *acceptSSL(SocketSource *source, ExceptionSink* xsink);
   DLLEXPORT QoreSocketObject *accept(int timeout_ms, ExceptionSink* xsink);
   DLLEXPORT QoreSocketObject *acceptSSL(int timeout_ms, ExceptionSink* xsink);

   DLLEXPORT int listen(int backlog);
   // send a buffer of a particular size
   DLLEXPORT int send(const char* buf, int size);
   DLLEXPORT int send(const char* buf, int size, int timeout_ms, ExceptionSink* xsink);
   // send a null-terminated string
   DLLEXPORT int send(const QoreString *msg, int timeout_ms, ExceptionSink* xsink);
   // send a binary object
   DLLEXPORT int send(const BinaryNode* b);
   DLLEXPORT int send(const BinaryNode* b, int timeout_ms, ExceptionSink* xsink);
   // send a certain number of bytes (read from an InputStream)
   DLLEXPORT void sendFromInputStream(InputStream *is, int64 size, int64 timeout_ms, ExceptionSink *xsink);

   // send from a file descriptor
   DLLEXPORT int send(int fd, int size = -1);
   // send bytes and convert to network order
   DLLEXPORT int sendi1(char b, int timeout_ms, ExceptionSink* xsink);
   DLLEXPORT int sendi2(short b, int timeout_ms, ExceptionSink* xsink);
   DLLEXPORT int sendi4(int b, int timeout_ms, ExceptionSink* xsink);
   DLLEXPORT int sendi8(int64 b, int timeout_ms, ExceptionSink* xsink);
   DLLEXPORT int sendi2LSB(short b, int timeout_ms, ExceptionSink* xsink);
   DLLEXPORT int sendi4LSB(int b, int timeout_ms, ExceptionSink* xsink);
   DLLEXPORT int sendi8LSB(int64 b, int timeout_ms, ExceptionSink* xsink);
   // receive a message
   DLLEXPORT QoreStringNode* recv(int timeout, ExceptionSink* xsink);
   // receive a certain number of bytes as a string
   DLLEXPORT QoreStringNode* recv(qore_offset_t bufsize, int timeout_ms, ExceptionSink* xsink);
   // receive a certain number of bytes as a binary object
   DLLEXPORT BinaryNode* recvBinary(int bufsize, int timeout, ExceptionSink* xsink);
   // receive a packet of bytes as a binary object
   DLLEXPORT BinaryNode* recvBinary(int timeout, ExceptionSink* xsink);
   // receive a certain number of bytes and write them to an OutputStream
   DLLEXPORT void recvToOutputStream(OutputStream *os, int64 size, int64 timeout_ms, ExceptionSink *xsink);

   // receive and write data to a file descriptor
   DLLEXPORT int recv(int fd, int size, int timeout);
   // receive integers and convert from network byte order
   DLLEXPORT int64 recvi1(int timeout, char* b, ExceptionSink* xsink);
   DLLEXPORT int64 recvi2(int timeout, short *b, ExceptionSink* xsink);
   DLLEXPORT int64 recvi4(int timeout, int *b, ExceptionSink* xsink);
   DLLEXPORT int64 recvi8(int timeout, int64 *b, ExceptionSink* xsink);
   DLLEXPORT int64 recvi2LSB(int timeout, short *b, ExceptionSink* xsink);
   DLLEXPORT int64 recvi4LSB(int timeout, int *b, ExceptionSink* xsink);
   DLLEXPORT int64 recvi8LSB(int timeout, int64 *b, ExceptionSink* xsink);
   DLLEXPORT int64 recvu1(int timeout, unsigned char* b, ExceptionSink* xsink);
   DLLEXPORT int64 recvu2(int timeout, unsigned short *b, ExceptionSink* xsink);
   DLLEXPORT int64 recvu4(int timeout, unsigned int *b, ExceptionSink* xsink);
   DLLEXPORT int64 recvu2LSB(int timeout, unsigned short *b, ExceptionSink* xsink);
   DLLEXPORT int64 recvu4LSB(int timeout, unsigned int *b, ExceptionSink* xsink);
   // send HTTP message
   DLLEXPORT int sendHTTPMessage(ExceptionSink* xsink, QoreHashNode* info, const char* method, const char* path, const char* http_version, const QoreHashNode* headers, const void* ptr, int size, int source, int timeout_ms);
   DLLEXPORT int sendHTTPMessageWithCallback(ExceptionSink* xsink, QoreHashNode *info, const char *method, const char *path, const char *http_version, const QoreHashNode *headers, const ResolvedCallReferenceNode& send_callback, int source, int timeout_ms);
   DLLEXPORT int sendHTTPMessageWithCallback(ExceptionSink* xsink, QoreHashNode* info, const char* method, const char *path, const char *http_version, const QoreHashNode *headers, const ResolvedCallReferenceNode& send_callback, int source, int timeout_ms, bool* aborted);

   // send HTTP response
   DLLEXPORT int sendHTTPResponse(ExceptionSink* xsink, int code, const char* desc, const char* http_version, const QoreHashNode* headers, const void* ptr, int size, int source, int timeout_ms);
   DLLEXPORT int sendHTTPResponseWithCallback(ExceptionSink* xsink, int code, const char *desc, const char *http_version, const QoreHashNode *headers, const ResolvedCallReferenceNode& send_callback, int source, int timeout_ms);
   DLLEXPORT int sendHTTPResponseWithCallback(ExceptionSink* xsink, int code, const char *desc, const char *http_version, const QoreHashNode *headers, const ResolvedCallReferenceNode& send_callback, int source, int timeout_ms, bool* aborted);

   // read and parse HTTP header
   DLLEXPORT AbstractQoreNode* readHTTPHeader(ExceptionSink* xsink, QoreHashNode* info, int timeout);
   // receive a binary message in HTTP chunked format
   DLLEXPORT QoreHashNode* readHTTPChunkedBodyBinary(int timeout, ExceptionSink* xsink);
   // receive a binary message in HTTP chunked format
   DLLEXPORT QoreHashNode* readHTTPChunkedBodyToOutputStream(OutputStream *os, int timeout_ms, ExceptionSink* xsink);
   // receive a string message in HTTP chunked format
   DLLEXPORT QoreHashNode* readHTTPChunkedBody(int timeout, ExceptionSink* xsink);

   // receive a binary message in HTTP chunked format
   DLLEXPORT void readHTTPChunkedBodyBinaryWithCallback(const ResolvedCallReferenceNode& recv_callback, QoreObject* obj, int timeout_ms, ExceptionSink* xsink);
   // receive a string message in HTTP chunked format
   DLLEXPORT void readHTTPChunkedBodyWithCallback(const ResolvedCallReferenceNode& recv_callback, QoreObject* obj, int timeout_ms, ExceptionSink* xsink);

   DLLEXPORT QoreStringNode* readHTTPHeaderString(ExceptionSink* xsink, int timeout_ms);

   DLLEXPORT int setSendTimeout(int ms);
   DLLEXPORT int setRecvTimeout(int ms);
   DLLEXPORT int getSendTimeout();
   DLLEXPORT int getRecvTimeout();
   DLLEXPORT int close();
   DLLEXPORT int shutdown();
   DLLEXPORT int shutdownSSL(ExceptionSink* xsink) ;
   DLLEXPORT const char* getSSLCipherName();
   DLLEXPORT const char* getSSLCipherVersion();
   DLLEXPORT bool isSecure();
   DLLEXPORT long verifyPeerCertificate();
   DLLEXPORT int getSocket();
   DLLEXPORT void setEncoding(const QoreEncoding *id);
   DLLEXPORT const QoreEncoding *getEncoding() const;
   DLLEXPORT bool isDataAvailable(ExceptionSink* xsink, int timeout = 0);
   DLLEXPORT bool isWriteFinished(ExceptionSink* xsink, int timeout = 0);
   DLLEXPORT bool isOpen() const;
   // c must be already referenced before this call
   DLLEXPORT void setCertificate(QoreSSLCertificate* c);
   // p must be already referenced before this call
   DLLEXPORT void setPrivateKey(QoreSSLPrivateKey* p);
   DLLEXPORT int setNoDelay(int nodelay);
   DLLEXPORT int getNoDelay();
   DLLEXPORT void setEventQueue(Queue *cbq, ExceptionSink* xsink);
   DLLEXPORT QoreHashNode* getPeerInfo(ExceptionSink* xsink, bool host_lookup = true) const;
   DLLEXPORT QoreHashNode* getSocketInfo(ExceptionSink* xsink, bool host_lookup = true) const;

   DLLEXPORT void upgradeClientToSSL(ExceptionSink* xsink);
   DLLEXPORT void upgradeServerToSSL(ExceptionSink* xsink);

   DLLEXPORT void upgradeClientToSSL(int timeout_ms, ExceptionSink* xsink);
   DLLEXPORT void upgradeServerToSSL(int timeout_ms, ExceptionSink* xsink);

   DLLEXPORT void clearWarningQueue(ExceptionSink* xsink);
   DLLEXPORT void setWarningQueue(ExceptionSink* xsink, int64 warning_ms, int64 warning_bs, class Queue* wq, AbstractQoreNode* arg, int64 min_ms = 1000);
   DLLEXPORT QoreHashNode* getUsageInfo() const;
   DLLEXPORT void clearStats();
   DLLEXPORT bool pendingHttpChunkedBody() const;
};

#endif // _QORE_QORE_SOCKET_OBJECT_H
