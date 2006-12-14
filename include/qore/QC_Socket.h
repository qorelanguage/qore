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

DLLLOCAL class QoreClass *initSocketClass();

DLLEXPORT extern int CID_SOCKET;

#include <qore/QoreSocket.h>
#include <qore/AbstractPrivateData.h>
#include <qore/LockedObject.h>
#include <qore/QC_SSLCertificate.h>
#include <qore/QC_SSLPrivateKey.h>

class mySocket : public AbstractPrivateData, public LockedObject
{
   private:
      class QoreSocket *socket;
      class QoreSSLCertificate *cert;
      class QoreSSLPrivateKey *pk;

      DLLLOCAL void init();
      DLLLOCAL mySocket(class QoreSocket *s);

   protected:
      DLLLOCAL virtual ~mySocket();

   public:
      DLLLOCAL mySocket();      
      DLLLOCAL int connect(char *name, class ExceptionSink *xsink = NULL);
      DLLLOCAL int connectINET(char *host, int port, class ExceptionSink *xsink = NULL);
      DLLLOCAL int connectUNIX(char *p, class ExceptionSink *xsink = NULL);
      DLLLOCAL int connectSSL(char *name, class ExceptionSink *xsink);
      DLLLOCAL int connectINETSSL(char *host, int port, class ExceptionSink *xsink);
      DLLLOCAL int connectUNIXSSL(char *p, class ExceptionSink *xsink);
      // to bind to either a UNIX socket or an INET interface:port
      DLLLOCAL int bind(char *name, bool reuseaddr = false);
      // to bind to an INET tcp port on all interfaces
      DLLLOCAL int bind(int port, bool reuseaddr = false);
      // to bind an open socket to an INET tcp port on a specific interface
      DLLLOCAL int bind(char *interface, int port, bool reuseaddr = false);
      // get port number for INET sockets
      DLLLOCAL int getPort();
      DLLLOCAL class mySocket *accept(class SocketSource *source, class ExceptionSink *xsink);
      DLLLOCAL class mySocket *acceptSSL(class SocketSource *source, class ExceptionSink *xsink);
      DLLLOCAL int listen();
      // send a buffer of a particular size
      DLLLOCAL int send(char *buf, int size);
      // send a null-terminated string
      DLLLOCAL int send(class QoreString *msg, class ExceptionSink *xsink);
      // send a binary object
      DLLLOCAL int send(class BinaryObject *b);
      // send from a file descriptor
      DLLLOCAL int send(int fd, int size = -1);
      // send bytes and convert to network order
      DLLLOCAL int sendi1(char b);
      DLLLOCAL int sendi2(short b);
      DLLLOCAL int sendi4(int b);
      DLLLOCAL int sendi8(int64 b);
      DLLLOCAL int sendi2LSB(short b);
      DLLLOCAL int sendi4LSB(int b);
      DLLLOCAL int sendi8LSB(int64 b);
      // receive a certain number of bytes as a string
      DLLLOCAL class QoreString *recv(int bufsize, int timeout, int *rc);
      // receive a certain number of bytes as a binary object
      DLLLOCAL class BinaryObject *recvBinary(int bufsize, int timeout, int *rc);
      // receive a message
      DLLLOCAL class QoreString *recv(int timeout, int *rc);
      // receive and write data to a file descriptor
      DLLLOCAL int recv(int fd, int size, int timeout);
      // receive integers and convert from network byte order
      DLLLOCAL int recvi1(int timeout, char *b);
      DLLLOCAL int recvi2(int timeout, short *b);
      DLLLOCAL int recvi4(int timeout, int *b);
      DLLLOCAL int recvi8(int timeout, int64 *b);
      DLLLOCAL int recvi2LSB(int timeout, short *b);
      DLLLOCAL int recvi4LSB(int timeout, int *b);
      DLLLOCAL int recvi8LSB(int timeout, int64 *b);
      // send HTTP message
      DLLLOCAL int sendHTTPMessage(char *method, char *path, char *http_version, class Hash *headers, void *ptr, int size);
      // send HTTP response
      DLLLOCAL int sendHTTPResponse(int code, char *desc, char *http_version, class Hash *headers, void *ptr, int size);
      // read and parse HTTP header
      DLLLOCAL class QoreNode *readHTTPHeader(int timeout, int *rc);
      DLLLOCAL int setSendTimeout(int ms);
      DLLLOCAL int setRecvTimeout(int ms);
      DLLLOCAL int getSendTimeout();
      DLLLOCAL int getRecvTimeout();
      DLLLOCAL int close();
      DLLLOCAL int shutdown();
      DLLLOCAL int shutdownSSL(class ExceptionSink *xsink) ;
      DLLLOCAL const char *getSSLCipherName();
      DLLLOCAL const char *getSSLCipherVersion();
      DLLLOCAL bool isSecure();
      DLLLOCAL long verifyPeerCertificate();
      DLLLOCAL int getSocket();
      DLLLOCAL void setEncoding(class QoreEncoding *id);
      DLLLOCAL class QoreEncoding *getEncoding() const;
      DLLLOCAL bool isDataAvailable(int timeout = 0);
      DLLLOCAL bool isOpen() const;
      // c must be already referenced before this call
      DLLLOCAL void setCertificate(class QoreSSLCertificate *c);
      // p must be already referenced before this call
      DLLLOCAL void setPrivateKey(class QoreSSLPrivateKey *p);
};

#endif // _QORE_CLASS_QORESOCKET_H
